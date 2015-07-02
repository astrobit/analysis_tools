
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
#include <float.h>
#include <best_fit_data.h>
#include <line_routines.h>
#include <wordexp.h>
#include <model_spectra_db.h>


class GAUSS_FIT_PARAMETERS
{
public:
	double	m_dW_Ratio_1;
	double	m_dW_Ratio_2;
	
	double	m_dH_Ratio_1;
	double	m_dH_Ratio_2;
	GAUSS_FIT_PARAMETERS(void)
	{
		m_dW_Ratio_1 = m_dW_Ratio_2 = m_dH_Ratio_1 = m_dH_Ratio_2 = 0.0;
	}

	GAUSS_FIT_PARAMETERS(const double & i_dWL_1, const double & i_dStrength_1, const double & i_dWL_2, const double & i_dStrength_2, const double & i_dWL_3 = 0.0, const double & i_dStrength_3 = 0.0)
	{
		m_dW_Ratio_1 = i_dWL_1 / i_dWL_2;
		m_dW_Ratio_2 = i_dWL_3 / i_dWL_2;
		m_dH_Ratio_1 = i_dStrength_1 / i_dStrength_2;
		m_dH_Ratio_2 = i_dStrength_2 / i_dStrength_2;
	}
};

GAUSS_FIT_PARAMETERS	g_cgfpCaNIR(8662.14,160.0,8542.09,170.0,8498.02,130.0);
GAUSS_FIT_PARAMETERS	g_cgfpCaHK(3968.47,1.0,3968.47,1.0); //@@TODO Get strengths for H&K lines
//@@TODO: Set up paramters for other features

XVECTOR Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData)
{
	XVECTOR vOut;

	vOut.Set_Size(4); // function, and derivatives wrt each a parameter

	GAUSS_FIT_PARAMETERS *lpvParam = (GAUSS_FIT_PARAMETERS *) i_lpvData;
	double	dDelx_X, dDel_X_Sigma_1, dDel_X_Sigma_2, dDel_X_Sigma_3 = 0.0;
	double	dExp_1, dExp_2, dExp_3 = 0.0;
	double	dDel_X;
	double	dInv_Sigma;

	dInv_Sigma = 1.0 / i_vA.Get(1);
	dDel_X = i_dX - i_vA.Get(2);
	dDel_X_Sigma_2 = dDel_X * dInv_Sigma;


	dDel_X = i_dX / lpvParam->m_dW_Ratio_1 - i_vA.Get(2);
	dDel_X_Sigma_1 = dDel_X * dInv_Sigma;

	dExp_1 = exp(-0.5 * dDel_X_Sigma_1 * dDel_X_Sigma_1) * lpvParam->m_dH_Ratio_1;
	dExp_2 = exp(-0.5 * dDel_X_Sigma_2 * dDel_X_Sigma_2);
	dExp_3 = 0.0;

	if (lpvParam->m_dW_Ratio_2 != 0.0)
	{
		dDel_X = i_dX / lpvParam->m_dW_Ratio_2 - i_vA.Get(2);
		dDel_X_Sigma_3 = dDel_X * dInv_Sigma;
		dExp_3 = exp(-0.5 * dDel_X_Sigma_3 * dDel_X_Sigma_3) * lpvParam->m_dH_Ratio_2;
	}
	vOut.Set(0,i_vA.Get(0) * (dExp_1 + dExp_2 + dExp_3));
	vOut.Set(1,dExp_1 + dExp_2 + dExp_3);
	vOut.Set(2,i_vA.Get(0) * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3 * dDel_X_Sigma_3));
	vOut.Set(3,i_vA.Get(0) * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3));
	return vOut;
}
XVECTOR Multi_Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData)
{
	XVECTOR vOut;
	vOut.Set_Size(7);
	XVECTOR vOut1;
	XVECTOR vOut2;

	XVECTOR vA_lcl;
	vA_lcl.Set_Size(3);
	vA_lcl.Set(0,i_vA.Get(3));
	vA_lcl.Set(1,i_vA.Get(4));
	vA_lcl.Set(2,i_vA.Get(5));

	vOut1 = Gaussian(i_dX,i_vA,i_lpvData);
	vOut2 = Gaussian(i_dX,vA_lcl,i_lpvData);

	vOut.Set(0,vOut1.Get(0) + vOut2.Get(0));
	vOut.Set(1,vOut1.Get(1));
	vOut.Set(2,vOut1.Get(2));
	vOut.Set(3,vOut1.Get(3));
	vOut.Set(4,vOut2.Get(1));
	vOut.Set(5,vOut2.Get(2));
	vOut.Set(6,vOut2.Get(3));
	return vOut;
}

class FIT_DATA
{
public:
	double * m_lpdWavelength;
	double * m_lpdFlux;
	unsigned int m_uiNum_Points;

	GAUSS_FIT_PARAMETERS	cGauss_Parameters;
};

XVECTOR	Error(const XVECTOR & i_vA, void * i_lpvData)
{

	FIT_DATA * lpcfdData = (FIT_DATA *) i_lpvData;
	XVECTOR vError;
	vError.Set_Size(7);
	XVECTOR vOut;
	vOut.Set_Size(7);
	vOut.Set(0,0.0);
	vOut.Set(1,0.0);
	vOut.Set(2,0.0);
	vOut.Set(3,0.0);
	vOut.Set(4,0.0);
	vOut.Set(5,0.0);
	vOut.Set(6,0.0);

	for (unsigned int uiI = 0; uiI < lpcfdData->m_uiNum_Points; uiI++)
	{
		vError = Multi_Gaussian(lpcfdData->m_lpdWavelength[uiI],i_vA,&(lpcfdData->cGauss_Parameters));
		vError.Set(0,vError.Get(0) - lpcfdData->m_lpdFlux[uiI]); // subtract known flux
		vOut += vError;
	}
	return vOut;
}

int main(int i_iArg_Count, char * i_lpszArg_Values[])
{
	msdb::DATABASE cMSDB(true);

	double	dDay = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--day",1.0);
	double	dPS_Velocity = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ps-velocity",-1.0);
	double	dPS_Temp = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ps-temp",1.0);
	double	dEjecta_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-scalar",1.0);
	double	dShell_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-scalar",1.0);
    unsigned int uiPS_Velocity_Ion_State = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--use-computed-ps-velocity",3);

	// get model list string
	const char *	lpszModel_List_String = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--models");
	enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};
	tFit_Region	eFit_Region = OTHER;
	unsigned int uiIon = 0;
	char lpszOutput_Name[256];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output-name",lpszOutput_Name,sizeof(lpszOutput_Name),"");

	char * lpszCursor = (char *)lpszModel_List_String;
	// count number of models to process
	unsigned int uiModel_Count = 0;
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			uiModel_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
			lpszCursor++; // bypass comma
	}
	// allocate model name pointer list
	const char ** lpszModel_List = new const char *[uiModel_Count];
	// count number of models to process
	uiModel_Count = 0;
	lpszCursor = (char *)lpszModel_List_String; // reset cursor
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// add pointer to model name and count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			lpszModel_List[uiModel_Count] = lpszCursor;
			uiModel_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
		{
			lpszCursor[0] = 0;
			lpszCursor++; // bypass comma
		}
	}

	// Identify which line we are interested in.
	char lpszFitRegion[8];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--line-region",lpszFitRegion,sizeof(lpszFitRegion),"");

	if (strcmp(lpszFitRegion,"CaHK") == 0 ||
		strcmp(lpszFitRegion,"CaNIR") == 0)
	{
		uiIon = 2001;
		eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
	}
	else if (strcmp(lpszFitRegion,"Si6355") == 0)
	{
		uiIon = 1401;
		eFit_Region = SI6355;
	}
	else if (strcmp(lpszFitRegion,"Si5968") == 0)
	{
		uiIon = 1401;
		eFit_Region = SI5968;
	}
	else if (strcmp(lpszFitRegion,"OI8446") == 0)
	{
		uiIon = 800;
		eFit_Region = OINIR;
	}
	// set wavelength reference and a normalization point
	double dNorm_WL = -1.0;
	double dWL_Ref;
	double dRange_Min;
	double dRange_Max;
	msdb::USER_PARAMETERS	cParam;
	
	cParam.m_uiIon = uiIon;
	cParam.m_dTime_After_Explosion = dDay;
//	cParam.m_dPhotosphere_Velocity_kkms = dPS_Velocity;
	cParam.m_dPhotosphere_Temp_kK = dPS_Temp;
	cParam.m_dEjecta_Log_Scalar = dEjecta_Scalar;
	cParam.m_dShell_Log_Scalar = dShell_Scalar;
	cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cParam.m_dShell_Effective_Temperature_kK = 10.0;

	switch (eFit_Region)
	{
	case CANIR:
		dNorm_WL = 7750.0;
		dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		dRange_Min = 7700.0;
		dRange_Max = 9200.0;
		cParam.m_eFeature = msdb::CaNIR;
		break;
	case CAHK:
		dNorm_WL = 3750.0;
		dWL_Ref = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
		dRange_Min = 1800.0;
		dRange_Max = 4000.0;
		cParam.m_eFeature = msdb::CaHK;
		break;
	case SI5968:
		dNorm_WL = 5750.0;
		dWL_Ref = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
		dRange_Min = 5000.0;
		dRange_Max = 7000.0;
		break;
	case SI6355:
		dNorm_WL = 5750.0;
		dWL_Ref = (6348.85 * pow(10.0,0.149)  + 6373.12 * pow(10.0,-0.082)) / (pow(10.0,0.149) + pow(10.0,-0.082));
		dRange_Min = 5000.0;
		dRange_Max = 7000.0;
		cParam.m_eFeature = msdb::Si6355;
		break;
	case OINIR:
		dNorm_WL = 7100.0;
		dWL_Ref = (7774.083 * pow(10.0,0.369) + 7776.305 * pow(10.0,0.223) + 7777.528 * pow(10.0,0.002)) / (pow(10.0,0.369) + pow(10.0,0.223) + pow(10.0,0.002));
		dRange_Min = 6000.0;
		dRange_Max = 8000.0;
		cParam.m_eFeature = msdb::O7773;
		break;
	}
	
	// generate model spectra
	if (dNorm_WL > 0.0)
	{
		cParam.m_dWavelength_Range_Lower_Ang = dRange_Min;
		cParam.m_dWavelength_Range_Upper_Ang = dRange_Max;
		cParam.m_dWavelength_Delta_Ang = 1.25;
		//printf("%f %f\n",dRange_Min,dRange_Max);
		//printf("%f %f %f\n", cParam.m_dWavelength_Range_Lower_Ang,cParam.m_dWavelength_Range_Upper_Ang,cParam.m_dWavelength_Delta_Ang);


		ES::Spectrum *lpcSpectrum = new ES::Spectrum[uiModel_Count];
		ES::Spectrum * lpcContinuum = new ES::Spectrum[uiModel_Count];
		double * lpdV_ps = new double[uiModel_Count];
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
			lpcContinuum[uiI] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);
		XVECTOR cParameters(7);
		XVECTOR cContinuum_Parameters(7);




		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			char	lpszPhotosphere_File_Ejecta[64];
			char	lpszModel_Name[64];
			XDATASET cOpacity_Map_Shell;
			XDATASET cOpacity_Map_Ejecta;

			lpcSpectrum[uiI] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);


			if (lpszModel_List != NULL && lpszModel_List[uiI] != NULL && lpszModel_List[uiI][0] != 0)
			{
				if (lpszModel_List[uiI][0] >= '0' && lpszModel_List[uiI][0] <= '9') // just a number, convert into "runXX")
				{
					lpszModel_Name[0] = 'r';
					lpszModel_Name[1] = 'u';
					lpszModel_Name[2] = 'n';
					strcpy(lpszModel_Name + 3,lpszModel_List[uiI]);
					cParam.m_uiModel_ID = atoi(lpszModel_List[uiI]);

				}
				else
				{
					strcpy(lpszModel_Name,lpszModel_List[uiI]);
					if (strncmp(lpszModel_Name,"run",3) == 0)
						cParam.m_uiModel_ID = atoi(&lpszModel_List[uiI][3]);
				}
				//printf("Param model id %i\n",cParam.m_uiModel_ID);
				switch (eFit_Region)
				{
				case CANIR:
				case CAHK:
				case SI6355:
				case SI5968:
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",lpszModel_Name);
					break;
				case OINIR:
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",lpszModel_Name);
					break;
				case OTHER:
					if (uiIon / 100 >= 14 && uiIon / 100 <= 20)
						sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",lpszModel_Name);
					else if (uiIon / 100 == 6)
						sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.C.xdataset",lpszModel_Name);
					else if (uiIon / 100 == 8)
						sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",lpszModel_Name);
					else if (uiIon / 100 >= 22)
						sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Fe.xdataset",lpszModel_Name);
					else if (uiIon / 100 >= 10 && uiIon / 100 <= 12)
						sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Mg.xdataset",lpszModel_Name);
					break;
				}
				if (dPS_Velocity > 0.0)
					cContinuum_Parameters.Set(0,dPS_Velocity);
				else
				{
					XDATASET cData;
					sprintf(lpszPhotosphere_File_Ejecta,"%s/photosphere.csv",lpszModel_Name);
					cData.ReadDataFile(lpszPhotosphere_File_Ejecta,false,false,',',1);
					bool bFound = false;
					for (unsigned int uiI = 0; uiI  < cData.GetNumElements() && !bFound; uiI++)
					{
						if (dDay == cData.GetElement(0,uiI))
						{
							cContinuum_Parameters.Set(0,cData.GetElement(uiPS_Velocity_Ion_State,uiI)*1e-9);
							bFound = true;
						}
					}
					if (!bFound)
					{
						fprintf(stderr,"Unable to find data in photosphere table for day %.2f for model %s\n",dDay,lpszModel_Name);
						exit(1);
					}
				}
				lpdV_ps[uiI] = cContinuum_Parameters.Get(0);
				cContinuum_Parameters.Set(1,dPS_Temp);
				cContinuum_Parameters.Set(2,-20.0); // PS ion log tau
				cContinuum_Parameters.Set(3,10.0); // PS ion exctication temp
				cContinuum_Parameters.Set(4,10.0); // PS ion vmin
				cContinuum_Parameters.Set(5,11.0); // PS ion vmax
				cContinuum_Parameters.Set(6,1.0); // PS ion vscale

				cParam.m_uiModel_ID = 0;
				cParam.m_dPhotosphere_Velocity_kkms = cContinuum_Parameters.Get(0);
				if (cMSDB.Get_Spectrum(cParam, msdb::CONTINUUM, lpcContinuum[uiI]) == 0)
				{
					printf("generating continuua\n");
					Generate_Synow_Spectra_Exp(lpcContinuum[uiI],2001,cContinuum_Parameters,lpcContinuum[uiI]); // ion irrelevant for this 
					printf("Adding to db\n");
					cMSDB.Add_Spectrum(cParam, msdb::CONTINUUM, lpcContinuum[uiI]);
					printf("done\n");
				}

				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",lpszModel_Name);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
				bool bShell = cOpacity_Map_Shell.GetNumElements() != 0;

				cParameters.Set_Size(bShell ? 7 : 5);
				cParameters.Set(0,dDay);
				cParameters.Set(1,cContinuum_Parameters.Get(0));
				cParameters.Set(2,dPS_Temp);

				cParameters.Set(3,10.0); // fix excitation temp
				cParameters.Set(4,dEjecta_Scalar);
				if (bShell)
				{
					cParameters.Set(5,10.0); // fix excitation temp
					cParameters.Set(6,dShell_Scalar);
				}

				if (cMSDB.Get_Spectrum(cParam, msdb::COMBINED, lpcSpectrum[uiI]) == 0)
				{
					Generate_Synow_Spectra(lpcSpectrum[uiI], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI]);
					printf("Adding to db for %s...",lpszModel_Name);
					fflush(stdout);
					msdb::dbid dbidID = cMSDB.Add_Spectrum(cParam, msdb::COMBINED, lpcSpectrum[uiI]);
					printf(".");
					fflush(stdout);
					if (!bShell)
					{
						cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, lpcSpectrum[uiI]);
						printf(".");
						fflush(stdout);
					}
					else
					{
						cParameters.Set(6,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI]);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, lpcSpectrum[uiI]);

						cParameters.Set(6,dShell_Scalar);
						cParameters.Set(4,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI]);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::SHELL_ONLY, lpcSpectrum[uiI]);
					}
					printf("Done\n");
				}


			}
		}

		epsplot::PAGE_PARAMETERS	cPlot_Parameters;
		epsplot::DATA cPlot;
		epsplot::AXIS_PARAMETERS	cX_Axis_Parameters;
		epsplot::AXIS_PARAMETERS	cY_Axis_Parameters;


		cPlot_Parameters.m_uiNum_Columns = 1;
		cPlot_Parameters.m_uiNum_Rows = 1;
		cPlot_Parameters.m_dWidth_Inches = 11.0;
		cPlot_Parameters.m_dHeight_Inches = 8.5;

		cX_Axis_Parameters.Set_Title("Wavelength [A]");
		cX_Axis_Parameters.m_dMajor_Label_Size = 24.0;
		cY_Axis_Parameters.Set_Title("Flux");
		cY_Axis_Parameters.m_dLower_Limit = 0.0;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);


		char lpszFilename[512];
		double ** lpdSpectra_WL = new double *[uiModel_Count];
		double ** lpdSpectra_Flux = new double *[uiModel_Count];
		double ** lpdContinuum_WL = new double *[uiModel_Count], ** lpdContinuum_Flux = new double *[uiModel_Count];
		unsigned int * lpuiSpectra_Count = new unsigned int[uiModel_Count], * lpuiContinuum_Count = new unsigned int[uiModel_Count];
		int iColor = (int)epsplot::BLACK;
		int iStipple = (int)epsplot::SOLID;
		epsplot::LINE_PARAMETERS cLine_Parameters;

		// Get spectral data in the desired range
		cLine_Parameters.m_dWidth = 1.0;
//		cPlot.Set_Plot_Data(lpdContinuum_WL, lpdContinuum_Flux, uiNum_Points_Continuum, cLine_Parameters, 0, 0);
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			lpdContinuum_WL[uiI] = NULL;
			lpdContinuum_Flux[uiI] = NULL;
			lpuiContinuum_Count[uiI] = 0;
			lpdSpectra_WL[uiI] = NULL;
			lpdSpectra_Flux[uiI] = NULL;
			lpuiSpectra_Count[uiI] = 0;
			Get_Spectra_Data(lpcContinuum[uiI], lpdContinuum_WL[uiI], lpdContinuum_Flux[uiI], lpuiContinuum_Count[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI], lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], dRange_Min, dRange_Max);
			for (unsigned int uiJ = 0; uiJ <= lpuiSpectra_Count[uiI]; uiJ++)
				lpdSpectra_Flux[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];

			cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + (uiI % 7));
			cLine_Parameters.m_eStipple = (epsplot::STIPPLE)(epsplot::SOLID + (uiI % 8));
			cPlot.Set_Plot_Data(lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
		}

	

		wordexp_t cResults;
		if (wordexp(lpszOutput_Name,&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
		{
			strcpy(lpszOutput_Name, cResults.we_wordv[0]);
			wordfree(&cResults);
		}

		sprintf(lpszFilename,"%s.caption.tex",lpszOutput_Name);
		FILE * fileCaption = fopen(lpszFilename,"wt");
		fprintf(fileCaption,"\\caption{Comparison of models ");
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
            if (uiI != 0)
				fprintf(fileCaption,", ");
			fprintf(fileCaption,"%s (",lpszModel_List[uiI]);
			epsplot::COLOR eColor = (epsplot::COLOR)(epsplot::BLACK + (uiI % 7));
			epsplot::STIPPLE eStipple = (epsplot::STIPPLE)(epsplot::SOLID + (uiI % 8));
			switch (eColor)
			{
			case epsplot::BLACK:
				fprintf(fileCaption,"black, ");
				break;
			case epsplot::RED:
				fprintf(fileCaption,"red, ");
				break;
			case epsplot::GREEN:
				fprintf(fileCaption,"green, ");
				break;
			case epsplot::BLUE:
				fprintf(fileCaption,"blue, ");
				break;
			case epsplot::CYAN:
				fprintf(fileCaption,"cyan, ");
				break;
			case epsplot::YELLOW:
				fprintf(fileCaption,"yellow, ");
				break;
			case epsplot::MAGENTA:
				fprintf(fileCaption,"magenta, ");
				break;
			}
			switch (eColor)
			{
			case epsplot::SOLID:
				fprintf(fileCaption,"solid)");
				break;
			case epsplot::SHORT_DASH:
				fprintf(fileCaption,"short dash)");
				break;
			case epsplot::LONG_DASH:
				fprintf(fileCaption,"long dash)");
				break;
			case epsplot::LONG_SHORT_DASH:
				fprintf(fileCaption,"long dash--short dash)");
				break;
			case epsplot::DOTTED:
				fprintf(fileCaption,"dotted)");
				break;
			case epsplot::SHORT_DASH_DOTTED:
				fprintf(fileCaption,"short dash--dot)");
				break;
			case epsplot::LONG_DASH_DOTTED:
				fprintf(fileCaption,"long dash--dot)");
				break;
			case epsplot::LONG_SHORT_DASH_DOTTED:
				fprintf(fileCaption,"long dash--short dash--dot)");
				break;
			}
		}
		fprintf(fileCaption," at %.2f days after explosion with",dDay);
		if (dPS_Velocity > 0.0)
			fprintf(fileCaption, " $v_{ps} = %.2f\\kkms$",dPS_Velocity);
		fprintf(fileCaption," $T_{ps} = %.0f,000\\K$, $\\log C_S = %.2f$, $\\log C_E = %.2f$.}\n",dDay,cContinuum_Parameters.Get(0),dPS_Temp,dShell_Scalar,dEjecta_Scalar);
		if (dPS_Velocity <= 0.0)
		{
			fprintf(fileCaption,"% v_ps =");
			for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
			{
				if (uiI != 0)
					fprintf(fileCaption,", ");
				fprintf(fileCaption,"%.2e\\kkms (%s)",lpdV_ps[uiI],lpszModel_List[uiI]);
			}
			fprintf(fileCaption,"\n");
		}
		fclose(fileCaption);

		cPlot.Set_Plot_Filename(lpszOutput_Name);
		cPlot.Plot(cPlot_Parameters);
		sprintf(lpszFilename,"%s.csv",lpszOutput_Name);
		FILE * fileOut = fopen(lpszFilename,"wt");
		for (unsigned int uiI = 0; uiI < lpcContinuum[0].size(); uiI++)
		{
			fprintf(fileOut, "%f",lpdContinuum_WL[0][uiI]);
			for (unsigned int uiJ = 0; uiJ < uiModel_Count; uiJ++)
			{
				fprintf(fileOut, " %f",lpdContinuum_Flux[uiJ][uiI]);
				fprintf(fileOut, " %f",lpdSpectra_Flux[uiJ][uiI]);
			}
			fprintf(fileOut, "\n");
		}
		fclose(fileOut);

		// compute pEW values for features of interest.
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			double dpEW_True = 0.0;
			// first generate pEW based on flattened spectra and excluding the p Cygni peak
			for (unsigned int uiJ = 0; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
			{
				double dFlat_Flux = lpdSpectra_Flux[uiI][uiJ] / lpdContinuum_Flux[uiI][uiJ];
				if (dFlat_Flux < 1.0)
					dpEW_True += (1.0 - dFlat_Flux) * cParam.m_dWavelength_Delta_Ang;
			}
			// perform Gaussian fitting routine
		}


		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			delete [] lpdSpectra_WL[uiI];
			delete [] lpdSpectra_Flux[uiI];
			delete [] lpdContinuum_WL[uiI];
			delete [] lpdContinuum_Flux[uiI];
		}
		delete [] lpdSpectra_WL;
		delete [] lpdSpectra_Flux;
		delete [] lpuiSpectra_Count;
		delete [] lpdContinuum_WL;
		delete [] lpdContinuum_Flux;
		delete [] lpuiContinuum_Count;
	}
	

	return 0;
}
