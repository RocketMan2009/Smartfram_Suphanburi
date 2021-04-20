// =============NPKpH===========//
// ----------------NB IoT AIS-------------//
// --------------RS485 module -------------//
// --------------auto 485 module RED board ===//
// ---- need delay time 50 ms ---------------------//
#include <SoftwareSerial.h>
#include "CRC16.h"
//#include <String.h>


//Crc16 crc; 

//byte byteSend[20];
//byte HEADER=0xFF;
//byte servoID_1 = 0x02;
//byte servoALL = 0xFE;


SoftwareSerial SoilSerial(32,33);   // (RX,TX)


byte Adrr=0x01, Fcode=0x03;
//const int EnTxPin =  6;
//const int EnRxPin =  7;
//int counter = 0;
//const byte nitro[] = {Adrr,Fcode, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0C};
//const byte phos[] = {Adrr,Fcode, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xCC};
//const byte pota[] = {Adrr,Fcode, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
byte values[11];
int regval[2];
void setup() {
  //pinMode(EnTxPin, OUTPUT);
  //pinMode(EnRxPin, OUTPUT);
  //digitalWrite(EnRxPin, HIGH); 
  Serial.begin(9600);
  while (!Serial);
  SoilSerial.begin(9600);
 
  
}

void loop() {
  //Serial.print("RCV: ");
  //Serial.println(counter);
  
  unsigned int val1=0;
  val1 = modRead(0x06);//pH;
  Serial.print ("ph="); 
  Serial.println(val1*0.01);delay(250);
  
  val1 = modRead(0x12);//Hud;
  Serial.print ("Hud="); 
  Serial.println(val1*0.1);  delay(250);
  
  val1 = modRead(0x13);//Temp;
  Serial.print ("temp="); 
  Serial.println(val1*0.1);  delay(250);
  
  val1 = modRead(0x1e);//N;
  Serial.print ("N="); 
  Serial.println(val1);delay(250);
  
  val1 = modRead(0x1f);//P;
  Serial.print ("P="); 
  Serial.println(val1);delay(250);
  
  val1 = modRead(0x20); //K;
  Serial.print ("K="); 
  Serial.println(val1);  delay(250);
  
  val1 = modRead(0x16); //ec;
  Serial.print ("ec="); 
  Serial.println(val1);  delay(250);  
   
   delay(1000);
 
}

unsigned int modRead(byte reg){

  byte RegisterH=0x00,RegisterL=reg, len =0x01;
  byte x[6] = { Adrr, Fcode,RegisterH,RegisterL,0x00,len};
  uint16_t u16CRC=0xffff;
  for (int i = 0; i < 6; i++)
    {
        u16CRC = crc16_update(u16CRC, x[i]);

    }   //Serial.print(lowByte(u16CRC),HEX); Serial.print(",");
        //Serial.println(highByte(u16CRC),HEX);
  byte request[8] = { Adrr, Fcode,RegisterH,RegisterL,0x00,len, lowByte(u16CRC),highByte(u16CRC)};
  
  //digitalWrite(EnTxPin, HIGH); digitalWrite(EnRxPin, HIGH); 
  //delay(50);
  if(SoilSerial.write(request,sizeof(request))==8){
     //digitalWrite(EnTxPin, LOW); digitalWrite(EnRxPin, LOW); 
     delay(50);
     for(byte i=0;i<7;i++){
        
        values[i] = SoilSerial.read();
       // Serial.print(" 0x");
       // Serial.print(values[i],HEX);
        
     }
     //Serial.println("");
     //Serial.flush();
  }
   unsigned int val = (values[3]<<8)| values[4];
     
   //Serial.print(val);
  return val;
}

#ifndef _UTIL_CRC16_H_
#define _UTIL_CRC16_H_

/** @ingroup util_crc16
    Processor-independent CRC-16 calculation.

    Polynomial: x^16 + x^15 + x^2 + 1 (0xA001)<br>
    Initial value: 0xFFFF

    This CRC is normally used in disk-drive controllers.

    @param uint16_t crc (0x0000..0xFFFF)
    @param uint8_t a (0x00..0xFF)
    @return calculated CRC (0x0000..0xFFFF)
*/
static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}


#endif /* _UTIL_CRC16_H_ */
