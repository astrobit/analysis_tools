#include <math.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>
#include <stack>





////////////////////////////////////////////////////////////////////////////////
//
// Get_Element_Number: given the 2-3 character element ID, return the atomic
// number of the element
//
////////////////////////////////////////////////////////////////////////////////

double Get_Element_Number(const char * i_lpszNuclide)
{
	double dZ;
	char lpszElem[4] = {(char)toupper(i_lpszNuclide[0]),0,0,0};
	if (i_lpszNuclide[1] != 0)
		lpszElem[1] = toupper(i_lpszNuclide[1]);
	if (i_lpszNuclide[2] != 0 && lpszElem[0] == 'U' && lpszElem[1] == 'U' && ((i_lpszNuclide[2] >= 'a' && i_lpszNuclide[2] <= 'z') || (i_lpszNuclide[2] >= 'A' && i_lpszNuclide[2] <= 'Z')))
		lpszElem[2] = toupper(i_lpszNuclide[2]);
	if (strcmp(lpszElem,"H") == 0)
		dZ = 1.0;
	else if (strcmp(lpszElem,"HE") == 0)
		dZ = 2.0;
	else if (strcmp(lpszElem,"LI") == 0)
		dZ = 3.0;
	else if (strcmp(lpszElem,"BE") == 0)
		dZ = 4.0;
	else if (strcmp(lpszElem,"B") == 0)
		dZ = 5.0;
	else if (strcmp(lpszElem,"C") == 0)
		dZ = 6.0;
	else if (strcmp(lpszElem,"N") == 0)
		dZ = 7.0;
	else if (strcmp(lpszElem,"O") == 0)
		dZ = 8.0;
	else if (strcmp(lpszElem,"F") == 0)
		dZ = 9.0;
	else if (strcmp(lpszElem,"NE") == 0)
		dZ = 10.0;
	else if (strcmp(lpszElem,"NA") == 0)
		dZ = 11.0;
	else if (strcmp(lpszElem,"MG") == 0)
		dZ = 12.0;
	else if (strcmp(lpszElem,"AL") == 0)
		dZ = 13.0;
	else if (strcmp(lpszElem,"SI") == 0)
		dZ = 14.0;
	else if (strcmp(lpszElem,"P") == 0)
		dZ = 15.0;
	else if (strcmp(lpszElem,"S") == 0)
		dZ = 16.0;
	else if (strcmp(lpszElem,"CL") == 0)
		dZ = 17.0;
	else if (strcmp(lpszElem,"AR") == 0)
		dZ = 18.0;
	else if (strcmp(lpszElem,"K") == 0)
		dZ = 19.0;
	else if (strcmp(lpszElem,"CA") == 0)
		dZ = 20.0;
	else if (strcmp(lpszElem,"SC") == 0)
		dZ = 21.0;
	else if (strcmp(lpszElem,"TI") == 0)
		dZ = 22.0;
	else if (strcmp(lpszElem,"V") == 0)
		dZ = 23.0;
	else if (strcmp(lpszElem,"CR") == 0)
		dZ = 24.0;
	else if (strcmp(lpszElem,"MN") == 0)
		dZ = 25.0;
	else if (strcmp(lpszElem,"FE") == 0)
		dZ = 26.0;
	else if (strcmp(lpszElem,"CO") == 0)
		dZ = 27.0;
	else if (strcmp(lpszElem,"NI") == 0)
		dZ = 28.0;
	else if (strcmp(lpszElem,"CU") == 0)
		dZ = 29.0;
	else if (strcmp(lpszElem,"ZN") == 0)
		dZ = 30.0;
	else if (strcmp(lpszElem,"GA") == 0)
		dZ = 31.0;
	else if (strcmp(lpszElem,"GE") == 0)
		dZ = 32.0;
	else if (strcmp(lpszElem,"AS") == 0)
		dZ = 33.0;
	else if (strcmp(lpszElem,"SE") == 0)
		dZ = 34.0;
	else if (strcmp(lpszElem,"BR") == 0)
		dZ = 35.0;
	else if (strcmp(lpszElem,"KR") == 0)
		dZ = 36.0;
	else if (strcmp(lpszElem,"RB") == 0)
		dZ = 37.0;
	else if (strcmp(lpszElem,"SR") == 0)
		dZ = 38.0;
	else if (strcmp(lpszElem,"Y") == 0)
		dZ = 39.0;
	else if (strcmp(lpszElem,"ZR") == 0)
		dZ = 40.0;
	else if (strcmp(lpszElem,"NB") == 0)
		dZ = 41.0;
	else if (strcmp(lpszElem,"MO") == 0)
		dZ = 42.0;
	else if (strcmp(lpszElem,"TC") == 0)
		dZ = 43.0;
	else if (strcmp(lpszElem,"RU") == 0)
		dZ = 44.0;
	else if (strcmp(lpszElem,"RH") == 0)
		dZ = 45.0;
	else if (strcmp(lpszElem,"PD") == 0)
		dZ = 46.0;
	else if (strcmp(lpszElem,"AG") == 0)
		dZ = 47.0;
	else if (strcmp(lpszElem,"CD") == 0)
		dZ = 48.0;
	else if (strcmp(lpszElem,"IN") == 0)
		dZ = 49.0;
	else if (strcmp(lpszElem,"SN") == 0)
		dZ = 50.0;
	else if (strcmp(lpszElem,"SB") == 0)
		dZ = 51.0;
	else if (strcmp(lpszElem,"TE") == 0)
		dZ = 52.0;
	else if (strcmp(lpszElem,"I") == 0)
		dZ = 53.0;
	else if (strcmp(lpszElem,"XE") == 0)
		dZ = 54.0;
	else if (strcmp(lpszElem,"CS") == 0)
		dZ = 55.0;
	else if (strcmp(lpszElem,"BA") == 0)
		dZ = 56.0;
	else if (strcmp(lpszElem,"LA") == 0)
		dZ = 57.0;
	else if (strcmp(lpszElem,"CE") == 0)
		dZ = 58.0;
	else if (strcmp(lpszElem,"PR") == 0)
		dZ = 59.0;
	else if (strcmp(lpszElem,"ND") == 0)
		dZ = 60.0;
	else if (strcmp(lpszElem,"PM") == 0)
		dZ = 61.0;
	else if (strcmp(lpszElem,"SM") == 0)
		dZ = 62.0;
	else if (strcmp(lpszElem,"EU") == 0)
		dZ = 63.0;
	else if (strcmp(lpszElem,"GD") == 0)
		dZ = 64.0;
	else if (strcmp(lpszElem,"TB") == 0)
		dZ = 65.0;
	else if (strcmp(lpszElem,"DY") == 0)
		dZ = 66.0;
	else if (strcmp(lpszElem,"HO") == 0)
		dZ = 67.0;
	else if (strcmp(lpszElem,"ER") == 0)
		dZ = 68.0;
	else if (strcmp(lpszElem,"TM") == 0)
		dZ = 69.0;
	else if (strcmp(lpszElem,"YB") == 0)
		dZ = 70.0;
	else if (strcmp(lpszElem,"LU") == 0)
		dZ = 71.0;
	else if (strcmp(lpszElem,"HF") == 0)
		dZ = 72.0;
	else if (strcmp(lpszElem,"TA") == 0)
		dZ = 73.0;
	else if (strcmp(lpszElem,"W") == 0)
		dZ = 74.0;
	else if (strcmp(lpszElem,"RE") == 0)
		dZ = 75.0;
	else if (strcmp(lpszElem,"OS") == 0)
		dZ = 76.0;
	else if (strcmp(lpszElem,"IR") == 0)
		dZ = 77.0;
	else if (strcmp(lpszElem,"PT") == 0)
		dZ = 78.0;
	else if (strcmp(lpszElem,"AU") == 0)
		dZ = 79.0;
	else if (strcmp(lpszElem,"HG") == 0)
		dZ = 80.0;
	else if (strcmp(lpszElem,"TL") == 0)
		dZ = 81.0;
	else if (strcmp(lpszElem,"PB") == 0)
		dZ = 82.0;
	else if (strcmp(lpszElem,"BI") == 0)
		dZ = 83.0;
	else if (strcmp(lpszElem,"PO") == 0)
		dZ = 84.0;
	else if (strcmp(lpszElem,"AT") == 0)
		dZ = 85.0;
	else if (strcmp(lpszElem,"RN") == 0)
		dZ = 86.0;
	else if (strcmp(lpszElem,"FR") == 0)
		dZ = 87.0;
	else if (strcmp(lpszElem,"RA") == 0)
		dZ = 88.0;
	else if (strcmp(lpszElem,"AC") == 0)
		dZ = 89.0;
	else if (strcmp(lpszElem,"TH") == 0)
		dZ = 90.0;
	else if (strcmp(lpszElem,"PA") == 0)
		dZ = 91.0;
	else if (strcmp(lpszElem,"U") == 0)
		dZ = 92.0;
	else if (strcmp(lpszElem,"NP") == 0)
		dZ = 93.0;
	else if (strcmp(lpszElem,"PU") == 0)
		dZ = 94.0;
	else if (strcmp(lpszElem,"AM") == 0)
		dZ = 95.0;
	else if (strcmp(lpszElem,"CM") == 0)
		dZ = 96.0;
	else if (strcmp(lpszElem,"BK") == 0)
		dZ = 97.0;
	else if (strcmp(lpszElem,"CF") == 0)
		dZ = 98.0;
	else if (strcmp(lpszElem,"ES") == 0)
		dZ = 99.0;
	else if (strcmp(lpszElem,"FM") == 0)
		dZ = 100.0;
	else if (strcmp(lpszElem,"MD") == 0)
		dZ = 101.0;
	else if (strcmp(lpszElem,"NO") == 0)
		dZ = 102.0;
	else if (strcmp(lpszElem,"LR") == 0)
		dZ = 103.0;
	else if (strcmp(lpszElem,"RF") == 0)
		dZ = 104.0;
	else if (strcmp(lpszElem,"DB") == 0)
		dZ = 105.0;
	else if (strcmp(lpszElem,"SG") == 0)
		dZ = 106.0;
	else if (strcmp(lpszElem,"BH") == 0)
		dZ = 107.0;
	else if (strcmp(lpszElem,"HS") == 0)
		dZ = 108.0;
	else if (strcmp(lpszElem,"MT") == 0)
		dZ = 109.0;
	else if (strcmp(lpszElem,"DS") == 0)
		dZ = 110.0;
	else if (strcmp(lpszElem,"RG") == 0)
		dZ = 111.0;
	else if (strcmp(lpszElem,"CN") == 0)
		dZ = 112.0;
	else if (strcmp(lpszElem,"UUT") == 0)
		dZ = 113.0;
	else if (strcmp(lpszElem,"FL") == 0)
		dZ = 114.0;
	else if (strcmp(lpszElem,"UUP") == 0)
		dZ = 115.0;
	else if (strcmp(lpszElem,"LV") == 0)
		dZ = 116.0;
	else if (strcmp(lpszElem,"UUS") == 0)
		dZ = 117.0;
	else if (strcmp(lpszElem,"UUO") == 0)
		dZ = 118.0;

	return dZ;
}

////////////////////////////////////////////////////////////////////////////////
//
// Compute_Velocity: compute the velocity of a line based on the rest wavelength
//      and observed wavelength of the line, using fully relativistic
//      term.
//      The result is returned in km/s.
//
////////////////////////////////////////////////////////////////////////////////


double	Compute_Velocity(const double & i_dObserved_Wavelength, const double & i_dRest_Wavelength)
{
	double	dz = i_dObserved_Wavelength / i_dRest_Wavelength;
	double	dz_sqr = dz * dz;
	return (2.99792458e5 * (dz_sqr - 1.0) / (dz_sqr + 1.0));
}

