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



void Find_Minimum(ES::Spectrum * i_lpcSpectra, unsigned int i_uiNum_Spectra, double * o_lpdMinima_WL, double * o_lpdMinima_Flux, const double & i_dMin_WL, const double & i_dMax_WL)
{
	if (o_lpdMinima_WL || o_lpdMinima_Flux)
	{
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
		{
			double	dMinimum_Lcl = DBL_MAX;
			if (o_lpdMinima_WL)
				o_lpdMinima_WL[uiJ] = 0.0;
			for (unsigned int uiK = 0; uiK < i_lpcSpectra[uiJ].size(); uiK++)
			{
				if ((i_dMin_WL < 0.0 || i_dMin_WL > i_dMax_WL || i_lpcSpectra[uiJ].wl(uiK) >= i_dMin_WL) && 
					(i_dMax_WL < 0.0 || i_dMax_WL < i_dMin_WL || i_lpcSpectra[uiJ].wl(uiK) <= i_dMax_WL))
				{
					if (i_lpcSpectra[uiJ].flux(uiK) < dMinimum_Lcl)
					{
						dMinimum_Lcl = i_lpcSpectra[uiJ].flux(uiK);
						if (o_lpdMinima_WL)
							o_lpdMinima_WL[uiJ] = i_lpcSpectra[uiJ].wl(uiK);
					}
				}
			}
			if (o_lpdMinima_Flux)
				o_lpdMinima_Flux[uiJ] = dMinimum_Lcl;
		}
	}
}
