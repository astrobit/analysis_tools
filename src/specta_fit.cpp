
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
#include <Plot_Utilities.h>
#include <best_fit_data.h>
#include <float.h>
#include <line_routines.h>
#include <model_spectra_db.h>




#define VERSION "0.2"
void Usage(const char * i_lpszError_Text)
{
		fprintf(stderr,"spectrafit v%s\n",VERSION);
		if (i_lpszError_Text)
			fprintf(stderr,"\nError: %s\n\n",i_lpszError_Text);
		fprintf(stderr,"Usage: \n\tspectrafit <Source_spectrum> <Fit_region> <Test_spectra> [options]\n");
		fprintf(stderr,"Source_spectrum: file containing wavelength, flux, and flux error.\n");
		fprintf(stderr,"\tMust be space, tab, or column separated.  If flux error is zero,\n");
		fprintf(stderr,"\tvariance will be used instead of chi^2.\n");
		fprintf(stderr,"Fit_region: Line for which to do the fit.  Options are 'CaNIR', 'CaHK',\n");
		fprintf(stderr,"\t'Si6355', or 'O7773'.\n");
//		fprintf(stderr,"\tor 'Si6355' or 'Full'.\n");  // Full currently disabled
		fprintf(stderr,"Test_spectra: List of opactiy map(s) to use.\n");
		fprintf(stderr,"\n");
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t--time=X\n\t\tSpecify the suggested time after the explosion for which to fit the spectra\n");
		fprintf(stderr,"Output: name of opacity map with best fit, as well as the chi^2 or variance\n");
		fprintf(stderr,"\tof the fit, the day number, photosphere temperature, ion excitation\n");
		fprintf(stderr,"\tand the log opacity reference used.\n");
}

//double Process_CL_Dbl(int &io_iArg_Count,const char * i_lpszArg_Values[], const char * i_lpszParameter, const double & i_dDefault_Value);

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	xrand_Set_Type(XRT_K);
	xrsrand();

	msdb::FEATURE	eFeature;

	char	lpszFilename[64];
	char	lpszBuffer[1024];
	char	lpszTargetFileStripped[256];
	char	lpszGroup_Data_File[4];
	XDATASET cDataSource;
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	char	chSeparator;
	double	dSuggested_Time;
	bool bNo_Exp;
	double		dSuggested_PS;
	double		dSuggested_PS_Temp;
	double		dSuggested_Ejecta_Scalar;
	double		dSuggested_Ejecta_Temp;
	double		dSuggested_Shell_Scalar;
	double		dSuggested_Shell_Temp;
	double		dSuggested_Normalization_WL;
	double		dFit_Max_WL;
	double		dFit_Min_WL;
	double		dNan = nan("");
	unsigned int uiNum_Files_To_Process;
	bool		bForce_Processing;
	double		dPlot_Max_WL,dPlot_Min_WL,dLine_Minimum;
//	const char * lpszFile_Lists
	if (bForce_Processing = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--ignore-cache"))
	{
		i_iArg_Count--; // dont' process the option as a file
		printf("Forced restart\n");
	}

	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--time"))
	{
		dSuggested_Time = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--time",0.0);
		i_iArg_Count--; // dont' process the option as a file
//		printf("%.0f\n",dSuggested_Time);
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--timebmax"))
	{
		dSuggested_Time = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--timebmax",-19.0);
		dSuggested_Time += 19.0;
//		printf("%.0f\n",dSuggested_Time);
		i_iArg_Count--; // dont' process the option as a file
	}

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

		const char * lpszTargetFileStripped = i_lpszArg_Values[1];
		if (strchr(lpszTargetFileStripped,'/'))
		{
			lpszTargetFileStripped += strlen(i_lpszArg_Values[1]);
			while (lpszTargetFileStripped[0] != '/')
				lpszTargetFileStripped--;
			lpszTargetFileStripped++;
		}
		// default to full range

		if (strcmp(i_lpszArg_Values[2],"CaNIR") == 0)
		{
			eFeature = msdb::CaNIR;
			uiIon = 2001;
			dMin_WL = 7750.0;
			dMax_WL = 8800.0;
			dPlot_Min_WL = 7000.0;
			dPlot_Max_WL = 8500.0;
			dLine_Minimum = 8000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else if (strcmp(i_lpszArg_Values[2],"CaHK") == 0)
		{
			eFeature = msdb::CaHK;
			uiIon = 2001;
			dMin_WL = 3000.0;
			dMax_WL = 4500.0;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
			dLine_Minimum = 4000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else if (strcmp(i_lpszArg_Values[2],"Si6355") == 0)
		{
			eFeature = msdb::Si6355;
			uiIon = 1401;
			dMin_WL = 5000.0;
			dMax_WL = 6500.0;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
			dLine_Minimum = 6000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else if (strcmp(i_lpszArg_Values[2],"O7773") == 0)
		{
			eFeature = msdb::Si6355;
			uiIon = 800;
			dMin_WL = 6000.0;
			dMax_WL = 7500.0;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
			dLine_Minimum = 6500.0;
			strcpy(lpszGroup_Data_File,"O");
		}
		double	dCoverage = 1.0;
		if (dMin_WL > cTarget.wl(0) && cTarget.wl(0) < dMax_WL && dMax_WL < cTarget.wl(cTarget.size() - 1))
		{
			// partial coverage - the target data starts in the middle of the range
			dCoverage = (dMax_WL - cTarget.wl(0)) / (dMax_WL - dMin_WL);
		}
		if (dMax_WL > cTarget.wl(cTarget.size() - 1) && dMin_WL < cTarget.wl(cTarget.size() - 1) && dMin_WL > cTarget.wl(0))
		{
			// partial coverage - the target data starts in the middle of the range
			dCoverage = (cTarget.wl(cTarget.size() - 1) - dMin_WL) / (dMax_WL - dMin_WL);
		}
		else if (cTarget.wl(0) > dMin_WL && cTarget.wl(cTarget.size() - 1) < dMax_WL)
		{
			dCoverage = (cTarget.wl(cTarget.size() - 1) - cTarget.wl(0)) / (dMax_WL - dMin_WL);
		}
		else if (dMin_WL > cTarget.wl(cTarget.size() - 1) || dMax_WL < cTarget.wl(0))
		{
			dCoverage = 0.0;
		}

		if (dCoverage < 0.75)
		{
			char lpszError[256];
			sprintf(lpszError,"Source spectrum does not cover desired range.  Requested %.2f - %.2f, Available %.2f - %.2f",dMin_WL,dMax_WL,cTarget.wl(0),cTarget.wl(cTarget.size() - 1));
			Usage(lpszError);
		}
		else if (dMin_WL > 0 && dMax_WL > 0)
		{
			if (dCoverage < 1.0)
			{
				if (dMin_WL > cTarget.wl(0) && cTarget.wl(0) < dMax_WL && dMax_WL < cTarget.wl(cTarget.size() - 1))
				{
					dMin_WL = cTarget.wl(0);
				}
				if (dMax_WL > cTarget.wl(cTarget.size() - 1) && dMin_WL < cTarget.wl(cTarget.size() - 1) && dMin_WL > cTarget.wl(0))
				{
					dMax_WL = cTarget.wl(cTarget.size() - 1);
				}
				else if (cTarget.wl(0) > dMin_WL && cTarget.wl(cTarget.size() - 1) < dMax_WL)
				{
					dMin_WL = cTarget.wl(0);
					dMax_WL = cTarget.wl(cTarget.size() - 1);
				}
			}
			unsigned int uiStart_Idx = 0;
			while (uiStart_Idx < cTarget.size() && cTarget.wl(uiStart_Idx) < dLine_Minimum)
				uiStart_Idx++;
			unsigned int uiTest_Idx = uiStart_Idx;
			double dLocal_Max = 0,dLocal_Max_WL;
			while (uiTest_Idx < cTarget.size() && cTarget.wl(uiTest_Idx) > dMin_WL)
			{
				if (cTarget.flux(uiTest_Idx) > dLocal_Max)
				{
					dLocal_Max = cTarget.flux(uiTest_Idx);
					dLocal_Max_WL = cTarget.wl(uiTest_Idx);
				}
				uiTest_Idx--;
			}
			dMin_WL = dLocal_Max_WL;
			uiTest_Idx = uiStart_Idx;
			dLocal_Max = 0;
			while (uiTest_Idx < cTarget.size() && cTarget.wl(uiTest_Idx) < dMax_WL)
			{
				if (cTarget.flux(uiTest_Idx) > dLocal_Max)
				{
					dLocal_Max = cTarget.flux(uiTest_Idx);
					dLocal_Max_WL = cTarget.wl(uiTest_Idx);
				}
				uiTest_Idx++;
			}
			dMax_WL = dLocal_Max_WL - 50.0; // go a little blueward of max on the red side

			ES::Spectrum cOutput(cTarget);
			ES::Spectrum cBest_Fit_Spectrum(cTarget);
			cOutput.zero_flux();
			cBest_Fit_Spectrum.zero_flux();
			bool bChi2 = ValidateChi2(cTarget,dMin_WL, dMax_WL);
			BEST_FIT_DATA	cBest_Fit;
			BEST_FIT_DATA	cFit;
			bool bRestart = false;
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			char lpszOutput_Filename[128];
			char lpszTitle[128];
			double	dPS_Vel, dPS_Temp, dPS_Ion_Temp, dPS_Log_Tau, dPS_Ion_Vmin, dPS_Ion_Vmax, dPS_Ion_Aux, dHVF_Ion_Temp, dHVF_Log_Tau, dHVF_Ion_Vmin, dHVF_Ion_Vmax, dHVF_Ion_Aux;
			int uiStartFile;

			bRestart = Read_Best_Fit_Cache(lpszTargetFileStripped,cTarget, uiIon, cBest_Fit, uiStartFile, i_iArg_Count, i_lpszArg_Values, bForce_Processing);
//			printf("%i\t%c\n",uiStartFile,bRestart?'t':'f');
			for (unsigned int uiI = uiStartFile; uiStartFile > 0 && uiI < i_iArg_Count; uiI++)
			{
				bRestart = false;
				printf("Reading %s",i_lpszArg_Values[uiI]);
				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.%s.xdataset",i_lpszArg_Values[uiI],lpszGroup_Data_File);
				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",i_lpszArg_Values[uiI]);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell,true);
				if (cOpacity_Map_Ejecta.GetNumElements() > 0)
				{
					double dDay;
					cFit.Reset();
					double dFit = CompareFits(lpszTargetFileStripped,i_lpszArg_Values[uiI], cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, dMin_WL, dMax_WL, bChi2, cFit,dSuggested_Time);
					cFit.SetBestFitFile(i_lpszArg_Values[uiI]);
					cFit.iBest_Fit_File = uiI;
					sprintf(lpszOutput_Filename,"%s.fit.%s.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[uiI],i_lpszArg_Values[2]);
					cFit.Output(lpszOutput_Filename,bChi2);
					if (cFit.dBest_Fit != -1.0 && cFit.dBest_Fit < cBest_Fit.dBest_Fit)
					{
						cBest_Fit = cFit;
					}
					if (bChi2)
						printf("\tChi^2 Fit = %.2e\n",dFit);
					else
						printf("\tVar   Fit = %.2e\n",dFit);
					sprintf(lpszOutput_Filename,"%s.fit.%s.%s.eps",i_lpszArg_Values[1],i_lpszArg_Values[uiI],i_lpszArg_Values[2]);
					Plot(cTarget, cFit.cBest_Fit_Spectrum, lpszOutput_Filename, dPlot_Min_WL, dPlot_Max_WL, lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, true);
				}
				else
					printf("Invalid file\n");
				Save_Best_Fit_Cache(lpszTargetFileStripped,cBest_Fit,uiI,i_lpszArg_Values,i_iArg_Count - 3);

			}

			//system("rm spectrafit.filelist.cache");// done - delete the cache
			fprintf(stdout,"spectrafit v%s\n",VERSION);
			cBest_Fit.Output(NULL,bChi2);
			if (cBest_Fit.iBest_Fit_File >= 0)
			{
				if (cBest_Fit.bBest_Fit_With_Shell)
					sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f thv %.2f Thv %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_Day,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_HVF_Log_Tau,cBest_Fit.dBest_Fit_HVF_Ion_Temp);
				else
					sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_Day,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp);
			}
			else
			{
				if (cBest_Fit.bBest_Fit_With_Shell)
					sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f thv %.2f Thv %.2f hvfvm %.2f hvfa %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_PS_Ion_Vmin,cBest_Fit.dBest_Fit_PS_Ion_Aux,cBest_Fit.dBest_Fit_HVF_Log_Tau,cBest_Fit.dBest_Fit_HVF_Ion_Temp,cBest_Fit.dBest_Fit_HVF_Ion_Vmin,cBest_Fit.dBest_Fit_HVF_Ion_Aux);
				else
					sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_PS_Ion_Vmin,cBest_Fit.dBest_Fit_PS_Ion_Aux);
			}
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.full.eps",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			Plot(cTarget, cBest_Fit.cBest_Fit_Spectrum, lpszOutput_Filename, cTarget.wl(0), cTarget.wl(cTarget.size() - 1), lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, false);
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.eps",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			Plot(cTarget, cBest_Fit.cBest_Fit_Spectrum, lpszOutput_Filename, dPlot_Min_WL, dPlot_Max_WL, lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, true);
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			cBest_Fit.Output(lpszOutput_Filename,bChi2);
			fprintf(stdout,"Best fit output to %s.\n",lpszOutput_Filename);
		}
		else
		{
			Usage("Invalid fit region");
		}
	}
	return 0;
}

