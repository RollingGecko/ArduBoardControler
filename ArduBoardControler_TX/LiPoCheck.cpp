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

#include "LiPoCheck.h"

int CountCells(float voltage) {
	
	for (int i = 1; i <= MAXNUMBERCELLS; i++)
	{
		if ((voltage > ((float)i*MINVOLRANGE)) && (voltage < ((float)i*MAXVOLRANGE)))
		{
			return i;
		}
	}
	return 0;
}

int CapCheckPerc(float voltage, int cells) {
	float voltageCell = 0;
	int ind = 0;

	if (cells > 0)
	{
		voltageCell = (voltage / cells);
	}

	if (voltageCell >=4.20)
	{
		return (100);
	}

	while (!(voltageCell<=lipoDc[0][ind+1] && voltageCell > lipoDc[0][ind])&& ind<=10)
	{
		ind++;
	}
	

	if (voltageCell <= lipoDc[0][ind + 1] && voltageCell > lipoDc[0][ind])
	{
		float CapacPers = (((lipoDc[1][ind + 1] - lipoDc[1][ind])/ (lipoDc[0][ind + 1] - lipoDc[0][ind]))*(voltageCell - lipoDc[0][ind])) + lipoDc[1][ind];
	//	int intCapacPers = (int)(CapacPers * 100);
		return (CapacPers * 100);
	}
	else
	{
		return 0;
		
	}
}




