/*
 * ArduBoardControler_Rx.ino
 *
 * Created: 9/5/2015 10:10:26 PM
 * Author: AC
 */ 

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
#include <SPI.h>

#include "Config.h"

//Using the nRF34 library from https://github.com/TMRh20/RF24
#include <nRF24L01.h>
#include "RF24.h"
#include "printf.h"

//Library for VESC UART
#include "VescUart.h"
#include "datatypes.h"
#include "local_datatypes.h"

//Definition of radio class

RF24 radio(CEPIN,CSPIN);

//Define variables for remote
struct remotePackage remPack;
bool recOK = true;
uint32_t lastTimeReceived = 0;

struct bldcMeasure VescMeasuredValues;


int8_t persXJoy = 0;

void setup()
{
	
	#ifdef DEBUG
	  DEBUGSERIAL.begin(115200);
	#endif
	  //Initial for Radio
	  SERIALIO.begin(115200);
	  delay(1000);
#ifdef DEBUG 
	  DEBUGSERIAL.println("Nrf24L01 Receiver Starting");
#endif // DEBUG 

	  radio.begin();
	  radio.enableAckPayload();
	  radio.enableDynamicPayloads();
	  radio.openReadingPipe(1,pipe);

	  radio.startListening();
	 

	// For initial start 
	
		remPack.valXJoy			= 512; //middle Position
		remPack.valYJoy			= 512;
		remPack.valLowerButton	= 0;
		remPack.valLowerButton	= 0;
}

void loop()
{
	//Getting Values from Vesc over UART

	if (VescUartGetValue(VescMeasuredValues)) {
	//	SerialPrint(VescMeasuredValues);
	}
	else
	{
#ifdef DEBUG
		DEBUGSERIAL.println("Failed to get data from UART!");
#endif // DEBUG

	}

	//writing package to TX in AckPayload
	//data will be send with next acknowledgement to TX

	radio.writeAckPayload(pipe, &VescMeasuredValues, sizeof(VescMeasuredValues));
	
	//Get data from TX	
	while (radio.available())
	{
		radio.read(&remPack, sizeof(remPack));
		recOK = true;
	}
	

	uint32_t now = millis();

	if (recOK == true)
	{
		lastTimeReceived = millis();
		
#ifdef DEBUG
		DEBUGSERIAL.println("Received TX successfully!");
		
#endif
		recOK = false;
	}
	//Check if package were received within timeout
	else if ((now - lastTimeReceived) > TIMEOUTMAX)
		{
			remPack.valXJoy = 128; //middle Position
			remPack.valYJoy = 128;
			remPack.valUpperButton = false;
			remPack.valLowerButton = false;
#ifdef DEBUG
			DEBUGSERIAL.println("TX-signal lost!!");
#endif
		}

#ifdef DEBUG

	DEBUGSERIAL.println("Received package / Package in main loop: ");
	DEBUGSERIAL.print("valXJoy = "); DEBUGSERIAL.print(remPack.valXJoy); DEBUGSERIAL.print(" valYJoy = "); DEBUGSERIAL.println(remPack.valYJoy);
	DEBUGSERIAL.print("LowerButton = "); DEBUGSERIAL.print(remPack.valLowerButton); DEBUGSERIAL.print(" UpperButton = "); DEBUGSERIAL.println(remPack.valUpperButton);
	DEBUGSERIAL.print("Calcx: "); DEBUGSERIAL.println(((float)persXJoy / 100) * 40.0);

#endif

// Nunchuck mode

#ifdef SET_NUNCHUK_CONTROL
	VescUartSetNunchukValues(remPack);
#endif

#ifdef SET_CURRENT_CONTROL
	//ToDo: was only used for initial tests. Never realy tested while driving!! Needs to be checked if it should be used
	//Read the remote controls and control Vesc
//Read the x-joystick and controls motor current and break

//transform values read to int

	persXJoy = map(remPack.valXJoy, 0, 255, -100, 100);

	if (persXJoy > DEADBAND_POS)
	{
		VescUartSetCurrent(((float)persXJoy / 100) * 40.0);
#ifdef DEBUG
		DEBUGSERIAL.println("Throttle");
#endif // DEBUG

	}
	else if (persXJoy < DEADBAND_NEG)
	{
		VescUartSetCurrentBrake(((float)persXJoy / 100) * -3.0);
#ifdef DEBUG
		DEBUGSERIAL.println("Break");
#endif
	}

	else
	{
		VescUartSetCurrent(0.0);
		VescUartSetCurrentBrake(0.0);
	}
#endif // SET_CURRENT_CONTROL


					

	
	//delay(1000);
	
}
