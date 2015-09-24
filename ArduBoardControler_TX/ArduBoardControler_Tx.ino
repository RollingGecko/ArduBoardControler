
/*
 * ArduBoardControler.ino
 *
 * Created: 9/5/2015 7:12:40 PM
 * Author: AC
 */ 

#include <SPI.h>

//Using the nRF34 library from https://github.com/TMRh20/RF24
#include "nRF24L01.h"
#include "RF24.h"

#include "printf.h"
#include "Config.h"


//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

remotePackage remPack;

long failedCounter = 0;
boolean sendOK = false;

void setup()
{
	Serial.begin(9600);
	Serial.println("Tx Started");
	radio.begin();
	radio.openWritingPipe(pipe);
	remPack.valLowerButton = 0;
	remPack.valUpperButton = 1;
	

	//Serial.begin(9600);
	//printf_begin();
	//
	//radio.begin();
	//radio.openWritingPipe(pipe);
	//
		//
	//
	////initialize remPack
	//
	//remPack.valXJoy			= 512; //middle Position
	//remPack.valYJoy			= 512;
	//remPack.valLowerButton	= 0;
	//remPack.valLowerButton	= 0;
	//
	//#ifdef debug
	//
	//// radio.printDetails();
	//// Serial.print("Package Size = "); Serial.println(sizeof(remPack));
	//
	//#endif
	
}

void loop()
{


  remPack.valXJoy = analogRead(JOY_X);
  remPack.valYJoy = analogRead(JOY_Y);
  
 
  sendOK = radio.write( &remPack, sizeof(remPack) );
  
  #ifdef debug
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
#endif
	
	delay(500);
}
