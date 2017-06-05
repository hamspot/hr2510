/******************************************************************
| Library for Uniden HR-2510, HR-2600, Lincoln, 2830 Display Unit | 
| https://github.com/hamspot/hr2510															 |
|																																 |
| Based on KTM-S1201 Arduino library															|				|
| Developed and maintanied by MCUdude														 |
| https://github.com/MCUdude/KTMS1201														 |						
|																																 |
| Based on Tronixstruff's work																		|
| tronixstuff.com/2013/03/11/arduino-and-ktm-s1201-lcd-modules		|
| Released to the public domain																	 |
******************************************************************/


#include "Arduino.h"
#include "HR2510_PB112.h"

/*******************************
******** PUBLIC METHODS ********
*******************************/

// Constructor with busy pin
HR2510_PB112::HR2510_PB112(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t BUSY, uint8_t CS)
{
	// Store parameters
	_SCK = NSCK;
	_SI = SI;
	_CD = CD;
	_RESET = RESET;
	_BUSY = BUSY;
	_CS = CS;
}


// Constructor without busy pin
HR2510_PB112::HR2510_PB112(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS)
{
	// Store parameters
	_SCK = NSCK;
	_SI = SI;
	_CD = CD;
	_RESET = RESET;
	_CS = CS;
}


void HR2510_PB112::master()
{
	_slave = false;

	//Set inputs and outputs
	pinMode(_SCK, OUTPUT);
	pinMode(_SI, OUTPUT);
	pinMode(_CD, OUTPUT);
	pinMode(_RESET, OUTPUT);
	if(_BUSY != 0xFF) pinMode(_BUSY, INPUT);
	pinMode(_CS, OUTPUT);
	
	digitalWrite(_SCK, HIGH);
	digitalWrite(_SI,	HIGH);
	digitalWrite(_CD, HIGH);	 // Put in command mode
	digitalWrite(_CS, HIGH);	 // Deselect KTM-S1201
	digitalWrite(_RESET, LOW); // Reset LCD
	delay(10);								 // Wait for the LCD to finish
	digitalWrite(_RESET, HIGH);

	// Setup command mode. See header for more info
	command(_Mode);
	command(_USync);
	command(_NoBlink); 
	command(_DispOn);
	command(_NoDecode); 
	command(_ClearDsp); 
}


void HR2510_PB112::slave()
{
	_slave = true;
	//Set inputs and outputs
	pinMode(_SCK, INPUT);
	pinMode(_SI, INPUT);
	pinMode(_CD, INPUT);
	pinMode(_RESET, INPUT);
	if(_BUSY != 0xFF) pinMode(_BUSY, INPUT);
	pinMode(_CS, INPUT);
	
	// @TODO Setup IRQ Handler Here to capture data
}

void HR2510_PB112::command(uint8_t cmd)
{
	if(!_slave){
		digitalWrite(_CD, HIGH);
		digitalWrite(_CS, LOW);
		wait(); // Wait for the LCD to finish
		write(cmd);
		wait(); // Wait for the LCD to finish
		digitalWrite(_CS, HIGH); // Deselect LCD to display data	
	}
}

uint8_t HR2510_PB112::lookupSegmentHex(uint8_t hex){
	// loop through all 16 valid characters
	for(uint8_t i=0; i < 16; i++){
		// find the index of the first match 
		if( sevenSegHex[i] == hex ){
			// return that index from the ascii character map
			return charArray[i];
		}
	}
	return 0xFF;
}

// Get the frequency from Memory Map
char* HR2510_PB112::getFrequency(){
		char *str = (char *) malloc(sizeof(char) * 6);

		// write to the display
		sprintf(str, "%c%c%c%c%c%c",
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_0]),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_1] &= ~0x08),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_2]),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_3]),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_4] &= ~0x08),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_FR_5])
		);		
		return str;
}

// Set the Frequency
bool HR2510_PB112::setFrequency(String str){
	uint8_t strLength = str.length();

	byte textString[strLength];

	str.toUpperCase();

	if(str.length() == 6){
		// Iterate through human readable array and stuff TextString with hex values. 
		for(uint8_t i = 0; i < strLength; i++)
		{ 
		for(uint8_t j = 0; j < numberOfCharacters; j++)
		{ 
			if(str[i] == charArray[j]) 
			{
				textString[i] = sevenSegHex[j];
				break;				
			}
		} 
		}
		
		// write to the display
		writeDisplay(HR2510_PB112_FR_0, textString[0]);
		writeDisplay(HR2510_PB112_FR_1, textString[1] |= 0x08); // @TODO OR decimal
		writeDisplay(HR2510_PB112_FR_2, textString[2]);
		writeDisplay(HR2510_PB112_FR_3, textString[3]);
		writeDisplay(HR2510_PB112_FR_4, textString[4] |= 0x08); // @TODO OR decimal
		writeDisplay(HR2510_PB112_FR_5, textString[5]);
/*		
		if(decimalPlace >= 0)
		textString[decimalPlace] ; // bitwise or in the dot
	
		for(uint8_t i = strLength; i > 0; i--) //display numbers (reverse order)
		write(textString[i-1]);
*/

		return true;
	}
	else{
		return false;
	}
}

// Get the Channel from Memory Map
char* HR2510_PB112::getChannel(){
		char *str = (char *) malloc(sizeof(char) * 2);

		// write to the display
		sprintf(str, "%c%c",
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_CH_0]),
			lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_CH_1])
		);		
		return str;
}

// Set the Channel
bool HR2510_PB112::setChannel(String str)
{
	uint8_t strLength = str.length();

	byte textString[strLength];

	str.toUpperCase();

	if(str.length() == 2){
		// Iterate through human readable array and stuff TextString with hex values. 
		for(uint8_t i = 0; i < strLength; i++)
		{ 
		for(uint8_t j = 0; j < numberOfCharacters; j++)
		{ 
			if(str[i] == charArray[j]) 
			{
				textString[i] = sevenSegHex[j]; 
				break;				
			}
		} 
		}
		
		writeDisplay(HR2510_PB112_CH_0, textString[0]);
		writeDisplay(HR2510_PB112_CH_1, textString[1]);

		return true;
	}
	else{
		return false;
	}
}

// Get the band from Memory Map
char HR2510_PB112::getBand(){
		return (char) lookupSegmentHex(_lcdMemoryMap[HR2510_PB112_BAND]);
}

// Set the Frequency
bool HR2510_PB112::setBand(uint8_t str)
{
	byte textString = 0x00; // no digit

	// Iterate through human readable array and stuff TextString with hex values. 
	for(uint8_t j = 0; j < numberOfCharacters; j++)
	{ 
		if(str == charArray[j]) 
		{
			textString = sevenSegHex[j]; 
			break;				
		}
	}

	writeDisplay(HR2510_PB112_BAND, textString);

	return true;
}

// Get the underscore cursor position
uint8_t HR2510_PB112::getCursor(){
	uint8_t pos = 0;
	/* Underlines */
	if(_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] != (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] &= ~HR2510_PB112_MAP_LINE_10KHZ[1] )){
		pos = pos |= HR2510_PB112_LINE_10KHZ;
	}
	if(_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ] != (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ] &= ~HR2510_PB112_MAP_LINE_1KHZ[1] )){
		pos = pos |= HR2510_PB112_LINE_1KHZ;
	}
	if(_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] != (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] &= ~HR2510_PB112_MAP_LINE_100HZ[1] )){
		pos = pos |= HR2510_PB112_LINE_100HZ;
	}

	Serial.println((_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ] ), HEX);

	return pos;
}

// Get the underscore cursor position
bool HR2510_PB112::setCursor(uint8_t pos){
	// if the cursor is not changing
	if(_cursorPos == pos){
		return true;
	}
	
	switch(pos){
		// 10 KHz is Underlined
		case HR2510_PB112_LINE_10KHZ:
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] |= HR2510_PB112_MAP_LINE_10KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  &= ~HR2510_PB112_MAP_LINE_1KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] &= ~HR2510_PB112_MAP_LINE_100HZ[1] );
			break;
		// 1 KHz is Underlined
		case HR2510_PB112_LINE_1KHZ:
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] &= ~HR2510_PB112_MAP_LINE_10KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  |= HR2510_PB112_MAP_LINE_1KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] &= ~HR2510_PB112_MAP_LINE_100HZ[1] );
			break;
		// 100 Hz is Underlined
		case HR2510_PB112_LINE_100HZ:
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] &= ~HR2510_PB112_MAP_LINE_10KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  &= ~HR2510_PB112_MAP_LINE_1KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] |= HR2510_PB112_MAP_LINE_100HZ[1] );
			break;
		// All 3 Digits Underlined
		case HR2510_PB112_LINE_ALL:
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] = _lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] |= HR2510_PB112_MAP_LINE_10KHZ[1];
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  = _lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  |= HR2510_PB112_MAP_LINE_1KHZ[1];
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] = _lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] |= HR2510_PB112_MAP_LINE_100HZ[1];
			break;
		// No line or unknown value 
		case HR2510_PB112_LINE_NONE:
		default:
			return false;
			/* keeping this, maybe. @TODO will delete after git import 
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ] &= ~HR2510_PB112_MAP_LINE_10KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]  &= ~HR2510_PB112_MAP_LINE_1KHZ[1] );
			_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] = (_lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ] &= ~HR2510_PB112_MAP_LINE_100HZ[1] );
			break;
			*/
	}
		
	// write it out
	writeDisplay(HR2510_PB112_MAP_LINE_10KHZ[0], _lcdMemoryMap[ HR2510_PB112_MAP_LINE_10KHZ[0] ]);
	writeDisplay(HR2510_PB112_MAP_LINE_1KHZ[0],  _lcdMemoryMap[ HR2510_PB112_MAP_LINE_1KHZ[0] ]);
	writeDisplay(HR2510_PB112_MAP_LINE_100HZ[0], _lcdMemoryMap[ HR2510_PB112_MAP_LINE_100HZ[0] ]);
		
	// set the cursor position
	_cursorPos = pos;
	
	return true;
}


void HR2510_PB112::writeDisplay(uint8_t address,uint8_t character)
{
	/* update the memory address with the new character */ 
	_lcdMemoryMap[ address ] = character;
	
	/* masters also write data to the SPI bus */
	if(!_slave){
		digitalWrite(_CD, HIGH); // enable command mode
		digitalWrite(_CS, LOW);  // enable chip select line
		wait();
		
		/* write the memory address to the SPI bus */
		write(address);
		
		digitalWrite(_CD, LOW); // enable data mode
		wait();
		
		/* write contents of memory address to the SPI bus */
		write(character);
		
		digitalWrite(_CS, HIGH); // disable chip select line 
	}
}


void HR2510_PB112::blink(bool speed)
{	
	// Blink fast
	if(speed == true)
		command(_FBlink);
	// Blink slow	
	else
		command(_SBlink);
}


void HR2510_PB112::noBlink()
{	
	command(_NoBlink);
}


void HR2510_PB112::display()
{	
	command(_DispOn);
}


void HR2510_PB112::noDisplay()
{	
	command(_DispOff);
}

void HR2510_PB112::write(uint8_t character)
{
	for(uint8_t i = 0; i < 8; i++) 
	{
		digitalWrite(_SI, !!(character & (1 << (7-i))));
		digitalWrite(_SCK,LOW);
		wait(); // Wait for the LCD to finish
		digitalWrite(_SCK, HIGH); 
	}
}

void HR2510_PB112::wait()
{
	if(_BUSY == 255)
		delay(busyDelay);
	else
		while(digitalRead(_BUSY) == 0);
}

void HR2510_PB112::dumpMemoryMap(){
	for( uint8_t i = 0; i < 255; i++){
		Serial.print(i, HEX);
		Serial.print(':');
		Serial.print(_lcdMemoryMap[i], HEX);
		Serial.println();
	}
}


