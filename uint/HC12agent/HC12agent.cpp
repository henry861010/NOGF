#include "HC12agent.h"

#include "Arduino.h"

HC12agent::GPSagent(uint8_t hc12_rx, uint8_t hc12_tx){
    SoftwareSerial hc12(hc12_tx, hc12_rx); 
    _hc12_rx = hc12_rx;
    _hc12_tx = hc12_tx;
    _buf_len = 0;

    Serial.begin(9600); //COMPORT鮑率
    hc12.begin(9600);//GPS
}

HC12agent::checkNewMessage(){
    static int _buf_len = 0;
    while(hc12.available()){
        char c = hc12.read();
        if(c=='\n'){ //get the eniter sentence from the gps
        hc12message[_buf_len]='\0';
        strcpy(hc12Now,hc12message);
        _buf_len = 0;
        return true;
        }
        else{ //get the char, but the sentence is not finish
        hc12message[_buf_len]=c;
        _buf_len++;
        }
    }
    return false;
}

