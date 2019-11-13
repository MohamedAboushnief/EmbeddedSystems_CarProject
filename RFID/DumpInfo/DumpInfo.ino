/*
 * 
 * All the resources for this project: https://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
 
   // Create MFRC522 instance.




#include <Arduino_FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <dht.h>
#include <Adafruit_GFX.h>    
#include <MCUFRIEND_kbv.h>
//#include <list.h>
//#include <mpu_wrappers.h>
//#include <portable.h>
//#include <portmacro.h>
//#include <projdefs.h>
//#include <queue.h>
//#include <semphr.h>
//#include <SPI.h>
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



 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     //content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println("vvvvvvsssssss");
  Serial.println(content);
  Serial.println("dxddddddddddd");
  if (mfrc522.uid.uidByte[0], HEX == 27 && mfrc522.uid.uidByte[1], HEX == 28 && mfrc522.uid.uidByte[2], HEX == 21 && mfrc522.uid.uidByte[3], HEX == 34) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
  }
 
 else   {
    Serial.println(" Access denied");
    delay(3000);
  }
} 
