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

// Config TX

#ifndef _CONFIG_h
#define _CONFIG_h



#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//Definition of Hardware used

#define OLED_USED			//define out if not used
#define STATUS_LED_USED		//define out if not used

//General settings:

//#define DEBUG //use if debug information over serial required

// Definition of Pipe for nRF24

const uint64_t pipe = 0xE8E8F0F0E1LL;

//Pin definition

//**for Mega2560 (old status!!)
#ifdef __AVR_ATmega2560__ 

#define JOY_X				A0
#define JOY_Y				A1
#define UPPER_BUTTON		6	
#define LOWER_BUTTON		5
#define CEPIN				9
#define CSPIN				10
#endif

//**for Arduino ATmega328p (Nano)

#ifdef ARDUINO_AVR_NANO

//SPI definition (for better readability)

#define MOSI				11
#define MISO				12
#define SCK					13
//Joystick and bottons
#define JOY_X				A0
#define JOY_Y				A1
#define UPPER_BUTTON		2	
#define LOWER_BUTTON		3
//nRF24: 
/*nRF24	>					Ardunio nano
------------------------
VCC		>					3.3 V
GND		>					GND
MOSI	>					11
MISO	>					12
SCK		>					13
IRQ		>					not connected*/
#define CEPIN				9
#define CSPIN				10
//other Pins
#define VOLTAGE_PIN			A2
#define LED_PIN				4
#define VIBRATOR_PIN		A3
//Definition of Serial
#define DEBUGSERIAL			Serial
#define SERIALIO			Serial
#ifdef OLED_USED
//OLED
//for the OLED please check to correct pin setting by the used OLED driver in the u8glib
//Here I use a SSD1306 with SW_SPI/*
//VCC		>					5V
//GND		>					GND
#define OLED_MOSI			6	//(to DC)
#define OLED_SCK			5	//(to RES)
#define OLED_MISO			20	//not used
#define OLED_CEPIN			7	//(to SDA)
#define OLED_CSPIN			8	//(to SCL)
#endif //OLED_USED
#endif

//Define voltage control setting

#define MAXNUMBERCELLS	10
#define LIPO	0
#define LIION	1

#define TXCELLTYPE			LIPO
#define DRIVECELLTYPE		LIPO	//OR LIION

//TX Voltage measurement
#define VOLTAGE_DIVISOR_TX	102.5

//WS2812 settings

#define NUM2812		4
#define BRIGHTNESS	20
#define LED_TX		0	//TX-Voltage
#define LED_TRANS	1	//Transmission
#define LED_VOLTAGE	2
#define LED_FOUR	3

//setting Vibrator

#define STRENGTH	255	//0-255
#define PULS		150 //ms

//#define SEND_LR

#define JOYSTICKBUTTON_DEADBAND		256


//Calculation Parameter

#define DIA_WHEEL	83 //mm
#define RATIO_GEAR	3.2
#define PULSE_REV   42  //Number of poles*3
#define ERPM_REV    7   //Number of poles/2
#define CORRECT_FACTOR_DISTANCE 0.825 //required out of practical tests.
#define AVERAGE_CYCLE	100


struct calcValues {
	int		numberCellsVesc = 0;
	int		numberCellsTx = 0;
	int		VescPersCap = 0;
	int		TxPersCap = 0;
	float	speed = 0;
	float	maxSpeed = 0;
	float	distanceTravel = 0;
	float	rpmAverage = 0;
	float	currentAverage = 0;
	float	maxCurrent = 0;
};

#endif



