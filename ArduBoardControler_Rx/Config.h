// Config.h

//Config RX

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Definition of Pipe

const uint64_t pipe = 0xE8E8F0F0E1LL;

#define debug

//Define remote Package

typedef struct {
	
	int		valXJoy;
	int		valYJoy;
	boolean	valUpperButton;
	boolean	valLowerButton;
	
} remotePackage;
#endif



