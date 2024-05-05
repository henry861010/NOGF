#include <SoftwareSerial.h>

const int HC12_RX = 7; 
const int HC12_TX = 6; 

SoftwareSerial hc12(HC12_TX, HC12_RX); // Corrected typo in variable name
int counter = 0; // Added semicolon to end the statement

void setup()  
{ 
  Serial.begin(9600); // COM port baud rate
  hc12.begin(9600);   // HC12 module baud rate
}

void loop() // Run repeatedly
{
  delay(2000); // Stop for 2 seconds
  
  // Send data over HC12 module
  hc12.print("hello world");
  hc12.print(counter); // Print counter value
  hc12.print("\n"); // Print new line
  Serial.println(counter);
  counter++; // Increment counter
}

