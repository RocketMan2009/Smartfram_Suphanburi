#include "ClosedCube_HDC1080.h"
#include "Magellan_SIM7020E.h"
#include <Wire.h>
#include <VL53L0X.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

TaskHandle_t TaskSensor;
TaskHandle_t TaskSW;

Magellan_SIM7020E magel;

VL53L0X sensor;

ClosedCube_HDC1080 hdc1080;

const int lightSensorPin=34; 
String sw_state;
String sw;
String payload;
const int ledPin = 2;      //LED on board Pin 2

void TaskPayload_sensors( void *pvParameters );
void TaskON_OFF_SW( void *pvParameters );

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
  //pinMode(ledPin,OUTPUT);

  xTaskCreatePinnedToCore(
      TaskPayload_sensors, /* Function to implement the task */
      "TaskPayload_sensors", /* Name of the task */
      10000, /* Stack size in words */
      NULL, /* Task input parameter */
      2, /* Priority of the task */
      &TaskSensor, /* Task handle. */
      ARDUINO_RUNNING_CORE); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskON_OFF_SW, /* Function to implement the task */
      "TaskON_OFF_SW", /* Name of the task */
      10000, /* Stack size in words */
      NULL, /* Task input parameter */
      0, /* Priority of the task */
      &TaskSW, /* Task handle. */
      ARDUINO_RUNNING_CORE); /* Core where the task should run */
}

void loop() 
{
  // Empty. Things are done in Tasks.
}
void TaskPayload_sensors(void *pvParameters)  // This is a task.
{
    for (;;) // A Task shall never return or exit.
  {
      String temperature=String(hdc1080.readTemperature());
      String humidity=String(hdc1080.readHumidity());
      String Light=String(analogRead(lightSensorPin));
      String Length=String(sensor.readRangeSingleMillimeters()/10);

      payload="{\"temperature\":"+temperature+",\"humidity\":"+humidity+",\"light\":"+Light+",\"Length_level\":"+Length+"}";
      magel.report(payload);
      vTaskDelay(5000);
  }
}
void TaskON_OFF_SW(void *pvParameters)  // This is a task.
{
   (void) pvParameters;
  pinMode(ledPin,OUTPUT);
    for (;;) // A Task shall never return or exit.
  {
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
  vTaskDelay(10);
  }
