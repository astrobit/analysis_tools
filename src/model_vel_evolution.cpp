
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
#include <xastro.h>

// Data from Pereira 2014 for SN 2011fe
class PEREIRA_DATA
{
public:
	double m_dEpoch;// estimate of time after explosion based on assumed time from Pereira 2014
	double m_dLog_Luminosity;
	PEREIRA_DATA(const double & i_dEpoch, const double & i_dLog_Luminosity)
	{
		m_dEpoch = i_dEpoch;
		m_dLog_Luminosity = i_dLog_Luminosity;
	}
};

PEREIRA_DATA	g_cPereira_Data[] = {
							PEREIRA_DATA(2.49, 7.9712932209),
							PEREIRA_DATA(3.39, 8.2662490688),
							PEREIRA_DATA(4.39, 8.4905376815),
							PEREIRA_DATA(5.39, 8.6960240873),
							PEREIRA_DATA(6.39, 8.8683090601),
							PEREIRA_DATA(7.39, 9.0071674775),
							PEREIRA_DATA(8.39, 9.1489900066),
							PEREIRA_DATA(9.39, 9.2108707374),
							PEREIRA_DATA(10.49, 9.3104132661),
							PEREIRA_DATA(11.39, 9.3629143399),
							PEREIRA_DATA(12.39, 9.4265611637),
							PEREIRA_DATA(16.39, 9.4857671829),
							PEREIRA_DATA(17.39, 9.4699865816),
							PEREIRA_DATA(18.39, 9.4653704764),
							PEREIRA_DATA(19.39, 9.453610882),
							PEREIRA_DATA(20.39, 9.4324414496),
							PEREIRA_DATA(21.39, 9.399067754),
							PEREIRA_DATA(24.39, 9.3250812657) };

double g_lpPereira_Days[] = {
	2.49,
	3.39,
	4.39,
	5.39,
	6.39,
	7.39,
	8.39,
	9.39,
	10.49,
	11.39,
	12.39,
	16.39,
	17.39,
	18.39,
	19.39,
	20.39,
	21.39,
	24.39};

double g_lpPereira_Luminosity[] = {
	7.9712932209,
	8.2662490688,
	8.4905376815,
	8.6960240873,
	8.8683090601,
	9.0071674775,
	9.1489900066,
	9.2108707374,
	9.3104132661,
	9.3629143399,
	9.4265611637,
	9.4857671829,
	9.4699865816,
	9.4653704764,
	9.4536108820,
	9.4324414496,
	9.3990677540,
	9.3250812657 };

unsigned int g_uiPereira_Count = sizeof(g_lpPereira_Luminosity)/sizeof(double);

XSPLINE_DATA	g_splPereira_Data(g_lpPereira_Days,g_lpPereira_Luminosity,g_uiPereira_Count);


//GAUSS_FIT_PARAMETERS	g_cgfpCaNIR(8662.14,160.0,8542.09,170.0,8498.02,130.0);
//GAUSS_FIT_PARAMETERS	g_cgfpCaHK(3934.77,230.0,3969.59,220.0);
//GAUSS_FIT_PARAMETERS	g_cgfpOINIR(774.08,870.0,776.31,810.0,7777.53,750.0);
//GAUSS_FIT_PARAMETERS	g_cgfpSi6355(6348.85,1000.0,6373.12,1000.0);
//GAUSS_FIT_PARAMETERS	g_cgfpSi5968(5959.21,500.0,5980.59,500.0);



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


void Process_Day_List(const char * lpszDay_List_String, double * &o_lpdDay_List, unsigned int &o_uiDay_Count)
{
	char * lpszCursor = (char *)lpszDay_List_String;
	// count number of models to process
	o_uiDay_Count = 0;
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_uiDay_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
			lpszCursor++; // bypass comma
	}
	// allocate model name pointer list
	o_lpdDay_List = new double [o_uiDay_Count];
	// count number of models to process
	o_uiDay_Count = 0;
	lpszCursor = (char *)lpszDay_List_String; // reset cursor
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// add pointer to model name and count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_lpdDay_List[o_uiDay_Count] = atof(lpszCursor);
			o_uiDay_Count++;
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
	bool bVerbose = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--verbose");
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
//	const char *	lpszIon_List = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ions");
	char lpszShell_Abundance[128];
	char lpszEjecta_Abundance[128];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-abundance",lpszShell_Abundance,sizeof(lpszShell_Abundance),"");
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-abundance",lpszEjecta_Abundance,sizeof(lpszShell_Abundance),"");
	double	dEjecta_Power_Law = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-power-law",4.0);
	double	dShell_Power_Law = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-power-law",4.0);


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

	if (bVerbose)
		printf("Parsing day list\n");
	// get model list string
	const char *	lpszDay_List_String = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--days");
	double * lpdDay_List;
	unsigned int uiDay_Count;
	Process_Day_List(lpszDay_List_String, lpdDay_List, uiDay_Count);

	if (bVerbose)
		printf("Parsing model list\n");
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
		dRange_Min = 500.0;
		dRange_Max = 15000.0;
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

		double	dEjecta_Scalar_Ref = -1.0;
		double	dShell_Scalar_Ref = -1.0;
		double	dEjecta_Scalar_Prof = -1.0;
		double	dShell_Scalar_Prof = -1.0;
		OPACITY_PROFILE_DATA	cOpacity_Profile;

		if (lpszRef_Model)
		{
			if (bVerbose)
				printf("Reading ref model opacity profile\n");
			char lpszOpacity_Profile[128];
			sprintf(lpszOpacity_Profile,"%s/opacity_map_scalars.opdata",lpszRef_Model_Extended);
			cOpacity_Profile.Load(lpszOpacity_Profile);
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(eScalar_Type);
			dShell_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);
		}
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
					if (bVerbose)
						printf("Reading photosphere data\n");
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
				if (bVerbose)
					printf("Reading model opacity scalars\n");
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

				if (bVerbose)
					printf("Getting continuum\n");
				cParam.m_dPhotosphere_Velocity_kkms = cContinuum_Parameters.Get(0);
				if (cMSDB.Get_Spectrum(cParam, msdb::CONTINUUM, lpcSpectrum[uiI][0]) == 0)
				{
					printf("generating continuua\n");
					Generate_Synow_Spectra_Exp(lpcSpectrum[uiI][0],2001,cContinuum_Parameters,lpcSpectrum[uiI][0]); // ion irrelevant for this 
					printf("Adding to db\n");
					cMSDB.Add_Spectrum(cParam, msdb::CONTINUUM, lpcSpectrum[uiI][0]);
					printf("done\n");
				}

				if (bVerbose)
					printf("Reading opacity map\n");
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

				cParam.m_dEjecta_Scalar_Time_Power_Law = -dEjecta_Power_Law;
				cParam.m_dShell_Scalar_Time_Power_Law = -dShell_Power_Law;


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
					Generate_Synow_Spectra(lpcSpectrum[uiI][1], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][1],-dEjecta_Power_Law,-dShell_Power_Law);
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
						Generate_Synow_Spectra(lpcSpectrum[uiI][2], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][2],-dEjecta_Power_Law,-dShell_Power_Law);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]);

						cParameters.Set(6,dShell_Scalar);
						cParameters.Set(4,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI][3], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][3],-dEjecta_Power_Law,-dShell_Power_Law);
						printf(".");
						fflush(stdout);
						cMSDB.Add_Spectrum(dbidID, msdb::SHELL_ONLY, lpcSpectrum[uiI][3]);
					}
					printf("Done\n");
				}
				else
				{
					printf("Read db for %s.\n",lpszModel_Name);
					if (cMSDB.Get_Spectrum(cParam, msdb::EJECTA_ONLY, lpcSpectrum[uiI][2]) == 0)
					{
						cParameters.Set(6,-40.0);
						Generate_Synow_Spectra(lpcSpectrum[uiI][2], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][1],-dEjecta_Power_Law,-dShell_Power_Law);
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
							Generate_Synow_Spectra(lpcSpectrum[uiI][3], cOpacity_Map_Ejecta, cOpacity_Map_Shell, uiIon, cParameters, lpcSpectrum[uiI][3],-dEjecta_Power_Law,-dShell_Power_Law);
							printf("Adding S-O to db for %s...",lpszModel_Name);
							fflush(stdout);
							msdb::dbid dbidID = cMSDB.Add_Spectrum(cParam, msdb::SHELL_ONLY, lpcSpectrum[uiI][3]);
							cParameters.Set(4,dEjecta_Scalar);
						}
					}
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
		cY_Axis_Parameters.m_dUpper_Limit = 2.0;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);

		if (bVerbose)
			printf("Allocating data containers.\n");
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
			if (bVerbose)
				printf("Extracting data and flattening model %s.\n",lpszModel_List[uiI]);
			Get_Spectra_Data(lpcSpectrum[uiI][0], lpdContinuum_WL[uiI], lpdContinuum_Flux[uiI], lpuiContinuum_Count[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][1], lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][2], lpdSpectra_WL_EO[uiI], lpdSpectra_Flux_EO[uiI], lpuiSpectra_Count_EO[uiI], dRange_Min, dRange_Max);
			Get_Spectra_Data(lpcSpectrum[uiI][3], lpdSpectra_WL_SO[uiI], lpdSpectra_Flux_SO[uiI], lpuiSpectra_Count_SO[uiI], dRange_Min, dRange_Max);
			for (unsigned int uiJ = 0; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
			{
				lpdSpectra_Flux[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
				lpdSpectra_Flux_EO[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
				lpdSpectra_Flux_SO[uiI][uiJ] /= lpdContinuum_Flux[uiI][uiJ];
			}
			cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + (uiI % 7));
			cLine_Parameters.m_eStipple = (epsplot::STIPPLE)(epsplot::SOLID + (uiI % 8));
			cPlot.Set_Plot_Data(lpdSpectra_WL[uiI], lpdSpectra_Flux[uiI], lpuiSpectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
		}

	
		if (bVerbose)
			printf("Generating caption\n");
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

		if (bVerbose)
			printf("Plotting\n");
		cPlot.Set_Plot_Filename(lpszOutput_Name);
		cPlot.Plot(cPlot_Parameters);
		if (bVerbose)
			printf("Writing spectral data\n");
		sprintf(lpszFilename,"%s.dat",lpszOutput_Name);
		FILE * fileOut = fopen(lpszFilename,"wt");
		if (!fileOut)
			fprintf(stderr,"Failed to open %s for write.\n",lpszFilename);
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

//		fflush(stdout);
		if (!bFull_Spectrum)
		{
			if (bVerbose)
				printf("Generating anaytic data\n");
			sprintf(lpszFilename,"%s.data.csv",lpszOutput_Name);
			FILE * fileData = fopen(lpszFilename,"wt");
			if (!fileData)
				fprintf(stderr,"Failed to open %s for write.\n",lpszFilename);

			fprintf(fileData,"Model, pEW (Combined - flat), Vmin (Combined - flat), pEW (EO - flat), Vmin (EO - flat), pEW (SO - flat), Vmin (SO - flat), pEW (Combined), Vmin (combined), Vmin (EO), Vmin (SO), Vmin-HVF (Jeff), Vmin-PVF (Jeff), pEW-HVF (Jeff), pEW-PVF (Jeff), a_0 (Jeff), sigma a_0 (Jeff), a_1 (Jeff), sigma_a_1 (Jeff), a_2 (Jeff), sigma_a_2 (Jeff), a_3 (Jeff), sigma a_3 (Jeff), a_4 (Jeff), sigma_a_4 (Jeff), a_5 (Jeff), sigma_a_5 (Jeff), a_0 (Flat), sigma a_0 (Flat), a_1 (Flat), sigma_a_1 (Flat), a_2 (Flat), sigma_a_2 (Flat), a_3 (Flat), sigma a_3 (Flat), a_4 (Flat), sigma_a_4 (Flat), a_5 (Flat), sigma_a_5 (Flat)\n");

			// compute pEW values for features of interest.
			for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
			{
                XVECTOR	vX, vY, vA, vW, vA_Flat, vSigma_Jeff, vSigma_Flat;
                double	dSmin_Single = DBL_MAX;
                double	dSmin;
                double	dSmin_Flat;
                double	dSmin_Single_Flat;

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
				printf("Processing model %s\n",lpszModel_List[uiI]);
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

					bIn_feature |= (1.0 - lpdSpectra_Flux[uiI][uiJ]) > 1.0e-2;
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
						uiP_Cygni_Min_Idx = 0;
					}
					
					if (uiJ > uiMin_Flux_Idx && lpdSpectra_Flux[uiI][uiJ] > 1.0000 && uiP_Cygni_Min_Idx == 0) // determine max index of absorption region
						uiP_Cygni_Min_Idx = uiJ;
				}
				if (uiMin_Flux_Idx == -1)
				{
					fprintf(stderr,"Fault in min flux for model %s\n",lpszModel_List[uiI]);
					uiMin_Flux_Idx = 0;
				}
				if (bVerbose)
					printf("Identified minimum at %.0f (%.2f kkm/s)\n",lpdSpectra_WL[uiI][uiMin_Flux_Idx],-1.0e-3*Compute_Velocity(lpdSpectra_WL[uiI][uiMin_Flux_Idx],lpgfpParamters->m_dWl[1]));
				if (bVerbose)
					printf("Identified blue feature edge at %.0f (%.2f kkm/s)\n",lpdSpectra_WL[uiI][uiContinuum_Blue_Idx],-1.0e-3*Compute_Velocity(lpdSpectra_WL[uiI][uiContinuum_Blue_Idx],lpgfpParamters->m_dWl[1]));
				double	dV_Jeff_HVF = 0.0, dV_Jeff_PVF = 0.0, dpEW_Jeff_HVF = 0.0, dpEW_Jeff_PVF = 0.0;
				double	dV_Flat_HVF = 0.0, dV_Flat_PVF = 0.0, dpEW_Flat_HVF = 0.0, dpEW_Flat_PVF = 0.0;
				uiContinuum_Red_Idx = uiMin_Flux_Idx;
				if (uiMin_Flux_Idx != (unsigned int)(-1))
				{
					while (uiContinuum_Blue_Idx > 0 && lpdSpectra_Flux[uiI][uiContinuum_Blue_Idx] < 0.99)
						uiContinuum_Blue_Idx--;
					for (unsigned int uiJ = uiMin_Flux_Idx; uiJ < lpuiSpectra_Count[uiI]; uiJ++)
					{
						if (lpdSpectra_Flux[uiI][uiJ] > 0.99 && lpcSpectrum[uiI][1].flux(uiJ) > dP_Cygni_Peak_Flux)
						{
							dP_Cygni_Peak_Flux = lpcSpectrum[uiI][1].flux(uiJ);
							uiContinuum_Red_Idx = uiJ;
						}
					}
					if (bVerbose)
						printf("Identified p Cygni peak at %.0f\n",lpdSpectra_WL[uiI][uiContinuum_Red_Idx]);
					if (bVerbose)
						printf("Identified red edge of absorption at %.0f (%.2f kkm/s)\n",lpdSpectra_WL[uiI][uiP_Cygni_Min_Idx],-1.0e-3*Compute_Velocity(lpdSpectra_WL[uiI][uiP_Cygni_Min_Idx],lpgfpParamters->m_dWl[1]));
					if (uiContinuum_Red_Idx >= uiContinuum_Blue_Idx)
					{
						unsigned int uiNum_Points = uiContinuum_Red_Idx - uiContinuum_Blue_Idx + 1;
			
						// perform Gaussian fitting routine
						// first define the pseudo-continuum as defined by Silverman
						double	dSlope = (lpcSpectrum[uiI][1].flux(uiContinuum_Red_Idx) - lpcSpectrum[uiI][1].flux(uiContinuum_Blue_Idx)) / (lpcSpectrum[uiI][1].wl(uiContinuum_Red_Idx) - lpcSpectrum[uiI][1].wl(uiContinuum_Blue_Idx));
			
						vY.Set_Size(uiNum_Points);
						vX.Set_Size(uiNum_Points);
						vW.Set_Size(uiNum_Points);
						if (bVerbose)
							printf("Performing unflat fit\n");
						fflush(stdout);
						for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
						{
							vX.Set(uiJ,lpcSpectrum[uiI][1].wl(uiJ + uiContinuum_Blue_Idx));
							vW.Set(uiJ,0.01); // arbitrary weight
							vY.Set(uiJ,lpcSpectrum[uiI][1].flux(uiJ + uiContinuum_Blue_Idx) - ((lpcSpectrum[uiI][1].wl(uiJ + uiContinuum_Blue_Idx) - lpcSpectrum[uiI][1].wl(uiContinuum_Blue_Idx)) * dSlope + lpcSpectrum[uiI][1].flux(uiContinuum_Blue_Idx)));

							cCombined_Unflat.Process_pEW(vY.Get(uiJ),cParam.m_dWavelength_Delta_Ang);
						}
                        vA = Perform_Gaussian_Fit(dMin_Flux_Flat, lpdSpectra_WL[uiI][uiMin_Flux_Idx], vX, vY, vW, lpgfpParamters,
                                        cParam.m_dWavelength_Delta_Ang, dpEW_Jeff_PVF, dpEW_Jeff_HVF, dV_Jeff_PVF, dV_Jeff_HVF, vSigma_Jeff, dSmin);
                    }
                    else
                    {
                        printf("Failed to identify blue and red sides of feature: %i %i (%i)\n",uiContinuum_Blue_Idx,uiContinuum_Red_Idx,uiP_Cygni_Min_Idx);
                    }


                    if (bVerbose)
                        printf("Performing flat fit\n");
                    vX.Set_Size(uiP_Cygni_Min_Idx);
                    vY.Set_Size(uiP_Cygni_Min_Idx);
                    vW.Set_Size(uiP_Cygni_Min_Idx);
                    for (unsigned int uiJ = 0; uiJ < uiP_Cygni_Min_Idx; uiJ++)
                    {
                        vX.Set(uiJ,lpdSpectra_WL[uiI][uiJ]);
                        vW.Set(uiJ,0.01); // arbitrary weight
                        vY.Set(uiJ,lpdSpectra_Flux[uiI][uiJ] - 1.0);
                    }
                    vA_Flat = Perform_Gaussian_Fit(dMin_Flux_Flat, lpdSpectra_WL[uiI][uiMin_Flux_Idx], vX, vY, vW, lpgfpParamters,
                                    cParam.m_dWavelength_Delta_Ang, dpEW_Flat_PVF, dpEW_Flat_HVF, dV_Flat_PVF, dV_Flat_HVF, vSigma_Flat, dSmin_Flat);
				}
				if (vA.Get_Size() > 0)
					fprintf(fileData,"%s, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e", 
	lpszModel_List[uiI], cCombined_Flat.m_d_pEW, -cCombined_Flat.m_dVmin, cEO_Flat.m_d_pEW, -cEO_Flat.m_dVmin,  cSO_Flat.m_d_pEW, -cSO_Flat.m_dVmin, cCombined_Unflat.m_d_pEW, -cCombined_Flat.m_dVmin, -cEO_Unflat.m_dVmin, -cSO_Unflat.m_dVmin, -dV_Jeff_HVF, -dV_Jeff_PVF, dpEW_Jeff_HVF, dpEW_Jeff_PVF);
				else
					fprintf(fileData,"%s, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0", lpszModel_List[uiI]);

				if (dSmin < DBL_MAX)
				{
					if (vA.Get_Size() > 0)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA.Get(0),vSigma_Jeff.Get(0),vA.Get(1),vSigma_Jeff.Get(1),vA.Get(2),vSigma_Jeff.Get(2));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
					if (vA.Get_Size() == 6)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA.Get(3),vSigma_Jeff.Get(3),vA.Get(4),vSigma_Jeff.Get(4),vA.Get(5),vSigma_Jeff.Get(5));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
				}
				else
					fprintf(fileData,", -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.");

				if (dSmin_Flat < DBL_MAX)
				{
					if (vA_Flat.Get_Size() > 0)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA_Flat.Get(0),vSigma_Flat.Get(0),vA_Flat.Get(1),vSigma_Flat.Get(1),vA_Flat.Get(2),vSigma_Flat.Get(2));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
					if (vA_Flat.Get_Size() == 6)
						fprintf(fileData,",%.17e, %.17e, %.17e, %.17e, %.17e, %.17e",vA_Flat.Get(3),vSigma_Flat.Get(3),vA_Flat.Get(4),vSigma_Flat.Get(4),vA_Flat.Get(5),vSigma_Flat.Get(5));
					else
						fprintf(fileData,", -1., -1., -1., -1., -1., -1.");
				}
				else
					fprintf(fileData,", -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.");
				fprintf(fileData,"\n");

			
			
			}
			fclose(fileData);
		}
		else
		{	// full spectrum; produce light curve analysis
			if (bVerbose)
				printf("Generating photometric data\n");
			unsigned int uiPLast = g_uiPereira_Count - 1;
			double	dLuminosity;
			if (dDay >= g_lpPereira_Days[0] && dDay <= g_lpPereira_Days[uiPLast])
			{
				dLuminosity = g_splPereira_Data.Interpolate(dDay);
			}
			else if (dDay < g_lpPereira_Days[0])
			{
				dLuminosity = (g_lpPereira_Luminosity[1] - g_lpPereira_Luminosity[0]) * (dDay - g_lpPereira_Days[0]) / (g_lpPereira_Days[1] - g_lpPereira_Days[0]) + g_lpPereira_Luminosity[0];
			}
			else if (dDay > g_lpPereira_Days[uiPLast])
			{
				dLuminosity = (g_lpPereira_Luminosity[uiPLast] - g_lpPereira_Luminosity[uiPLast - 1]) * (dDay - g_lpPereira_Days[uiPLast - 1]) / (g_lpPereira_Days[uiPLast] - g_lpPereira_Days[uiPLast - 1]) + g_lpPereira_Luminosity[uiPLast - 1];
			}
			dLuminosity = pow(10.0,dLuminosity) * g_XASTRO.k_dLsun;
			sprintf(lpszFilename,"%s.photometry.csv",lpszOutput_Name);
			FILE * filePhotometry = fopen(lpszFilename,"wt");
			fprintf(filePhotometry,"Day, Model, u ,b, v, uvw1, uvw2, uvm2, white\n");
			for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
			{
				double	dDelta_Wl = lpcSpectrum[uiI][1].wl(1) - lpcSpectrum[uiI][1].wl(0);
//				double * lpdFlux_Corr_Spectrum = new double[lpuiSpectra_Count[uiI]];
				double	dSwift_u = 0.0,dSwift_v = 0.0,dSwift_b = 0.0,dSwift_uvw1 = 0.0,dSwift_uvm2 = 0.0,dSwift_uvw2 = 0.0,dSwift_white = 0.0;
				double	dSum = 0.0;
				for (unsigned int uiJ = 0; uiJ < lpcSpectrum[uiI][1].size(); uiJ++)
				{
					dSum += lpcSpectrum[uiI][1].flux(uiJ);
				}
				double dLum_Model = dLuminosity / (lpcSpectrum[uiI][1].wl(lpcSpectrum[uiI][1].size() - 1) - lpcSpectrum[uiI][1].wl(0));
				for (unsigned int uiJ = 0; uiJ < lpcSpectrum[uiI][1].size(); uiJ++)
				{
					double dFlux = lpcSpectrum[uiI][1].flux(uiJ) / dSum * dLum_Model;
					dSwift_u += dFlux * (g_XASTRO_Filter_Swift_u << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_v += dFlux * (g_XASTRO_Filter_Swift_v << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_b += dFlux * (g_XASTRO_Filter_Swift_b << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_uvw1 += dFlux * (g_XASTRO_Filter_Swift_uvw1 << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_uvw2 += dFlux * (g_XASTRO_Filter_Swift_uvw2 << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_uvm2 += dFlux * (g_XASTRO_Filter_Swift_uvm2 << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
					dSwift_white += dFlux * (g_XASTRO_Filter_Swift_white << lpcSpectrum[uiI][1].wl(uiJ)) * dDelta_Wl;
				}
				fprintf(filePhotometry,"%.2f, %s, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",dDay,lpszModel_List[uiI],
					dSwift_u,dSwift_b,dSwift_v,dSwift_uvw1,dSwift_uvw2,dSwift_uvm2,dSwift_white);
			}
			fclose(filePhotometry);		
		}
		for (unsigned int uiI = 0; uiI < uiModel_Count; uiI++)
		{
			if (bVerbose)
				printf("delete Spectra WL %i\n",uiI);
			delete [] lpdSpectra_WL[uiI];
			if (bVerbose)
				printf("delete Spectra Flux %i\n",uiI);
			delete [] lpdSpectra_Flux[uiI];
			if (bVerbose)
				printf("delete Spectra WL EO %i\n",uiI);
			delete [] lpdSpectra_WL_EO[uiI];
			if (bVerbose)
				printf("delete Spectra Flux EO %i\n",uiI);
			delete [] lpdSpectra_Flux_EO[uiI];
			if (bVerbose)
				printf("delete Spectra WL SO %i\n",uiI);
			delete [] lpdSpectra_WL_SO[uiI];
			if (bVerbose)
				printf("delete Spectra Flux SO %i\n",uiI);
			delete [] lpdSpectra_Flux_SO[uiI];
			if (bVerbose)
				printf("delete Continuum WL %i\n",uiI);
			delete [] lpdContinuum_WL[uiI];
			if (bVerbose)
				printf("delete Continuum Flux %i\n",uiI);
			delete [] lpdContinuum_Flux[uiI];
		}
		if (bVerbose)
			printf("delete Spectra WL\n");
		delete [] lpdSpectra_WL;
		if (bVerbose)
			printf("delete Spectra Flux\n");
		delete [] lpdSpectra_Flux;
		if (bVerbose)
			printf("delete Spectra WL EO\n");
		delete [] lpdSpectra_WL_EO;
		if (bVerbose)
			printf("delete Spectra Flux EO\n");
		delete [] lpdSpectra_Flux_EO;
		if (bVerbose)
			printf("delete Spectra WL SO\n");
		delete [] lpdSpectra_WL_SO;
		if (bVerbose)
			printf("delete Spectra Flux SO\n");
		delete [] lpdSpectra_Flux_SO;
		if (bVerbose)
			printf("delete Spectra Count\n");
		delete [] lpuiSpectra_Count;
		if (bVerbose)
			printf("delete Spectra Count EO\n");
		delete [] lpuiSpectra_Count_EO;
		if (bVerbose)
			printf("delete Spectra Count SO\n");
		delete [] lpuiSpectra_Count_SO;
		if (bVerbose)
			printf("delete Continuum WL\n");
		delete [] lpdContinuum_WL;
		if (bVerbose)
			printf("delete Continuum Flux\n");
		delete [] lpdContinuum_Flux;
		if (bVerbose)
			printf("delete Continuum Count\n");
		delete [] lpuiContinuum_Count;
	}
	

	return 0;
}
