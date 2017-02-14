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
// Routines to find the flux and wavelength at which a minimum or edge of a feature occur
//
////////////////////////////////////////////////////////////////////////////////

void Find_Flux_Minimum(const double * i_lpdWavelengths, const double * i_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_uiNum_Points && i_lpdWavelengths[uiStart_Idx] < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMinima_Flux = DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_uiNum_Points && (i_dMax_WL < 0 || i_lpdWavelengths[uiI] < i_dMax_WL); uiI++)
	{
		if (i_lpdFluxes[uiI] < o_dMinima_Flux)
		{
			o_dMinima_Flux = i_lpdFluxes[uiI];
			o_dMinima_WL = i_lpdWavelengths[uiI];
		}
	}
}

void Find_Flux_At_WL(const ES::Spectrum & i_cSpectrum, const double & i_dWL, double & o_dFlux)
{
	unsigned int uiIdx = 0;
	o_dFlux = nan("");
	while (uiIdx < i_cSpectrum.size() && i_cSpectrum.wl(uiIdx) < i_dWL)
		uiIdx++;
	if (uiIdx < i_cSpectrum.size())
		o_dFlux = i_cSpectrum.flux(uiIdx);

}

void Find_Flux_Minimum(const ES::Spectrum & i_cSpectrum, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMinima_Flux = DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_cSpectrum.size() && (i_dMax_WL < 0 || i_cSpectrum.wl(uiI) < i_dMax_WL); uiI++)
	{
		if (i_cSpectrum.flux(uiI) > 0.0 && i_cSpectrum.flux(uiI) < o_dMinima_Flux)
		{
			o_dMinima_Flux = i_cSpectrum.flux(uiI);
			o_dMinima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Flux_Global_Maximum(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMaxima_Flux = -DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_cSpectrum.size() && (i_dMax_WL < 0 || i_cSpectrum.wl(uiI) < i_dMax_WL); uiI++)
	{
		if (i_cSpectrum.flux(uiI) > 0.0 && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Blue_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(i_cSpectrum, dMinima_WL, dMinima_Flux,i_dMin_WL,i_dMax_WL);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && i_cSpectrum.wl(uiI) < dMinima_WL; uiI++)
	{
		if ((i_cSpectrum.wl(uiI) > (i_dMin_WL - 250.0) || i_dMin_WL < 0) && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Red_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(i_cSpectrum, dMinima_WL, dMinima_Flux,i_dMin_WL,i_dMax_WL);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && (i_cSpectrum.wl(uiI) < i_dMax_WL || i_dMax_WL < 0); uiI++)
	{
		if (i_cSpectrum.wl(uiI) > (dMinima_WL + 250.0) && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}
