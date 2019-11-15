/*
PINOUT:
RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D9
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V
*/
/* Include the standard Arduino SPI library */
#include <SPI.h>
/* Include the RFID library */
#include <RFID.h>
#include <Arduino_FreeRTOS.h>

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 53
#define RESET_DIO 49


/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 

void setup()
{ 
  Serial.begin(9600);
  /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();
}

void loop()
{
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.println("Card detected:");
    for(int i=0;i<5;i++)
    {
    Serial.println(RC522.serNum[i],DEC);
    String x = String(RC522.serNum[i],DEC);
    Serial.println(x);
    //Serial.print(RC522.serNum[i],HEX); //to print card detail in Hexa Decimal format
    }
    Serial.println();
    Serial.println();
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}
