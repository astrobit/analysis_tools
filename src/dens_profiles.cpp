#include <xio.h>
#include <string.h>
#include <math.h>
#include "ES_Synow.hh"

#include <ostream>
#include <fstream>

//#include <plplot/plplot.h>
//#include <plplot/plstream.h>
#include <Plot_Utilities.h>
#include <sys/time.h>
#include <xflash.h>
#include <opacity_profile_data.h>

double	g_dTime_Ref = 0.0;

template <typename T> T** new2d(unsigned int i_uiA, unsigned int i_uiB)
{
	T** tRet;

	tRet = new T*[i_uiA];
	for (unsigned int uiI = 0; uiI < i_uiA; uiI++)
		tRet[uiI] = new T[i_uiB];

	return tRet;
}

template <typename T> void delete2d(T** i_tT, unsigned int i_uiA)
{
	for (unsigned int uiI = 0; uiI < i_uiA; uiI++)
		delete [] i_tT[uiI];
	delete [] i_tT;
}


class FIT_RESULTS
{
public:
	double	dVmin;
	double	dVmax;
	double	dNr;
	double	dNv;
	double	dRc;
	double	dVc;
	double	dVariance_Error_Power_R;
	double	dVariance_Error_Power_V;
	double	dVariance_Error_Exp;
	double	dMean_Error_Power_R;
	double	dMean_Error_Power_V;
	double	dMean_Error_Exp;
	double	dStdDev_Error_Power_R;
	double	dStdDev_Error_Power_V;
	double	dStdDev_Error_Exp;
	double	dDensity_115_hkmps; // density at v=11.5 kkm/s
	double	dDensity_175_hkmps; // density at v = 17.5 kkm/s

	FIT_RESULTS(void)
	{
		dVmin =
		dVmax = 
		dNr = 
		dNv = 
		dRc = 
		dVc = 
		dVariance_Error_Power_R = 
		dVariance_Error_Power_V = 
		dVariance_Error_Exp = 
		dMean_Error_Power_R = 
		dMean_Error_Power_V = 
		dMean_Error_Exp = 
		dStdDev_Error_Power_R = 
		dStdDev_Error_Power_V = 
		dStdDev_Error_Exp = 
		dDensity_115_hkmps = 
		dDensity_175_hkmps = 0.0;
	}
};

FIT_RESULTS ProcessFile(FILE * io_fileOut,const char * i_lpszPrefix, unsigned int i_uiNumber, const char * i_lpszTitle, XDATASET &o_cData)
{
	FIT_RESULTS cResults;
	unsigned int uiI, uiJ;
	char lpszFilename[32];
	char	lpszGraph_Title[64];
	unsigned int uiNum_Elements, uiIdx1,uiIdx2;
	double	dRadius;
	double	dRefRho, dRefV, dRefR;
	double	dVel,dDens,dPower_R,dPower_V,dExp;
	double	dXmin,dXmax,dXref;
	FILE * fileOut;

	int lpiOptions_Array[4];
	int lpiLegend_Text_Colors[4];
	const char *lplpszLegend_Test[4];
	int lpiLegend_Line_Colors[4];
	int lpiLegend_Line_Styles[4];
	double lpfLegend_ine_Widths[4];
	double fLgnd_Width,fLgnd_Height;
	double *lpfRadius,*lpfDensity,*lpfDensity_Exp,*lpfDensity_Power_R,*lpfDensity_Power_V;
	double	dDensMax = 0.0,dDensMin = 1.0e300;

	sprintf(lpszFilename,"%s%04i.xdataset",i_lpszPrefix,i_uiNumber);
	o_cData.ReadDataFileBin(lpszFilename,true);
	uiNum_Elements = o_cData.GetNumElements();
	unsigned int uiRadius_Idx = 0;
	unsigned int uiDensity_Idx = 8;
	unsigned int uiVelocity_Idx = 25;

	if (uiNum_Elements != 0)
	{
		lpfRadius = new double[uiNum_Elements];
		lpfDensity = new double[uiNum_Elements];
		lpfDensity_Exp = new double[uiNum_Elements];
		lpfDensity_Power_R = new double[uiNum_Elements];
		lpfDensity_Power_V = new double[uiNum_Elements];


		// find point 75% from inner edge
		dXmax = o_cData.GetElement(uiRadius_Idx,0);
		dXmin = o_cData.GetElement(uiRadius_Idx,uiNum_Elements - 1);
		dXref = (dXmax - dXmin) * 0.75 + dXmin;

		uiJ = 0;
		while (o_cData.GetElement(uiRadius_Idx,uiJ) < dXref)
			uiJ++;

		uiIdx1 = uiJ;

		// find point 25% from outer edge
		dXref = (dXmax - dXmin) * 0.25 + dXmin;

		uiJ = 0;
		while (o_cData.GetElement(uiRadius_Idx,uiJ) < dXref)
			uiJ++;

		uiIdx2 = uiJ;
		double dVel_[2] = {o_cData.GetElement(uiVelocity_Idx,uiIdx1),o_cData.GetElement(uiVelocity_Idx,uiIdx2)};
		double dDens_[2] = {o_cData.GetElement(uiDensity_Idx,uiIdx1),o_cData.GetElement(uiDensity_Idx,uiIdx2)};
		double dRad_[2] = {o_cData.GetElement(uiRadius_Idx,uiIdx1),o_cData.GetElement(uiRadius_Idx,uiIdx2)};

		cResults.dNr = log(dDens_[0] / dDens_[1]) / log(dRad_[0] / dRad_[1]);
		cResults.dNv = log(dDens_[0] / dDens_[1]) / log(dVel_[0] / dVel_[1]);
		cResults.dRc = (dRad_[1] - dRad_[0]) / log(dDens_[0] / dDens_[1]);
		cResults.dVc = (dVel_[1] - dVel_[0]) / log(dDens_[0] / dDens_[1]);

		cResults.dVmin = o_cData.GetElement(uiVelocity_Idx,0);

		sprintf(lpszFilename,"%s%04i_fit.csv",i_lpszPrefix,i_uiNumber);
		fileOut = fopen(lpszFilename,"wt");
	//	dXref = (dXmax - dXmin) * 0.5 + dXmin;
	//	uiJ = 0;
	//	while (o_cData.GetElement(uiRadius_Idx,uiJ) < dXref)
	//		uiJ++;
		// set reference density and velocity to that of the middle
		dRefRho = o_cData.GetElement(uiDensity_Idx,uiIdx1);
		dRefV = o_cData.GetElement(uiVelocity_Idx,uiIdx1);
		dRefR = o_cData.GetElement(uiRadius_Idx,uiIdx1);

		fprintf(fileOut,"r[cm], dens[g/cm^3], vel[cm/s], rho = exp(r/rc), rho = r^n, rho = v^n\n");
		for (uiJ = 0; uiJ < uiNum_Elements; uiJ++)
		{
			dRadius = o_cData.GetElement(uiRadius_Idx,uiJ);
			dVel = o_cData.GetElement(uiVelocity_Idx,uiJ);
			dDens = o_cData.GetElement(uiDensity_Idx,uiJ);
			dPower_R = dRefRho * pow(dRadius / dRefR,cResults.dNr);
			dPower_V = dRefRho * pow(dVel / dRefV,cResults.dNv);
			dExp = dRefRho * exp(-(dRadius - dRefR) / cResults.dRc);

			if (cResults.dDensity_115_hkmps == 0.0 && dVel > 11.5e8 && uiJ > 0)
				cResults.dDensity_115_hkmps = dDens;
			if (cResults.dDensity_175_hkmps == 0.0 && dVel > 17.5e8 && uiJ > 0)
				cResults.dDensity_175_hkmps = dDens;

			lpfRadius[uiJ] = dRadius;
			lpfDensity[uiJ] = log10(dDens);
			lpfDensity_Exp[uiJ] = log10(dExp);
			lpfDensity_Power_R[uiJ] = log10(dPower_R);
			lpfDensity_Power_V[uiJ] = log10(dPower_V);

			if (lpfDensity[uiJ] > dDensMax)
				dDensMax = lpfDensity[uiJ];
			if (lpfDensity_Exp[uiJ] > dDensMax)
				dDensMax = lpfDensity_Exp[uiJ];
			if (lpfDensity_Power_R[uiJ] > dDensMax)
				dDensMax = lpfDensity_Power_R[uiJ];
			if (lpfDensity_Power_V[uiJ] > dDensMax)
				dDensMax = lpfDensity_Power_V[uiJ];
			if (lpfDensity[uiJ] < dDensMin)
				dDensMin = lpfDensity[uiJ];
			if (lpfDensity_Exp[uiJ] < dDensMin)
				dDensMin = lpfDensity_Exp[uiJ];
			if (lpfDensity_Power_R[uiJ] < dDensMin)
				dDensMin = lpfDensity_Power_R[uiJ];
			if (lpfDensity_Power_V[uiJ] < dDensMin)
				dDensMin = lpfDensity_Power_V[uiJ];

			if (dVel > cResults.dVmax)
				cResults.dVmax = dVel;

			fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",dRadius,dDens,dVel,dExp,dPower_R,dPower_V);

			if (uiJ >= uiIdx1 && uiJ <= uiIdx2)
			{
				cResults.dMean_Error_Power_R += dDens - dPower_R;
				cResults.dMean_Error_Power_V += dDens - dPower_V;
				cResults.dMean_Error_Exp += dDens - dExp;
				cResults.dVariance_Error_Power_R += pow(dDens - dPower_R,2.0);
				cResults.dVariance_Error_Power_V += pow(dDens - dPower_V,2.0);
				cResults.dVariance_Error_Exp += pow(dDens - dExp,2.0);
			}
		}
		fclose(fileOut);
		cResults.dMean_Error_Power_R /= (uiIdx2 - uiIdx1);
		cResults.dMean_Error_Power_V /= (uiIdx2 - uiIdx1);
		cResults.dMean_Error_Exp /= (uiIdx2 - uiIdx1);
		cResults.dVariance_Error_Power_R /= (uiIdx2 - uiIdx1);
		cResults.dVariance_Error_Power_V /= (uiIdx2 - uiIdx1);
		cResults.dVariance_Error_Exp /= (uiIdx2 - uiIdx1);

		printf("%04i, %s, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n", i_uiNumber, i_lpszPrefix, cResults.dVmin, cResults.dVmax, cResults.dRc, cResults.dVc, cResults.dNr, cResults.dNv, cResults.dVariance_Error_Exp, cResults.dVariance_Error_Power_R, cResults.dVariance_Error_Power_V,cResults.dDensity_115_hkmps,cResults.dDensity_175_hkmps);
		fprintf(io_fileOut,"%04i, %s, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n", i_uiNumber, i_lpszPrefix, cResults.dVmin, cResults.dVmax, cResults.dRc, cResults.dVc, cResults.dNr, cResults.dNv, cResults.dVariance_Error_Exp, cResults.dVariance_Error_Power_R, cResults.dVariance_Error_Power_V,cResults.dDensity_115_hkmps,cResults.dDensity_175_hkmps);



	
		sprintf(lpszFilename,"density_%s_%04i.eps",i_lpszPrefix,i_uiNumber);

		epsplot::PAGE_PARAMETERS	cPlot_Parameters;
		epsplot::DATA cPlot;
		epsplot::LINE_PARAMETERS	cLine_Parameters;

		cPlot_Parameters.m_uiNum_Columns = 1;
		cPlot_Parameters.m_uiNum_Rows = 1;
		cPlot_Parameters.m_dWidth_Inches = 11.0;
		cPlot_Parameters.m_dHeight_Inches = 8.5;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Radius [cm]", false, false, true, lpfRadius[0], true, lpfRadius[uiNum_Elements - 1]);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "log Density [g/cm#u3#d]", false, false, true, dDensMin, true, dDensMax);

		sprintf(lpszGraph_Title,"%s : %s",i_lpszTitle,i_lpszPrefix);
		cPlot.Set_Plot_Title(lpszGraph_Title,18.0);
		cPlot.Set_Plot_Filename(lpszFilename);


		cLine_Parameters.m_eColor = epsplot::RED;
		cLine_Parameters.m_eStipple = epsplot::SHORT_DASH;
		cPlot.Set_Plot_Data(lpfRadius, lpfDensity, uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
		cLine_Parameters.m_eColor = epsplot::BLUE;
		cLine_Parameters.m_eStipple = epsplot::LONG_DASH;
		cPlot.Set_Plot_Data(lpfRadius, lpfDensity_Power_R, uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
		cLine_Parameters.m_eStipple = epsplot::DOTTED;
		cPlot.Set_Plot_Data(lpfRadius, lpfDensity_Power_V, uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
		cLine_Parameters.m_eColor = epsplot::BLACK;
		cLine_Parameters.m_eStipple = epsplot::LONG_SHORT_DASH;
		cPlot.Set_Plot_Data(lpfRadius, lpfDensity_Exp, uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
/*
		//@@TODO: Legend

		lpiOptions_Array[3]   = PL_LEGEND_LINE;
		lpiLegend_Text_Colors[3] = 9;
		lplpszLegend_Test[3]        = "Power (v)";
		lpiLegend_Line_Colors[3] = 9;
		lpiLegend_Line_Styles[3] = 5;
		lpfLegend_ine_Widths[3] = 1.0;

		lpiOptions_Array[2]   = PL_LEGEND_LINE;
		lpiLegend_Text_Colors[2] = 9;
		lplpszLegend_Test[2]        = "Power (r)";
		lpiLegend_Line_Colors[2] = 9;
		lpiLegend_Line_Styles[2] = 3;
		lpfLegend_ine_Widths[2] = 1.0;

		lpiOptions_Array[1]   = PL_LEGEND_LINE;
		lpiLegend_Text_Colors[1] = 15;
		lplpszLegend_Test[1]        = "Exp";
		lpiLegend_Line_Colors[1] = 15;
		lpiLegend_Line_Styles[1] = 4;
		lpfLegend_ine_Widths[1] = 1.0;

		lpiOptions_Array[0]   = PL_LEGEND_LINE;
		lpiLegend_Text_Colors[0] = 1;
		lplpszLegend_Test[0]        = "Density";
		lpiLegend_Line_Colors[0] = 1;
		lpiLegend_Line_Styles[0] = 2;
		lpfLegend_ine_Widths[0] = 1.0;

		lpcPLStream->legend( &fLgnd_Width,&fLgnd_Height,
			PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX, 0,
			0.0, 0.0, 0.1, 
			0, 15, 1, 0, 0,
			4, lpiOptions_Array,
			1.0, 1.0, 2.0,
			0., lpiLegend_Text_Colors, (const char * const *) lplpszLegend_Test,
			NULL, NULL, NULL, NULL,
			lpiLegend_Line_Colors, lpiLegend_Line_Styles, lpfLegend_ine_Widths,
			NULL, NULL, NULL, NULL );
*/

		delete [] lpfRadius;
		delete [] lpfDensity;
		delete [] lpfDensity_Exp;
		delete [] lpfDensity_Power_R;
		delete [] lpfDensity_Power_V;
	}
	return cResults;
}

enum PROFILETYPE {POWERLAW_R,POWERLAW_V,EXPONENTIAL};
double Conditional_Output_Velocity(FILE * io_fileOut, const FIT_RESULTS & i_cResults, const double & i_dT_25, const double &i_dV_Ref, PROFILETYPE i_eProfile_Type)
{
	double dResult;
	switch (i_eProfile_Type)
	{
	case POWERLAW_R:
		dResult = i_dV_Ref * pow(i_dT_25,2.0 / i_cResults.dNr);
		break;
	case POWERLAW_V:
		dResult = i_dV_Ref * pow(i_dT_25,2.0 / i_cResults.dNv);
		break;
	case EXPONENTIAL:
		dResult = i_dV_Ref - 2.0 * i_cResults.dVc * log(i_dT_25);
		break;
	}
	if (dResult < i_cResults.dVmin || dResult > i_cResults.dVmax)
		dResult = -1.0;
	else
		dResult *= 1e-8;

	if (dResult != -1.0)
		fprintf(io_fileOut,", %.17e",dResult);
	else
		fprintf(io_fileOut,", ");
	return dResult;
}

void Check_Indices(const float & i_fValue, unsigned int i_uiI, unsigned int *io_lpuiBracket_Idx)
{
//	printf("%.1f\t%i\t%i\t%i\n",i_fValue,i_uiI,io_lpuiBracket_Idx[0],io_lpuiBracket_Idx[1]);
	if (i_fValue != -1.0 && io_lpuiBracket_Idx[0] == -1)
		io_lpuiBracket_Idx[0] = i_uiI;
	if (i_fValue == -1.0 && io_lpuiBracket_Idx[0] != -1 && io_lpuiBracket_Idx[1] == -1)
		io_lpuiBracket_Idx[1] = i_uiI - 1;
//	printf("%.1f\t%i\t%i\t%i\n",i_fValue,i_uiI,io_lpuiBracket_Idx[0],io_lpuiBracket_Idx[1]);
}

void Velocity_Evolution_Theoretical(const FIT_RESULTS &i_cEjecta, const FIT_RESULTS &i_cShell, unsigned int i_uiNumber, const char * i_lpszTitle)
{
	FILE * fileOut;
	char lpszFilename[32];
	double dT,dT_25;
	unsigned int uiT;
	double dConst;

	int lpiOptions_Array[4];
	int lpiLegend_Text_Colors[4];
	const char *lplpszLegend_Test[4];
	int lpiLegend_Line_Colors[4];
	int lpiLegend_Line_Styles[4];
	double lpfLegend_ine_Widths[4];
	double fLgnd_Width,fLgnd_Height;
	double lpfDay[25], lpfEjecta_Exp[25], lpfEjecta_Power_V[25], lpfEjecta_Power_R[25], lpfShell_Exp[25], lpfShell_Power_R[25], lpfShell_Power_V[25], lpfObs_Const_2[25], lpfObs_Const_3[25], lpfShell_Power_PS[25];
	unsigned int uiEjecta_Exp_Idx[2] = {-1,-1}, uiEjecta_Power_R_Idx[2] = {-1,-1}, uiEjecta_Power_V_Idx[2] = {-1,-1}, uiShell_Exp_Idx[2] = {-1,-1}, uiShell_Power_R_Idx[2] = {-1,-1}, uiShell_Power_V_Idx[2] = {-1,-1}, uiShell_Power_PS_Idx[2] = {-1,-1};


	sprintf(lpszFilename,"velocity_evolution_theoretical_%04i.csv",i_uiNumber);
	fileOut = fopen(lpszFilename,"wt");
	if (i_cShell.dVmin != 0.0) // no shell
		fprintf(fileOut,"t,PS (exp), PS (power r), PS (power v), HVF (exp), HVF(power r), HVF(power v)\n");
	else
		fprintf(fileOut,"t,PS (exp), PS (power r), PS (power v)\n");
	float fVMax = 0.0;
	for (uiT = 0; uiT < 25; uiT++)
	{
		dT = uiT + 1.0;
		dT_25 = dT / 25.0;

		fprintf(fileOut,"%.0f", dT);

		lpfDay[uiT] = dT;
		lpfEjecta_Exp[uiT] = Conditional_Output_Velocity(fileOut,i_cEjecta,dT_25,11.5e8,EXPONENTIAL);
		Check_Indices(lpfEjecta_Exp[uiT],uiT,uiEjecta_Exp_Idx);
		
		lpfEjecta_Power_R[uiT] = Conditional_Output_Velocity(fileOut,i_cEjecta,dT_25,11.5e8,POWERLAW_R);
		Check_Indices(lpfEjecta_Power_R[uiT],uiT,uiEjecta_Power_R_Idx);

		lpfEjecta_Power_V[uiT] = Conditional_Output_Velocity(fileOut,i_cEjecta,dT_25,11.5e8,POWERLAW_V);
		Check_Indices(lpfEjecta_Power_V[uiT],uiT,uiEjecta_Power_V_Idx);
		if (i_cShell.dVmin != 0.0) // no shell
		{
			lpfShell_Exp[uiT] = Conditional_Output_Velocity(fileOut,i_cShell,dT_25,17.5e8,EXPONENTIAL);
			Check_Indices(lpfShell_Exp[uiT],uiT,uiShell_Exp_Idx);

			lpfShell_Power_R[uiT] = Conditional_Output_Velocity(fileOut,i_cShell,dT_25,17.5e8,POWERLAW_R);
			Check_Indices(lpfShell_Power_R[uiT],uiT,uiShell_Power_R_Idx);

			lpfShell_Power_V[uiT] = Conditional_Output_Velocity(fileOut,i_cShell,dT_25,17.5e8,POWERLAW_V);
			Check_Indices(lpfShell_Power_V[uiT],uiT,uiShell_Power_V_Idx);

			lpfObs_Const_2[uiT] = 15.0 * pow(dT_25,-0.2);
			lpfObs_Const_3[uiT] = 20.0 * pow(dT_25,-0.3);
		}

		fprintf(fileOut,"\n");
	}
	if (uiEjecta_Exp_Idx[0] == -1)
		uiEjecta_Exp_Idx[0] = 24;
	if (uiEjecta_Power_R_Idx[0] == -1)
		uiEjecta_Power_R_Idx[0] = 24;
	if (uiEjecta_Power_V_Idx[0] == -1)
		uiEjecta_Power_V_Idx[0] = 24;
	if (uiShell_Exp_Idx[0] == -1)
		uiShell_Exp_Idx[0] = 24;
	if (uiShell_Power_R_Idx[0] == -1)
		uiShell_Power_R_Idx[0] = 24;
	if (uiShell_Power_V_Idx[0] == -1)
		uiShell_Power_V_Idx[0] = 24;

	if (uiEjecta_Exp_Idx[1] == -1)
		uiEjecta_Exp_Idx[1] = 24;
	if (uiEjecta_Power_R_Idx[1] == -1)
		uiEjecta_Power_R_Idx[1] = 24;
	if (uiEjecta_Power_V_Idx[1] == -1)
		uiEjecta_Power_V_Idx[1] = 24;
	if (uiShell_Exp_Idx[1] == -1)
		uiShell_Exp_Idx[1] = 24;
	if (uiShell_Power_R_Idx[1] == -1)
		uiShell_Power_R_Idx[1] = 24;
	if (uiShell_Power_V_Idx[1] == -1)
		uiShell_Power_V_Idx[1] = 24;
	fclose(fileOut);
	uiShell_Power_PS_Idx[0] = 0;
	uiShell_Power_PS_Idx[1] = uiEjecta_Exp_Idx[0];
	if (i_cShell.dVariance_Error_Power_R < i_cShell.dVariance_Error_Power_V)
		dConst = pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNr + 1) - 1.0;
	else
		dConst = pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNv + 1) - 1.0;
//	printf("%.10e\t%.2e\t%.2e\t\n",dConst,i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNv);
	for (uiT = 0; uiT <= uiShell_Power_PS_Idx[1]; uiT++)
	{
		dT = uiT + 1.0;
		dT_25 = dT / lpfDay[uiEjecta_Exp_Idx[0]];
//		double dVal;
//		if (i_cShell.dVariance_Error_Power_R < i_cShell.dVariance_Error_Power_V)
//			dVal = pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNr + 1);
//		else
//			dVal = pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNv + 1);
//		double dCpart = dConst * dT_25 * dT_25;
//		printf("%.10e\t%.10e\n",dVal,dCpart);
		if (i_cShell.dVariance_Error_Power_R < i_cShell.dVariance_Error_Power_V)
			lpfShell_Power_PS[uiT] = i_cEjecta.dVmax * pow(pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNr + 1) - dConst * dT_25 * dT_25,1.0/(i_cShell.dNr + 1)) * 1e-8;
		else
			lpfShell_Power_PS[uiT] = i_cEjecta.dVmax * pow(pow(i_cShell.dVmax / i_cEjecta.dVmax,i_cShell.dNv + 1) - dConst * dT_25 * dT_25,1.0/(i_cShell.dNv + 1)) * 1e-8;
//		printf("%i\t%.2e\n",uiT,lpfShell_Power_PS[uiT]);
	}

	sprintf(lpszFilename,"velocity_evolution_theoretical_%04i.eps",i_uiNumber);


	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;
	epsplot::LINE_PARAMETERS	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Time after explosion [days]", false, false, true, 1.0,true, 25.0);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Velocity [kkm/s]", false, false, true, 0.0, true, 65.0);

	cPlot.Set_Plot_Title(i_lpszTitle,18.0);
	cPlot.Set_Plot_Filename(lpszFilename);



	if (i_cShell.dVmin != 0.0) // no shell
	{	// Observational constraints
		cLine_Parameters.m_eColor = epsplot::BLACK;
		cLine_Parameters.m_eStipple = epsplot::SHORT_DASH;
		cPlot.Set_Plot_Data(lpfDay, lpfObs_Const_2, 25, cLine_Parameters, uiX_Axis, uiY_Axis);
		cPlot.Set_Plot_Data(lpfDay, lpfObs_Const_3, 25, cLine_Parameters, uiX_Axis, uiY_Axis);
	}

	cLine_Parameters.m_eColor = epsplot::RED;
	if (uiEjecta_Exp_Idx[1] != uiEjecta_Exp_Idx[0])
	{
		if (i_cEjecta.dVariance_Error_Exp < i_cEjecta.dVariance_Error_Power_R && i_cEjecta.dVariance_Error_Exp < i_cEjecta.dVariance_Error_Power_V)
		{
			cLine_Parameters.m_eStipple = epsplot::LONG_DASH;
			cPlot.Set_Plot_Data(&lpfDay[uiEjecta_Exp_Idx[0]], &lpfEjecta_Exp[uiEjecta_Exp_Idx[0]], uiEjecta_Exp_Idx[1] - uiEjecta_Exp_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[0]        = "PS (exp)          ";
			lpiLegend_Line_Styles[0] = 3;
		}
		else if (i_cEjecta.dVariance_Error_Power_R < i_cEjecta.dVariance_Error_Power_V)
		{
			cLine_Parameters.m_eStipple = epsplot::LONG_SHORT_DASH;
			cPlot.Set_Plot_Data( &lpfDay[uiEjecta_Power_R_Idx[0]], &lpfEjecta_Power_R[uiEjecta_Power_R_Idx[0]], uiEjecta_Power_R_Idx[1] - uiEjecta_Power_R_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[0]        = "PS (power r)          ";
			lpiLegend_Line_Styles[0] = 4;
		}
		else
		{
			cLine_Parameters.m_eStipple = epsplot::DOTTED;
			cPlot.Set_Plot_Data( &lpfDay[uiEjecta_Power_V_Idx[0]], &lpfEjecta_Power_V[uiEjecta_Power_V_Idx[0]], uiEjecta_Power_V_Idx[1] - uiEjecta_Power_V_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[0]        = "PS (power v)          ";
			lpiLegend_Line_Styles[0] = 5;
		}
	}
	else
	{
			lplpszLegend_Test[0]        = "PS                    ";
			lpiLegend_Line_Styles[0] = 1;
	}
	if (i_cShell.dVmin != 0.0) // no shell
	{
		cLine_Parameters.m_eColor = epsplot::BLUE;
		
		if (i_cShell.dVariance_Error_Exp < i_cShell.dVariance_Error_Power_R && i_cShell.dVariance_Error_Exp < i_cShell.dVariance_Error_Power_V)
		{
			cLine_Parameters.m_eStipple = epsplot::LONG_DASH;
			cPlot.Set_Plot_Data(&lpfDay[uiShell_Exp_Idx[0]], &lpfShell_Exp[uiShell_Exp_Idx[0]], uiShell_Exp_Idx[1] - uiShell_Exp_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);

			lplpszLegend_Test[1]        = "Shell (exp)         ";
			lpiLegend_Line_Styles[1] = 3;
		}
		else if (i_cShell.dVariance_Error_Power_R < i_cShell.dVariance_Error_Power_V)
		{
			cLine_Parameters.m_eStipple = epsplot::LONG_SHORT_DASH;
			cPlot.Set_Plot_Data( &lpfDay[uiShell_Power_R_Idx[0]], &lpfShell_Power_R[uiShell_Power_R_Idx[0]], uiShell_Power_R_Idx[1] - uiShell_Power_R_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[1]        = "Shell (power r)         ";
			lpiLegend_Line_Styles[1] = 4;
		}
		else
		{
			cLine_Parameters.m_eStipple = epsplot::DOTTED;
			cPlot.Set_Plot_Data( &lpfDay[uiShell_Power_V_Idx[0]], &lpfShell_Power_V[uiShell_Power_V_Idx[0]], uiShell_Power_V_Idx[1] - uiShell_Power_V_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[1]        = "Shell (power v)         ";
			lpiLegend_Line_Styles[1] = 5;
		}
		if (uiShell_Power_PS_Idx[1] != 0)
		{
			cLine_Parameters.m_eStipple = epsplot::SHORT_DASH;
			cPlot.Set_Plot_Data( lpfDay,lpfShell_Power_PS, uiShell_Power_PS_Idx[1] - uiShell_Power_PS_Idx[0] + 1, cLine_Parameters, uiX_Axis, uiY_Axis);
			lplpszLegend_Test[3]        = "Shell (PS)         ";
			lpiLegend_Line_Styles[3] = 2;
		}
	}
	else
	{
		lplpszLegend_Test[1]        = "Shell              ";
		lpiLegend_Line_Styles[1] = 1;
	}

/*
	//@@TODO Legend
	lpiOptions_Array[0]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[0] = 1;
	lpiLegend_Line_Colors[0] = 1;
	lpfLegend_ine_Widths[0] = 1.0;
 
	lpiOptions_Array[1]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[1] = 9;
	lpiLegend_Line_Colors[1] = 9;
	lpfLegend_ine_Widths[1] = 1.0;

	lpiOptions_Array[2]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[2] = 15;
	lpiLegend_Line_Colors[2] = 15;
	lpfLegend_ine_Widths[2] = 1.0;
	lplpszLegend_Test[2]        = "Obs. HVF         ";
	lpiLegend_Line_Styles[2] = 2;

	lpiOptions_Array[3]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[3] = 9;
	lpiLegend_Line_Colors[3] = 9;
	lpfLegend_ine_Widths[3] = 1.0;
	if (uiEjecta_Exp_Idx[1] != uiEjecta_Exp_Idx[0])
	{
		lpcPLStream->legend( &fLgnd_Width,&fLgnd_Height,
			PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX, 0,
			0.0, 0.0, 0.1, 
			0, 15, 1, 0, 0,
			(i_cShell.dVmin != 0.0) ? 4 : 1, lpiOptions_Array,
			1.0, 1.0, 2.0,
			0., lpiLegend_Text_Colors, (const char * const *) lplpszLegend_Test,
			NULL, NULL, NULL, NULL,
			lpiLegend_Line_Colors, lpiLegend_Line_Styles, lpfLegend_ine_Widths,
			NULL, NULL, NULL, NULL );
	}
	else
	{
		lpcPLStream->legend( &fLgnd_Width,&fLgnd_Height,
			PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX, 0,
			0.0, 0.0, 0.1, 
			0, 15, 1, 0, 0,
			(i_cShell.dVmin != 0.0) ? 3 : 0, lpiOptions_Array,
			1.0, 1.0, 2.0,
			0., &lpiLegend_Text_Colors[1], (const char * const *) &lplpszLegend_Test[1],
			NULL, NULL, NULL, NULL,
			&lpiLegend_Line_Colors[1], &lpiLegend_Line_Styles[1], &lpfLegend_ine_Widths[1],
			NULL, NULL, NULL, NULL );
	}*/


//	printf("%.2f\t%i\t%i\n",lpfEjecta_Exp[uiEjecta_Exp_Idx[0]],uiEjecta_Exp_Idx[0],uiEjecta_Exp_Idx[1]);
/*	for (uiT = 0; uiT < 25; uiT++)
	{
		printf("%.1f\t",lpfDay[uiT]);
		if (uiEjecta_Exp_Idx[0] <= uiT && uiEjecta_Exp_Idx[1] >= uiT)
			printf("%.1f\t",lpfEjecta_Exp[uiT]);
		else
			printf("\t");
		if (uiEjecta_Power_Idx[0] <= uiT && uiEjecta_Power_Idx[1] >= uiT)
			printf("%.1f\t",lpfEjecta_Power[uiT]);
		else
			printf("\t");
		if (uiShell_Exp_Idx[0] <= uiT && uiShell_Exp_Idx[1] >= uiT)
			printf("%.1f\t",lpfShell_Exp[uiT]);
		else
			printf("\t");
		if (uiShell_Power_Idx[0] <= uiT && uiShell_Power_Idx[1] >= uiT)
			printf("%.1f\t",lpfShell_Power[uiT]);
		else
			printf("\t");
		printf("\n");
	}*/
}

void Find_Velocity_Range(const XDATASET &i_cData, double o_dVelocity_Range[2], unsigned int i_uiVelocity_Idx, unsigned int i_uiV_Lower_Idx, unsigned int i_uiV_Upper_Idx)
{
	o_dVelocity_Range[1] = 0.0; 
	o_dVelocity_Range[0] = 3.0e20; // very large value
	for(unsigned int uiI = 0; uiI < i_cData.GetNumElements(); uiI++)
	{
		double	dVlower = i_cData.GetElement(i_uiV_Lower_Idx,uiI);
		double	dVupper = i_cData.GetElement(i_uiV_Upper_Idx,uiI);
		double	dV = i_cData.GetElement(i_uiVelocity_Idx,uiI);
		if (dV > o_dVelocity_Range[1])
			o_dVelocity_Range[1] = dV;
		if (dVupper > o_dVelocity_Range[1])
			o_dVelocity_Range[1] = dVupper;
		if (dVlower > o_dVelocity_Range[1])
			o_dVelocity_Range[1] = dVlower;

		if (dV < o_dVelocity_Range[0])
			o_dVelocity_Range[0] = dV;
		if (dVupper < o_dVelocity_Range[0])
			o_dVelocity_Range[0] = dVupper;
		if (dVlower < o_dVelocity_Range[0])
			o_dVelocity_Range[0] = dVlower;
	}
	//printf("%.1f-%.1f\n",o_dVelocity_Range[0],o_dVelocity_Range[1]);
}

void Determine_Photosphere(const double & i_dPhotosphere_Reference_Velocity, 
							const XDATASET &i_cEjecta, const XDATASET & i_cShell,
							unsigned int i_uiVelocity_Idx, unsigned int i_uiDelta_Radius_Idx, unsigned int i_uiDensity_Idx,
							unsigned int i_uiTime_Grid_Data_Points, unsigned int i_uiTime_Ref_Idx, 
							const double * i_lpdDay, double * o_lpdV_ps, 
							unsigned int o_uiPS_Idx[2])
{
	unsigned int uiI, uiT;
	double	dScalar = 1.0;
	double	dSum = 0.0;
	unsigned int uiPS_Reference_Idx = 0;
	bool bPS_In_Shell = false;

	// First, identify the material density at PS vref.  Here, we assume the PS velocity will be 11.5 kkm/s on day 25.
	while (uiPS_Reference_Idx < i_cEjecta.GetNumElements() && i_cEjecta.GetElement(i_uiVelocity_Idx,uiPS_Reference_Idx) < i_dPhotosphere_Reference_Velocity)
		uiPS_Reference_Idx++;
	if (uiPS_Reference_Idx >= i_cEjecta.GetNumElements())
	{
		bPS_In_Shell = true;
		uiPS_Reference_Idx = 0;
		while (uiPS_Reference_Idx < i_cShell.GetNumElements() && i_cShell.GetElement(i_uiVelocity_Idx,uiPS_Reference_Idx) < i_dPhotosphere_Reference_Velocity)
			uiPS_Reference_Idx++;
	}
	if (!bPS_In_Shell)
	{
		
		for (uiI = uiPS_Reference_Idx; uiI < i_cEjecta.GetNumElements(); uiI++)
		{
			dSum += i_cEjecta.GetElement(i_uiDensity_Idx,uiI) * i_cEjecta.GetElement(i_uiDelta_Radius_Idx,uiI) * i_cEjecta.GetElement(i_uiVelocity_Idx,uiI);
		}
		uiI = 0;
	}
	else
		uiI = uiPS_Reference_Idx;
//	printf("%.2e\t%i\n",dSum,uiI);		
	double	dShell_Sum = 0.0;
	for (; uiI < i_cShell.GetNumElements(); uiI++)
	{
		dShell_Sum += i_cShell.GetElement(i_uiDensity_Idx,uiI) * i_cShell.GetElement(i_uiDelta_Radius_Idx,uiI) * i_cShell.GetElement(i_uiVelocity_Idx,uiI);
	}
	dSum += dShell_Sum;
	dScalar = pow(i_lpdDay[i_uiTime_Ref_Idx] * 24.0 * 3600.0,2.0) / dSum; // this scalar term incorporates the scattering coefficient, reference time of vps and of the source data, and some ionization fraction
//	printf("%.1f %.2e %.2e %i %c\n",i_lpdDay[i_uiTime_Ref_Idx],dSum,dScalar,uiPS_Reference_Idx,bPS_In_Shell ? 'S' : 'E');
	for (uiT = 0; uiT < i_uiTime_Grid_Data_Points; uiT++)
	{

		double dTime_s = i_lpdDay[uiT] * (24.0 * 3600.0);
		double	dDay_Const = pow(dTime_s,-2.0) * dScalar;
		double dTau = 0.0;

		uiI = i_cShell.GetNumElements() - 1;
		while (uiI < i_cShell.GetNumElements() && (dTau * dDay_Const) < 1.0)
		{
			dTau += i_cShell.GetElement(i_uiDensity_Idx,uiI) * i_cShell.GetElement(i_uiDelta_Radius_Idx,uiI) * i_cShell.GetElement(i_uiVelocity_Idx,uiI);
			uiI--;
		}
		if ((dTau * dDay_Const) >= 1.0 && uiI < i_cShell.GetNumElements())
		{
			o_lpdV_ps[uiT] = i_cShell.GetElement(i_uiVelocity_Idx,uiI) * 1.0e-8;
//			printf("PS shell %i %.1f %.1e %.1e %.1e\n",uiI, i_lpdDay[uiT],dTau,o_lpdV_ps[uiT],dDay_Const);
		}
		else
		{
			uiI = i_cEjecta.GetNumElements() - 1;
//			printf("%.1e\t",dTau);
			while (uiI < i_cEjecta.GetNumElements() && (dTau * dDay_Const) < 1.0)
			{
				dTau += i_cEjecta.GetElement(i_uiDensity_Idx,uiI) * i_cEjecta.GetElement(i_uiDelta_Radius_Idx,uiI) * i_cEjecta.GetElement(i_uiVelocity_Idx,uiI);
				uiI--;
			}

			if ((dTau * dDay_Const) >= 1.0 && uiI < i_cEjecta.GetNumElements())
			{
				o_lpdV_ps[uiT] = i_cEjecta.GetElement(i_uiVelocity_Idx,uiI) * 1.0e-8;
//			printf("PS ejecta %i %.1f %.1e %.1e %.1e\n",uiI, i_lpdDay[uiT],dTau,o_lpdV_ps[uiT],dDay_Const);
			}
		}
	}

	uiT = 0;
	while (uiT < i_uiTime_Grid_Data_Points && o_lpdV_ps[uiT] < 0)
		uiT++;
	o_uiPS_Idx[0] = uiT;
	while (uiT < i_uiTime_Grid_Data_Points && o_lpdV_ps[uiT] > 0)
		uiT++;
	o_uiPS_Idx[1] = uiT;

	if (o_uiPS_Idx[0] == i_uiTime_Grid_Data_Points)
		o_uiPS_Idx[0] = i_uiTime_Grid_Data_Points - 1;
	if (o_uiPS_Idx[1] == i_uiTime_Grid_Data_Points)
		o_uiPS_Idx[1] = i_uiTime_Grid_Data_Points - 1;
}

void Fill_Opacity_Map(const double &i_dV_Ref, double ** o_lpdOpacity_Map, const XDATASET i_cData, unsigned int i_uiTime_Grid_Data_Points, unsigned int i_uiVel_Grid_Data_Points,
	const double * i_lpdDay, unsigned int i_uiVelocity_Idx, unsigned int i_uiV_Lower_Face_Idx, unsigned int i_uiV_Upper_Face_Idx, 
	unsigned int * i_lpuiElement_Group_Idx, unsigned int i_uiNum_Element_Group, unsigned int i_uiDensity_Idx, const double i_lpdVelocity_Range[2], const double & i_dDelta_Vel_Bin,
	unsigned int i_uiTime_Ref_Idx, const double * i_lpdV_ps, OPACITY_PROFILE_DATA & o_cOP_Data, OPACITY_PROFILE_DATA::GROUP i_eGroup)
{
	unsigned int uiI, uiT;
	unsigned int uiAbd_Max_Bin = 0;
	double	dAbd_Density;
	double	dAbd_Dens_Max = 0.0;
	double	dAbd_Max;
	double	dAbd_Dens;
	for (uiT = 0; uiT < i_uiTime_Grid_Data_Points; uiT++)
		memset(o_lpdOpacity_Map[uiT],0,sizeof(double) * i_uiVel_Grid_Data_Points); // clear all data

	uiI = 0;
	while (uiI < i_cData.GetNumElements())
	{
		double dVel = i_cData.GetElement(i_uiVelocity_Idx,uiI);
//			double dV_upper = i_cData.GetElement(i_uiVelocity_Idx,uiI);
		double	dVlower = i_cData.GetElement(i_uiV_Lower_Face_Idx,uiI);
		double	dVupper = i_cData.GetElement(i_uiV_Upper_Face_Idx,uiI);
		double	dAbd = 1.0;
		if (i_uiNum_Element_Group > 0)
		{
			dAbd = 0.0;
			for (unsigned int uiJ = 0; uiJ < i_uiNum_Element_Group; uiJ++)
			{
//					printf("%i - %i: %.2e\n",uiJ,i_lpuiElement_Group_Idx[uiJ],i_cData.GetElement(i_lpuiElement_Group_Idx[uiJ],uiI));
				double 	dPotential_Abd = i_cData.GetElement(i_lpuiElement_Group_Idx[uiJ],uiI);
				if (dPotential_Abd > 1.01e-10)
					dAbd += dPotential_Abd;
			}
//				printf("%i: %.2e\n",i_uiNum_Element_Group,dAbd);
		}


//			double	dVupper = dVel + 0.5 * dDelta_Vel;
		double	dMult = 1.0;
		if (dVlower < 0.0)
			dVlower = 0.0;
		if (dVlower > dVupper)
		{
			double dTemp = dVupper;
			dVupper = dVlower;
			dVlower = dTemp;
		}
//			printf("%.2e %.2e %.2e %.2e\n",dVlower,dVupper,i_lpdVelocity_Range[0],i_dDelta_Vel_Bin);
		unsigned int uiBin_Lower = (dVlower - i_lpdVelocity_Range[0]) / i_dDelta_Vel_Bin;
		unsigned int uiBin_Upper = (dVupper - i_lpdVelocity_Range[0]) / i_dDelta_Vel_Bin;
		if (dVlower < i_lpdVelocity_Range[0] || dVupper < i_lpdVelocity_Range[0])
		{
			uiBin_Lower = 0; // this can happen because range values are at bin center
		}
		if (uiBin_Lower > uiBin_Upper)
		{
			unsigned int uiTemp = uiBin_Upper;
			uiBin_Upper = uiBin_Lower;
			uiBin_Lower = uiTemp;
		}
		if (uiBin_Upper > (i_uiVel_Grid_Data_Points - 1))
			uiBin_Upper = i_uiVel_Grid_Data_Points - 1;
//			printf("%i %i\n",uiBin_Lower,uiBin_Upper);
		double dDens = i_cData.GetElement(i_uiDensity_Idx,uiI);
		for (unsigned int uiBin = uiBin_Lower; uiBin <= uiBin_Upper && uiBin < (i_uiVel_Grid_Data_Points - 1); uiBin++)
		{
			double dBin_Lower = (uiBin - 0.5) * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0];
			double dBin_Upper = (uiBin + 0.5) * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0];
			if (dBin_Lower <= dVlower && dVlower <= dBin_Upper && dBin_Lower <= dVupper && dVupper <= dBin_Upper)
			{
				dMult = fabs(dVupper - dVlower) / i_dDelta_Vel_Bin;
			}
			else if (dBin_Lower <= dVlower && dVlower <= dBin_Upper)
			{
				dMult = 1.0 - (dVlower - dBin_Lower) / i_dDelta_Vel_Bin;
			}
			else if (dBin_Lower <= dVupper && dVupper <= dBin_Upper)
			{
				dMult = (dVupper - dBin_Lower) / i_dDelta_Vel_Bin;
			}
			else
				dMult = 1.0;

			for (uiT = 0; uiT < i_uiTime_Grid_Data_Points; uiT++)
			{
				double dTime_s = i_lpdDay[uiT] * (24.0 * 3600.0);
				o_lpdOpacity_Map[uiT][uiBin] += dMult * dAbd * dDens * pow(dTime_s,-2.0);
			}
		}
		dAbd_Dens = dAbd * dDens;

		if (dAbd_Dens > dAbd_Dens_Max)
		{
			uiAbd_Max_Bin = (uiBin_Upper + uiBin_Lower) >> 1; // put it in the middle
			dAbd_Dens_Max = dAbd_Dens;
			dAbd_Max = dAbd;
			dAbd_Density = dDens;
		}

		uiI++;
	}
	//printf("First while\n");
	unsigned int uiBin_Ref = uiAbd_Max_Bin;//(i_dV_Ref - i_lpdVelocity_Range[0]) / i_dDelta_Vel_Bin;
	//printf("bin ref  = %.2e %.2e %.2e %i (%i)\n",i_dV_Ref,i_lpdVelocity_Range[0],i_dDelta_Vel_Bin,uiBin_Ref,i_uiVel_Grid_Data_Points);
	if (uiBin_Ref < i_uiVel_Grid_Data_Points)// < i_dV_Ref);
	{
		//printf("Dest bin != 0\n");
		double dRef_Mult = o_lpdOpacity_Map[i_uiTime_Grid_Data_Points - 1][uiBin_Ref];

		o_cOP_Data.Set_Velocity(i_eGroup,i_lpdVelocity_Range[0] + i_dDelta_Vel_Bin * uiAbd_Max_Bin);
		o_cOP_Data.Set_Scalar(i_eGroup,dRef_Mult);
		o_cOP_Data.Set_Abundance(i_eGroup,dAbd_Max);
		o_cOP_Data.Set_Density(i_eGroup,dAbd_Density);
		o_cOP_Data.Set_Normalization_Time(i_eGroup,i_lpdDay[i_uiTime_Grid_Data_Points - 1] * (24.0 * 3600.0));
		double dRef = 1.0 / dRef_Mult;
		for (uiI = 0; uiI < i_uiVel_Grid_Data_Points; uiI++)
		{
			for (uiT = 0; uiT < i_uiTime_Grid_Data_Points; uiT++)
			{
//				double dVel = (uiI * i_dDelta_Vel_Bin + i_lpdVelocity_Range[0]) * 1e-8;
				// normalize to reference time
				o_lpdOpacity_Map[uiT][uiI] *= dRef;
//				if (dVel < i_lpdV_ps[uiT])
//					o_lpdOpacity_Map[uiT][uiI] = 0.0;
			}
		}
	}
	else // 
	{
		o_cOP_Data.Set_Velocity(i_eGroup,0.0);
		o_cOP_Data.Set_Scalar(i_eGroup,0.0);
		o_cOP_Data.Set_Abundance(i_eGroup,0.0);
		o_cOP_Data.Set_Density(i_eGroup,0.0);
		o_cOP_Data.Set_Normalization_Time(i_eGroup,0.0);
		for (uiI = 0; uiI < i_uiTime_Grid_Data_Points; uiI++)
		{
			memset(o_lpdOpacity_Map[uiI],0,sizeof(double) * i_uiVel_Grid_Data_Points); // clear all data
		}
	}
}


void Save_Opacity_Map_Data(XDATASET & o_cOpacity_Map, unsigned int i_uiTime_Grid_Data_Points, unsigned int i_uiVel_Grid_Data_Points, 
	const double * i_lpdDay, const double * i_lpdVelocity_Range_Grid, const double & i_dDelta_V_Grid, 
	const double ** i_lpdOpacity_Map_Data,	const char * i_lpszMap_Component, const char * i_lpszMap_Element_Group)
{
	char lpszFilename[128];
	o_cOpacity_Map.Allocate(i_uiTime_Grid_Data_Points + 1,i_uiVel_Grid_Data_Points+1);
	// clear out the data at 0,0 - this space won't be used for anything.
	o_cOpacity_Map.Zero();
	//o_cOpacity_Map.SetElement(0,0,0.0);

	for (unsigned int uiT = 0; uiT < i_uiTime_Grid_Data_Points; uiT++)
	{
		o_cOpacity_Map.SetElement(uiT + 1,0,i_lpdDay[uiT]);
		for (unsigned int uiI = 0; uiI < i_uiVel_Grid_Data_Points; uiI++)
		{
			if (uiT == 0)
				o_cOpacity_Map.SetElement(0,uiI + 1,i_lpdVelocity_Range_Grid[0] + i_dDelta_V_Grid * uiI);
			if (i_lpdOpacity_Map_Data[uiT][uiI] != 0.0) // leave empty cell flag set if no opacity at a given velocity
				o_cOpacity_Map.SetElement(uiT + 1,uiI + 1,i_lpdOpacity_Map_Data[uiT][uiI]);
		}
	}
	if (i_lpszMap_Element_Group)
		sprintf(lpszFilename,"opacity_map_%s.%s.xdataset",i_lpszMap_Component,i_lpszMap_Element_Group);
	else
		sprintf(lpszFilename,"opacity_map_%s.xdataset",i_lpszMap_Component);

	o_cOpacity_Map.SaveDataFileBin(lpszFilename);
}

void Velocity_Evolution_Simulation(const XDATASET &i_cEjecta, const XDATASET &i_cShell, unsigned int i_uiNumber, const char * i_lpszTitle)
{
#define VEL_GRID_DATA_POINTS	2048
#define TIME_GRID_DATA_POINTS	128
	FILE * fileOut;
	char lpszFilename[128];
	XDATASET cOpactiy_Map;
	XDATASET cOpactiy_Map_Shell;
	XDATASET cOpactiy_Map_Ejecta;

	int lpiOptions_Array[4];
	int lpiLegend_Text_Colors[4];
	const char *lplpszLegend_Test[4];
	int lpiLegend_Line_Colors[4];
	int lpiLegend_Line_Styles[4];
	double lpfLegend_ine_Widths[4];
	double fLgnd_Width,fLgnd_Height;

	unsigned int uiI, uiT;
	double	lpdDay[TIME_GRID_DATA_POINTS];
	double	lpdV_ps[TIME_GRID_DATA_POINTS];
	double	lpdV_hvf[TIME_GRID_DATA_POINTS];
	double	lpdObs_Const_2[TIME_GRID_DATA_POINTS];
	double	lpdObs_Const_3[TIME_GRID_DATA_POINTS];
	double	** lpdOpacity_Map_Shell = NULL;
	double	** lpdOpacity_Map_Ejecta = NULL;
	double	** lpdOpacity_Map_Combined = NULL;

	unsigned int uiTime_Ref_Idx = 96;

	unsigned int uiRadius_Idx = 0;
	unsigned int uiDelta_Radius_Idx = 1;
	unsigned int uiDensity_Idx = 8;
	unsigned int uiVelocity_Idx = 25;
	unsigned int uiV_Lower_Face_Idx = 26;
	unsigned int uiV_Upper_Face_Idx = 27;
	unsigned int uiCalcium_Idx = 4;
	unsigned int uiSilicon_Idx = 21;
	unsigned int uiSulfur_Idx = 20;
	unsigned int uiArgon_Idx = 2;
	unsigned int uiTitanium44_Idx = 22;
	unsigned int uiTitanium48_Idx = 23;
	unsigned int uiChromium48_Idx = 6;
	unsigned int uiChromium52_Idx = 7;
	unsigned int uiManganese52_Idx = 15;
	unsigned int uiIron52_Idx = 9;
	unsigned int uiIron56_Idx = 10;
	unsigned int uiNickel_Idx = 18;
	unsigned int uiCobalt_Idx = 5;
	unsigned int uiNeon_Idx = 17;
	unsigned int uiMagnesium_Idx = 14;
	unsigned int uiCarbon_Idx = 3;
	unsigned int uiOxygen_Idx = 19;
	unsigned int uiVanadium_Idx = 19;

	unsigned int uiSilicon_Group_Idx[] = {uiSilicon_Idx, uiSulfur_Idx, uiArgon_Idx, uiCalcium_Idx, uiTitanium44_Idx, uiTitanium48_Idx,uiChromium48_Idx,uiChromium52_Idx,uiVanadium_Idx,uiManganese52_Idx};
	unsigned int uiSilicon_Group_Size = sizeof(uiSilicon_Group_Idx) / sizeof(unsigned int);
	unsigned int uiIron_Group_Idx[] = {uiIron52_Idx,uiIron56_Idx,uiCobalt_Idx,uiNickel_Idx};
	unsigned int uiIron_Group_Size = sizeof(uiIron_Group_Idx) / sizeof(unsigned int);
	unsigned int uiMagnesium_Group_Idx[] = {uiNeon_Idx,uiMagnesium_Idx};
	unsigned int uiMagnesium_Group_Size = sizeof(uiMagnesium_Group_Idx) / sizeof(unsigned int);

	unsigned int uiPS_Idx[2] = {0,24};
	unsigned int uiHVF_Idx[2] = {0,24};

	double	dTime_s,dTau;
	double	dVel_Range_Ejecta[2];
	double	dVel_Range_Shell[2];
	double	dVelocity_Range_Grid[2];
	double	dDelta_V_Grid;

	OPACITY_PROFILE_DATA	cOP_Data;


	Find_Velocity_Range(i_cEjecta,dVel_Range_Ejecta,uiVelocity_Idx,uiV_Lower_Face_Idx,uiV_Upper_Face_Idx);
	Find_Velocity_Range(i_cShell,dVel_Range_Shell,uiVelocity_Idx,uiV_Lower_Face_Idx,uiV_Upper_Face_Idx);

	dVelocity_Range_Grid[0] = dVel_Range_Ejecta[0];
	dVelocity_Range_Grid[1] = dVel_Range_Shell[1];
	if (i_cShell.GetNumElements() == 0 || dVel_Range_Shell[1] > 3.0e10) // no shell or invalid data
		dVelocity_Range_Grid[1] = dVel_Range_Ejecta[1];
	dDelta_V_Grid = (dVelocity_Range_Grid[1] - dVelocity_Range_Grid[0]) / VEL_GRID_DATA_POINTS;
	printf("%.2e\t%.2e\t%.2e\n",dVelocity_Range_Grid[0],dVelocity_Range_Grid[1],dDelta_V_Grid);
	if (dDelta_V_Grid * 0.5 > dVelocity_Range_Grid[0])
	{ // ensure minimum grid velocity >= 0
		dVelocity_Range_Grid[1] = dDelta_V_Grid * (VEL_GRID_DATA_POINTS - 0.5);
		dVelocity_Range_Grid[0] = dDelta_V_Grid * 0.5;
	}

	double	dTime_Const = 32.0 / TIME_GRID_DATA_POINTS;
	// Initialize variables
	for (uiT = 0; uiT < TIME_GRID_DATA_POINTS; uiT++)
	{
		lpdDay[uiT] = uiT * dTime_Const + 1.0;
		lpdV_ps[uiT] = -1.0;
		lpdV_hvf[uiT] = -1.0;
		lpdObs_Const_2[uiT] = 15.0 * pow(lpdDay[uiT] / 25.0,-0.2);
		lpdObs_Const_3[uiT] = 20.0 * pow(lpdDay[uiT] / 25.0,-0.3);
	}

	// Call routine to fill the lpdV_ps variable with the photosphere information
//	printf("photosphere\n");
	Determine_Photosphere(11.5e8, i_cEjecta, i_cShell, uiVelocity_Idx, uiDelta_Radius_Idx, uiDensity_Idx, TIME_GRID_DATA_POINTS, uiTime_Ref_Idx, lpdDay, lpdV_ps, uiPS_Idx);


	// Output the photospere information to a file
	sprintf(lpszFilename,"velocity_evolution_simulation_%04i.csv",i_uiNumber);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"t,PS\n");
	for (uiT = 0; uiT < TIME_GRID_DATA_POINTS; uiT++)
	{
		fprintf(fileOut,"%.2f, ", lpdDay[uiT]);
		if (lpdV_ps[uiT] > 0.0)
			fprintf(fileOut,"%.17e",lpdV_ps[uiT]);
		fprintf(fileOut,"\n");
	}
	fclose(fileOut);

//	printf("grid alloc\n");
	//@@TODO implement imagemapping
//	plAlloc2dGrid( &lpdOpacity_Map_Shell, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	lpdOpacity_Map_Shell = new2d<double>(TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS);
//	plAlloc2dGrid( &lpdOpacity_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	lpdOpacity_Map_Ejecta = new2d<double>(TIME_GRID_DATA_POINTS,  VEL_GRID_DATA_POINTS);
//	plAlloc2dGrid( &lpdOpacity_Map_Combined, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	lpdOpacity_Map_Combined = new2d<double>(TIME_GRID_DATA_POINTS,  VEL_GRID_DATA_POINTS);

//	printf("fill ejecta map\n");
//	double	dRef_Mult[6] = {0,0,0,0,0,0};
//	double	dRef_Abd[5] = {0,0,0,0,0};
	
	cOP_Data.Set_Abundance(OPACITY_PROFILE_DATA::MAGNESIUM,0.0);
	cOP_Data.Set_Abundance(OPACITY_PROFILE_DATA::IRON,0.0);
	cOP_Data.Set_Abundance(OPACITY_PROFILE_DATA::SILICON,0.0);

//	printf("Filling opacities - Mg\n");
	Fill_Opacity_Map(11.5e8, lpdOpacity_Map_Ejecta, i_cEjecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
				lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
				uiMagnesium_Group_Idx, uiMagnesium_Group_Size, uiDensity_Idx,
				dVelocity_Range_Grid, dDelta_V_Grid, uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::MAGNESIUM);
	Save_Opacity_Map_Data(cOpactiy_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay, dVelocity_Range_Grid,
				dDelta_V_Grid, (const double **) lpdOpacity_Map_Ejecta, "ejecta", "Mg");
//	printf("Filling opacities - Fe\n");
	Fill_Opacity_Map(11.5e8, lpdOpacity_Map_Ejecta, i_cEjecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
				lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
				uiIron_Group_Idx, uiIron_Group_Size, uiDensity_Idx, dVelocity_Range_Grid,dDelta_V_Grid,
				uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::IRON);
	Save_Opacity_Map_Data(cOpactiy_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay, dVelocity_Range_Grid,
				dDelta_V_Grid, (const double **) lpdOpacity_Map_Ejecta, "ejecta", "Fe");
//	printf("Filling opacities - C\n");
	Fill_Opacity_Map(11.5e8, lpdOpacity_Map_Ejecta, i_cEjecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
				lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
				&uiCarbon_Idx, 1, uiDensity_Idx, dVelocity_Range_Grid,dDelta_V_Grid,
				uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::CARBON);
	Save_Opacity_Map_Data(cOpactiy_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay, dVelocity_Range_Grid,
				dDelta_V_Grid, (const double **) lpdOpacity_Map_Ejecta, "ejecta", "C");
//	printf("Filling opacities - O\n");
	Fill_Opacity_Map(11.5e8, lpdOpacity_Map_Ejecta, i_cEjecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
				lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
				&uiOxygen_Idx, 1, uiDensity_Idx, dVelocity_Range_Grid,dDelta_V_Grid,
				uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::OXYGEN);
	Save_Opacity_Map_Data(cOpactiy_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay, dVelocity_Range_Grid,
				dDelta_V_Grid, (const double **) lpdOpacity_Map_Ejecta, "ejecta", "O");
	
//	printf("Filling opacities - Si\n");
	Fill_Opacity_Map(11.5e8, lpdOpacity_Map_Ejecta, i_cEjecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
				lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
				uiSilicon_Group_Idx, uiSilicon_Group_Size, uiDensity_Idx,
				dVelocity_Range_Grid, dDelta_V_Grid,	uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::SILICON);
	Save_Opacity_Map_Data(cOpactiy_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay,
				dVelocity_Range_Grid, dDelta_V_Grid, (const double **) lpdOpacity_Map_Ejecta, "ejecta", "Si");
//	printf("fill shell map\n");
	if (i_cShell.GetNumElements() > 0)
	{
//		printf("Filling opacities - Shell\n");
		Fill_Opacity_Map(18.5e8,lpdOpacity_Map_Shell, i_cShell, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS,
					lpdDay, uiVelocity_Idx, uiV_Lower_Face_Idx,uiV_Upper_Face_Idx, 
					NULL, 0, uiDensity_Idx, dVelocity_Range_Grid,dDelta_V_Grid,
					uiTime_Ref_Idx,lpdV_ps,cOP_Data,OPACITY_PROFILE_DATA::SHELL);
		Save_Opacity_Map_Data(cOpactiy_Map_Shell, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS, lpdDay, 
					dVelocity_Range_Grid, dDelta_V_Grid, (const double **) lpdOpacity_Map_Shell, "shell", NULL);
	}


	cOpactiy_Map.Allocate(TIME_GRID_DATA_POINTS + 1,VEL_GRID_DATA_POINTS+1);
	// clear out the data at 0,0 - this space won't be used for anything.
	cOpactiy_Map.Zero();
//	cOpactiy_Map.SetElement(0,0,0.0);

	FILE * fileOP_Map = fopen("opacity_map_ejecta_Si.csv","wt");
	if (fileOP_Map)
	{
		fprintf(fileOP_Map,"Velocity, Tau(t = 1d), Tau(t = 32.75d)\n");
		for (unsigned int uiI = 0; uiI < VEL_GRID_DATA_POINTS; uiI++)
		{
			fprintf(fileOP_Map,"%.17e, %.17e, %.17e\n",dDelta_V_Grid * uiI + dVelocity_Range_Grid[0],lpdOpacity_Map_Ejecta[0][uiI],lpdOpacity_Map_Ejecta[TIME_GRID_DATA_POINTS - 1][uiI]);
		}
		fclose(fileOP_Map);
	}

	//printf("combined opacity map\n");
	for (uiT = 0; uiT < TIME_GRID_DATA_POINTS; uiT++)
	{
		cOpactiy_Map.SetElement(uiT + 1,0,lpdDay[uiT]);
		for (uiI = 0; uiI < VEL_GRID_DATA_POINTS; uiI++)
		{
			if (i_cShell.GetNumElements() > 0)
				lpdOpacity_Map_Combined[uiT][uiI] = lpdOpacity_Map_Ejecta[uiT][uiI] + lpdOpacity_Map_Shell[uiT][uiI];
			else
				lpdOpacity_Map_Combined[uiT][uiI] = lpdOpacity_Map_Ejecta[uiT][uiI];
			if (uiT == 0)
			{
				cOpactiy_Map.SetElement(0,uiI + 1,dVelocity_Range_Grid[0] + dDelta_V_Grid * uiI);
			}
			if (lpdOpacity_Map_Combined[uiT][uiI] != 0.0)
				cOpactiy_Map.SetElement(uiT + 1,uiI + 1,lpdOpacity_Map_Combined[uiT][uiI]);
		}
	}
	double	dShell_Max = -100.0, dEjecta_Max = -100.0;
	for (uiT = 0; uiT < TIME_GRID_DATA_POINTS; uiT++)
	{
		for (uiI = 0; uiI < VEL_GRID_DATA_POINTS; uiI++)
		{
			if (i_cShell.GetNumElements() > 0)
			{
				if (lpdOpacity_Map_Shell[uiT][uiI] > 0)
					lpdOpacity_Map_Shell[uiT][uiI] = log10(lpdOpacity_Map_Shell[uiT][uiI]);
				if (lpdOpacity_Map_Shell[uiT][uiI] < 0.01)
					lpdOpacity_Map_Shell[uiT][uiI] = -2.0;

				if (lpdOpacity_Map_Shell[uiT][uiI] > dShell_Max)
					dShell_Max = lpdOpacity_Map_Shell[uiT][uiI];
			}
			if (lpdOpacity_Map_Ejecta[uiT][uiI] > 0)
				lpdOpacity_Map_Ejecta[uiT][uiI] = log10(lpdOpacity_Map_Ejecta[uiT][uiI]);
			if (lpdOpacity_Map_Ejecta[uiT][uiI] < 0.01)
				lpdOpacity_Map_Ejecta[uiT][uiI] = -2.0;

			if (lpdOpacity_Map_Ejecta[uiT][uiI] > dEjecta_Max)
				dEjecta_Max = lpdOpacity_Map_Ejecta[uiT][uiI];
		}
	}

//	printf("Saving OP data\n");
	cOP_Data.m_dReference_Time_s = g_dTime_Ref;
	cOP_Data.Save_Verbose("opacity_map_scalars.txt");
	cOP_Data.Save("opacity_map_scalars.opdata");
//	printf("Done\n");

	char lpszTitle_Lcl[64];

	for (uiT = 0; uiT < TIME_GRID_DATA_POINTS; uiT++)
	{
		for (uiI = 0; uiI < VEL_GRID_DATA_POINTS; uiI++)
		{
			// get rid of photosphere for plotting
			double dVel = (uiI * dDelta_V_Grid + dVelocity_Range_Grid[0]) * 1e-8;
			if (dVel < lpdV_ps[uiT])
				lpdOpacity_Map_Combined[uiT][uiI] = 0.0;
			// limit to 1 for plotting
			if (lpdOpacity_Map_Combined[uiT][uiI] > 1.0)
				lpdOpacity_Map_Combined[uiT][uiI] = 1.0;

		}
	}
//	printf("Generating plots\n");
/*
	sprintf(lpszFilename,"velocity_evolution_simulation_%04i.eps",i_uiNumber);
	plstream * lpcPLStream = new plstream;
	lpcPLStream->scolbg(255,255,255);
	lpcPLStream->sdev("epscairo");
	lpcPLStream->sfnam(lpszFilename);
	lpcPLStream->sori(1);// supposed to be portrait, but comes out landscape	

	lpcPLStream->init();
	lpcPLStream->scol0(15,0,0,0); // since black was redefined to white, redefine white to black
	lpcPLStream->col0(15); // use black for axes
	lpcPLStream->env(1.0,25.0,0.0,65.0,0,0);
	
//	lpcPLStream->lab("Time after explosion [days]", "Velocity [kkm/s]", i_lpszTitle);
	lpcPLStream->lab("Time after explosion [days]", "Velocity [kkm/s]", "");

	{
	
		double fInt[3] = {0.0,1e-10, 1.0};
		double fRed[3] = {0.0,0.5, 1.0};
		double fGreen[3] = {0.0,0.0, 0.0};
		double fBlue[3] = {0.0,0.0, 0.0};
		double fAlpha[3] = {0.0,0.1,1.0};
		bool	bRev[3] = {false,false,false};
		lpcPLStream->scmap1n(VEL_GRID_DATA_POINTS);
		lpcPLStream->scmap1la(true, 3, fInt, fRed, fGreen, fBlue, fAlpha, bRev);
		lpcPLStream->image(lpdOpacity_Map_Ejecta,TIME_GRID_DATA_POINTS,VEL_GRID_DATA_POINTS,lpdDay[0],lpdDay[TIME_GRID_DATA_POINTS - 1],dVelocity_Range_Grid[0]*1e-8,dVelocity_Range_Grid[1]*1e-8,0.0,0.0,lpdDay[0],lpdDay[TIME_GRID_DATA_POINTS - 1],dVelocity_Range_Grid[0]*1e-8,dVelocity_Range_Grid[1]*1e-8);

	}
	if (i_cShell.GetNumElements() > 0)
	{
		
		double fInt[3] = {0.0,1e-10, 1.0};
		double fRed[3] = {0.0,0.0, 0.0};
		double fGreen[3] = {0.0,0.0, 0.0};
		double fBlue[3] = {0.0,0.5, 1.0};
		double fAlpha[3] = {0.0,0.1,1.0};
		bool	bRev[3] = {false,false,false};
		lpcPLStream->scmap1n(VEL_GRID_DATA_POINTS);
		lpcPLStream->scmap1la(true, 3, fInt, fRed, fGreen, fBlue, fAlpha, bRev);
		lpcPLStream->image(lpdOpacity_Map_Shell,TIME_GRID_DATA_POINTS,VEL_GRID_DATA_POINTS,lpdDay[0],lpdDay[TIME_GRID_DATA_POINTS - 1],dVelocity_Range_Grid[0]*1e-8,dVelocity_Range_Grid[1]*1e-8,0.0,0.0,lpdDay[0],lpdDay[TIME_GRID_DATA_POINTS - 1],dVelocity_Range_Grid[0]*1e-8,dVelocity_Range_Grid[1]*1e-8);

	}


	// draw line showing actual photosphere	
	lpcPLStream->col0(7); // gray
	if (uiPS_Idx[0] != uiPS_Idx[1])
	{
		lpcPLStream->lsty(1);
		lpcPLStream->line(uiPS_Idx[1] - uiPS_Idx[0] + 1, &lpdDay[uiPS_Idx[0]], &lpdV_ps[uiPS_Idx[0]]);
	}

	// Draw color chart
	lpcPLStream->col0(0); // white
	double lpCoordsX[5], lpCoordsY[5];
	lpCoordsX[0] = 15.0; lpCoordsY[0] = 65.00;
	lpCoordsX[1] = 25.00; lpCoordsY[1] = 65.00;
	lpCoordsX[2] = 25.00; lpCoordsY[2] = 42.0;
	lpCoordsX[3] = 15.0; lpCoordsY[3] = 42.0;
	lpCoordsX[4] = lpCoordsX[0]; lpCoordsY[4] = lpCoordsY[0];
	lpcPLStream->fill(4,lpCoordsX,lpCoordsY);
	lpcPLStream->col0(15); // black
	lpcPLStream->lsty(1);
	lpcPLStream->line(5, &lpCoordsX[0], &lpCoordsY[0]);
	double	**lpdRange;
//	plAlloc2dGrid( &lpdRange, 1, 257 );
	lpdRange = new double [1 * 257];
	for (unsigned int uiI = 0; uiI < 257; uiI++)
	{
		lpdRange[0][uiI] = uiI / 256.0;
	}
	{
		char lpszValue[8];
		double fInt[3] = {0.0,1e-10, 1.0};
		double fRed[3] = {0.0,0.5, 1.0};
		double fGreen[3] = {0.0,0.0, 0.0};
		double fBlue[3] = {0.0,0.0, 0.0};
		double fAlpha[3] = {0.0,0.1,1.0};
		double dChar_Height, dChar_Scale;
		bool	bRev[3] = {false,false,false};
		lpcPLStream->scmap1n(256);
		lpcPLStream->scmap1la(true, 3, fInt, fRed, fGreen, fBlue, fAlpha, bRev);
		lpcPLStream->image(lpdRange,1,257,18.0,20.0,49.0,59.0,0.0,0.0,18.0,20.0,49.0,59.0);
		lpcPLStream->col0(15); // black
		lpcPLStream->gchr(dChar_Height,dChar_Scale);
		lpcPLStream->schr(0.0,0.675);
		lpcPLStream->ptex(19.0,61.0,1.0,0.0,0.5,"PS LGR");
		sprintf(lpszValue,"%.1f",dEjecta_Max);
		lpcPLStream->ptex(18.0,59.0,1.0,0.0,01.0,lpszValue);
		lpcPLStream->ptex(18.0,49.0,1.0,0.0,01.0,"<= -2.0");
		lpcPLStream->schr(0.0,1.0);
	}
	if (i_cShell.GetNumElements() > 0)
	{
		char lpszValue[8];
		double fInt[3] = {0.0,1e-10, 1.0};
		double fRed[3] = {0.0,0.0, 0.0};
		double fGreen[3] = {0.0,0.0, 0.0};
		double fBlue[3] = {0.0,0.5, 1.0};
		double fAlpha[3] = {0.0,0.1,1.0};
		double dChar_Height, dChar_Scale;
		bool	bRev[3] = {false,false,false};
		lpcPLStream->scmap1n(256);
		lpcPLStream->scmap1la(true, 3, fInt, fRed, fGreen, fBlue, fAlpha, bRev);
		lpcPLStream->image(lpdRange,1,257,20.0,22.0,49.0,59.0,0.0,0.0,20.0,22.0,49.0,59.0);
		lpcPLStream->col0(15); // black
		lpcPLStream->gchr(dChar_Height,dChar_Scale);
		lpcPLStream->schr(0.0,0.675);
		lpcPLStream->ptex(21.0,61.0,1.0,0.0,0.5,"HVF LGR");
		sprintf(lpszValue,"%.1f",dShell_Max);
		lpcPLStream->ptex(22.0,59.0,1.0,0.0,0.0,lpszValue);
		lpcPLStream->ptex(22.0,49.0,1.0,0.0,0.0,"<= -2.0");
		lpcPLStream->schr(0.0,1.0);
	}
	
	if (i_cShell.GetNumElements() > 0)
		lpcPLStream->ptex(20.0,63.0,1.0,0.0,0.5,"log opacity");
	else
		lpcPLStream->ptex(19.0,63.0,1.0,0.0,0.5,"log opacity");
	{
		double	lpdX[2] = {18.0,21.0},lpdY[2] = {44.0,44.0};
		lpcPLStream->col0(15); // black
		lpcPLStream->lsty(2);
		lpcPLStream->line(2, lpdX, lpdY);
		lpdY[0] = lpdY[1] = 47.0;
		lpcPLStream->lsty(2);
		lpcPLStream->col0(7); // gray
		lpcPLStream->line(2, lpdX, lpdY);
		lpcPLStream->col0(15); // black
		lpcPLStream->schr(0.0,0.75);
		lpcPLStream->ptex(21.5,44.0,1.0,0.0,0.0,"Obs. HVF");
		lpcPLStream->ptex(21.5,47.0,1.0,0.0,0.0,"PS");
		lpcPLStream->schr(0.0,1.0);
	}

	// Draw observational constraints
	lpcPLStream->col0(15); // black
	lpcPLStream->lsty(2);
	lpcPLStream->line(TIME_GRID_DATA_POINTS, lpdDay, lpdObs_Const_2);
	lpcPLStream->line(TIME_GRID_DATA_POINTS, lpdDay, lpdObs_Const_3);

//	lpiOptions_Array[0]   = PL_LEGEND_LINE;
//	lpiLegend_Text_Colors[0] = 1;
//	lpiLegend_Line_Colors[0] = 1;
//	lpfLegend_ine_Widths[0] = 1.0;
//	lplpszLegend_Test[0]        = "PS LGR";
//	lpiLegend_Line_Styles[0] = 1;
 
//	lpiOptions_Array[1]   = PL_LEGEND_LINE;
//	lpiLegend_Text_Colors[1] = 9;
//	lpiLegend_Line_Colors[1] = 9;
//	lpfLegend_ine_Widths[1] = 1.0;
//	lplpszLegend_Test[1]        = "HVF LGR";
//	lpiLegend_Line_Styles[1] = 1;

//	lpiOptions_Array[0]   = PL_LEGEND_LINE;
//	lpiLegend_Text_Colors[0] = 15;
//	lpiLegend_Line_Colors[0] = 15;
//	lpfLegend_ine_Widths[0] = 1.0;
//	lplpszLegend_Test[0]        = "Obs. HVF         ";
//	lpiLegend_Line_Styles[0] = 2;

//	lpiOptions_Array[1]   = PL_LEGEND_LINE;
//	lpiLegend_Text_Colors[1] = 15;
//	lpiLegend_Line_Colors[1] = 7;
//	lpfLegend_ine_Widths[1] = 1.0;
//	lplpszLegend_Test[1]        = "PS       ";
//	lpiLegend_Line_Styles[1] = 1;


//	lpcPLStream->legend( &fLgnd_Width,&fLgnd_Height,
//		PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX, 0,
//		0.0, 0.0, 0.1, 
//		0, 15, 1, 0, 0,
//		2, lpiOptions_Array,
//		1.0, 1.0, 2.0,
//		0., lpiLegend_Text_Colors, (const char * const *) lplpszLegend_Test,
//		NULL, NULL, NULL, NULL,
//		lpiLegend_Line_Colors, lpiLegend_Line_Styles, lpfLegend_ine_Widths,
//		NULL, NULL, NULL, NULL );
	*/
//	printf("Freeing grid - shell\n");
//	fflush(stdout);
//	plFree2dGrid( lpdOpacity_Map_Shell, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	delete2d(lpdOpacity_Map_Shell,TIME_GRID_DATA_POINTS);
//	printf("Freeing grid - ejecta\n");
//	fflush(stdout);
//	plFree2dGrid( lpdOpacity_Map_Ejecta, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	delete2d(lpdOpacity_Map_Ejecta,TIME_GRID_DATA_POINTS);
//	printf("Freeing grid - combined\n");
//	fflush(stdout);
//	plFree2dGrid( lpdOpacity_Map_Combined, TIME_GRID_DATA_POINTS, VEL_GRID_DATA_POINTS );
	delete2d(lpdOpacity_Map_Combined,TIME_GRID_DATA_POINTS);
//	printf("Freeing grid - range\n");
//	fflush(stdout);
//	plFree2dGrid( lpdRange, 1, 257);
//	delete [] lpdRange;
//	printf("delete plot\n");
//	fflush(stdout);
//	printf("return\n");
//	fflush(stdout);
}


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	FILE * fileOut;
	unsigned int uiI;
	char lpszFilename[32];
	unsigned int uiNumber = atoi(i_lpszArg_Values[1]);
	XDATASET	cEjecta_Data, cShell_Data;

	sprintf(lpszFilename,"dens_prof_%04i.csv",uiNumber);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"file, component, vmin, vmax, rc,vc,nr,nv,variance (exp), variance(power r), variance(power v), density (11.5 kkm/s), density (17.5 kkm/s)\n");
	printf("file, component, vmin, vmax, rc,vc,nr,nv,variance (exp), variance(power r), variance(power v), density (11.5 kkm/s), density (17.5 kkm/s) \n");
	XFLASH_File	cFlash_File;
	sprintf(lpszFilename,"explos_hdf5_chk_%04i",uiNumber);
	cFlash_File.Open(lpszFilename);
	g_dTime_Ref = cFlash_File.m_dTime;
	FIT_RESULTS cEjecta = ProcessFile(fileOut,"ejecta",uiNumber,i_lpszArg_Values[2],cEjecta_Data);
	FIT_RESULTS cShell = ProcessFile(fileOut,"shell",uiNumber,i_lpszArg_Values[2],cShell_Data);
	fclose(fileOut);
//	printf("VE\n");
	Velocity_Evolution_Simulation(cEjecta_Data,cShell_Data,uiNumber,i_lpszArg_Values[2]);
//	printf("VEt\n");
	Velocity_Evolution_Theoretical(cEjecta,cShell,uiNumber,i_lpszArg_Values[2]);
}

