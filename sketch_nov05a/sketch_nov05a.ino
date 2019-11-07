#include <Arduino_FreeRTOS.h>
#include <croutine.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <StackMacros.h>
#include <task.h>
#include <timers.h>
#include <dht.h>
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h" 



const int headLight1=50;
const int headLight2=51;


const int lightSensor = A2;


//temperature and humidity sensor 
dht DHT;
int tempSensor= 52;
float hum;  //Stores humidity value
float temp; //Stores temperature value

//motors
int motorR1=31;
int motorR2=33;
int motorL1=30;
int motorL2=32;
int speedR=3;
int speedL=2;

//ultrasonic sensor
const int trigPin = 35;
const int echoPin = 37;
bool brake;
long duration;
int distance;

//joystick pins
//const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A0; // analog pin connected to X output
const int Y_pin = A1; // analog pin connected to Y output


int brakeLight=53;


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
  //pinMode(SW_pin, INPUT);
  //digitalWrite(SW_pin, HIGH);
  
  xTaskCreate(control,"Drive",100,NULL,3,NULL);
  //xTaskCreate(dashboard,"dashboard",100,NULL,1,NULL);
  xTaskCreate(headLights,"headLights",100,NULL,2,NULL);
 
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



void headLights(void *pvParameters){
   
  while(1){
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
    vTaskDelay(pdMS_TO_TICKS ( 100 ));
  }
  
}

//to control the direction P D N R
void control(void *pvParameters){
  //Serial.println("D is running Driving");
  while(1){ 
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
      vTaskDelay(pdMS_TO_TICKS ( 80 ));
    }
}
