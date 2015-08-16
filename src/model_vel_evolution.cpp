
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include <xfit.h>
#include "ES_Synow.hh"
#include "ES_Generic_Error.hh"
#include <Plot_Utilities.h>
#include <float.h>
#include <best_fit_data.h>
#include <line_routines.h>
#include <wordexp.h>
#include <model_spectra_db.h>
#include <opacity_profile_data.h>

class GAUSS_FIT_PARAMETERS
{
public:
	double	m_dWl[3];
	double	m_dStr[3];
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
		m_dWl[0] = i_dWL_1;
		m_dWl[1] = i_dWL_2;
		m_dWl[2] = i_dWL_3;

		m_dStr[0] = i_dStrength_1;
		m_dStr[1] = i_dStrength_2;
		m_dStr[2] = i_dStrength_3;

		m_dW_Ratio_1 = i_dWL_1 / i_dWL_2;
		m_dW_Ratio_2 = i_dWL_3 / i_dWL_2;
		m_dH_Ratio_1 = i_dStrength_1 / i_dStrength_2;
		m_dH_Ratio_2 = i_dStrength_2 / i_dStrength_2;
	}
};

GAUSS_FIT_PARAMETERS	g_cgfpCaNIR(8662.14,160.0,8542.09,170.0,8498.02,130.0);
GAUSS_FIT_PARAMETERS	g_cgfpCaHK(3934.77,230.0,3969.59,220.0);
GAUSS_FIT_PARAMETERS	g_cgfpOINIR(774.08,870.0,776.31,810.0,7777.53,750.0);
GAUSS_FIT_PARAMETERS	g_cgfpSi6355(6348.85,1000.0,6373.12,1000.0);
GAUSS_FIT_PARAMETERS	g_cgfpSi5968(5959.21,500.0,5980.59,500.0);


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
	XVECTOR vOut1;
	XVECTOR vOut2;

	XVECTOR vA_lcl;
	if (i_vA.Get_Size() == 6)
	{
		vOut.Set_Size(7);
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
	}
	else if (i_vA.Get_Size() == 3)
	{
		vOut = Gaussian(i_dX,i_vA,i_lpvData);
	}
	return vOut;
}

double	Compute_Velocity(const double & i_dObserved_Wavelength, const double & i_dRest_Wavelength)
{
	double	dz = i_dObserved_Wavelength / i_dRest_Wavelength;
	double	dz_sqr = dz * dz;
	return (2.99792458e5 * (dz_sqr - 1.0) / (dz_sqr + 1.0));
}


class FEATURE_PARAMETERS
{
public:
	double	m_d_pEW;
	double	m_dVmin;
	double	m_dFlux_vmin;

	void Process_Vmin(const double &i_dWL, const double &i_dFlux, const double & i_dWL_Ref, bool i_bProcess = true)
	{
		if (i_bProcess)
		{
			if (i_dFlux < m_dFlux_vmin)
			{
				m_dVmin = Compute_Velocity(i_dWL,i_dWL_Ref);
				m_dFlux_vmin = i_dFlux;
			}
		}
	}
	void Process_pEW(const double & i_dFlux, const double & i_dDelta_WL)
	{
		m_d_pEW += (1.0 - i_dFlux) * i_dDelta_WL;
	}
	void	Reset(void)
	{
		m_d_pEW = 0.0;
		m_dVmin = 0.0;
		m_dFlux_vmin = DBL_MAX;
	}
	FEATURE_PARAMETERS(void) { Reset();}
};


void Process_Model_List(const char * lpszModel_List_String, const char ** &o_lpszModel_List, unsigned int &o_uiModel_Count)
{
	char * lpszCursor = (char *)lpszModel_List_String;
	// count number of models to process
	o_uiModel_Count = 0;
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_uiModel_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
			lpszCursor++; // bypass comma
	}
	// allocate model name pointer list
	o_lpszModel_List = new const char *[o_uiModel_Count];
	// count number of models to process
	o_uiModel_Count = 0;
	lpszCursor = (char *)lpszModel_List_String; // reset cursor
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// add pointer to model name and count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_lpszModel_List[o_uiModel_Count] = lpszCursor;
			o_uiModel_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
		{
			lpszCursor[0] = 0;
			lpszCursor++; // bypass comma
		}
	}
}
int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA::GROUP eScalar_Type;
	msdb::DATABASE cMSDB(true);
	GAUSS_FIT_PARAMETERS * lpgfpParamters;
	double	dDay = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--day",1.0);
	double	dPS_Velocity = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ps-velocity",-1.0);
	double	dPS_Temp = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ps-temp",1.0);
	double	dEjecta_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-scalar",1.0);
	double	dShell_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-scalar",1.0);
	bool	bFull_Spectrum = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--full-spectrum");
    unsigned int uiPS_Velocity_Ion_State = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--use-computed-ps-velocity",3);
	const char *	lpszRef_Model = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-model");
	bool	bNo_Ref_Model = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--no-ref-model");
	char lpszRef_Model_Extended[128] = {0};
	const char *	lpszIon_List = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ions");
	char lpszShell_Abundance[128];
	char lpszEjecta_Abundance[128];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-abundance",lpszShell_Abundance,sizeof(lpszShell_Abundance),"");
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-abundance",lpszEjecta_Abundance,sizeof(lpszShell_Abundance),"");


	if (lpszRef_Model && lpszRef_Model[0] >= '0' && lpszRef_Model[0] <= '9') // just a number, convert into "runXX")
	{
		lpszRef_Model_Extended[0] = 'r';
		lpszRef_Model_Extended[1] = 'u';
		lpszRef_Model_Extended[2] = 'n';
		strcpy(lpszRef_Model_Extended + 3,lpszRef_Model);

	}
	else if (lpszRef_Model)
	{
		strcpy(lpszRef_Model_Extended,lpszRef_Model);
	}

//	printf("here 0\n");
	// get model list string
	const char *	lpszModel_List_String = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--models");
	const char ** lpszModel_List;
	unsigned int uiModel_Count;
	Process_Model_List(lpszModel_List_String, lpszModel_List, uiModel_Count);

	enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};
	tFit_Region	eFit_Region = OTHER;
	unsigned int uiIon = 0;
	char lpszOutput_Name[256];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output-name",lpszOutput_Name,sizeof(lpszOutput_Name),"");



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
	cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cParam.m_dShell_Effective_Temperature_kK = 10.0;

	switch (eFit_Region)
	{
	case CANIR:
		dNorm_WL = 7750.0;
		dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		dRange_Min = 7000.0;
		dRange_Max = 9500.0;
		cParam.m_eFeature = msdb::CaNIR;
		lpgfpParamters = &g_cgfpCaNIR;
		eScalar_Type = OPACITY_PROFILE_DATA::SILICON;
		break;
	case CAHK:
		dNorm_WL = 3750.0;
		dWL_Ref = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
		dRange_Min = 1800.0;
		dRange_Max = 4000.0;
		cParam.m_eFeature = msdb::CaHK;
		lpgfpParamters = &g_cgfpCaHK;
		eScalar_Type = OPACITY_PROFILE_DATA::SILICON;
		break;
	case SI5968:
		dNorm_WL = 5750.0;
		dWL_Ref = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
		dRange_Min = 5000.0;
		dRange_Max = 7000.0;
		lpgfpParamters = &g_cgfpSi5968;
		eScalar_Type = OPACITY_PROFILE_DATA::SILICON;
		break;
	case SI6355:
		dNorm_WL = 5750.0;
		dWL_Ref = (6348.85 * pow(10.0,0.149)  + 6373.12 * pow(10.0,-0.082)) / (pow(10.0,0.149) + pow(10.0,-0.082));
		dRange_Min = 5000.0;
		dRange_Max = 7000.0;
		cParam.m_eFeature = msdb::Si6355;
		lpgfpParamters = &g_cgfpSi6355;
		eScalar_Type = OPACITY_PROFILE_DATA::SILICON;
		break;
	case OINIR:
		dNorm_WL = 7100.0;
		dWL_Ref = (7774.083 * pow(10.0,0.369) + 7776.305 * pow(10.0,0.223) + 7777.528 * pow(10.0,0.002)) / (pow(10.0,0.369) + pow(10.0,0.223) + pow(10.0,0.002));
		dRange_Min = 6000.0;
		dRange_Max = 8000.0;
		cParam.m_eFeature = msdb::O7773;
		lpgfpParamters = &g_cgfpOINIR;
		eScalar_Type = OPACITY_PROFILE_DATA::OXYGEN;
		break;
	}

	if (bFull_Spectrum)
	{
		dRange_Min = 2000.0;
		dRange_Max = 10000.0;
	}
	
	// generate model spectra
	if (dNorm_WL > 0.0)
	{
		cParam.m_dWavelength_Range_Lower_Ang = dRange_Min;
		cParam.m_dWavelength_Range_Upper_Ang = dRange_Max;
		cParam.m_dWavelength_Delta_Ang = 1.25;
		//printf("%f %f\n",dRange_Min,dRange_Max);
		//printf("%f %f %f\n", cParam.m_dWavelength_Range_Lower_Ang,cParam.m_dWavelength_Range_Upper_Ang,cParam.m_dWavelength_Delta_Ang);


		ES::Spectrum ** lpcSpectrum = new ES::Spectrum *[uiModel_Count];
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			lpcSpectrum[uiI] = new ES::Spectrum[4];
			lpcSpectrum[uiI][0] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);
			lpcSpectrum[uiI][1] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);
			lpcSpectrum[uiI][2] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);
			lpcSpectrum[uiI][3] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, cParam.m_dWavelength_Delta_Ang);
		}
		double * lpdV_ps = new double[uiModel_Count];
		XVECTOR cParameters(7);
		XVECTOR cContinuum_Parameters(7);

//		printf("here 1\n");

		double	dEjecta_Scalar_Ref = -1.0;
		double	dShell_Scalar_Ref = -1.0;
		double	dEjecta_Scalar_Prof = -1.0;
		double	dShell_Scalar_Prof = -1.0;
		OPACITY_PROFILE_DATA	cOpacity_Profile;

		if (lpszRef_Model)
		{
			char lpszOpacity_Profile[128];
			sprintf(lpszOpacity_Profile,"%s/opacity_map_scalars.opdata",lpszRef_Model_Extended);
			cOpacity_Profile.Load(lpszOpacity_Profile);
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(eScalar_Type);
			dShell_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);
		}
		printf("model count %i\n",uiModel_Count);
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			char	lpszPhotosphere_File_Ejecta[64];
			char	lpszModel_Name[64];
			XDATASET cOpacity_Map_Shell;
			XDATASET cOpacity_Map_Ejecta;


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
					for (unsigned int uiJ = 0; uiJ  < cData.GetNumElements() && !bFound; uiJ++)
					{
						if (dDay == cData.GetElement(0,uiJ))
						{
							cContinuum_Parameters.Set(0,cData.GetElement(uiPS_Velocity_Ion_State,uiJ)*1e-8);
							bFound = true;
						}
					}
					if (!bFound)
					{
						fprintf(stderr,"Unable to find data in photosphere table for day %.2f for model %s\n",dDay,lpszModel_Name);
						exit(1);
					}
				}
				char lpszOpacity_Profile[128];
				sprintf(lpszOpacity_Profile,"%s/opacity_map_scalars.opdata",lpszModel_Name);
				cOpacity_Profile.Load(lpszOpacity_Profile);
				dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(eScalar_Type);
				dShell_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);
				if (dEjecta_Scalar_Ref == -1.0)
					dEjecta_Scalar_Ref = dEjecta_Scalar_Prof;
				if (dShell_Scalar_Ref == -1.0)
					dShell_Scalar_Ref = dShell_Scalar_Prof;
//				printf("%.2e/%.2e [%.2e] %.2e/%.2e [%.2e]\n",dEjecta_Scalar_Prof,dEjecta_Scalar_Ref,log10(dEjecta_Scalar_Prof/dEjecta_Scalar_Ref),dShell_Scalar_Prof,dShell_Scalar_Ref,log10(dShell_Scalar_Prof/dShell_Scalar_Ref));


				lpdV_ps[uiI] = cContinuum_Parameters.Get(0);
				cContinuum_Parameters.Set(1,dPS_Temp);
				cContinuum_Parameters.Set(2,-20.0); // PS ion log tau
				cContinuum_Parameters.Set(3,10.0); // PS ion exctication temp
				cContinuum_Parameters.Set(4,lpdV_ps[uiI]); // PS ion vmin
				cContinuum_Parameters.Set(5,80.0); // PS ion vmax
				cContinuum_Parameters.Set(6,1.0); // PS ion vscale

				cParam.m_uiModel_ID = 0;
				cParam.m_dPhotosphere_Velocity_kkms = cContinuum_Parameters.Get(0);
				if (cMSDB.Get_Spectrum(cParam, msdb::CONTINUUM, lpcSpectrum[uiI][0]) == 0)
				{
					printf("generating continuua\n");
					Generate_Synow_Spectra_Exp(lpcSpectrum[uiI][0],2001,cContinuum_Parameters,lpcSpectrum[uiI][0]); // ion irrelevant for this 
					printf("Adding to db\n");
					cMSDB.Add_Spectrum(cParam, msdb::CONTINUUM, lpcSpectrum[uiI][0]);
					printf("done\n");
				}

				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",lpszModel_Name);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
				bool bShell = cOpacity_Map_Shell.GetNumElements() != 0;

				cParam.m_dEjecta_Log_Scalar = dEjecta_Scalar;
				cParam.m_dShell_Log_Scalar = dShell_Scalar;
				if (!bNo_Ref_Model)
				{
					cParam.m_dEjecta_Log_Scalar += log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref);
					cParam.m_dShell_Log_Scalar += log10(dShell_Scalar_Prof / dShell_Scalar_Ref);
				}


				cParameters.Set_Size(bShell ? 7 : 5);
				cParameters.Set(0,dDay);
				cParameters.Set(1,cContinuum_Parameters.Get(0));
				cParameters.Set(2,dPS_Temp);

				cParameters.Set(3,10.0); // fix excitation temp
				cParameters.Set(4,cParam.m_dEjecta_Log_Scalar);
//				printf("%.5e %.5e\n",dEjecta_Scalar,dEjecta_Scalar + log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref));
				if (bShell)
				{
					cParameters.Set(5,10.0); // fix excitation temp
					cParameters.Set(6,cParam.m_dShell_Log_Scalar);
				}

				if (cMSDB.Get_Spectrum(cParam, msdb::COMBINED, lpcSpectrum[uiI][1]) == 0)
				{
					Generate_Synow_Spectra(lpcSpectrum[uiI][1], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][1]);
					printf("Adding to db for %s...",lpszModel_Name);
					fflush(stdout);
					msdb::dbid dbidID = cMSDB.Add_Spectrum(cParam, msdb::COMBINED, lpcSpectrum[uiI][1]);
					printf(".");
					fflush(stdout);
					if (!bShell)
					{
						lpcSpectrum[uiI][2] = lpcSpectrum[uiI][1];
						cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]);
						printf(".");
						fflush(stdout);
					}
					else
					{
						cParameters.Set(6,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI][2], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][2]);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]);

						cParameters.Set(6,dShell_Scalar);
						cParameters.Set(4,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI][3], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][3]);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::SHELL_ONLY, lpcSpectrum[uiI][3]);
					}
					printf("Done\n");
				}
				else
				{
					if (cMSDB.Get_Spectrum(cParam, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]) == 0)
					{
						cParameters.Set(6,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI][2], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][1]);
						printf("Adding E-O to db for %s...",lpszModel_Name);
						fflush(stdout);
						msdb::dbid dbidID = cMSDB.Add_Spectrum(cParam, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]);
						cParameters.Set(6,dShell_Scalar);
					}
					if (bShell)
					{
						if (cMSDB.Get_Spectrum(cParam, msdb::SHELL_ONLY, lpcSpectrum[uiI][3]) == 0)
						{
							cParameters.Set(4,-40.0);
							Generate_Synow_Spectra(lpcSpectrum[uiI][3], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][3]);
							printf("Adding S-O to db for %s...",lpszModel_Name);
							fflush(stdout);
							msdb::dbid dbidID = cMSDB.Add_Spectrum(cParam, msdb::SHELL_ONLY, lpcSpectrum[uiI][3]);
							cParameters.Set(4,dEjecta_Scalar);
						}
					}
				}
			}
		}

//		printf("here 2\n");
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
		cY_Axis_Parameters.m_dUpper_Limit = 2.0;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);


		char lpszFilename[512];
		double ** lpdSpectra_WL = new double *[uiModel_Count];
		double ** lpdSpectra_Flux = new double *[uiModel_Count];
		double ** lpdSpectra_WL_EO = new double *[uiModel_Count];
		double ** lpdSpectra_Flux_EO = new double *[uiModel_Count];
		double ** lpdSpectra_WL_SO = new double *[uiModel_Count];
		double ** lpdSpectra_Flux_SO = new double *[uiModel_Count];
		double ** lpdContinuum_WL = new double *[uiModel_Count], ** lpdContinuum_Flux = new double *[uiModel_Count];
		unsigned int * lpuiSpectra_Count = new unsigned int[uiModel_Count], * lpuiContinuum_Count = new unsigned int[uiModel_Count], * lpuiSpectra_Count_EO = new unsigned int[uiModel_Count], * lpuiSpectra_Count_SO = new unsigned int[uiModel_Count];
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
			lpdSpectra_WL_EO[uiI] = NULL;
			lpdSpectra_Flux_EO[uiI] = NULL;
			lpdSpectra_WL_SO[uiI] = NULL;
			lpdSpectra_Flux_SO[uiI] = NULL;
			lpuiSpectra_Count[uiI] = 0;
			lpuiSpectra_Count[uiI] = 0;
			lpuiSpectra_Count_EO[uiI] = 0;
			lpuiSpectra_Count_SO[uiI] = 0;
			Get_Spectra_Data(lpcSpectrum[uiI][0], lpdContinuum_WL[uiI], lpdContinuum_Flux[uiI], lpuiContinuum_Count[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][1], lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][2], lpdSpectra_WL_EO[uiI], lpdSpectra_Flux_EO[uiI], lpuiSpectra_Count_EO[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][3], lpdSpectra_WL_SO[uiI], lpdSpectra_Flux_SO[uiI], lpuiSpectra_Count_SO[uiI], dRange_Min, dRange_Max);
			for (unsigned int uiJ = 0; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
			{
				lpdSpectra_Flux[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
				lpdSpectra_Flux_EO[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
				lpdSpectra_Flux_SO[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
//				printf("%.0f %.4f %.4f %.4f\n",lpdContinuum_WL[uiI][uiJ],lpdContinuum_Flux[uiI][uiJ],lpcSpectrum[uiI][1].flux(uiJ),lpdSpectra_Flux[uiI][uiJ]);
			}
			cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + (uiI % 7));
			cLine_Parameters.m_eStipple = (epsplot::STIPPLE)(epsplot::SOLID + (uiI % 8));
			cPlot.Set_Plot_Data(lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
		}
//		printf("here 3\n");
//		fflush(stdout);

	

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
			switch (eStipple)
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
		fprintf(fileCaption," $T_{ps} = %.0f,000\\K$, $\\log C_S = %.2f$, $\\log C_E = %.2f$.}\n",dPS_Temp,dShell_Scalar,dEjecta_Scalar);
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
		sprintf(lpszFilename,"%s.dat",lpszOutput_Name);
		FILE * fileOut = fopen(lpszFilename,"wt");
		fprintf(fileOut,"wl");
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			fprintf(fileOut," %s-cont",lpszModel_List[uiI]);
			fprintf(fileOut," %s-comb",lpszModel_List[uiI]);
			fprintf(fileOut," %s-comb-flat",lpszModel_List[uiI]);
			fprintf(fileOut," %s-EO",lpszModel_List[uiI]);
			fprintf(fileOut," %s-EO-flat",lpszModel_List[uiI]);
			fprintf(fileOut," %s-SO",lpszModel_List[uiI]);
			fprintf(fileOut," %s-SO-flat",lpszModel_List[uiI]);
		}
		fprintf(fileOut,"\n");
		for (unsigned int uiI = 0; uiI < lpuiContinuum_Count[0]; uiI++)
		{
			fprintf(fileOut, "%f",lpdContinuum_WL[0][uiI]);
			for (unsigned int uiJ = 0; uiJ < uiModel_Count; uiJ++)
			{
				fprintf(fileOut, " %f",lpcSpectrum[uiJ][0].flux(uiI));
				fprintf(fileOut, " %f",lpcSpectrum[uiJ][1].flux(uiI));
				fprintf(fileOut, " %f",lpdSpectra_Flux[uiJ][uiI]);
				fprintf(fileOut, " %f",lpcSpectrum[uiJ][2].flux(uiI));
				fprintf(fileOut, " %f",lpdSpectra_Flux_EO[uiJ][uiI]);
				fprintf(fileOut, " %f",lpcSpectrum[uiJ][3].flux(uiI));
				fprintf(fileOut, " %f",lpdSpectra_Flux_SO[uiJ][uiI]);
			}
			fprintf(fileOut, "\n");
		}
		fclose(fileOut);

//		printf("here 4\n");
//		fflush(stdout);
		if (!bFull_Spectrum)
		{
			sprintf(lpszFilename,"%s.data.csv",lpszOutput_Name);
			FILE * fileData = fopen(lpszFilename,"wt");
			XVECTOR	vX, vY, vA, vW,vA_Single, vA_Flat, vA_Single_Flat;
			double	dSmin_Single = DBL_MAX;
			XSQUARE_MATRIX mCovariance_Matrix;
			XSQUARE_MATRIX mCovariance_Matrix_Single;
			XSQUARE_MATRIX mCovariance_Matrix_Flat;
			XSQUARE_MATRIX mCovariance_Matrix_Single_Flat;
			double	dSmin;
			double	dSmin_Flat;
			double	dSmin_Single_Flat;

			fprintf(fileData,"Model, pEW (Combined - flat), Vmin (Combined - flat), pEW (EO - flat), Vmin (EO - flat), pEW (SO - flat), Vmin (SO - flat), pEW (Combined), Vmin (combined), Vmin (EO), Vmin (SO), Vmin-HVF (Jeff), Vmin-PVF (Jeff), pEW-HVF (Jeff), pEW-PVF (Jeff), a_0 (Jeff), sigma a_0 (Jeff), a_1 (Jeff), sigma_a_1 (Jeff), a_2 (Jeff), sigma_a_2 (Jeff), a_3 (Jeff), sigma a_3 (Jeff), a_4 (Jeff), sigma_a_4 (Jeff), a_5 (Jeff), sigma_a_5 (Jeff), a_0 (Flat), sigma a_0 (Flat), a_1 (Flat), sigma_a_1 (Flat), a_2 (Flat), sigma_a_2 (Flat), a_3 (Flat), sigma a_3 (Flat), a_4 (Flat), sigma_a_4 (Flat), a_5 (Flat), sigma_a_5 (Flat)\n");

			// compute pEW values for features of interest.
			for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
			{
				FEATURE_PARAMETERS	cCombined_Flat;
				FEATURE_PARAMETERS	cEO_Flat;
				FEATURE_PARAMETERS	cSO_Flat;
				FEATURE_PARAMETERS	cCombined_Unflat;
				FEATURE_PARAMETERS	cEO_Unflat;
				FEATURE_PARAMETERS	cSO_Unflat;
				unsigned int uiContinuum_Blue_Idx = 0;
				unsigned int uiContinuum_Red_Idx = 0;
				bool	bIn_feature = false;
				double	dMin_Flux = DBL_MAX;
				double	dP_Cygni_Peak_Flux = 0.0;
				unsigned int uiMin_Flux_Idx = -1;
				double	dMin_Flux_Flat = DBL_MAX;
				unsigned int uiP_Cygni_Min_Idx = 0;
				// first generate pEW based on flattened spectra and excluding the p Cygni peak
				for (unsigned int uiJ = 0; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
				{
					double dFlat_Flux = lpdSpectra_Flux[uiI][uiJ];
					double dFlat_Flux_EO = lpdSpectra_Flux_EO[uiI][uiJ];
					double dFlat_Flux_SO = lpdSpectra_Flux_SO[uiI][uiJ];

					cCombined_Flat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpdSpectra_Flux[uiI][uiJ],dWL_Ref);
					cEO_Flat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpdSpectra_Flux_EO[uiI][uiJ],dWL_Ref);
					cSO_Flat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpdSpectra_Flux_SO[uiI][uiJ],dWL_Ref);

					cCombined_Flat.Process_pEW(lpdSpectra_Flux[uiI][uiJ],cParam.m_dWavelength_Delta_Ang);
					cEO_Flat.Process_pEW(lpdSpectra_Flux_EO[uiI][uiJ],cParam.m_dWavelength_Delta_Ang);
					cSO_Flat.Process_pEW(lpdSpectra_Flux_SO[uiI][uiJ],cParam.m_dWavelength_Delta_Ang);

					cCombined_Unflat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpcSpectrum[uiI][1].flux(uiJ),dWL_Ref,lpdSpectra_Flux[uiI][uiJ] < 1.0);
					cEO_Unflat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpcSpectrum[uiI][2].flux(uiJ),dWL_Ref, lpdSpectra_Flux_EO[uiI][uiJ] < 1.0);
					cSO_Unflat.Process_Vmin(lpdSpectra_WL[uiI][uiJ],lpcSpectrum[uiI][3].flux(uiJ),dWL_Ref, lpdSpectra_Flux_SO[uiI][uiJ] < 1.0);

					bIn_feature |= (1.0 - lpdSpectra_Flux[uiI][uiJ]) > 1.0e-4;
					//printf("%.0f %.5f\n",lpdSpectra_WL[uiI][uiJ],lpdSpectra_Flux[uiI][uiJ]);
					if (!bIn_feature)
						uiContinuum_Blue_Idx = uiJ;
	//				if (lpdSpectra_Flux[uiI][uiJ] > 1.0001 && lpcSpectrum[uiI][1].flux(uiJ) > dP_Cygni_Peak_Flux)
	//				{
	//					dP_Cygni_Peak_Flux = lpcSpectrum[uiI][1].flux(uiJ);
	//					uiContinuum_Red_Idx = uiJ;
	//				}
	//				if (bIn_feature && lpdSpectra_Flux[uiI][uiJ] < 1.000 && lpcSpectrum[uiI][1].flux(uiJ) < dMin_Flux)
	//				{
	//					dMin_Flux = lpcSpectrum[uiI][1].flux(uiJ);
	//				}
					if (lpdSpectra_Flux[uiI][uiJ] < 1.000 && lpdSpectra_Flux[uiI][uiJ] < dMin_Flux_Flat)
					{
						dMin_Flux_Flat = lpdSpectra_Flux[uiI][uiJ];
						uiMin_Flux_Idx = uiJ;
					}

					if (bIn_feature && lpdSpectra_Flux[uiI][uiJ] > 1.0000 && uiP_Cygni_Min_Idx == 0) // determine max index of absorption region
						uiP_Cygni_Min_Idx = uiJ;
				}	
				double	dV_Jeff_HVF = 0.0, dV_Jeff_PVF = 0.0, dpEW_Jeff_HVF = 0.0, dpEW_Jeff_PVF = 0.0;
				uiContinuum_Red_Idx = uiMin_Flux_Idx;
				if (uiMin_Flux_Idx != (unsigned int)(-1))
				{
					while (uiContinuum_Blue_Idx > 0 && lpdSpectra_Flux[uiI][uiContinuum_Blue_Idx] < 0.9999)
						uiContinuum_Blue_Idx--;
					for (unsigned int uiJ = uiMin_Flux_Idx; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
					{
						if (lpdSpectra_Flux[uiI][uiJ] > 0.9999 && lpcSpectrum[uiI][1].flux(uiJ) > dP_Cygni_Peak_Flux)
						{
							dP_Cygni_Peak_Flux = lpcSpectrum[uiI][1].flux(uiJ);
							uiContinuum_Red_Idx = uiJ;
						}
					}
		//		printf("here 4a\n");
					unsigned int uiNum_Points = uiContinuum_Red_Idx - uiContinuum_Blue_Idx + 1;
		//			printf("%i %i %i %i\n",uiContinuum_Blue_Idx, uiContinuum_Red_Idx, uiNum_Points, lpuiSpectra_Count[uiI]);
		//		fflush(stdout);
			
					// perform Gaussian fitting routine
					// first define the pseudo-continuum as defined by Silverman
					double	dSlope = (lpcSpectrum[uiI][1].flux(uiContinuum_Red_Idx) - lpcSpectrum[uiI][1].flux(uiContinuum_Blue_Idx)) / (lpcSpectrum[uiI][1].wl(uiContinuum_Red_Idx) - lpcSpectrum[uiI][1].wl(uiContinuum_Blue_Idx));
			
					vY.Set_Size(uiNum_Points);
					vX.Set_Size(uiNum_Points);
					vW.Set_Size(uiNum_Points);

					fflush(stdout);
					for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
					{
						vX.Set(uiJ,lpcSpectrum[uiI][1].wl(uiJ + uiContinuum_Blue_Idx));
						vW.Set(uiJ,0.01); // arbitrary weight
						vY.Set(uiJ,lpcSpectrum[uiI][1].flux(uiJ + uiContinuum_Blue_Idx) - ((lpcSpectrum[uiI][1].wl(uiJ + uiContinuum_Blue_Idx) - lpcSpectrum[uiI][1].wl(uiContinuum_Blue_Idx)) * dSlope + lpcSpectrum[uiI][1].flux(uiContinuum_Blue_Idx)));

						cCombined_Unflat.Process_pEW(vY.Get(uiJ),cParam.m_dWavelength_Delta_Ang);
					}
					// try single gaussian fit
					// some rough initial guesses for the parameters
					vA.Set_Size(3);
					vA.Set(0,-dMin_Flux_Flat/3.0);
					vA.Set(1,200.0);
					vA.Set(2,lpdSpectra_WL[uiI][uiMin_Flux_Idx]);

					// Perform LSQ fit
					if (GeneralFit(vX, vY ,vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, lpgfpParamters,100))
					{
						vA_Single = vA;
						dSmin_Single = dSmin;
						mCovariance_Matrix_Single = mCovariance_Matrix;
					}
					// try double gaussian fit
					vA.Set_Size(6);
					vA.Set(0,-dMin_Flux_Flat/6.0);
					vA.Set(1,200.0);
					vA.Set(2,lpdSpectra_WL[uiI][uiMin_Flux_Idx] - 250.0);
					vA.Set(3,-dMin_Flux_Flat/6.0);
					vA.Set(4,200.0);
					vA.Set(5,lpdSpectra_WL[uiI][uiMin_Flux_Idx] + 250.0);
					// Perform LSQ fit
					if (GeneralFit(vX, vY ,vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, lpgfpParamters,100))
					{
						// if the single guassian fit is better, use those results
						if (dSmin > dSmin_Single)
						{
							vA = vA_Single;
							dSmin = dSmin_Single;
							mCovariance_Matrix = mCovariance_Matrix_Single;
						}
					}
					else
					{
						// if the double guassian fit fails, use single gaussian results
						vA = vA_Single;
						dSmin = dSmin_Single;
						mCovariance_Matrix = mCovariance_Matrix_Single;
					}

	//			printf("here 4c\n");
	//			fflush(stdout);
					if (vA.Get_Size() == 6)
					{
						dV_Jeff_HVF = Compute_Velocity(vA.Get(2),lpgfpParamters->m_dWl[1]);
						dV_Jeff_PVF = Compute_Velocity(vA.Get(5),lpgfpParamters->m_dWl[1]);
					}
					else
					{
						dV_Jeff_PVF = Compute_Velocity(vA.Get(2),lpgfpParamters->m_dWl[1]);
					}
					for (unsigned int uiJ = uiContinuum_Blue_Idx; uiJ < uiContinuum_Red_Idx; uiJ++)
					{
						XVECTOR vF = Multi_Gaussian(vX.Get(uiJ), vA, lpgfpParamters);
						if (vA.Get_Size() == 6)
						{
							XVECTOR vAlcl;
							vAlcl.Set_Size(3);
							vAlcl.Set(0,vA.Get(0));
							vAlcl.Set(1,vA.Get(1));
							vAlcl.Set(2,vA.Get(2));

							Gaussian(vX.Get(uiJ), vAlcl, lpgfpParamters);
							dpEW_Jeff_HVF -= vF.Get(0) * cParam.m_dWavelength_Delta_Ang; // - sign to keep pEW positive

							vAlcl.Set(0,vA.Get(3));
							vAlcl.Set(1,vA.Get(4));
							vAlcl.Set(2,vA.Get(5));

							Gaussian(vX.Get(uiJ), vAlcl, lpgfpParamters);
							dpEW_Jeff_PVF -= vF.Get(0) * cParam.m_dWavelength_Delta_Ang;
						}
						else
						{
							Gaussian(vX.Get(uiJ), vA, lpgfpParamters);
							dpEW_Jeff_PVF -= vF.Get(0) * cParam.m_dWavelength_Delta_Ang; // - sign to keep pEW positive
						}
					}

					vX.Set_Size(uiP_Cygni_Min_Idx);
					vY.Set_Size(uiP_Cygni_Min_Idx);
					vW.Set_Size(uiP_Cygni_Min_Idx);
					for (unsigned int uiJ = 0; uiJ < uiP_Cygni_Min_Idx; uiJ++)
					{
						vX.Set(uiJ,lpdSpectra_WL[uiI][uiJ]);
						vW.Set(uiJ,0.01); // arbitrary weight
						vY.Set(uiJ,lpdSpectra_Flux[uiI][uiJ] - 1.0);
					}
					// try single gaussian fit
					// some rough initial guesses for the parameters
					vA_Flat.Set_Size(3);
					vA_Flat.Set(0,-dMin_Flux_Flat/3.0);
					vA_Flat.Set(1,200.0);
					vA_Flat.Set(2,lpdSpectra_WL[uiI][uiMin_Flux_Idx]);

					// Perform LSQ fit
					if (GeneralFit(vX, vY ,vW, Multi_Gaussian, vA_Flat, mCovariance_Matrix_Flat, dSmin_Flat, lpgfpParamters,100))
					{
						vA_Single_Flat = vA_Flat;
						dSmin_Single_Flat = dSmin_Flat;
						mCovariance_Matrix_Single_Flat = mCovariance_Matrix_Flat;
					}
					// try double gaussian fit
					vA_Flat.Set_Size(6);
					vA_Flat.Set(0,-dMin_Flux_Flat/6.0);
					vA_Flat.Set(1,200.0);
					vA_Flat.Set(2,lpdSpectra_WL[uiI][uiMin_Flux_Idx] - 250.0);
					vA_Flat.Set(3,-dMin_Flux_Flat/6.0);
					vA_Flat.Set(4,200.0);
					vA_Flat.Set(5,lpdSpectra_WL[uiI][uiMin_Flux_Idx] + 250.0);
					// Perform LSQ fit
					if (GeneralFit(vX, vY ,vW, Multi_Gaussian, vA_Flat, mCovariance_Matrix_Flat, dSmin_Flat, lpgfpParamters,100))
					{
						// if the single guassian fit is better, use those results
						if (dSmin_Flat > dSmin_Single_Flat)
						{
							vA_Flat = vA_Single_Flat;
							dSmin_Flat = dSmin_Single_Flat;
							mCovariance_Matrix_Flat = mCovariance_Matrix_Single_Flat;
						}
					}
					else
					{
						// if the double guassian fit fails, use single gaussian results
						vA_Flat = vA_Single_Flat;
						dSmin_Flat = dSmin_Single_Flat;
						mCovariance_Matrix_Flat = mCovariance_Matrix_Single_Flat;
					}

				}

	//		printf("here 4d\n");
	//		fflush(stdout);
				fprintf(fileData,"%s, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e", 
	lpszModel_List[uiI], cCombined_Flat.m_d_pEW, -cCombined_Flat.m_dVmin, cEO_Flat.m_d_pEW, -cEO_Flat.m_dVmin,  cSO_Flat.m_d_pEW, -cSO_Flat.m_dVmin, cCombined_Unflat.m_d_pEW, -cCombined_Flat.m_dVmin, -cEO_Unflat.m_dVmin, -cSO_Unflat.m_dVmin, -dV_Jeff_HVF, -dV_Jeff_PVF, dpEW_Jeff_HVF, dpEW_Jeff_PVF);
				if (dSmin < DBL_MAX)
				{
					fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA.Get(0),sqrt(mCovariance_Matrix.Get(0,0)),vA.Get(1),sqrt(mCovariance_Matrix.Get(1,1)),vA.Get(2),sqrt(mCovariance_Matrix.Get(2,2)));
					if (vA.Get_Size() == 6)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA.Get(3),sqrt(mCovariance_Matrix.Get(3,3)),vA.Get(4),sqrt(mCovariance_Matrix.Get(4,4)),vA.Get(5),sqrt(mCovariance_Matrix.Get(5,5)));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
				}
				else
					fprintf(fileData,", -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.");

				if (dSmin_Flat < DBL_MAX)
				{
					fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA_Flat.Get(0),sqrt(mCovariance_Matrix_Flat.Get(0,0)),vA_Flat.Get(1),sqrt(mCovariance_Matrix_Flat.Get(1,1)),vA_Flat.Get(2),sqrt(mCovariance_Matrix_Flat.Get(2,2)));
					if (vA_Flat.Get_Size() == 6)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA_Flat.Get(3),sqrt(mCovariance_Matrix_Flat.Get(3,3)),vA_Flat.Get(4),sqrt(mCovariance_Matrix_Flat.Get(4,4)),vA_Flat.Get(5),sqrt(mCovariance_Matrix_Flat.Get(5,5)));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
				}
				else
					fprintf(fileData,", -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.");
				fprintf(fileData,"\n");

			
			
			}
			fclose(fileData);
		}
//		printf("here 5\n");
//		fflush(stdout);
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			delete [] lpdSpectra_WL[uiI];
			delete [] lpdSpectra_Flux[uiI];
			delete [] lpdSpectra_WL_EO[uiI];
			delete [] lpdSpectra_Flux_EO[uiI];
			delete [] lpdSpectra_WL_SO[uiI];
			delete [] lpdSpectra_Flux_SO[uiI];
			delete [] lpdContinuum_WL[uiI];
			delete [] lpdContinuum_Flux[uiI];
		}
		delete [] lpdSpectra_WL;
		delete [] lpdSpectra_Flux;
		delete [] lpdSpectra_WL_EO;
		delete [] lpdSpectra_Flux_EO;
		delete [] lpdSpectra_WL_SO;
		delete [] lpdSpectra_Flux_SO;
		delete [] lpuiSpectra_Count;
		delete [] lpuiSpectra_Count_EO;
		delete [] lpuiSpectra_Count_SO;
		delete [] lpdContinuum_WL;
		delete [] lpdContinuum_Flux;
		delete [] lpuiContinuum_Count;
	}
	

	return 0;
}
