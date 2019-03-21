// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Senso

#include "DHT.h"
#define light 2

#define sec 300     // период считывания данных с датчика для их последующего усреднения
#define n 5    // кол-во значений для усреднения

#define lampON 21
#define hatchCLOSE 22
#define hatchOPEN 23
#define hatchCLOSElampON 24

#define DHTPIN 7    // what digital pin we're connected to
#include <LiquidCrystal.h>
int backlight=3, todo;
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
// Uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
void output(int h, int t){      // вывод данных на экран
  lcd.setCursor(0, 0);
  lcd.print(F("Hum: "));
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(F("Temp: "));
  lcd.print(t);
  lcd.print("\xdf");
  lcd.print("C");
  lcd.setCursor(0, 1);
  
  Serial.print("Hum: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.println(" C");
  Serial.println(" ");
}

void openhatch(){      // открытие крышки, продолжительность и закрытие
  //открвыаем люк
}

void lamp(){
  digitalWrite(light, HIGH);
  int midh, midt;
  for (int i = 0; i < 5; i ++){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
      delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      delay(5000);
    }
  midh /= 5;
  midt /= 5;
  while(midt > 27 or midh > 75){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
      delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
  }
  digitalWrite(light, LOW);
}

void allsbad(){
  // закрываем люк
  lamp();
  //открываем люк
}

void closehatch(){
  //закрываем люк
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(light,OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Hello word!");
  delay(5000);
  lcd.clear();
}

void loop() {

  int midh = 0;
  int midt = 0;
  for (int i = 0; i < n; i ++){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
    // Wait a few seconds between measurements.
    delay(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    midh += h;
    midt += t;
    output(h, t);
    delay(sec * 1000);
  }
  midh /= n;
  midt /= n;
  
  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    //return;
  //}

  if (midt < 22){
    if (midh > 60) todo = lampON;
    else todo = hatchCLOSElampON;
  }
  if (midt > 27) todo = hatchOPEN;

  switch (todo) {
       case lampON:
            lamp();
            break;

       case hatchCLOSElampON:
            allsbad();
            break;

       case hatchOPEN:
            openhatch();
            break;
  }
  
  if (midh < 60){
    if (midh < 50) todo = hatchCLOSElampON;
    else{
      if (midt < 22) todo = lampON;
      else todo = hatchCLOSE;
    }
  }
  if (midh > 75) todo = hatchOPEN;

  switch (todo) {
       case lampON:
            lamp();
            break;

       case hatchCLOSElampON:
            allsbad();
            break;

       case hatchOPEN:
            openhatch();
            break;

       case hatchCLOSE:
            closehatch();
            break;
  }
}
