// Config.h

// Config TX

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Definition of Pipe

const uint64_t pipe = 0xE8E8F0F0E1LL;

//Pin definition

//**for Mega2560
#ifdef __AVR_ATmega2560__ 

#define JOY_X				A0
#define JOY_Y				A1
#define UPPER_BUTTON		6	
#define LOWER_BUTTON		5

#endif

//**for Arduino ATmega328p (Nano)

#ifdef ARDUINO_AVR_NANO
#define JOY_X				A0
#define JOY_Y				A1
#define UPPER_BUTTON		6	
#define LOWER_BUTTON		5
#define DEBUGSERIAL Serial
#define SERIALIO Serial
#endif

#define DEBUG

//Define voltage controll setting

#define LIPO

#ifdef LIPO

#define MINVOLRANGE 3.3
#define MAXVOLRANGE 4.2
#define MAXNUMBERCELLS	10

#endif



//Define remote Package

struct remotePackage {
	
	int		valXJoy;
	int		valYJoy;
	boolean	valUpperButton;
	boolean	valLowerButton;
	
} ;

struct calcValues {
	int numberCells = 0;
};

#endif



