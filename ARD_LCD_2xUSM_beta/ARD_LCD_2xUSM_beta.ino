#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
// vytvoří objekt lcd a nastaví jeho adresu
// 0x20 a 16 zanků na 2 řádcích
LiquidCrystal_I2C lcd(0x27,16,2);  
// specifikace pinů_1
int pTrig[] = {3,5};
int pEcho[] = {2,4};
int led[] = {13,12};

int odezva[2]; // Pevně dané pole
int vzdalenost[2];
int body[] = {0,0};
int obsazenost[] = {0,0};

int sens_num = 2; 

void setup()
{
  lcd.begin();// inicializuje displej

   
  lcd.backlight(); // zapne podsvětlení
// nastavení pinů modulu
for ( int i = 0; i < sens_num; i++){
    pinMode(pTrig[i], OUTPUT);
    pinMode(pEcho[i], INPUT);
  // Rychlost komunikace (9600 baud)
    Serial.begin(9600);
  // Nastavení LED
    pinMode(led[i], OUTPUT);
  }
}
void loop()
{
  for ( int i = 0; i < sens_num; i++){
    
    odezva[i] = mereni(odezva[i], pTrig[i], pEcho[i]);  // Změří vzdálenost  
    vzdalenost[i] = count(odezva[i]); // Vypočítá hodnotu vzdálenosti   
    body[i] = ob_count(vzdalenost[i], body[i], led[i]);// Přiřadí hodnotu z rozsahu    
    obsazenost[i] = prepocet(body[i], obsazenost[i]);
  }
    zobrazeni(body[0], obsazenost[0], body[1], obsazenost[1]); // Zobrazí na displeji
    delay(1000);    
}

// Funkce počítající vzdálenost objektu

       int count(int x){
        int y;
        y = x*0.034/2;
        return y;
       }

// Funkce určující rozsah a ohodnocení

      int ob_count(int vzdalenost, int hodnota, int led){
        if ( vzdalenost > 80){
           digitalWrite(led, LOW);
           hodnota = 0;
        }
        else if (vzdalenost < 40) {
           digitalWrite(led, LOW);
           hodnota = 0;
        }
        else {
          hodnota++;
          digitalWrite(led, HIGH);
        }
        return hodnota;
      }

// Funkce měření vzdálenosti

      int mereni(int value, int trig, int echo){
          digitalWrite(trig, LOW);
          delayMicroseconds(2);
          digitalWrite(trig, HIGH);
          delayMicroseconds(10);
          digitalWrite(trig, LOW);
          value = pulseIn(echo, HIGH);
          return value;
}

// Funkce zobrazující informace na LCD

      int zobrazeni(int bo1, int ob1, int bo2, int ob2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1 ->");
      lcd.setCursor(6,0);
      lcd.print(bo1);
      lcd.setCursor(10,0);
      lcd.print(ob1);
      lcd.setCursor(0,1);
      lcd.print("2 ->");
      lcd.setCursor(6,1);
      lcd.print(bo2);
      lcd.setCursor(10,1);
      lcd.print(ob2);
      }

 // Funkce přepočtu bodů na obsazenost

      int prepocet(int bo, int ob){
            if ( bo == 5 ) {
                ob++;
             } 
             else {
             }
       return ob;
  }
