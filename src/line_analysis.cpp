#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <line_anal.h>


void Bracket_Value(const double &i_dSearch_Value, const XDATASET &i_cAbundance_Data,const XDATASET &i_cDensity_Data, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double & i_dTime_Ref,unsigned int * o_uiLow_Index_Values, double * o_dInterpolation_Values, unsigned int i_uiMax_Values, unsigned int &o_uiNum_Values, bool &o_bMore_Points)
{
	unsigned int uiNum_Points = i_cAbundance_Data.GetNumElements() - 1;
	unsigned int uiI;

	o_uiNum_Values = 0;
	double	dLow = Optical_Depth(i_cAbundance_Data.GetElement(1,0) * i_cDensity_Data.GetElement(1,0),i_cLine_Data,i_dFluid_Temp,i_dTime,i_dTime_Ref);
	o_bMore_Points = false;

//	printf("----------------\n Search: %.3f\n",i_dSearch_Value);
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		double	dNext = Optical_Depth(i_cAbundance_Data.GetElement(1,uiI + 1) * i_cDensity_Data.GetElement(1,uiI + 1),i_cLine_Data,i_dFluid_Temp,i_dTime,i_dTime_Ref);
		if ((i_dSearch_Value >= dLow && i_dSearch_Value <= dNext && (dLow < dNext)) ||
			(i_dSearch_Value <= dLow && i_dSearch_Value >= dNext && (dNext < dLow)))
		{
//			printf("%.3f - %.3f\n",dLow,dNext);
			if (o_uiNum_Values < i_uiMax_Values)
			{
				o_uiLow_Index_Values[o_uiNum_Values] = uiI;
				o_dInterpolation_Values[o_uiNum_Values] = (i_dSearch_Value - dLow) / (dNext - dLow);
				o_uiNum_Values++;
			}
			else
				o_bMore_Points = true;
		}
		dLow = dNext;
	}
//	printf("----------------\n");
}

double ComputeDensity(unsigned int uiIndex, const XDATASET &i_cAbundance_Data,const XDATASET &i_cDensity_Data, const XDATASET &i_cHe3_Data,const XDATASET &i_cHyd_Data)
{
	double	dAbundance = i_cAbundance_Data.GetElement(1,uiIndex);
	if (dAbundance < 1.01e-10) // for some reason we seem to be getting a floor of 1e-10 in FLASH.
		dAbundance = 0.0; 
	double dDensity = dAbundance * i_cDensity_Data.GetElement(1,uiIndex);
	if (i_cHyd_Data.GetElement(1,uiIndex) > 1.0e-5 && i_cHe3_Data.GetElement(1,uiIndex) < 1.0e-5) // in the CSM
		dDensity *= 1e-15;
	return dDensity;
}

void Bracket_Pairs(const double &i_dSearch_Value, const XDATASET &i_cAbundance_Data,const XDATASET &i_cDensity_Data, const XDATASET &i_cHe3_Data,const XDATASET &i_cHyd_Data, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double & i_dTime_Ref,unsigned int * o_uiLow_Index_Values, double * o_dLow_Interpolation_Values, unsigned int * o_uiHigh_Index_Values, double * o_dHigh_Interpolation_Values, unsigned int i_uiMax_Values, unsigned int &o_uiNum_Pairs, bool &o_bMore_Points)
{
	unsigned int uiNum_Points = i_cAbundance_Data.GetNumElements() - 1;
	unsigned int uiI;

	o_uiNum_Pairs = 0;
	double	dLow = Optical_Depth(ComputeDensity(0, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data),i_cLine_Data,i_dFluid_Temp,i_dTime,i_dTime_Ref);
	o_bMore_Points = false;

//	printf("----------------\n Search: %.3f\n",i_dSearch_Value);
	uiI = 0;
	while (uiI < uiNum_Points)
	{
		double	dPrev = dLow;
		if (uiI != 0)
		{
			dPrev = Optical_Depth(ComputeDensity(uiI - 1, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref);
		}

		if (dLow > i_dSearch_Value)
		{
			if (o_uiNum_Pairs < i_uiMax_Values)
			{
				o_uiLow_Index_Values[o_uiNum_Pairs] = uiI;
				if (uiI != 0)
					o_dLow_Interpolation_Values[o_uiNum_Pairs] = (i_dSearch_Value - dLow) / (dPrev - dLow);
				else
					o_dLow_Interpolation_Values[o_uiNum_Pairs] = 0.0;
			}
			else 
				o_bMore_Points = true;
			
			uiI++;
			while (Optical_Depth(i_cAbundance_Data.GetElement(1,uiI) * i_cDensity_Data.GetElement(1,uiI),i_cLine_Data,i_dFluid_Temp,i_dTime,i_dTime_Ref) > (i_dSearch_Value) && uiI < uiNum_Points)
			{
				uiI++;
			}
			dLow = Optical_Depth(ComputeDensity(uiI, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref);
			dPrev = Optical_Depth(ComputeDensity(uiI - 1, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref);
			if (o_uiNum_Pairs < i_uiMax_Values)
			{
				o_dHigh_Interpolation_Values[o_uiNum_Pairs] = (i_dSearch_Value - dLow) / (dPrev - dLow);
				o_uiHigh_Index_Values[o_uiNum_Pairs] = uiI - 1;
				o_uiNum_Pairs++;
			}
		}
		uiI++;
		dLow = Optical_Depth(ComputeDensity(uiI, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref);
	}

//	printf("----------------\n");
}

void Velocity_Lookup(const XDATASET &i_cVelocity_Data, const unsigned int * i_uiLow_Index_Values, const double * i_dInterpolation_Values, unsigned int i_uiNum_Values, double * o_dVelocity_Values)
{
	unsigned int uiI;

	for (uiI = 0; uiI < i_uiNum_Values; uiI++)
	{
		o_dVelocity_Values[uiI] = i_cVelocity_Data.GetElement(1,i_uiLow_Index_Values[uiI]) * (1.0 - i_dInterpolation_Values[uiI]) + i_cVelocity_Data.GetElement(1,i_uiLow_Index_Values[uiI] + 1) * i_dInterpolation_Values[uiI];
	}
}

void LineData(const XDATASET &i_cVelocity_Data,const XDATASET &i_cAbundance_Data,const XDATASET &i_cDensity_Data, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double & i_dTime_Ref, const char * lpszLine_Name)
{
	unsigned int	uiLow_Index_Values[128];
	double			dInterpolation_Values[128];
	double			dVelocity_Values[128];
	unsigned int	uiNum_Values;
	bool			bExcess_Values;

	unsigned int uiI, uiJ;

	Bracket_Value(1.0,i_cAbundance_Data,i_cDensity_Data, i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref,uiLow_Index_Values,dInterpolation_Values,128,uiNum_Values,bExcess_Values);
	Velocity_Lookup(i_cVelocity_Data,uiLow_Index_Values,dInterpolation_Values,uiNum_Values,dVelocity_Values);

	printf("----- %s -----\n",lpszLine_Name);
	if (bExcess_Values)
		printf("----------------------------- Warning: Excess Values\n");
	for (uiJ = 0; uiJ < uiNum_Values; uiJ++)
	{
		printf("%.2e km/s\n",dVelocity_Values[uiJ] * 10000.0);
	}
}

void BroadLineData(const XDATASET &i_cVelocity_Data, const XDATASET &i_cAbundance_Data,const XDATASET &i_cDensity_Data, const XDATASET &i_cHe3_Data, const XDATASET &i_cHyd_Data, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double & i_dTime_Ref, const char * lpszLine_Name)
{
	unsigned int	uiLow_Index_Values[128];
	double			dLow_Interpolation_Values[128];
	unsigned int	uiHigh_Index_Values[128];
	double			dHigh_Interpolation_Values[128];
	double			dVelocity_Values_Low[128];
	double			dVelocity_Values_High[128];
	unsigned int	uiNum_Values;
	bool			bExcess_Values;

	unsigned int uiI, uiJ;

	Bracket_Pairs(1.0,i_cAbundance_Data,i_cDensity_Data, i_cHe3_Data, i_cHyd_Data, i_cLine_Data, i_dFluid_Temp, i_dTime, i_dTime_Ref,uiLow_Index_Values,dLow_Interpolation_Values,uiHigh_Index_Values,dHigh_Interpolation_Values,128,uiNum_Values,bExcess_Values);
	Velocity_Lookup(i_cVelocity_Data,uiLow_Index_Values,dLow_Interpolation_Values,uiNum_Values,dVelocity_Values_Low);
	Velocity_Lookup(i_cVelocity_Data,uiHigh_Index_Values,dHigh_Interpolation_Values,uiNum_Values,dVelocity_Values_High);

	printf("----- %s -----\n",lpszLine_Name);
	if (bExcess_Values)
		printf("----------------------------- Warning: Excess Values\n");
	for (uiJ = 0; uiJ < uiNum_Values; uiJ++)
	{
		printf("%.2e - %.2e km/s\n",dVelocity_Values_Low[uiJ] * 10000.0,dVelocity_Values_High[uiJ] * 10000.0);
	}
}

void BroadLineDataCSV(const XDATASET &i_cVelocity_Data, const XDATASET &i_cAbundance_Data,const XDATASET &i_cHe3_Data,const XDATASET &i_cHyd_Data,const XDATASET &i_cDensity_Data, const LINE_DATA & i_cLine_Data, const double & i_dTime_Ref, const char * lpszLineName, const double * i_lpdTemps, unsigned int i_uiNum_Temps, const char * i_lpszFilename_Lines, const char * i_lpszFilename_Specta, const char * i_lpszFilename_OpDepth)
{
#define NUM_VALUES 16

	unsigned int	uiLow_Index_Values[128][NUM_VALUES];
	double			dLow_Interpolation_Values[128][NUM_VALUES];
	unsigned int	uiHigh_Index_Values[128][NUM_VALUES];
	double			dHigh_Interpolation_Values[128][NUM_VALUES];
	double			dVelocity_Values_Low[128][NUM_VALUES];
	double			dVelocity_Values_High[128][NUM_VALUES];
//	double			dVelocity_Values_Report[128][16];
	unsigned int	uiRow_Mapping[128][NUM_VALUES];
	unsigned int	uiNum_Values[128];
	bool			bExcess_Values[128];
	unsigned int	uiMax_Values = 0;
	unsigned int	uiIndex_Max = 0;
	double			dVelocity_Max = 0.0;
//	double			dHV_Lines[128];
//	double			dPS_Lines[128];

	XDATASET		cHV_Line_Data;
		unsigned int uiI, uiJ, uiK, uiTemp;

	FILE * fileSpectra = fopen(i_lpszFilename_Specta,"wt");
	if (!fileSpectra)
	{
		printf("Unable to open file %s for writing.\n",i_lpszFilename_Specta);
		return;
	}
	FILE * fileLines = fopen(i_lpszFilename_Lines,"wt");
	if (!fileLines)
	{
		printf("Unable to open file %s for writing.\n",i_lpszFilename_Lines);
		return;
	}
	FILE * fileOpDepth = fopen(i_lpszFilename_OpDepth,"wt");
	if (!fileOpDepth)
	{
		printf("Unable to open file %s for writing.\n",i_lpszFilename_OpDepth);
		return;
	}

	// allocate and zero the line data
	cHV_Line_Data.Allocate(i_uiNum_Temps * 2,128);
	for (uiI = 0; uiI < 128; uiI++)
	{
		for (uiJ = 0; uiJ < 2 + i_uiNum_Temps * 2; uiJ++)
		{
			cHV_Line_Data.SetElement(uiJ,uiI,0.0);
		}
	}
	unsigned int uiNumVelBins = 128;
	double	dVelocity_Bins[128][128];
//	for (uiI = 0; uiI < 128; uiI++)
//		dVelocity_Bins[uiI] = new double[uiNumVelBins];
	for (uiI = 0; uiI < i_cVelocity_Data.GetNumElements(); uiI++)
	{
		if (dVelocity_Max < i_cVelocity_Data.GetElement(1,uiI))
			dVelocity_Max = i_cVelocity_Data.GetElement(1,uiI);
	}
//		double	dDoppler_Width = sqrt(g_XASTRO.k_dKb * dFluid_Temp / g_XASTRO.k_dmh) / 1.0e9; // in 10,000 km/s units
	double dDoppler_Width = dVelocity_Max / 130.0;
	for (uiTemp = 0; uiTemp < i_uiNum_Temps; uiTemp++)
	{
		double dFluid_Temp = i_lpdTemps[uiTemp];

		for (uiI = 0; uiI < 128; uiI++)
		{
			for (uiJ =0; uiJ < uiNumVelBins; uiJ++)
				dVelocity_Bins[uiI][uiJ] = 1.0;
		}
			
		for (uiI = 0; uiI < 128; uiI++)
		{
			double	dTime = 86500.0 * (uiI + 1)  * 0.25;
	//		printf("%.2f,%.17e",(uiI+1) * 0.25,log10(dTRef * dTRef * dTRef / (dTime * dTime)));
	//		Bracket_Value(1.0,i_cAbundance_Data,i_cDensity_Data, i_cLine_Data, i_dFluid_Temp, dTime, i_dTime_Ref,uiLow_Index_Values[uiI],dLow_Interpolation_Values[uiI],16,uiNum_Values[uiI],bExcess_Values[uiI]);
			Bracket_Pairs(1.0,i_cAbundance_Data,i_cDensity_Data, i_cHe3_Data, i_cHyd_Data, i_cLine_Data, dFluid_Temp, dTime, i_dTime_Ref,uiLow_Index_Values[uiI],dLow_Interpolation_Values[uiI],uiHigh_Index_Values[uiI],dHigh_Interpolation_Values[uiI],NUM_VALUES,uiNum_Values[uiI],bExcess_Values[uiI]);
			Velocity_Lookup(i_cVelocity_Data,uiLow_Index_Values[uiI],dLow_Interpolation_Values[uiI],uiNum_Values[uiI],dVelocity_Values_Low[uiI]);
			Velocity_Lookup(i_cVelocity_Data,uiHigh_Index_Values[uiI],dHigh_Interpolation_Values[uiI],uiNum_Values[uiI],dVelocity_Values_High[uiI]);
			for (uiJ = 0; uiJ < uiNum_Values[uiI]; uiJ++)
			{
				// fill 'synthetic spectra' data
				unsigned int uiLow = dVelocity_Values_Low[uiI][uiJ] / dDoppler_Width;
				unsigned int uiHigh = dVelocity_Values_High[uiI][uiJ] / dDoppler_Width + 1;
				if (uiLow > 0)
					dVelocity_Bins[uiI][uiLow - 1] *= 0.5;
				if (uiHigh < (uiNumVelBins - 1))
					dVelocity_Bins[uiI][uiHigh + 1] *= 0.5;
				for (uiK = uiLow; uiK <= uiHigh; uiK++)
				{
					dVelocity_Bins[uiI][uiK] = 0.0;
				}
				// Fill HV line and PS line data
				if (i_cHe3_Data.GetElement(1,uiHigh_Index_Values[uiI][uiJ]) > 1.0e-9) // shell
				{
					if (cHV_Line_Data.GetElement(uiTemp * 2    ,uiI) < dVelocity_Values_High[uiI][uiJ])
						cHV_Line_Data.SetElement(uiTemp * 2    ,uiI,dVelocity_Values_High[uiI][uiJ]);
				}
				if (i_cHe3_Data.GetElement(1,uiHigh_Index_Values[uiI][uiJ]) < 1.0e-9 && i_cHyd_Data.GetElement(1,uiHigh_Index_Values[uiI][uiJ]) < 1.0e-9) // not in shell
				{
					if (cHV_Line_Data.GetElement(uiTemp * 2 + 1,uiI) < dVelocity_Values_High[uiI][uiJ])
						cHV_Line_Data.SetElement(uiTemp * 2 + 1,uiI,dVelocity_Values_High[uiI][uiJ]);
				}
			}
		}

		// output 'synthetic spectra' data

		fprintf(fileSpectra,"-----,-----,-----,-----,%s,-----,-----,-----,-----\n-----,-----,-----,-----,%.0f K,-----,-----,-----,-----\nDay,logTau",lpszLineName,dFluid_Temp);
		for (uiI = 0; uiI < uiNumVelBins; uiI++)
		{
			fprintf(fileSpectra,",%.0f",(uiI + 0.5) * dDoppler_Width * 10000.0);
		}
		fprintf(fileSpectra,"\n");

		for (uiI = 0; uiI < 128; uiI++)
		{
			double	dTime = (uiI + 1)  * 0.25;
			fprintf(fileSpectra,"%.2f,%.17e",dTime,log10(i_dTime_Ref * i_dTime_Ref * i_dTime_Ref / (dTime * dTime)));
			for (uiJ = 0; uiJ < uiNumVelBins; uiJ++)
			{
				fprintf(fileSpectra,",%.2f",dVelocity_Bins[uiI][uiJ]);
			} 
			fprintf(fileSpectra,"\n");
		}
	}
	fclose(fileSpectra);
	fileSpectra = NULL;


	dDoppler_Width = 6.0 / 130.0; // use smaller range for the synthetic spectra
	fprintf(fileOpDepth,"-----,-----,-----,-----,%s,-----,-----,-----,-----\n",lpszLineName);
	for (uiI = 0; uiI < i_uiNum_Temps; uiI++)
	{
		double dFluid_Temp = i_lpdTemps[uiI];
		double dOpDepth[128];
		fprintf(fileOpDepth,"-----,-----,-----,-----,%.0f K,-----,-----,-----,-----\nDay",dFluid_Temp);
		for (uiJ = 0; uiJ < uiNumVelBins; uiJ++)
		{
			fprintf(fileOpDepth,",%.0f",(uiJ + 0.5) * dDoppler_Width * 10000.0);
		}
		fprintf(fileOpDepth,"\n");
		for (uiJ = 0; uiJ < 8; uiJ++)
		{
//			uiJ = 0;
			memset(dOpDepth,0,sizeof(dOpDepth));
			double	dTime = uiJ * 4 + 1;//  * 0.25;
			for (uiK = 0; uiK < i_cVelocity_Data.GetNumElements(); uiK++)
			{
				unsigned int uiBin = i_cVelocity_Data.GetElement(1,uiK) / dDoppler_Width;
//				printf("%.0f,%.0f,%.1e\n",dFluid_Temp,i_cVelocity_Data.GetElement(1,uiK) * 10000.0,Optical_Depth(ComputeDensity(uiK, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, dFluid_Temp, dTime, i_dTime_Ref));
				if (uiBin < 128)
				{
					double dOpDepthLcl = Optical_Depth(ComputeDensity(uiK, i_cAbundance_Data, i_cDensity_Data, i_cHe3_Data ,i_cHyd_Data), i_cLine_Data, dFluid_Temp, dTime, i_dTime_Ref);
					dOpDepth[uiBin] += dOpDepthLcl;
					if (dOpDepthLcl < 0.0)
						printf("Strange value on day %i, vel = %.0f for %s\n",(uiJ + 1) * 4,i_cVelocity_Data.GetElement(1,uiK),lpszLineName);
				}
			}
			fprintf(fileOpDepth,"%.2f",dTime);
			for (uiK = 0; uiK < 128; uiK++)
			{
				double dTransmission = 0.0;
				if (dOpDepth[uiK] < 700.0)
					dTransmission = exp(-dOpDepth[uiK]);
				if (dTransmission > 1.0)
					printf("Strange value on day %i, vel = %.0f for %s\n",(uiJ + 1) * 4,(uiK + 0.5) * dDoppler_Width * 10000.0,lpszLineName);
				fprintf(fileOpDepth,",%.17e",dTransmission);
			}
			fprintf(fileOpDepth,"\n");
		}
	}
	fclose(fileOpDepth);
			


	fprintf(fileLines,"-----,-----,-----,-----,%s,-----,-----,-----,-----\nDay,log Tau",lpszLineName);
	for (uiJ = 0; uiJ < i_uiNum_Temps; uiJ++)
		fprintf(fileLines,",V(HV:%.0f K) [km/s],V(PS:%.0f K) [km/s]",i_lpdTemps[uiJ],i_lpdTemps[uiJ]);
	fprintf(fileLines,"\n");

	for (uiI = 0; uiI < 128; uiI++)
	{
		double	dTime = (uiI + 1)  * 0.25;
		fprintf(fileLines,"%.2f,%.17e",dTime,log10(i_dTime_Ref * i_dTime_Ref * i_dTime_Ref / (dTime * dTime)));
		for (uiJ = 0; uiJ < i_uiNum_Temps; uiJ++)
		{
			if (cHV_Line_Data.GetElement(uiJ * 2   ,uiI) > 0.0)
				fprintf(fileLines,",%.17e",cHV_Line_Data.GetElement(uiJ * 2   ,uiI) * 10000.0);
			else
				fprintf(fileLines,",");
			if (cHV_Line_Data.GetElement(uiJ * 2 + 1,uiI) > 0.0)
				fprintf(fileLines,",%.17e",cHV_Line_Data.GetElement(uiJ * 2 + 1,uiI) * 10000.0);
			else
				fprintf(fileLines,",");
		}
		fprintf(fileLines,"\n");
	}

	fclose(fileLines);

	for (uiI = 0; uiI < 128; uiI++)
	{
		if (bExcess_Values[uiI])
		{
			printf("Warning: Excess Values\n");
			uiI = 128;
		}
	}

//	for (uiI = 0; uiI < 128; uiI++)
//	{
//		delete dVelocity_Bins[uiI];
//	}
#undef NUM_VALUES
}


int main(void)
{
	char lpszLine_Buffer[1024];
	double	dTRef = 0.0;
	double	dTemp = 0.0;
	double	dCaAbdAdj = 0.0;
	double	dSiAbdAdj = 0.0;
	double	dFeAbdAdj = 0.0;
	FILE * fileIn = fopen("params.txt","rt");
	if (fileIn)
	{
		fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn);
		dTRef = atof(lpszLine_Buffer);
		fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn);
		dTemp = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dCaAbdAdj = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dSiAbdAdj = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dFeAbdAdj = atof(lpszLine_Buffer);
		fclose(fileIn);
	}
	else
	{
		printf("Unable to open params.txt\n");
		exit(1);
	}
	if (dTRef < 1.0 || dTemp < 30.0)
	{	
		printf("Invalid parameters.  Time = %.2e, Temp = %.2e\n",dTRef,dTemp);
		exit(1);
	}
                           //  Jlo   Jhi      A         lambda    A   E (eV)
	LINE_DATA	cCaH         ( 0.5 , 1.5 ,   1.466e8  , 3934.777, 40, 6.11316); // Hatano
	LINE_DATA	cCaK         ( 0.5 , 0.5 ,   1.444e8  , 3969.592, 40, 6.11316);
	LINE_DATA	cCaIIIR1     ( 1.5 , 1.5 ,   1.125e6  , 8498.023, 40, 1.692408 + 6.11316);
	LINE_DATA	cCaIIIR2     ( 2.5 , 1.5 ,   9.924e6  , 8542.091, 40, 1.692408 + 6.11316);
	LINE_DATA	cCaIIIR3     ( 1.5 , 0.5 ,   1.058e7  , 8662.141, 40, 1.692408 + 6.11316);
	LINE_DATA	cSiII6355_1  ( 0.5 , 1.5 ,   8.197e7  , 6348.864, 28, 8.121023 + 8.15168);
	LINE_DATA	cSiII6355_2  ( 0.5 , 0.5 ,   8.154e7  , 6373.132, 28, 8.121023 + 8.15168);
	LINE_DATA	cSiII5463    ( 0.5 , 1.5 ,   5.586e6  , 5462.145, 28, 12.877090 + 8.15168);
	LINE_DATA	cSiIII4560_1 ( 1.0 , 2.0 ,   9.759e7  , 4553.898, 28, 19.016329 + 16.34584 + 8.15168); // Hatano
	LINE_DATA	cSiIII4560_2 ( 1.0 , 1.0 ,   9.735e7  , 4569.120, 28, 19.016329 + 16.34584 + 8.15168);
	LINE_DATA	cSiIII4560_3 ( 1.0 , 0.0 ,   9.866e7  , 4576.039, 28, 19.016329 + 16.34584 + 8.15168);
	LINE_DATA	cFeII4925    ( 2.5 , 1.5 ,   3.290e6  , 4925.302, 56, 2.89101812 + 7.9024);
	LINE_DATA	cFeII5018    ( 2.5 , 2.5 ,   2.658e6  , 5019.840, 56, 2.89101812 + 7.9024); // Hatano
	LINE_DATA	cFeII5169    ( 2.5 , 3.5 ,   4.207e6  , 5170.473, 56, 2.89101812 + 7.9024);


	XDATASET	cVelocity;
	XDATASET	cDens;
	XDATASET	cCa;
	XDATASET	cSi;
	XDATASET	cNi;
	XDATASET	cHe3;
	XDATASET	cHyd;
	XDATASET 	cCaHK_Line_Results;
	XDATASET 	cCaHK_Spectra_Results;
	unsigned int uiI;
	double		dTemps[32];// = {1000.0,11,3000.0,5000.0,7000.0,9000.0,11000.0,13000.0};
	unsigned int uiNumTemps = sizeof(dTemps) / sizeof(double);
	for (uiI = 0; uiI < 20; uiI++)
	{
		dTemps[uiI] = 1000.0 + 	100.0 * uiI;
	}
	for (uiI = 20; uiI < 32; uiI++)
	{
		dTemps[uiI] = 3000.0 + (uiI - 19) * 1000.0;
	}


	cVelocity.ReadDataFile("velx.txt", true, false);
	cDens.ReadDataFile("dens.txt", true, false);
	cCa.ReadDataFile("ca40.txt", true, false);
	cSi.ReadDataFile("sig.txt", true, false);
	cNi.ReadDataFile("fe.txt",true,false);
	cHe3.ReadDataFile("he3.txt",true,false);
	cHyd.ReadDataFile("hyd.txt",true,false);


/*	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
		LineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
		LineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
		LineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
		LineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
		LineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
		LineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
		LineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		BroadLineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
		BroadLineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
		BroadLineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
		BroadLineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
		BroadLineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
		BroadLineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
		BroadLineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
		BroadLineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}
*/
//	FILE * file
	BroadLineDataCSV(cVelocity, cCa, cHe3, cHyd, cDens, cCaH + cCaK, dTRef, "Ca H&K", dTemps, uiNumTemps, "CaHKLines.csv", "CaHKSpectra.csv", "CaHKtrx.csv");
	BroadLineDataCSV(cVelocity,cCa,cHe3,cHyd,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTRef,"Ca IR Triplet",dTemps,uiNumTemps,"CaIRLines.csv","CaIRSpectra.csv","CaIRtrx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiII6355_1 + cSiII6355_2,dTRef,"Si II 6355",dTemps,uiNumTemps,"Si6355Lines.csv","Si6355Spectra.csv","Si6355trx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiII5463,dTRef,"Si II 5463",dTemps,uiNumTemps,"Si5463Lines.csv","Si5463Spectra.csv","Si5463trx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,dTRef,"Si III 4560",dTemps,uiNumTemps,"Si4560Lines.csv","Si4560Spectra.csv","Si4560trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII4925,dTRef,"Fe II 4925",dTemps,uiNumTemps,"Fe4925Lines.csv","Fe4925Spectra.csv","Fe4925trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII5018,dTRef,"Fe II 5018",dTemps,uiNumTemps,"Fe5018Lines.csv","Fe5018Spectra.csv","Fe5018trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII5169,dTRef,"Fe II 5169",dTemps,uiNumTemps,"Fe5169Lines.csv","Fe5169Spectra.csv","Fe5169trx.csv");

}
