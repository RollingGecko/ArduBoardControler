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

int CountCells(float voltage, int celltype) {
	
	for (int i = 1; i <= MAXNUMBERCELLS; i++)
	{
		if (celltype == CELL_LIPO)
		{
			if ((voltage > ((float)i*LIPO_MINVOLRANGE)) && (voltage < ((float)i*LIPO_MAXVOLRANGE)))
			{
				return i;
			}
		}
		if (celltype == CELL_LIION)
		{
			if ((voltage > ((float)i*LIION_MINVOLRANGE)) && (voltage < ((float)i*LIION_MAXVOLRANGE)))
			{
				return i;
			}
		}
	}
	return 0;
}

int CapCheckPerc(float voltage, int cells, int celltype) {
	float voltageCell = 0;
	int ind = 0;

	switch (celltype)
	{
	case 0:
		if (cells > 0)
		{
			voltageCell = (voltage / cells);
		}

		if (voltageCell >= LIPO_MAXVOLRANGE)
		{
			return (100);
		}

		//while (!(voltageCell <= lipoDc[0][ind + 1] && voltageCell > lipoDc[0][ind]) && ind <= (sizeof(lipoDc)/sizeof(*lipoDc)))
		while (!(voltageCell <= lipoDc[0][ind + 1] && voltageCell > lipoDc[0][ind]) && ind <= 10)
		{
			ind++;
		}

		if (voltageCell <= lipoDc[0][ind + 1] && voltageCell > lipoDc[0][ind])
		{
			float CapacPers = (((lipoDc[1][ind + 1] - lipoDc[1][ind]) / (lipoDc[0][ind + 1] - lipoDc[0][ind]))*(voltageCell - lipoDc[0][ind])) + lipoDc[1][ind];
			//	int intCapacPers = (int)(CapacPers * 100);
			return (CapacPers * 100);
		}
		else
		{
			return 0;

		}

		break;
	case 1:
		if (cells > 0)
		{
			voltageCell = (voltage / cells);
		}

		if (voltageCell >= LIION_MAXVOLRANGE)
		{
			return (100);
		}

		//while (!(voltageCell <= liionDc[0][ind + 1] && voltageCell > liionDc[0][ind]) && ind <= (sizeof(liionDc) / sizeof(*liionDc)))
		while(!(voltageCell <= liionDc[0][ind + 1] && voltageCell > liionDc[0][ind]) && ind <= 6)
		{
			ind++;
		}

		if (voltageCell <= liionDc[0][ind + 1] && voltageCell > liionDc[0][ind])
		{
			float CapacPers = (((liionDc[1][ind + 1] - liionDc[1][ind]) / (liionDc[0][ind + 1] - liionDc[0][ind]))*(voltageCell - liionDc[0][ind])) + liionDc[1][ind];
			//	int intCapacPers = (int)(CapacPers * 100);
			return (CapacPers * 100);
		}
		else
		{
			return 0;

		}

		break;

	default:
		return 0;
		break;
	}
}




