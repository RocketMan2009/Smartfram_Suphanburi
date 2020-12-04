#include "ClosedCube_HDC1080.h"
#include "Magellan_SIM7020E.h"
#include <Wire.h>
#include <VL53L0X.h>
Magellan_SIM7020E magel;

VL53L0X sensor;

ClosedCube_HDC1080 hdc1080;

const int lightSensorPin=34; 
String sw_state;
String sw;
String payload;
const int ledPin = 2;      //LED on board Pin 2

void setup() 
{
  Serial.begin(115200);
    Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  Serial.println("ClosedCube HDC1080");

  // Default settings:
  // - Heater off
  // - 14 bit Temperature and Humidity Measurement Resolutions
  hdc1080.begin(0x40);
   
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);       // 0x1050 ID of the device
  
      
  magel.begin();                                     //Init Magellan LIB
  payload="{\"sw\":0}";    //Please provide payload with json format
  magel.report(payload);   //Initial switch data to Magellan 
  pinMode(ledPin,OUTPUT);

}

void loop() 
{
     ON_OFF_SW();
     Payload_sensors();
}
void ON_OFF_SW(void) {
  sw_state=magel.getControl("sw");  //Get switch data from Magellan IoT platform

  if(sw_state.indexOf("40300")==-1) //Response data is not 40300
  {
    sw=sw_state;
  }

  Serial.print("sw ");
  Serial.println(sw);

  if(sw=="1")
  {
    digitalWrite(ledPin,1); //Turn on LED
  }
  else
  {
    digitalWrite(ledPin,0); //Turn off LED
  }
}

void Payload_sensors(void) {
   /*
    Example read sensor on board and report data to Magellan IoT platform
  */
  String temperature=String(hdc1080.readTemperature());
  String humidity=String(hdc1080.readHumidity());
  String Light=String(analogRead(lightSensorPin));
  String Length=String(sensor.readRangeSingleMillimeters()/10);

  payload="{\"temperature\":"+temperature+",\"humidity\":"+humidity+",\"light\":"+Light+",\"Length_level\":"+Length+"}";  //Please provide payload with json format
  magel.report(payload);                                                                      //Report sensor data
  delay(5000);                                                                                //Delay 5 second
}
