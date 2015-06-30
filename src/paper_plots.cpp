
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
		if (g_cEjecta.GetNumElements() == 0)
		{
			fprintf(stderr,"Failed to read ejecta dataset for photosphere.  Exiting.\n");
			exit(1);
		}
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


void GenerateSpectrum(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const double &i_dDay, const double & i_dPS_Vel, const double &i_dPS_Temp, const double & i_dPS_Ion_Temp, const double &i_dPS_Log_Tau, const double & i_dHVF_Ion_Temp, const double &i_dHVF_Log_Tau, ES::Spectrum &o_cOutput)
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
//	if (!Check_Spectra_Cache(cResult,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cOutput))
//	{
		Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
//		Save_Spectra_Cache(cResult,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cOutput);
//	}
	o_cOutput = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
}

char g_lpszSource_Dir[128];
unsigned int g_uiSource_File_ID;

typedef	void fGet_Parameters(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp);

void	Convolute_PS(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp)
{
	o_dPS_Vel = Determine_Photosphere(g_lpszSource_Dir, g_uiSource_File_ID, i_uiIdx * 1.0 + 7.0,25.0, i_dDay);
	o_dPS_Temp = 12.0;
	o_dPS_Ion_Scale = 1.25;
	o_dPS_Ion_Temp = 12.0;
	o_dHVF_Ion_Scale = 1.25;
	o_dHVF_Ion_Temp = 12.0;
}
void	Convolute_PS_Ion_Scale(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp)
{
	o_dPS_Vel = 10.0;
	o_dPS_Temp = 12.0;
	o_dPS_Ion_Scale = -0.25 + i_uiIdx * 0.5;
	o_dPS_Ion_Temp = 12.0;
	o_dHVF_Ion_Scale = 1.25;
	o_dHVF_Ion_Temp = 12.0;
}
void	Convolute_PS_Temp(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp)
{
	o_dPS_Vel = 10.0;
	o_dPS_Temp = 12.0;
	o_dPS_Ion_Scale = 1.25;
	o_dPS_Ion_Temp = 8.0 + i_uiIdx;
	o_dHVF_Ion_Scale = 1.25;
	o_dHVF_Ion_Temp = 12.0;
}
void	Convolute_HVF_Ion_Scale(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp)
{
	o_dPS_Vel = 10.0;
	o_dPS_Temp = 12.0;
	o_dPS_Ion_Scale = 1.25;
	o_dPS_Ion_Temp = 12.0;
	o_dHVF_Ion_Scale = -0.25 + i_uiIdx * 0.5;
	o_dHVF_Ion_Temp = 12.0;
}
void	Convolute_HVF_Temp(unsigned int i_uiIdx, const double & i_dDay, double & o_dPS_Vel, double	&o_dPS_Temp,
		double	&o_dPS_Ion_Scale, double	&o_dPS_Ion_Temp, double	&o_dHVF_Ion_Scale, double	&o_dHVF_Ion_Temp)
{
	o_dPS_Vel = 10.0;
	o_dPS_Temp = 12.0;
	o_dPS_Ion_Scale = 1.25;
	o_dPS_Ion_Temp = 12.0;
	o_dHVF_Ion_Scale = 1.25;
	o_dHVF_Ion_Temp = 8.0 + i_uiIdx;
}

void Process(fGet_Parameters * Get_Parameters,
		const XDATASET &cOpacity_Map_Shell,
		const XDATASET &cOpacity_Map_Ejecta,
		unsigned int uiDays_Count,
		double * lpdDays,
		const ES::Spectrum &cTarget,
		unsigned int uiIon,
		const char * lpszGraphType,
		const char * lpszOutput_File_Prefix,
		bool bDebug
	)
{
	epsplot::DATA cPlot;

    ES::Spectrum * cSpectra[8];
	char lpszOutput_Filename[256];
	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	// photosphere variation
	printf("type (num): day Vps Tps Cpsf Tpsf Chvf Thvf\n");
	for (unsigned int uiJ = 0; uiJ < 8; uiJ++)
	{
		cSpectra[uiJ] = new ES::Spectrum[uiDays_Count];
		if (!bDebug || uiJ == 0)//@@DEBUG
		{//@@DEBUG
			for (unsigned int uiI = 0; uiI < uiDays_Count; uiI++)
			{
				cSpectra[uiJ][uiI] = cTarget;
				double	dPS_Vel;
				double	dPS_Temp;
				double	dPS_Ion_Scale;
				double	dPS_Ion_Temp;
				double	dHVF_Ion_Scale;
				double	dHVF_Ion_Temp;
				Get_Parameters(uiJ,lpdDays[uiI],dPS_Vel,dPS_Temp,dPS_Ion_Scale,dPS_Ion_Temp,dHVF_Ion_Scale,dHVF_Ion_Temp);
				printf("%s (%i): %.1f %.2f %.2f %.2f %.2f %.2f %.2f\n",lpszGraphType,uiJ,lpdDays[uiI],dPS_Vel,dPS_Temp,dPS_Ion_Scale,dPS_Ion_Temp,dHVF_Ion_Scale,dHVF_Ion_Temp);
				GenerateSpectrum(cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, lpdDays[uiI], dPS_Vel, dPS_Temp, dPS_Ion_Temp, dPS_Ion_Scale, dHVF_Ion_Temp, dHVF_Ion_Scale, cSpectra[uiJ][uiI]);
			}
		} //@@DEBUG
	}
	if (bDebug)
	{	
		for (unsigned int uiI = 0; uiI < uiDays_Count; uiI++)//@@DEBUG
		{
			cSpectra[1][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[2][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[3][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[4][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[5][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[6][uiI] = cSpectra[0][uiI];//@@DEBUG
			cSpectra[7][uiI] = cSpectra[0][uiI];//@@DEBUG
		}
	}
	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;

	double	*lpdEquivalent_Width[8];
	double	*lpdMinima_WL[8];
	for (unsigned int uiI = 0; uiI < 8; uiI++)
	{
		lpdEquivalent_Width[uiI] = new double[uiDays_Count];
		lpdMinima_WL[uiI] = new double[uiDays_Count];
		Find_Minimum(cSpectra[uiI], uiDays_Count, lpdMinima_WL[uiI], NULL, 7400.0, 8300.0);
		for (unsigned int uiJ = 0; uiJ < uiDays_Count; uiJ++)
		{
			double	dWL_Min = 7600.0, dWL_Max = 8600.0;
			lpdMinima_WL[uiI][uiJ] = (1.0 - lpdMinima_WL[uiI][uiJ] / 8579.0) * 300.0; // convert to velocity [kkm/s]
			lpdEquivalent_Width[uiI][uiJ] = Equivalent_Width(cSpectra[uiI][uiJ], dWL_Min, dWL_Max, 4);
			if (isnan(lpdEquivalent_Width[uiI][uiJ]))
				printf("nan at %i, %.1f\n",uiI,lpdDays[uiJ]);
		}
	}
	


	sprintf(lpszOutput_Filename,"%s.%s.paper.%s.%i.full.eps",lpszOutput_File_Prefix,g_lpszSourceDir, lpszGraphType,uiIon);
	cPlot.Set_Plot_Filename(lpszOutput_Filename);
	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Time [days]", false, false, false, 0.0, false, 0.0);
	unsigned int uiY_Axis_Vel = cPlot.Set_Y_Axis_Parameters( "Velocity [kkm/s]", false, false, true, 5.0, true, 60.0);
	unsigned int uiY_Axis_pEW = cPlot.Set_Y_Axis_Parameters( "Equivalent Width [Angstroms]", false, false, true, 0.0, true, 1000.0);

//	cPlot.Set_Num_Plots(16);
	for (unsigned int uiI = 0; uiI < 8; uiI++)
	{
		epsplot::STIPPLE eStipple;
		switch (uiI)
		{
		case 0: eStipple = epsplot::LONG_DASH; break;
		case 1: eStipple = epsplot::LONG_DASH; break;
		case 2: eStipple = epsplot::LONG_SHORT_DASH; break;
		case 3: eStipple = epsplot::SOLID; break;
		case 4: eStipple = epsplot::DOTTED; break;
		case 5: eStipple = epsplot::SHORT_DASH_DOTTED; break;
		case 6: eStipple = epsplot::LONG_DASH_DOTTED; break;
		case 7: eStipple = epsplot::LONG_SHORT_DASH_DOTTED; break;
		}

		cPlot.Set_Plot_Data(lpdDays, lpdMinima_WL[uiI], uiDays_Count, epsplot::RED, eStipple, uiX_Axis, uiY_Axis_Vel, -1.0);
	}
	for (unsigned int uiI = 0; uiI < 8; uiI++)
	{
		epsplot::STIPPLE eStipple;
		switch (uiI)
		{
		case 0: eStipple = epsplot::LONG_DASH; break;
		case 1: eStipple = epsplot::LONG_DASH; break;
		case 2: eStipple = epsplot::LONG_SHORT_DASH; break;
		case 3: eStipple = epsplot::SOLID; break;
		case 4: eStipple = epsplot::DOTTED; break;
		case 5: eStipple = epsplot::SHORT_DASH_DOTTED; break;
		case 6: eStipple = epsplot::LONG_DASH_DOTTED; break;
		case 7: eStipple = epsplot::LONG_SHORT_DASH_DOTTED; break;
		}

		cPlot.Set_Plot_Data(lpdDays, lpdEquivalent_Width[uiI], uiDays_Count, epsplot::BLUE, eStipple, uiX_Axis, uiY_Axis_pEW, -1.0);
	}
	cPlot.Plot(cPlot_Parameters);


/*	cData.m_bLog_Flux = true; // if true, y axis will be log flux; if false, y axis will be flux
	cData.m_lpszTitle = NULL;
	cData.m_lpszFilename = lpszOutput_Filename;
	cData.m_lpcSpectra = cSpectra;
	cData.m_uiNum_Days = uiDays_Count;
	cData.m_uiNum_Plots = 8;
	cData.m_lpdDays = lpdDays;
	cData.m_dNormalization_WL = 7400.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
//	cData.m_dVelocity_Reference_WL = -1.0; // if > 0, plot will be in velocity instead of wavelength space
//	printf("First a");
	Plot_EPS_Grid(cPlot, cData);
	cData.m_dMin_WL = 7000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dMax_WL = 9000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dNormalization_WL = 7400.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dVelocity_Reference_WL = 8579.0; // if > 0, plot will be in velocity instead of wavelength space
	cData.m_bPlot_Minima_Curve = true;
	cData.m_dMin_WL_For_Minima = 7400.0;
	cData.m_dMax_WL_For_Minima = 8300.0;
	cData.m_bPlot_EW_Curve = true;
	cData.m_dMin_WL_For_EW = 7600.0;
	cData.m_dMax_WL_For_EW = 8600.0;

	sprintf(lpszOutput_Filename,"%s.%s.paper.%s.%i.CaNIR.eps",lpszOutput_File_Prefix,g_lpszSourceDir, lpszGraphType,uiIon);
	Plot_EPS_Grid(cPlot, cData);
	sprintf(lpszOutput_Filename,"%s.%s.paper.%s.%i.CaHK.eps",lpszOutput_File_Prefix,g_lpszSourceDir, lpszGraphType,uiIon);
	cData.m_dMin_WL = 3000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dMax_WL = 5000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dNormalization_WL = 3200.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dVelocity_Reference_WL = 3945.0; // if > 0, plot will be in velocity instead of wavelength space
	cData.m_bPlot_Minima_Curve = true;
	cData.m_dMin_WL_For_Minima = 3400.0;
	cData.m_dMax_WL_For_Minima = 4300.0;
	cData.m_bPlot_EW_Curve = true;
	cData.m_dMin_WL_For_EW = 3000.0;
	cData.m_dMax_WL_For_EW = 5000.0;
	Plot_EPS_Grid(cPlot, cData);

	sprintf(lpszOutput_Filename,"%s.%s.paper.%s.%i.Si6355.eps",lpszOutput_File_Prefix,g_lpszSourceDir, lpszGraphType,uiIon);
	cData.m_dMin_WL = 5000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dMax_WL = 7000.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dNormalization_WL = 5200.0; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	cData.m_dVelocity_Reference_WL = 6355.0; // if > 0, plot will be in velocity instead of wavelength space
	cData.m_bPlot_Minima_Curve = true;
	cData.m_dMin_WL_For_Minima = 5400.0;
	cData.m_dMax_WL_For_Minima = 6300.0;
	cData.m_bPlot_EW_Curve = true;
	cData.m_dMin_WL_For_EW = 5000.0;
	cData.m_dMax_WL_For_EW = 7000.0;
	Plot_EPS_Grid(cPlot, cData);
*/
	for (unsigned int uiJ = 0; uiJ < 8; uiJ++)
	{
		delete [] cSpectra[uiJ];
		delete [] lpdEquivalent_Width[uiJ];
		delete [] lpdMinima_WL[uiJ];
	}
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	bool bData_valid;
	char lpszTargetFile[256] = {""};
	char lpszOutput_File_Prefix[256] = {""};
	char	lpszGraphType[32];
	char lpszTitle[256];
	unsigned int uiIon = 2001;
	double			lpdDays[25] = {1,3,5,7,9,11,13,15,17,19,21,23,25};
	unsigned int	uiDays_Count = 13;	
	bool		bDebug;

	double dNan = nan("");
	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-file"))
		g_uiSource_File_ID = xParse_Command_Line_UInt(i_iArg_Count,i_lpszArg_Values,"--PS-vel-ref-file",-1);
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--reffile"))
		g_uiSource_File_ID = xParse_Command_Line_UInt(i_iArg_Count,i_lpszArg_Values,"--reffile",-1);
	else
	{
		fprintf(stderr,"No reference file # specified.\n");
		exit(1);
	}
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--sourcedir",g_lpszSource_Dir,256,NULL);
	if (g_lpszSource_Dir == NULL || g_lpszSource_Dir[0] == 0)
	{
		fprintf(stderr,"--sourcedir not specified.\n");
		exit(1);
	}
	bDebug = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--debug");
	if (bDebug)
		printf("\e[0;31mDebug mode enabled.\e[0m\n");
	
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


    ES::Spectrum cTarget;
	cTarget = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 5.0);

	char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64], lpszParameters_File[64];
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.%s.xdataset",g_lpszSource_Dir,lpszElement_ID);
	sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",g_lpszSource_Dir,lpszElement_ID);
	sprintf(lpszOutput_File_Prefix,"%s/series",g_lpszSource_Dir);

	cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
	cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);


	Process(Convolute_PS,cOpacity_Map_Shell,cOpacity_Map_Ejecta,uiDays_Count,lpdDays,cTarget,uiIon,"PS",lpszOutput_File_Prefix,bDebug);
	if (!bDebug)
	{
		Process(Convolute_PS_Ion_Scale,cOpacity_Map_Shell,cOpacity_Map_Ejecta,uiDays_Count,lpdDays,cTarget,uiIon,"PS_Scale",lpszOutput_File_Prefix,bDebug);
		Process(Convolute_PS_Temp,cOpacity_Map_Shell,cOpacity_Map_Ejecta,uiDays_Count,lpdDays,cTarget,uiIon,"PS_Temp",lpszOutput_File_Prefix,bDebug);
		Process(Convolute_HVF_Ion_Scale,cOpacity_Map_Shell,cOpacity_Map_Ejecta,uiDays_Count,lpdDays,cTarget,uiIon,"HVF_Scale",lpszOutput_File_Prefix,bDebug);
		Process(Convolute_HVF_Temp,cOpacity_Map_Shell,cOpacity_Map_Ejecta,uiDays_Count,lpdDays,cTarget,uiIon,"HVF_Temp",lpszOutput_File_Prefix,bDebug);
	}
	return 0;
}

