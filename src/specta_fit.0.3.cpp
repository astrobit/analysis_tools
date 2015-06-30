
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
#include <plplot/plplot.h>
#include <plplot/plstream.h>

#define VERSION "0.3"
void Usage(const char * i_lpszError_Text)
{
		fprintf(stderr,"spectrafit v%s\n",VERSION);
		if (i_lpszError_Text)
			fprintf(stderr,"\nError: %s\n\n",i_lpszError_Text);
		fprintf(stderr,"Usage: \n\tspectrafit <Source_spectrum> <Fit_region> <Test_spectra>\n");
		fprintf(stderr,"Source_spectrum: file containing wavelength, flux, and flux error.\n");
		fprintf(stderr,"\tMust be space, tab, or column separated.  If flux error is zero,\n");
		fprintf(stderr,"\tvariance will be used instead of chi^2.\n");
		fprintf(stderr,"Fit_region: Line for which to do the fit.  Options are 'CaNIR', 'CaHK',\n");
		fprintf(stderr,"\tor 'Si6355'.\n");
//		fprintf(stderr,"\tor 'Si6355' or 'Full'.\n");  // Full currently disabled
		fprintf(stderr,"Test_spectra: List of opactiy map(s) to use.\n");
		fprintf(stderr,"\n");
		fprintf(stderr,"Output: name of opacity map with best fit, as well as the chi^2 or variance\n");
		fprintf(stderr,"\tof the fit, the day number, photosphere temperature, ion excitation\n");
		fprintf(stderr,"\tand the log opacity reference used.\n");
}

bool ValidateChi2(ES::Spectrum &i_cTarget,const double &i_dMin_WL, const double &i_dMax_WL)
{
	bool bChi2 = true;
	unsigned int uiSource_Idx = 0;
	// Find lower bound for source data
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) <= i_dMax_WL && bChi2)
	{
		bChi2 &= (i_cTarget.flux_error(uiSource_Idx) != 0);
		uiSource_Idx++;
	}
	return bChi2;
}

double Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, bool i_bChi2)
{
	double dFit = -1;
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		double dSource_Normalization = 1.0 / i_cTarget.wl(uiSource_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.wl(uiSource_Idx);
//		printf("Source test %i %.2e %.2e\n",uiSource_Idx,dSource_Normalization,dGenerated_Normalization);
		dFit = 0.0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) <= i_dMax_WL)
		{
			double dTest_Flux = i_cGenerated.flux(uiSource_Idx) * dGenerated_Normalization;
			double dSource_Flux = i_cTarget.flux(uiSource_Idx) * dSource_Normalization;
			double dDelta_Flux = dSource_Flux - dTest_Flux;
			if (i_bChi2)
				dDelta_Flux /= (i_cTarget.flux_error(uiSource_Idx) * dSource_Normalization);
			dFit += (dDelta_Flux * dDelta_Flux);
			uiSource_Idx++;
		}
	}
	else
		fprintf(stderr,"spectrafit: Mismatched target and generated files.\n");
	return dFit;
}

unsigned int g_uiVelocity_Map_Alloc_Size = 0;
double		* g_lpdVelocities = NULL;
double		* g_lpdOpacity_Profile[2] = {NULL,NULL};
ES::Synow::Grid * g_lpcGrid = NULL;
ES::Synow::Opacity * g_lpcOpacity = NULL;
ES::Spectrum * g_lpcReference = NULL;
ES::Synow::Source * g_lpcSource = NULL;
ES::Synow::Spectrum * g_lpcSpectrum = NULL;
bool		g_bMin_Delta_Init = false;
double		g_dMin_Delta = DBL_MAX;

void Generate_Synow_Spectra(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput)
{
//    o_cOutput = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());
//    ES::Spectrum output = ES::Spectrum::create_from_spectrum( o_cOutput );
    ES::Spectrum reference = ES::Spectrum::create_from_spectrum( i_cTarget );

//	printf("I: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
//	printf("%.3e\n",o_cOutput.flux(100));

	double	dVmax = i_cOpacity_Map_A.GetElement(0,i_cOpacity_Map_A.GetNumElements() - 1) * 1.0e-8;
    // Grid object.
	
	for (unsigned int uiI = 1; uiI < i_cTarget.size() && !g_bMin_Delta_Init; uiI++)
	{
		if ((i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1)) < g_dMin_Delta)
			g_dMin_Delta = i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1);
	}

	if (i_cOpacity_Map_A.GetNumElements() - 1 > g_uiVelocity_Map_Alloc_Size)
	{
		ES::ERROR::Inhibit_Error_Reporting();

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

		g_lpdVelocities = new double[i_cOpacity_Map_A.GetNumElements() - 1];
		g_lpdOpacity_Profile[0] = new double[i_cOpacity_Map_A.GetNumElements() - 1];
		g_lpdOpacity_Profile[1] = new double[i_cOpacity_Map_A.GetNumElements() - 1];
		g_uiVelocity_Map_Alloc_Size = i_cOpacity_Map_A.GetNumElements() - 1;
//		printf("%i\n",i_cOpacity_Map_A.GetNumElements());
		g_lpcGrid = new ES::Synow::Grid(ES::Synow::Grid::create( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, i_cOpacity_Map_A.GetNumElements() - 1, g_lpdVelocities));
//		g_lpcGrid = new ES::Synow::Grid(ES::Synow::Grid::create( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, 2048, g_lpdVelocities));
		g_lpcOpacity = new ES::Synow::Opacity( g_lpcGrid[0], getenv("SYNXX_LINES_DATA_PATH"), getenv("SYNXX_REF_LINE_DATA_PATH"), "user_profile", 10.0, -3.0 );
		g_lpcReference = new ES::Spectrum( o_cOutput );
		g_lpcSource = new ES::Synow::Source (g_lpcGrid[0], 10.0);
		g_lpcSpectrum = new ES::Synow::Spectrum( g_lpcGrid[0], o_cOutput, g_lpcReference[0], 60, false);
	}

	for (unsigned int uiI = 1; uiI < i_cOpacity_Map_A.GetNumElements(); uiI++)
	{
		g_lpdVelocities[uiI - 1] = i_cOpacity_Map_A.GetElement(0,uiI) * 1e-8;
	}


    // Attach setups one by one.
		// Initialize Ion data
    ES::Synow::Setup cSetup;

	int iT_ref = (int )(i_cParameters.Get(0) + 0.5);
	if (iT_ref < 0)
		iT_ref = 0;
	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
	iT_ref++;

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
		cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(iT_ref,uiI + 1);
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
			cSetup.user_profile[1][uiI] = i_cOpacity_Map_B.GetElement(iT_ref,uiI + 1);
		}
	}

	g_lpcGrid->reset(cSetup);
    g_lpcGrid[0]( cSetup );
//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
//	o_cOutput = output;
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

//	printf("%.3e\n",o_cOutput.flux(100));
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

}

bool Convergence_Point(const XVECTOR &i_lpVector, const XVECTOR &i_cThreshold)
{
	XVECTOR cTest;
	bool bRet = (i_cThreshold.Get_Size() == i_lpVector.Get_Size());
	if (!bRet)
		fprintf(stderr,"Convergence threshold size not the same as paramter vector size!\n");

	for (unsigned int uiK = 0; uiK < i_lpVector.Get_Size() && bRet; uiK++)
	{
		bRet = (fabs(i_lpVector.Get(uiK)) < i_cThreshold.Get(uiK));
	}
	return bRet;
}

bool Convergence(const XVECTOR * i_lpVectors, unsigned int i_uiNum_Points,const XVECTOR &i_cThreshold, XVECTOR * o_lpcConvergence_Fault = NULL)
{
	XVECTOR cTest;
	bool bRet = true;
	for(unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
		bRet &= (i_cThreshold.Get_Size() == i_lpVectors[uiI].Get_Size());
	if (!bRet)
		fprintf(stderr,"Convergence threshold size not the same as paramter vector size!\n");

	for(unsigned int uiI = 0; uiI < i_uiNum_Points && bRet; uiI++)
	{
		for (unsigned int uiJ = uiI + 1; uiJ < i_uiNum_Points && bRet; uiJ++)
		{
			cTest = i_lpVectors[uiJ] - i_lpVectors[uiI];
			for (unsigned int uiK = 0; uiK < cTest.Get_Size() && bRet; uiK++)
			{
				bRet = (fabs(cTest.Get(uiK)) < i_cThreshold.Get(uiK));
				if (!bRet && o_lpcConvergence_Fault)
				{
					o_lpcConvergence_Fault[0] = cTest;
				}
			}
		}
	}
	return bRet;
}
bool BoundTest(const XVECTOR & i_cTest_Point, const XVECTOR * i_lpcBounds)
{
	bool bRet;
	bRet = (i_cTest_Point.Get_Size() == i_lpcBounds[0].Get_Size() && i_cTest_Point.Get_Size() == i_lpcBounds[1].Get_Size());
	if (!bRet)
		fprintf(stderr,"Bounds size not the same as test point size!\n");
	XVECTOR cTest = i_cTest_Point - i_lpcBounds[0];
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

void SavePointsCache(const char * i_lpszFileSource, const XVECTOR * lpvPoints, unsigned int uiNum_Points, unsigned int uiNum_Parameters, const double * lpdFit)
{
	XDATASET	cDataset;
	char lpszFilename[256];
	sprintf(lpszFilename,"spectrafit.%s.cache",i_lpszFileSource);
	cDataset.Allocate(uiNum_Parameters + 1,uiNum_Points);
	for (unsigned int uiI = 0; uiI < uiNum_Parameters; uiI++)
		for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			cDataset.SetElement(uiI,uiJ,lpvPoints[uiJ].Get(uiI));
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
		cDataset.SetElement(uiNum_Parameters,uiJ,lpdFit[uiJ]);
	cDataset.SaveDataFileBin(lpszFilename,true);
}
bool ReadPointsCache(const char * i_lpszFileSource, XVECTOR * lpvPoints, unsigned int uiNum_Points, unsigned int uiNum_Parameters, double * lpdFit)
{
	XDATASET	cDataset;
	bool	bRet = false;
	char lpszFilename[256];
	sprintf(lpszFilename,"spectrafit.%s.cache",i_lpszFileSource);
	cDataset.ReadDataFileBin(lpszFilename,true);
	if (uiNum_Points == cDataset.GetNumElements() && (uiNum_Parameters + 1) == cDataset.GetNumColumns())
	{
		bRet = true;
		for (unsigned int uiI = 0; uiI < uiNum_Parameters; uiI++)
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				//printf("%.1f\t",cDataset.GetElement(uiI,uiJ));
				lpvPoints[uiJ].Set(uiI,cDataset.GetElement(uiI,uiJ));
			}
		for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			lpdFit[uiJ] = cDataset.GetElement(uiNum_Parameters,uiJ);
	}
	return bRet;
}
double CompareFits(const char * i_lpszFileSource, const ES::Spectrum &i_cTarget, XDATASET & i_cOpacity_Map_A, XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const double &i_dMin_WL, const double &i_dMax_WL, bool i_bChi2, double &o_dDay, double & o_dPS_Vel, double &o_dPS_Temp, double & o_dPS_Ion_Temp, double &o_dPS_Log_Tau, double & o_dHVF_Ion_Temp, double &o_dHVF_Log_Tau, ES::Spectrum &o_cOutput)
{
	// use the following define to set the numberr of dimensions to test - value should be # dim + 1 (e.g. 3-d = 4 points)
#define NUM_PARAMETERS 7
	unsigned int uiParameters = NUM_PARAMETERS;
	ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());

	double	dFit_Min = DBL_MAX;
	double	*lpdFit;
	// define tetrahedron spanning the search space
	XVECTOR *lpcTest_Points;
	XVECTOR cBounds[2] = {XVECTOR(NUM_PARAMETERS),XVECTOR(NUM_PARAMETERS)};
	XVECTOR cThreshold(NUM_PARAMETERS);
	XVECTOR cCentroid(NUM_PARAMETERS);
	XVECTOR cTestOuter(NUM_PARAMETERS);
	XVECTOR cTestInner(NUM_PARAMETERS);
	XVECTOR cTest(NUM_PARAMETERS);
	XVECTOR cDelta(NUM_PARAMETERS);
	XVECTOR cConvergence_Fault(NUM_PARAMETERS);
	double dTest_Fit = 0.0;
	unsigned int uiNum_Vertices;

	cBounds[0].Set(0,0.0); // time
	cBounds[0].Set(1,5.0); // PS velocity (kkm/s)
	cBounds[0].Set(2,5.0); // PS temp (kK)
	cBounds[0].Set(3,1.0); // PS ion temp (kK)
	cBounds[0].Set(4,-10.0); // PS ion log opacity scaling
	cBounds[0].Set(5,1.0); // HVF ion temp (kK)
	cBounds[0].Set(6,-10.0); // HVF ion log opacity scaling

	cBounds[1].Set(0,i_cOpacity_Map_A.GetNumColumns() - 2); // time
	cBounds[1].Set(1,40.0); // PS velocity (kkm/s)
	cBounds[1].Set(2,30.0); // PS temp (kK)
	cBounds[1].Set(3,30.0); // PS ion temp (kK)
	cBounds[1].Set(4,10.0); // PS ion log opacity scaling
	cBounds[1].Set(5,30.0); // HVF ion temp (kK)
	cBounds[1].Set(6,-8.0); // HVF ion log opacity scaling

	cThreshold.Set(0,1.0);
	cThreshold.Set(1,0.5);
	cThreshold.Set(2,0.25);
	cThreshold.Set(3,0.5);
	cThreshold.Set(4,0.1);
	cThreshold.Set(5,0.5);
	cThreshold.Set(6,0.1);
	cThreshold.Set(7,0.5);

	if (i_cOpacity_Map_B.GetNumElements() == 0)
		uiParameters = 6; // don't look for HVF component separately

	uiNum_Vertices = 1 << uiParameters;
	lpcTest_Points = new XVECTOR[uiNum_Vertices];
	lpdFit = new double[uiNum_Vertices];

	for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
		lpcTest_Points[uiI].Set_Size(NUM_PARAMETERS);

	double	dFit_Sum = 0.0;
	if (!ReadPointsCache(i_lpszFileSource,lpcTest_Points,uiNum_Vertices,uiParameters,lpdFit))
	{
		// create a cuboid that spans the search space
		for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
		{
			lpcTest_Points[uiI] = cBounds[0];
			for (unsigned int uiJ = 0; uiJ < uiParameters; uiJ++)
			{
				if (uiI & (1 << uiJ)) 
					lpcTest_Points[uiI].Set(uiJ,cBounds[1].Get(uiJ));
			}
		}
		for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
		{
			cCentroid += lpcTest_Points[uiI];
			dFit_Sum ++;
		}
		cCentroid /= dFit_Sum;
//		printf("Centroid\n");
	//	cCentroid.Print();
	
		// Make sure the centroid produces a valid spectrum - if not, randomly generate a point within the search space to act as the centroid for the time being...
		// This centroid is going to be used to adjust edges of the cuboid to have a valid fit
		double dCentroid_Fit = 0.0;
		do
		{
			fprintf(stdout,".");
			fflush(stdout);
			if (isnan(dCentroid_Fit) || isinf(dCentroid_Fit)) // need a different point - try random point within the grid
			{
				cDelta = cBounds[1] - cBounds[0];
				for (unsigned int uiI = 0; uiI < NUM_PARAMETERS; uiI++)
				{
					cCentroid.Set(uiI,cBounds[0].Get(uiI) + xrand_d() * cDelta.Get(uiI));
				}
			}
			Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cCentroid,cOutput);
			dCentroid_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
		}
		while (isnan(dCentroid_Fit) || isinf(dCentroid_Fit));
	
		for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
		{
			fprintf(stdout,"-");
			fflush(stdout);
			Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,lpcTest_Points[uiI],cOutput);
			lpdFit[uiI] = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
			cTestOuter = lpcTest_Points[uiI];
			cTest = lpcTest_Points[uiI];
			while(isnan(lpdFit[uiI]) || isinf(lpdFit[uiI]))
			{
				fprintf(stdout,"_");
				fflush(stdout);
				XVECTOR cDelta = cTestOuter - cCentroid;
				cDelta *= 0.5;
				cTest = cCentroid + cDelta;
	//			cTest.Print();
				Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cTest,cOutput);
				lpdFit[uiI] = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
				cTestOuter = cTest;
			}
			lpcTest_Points[uiI] = cTest;
		}

		SavePointsCache(i_lpszFileSource,lpcTest_Points,uiNum_Vertices,uiParameters,lpdFit);
	}
	unsigned int uiFit_Max_Point = uiNum_Vertices;
	unsigned int uiFit_Min_Point = uiNum_Vertices;
	
	while (!Convergence(lpcTest_Points, uiNum_Vertices , cThreshold, &cConvergence_Fault))
	{
		fprintf(stdout,"+");
		fflush(stdout);
//		fprintf(stdout,"F: %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",cConvergence_Fault.Get(0),cConvergence_Fault.Get(1),cConvergence_Fault.Get(2),cConvergence_Fault.Get(3),cConvergence_Fault.Get(4),cConvergence_Fault.Get(5),cConvergence_Fault.Get(6));

		// Find point with worst fit
		double	dFit_Max = 0.0;
		double	dFit_Near_Max = 0.0;
		double	dFit_Min = DBL_MAX;
		double	dFit_Variance = 0.0;
		double	dFit_Mean = 0.0;
		bool	bClose_Fit = true;
		uiFit_Max_Point = uiNum_Vertices;
		uiFit_Min_Point = uiNum_Vertices;
		for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
		{
//			printf("%i : %.2e\n",uiI,lpdFit[uiI]);
			dFit_Variance += lpdFit[uiI] * lpdFit[uiI];
			dFit_Mean += lpdFit[uiI];
			if (lpdFit[uiI] > dFit_Max)
			{
				dFit_Max = lpdFit[uiI];
				uiFit_Max_Point = uiI;
			}
			if (lpdFit[uiI] < dFit_Min)
			{
				dFit_Min = lpdFit[uiI];
				uiFit_Min_Point = uiI;
			}
		}
		if (dFit_Min <= 0.5)
		{
			for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
			{
				lpdFit[uiI] = lpdFit[uiFit_Min_Point];
				lpcTest_Points[uiI] = lpcTest_Points[uiFit_Min_Point];
			}
		}
		else
		{
			dFit_Variance /= uiNum_Vertices;
			dFit_Mean /= uiNum_Vertices;
			double dFit_Std_Dev = dFit_Variance - dFit_Mean * dFit_Mean;
//			for (unsigned int uiI = 0; uiI < uiNum_Vertices && bClose_Fit; uiI++)
//			{
//				bClose_Fit = (fabs(lpdFit[uiI] - dFit_Mean) < (2.0 * dFit_Std_Dev));
//			}
//			bClose_Fit |= (dFit_Std_Dev < 10.0);
			bClose_Fit &= (dFit_Std_Dev < 10.0);
			if (dFit_Near_Max == 0.0)
			{
				for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
				{
					if (lpdFit[uiI] > dFit_Near_Max && uiI != uiFit_Max_Point)
						dFit_Near_Max = lpdFit[uiI];
				}
			}
	//		fprintf(stdout,"%i",uiFit_Max_Point);
	//		fprintf(stdout,"F: %.8e %.8e %.8e %.8e %.8e %.8e %.8e\n",dFit[0],dFit[1],dFit[2],dFit[3],dFit[4],dFit[5],dFit[6],dFit[7]);
	//		if (uiFit_Max_Point < uiNum_Vertices)
			{
				// Compute weighted centroid of remaining parameters
				for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
					cCentroid.Set(uiI,0.0);
				dFit_Sum = 0.0;
				for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
				{
					if (uiI != uiFit_Max_Point)
					{
						XVECTOR cTemp = lpcTest_Points[uiI];
	//					lpcTest_Points[uiI].Print(NULL);
	//					cTemp.Print(NULL);
						if (lpdFit[uiI] > 1.0)
							cTemp *= lpdFit[uiI];
						cCentroid += cTemp;
	//					cCentroid.Print(NULL);
						if (lpdFit[uiI] > 1.0)
							dFit_Sum += lpdFit[uiI];
						else
							dFit_Sum += 1.0;
					}
				}
				cCentroid /= dFit_Sum;
	//			printf("Centroid\n");
	//			for (unsigned int uiI = 0; uiI < uiParameters; uiI++)
	//			{
	//				printf("%.1f\n",cCentroid.Get(uiI));
	//			}
				cTestOuter = lpcTest_Points[uiFit_Max_Point];
				cTestInner = cCentroid;
//				printf("\n");
//				cCentroid.Print();
				Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cCentroid,cOutput);
				dTest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
				if (bClose_Fit && !isnan(dTest_Fit) && !isinf(dTest_Fit))
				{
					for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
					{
//						fprintf(stdout,"-%.0f\t%.2e\t%.0f\t%.0f\t%c\t%.1f\n",dTest_Fit,dFit_Min,dFit_Max,dFit_Near_Max,bClose_Fit ? 't' : 'f',dFit_Std_Dev);
						fprintf(stdout,"-");
						fflush(stdout);
						cDelta = cCentroid - lpcTest_Points[uiI];
						cDelta *= 0.5;
						lpcTest_Points[uiI] += cDelta;
						Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,lpcTest_Points[uiI],cOutput);
						lpdFit[uiI] = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
					}
				}
				else if (dTest_Fit > dFit_Near_Max)
				{
					cTestOuter = cCentroid;
					cTestInner = lpcTest_Points[uiFit_Min_Point];
					cTest = cTestInner;
					while (isnan(dTest_Fit) || isinf(dTest_Fit) || dTest_Fit > dFit_Near_Max)
					{
						//cCentroid.Print();
//						fprintf(stdout,"@%.0f\t%.2e\t%.0f\t%.0f\t%c\t%.1f\n",dTest_Fit,dFit_Min,dFit_Max,dFit_Near_Max,bClose_Fit ? 't' : 'f',dFit_Std_Dev);
						fprintf(stdout,"@");
						fflush(stdout);
						cDelta = cTestOuter - cTestInner;
						cDelta *= 0.5;
						cTest = cTestInner + cDelta;
//						cTest.Print();
						Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cTest,cOutput);
						dTest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
						cTestOuter = cTest;
/*						lpcTest_Points[uiFit_Max_Point] = cCentroid;
						lpdFit[uiFit_Max_Point] = dTest_Fit;
						for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
							cCentroid.Set(uiI,0.0);
						dFit_Sum = 0.0;
						for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
						{
							XVECTOR cTemp = lpcTest_Points[uiI];
							if (lpdFit[uiI] > 1.0)
								cTemp *= lpdFit[uiI];
							cCentroid += cTemp;
							if (lpdFit[uiI] > 1.0)
								dFit_Sum += lpdFit[uiI];
							else
								dFit_Sum == 1.0;
						}
						cCentroid /= dFit_Sum;
						Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cCentroid,cOutput);
						dTest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
*/
					}
					lpcTest_Points[uiFit_Max_Point] = cTest;
					lpdFit[uiFit_Max_Point] = dTest_Fit;
				}
				else
				{
					cTest = lpcTest_Points[uiFit_Max_Point];
					double dTest_Fit = 0.0;
					while(isnan(dTest_Fit) || isinf(dTest_Fit) || (!bClose_Fit && dTest_Fit < dFit_Min))
					{
//						fprintf(stdout,"*%.0f\t%.2e\t%.0f\t%.0f\t%c\t%.1f\n",dTest_Fit,dFit_Min,dFit_Max,dFit_Near_Max,bClose_Fit ? 't' : 'f',dFit_Std_Dev);
						fprintf(stdout,"*");
						fflush(stdout);
						cDelta = cTestOuter - cTestInner;
						cDelta *= 0.5;
						cTest = cTestInner + cDelta;
//						cTest.Print();
						Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cTest,cOutput);
						dTest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
						cTestInner = cTest;
					}
					cTestOuter = cTest;
					cTestInner = cCentroid;
					while(isnan(dTest_Fit) || isinf(dTest_Fit) || dTest_Fit == 0.0 || dTest_Fit > dFit_Near_Max)
					{
//						fprintf(stdout,"%%%.0f\t%.2e\t%.0f\t%.0f\t%c\t%.1f\n",dTest_Fit,dFit_Min,dFit_Max,dFit_Near_Max,bClose_Fit ? 't' : 'f',dFit_Std_Dev);
						fprintf(stdout,"%%");
						fflush(stdout);
						cDelta = cTestOuter - cTestInner;
						cDelta *= 0.5;
						cTest = cTestInner + cDelta;
//						cTest.Print();
						Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cTest,cOutput);
						dTest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
						cTestOuter = cTest;
					}
					lpcTest_Points[uiFit_Max_Point] = cTest;
					lpdFit[uiFit_Max_Point] = dTest_Fit;
				}
			}
		}
		SavePointsCache(i_lpszFileSource,lpcTest_Points,uiNum_Vertices,uiParameters,lpdFit);
	}

//	printf("Convergence\n");
//	for (unsigned int uiI = 0; uiI < uiParameters; uiI++)
//	{
//		printf("%.1f\n",cConvergence_Fault.Get(uiI));
//	}
	for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
		cCentroid.Set(uiI,0.0);
	for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
	{
		cCentroid += lpcTest_Points[uiI];
	}
	cCentroid /= (double)(uiNum_Vertices);

	int iT_ref = (int )(cCentroid.Get(0) + 0.5);
	if (iT_ref < 0)
		iT_ref = 0;
	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
	iT_ref++;

	o_dDay = i_cOpacity_Map_A.GetElement(iT_ref,0);
//	printf("%i %.2f\n",iT_ref,o_dDay);
	o_dPS_Vel = cCentroid.Get(1);
	o_dPS_Temp = cCentroid.Get(2);
	o_dPS_Ion_Temp = cCentroid.Get(3);
	o_dPS_Log_Tau = cCentroid.Get(4);
	o_dHVF_Ion_Temp = cCentroid.Get(5);
	o_dHVF_Log_Tau = cCentroid.Get(6);

	fprintf(stdout,"X");
	fflush(stdout);
	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cCentroid,cOutput);
	o_cOutput = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
//	system("rm spectrafit.cache");
	return Fit(i_cTarget, o_cOutput, i_dMin_WL, i_dMax_WL, i_bChi2);
}



plstream * Create_Plot(const char * lpszFilename, const double &i_dXmin, const double & i_dXmax, const double & i_dYmin, const double & i_dYmax, const char * i_lpszX_label, const char * i_lpszY_label, const char * i_lpszTitle )
{
	plstream * lpcPLStream = new plstream;
	if (lpcPLStream)
	{
		lpcPLStream->scolbg(255,255,255); // set background color to white
		lpcPLStream->sdev("epscairo"); // select encapsulated postscript with color
		lpcPLStream->sfnam(lpszFilename); // set output filename
		lpcPLStream->sori(1);// supposed to be portrait, but comes out landscape	


		lpcPLStream->init();
		lpcPLStream->scol0(15,0,0,0); // since black was redefined to white, redefine white to black
		lpcPLStream->col0(15); // use black for axes
		lpcPLStream->env(i_dXmin,i_dXmax,i_dYmin,i_dYmax,0.0,0.0);

		lpcPLStream->lab(i_lpszX_label, i_lpszY_label, i_lpszTitle);
	}
	return lpcPLStream;
}

enum OPERATION	{op_none,op_log,op_mult};

void Plot(ES::Spectrum & i_cTarget, ES::Spectrum & i_cFit, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle)
{
	double	dMin_Flux = 1e30;
	double	dMax_Flux = -1e30;
	unsigned int uiTarget_Element_Count = 0;
	unsigned int uiFit_Element_Count = 0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		double	dFlux = i_cTarget.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiTarget_Element_Count++;
		}
	}
	for (unsigned int uiI = 0; uiI < i_cFit.size(); uiI++)
	{
		double dWL = i_cFit.wl(uiI);
		double	dFlux = i_cFit.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiFit_Element_Count++;
		}
	}
	double	*lpdTarget_WL = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux_Error_Upper = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux_Error_Lower = new double[uiTarget_Element_Count];
	double	*lpdFit_WL = new double[uiFit_Element_Count];
	double	*lpdFit_Flux = new double[uiFit_Element_Count];
	uiTarget_Element_Count = 0; // use it as an index
	double	dTarget_Normalization = 0.0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		double	dFlux = i_cTarget.flux(uiI);
		double	dFlux_Error = i_cTarget.flux_error(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			if (dTarget_Normalization == 0.0)
				dTarget_Normalization = dFlux;
			lpdTarget_WL[uiTarget_Element_Count] = dWL;
			lpdTarget_Flux[uiTarget_Element_Count] = log10(dFlux / dTarget_Normalization);
			lpdTarget_Flux_Error_Upper[uiTarget_Element_Count] = log10((dFlux + dFlux_Error) / dTarget_Normalization);
			double dFlux_Error_Lower = (dFlux - dFlux_Error) / dTarget_Normalization;
			if (dFlux_Error_Lower > 1e-10)
				lpdTarget_Flux_Error_Lower[uiTarget_Element_Count] = log10(dFlux_Error_Lower);
			else
				lpdTarget_Flux_Error_Lower[uiTarget_Element_Count] = -10.0;

			uiTarget_Element_Count++;
		}
	}
	uiFit_Element_Count = 0; // use it as an index
	double	dFit_Normalization = 0.0;
	for (unsigned int uiI = 0; uiI < i_cFit.size(); uiI++)
	{
		double dWL = i_cFit.wl(uiI);
		double	dFlux = i_cFit.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			if (dFit_Normalization == 0.0)
				dFit_Normalization = dFlux;
			lpdFit_WL[uiFit_Element_Count] = dWL;
			lpdFit_Flux[uiFit_Element_Count] = log10(dFlux / dFit_Normalization);

			uiFit_Element_Count++;
		}
	}

	for (unsigned int uiI = 0; uiI < uiTarget_Element_Count; uiI++)
	{
		if (lpdTarget_Flux[uiI] < dMin_Flux)
			dMin_Flux = lpdTarget_Flux[uiI];
		if (lpdTarget_Flux[uiI] > dMax_Flux)
			dMax_Flux = lpdTarget_Flux[uiI];

		if (lpdTarget_Flux_Error_Upper[uiI] < dMin_Flux)
			dMin_Flux = lpdTarget_Flux_Error_Upper[uiI];
		if (lpdTarget_Flux_Error_Upper[uiI] > dMax_Flux)
			dMax_Flux = lpdTarget_Flux_Error_Upper[uiI];

		if (lpdTarget_Flux_Error_Lower[uiI] > -10.0)
		{
			if (lpdTarget_Flux_Error_Lower[uiI] < dMin_Flux)
				dMin_Flux = lpdTarget_Flux_Error_Lower[uiI];
			if (lpdTarget_Flux_Error_Lower[uiI] > dMax_Flux)
				dMax_Flux = lpdTarget_Flux_Error_Lower[uiI];
		}
	}
	for (unsigned int uiI = 0; uiI < uiFit_Element_Count; uiI++)
	{
		if (lpdFit_Flux[uiI] < dMin_Flux)
			dMin_Flux = lpdFit_Flux[uiI];
		if (lpdFit_Flux[uiI] > dMax_Flux)
			dMax_Flux = lpdFit_Flux[uiI];
	}

	plstream * lpcPLStream = Create_Plot(i_lpszOutput_Filename, i_dMin_WL,i_dMax_WL,dMin_Flux,dMax_Flux, "Wavelength [Angstroms]", "Log Flux", i_lpszTitle );
	lpcPLStream->col0(7); // grey
	lpcPLStream->lsty(1);
	lpcPLStream->line(uiTarget_Element_Count, lpdTarget_WL, lpdTarget_Flux_Error_Upper);
	lpcPLStream->line(uiTarget_Element_Count, lpdTarget_WL, lpdTarget_Flux_Error_Lower);
	lpcPLStream->col0(15); // black
	lpcPLStream->line(uiTarget_Element_Count, lpdTarget_WL, lpdTarget_Flux);
	lpcPLStream->col0(1); // red
	lpcPLStream->line(uiFit_Element_Count, lpdFit_WL, lpdFit_Flux);
	delete lpcPLStream;
}


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	xrand_Set_Type(XRT_K);
	xrsrand();

	char	lpszFilename[64];
	char	lpszBuffer[1024];
	XDATASET cDataSource;
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	char	chSeparator;
	if (i_iArg_Count < 4)
	{
		Usage("Missing parameters");
	}
	else
	{
		// generate the target spectrum
        ES::Spectrum cTarget = ES::Spectrum::create_from_ascii_file( i_lpszArg_Values[1] );
		double	dMin_WL,dMax_WL;
		unsigned int uiIon;


		// default to full range

		if (strcmp(i_lpszArg_Values[2],"CaNIR") == 0)
		{
			uiIon = 2001;
			dMin_WL = 7000.0;
			dMax_WL = 8300.0;
		}
		else if (strcmp(i_lpszArg_Values[2],"CaHK") == 0)
		{
			uiIon = 2001;
			dMin_WL = 3000.0;
			dMax_WL = 4300.0;
		}
		else if (strcmp(i_lpszArg_Values[2],"Si6355") == 0)
		{
			uiIon = 1401;
			dMin_WL = 5200.0;
			dMax_WL = 6500.0;
		}
		else //if (strcmp(i_lpszArg_Values[2],"Full") != 0)
		{
			uiIon = 0;
			dMin_WL = -1;
			dMax_WL = -1;
		}
//		else
//		{
//			uiIon = 2001; // need to decide how to handle this case
//			dMin_WL = cTarget.wl(0);
//			dMax_WL = cTarget.wl(target.size() - 1);
//		}
		if (dMin_WL < cTarget.wl(0) || dMax_WL > cTarget.wl(cTarget.size() - 1))
		{
			Usage("Source spectrum does not cover desired range");
		}
		else if (dMin_WL > 0 && dMax_WL > 0)
		{
			ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
			ES::Spectrum cBest_Fit_Output  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
			bool bChi2 = ValidateChi2(cTarget,dMin_WL, dMax_WL);
			double	dBest_Fit = DBL_MAX;
			double	dBest_Fit_PS_Ion_Temp, dBest_Fit_PS_Temp, dBest_Fit_Day, dBest_Fit_PS_Log_Tau, dBest_Fit_HVF_Ion_Temp, dBest_Fit_PS_Vel, dBest_Fit_HVF_Log_Tau;
			bool bRestart = false;
			int iBest_Fit_File;
			const char * lpszBest_Fit_File;
			bool bBest_Fit_With_Shell;
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			char lpszOutput_Filename[128];
			char lpszTitle[128];
			FILE * fileCache = fopen("spectrafit.filelist.cache","rt");
			unsigned int uiStartFile = 3;
			if (fileCache)
			{
				char lpszBuffer[256];
				fgets(lpszBuffer,256,fileCache);
				uiStartFile = atoi(lpszBuffer);
				fgets(lpszBuffer,256,fileCache);
				unsigned int uiNumFiles = atoi(lpszBuffer);
				bool bListTest = false;
				if (uiNumFiles == i_iArg_Count - 3)
				{
					bListTest = true;
					bRestart = true;
					for (unsigned int uiI = 1; uiI < i_iArg_Count && bListTest; uiI++)
					{
						fgets(lpszBuffer,256,fileCache);
						// need to strip CR/LF
						char * lpszCursor = lpszBuffer;
						lpszCursor += strlen(lpszBuffer);
						lpszCursor--;
						if (lpszCursor[0] == 10 || lpszCursor[0] == 13)
							lpszCursor[0] = 0;
						lpszCursor--;
						if (lpszCursor[0] == 10 || lpszCursor[0] == 13)
							lpszCursor[0] = 0;
						bListTest = (strcmp(lpszBuffer,i_lpszArg_Values[uiI]) == 0);
					}
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_PS_Ion_Temp = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_PS_Temp = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_Day = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_PS_Log_Tau = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_HVF_Ion_Temp = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_PS_Vel = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					dBest_Fit_HVF_Log_Tau = atof(lpszBuffer);
					fgets(lpszBuffer,256,fileCache);
					int iBest_Fit_File = atoi(lpszBuffer);
					lpszBest_Fit_File = i_lpszArg_Values[iBest_Fit_File];
					fgets(lpszBuffer,256,fileCache);
					bBest_Fit_With_Shell = lpszBuffer[0] == 't';
					printf("Generating spectra\n");
					// Generate the output for current best fit
					XVECTOR cTest(7);
					for (unsigned int uiI = 1; uiI < cOpacity_Map_Ejecta.GetNumColumns(); uiI++)
						if (cOpacity_Map_Ejecta.GetElement(uiI,0) == dBest_Fit_Day)
							cTest.Set(0,uiI); // time
					cTest.Set(1,dBest_Fit_PS_Vel); // PS velocity (kkm/s)
					cTest.Set(2,dBest_Fit_PS_Temp); // PS temp (kK)
					cTest.Set(3,dBest_Fit_PS_Ion_Temp); // PS ion temp (kK)
					cTest.Set(4,dBest_Fit_PS_Log_Tau); // PS ion log opacity scaling
					cTest.Set(5,dBest_Fit_HVF_Ion_Temp); // HVF ion temp (kK)
					cTest.Set(6,dBest_Fit_HVF_Log_Tau); // HVF ion log opacity scaling

					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.xdataset",i_lpszArg_Values[iBest_Fit_File]);
					sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",i_lpszArg_Values[iBest_Fit_File]);
					cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
					cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
					Generate_Synow_Spectra(cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,cTest,cBest_Fit_Output);
					printf("Done\n");
				}
				

				if (!bListTest)
				{
					uiStartFile = 3; // different file list - restart.
					system("rm spectrafit.filelist.cache");
//					system("rm spectrafit.*.cache");
					dBest_Fit = DBL_MAX;
				}
				fclose(fileCache);
			}
				
			for (unsigned int uiI = uiStartFile; uiI < i_iArg_Count; uiI++)
			{
				bRestart = false;
				printf("Reading %s",i_lpszArg_Values[uiI]);
				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.xdataset",i_lpszArg_Values[uiI]);
				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",i_lpszArg_Values[uiI]);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
				if (cOpacity_Map_Ejecta.GetNumElements() > 0)
				{
					double dDay,dPS_Vel, dPS_Temp,dPS_Ion_Temp,dPS_Log_Tau,dHVF_Ion_Temp,dHVF_Log_Tau;
					double dFit = CompareFits(i_lpszArg_Values[uiI], cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, dMin_WL, dMax_WL, bChi2, dDay, dPS_Vel, dPS_Temp, dPS_Ion_Temp, dPS_Log_Tau, dHVF_Ion_Temp, dHVF_Log_Tau, cOutput);
					if (dFit != -1.0 && dFit < dBest_Fit)
					{
						dBest_Fit = dFit;
						dBest_Fit_Day = dDay;
						dBest_Fit_PS_Temp = dPS_Temp;
						dBest_Fit_PS_Vel = dPS_Vel;
						dBest_Fit_PS_Log_Tau = dPS_Log_Tau;
						dBest_Fit_PS_Ion_Temp = dPS_Ion_Temp;
						dBest_Fit_HVF_Log_Tau = dHVF_Log_Tau;
						dBest_Fit_HVF_Ion_Temp = dHVF_Ion_Temp;
						bBest_Fit_With_Shell = (cOpacity_Map_Shell.GetNumElements() > 0);
						cBest_Fit_Output = cOutput;
						lpszBest_Fit_File = i_lpszArg_Values[uiI];
						iBest_Fit_File = uiI;
					}
					if (bChi2)
						printf("\tChi^2 Fit = %.2e\n",dFit);
					else
						printf("\tVar   Fit = %.2e\n",dFit);
				}
				else
					printf("Invalid file\n");
				fileCache = fopen("spectrafit.filelist.cache","wt");
				fprintf(fileCache,"%i\n",uiI);
				fprintf(fileCache,"%i\n",i_iArg_Count - 3);
				for (unsigned int uiJ = 1; uiJ < i_iArg_Count; uiJ++)
					fprintf(fileCache,"%s\n",i_lpszArg_Values[uiJ]);
				fprintf(fileCache,"%.17e\n",dBest_Fit);
				fprintf(fileCache,"%.17e\n",dBest_Fit_PS_Ion_Temp);
				fprintf(fileCache,"%.17e\n",dBest_Fit_PS_Temp);
				fprintf(fileCache,"%.17e\n",dBest_Fit_Day);
				fprintf(fileCache,"%.17e\n",dBest_Fit_PS_Log_Tau);
				fprintf(fileCache,"%.17e\n",dBest_Fit_HVF_Ion_Temp);
				fprintf(fileCache,"%.17e\n",dBest_Fit_PS_Vel);
				fprintf(fileCache,"%.17e\n",dBest_Fit_HVF_Log_Tau);
				fprintf(fileCache,"%i\n",iBest_Fit_File);
				fprintf(fileCache,"%c\n",bBest_Fit_With_Shell ? 't' : 'f');
				fclose(fileCache);
			}
			//system("rm spectrafit.filelist.cache");// done - delete the cache
			fprintf(stdout,"spectrafit v%s\n",VERSION);
			if (bChi2)
				fprintf(stdout,"Minimum chi^2: %.3e\n", dBest_Fit, lpszBest_Fit_File);
			else
				fprintf(stdout,"Minimum variance: %.3e", dBest_Fit, lpszBest_Fit_File);
			fprintf(stdout,"model: %s\n",lpszBest_Fit_File);
			fprintf(stdout,"Day: %.2f\n",dBest_Fit_Day);
			fprintf(stdout,"PS Vel: %.2f\n",dBest_Fit_PS_Vel);
			fprintf(stdout,"PS Temp: %.2f\n",dBest_Fit_PS_Temp);
			fprintf(stdout,"PS Log Tau: %.2f\n",dBest_Fit_PS_Log_Tau);
			fprintf(stdout,"PS Ion Temp: %.2f\n",dBest_Fit_PS_Ion_Temp);
			if (bBest_Fit_With_Shell)
			{
				fprintf(stdout,"HVF Log Tau: %.2f\n",dBest_Fit_HVF_Log_Tau);
				fprintf(stdout,"HVF Ion Temp: %.2f\n",dBest_Fit_HVF_Ion_Temp);
			}
			sprintf(lpszOutput_Filename,"%s.bestfit.eps",i_lpszArg_Values[1]);
			if (bBest_Fit_With_Shell)
				sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.1f tps %.2f Tpsi %.2f thv %.2f Thv %.2f",lpszBest_Fit_File,dBest_Fit_Day,dBest_Fit_PS_Vel,dBest_Fit_PS_Temp,dBest_Fit_PS_Log_Tau,dBest_Fit_PS_Ion_Temp,dBest_Fit_HVF_Log_Tau,dBest_Fit_HVF_Ion_Temp);
			else
				sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.1f tps %.2f Tpsi %.2f",lpszBest_Fit_File,dBest_Fit_Day,dBest_Fit_PS_Vel,dBest_Fit_PS_Temp,dBest_Fit_PS_Log_Tau,dBest_Fit_PS_Ion_Temp);
			Plot(cTarget, cBest_Fit_Output, lpszOutput_Filename, dMin_WL, dMax_WL, lpszTitle);
			fprintf(stdout,"Best fit output to %s.\n",lpszOutput_Filename);
		}
		else
		{
			Usage("Invalid fit region");
		}
	}
	return 0;
}

