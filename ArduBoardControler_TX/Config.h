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

//Decide here if you want to use an NRF-Module or direct serial communication to connect to VESC

#define VESC_OVER_UART
//#define VESC_OVER_NRF

#ifdef VESC_OVER_NRF
// Definition of Pipe

const uint64_t pipe = 0xE8E8F0F0E1LL;

#endif // VESC_OVER_NRF

//Pin definition

//**for Mega2560
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


#ifdef VESC_OVER_NRF
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
#endif // VESC_OVER_NRF
//other Pins
#define VOLTAGE_PIN			A2
#define LED_PIN				4
#define VIBRATOR_PIN		A3
//Definition of Serial
#define DEBUGSERIAL			Serial
#ifdef VESC_OVER_UART
#define SERIALIO			Serial
#define SERIALIO_BAUD		115200
#endif // VESC_OVER_UART
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
#endif

//#define DEBUG

//Communication settings

#define INTERVAL_GETTELEMETRY	500 //Interval for telemetry data in ms
#define MAXMESSAGESIZE_IO	100
//Define voltage controll setting

#define LIPO

#ifdef LIPO

#define MINVOLRANGE 3.3
#define MAXVOLRANGE 4.25
#define MAXNUMBERCELLS	10

#endif

//TX Voltage measurement
#define VOLTAGE_DIVISOR_TX	102.5

//WS2812 settings

#define NUM2812		4
#define BRIGHTNESS	20
#define LED_TX		0	//TX-Voltage
#define LED_TRANS	1	//Transmission
#define LED_VOLTAGE	2

//setting Vibrator

#define STRENGTH	255	//0-255
#define PULS		150 //ms

//Calculation Parameter

#define DIA_WHEEL	76 //mm
#define RATIO_GEAR	3.2

struct calcValues {
	int		numberCellsVesc = 0;
	int		numberCellsTx = 0;
	int		VescPersCap = 0;
	int		TxPersCap = 0;
	float	speed = 0;
	float	distanceTravel = 0;
};

#endif



