#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>

// PINS and SENS settings

// Group Trig and single Echos pins setup
int pTrig = 18;
int pEcho[] = {19, 21}; 

// Initial ints
int odezva [2];
int vzdalenost[2];
int body[] = {0, 0};
int obsazenost[] = {0, 0};

// # of connected sensors
int sens_num = 2;

// Wifi-settings

const char* ssid = "SSID";
const char* wifipassword = "PASSWORD";

// MQTT settings

const char* mqtt_server = "MQTT-IP";

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
    //    client.setCallback(callback);
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
  Serial.print(WiFi.localIP());
}

// MQTT reconnecting
void reconnect() {
  while (!client.connected()) {
    Serial.println("Trying to connect to MQTT");
    if (client.connect("EPS32Client")) {
      Serial.println("connected");
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
  // Measuring, counting distance, making values and final points
  for ( int i = 0; i < sens_num; i++) {

    odezva[i] = mereni(odezva[i], pTrig, pEcho[i]);  // Změří vzdálenost
    vzdalenost[i] = count(odezva[i]); // Vypočítá hodnotu vzdálenosti
    body[i] = ob_count(vzdalenost[i], body[i]);// Přiřadí hodnotu z rozsahu
   
    if (body[i] != 0) {
      
      obsazenost[i] = prepocet(body[i], obsazenost[i]);

    }
    
    
    // publishování informací

    if (body[0] != 0) { 
      
      publishing(body[0], "esp32/z-1-bo");
      publishing(obsazenost[0], "esp32/z-1-ob");
      
    }

    if (body[1] != 0) {
      
      publishing(body[1], "esp32/z-2-bo");
      publishing(obsazenost[1], "esp32/z-2-ob");
    }
    

  }
  
  delay(1000);
  
}

// Data publishing

void publishing(int in, const char* topic) {
  char out[8];
  dtostrf(in,1,0,out);
  client.publish(topic, out);
}

// Time to distance conversion

int count(int x) {
  int y;
  y = x * 0.034 / 2;
  return y;
}

// Evaluation and distances required

int ob_count(int vzdalenost, int hodnota) {
  if ( vzdalenost > 60) {
    hodnota = 0;
  }
  else if (vzdalenost < 20) {
    hodnota = 0;
  }
  else {
    hodnota++;
  }
  return hodnota;
}

// Basic measuring
int mereni(int value, int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  value = pulseIn(echo, HIGH);
  return value;
}

// Points to occupation

int prepocet(int bo, int ob) {
  if ( bo == 5 ) {
    ob++;
  }
  else {
  }
  return ob;
}
