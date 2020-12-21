#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
// vytvoří objekt lcd a nastaví jeho adresu
// 0x20 a 16 zanků na 2 řádcích
LiquidCrystal_I2C lcd(0x27,16,2);  
// specifikace pinů
int pTrig = 3;
int pEcho = 2;
// inicializace promněnných
int odezva;
int vzdalenost;
int obsazenost = 0;
int led = 13;
int body = 0;



void setup()
{
  lcd.begin();// inicializuje displej
   
  lcd.backlight(); // zapne podsvětlení
// nastavení pinů modulu
  pinMode(pTrig, OUTPUT);
  pinMode(pEcho, INPUT);
// Rychlost komunikace (9600 baud)
  Serial.begin(9600);
// Nastavení LED
  pinMode(led, OUTPUT);
}

void loop()
{
odezva = mereni(odezva);  // Změří vzdálenost
vzdalenost = count(odezva); // Vypočítá hodnotu vzdálenosti
body = ob_count(vzdalenost, body); // Přiřadí hodnotu z rozsahu
obsazenost = prepocet(body, obsazenost);
zobrazeni(vzdalenost, body, obsazenost); // Zobrazí na displeji
    
}

// Funkce počítající vzdálenost objektu

       int count(int x){
        int y;
        y = x*0.034/2;
        return y;
       }

// Funkce určující rozsah a ohodnocení

      int ob_count(int x, int o){
        if ( x > 10){
           digitalWrite(led, LOW);
           o = 0;
        }
        else if (x < 8) {
           digitalWrite(led, LOW);
           o = 0;
        }
        else {
          o++;
          digitalWrite(led, HIGH);
        }
        return o;
      }

// Funkce měření vzdálenosti

      int mereni(int echo){
          digitalWrite(pTrig, LOW);
          delayMicroseconds(2);
          digitalWrite(pTrig, HIGH);
          delayMicroseconds(10);
          digitalWrite(pTrig, LOW);
          echo = pulseIn(pEcho, HIGH);
          return echo;
}

// Funkce zobrazující informace na LCD

      int zobrazeni(int vz, int bo, int ob){
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("Vzdalenost je:");
            lcd.setCursor(1,1);
            lcd.print(vz);
            lcd.print(" cm");
            lcd.setCursor(8,1);
            lcd.print(bo);
            lcd.setCursor(12,1);
            lcd.print(ob);
            delay(250);
      }

 // Funkce přepočtu bodů na obsazenost

      int prepocet(int bo, int ob){
            if ( body == 50 ) {
                ob++;
             } 
             else {
             }
       return ob;
  }
