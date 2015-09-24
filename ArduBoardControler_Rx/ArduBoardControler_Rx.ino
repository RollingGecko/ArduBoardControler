/*
 * ArduBoardControler_Rx.ino
 *
 * Created: 9/5/2015 10:10:26 PM
 * Author: AC
 */ 

#include <SPI.h>


//Using the nRF34 library from https://github.com/TMRh20/RF24
#include "nRF24L01.h"
#include "RF24.h"

#include "printf.h"
#include "Config.h"


RF24 radio(9,10);

//Define Remote Package

remotePackage remPack;
bool recOK = true;

void setup()
{
	
	  Serial.begin(9600);
	  delay(1000);
	  Serial.println("Nrf24L01 Receiver Starting");
	  radio.begin();
	  radio.openReadingPipe(1,pipe);
	  radio.startListening();
	#ifdef debug
	
	radio.printDetails();
	
	#endif
	
		remPack.valXJoy			= 512; //middle Position
		remPack.valYJoy			= 512;
		remPack.valLowerButton	= 0;
		remPack.valLowerButton	= 0;
	
}

void loop()
{
		
	while (radio.available())
	{
	//	Serial.println("Available");
		radio.read(&remPack, sizeof(remPack));
		recOK = true;
	}
		#ifdef debug
					
		if (recOK == true)
		{
			Serial.println("Received successfully!");
			Serial.println("recieved package: ");
			Serial.print("valXJoy = "); Serial.print(remPack.valXJoy); Serial.print(" valYJoy = "); Serial.println(remPack.valYJoy);
			Serial.print("LowerButton = ");Serial.print(remPack.valLowerButton); Serial.print(" UpperButton = "); Serial.println(remPack.valUpperButton);
			//Serial.print("Failed= ");Serial.println(failedCounter);
			recOK = false;
			
		}
		#endif
		
	//if ( radio.available() )
	//{
		//// Serial.println("Radio available");
		//// Read the data payload until we've received everything
		//bool done = false;
		//while (!done)
		//{
			//// Fetch the data payload
			//done = radio.read( &remPack, sizeof(remPack) );
			//Serial.print("X = ");
			//Serial.print(remPack.valXJoy);
			//Serial.print(" Y = ");
			//Serial.println(remPack.valYJoy);
		//}
	//}
	//else
	//{
		//Serial.println("No radio available");
		//
		//
	//}
}
