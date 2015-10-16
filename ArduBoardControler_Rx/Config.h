// Config.h

//Config RX

#ifndef _CONFIG_h
#define _CONFIG_h
#endif

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Definition of Pipe
 
const uint64_t pipe = 0xE8E8F0F0E1LL;

//**for Mega2560
#ifdef __AVR_ATmega2560__

//Pin definition

#define CEPIN	9
#define CSPIN	10

//Definition of Serial ports
#define SERIALIO Serial1
#define DEBUGSERIAL Serial

//**for Arduino Micro

#ifdef ARDUINO_AVR_MICRO

//Pin definition

#define CEPIN	9
#define CSPIN	10

//Definition of Serial ports
#define SERIALIO Serial
#endif

#define DEBUG

//Setting for current control

#define DEADBAND_POS 1
#define DEADBAND_NEG -1

//Settings for remote

#define TIMEOUTMAX	500

//Define remote Package

struct remotePackage {
	
	int		valXJoy;
	int		valYJoy;
	boolean	valUpperButton;
	boolean	valLowerButton;
	
} ;
#endif



