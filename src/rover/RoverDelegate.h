#ifndef ROVERDELEGATE_H
#define ROVERDELEGATE_H

#include <Arduino.h>
#include "data.h"

class RoverDelegate{
private:
  //
  static RoverDelegate* instance;

  // share info
  int command;
  state_t rover_state;

  // transmittion info   
  uint8_t roverAddress;   
  uint8_t baseAddress;

  /*
    command 1 [0]     uint8_t
    lat     8 [1~8]   double
    lon     8 [9~17]  double
    time    4 [18~21] int
    a_x     2 [22~23] int16_t
    a_y     2 [24~25] int16_t
    a_z     2 [26~27] int16_t
  */
  void encode_message(char* message);
  void decode_message(char* message);

public:
  RoverDelegate();
  void setting(int _csPin, int _resetPin, int _irqPin, uint8_t _roverAddress, uint8_t _baseAddress, role_e role);
  bool checkIfConnect();

  /*
    call by base
  */
  int get_rover_command();
  state_t get_rover_state();

  static void staticOnReceive_base(int packetSize);
  void onReceive_base(int packetSize);

  void send_to_rover(char* message);

  /*
    call by rover
  */
  void set_rover_gps(double lon, double lat);
  void set_rover_acc(int16_t a_x, int16_t a_y, int16_t a_z);
  void set_rover_time(int t);

  static void staticOnReceive_rover(int packetSize);
  void onReceive_rover(int packetSize);

  void send_roverState();
};

#endif
