#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

typedef struct gps{
    double lon;
    double lat;
} gps_t;

typedef struct acc{
    int16_t x;
    int16_t y;
    int16_t z;
} acc_t;

typedef struct rover{
    gps_t gps;
    acc_t acc;
    int time;
} state_t;

enum role_e{
  ROVER,
  BASE
};

#endif