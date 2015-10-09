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

#include "Config_RX.h"

//Library for VESC UART
#include "VescUart.h"
#include "datatypes.h"

RF24 radio(CEPIN,CSPIN);

//Define Remote Package

remotePackage remPack;
bool recOK = true;

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
	  Serial.println("Nrf24L01 Receiver Starting");
	  radio.begin();
	  radio.enableAckPayload();
	  radio.enableDynamicPayloads();
	  radio.openReadingPipe(1,pipe);

	  radio.startListening();
	#ifdef DEBUG
	
	radio.printDetails();
	
	#endif

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
		//SerialPrint(VescMeasuredValues);
	}
	else
	{
		Serial.println("Failed to get data!");
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
	#ifdef DEBUG

	//Read the remote controls and control Vesc
	//Read the x-joystick and controls motor current and break

	//transform values read to int

	persXJoy = map(remPack.valXJoy, 0, 1023, -100, 100);

	if (persXJoy > DEADBAND_POS )
	{
		Serial.println("Throttle");
	}
	else if (persXJoy < DEADBAND_NEG) 
	{
		Serial.println("Break");
	}

					
	if (recOK == true)
	{
		Serial.println("Received successfully!");
		Serial.println("recieved package: ");
		Serial.print("valXJoy = "); Serial.print(remPack.valXJoy); Serial.print(" valYJoy = "); Serial.println(remPack.valYJoy);
		Serial.print("LowerButton = ");Serial.print(remPack.valLowerButton); Serial.print(" UpperButton = "); Serial.println(remPack.valUpperButton);
		recOK = false;
	}
	#endif
	
	
	
}
