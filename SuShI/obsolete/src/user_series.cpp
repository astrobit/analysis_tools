
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
#include <line_routines.h>


XDATASET g_cEjecta;
XDATASET g_cShell;
char	* g_lpszSourceDir = NULL;
double Determine_Photosphere(const char * i_lpszSourceDir, unsigned int i_uiSource_File_ID, 
							const double &  i_dPhotosphere_Reference_Velocity, 
							const double & i_dPhotosphere_Reference_Time,
							const double	& i_dDay)
{
	unsigned int uiTime_Ref_Idx = 96;

	unsigned int uiRadius_Idx = 0;
	unsigned int uiDelta_Radius_Idx = 1;
	unsigned int uiDensity_Idx = 8;
	unsigned int uiVelocity_Idx = 25;
	unsigned int uiDelta_V_Idx = 26;
	unsigned int uiCalcium_Idx = 4;

	unsigned int uiI, uiT;
	double	dScalar = 1.0;
	double	dSum = 0.0;
	unsigned int uiPS_Reference_Idx = 0;
	bool bPS_In_Shell = false;

	double	dPS_Vel = 0.0;

	if (g_lpszSourceDir == NULL || strcmp(i_lpszSourceDir,g_lpszSourceDir) != 0)
	{
		char	lpszFilename[256];
		if (g_lpszSourceDir)
			delete [] g_lpszSourceDir;
		g_lpszSourceDir = new char[strlen(i_lpszSourceDir) + 1];
		strcpy(g_lpszSourceDir,i_lpszSourceDir);
		sprintf(lpszFilename,"%s/ejecta%04i.xdataset",i_lpszSourceDir,i_uiSource_File_ID);
		g_cEjecta.ReadDataFileBin(lpszFilename);
		sprintf(lpszFilename,"%s/shell%04i.xdataset",i_lpszSourceDir,i_uiSource_File_ID);
		g_cShell.ReadDataFileBin(lpszFilename);
	}


	// First, identify the material density at PS vref.  Here, we assume the PS velocity will be 11.5 kkm/s on day 25.
	while (uiPS_Reference_Idx < g_cEjecta.GetNumElements() && g_cEjecta.GetElement(uiVelocity_Idx,uiPS_Reference_Idx) < i_dPhotosphere_Reference_Velocity * 1.0e8)
		uiPS_Reference_Idx++;
	if (uiPS_Reference_Idx >= g_cEjecta.GetNumElements())
	{
		bPS_In_Shell = true;
		uiPS_Reference_Idx = 0;
		while (uiPS_Reference_Idx < g_cShell.GetNumElements() && g_cShell.GetElement(uiVelocity_Idx,uiPS_Reference_Idx) < i_dPhotosphere_Reference_Velocity * 1.0e8)
			uiPS_Reference_Idx++;
	}
	if (!bPS_In_Shell)
	{
		
		for (uiI = uiPS_Reference_Idx; uiI < g_cEjecta.GetNumElements(); uiI++)
		{
			dSum += g_cEjecta.GetElement(uiDensity_Idx,uiI) * g_cEjecta.GetElement(uiDelta_Radius_Idx,uiI) * g_cEjecta.GetElement(uiVelocity_Idx,uiI);
		}
		uiI = 0;
	}
	else
		uiI = uiPS_Reference_Idx;
//	printf("%.2e\t%i\t%.2e\n",dSum,uiI, g_cEjecta.GetElement(uiVelocity_Idx,uiPS_Reference_Idx));		
	double	dShell_Sum = 0.0;
	for (; uiI < g_cShell.GetNumElements(); uiI++)
	{
		dShell_Sum += g_cShell.GetElement(uiDensity_Idx,uiI) * g_cShell.GetElement(uiDelta_Radius_Idx,uiI) * g_cShell.GetElement(uiVelocity_Idx,uiI);
	}
	dSum += dShell_Sum;
	dScalar = pow(i_dPhotosphere_Reference_Time * 24.0 * 3600.0,2.0) / dSum; // this scalar term incorporates the scattering coefficient, reference time of vps and of the source data, and some ionization fraction
//	printf("%.1f %.2e %.2e %i %c\n",i_dDay,dSum,dScalar,uiPS_Reference_Idx,bPS_In_Shell ? 'S' : 'E');
	double dTime_s = i_dDay * (24.0 * 3600.0);
	double	dDay_Const = pow(dTime_s,-2.0) * dScalar;
	double dTau = 0.0;

	uiI = g_cShell.GetNumElements() - 1;
	while (uiI < g_cShell.GetNumElements() && (dTau * dDay_Const) < 1.0)
	{
		dTau += g_cShell.GetElement(uiDensity_Idx,uiI) * g_cShell.GetElement(uiDelta_Radius_Idx,uiI) * g_cShell.GetElement(uiVelocity_Idx,uiI);
		uiI--;
	}
	if ((dTau * dDay_Const) >= 1.0 && uiI < g_cShell.GetNumElements())
	{
		dPS_Vel = g_cShell.GetElement(uiVelocity_Idx,uiI) * 1.0e-8;
//			printf("PS shell %i %.1f %.1e %.1e %.1e\n",uiI, i_dDay,dTau,dPS_Vel,dDay_Const);
	}
	else
	{
		uiI = g_cEjecta.GetNumElements() - 1;
//			printf("%.1e\t",dTau);
		while (uiI < g_cEjecta.GetNumElements() && (dTau * dDay_Const) < 1.0)
		{
			dTau += g_cEjecta.GetElement(uiDensity_Idx,uiI) * g_cEjecta.GetElement(uiDelta_Radius_Idx,uiI) * g_cEjecta.GetElement(uiVelocity_Idx,uiI);
			uiI--;
		}

		if ((dTau * dDay_Const) >= 1.0 && uiI < g_cEjecta.GetNumElements())
		{
			dPS_Vel = g_cEjecta.GetElement(uiVelocity_Idx,uiI) * 1.0e-8;
//			printf("PS ejecta %i %.1f %.1e %.1e %.1e\n",uiI, i_dDay,dTau,dPS_Vel,dDay_Const);
		}
	}
	return dPS_Vel;
}


void CompareFits(const ES::Spectrum &i_cTarget, XDATASET & i_cOpacity_Map_A, XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const double &i_dDay, const double & i_dPS_Vel, const double &i_dPS_Temp, const double & i_dPS_Ion_Temp, const double &i_dPS_Log_Tau, const double & i_dHVF_Ion_Temp, const double &i_dHVF_Log_Tau, ES::Spectrum &o_cOutput)
{
	ES::Spectrum cOutput(i_cTarget);

	unsigned int uiParameters = 7;
	if (i_cOpacity_Map_B.GetNumElements() == 0 || isnan(i_dHVF_Ion_Temp) || isnan(i_dHVF_Log_Tau))
		uiParameters = 5; // don't look for HVF component separately

	XVECTOR cResult(uiParameters);
//	printf("\n");
//	cResult.Print();
	unsigned int uiI = 1;
	while (uiI < i_cOpacity_Map_A.GetNumElements() && i_cOpacity_Map_A.GetElement(uiI,0) < i_dDay)
		uiI++;
	if (uiI > 0)
		uiI--;

	cResult.Set(0,uiI);
	cResult.Set(1,i_dPS_Vel);
	cResult.Set(2,i_dPS_Temp);
	cResult.Set(3,i_dPS_Ion_Temp);
	cResult.Set(4,i_dPS_Log_Tau);
	if (uiParameters > 5)
	{
		cResult.Set(5,i_dHVF_Ion_Temp);
		cResult.Set(6,i_dHVF_Log_Tau);
	}
//	printf("Generating\n");

	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
	o_cOutput = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
}



class PARAMETER_SET
{
public:
	double	dRef25;
	double	dRef_Extreme;
	double	dDecay;

	enum	TYPE	{EXPONENTIAL,POWER,LOG};

	TYPE	eType;

	PARAMETER_SET(void)
	{
		dRef25 = 1.0;
		dRef_Extreme = 0.0;
		dDecay = 1.0;
		eType = LOG;
	}

	double	Get_Value(const double & i_dDay)
	{
		double dRet;
		switch (eType)
		{
		case POWER:
			dRet = pow(i_dDay / 25.0,dDecay) * dRef25 + dRef_Extreme;
			break;
		case EXPONENTIAL:
			dRet = exp((i_dDay - 25.0) / dDecay) * dRef25 + dRef_Extreme;
			break;
		case LOG:
			dRet = log(i_dDay / 25.0) * dDecay + dRef25;
			break;
		}
		return dRet;
	}
	void Set_Type(const char * i_lpszType_String)
	{
		if (i_lpszType_String == NULL || i_lpszType_String[0] == 0)
			eType = LOG;
		else if (strcmp(i_lpszType_String,"pow") == 0 ||
			strcmp(i_lpszType_String,"POW") == 0 ||
			strcmp(i_lpszType_String,"Pow") == 0 ||
			strcmp(i_lpszType_String,"power") == 0 ||
			strcmp(i_lpszType_String,"POWER") == 0 ||
			strcmp(i_lpszType_String,"Power") == 0)
		{
			eType = POWER;
		}
		else if (strcmp(i_lpszType_String,"exp") == 0 ||
			strcmp(i_lpszType_String,"EXP") == 0 ||
			strcmp(i_lpszType_String,"Exp") == 0 ||
			strcmp(i_lpszType_String,"exponential") == 0 ||
			strcmp(i_lpszType_String,"EXPONENTIAL") == 0 ||
			strcmp(i_lpszType_String,"Exponential") == 0)
		{
			eType = EXPONENTIAL;
		}
		else
		{
			eType = LOG;
		}
	}

};

void Read_Parameter(int i_iArg_Count,const char * i_lpszArg_Values[], const char * i_lpszOption_Name, const double &i_dDefault_Decay, PARAMETER_SET	& o_cParameter)
{
	double dNan = nan("");
	char lpszOption_String[64];
	char	lpszType[32];

	o_cParameter.dRef25 =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,i_lpszOption_Name,dNan);
	sprintf(lpszOption_String,"%s-decay",i_lpszOption_Name);
	o_cParameter.dDecay =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,lpszOption_String,i_dDefault_Decay);
	sprintf(lpszOption_String,"%s-law",i_lpszOption_Name);
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,lpszOption_String,lpszType,sizeof(lpszType),NULL);
	o_cParameter.Set_Type(lpszType);
	sprintf(lpszOption_String,"%s-extreme",i_lpszOption_Name);
	o_cParameter.dRef_Extreme =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,lpszOption_String,0.0);

}


SERIESLISTNODE			* lpcSeriesListHead = NULL;
SERIESLISTNODE			* lpcSeriesListTail = NULL;

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	bool bData_valid;
	char lpszTargetFile[256] = {""};
	char lpszOutput_File_Prefix[256] = {""};
	char lpszOutput_Filename[256];
	char lpszTitle[256];
	unsigned int uiIon;
	bool bData_Valid = false;

	double dNan = nan("");
	char lpszSourceDir[256];

	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--sourcedir",lpszSourceDir,256,NULL);

	PARAMETER_SET	cPS_Vel;
	double			dPS_Ref_Velocity = dNan;
	double			dPS_Ref_Time = dNan;
	PARAMETER_SET	cPS_Temp;
	PARAMETER_SET	cPS_Log_Tau;
	PARAMETER_SET	cPS_Ion_Temp;
	PARAMETER_SET	cHVF_Log_Tau;
	PARAMETER_SET	cHVF_Ion_Temp;
	bool			bWith_Shell;
	bool			bNo_Title;
	bool			bUse_Sourcedir_Title;
	bool			bLinear_Scale;
	bool			bUse_Ref_Model_Title_In_Filename;
	char			lpszReference_Model_Title[64] = {0};
	unsigned int 	uiSource_File_ID;
	double			* lpdDays = NULL;
	unsigned int	uiDays_Count = 0;	
	unsigned int uiNum_Reference_Spectra = 0;
    ES::Spectrum  * lpcReference_Spectra = NULL;
	double			*lpdReference_Day = NULL;

	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--ref-model-title",lpszReference_Model_Title,64,NULL);
	bUse_Ref_Model_Title_In_Filename = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--use-ref-model-title-in-filename");
	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--days"))
	{
//		printf("Processing days CL\n");
		unsigned int uiDays_Data = -1;
		const char * lpszCursor;
		for (unsigned int uiI = 1; uiI < i_iArg_Count && uiDays_Data == -1; uiI++)
		{
			if (strncmp(i_lpszArg_Values[uiI],"--days=",7) == 0)
				uiDays_Data = uiI;
			else if (strcmp(i_lpszArg_Values[uiI],"--days") == 0 && i_lpszArg_Values[uiI + 1][0] == '=')
				uiDays_Data = uiI + 1;
		}
		lpszCursor = i_lpszArg_Values[uiDays_Data];
		if (strncmp(i_lpszArg_Values[uiDays_Data],"--days",6) == 0)
			lpszCursor += 6;
		if (lpszCursor[0] == 0)
		{
			uiDays_Data++;
			lpszCursor = i_lpszArg_Values[uiDays_Data];
		}
		if (lpszCursor[0] == '=')
			lpszCursor++;
		if (lpszCursor[0] == 0)
		{
			uiDays_Data++;
			lpszCursor = i_lpszArg_Values[uiDays_Data];
		}
		unsigned int uiStart_Ref = uiDays_Data;
		const char * lpszStart_Cursor = lpszCursor;

		// first count how many items are in the list
		while (xIsANumber(lpszCursor) && uiDays_Data < i_iArg_Count)
		{
//			printf("%s\n",lpszCursor);
			if (xIsANumber(lpszCursor))
				uiDays_Count++;
			while (xIsANumber(lpszCursor))
			{
				lpszCursor++;
			}
			if (lpszCursor[0] == 0)
			{
				uiDays_Data++;
				if (uiDays_Data < i_iArg_Count)
				{
					lpszCursor = i_lpszArg_Values[uiDays_Data];
				}
			}
			if (lpszCursor[0] == ',')
				lpszCursor++;
			if (lpszCursor[0] == 0)
			{
				uiDays_Data++;
				if (uiDays_Data < i_iArg_Count)
				{
					lpszCursor = i_lpszArg_Values[uiDays_Data];
				}
			}
		}
		lpdDays = new double[uiDays_Count];
		uiDays_Data =  uiStart_Ref;
		lpszCursor = lpszStart_Cursor;
//		printf("%s\n",lpszStart_Cursor);
		uiDays_Count=0;
			// first count how many items are in the list
		while (lpszCursor[0] != 0 && xIsANumber(lpszCursor) && uiDays_Data < i_iArg_Count)
		{
//			printf("%i: %s\n",uiDays_Count,lpszCursor);
			if (xIsANumber(lpszCursor))
			{
				lpdDays[uiDays_Count] = atof(lpszCursor);
				uiDays_Count++;
			}
			while (xIsANumber(lpszCursor) && lpszCursor[0] != 0)
			{
				lpszCursor++;
			}
			if (lpszCursor[0] == ',')
				lpszCursor++;
		}
//		printf("Days count %i\n",	uiDays_Count);
	}
	else
	{
		uiDays_Count = 11;
		lpdDays = new double[uiDays_Count];
		for (unsigned int uiI = 0; uiI < uiDays_Count; uiI++)
			lpdDays[uiI] = 5.0 + 2.0 * uiI;
	}

	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-value"))
	{
		dPS_Ref_Velocity = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-value",dNan);
		dPS_Ref_Time = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-time",dNan);
		uiSource_File_ID = xParse_Command_Line_UInt(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-file",-1);
	}
	Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--PS-vel",-0.3,cPS_Vel);
	Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--PS-temp",-1.0,cPS_Temp);
	Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--PS-ion-log-tau",-0.1,cPS_Log_Tau);
	Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--PS-ion-temp",-0.1,cPS_Ion_Temp);

	bWith_Shell = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-log-tau");
	
	if (bWith_Shell)
	{
		Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-log-tau",-0.1,cHVF_Log_Tau);
		Read_Parameter(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-temp",-0.1,cHVF_Ion_Temp);
	}
	uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);
	sprintf(lpszOutput_File_Prefix,"%s/series",lpszSourceDir);

	bNo_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-plot-title");
	bUse_Sourcedir_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--use-sourcedir-title");
	bLinear_Scale = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--linear-scale");

	if (xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "--serieslist"))
	{
		char lpszSeriesListFilename[128];
		char lpszSeriesListDirectory[128];
		char	lpszTemp[256];
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--serieslist",lpszSeriesListFilename,128,"");
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--serieslist-directory",lpszSeriesListDirectory,128,"");
		FILE * fileIn = fopen(lpszSeriesListFilename,"rt");
		if (fileIn)
		{
			char * lpszBuffer = new char [1024];
			while (fgets(lpszBuffer,sizeof(char) * 1024, fileIn))
			{
				char * lpszCursor = strchr(lpszBuffer,',');
				if (lpszBuffer[0] != 0 && lpszCursor)
				{
					lpszCursor--;
					while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						lpszCursor--;
					lpszCursor++;
					lpszCursor[0] = 0;
					lpszCursor++;
					while (lpszCursor[0] != 0 && !((lpszCursor[0] >= '0' && lpszCursor[0] <= '9') || lpszCursor[0] == '-' || lpszCursor[0] == '+'))
						lpszCursor++;
					SERIESLISTNODE * lpCurr = new SERIESLISTNODE;
					lpCurr->m_dDay = atof(lpszCursor);
					lpszCursor = lpszBuffer;
					while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						lpszCursor++;
					if (lpszSeriesListDirectory[0] != 0)
						sprintf(lpszTemp,"%s/%s",lpszSeriesListDirectory,lpszCursor);
					else
						strcpy(lpszTemp,lpszCursor);
					lpCurr->m_lpszFilename = new char[strlen(lpszTemp) + 1];
					strcpy(lpCurr->m_lpszFilename,lpszTemp);


					uiNum_Reference_Spectra++;
					if (!lpcSeriesListTail)
					{
						lpcSeriesListTail = lpcSeriesListHead = lpCurr;
					}
					else
					{
						lpcSeriesListTail->m_lpcNext = lpCurr;
						lpCurr->m_lpcPrev = lpcSeriesListTail;
						lpcSeriesListTail = lpCurr;
					}
				}
			}
		}
	}
//	printf("Series list processed\n");
	if (uiNum_Reference_Spectra > 0)
	{
		lpcReference_Spectra = new ES::Spectrum[uiNum_Reference_Spectra];
		lpdReference_Day = new double[uiNum_Reference_Spectra];
		unsigned int uiCurr_Idx = 0;
		SERIESLISTNODE * lpCurr = lpcSeriesListHead;
		while (lpCurr)
		{
		    lpcReference_Spectra[uiCurr_Idx] = ES::Spectrum::create_from_ascii_file( lpCurr->m_lpszFilename );
		
			lpdReference_Day[uiCurr_Idx] = lpCurr->m_dDay + 20.0;
			if (lpcReference_Spectra[uiCurr_Idx].size() == 0)
				printf("File %s seems to be empty!\n",lpCurr->m_lpszFilename);
			uiCurr_Idx++;
			lpCurr = lpCurr->m_lpcNext;
		}
	}
//	printf("Series list loaded\n");

	// make sure all necessary data is available to generate the spectrum
	bData_Valid = !isnan(cPS_Vel.dRef25) || (!isnan(dPS_Ref_Velocity) && !isnan(dPS_Ref_Time) && uiSource_File_ID != -1);
	bData_Valid &= !isnan(cPS_Temp.dRef25) && !isnan(cPS_Log_Tau.dRef25) && !isnan(cPS_Ion_Temp.dRef25);
	if (bWith_Shell)
		bData_valid &= !isnan(cHVF_Log_Tau.dRef25) && !isnan(cHVF_Ion_Temp.dRef25);
	bData_Valid &= (uiIon != -1);
	const char * lpszElement_ID;
	switch (uiIon / 100)
	{
	case 6:
		lpszElement_ID = "C";
		break;
	case 8:
		lpszElement_ID = "O";
		break;
	case 10:
	case 12:
		lpszElement_ID = "Mg";
		break;
	case 14: // Si
	case 16: // S
	case 18: // Ar
	case 20: // Ca
	case 22: // Ti 
	case 23: // V
	case 24: // Cr
	case 25: // Mn
		lpszElement_ID = "Si";
		break;
	case 26:
	case 27:
	case 28:
		lpszElement_ID = "Fe";
		break;
	}


	if (bData_Valid)
	{
        ES::Spectrum cTarget;
		cTarget = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 5.0);

		char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64], lpszParameters_File[64];
		XDATASET cOpacity_Map_Shell;
		XDATASET cOpacity_Map_Ejecta;
		sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.%s.xdataset",lpszSourceDir,lpszElement_ID);
		sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",lpszSourceDir,lpszElement_ID);
		sprintf(lpszParameters_File,"%s.%s.user.parameters.csv",lpszOutput_File_Prefix,lpszSourceDir);

		if (bWith_Shell)
			cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
		cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
        ES::Spectrum * cSpectra = new ES::Spectrum[uiDays_Count];
		FILE * fileOutParameters = fopen(lpszParameters_File,"wt");
		fprintf(fileOutParameters,"Day, Ion, PS Velocity, PS Temp, PS LGR Scale, PS LGR Temp, HVF LGR Scale, HVF LGR Temp\n");

		for (unsigned int uiI = 0; uiI < uiDays_Count; uiI++)
		{
			cSpectra[uiI] = cTarget;
			double	dPS_Vel;
//			printf("%i %f\n",uiI,dDay[uiI]);
			if (!isnan(dPS_Ref_Velocity) && !isnan(dPS_Ref_Time))
				dPS_Vel = Determine_Photosphere(lpszSourceDir, uiSource_File_ID, dPS_Ref_Velocity,dPS_Ref_Time, lpdDays[uiI]);
			else			
				dPS_Vel = cPS_Vel.Get_Value(lpdDays[uiI]);
			printf("Day %.2f PS = %.2f\n",lpdDays[uiI],dPS_Vel);
			double	dPS_Temp = cPS_Temp.Get_Value(lpdDays[uiI]);
			double	dPS_Ion_Scale = cPS_Log_Tau.Get_Value(lpdDays[uiI]);
			double	dPS_Ion_Temp = cPS_Ion_Temp.Get_Value(lpdDays[uiI]);
			double	dHVF_Ion_Scale = cHVF_Log_Tau.Get_Value(lpdDays[uiI]);
			double	dHVF_Ion_Temp = cHVF_Ion_Temp.Get_Value(lpdDays[uiI]);

			CompareFits(cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, lpdDays[uiI], dPS_Vel, dPS_Temp, dPS_Ion_Temp, dPS_Ion_Scale, dHVF_Ion_Temp, dHVF_Ion_Scale, cSpectra[uiI]);
			fprintf(fileOutParameters,"%.0f,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",lpdDays[uiI], uiIon, dPS_Vel, dPS_Temp, dPS_Ion_Scale, dPS_Ion_Temp, dHVF_Ion_Scale, dHVF_Ion_Temp);
		}
		fclose(fileOutParameters);

		if (bNo_Title)
			lpszTitle[0] = 0;
		else if (bUse_Sourcedir_Title && bUse_Ref_Model_Title_In_Filename)
			sprintf(lpszTitle,"%s : %s", lpszSourceDir, lpszReference_Model_Title);
		else if (bUse_Sourcedir_Title)
			strcpy(lpszTitle,lpszSourceDir);
		else if (bWith_Shell)
			sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f thv %.2f Thv %.2f",lpszSourceDir,cPS_Vel.dRef25,cPS_Temp.dRef25,cPS_Log_Tau.dRef25,cPS_Ion_Temp.dRef25,cHVF_Log_Tau.dRef25,cHVF_Ion_Temp.dRef25);
		else
			sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f",lpszSourceDir,cPS_Vel.dRef25,cPS_Temp.dRef25,cPS_Log_Tau.dRef25,cPS_Ion_Temp.dRef25);
//		printf("Plotting\n");
		if (bUse_Ref_Model_Title_In_Filename)
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.%s.full.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon,lpszReference_Model_Title);
		else
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.full.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon);

		PlotSeries(cSpectra, lpdDays, NULL, uiDays_Count, lpszOutput_Filename, cTarget.wl(0), cTarget.wl(cTarget.size() - 1), bLinear_Scale, lpszTitle, -1.0, false, false, -1.0,lpcReference_Spectra,lpdReference_Day, uiNum_Reference_Spectra);
		if (bUse_Ref_Model_Title_In_Filename)
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.%s.CaNIR.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon,lpszReference_Model_Title);
		else
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.CaNIR.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon);
		PlotSeries(cSpectra, lpdDays, NULL, uiDays_Count, lpszOutput_Filename, 7000.0, 9000.0, bLinear_Scale, lpszTitle, -1.0, true, true, 8579.0,lpcReference_Spectra,lpdReference_Day, uiNum_Reference_Spectra, 7400.0, 8300.0);
		if (bUse_Ref_Model_Title_In_Filename)
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.%s.CaHK.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon,lpszReference_Model_Title);
		else
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.CaHK.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon);
		PlotSeries(cSpectra, lpdDays, NULL, uiDays_Count, lpszOutput_Filename, 3000.0, 5000.0, bLinear_Scale, lpszTitle, -1.0, true, true, 3945.0,lpcReference_Spectra,lpdReference_Day, uiNum_Reference_Spectra, 3500.0, 4300.0);
		if (bUse_Ref_Model_Title_In_Filename)
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.%s.Si6355.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon,lpszReference_Model_Title);
		else
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.Si6355.eps",lpszOutput_File_Prefix,lpszSourceDir, uiIon);
		PlotSeries(cSpectra, lpdDays, NULL, uiDays_Count, lpszOutput_Filename, 5000.0, 7000.0, bLinear_Scale, lpszTitle, -1.0, true, true, 6355.0,lpcReference_Spectra,lpdReference_Day, uiNum_Reference_Spectra, 5500.0, 6500.0);
		for (unsigned int uiI = 0; uiI < uiDays_Count; uiI++)
		{
			sprintf(lpszOutput_Filename,"%s.%s.user.%i.day%.2f.csv",lpszOutput_File_Prefix,lpszSourceDir,uiIon,lpdDays[uiI]);
			FILE * fileOut = fopen(lpszOutput_Filename,"wt");
			for (unsigned int uiJ = 0; uiJ < cSpectra[uiI].size(); uiJ++)
				fprintf(fileOut, "%f %f %f\n",cSpectra[uiI].wl(uiJ), cSpectra[uiI].flux(uiJ), cSpectra[uiI].flux_error(uiJ));
			fclose(fileOut);
		}
		printf("Done\n");
	}		

	return 0;
}

