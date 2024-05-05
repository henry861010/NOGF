#include "RoverDelegate.h"
#include <LoRa.h>
#include <SPI.h> 
#include <Arduino.h>
#include "data.h"

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

using namespace std;

RoverDelegate* RoverDelegate::instance = nullptr;

RoverDelegate::RoverDelegate(){
    instance = this;
}

void RoverDelegate::setting(int _csPin, int _resetPin, int _irqPin, uint8_t _roverAddress, uint8_t _baseAddress, role_e role){
  roverAddress = _roverAddress;
  baseAddress = _baseAddress;

  // settinf lora
  LoRa.setPins(_csPin, _resetPin, _irqPin);
  if (!LoRa.begin(915E6)) {  // initialize ratio at 915 MHz
      Serial.println("lora init failed. Check your connections.");
  }

  switch(role) {
    case ROVER:
      LoRa.onReceive(staticOnReceive_rover);
      break;
    case BASE:
      LoRa.onReceive(staticOnReceive_base);
      break;
  }

  if(role == ROVER) LoRa.onReceive(staticOnReceive_rover);
  else LoRa.onReceive(staticOnReceive_base);
  LoRa.receive();

  // check if connect to rover
  if(checkIfConnect()){}
}

bool RoverDelegate::checkIfConnect(){
  return true;
}

void RoverDelegate::decode_message(char* message){
  memcpy(&command, message, 1);
  memcpy(&rover_state.gps.lon, message + 1, 8);
  memcpy(&rover_state.gps.lat, message + 9, 8);
  memcpy(&rover_state.time, message + 17, 4);
  memcpy(&rover_state.acc.x, message + 21, 2);
  memcpy(&rover_state.acc.y, message + 23, 2);
  memcpy(&rover_state.acc.z, message + 25, 2);
}

void RoverDelegate::encode_message(char* message){
  memcpy(message + 0, &command, 1);
  memcpy(message + 1, &rover_state.gps.lon, 8);
  memcpy(message + 9, &rover_state.gps.lat, 8);
  memcpy(message + 17, &rover_state.time, 4);
  memcpy(message + 21, &rover_state.acc.x, 2);
  memcpy(message + 23, &rover_state.acc.y, 2);
  memcpy(message + 25, &rover_state.acc.z, 2);
  message[27] = '\0';
}

/*
  call by base
*/
int RoverDelegate::get_rover_command(){
  return command;
}

state_t RoverDelegate::get_rover_state(){
  return rover_state;
}

void RoverDelegate::staticOnReceive_base(int packetSize) {
    if (instance) {
        instance->onReceive_base(packetSize);
    }
}

void RoverDelegate::onReceive_base(int packetSize){
    if(packetSize == 0) return; 

    char message[28];

    uint8_t recipient = LoRa.read();          // recipient address
    uint8_t sender = LoRa.read();            // sender address
    uint8_t messageLength = LoRa.read();    // incoming msg length
    
    int counter = 0;
    while(LoRa.available() && counter<28){
      message[counter] = LoRa.read();
      counter++;
    }

    // check if the message is not damaged
    if (messageLength != counter) {
        Serial.println("error: message length does not match length");
        return;
    }

    // check if the message is for base
    if (recipient != baseAddress && recipient != 0xFF) {
        Serial.println("This message is not for me.");
        return;                  
    }

    decode_message(message);
}

/*
  call by rover
*/
void RoverDelegate::set_rover_gps(double lon, double lat){
  rover_state.gps.lon = lon;
  rover_state.gps.lat = lat;
}

void RoverDelegate::set_rover_acc(int16_t a_x, int16_t a_y, int16_t a_z){
  rover_state.acc.x = a_x;
  rover_state.acc.y = a_y;
  rover_state.acc.z = a_z;
}

void RoverDelegate::set_rover_time(int t){
  rover_state.time = t;
}

void RoverDelegate::staticOnReceive_rover(int packetSize) {
    if (instance) {
        instance->onReceive_rover(packetSize);
    }
}
void RoverDelegate::onReceive_rover(int packetSize){
  // base state...
}

void RoverDelegate::send_roverState(){
  char message[28]; 
  encode_message(message);

  LoRa.beginPacket();                   // start packet
  LoRa.write(roverAddress);              // add destination address
  LoRa.write(baseAddress);             // add sender address
  LoRa.write(28);        // add payload length
  LoRa.print(message);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

