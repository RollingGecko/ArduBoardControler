/*
 Name:		VescUartSample.ino
 Created:	9/26/2015 10:12:38 PM
 Author:	AC
*/

// the setup function runs once when you press reset or power the board

//Include libraries copied from VESC
 #include "crc.h"
#include "buffer.h"
#include "datatypes.h"


bool UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPa);
int PackSendPayload(uint8_t* payload, int lenPay);
int ReceiveUartMessage(uint8_t* payloadReceived); 
void ProcessReadPacket(uint8_t* message, bldcMeasure& values, int len);
void SerialPrint(const bldcMeasure& values);
void SerialPrint(uint8_t* data, int len);
void VescUartGetValue(bldcMeasure& values);



uint8_t message[200];
uint8_t messageSend[200];
bool messageReceived = false;
bldcMeasure measuredValues;

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
	//int len=0;
	//len = ReceiveUartMessage(message);
	//if (len > 0)
	//{
	//	len = PackSendPayload(message, len);
	//	len = 0;
	//}
	VescUartGetValue(measuredValues);
	SerialPrint(measuredValues);
	delay(500);
}

int ReceiveUartMessage(uint8_t* payloadReceived) { 

	//Messages <= 255 start with 2. 2nd byte is length
	//Messages >255 start with 3. 2nd and 3rd byte is length combined with 1st >>8 and then &0xFF

	int counter = 0;
	int endMessage = 256;
	bool messageRead = false;
	uint8_t messageReceived[256];
	int lenPayload = 0;

	while (Serial1.available()) {

		messageReceived[counter++] = Serial1.read();
#ifdef DEBUG
	//	Serial.print("Read: "); Serial.println(messageReceived[counter - 1]);
	//	Serial.print("Counter: "); Serial.println(counter);
#endif // DEBUG

		
		if (counter == 2) {//case if state of 'counter' with last read 1
			
			switch (messageReceived[0])
			{
			case 2: 
				endMessage = messageReceived[1] + 5; //Payload size + 2 for sice + 3 for SRC and End.
				lenPayload = messageReceived[1];
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
	UnpackPayload(messageReceived, endMessage, payloadReceived, messageReceived[1]);
#ifdef DEBUG
		//Serial.println("Read sucessfull!");
		//SerialPrint(messageReceived, endMessage - 1);
#endif // DEBUG

		return lenPayload; //Message was read

	} 
	else {
		return 0; //No Message Read
	}
}

bool UnpackPayload(uint8_t* message,int lenMes, uint8_t* payload, int lenPay) {
	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;
	//Rebuild src:
	crcMessage = message[lenMes - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[lenMes - 2];
	Serial.print("SRC received: "); Serial.println(crcMessage);
	//Extract payload:
	memcpy(payload, &message[2], message[1]);

	crcPayload = crc16(payload, message[1]);
	Serial.print("SRC calc: "); Serial.println(crcPayload);
	if (crcPayload == crcMessage)
	{
	#ifdef DEBUG
		Serial.print("Received: "); SerialPrint(message, lenMes); Serial.println();
		Serial.print("Payload :      "); SerialPrint(payload, message[1] - 1); Serial.println();
	#endif // DEBUG
		
		return true;
	}
	else
	{
		return false;
	}
}

int PackSendPayload(uint8_t* payload, int lenPay) {
	uint16_t crcPayload = crc16(payload, lenPay);
	int count = 0;
	uint8_t messageSend[256];

	if (lenPay <=256)
	{
		messageSend[count++] = 2;
		messageSend[count++] = lenPay;
	}
	else
	{
		messageSend[count++] = 3;
		messageSend[count++] = (uint8_t)(lenPay >> 8);
		messageSend[count++] = (uint8_t)(lenPay& 0xFF);
	}
	memcpy(&messageSend[count], payload, lenPay);

	count += lenPay;
	messageSend[count++] = (uint8_t)(crcPayload >> 8);
	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
	messageSend[count++] = 3;

	//Serial.print("Pack for sending:"); SerialPrint(messageSend, count); Serial.println();

	//Sending package
	Serial1.write(messageSend, count);
	Serial.println("Tx send.");

	return count;

}

void SerialPrint(uint8_t* data, int len) {

//	Serial.print("Data to display: "); Serial.println(sizeof(data));

	for (int i = 0; i <= len; i++)
	{
		Serial.print(data[i]); 
		Serial.print(" ");
	}
	Serial.println("");
}

void SerialPrint(const bldcMeasure& values) {
	Serial.print("avgMotorCurrent: "); Serial.println(values.avgMotorCurrent);
	Serial.print("avgInputCurrent: "); Serial.println(values.avgInputCurrent);
	Serial.print("dutyCycleNow: "); Serial.println(values.dutyCycleNow);
	Serial.print("rpm: "); Serial.println(values.rpm);
	Serial.print("inputVoltage: "); Serial.println(values.inpVoltage);
	Serial.print("ampHours: "); Serial.println(values.ampHours);
	Serial.print("ampHoursCharges: "); Serial.println(values.ampHoursCharged);
	Serial.print("tachometer: "); Serial.println(values.tachometer);
	Serial.print("tachometerAbs: "); Serial.println(values.tachometerAbs);
}

void ProcessReadPacket(uint8_t* message,bldcMeasure& values, int len) {
	COMM_PACKET_ID packetId;
	int32_t ind = 0;

	packetId = (COMM_PACKET_ID) message[0];
	message++;//Eliminates the message id
	len--; 

	switch (packetId)
	{
	case COMM_GET_VALUES:
		ind = 14; //Skipped the first 14 bit.
		values.avgMotorCurrent = buffer_get_float32(message, 100.0, &ind);
		values.avgInputCurrent = buffer_get_float32(message, 100.0, &ind);
		values.dutyCycleNow = buffer_get_float16(message, 1000.0, &ind);
		values.rpm = buffer_get_int32(message, &ind);
		values.inpVoltage = buffer_get_float16(message, 10.0, &ind);
		values.ampHours = buffer_get_float32(message, 10000.0, &ind);
		values.ampHoursCharged = buffer_get_float32(message,10000.0, &ind);
		ind += 9; //Skip 9 bit
		values.tachometer = buffer_get_int32(message, &ind);
		values.tachometerAbs = buffer_get_int32(message, &ind);
		break;

	default:
		break;
	}

}

void VescUartGetValue(bldcMeasure& values) {
	uint8_t command[1] = { COMM_GET_VALUES };
	uint8_t payload[256];
	PackSendPayload(command, 1);
	int lenPayload = ReceiveUartMessage(payload);
	ProcessReadPacket(payload, values, lenPayload);
}



