#include <SoftwareSerial.h>
const int RX = 4;
const int TX = 3 ;
int count = 0;
 
SoftwareSerial gps(RX, TX);
 
void setup()
{
   Serial.begin(19200);
   gps.begin(9600);
}
 
void loop()
{
   if (gps.available())
   {
      Serial.print("test");
      Serial.print(count++);
      Serial.print(": ");

      char data;
      data = gps.read();
      Serial.print(data);
      Serial.println(".");
   }
   delay(1000);
}