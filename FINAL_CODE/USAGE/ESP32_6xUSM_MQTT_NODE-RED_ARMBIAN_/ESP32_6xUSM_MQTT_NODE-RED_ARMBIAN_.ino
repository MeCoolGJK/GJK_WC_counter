#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>

// PINS and SENS settings

// Group Trig and single Echos pins setup
int pTrig = 32;
int pEcho[] = {13, 14, 27, 26, 25, 33}; 

// Initial ints

int odezva[6];
int vzdalenost[6];
int body[] = {0, 0, 0, 0, 0, 0};
int obsazenost[] = {0, 0, 0, 0, 0, 0};

// Conditions setting

int maxDist = 60; // distance in cm
int minDist = 15; // distance in cm
int minTime = 12000; // time in ms

// Inital longs for time counting

unsigned long TimeIn[] = {0, 0, 0, 0, 0, 0};
unsigned long TimeOut[] = {0, 0, 0, 0, 0, 0};
unsigned long TimeFinal[] = {0, 0, 0, 0, 0, 0};

// Status of sensors

bool stav[] = {false, false, false, false, false, false};

// # of connected sensors
int sens_num = 6;

// Wifi-settings

const char* ssid = "ARMBIAN";
const char* wifipassword = "ESP32-NODERED";

// MQTT settings

const char* mqtt_server = "172.24.1.1";

// MQTT boot

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  // Initializing sensors pins
  for ( int i = 0; i < sens_num; i++) {
    pinMode(pTrig, OUTPUT);
    pinMode(pEcho[i], INPUT);
    Serial.begin(115200);

    // Initializing WIFI and MQTT
    setup_wifi();

    client.setServer(mqtt_server, 1883);
  }
}

// Wifi setup code

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifipassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP adress: ");
  Serial.println(WiFi.localIP());
}

// MQTT reconnecting

void reconnect() {
  while (!client.connected()) {
    Serial.println("Trying to connect to MQTT");
    if (client.connect("EPS32Client")) {
      Serial.println("connected");
      client.publish( "esp32/status", "Připojeno");
      client.subscribe("esp32/output");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {

 if (!client.connected()) {
    reconnect();
 }
  client.loop();
  
  // Measuring, counting distance, measuring time spent in front of the sensor and valuating
  
  for (int i = 0; i < sens_num; i++) {

    odezva[i] = mereni(pTrig, pEcho[i]);  // Změří vzdálenost
    vzdalenost[i] = prepocet(odezva[i]); // Vypočítá hodnotu vzdálenosti
    TimeIn[i] = cas_in(vzdalenost[i], minDist, maxDist, stav[i], TimeIn[i]);
    TimeOut[i] = cas_out(vzdalenost[i], minDist, maxDist, stav[i], TimeOut[i]);
    TimeFinal[i] = cas_final(vzdalenost[i], minDist, maxDist, TimeOut[i], TimeIn[i], TimeFinal[i], stav[i]);
    stav[i] = zmena_stavu(vzdalenost[i], stav[i], minDist, maxDist);
    obsazenost[i] = vyhodnoceni(TimeFinal[i], obsazenost[i], minTime);
    delay(100);
    publishing(stav[0],"esp32/stav-1");
    publishing(obsazenost[0], "esp32/obsazenost-1"); 
    publishing(stav[1],"esp32/stav-2");
    publishing(obsazenost[1], "esp32/obsazenost-2"); 
    publishing(stav[2],"esp32/stav-3");
    publishing(obsazenost[2], "esp32/obsazenost-3"); 
    publishing(stav[3],"esp32/stav-4");
    publishing(obsazenost[3], "esp32/obsazenost-4"); 
    publishing(stav[4],"esp32/stav-5");
    publishing(obsazenost[4], "esp32/obsazenost-5"); 
    publishing(stav[5],"esp32/stav-6");
    publishing(obsazenost[5], "esp32/obsazenost-6"); 
  }  
}


// Data publishing

void publishing(int in, const char* topic) {
  char out[8];
  dtostrf(in,1,0,out);
  client.publish(topic, out);
}


// Basic measuring

int mereni(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  int value = pulseIn(echo, HIGH);
  return value;
}

// Time to distance conversion

int prepocet(int x) {
  int y;
  y = x * 0.034 / 2;
  return y;
}

// Points to occupation

int vyhodnoceni(unsigned long TimeFinal, int body, int minTime) {
  if ( TimeFinal > minTime ) {
    body++;
  }
  else {
  }
  return body;
}

// Starting relative time

unsigned long cas_in(int Dist, int minDist, int maxDist, bool stav, unsigned long TimeIn){
  if (stav == false){
    if (Dist > minDist && Dist < maxDist){
      TimeIn = millis();
    }
  }
  return TimeIn;
}

// Ending relative time

unsigned long cas_out(int Dist, int minDist, int maxDist, bool stav, unsigned long TimeOut){
   if (stav == true){
    if (Dist < minDist || Dist > maxDist){
      TimeOut = millis(); 
    }
  }
  return TimeOut;
}

// Time spent in front of the sensor

unsigned long cas_final(int Dist, int minDist, int maxDist, unsigned long TimeOut, unsigned long TimeIn,unsigned long TimeFinal, bool stav){
  if (stav == true){
    if (Dist < minDist || Dist > maxDist){
       TimeFinal = TimeOut - TimeIn;
    }
  }
  else {
    TimeFinal = 0;
  }
  return TimeFinal;

}

// Status change depending on distance

bool zmena_stavu(int Dist, bool stav, int minDist, int maxDist){
    if (Dist > minDist && Dist < maxDist){
      stav = true;
    }
    if (Dist < minDist || Dist > maxDist){
      stav = false;
    }
    return stav;
}
