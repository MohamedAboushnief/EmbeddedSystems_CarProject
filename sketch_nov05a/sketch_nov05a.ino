#include <Arduino_FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <dht.h>
#include <Adafruit_GFX.h>    
#include <MCUFRIEND_kbv.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <SPI.h>
#include <MFRC522.h>


 

//#include <StackMacros.h>
#include <task.h>
#include <timers.h>
#include <croutine.h>
#include <event_groups.h>

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



MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


int SS_PIN = 53;
int RST_PIN = 49;

MFRC522 mfrc522(SS_PIN, RST_PIN);

const int headLight1=40;
const int headLight2=41;


const int lightSensor = A5;


//temperature and humidity sensor 
dht DHT;
int tempSensor= 42;
float hum;  //Stores humidity value
float temp; //Stores temperature value

//motors
int motorR1=33;
int motorR2=31;
int motorL1=32;
int motorL2=30;
int speedR=11;
int speedL=10;

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


int brakeLight=43;

bool start = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(motorR1,OUTPUT);
  pinMode(motorR2,OUTPUT);
  pinMode(motorL1,OUTPUT);
  pinMode(motorL2,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  pinMode(brakeLight,OUTPUT);
  pinMode(headLight1,OUTPUT);
  pinMode(headLight2,OUTPUT);

// LCD TFT ::::
  tft.reset();
  tft.begin(0x9481);
  tft.setRotation(1);
  tft.fillScreen(RED);
//  tft.fillScreen(GREEN);
//  tft.fillScreen(BLUE);
//  tft.fillScreen(BLACK);
  

//  tft.setCursor(135,215);
//  tft.setTextColor(WHITE);
//  tft.setTextSize(4);
//  tft.print("Subscribe");

  tft.drawRect(0,0,480,320,WHITE);
  delay(1000);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
    delay(1000);
//  Serial.begin(9600);   // Initiate a serial communication
//  SPI.begin();      // Initiate  SPI bus
//  mfrc522.PCD_Init();   // Initiate MFRC522
//  Serial.println("Approximate your card to the reader...");
//  Serial.println();
  
  //pinMode(SW_pin, INPUT);
  //digitalWrite(SW_pin, HIGH);

 
  
     xTaskCreate(startEng,"startEngine",600,NULL,1,NULL);
     
     xTaskCreate(control,"Drive",100,NULL,4,NULL);
     xTaskCreate(headLights,"headLights",100,NULL,3,NULL);
     xTaskCreate(dashboard,"dashboard",300,NULL,2,NULL);
  
 
}



void loop() {
//    int chk = DHT.read11(tempSensor);
//    //Read data and store it to variables hum and temp
//    hum = DHT.humidity;
//    temp= DHT.temperature;
//    //Print temp and humidity values to serial monitor
//    Serial.print("Humidity: ");
//    Serial.print(hum);
//    Serial.print(" %, Temp: ");
//    Serial.print(temp);
//    Serial.println(" Celsius");
//    delay(2000); //Delay 2 sec.


//      unsigned int AnalogValue;
//      AnalogValue = analogRead(lightSensor);
//      Serial.println(AnalogValue);

 // LCD :::
//  tft.fillRect(80,200,321,60,BLACK);
//  delay(1000);
//  tft.fillRect(80,200,321,60,RED);
//  tft.setCursor(135,215);
//  tft.setTextColor(WHITE);
//  tft.setTextSize(4);
//  tft.print("Subscribe");
//  delay(1000);

 

}

void move(int x,int y){

      if(x>=800  && !brake){
        digitalWrite(motorR1,HIGH);
        digitalWrite(motorR2,LOW);
        analogWrite(speedR,100);
        digitalWrite(motorL1,LOW);
        digitalWrite(motorL2,HIGH);
        analogWrite(speedL,100);
        digitalWrite(brakeLight,LOW);

      }
      else if(x<=100 ){
        digitalWrite(motorR1,LOW);
        digitalWrite(motorR2,HIGH);
        analogWrite(speedR,100);
        digitalWrite(motorL1,HIGH);
        digitalWrite(motorL2,LOW);
        analogWrite(speedL,100);
      }
      else{
        digitalWrite(motorR1,LOW);
        digitalWrite(motorR2,LOW);
        digitalWrite(motorL1,LOW);
        digitalWrite(motorL2,LOW);
        if(brake){
          digitalWrite(brakeLight,HIGH);
        }

      }
      
}

void startEng(void *pvParameters){
      TickType_t xLast = xTaskGetTickCount();
  while(1){
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
  }
  if (mfrc522.uid.uidByte[0] == 39 && mfrc522.uid.uidByte[1] == 40 && mfrc522.uid.uidByte[2] == 33 && mfrc522.uid.uidByte[3] == 52) //change here the UID of the card/cards that you want to give access
  {
    start = true;
    Serial.println("Authorized access");
    Serial.println();
    delay(1000);
    return;
  }
 
 else   {
    Serial.println("Access denied");
    delay(3000);
  }
   Serial.print("sssss");
  // vTaskDelayUntil(&xLast, pdMS_TO_TICKS(200));

  }
  
}

void dashboard(void *pvParameters){
    //LCD :::
    TickType_t xLast = xTaskGetTickCount();
   
      while(1){
         if(start){
              int chk = DHT.read11(tempSensor);
        //Read data and store it to variables hum and temp
        if(hum!=DHT.humidity || temp!=DHT.temperature){
           tft.fillRect(70,95,321,50,RED);
        }
        hum = DHT.humidity;
        temp= DHT.temperature;
        //Print temp and humidity values to serial monitor
//        Serial.print("Humidity: ");
//        Serial.print(hum);
//        Serial.print(" %, Temp: ");
//        Serial.print(temp);
//        Serial.println(" Celsius");
        //delay(2000); //Delay 2 sec.
        
        //delay(1000);
        tft.setCursor(80,100);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print(hum);
      
        tft.setCursor(220,100);
        tft.setTextColor(BLACK);
        tft.setTextSize(4);
        tft.print(temp);
      
        tft.fillRect(80,200, 321, 60, RED);
      
        tft.fillRect(80,200,321,60,BLACK);
       // delay(1000);
        tft.fillRect(80,200,321,60,RED);
        tft.setCursor(135,215);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("Subscribe");
       
    }
     vTaskDelayUntil(&xLast, pdMS_TO_TICKS(80));
    }
    
}



void headLights(void *pvParameters){
   TickType_t xLast = xTaskGetTickCount();

     while(1){
       if(start){
    unsigned int AnalogValue;
    AnalogValue = analogRead(lightSensor);
    Serial.println(AnalogValue);
    if(AnalogValue>500){
      digitalWrite(headLight1,HIGH);
      digitalWrite(headLight2,HIGH); 
    }
    else{
      digitalWrite(headLight1,LOW);
      digitalWrite(headLight2,LOW); 
    }
   
  }
   vTaskDelayUntil(&xLast, pdMS_TO_TICKS(100));
  }
 
}

//to control the direction P D N R
void control(void *pvParameters){
  //Serial.println("D is running Driving");
      TickType_t xLast = xTaskGetTickCount();

  while(1){ 
     if(start){
       // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    if(distance < 25){
      brake = true;
    }
    else{
      brake = false;
    }
      move(analogRead(X_pin),analogRead(Y_pin));
      
    
    }
    vTaskDelayUntil(&xLast, pdMS_TO_TICKS(150));
}
  
}
