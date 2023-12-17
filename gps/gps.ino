#include <SoftwareSerial.h>

const int GPS_RX = 4;
const int GPS_TX = 3;
const int HC12_RX = 7;
const int HC12_tX = 6;

SoftwareSerial gps(GPS_RX, GPS_TX); 
SoftwareSerial hc12(HC12_RX, HC12_tX); 

static char gpsmessage[100];
static char gpsNow[100];
static int length = 0;
bool ifCopy = false;
char c;

bool ifGPRMC(char* s){
  if(s[1]=='G' and s[2]=='P' and s[3]=='R' and s[4]=='M' and s[5]=='C') return true;
  else return false;
}

bool getMessage(){
  if(gps.available()){
    c = gps.read();
    if(c=='\n'){
      gpsmessage[length]='\0';
      strcpy(gpsNow,gpsmessage);
      length = 0;
      return true;
    }
    else{
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
  while(getMessage()){
    if(ifGPRMC(gpsNow)){
      Serial.println(gpsNow);
      hc12.write(gpsNow);
    }
  }
}
