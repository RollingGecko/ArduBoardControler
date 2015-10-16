
/*
 * ArduBoardControler.ino
 *
 * Created: 9/5/2015 7:12:40 PM
 * Author: AC
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
	radio.begin();
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.openWritingPipe(pipe);
	remPack.valLowerButton = 0;
	remPack.valUpperButton = 1;
	Serial.println(calculatedValues.numberCells);

}

void loop()
{
	if (calculatedValues.numberCells == 0)
	{
		calculatedValues.numberCells = CountCells(VescMeasuredValues.inpVoltage);
	}
//read iputs
  remPack.valXJoy = analogRead(JOY_X);
  remPack.valYJoy = analogRead(JOY_Y);

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
