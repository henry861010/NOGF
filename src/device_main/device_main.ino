#include <math.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LSM303.h>

'''
hardwaresetting
'''
//hc12
const int HC12_RX = 8; //connec to tx
const int HC12_TX = 9; // Corrected to rx
SoftwareSerial hc12(HC12_TX, HC12_RX); 

//compass
LSM303 compass;

//stepper motor
int DIR_PIN 2;
int STEP_PIN 3;
int MS1_PIN 4;
int MS2_PIN 5;
int MS3_PIN 6;
int STEPS_PER_REVOLUTION 200;


'''
program variable declare
'''
//hc12 message
char hc12_message[100];
char hc12_buffer[100];

//position
double camera_lat = 0.0;
double camera_lon = 0.0;
double mover_lat =0.0 ;
double mover_lon = 0.0;

//angle
double magneticDeclination = -5.06  //defaul magnetic declination in taiwan, can get the truth value from connected iphone
double bearing_trueN = 0;
double bearing_magneticN = 0;
double delta_angle = 0;

bool if_get_message_hc12(){
  static int length = 0;
  while(hc12.available()){
    char c = hc12.read();
    if(c=='\n'){ //get the eniter sentence from the gps
      hc12_buffer[length]='\0';
      strcpy(hc12_message, hc12_buffer);
      length = 0;
      return true;
    }
    else{ //get the char, but the sentence is not finish
      hc12_buffer[length]=c;
      length++;
    }
  }
  return false;
}

double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
  //https://www.lifewire.com/what-is-bearing-in-gps-1683320
  double delta_lat = lat2 - lat1;
  double delta_lon = lon2 - lon1;

  double bearing = atan2(delta_lat, delta_lon);
  bearing = bearing * (180 / PI);
  bearing = fmod((bearing + 360), 360);

  return bearing;
}

float getHeading(){
  compass.read();
  return compass.heading();
}

void rotateMotor(double desiredAngle) {
    int dirPinState = desiredAngle >= 0 ? HIGH : LOW;
    digitalWrite(DIR_PIN, dirPinState);
    
    double fullStepAngle = 1.8; // Motor's full step angle
    desiredAngle = abs(desiredAngle); // Ensure angle is positive for calculation

    // Rotate with different microstepping resolutions
    int microsteps[4] = {0, 4, 2, 6}; // Array of microstepping resolutions 1/1, 1/2, 1/4, 1/8
    for (int i = 0; i < 4; i++) {
        digitalWrite(MS1_PIN, microsteps[i] & 0x04); // Set MS1
        digitalWrite(MS2_PIN, microsteps[i] & 0x02); // Set MS2
        digitalWrite(MS3_PIN, microsteps[i] & 0x01); // Set MS3

        double effectiveStepAngle = fullStepAngle / (1<<i);
        int steps = (int)(desiredAngle / effectiveStepAngle);
        for (int step = 0; step < steps; step++) {
          Serial.print("*");
          digitalWrite(STEP_PIN, LOW);
          delayMicroseconds(1000);
          digitalWrite(STEP_PIN, HIGH);
          delayMicroseconds(1000); // Adjust speed
        }
        // Subtract the completed rotation from the desired angle
        desiredAngle -= steps * effectiveStepAngle;
    }
}

void setup() {
  Serial.begin(9600);

  //hc12
  hc12.begin(9600);//GPS

  //compass setting
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};

  //stepper motor
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);

}

void loop() {
  //1. check if there is new hc-12 message comming   lat,lon,mode,other data
  if(if_get_message_hc12()){
    //2. decode the message
    for(int i = 0; i < 8; i++) {
      mover_lat[i] = hc12_message[i];      // Copy first 8 characters for latitude
      mover_lon[i] = hc12_message[i + 8];  // Copy next 8 characters for longitude
    }

    //3. calculate the bearning
    bearing_trueN = calculateBearing(camera_lat, camera_lon, mover_lat, mover_lon);

    //4. calculate the angle between the mover and the magnetic declination
    bearing_magneticN = bearing_trueN + magneticDeclination;
    delta_angle = bearing_magneticN - getHeading();

    //5. move the camera
    rotateMotor(delta_angle);
  }
}














