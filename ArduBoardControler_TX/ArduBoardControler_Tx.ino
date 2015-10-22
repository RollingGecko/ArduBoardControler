
/*
 * ArduBoardControler.ino
 *
 * Created: 9/5/2015 7:12:40 PM
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

//Using the nRF34 library from https://github.com/TMRh20/RF24
#include "Config.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "datatypes.h"
#ifdef DEBUG
#include "VescUart.h" //SerialPrint for received Data Package
#endif
#include "printf.h"
#include "VoltageCheck.h"


//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

remotePackage remPack;
struct bldcMeasure VescMeasuredValues;
struct calcValues calculatedValues;


long failedCounter = 0;
boolean sendOK = false;
boolean recOK = false;


void setup()
{
	Serial.begin(9600);
	Serial.println("Tx Started");
	//Initialization of Radio
	
	radio.begin();
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.openWritingPipe(pipe);

	//Initialization of buttons

	pinMode(UPPER_BUTTON, INPUT_PULLUP);
	//digitalWrite(UPPER_BUTTON, HIGH); // turn on pullup resistors

	pinMode(LOWER_BUTTON, INPUT_PULLUP);
	//digitalWrite(LOWER_BUTTON, HIGH); // turn on pullup resistors

	//remPack.valLowerButton = 0;
	//remPack.valUpperButton = 1;
	//Some debug stuff
	//Serial.println(calculatedValues.numberCells);

}

void loop()
{
	if (calculatedValues.numberCells == 0)
	{
		calculatedValues.numberCells = CountCells(VescMeasuredValues.inpVoltage);
	}
//read iputs
  remPack.valXJoy = map(analogRead(JOY_X), 0, 1023, 0, 255);
  remPack.valYJoy = map(analogRead(JOY_Y), 0, 1023, 0, 255);
  remPack.valLowerButton = !digitalRead(LOWER_BUTTON);
  remPack.valUpperButton = !digitalRead(UPPER_BUTTON);
  //send data via radio to RX
  sendOK = radio.write( &remPack, sizeof(remPack) );

  //read Acknowledegement message from RX

  while (radio.isAckPayloadAvailable())
  {
	  radio.read(&VescMeasuredValues, sizeof(VescMeasuredValues));
	  recOK = true;

  }
  #ifdef DEBUG
if (sendOK)
  {
	Serial.print("X= "); Serial.print(remPack.valXJoy);Serial.print(" Y= "); Serial.println(remPack.valYJoy);
	Serial.println("Send successfully!");
	Serial.print("Failed= ");Serial.println(failedCounter);
	sendOK = false;
  }
  else
  {
  //Serial.println("Send failed!");
  failedCounter++;
  }

if (recOK)
{
	Serial.println("Received values from Vesc:");
	SerialPrint(VescMeasuredValues);
}
#endif
Serial.println(calculatedValues.numberCells);
	delay(20);
}
