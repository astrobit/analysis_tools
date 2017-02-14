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



///////////////////////////////////////////////////////////////////////////////
//
// Routines that normalize one or more spectra
//
///////////////////////////////////////////////////////////////////////////////


void Normalize(double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_Flux)
{	// this routine normalizes to a given flux value
	double	dNormalization = 1.0 / i_dNormalization_Flux;
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		io_lpdFluxes[uiI] *= dNormalization;
	}
}

void Normalize_At_WL(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_WL, double & o_dNormalization_Flux)
{	// this routine searches for a given wavelength and normalizes to the flux value at that wavelength
	unsigned int uiNorm_Idx = 0;
	while (uiNorm_Idx < i_uiNum_Points && io_lpdWavelengths[uiNorm_Idx] < i_dNormalization_WL)
		uiNorm_Idx++;
	o_dNormalization_Flux = io_lpdFluxes[uiNorm_Idx];
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dNormalization_Flux);
}

void Normalize_At_Minima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux)
{	// this routine searches for the globlal flux minimum over the given data range, then normalizes the data to that value
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,o_dMinima_WL, o_dMinima_Flux);
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMinima_Flux);
}
void Normalize_At_Blue_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux)
{	// this routine first searches for a minimum, then finds the maximum that is blueward of the minimum.  The peak of this maximum is used as the normalization factor
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,dMinima_WL, dMinima_Flux);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_uiNum_Points && io_lpdWavelengths[uiI] < dMinima_WL; uiI++)
	{
		if (io_lpdFluxes[uiI] > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = io_lpdFluxes[uiI];
			o_dMaxima_WL = io_lpdWavelengths[uiI];
		}
	}

	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMaxima_Flux);
}
void Normalize_At_Red_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux)
{	// this routine first searches for a minimum, then finds the maximum that is redward of the minimum.  The peak of this maximum is used as the normalization factor
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,dMinima_WL, dMinima_Flux);
	unsigned int uiStart_Idx = 0;
	while (uiStart_Idx < i_uiNum_Points && io_lpdWavelengths[uiStart_Idx] < dMinima_WL)
		uiStart_Idx++;
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = uiStart_Idx; uiI < i_uiNum_Points; uiI++)
	{
		if (io_lpdFluxes[uiI] > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = io_lpdFluxes[uiI];
			o_dMaxima_WL = io_lpdWavelengths[uiI];
		}
	}
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMaxima_Flux);
}
