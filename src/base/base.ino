#include "RoverDelegate.h"
#include <SoftwareSerial.h>

// for GPS
#include <TinyGPSPlus.h>

// for GY-511
#include <Wire.h>
#include <LSM303.h>

#define CSPIN 7
#define RESETPIN 6
#define IRQPIN 1
#define ROVER_ADDRESS 0
#define BASE_ADDRESS 0

// create NOGF protocol object
RoverDelegate Rover;

// create GPS object
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps; // The TinyGPSPlus object
SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

// create FY-511 object
LSM303 compass;

// state 
state_t base_state;
state_t rover_state;

void setup() {
  // NOGF protocol object initialize
  Rover.setting(CSPIN, RESETPIN, IRQPIN, ROVER_ADDRESS, BASE_ADDRESS, role_e(BASE));
  while(!Rover.checkIfConnect()){}  

  // GPS object initialize
  Serial.begin(115200);
  ss.begin(GPSBaud);

  // FY-511 object initialize
  Wire.begin();
  compass.init();
  compass.enableDefault();
}

void loop() {
  set_gps_info();
  
}

void set_gps_info(){
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      // get lon ans lat
      if (gps.location.isValid()){
        base_state.gps.lon = gps.location.lng();
        base_state.gps.lat = gps.location.lat();
      }
      // get time
      if (gps.time.isValid()){
        base_state.time = gps.time.value();
      }
    }
  }
}