
#include <cmath>
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
#include <Plot_Utilities.h>
#include <cfloat>
#include <best_fit_data.h>
#include <line_routines.h>
#include <xfit.h>

void Plot(epsplot::page_parameters	& i_cPlot_Parameters, epsplot::data &i_cPlot, const char * i_lpszFilename_Format, const char * i_lpszOutput_File_Prefix, const ES::Spectrum & i_cTarget, const ES::Spectrum & i_cFit, const ES::Spectrum & i_cFit_noPS, const ES::Spectrum & i_cFit_noHVF, const double & i_dTarget_Normalization_Flux, const double & i_dFit_Normalization_Flux,const double &i_dPlot_Min_WL, const double & i_dPlot_Max_WL,bool i_bNo_Shell, const double & i_dIF_Min, const double & i_dIF_Max, unsigned int i_uiX_Axis, unsigned int i_uiY_Axis)
{
	// Make sure that both spectra are within the desired plot range, otherwise don't bother plotting.
	if (((i_cTarget.wl(0) >= i_dPlot_Min_WL && i_cTarget.wl(0) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(i_cTarget.size() - 1)>= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(0) <= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) >= i_dPlot_Max_WL)) &&
	   ((i_cFit.wl(0) >= i_dPlot_Min_WL && i_cFit.wl(0) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(i_cFit.size() - 1)>= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(0) <= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) >= i_dPlot_Max_WL)))
	{
		double * lpdTarget_WL = NULL, *lpdTarget_Flux = NULL, *lpdFit_WL = NULL, *lpdFit_Flux = NULL, *lpdFit_noHVF_WL = NULL, * lpdFit_noHVF_Flux = NULL, *lpdFit_noPS_WL = NULL, *lpdFit_noPS_Flux = NULL;
		unsigned int uiNum_Points_Target, uiNum_Points_Fit, uiNum_Points_Fit_noPS, uiNum_Points_Fit_noHVF;
		char lpszFilename[256];

		// Get spectral data in the desired range
		Get_Spectra_Data(i_cTarget, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, i_dPlot_Min_WL, i_dPlot_Max_WL);
		Get_Spectra_Data(i_cFit, lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, i_dPlot_Min_WL, i_dPlot_Max_WL);
		if (!i_bNo_Shell)
		{
			Get_Spectra_Data(i_cFit_noHVF, lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, i_dPlot_Min_WL, i_dPlot_Max_WL);
			Get_Spectra_Data(i_cFit_noPS, lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, i_dPlot_Min_WL, i_dPlot_Max_WL);
		}
		// normalize spectra
//		printf("%.2e\t%.2e\n",i_dTarget_Normalization_Flux,i_dFit_Normalization_Flux);
		Normalize(lpdTarget_Flux, uiNum_Points_Target,i_dTarget_Normalization_Flux);
		Normalize(lpdFit_Flux, uiNum_Points_Fit, i_dFit_Normalization_Flux);

		epsplot::line_parameters	cLP;

		i_cPlot.Clear_Plots();

		cLP.m_eStipple = epsplot::SOLID;
		cLP.m_eColor = epsplot::BLACK;
		if (!i_bNo_Shell)
		{
			Normalize(lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF,i_dFit_Normalization_Flux);
			Normalize(lpdFit_noPS_Flux, uiNum_Points_Fit_noPS,i_dFit_Normalization_Flux);
		}
		sprintf(lpszFilename,i_lpszFilename_Format,i_lpszOutput_File_Prefix);
		i_cPlot.Set_Plot_Filename(lpszFilename);

		if (!std::isnan(i_dIF_Min) && !std::isnan(i_dIF_Max) && i_dIF_Min > 0 && i_dIF_Max > 0)
		{
			epsplot::RECTANGLE cRect;
			cRect.m_dX_min = i_dIF_Min;
			cRect.m_dX_max = i_dIF_Max;

			cRect.m_dY_min = 0.2;
			cRect.m_dY_max = 2.0;
			i_cPlot.Set_Rectangle_Data(cRect, true, epsplot::GREY_75, false, cLP, i_uiX_Axis, i_uiY_Axis);
		}


		cLP.m_eColor = epsplot::BLACK;
		i_cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, cLP, i_uiX_Axis, i_uiY_Axis);
		cLP.m_eColor = epsplot::RED;
		i_cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, cLP, i_uiX_Axis, i_uiY_Axis);
		if (!i_bNo_Shell)
		{
			cLP.m_eStipple = epsplot::STPL_CUSTOM_1;
			i_cPlot.Set_Plot_Data(lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, cLP, i_uiX_Axis, i_uiY_Axis);
			cLP.m_eColor = epsplot::BLUE;
			i_cPlot.Set_Plot_Data(lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, cLP, i_uiX_Axis, i_uiY_Axis);
		}



		i_cPlot.Plot(i_cPlot_Parameters);

		delete [] lpdTarget_WL;
		delete [] lpdTarget_Flux;
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
		printf("No plot: %.1f %.1f %.1f %.1f %.1f %.1f\n",i_dPlot_Min_WL, i_dPlot_Max_WL,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1), i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1));
	}
}


#define FIT_BLUE_WL 4500.0
#define FIT_RED_WL 9000.0
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

double	g_dTarget_Normalization_Flux = -1;
double	g_dGenerated_Normalization_Flux = -1;


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit_Info;
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
	bool bData_Valid = false;
	bool	bNo_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-plot-title");
	bool	bLinear_Plot = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--linear-scale");
	bool	bNo_Smart_Range = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range");
	bool	bNo_Smart_Range_Regions = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range-regions");
	bool	bInhibit_Resave = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-resave");
	bool	bInteraction_Feature = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--IF");
	double	dNan = nan("");
	double dIF_Min = dNan, dIF_Max = dNan;
	xrsrand();

	enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};
	tFit_Region	eFit_Region = OTHER;

	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--target") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--region") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--model"))
	{
		char lpszFitRegion[32];
		char	lpszFitModel[128];
		char 	lpszInteraction_Feature[256];
	
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--target", lpszTarget_Complete_Path, 256, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--region", lpszFitRegion, 32, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--model", lpszFitModel, 128, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--IF", lpszInteraction_Feature, 256, NULL);
		if (bInteraction_Feature)
		{
			char * lpszIF_Cursor = lpszInteraction_Feature;
			dIF_Min = atof(lpszInteraction_Feature);
			while (lpszIF_Cursor[0] != '-')
				lpszIF_Cursor++;
			lpszIF_Cursor++;
			dIF_Max = atof(lpszIF_Cursor);
		}
		
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
		if (strcmp(lpszFitRegion,"CaHK") == 0 ||
			strcmp(lpszFitRegion,"CaNIR") == 0)
		{
			eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si6355") == 0)
		{
			eFit_Region = SI6355;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si5968") == 0)
		{
			eFit_Region = SI5968;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"OI8446") == 0)
		{
			eFit_Region = OINIR;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"user") == 0)
		{
			eFit_Region = OTHER;
			sprintf(lpszOutput_Filename,"%s%s/%s.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
		}
		printf("Reading %s\n",lpszOutput_Binary_Filename);
		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
		{
			printf("... failed.\nReading %s\n",lpszOutput_Filename);
			cFit_Info.ReadFromFile(lpszOutput_Filename);
		}
		printf("Read done\n");
		if (eFit_Region != OTHER)
			sprintf(lpszOutput_File_Prefix,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File,lpszFitModel,lpszFitRegion);
		else
			sprintf(lpszOutput_File_Prefix,"%s%s/%s.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
	}

	// make sure all necessary data is available to generate the spectrum
	if (cFit_Info.iBest_Fit_File == 0)
	{
		bData_Valid = !std::isnan(cFit_Info.dBest_Fit_Day) && !std::isnan(cFit_Info.dBest_Fit_PS_Vel) && !std::isnan(cFit_Info.dBest_Fit_PS_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Temp);
		if (cFit_Info.bBest_Fit_With_Shell)
			bData_valid &= !std::isnan(cFit_Info.dBest_Fit_HVF_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Temp);
	}
	else
	{
		bData_Valid = !std::isnan(cFit_Info.dBest_Fit_PS_Vel) && !std::isnan(cFit_Info.dBest_Fit_PS_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Vmin) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Vmax) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Aux);
		if (cFit_Info.bBest_Fit_With_Shell)
			bData_Valid &= !std::isnan(cFit_Info.dBest_Fit_HVF_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Temp) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Vmin) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Vmax) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Aux);
	}


	double	dNorm_WL;
	if (bData_Valid)
	{
        ES::Spectrum cTarget;
		if (lpszTarget_Complete_Path[0] != 0)
		{
			cTarget = ES::Spectrum::create_from_ascii_file( lpszTarget_Complete_Path );
//			cFit_Info.cBest_Fit_Spectrum = cTarget;
//			cFit_Info.cBest_Fit_Spectrum.zero_flux();
//			cSpectrum_No_PS = cTarget;
//			cSpectrum_No_PS.zero_flux();
//			cSpectrum_No_HVF = cTarget;
//			cSpectrum_No_HVF.zero_flux();
		}
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
		case OTHER:
			dFit_Range_Min_WL = cTarget.wl(0);
			dFit_Range_Max_WL = cTarget.wl(cTarget.size() - 1);
			dNorm_WL = (cTarget.wl(0) + cTarget.wl(cTarget.size() - 1)) * 0.5;
			dWL_Ref = (dFit_Range_Max_WL * 2.0 + dFit_Range_Min_WL) / 3.0;
			break;
		}
		bool bNo_Shell = cFit_Info.dBest_Fit_HVF_Log_Tau == 0.0;//cBest_Fit_Spectrum_No_PS.size() == 0;
//		printf("%f\n",cFit_Info.dBest_Fit_HVF_Log_Tau);

		if (cFit_Info.cBest_Fit_Spectrum.size() > 0 && cFit_Info.cBest_Fit_Spectrum_No_HVF.size() > 0)
		{
			Get_Normalization_Fluxes(cTarget,cFit_Info.cBest_Fit_Spectrum, FIT_BLUE_WL, FIT_RED_WL, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);

			epsplot::page_parameters	cPlot_Parameters;
			epsplot::data cPlot;

			cPlot_Parameters.m_uiNum_Columns = 1;
			cPlot_Parameters.m_uiNum_Rows = 1;
			cPlot_Parameters.m_dWidth_Inches = 11.0;
			cPlot_Parameters.m_dHeight_Inches = 8.5;

			unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
			unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Flux", false, false, false, 0.2, false, 2.0);
//			if (!bNo_Shell)
//				cPlot.Set_Num_Plots(4);
//			else
//				cPlot.Set_Num_Plots(2);

			cPlot.Set_Plot_Title(bNo_Title ? NULL : lpszTitle,18.0);
			double	dStipple[2] = {16,16}; // long long dash
			cPlot.Define_Custom_Stipple(epsplot::STPL_CUSTOM_1,dStipple,2);

			double dTarget_Minima_WL, dTarget_Minima_Flux, dFit_Minima_WL, dFit_Minima_Flux;

			// Find the minima of the line used for fitting
	//		Find_Flux_Minimum(cTarget, dTarget_Minima_WL, dTarget_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
	//		Find_Flux_At_WL(cFit_Info.cBest_Fit_Spectrum,dTarget_Minima_WL, dNormalization_Flux_Fit);
	//		Find_Flux_Minimum(cFit_Info.cBest_Fit_Spectrum,dFit_Minima_WL, dFit_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);

			printf("Plot full\n");
			Plot(cPlot_Parameters, cPlot, "%s.user.full.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux, cTarget.wl(0), cTarget.wl(cTarget.size()- 1),bNo_Shell,dIF_Min,dIF_Max, uiX_Axis, uiY_Axis);

			printf("Plot Ca NIR\n");
			epsplot::axis_parameters cAxis_Param_Save = cPlot.Get_Y_Axis_Parameters(uiY_Axis);
			epsplot::axis_parameters cAxis_Param_Mod = cAxis_Param_Save;
			cAxis_Param_Mod.m_dLower_Limit = 0.2;
			cAxis_Param_Mod.m_dUpper_Limit = 2.0;

			cPlot.Modify_Y_Axis_Parameters(uiY_Axis, cAxis_Param_Mod);
			Plot(cPlot_Parameters, cPlot, "%s.user.CaNIR.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,7700.0, 9000.0,bNo_Shell,dIF_Min,dIF_Max, uiX_Axis, uiY_Axis);

			printf("Plot Ca H&K\n");
			cPlot.Modify_Y_Axis_Parameters(uiY_Axis, cAxis_Param_Save);
			Plot(cPlot_Parameters, cPlot, "%s.user.CaHK.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,3000.0, 5000.0,bNo_Shell,dIF_Min,dIF_Max, uiX_Axis, uiY_Axis);

			printf("Plot Si 6355\n");
			Plot(cPlot_Parameters, cPlot, "%s.user.Si6355.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,5000.0, 7000.0,bNo_Shell,dIF_Min,dIF_Max, uiX_Axis, uiY_Axis);
		}
	}
	else
	{
		fprintf(stderr,"Failed\n");
		fflush(stderr);
	}
	return 0;
}

