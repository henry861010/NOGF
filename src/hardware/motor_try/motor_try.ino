#include "Motor.h"

#define dir_pin 9
#define step_pin 8
#define MS1_pin 7
#define MS2_pin 6
#define MS3_pin 5
#define enable_pin 4

Motor motor(dir_pin, step_pin, MS1_pin, MS2_pin, MS3_pin, enable_pin);

void setup() {
    motor.set_pin();
    motor.set_speed(200);
    Serial.begin(9600);
}

void loop() {
    delay(100);
    motor.move(10, 1);

    Serial.println("-");
}
