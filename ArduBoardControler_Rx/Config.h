// Config.h

/*
Copyright 2015 - 2017 Andreas Chaitidis Andreas.Chaitidis@gmail.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

*/

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

//**for Mega2560
#ifdef __AVR_ATmega2560__

//Pin definition

#define CEPIN	9
#define CSPIN	10

//Definition of Serial ports
#define SERIALIO Serial1
#define DEBUGSERIAL Serial

#define DEBUG

#endif

//**for Arduino Nano

#ifdef ARDUINO_AVR_NANO

//Pin definition

#define CEPIN	9
#define CSPIN	10

//Definition of Serial ports
#define SERIALIO Serial
#define DEBUGSERIAL Serial
#endif

//Settings for Nunchuk control

#define SET_NUNCHUK_CONTROL


//Setting for current control

//#define SET_CURRENT_CONTROL

#define DEADBAND_POS 1
#define DEADBAND_NEG -1

//Settings for remote

#define TIMEOUTMAX	500

////Define remote Package
//
//struct remotePackage {
//	
//	int		valXJoy;
//	int		valYJoy;
//	boolean	valUpperButton;
//	boolean	valLowerButton;
//	
//} ; >> moved to datatypes.h in lib VescUartControl
#endif



