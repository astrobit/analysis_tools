#include "cpgplot.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	int i;
	static float xs[] = {1.0, 2.0, 3.0, 4.0, 5.0 };
	static float ys[] = {1.0, 4.0, 9.0, 16.0, 25.0 };
	float xr[60], yr[60];
	int n = sizeof(xr) / sizeof(xr[0]);
	/*
	* Call PGBEG to initiate PGPLOT and open the output device; PGBEG
	* will prompt the user to supply the device name and type.
	*/
	if(cpgbeg(0, "?", 1, 1) != 1)
		return 1;
	/*
	* Call PGENV to specify the range of the axes and to draw a box, and
	* PGLAB to label it. The x-axis runs from 0 to 10, and y from 0 to 20.
	*/
	cpgenv(0.0, 10.0, 0.0, 20.0, 0, 1);
	cpglab("(x)", "(y)", "PGPLOT Example 1: y = x\\u2\\d");
	/*
	* Mark five points (coordinates in arrays XS and YS), using symbol
	* number 9.
	*/
	cpgpt(5, xs, ys, 9);
	/*
	* Compute the function at 'n=60' points, and use PGLINE to draw it.
	*/
	for(i=0; i<n; i++)
	{
		xr[i] = 0.1*i;
		yr[i] = xr[i]*xr[i];
	}
	cpgline(n, xr, yr);
	/*
	* Finally, call PGEND to terminate things properly.
	*/
	cpgend();
	return 0;
}
