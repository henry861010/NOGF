#define DIR_PIN 2
#define STEP_PIN 3
#define MS1_PIN 4
#define MS2_PIN 5
#define MS3_PIN 6
#define STEPS_PER_REVOLUTION 200 // Full steps per revolution for a 1.8-degree step motor

/*
  (1/1):  MS1 = 0, MS2 = 0, MS3 = 0   => 000 ~ 0x00
  (1/2):  MS1 = 1, MS2 = 0, MS3 = 0   => 100 ~ 0x04
  (1/4):  MS1 = 0, MS2 = 1, MS3 = 0   => 010 ~ 0x02
  (1/8):  MS1 = 1, MS2 = 1, MS3 = 0   => 110 ~ 0x06
  (1/16): MS1 = 1, MS2 = 1, MS3 = 1   => 111 ~ 0x07
*/

void rotateMotor(double desiredAngle) {
    int dirPinState = desiredAngle >= 0 ? HIGH : LOW;
    digitalWrite(DIR_PIN, dirPinState);
    
    double fullStepAngle = 1.8; // Motor's full step angle
    desiredAngle = abs(desiredAngle); // Ensure angle is positive for calculation

    // Example: Rotate with different microstepping resolutions
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
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);

  Serial.begin(9600); // COM port baud rate
  //digitalWrite(DIR_PIN, HIGH); // Set motor direction to clockwise
  //setMicrostepping(0x02);
}

void loop() {

  rotateMotor((1.8/1)+(1.8*3/16));
  
  delay(2000); // Wait for 2 seconds before the next move
}

