#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>

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


unsigned int g_uiVelocity_Map_Alloc_Size = 0;
double		* g_lpdVelocities = NULL;
double		* g_lpdOpacity_Profile[2] = {NULL,NULL};
ES::Synow::Grid * g_lpcGrid = NULL;
ES::Synow::Grid * g_lpcGrid_Exp = NULL;
ES::Synow::Opacity * g_lpcOpacity = NULL;
ES::Synow::Opacity * g_lpcOpacity_Exp = NULL;
ES::Spectrum * g_lpcReference = NULL;
ES::Synow::Source * g_lpcSource = NULL;
ES::Synow::Source * g_lpcSource_Exp = NULL;
ES::Synow::Spectrum * g_lpcSpectrum = NULL;
ES::Synow::Spectrum * g_lpcSpectrum_Exp = NULL;
ES::Spectrum * g_lpcOutput = NULL;
bool		g_bMin_Delta_Init = false;
double		g_dMin_Delta = DBL_MAX;

void Allocate_Synow_Classes(const ES::Spectrum &i_cTarget, unsigned int uiNum_Elements, const XDATASET * i_lpOpacity_Map)
{
	if (i_lpOpacity_Map)
		uiNum_Elements = i_lpOpacity_Map->GetNumElements() - 1;

	for (unsigned int uiI = 1; uiI < i_cTarget.size() && !g_bMin_Delta_Init; uiI++)
	{
		if ((i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1)) < g_dMin_Delta)
			g_dMin_Delta = i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1);
	}


	if (uiNum_Elements > g_uiVelocity_Map_Alloc_Size)
	{
		ES::ERROR::Inhibit_Error_Reporting();

//		printf("Realloc\n");
		if (g_lpdVelocities)
			delete [] g_lpdVelocities;
		if (g_lpdOpacity_Profile[0])
			delete [] g_lpdOpacity_Profile[0];
		if (g_lpdOpacity_Profile[1])
			delete [] g_lpdOpacity_Profile[1];
		if (g_lpcGrid)
			delete [] g_lpcGrid;
		if (g_lpcOpacity)
			delete [] g_lpcOpacity;
		if (g_lpcReference)
			delete [] g_lpcReference;
		if (g_lpcSource)
			delete [] g_lpcSource;
		if (g_lpcSpectrum)
			delete [] g_lpcSpectrum;
		if (g_lpcOutput)
			delete [] g_lpcOutput;
		if (g_lpcGrid_Exp)
			delete [] g_lpcGrid_Exp;
		if (g_lpcOpacity_Exp)
			delete [] g_lpcOpacity_Exp;
		if (g_lpcSource_Exp)
			delete [] g_lpcSource_Exp;
		if (g_lpcSpectrum_Exp)
			delete [] g_lpcSpectrum_Exp;

		g_lpdVelocities = new double[uiNum_Elements];
		g_lpdOpacity_Profile[0] = new double[uiNum_Elements];
		g_lpdOpacity_Profile[1] = new double[uiNum_Elements];
		g_uiVelocity_Map_Alloc_Size = uiNum_Elements;
//		printf("%i\n",i_cOpacity_Map_A.GetNumElements());
		g_lpcOutput = new ES::Spectrum(i_cTarget);
		g_lpcReference = new ES::Spectrum( g_lpcOutput[0] );

		if (i_lpOpacity_Map)
		{
			for (unsigned int uiI = 1; uiI < i_lpOpacity_Map->GetNumElements(); uiI++)
			{
				g_lpdVelocities[uiI - 1] = i_lpOpacity_Map->GetElement(0,uiI) * 1e-8;
//				printf("%.2e\n",g_lpdVelocities[uiI - 1]);
			}
		}
		else
			for (unsigned int uiI = 0; uiI < uiNum_Elements; uiI++)
				g_lpdVelocities[uiI] = (128.0 / (uiNum_Elements + 1.0)) * (uiI + 1); // between 0 and 128

		g_lpcGrid = new ES::Synow::Grid(ES::Synow::Grid::create( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, uiNum_Elements, g_lpdVelocities));
		g_lpcGrid_Exp = new ES::Synow::Grid(ES::Synow::Grid::create( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, 2048, g_lpdVelocities));

		g_lpcOpacity = new ES::Synow::Opacity( g_lpcGrid[0], getenv("SYNXX_LINES_DATA_PATH"), getenv("SYNXX_REF_LINE_DATA_PATH"), "user_profile", 10.0, -3.0 );
		g_lpcOpacity_Exp = new ES::Synow::Opacity( g_lpcGrid_Exp[0], getenv("SYNXX_LINES_DATA_PATH"), getenv("SYNXX_REF_LINE_DATA_PATH"), "exp", 10.0, -3.0 );

		g_lpcSource = new ES::Synow::Source (g_lpcGrid[0], 16.0);
		g_lpcSource_Exp = new ES::Synow::Source (g_lpcGrid_Exp[0], 16.0);

		g_lpcSpectrum = new ES::Synow::Spectrum( g_lpcGrid[0], g_lpcOutput[0], g_lpcReference[0], 60, false);
		g_lpcSpectrum_Exp = new ES::Synow::Spectrum( g_lpcGrid_Exp[0], g_lpcOutput[0], g_lpcReference[0], 60, false);
	}
	else if (i_lpOpacity_Map)
	{
		for (unsigned int uiI = 1; uiI < i_lpOpacity_Map->GetNumElements(); uiI++)
		{
			g_lpdVelocities[uiI - 1] = i_lpOpacity_Map->GetElement(0,uiI) * 1e-8;
		}
	}
	else
	{
		for (unsigned int uiI = 0; uiI < uiNum_Elements; uiI++)
			g_lpdVelocities[uiI] = (128.0 / (uiNum_Elements + 1.0)) * (uiI + 1); // between 0 and 128
	}
}

void Generate_Synow_Spectra(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput)
{
	Allocate_Synow_Classes(i_cTarget,0,&i_cOpacity_Map_A);
//    o_cOutput = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());
//    ES::Spectrum output = ES::Spectrum::create_from_spectrum( o_cOutput );
//    ES::Spectrum reference = ES::Spectrum::create_from_spectrum( i_cTarget );

//	printf("I: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
//	printf("%.3e\n",o_cOutput.flux(100));

	double	dVmax = i_cOpacity_Map_A.GetElement(0,i_cOpacity_Map_A.GetNumElements() - 1) * 1.0e-8;
    // Grid object.
	

    // Attach setups one by one.
		// Initialize Ion data
    ES::Synow::Setup cSetup;
	g_lpcOutput[0].zero_flux();

//	double	dT_ref = i_cParameters.Get(0) * (i_cOpacity_Map_A.GetElement(2,0) - i_cOpacity_Map_A.GetElement(1,0)) + i_cOpacity_Map_A.GetElement(1,0);
	double	dT_Scalar = pow(i_cParameters.Get(0) / i_cOpacity_Map_A.GetElement(1,0),-2.0);
//	printf("t scalar = %.2e (%.4f %.4f)\n",dT_Scalar,i_cParameters.Get(0),i_cOpacity_Map_A.GetElement(1,0));
//	int iT_ref = (int )(i_cParameters.Get(0) + 0.5);
//	if (iT_ref < 0)
//		iT_ref = 0;
//	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
//		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
//	iT_ref++;

    cSetup.a0 = 1.0;
    cSetup.a1 = 0.0;
    cSetup.a2 = 0.0;
    cSetup.v_phot = i_cParameters.Get(1);
//	printf("Vphot = %.2f\n",cSetup.v_phot);
    cSetup.v_outer = dVmax;
    cSetup.t_phot = i_cParameters.Get(2);
	cSetup.ions.push_back(i_uiIon);
	cSetup.active.push_back(true);
	cSetup.log_tau.push_back(i_cParameters.Get(4));
	cSetup.temp.push_back(i_cParameters.Get(3));
	cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
	cSetup.user_profile = g_lpdOpacity_Profile;
	for (unsigned int uiI = 0; uiI < i_cOpacity_Map_A.GetNumElements() - 1; uiI++)
	{
//		cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(iT_ref,uiI + 1);
		cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(1,uiI + 1) * dT_Scalar;
	}
	if (i_cOpacity_Map_B.GetNumElements() > 0)
	{
		cSetup.ions.push_back(i_uiIon);
		cSetup.active.push_back(true);
		cSetup.log_tau.push_back(i_cParameters.Get(6));
		cSetup.temp.push_back(i_cParameters.Get(5));
		cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
		for (unsigned int uiI = 0; uiI < i_cOpacity_Map_B.GetNumElements() - 1; uiI++)
		{
//			cSetup.user_profile[1][uiI] = i_cOpacity_Map_B.GetElement(iT_ref,uiI + 1);
			cSetup.user_profile[1][uiI] = i_cOpacity_Map_B.GetElement(1,uiI + 1) * dT_Scalar;
		}
	}

	g_lpcGrid->reset(cSetup);
    g_lpcGrid[0]( cSetup );
//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
	o_cOutput = g_lpcOutput[0];
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

//	printf("%.3e\n",o_cOutput.flux(100));
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

}

void Generate_Synow_Spectra_Exp(const ES::Spectrum &i_cTarget, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput)
{
//    o_cOutput = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());
//    ES::Spectrum output = ES::Spectrum::create_from_spectrum( o_cOutput );
//    ES::Spectrum reference = ES::Spectrum::create_from_spectrum( i_cTarget );

//	printf("I: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
//	printf("%.3e\n",o_cOutput.flux(100));

    // Grid object.
	
	Allocate_Synow_Classes(i_cTarget,2048,NULL);

	double	dVmax = i_cParameters.Get(5);
	if (i_cParameters.Get_Size() > 7 && i_cParameters.Get(10) > dVmax)
		dVmax = i_cParameters.Get(10);

    // Attach setups one by one.
		// Initialize Ion data
    ES::Synow::Setup cSetup;
	g_lpcOutput[0].zero_flux();

    cSetup.a0 = 1.0;
    cSetup.a1 = 0.0;
    cSetup.a2 = 0.0;
    cSetup.v_phot = i_cParameters.Get(0);
//	printf("Vphot = %.2f\n",cSetup.v_phot);
    cSetup.v_outer = dVmax;
    cSetup.t_phot = i_cParameters.Get(1);
	cSetup.ions.push_back(i_uiIon);
	cSetup.active.push_back(true);
	cSetup.log_tau.push_back(i_cParameters.Get(2));
	cSetup.temp.push_back(i_cParameters.Get(3));
	cSetup.v_min.push_back(i_cParameters.Get(4));
	cSetup.v_max.push_back(i_cParameters.Get(5));
	cSetup.aux.push_back(i_cParameters.Get(6));
	cSetup.form.push_back(ES::Synow::Setup::form_exp);
	if (i_cParameters.Get_Size() > 7)
	{
		cSetup.ions.push_back(i_uiIon);
		cSetup.active.push_back(true);
		cSetup.log_tau.push_back(i_cParameters.Get(7));
		cSetup.temp.push_back(i_cParameters.Get(8));
		cSetup.v_min.push_back(i_cParameters.Get(9));
		cSetup.v_max.push_back(i_cParameters.Get(10));
		cSetup.aux.push_back(i_cParameters.Get(11));
		cSetup.form.push_back(ES::Synow::Setup::form_exp);
	}

	g_lpcGrid_Exp->reset(cSetup);
    g_lpcGrid_Exp[0]( cSetup );
//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
	o_cOutput = g_lpcOutput[0];
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

//	printf("%.3e\n",o_cOutput.flux(100));
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

}


// Does a point (vector) fall fully inside a given set of bounds.  e.g. x_i < y_i for all i
bool xPoint_Within_Bound(const XVECTOR &i_lpVector, const XVECTOR &i_cThreshold)
{
	XVECTOR cTest;
	bool bRet = (i_cThreshold.Get_Size() == i_lpVector.Get_Size());
	if (!bRet)
		fprintf(stderr,"xPoint_Within_Bound: Convergence threshold size not the same as paramter vector size!\n");

	for (unsigned int uiK = 0; uiK < i_lpVector.Get_Size() && bRet; uiK++)
	{
		bRet = (fabs(i_lpVector.Get(uiK)) < i_cThreshold.Get(uiK));
	}
	return bRet;
}

// Given a set of points in n-space and a bound, is the space between all points less than the bound.  e.g. (|x_i,j - x_k,j| < y_j) for all i,j,k
bool xConvergence(const XVECTOR * i_lpVectors, unsigned int i_uiNum_Points,const XVECTOR &i_cThreshold, XVECTOR * o_lpcConvergence_Fault)
{
	XVECTOR cTest;
	bool bRet = true;
	for(unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
		bRet &= (i_cThreshold.Get_Size() == i_lpVectors[uiI].Get_Size());
	if (!bRet)
		fprintf(stderr,"xConvergence: Convergence threshold size not the same as paramter vector size!\n");

	for(unsigned int uiI = 0; uiI < i_uiNum_Points && bRet; uiI++)
	{
		for (unsigned int uiJ = uiI + 1; uiJ < i_uiNum_Points && bRet; uiJ++)
		{
			cTest = i_lpVectors[uiJ] - i_lpVectors[uiI];
			for (unsigned int uiK = 0; uiK < cTest.Get_Size() && bRet; uiK++)
			{
				bRet = (fabs(cTest.Get(uiK)) < i_cThreshold.Get(uiK));
//				if (!bRet)
//					printf("%i %.2e %.2e\n",uiK,cTest.Get(uiK),i_cThreshold.Get(uiK));
				if (!bRet && o_lpcConvergence_Fault)
				{
					o_lpcConvergence_Fault[0] = cTest;
				}
			}
		}
	}
//	printf("Conv: %c\n",bRet ? 'y' : 'n');
	return bRet;
}

// is a given point within a cuboid.  i_lpcBounds must be a 2 element array, with the 
bool xBoundTest(const XVECTOR & i_cTest_Point, const XVECTOR * i_lpcBounds)
{
	bool bRet;
	bRet = (i_cTest_Point.Get_Size() == i_lpcBounds[0].Get_Size() && i_cTest_Point.Get_Size() == i_lpcBounds[1].Get_Size());
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds size not the same as test point size! %i %i %i\n",i_cTest_Point.Get_Size(),i_lpcBounds[0].Get_Size(), i_lpcBounds[1].Get_Size());
	XVECTOR cTest = i_cTest_Point - i_lpcBounds[0];
	for (unsigned int uiI = 0; uiI < i_lpcBounds[0].Get_Size() && bRet; uiI++)
	{
		bRet = i_lpcBounds[0].Get(uiI) <= i_lpcBounds[0].Get(uiI);
	}
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds invalid.  Must be a two element array, with index 0 as the lower bound and index 1 as upper bound.\n");
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	cTest = i_lpcBounds[1] - i_cTest_Point;
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	return bRet;
}

XVECTOR xCompute_Centroid(XVECTOR * i_lpvPoints, unsigned int i_uiNum_Points)
{
	XVECTOR cCentroid(i_lpvPoints[0].Get_Size());
	for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
		cCentroid.Set(uiI,0.0);

	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		cCentroid += i_lpvPoints[uiI];
	}
	cCentroid /= (double)(i_uiNum_Points);
	return cCentroid;
}



bool ValidateChi2(const ES::Spectrum &i_cTarget,const double &i_dMin_WL, const double &i_dMax_WL)
{
	bool bChi2 = true;
	unsigned int uiSource_Idx = 0;
	bool	bInvalid_Error = true;
	// Find lower bound for source data
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) <= i_dMax_WL && bChi2)
	{
		bChi2 &= (i_cTarget.flux_error(uiSource_Idx) != 0);
		bInvalid_Error &= (i_cTarget.flux_error(uiSource_Idx) == 0.0 || i_cTarget.flux_error(uiSource_Idx) == 1.0);
		uiSource_Idx++;
	}
	bChi2 &= !bInvalid_Error; // if the flux error is uniformly 0 or 1, don't use it
	return bChi2;
}
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, double * o_dMoments) // moments must be large enough to fit results
{
	memset(o_dMoments,0,sizeof(double) * i_uiNum_Moments);
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		double dXprod = i_dX[uiI];
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_dMoments[uiM] += dXprod;
			dXprod *=  i_dX[uiI];
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_dMoments[uiM] /= i_uiNum_Points;
}
void Get_Central_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dCentral_Moments)
{
	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dCentral_Moments[5] = i_dMoments[5] + ((((-5.0 * i_dMoments[0] * i_dMoments[0] + 15.0 * i_dMoments[1]) * i_dMoments[0] - 20.0 * i_dMoments[2]) * i_dMoments[0] + 15.0 * i_dMoments[3]) * i_dMoments[0] - 6.0 * i_dMoments[4]) * i_dMoments[0];
	case 5:
		o_dCentral_Moments[4] = i_dMoments[4] + (((4.0 * i_dMoments[0] * i_dMoments[0] - 10.0 * i_dMoments[1]) * i_dMoments[0] + 10.0 * i_dMoments[2]) * i_dMoments[0] - 5.0 * i_dMoments[3]) * i_dMoments[0];
	case 4:
		o_dCentral_Moments[3] = i_dMoments[3] + ((-3.0 * i_dMoments[0] * i_dMoments[0]  + 6.0 * i_dMoments[1]) * i_dMoments[0] - 4.0 * i_dMoments[2]) * i_dMoments[0];
	case 3:
		o_dCentral_Moments[2] = i_dMoments[2] + (2.0 * i_dMoments[0] * i_dMoments[0]  - 3.0 * i_dMoments[1]) * i_dMoments[0];
	case 2:
		o_dCentral_Moments[1] = i_dMoments[1] - i_dMoments[0] * i_dMoments[0];
	case 1:
		o_dCentral_Moments[0] = 0.0;
	}
}
void Get_Standardized_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dStandardized_Moments)
{
	double	* lpdCentral_Moments = new double[i_uiNum_Moments];
	Get_Central_Moments(i_uiNum_Moments, i_dMoments,lpdCentral_Moments);

	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dStandardized_Moments[5] = lpdCentral_Moments[5] / pow(lpdCentral_Moments[1],3.0);
	case 5:
		o_dStandardized_Moments[4] = lpdCentral_Moments[4] / pow(lpdCentral_Moments[1],2.5);
	case 4:
		o_dStandardized_Moments[3] = lpdCentral_Moments[3] / pow(lpdCentral_Moments[1],2.0);
	case 3:
		o_dStandardized_Moments[2] = lpdCentral_Moments[2] / pow(lpdCentral_Moments[1],1.5);
	case 2:
		o_dStandardized_Moments[1] = 1.0;
	case 1:
		o_dStandardized_Moments[0] = 0.0;
	}
}



void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments)
{
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);
		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];

		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			lpdErrors[uiI] = dError;
		}

		if (o_lpdRaw_Moments)
			delete [] o_lpdRaw_Moments;
		if (o_lpdCentral_Moments)
			delete [] o_lpdCentral_Moments;
		if (o_lpdStandardized_Moments)
			delete [] o_lpdStandardized_Moments;

		o_lpdRaw_Moments = new double [i_uiNum_Moments];
		o_lpdCentral_Moments = new double [i_uiNum_Moments];
		o_lpdStandardized_Moments = new double [i_uiNum_Moments];

		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
		Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
		Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

		delete [] lpdErrors;
	}
}

void Bracket_Wavelength(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int &o_uiLower_Bound, unsigned int &o_uiUpper_Bound, unsigned int i_uiSuggested_Starting_Idx)
{
	while (i_cData.wl(i_uiSuggested_Starting_Idx) <= i_dTarget_Wavelength && i_uiSuggested_Starting_Idx < i_cData.size())
		i_uiSuggested_Starting_Idx++;
//	if (i_uiSuggested_Starting_Idx == 0)
//		printf("%.1f %.1f\n",i_dTarget_Wavelength,i_cData.wl(i_uiSuggested_Starting_Idx));
//	printf("%i %i\t",i_uiSuggested_Starting_Idx,i_cData.size());
	if (i_uiSuggested_Starting_Idx < i_cData.size())
	{
		if (i_uiSuggested_Starting_Idx != 0)
		{
			o_uiLower_Bound = i_uiSuggested_Starting_Idx - 1;
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
	}
	else
	{
		if (i_cData.wl(i_uiSuggested_Starting_Idx - 1) <= i_dTarget_Wavelength)
		{
			o_uiLower_Bound = i_cData.size() - 1;
			o_uiUpper_Bound = i_cData.size();
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_cData.size();
		}
	}
//	printf("%i %i\n",o_uiLower_Bound,o_uiUpper_Bound);
}

double Interpolate_Flux(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int i_uiLower_Bound, unsigned int i_uiUpper_Bound)
{
	double	dFlux = nan("");
	if (i_uiLower_Bound != i_uiUpper_Bound && i_uiUpper_Bound < i_cData.size())
	{ 
		double dSlope = (i_cData.flux(i_uiLower_Bound + 1) - i_cData.flux(i_uiLower_Bound)) / (i_cData.wl(i_uiLower_Bound + 1) - i_cData.wl(i_uiLower_Bound));
		dFlux = (i_dTarget_Wavelength - i_cData.wl(i_uiLower_Bound)) * dSlope + i_cData.flux(i_uiLower_Bound);
	}
//	else
//		printf("%i %i %i\n",i_uiLower_Bound,i_uiUpper_Bound,i_cData.size());
	return dFlux;
}

void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	double * lpdErrors = new double[uiCompare_Count];
	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			lpdErrors[uiI] = dError;
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	if (o_lpdRaw_Moments)
		delete [] o_lpdRaw_Moments;
	if (o_lpdCentral_Moments)
		delete [] o_lpdCentral_Moments;
	if (o_lpdStandardized_Moments)
		delete [] o_lpdStandardized_Moments;

	o_lpdRaw_Moments = new double [i_uiNum_Moments];
	o_lpdCentral_Moments = new double [i_uiNum_Moments];
	o_lpdStandardized_Moments = new double [i_uiNum_Moments];

	Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
	Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
	Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

	delete [] lpdErrors;
}


double Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, FIT_TYPE i_eFit,unsigned int i_uiFit_Moment)
{
	if (i_uiFit_Moment > 0)
		i_uiFit_Moment--; // use moment is an index, so the 1st moment is index 0

	double dFit = nan("");
	double	*lpdMoments = new double [i_uiFit_Moment];
	double	*lpdExtended_Moments = new double [i_uiFit_Moment];
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);

		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];
		double	dChi2 = 0.0;
		double dErr_Sum = 0.0;
		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			if (i_eFit == FIT_CHI2)
				dChi2 += dError / (i_cTarget.flux_error(uiI + uiSource_Idx) * dSource_Normalization);
			lpdErrors[uiI] = dError;
			dErr_Sum += (dError * dError * dError);
		}
		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiFit_Moment + 1,lpdMoments);
		switch (i_eFit)
		{
		case FIT_CHI2:
			dFit = dChi2;
			break;
		case FIT_RAW:
			dFit = lpdMoments[i_uiFit_Moment];
			break;
		case FIT_CENTRAL:
			Get_Central_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		case FIT_STANDARDIZED:
			Get_Standardized_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		}
		

		delete [] lpdErrors;
	}
	else
		fprintf(stderr,"spectrafit: Mismatched target and generated files.\n");
	delete [] lpdMoments;
	delete [] lpdExtended_Moments;
	return dFit;
}



void	SPECTRAL_DATABASE::Set_Base_Data(unsigned int i_uiIon, const char * i_lpszModel_Name)
{
	if (i_uiIon != 0 && i_uiIon != m_uiIon && ((!i_lpszModel_Name && m_lpszModel_Name && m_lpszModel_Name[0] != 0) || (!m_lpszModel_Name && i_lpszModel_Name && i_lpszModel_Name[0] != 0) || (m_lpszModel_Name && i_lpszModel_Name && strcmp(i_lpszModel_Name,m_lpszModel_Name) != 0)))
		Deallocate(); // there has been a change in the ion or model
	m_uiIon = i_uiIon;
	if (i_lpszModel_Name && i_lpszModel_Name[0] != 0)
	{
		m_lpszModel_Name = new char[strlen(i_lpszModel_Name) + 1];
		strcpy(m_lpszModel_Name,i_lpszModel_Name);
	}
	if (m_lpszModel_Name && m_lpszModel_Name[0] != 0)
	{
		unsigned int i_uiLength = 30 + strlen(m_lpszModel_Name);
		m_lpszDatabase_Filename = new char[i_uiLength];
		sprintf(m_lpszDatabase_Filename,".spectradata.%s.%i.database",m_lpszModel_Name,m_uiIon);
	}
}
void	SPECTRAL_DATABASE::Get_Base_Data(unsigned int &o_uiIon, char * o_lpszModel_Name, unsigned int i_uiMax_Model_Name_Length)
{
	o_uiIon = m_uiIon;
	if (o_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
		o_lpszModel_Name[0] = 0;
	if (o_lpszModel_Name && m_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
	{
		strncpy(o_lpszModel_Name,m_lpszModel_Name,i_uiMax_Model_Name_Length);
	}
}

unsigned int SPECTRAL_DATABASE::Read_Database(void)
{
	FILE * fileIn;
	unsigned int uiFault_Code = 0;
	unsigned int uiStrLen_Model_Name = 0;
	double			* lpdDouble_Buffer = NULL;
	unsigned int	uiDouble_Buffer_Length = 0;
	char * 			lpszModel_Name_Buffer = NULL;
	unsigned int	uiIon = 0;
	unsigned int	uiModel_Name_Length = 0;
	unsigned int	uiParameter_Vector_Length = 0;
	unsigned int	uiNum_Parameter_Vectors = 0;
	unsigned int	uiNum_Tuples = 0;
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiStrLen_Model_Name = strlen(m_lpszModel_Name) + 1;
		if (uiStrLen_Model_Name == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}
	// make sure that there is no data in the database
	Deallocate_Non_Base_Data();
	Zero_Non_Base_Data();

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"rb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiIon != m_uiIon)
			uiFault_Code = 405; // ion mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiModel_Name_Length != uiStrLen_Model_Name) // ensure that the current and saved model names are of the same length
			uiFault_Code = 505; // model name length mismatch
	}

	if (uiFault_Code == 0)
	{
		lpszModel_Name_Buffer = new char [uiModel_Name_Length];
		size_t iRead_Size = fread(lpszModel_Name_Buffer,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (strcmp(lpszModel_Name_Buffer,m_lpszModel_Name) != 0) // make sure the data in the file matches
			uiFault_Code = 515; // model name mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}
	if (uiFault_Code == 0)
	{
		m_lpvParameter_Data = new XVECTOR[uiNum_Parameter_Vectors];
		if (!m_lpvParameter_Data)
			uiFault_Code = 800; // failed to allocate parameter vector data
	}
	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Heads)
			uiFault_Code = 900; // failed to allocate tuple list head array
	}

	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Tails)
			uiFault_Code = 905; // failed to allocate tuple list tail array
	}

	if (uiFault_Code == 0)
	{
		lpdDouble_Buffer = new double[uiParameter_Vector_Length];
		uiDouble_Buffer_Length = uiParameter_Vector_Length;
		if (!lpdDouble_Buffer)
			uiFault_Code = 1000; // failed to allocate temporary buffer for double precision data
	}
	for(unsigned int uiI = 0; uiI < uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		
		m_lpvParameter_Data[uiI].Set_Size(uiParameter_Vector_Length);
		m_lplpcTuple_List_Heads[uiI] = m_lplpcTuple_List_Tails[uiI] = NULL;
		size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiParameter_Vector_Length,fileIn);
		if (iRead_Size < uiParameter_Vector_Length)
			uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			if (uiDouble_Buffer_Length < (uiNum_Tuples * 2))
			{
				if (lpdDouble_Buffer)
					delete [] lpdDouble_Buffer;
				uiDouble_Buffer_Length = uiNum_Tuples * 2;
				lpdDouble_Buffer = new double[uiDouble_Buffer_Length];
				if (!lpdDouble_Buffer)
					uiFault_Code = 300000000 + uiI; // failed to reallocate double buffer during vector i. 
			}
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiNum_Tuples,fileIn);
			if (iRead_Size != uiNum_Tuples)
				uiFault_Code = 400000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			for (unsigned int uiJ = 0; uiJ < uiNum_Tuples && uiFault_Code == 0; uiJ+=2)
			{
				unsigned int uiWL_Idx = uiJ * 2;
				unsigned int uiInt_Idx = uiWL_Idx + 1;
				SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (!lpCurr)
					uiFault_Code = 500000000 + uiI; // failed to allocate a tuple node for vector i
				if (uiFault_Code == 0)
				{
					lpCurr->m_dWavelength = lpdDouble_Buffer[uiWL_Idx];
					lpCurr->m_dIntensity = lpdDouble_Buffer[uiInt_Idx];
					if (m_lplpcTuple_List_Heads[uiI] == NULL)
					{
						m_lplpcTuple_List_Heads[uiI] = lpCurr;
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
					else
					{
						lpCurr->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
				}
			}
		}
	}
	if (uiFault_Code != 0)
	{
		Deallocate();
	}
	// deallocate local temporary storage
	if (lpdDouble_Buffer)
		delete [] lpdDouble_Buffer;
	if (lpszModel_Name_Buffer)
		delete [] lpszModel_Name_Buffer;
	// return fault code
	return uiFault_Code;	
}
unsigned int SPECTRAL_DATABASE::Save_Database(void)
{
	FILE * fileIn = NULL;
	unsigned int uiFault_Code = 0;
	unsigned int uiModel_Name_Length;
	unsigned int uiParameter_Vector_Length;
	
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiModel_Name_Length = strlen(m_lpszModel_Name) + 1;
		if (uiModel_Name_Length == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"wb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(m_lpszModel_Name,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}
	if (uiFault_Code == 0)
	{
		uiParameter_Vector_Length = m_lpvParameter_Data[0].Get_Size();
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}


	if (uiFault_Code == 0)
	{
		if (m_uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	for(unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		for( unsigned int uiJ = 0; uiJ < uiParameter_Vector_Length && uiFault_Code == 0; uiJ++)
		{
			double dBuffer = m_lpvParameter_Data[uiI].Get(uiJ);
			size_t iRead_Size = fwrite(&dBuffer,sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		}
		unsigned int uiNum_Tuples = 0;
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiI];
		// count tuples
		while (lpCurr != NULL)
		{
			uiNum_Tuples++;
			lpCurr = lpCurr->m_lpNext;
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		// go through tuple list, writing each one to file
		lpCurr = m_lplpcTuple_List_Heads[uiI];
		while (lpCurr != NULL && uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&(lpCurr->m_dWavelength),sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 600000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			if (uiFault_Code == 0)
			{
				size_t iRead_Size = fwrite(&(lpCurr->m_dIntensity),sizeof(double),1,fileIn);
				if (iRead_Size != 1)
					uiFault_Code = 700000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			}

			lpCurr = lpCurr->m_lpNext;
		}
		
	}
	if (uiFault_Code != 0)
	{
		if (fileIn)
			fclose(fileIn);
		if (uiFault_Code > 305)
		{
			char lpszCommand[256];
			sprintf(lpszCommand,"rm %s",m_lpszDatabase_Filename);
			system(lpszCommand);
		}
	}
	// return fault code
	return uiFault_Code;	
}


unsigned int		SPECTRAL_DATABASE::Add_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = Find_Parameter_Vector(i_vParameters);
	if (uiRet == -1)
	{// not found, add
		m_uiNum_Parameter_Vectors++;
		XVECTOR			* lpNew_List = new XVECTOR[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		m_uiNum_Parameter_Vectors--;
		if (lpNew_List && lplpcNew_Heads && lplpcNew_Tails)
		{
			for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors; uiI++)
			{
				lpNew_List[uiI] = m_lpvParameter_Data[uiI];
				lplpcNew_Heads[uiI] = m_lplpcTuple_List_Heads[uiI];
				lplpcNew_Tails[uiI] = m_lplpcTuple_List_Tails[uiI];
			}
			lpNew_List[m_uiNum_Parameter_Vectors] = i_vParameters;
			lplpcNew_Heads[m_uiNum_Parameter_Vectors] = NULL;
			lplpcNew_Tails[m_uiNum_Parameter_Vectors] = NULL;
			m_uiNum_Parameter_Vectors++;
			if (m_lpvParameter_Data)
				delete [] m_lpvParameter_Data;
			if (m_lplpcTuple_List_Heads)
				delete [] m_lplpcTuple_List_Heads;
			if (m_lplpcTuple_List_Tails)
				delete [] m_lplpcTuple_List_Tails;

			m_lpvParameter_Data = lpNew_List;
			m_lplpcTuple_List_Heads = lplpcNew_Heads;
			m_lplpcTuple_List_Tails = lplpcNew_Tails;
		}
	}
}
unsigned int		SPECTRAL_DATABASE::Find_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = -1;
	for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiRet == -1; uiI++)
	{
		if (i_vParameters == m_lpvParameter_Data[uiI])
			uiRet = uiI;
	}
	return uiRet;
}

void 	SPECTRAL_DATABASE::Add_Spectrum(const XVECTOR & i_vParameters, const ES::Spectrum &i_cSpectrum)
{
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (uiIdx == -1)
		uiIdx = Add_Parameter_Vector(i_vParameters);
	if (uiIdx != -1)
	{
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
		for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && uiIdx != -1; uiI++)
		{
			while (lpCurr && lpCurr->m_dWavelength <= i_cSpectrum.wl(uiI))
				lpCurr = lpCurr->m_lpNext;
			if (!lpCurr || lpCurr->m_dWavelength != i_cSpectrum.wl(uiI))
			{
				SPECTRAL_DATABASE_TUPLE_NODE * lpNew = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (lpNew)
				{
					lpNew->m_dWavelength = i_cSpectrum.wl(uiI);
					lpNew->m_dIntensity = i_cSpectrum.flux(uiI);
					if (!m_lplpcTuple_List_Heads[uiI])
					{
						m_lplpcTuple_List_Heads[uiI] = lpNew;
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else if (!lpCurr)
					{
						m_lplpcTuple_List_Tails[uiI]->m_lpNext = lpNew;
						lpNew->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else
					{
						lpNew->m_lpNext = lpCurr;
						lpNew->m_lpPrev = lpCurr->m_lpPrev;
						lpCurr->m_lpPrev = lpNew;
						if (lpNew->m_lpPrev)
							lpNew->m_lpPrev->m_lpNext = lpNew;
						if (lpCurr == m_lplpcTuple_List_Heads[uiI]) // if we are pointing at the head, update the head.
							m_lplpcTuple_List_Heads[uiI] = lpNew;
					}
					lpCurr = lpNew; // set the current pointer to the newly added node
				}
				else
					uiIdx = -1;
			}
		}
	}
	if (uiIdx != -1)
	{
		unsigned int uiError = Save_Database();
		if (uiError != 0)
			fprintf(stderr,"Error %i while saving spectral database.\n",uiError);
	}
}


bool 	SPECTRAL_DATABASE::Find_Spectrum(const XVECTOR & i_vParameters, ES::Spectrum & io_cOutput, bool i_bAllow_Interpolate, bool i_bGenerate_Wavelength_List) // allow interpolate flag will return f(wl) interpolated between known nearby wl if the exact wl value is 
{
	bool bSuccess = false;
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (i_bGenerate_Wavelength_List)
		io_cOutput.zero_out();
	else
		io_cOutput.zero_flux();
	if (uiIdx != -1)
	{
		if (i_bGenerate_Wavelength_List)
		{
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			unsigned int uiCount = 0;
			while(lpCurr)
			{
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			io_cOutput = ES::Spectrum::create_from_size(uiCount);

			lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			uiCount = 0;
			while(lpCurr)
			{
				io_cOutput.wl(uiCount) = lpCurr->m_dWavelength;
				io_cOutput.flux(uiCount) = lpCurr->m_dIntensity;
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			bSuccess = true;
		}
		else
		{
			bSuccess = true;
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			for (unsigned int uiI = 0; uiI < io_cOutput.size() && bSuccess; uiI++)
			{
				while (lpCurr && lpCurr->m_dWavelength < io_cOutput.wl(uiI))
					lpCurr = lpCurr->m_lpNext;
				if (lpCurr->m_dWavelength == io_cOutput.wl(uiI))
					io_cOutput.flux(uiI) = lpCurr->m_dIntensity;
				else if (i_bAllow_Interpolate && lpCurr && lpCurr->m_lpPrev)
				{
					// @@TODO more interpolation techniques
					double dSlope = (lpCurr->m_dIntensity - lpCurr->m_lpPrev->m_dIntensity) / (lpCurr->m_dWavelength - lpCurr->m_lpPrev->m_dWavelength);
					double	dX = (io_cOutput.wl(uiI) - lpCurr->m_lpPrev->m_dWavelength);
					io_cOutput.flux(uiI) = dX * dSlope + lpCurr->m_lpPrev->m_dIntensity;
				}
				else
					bSuccess = false;
			}
		}
	}
	return bSuccess;
}


void Get_Spectra_Data(const ES::Spectrum & i_cSpectrum, double * &o_lpdWavelengths, double * & o_lpdFluxes, unsigned int & o_uiNum_Points, const double & i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	unsigned int uiCount = 0;
	while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
		uiStart_Idx++;
	while ((uiStart_Idx + uiCount) < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx + uiCount) < i_dMax_WL)
		uiCount++;
	o_uiNum_Points = uiCount;
	if (o_lpdWavelengths)
		delete [] o_lpdWavelengths;
	if (o_lpdFluxes)
		delete [] o_lpdFluxes;
	o_lpdWavelengths = new double[uiCount];
	o_lpdFluxes = new double[uiCount];
	uiCount = 0;
	while ((uiStart_Idx + uiCount) < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx + uiCount) < i_dMax_WL)
	{
		o_lpdWavelengths[uiCount] = i_cSpectrum.wl(uiStart_Idx + uiCount);
		o_lpdFluxes[uiCount] = i_cSpectrum.flux(uiStart_Idx + uiCount);
		uiCount++;
	}
}

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

