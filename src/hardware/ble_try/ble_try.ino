#include <SoftwareSerial.h>
#include <Motor.h>


#define ble_rt_pin 2
#define ble_tx_pin 3

SoftwareSerial BLESerial(ble_rt_pin, ble_tx_pin); // RX, TX
int SynchronizationPeriod

void setup()
{
  Serial.begin(9600);
  
  // set the serial for BLE bluetooth
  BLESerial.begin(9600);
}

void loop() // run over and over
{
  if (BLESerial.available()){  //如果藍牙有傳資料過來
    char i = BLESerial.read();  //把讀到的資料丟給i
  }

}