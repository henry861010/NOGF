#include <SoftwareSerial.h>
#include <string.h>

const int GPS_RX = 4;
const int GPS_TX = 3;

const int HC12_RX = 7; 
const int HC12_TX = 6; 

SoftwareSerial gps(GPS_RX, GPS_TX);
SoftwareSerial hc12(HC12_TX, HC12_RX);

static char gpsmessage[100];
static char gpsNow[100];

bool ifGPRMC(char* s){
  if(s[1]=='G' and s[2]=='P' and s[3]=='R' and s[4]=='M' and s[5]=='C') return true;
  else return false;
}

bool getMessage(){
  static int length = 0;
  while(gps.available()){
    char c = gps.read();
    if(c=='\n'){ //get the eniter sentence from the gps
      gpsmessage[length]='\0';
      strcpy(gpsNow,gpsmessage);
      length = 0;
      return true;
    }
    else{ //get the char, but the sentence is not finish
      gpsmessage[length]=c;
      length++;
    }
  }
  return false;
}

void setup()  
{ 
  Serial.begin(9600); //COMPORT鮑率
  gps.begin(9600);//GPS
}

void loop() // run over and over
{
  if(getMessage()){
    if(ifGPRMC(gpsNow)){
      Serial.println(gpsNow);
      hc12.print(gpsNow);
    }
  }
}
