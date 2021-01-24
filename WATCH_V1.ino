#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#include <DS3232RTC.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();



#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8

#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, rst);

float p = 3.1415926;

const float referenceVolts = 4.45;
const int batteryPin = A1;

//Thermistor
int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

//switch button
int BUTTON_PIN_H = 7;
int BUTTON_PIN_N = 6;
int BUTTON_PIN_S = 2;
int buttonStateH = 0;
int buttonStateN = 0;
int buttonStateS = 0;
bool flag = false;

void setup() {

  pinMode(BUTTON_PIN_H, INPUT);
  pinMode(BUTTON_PIN_N, INPUT);
  pinMode(BUTTON_PIN_S, INPUT);

  pinMode(BUTTON_PIN_H, INPUT_PULLUP);
  pinMode(BUTTON_PIN_N, INPUT_PULLUP);
  pinMode(BUTTON_PIN_S, INPUT_PULLUP);


  Serial.begin(9600);
  Serial.print("hello!");
  display.begin();

  Serial.println("init");
  uint16_t time = millis();
  display.fillScreen(BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
  //lcdTestPattern();
  for (int i = 0; i < 20; i++) {
    display.fillScreen(BLACK);
    int ran = random(1, 4);
    if (ran == 1) {
      display.fillCircle(random(1, 90), random(1, 60), random(1, 10), GREEN);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 15), CYAN);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 20), RED);

    } else if (ran == 2) {
      display.fillCircle(random(1, 90), random(1, 60), random(1, 10), YELLOW);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 15), CYAN);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 20), MAGENTA);

    } else if (ran == 3) {
      display.fillCircle(random(1, 90), random(1, 60), random(1, 10), GREEN);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 15), YELLOW);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 20), RED);
    } else {
      display.fillCircle(random(1, 90), random(1, 60), random(1, 10), MAGENTA);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 15), RED);
      display.fillCircle(random(1, 90), random(1, 60), random(1, 20), WHITE);
    }
    delay(50);
  }
  display.fillScreen(BLACK);
  display.setCursor(5, 25);
  display.setTextSize(2);
  display.setTextColor(CYAN);
  display.println("MeawCom");

  delay(1000);

  display.fillScreen(BLACK);//BLUE
  display.fillRect(13, 18, 25, 20, BLACK);//clear screen h RED
  display.fillRect(53, 18, 25, 20, BLACK);//clear screen n GREEN

  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  if (! sht31.begin(0x44))
  {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

}

int i = 0;
bool isTime = true;
int h = 0;
int n = 0;
int x = 5;
int xq = 85;
bool isRun = false;
String message = ">>-->";

void loop() {
  digitalClockDisplay();
  float h = sht31.readHumidity();
  display.fillRect(3, 40, 90, 10, YELLOW);//clear point cursor
  display.setCursor(5, 42);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.print(h);
  display.print(" Hum.");

  float t = sht31.readTemperature();
  float celsius = t;
  float fahrenheit = celsius * 9.0 / 5.0 + 32.0;

  display.fillRect(3, 50, 90, 12, BLACK);//clear point cursor
  display.setCursor(5, 52);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print((int)celsius);
  display.print("C/");
  display.print((int)fahrenheit);
  display.print("F");

  ClockTime();


}
void digitalClockDisplay()
{
  // digital clock display of the time

  printHourDigits(hour());
  printMinuteDigits(minute());
  printSecondDigits(second());
  //Serial.println();
}

void printHourDigits(int digits)
{
  Serial.print(digits);
  display.setCursor(15, 20);
  display.setTextSize(2);
  display.setTextColor(CYAN);//WHITE
  display.fillRect(13, 18, 25, 20, BLACK);//clear screen h RED
  if (digits < 10) {
    display.print('0');
  }
  display.print(digits);
}
void printMinuteDigits(int digits)
{
  display.setCursor(55, 20);
  display.setTextSize(2);
  display.setTextColor(CYAN);//WHITE
  display.fillRect(53, 18, 25, 20, BLACK);//clear screen n  GREEN
  if (digits < 10) {
    display.print('0');
  }
  display.print(digits);
}
void printSecondDigits(int digits)
{
  display.setCursor(80, 25);
  display.setTextSize(1);
  display.setTextColor(RED);
  display.fillRect(78, 23, 15, 10, BLACK);//clear point cursor
  if (digits < 10) {
    display.print('0');
  }
  display.print(digits);
}
void ClockTime() {
  if (flag == false) {
    i++;

    if (x == 50 || isRun) {
      isRun = true;
      message = "<--<<";

      x--;
      if (x == 11) {
        xq = 85;
      }
      if (x == 5) {
        isRun = false;
      }
    } else {
      message = ">>-->";
      x++;
      if (x == 50) {
        xq = 5;
      }
    }
    if (i <= 59) {
      isTime = true;
    } else {
      n += 1;
      i = 0;
      isTime = false;
    }
    if (n >= 60) {
      h += 1;
      n = 0;
      if (h >= 23) {
        h = 0;
        n = 0;
      }
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(x, 5);
    display.fillRect(3, 2, 90, 12, BLACK);//clear point cursor
    display.print(message);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(xq, 5);
    //display.fillRect(3, 2, 90, 12, BLACK);//clear point cursor
    display.print("Q");


    display.setTextColor(WHITE);
    display.setCursor(40, 20);
    display.setTextSize(2);
    display.fillRect(40, 18, 10, 20, BLACK);//clear screen :
    display.print(":");

    int val = analogRead(batteryPin);
    float valts = (val / 1024.0) * referenceVolts;
    display.setCursor(55, 52);
    display.setTextSize(1);
    display.setTextColor(GREEN);
    //display.fillRect(54, 50, 36, 10, BLUE);//clear screen n
    display.print(valts);
    display.print("V.");

    display.drawFastHLine(1, 60, display.width() - 1, YELLOW); // blue line x=1 to screen width-1 and y=60
    int bStateH = digitalRead(BUTTON_PIN_H);
    int bStateN = digitalRead(BUTTON_PIN_N);
    onReadInputButton();
    delay(980);
  } else {
    onReadInputButton();
    delay(980);
  }

}

void onReadInputButton() {
  buttonStateS = digitalRead(BUTTON_PIN_S);

  if (buttonStateS == 0) {
    flag = !flag;
  } else {

  }
  if (flag) {
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(15, 5);
    display.fillRect(3, 2, 90, 12, YELLOW);//clear point cursor  //CYAN MAGENTA YELLOW
    display.print("QWATCH V1.0");

    buttonStateH = digitalRead(BUTTON_PIN_H);
    buttonStateN = digitalRead(BUTTON_PIN_N);
    if (buttonStateH == 0) {
      h++;
      if (h >= 24) {
        h = 0;
      }
    }
    if (buttonStateN == 0) {
      n++;
      if (n >= 60) {
        n = 0;
      }
    }
    setTime(h, n, 0, 15, 1, 2019);   //set the system time to 23h31m30s on 13Feb2009
    RTC.set(now());
  } else {
  }
}
void lcdTestPattern(void)
{
  uint8_t w, h;
  display.setAddrWindow(0, 0, 96, 64);

  for (h = 0; h < 64; h++)
  {
    for (w = 0; w < 96; w++)
    {
      if (w > 83) {
        display.writePixel(WHITE);
      }
      else if (w > 71) {
        display.writePixel(BLUE);
      }
      else if (w > 59) {
        display.writePixel(GREEN);
      }
      else if (w > 47) {
        display.writePixel(CYAN);
      }
      else if (w > 35) {
        display.writePixel(RED);
      }
      else if (w > 23) {
        display.writePixel(MAGENTA);
      }
      else if (w > 11) {
        display.writePixel(YELLOW);
      }
      else {
        display.writePixel(BLACK);
      }
    }
  }
  display.endWrite();
}
