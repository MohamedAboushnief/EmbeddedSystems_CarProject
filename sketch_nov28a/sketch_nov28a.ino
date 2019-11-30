#include <Arduino_FreeRTOS.h>
//#include <FreeRTOSConfig.h>
//#include <FreeRTOSVariant.h>
#include <dht.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
//#include <list.h>
//#include <mpu_wrappers.h>
//#include <portable.h>
//#include <portmacro.h>
//#include <projdefs.h>
//#include <queue.h>
#include <semphr.h>
#include <SPI.h>
#include <MFRC522.h>




//#include <StackMacros.h>
//#include <task.h>
//#include <timers.h>
//#include <croutine.h>
//#include <event_groups.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;
bool buttonDown = false;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

String gear = "P";
int SS_PIN = 53;
int RST_PIN = 49;

MFRC522 mfrc522(SS_PIN, RST_PIN);

const int headLight1 = 40;
const int headLight2 = 41;
bool firstStart = true;


const int lightSensor = A5;


//temperature and humidity sensor
dht DHT;
int tempSensor = 42;
float hum;  //Stores humidity value
float temp; //Stores temperature value

//motors
int motorR1 = 33;
int motorR2 = 31;
int motorL1 = 32;
int motorL2 = 30;
int speedR = 11;
int speedL = 10;

//ultrasonic sensor
const int trigPin = 35;
const int echoPin = 37;
bool brake;
long duration;
int distance;

//joystick pins
//const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A6; // analog pin connected to X output
const int Y_pin = A7; // analog pin connected to Y output

String tempGear = "";
int brakeLight = 43;

bool start = false;

int unlockLight = 48;

int powerButton = 12;

char command;

SemaphoreHandle_t xSemaphore;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(powerButton, INPUT);
  pinMode(brakeLight, OUTPUT);
  pinMode(headLight1, OUTPUT);
  pinMode(headLight2, OUTPUT);
  pinMode(unlockLight, OUTPUT);
  pinMode(powerButton, INPUT_PULLUP);



  //create semaphore
  xSemaphore = xSemaphoreCreateBinary();

  // LCD TFT ::::
  tft.reset();
  tft.begin(0x9481);
  tft.setRotation(1);
  tft.fillScreen(BLUE);



  tft.drawRect(0, 0, 480, 320, WHITE);
  delay(1000);

  tft.setCursor(78, 100);
  tft.setTextColor(CYAN);
  tft.setTextSize(3.8);
  tft.print("Enter key to unlock");

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  delay(1000);

  //clock
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // for example to set January 27 2017 at 12:56 you would call:
    //rtc.adjust(DateTime(2019, 11, 20, 5, 20, 0));
  }


  xTaskCreate(startEng, "startEngine", 600, NULL, 2, NULL);
  xTaskCreate(control, "Drive", 500, NULL, 4, NULL);
  xTaskCreate(headLights, "headLights", 100, NULL, 3, NULL);
  xTaskCreate(dashboard, "dashboard", 300, NULL, 2, NULL);

}



void loop() {

}

void move(char command) {
  if (command == 'F'  && !brake && gear == "D") {
    digitalWrite(motorR1, HIGH);
    digitalWrite(motorR2, LOW);
    analogWrite(speedR, 100);
    digitalWrite(motorL1, LOW);
    digitalWrite(motorL2, HIGH);
    analogWrite(speedL, 100);
    digitalWrite(brakeLight, LOW);

  }
  else if (command == 'B' && gear == "R" ) {
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, HIGH);
    analogWrite(speedR, 100);
    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, LOW);
    analogWrite(speedL, 100);
  }
  else if (command == 'L' && gear == "D") {
    analogWrite(speedR, 200);
    analogWrite(speedL, 150);
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, HIGH);


    digitalWrite(motorL1, LOW);
    digitalWrite(motorL2, HIGH);

  }
  else if (command == 'R' && gear == "D" ) {
    analogWrite(speedR, 150);
    analogWrite(speedL, 200);
    digitalWrite(motorR1, HIGH);
    digitalWrite(motorR2, LOW);
    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, LOW);

  }


  else {
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, LOW);
    digitalWrite(motorL1, LOW);
    digitalWrite(motorL2, LOW);
    if (brake) {
      digitalWrite(brakeLight, HIGH);
    }
    else {
      digitalWrite(brakeLight, LOW);

    }

  }

}


void startEng(void *pvParameters) {
  TickType_t xLast = xTaskGetTickCount();
  //if (!start) {
  while (1) {
    Serial.println(digitalRead(powerButton));

    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
    }
    if (mfrc522.uid.uidByte[0] == 39 && mfrc522.uid.uidByte[1] == 40 && mfrc522.uid.uidByte[2] == 33 && mfrc522.uid.uidByte[3] == 52) //change here the UID of the card/cards that you want to give access
    {
      digitalWrite(unlockLight, HIGH);
      Serial.println("Authorized access");
      Serial.println();

      if (digitalRead(powerButton) == HIGH ) {
        Serial.println("Car Ready to move");
        start = true;

        xSemaphoreGive(xSemaphore);

      }


    }

    else   {
      Serial.println("Access denied");
    }


    if (digitalRead(powerButton) == LOW && start == true) {
      Serial.println("sssssssssssssssssssssss");
      digitalWrite(13, LOW);
      buttonDown = true;
      start = false;
      digitalWrite(13, HIGH);

      digitalWrite(unlockLight, LOW);
      Serial.println("Car Locked aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
      xSemaphoreTake(xSemaphore, portMAX_DELAY);

      Serial.println();
      if ( ! mfrc522.PICC_IsNewCardPresent())
      {
      }
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial())
      {
      }
      if (mfrc522.uid.uidByte[0] == 39 && mfrc522.uid.uidByte[1] == 40 && mfrc522.uid.uidByte[2] == 33 && mfrc522.uid.uidByte[3] == 52) //change here the UID of the card/cards that you want to give access
      {
        digitalWrite(unlockLight, LOW);



      }
    }


    vTaskDelayUntil(&xLast, pdMS_TO_TICKS(100)); // to be removed after modifing the RFID
  }
  //}
}

void dashboard(void *pvParameters) {
  //LCD :::
  TickType_t xLast = xTaskGetTickCount();
  while (1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    if (firstStart) {
      tft.fillScreen(BLUE);
      tft.drawRect(0, 0, 480, 320, WHITE);
      firstStart = false;
    }


    DateTime now = rtc.now();
    int chk = DHT.read11(tempSensor);
    //Read data and store it to variables hum and temp
    if (hum != DHT.humidity || temp != DHT.temperature) {
      tft.fillRect(70, 95, 321, 50, BLUE);
    }
    hum = DHT.humidity;
    temp = DHT.temperature;


    //delay(1000);
    tft.setCursor(80, 100);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.print(hum);

    tft.setCursor(220, 100);
    tft.setTextColor(BLACK);
    tft.setTextSize(4);
    tft.print(temp);

    if (tempGear != gear) {
      tft.fillRect(70, 150, 25, 40, BLUE);
    }
    tft.setCursor(70, 150);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print(gear);
    tempGear = gear;

    // clock
    tft.setCursor(70, 215);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    //tft.println("Current Date & Time: ");
    tft.print(now.year(), DEC);
    tft.print('/');
    tft.print(now.month(), DEC);
    tft.print('/');
    tft.print(now.day(), DEC);
    tft.print(" (");
    tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
    tft.println(") ");
    tft.setCursor(70, 245);
    tft.print(now.hour(), DEC);
    tft.print(':');
    tft.print(now.minute(), DEC);
    if (rtc.now().minute() != now.minute()) {
      tft.fillRect(70, 85, 400, 265, BLUE);
    }



    xSemaphoreGive(xSemaphore);
    vTaskDelayUntil(&xLast, pdMS_TO_TICKS(80)); // to be removed after modifing the RFID
  }

}



void headLights(void *pvParameters) {
  TickType_t xLast = xTaskGetTickCount();

  while (1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    unsigned int AnalogValue;
    AnalogValue = analogRead(lightSensor);
    //Serial.println(AnalogValue);
    if (AnalogValue > 500) {
      digitalWrite(headLight1, HIGH);
      digitalWrite(headLight2, HIGH);
    }
    else {
      digitalWrite(headLight1, LOW);
      digitalWrite(headLight2, LOW);
    }


    xSemaphoreGive(xSemaphore);
    vTaskDelayUntil(&xLast, pdMS_TO_TICKS(100));
  }

}



//to control the direction P D N R
void control(void *pvParameters) {

  TickType_t xLast = xTaskGetTickCount();
  while (1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    if (distance < 25) {
      brake = true;
    }
    else {
      brake = false;
    }
    if (Serial1.available() > 0) {
      command = Serial1.read();

      move(command);

    }
    setGear(analogRead(X_pin), analogRead(Y_pin));



    xSemaphoreGive(xSemaphore);
    vTaskDelayUntil(&xLast, pdMS_TO_TICKS(30));
  }

}

void setGear(int x, int y) {
  if (x > 800) {
    gear = "D";
  }
  if (x < 100) {
    gear = "R";
  }
  if (y > 800) {
    gear = "N";
  }
  if (y < 100) {
    gear = "P";
  }
}
