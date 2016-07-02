
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
#include <xfit.h>

void Plot(epsplot::PAGE_PARAMETERS	& i_cPlot_Parameters, epsplot::DATA &i_cPlot, const char * i_lpszFilename_Format, const char * i_lpszOutput_File_Prefix, const ES::Spectrum & i_cFit, const ES::Spectrum & i_cFit_noPS, const ES::Spectrum & i_cFit_noHVF ,const double &i_dPlot_Min_WL, const double & i_dPlot_Max_WL,bool i_bNo_Shell, unsigned int i_uiX_Axis, unsigned int i_uiY_Axis)
{
	// Make sure that both spectra are within the desired plot range, otherwise don't bother plotting.
	if (((i_cFit.wl(0) >= i_dPlot_Min_WL && i_cFit.wl(0) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(i_cFit.size() - 1)>= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(0) <= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) >= i_dPlot_Max_WL)))
	{
		double * lpdFit_WL = NULL, *lpdFit_Flux = NULL, *lpdFit_noHVF_WL = NULL, * lpdFit_noHVF_Flux = NULL, *lpdFit_noPS_WL = NULL, *lpdFit_noPS_Flux = NULL;
		unsigned int uiNum_Points_Fit, uiNum_Points_Fit_noPS, uiNum_Points_Fit_noHVF;
		char lpszFilename[256];

		// Get spectral data in the desired range
		Get_Spectra_Data(i_cFit, lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, i_dPlot_Min_WL, i_dPlot_Max_WL);
		if (!i_bNo_Shell)
		{
			Get_Spectra_Data(i_cFit_noHVF, lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, i_dPlot_Min_WL, i_dPlot_Max_WL);
			Get_Spectra_Data(i_cFit_noPS, lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, i_dPlot_Min_WL, i_dPlot_Max_WL);
		}
		// normalize spectra
//		printf("%.2e\t%.2e\n",i_dTarget_Normalization_Flux,i_dFit_Normalization_Flux);
//		Normalize(lpdTarget_Flux, uiNum_Points_Target,i_dTarget_Normalization_Flux);
//		Normalize(lpdFit_Flux, uiNum_Points_Fit, i_dFit_Normalization_Flux);
//		if (!i_bNo_Shell)
//		{
//			Normalize(lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF,i_dFit_Normalization_Flux);
//			Normalize(lpdFit_noPS_Flux, uiNum_Points_Fit_noPS,i_dFit_Normalization_Flux);
//		}
		sprintf(lpszFilename,i_lpszFilename_Format,i_lpszOutput_File_Prefix);
		i_cPlot.Set_Plot_Filename(lpszFilename);

//		i_cPlot.Set_Plot_Data(0, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, epsplot::BLACK, epsplot::SOLID, 0, 0, -1.0);
		i_cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, epsplot::RED, epsplot::SOLID, i_uiX_Axis, i_uiY_Axis, -1.0);
		if (!i_bNo_Shell)
		{
			i_cPlot.Set_Plot_Data(lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, epsplot::RED, epsplot::STPL_CUSTOM_1, i_uiX_Axis, i_uiY_Axis, -1.0);
			i_cPlot.Set_Plot_Data(lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, epsplot::BLUE, epsplot::STPL_CUSTOM_1, i_uiX_Axis, i_uiY_Axis, -1.0);
		}
		i_cPlot.Plot(i_cPlot_Parameters);

		delete [] lpdFit_WL;
		delete [] lpdFit_Flux;
		if (!i_bNo_Shell)
		{
			delete [] lpdFit_noHVF_WL;
			delete [] lpdFit_noHVF_Flux;
			delete [] lpdFit_noPS_WL;
			delete [] lpdFit_noPS_Flux;
		}
	}
	else
	{
		printf("No plot: %.1f %.1f %.1f %.1f %.1f %.1f\n",i_dPlot_Min_WL, i_dPlot_Max_WL,i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1), i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1));
	}
}



#define FIT_BLUE_WL 4500.0
#define FIT_RED_WL 9000.0


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	ES::Spectrum	cSpectrum_No_HVF, cSpectrum_No_PS;
	double dFit_Range_Min_WL,dFit_Range_Max_WL,dWL_Ref;
	bool bData_valid;
	char lpszTarget_Path[256];
	char lpszTarget_File[256];
	char lpszTarget_Complete_Path[256] = {""};
	char lpszOutput_File_Prefix[256] = {""};
	char lpszOutput_Filename[256];
	char lpszOutput_Binary_Filename[256];
	char lpszTitle[256];
	unsigned int uiIon;
	bool bData_Valid = false;
	bool	bNo_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-plot-title");
	bool	bLinear_Plot = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--linear-scale");
	bool	bNo_Smart_Range = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range");
	bool	bNo_Smart_Range_Regions = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range-regions");
	bool	bSpectrum_Pre_Loaded = false;
	bool	bAllow_Ion_Temp_Fit, bDisallow_Scalar_Fit = false;
	bool 	bNo_Shell;
	double	dNan = nan("");
	xrsrand();

	int iModel = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--model",0);
	char lpszFitRegion[32];
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitregion", lpszFitRegion, 32, NULL);
	
	double dPVF_Scaling = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PVF",-20);
	double dHVF_Scaling = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF",-20);
	double	dPS_Velocity = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-velocity-init",20.0);
	double	dPS_Temp = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-temp",10.0);

	bool	bCalc_Fit = false;
	enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};
	tFit_Region	eFit_Region = OTHER;
	if (strcmp(lpszFitRegion,"CaHK") == 0 ||
		strcmp(lpszFitRegion,"CAHK") == 0 ||
		strcmp(lpszFitRegion,"CANIR") == 0 ||
		strcmp(lpszFitRegion,"CaNIR") == 0)
	{
		eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
		
		uiIon = 2001;
	}
	else if (strcmp(lpszFitRegion,"Si6355") == 0 || strcmp(lpszFitRegion,"SI6355") == 0)
	{
		uiIon = 1401;
		eFit_Region = SI6355;
	}
	else if (strcmp(lpszFitRegion,"Si5968") == 0 || strcmp(lpszFitRegion,"SI5968") == 0)
	{
		uiIon = 1401;
		eFit_Region = SI5968;
	}
	else if (strcmp(lpszFitRegion,"OI8446") == 0)
	{
		uiIon = 800;
		eFit_Region = OINIR;
	}

	double	dNorm_WL;
	ES::Spectrum cTarget = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 2.5);
	ES::Spectrum cOutput = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 2.5);
	ES::Spectrum cOutput_Continuum = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 2.5);
	ES::Spectrum cOutput_No_PVF = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 2.5);
	ES::Spectrum cOutput_No_HVF = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 2.5);
	switch (eFit_Region)
	{
	case CANIR:
		dFit_Range_Min_WL = 7750.0;
		dFit_Range_Max_WL = 8400.0;
		dNorm_WL = 7750.0;
//			dWL_Ref = (8203.97 + 8251.07 + 8256.99) / 3.0;
		dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		break;
	case CAHK:
		dNorm_WL = 3750.0;
		dFit_Range_Min_WL = 3500.0;
		dFit_Range_Max_WL = 3950.0;
		dWL_Ref = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
//			dWL_Ref = (3934.777  + 3969.592) * 0.5;
		break;
	case SI5968:
		dNorm_WL = 5750.0;
		dFit_Range_Min_WL = 5600.0;
		dFit_Range_Max_WL = 5950.0;
//			dWL_Ref = (5959.21  + 5980.59) * 0.5;
		dWL_Ref = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
		break;
	case SI6355:
		dNorm_WL = 5750.0;
		dFit_Range_Min_WL = 5700.0;
		dFit_Range_Max_WL = 6350.0;
		dWL_Ref = (6348.85 * pow(10.0,0.149)  + 6373.12 * pow(10.0,-0.082)) / (pow(10.0,0.149) + pow(10.0,-0.082));
		break;
	case OINIR:
		dNorm_WL = 7100.0;
		dFit_Range_Min_WL = 7100.0;
		dFit_Range_Max_WL = 7800.0;
		dWL_Ref = (7774.083 * pow(10.0,0.369) + 7776.305 * pow(10.0,0.223) + 7777.528 * pow(10.0,0.002)) / (pow(10.0,0.369) + pow(10.0,0.223) + pow(10.0,0.002));
//			dWL_Ref = (7774.083  + 7776.305 + 7777.528) / 3.0;
		break;
	}

	char	lpszFit_File[256];
	sprintf(lpszFit_File,"run%i",iModel);
	char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	switch (eFit_Region)
	{
	case CANIR:
	case CAHK:
	case SI6355:
	case SI5968:
		sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",lpszFit_File);
		break;
	case OINIR:
		sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",lpszFit_File);
		break;
	case OTHER:
		if (uiIon / 100 >= 14 && uiIon / 100 <= 20)
			sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",lpszFit_File);
		else if (uiIon / 100 == 6)
			sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.C.xdataset",lpszFit_File);
		else if (uiIon / 100 == 8)
			sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",lpszFit_File);
		else if (uiIon / 100 >= 22)
			sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Fe.xdataset",lpszFit_File);
		else if (uiIon / 100 >= 10 && uiIon / 100 <= 12)
			sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Mg.xdataset",lpszFit_File);
		break;
	}

	sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",lpszFit_File);
	cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
	cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
	bNo_Shell = cOpacity_Map_Shell.GetNumElements() == 0;
	double	dPS_Depth = 0.0;
	if (!bNo_Shell)
	{
		for (unsigned int uiI = cOpacity_Map_Shell.GetNumElements() - 1; uiI > 0; uiI--)
		{
			if (cOpacity_Map_Shell.GetElement(0,uiI) > dPS_Velocity * 1e8)
				dPS_Depth += cOpacity_Map_Shell.GetElement(4,uiI);
		}
	}
	for (unsigned int uiI = cOpacity_Map_Ejecta.GetNumElements() - 1; uiI > 0; uiI--)
	{
		if (cOpacity_Map_Ejecta.GetElement(0,uiI) > dPS_Velocity * 1e8)
			dPS_Depth += cOpacity_Map_Ejecta.GetElement(4,uiI);
	}
	printf("PS depth = %.3e\n",dPS_Depth);

	FILE * fileOut = fopen("velev.csv","wt");
	fprintf(fileOut,"Day, PVF velocity, HVF velocity\n");
	fflush(fileOut);

	for (double dDay = 1.0; dDay < 25.0; dDay+= 0.5)
	{
		double dPS_Current = 0.0;
		double	dPS_Test = 0.0;
		double	dScalar = pow(dDay,-2.0);
		XVECTOR	vDat(7);

		if (!bNo_Shell)
		{
			for (unsigned int uiI = cOpacity_Map_Shell.GetNumElements() - 1; uiI > 0 && dPS_Current == 0.0; uiI--)
			{
				dPS_Test += cOpacity_Map_Shell.GetElement(4,uiI) * dScalar;
//				printf("%.2e %.2e %.2e\n",cOpacity_Map_Shell.GetElement(0,uiI),cOpacity_Map_Shell.GetElement(4,uiI),dPS_Test);
				if (dPS_Test >= dPS_Depth && dPS_Current == 0.0)
					dPS_Current = cOpacity_Map_Shell.GetElement(0,uiI) * 1e-8;
			}
		}
		for (unsigned int uiI = cOpacity_Map_Ejecta.GetNumElements() - 1; uiI > 0 && dPS_Current == 0.0; uiI--)
		{
			dPS_Test += cOpacity_Map_Ejecta.GetElement(4,uiI) * dScalar;
//			printf("%.2e %.2e %.2e\n",cOpacity_Map_Ejecta.GetElement(0,uiI),cOpacity_Map_Ejecta.GetElement(4,uiI),dPS_Test);
			if (dPS_Test >= dPS_Depth && dPS_Current == 0.0)
				dPS_Current = cOpacity_Map_Ejecta.GetElement(0,uiI) * 1e-8;
		}
		if (dPS_Current < 8.0)
			dPS_Current = 8.0;
		printf("Day %f: PS = %f (%.5e)\n",dDay,dPS_Current,dPS_Test);
		vDat.Set(0,dDay);
		vDat.Set(1,dPS_Current);
		vDat.Set(2,dPS_Temp);
		vDat.Set(3,10.0);
		vDat.Set(4,dPVF_Scaling);
		vDat.Set(5,10.0);
		vDat.Set(6,dHVF_Scaling);

		printf("Generation starting\n");
		Generate_Synow_Spectra(cTarget,cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,vDat,cOutput);

		vDat.Set(4,-20.0); // turn off PS 
	//	printf("%.2f %.2f\n",cResult.Get(4),cResult.Get(6));
		Generate_Synow_Spectra(cTarget,cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,vDat,cOutput_No_PVF);
		vDat.Set(4,dPVF_Scaling); // turn off PS 
		vDat.Set(6,-20.0); // turn off HVF
	//	printf("%.2f %.2f\n",cResult.Get(4),cResult.Get(6));
		Generate_Synow_Spectra(cTarget,cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,vDat,cOutput_No_HVF);

		vDat.Set(4,-20.0); // turn off PS 
		vDat.Set(6,-20.0); // turn off HVF
		Generate_Synow_Spectra(cTarget,cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,vDat,cOutput_Continuum);
		printf("Generation complete\n");
		unsigned int uiIdx = 0;
		while (uiIdx < cTarget.size() && cTarget.wl(uiIdx) < dFit_Range_Min_WL)
			uiIdx++;
		double	dMin_PS = DBL_MAX,dMin_HVF = DBL_MAX,dMin_WL_PS,dMin_WL_HVF;
		while (uiIdx < cTarget.size() && cTarget.wl(uiIdx) < dFit_Range_Max_WL)
		{
			double dFlux = cOutput_No_PVF.flux(uiIdx) / cOutput_Continuum.flux(uiIdx);
			if (dFlux < dMin_HVF)
			{
				dMin_WL_HVF = cOutput_Continuum.wl(uiIdx);
				dMin_HVF = dFlux;
			}
			dFlux = cOutput_No_HVF.flux(uiIdx) / cOutput_Continuum.flux(uiIdx);
			if (dFlux < dMin_PS)
			{
				dMin_WL_PS = cOutput_Continuum.wl(uiIdx);
				dMin_PS = dFlux;
			}
			uiIdx++;
		}
		double	dZ = dMin_WL_PS / dWL_Ref - 1.0;
		double	dZ_p1_sqr = (dZ + 1.0) * (dZ + 1.0);
		double dPVF_Velocity = (dZ_p1_sqr - 1) / (dZ_p1_sqr + 1) * -300.0;
		dZ = dMin_WL_HVF / dWL_Ref - 1.0;
		dZ_p1_sqr = (dZ + 1.0) * (dZ + 1.0);
		double dHVF_Velocity = (dZ_p1_sqr - 1) / (dZ_p1_sqr + 1) * -300.0;


		epsplot::PAGE_PARAMETERS	cPlot_Parameters;
		epsplot::DATA cPlot;

		cPlot_Parameters.m_uiNum_Columns = 1;
		cPlot_Parameters.m_uiNum_Rows = 1;
		cPlot_Parameters.m_dWidth_Inches = 11.0;
		cPlot_Parameters.m_dHeight_Inches = 8.5;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Flux", false, false, false, 5.0, false, 60.0);
//		if (!bNo_Shell)
//			cPlot.Set_Num_Plots(3);
//		else
//			cPlot.Set_Num_Plots(1);

		cPlot.Set_Plot_Title(bNo_Title ? NULL : lpszTitle,18.0);
		double	dStipple[2] = {16,16}; // long long dash
		cPlot.Define_Custom_Stipple(epsplot::STPL_CUSTOM_1,dStipple,2);

		double dTarget_Minima_WL, dTarget_Minima_Flux, dFit_Minima_WL, dFit_Minima_Flux;

		// Find the minima of the line used for fitting
//		Find_Flux_Minimum(cTarget, dTarget_Minima_WL, dTarget_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
//		Find_Flux_At_WL(cFit_Info.cBest_Fit_Spectrum,dTarget_Minima_WL, dNormalization_Flux_Fit);
//		Find_Flux_Minimum(cFit_Info.cBest_Fit_Spectrum,dFit_Minima_WL, dFit_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);

		sprintf(lpszOutput_File_Prefix,"day%.2f",dDay);
		printf("Plot full\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.full.eps", lpszOutput_File_Prefix,  cOutput,  cOutput_No_PVF, cOutput_No_HVF,  cTarget.wl(0), cTarget.wl(cTarget.size()- 1),bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Ca NIR\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.CaNIR.eps", lpszOutput_File_Prefix,  cOutput,  cOutput_No_PVF, cOutput_No_HVF,  7000.0, 9000.0,bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Ca H&K\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.CaHK.eps", lpszOutput_File_Prefix,  cOutput,  cOutput_No_PVF, cOutput_No_HVF,  3000.0, 5000.0,bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Si 6355\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.Si6355.eps", lpszOutput_File_Prefix,  cOutput,  cOutput_No_PVF, cOutput_No_HVF, 5000.0, 7000.0,bNo_Shell, uiX_Axis, uiY_Axis);



		fprintf(fileOut, "%.2f,%.2f,%.2f\n",dDay, dPVF_Velocity, dHVF_Velocity);
		fflush(fileOut);
	}
	fclose(fileOut);
	return 0;
}

