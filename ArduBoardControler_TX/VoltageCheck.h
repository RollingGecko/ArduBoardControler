// VoltageCheck.h

#ifndef _VOLTAGECHECK_h
#define _VOLTAGECHECK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef _CONFIG_h

#include "Config.h"

#endif // !_CONFIG_h

int CountCells(float voltage);

#endif

