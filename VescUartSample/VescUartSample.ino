/*
 Name:		VescUartSample.ino
 Created:	9/26/2015 10:12:38 PM
 Author:	AC
*/

// the setup function runs once when you press reset or power the board

//Include libraries copied from VESC
//#include "crc.h"
//#include "buffer.h"
#include "datatypes.h"

#define DEBUG

bool UnpackPayload(uint8_t* message, uint8_t* payload);
void PackPayload(uint8_t* messageSend, uint8_t* payload);
bool ReceiveUartMessage(uint8_t* payloadReceived); 
void SerialPrint(uint8_t* data, int len);

uint8_t message[200];
bool messageReceived = false;

void setup() {
	//SEtup debug port
	Serial.begin(115200);
	//Setup UART port
	Serial1.begin(115200);
#ifdef DEBUG
	Serial.println("Start reading port:");
#endif
}


	
// the loop function runs over and over again until power down or reset
void loop() {
	messageReceived = ReceiveUartMessage(message);

	}

bool ReceiveUartMessage(uint8_t* payloadReceived) { 

	//Messages <= 255 start with 2. 2nd byte is length
	//Messages >255 start with 3. 2nd and 3rd byte is length combined with 1st >>8 and then &0xFF

	int counter = 0;
	int endMessage = 256;
	bool messageRead = false;
	uint8_t messageReceived[256];

	while (Serial1.available()) {

		messageReceived[counter++] = Serial1.read();
#ifdef DEBUG
		Serial.print("Read: "); Serial.println(messageReceived[counter - 1]);
		Serial.print("Counter: "); Serial.println(counter);
#endif // DEBUG

		
		if (counter == 2) {//case if state of 'counter' with last read 1
			
			switch (messageReceived[0])
			{
			case 2: 
				endMessage = messageReceived[1] + 5; //Payload size + 2 for sice + 3 for SRC and End.
				break;
			case 3: 
				//ToDo: Add Message Handling > 255 (starting with 3)
				break;
			default:
				break;
			}
				
		}
		if (counter >= sizeof(messageReceived))
		{
			break;
		}

		if(counter == endMessage && messageReceived[endMessage-1] == 3) {//+1: Because of counter++ state of 'counter' with last read = "endMessage"
			messageReceived[endMessage] = 0; 
#ifdef DEBUG
			Serial.println("End of message reached!");
#endif			
			messageRead = true;
			break; //Exit if end of message is reached, even if there is still more data in buffer. 
		}
	}
	if (messageRead) {
	//	UnpackPayload(messageReceived, payloadReceived);
#ifdef DEBUG
		Serial.println("Read sucessfull!");
		Serial.print("Data to display: "); Serial.println(sizeof(messageReceived));
		SerialPrint(messageReceived, endMessage - 1);
#endif // DEBUG

		return true; //Message was read
	} 
	else {
		return false; //No Message Read
	}
}

bool UnpackPayload(uint8_t* message, uint8_t* payload) {
	
}

void SerialPrint(uint8_t* data, int len) {

	Serial.print("Data to display: "); Serial.println(sizeof(data));

	for (int i = 0; i <= len; i++)
	{
		Serial.print(data[i]); 
		Serial.print(" ");
	}
	Serial.println("");
}

