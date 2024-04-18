#ifndef HC12agent_h
#define HC12agent_h

#include "Arduino.h"
#include <SoftwareSerial.h>

class GPSagent{
public:
    GPSagent(uint8_t hc12_rx, uint8_t hc12_tx);

    bool checkNewMessage(void);

    char* getMessage(void);

private:
    uint8_t _hc12_rx;
    uint8_t _hc12_tx;
    uint8_t _buf_len;

    SoftwareSerial hc12;

    char _hc12message[100];
    char _hc12Now[100];
}