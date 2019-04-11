#include "DHT.h"
#define light 2

#define min 0.2        // период считывания данных с датчика для их последующего усреднения в минутах
#define n 5    // кол-во значений для усреднения

#include <Servo.h>
Servo servo;
int norm 90       //нормальное значение высоты люка (угол)

#define lampON 21
#define hatchCLOSE 22
#define hatchOPEN 23
#define hatchCLOSElampON 24

int openings;       //кол-во открытий крышки
int closings;       //кол-во закрытий крышки, ф-ция allsbad считается за два
int count;          //кол-во выполнения loop

#define DHTPIN 7    // what digital pin we're connected to
#include <LiquidCrystal.h>
int backlight = 3, todo;
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

void openhatch(){
  servo.write(180);       //открываем люк на максимум
  float midh, midt;
  int i;
  for (i = 0; i < 5; i ++){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      delay(1000);
    }
  output(midh/5, midt/5);
  while(midt/i > 22; midh/i > 55){                        //пока данные выйдут за границу нормы, продолжаем
      delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      i ++;
  }
  servo.write(norm);      //возвращаем люк в норму
}

void lamp(){
  digitalWrite(light, HIGH);
  float midh, midt;
  int i;
  for (i = 0; i < 5; i ++){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      delay(1000);
    }
  output(midh/5, midt/5);
  while(midt/i < 30 or midh/i < 75){                        //пока данные выйдут за границу нормы, продолжаем
      delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      i ++;
  }
  digitalWrite(light, LOW);
}

void allsbad(){
  servo.write(0);     //закрываем люк
  lamp();
  servo.write(norm);    //возвращаем люк в норму
}

void closehatch(){
  servo.write(0);       //закрываем люк
  float midh, midt;
  int i;
  for (i = 0; i < 5; i ++){                        //считываем несколько значений и находим среднее, чтобы сгладить разброс
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      delay(1000);
    }
  output(midh/5, midt/5);
  while(midh/i < 75){                        //пока данные выйдут за границу нормы, продолжаем
      delay(2000);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      midh += h;
      midt += t;
      i ++;
  }
  servo.write(norm);      //возвращаем люк в норму
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(light,OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Hello word!");
  delay(5000);
  lcd.clear();
  servo.attach(9);
}

void loop() {
  int k = 15;
  if(count = k){
    if(closings - openings > 2 and norm <= 175) norm += 5;
    if(openings - closings > 2 and norm >= 5) norm -= 5;
  }

  delay(min * 60000);
  
  float midh = 0, midt = 0;
  for (int i = 0; i < 12; i ++){                        //считываем 12 значений на протяжении 3 минут (каждую четверть минуты)и находим среднее, чтобы сгладить разброс
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    midh += h;
    midt += t;
    output(h, t);
    delay(250);
  }
  midh /= n;
  midt /= n;
  
  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    //return;
  //}
  
  k = 5;
  
  if (midt < 22){
    if (midh > 60) todo = lampON;
    else todo = hatchCLOSElampON;
  }
  if (midt > 27) todo = hatchOPEN;


  switch (todo) {
       case lampON:
            Serial.println("lamp on");
            lamp();
            break;

       case hatchCLOSElampON:
            Serial.println("ALL'SBAD!!!");
            allsbad();
            break;

       case hatchOPEN:
            Serial.println("hatch open");
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
            Serial.println("lamp on");
            lamp();
            break;

       case hatchCLOSElampON:
            Serial.println("ALL'SBAD!!!");
            allsbad();
            break;

       case hatchOPEN:
            Serial.print("hatch open");
            openhatch();
            break;

       case hatchCLOSE:
            Serial.println("hatch close");
            closehatch();
            break;
  }
}
