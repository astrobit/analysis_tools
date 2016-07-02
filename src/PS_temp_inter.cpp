
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
//#include "ES_Generic_Error.hh"
#include <Plot_Utilities.h>
#include <float.h>
#include <best_fit_data.h>
#include <line_routines.h>


#define FIT_BLUE_WL 6500.0
#define FIT_RED_WL 6750.0
void Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, double & o_dTarget_Flux, double & o_dGenerated_Flux)
{
//	double dMax_WL;
//	Find_Flux_Global_Maximum(i_cTarget,dMax_WL,o_dTarget_Flux, i_dMin_WL, i_dMax_WL);
//	Find_Flux_At_WL(i_cGenerated, dMax_WL, o_dGenerated_Flux);
//	Find_Flux_At_WL(i_cTarget, 6250.0, o_dTarget_Flux);
//	Find_Flux_At_WL(i_cGenerated, 6250.0, o_dGenerated_Flux);
//	double dSi6355_Min_Wl,dSi6355_Min_Flux;
//	double dSi6355_Red_Max_Wl,dSi6355_Red_Max_Flux;

//	Find_Flux_Minimum(i_cTarget,dSi6355_Min_Wl,dSi6355_Min_Flux, 6000.0,6500.0);
//	Find_Red_Maxima(i_cTarget,dSi6355_Red_Max_Wl,dSi6355_Red_Max_Flux, 6000.0,6500.0);

//	unsigned int uiI = 0;
//	double dTarget = (dSi6355_Red_Max_Flux - dSi6355_Min_Flux) * 0.9 + dSi6355_Min_Flux;
//	while (uiI < i_cTarget.size() && i_cTarget.wl(uiI) < dSi6355_Min_Wl)
//		uiI++;
//	while (uiI < i_cTarget.size() && i_cTarget.flux(uiI) < dTarget)
//		uiI++;
//	Find_Flux_At_WL(i_cTarget, i_cTarget.wl(uiI), o_dTarget_Flux);
//	Find_Flux_At_WL(i_cGenerated, i_cTarget.wl(uiI), o_dGenerated_Flux);

//	o_dGenerated_Flux = i_cGenerated.flux(0);
//	o_dTarget_Flux = i_cTarget.flux(0);
//	double dNorm = o_dTarget_Flux / o_dGenerated_Flux;
//	for (unsigned int uiI = 1;  uiI < i_cTarget.size(); uiI++)
//	{
//		double	dGen_Flux = i_cGenerated.flux(uiI) * dNorm;
//		if (dGen_Flux < i_cTarget.flux(uiI))
//		{
//			o_dGenerated_Flux = i_cGenerated.flux(uiI);
//			o_dTarget_Flux = i_cTarget.flux(uiI);
//			dNorm = o_dTarget_Flux / o_dGenerated_Flux;
//		}
//	}

	unsigned int uiIdx = 0;
	double	dLuminosity_Target = 0.0, dLuminosity_Synth = 0.0;
	double	dFit_Delta_Lambda;
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_BLUE_WL)
		uiIdx++;
	dFit_Delta_Lambda = -i_cTarget.wl(uiIdx);

	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_RED_WL)
	{
		double dDelta_Lambda;
		if (uiIdx > 0 && uiIdx  < (i_cTarget.size() - 1))
			dDelta_Lambda = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx - 1)) * 0.5;
		else if (uiIdx == 0)
			dDelta_Lambda = i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx);
		else if (uiIdx == (i_cTarget.size() - 1))
			dDelta_Lambda = i_cTarget.wl(uiIdx) - i_cTarget.wl(uiIdx - 1);

		dLuminosity_Target += i_cTarget.flux(uiIdx) * dDelta_Lambda;
		dLuminosity_Synth += i_cGenerated.flux(uiIdx) * dDelta_Lambda;
		uiIdx++;

	}
	uiIdx--;
	dFit_Delta_Lambda += i_cTarget.wl(uiIdx);

	dLuminosity_Target /= dFit_Delta_Lambda; 
	dLuminosity_Synth /= dFit_Delta_Lambda; 
//	double dSynth_Normalization = dLuminosity_Synth / dLuminosity_Target;
	o_dTarget_Flux = dLuminosity_Target * 0.5;// * 1.1;
	o_dGenerated_Flux = dLuminosity_Synth * 0.5; //dSynth_Normalization;

//	o_dTarget_Flux = i_cTarget.flux(uiIdx);

//	double dSynth_Normalization = dLuminosity_Synth / dLuminosity_Target;
//	o_dTarget_Flux = 1.0;
//	o_dGenerated_Flux = dSynth_Normalization * o_dTarget_Flux;
}

double	Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL,  double * &lpdRaw_Moments, double * & lpdCentral_Moments, double * & lpdStandardized_Moments)
{
//	double * lpdRaw_Moments = NULL, *lpdCentral_Moments = NULL, *lpdStandardized_Moments = NULL;
	double dMax_Flux,dGenerated_Flux;
	Get_Normalization_Fluxes(i_cTarget, i_cGenerated, i_dMin_WL, i_dMax_WL,dMax_Flux,dGenerated_Flux);
	Get_Fit_Moments_2(i_cTarget, i_cGenerated, i_dMin_WL, i_dMax_WL, 6, lpdRaw_Moments, lpdCentral_Moments, lpdStandardized_Moments, dMax_Flux, dGenerated_Flux);

	return fabs(lpdStandardized_Moments[4]);//fabs(lpdRaw_Moments[1]); // use mean
}
/*
void CompareFits(const ES::Spectrum &i_cTarget, ES::Spectrum &o_cOutput, double & o_dTemp)
{
	ES::Spectrum cOutput(i_cTarget);
	printf("Generating vector\n");
	XVECTOR cResult(7);
	unsigned int uiParameters = cResult.Get_Size();
	XVECTOR vBest_Fit(uiParameters);
//	printf("\n");
//	cResult.Print();

	cResult.Set(0,10.0);//ps vel
	cResult.Set(1,11.0); // ps temp
	cResult.Set(2,-10.0); // PS ion log tau
	cResult.Set(3,10.0); // PS ion exctication temp
	cResult.Set(4,10.0); // PS ion vmin
	cResult.Set(5,11.0); // PS ion vmax
	cResult.Set(6,1.0); // PS ion vscale

	Generate_Synow_Spectra_Exp(i_cTarget,2001,cResult,cOutput); // ion irrelevant for this 

	//ameoba algorithm to refine fit
	double	dBest_Fit = Get_Fit(i_cTarget,cOutput,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1));
//	printf("Starting fit %.2e\n",dBest_Fit);
	vBest_Fit = cResult;
	double dRange = 2.0;
	double dBest_Fit_Range = dRange;
	printf("Fit = %.2e\n",dBest_Fit);

	{
		unsigned int iNo_Update_Counter = 0;
		while (dBest_Fit_Range > 0.01 && iNo_Update_Counter < 2)
		{
			dRange =  dBest_Fit_Range;
			bool bFound_Better_Fit = false;
			XVECTOR vBase(vBest_Fit);
			for (unsigned int uiI = 0; uiI < 8; uiI++) // p * 2 isn't analytic - can probably refine this better - it's probably p + 1 for optimal search (akin to simplex)
			{
				XVECTOR vCurr(vBase);
				double	dDelta = (xrand_d() * 2.0 - 1.0) * dRange;
				vCurr.Set(1,vBase.Get(1) + dDelta); // set PS LGR temp to PS temp
				fprintf(stdout,"Trying %.1f\t",vCurr.Get(1));
				fflush(stdout);
				Generate_Synow_Spectra_Exp(i_cTarget,2001,vCurr,cOutput); // ion irrelevant for this 
				double dFit = Get_Fit(i_cTarget,cOutput,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1));
				printf("Fit = %.2e Delta = %.2e Range = %.2e",dFit,dDelta,dRange);
				if (dFit < dBest_Fit)
				{
					dBest_Fit = dFit;
					printf("\tBF");
					dBest_Fit_Range = fabs(dDelta); // set the next range to the magnitude of the delta
					vBest_Fit = vCurr;
					bFound_Better_Fit = true;
				}
				printf("\n");
			}
			if (bFound_Better_Fit)
				iNo_Update_Counter = 0; // reset
			else
				iNo_Update_Counter++;
		}
	}
	cResult = vBest_Fit;
	Generate_Synow_Spectra_Exp(i_cTarget,2001,vBest_Fit,cOutput); // ion irrelevant for this 
	o_cOutput = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	o_dTemp = vBest_Fit.Get(1);
}
*/

void Plot(epsplot::PAGE_PARAMETERS	& i_cPlot_Parameters, epsplot::DATA &i_cPlot, const char * i_lpszFilename_Format, const char * i_lpszOutput_File_Prefix, const ES::Spectrum & i_cTarget, const ES::Spectrum & i_cFit, const double & i_dTarget_Normalization_Flux, const double & i_dFit_Normalization_Flux,const double &i_dPlot_Min_WL, const double & i_dPlot_Max_WL, unsigned int i_uiX_Axis, unsigned int i_uiY_Axis)
{
	// Make sure that both spectra are within the desired plot range, otherwise don't bother plotting.
	if (((i_cTarget.wl(0) >= i_dPlot_Min_WL && i_cTarget.wl(0) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(i_cTarget.size() - 1)>= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(0) <= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) >= i_dPlot_Max_WL)) &&
	   ((i_cFit.wl(0) >= i_dPlot_Min_WL && i_cFit.wl(0) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(i_cFit.size() - 1)>= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(0) <= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) >= i_dPlot_Max_WL)))
	{
		double * lpdTarget_WL = NULL, *lpdTarget_Flux = NULL, *lpdFit_WL = NULL, *lpdFit_Flux = NULL;
		unsigned int uiNum_Points_Target, uiNum_Points_Fit;
		char lpszFilename[256];

		// Get spectral data in the desired range
		Get_Spectra_Data(i_cTarget, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, i_dPlot_Min_WL, i_dPlot_Max_WL);
		Get_Spectra_Data(i_cFit, lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, i_dPlot_Min_WL, i_dPlot_Max_WL);

		// normalize spectra
//		printf("%.2e\t%.2e\n",i_dTarget_Normalization_Flux,i_dFit_Normalization_Flux);
		Normalize(lpdTarget_Flux, uiNum_Points_Target,i_dTarget_Normalization_Flux);
		Normalize(lpdFit_Flux, uiNum_Points_Fit, i_dFit_Normalization_Flux);

		sprintf(lpszFilename,i_lpszFilename_Format,i_lpszOutput_File_Prefix);
		i_cPlot.Set_Plot_Filename(lpszFilename);

		i_cPlot.Set_Plot_Data( lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, epsplot::BLACK, epsplot::SOLID, i_uiX_Axis, i_uiY_Axis, -1.0);
		i_cPlot.Set_Plot_Data( lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, epsplot::RED, epsplot::SOLID, i_uiX_Axis, i_uiY_Axis, -1.0);
		i_cPlot.Plot(i_cPlot_Parameters);

		delete [] lpdTarget_WL;
		delete [] lpdTarget_Flux;
		delete [] lpdFit_WL;
		delete [] lpdFit_Flux;
	}
	else
	{
		printf("No plot: %.1f %.1f %.1f %.1f %.1f %.1f\n",i_dPlot_Min_WL, i_dPlot_Max_WL,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1), i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1));
	}
}


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	double dFit_Range_Min_WL,dFit_Range_Max_WL;
	bool bData_valid;
	char lpszTarget_Path[256];
	char lpszTarget_File[256];
	char lpszTarget_Complete_Path[256] = {""};
	char lpszOutput_File_Prefix[256] = {""};
	char lpszOutput_Filename[256];
	char lpszTitle[256];
	bool bData_Valid = false;

	double * lpdRaw_Moments[2] = {NULL,NULL}, * lpdCentral_Moments[2] = {NULL,NULL}, * lpdStandardized_Moments[2] = {NULL,NULL};
	xrsrand();

	char lpszFitRegion[32];
	char	lpszFitModel[128];
	if (i_iArg_Count < 2)
	{
		printf("Must specify file to do temp fitting.\n");
		exit(1);
	}
	unsigned int uiPath_Idx = 1;
	while (i_lpszArg_Values[uiPath_Idx][0] == '-')
		uiPath_Idx++;
	strcpy(lpszTarget_Complete_Path,i_lpszArg_Values[uiPath_Idx]);
	
	strcpy(lpszTarget_Path,lpszTarget_Complete_Path);
	unsigned int uiI = strlen(lpszTarget_Path);
	while (lpszTarget_Path[uiI] != '/' && uiI > 0)
		uiI--;
	if (uiI != 0)
	{
		uiI++;
		lpszTarget_Path[uiI] = 0;// 
	}
	else
		strcpy(lpszTarget_Path,"./");
	strcpy(lpszTarget_File,&lpszTarget_Complete_Path[uiI]);

	sprintf(lpszOutput_File_Prefix,"%s%s/%s",lpszTarget_Path,"Temp",lpszTarget_File);

    ES::Spectrum cTarget = ES::Spectrum::create_from_ascii_file( lpszTarget_Complete_Path );
    ES::Spectrum cGenerated = ES::Spectrum::create_from_ascii_file( lpszTarget_Complete_Path );
	double	dTemp;
	do
	{
		char lpszTemp_Str[16];

		fprintf(stdout,"Enter temp (in kK) to test ( or -1 to quit): ");
		fflush(stdout);
		fgets(lpszTemp_Str,16,stdin);
		char * lpszCursor = lpszTemp_Str;
		while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
			lpszCursor++;
		dTemp = atof(lpszCursor);
		if (dTemp > 0.0 && dTemp < 8.0)
		{
			char lpszResponse[8];
			fprintf(stdout,"Really? ");
			fflush(stdout);
			fgets(lpszResponse,8,stdin);
			if (lpszResponse[0] != 'y' && lpszResponse[0] != 'Y' && lpszResponse[0] != 't' && lpszResponse[0] != 'T')
				dTemp = -1.0;
		}
		if (dTemp > 0.0)
		{
			XVECTOR cResult(7);
		//	printf("\n");
		//	cResult.Print();

			cResult.Set(0,10.0);//ps vel
			cResult.Set(1,dTemp); // ps temp
			cResult.Set(2,-10.0); // PS ion log tau
			cResult.Set(3,10.0); // PS ion exctication temp
			cResult.Set(4,10.0); // PS ion vmin
			cResult.Set(5,11.0); // PS ion vmax
			cResult.Set(6,1.0); // PS ion vscale

			Generate_Synow_Spectra_Exp(cTarget,2001,cResult,cGenerated); // ion irrelevant for this 
	
			epsplot::PAGE_PARAMETERS	cPlot_Parameters;
			epsplot::DATA cPlot;

			sprintf(lpszTitle,"Tps %.3f\n",dTemp);

			cPlot_Parameters.m_uiNum_Columns = 1;
			cPlot_Parameters.m_uiNum_Rows = 1;
			cPlot_Parameters.m_dWidth_Inches = 11.0;
			cPlot_Parameters.m_dHeight_Inches = 8.5;

			unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
			unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Flux", false, false, false, 5.0, false, 60.0);
//			cPlot.Set_Num_Plots(2);
			cPlot.Set_Plot_Title(lpszTitle,18.0);
			double	dStipple[2] = {16,16}; // long long dash
			cPlot.Define_Custom_Stipple(epsplot::STPL_CUSTOM_1,dStipple,2);

			double dTarget_Minima_WL, dTarget_Minima_Flux, dFit_Minima_WL, dFit_Minima_Flux;

			Get_Normalization_Fluxes(cTarget, cGenerated, cTarget.wl(0), cTarget.wl(cTarget.size() - 1), dTarget_Minima_Flux, dFit_Minima_Flux);

			Plot(cPlot_Parameters, cPlot, "%s.tempfit.full.eps", lpszOutput_File_Prefix, cTarget, cGenerated, dTarget_Minima_Flux, dFit_Minima_Flux,cTarget.wl(0), cTarget.wl(cTarget.size()- 1), uiX_Axis, uiY_Axis);

			sprintf(lpszOutput_Filename,"%s.tempfit.data",lpszOutput_File_Prefix);
			
//			double dBest_Fit = Get_Fit(cTarget, cGenerated, FIT_BLUE_WL, FIT_RED_WL,lpdRaw_Moments, lpdCentral_Moments, lpdStandardized_Moments);
			double dBest_Fit[2] = {Get_Fit(cTarget, cGenerated, 7750.0 - 25.0, 7750.0 + 25.0,lpdRaw_Moments[0], lpdCentral_Moments[0], lpdStandardized_Moments[0]), Get_Fit(cTarget, cGenerated, 6700.0 - 25.0, 6700.0 + 25.0,lpdRaw_Moments[1], lpdCentral_Moments[1], lpdStandardized_Moments[1])};
			
			printf("Raw Fits:\t\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdRaw_Moments[0][0],lpdRaw_Moments[0][1],lpdRaw_Moments[0][2],lpdRaw_Moments[0][3],lpdRaw_Moments[0][4],lpdRaw_Moments[0][5]);
			printf("Central Fits:\t\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdCentral_Moments[0][0],lpdCentral_Moments[0][1],lpdCentral_Moments[0][2],lpdCentral_Moments[0][3],lpdCentral_Moments[0][4],lpdCentral_Moments[0][5]);
			printf("Standardized Fits:\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdStandardized_Moments[0][0],lpdStandardized_Moments[0][1],lpdStandardized_Moments[0][2],lpdStandardized_Moments[0][3],lpdStandardized_Moments[0][4],lpdStandardized_Moments[0][5]);
		
			printf("Raw Fits:\t\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdRaw_Moments[1][0],lpdRaw_Moments[1][1],lpdRaw_Moments[1][2],lpdRaw_Moments[1][3],lpdRaw_Moments[1][4],lpdRaw_Moments[1][5]);
			printf("Central Fits:\t\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdCentral_Moments[1][0],lpdCentral_Moments[1][1],lpdCentral_Moments[1][2],lpdCentral_Moments[1][3],lpdCentral_Moments[1][4],lpdCentral_Moments[1][5]);
			printf("Standardized Fits:\t%.4e %.4e %.4e %.4e %.4e %.4e\n",lpdStandardized_Moments[1][0],lpdStandardized_Moments[1][1],lpdStandardized_Moments[1][2],lpdStandardized_Moments[1][3],lpdStandardized_Moments[1][4],lpdStandardized_Moments[1][5]);

			sprintf(lpszOutput_Filename,"%s.tempfit.csv",lpszOutput_File_Prefix);
			FILE * fileOut = fopen(lpszOutput_Filename,"wt");
			for (unsigned int uiI = 0; uiI < cGenerated.size(); uiI++)
				fprintf(fileOut, "%f %f %f\n",cGenerated.wl(uiI), cGenerated.flux(uiI), cGenerated.flux_error(uiI));
			fclose(fileOut);
		}
	} while (dTemp > 0.0);		
	return 0;
}

