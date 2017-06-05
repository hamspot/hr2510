/******************************************************************
| Library for Uniden HR-2510, HR-2600, Lincoln, 2830 Display Unit | 
| https://github.com/hamspot/hr2510                               |
|                                                                 |
| Based on KTM-S1201 Arduino library                              |        |
| Developed and maintanied by MCUdude                             |
| https://github.com/MCUdude/KTMS1201                             |            
|                                                                 |
| Based on Tronixstruff's work                                    |
| tronixstuff.com/2013/03/11/arduino-and-ktm-s1201-lcd-modules    |
| Released to the public domain                                   |
******************************************************************/

// Include the library code
#include "HR2510_PB112.h"

// Pin definitions
byte N_SCK = 13;
byte SI = 11;
byte CD = 9;
byte RESET = 6;
byte BUSY = 5;
byte CS = 10;

// Initialize the library with the interface pins
HR2510_PB112 pb112(N_SCK, SI, CD, RESET, BUSY, CS);

void setup()
{
	// Start the serial interface
  Serial.begin(9600);
  
  // Set up the LCD
  pb112.master();
  
  // Print message
  pb112.setFrequency("999999");
  pb112.setChannel("02");
  pb112.setBand('A');
  pb112.setCursor(HR2510_PB112_LINE_1KHZ);
  //radio.dumpMemoryMap();
  
  Serial.println("Getting Frequency, Channel, Band...");
  Serial.println(pb112.getFrequency());
  Serial.println(pb112.getChannel());
  Serial.println(pb112.getBand());
  Serial.println(pb112.getCursor());
}


void loop() 
{

}
