#include "ClosedCube_HDC1080.h"
#include "Magellan_SIM7020E.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Magellan_SIM7020E magel;

VL53L0X sensor;

ClosedCube_HDC1080 hdc1080;

const int lightSensorPin=34; 
String payload;

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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);       // 0x1050 ID of the device
  
      
  magel.begin();                                     //Init Magellan LIB
}

void loop() 
{
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
  Distanceshow();
  Tempshow();
  Humidityshow();
  Lightshow();
  delay(1000);
}

void Distanceshow(void) {
 int length_level =sensor.readRangeSingleMillimeters()/10;
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20,0);             // Start at top-left corner
  display.println(F("DISTANCE"));
  display.setTextSize(2); 
  display.setCursor(25,40);
  display.print(length_level);
  display.print((" cm"));
  display.display();
  delay(4000);
}

void Tempshow(void) {
 int Temp_level =hdc1080.readTemperature();
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20,0);             // Start at top-left corner
  display.println(F("TEMP"));
  display.setTextSize(2); 
  display.setCursor(25,40);
  display.print(Temp_level);
  display.print((char)247);
  display.print(("C"));
  display.display();
  delay(4000);
}

void Humidityshow(void) {
 int humidity_level =hdc1080.readHumidity();
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20,0);             // Start at top-left corner
  display.println(F("HUMIDITY"));
  display.setTextSize(2); 
  display.setCursor(25,40);
  display.print(humidity_level);
  display.print((" %"));
  display.display();
  delay(4000);
}

void Lightshow(void) {
 int light_level=analogRead(lightSensorPin);
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20,0);             // Start at top-left corner
  display.println(F("LIGHT"));
  display.setTextSize(2); 
  display.setCursor(25,40);
  display.print(light_level);
  display.print((" Lux"));
  display.display();
  delay(4000);
}
