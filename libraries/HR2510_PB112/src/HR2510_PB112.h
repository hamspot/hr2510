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


#include "Arduino.h"

#ifndef HR2510_PB112_H
#define HR2510_PB112_H

// Digits with Decimal
//      ---   0x01
//0x10 |   |  0x02
//0x20  ---  
//0x40 |   |  0x04
//0x80  --- o 0x08 

// Digits with Underline
//      ---   0x01
//0x10 |   |  0x02
//0x20  ---  
//0x40 |   |  0x04
//0x80  --- 
//     -----  0x08 

/* LCD characters */
static const uint8_t numberOfCharacters = 16;
static const uint8_t sevenSegHex[16] = {
	0xD7, // 0
	0x06, // 1
	0xE3, // 2
	0xA7, // 3
	0x36, // 4
	0xB5, // 5
	0xF4, // 6
	0x07, // 7
	0xF7, // 8
	0x37, // 9
	0x77, // A
	0xF4, // B
	0xE0, // C
	0xE6, // D
	0xF1, // E
	0x71, // F
};

static const uint8_t charArray[36] = {
	'0', 
	'1', 
	'2', 
	'3', 
	'4', 
	'5', 
	'6', 
	'7', 
	'8', 
	'9', 
	'A', 
	'B', 
	'C', 
	'D', 
	'E', 
	'F', 
	// May Not Implement???
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};


/* ************************* DATA MODE *********************** */
/* Band and Channel Display - 7 Segment only */
const uint8_t HR2510_PB112_BAND = 0xF9; // Band Display
const uint8_t HR2510_PB112_CH_0 = 0xF7; // Channel X_
const uint8_t HR2510_PB112_CH_1 = 0xF5; // Channel _X

/* Frequency Digits - 7 Segment OR'd with Underlines and Dots */
const uint8_t HR2510_PB112_FR_0 = 0xF3; // Freq X_.___._
const uint8_t HR2510_PB112_FR_1 = 0xF1; // Freq _X.___._
const uint8_t HR2510_PB112_FR_2 = 0xEF; // Freq __.X__._
const uint8_t HR2510_PB112_FR_3 = 0xED; // Freq __._X_._
const uint8_t HR2510_PB112_FR_4 = 0xEB; // Freq __.__X._
const uint8_t HR2510_PB112_FR_5 = 0xE9; // Freq __.___.X

/* Decimal Point Memory Map */
const uint8_t HR2510_PB112_DOT_1MHZ[2] = {0xF1, 0x08}; // 88.8888
const uint8_t HR2510_PB112_DOT_1KHZ[2] = {0xEB, 0x08}; // 88888.8

/* Underline Memory Map */
const uint8_t HR2510_PB112_MAP_LINE_10KHZ[2]  = {0xEF, 0x08}; // _X.X
const uint8_t HR2510_PB112_MAP_LINE_1KHZ[2]   = {0xED, 0x08}; // X_.X
const uint8_t HR2510_PB112_MAP_LINE_100HZ[2]  = {0xE9, 0x08};  // XX._

/* Underline Flags */
const uint8_t HR2510_PB112_LINE_NONE   = 0x00; // XX.X
const uint8_t HR2510_PB112_LINE_10KHZ  = 0x01; // _X.X
const uint8_t HR2510_PB112_LINE_1KHZ   = 0x02; // X_.X
const uint8_t HR2510_PB112_LINE_100HZ  = 0x04; // XX._
const uint8_t HR2510_PB112_LINE_ALL    = 0x07; // __._

/* Icon Memory Map */
const uint8_t HR2510_PB112_MOD[2] = {0xFB, 0x10}; // Mod
const uint8_t HR2510_PB112_SWR[2] = {0xFB, 0x01}; // SWR
const uint8_t HR2510_PB112_SET[2] = {0xFD, 0x08}; // ^
const uint8_t HR2510_PB112_RF[2]  = {0xFD, 0x01};  // RF

/* *********************** COMMAND  MODE ********************* */
const uint8_t _Mode = 0x40;     // LCD speed. Se uPD7225 datasheet for more infp
const uint8_t _Sync = 0x31;     // Synchronized transfer
const uint8_t _USync = 0x30;    // Unsynchronized transfer
const uint8_t _FBlink = 0x1b;   // Blink fast (~2 Hz)
const uint8_t _SBlink = 0x1a;   // Blink slow (~1 Hz)
const uint8_t _NoBlink = 0x18;  // No blink
const uint8_t _DispOn = 0x11;   // Display on
const uint8_t _DispOff = 0x10;  // Display off
const uint8_t _Decode = 0x15;   // Turn on segment decoding
const uint8_t _NoDecode = 0x14; // Turn off segment decoding
const uint8_t _LoadPtr = 0xE0;  // Load data pointer
const uint8_t _SetDp = 0xb8;    // Set decimal point (Dp)
const uint8_t _ClearDp = 0x9f;  // Clear decimal point (Dp)
const uint8_t _ClearDsp = 0x20; // Clear display memory

const bool fast = true;
const bool slow = false;
const uint8_t busyDelay = 1;


class HR2510_PB112
{
  public:
    /* Public methods */
    HR2510_PB112(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t BUSY, uint8_t CS);
    HR2510_PB112(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS);

    void master(); // send data to the PB112 display.
    void slave();  // passively receive data for PB112 display.
    void command(uint8_t);
    void customChar(uint8_t);
    void test(uint8_t);
    void clear(uint8_t start = 0, uint8_t stop = 11);
    void blink(bool speed = slow);
    void noBlink();
    void display();
    void noDisplay();
	void dumpMemoryMap();

    
    bool setFrequency(String);
	char* getFrequency();
    bool setChannel(String);
	char* getChannel();
    bool setBand(uint8_t);
	char getBand();
	uint8_t getCursor();
	bool setCursor(uint8_t);
	void writeDisplay(uint8_t,uint8_t);
	uint8_t lookupSegmentHex(uint8_t);

    void print(char*);
    void print(char);
    void print(uint32_t, uint8_t base = DEC);
    void print(int32_t, uint8_t base = DEC);
    void print(uint16_t, uint8_t base = DEC);
    void print(int16_t, uint8_t base = DEC);
    void print(double, uint8_t decimals = 2);


  private:
    /* Private methods */
    void printNumber(uint32_t, uint8_t base = DEC, bool table = false);
    void printFloat(double, uint8_t = 2);
    void write(uint8_t);
    void wait();

    /* Private variables */
    uint8_t _cursorPos = 0;
	uint8_t _lcdMemoryMap[0xFF] = { 0x00 }; // Init all locations to 0x00
	uint8_t _slave = 0x00;
	

    /* Digital pins */
    uint8_t _SCK = 0;
    uint8_t _SI = 0;
    uint8_t _CD = 0;
    uint8_t _RESET = 0;
    uint8_t _BUSY = 255;
    uint8_t _CS = 0;
};

#endif