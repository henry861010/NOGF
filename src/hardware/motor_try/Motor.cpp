#include "Motor.h"

Motor::Motor(int _dir_pin, int _step_pin) {
    dir_pin = _dir_pin;
    step_pin = _step_pin;
    MS1_pin = -1;
    MS2_pin = -1;
    MS3_pin = -1;
    enable_pin = -1;
}

Motor::Motor(int _dir_pin, int _step_pin, int _MS1_pin, int _MS2_pin, int _MS3_pin, int _enable_pin) {
    dir_pin = _dir_pin;
    step_pin = _step_pin;
    MS1_pin = _MS1_pin;
    MS2_pin = _MS3_pin;
    MS3_pin = _MS3_pin;
    enable_pin = _enable_pin;
}

void Motor::set_pin() {
    /*
        set the step, dir, MS1, MS2, and MS3 pin to deigital output
    */
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    if (MS1_pin > 0 && MS2_pin > 0 && MS3_pin > 0) {
        pinMode(MS1_pin, OUTPUT);
        pinMode(MS2_pin, OUTPUT);
        pinMode(MS3_pin, OUTPUT);
    }
    if (enable_pin > 0) {
        pinMode(enable_pin, OUTPUT);
        digitalWrite(enable_pin, LOW);
    }
}

void Motor::set_stepSize(int _stepSize) {
    /*
        set the step size. the valid value is 1(full), 2(half), 4(quater), 8(eighth) and 16(sixteenth).
        if is the unvalid vale *out of (1, 2, 4, 8, 16), then it is setted to 1 (full step)

        * the step size determine how smooth the stepper motor move (it affect the speed of the motor, but not the only one factor)
            * the larger size per step, the faster but more unsmooth
            * the smaller size pre step, the slower but more smooth
    */
    switch(_stepSize) {
        case 2:
            stepSize = 2;
            digitalWrite(MS1_pin, HIGH);
            digitalWrite(MS2_pin, LOW);
            digitalWrite(MS3_pin, LOW);
            break;
        case 4:
            stepSize = 4;
            digitalWrite(MS1_pin, LOW);
            digitalWrite(MS2_pin, HIGH);
            digitalWrite(MS3_pin, LOW);
            break;
        case 8:
            stepSize = 8;
            digitalWrite(MS1_pin, HIGH);
            digitalWrite(MS2_pin, HIGH);
            digitalWrite(MS3_pin, LOW);
            break;
        case 16:
            stepSize = 16;
            digitalWrite(MS1_pin, HIGH);
            digitalWrite(MS2_pin, HIGH);
            digitalWrite(MS3_pin, HIGH);
            break;
        default:
            stepSize = 1;
            digitalWrite(MS1_pin, LOW);
            digitalWrite(MS2_pin, LOW);
            digitalWrite(MS3_pin, LOW);
            break;
    }
}

void Motor::set_speed(float _speed) {
    /*
        set the speed of motor
        * use "rpm" as unit of the speed

        stepPerRotation = 360 / (stepSize * stepAngle)
        delayPerRotation = stepDelay * stepPerRotation
        rotationPerSec = rpm / 60
        delayPerRotation = 60 / rpm

        60 / rpm = stepDelay * stepPerRotation 
        => 60 / rpm = stepDelay * (360 / (stepSize * stepAngle))
        => stepDelay (s) = (60 * stepSize * stepAngle) / (rpm * 360) = stepSize * stepAngle / (rpm * 6)
        => stepDelay (ms) = 1000 * stepSize * stepAngle / (rpm * 6)
    */
    speed = _speed;
    delayPerStep = 1000 * stepSize * stepAngle / (speed * 6);
}

void Motor::move_step(bool direction) {
    // set the direction
    digitalWrite(dir_pin, direction ? HIGH : LOW);

    digitalWrite(step_pin, HIGH);
    delay(delayPerStep);
    digitalWrite(step_pin, LOW);
    delay(delayPerStep);
}

