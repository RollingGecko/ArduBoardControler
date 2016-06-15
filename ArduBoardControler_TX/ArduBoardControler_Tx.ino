
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
#include "U8glib.h"
#include "datatypes.h"
//#include "local_datatypes.h" 
//#include "VescUart.h" 
#include"VescIo.h"
#include"VESC.h"
#include <Adafruit_NeoPixel.h>
#include "WS2812Color.h"
#include "printf.h"
#include "LiPoCheck.h"


//
// Hardware configuration
//
//Set up LED WS2812

Adafruit_NeoPixel Led = Adafruit_NeoPixel(NUM2812, LED_PIN, NEO_GRB + NEO_KHZ800);

//Setup communication interface

#ifdef VESC_OVER_UART

VescUartIo UartIo(&SERIALIO);	//Dependency injection for UART
Vesc	vesc(&UartIo); //Dependency injection for communication interface

#endif // VESC_OVER_UART

//// Set up nRF24L01 radio

//RF24 radio(CEPIN,CSPIN);

//Setup OLED display

//Please check the usage of the right constructor for your OLED driver in the u8glib. 
//Here we use a SSD1306 with HW_SPI
U8GLIB_SSD1306_128X64 u8g(OLED_CSPIN, OLED_CEPIN, OLED_MISO, OLED_MOSI, OLED_SCK);
//U8GLIB_SSD1306_128X64 u8g(SCL, SDA, CS, DC, RES);

struct Vesc::remotePackage remPack;

//struct remotePackage remPack;
//struct bldcMeasure vesc.telemetryDataFromVesc.;
struct calcValues calculatedValues;

// Declaration of global variables and const

long failedCounter = 0;
boolean sendOK = false;
boolean recOK = false;
COMM_PACKET_ID lastReceivedMessageType = COMM_NON;

const float ratioRpmSpeed = ((DIA_WHEEL * 3.14156) / RATIO_GEAR) * 60 / 1000000; //RPM to Km/h
const float	rationRotDist = ((DIA_WHEEL * 3.14156) / RATIO_GEAR) / 1000000; //RPM to travelled km

//timerVariables

double timeLastTelemetryRequest = 0;

//function declaration

void inline Vibrator();
void inline Vibrator(int numberCycles);
void BatCapIndLED(int led, float voltage, int numberCells);
void DrawScreenMain(void);

void setup()
{	
	//Led class is started and brightness is defined
	Led.begin();
	Led.setBrightness(BRIGHTNESS);

#ifdef DEBUG
	DEBUGSERIAL.begin(9600);
#endif
	//Setup of communication interface
#ifdef VESC_OVER_UART
	SERIALIO.begin(SERIALIO_BAUD); 
#endif // VESC_OVER_UART
	UartIo.setMaxMessageSize(MAXMESSAGESIZE_IO); //limited by max Payload size of nRF 

	//Some light play at startup

	Led.setPixelColor(0, COLOR_BLUE);
	Led.show();
	delay(300);
	Led.setPixelColor(1, COLOR_GREEN);
	Led.show();
	delay(300);
	Led.setPixelColor(2, COLOR_RED);
	Led.show();
	delay(300);
	Led.setPixelColor(3, COLOR_ORANGE);
	Led.show();
	delay(300);
	for (size_t i = 0; i < NUM2812; i++)
	{
		Led.setPixelColor(i, COLOR_OFF);
	}
	Led.show();

	////Initialization of Radio
	//
	//radio.begin();
	//radio.enableAckPayload();
	//radio.enableDynamicPayloads();
	//radio.openWritingPipe(pipe);

	//Initialization of buttons

	pinMode(UPPER_BUTTON, INPUT_PULLUP);
	pinMode(LOWER_BUTTON, INPUT_PULLUP);

	//Initialistation of Vibrator

	pinMode(VIBRATOR_PIN, OUTPUT);

	// number of cells for tx is calculates once

	calculatedValues.numberCellsTx = CountCells((float) (analogRead(VOLTAGE_PIN))/ VOLTAGE_DIVISOR_TX);

	//Short feedback

	Vibrator(3);

}//Setup End

void loop()
{
	//calculates the number of cells of the board if not allready happen
	//needs to be in the loop because it is not clear when board is powered uo
	if (calculatedValues.numberCellsVesc == 0)
	{
		calculatedValues.numberCellsVesc = CountCells(vesc.telemetryDataFromVesc.inpVoltage);
		}
	
	if (millis() - timeLastTelemetryRequest >= INTERVAL_GETTELEMETRY)
	{
		timeLastTelemetryRequest = millis();
		if (vesc.statusRequestGetTelemetryValues() )
		{
			failedCounter++;
			//ToDo: Add event seperated error logging
		}
		vesc.sendRequestGetTelemetryValues();
	}

	lastReceivedMessageType = vesc.ReceiveHandleMessage();

	//Calculation from telemtry data


	//ToDo: Mittelwertbildung
	if (lastReceivedMessageType == COMM_GET_VALUES)
	{
		calculatedValues.VescPersCap = CapCheckPerc(vesc.telemetryDataFromVesc.inpVoltage, calculatedValues.numberCellsVesc);
		calculatedValues.speed = vesc.telemetryDataFromVesc.rpm * ratioRpmSpeed;
		calculatedValues.distanceTravel = vesc.telemetryDataFromVesc.tachometer * rationRotDist;
		BatCapIndLED(LED_VOLTAGE, vesc.telemetryDataFromVesc.inpVoltage, calculatedValues.numberCellsVesc);
	}
	calculatedValues.TxPersCap = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	BatCapIndLED(LED_TX, ((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	

//read iputs
  remPack.valXJoy = map(analogRead(JOY_X), 0, 1023, 0, 255);
  remPack.valYJoy = map(analogRead(JOY_Y), 0, 1023, 0, 255);
	//remPack.valXJoy = 128;
 // remPack.valYJoy = 128;
  remPack.valLowerButton = !digitalRead(LOWER_BUTTON);
  remPack.valUpperButton = !digitalRead(UPPER_BUTTON);
  //send data to RX
 // sendOK = radio.write( &remPack, sizeof(remPack) );
 sendOK = vesc.SetNunchukValues(remPack);

  //read Acknowledegement message from RX

  //while (radio.isAckPayloadAvailable())
  //{
	 // radio.read(&vesc.telemetryDataFromVesc., sizeof(vesc.telemetryDataFromVesc.));
	 // recOK = true;

  //}

  //#ifdef DEBUG
if (sendOK)
  {
	sendOK = false;
	Led.setPixelColor(LED_TRANS, COLOR_GREEN);
	Led.show();

  }
  else
  {
  failedCounter++;
  Led.setPixelColor(LED_TRANS, COLOR_RED);
  Led.show();
  }

#ifdef DEBUG
if (recOK)
{
	Serial.println("Received values from Vesc:");
	vesc.SerialPrint(DEBUGSERIAL);
}
#endif //DEBUG

// picture loop for oled display

	u8g.firstPage();
	do {
		DrawScreenMain();
	} while (u8g.nextPage());

	//// rebuild the picture after some delay
	//delay(200);

} //End main loop

void inline Vibrator() {

	analogWrite(VIBRATOR_PIN, STRENGTH);
	delay(PULS);
	analogWrite(VIBRATOR_PIN, 0);
}

void inline Vibrator(int numberCycles) {

	for (size_t i = 0; i < numberCycles; i++)
	{
		Vibrator();
		delay(200);
	}

}

void BatCapIndLED(int led, float voltage, int numberCells) {
	//	float capTx = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	int cap = CapCheckPerc(voltage, numberCells);
#ifdef DEBUG
	DEBUGSERIAL.print("voltag: "); DEBUGSERIAL.println(voltage);
	DEBUGSERIAL.print("numberCells: "); DEBUGSERIAL.println(numberCells);
	DEBUGSERIAL.print("Capacity: "); DEBUGSERIAL.println(cap);
#endif // DEBUG

	if (cap > 80)
	{
		Led.setPixelColor(led, COLOR_GREEN);
		Led.show();
	}
	else if (cap <= 80 && cap > 60)
	{
		Led.setPixelColor(led, COLOR_YELLOWGREEN);
		Led.show();
	}
	else if (cap <= 60 && cap > 30)
	{
		Led.setPixelColor(led, COLOR_ORANGE);
		Led.show();
	}
	else if (cap <= 30)
	{
		Led.setPixelColor(led, COLOR_RED);
		Led.show();
		//For Test Purpose:
		uint8_t offset = 0;
	}
}

void DrawScreenMain(void) {
	// graphic commands to redraw the complete screen should be placed here 
	u8g.setFontPosTop();
	u8g.setFont(u8g_font_courB08);
	u8g.setPrintPos(0, 0);
	u8g.print(calculatedValues.TxPersCap);
	u8g.drawStr(25, 0, "%");
	u8g.drawStr(50, 0, "con");
	u8g.setPrintPos(90, 0);
	u8g.print(failedCounter);
	u8g.drawStr(110, 0, "Err");
	u8g.drawHLine(0, 9, 128);
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 11);
	u8g.print(calculatedValues.speed, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(48, 11, "km/h");
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 30);
	u8g.print(calculatedValues.distanceTravel, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(48, 30, "km");
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.setPrintPos(78, 11);
	u8g.print(vesc.telemetryDataFromVesc.avgMotorCurrent, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(120, 11, "A");
	u8g.drawHLine(0, 53, 128);
	u8g.setFontPosBottom();
	u8g.setFont(u8g_font_courB08);
	u8g.setPrintPos(0, 64);
	u8g.print(vesc.telemetryDataFromVesc.ampHours, 0);
	u8g.drawStr(28, 64, "mAh");
	u8g.setPrintPos(50, 64);
	u8g.print(vesc.telemetryDataFromVesc.inpVoltage, 1);
	u8g.drawStr(80, 64, "V");
	u8g.setPrintPos(103, 64);
	u8g.print(calculatedValues.VescPersCap);
	u8g.drawStr(120, 64, "%");
}