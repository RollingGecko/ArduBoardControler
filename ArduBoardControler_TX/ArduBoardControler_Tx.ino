
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
#ifdef DEBUG
#include "VescUart.h" //SerialPrint for received Data Package
#endif
#include <Adafruit_NeoPixel.h>
#include "WS2812Color.h"
#include "printf.h"
#include "LiPoCheck.h"


//
// Hardware configuratio
//
//Set up LED WS2812

Adafruit_NeoPixel Led = Adafruit_NeoPixel(NUM2812, LED_PIN, NEO_GRB + NEO_KHZ800);

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(CEPIN,CSPIN);

//Setup OLED display

//Please check the usage of the right constructor for your OLED driver in the u8glib. 
//Here we use a SSD1306 with HW_SPI
U8GLIB_SSD1306_128X64 u8g(OLED_CSPIN, OLED_CEPIN, OLED_MISO, OLED_MOSI, OLED_SCK);
//U8GLIB_SSD1306_128X64 u8g(SCL, SDA, CS, DC, RES);

remotePackage remPack;
struct bldcMeasure VescMeasuredValues;
struct calcValues calculatedValues;


long failedCounter = 0;
boolean sendOK = false;
boolean recOK = false;

void inline Vibrator();
void inline Vibrator(int numberCycles);

void BatCapIndLED(int led, float voltage, int numberCells) {
//	float capTx = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	int cap = CapCheckPerc(voltage, numberCells);
	DEBUGSERIAL.print("voltag: "); DEBUGSERIAL.println(voltage);
	DEBUGSERIAL.print("numberCells: "); DEBUGSERIAL.println(numberCells);
	DEBUGSERIAL.print("Capacity: "); DEBUGSERIAL.println(cap);
	if (cap > 80)
	{
		Led.setPixelColor(led, COLOR_GREEN);
		Led.show();
		Serial.println("1");
	}
	else if (cap <= 80 && cap > 60)
	{
		Led.setPixelColor(led, COLOR_YELLOWGREEN);
		Led.show();
		Serial.println("2");
	}
	else if (cap <= 60 && cap > 30)
	{
		Led.setPixelColor(led, COLOR_ORANGE);
		Led.show();
		Serial.println("3");
	}
	else if (cap <= 30)
	{
		Led.setPixelColor(led, COLOR_RED);
		Led.show();
		Serial.println("4");
//For Test Purpose:
uint8_t offset = 0;
	}
}

void DrawScreenMain(void) {
		// graphic commands to redraw the complete screen should be placed here 
		u8g.setFontPosTop();
		u8g.setFont(u8g_font_courB08);
		u8g.drawStr(0 + 0, 0, "80%");
		u8g.drawStr(50, 0, "con");
		u8g.drawStr(90, 0, "02Br");
		u8g.drawHLine(0, 9, 128);
		u8g.setFont(u8g_font_courB14r);
		u8g.setFontPosTop();
		u8g.drawStr(0, 11, "25.5");
		u8g.setFont(u8g_font_courB08);
		u8g.setFontPosTop();
		u8g.drawStr(48, 11, "km/h");
		u8g.setFont(u8g_font_courB14r);
		u8g.setFontPosTop();
		u8g.drawStr(0, 30, "09.5");

		u8g.setFont(u8g_font_courB08);
		u8g.setFontPosTop();
		u8g.drawStr(48, 30, "km");
		u8g.setFont(u8g_font_courB14r);
		u8g.setFontPosTop();
		u8g.setPrintPos(78, 11);
		u8g.print(VescMeasuredValues.avgMotorCurrent, 1);
	//	u8g.drawStr(78, 11, "43.1");
		u8g.setFont(u8g_font_courB08);
		u8g.setFontPosTop();
		u8g.drawStr(120, 11, "A");
		u8g.drawHLine(0, 53, 128);
		u8g.setFontPosBottom();
		u8g.setFont(u8g_font_courB08);
		u8g.setPrintPos(0, 64);
		u8g.print(VescMeasuredValues.ampHours, 0);
		u8g.drawStr(28, 64, "mAh");
		u8g.setPrintPos(50, 64);
		u8g.print(VescMeasuredValues.inpVoltage, 1);
		u8g.drawStr(80, 64, "V");
		u8g.drawStr(105, 64, "75%");
}

void setup()
{	

	Led.begin();
	Led.setBrightness(BRIGHTNESS);

#ifdef DEBUG
	Serial.begin(9600);
	Serial.println("Tx Started");
#endif

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

	//Initialization of Radio
	
	radio.begin();
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.openWritingPipe(pipe);

	//Initialization of buttons

	pinMode(UPPER_BUTTON, INPUT_PULLUP);
	pinMode(LOWER_BUTTON, INPUT_PULLUP);

	//Initialistation of Vibrator

	pinMode(VIBRATOR_PIN, OUTPUT);

	//Short feedback
	//for (size_t i = 0; i < 3; i++)
	//{
	//	Vibrator();
	//	delay(100);
	//}

	calculatedValues.numberCellsTx = CountCells((float) (analogRead(VOLTAGE_PIN))/ VOLTAGE_DIVISOR_TX);
	DEBUGSERIAL.println(calculatedValues.numberCellsTx);
	Vibrator(3);

}

void loop()
{
	if (calculatedValues.numberCellsVesc == 0)
	{
		calculatedValues.numberCellsVesc = CountCells(VescMeasuredValues.inpVoltage);
		}
	DEBUGSERIAL.print("calculatedNumberCellsVesc: "); DEBUGSERIAL.println(calculatedValues.numberCellsVesc);
	

//Read TxVoltage

	
	//float capTx = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);

	//if (capTx > 80)
	//{
	//	Led.setPixelColor(LED_TX, COLOR_GREEN);
	//	Led.show();
	//	Serial.println("1");
	//}
	//else if (capTx <= 80 && capTx > 60)
	//{
	//	Led.setPixelColor(LED_TX, COLOR_YELLOWGREEN);
	//	Led.show();
	//	Serial.println("2");
	//}
	//else if (capTx <= 60 && capTx > 30)
	//{
	//	Led.setPixelColor(LED_TX, COLOR_ORANGE);
	//	Led.show();
	//	Serial.println("3");
	//}
	//else if (capTx <= 30)
	//{
	//	Led.setPixelColor(LED_TX, COLOR_RED);
	//	Led.show();
	//	Serial.println("4");
	//}

	BatCapIndLED(LED_TX, ((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	BatCapIndLED(LED_VOLTAGE, VescMeasuredValues.inpVoltage, calculatedValues.numberCellsVesc);

	DEBUGSERIAL.print("inpVoltage: "); DEBUGSERIAL.println(VescMeasuredValues.inpVoltage);
//	DEBUGSERIAL.print("Capacity: "); DEBUGSERIAL.println(capTx);

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
	Led.setPixelColor(LED_TRANS, COLOR_GREEN);
	Led.show();

  }
  else
  {
  //Serial.println("Send failed!");
  failedCounter++;
  Led.setPixelColor(LED_TRANS, COLOR_RED);
  Led.show();
  }

if (recOK)
{
	Serial.println("Received values from Vesc:");
	SerialPrint(VescMeasuredValues);
	
}
#endif


	
	// picture loop
	u8g.firstPage();
	do {
		DrawScreenMain();
	} while (u8g.nextPage());

	// rebuild the picture after some delay
	delay(100);
//END Test 
}

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

