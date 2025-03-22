#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>  // Include Arduino functions

class Motor {
private:
    int dir_pin;
    int step_pin;
    int MS1_pin;
    int MS2_pin;
    int MS3_pin;
    int enable_pin;

    int stepSize = 1; //1, 2, 4, 8, 16
    int stepAngle = 1.8; //degree(360)
    float speed = 200; //RPM
    float delayPerStep;

public:
// constructor
    Motor(int _dir_pin, int _step_pin);
    Motor(int _dir_pin, int _step_pin, int _MS1_pin, int _MS2_pin, int _MS3_pin, int _enable_pin);

    // Methods - setup
    void set_pin();
    void set_stepSize(int _stepSize);
    void set_speed(float _speed);

    // Method - 
    void move_step(bool direction);
};

#endif
