#ifndef NOGFUTILS_H
#define NOGFUTILS_H

#include <Arduino.h>

#define EARTH_RADIUS_KM 6371.0

// Function declarations
double toRadians(double degree);
double cal_bearing(double base_lat, double base_lon, double target_lat, double target_lon);
bool decode_ble_message(String message, double &base_lat, double &base_lon, double &target_lat, double &target_lon, double &velocity, int &direction,  double &angle);

/*
    determine the angle gap from the base_angle to target_angle
        positive: the gap is the clockwise from base_angle to target_angle
        negative: the gap is the ccounterlockwise from base_angle to target_angle
    NOTE: the return calculate the smaller amount of angle reuired to move from. form example, target=330
          and base=0. rotate from base to target in clockwise required 330 degree; rotate from base to target in 
          counter-clockwise required 30 degree, so the function return 30 because of less angle reuired.
*/
double cal_angle_gap(double base_angle, double target_angle);

double cal_velocityToRpm(double velocity);
#endif