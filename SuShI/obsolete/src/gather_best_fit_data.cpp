#include <best_fit_data.h>
#include <opacity_profile_data.h>
#include <stdio.h>

bool Is_Leap_Year(int i_iYear)
{
	return  ((i_iYear % 4) == 0 && ((i_iYear % 400 == 0) || (i_iYear % 100 != 0)));
}
class LINE_REFERENCE_DATA
{
public:
	unsigned int m_uiIon;
	double		m_dReference_Wavelength_A;
	double		m_dOscillator_Strength;
	double		m_dExcitation_Energy_eV;

	LINE_REFERENCE_DATA(void)
	{
		m_uiIon = 0;
		m_dReference_Wavelength_A = 0.0;
		m_dOscillator_Strength = 0.0;
		m_dExcitation_Energy_eV = 0.0;
	}		
};

LINE_REFERENCE_DATA Read_Reference_Line_Data(unsigned int i_uiIon)
{
	char * lpszFilepath = getenv("SYNXX_REF_LINE_DATA_PATH");
	LINE_REFERENCE_DATA	cData;
	if (lpszFilepath)
	{
		FILE * fileRef = fopen(lpszFilepath,"rt");
		char	lpszBuffer[128];
		if (fileRef)
		{
			while (cData.m_uiIon == 0 && fgets(lpszBuffer,sizeof(lpszBuffer),fileRef))
			{
				char * lpszCursor = lpszBuffer;
				while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
					lpszCursor++;
				unsigned int uiIon = atoi(lpszCursor);
				if (uiIon == i_uiIon)
				{
					cData.m_uiIon = uiIon;
					while (lpszCursor[0] != 0 && lpszCursor[0] != ' ' && lpszCursor[0] != '\t')
						lpszCursor++;
					while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						lpszCursor++;
					cData.m_dReference_Wavelength_A = atof(lpszCursor);
					while (lpszCursor[0] != 0 && lpszCursor[0] != ' ' && lpszCursor[0] != '\t')
						lpszCursor++;
					while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						lpszCursor++;
					cData.m_dOscillator_Strength = atof(lpszCursor);
					while (lpszCursor[0] != 0 && lpszCursor[0] != ' ' && lpszCursor[0] != '\t')
						lpszCursor++;
					while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
						lpszCursor++;
					cData.m_dExcitation_Energy_eV = atof(lpszCursor);
				}
			}
			if (cData.m_uiIon == 0)
				fprintf(stderr,"Unable to find line reference data for %i\n",i_uiIon);
			fclose(fileRef);
		}
	}
	return cData;
}


int Get_Day_Of_Year(int i_iYear, int i_iMonth, int i_iDay)
{
	int iDay = i_iDay;
	switch (i_iMonth)
	{
	case 12:
		iDay += 30; // November
	case 11:
		iDay += 31; // October
	case 10:
		iDay += 30; // September
	case 9:
		iDay += 31; // August
	case 8:
		iDay += 31; // July
	case 7:
		iDay += 30; // June
	case 6:
		iDay += 31; // May
	case 5:
		iDay += 30; // April
	case 4:
		iDay += 31; // March
	case 3:
		iDay += 28; // February
		if (Is_Leap_Year(i_iYear))
			iDay ++; // February - leap year
	case 2:
		iDay += 31; // January
	}
	return iDay;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit_Data;
	OPACITY_PROFILE_DATA	cOP_Data;
	OPACITY_PROFILE_DATA::GROUP	eGroup;
	double dLine_Const;
	double dTime_Const;
	double dMass_i;
	double dTemp_Term;
	double dIonization;
	double dShell_Abundance;
	double	dAbd,dRel_Abd,dScalar,dRef_Dens;

	bool bDebug = false;

	FILE * fileOut = fopen("fit.user.csv","rt");
	if (fileOut == NULL)
	{
		fileOut = fopen("fit.user.csv","wt");
		if (fileOut)
		{
			fprintf(fileOut,"model, Date, Fit Region, MJD, Model day, PS velocity, PS temp, PVF scaling, HVF scaling, PS Abundance, PS ionization, HVF abundance, PVF velocity, HVF velocity, PVF pEW, HVF pEW");
			for (unsigned int uiI = 1; uiI <= 6; uiI++)
			{
				fprintf(fileOut,", Fit Raw Moment %i, Fit Central Moment %i, Fit Standardized Moment %i",uiI,uiI,uiI);
			}
			fprintf(fileOut,"\n");
		}
	}
	else
	{
		fclose(fileOut);
		fileOut = fopen("fit.user.csv","at");
	}
	for (unsigned int uiI = 1; uiI < i_iArg_Count && !bDebug; uiI++)
	{
		bDebug |= (strcmp(i_lpszArg_Values[uiI],"--debug") == 0);
	}
	
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		if (strcmp(i_lpszArg_Values[uiI],"--debug") != 0)
		{
			if (bDebug)
				printf("Reading %s\n",i_lpszArg_Values[uiI]);
			if (strstr(i_lpszArg_Values[uiI],"databin"))
				cFit_Data.ReadBinary(i_lpszArg_Values[uiI],false);
			else
				cFit_Data.ReadFromFile(i_lpszArg_Values[uiI]);
			if (bDebug)
				printf("Parsing Date\n");

			const char * lpszDate_Cursor = i_lpszArg_Values[uiI] + 9;
			char lpszDate_String[9];
			strncpy(lpszDate_String,lpszDate_Cursor,8);
			lpszDate_String[8] = 0;
			char lpszModel[3];
			strncpy(lpszModel,cFit_Data.lpszBest_Fit_File+3,2);
			lpszModel[2] = 0;
			char lpszScalars_Path[256];
			sprintf(lpszScalars_Path,"/work/01925/astrobit/SNexplosion.4.01.64/run%s/opacity_map_scalars.opdata",lpszModel);
			cOP_Data.Load(lpszScalars_Path);
//			printf("%s %e\n",lpszScalars_Path,cOP_Data.m_dReference_Velocity_Shell);
			char lpszFit_Region[16];
			if (bDebug)
				printf("Parsing Fit Region\n");
			const char * lpszCursor = strstr(i_lpszArg_Values[uiI],".run");
			lpszCursor += 7;
			unsigned int uiIdx = 0;
			while (uiIdx < 16 && lpszCursor[uiIdx] != '.')
			{
				lpszFit_Region[uiIdx] = lpszCursor[uiIdx];
				uiIdx++;
			}
			lpszFit_Region[uiIdx] = 0;
			double	df,dLambda,dLine_Const;
			LINE_REFERENCE_DATA cLine_Data;
			if (strcmp(lpszFit_Region,"CaNIR") == 0)
			{
				cLine_Data = Read_Reference_Line_Data(2001);
//				cLine_Data.m_dOscillator_Strength = 1.0 + 0.93 + 0.1;
//				cLine_Data.m_dReference_Wavelength_A = (8203.97 + 8251.07 + 8256.99) / 3.0;
				dRel_Abd = 0.03367675;
				dMass_i = 39.96259098;
				eGroup = OPACITY_PROFILE_DATA::SILICON;

			}
			else if (strcmp(lpszFit_Region,"CaHK") == 0)
			{
				cLine_Data = Read_Reference_Line_Data(2001);
//				cLine_Data.m_dOscillator_Strength = 0.682 + 0.330;
//				cLine_Data.m_dReference_Wavelength_A = (3934.777  + 3969.592) * 0.5;
				dRel_Abd = 0.03367675;
				dMass_i = 39.96259098;
				eGroup = OPACITY_PROFILE_DATA::SILICON;
			}
			else if (strcmp(lpszFit_Region,"Si6355") == 0)
			{
				cLine_Data = Read_Reference_Line_Data(1401);
//				cLine_Data.m_dOscillator_Strength = 0.705 + 0.414;
//				cLine_Data.m_dReference_Wavelength_A = (6348.85  + 6373.12) * 0.5;
				dRel_Abd = 0.656090633;
				dMass_i = 27.9769265325;
				eGroup = OPACITY_PROFILE_DATA::SILICON;
			}
			else if (strcmp(lpszFit_Region,"OI8446") == 0)
			{
				cLine_Data = Read_Reference_Line_Data(800);
//				cLine_Data.m_dOscillator_Strength = 0.468 + 0.335 + 0.201;
//				cLine_Data.m_dReference_Wavelength_A = (7774.083  + 7776.305 + 7777.528) / 3.0;
				dMass_i = 15.99491461956;
				dRel_Abd = 1.0;
				eGroup = OPACITY_PROFILE_DATA::OXYGEN;
			}
			dScalar = cOP_Data.Get_Scalar(eGroup) * dRel_Abd; // account for Ca/(Si + Ca + ...) - Seitenzahl et al 2013, N100
			dAbd = cOP_Data.Get_Abundance(eGroup);
			dTime_Const = pow(cOP_Data.m_dReference_Time_s / cOP_Data.Get_Normalization_Time(eGroup),2.0) * cOP_Data.m_dReference_Time_s;
			dRef_Dens = cOP_Data.Get_Density(eGroup);


			dLine_Const = 2.654e-2 * cLine_Data.m_dOscillator_Strength * cLine_Data.m_dReference_Wavelength_A * 1e-8; // 2.654e-2 = pi e^2 / (m_e c); 1e-8 = conversion from Angstrom to cm
			dMass_i *= 1.660538921e-24; // convert from AMU to g
			dTemp_Term = exp(-cLine_Data.m_dExcitation_Energy_eV/(8.6173324e-5 * cFit_Data.dBest_Fit_PS_Temp * 1000.0));
			if (dTemp_Term == 0.0)
				printf("%e %e\n",cLine_Data.m_dExcitation_Energy_eV,cFit_Data.dBest_Fit_PS_Temp);
//			printf("%e %e %e %e\n",dLine_Const,dTime_Const,dMass_i,dTemp_Term);
			if (dTemp_Term == 0.0 || dTime_Const == 0.0 || dLine_Const == 0.0 || dAbd == 0.0 || dRel_Abd == 0.0)
				printf("Warning - term = 0: %e %e %e %e %e\n",dTemp_Term, dTime_Const, dLine_Const, dAbd, dRel_Abd);
			dIonization = pow(10.0,cFit_Data.dBest_Fit_PS_Log_Tau) * dScalar * dMass_i / (dTemp_Term * dTime_Const * dLine_Const * dRef_Dens * dRel_Abd * dAbd);

//			double	dElectron_Density = cOP_Data.m_dReference_Density_Ejecta / (2.0 * 1.67e-24)
			dShell_Abundance = pow(10.0,cFit_Data.dBest_Fit_HVF_Log_Tau) * cOP_Data.Get_Scalar(OPACITY_PROFILE_DATA::SHELL) * dMass_i / (dTemp_Term * dTime_Const * dLine_Const * cOP_Data.Get_Density(OPACITY_PROFILE_DATA::SHELL) * dIonization);


			fprintf(fileOut,"%s, %s, %s, %.2f, %.2f, %.4f, %.4f, %.4f, %.4f, %.17e, %.17e, %.17e, %.4f, %.4f, %.17e, %.17e",lpszModel,lpszDate_String, lpszFit_Region, cFit_Data.dSource_Data_MJD, cFit_Data.dBest_Fit_Day, cFit_Data.dBest_Fit_PS_Vel, cFit_Data.dBest_Fit_PS_Temp, cFit_Data.dBest_Fit_PS_Log_Tau, cFit_Data.dBest_Fit_HVF_Log_Tau,dAbd,dIonization,dShell_Abundance, cFit_Data.m_dPS_Absorption_Peak_Velocity, cFit_Data.m_dHVF_Absorption_Peak_Velocity,cFit_Data.m_dPVF_Pseudo_Equivalent_Width, cFit_Data.m_dHVF_Pseudo_Equivalent_Width);
			for (unsigned int uiI = 0; uiI < cFit_Data.GetNumMoments(); uiI++)
			{
				double dRaw,dCentral,dStandardized;
				cFit_Data.GetMoments(uiI + 1,dRaw,dCentral,dStandardized);
				fprintf(fileOut,", %.17e, %.17e, %.17e",dRaw,dCentral,dStandardized);
			}
			fprintf(fileOut,"\n");
		}
	}
}
