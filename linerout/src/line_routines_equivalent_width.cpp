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



double	Equivalent_Width(const XDATASET & i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length)
{
	double	dEW = -1.0;
	double	dContinuum_Flux[2] = {0,0};
	unsigned int uiContinuum_Flux_Index[2] = {0,0};
	for (unsigned int uiI = 0; uiI < i_cData.GetNumElements() && (uiContinuum_Flux_Index[0] == 0 || uiContinuum_Flux_Index[1] == 0); uiI++)
	{
		if (i_cData.GetElement(0,uiI) >= io_dContinuum_WL_Blue && uiContinuum_Flux_Index[0] == 0)
			uiContinuum_Flux_Index[0] = uiI;
		if (i_cData.GetElement(0,uiI) >= io_dContinuum_WL_Red && uiContinuum_Flux_Index[1] == 0)
			uiContinuum_Flux_Index[1] = uiI;
	}
	i_uiAveraging_Length <<= 1; // divide by 2
	if (i_uiAveraging_Length <= 0)
		i_uiAveraging_Length = 1;
	if (uiContinuum_Flux_Index[0] > 0 && uiContinuum_Flux_Index[1] > 0) // potential problem here if first index is about equal to continuum....
	{
		if (uiContinuum_Flux_Index[0] < i_uiAveraging_Length)
			uiContinuum_Flux_Index[0] = i_uiAveraging_Length;
		if (uiContinuum_Flux_Index[1] > (i_cData.GetNumElements() - i_uiAveraging_Length - 1))
			uiContinuum_Flux_Index[1] = (i_cData.GetNumElements() - i_uiAveraging_Length - 1);
		io_dContinuum_WL_Blue = i_cData.GetElement(0,uiContinuum_Flux_Index[0]);
		io_dContinuum_WL_Red = i_cData.GetElement(0,uiContinuum_Flux_Index[1]);

		uiContinuum_Flux_Index[0] -= i_uiAveraging_Length;
		uiContinuum_Flux_Index[1] -= i_uiAveraging_Length;

		for (unsigned int uiI = 0; uiI < 2 * i_uiAveraging_Length; uiI++)
		{
			dContinuum_Flux[0] += i_cData.GetElement(1,uiI + uiContinuum_Flux_Index[0]);
			dContinuum_Flux[1] += i_cData.GetElement(1,uiI + uiContinuum_Flux_Index[1]);
		}
		dContinuum_Flux[0] /= (2.0 * i_uiAveraging_Length);
		dContinuum_Flux[1] /= (2.0 * i_uiAveraging_Length);

		dEW = 0.0;
		for (unsigned int uiI = uiContinuum_Flux_Index[0]; uiI <= uiContinuum_Flux_Index[1]; uiI++)
		{
			double dCont = ((dContinuum_Flux[1] - dContinuum_Flux[0]) * (i_cData.GetElement(0,uiI) - io_dContinuum_WL_Blue) / (io_dContinuum_WL_Red - io_dContinuum_WL_Blue) + dContinuum_Flux[0]);
			double	dDelta_Wl;
			if (uiI > 0)
				dDelta_Wl = i_cData.GetElement(0,uiI) - i_cData.GetElement(0,uiI - 1);
			else
				dDelta_Wl = i_cData.GetElement(0,uiI + 1) - i_cData.GetElement(0,uiI);

			dEW += (1.0 - i_cData.GetElement(1,uiI) / dCont) * dDelta_Wl;
		}
	}
	return dEW;
}



double	Equivalent_Width(const ES::Spectrum &i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length)
{
	double	dEW = -1.0;
	double	dContinuum_Flux[2] = {0.0,0.0};
	unsigned int uiContinuum_Flux_Index[2] = {0,0};
	for (unsigned int uiI = 0; uiI < i_cData.size() && (uiContinuum_Flux_Index[0] == 0 || uiContinuum_Flux_Index[1] == 0); uiI++)
	{
		if (i_cData.wl(uiI) >= io_dContinuum_WL_Blue && uiContinuum_Flux_Index[0] == 0)
			uiContinuum_Flux_Index[0] = uiI;
		if (i_cData.wl(uiI) >= io_dContinuum_WL_Red && uiContinuum_Flux_Index[1] == 0)
			uiContinuum_Flux_Index[1] = uiI;
	}
	i_uiAveraging_Length <<= 1; // divide by 2
	if (i_uiAveraging_Length <= 0)
		i_uiAveraging_Length = 1;
	if (uiContinuum_Flux_Index[0] > 0 && uiContinuum_Flux_Index[1] > 0) // potential problem here if first index is about equal to continuum....
	{
		if (uiContinuum_Flux_Index[0] < i_uiAveraging_Length)
			uiContinuum_Flux_Index[0] = i_uiAveraging_Length;
		if (uiContinuum_Flux_Index[1] > (i_cData.size() - i_uiAveraging_Length - 1))
			uiContinuum_Flux_Index[1] = (i_cData.size() - i_uiAveraging_Length - 1);
		io_dContinuum_WL_Blue = i_cData.wl(uiContinuum_Flux_Index[0]);
		io_dContinuum_WL_Red = i_cData.wl(uiContinuum_Flux_Index[1]);

		uiContinuum_Flux_Index[0] -= i_uiAveraging_Length;
		uiContinuum_Flux_Index[1] -= i_uiAveraging_Length;

		for (unsigned int uiI = 0; uiI < 2 * i_uiAveraging_Length; uiI++)
		{
			dContinuum_Flux[0] += i_cData.flux(uiI + uiContinuum_Flux_Index[0]);
			dContinuum_Flux[1] += i_cData.flux(uiI + uiContinuum_Flux_Index[1]);
//			printf("Continuum at %i,%i: %.2e %.2e\n",uiI + uiContinuum_Flux_Index[0],uiI + uiContinuum_Flux_Index[1],i_cData.flux(uiI + uiContinuum_Flux_Index[0]),i_cData.flux(uiI + uiContinuum_Flux_Index[1]));
		}
		dContinuum_Flux[0] /= (2.0 * i_uiAveraging_Length);
		dContinuum_Flux[1] /= (2.0 * i_uiAveraging_Length);
//		printf("Continuum at min/max: %.2e %.2e\n",dContinuum_Flux[0],dContinuum_Flux[1]);
		dEW = 0.0;
		for (unsigned int uiI = uiContinuum_Flux_Index[0]; uiI <= uiContinuum_Flux_Index[1]; uiI++)
		{
			double dCont = ((dContinuum_Flux[1] - dContinuum_Flux[0]) * (i_cData.wl(uiI) - io_dContinuum_WL_Blue) / (io_dContinuum_WL_Red - io_dContinuum_WL_Blue) + dContinuum_Flux[0]);
			double	dDelta_Wl;
			if (uiI > 0)
				dDelta_Wl = i_cData.wl(uiI) - i_cData.wl(uiI - 1);
			else
				dDelta_Wl = i_cData.wl(uiI + 1) - i_cData.wl(uiI);

			dEW += (1.0 - i_cData.flux(uiI) / dCont) * dDelta_Wl;
		}
	}
	return dEW;
}
