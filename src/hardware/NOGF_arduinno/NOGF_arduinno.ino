#include <SoftwareSerial.h>
#include "Motor.h"
#include "NOGFUtiles.h"

#define dir_pin 9
#define step_pin 8
#define MS1_pin 7
#define MS2_pin 6
#define MS3_pin 5
#define enable_pin 4

#define ble_rt_pin 2
#define ble_tx_pin 3

#define ANGLE_LOWER_BOUND 1
#define ANGLE_UPPER_BOUND 1

SoftwareSerial BLESerial(ble_rt_pin, ble_tx_pin); // RX, TX
Motor motor(dir_pin, step_pin, MS1_pin, MS2_pin, MS3_pin, enable_pin);

// temp
double temp_base_lon = 0.0;
double temp_base_lat = 0.0;
double temp_target_lon = 0.0;
double temp_target_lat = 0.0;
double temp_target_velocity = 0.0;
int temp_wave_direction = 0.0;
double temp_base_angle = 0.0;

// base(camera) position
double base_lon = 0.0;
double base_lat = 0.0;

// mover(surfer) position
double target_lon = 0.0;
double target_lat = 0.0;
double target_velocity = 0.0;
int wave_direction = 0;  // 0:stop, -1:left-hand, 1:right-hand 

//NOGF
double target_angle;
double base_angle;

double quick_speed = 200; //rpm
double mover_speed = 200; //rpm
double slow_speed = 200;  //rpm

unsigned long previousMillis = 0;

bool ifNewBleMessage = false;

// store the message receive from ble
String bleMessage = "";

double cal_speed(double angle_gap) {
  // set motor speed rotate the motor
  // algorithm???
  if(ANGLE_LOWER_BOUND < angle_gap && angle_gap < ANGLE_UPPER_BOUND) {
    motor.set_speed(mover_speed);
  } else {
    motor.set_speed(quick_speed);
  }
}

void setup()
{
  Serial.begin(9600);
  
  // set the serial for BLE bluetooth
  BLESerial.begin(9600);
}

void loop()
{
  // read the ble message & decode message
  if (BLESerial.available()){
    char c = BLESerial.read();
    if (c == '\0') {  
        if(decode_ble_message(bleMessage, temp_base_lat, temp_base_lon, temp_target_lat, temp_target_lon, temp_target_velocity, temp_wave_direction, temp_base_angle)) {
          base_lon = temp_base_lon;
          base_lat = temp_base_lat;
          target_lon = temp_target_lon;
          target_lat = temp_target_lat;
          target_velocity = temp_target_velocity;
          wave_direction = temp_wave_direction;
          base_angle = temp_base_angle;

          target_angle = cal_bearing(base_lon, base_lat, target_lon, target_lat);
          mover_speed = cal_velocityToRpm(target_velocity);
        }
        bleMessage = ""; 
    } else {
      bleMessage += c;  
    }
  }


  double currentMillis = millis();
  if(previousMillis + motor.get_iterationDelay() < currentMillis) {
    //update the mover postion based on the perdicted speed (mover_speed)
    double temp_angle = target_angle + 360*mover_speed/60 * ((currentMillis - previousMillis)/1000);
    
    // calculate the angle to rotate
    double angle_gap = cal_angle_gap(base_angle, temp_angle);

    // set the speed
    motor.set_speed(cal_speed(angle_gap));

    // rotate the motor
    if(angle_gap > 0) {
      motor.move_step(1);
    } else if(angle_gap < 0) {
      motor.move_step(0);
    }
    base_angle += motor.get_stepSize();

    // set next time step
    previousMillis += motor.iterationDelay();
  }
}

