// 
// 
// 

#include "VoltageCheck.h"

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



