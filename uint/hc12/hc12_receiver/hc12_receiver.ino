#include <SoftwareSerial.h>

const int HC12_RX = 7; //connec to tx
const int HC12_TX = 6; // Corrected to rx

SoftwareSerial hc12(HC12_TX, HC12_RX); 

static char hc12message[100];
static char hc12Now[100];


bool getMessage_hc12(){
  static int length = 0;
  while(hc12.available()){
    char c = hc12.read();
    if(c=='\n'){ //get the eniter sentence from the gps
      hc12message[length]='\0';
      strcpy(hc12Now,hc12message);
      length = 0;
      return true;
    }
    else{ //get the char, but the sentence is not finish
      hc12message[length]=c;
      length++;
    }
  }
  return false;
}

void setup()  
{ 
  Serial.begin(9600); //COMPORT鮑率
  hc12.begin(9600);//GPS
}

void loop() // run over and over
{
  if(getMessage_hc12()){
    Serial.println(hc12Now);
  }
}

