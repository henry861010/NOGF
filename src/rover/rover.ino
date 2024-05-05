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

void setup() {
  // NOGF protocol object initialize
  Rover.setting(CSPIN, RESETPIN, IRQPIN, ROVER_ADDRESS, BASE_ADDRESS, role_e(ROVER));
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
  set_acc_info();

  Rover.send_roverState();
}

void set_gps_info(){
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      // get lon ans lat
      if (gps.location.isValid()){
        Rover.set_rover_gps(gps.location.lng(), gps.location.lat());
      }
      // get time
      if (gps.time.isValid()){
        Rover.set_rover_time(gps.time.value());
      }
    }
  }
}

void set_acc_info(){
  compass.read();
  Rover.set_rover_acc(compass.m.x, compass.m.y, compass.m.z);
}
