/*LoRa + NB +Magellan*/
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <ClosedCube_HDC1080.h>
#include "Magellan_SIM7020E.h"

Magellan_SIM7020E magel; 
ClosedCube_HDC1080 hdc1080;

//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 15 //pin 2
#define BAND 433E6 

int rssi;
String temperature="";
String humidity="";
String readingID="";
float temp;   //ประกาศตัวแปรเพื่อไปใช้เปลี่ยน String to float
float hud;    //ประกาศตัวแปรเพื่อไปใช้เปลี่ยน String to float
String sw_state;
String sw;
String payload;
const int ledPin = 2;      //LED on bo
   
TaskHandle_t Task1;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void SetupLoRa(){
  Serial.println("LoRa Receiver");
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module
 
  while (!LoRa.begin(BAND)) {
   Serial.println("Starting LoRa failed!");
   while (1);
  }
}

void SetupNB(){
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX);
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);
  delay(1000);
}
void ActiveSw(){
  payload="{\"sw\":0}";      //Please provide payload with json format
  magel.report(payload);     //Initial switch data to Magellan IoT Platform
  pinMode(ledPin,OUTPUT);
}

void setup() {
Serial.begin(115200);
magel.begin();             //Init Magellan LIB
SetupLoRa();
SetupNB();
ActiveSw();

  xTaskCreatePinnedToCore(
                    Payload_code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(100); 
}
    
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    Serial.print("Lora packet received: ");
    while (LoRa.available())    // Read packet
  {
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData); 
    
    int pos1 = LoRaData.indexOf('/');   
    int pos2 = LoRaData.indexOf('&');   
    readingID = LoRaData.substring(0, pos1);                   // Get readingID
    
    temperature = LoRaData.substring(pos1 +1, pos2);           // Get temperature
    temp=temperature.toFloat();
    humidity = LoRaData.substring(pos2+1, LoRaData.length());  // Get humidity
    hud =humidity.toFloat(); //แปลงข้อมูลแบบ string เป็น float
  }
    rssi = LoRa.packetRssi();       // Get RSSI
    Serial.print(" with RSSI ");    
    Serial.println(rssi);
    
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(F("*C"));

    Serial.print(F("Humidity = "));
    Serial.print(humidity);
    Serial.println(F("%"));
    
    Serial.println();

    ON_OFF_SW();
  }
}

/*************************ส่งค่าอุณหภูมิและความชื้นเข้า Magelland NB IoT*************/
void Payload_code( void * pvParameters ){

  for(;;){
    payload="{\"temperature\":"+temperature+",\"humidity\":"+humidity+"}";  //Please provide payload with json format
    magel.report(payload);
    vTaskDelay(10000);
  } 
}

/****************************************ON-OFF Switch on Magelland************************/

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
