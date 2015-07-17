
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


void Process_Model_List(const char * lpszModel_List_String, const char ** o_lpszModel_List, unsigned int o_uiModel_Count)
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
void Process_Ion_List(const char * i_lpszIon_List_String, unsigned int * &o_lpuiIon_List, unsigned int &o_uiIon_Count)
{
	char * lpszCursor = (char *)i_lpszIon_List_String;
//	printf("ions: %s\n",i_lpszIon_List_String);
	// count number of models to process
	o_uiIon_Count = 0;
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_uiIon_Count++;
			while (lpszCursor[0] != 0 && lpszCursor[0] != ',')
				lpszCursor++;
		}
		if(lpszCursor[0] == ',')
			lpszCursor++; // bypass comma
	}
	// allocate model name pointer list
	o_lpuiIon_List = new unsigned int[o_uiIon_Count];
	// count number of models to process
//	printf("ions: %i\n",o_uiIon_Count);
	o_uiIon_Count = 0;
	lpszCursor = (char *)i_lpszIon_List_String; // reset cursor
	while (lpszCursor && lpszCursor[0] != 0)
	{
		// bypass whitespace
		while (lpszCursor[0] != 0 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
			lpszCursor++;
		// add pointer to model name and count model
		if (lpszCursor[0] != 0 && lpszCursor[0] != ',')
		{
			o_lpuiIon_List[o_uiIon_Count] = atoi(lpszCursor);
//			printf("ion: %i\n",o_lpuiIon_List[o_uiIon_Count]);
			o_uiIon_Count++;
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


unsigned int Find_Ion_Index_Ref(const XDATASET & cData, unsigned int i_uiIon)
{
	unsigned int uiIdx = cData.GetNumElements() + 1;
	for (unsigned int uiI = 0; uiI < cData.GetNumElements() && uiIdx  > cData.GetNumElements(); uiI++)
	{
		if ((unsigned int)(cData.GetElement(0,uiI)) == i_uiIon)
			uiIdx = uiI;
	}
	return uiIdx;
}


enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};

double Get_WL_Ref(tFit_Region i_eFit_Region)
{
	double dWL_Ref;
	switch (i_eFit_Region)
	{
	case CANIR:
		dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		break;
	case CAHK:
		dWL_Ref = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
		break;
	case SI5968:
		dWL_Ref = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
		break;
	case SI6355:
		dWL_Ref = (6348.85 * pow(10.0,0.149)  + 6373.12 * pow(10.0,-0.082)) / (pow(10.0,0.149) + pow(10.0,-0.082));
		break;
	case OINIR:
		dWL_Ref = (7774.083 * pow(10.0,0.369) + 7776.305 * pow(10.0,0.223) + 7777.528 * pow(10.0,0.002)) / (pow(10.0,0.369) + pow(10.0,0.223) + pow(10.0,0.002));
		break;
	}
	return dWL_Ref;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA::GROUP eScalar_Type;
	char lpszOutput_Name[256];
	GAUSS_FIT_PARAMETERS * lpgfpParamters;
	ABUNDANCE_LIST	cShell_Abd;
	ABUNDANCE_LIST	cEjecta_Abd;
	char lpszShell_Abundance[128];
	char lpszEjecta_Abundance[128];
	unsigned int * lpuiIon_List;
	unsigned int uiIon_Count;
	ION_DATA	* lpcIon_Data;
	ION_DATA	* lpcIon_Data_EO;
	ION_DATA	* lpcIon_Data_SO;
	char	lpszModel[16];

	double	dDay = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--day",1.0);
	double	dPS_Temp = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ps-temp",1.0);
	double	dEjecta_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-scalar",1.0);
	double	dShell_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-scalar",1.0);
	unsigned int uiRef_Ion = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-ion",1.0);
	const char *	lpszIon_List = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ions");
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--shell-abundance",lpszShell_Abundance,sizeof(lpszShell_Abundance),"");
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--ejecta-abundance",lpszEjecta_Abundance,sizeof(lpszShell_Abundance),"");
	const char *	lpszModel_List_String = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--model");
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output-name",lpszOutput_Name,sizeof(lpszOutput_Name),"");
    unsigned int uiPS_Velocity_Ion_State = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--use-computed-ps-velocity",3);
	const char *	lpszRef_Model = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-model");
	const char *	lpszRef_Spectrum[6];
	lpszRef_Spectrum[0] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-1");
	lpszRef_Spectrum[1] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-2");
	lpszRef_Spectrum[2] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-3");
	lpszRef_Spectrum[3] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-4");
	lpszRef_Spectrum[4] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-5");
	lpszRef_Spectrum[5] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-6");
	lpszRef_Spectrum[6] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-7");
	lpszRef_Spectrum[7] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-8");
	lpszRef_Spectrum[8] = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--ref-spectrum-9");

	char lpszRef_Model_Extended[128] = {0};
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


//	printf("Abundance\n");
	if (strcmp(lpszShell_Abundance,"Solar") == 0)
		cShell_Abd.Read_Table(Solar);
	else if (strcmp(lpszShell_Abundance,"Seitenzahl") == 0)
		cShell_Abd.Read_Table(Seitenzahl_N100_2013);
	else if (strcmp(lpszShell_Abundance,"CO_Rich") == 0)
		cShell_Abd.Read_Table(CO_Rich);
	else
		fprintf(stderr,"Unable to identify shell abundance type\n");

	if (strcmp(lpszEjecta_Abundance,"Solar") == 0)
		cEjecta_Abd.Read_Table(Solar);
	else if (strcmp(lpszEjecta_Abundance,"Seitenzahl") == 0)
		cEjecta_Abd.Read_Table(Seitenzahl_N100_2013);
	else if (strcmp(lpszEjecta_Abundance,"CO_Rich") == 0)
		cEjecta_Abd.Read_Table(CO_Rich);
	else
		fprintf(stderr,"Unable to identify ejecta abundance type\n");


	cEjecta_Abd.Normalize_Groups();
	Process_Ion_List(lpszIon_List, lpuiIon_List, uiIon_Count);

//	printf("Model\n");
	if (lpszModel_List_String[0] == 'r' && lpszModel_List_String[1] == 'u' && lpszModel_List_String[2] == 'n')
		strcpy(lpszModel,lpszModel_List_String);
	else if (lpszModel_List_String[0] >= '0' && lpszModel_List_String[0] <= '9')
		sprintf(lpszModel,"run%s",lpszModel_List_String);
	else
		strcpy(lpszModel,lpszModel_List_String);

	XDATASET cPhotosphere;
	XDATASET cSi_Opacity;
	XDATASET cMg_Opacity;
	XDATASET cO_Opacity;
	XDATASET cC_Opacity;
	XDATASET cFe_Opacity;
	XDATASET cShell_Opacity;
	XDATASET	cRef_Lines;
	OPACITY_PROFILE_DATA	cOpacity_Profile;

	double	dEjecta_Scalar_Ref = -1.0;
	double	dShell_Scalar_Ref = -1.0;
	double	dEjecta_Scalar_Prof = -1.0;
	double	dShell_Scalar_Prof = -1.0;

	unsigned int	uiRef_Elem = uiRef_Ion / 100;
//	printf("Ref Lines\n");
	cRef_Lines.ReadDataFile(getenv("SYNXX_REF_LINE_DATA_PATH"),true,false);
	unsigned int uiRef_Idx = Find_Ion_Index_Ref(cRef_Lines,uiRef_Ion);

	double dRef_Lambda = cRef_Lines.GetElement(1,uiRef_Idx);
	double dRef_Oscillator_Strength = cRef_Lines.GetElement(2,uiRef_Idx);
	double dRef_Energy = cRef_Lines.GetElement(3,uiRef_Idx);
//	printf("Ref line data: %f %f %f\n",dRef_Lambda,dRef_Oscillator_Strength,dRef_Energy);


//	printf("Opacity Maps\n");
	
	char lpszFilename[256];
	sprintf(lpszFilename,"%s/opacity_map_ejecta.C.xdataset",lpszModel);
	cC_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_ejecta.O.xdataset",lpszModel);
	cO_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_ejecta.Mg.xdataset",lpszModel);
	cMg_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_ejecta.Si.xdataset",lpszModel);
	cSi_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_ejecta.Fe.xdataset",lpszModel);
	cFe_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_shell.xdataset",lpszModel);
	cShell_Opacity.ReadDataFileBin(lpszFilename);
	sprintf(lpszFilename,"%s/opacity_map_scalars.opdata",lpszModel);
	cOpacity_Profile.Load(lpszFilename);
	sprintf(lpszFilename,"%s/photosphere.csv",lpszModel);
	cPhotosphere.ReadDataFile(lpszFilename,false,false,',');

//	printf("Ref Model\n");
	if (lpszRef_Model)
	{
		
		OPACITY_PROFILE_DATA	cOpacity_Profile_Ref;
		char lpszOpacity_Profile[128];
		sprintf(lpszOpacity_Profile,"%s/opacity_map_scalars.opdata",lpszRef_Model_Extended);
		cOpacity_Profile_Ref.Load(lpszOpacity_Profile);
		if (uiRef_Elem == 6)
			dEjecta_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::CARBON);
		else if (uiRef_Elem == 8)
			dEjecta_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::OXYGEN);
		else if (uiRef_Elem >= 9 && uiRef_Elem <= 13)
			dEjecta_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::MAGNESIUM);
		else if (uiRef_Elem >= 14 && uiRef_Elem <= 25)
			dEjecta_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::SILICON);
		else
			dEjecta_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::IRON);

		dShell_Scalar_Ref = cOpacity_Profile_Ref.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);
	}
	else
	{
		if (uiRef_Elem == 6)
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::CARBON);
		else if (uiRef_Elem == 8)
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::OXYGEN);
		else if (uiRef_Elem >= 9 && uiRef_Elem <= 13)
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::MAGNESIUM);
		else if (uiRef_Elem >= 14 && uiRef_Elem <= 25)
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SILICON);
		else
			dEjecta_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::IRON);

		dShell_Scalar_Ref = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);
	}

	lpcIon_Data = new ION_DATA[uiIon_Count * 2];
	lpcIon_Data_EO = new ION_DATA[uiIon_Count];
	lpcIon_Data_SO = new ION_DATA[uiIon_Count];
//	printf("Ion table\n");
//	printf("Ref abd: %f %f\n",cEjecta_Abd.m_dAbundances[uiRef_Elem],cShell_Abd.m_dAbundances[uiRef_Elem]);
//	printf("Ref scalar: %e %e\n",dEjecta_Scalar_Ref,dShell_Scalar_Ref);

//	for (unsigned int uiI = 0; uiI < 12; uiI++)
//		printf("Solar: %i %f\n",uiI,cShell_Abd.m_dAbundances[uiI]);
	wordexp_t cResults;
	if (wordexp(lpszOutput_Name,&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
	{
		strcpy(lpszOutput_Name, cResults.we_wordv[0]);
		wordfree(&cResults);
	}
	sprintf(lpszFilename,"%s.ions.dat",lpszOutput_Name);
	FILE * fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"ion, PVF scalar, HVF scalar\n");
	for (unsigned int uiI = 0; uiI < uiIon_Count; uiI ++)
	{
		unsigned int uiJJ = 2 * uiI;
		unsigned int uiElem = lpuiIon_List[uiI] / 100;
		uiRef_Idx = Find_Ion_Index_Ref(cRef_Lines,lpuiIon_List[uiI]);
//		printf("Ion ref idx: %i %i %i\n",uiRef_Idx,lpuiIon_List[uiI],uiJJ);

		double dLambda = cRef_Lines.GetElement(1,uiRef_Idx);
		double dOscillator_Strength = cRef_Lines.GetElement(2,uiRef_Idx);
		double dEnergy = cRef_Lines.GetElement(3,uiRef_Idx);

		if (dOscillator_Strength == 0.0)
			dOscillator_Strength = 1.0e-20; // make it a small positive value since we are taking the log
		double	dLine_Tau_Eff = log10(dLambda * dOscillator_Strength / (dRef_Lambda * dRef_Oscillator_Strength) *  exp(-(dEnergy - dRef_Energy) / 0.86173324));
		double	dEjecta_Abd_Eff = log10(cEjecta_Abd.m_dAbundances[uiElem]); // ejecta is normalized to group; group scalar handles relative level
		double	dShell_Abd_Eff = log10(cShell_Abd.m_dAbundances[uiElem] / cShell_Abd.m_dAbundances[uiRef_Elem]);

		dShell_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SHELL);

//		printf("Ion ref line data: %i %f %f %f %f %f %f %.2e %.2e\n",lpuiIon_List[uiI], dLambda,dOscillator_Strength,dEnergy,cEjecta_Abd.m_dAbundances[uiElem],cShell_Abd.m_dAbundances[uiElem],dShell_Abd_Eff,dShell_Scalar_Prof,dShell_Scalar_Ref);

		lpcIon_Data[uiJJ].m_uiIon = lpuiIon_List[uiI];
		lpcIon_Data[uiJJ].m_dScalar = dEjecta_Scalar + dLine_Tau_Eff + dEjecta_Abd_Eff;
		lpcIon_Data[uiJJ].m_dExcitation_Temp = 10.0;
		if (uiElem == 6)
		{
			dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::CARBON);
			lpcIon_Data[uiJJ].m_lpcOpacity_Map = &cC_Opacity;
		}
		else if (uiElem == 8)
		{
			dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::OXYGEN);
			lpcIon_Data[uiJJ].m_lpcOpacity_Map = &cO_Opacity;
		}
		else if (uiElem >= 9 && uiElem <= 13)
		{
			dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::MAGNESIUM);
			lpcIon_Data[uiJJ].m_lpcOpacity_Map = &cMg_Opacity;
		}
		else if (uiElem >= 14 && uiElem <= 25)
		{
			dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::SILICON);
			lpcIon_Data[uiJJ].m_lpcOpacity_Map = &cSi_Opacity;
		}
		else
		{
			dEjecta_Scalar_Prof = cOpacity_Profile.Get_Scalar(OPACITY_PROFILE_DATA::IRON);
			lpcIon_Data[uiJJ].m_lpcOpacity_Map = &cFe_Opacity;
		}
		lpcIon_Data[uiJJ].m_dScalar += log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref);

		lpcIon_Data[uiJJ + 1].m_uiIon = lpuiIon_List[uiI];
		lpcIon_Data[uiJJ + 1].m_dExcitation_Temp = 10.0;
		lpcIon_Data[uiJJ + 1].m_lpcOpacity_Map = &cShell_Opacity;
		lpcIon_Data[uiJJ + 1].m_dScalar = dShell_Scalar + dLine_Tau_Eff + dShell_Abd_Eff + log10(dShell_Scalar_Prof / dShell_Scalar_Ref);;
//		printf("%i %.1e %.1e %.1e %.1e %.1e %.1e\n",lpuiIon_List[uiI],lpcIon_Data[uiJJ + 1].m_dScalar, dShell_Scalar,dLine_Tau_Eff,dShell_Abd_Eff,log10(dShell_Scalar_Prof / dShell_Scalar_Ref));

		lpcIon_Data_EO[uiI] = lpcIon_Data[uiJJ];
		lpcIon_Data_SO[uiI] = lpcIon_Data[uiJJ + 1];

		fprintf(fileOut,"%i, %.6f, %.6f \n", lpcIon_Data[uiJJ].m_uiIon, lpcIon_Data[uiJJ].m_dScalar, lpcIon_Data[uiJJ + 1].m_dScalar);
	}
	
//	FILE * fileOut;

	double dRange_Min = 1000.0;
	double dRange_Max = 10000.0;
	
	// generate model spectra
	double dWavelength_Delta_Ang = 1.25;
		//printf("%f %f\n",dRange_Min,dRange_Max);
		//printf("%f %f %f\n", cParam.m_dWavelength_Range_Lower_Ang,cParam.m_dWavelength_Range_Upper_Ang,cParam.m_dWavelength_Delta_Ang);


//	printf("Spectra\n");
	ES::Spectrum lpcSpectrum[4];
	lpcSpectrum[0] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, dWavelength_Delta_Ang);
	lpcSpectrum[1] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, dWavelength_Delta_Ang);
	lpcSpectrum[2] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, dWavelength_Delta_Ang);
	lpcSpectrum[3] = ES::Spectrum::create_from_range_and_step(dRange_Min,dRange_Max, dWavelength_Delta_Ang);
	ES::Spectrum cComparison_Spectra[9];
	for (unsigned int uiI = 0; uiI < 9; uiI++)
	{
		if (lpszRef_Spectrum[uiI])
			 cComparison_Spectra[uiI] = ES::Spectrum::create_from_ascii_file(lpszRef_Spectrum[uiI]);
	}
	XVECTOR cContinuum_Parameters(7);

//		printf("here 1\n");


//	printf("Photosphere\n");
	double dPS_Velocity = -1.0;
	for (unsigned int uiJ = 0; uiJ  < cPhotosphere.GetNumElements() && dPS_Velocity == -1.0; uiJ++)
	{
		if (dDay == cPhotosphere.GetElement(0,uiJ))
		{
			dPS_Velocity = cPhotosphere.GetElement(uiPS_Velocity_Ion_State,uiJ)*1e-8;
		}
	}
	if (dPS_Velocity == -1.0)
	{
		fprintf(stderr,"Unable to find data in photosphere table for day %.2f for model %s\n",dDay,lpszModel);
		exit(1);
	}


	cContinuum_Parameters.Set(0,dPS_Velocity);
	cContinuum_Parameters.Set(1,dPS_Temp);
	cContinuum_Parameters.Set(2,-20.0); // PS ion log tau
	cContinuum_Parameters.Set(3,10.0); // PS ion exctication temp
	cContinuum_Parameters.Set(4,dPS_Velocity); // PS ion vmin
	cContinuum_Parameters.Set(5,128.0); // PS ion vmax
	cContinuum_Parameters.Set(6,1.0); // PS ion vscale

//	printf("Continuum\n");
	Generate_Synow_Spectra_Exp(lpcSpectrum[0],2001,cContinuum_Parameters,lpcSpectrum[0]); // ion irrelevant for this 

//	printf("Generate combined\n");
	Generate_Synow_Multi_Ion_Spectra(dDay, dPS_Temp, dPS_Velocity, lpcIon_Data, uiIon_Count * 2, lpcSpectrum[1]);
//	printf("Generate EO\n");
//	Generate_Synow_Multi_Ion_Spectra(dDay, dPS_Temp, dPS_Velocity, lpcIon_Data_EO, uiIon_Count, lpcSpectrum[2]);
//	printf("Generate SO\n");
//	Generate_Synow_Multi_Ion_Spectra(dDay, dPS_Temp, dPS_Velocity, lpcIon_Data_SO, uiIon_Count, lpcSpectrum[3]);

//	printf("Plot\n");


	double * lpdSpectra_WL = NULL;
	double * lpdSpectra_Vel = NULL;
	double * lpdSpectra_Flux = NULL;
	double * lpdSpectra_WL_EO = NULL;
	double * lpdSpectra_Flux_EO = NULL;
	double * lpdSpectra_WL_SO = NULL;
	double * lpdSpectra_Flux_SO = NULL;
	double * lpdContinuum_WL = NULL, * lpdContinuum_Flux = NULL;
	unsigned int uiSpectra_Count = 0, uiContinuum_Count = 0, uiSpectra_Count_EO = 0, uiSpectra_Count_SO = 0;

		// Get spectral data in the desired range
	Get_Spectra_Data(lpcSpectrum[0], lpdContinuum_WL, lpdContinuum_Flux, uiContinuum_Count, dRange_Min, dRange_Max);
	Get_Spectra_Data(lpcSpectrum[1], lpdSpectra_WL, lpdSpectra_Flux, uiSpectra_Count, dRange_Min, dRange_Max);
	Get_Spectra_Data(lpcSpectrum[2], lpdSpectra_WL_EO, lpdSpectra_Flux_EO, uiSpectra_Count_EO, dRange_Min, dRange_Max);
	Get_Spectra_Data(lpcSpectrum[3], lpdSpectra_WL_SO, lpdSpectra_Flux_SO, uiSpectra_Count_SO, dRange_Min, dRange_Max);

	double * lpdRef_Spectra_WL[9];
	double * lpdRef_Spectra_Vel[9];
	double * lpdRef_Spectra_Flux[9];

	unsigned int uiRef_Spectra_Count[9];
	unsigned int uiNum_Ref_Spectra = 0;
	double	dRef_Flux = -1.0;
	for (unsigned int uiI = 0; uiI < uiSpectra_Count && dRef_Flux < 0.0; uiI++)
	{
		if (lpdSpectra_WL[uiI] > 6500.0 && lpdSpectra_Flux[uiI] > 0.0)
			dRef_Flux = lpdSpectra_Flux[uiI];
	}
	for (unsigned int uiI = 0; uiI < 9; uiI++)
	{
		lpdRef_Spectra_WL[uiI] = NULL;
		lpdRef_Spectra_Flux[uiI] = NULL;
		lpdRef_Spectra_Vel[uiI] = NULL;
		uiRef_Spectra_Count[uiI] = 0;

		if (cComparison_Spectra[uiI].size() > 0)
		{
			Get_Spectra_Data(cComparison_Spectra[uiI], lpdRef_Spectra_WL[uiNum_Ref_Spectra], lpdRef_Spectra_Flux[uiNum_Ref_Spectra], uiRef_Spectra_Count[uiNum_Ref_Spectra], dRange_Min, dRange_Max);
			double dRef_Comp_Flux = -1.0;
			for (unsigned int uiJ = 0; uiJ < uiRef_Spectra_Count[uiNum_Ref_Spectra] && dRef_Comp_Flux < 0.0; uiJ++)
			{
				if (lpdRef_Spectra_WL[uiNum_Ref_Spectra][uiJ] > 6500.0 && lpdRef_Spectra_Flux[uiNum_Ref_Spectra][uiJ] > 0.0)
					dRef_Comp_Flux = lpdRef_Spectra_Flux[uiNum_Ref_Spectra][uiJ];
			}
			for (unsigned int uiJ = 0; uiJ < uiRef_Spectra_Count[uiNum_Ref_Spectra]; uiJ++)
			{
				lpdRef_Spectra_Flux[uiNum_Ref_Spectra][uiJ] *= (dRef_Flux / dRef_Comp_Flux);
			}
			lpdRef_Spectra_Vel[uiNum_Ref_Spectra] = new double[uiRef_Spectra_Count[uiNum_Ref_Spectra]];
			uiNum_Ref_Spectra++;
		}

	}

	lpdSpectra_Vel = new double[uiSpectra_Count];

	epsplot::LINE_PARAMETERS cLine_Parameters;
	cLine_Parameters.m_dWidth = 1.0;
	cLine_Parameters.m_eColor = epsplot::BLACK;
	cLine_Parameters.m_eStipple = epsplot::SOLID;

	


	sprintf(lpszFilename,"%s.caption.tex",lpszOutput_Name);
	FILE * fileCaption = fopen(lpszFilename,"wt");
	fprintf(fileCaption,"\\caption{Multi ion models %s",lpszModel_List_String);
	fprintf(fileCaption,"with ions ");
	for (unsigned int uiI = 0; uiI < uiIon_Count; uiI++)
	{
		if (uiI != 0)
			fprintf(fileCaption,", ");
		if (uiI == (uiIon_Count - 1))
			fprintf(fileCaption,"and ");
		fprintf(fileCaption,"%i",lpuiIon_List[uiI]);
	}
	fprintf(fileCaption," at %.2f days after explosion with",dDay);
	fprintf(fileCaption, " $v_{ps} = %.2f\\kkms$",dPS_Velocity);
	fprintf(fileCaption," $T_{ps} = %.0f,000\\K$, $\\log C_S = %.2f$, $\\log C_E = %.2f$.}\n",dPS_Temp,dShell_Scalar,dEjecta_Scalar);
	fclose(fileCaption);

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
//	cY_Axis_Parameters.m_dUpper_Limit = 2.0;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);

	unsigned int uiPlot_Data_ID = cPlot.Set_Plot_Data(lpdSpectra_WL, lpdSpectra_Flux, uiSpectra_Count, cLine_Parameters, uiX_Axis, uiY_Axis);
	unsigned int uiPlot_Data_ID_Comp[9];
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
	{
		cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + uiI);
//		cLine_Parameters.m_eStipple = epsplot::SOLID;
		uiPlot_Data_ID_Comp[uiI] = cPlot.Set_Plot_Data(lpdRef_Spectra_WL[uiI], lpdRef_Spectra_Flux[uiI], uiRef_Spectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
	}


	cPlot.Set_Plot_Filename(lpszOutput_Name);
	cPlot.Plot(cPlot_Parameters);


	sprintf(lpszFilename,"%s.OI.eps",lpszOutput_Name);
	cX_Axis_Parameters.Set_Title("Velocity [1,000 km/s]");
	cX_Axis_Parameters.m_dMajor_Label_Size = 24.0;
	cX_Axis_Parameters.m_dLower_Limit = -20.0;
	cX_Axis_Parameters.m_dUpper_Limit = 50.0;
	cX_Axis_Parameters.m_bInvert = true;
	cPlot.Modify_X_Axis_Parameters( uiX_Axis, cX_Axis_Parameters);
	
	double dWL_Ref = Get_WL_Ref(OINIR);
	for (unsigned int uiI = 0; uiI < uiSpectra_Count; uiI++)
	{
		lpdSpectra_Vel[uiI] = -Compute_Velocity(lpdSpectra_WL[uiI], dWL_Ref) * 1.0e-3;
//		printf("%e %e %f\n",lpdSpectra_WL[uiI],lpdSpectra_Vel[uiI],dWL_Ref);
	}
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
		for (unsigned int uiJ = 0; uiJ < uiRef_Spectra_Count[uiI]; uiJ++)
		{
			lpdRef_Spectra_Vel[uiI][uiJ] = -Compute_Velocity(lpdRef_Spectra_WL[uiI][uiJ], dWL_Ref) * 1.0e-3;
		}

	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Modify_Plot_Data(uiPlot_Data_ID,lpdSpectra_Vel, lpdSpectra_Flux, uiSpectra_Count, cLine_Parameters, uiX_Axis, uiY_Axis);
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
	{
		cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + uiI);
		cPlot.Modify_Plot_Data(uiPlot_Data_ID_Comp[uiI],lpdRef_Spectra_Vel[uiI], lpdRef_Spectra_Flux[uiI], uiRef_Spectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
	}

	cPlot.Set_Plot_Filename(lpszFilename);
	cPlot.Plot(cPlot_Parameters);


	sprintf(lpszFilename,"%s.Si6355.eps",lpszOutput_Name);
	
	dWL_Ref = Get_WL_Ref(SI6355);
	for (unsigned int uiI = 0; uiI < uiSpectra_Count; uiI++)
	{
		lpdSpectra_Vel[uiI] = -Compute_Velocity(lpdSpectra_WL[uiI], dWL_Ref) * 1.0e-3;
//		printf("%e %e %f\n",lpdSpectra_WL[uiI],lpdSpectra_Vel[uiI],dWL_Ref);
	}
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
		for (unsigned int uiJ = 0; uiJ < uiRef_Spectra_Count[uiI]; uiJ++)
		{
			lpdRef_Spectra_Vel[uiI][uiJ] = -Compute_Velocity(lpdRef_Spectra_WL[uiI][uiJ], dWL_Ref) * 1.0e-3;
		}
	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Modify_Plot_Data(uiPlot_Data_ID,lpdSpectra_Vel, lpdSpectra_Flux, uiSpectra_Count, cLine_Parameters, uiX_Axis, uiY_Axis);
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
	{
		cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + uiI);
		cPlot.Modify_Plot_Data(uiPlot_Data_ID_Comp[uiI],lpdRef_Spectra_Vel[uiI], lpdRef_Spectra_Flux[uiI], uiRef_Spectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
	}
	cPlot.Set_Plot_Filename(lpszFilename);
	cPlot.Plot(cPlot_Parameters);

	sprintf(lpszFilename,"%s.CaNIR.eps",lpszOutput_Name);
	
	dWL_Ref = Get_WL_Ref(CANIR);
	for (unsigned int uiI = 0; uiI < uiSpectra_Count; uiI++)
	{
		lpdSpectra_Vel[uiI] = -Compute_Velocity(lpdSpectra_WL[uiI], dWL_Ref) * 1.0e-3;
//		printf("%e %e %f\n",lpdSpectra_WL[uiI],lpdSpectra_Vel[uiI],dWL_Ref);
	}
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
		for (unsigned int uiJ = 0; uiJ < uiRef_Spectra_Count[uiI]; uiJ++)
		{
			lpdRef_Spectra_Vel[uiI][uiJ] = -Compute_Velocity(lpdRef_Spectra_WL[uiI][uiJ], dWL_Ref) * 1.0e-3;
		}

	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Modify_Plot_Data(uiPlot_Data_ID,lpdSpectra_Vel, lpdSpectra_Flux, uiSpectra_Count, cLine_Parameters, uiX_Axis, uiY_Axis);
	for (unsigned int uiI = 0; uiI < uiNum_Ref_Spectra; uiI++)
	{
		cLine_Parameters.m_eColor = (epsplot::COLOR)(epsplot::BLACK + uiI);
		cPlot.Modify_Plot_Data(uiPlot_Data_ID_Comp[uiI],lpdRef_Spectra_Vel[uiI], lpdRef_Spectra_Flux[uiI], uiRef_Spectra_Count[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
	}
	cPlot.Set_Plot_Filename(lpszFilename);
	cPlot.Plot(cPlot_Parameters);


	sprintf(lpszFilename,"%s.dat",lpszOutput_Name);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"wl");
	fprintf(fileOut," cont",lpszModel);
	fprintf(fileOut," comb",lpszModel);
	fprintf(fileOut," EO",lpszModel);
	fprintf(fileOut," SO",lpszModel);
	fprintf(fileOut,"\n");
	for (unsigned int uiI = 0; uiI < uiContinuum_Count; uiI++)
	{
		fprintf(fileOut, "%f",lpcSpectrum[0].wl(uiI));
		fprintf(fileOut, " %f",lpcSpectrum[0].flux(uiI));
		fprintf(fileOut, " %f",lpcSpectrum[1].flux(uiI));
		fprintf(fileOut, " %f",lpcSpectrum[2].flux(uiI));
		fprintf(fileOut, " %f",lpcSpectrum[3].flux(uiI));
		fprintf(fileOut, "\n");
	}
	fclose(fileOut);


	delete [] lpdSpectra_WL;
	delete [] lpdSpectra_Vel;
	delete [] lpdSpectra_Flux;
	delete [] lpdSpectra_WL_EO;
	delete [] lpdSpectra_Flux_EO;
	delete [] lpdSpectra_WL_SO;
	delete [] lpdSpectra_Flux_SO;
	delete [] lpdContinuum_WL;
	delete [] lpdContinuum_Flux;
	

	return 0;
}

