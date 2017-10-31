// VoltageCheck.h

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

#ifndef _LIPOCHECK_h
#define _LIPOCHECK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef _CONFIG_h

#include "Config.h"

#endif // !_CONFIG_h

#define CELL_LIPO	0
#define LIPO_MINVOLRANGE	3.3
#define	LIPO_MAXVOLRANGE	4.2
#define CELL_LIION	1
#define LIION_MINVOLRANGE	2.5
#define	LIION_MAXVOLRANGE	4.2

//const float lipoDc[2][8] = { { 0.0, 3.670,3.740,3.790,3.820,3.900,4.030,4.200 } , {0.000, 0.100,0.200,0.300,0.400,0.600,0.800,1.000 } };
const float lipoDc[2][11] = { { 3.2, 3.680,3.740,3.780,3.810,3.850,3.870,3.960,4.02,4.100,4.200 } ,{ 0.000, 0.100,0.200,0.300,0.400,0.500,0.600,0.700,0.800,0.900,1.000 } };
const float liionDc[2][7] = { {2.5, 3.4, 3.6, 3.68, 3.75, 3.88, 4.2} ,{ 0.000, 0.100, 0.200,0.400,0.600,0.800, 1.000} }; //source: http://www.ibt-power.com/bkencel/Graphs/LiIonDischGph.JPG
int CountCells(float voltage, int celltype);

int CapCheckPerc(float voltage, int cells, int celltype);


#endif

