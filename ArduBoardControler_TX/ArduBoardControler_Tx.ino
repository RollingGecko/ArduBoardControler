
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
#ifdef OLED_USED
#include "U8glib.h"  
#endif // OLED_USED
#include "datatypes.h"
#include "local_datatypes.h"
#ifdef DEBUG
#include "VescUart.h" //SerialPrint for received Data Package
#endif
#ifdef STATUS_LED_USED
#include <Adafruit_NeoPixel.h>
#include "WS2812Color.h"
#endif //STATUS_LED_USED
#include "printf.h"
#include "LiPoCheck.h"


//
// Hardware configuration
//
//Set up LED WS2812

#ifdef STATUS_LED_USED
Adafruit_NeoPixel Led = Adafruit_NeoPixel(NUM2812, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif // STATUS_LEDS_USED

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(CEPIN,CSPIN);

//Setup OLED display

//Please check the usage of the right constructor for your OLED driver in the u8glib. 
//Here we use a SSD1306 with HW_SPI
#ifdef OLED_USED
U8GLIB_SSD1306_128X64 u8g(OLED_CSPIN, OLED_CEPIN, OLED_MISO, OLED_MOSI, OLED_SCK);
//U8GLIB_SSD1306_128X64 u8g(SCL, SDA, CS, DC, RES);
#endif // OLED_USED

struct remotePackage remPack;
struct bldcMeasure VescMeasuredValues;
struct calcValues calculatedValues;

// Declaration of global variables and const

long failedCounter = 0;
boolean sendOK = false;
boolean recOK = false;
int averageCycles = 2;

enum Display
{
	MIN_DISPLAY_ENUM,
	DEF_SCR,
	CURRENT_SCR,
	SPEED_SCR,
	CAP_SCR,
	DISTANCE_SCR,
	MAH_SCR,
	MAXSPEED_SCR,
	MAXCURRENT_SCR,
	MAX_DISPLAY_ENUM
};
int numberDisplayShown = DEF_SCR;

#ifndef SEND_LR

enum JoyStatus {
	center,
    left,
	right
};

JoyStatus joyStatus;

int leftright = 127;

#endif //SEND_LR


const float ratioRpmSpeed = (DIA_WHEEL * 3.14156 * 60) / (ERPM_REV * RATIO_GEAR * 1000000);  //ERPM to Km/h
const float   rationRotDist = ((DIA_WHEEL * 3.14156) / (PULSE_REV * RATIO_GEAR * 1000000))*CORRECT_FACTOR_DISTANCE; //Pulses to Km

//function declaration

void inline Vibrator();
void inline Vibrator(int numberCycles);
void BatCapIndLED(int led, float voltage, int numberCells);
#ifdef OLED_USED
void DrawScreenMain(void);
void DrawScreenSingleValue(float value, char digits[3]);

#endif // OLED_USED

void setup()
{	

	

	//Led class is started and brightness is defined
#ifdef STATUS_LED_USED
	Led.begin();
	Led.setBrightness(BRIGHTNESS);
#endif // STATUS_LED_USED

#ifdef DEBUG
	Serial.begin(9600);
//	Serial.println("Tx Started");
#endif

#ifdef STATUS_LED_USED

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
#endif // STATUS_LED_USED


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

	// number of cells for tx is calculates once

	calculatedValues.numberCellsTx = CountCells((float) (analogRead(VOLTAGE_PIN))/ VOLTAGE_DIVISOR_TX);

	//Short feedback

	Vibrator(3);

}

void loop()
{
	//calculates the number of cells of the board if not allready happen
	//needs to be in the loop because it is not clear when board is powered uo
	if (calculatedValues.numberCellsVesc == 0)
	{
		calculatedValues.numberCellsVesc = CountCells(VescMeasuredValues.inpVoltage);
	}

	//Calculation from measured values	
	//Average calculation

	if (averageCycles < AVERAGE_CYCLE)
	{
		calculatedValues.currentAverage = ((averageCycles - 1) * calculatedValues.currentAverage + VescMeasuredValues.avgMotorCurrent) / averageCycles;
		calculatedValues.rpmAverage = ((averageCycles - 1) * calculatedValues.rpmAverage + VescMeasuredValues.rpm) / averageCycles;
		averageCycles++;
	}
	else
	{
		calculatedValues.currentAverage = ((averageCycles - 1) * calculatedValues.currentAverage + VescMeasuredValues.avgMotorCurrent) / averageCycles;
		calculatedValues.rpmAverage = ((averageCycles - 1) * calculatedValues.rpmAverage + VescMeasuredValues.rpm) / averageCycles;
		averageCycles = 2;
	}

	calculatedValues.VescPersCap = CapCheckPerc(VescMeasuredValues.inpVoltage, calculatedValues.numberCellsVesc);
	calculatedValues.TxPersCap = CapCheckPerc(((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	calculatedValues.speed = calculatedValues.rpmAverage * ratioRpmSpeed;
	calculatedValues.distanceTravel = VescMeasuredValues.tachometer * rationRotDist;

//Check for maxima
	if (calculatedValues.speed > calculatedValues.maxSpeed)
	{
		calculatedValues.maxSpeed = calculatedValues.speed;
	}

	if (calculatedValues.currentAverage > calculatedValues.maxCurrent)
	{
		calculatedValues.maxCurrent = calculatedValues.currentAverage;
	}
#ifdef STATUS_LED_USED

	BatCapIndLED(LED_TX, ((float)analogRead(VOLTAGE_PIN) / VOLTAGE_DIVISOR_TX), calculatedValues.numberCellsTx);
	BatCapIndLED(LED_VOLTAGE, VescMeasuredValues.inpVoltage, calculatedValues.numberCellsVesc);

#endif // STATUS_LED_USED

	//read iputs
#ifdef SEND_LR	
	remPack.valXJoy = map(analogRead(JOY_X), 0, 1023, 0, 255);
#else //if Joystick information of Y is not used for remote it can be used in remote
	remPack.valXJoy = 127;
	leftright = analogRead(JOY_X);
#endif // END_LR
	remPack.valYJoy = map(analogRead(JOY_Y), 0, 1023, 0, 255);
	remPack.valLowerButton = !digitalRead(LOWER_BUTTON);
	remPack.valUpperButton = !digitalRead(UPPER_BUTTON);
	//send data via radio to RX
	sendOK = radio.write(&remPack, sizeof(remPack));

	//read Acknowledegement message from RX

	while (radio.isAckPayloadAvailable())
	{
		radio.read(&VescMeasuredValues, sizeof(VescMeasuredValues));
		recOK = true;

	}

	if (sendOK)
	{
#ifdef DEBUG
		Serial.print("X= "); Serial.print(remPack.valXJoy); Serial.print(" Y= "); Serial.println(remPack.valYJoy);
		Serial.println("Send successfully!");
		Serial.print("Failed= "); Serial.println(failedCounter);
#endif //DEBUG
		sendOK = false;
#ifdef STATUS_LED_USED
		Led.setPixelColor(LED_TRANS, COLOR_GREEN);
		Led.show();
#endif // STATUS_LED_USED


	}
	else
	{
		//Serial.println("Send failed!");
		failedCounter++;
#ifdef STATUS_LED_USED
		Led.setPixelColor(LED_TRANS, COLOR_RED);
		Led.show();

#endif // STATUS_LED_USED  }
	}
		if (recOK)
		{
#ifdef DEBUG
			Serial.println("Received values from Vesc:");
			SerialPrint(VescMeasuredValues);
#endif //DEBUG
		}

//Read y Joystick as switch left right for display

#ifndef SEND_LR
		if (leftright < (512 - JOYSTICKBUTTON_DEADBAND) && joyStatus != right)
		{
			joyStatus = right;
			numberDisplayShown++;
			if (numberDisplayShown == MAX_DISPLAY_ENUM)
			{
				numberDisplayShown = DEF_SCR;
			}
			//Led.setPixelColor(LED_FOUR, COLOR_RED);
			//Led.show();
			//delay(500);
			//Led.setPixelColor(LED_FOUR, COLOR_YELLOW);
			//Led.show();
			//do something
		}

		else if (leftright > (512 + JOYSTICKBUTTON_DEADBAND) && joyStatus != left)
		{
			joyStatus = left;

			numberDisplayShown--;
			if (numberDisplayShown == MIN_DISPLAY_ENUM)
			{
				numberDisplayShown = MAX_DISPLAY_ENUM - 1;
			}

			//Led.setPixelColor(LED_FOUR, COLOR_GREEN);
			//Led.show();
			//delay(500);
			//Led.setPixelColor(LED_FOUR, COLOR_YELLOW);
			//Led.show();
			
			
		}
		else if (leftright < (512+JOYSTICKBUTTON_DEADBAND) && leftright > (512-JOYSTICKBUTTON_DEADBAND))
		{
			joyStatus = center;
		}
#endif //SEND_LR

		//// picture loop
	// picture loop for oled display

#ifdef OLED_USED
		u8g.firstPage();
		do {
			switch (numberDisplayShown)
			{
			case DEF_SCR:
				DrawScreenMain();
				break;
			case CURRENT_SCR:
				DrawScreenSingleValue("Current",calculatedValues.currentAverage, "A");
				break;
			case SPEED_SCR:
				DrawScreenSingleValue("Speed",calculatedValues.speed, "kmh");
				break;
			case CAP_SCR:
				DrawScreenSingleValue("Capacity", calculatedValues.VescPersCap, "%");
				break;
			case MAXSPEED_SCR:
				DrawScreenSingleValue("Speed max", calculatedValues.maxSpeed, "kmh");
				break;
			case MAXCURRENT_SCR:
				DrawScreenSingleValue("Current max", calculatedValues.maxCurrent, "A");
				break;
			case DISTANCE_SCR:
				DrawScreenSingleValue("Distance", calculatedValues.distanceTravel, "km");
				break;
			case MAH_SCR:
				DrawScreenSingleValue("used mAh", (VescMeasuredValues.ampHours/1000), "mAh");
				break;
			default:
				break;
			}
			//DrawScreenMain();
		} while (u8g.nextPage());

		

		//// rebuild the picture after some delay
#endif // OLED_USED
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

#ifdef STATUS_LED_USED
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
#endif // STATUS_LED_USED


#ifdef OLED_USED
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
	u8g.print(calculatedValues.currentAverage, 1);
	u8g.setFont(u8g_font_courB08);
	u8g.setFontPosTop();
	u8g.drawStr(120, 11, "A");
	u8g.drawHLine(0, 53, 128);
	u8g.setFontPosBottom();
	u8g.setFont(u8g_font_courB08);
	u8g.setPrintPos(0, 64);
	u8g.print((VescMeasuredValues.ampHours/1000), 0);
	u8g.drawStr(28, 64, "mAh");
	u8g.setPrintPos(50, 64);
	u8g.print(VescMeasuredValues.inpVoltage, 1);
	u8g.drawStr(80, 64, "V");
	u8g.setPrintPos(103, 64);
	u8g.print(calculatedValues.VescPersCap);
	u8g.drawStr(120, 64, "%");
}

void DrawScreenSingleValue(char titel[10], float value, char digits[3]) {
	u8g.setFontPosTop();
	u8g.setFont(u8g_font_courB14r);
	u8g.setPrintPos(0, 6);
	u8g.print(titel);
	u8g.setFont(u8g_font_ncenB24r);
	u8g.setFontPosTop();
	u8g.setPrintPos(0, 25);
	u8g.print(value, 1);
	u8g.setFont(u8g_font_courB14r);
	u8g.setFontPosTop();
	u8g.drawStr(80, 28, digits);
}
#endif // OLED_USED
