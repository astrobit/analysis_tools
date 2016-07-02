
#include <cmath>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <Plot_Utilities.h>
#include <cfloat>
#include <best_fit_data.h>
#include <line_routines.h>
#include <xfit.h>

void Plot(epsplot::PAGE_PARAMETERS	& i_cPlot_Parameters, epsplot::DATA &i_cPlot, const char * i_lpszFilename_Format, const char * i_lpszOutput_File_Prefix, const ES::Spectrum & i_cTarget, const ES::Spectrum & i_cFit, const ES::Spectrum & i_cFit_noPS, const ES::Spectrum & i_cFit_noHVF, const double & i_dTarget_Normalization_Flux, const double & i_dFit_Normalization_Flux,const double &i_dPlot_Min_WL, const double & i_dPlot_Max_WL,bool i_bNo_Shell,  unsigned int i_uiX_Axis, unsigned int i_uiY_Axis)
{
	epsplot::LINE_PARAMETERS	cLine_Parameters;
	// Make sure that both spectra are within the desired plot range, otherwise don't bother plotting.
	if (((i_cTarget.wl(0) >= i_dPlot_Min_WL && i_cTarget.wl(0) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(i_cTarget.size() - 1)>= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(0) <= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) >= i_dPlot_Max_WL)) &&
	   ((i_cFit.wl(0) >= i_dPlot_Min_WL && i_cFit.wl(0) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(i_cFit.size() - 1)>= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(0) <= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) >= i_dPlot_Max_WL)))
	{
		double * lpdTarget_WL = NULL, *lpdTarget_Flux = NULL, *lpdFit_WL = NULL, *lpdFit_Flux = NULL, *lpdFit_noHVF_WL = NULL, * lpdFit_noHVF_Flux = NULL, *lpdFit_noPS_WL = NULL, *lpdFit_noPS_Flux = NULL;
		unsigned int uiNum_Points_Target, uiNum_Points_Fit, uiNum_Points_Fit_noPS, uiNum_Points_Fit_noHVF;
		char lpszFilename[256];

		// Get spectral data in the desired range
		Get_Spectra_Data(i_cTarget, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, i_dPlot_Min_WL, i_dPlot_Max_WL);
		Get_Spectra_Data(i_cFit, lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, i_dPlot_Min_WL, i_dPlot_Max_WL);
		if (!i_bNo_Shell)
		{
			Get_Spectra_Data(i_cFit_noHVF, lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, i_dPlot_Min_WL, i_dPlot_Max_WL);
			Get_Spectra_Data(i_cFit_noPS, lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, i_dPlot_Min_WL, i_dPlot_Max_WL);
		}
		// normalize spectra
//		printf("%.2e\t%.2e\n",i_dTarget_Normalization_Flux,i_dFit_Normalization_Flux);
		Normalize(lpdTarget_Flux, uiNum_Points_Target,i_dTarget_Normalization_Flux);
		Normalize(lpdFit_Flux, uiNum_Points_Fit, i_dFit_Normalization_Flux);
		if (!i_bNo_Shell)
		{
			Normalize(lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF,i_dFit_Normalization_Flux);
			Normalize(lpdFit_noPS_Flux, uiNum_Points_Fit_noPS,i_dFit_Normalization_Flux);
		}
		sprintf(lpszFilename,i_lpszFilename_Format,i_lpszOutput_File_Prefix);
		i_cPlot.Set_Plot_Filename(lpszFilename);

		cLine_Parameters.m_eColor = epsplot::BLACK;
		cLine_Parameters.m_eStipple = epsplot::SOLID;

		i_cPlot.Clear_Plots();
		i_cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, cLine_Parameters, i_uiX_Axis, i_uiY_Axis);
		cLine_Parameters.m_eColor = epsplot::RED;
		i_cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, cLine_Parameters, i_uiX_Axis, i_uiY_Axis);
		if (!i_bNo_Shell)
		{
			cLine_Parameters.m_eStipple = epsplot::STPL_CUSTOM_1;
			i_cPlot.Set_Plot_Data(lpdFit_noHVF_WL, lpdFit_noHVF_Flux, uiNum_Points_Fit_noHVF, cLine_Parameters, i_uiX_Axis, i_uiY_Axis);
			cLine_Parameters.m_eColor = epsplot::BLUE;
			i_cPlot.Set_Plot_Data(lpdFit_noPS_WL, lpdFit_noPS_Flux, uiNum_Points_Fit_noPS, cLine_Parameters, i_uiX_Axis, i_uiY_Axis);
		}
		i_cPlot.Plot(i_cPlot_Parameters);

		delete [] lpdTarget_WL;
		delete [] lpdTarget_Flux;
		delete [] lpdFit_WL;
		delete [] lpdFit_Flux;
		if (!i_bNo_Shell)
		{
			delete [] lpdFit_noHVF_WL;
			delete [] lpdFit_noHVF_Flux;
			delete [] lpdFit_noPS_WL;
			delete [] lpdFit_noPS_Flux;
		}
	}
	else
	{
		printf("No plot: %.1f %.1f %.1f %.1f %.1f %.1f\n",i_dPlot_Min_WL, i_dPlot_Max_WL,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1), i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1));
	}
}


double Pereira_Luminosity_SN2011fe(const double & i_dDate_MJD5455)
{
	double dLum;// = nan("");
	const double i_dLuminosities[] ={0.36e42,0.71e42,1.19e42,1.91e42,2.84e42,3.91e42,5.42e42,6.25e42,7.86e42,8.87e42,10.27e42,11.77e42,11.35e42,10.93e42,10.41e42,9.64e42,8.13e42,7.12e42,5.84e42,4.99e42,4.11e42,3.67e42,3.43e42,3.14e42};
	const double i_dMJD[] = {799.3,800.2,801.2,802.3,803.2,804.2,805.2,806.2,807.3,808.2,809.2,813.2,814.2,815.2,816.2,817.2,818.2,821.2,823.2,826.2,828.2,831.2,833.2,836.2,838.2};
	if (i_dDate_MJD5455 <= i_dMJD[0])
	{	// values from power law fit performed to Pereira luminosity data for first 6 days; Mulligan & Wheeler in prep (Jan 2015)
		double	dDay0 = 796.155540966755552;
		double	dPower = 2.53158900546870713;
		double	dScalar = 1.99107577414679403e+40;// erg/s
		dLum = pow(i_dDate_MJD5455 - dDay0,dPower) * dScalar;
	}
	else
	{
		unsigned int uiNum_Pairs = sizeof(i_dLuminosities) / sizeof(double);
		XSPLINE_DATA	cLuminosity_Spline;
		cLuminosity_Spline.Initialize(i_dMJD,i_dLuminosities,uiNum_Pairs);
		if (i_dDate_MJD5455 > i_dMJD[uiNum_Pairs - 1])
			fprintf(stderr,"Warning: Pereira Luminosity Estimator Called with date %f. Data only available through date (5455)838.\n",i_dDate_MJD5455);
		else
		{
			unsigned int uiI = 0;
			while(uiI < uiNum_Pairs && i_dDate_MJD5455 > i_dMJD[uiI])
				uiI++;
			dLum = cLuminosity_Spline.Interpolate(i_dDate_MJD5455);
//			double dX = (i_dDate_MJD5455 - i_dMJD[uiI- 1]) / (i_dMJD[uiI] - i_dMJD[uiI - 1]);
//			printf("%e\t%e\t%f\t%f\t%f\n",i_dLuminosities[uiI - 1], i_dLuminosities[uiI],i_dMJD[uiI - 1], i_dMJD[uiI],dX);
//			dLum = i_dLuminosities[uiI - 1] + (i_dLuminosities[uiI] - i_dLuminosities[uiI - 1]) * dX;
		}
	}
	return dLum;
}
double Photosphere_Temp(const double & i_dDate_MJD, const double & i_dDays_After_Explosion, const double & i_dPhotosphere_Vel_kkm_s, const double & i_dGreybody_Factor = 0.5)
{
	const double dSB_Const = 5.670373e-5;
	double dPi = acos(-1.0);
	double	dLuminosity = Pereira_Luminosity_SN2011fe(i_dDate_MJD);
	double	dPS_Radius = i_dDays_After_Explosion * 86400.0 * i_dPhotosphere_Vel_kkm_s * 1e8;
	double	dTemp_kK = pow(dLuminosity / (4.0 * dPi * dPS_Radius * dPS_Radius * dSB_Const * i_dGreybody_Factor),0.25) * 1e-3;
	return dTemp_kK;
}
double Photosphere_Day(const double & i_dDate_MJD, const double & i_dPhotosphere_Temp_kK, const double & i_dPhotosphere_Vel_kkm_s, const double & i_dGreybody_Factor = 0.5)
{
	const double dSB_Const = 5.670373e-5;
	double dPi = acos(-1.0);
	double	dLuminosity = Pereira_Luminosity_SN2011fe(i_dDate_MJD);
	double	dPS_Radius = sqrt(dLuminosity / (4.0 * dPi * pow(i_dPhotosphere_Temp_kK * 1e3,4.0) * dSB_Const * i_dGreybody_Factor));
	double	dDay = dPS_Radius / (86400.0 * i_dPhotosphere_Vel_kkm_s * 1e8);
	return dDay;
}

void Restrict_Bound(const XVECTOR & i_vCurrent, XVECTOR & io_vNext, unsigned int uiI, const double &dUpper_Bound, const double & dLower_Bound)
{
	XVECTOR vDelta(i_vCurrent.Get_Size());

	vDelta = io_vNext - i_vCurrent; // get differnce vector
//	printf("Eff Range(t) %.2e\n",vDelta.Magnitude());
	if (io_vNext.Get(uiI) > dUpper_Bound)
	{
		if (i_vCurrent.Get(uiI) == dUpper_Bound)
		{
//			vDelta.Set(uiI,vDelta.Get(uiI) * -1.0 ); // reflection
			vDelta.Set(uiI,0.0 ); // barrier
		}
		else
		{
			double dScalar = (dUpper_Bound - i_vCurrent.Get(uiI)) / vDelta.Get(uiI); // current delta vs maximum delta
			vDelta *= dScalar;
//			printf("Scalar(u) %.2e\n",dScalar);
		}
		io_vNext = i_vCurrent + vDelta;
//		printf("Eff Range(u) %.2e\n",vDelta.Magnitude());
	}
	if (io_vNext.Get(uiI) < dLower_Bound)
	{
		if (i_vCurrent.Get(uiI) == dLower_Bound) // special case
		{
///			vDelta.Set(uiI,vDelta.Get(uiI) * -1.0 ); // reflection
			vDelta.Set(uiI,0.0 ); // barrier
		}
		else
		{
			if (vDelta.Get(uiI) != 0.0 && !std::isinf(i_vCurrent.Get(uiI)) && !std::isinf(vDelta.Get(uiI)) && !std::isnan(i_vCurrent.Get(uiI)) && !std::isnan(vDelta.Get(uiI)))
			{
				double dScalar = (dLower_Bound - i_vCurrent.Get(uiI)) / vDelta.Get(uiI); // current delta vs maximum delta
//				printf("Scalar(l)%i %.2e\n",uiI,dScalar);
				vDelta *= dScalar;
			}
			else
				printf("Invalid scalar at %i: %.2f %.2f %.2f\n",uiI,dLower_Bound,i_vCurrent.Get(uiI),vDelta.Get(uiI));
		}
		io_vNext = i_vCurrent + vDelta;
	}
//	io_vNext.Print();
//	printf("Eff Range %.2e\n",vDelta.Magnitude());
}

/*void rsrand(void)
{
	unsigned int uiSeed;
	FILE* urandom = fopen("/dev/urandom", "r");
	fread(&uiSeed, sizeof(uiSeed), 1, urandom);
	fclose(urandom);
	srand(uiSeed);
}
double	Rand(void)
{
	double	dVal = rand() / (RAND_MAX * 1.0);
	if (dVal > 1.0)
		printf("Rand > 1\n");
	return dVal;
}*/

XVECTOR Generate_Vector_From_Best_Fit(const BEST_FIT_DATA & i_cFit_Data, const XDATASET & i_cOpacity_Map, bool i_bUse_PS_Temp_For_Ion)
{
	XVECTOR vRet;
	if (!std::isnan(i_cFit_Data.dBest_Fit_PS_Ion_Aux))
	{ // esponential or power law profile
		if (!std::isnan(i_cFit_Data.dBest_Fit_HVF_Ion_Aux))
		{ // include HVF
			vRet.Set_Size(15);
		}
		else
			vRet.Set_Size(10);
		//@@TODO
	}
	else
	{
		if (!std::isnan(i_cFit_Data.dBest_Fit_HVF_Log_Tau))
		{
			vRet.Set_Size(7);
			if (i_bUse_PS_Temp_For_Ion)
				vRet.Set(5,i_cFit_Data.dBest_Fit_PS_Temp);
			else
				vRet.Set(5,i_cFit_Data.dBest_Fit_HVF_Ion_Temp);
			vRet.Set(6,i_cFit_Data.dBest_Fit_HVF_Log_Tau);
		}
		else
			vRet.Set_Size(5);
//		unsigned int uiI = 1;
//		while (uiI < i_cOpacity_Map.GetNumElements() && i_cOpacity_Map.GetElement(uiI,0) < i_cFit_Data.dBest_Fit_Day)
//			uiI++;
//		if (uiI > 0)
//			uiI--;

		vRet.Set(0,i_cFit_Data.dBest_Fit_Day);
		vRet.Set(1,i_cFit_Data.dBest_Fit_PS_Vel);
		vRet.Set(2,i_cFit_Data.dBest_Fit_PS_Temp);
		if (i_bUse_PS_Temp_For_Ion)
			vRet.Set(3,i_cFit_Data.dBest_Fit_PS_Temp);
		else
			vRet.Set(3,i_cFit_Data.dBest_Fit_PS_Ion_Temp);
		vRet.Set(4,i_cFit_Data.dBest_Fit_PS_Log_Tau);
	}
	return vRet;
}
void Get_Fit_Data_From_Vector(const XVECTOR & i_vParameters, BEST_FIT_DATA & o_cFit_Data, const XDATASET & i_cOpacity_Map, bool i_bUse_PS_Temp_For_Ion)
{
	int iT_ref;
	switch (i_vParameters.Get_Size())
	{
	case 15:
	case 10:
		//@@TODO
		break;
	case 7:
		if (i_bUse_PS_Temp_For_Ion)
			o_cFit_Data.dBest_Fit_HVF_Ion_Temp = i_vParameters.Get(2);
		else
			o_cFit_Data.dBest_Fit_HVF_Ion_Temp = i_vParameters.Get(5);
		o_cFit_Data.dBest_Fit_HVF_Log_Tau = i_vParameters.Get(6);
	case 5:
//		iT_ref = (int )(i_vParameters.Get(0) + 0.5);
//		if (iT_ref < 0)
//			iT_ref = 0;
//		if (iT_ref > (i_cOpacity_Map.GetNumColumns() - 2))
//			iT_ref = i_cOpacity_Map.GetNumColumns() - 2;
//		o_cFit_Data.dBest_Fit_Day =  i_cOpacity_Map.GetElement(iT_ref + 1,0);
		o_cFit_Data.dBest_Fit_Day =  i_vParameters.Get(0);// * (i_cOpacity_Map.GetElement(2,0) - i_cOpacity_Map.GetElement(1,0)) + i_cOpacity_Map.GetElement(1,0);
		o_cFit_Data.dBest_Fit_PS_Vel = i_vParameters.Get(1);
		o_cFit_Data.dBest_Fit_PS_Temp= i_vParameters.Get(2);
		if (i_bUse_PS_Temp_For_Ion)
			o_cFit_Data.dBest_Fit_PS_Ion_Temp = i_vParameters.Get(2);
		else
			o_cFit_Data.dBest_Fit_PS_Ion_Temp = i_vParameters.Get(3);
		o_cFit_Data.dBest_Fit_PS_Log_Tau = i_vParameters.Get(4);
		break;
	}
}


#define FIT_BLUE_WL 4500.0
#define FIT_RED_WL 9000.0
void Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, double & o_dTarget_Flux, double & o_dGenerated_Flux)
{
//	double dMax_WL;
//	Find_Flux_Global_Maximum(i_cTarget,dMax_WL,o_dTarget_Flux, i_dMin_WL, i_dMax_WL);
//	Find_Flux_At_WL(i_cGenerated, dMax_WL, o_dGenerated_Flux);
//	Find_Flux_At_WL(i_cTarget, 6250.0, o_dTarget_Flux);
//	Find_Flux_At_WL(i_cGenerated, 6250.0, o_dGenerated_Flux);
//	double dSi6355_Min_Wl,dSi6355_Min_Flux;
//	double dSi6355_Red_Max_Wl,dSi6355_Red_Max_Flux;

//	Find_Flux_Minimum(i_cTarget,dSi6355_Min_Wl,dSi6355_Min_Flux, 6000.0,6500.0);
//	Find_Red_Maxima(i_cTarget,dSi6355_Red_Max_Wl,dSi6355_Red_Max_Flux, 6000.0,6500.0);

//	unsigned int uiI = 0;
//	double dTarget = (dSi6355_Red_Max_Flux - dSi6355_Min_Flux) * 0.9 + dSi6355_Min_Flux;
//	while (uiI < i_cTarget.size() && i_cTarget.wl(uiI) < dSi6355_Min_Wl)
//		uiI++;
//	while (uiI < i_cTarget.size() && i_cTarget.flux(uiI) < dTarget)
//		uiI++;
//	Find_Flux_At_WL(i_cTarget, i_cTarget.wl(uiI), o_dTarget_Flux);
//	Find_Flux_At_WL(i_cGenerated, i_cTarget.wl(uiI), o_dGenerated_Flux);

//	o_dGenerated_Flux = i_cGenerated.flux(0);
//	o_dTarget_Flux = i_cTarget.flux(0);
//	double dNorm = o_dTarget_Flux / o_dGenerated_Flux;
//	for (unsigned int uiI = 1;  uiI < i_cTarget.size(); uiI++)
//	{
//		double	dGen_Flux = i_cGenerated.flux(uiI) * dNorm;
//		if (dGen_Flux < i_cTarget.flux(uiI))
//		{
//			o_dGenerated_Flux = i_cGenerated.flux(uiI);
//			o_dTarget_Flux = i_cTarget.flux(uiI);
//			dNorm = o_dTarget_Flux / o_dGenerated_Flux;
//		}
//	}

	unsigned int uiIdx = 0;
	double	dLuminosity_Target = 0.0, dLuminosity_Synth = 0.0;
	double	dFit_Delta_Lambda;
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_BLUE_WL)
		uiIdx++;
	dFit_Delta_Lambda = -i_cTarget.wl(uiIdx);

	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_RED_WL)
	{
		double dDelta_Lambda;
		if (uiIdx > 0 && uiIdx  < (i_cTarget.size() - 1))
			dDelta_Lambda = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx - 1)) * 0.5;
		else if (uiIdx == 0)
			dDelta_Lambda = i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx);
		else if (uiIdx == (i_cTarget.size() - 1))
			dDelta_Lambda = i_cTarget.wl(uiIdx) - i_cTarget.wl(uiIdx - 1);

		dLuminosity_Target += i_cTarget.flux(uiIdx) * dDelta_Lambda;
		dLuminosity_Synth += i_cGenerated.flux(uiIdx) * dDelta_Lambda;
		uiIdx++;

	}
	uiIdx--;
	dFit_Delta_Lambda += i_cTarget.wl(uiIdx);

	dLuminosity_Target /= dFit_Delta_Lambda; 
	dLuminosity_Synth /= dFit_Delta_Lambda; 
//	double dSynth_Normalization = dLuminosity_Synth / dLuminosity_Target;
	o_dTarget_Flux = dLuminosity_Target * 0.5;// * 1.1;
	o_dGenerated_Flux = dLuminosity_Synth * 0.5; //dSynth_Normalization;

//	o_dTarget_Flux = i_cTarget.flux(uiIdx);

//	double dSynth_Normalization = dLuminosity_Synth / dLuminosity_Target;
//	o_dTarget_Flux = 1.0;
//	o_dGenerated_Flux = dSynth_Normalization * o_dTarget_Flux;
}

double	g_dTarget_Normalization_Flux = -1;
double	g_dGenerated_Normalization_Flux = -1;

double	Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiMoment, bool i_bRegenerate_Normalization = true)
{
	double * lpdRaw_Moments = NULL, *lpdCentral_Moments = NULL, *lpdStandardized_Moments = NULL;
	double	dRet;
	if (i_bRegenerate_Normalization)
		Get_Normalization_Fluxes(i_cTarget, i_cGenerated, FIT_BLUE_WL, FIT_RED_WL, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
//		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
//	{
//		if (i_cTarget.wl(uiI) == 0.0 || std::isnan(i_cTarget.flux(uiI)) || std::isnan(i_cGenerated.flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",i_cTarget.wl(uiI),i_cTarget.flux(uiI),i_cGenerated.flux(uiI));
//		}
//	}

	Get_Fit_Moments_2(i_cTarget, i_cGenerated, i_dMin_WL, i_dMax_WL, i_uiMoment, lpdRaw_Moments, lpdCentral_Moments, lpdStandardized_Moments, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	dRet = fabs(lpdRaw_Moments[i_uiMoment - 1]);
	delete [] lpdRaw_Moments;
	delete [] lpdCentral_Moments;
	delete [] lpdStandardized_Moments;
	return dRet; // use mean
}

double Continuum_Fit(const ES::Spectrum &i_cTarget)
{
	ES::Spectrum cOutput(i_cTarget);
	XVECTOR cResult(7);
	unsigned int uiParameters = cResult.Get_Size();
	XVECTOR vBest_Fit(uiParameters);
	double dBest_Fit = DBL_MAX;
//	printf("\n");
//	cResult.Print();

	cResult.Set(0,10.0);//ps vel
	cResult.Set(1,11.0); // ps temp
	cResult.Set(2,-10.0); // PS ion log tau
	cResult.Set(3,10.0); // PS ion exctication temp
	cResult.Set(4,10.0); // PS ion vmin
	cResult.Set(5,11.0); // PS ion vmax
	cResult.Set(6,1.0); // PS ion vscale

	for (double dTemp = 7.5; dTemp < 15.0; dTemp += 0.1)
	{
		cResult.Set(1,dTemp); // set PS LGR temp to PS temp
		fprintf(stdout,"Trying %.1f\t",cResult.Get(1));
		fflush(stdout);
		Generate_Synow_Spectra_Exp(i_cTarget,2001,cResult,cOutput); // ion irrelevant for this 
		double dFit = Get_Fit(i_cTarget,cOutput,FIT_BLUE_WL,FIT_RED_WL,2,true);
		printf("Fit = %.2e",dFit);
		if (dFit < dBest_Fit)
		{
			dBest_Fit = dFit;
			printf("\tBF");
			vBest_Fit = cResult;
		}
		printf("\n");
	}
	Generate_Synow_Spectra_Exp(i_cTarget,2001,vBest_Fit,cOutput); // ion irrelevant for this 
	double dFit = Get_Fit(i_cTarget,cOutput,FIT_BLUE_WL,FIT_RED_WL,2,true);
	return vBest_Fit.Get(1);
}

class FIT_DATA_CONTAINER
{
public:
	const ES::Spectrum * lpTarget;
	ES::Spectrum * lpOutput;
	XDATASET * lpOpacity_Map_Ejecta;
	XDATASET * lpOpacity_Map_Shell;
	unsigned int uiIon;
	double	dFit_WL_Blue;
	double	dFit_WL_Red;

	bool bAllow_Temp_Fit;
	bool bDisallow_Scalar_Fit;
	bool bDisallow_Day_Fit;

	double	dMJD;
	double	dPS_Scalar;
	double	dHVF_Scalar;

	double	dSet_Excitation_Temp_HVF;
	double	dSet_Excitation_Temp_PVF;
	double	dSet_Day;

};
enum CONVERT_METHOD {FIXED_DAY,FIXED_TEMP,SET_DAY};

void Convert_Parameter_To_Full_Vector(const XVECTOR & i_vIn, const FIT_DATA_CONTAINER & i_cFit_Data, XVECTOR & o_vOut, CONVERT_METHOD	i_eMethod)
{
	o_vOut.Set_Size(7);
	double	dPS_Temp;
	double	dDay;
	double	dPS_Vel = i_vIn.Get(1);
	double	dScalar = 1.0;
	switch (i_eMethod)
	{
	case FIXED_DAY:
		dDay = i_vIn.Get(0);
		dPS_Temp = Photosphere_Temp(i_cFit_Data.dMJD, dDay, dPS_Vel);
		break;
	case FIXED_TEMP:
	default:
		dPS_Temp = i_vIn.Get(0);
		dDay = i_cFit_Data.dSet_Day;//Photosphere_Day(i_cFit_Data.dMJD, dPS_Temp, dPS_Vel);
		break;
	case SET_DAY:
		dPS_Temp = i_vIn.Get(0);
		dDay = Photosphere_Day(i_cFit_Data.dMJD, dPS_Temp, dPS_Vel);
		dScalar = log10(pow(dDay / i_cFit_Data.dSet_Day,3.0));
		break;
	}

	bool bShell = (i_cFit_Data.lpOpacity_Map_Shell->GetNumElements() > 0);
		
	o_vOut.Set(0,dDay);
	o_vOut.Set(1,dPS_Vel);
	o_vOut.Set(2,dPS_Temp);

	if (i_cFit_Data.bAllow_Temp_Fit && !i_cFit_Data.bDisallow_Scalar_Fit)
	{
		o_vOut.Set(3,i_vIn.Get(2));
		o_vOut.Set(4,i_vIn.Get(3));
		if (bShell)
		{
			o_vOut.Set(5,i_vIn.Get(4));
			o_vOut.Set(6,i_vIn.Get(5));
		}
	}
	else if (i_cFit_Data.bAllow_Temp_Fit)
	{
		o_vOut.Set(3,i_vIn.Get(2));
		o_vOut.Set(4,i_cFit_Data.dPS_Scalar + dScalar);
		if (bShell)
		{
			o_vOut.Set(5,i_vIn.Get(3));
			o_vOut.Set(6,i_cFit_Data.dHVF_Scalar + dScalar);
		}
	}
	else if (!i_cFit_Data.bDisallow_Scalar_Fit)
	{
		o_vOut.Set(3,i_cFit_Data.dSet_Excitation_Temp_PVF);
		o_vOut.Set(4,i_vIn.Get(2) + dScalar);
		if (bShell)
		{
			o_vOut.Set(5,i_cFit_Data.dSet_Excitation_Temp_HVF);
			o_vOut.Set(6,i_vIn.Get(3) + dScalar);
		}
	}
	else
	{
		o_vOut.Set(3,i_cFit_Data.dSet_Excitation_Temp_PVF);
		o_vOut.Set(4,i_cFit_Data.dPS_Scalar + dScalar);
		if (bShell)
		{
			o_vOut.Set(5,i_cFit_Data.dSet_Excitation_Temp_HVF);
			o_vOut.Set(6,i_cFit_Data.dHVF_Scalar + dScalar);
		}
	}	
}

void Convert_Parameter_To_Best_Fit_Data(const XVECTOR & i_vIn, const FIT_DATA_CONTAINER & i_cFit_Data, BEST_FIT_DATA & o_cBest_Fit, CONVERT_METHOD	i_eMethod)
{
	double	dPS_Temp;
	double	dDay;
	double	dPS_Vel = i_vIn.Get(1);
	double	dScalar = 1.0;
	switch (i_eMethod)
	{
	case FIXED_DAY:
		dDay = i_vIn.Get(0);
		dPS_Temp = Photosphere_Temp(i_cFit_Data.dMJD, dDay, dPS_Vel);
		dScalar = 1.0;
		break;
	case FIXED_TEMP:
	default:
		dPS_Temp = i_vIn.Get(0);
		dDay = i_cFit_Data.dSet_Day;//Photosphere_Day(i_cFit_Data.dMJD, dPS_Temp, dPS_Vel);
		//dScalar = log10(pow(dDay / i_cFit_Data.dSet_Day,3.0));
		break;
	case SET_DAY:
		dPS_Temp = i_vIn.Get(0);
		dDay = Photosphere_Day(i_cFit_Data.dMJD, dPS_Temp, dPS_Vel);
		// need to scale scalars
		dScalar = log10(pow(dDay / i_cFit_Data.dSet_Day,3.0));
		break;
	}

	bool bShell = (i_cFit_Data.lpOpacity_Map_Shell->GetNumElements() > 0);
		
	o_cBest_Fit.dBest_Fit_Day = dDay;
	o_cBest_Fit.dBest_Fit_PS_Temp = dPS_Temp;
	o_cBest_Fit.dBest_Fit_PS_Vel = dPS_Vel;

	if (i_cFit_Data.bAllow_Temp_Fit && !i_cFit_Data.bDisallow_Scalar_Fit)
	{
		o_cBest_Fit.dBest_Fit_PS_Ion_Temp = i_vIn.Get(2);
		o_cBest_Fit.dBest_Fit_PS_Log_Tau = i_vIn.Get(3) + dScalar;
		if (bShell)
		{
			o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = i_vIn.Get(4);
			o_cBest_Fit.dBest_Fit_HVF_Log_Tau = i_vIn.Get(5) + dScalar;
		}
	}
	else if (i_cFit_Data.bAllow_Temp_Fit)
	{
		o_cBest_Fit.dBest_Fit_PS_Ion_Temp = i_vIn.Get(2);
		o_cBest_Fit.dBest_Fit_PS_Log_Tau = i_cFit_Data.dPS_Scalar + dScalar;
		if (bShell)
		{
			o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = i_vIn.Get(3);
			o_cBest_Fit.dBest_Fit_HVF_Log_Tau = i_cFit_Data.dHVF_Scalar + dScalar;
		}
	}
	else if (!i_cFit_Data.bDisallow_Scalar_Fit)
	{
		o_cBest_Fit.dBest_Fit_PS_Ion_Temp = i_cFit_Data.dSet_Excitation_Temp_PVF;
		o_cBest_Fit.dBest_Fit_PS_Log_Tau = i_vIn.Get(2) + dScalar;
		if (bShell)
		{
			o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = i_cFit_Data.dSet_Excitation_Temp_HVF;
			o_cBest_Fit.dBest_Fit_HVF_Log_Tau = i_vIn.Get(3) + dScalar;
		}
	}
	else
	{
		o_cBest_Fit.dBest_Fit_PS_Ion_Temp = i_cFit_Data.dSet_Excitation_Temp_PVF;
		o_cBest_Fit.dBest_Fit_PS_Log_Tau = i_cFit_Data.dPS_Scalar + dScalar;
		if (bShell)
		{
			o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = i_cFit_Data.dSet_Excitation_Temp_HVF;
			o_cBest_Fit.dBest_Fit_HVF_Log_Tau = i_cFit_Data.dHVF_Scalar + dScalar;
		}
	}	
	if (!bShell)
	{
		o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = i_cFit_Data.dSet_Excitation_Temp_HVF;
		o_cBest_Fit.dBest_Fit_HVF_Log_Tau = -20.0;
	}
}

double Fit_Function(const XVECTOR &i_vX, void * i_lpvData)
{
	XVECTOR	vX;
	XVECTOR	vY;
	FIT_DATA_CONTAINER * lpcData = (FIT_DATA_CONTAINER*)i_lpvData;
	Convert_Parameter_To_Full_Vector(i_vX,lpcData[0],vX,FIXED_TEMP);
//	vY = vX;
//	vY.Set(4,-20.0);
//	vY.Set(6,-20.0);
	// first generate a blackbody spectrum
//	Generate_Synow_Spectra(lpcData->lpTarget[0], lpcData->lpOpacity_Map_Ejecta[0], lpcData->lpOpacity_Map_Shell[0],lpcData->uiIon, vY, lpcData->lpOutput[0]);
//	double dFit = Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], lpcData->dFit_WL_Blue, lpcData->dFit_WL_Red,2,true);
//	vX.Print();
	// now generate actual spectrum
	Generate_Synow_Spectra(lpcData->lpTarget[0], lpcData->lpOpacity_Map_Ejecta[0], lpcData->lpOpacity_Map_Shell[0],lpcData->uiIon, vX, lpcData->lpOutput[0]);

//	for (unsigned int uiI = 0; uiI < lpcData->lpTarget[0].size(); uiI++)
//	{
//		if (std::isnan(lpcData->lpTarget[0].flux(uiI)) || std::isnan(lpcData->lpOutput[0].flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",lpcData->lpTarget[0].wl(uiI),lpcData->lpTarget[0].flux(uiI),lpcData->lpOutput[0].flux(uiI));
//		}
//	}
	//dFit = Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], 6750.0, 7250.0,2,false);
	//dFit += Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], lpcData->dFit_WL_Blue, lpcData->dFit_WL_Red,2,false);
	double dFit = Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], lpcData->dFit_WL_Blue, lpcData->dFit_WL_Red,2,true);
//	printf("\t%f",dFit);

	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Flux", false, false, false, 5.0, false, 60.0);
//	cPlot.Set_Num_Plots(2); // Obsolete

	cPlot.Set_Plot_Title(NULL);

	Plot(cPlot_Parameters, cPlot, "%s","test.CaNIR.eps", lpcData->lpTarget[0], lpcData->lpOutput[0],  lpcData->lpOutput[0], lpcData->lpOutput[0],  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,7000.0, 9000.0,true, uiX_Axis, uiY_Axis);
	Plot(cPlot_Parameters, cPlot, "%s","test.full.eps", lpcData->lpTarget[0], lpcData->lpOutput[0],  lpcData->lpOutput[0], lpcData->lpOutput[0],  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,lpcData->lpTarget[0].wl(0), lpcData->lpTarget[0].wl(lpcData->lpTarget[0].size()- 1),true, uiX_Axis, uiY_Axis);

	return dFit;
}

/*double Fit_Function_Degeneracies(double & i_dDay, const XVECTOR &i_vX, void * i_lpvData)
{
	XVECTOR	vX;
	XVECTOR	vY;
	FIT_DATA_CONTAINER * lpcData = (FIT_DATA_CONTAINER*)i_lpvData;
	Convert_Parameter_To_Full_Vector(i_vX,lpcData[0],vX,FIXED_TEMP);
	double	dPS_Temp = i_vX.Get(0);
	double	dPS_Vel = i_vX.Get(1);;
	double	dDay = Photosphere_Day(i_cFit_Data.dMJD, dPS_Temp, dPS_Vel);
	double	dDay_Scalar = dPS_Vel / i_dDay;

	double dV = dDay * dPS_Vel / i_dDay;
	double dSpvf = vX.Get(4) - log10(dDay_Scalar * dDay_Scalar);
	double dShvf = vX.Get(6) - log10(dDay_Scalar * dDay_Scalar);
	vY = vX;
	vY.Set(0,i_dDay);
	vY.Set(1,dV);
	vY.Set(4,-20.0);
	vY.Set(6,-20.0);
	// first generate a blackbody spectrum
	Generate_Synow_Spectra(lpcData->lpTarget[0], lpcData->lpOpacity_Map_Ejecta[0], lpcData->lpOpacity_Map_Shell[0],lpcData->uiIon, vY, lpcData->lpOutput[0]);
	// normalize the spectrum.
	double dFit = Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], lpcData->dFit_WL_Blue, lpcData->dFit_WL_Red,2,true);
	vY.Set(4,dSpvf);
	vY.Set(6,dShvf);
//	vX.Print();
	// now generate actual spectrum
	Generate_Synow_Spectra(lpcData->lpTarget[0], lpcData->lpOpacity_Map_Ejecta[0], lpcData->lpOpacity_Map_Shell[0],lpcData->uiIon, vY, lpcData->lpOutput[0]);

//	for (unsigned int uiI = 0; uiI < lpcData->lpTarget[0].size(); uiI++)
//	{
//		if (std::isnan(lpcData->lpTarget[0].flux(uiI)) || std::isnan(lpcData->lpOutput[0].flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",lpcData->lpTarget[0].wl(uiI),lpcData->lpTarget[0].flux(uiI),lpcData->lpOutput[0].flux(uiI));
//		}
//	}
	dFit = Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], 6750.0, 7250.0,2,false);
//	printf("\t%f",dFit);
	dFit += Get_Fit(lpcData->lpTarget[0], lpcData->lpOutput[0], lpcData->dFit_WL_Blue, lpcData->dFit_WL_Red,2,false);

	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	cPlot.Set_Num_X_Axis_Types(1);
	cPlot.Set_Num_Y_Axis_Types(1);
	cPlot.Set_X_Axis_Parameters(0, "Wavelength [A]", false, false, false, 0.0, false, 0.0);
	cPlot.Set_Y_Axis_Parameters(0, "Flux", false, false, false, 5.0, false, 60.0);
	cPlot.Set_Num_Plots(2);

	cPlot.Set_Plot_Title(NULL);

	Plot(cPlot_Parameters, cPlot, "%s","test.CaNIR.eps", lpcData->lpTarget[0], lpcData->lpOutput[0],  lpcData->lpOutput[0], lpcData->lpOutput[0],  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,7000.0, 9000.0,true);
	Plot(cPlot_Parameters, cPlot, "%s","test.full.eps", lpcData->lpTarget[0], lpcData->lpOutput[0],  lpcData->lpOutput[0], lpcData->lpOutput[0],  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,lpcData->lpTarget[0].wl(0), lpcData->lpTarget[0].wl(lpcData->lpTarget[0].size()- 1),true);

	return dFit;
}

double	Refine_Day_Fit(const XVECTOR & i_vStarting_Point, FIT_DATA_CONTAINER &cFit_data, const double & i_dStart, const double & i_dEnd, const double & i_dIncrement)
{
	double	dBest_Fit_Day;
	double dBest_Fit = DBL_MAX;
	for (double dDay = i_dStart; dDay <= i_dEnd; dDay += i_dIncrement)
	{
		printf("Trying Day %.3f",dDay);
		double dFit = Fit_Function_Degeneracies(dDay, i_vStarting_Point, &cFit_data);
		printf("\t%.3e",dFit);
		if (dBest_Fit >= dFit)
		{
			dBest_Fit = dFit;
			dBest_Fit_Day = dDay;
			printf("\tB");
		}
		printf("\n");
	}
	return dBest_Fit_Day;
}*/


void CompareFits(const ES::Spectrum &i_cTarget, const double & i_dDate_MJD, XDATASET & i_cOpacity_Map_A, XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const double & i_dFit_WL_Blue, const double & i_dFit_WL_Red, BEST_FIT_DATA & io_cFit_Data, bool i_bNo_Fit, bool i_bSpectrum_Pre_Loaded, bool i_bAllow_Temp_Fit, bool i_bDisallow_Scalar_Fit, const double & i_dWL_Ref, const double & i_dHVF_Scalar_Upper_Bound)
{
	FIT_DATA_CONTAINER cFit_data;
	ES::Spectrum cOutput(i_cTarget);

	cFit_data.lpTarget = &i_cTarget;
	cFit_data.lpOutput = &cOutput;
	cFit_data.lpOpacity_Map_Ejecta = &i_cOpacity_Map_A;
	cFit_data.lpOpacity_Map_Shell = &i_cOpacity_Map_B;
	cFit_data.uiIon = i_uiIon;
	cFit_data.dFit_WL_Blue = i_dFit_WL_Blue;
	cFit_data.dFit_WL_Red = i_dFit_WL_Red;

	cFit_data.bAllow_Temp_Fit = i_bAllow_Temp_Fit;
	cFit_data.bDisallow_Scalar_Fit = i_bDisallow_Scalar_Fit;

	cFit_data.dMJD = i_dDate_MJD;
	cFit_data.dPS_Scalar = io_cFit_Data.dBest_Fit_PS_Log_Tau;
	cFit_data.dHVF_Scalar = io_cFit_Data.dBest_Fit_HVF_Log_Tau;


	printf("Generating vector\n");
//	double	dPS_Temp = Continuum_Fit(i_cTarget);
//	io_cFit_Data.dBest_Fit_PS_Temp = dPS_Temp;
	XVECTOR cResult = Generate_Vector_From_Best_Fit(io_cFit_Data,i_cOpacity_Map_A,false);
	XVECTOR vBest_Fit(cResult);
	XVECTOR	vStarting_Point;
	XVECTOR	vVariations;
	XVECTOR	vEpsilon;
	XVECTOR	vLower_Bounds;
	XVECTOR	vUpper_Bounds;
	std::vector<bool> vLower_Bounds_Valid;
	std::vector<bool> vUpper_Bounds_Valid;


//	printf("\n");
//	cResult.Print();
	unsigned int uiI;// = (unsigned int)(cResult.Get(0) + 0.25);
	printf("Day %.2f\n",cResult.Get(0));
	double	dDay_Min;
	if (cResult.Get(0) > 3.0)
		dDay_Min = cResult.Get(0) - 3.0;
	else
		dDay_Min = 0.01;
	double	dDay_Max = uiI + 12;
	io_cFit_Data.dSource_Data_MJD = i_dDate_MJD;
	double	dPS_Min = 0.0;
	for (unsigned int uiI = 1; uiI < i_cOpacity_Map_A.GetNumElements() && dPS_Min == 0.0; uiI++)
		if (i_cOpacity_Map_A.GetElement(1,uiI) > 0.0)
			dPS_Min = i_cOpacity_Map_A.GetElement(0,uiI) * 1e-8;
	if (cResult.Get(0) < dPS_Min)
		cResult.Set(0,dPS_Min);

	if (!i_bNo_Fit || (i_bNo_Fit && !i_bSpectrum_Pre_Loaded))
	{
		unsigned int uiNum_Parameters;
		cFit_data.dSet_Day = 1.0;// fixed
		cFit_data.dSet_Excitation_Temp_HVF = io_cFit_Data.dBest_Fit_PS_Ion_Temp;
		cFit_data.dSet_Excitation_Temp_PVF = io_cFit_Data.dBest_Fit_HVF_Ion_Temp;
		if (i_cOpacity_Map_B.GetNumElements()> 0) // shell data available
			uiNum_Parameters = 6;
		else
			uiNum_Parameters = 4;
		double	dScalar = log10(pow(io_cFit_Data.dBest_Fit_Day / cFit_data.dSet_Day,-3.0));
		if (i_bAllow_Temp_Fit && !i_bDisallow_Scalar_Fit)
		{

			vStarting_Point.Set_Size(uiNum_Parameters);
			vStarting_Point.Set(0,io_cFit_Data.dBest_Fit_PS_Temp);
			vStarting_Point.Set(1,io_cFit_Data.dBest_Fit_PS_Vel);
			vStarting_Point.Set(2,io_cFit_Data.dBest_Fit_PS_Ion_Temp);
			vStarting_Point.Set(3,io_cFit_Data.dBest_Fit_PS_Log_Tau + dScalar);
			if (uiNum_Parameters > 4)
			{
				vStarting_Point.Set(4,io_cFit_Data.dBest_Fit_HVF_Ion_Temp);
				vStarting_Point.Set(5,io_cFit_Data.dBest_Fit_HVF_Log_Tau + dScalar);
			}
			vEpsilon.Set_Size(uiNum_Parameters);
			vEpsilon.Set(0,0.05);
			vEpsilon.Set(1,0.05);
			vEpsilon.Set(2,0.1);
			vEpsilon.Set(3,0.01);
			if (uiNum_Parameters > 4)
			{
				vEpsilon.Set(4,0.1);
				vEpsilon.Set(5,0.01);
			}
			vVariations.Set_Size(uiNum_Parameters);
			vVariations.Set(0,1.0);
			vVariations.Set(1,1.0);
			vVariations.Set(2,1.0);
			vVariations.Set(3,0.5);
			if (uiNum_Parameters > 4)
			{
				vVariations.Set(4,1.0);
				vVariations.Set(5,0.5);
			}
			vLower_Bounds.Set_Size(uiNum_Parameters);
			vLower_Bounds_Valid.resize(uiNum_Parameters);
			vLower_Bounds.Set(0,8.0);
			vLower_Bounds.Set(1,5.0);
			vLower_Bounds.Set(2,3.0);
			vLower_Bounds_Valid[0] = true;
			vLower_Bounds_Valid[1] = true;
			vLower_Bounds_Valid[2] = true;
			vLower_Bounds_Valid[3] = false;
			if (uiNum_Parameters > 4)
			{
				vLower_Bounds.Set(4,3.0);
				vLower_Bounds_Valid[4] = true;
				vLower_Bounds_Valid[5] = false;
			}
			vUpper_Bounds.Set_Size(uiNum_Parameters);
			vUpper_Bounds_Valid.resize(uiNum_Parameters);
			vUpper_Bounds_Valid[0] = false;
			vUpper_Bounds_Valid[1] = true;
			vUpper_Bounds_Valid[2] = false;
			vUpper_Bounds.Set(1,25.0);
			vUpper_Bounds.Set(3,2.0);
			vUpper_Bounds_Valid[3] = true;
			if (uiNum_Parameters > 4)
			{
				vUpper_Bounds_Valid[4] = false;
				vUpper_Bounds.Set(5,i_dHVF_Scalar_Upper_Bound);
				vUpper_Bounds_Valid[5] = true;
			}
			
		}
		else if (i_bAllow_Temp_Fit)
		{
			if (i_cOpacity_Map_B.GetNumElements()> 0) // shell data available
				uiNum_Parameters -= 2;
			else
				uiNum_Parameters -= 1;
			vStarting_Point.Set_Size(uiNum_Parameters);
			vStarting_Point.Set(0,io_cFit_Data.dBest_Fit_PS_Temp);
			vStarting_Point.Set(1,io_cFit_Data.dBest_Fit_PS_Vel);
			vStarting_Point.Set(2,io_cFit_Data.dBest_Fit_PS_Ion_Temp);
			if (uiNum_Parameters > 3)
				vStarting_Point.Set(3,io_cFit_Data.dBest_Fit_HVF_Ion_Temp);
			vEpsilon.Set_Size(uiNum_Parameters);
			vEpsilon.Set(0,0.05);
			vEpsilon.Set(1,0.05);
			vEpsilon.Set(2,0.1);
			if (uiNum_Parameters > 3)
				vEpsilon.Set(3,0.1);
			vVariations.Set_Size(uiNum_Parameters);
			vVariations.Set(0,1.0);
			vVariations.Set(1,1.0);
			vVariations.Set(2,1.0);
			if (uiNum_Parameters > 3)
				vVariations.Set(3,1.0);
			vLower_Bounds.Set_Size(uiNum_Parameters);
			vLower_Bounds_Valid.resize(uiNum_Parameters);
			vLower_Bounds.Set(0,8.0);
			vLower_Bounds.Set(1,5.0);
			vLower_Bounds.Set(2,3.0);
			vLower_Bounds_Valid[0] = true;
			vLower_Bounds_Valid[1] = true;
			vLower_Bounds_Valid[2] = true;
			if (uiNum_Parameters > 3)
			{
				vLower_Bounds.Set(3,3.0);
				vLower_Bounds_Valid[3] = true;
			}
			vUpper_Bounds.Set_Size(uiNum_Parameters);
			vUpper_Bounds_Valid.resize(uiNum_Parameters);
			vUpper_Bounds_Valid[0] = false;
			vUpper_Bounds_Valid[1] = true;
			vUpper_Bounds_Valid[2] = false;
			vUpper_Bounds.Set(1,25.0);
			if (uiNum_Parameters > 3)
			{
				vUpper_Bounds_Valid[3] = false;
			}
		}
		else if (!i_bDisallow_Scalar_Fit)
		{
			if (i_cOpacity_Map_B.GetNumElements()> 0) // shell data available
				uiNum_Parameters -= 2;
			else
				uiNum_Parameters -= 1;
			vStarting_Point.Set_Size(uiNum_Parameters);
			vStarting_Point.Set(0,io_cFit_Data.dBest_Fit_PS_Temp);
			vStarting_Point.Set(1,io_cFit_Data.dBest_Fit_PS_Vel);
			vStarting_Point.Set(2,io_cFit_Data.dBest_Fit_PS_Log_Tau + dScalar);
			if (uiNum_Parameters > 3)
				vStarting_Point.Set(3,io_cFit_Data.dBest_Fit_HVF_Log_Tau + dScalar);
			vEpsilon.Set_Size(uiNum_Parameters);
			vEpsilon.Set(0,0.05);
			vEpsilon.Set(1,0.05);
			vEpsilon.Set(2,0.01);
			if (uiNum_Parameters > 3)
				vEpsilon.Set(3,0.01);
			vVariations.Set_Size(uiNum_Parameters);
			vVariations.Set(0,1.0);
			vVariations.Set(1,1.0);
			vVariations.Set(2,0.5);
			if (uiNum_Parameters > 3)
				vVariations.Set(3,0.5);
			vLower_Bounds.Set_Size(uiNum_Parameters);
			vLower_Bounds_Valid.resize(uiNum_Parameters);
			vLower_Bounds.Set(0,8.0);
			vLower_Bounds.Set(1,5.0);
			vLower_Bounds_Valid[0] = true;
			vLower_Bounds_Valid[1] = true;
			vLower_Bounds_Valid[2] = false;
			if (uiNum_Parameters > 3)
			{
				vLower_Bounds.Set(3,3.0);
				vLower_Bounds_Valid[3] = false;
			}
			vUpper_Bounds.Set_Size(uiNum_Parameters);
			vUpper_Bounds_Valid.resize(uiNum_Parameters);
			vUpper_Bounds_Valid[0] = false;
			vUpper_Bounds_Valid[1] = true;
			vUpper_Bounds.Set(1,25.0);
			vUpper_Bounds.Set(2,2.0);
			vUpper_Bounds_Valid[2] = true;
			if (uiNum_Parameters > 3)
			{
				vUpper_Bounds.Set(3,i_dHVF_Scalar_Upper_Bound);
				vUpper_Bounds_Valid[3] = true;
			}
		}
		else
		{
			vStarting_Point.Set_Size(2);
			vStarting_Point.Set(0,io_cFit_Data.dBest_Fit_PS_Temp);
			vStarting_Point.Set(1,io_cFit_Data.dBest_Fit_PS_Vel);
			vEpsilon.Set_Size(2);
			vEpsilon.Set(0,0.05);
			vEpsilon.Set(1,0.05);
			vVariations.Set_Size(2);
			vVariations.Set(0,1.0);
			vVariations.Set(1,1.0);
			vLower_Bounds.Set_Size(2);
			vLower_Bounds_Valid.resize(2);
			vLower_Bounds.Set(0,8.0);
			vLower_Bounds.Set(1,5.0);
			vLower_Bounds_Valid[0] = true;
			vLower_Bounds_Valid[1] = true;
			vUpper_Bounds.Set_Size(uiNum_Parameters);
			vUpper_Bounds_Valid.resize(uiNum_Parameters);
			vUpper_Bounds_Valid[0] = false;
			vUpper_Bounds_Valid[1] = true;
			vUpper_Bounds.Set(1,25.0);
		}
		// make sure day doesn't get too small
		if (vStarting_Point.Get(0) < 0.3)
			vStarting_Point.Set(0,0.3);
		if (vStarting_Point.Get(1) < dPS_Min)
			vStarting_Point.Set(1,dPS_Min);
	}
	vBest_Fit = cResult;
	printf("Starting %.1f %.1f %.1f %.1f %.1f\t",vBest_Fit.Get(0),vBest_Fit.Get(1),vBest_Fit.Get(2),vBest_Fit.Get(4),vBest_Fit.Get(6));
	fflush(stdout);
	if (!i_bNo_Fit || (i_bNo_Fit && !i_bSpectrum_Pre_Loaded))
	{
		double dDay =  cResult.Get(0);
		double	dPS_Vel = cResult.Get(1);
		double	dPS_Temp = Photosphere_Temp(i_dDate_MJD, dDay, dPS_Vel);
//		printf("%.1f %.1f %.1f %.1f\n",i_dDate_MJD,dDay,dPS_Vel,dPS_Temp);
		cResult.Set(2,dPS_Temp);
		cResult.Set(3,io_cFit_Data.dBest_Fit_PS_Ion_Temp);
		cResult.Set(5,io_cFit_Data.dBest_Fit_HVF_Ion_Temp);
//		cResult.Print();
		Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
	}
	else
	{
		printf("Copying\n");
		cOutput = io_cFit_Data.cBest_Fit_Spectrum;
		if (cOutput.wl(15) != io_cFit_Data.cBest_Fit_Spectrum.wl(15) || cOutput.flux(15) != io_cFit_Data.cBest_Fit_Spectrum.flux(15))
			printf("Copy failed\n");
	}
//	double dDay_Abs_Max = i_cOpacity_Map_A.GetNumColumns() - 2;
//	if (dDay_Min < 0.0)
//		dDay_Min = 0.0;
//	if (dDay_Max > dDay_Abs_Max)
//		dDay_Max = dDay_Abs_Max;
//	double	dDay_Range = (dDay_Max - dDay_Min);
//	double	dRange = sqrt(dDay_Range * dDay_Range + 3.0 * 100.0 + 30.0 * 30.0); // day range^2 + temp range^2 + PS scalar range^2 + HVF scalar range ^ + PS vel range ^2; this is really just a rough guideline
	//ameoba algorithm to refine fit
	double	dBest_Fit = Get_Fit(i_cTarget,cOutput,i_dFit_WL_Blue,i_dFit_WL_Red,2);
//	printf("Starting fit %.2e\n",dBest_Fit);
//	double dRange = DBL_MAX;
	// identify nearest boundary
//	if (vBest_Fit.Get(0) != dDay_Min)
//		dRange = vBest_Fit.Get(0) - dDay_Min;
//	if (vBest_Fit.Get(0) != dDay_Max && dRange > dDay_Max - vBest_Fit.Get(0))
//		dRange = dDay_Max - vBest_Fit.Get(0);
//	if (vBest_Fit.Get(1) != 35.0 && dRange > 35.0 - vBest_Fit.Get(1))
//		dRange = 35.0 - vBest_Fit.Get(1);
//	if (vBest_Fit.Get(1) != dPS_Min && dRange > vBest_Fit.Get(1) - dPS_Min)
//		dRange = vBest_Fit.Get(1) - dPS_Min;
//	if (vBest_Fit.Get(2) != 20.0 && dRange > 20.0 - vBest_Fit.Get(2))
//		dRange = 20.0 - vBest_Fit.Get(2);
//	if (vBest_Fit.Get(2) != 8.0 && dRange > vBest_Fit.Get(2) - 8.0)
//		dRange = vBest_Fit.Get(2) - 8.0;
//	dRange = 2.0;
//	double	dBest_Fit_Range = dRange;
//	if (dBest_Fit_Range < 2.0)
//		dBest_Fit_Range = 2.0; // minimum search range is 2
	printf("Fit = %.2e\n",dBest_Fit);
	if (!i_bNo_Fit)
	{
		XFIT_Simplex(vStarting_Point, vVariations, vEpsilon, Fit_Function, &cFit_data, false, &vLower_Bounds, &vLower_Bounds_Valid, &vUpper_Bounds, &vUpper_Bounds_Valid);
		// There is a degeneracy between day, velocity, and the scalars. Try to resolve that
/*		if (!i_bDisallow_Scalar_Fit)
		{
			double dIncr = 0.25;
			double	dDay_Start = 0.25;
			double	dDay_End = 30.0;
			do
			{
				double dDay = Refine_Day_Fit(vStarting_Point, cFit_data, vStarting_Point.Get(0) - 1.0, vStarting_Point.Get(0) + 1.0, dIncr);
				dDay_Start = dDay - dIncr;
				if (dDay_Start <= 0.01)
					dDay_Start = dIncr;
				dDay_End = dDay + dIncr;
			
				dIncr *= 0.5;
			}
			while (dIncr > 0.05);
			double	dBest_Day = (dDay_Start + dDay_End) * 0.5;
			double	dDay_Scalar = vStarting_Point.Get(0) / dBest_Day;
			double dV = vStarting_Point.Get(1) * dDay_Scalar;
			double dSpvf = vStarting_Point.Get(4) - log10(dDay_Scalar * dDay_Scalar);
			double dShvf = vStarting_Point.Get(6) - log10(dDay_Scalar * dDay_Scalar);
			vStarting_Point.Set(0,dBest_Day);
			vStarting_Point.Set(1,dV);
			vStarting_Point.Set(4,dSpvf);
			vStarting_Point.Set(6,dShvf);
		}*/
		Convert_Parameter_To_Full_Vector(vStarting_Point, cFit_data, vBest_Fit,FIXED_TEMP);

		
/*		unsigned int iNo_Update_Counter = 0;
		while (dBest_Fit_Range > 0.01 && iNo_Update_Counter < 2)
		{
			dRange =  dBest_Fit_Range;
			XVECTOR vCurr(vBest_Fit);
			bool bFound_Better_Fit = false;
			for (unsigned int uiI = 0; uiI < uiParameters * 2; uiI++) // p * 2 isn't analytic - can probably refine this better - it's probably p + 1 for optimal search (akin to simplex)
			{
				cAmoeba_Test_Angle.Set(0,dRange * xrand_d());
				cAmoeba_Test_Angle.Set(uiParameters - 1,g_cConstants.dPi * 2.0 * xrand_d());

				for (unsigned int uiJ = 1; uiJ < (uiParameters - 1); uiJ++)
				{
					cAmoeba_Test_Angle.Set(uiJ,g_cConstants.dPi * xrand_d());
				}
				double	dCommon = cAmoeba_Test_Angle.Get(0);
	//			printf("Trying angle %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",cAmoeba_Test_Angle.Get(0),cAmoeba_Test_Angle.Get(1),cAmoeba_Test_Angle.Get(2),cAmoeba_Test_Angle.Get(3),cAmoeba_Test_Angle.Get(4),cAmoeba_Test_Angle.Get(5),cAmoeba_Test_Angle.Get(6));
				for (unsigned int uiJ = 0; uiJ < uiParameters; uiJ++)
				{
					if (uiJ != (uiParameters - 1))	
					{
						double dAngle_Next = cAmoeba_Test_Angle.Get(uiJ + 1);
						cAmoeba_Test_Euclidian.Set(uiJ,dCommon * cos(dAngle_Next));
						dCommon *= sin(dAngle_Next);
					}
					else
						cAmoeba_Test_Euclidian.Set(uiJ,dCommon);
				}
				cAmoeba_Test_Euclidian.Set(2,0.0); // Don't vary PS temp - it was already set
				if (!i_bAllow_Temp_Fit)
				{
					cAmoeba_Test_Euclidian.Set(3,cAmoeba_Test_Euclidian.Get(2)); // set PS LGR temp to PS temp
					cAmoeba_Test_Euclidian.Set(5,cAmoeba_Test_Euclidian.Get(2)); // set HVF LGR temp to PS temp
				}
				if (i_bDisallow_Scalar_Fit)
				{
					cAmoeba_Test_Euclidian.Set(4,0.0); // If scalar fitting is disallowed, set delta to 0
					cAmoeba_Test_Euclidian.Set(6,0.0); // 
				}
				if (cAmoeba_Test_Euclidian.Magnitude() > dRange)
					cAmoeba_Test_Euclidian *= (dRange / cAmoeba_Test_Euclidian.Magnitude());
				cAmoeba_Test_Euclidian += vCurr;
	//			printf("In Euclidian %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",cAmoeba_Test_Euclidian.Get(0),cAmoeba_Test_Euclidian.Get(1),cAmoeba_Test_Euclidian.Get(2),cAmoeba_Test_Euclidian.Get(3),cAmoeba_Test_Euclidian.Get(4),cAmoeba_Test_Euclidian.Get(5),cAmoeba_Test_Euclidian.Get(6));
				// make sure temperature, velocity, and day stay within valid range
				Restrict_Bound(vCurr,cAmoeba_Test_Euclidian,0,dDay_Max,dDay_Min); // day
				Restrict_Bound(vCurr,cAmoeba_Test_Euclidian,1,35.0,dPS_Min); // ps velocity
//				Restrict_Bound(vCurr,cAmoeba_Test_Euclidian,2,15.0,7.5); // temp; note - by definition
				int iT_ref = (int )(cAmoeba_Test_Euclidian.Get(0) + 0.5);
				if (iT_ref < 0)
					iT_ref = 0;
				if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
					iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
				double dDay =  i_cOpacity_Map_A.GetElement(iT_ref + 1,0);
//				printf("%.2f %.2f %.2f %.1f\n",i_dDate_MJD, dDay, cAmoeba_Test_Euclidian.Get(1),Photosphere_Temp(i_dDate_MJD, dDay, cAmoeba_Test_Euclidian.Get(1)));
				cAmoeba_Test_Euclidian.Set(2,Photosphere_Temp(i_dDate_MJD, dDay, cAmoeba_Test_Euclidian.Get(1)));

				cAmoeba_Test_Euclidian.Set(3,cAmoeba_Test_Euclidian.Get(2)); // set PS LGR temp to PS temp
				cAmoeba_Test_Euclidian.Set(5,cAmoeba_Test_Euclidian.Get(2)); // set PS LGR temp to PS temp
				vAmoeba_Test_Delta = cAmoeba_Test_Euclidian - vCurr; // find the delta for the move
				fprintf(stdout,"Trying %.1f %.1f %.1f %.1f %.1f (%.2f)\t",cAmoeba_Test_Euclidian.Get(0),cAmoeba_Test_Euclidian.Get(1),cAmoeba_Test_Euclidian.Get(2),cAmoeba_Test_Euclidian.Get(4),cAmoeba_Test_Euclidian.Get(6),vAmoeba_Test_Delta.Magnitude());
				Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cAmoeba_Test_Euclidian,cOutput);
				double dFit = Get_Fit(i_cTarget,cOutput,i_dFit_WL_Blue,i_dFit_WL_Red,2);
				fflush(stdout);
				printf("Fit = %.2e Range = %.2e",dFit,dRange);
				if (dFit < dBest_Fit)
				{
					dBest_Fit = dFit;
					printf("\tBF");
					dBest_Fit_Range = vAmoeba_Test_Delta.Magnitude(); // set the next range to the magnitude of the delta
					vBest_Fit = cAmoeba_Test_Euclidian;
					bFound_Better_Fit = true;
				}
				printf("\n");
			}
			if (bFound_Better_Fit)
				iNo_Update_Counter = 0; // reset
			else
				iNo_Update_Counter++;
		}*/
	}
	else
	{
		Fit_Function(vStarting_Point,&cFit_data);
	}
	cResult = vBest_Fit;
	if (!i_bNo_Fit || (i_bNo_Fit && !i_bSpectrum_Pre_Loaded))
	{
		Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
		io_cFit_Data.cBest_Fit_Spectrum = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	}
	cResult.Set(4,-20.0); // turn off PS 
//	printf("%.2f %.2f\n",cResult.Get(4),cResult.Get(6));
	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
	io_cFit_Data.cBest_Fit_Spectrum_No_PS = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...

	cResult.Set(4,vBest_Fit.Get(4)); // turn off PS 
	cResult.Set(6,-20.0); // turn off HVF
//	printf("%.2f %.2f\n",cResult.Get(4),cResult.Get(6));
	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput);
	io_cFit_Data.cBest_Fit_Spectrum_No_HVF = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	cResult.Set(4,-20.0); // turn off PS 
	cResult.Set(6,-20.0); // turn off HVF
	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,cResult,cOutput); // continuum
	io_cFit_Data.cBest_Fit_Spectrum_Continuum = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...	
	unsigned int uiIdx = 0;
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < i_dFit_WL_Blue)
		uiIdx++;
	double	dMin_PS = DBL_MAX,dMin_HVF = DBL_MAX,dMin_WL_PS,dMin_WL_HVF;
	double	dPVF_pEW = 0.0, dHVF_pEW = 0.0;
//	Find_Blue_Maxima(i_c
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < i_dFit_WL_Red)
	{
		double	dDelta_w;
		if (uiIdx > 0 && uiIdx != (i_cTarget.size() - 1))
			dDelta_w = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx - 1)) * 0.5;
		else if (uiIdx == 0)
			dDelta_w = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx));
		else
			dDelta_w = (i_cTarget.wl(uiIdx) - i_cTarget.wl(uiIdx - 1));


		double dFlux = io_cFit_Data.cBest_Fit_Spectrum_No_PS.flux(uiIdx) / cOutput.flux(uiIdx);
		if (dFlux < dMin_HVF)
		{
			dMin_WL_HVF = cOutput.wl(uiIdx);
			dMin_HVF = dFlux;
		}

		if (dFlux < 1.0)
			dHVF_pEW += (1.0 - dFlux) * dDelta_w;

		dFlux = io_cFit_Data.cBest_Fit_Spectrum_No_HVF.flux(uiIdx) / cOutput.flux(uiIdx);
		if (dFlux < dMin_PS)
		{
			dMin_WL_PS = cOutput.wl(uiIdx);
			dMin_PS = dFlux;
		}
		if (dFlux < 1.0)
			dPVF_pEW += (1.0 - dFlux) * dDelta_w;
		uiIdx++;
	}

	double	dZ = dMin_WL_PS / i_dWL_Ref - 1.0;
	double	dZ_p1_sqr = (dZ + 1.0) * (dZ + 1.0);
	io_cFit_Data.m_dPS_Absorption_Peak_Velocity = (dZ_p1_sqr - 1) / (dZ_p1_sqr + 1) * -300.0;
	io_cFit_Data.m_dPVF_Pseudo_Equivalent_Width = dPVF_pEW;
	dZ = dMin_WL_HVF / i_dWL_Ref - 1.0;
	dZ_p1_sqr = (dZ + 1.0) * (dZ + 1.0);
	io_cFit_Data.m_dHVF_Absorption_Peak_Velocity = (dZ_p1_sqr - 1) / (dZ_p1_sqr + 1) * -300.0;
	io_cFit_Data.m_dHVF_Pseudo_Equivalent_Width = dHVF_pEW;

//	Convert_Parameter_To_Full_Vector(vStarting_Point, cFit_data, vBest_Fit,SET_DAY);
	Get_Fit_Data_From_Vector(vBest_Fit,io_cFit_Data,i_cOpacity_Map_A,false);
	io_cFit_Data.SetNumMoments(6);
	double * lpdRaw_Moments = NULL, *lpdCentral_Moments = NULL, *lpdStandardized_Moments = NULL;
	double dMax_Flux,dGenerated_Flux;
//	Get_Normalization_Fluxes(i_cTarget, io_cFit_Data.cBest_Fit_Spectrum, i_dFit_WL_Blue,i_dFit_WL_Red,dMax_Flux,dGenerated_Flux);
	Get_Fit_Moments_2(i_cTarget, io_cFit_Data.cBest_Fit_Spectrum, i_dFit_WL_Blue,i_dFit_WL_Red, 6, lpdRaw_Moments, lpdCentral_Moments, lpdStandardized_Moments, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_cFit_Data.SetMoments(uiI,lpdRaw_Moments[uiI - 1], lpdCentral_Moments[uiI - 1], lpdStandardized_Moments[uiI - 1]);
	printf("Done\n");
}

void CompareFitsExp(const ES::Spectrum &i_cTarget, unsigned int i_uiIon, bool i_bInclude_HVF, const double &i_dPS_Vel, const double &i_dPS_Temp, const double & i_dPS_Ion_Temp, const double &i_dPS_Log_Tau, const double &i_dPS_Ion_Vmin, const double &i_dPS_Ion_Vmax, const double & i_dPS_Ion_Aux, const double & i_dHVF_Ion_Temp, const double &i_dHVF_Log_Tau, const double &i_dHVF_Ion_Vmin, const double &i_dHVF_Ion_Vmax, const double & i_dHVF_Ion_Aux, ES::Spectrum &o_cOutput, ES::Spectrum &o_cOutput_No_PS, ES::Spectrum &o_cOutput_No_HVF, bool i_bNo_Fit, bool i_bSpectrum_Pre_Loaded)
{
	ES::Spectrum cOutput(i_cTarget);

	unsigned int uiParameters = 7;
	if (i_bInclude_HVF)
		uiParameters = 12;

	XVECTOR cResult(uiParameters);

	cResult.Set(0,i_dPS_Vel);
	cResult.Set(1,i_dPS_Temp);
	cResult.Set(2,i_dPS_Log_Tau);
	cResult.Set(3,i_dPS_Ion_Temp);
	cResult.Set(4,i_dPS_Ion_Vmin);
	cResult.Set(5,i_dPS_Ion_Vmax);
	cResult.Set(6,i_dPS_Ion_Aux);
	if (i_bInclude_HVF)
	{
		cResult.Set(7,i_dHVF_Ion_Temp);
		cResult.Set(8,i_dHVF_Log_Tau);
		cResult.Set(9,i_dHVF_Ion_Vmin);
		cResult.Set(10,i_dHVF_Ion_Vmax);
		cResult.Set(11,i_dHVF_Ion_Aux);
	}
	if (!i_bNo_Fit || (i_bNo_Fit && !i_bSpectrum_Pre_Loaded))
	{
		Generate_Synow_Spectra_Exp(i_cTarget,i_uiIon,cResult,cOutput);
		o_cOutput = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	}
	cResult.Set(2,-20.0);
	cResult.Set(6,1.0);
	Generate_Synow_Spectra_Exp(i_cTarget,i_uiIon,cResult,cOutput);
	o_cOutput_No_PS = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	cResult.Set(2,i_dPS_Log_Tau);
	cResult.Set(6,i_dPS_Ion_Aux);
	cResult.Set(8,-20.0);
	cResult.Set(11,1.0);
	Generate_Synow_Spectra_Exp(i_cTarget,i_uiIon,cResult,cOutput);
	o_cOutput_No_HVF = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
}





int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit_Info;
//	ES::Spectrum	cSpectrum_No_HVF, cSpectrum_No_PS;
	double dFit_Range_Min_WL,dFit_Range_Max_WL,dWL_Ref;
	bool bData_valid;
	char lpszTarget_Path[256];
	char lpszTarget_File[256];
	char lpszTarget_Complete_Path[256] = {""};
	char lpszOutput_File_Prefix[256] = {""};
	char lpszOutput_Filename[256];
	char lpszOutput_Binary_Filename[256];
	char lpszTitle[256];
	unsigned int uiIon;
	bool bData_Valid = false;
	bool	bNo_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-plot-title");
	bool	bLinear_Plot = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--linear-scale");
	bool	bNo_Smart_Range = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range");
	bool	bNo_Smart_Range_Regions = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-smart-range-regions");
	bool	bInhibit_Resave = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-resave");
	bool	bNo_Fit = xParse_Command_Line_Bool(i_iArg_Count,i_lpszArg_Values,"--no-fit",false);
	bool	bForce_Generate = xParse_Command_Line_Bool(i_iArg_Count,i_lpszArg_Values,"--force-generate",false);
	bool	bDisable_P_Cygni_Dynamic_Inclusion = xParse_Command_Line_Bool(i_iArg_Count,i_lpszArg_Values,"--disable-P-Cygni-dynamic-inclusion",false);
	double	dUpper_HVF_Scalar_Bound = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-scalar-upper-bound",1.0);
	bool	bSpectrum_Pre_Loaded = false;
	bool	bAllow_Ion_Temp_Fit, bDisallow_Scalar_Fit = false;
	bool 	bNo_Shell;
	double	dNan = nan("");
	double	dTarget_MJD = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--target-MJD",dNan);
	xrsrand();

	bool	bCalc_Fit = false;
	enum	tFit_Region		{CANIR,CAHK,SI5968,SI6355,OINIR,OTHER};
	tFit_Region	eFit_Region = OTHER;
	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--bestfittarget") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fitregion"))
	{
		char lpszFitRegion[32];
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--bestfittarget", lpszTarget_Complete_Path, 256, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitregion", lpszFitRegion, 32, NULL);
		
		strcpy(lpszTarget_Path,lpszTarget_Complete_Path);
		unsigned int uiI = strlen(lpszTarget_Path);
		while (lpszTarget_Path[uiI] != '/' && uiI > 0)
			uiI--;
		if (uiI != 0)
		{
			uiI++;
			lpszTarget_Path[uiI] = 0;// 
		}
		else
			strcpy(lpszTarget_Path,"./");
		strcpy(lpszTarget_File,&lpszTarget_Complete_Path[uiI]);

		if (strcmp(lpszFitRegion,"CaHK") == 0 ||
			strcmp(lpszFitRegion,"CAHK") == 0 ||
			strcmp(lpszFitRegion,"CANIR") == 0 ||
			strcmp(lpszFitRegion,"CaNIR") == 0)
		{
			eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
			
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;//eFit_Region == CANIR;@@TODO
			uiIon = 2001;
			sprintf(lpszOutput_Filename,"%s/%s/%s.bestfit.%s.data",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.bestfit.%s.databin",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si6355") == 0 || strcmp(lpszFitRegion,"SI6355") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI6355;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.bestfit.%s.data",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.bestfit.%s.databin",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si5968") == 0 || strcmp(lpszFitRegion,"SI5968") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI5968;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = true;
			sprintf(lpszOutput_Filename,"%s%s/%s.bestfit.%s.data",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.bestfit.%s.databin",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"OI8446") == 0)
		{
			uiIon = 800;
			eFit_Region = OINIR;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.bestfit.%s.data",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.bestfit.%s.databin",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"user") == 0)
		{
			eFit_Region = OTHER;
			uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);
			sprintf(lpszOutput_Filename,"%s%s/%s.user.data",lpszTarget_Path,lpszFitRegion,lpszTarget_File);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.bestfit.%s.databin",lpszTarget_Path,lpszFitRegion,lpszTarget_File,lpszFitRegion);
		}
		else
			uiIon = -1;

		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
			cFit_Info.ReadFromFile(lpszOutput_Filename);

		sprintf(lpszOutput_File_Prefix,"%s%s/%s",lpszTarget_Path,lpszFitRegion,lpszTarget_File);
		bCalc_Fit = true;
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fittarget") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fitregion") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fitmodel"))
	{
		char lpszFitRegion[32];
		char	lpszFitModel[128];
	
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fittarget", lpszTarget_Complete_Path, 256, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitregion", lpszFitRegion, 32, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitmodel", lpszFitModel, 128, NULL);
		
		strcpy(lpszTarget_Path,lpszTarget_Complete_Path);
		unsigned int uiI = strlen(lpszTarget_Path);
		while (lpszTarget_Path[uiI] != '/' && uiI > 0)
			uiI--;
		if (uiI != 0)
		{
			uiI++;
			lpszTarget_Path[uiI] = 0;// 
		}
		else
			strcpy(lpszTarget_Path,"./");
		strcpy(lpszTarget_File,&lpszTarget_Complete_Path[uiI]);
		if (strcmp(lpszFitRegion,"CaHK") == 0 ||
			strcmp(lpszFitRegion,"CaNIR") == 0)
		{
			uiIon = 2001;
			eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;//(eFit_Region == CANIR);@@TODO
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.databin",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si6355") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI6355;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.databin",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si5968") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI5968;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = true;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.databin",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"OI8446") == 0)
		{
			uiIon = 800;
			eFit_Region = OINIR;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.databin",lpszTarget_Path, lpszFitRegion, lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"user") == 0)
		{
			eFit_Region = OTHER;
			uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);
			sprintf(lpszOutput_Filename,"%s%s/%s.user.data",lpszTarget_Path, lpszFitRegion, lpszTarget_File);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.user.databin",lpszTarget_Path, lpszFitRegion, lpszTarget_File);
		}
		else
			uiIon = -1;

		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
			cFit_Info.ReadFromFile(lpszOutput_Filename);

		sprintf(lpszOutput_File_Prefix,"%s%s/%s",lpszTarget_Path,lpszFitRegion,lpszTarget_File);
		bCalc_Fit = true;
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--userfittarget") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fitregion") && xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--fitmodel"))
	{
		char lpszFitRegion[32];
		char	lpszFitModel[128];
	
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--userfittarget", lpszTarget_Complete_Path, 256, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitregion", lpszFitRegion, 32, NULL);
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fitmodel", lpszFitModel, 128, NULL);
		
		strcpy(lpszTarget_Path,lpszTarget_Complete_Path);
		unsigned int uiI = strlen(lpszTarget_Path);
		while (lpszTarget_Path[uiI] != '/' && uiI > 0)
			uiI--;
		if (uiI != 0)
		{
			uiI++;
			lpszTarget_Path[uiI] = 0;// 
		}
		else
			strcpy(lpszTarget_Path,"./");
		strcpy(lpszTarget_File,&lpszTarget_Complete_Path[uiI]);
		if (strcmp(lpszFitRegion,"CaHK") == 0 ||
			strcmp(lpszFitRegion,"CaNIR") == 0)
		{
			uiIon = 2001;
			eFit_Region = (lpszFitRegion[2] == 'H' ? CAHK : CANIR);
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;//(eFit_Region == CANIR);@@TODO
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si6355") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI6355;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"Si5968") == 0)
		{
			uiIon = 1401;
			eFit_Region = SI5968;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"OI8446") == 0)
		{
			uiIon = 800;
			eFit_Region = OINIR;
			bDisallow_Scalar_Fit = bAllow_Ion_Temp_Fit = false;
			sprintf(lpszOutput_Filename,"%s%s/%s.fit.%s.%s.data.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.fit.%s.%s.data.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File, lpszFitModel, lpszFitRegion);
		}
		else if (strcmp(lpszFitRegion,"user") == 0)
		{
			eFit_Region = OTHER;
			uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);
			sprintf(lpszOutput_Filename,"%s%s/%s.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
			sprintf(lpszOutput_Binary_Filename,"%s%s/%s.user.databin",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
		}
		else
			uiIon = -1;
		printf("Reading %s\n",lpszOutput_Filename);
		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
			cFit_Info.ReadFromFile(lpszOutput_Filename);
		printf("Read done\n");
		if (eFit_Region != OTHER)
			sprintf(lpszOutput_File_Prefix,"%s%s/%s.fit.%s.%s.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File,lpszFitModel,lpszFitRegion);
		else
			sprintf(lpszOutput_File_Prefix,"%s%s/%s.user.data",lpszTarget_Path, lpszFitRegion,lpszTarget_File);
		bCalc_Fit = true;
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--datafile"))
	{
		char lpszDatafile[256];
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--datafile", lpszDatafile, 256, NULL);
		
		sprintf(lpszOutput_Binary_Filename,"%sbin",lpszDatafile);
		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
			cFit_Info.ReadFromFile(lpszDatafile);
		uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);

		strcpy(lpszOutput_File_Prefix,lpszDatafile);
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--seriesfile"))
	{
		char lpszDatafile[256];
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--seriesfile", lpszDatafile, 256, NULL);
		
		strcpy(lpszTarget_Path,lpszDatafile);
		unsigned int uiI = strlen(lpszTarget_Path);
		while (lpszTarget_Path[uiI] != '/' && uiI > 0)
			uiI--;
		if (uiI != 0)
		{
			uiI++;
			lpszTarget_Path[uiI] = 0;// 
		}
		else
			strcpy(lpszTarget_Path,"./");
		strcpy(lpszTarget_File,&lpszDatafile[uiI]);
		sprintf(lpszOutput_Binary_Filename,"%sbin",lpszDatafile);
		if (cFit_Info.ReadBinary(lpszOutput_Binary_Filename,true) > 0)
			cFit_Info.ReadFromFile(lpszDatafile);
		uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);

		strcpy(lpszOutput_File_Prefix,lpszDatafile);
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--expfit"))
	{
		if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--HVF-log-tau"))
		{
			cFit_Info.SetBestFitFile("exphvf");
			cFit_Info.bBest_Fit_With_Shell = true;
			cFit_Info.iBest_Fit_File = -1;
		}
		else
		{
			cFit_Info.SetBestFitFile("expps");
			cFit_Info.bBest_Fit_With_Shell = true;
			cFit_Info.iBest_Fit_File = -2;
		}
		cFit_Info.dBest_Fit_PS_Vel =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel",dNan);
		cFit_Info.dBest_Fit_PS_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-temp",dNan);
		cFit_Info.dBest_Fit_PS_Log_Tau =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-log-tau",dNan);
		cFit_Info.dBest_Fit_PS_Ion_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-temp",dNan);
		cFit_Info.dBest_Fit_PS_Ion_Vmin =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-vmin",dNan);
		cFit_Info.dBest_Fit_PS_Ion_Vmax =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-vmax",dNan);
		cFit_Info.dBest_Fit_PS_Ion_Aux =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-aux",dNan);

		if (cFit_Info.bBest_Fit_With_Shell)
		{
			cFit_Info.dBest_Fit_HVF_Log_Tau =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-log-tau",dNan);
			cFit_Info.dBest_Fit_HVF_Ion_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-temp",dNan);
			cFit_Info.dBest_Fit_HVF_Ion_Vmin =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-vmin",dNan);
			cFit_Info.dBest_Fit_HVF_Ion_Vmax =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-vmax",dNan);
			cFit_Info.dBest_Fit_HVF_Ion_Aux =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-aux",dNan);
		}
		uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);

		sprintf(lpszOutput_File_Prefix,"profile.%s",cFit_Info.lpszBest_Fit_File);
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--sourcedir"))
	{
		char lpszSourceDir[256];

		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--sourcedir",lpszSourceDir,256,NULL);
		strcpy(lpszTarget_Path,lpszSourceDir);
		unsigned int uiI = strlen(lpszTarget_Path);
		while (lpszTarget_Path[uiI] != '/' && uiI > 0)
			uiI--;
		if (uiI != 0)
		{
			uiI++;
			lpszTarget_Path[uiI] = 0;// 
		}
		else
			strcpy(lpszTarget_Path,"./");
		strcpy(lpszTarget_File,&lpszSourceDir[uiI]);

		cFit_Info.SetBestFitFile(lpszSourceDir);
		cFit_Info.iBest_Fit_File = 0;

		cFit_Info.dBest_Fit_Day =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--day",dNan);
		cFit_Info.dBest_Fit_PS_Vel =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel",dNan);
		cFit_Info.dBest_Fit_PS_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-temp",dNan);
		cFit_Info.dBest_Fit_PS_Log_Tau =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-log-tau",dNan);
		cFit_Info.dBest_Fit_PS_Ion_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-ion-temp",dNan);

		cFit_Info.bBest_Fit_With_Shell = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-log-tau");
		
		if (cFit_Info.bBest_Fit_With_Shell)
		{
			cFit_Info.dBest_Fit_HVF_Log_Tau =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-log-tau",dNan);
			cFit_Info.dBest_Fit_HVF_Ion_Temp =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-ion-temp",dNan);
		}
		uiIon = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--ion",-1);
		sprintf(lpszOutput_File_Prefix,"%s/profile",cFit_Info.lpszBest_Fit_File);
	}
	else
		fprintf(stderr,"Command line read error\n");

	// make sure all necessary data is available to generate the spectrum
	if (cFit_Info.iBest_Fit_File == 0)
	{
		bData_Valid = !std::isnan(cFit_Info.dBest_Fit_Day) && !std::isnan(cFit_Info.dBest_Fit_PS_Vel) && !std::isnan(cFit_Info.dBest_Fit_PS_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Temp);
		if (cFit_Info.bBest_Fit_With_Shell)
			bData_valid &= !std::isnan(cFit_Info.dBest_Fit_HVF_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Temp);
	}
	else
	{
		bData_Valid = !std::isnan(cFit_Info.dBest_Fit_PS_Vel) && !std::isnan(cFit_Info.dBest_Fit_PS_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Temp) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Vmin) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Vmax) && !std::isnan(cFit_Info.dBest_Fit_PS_Ion_Aux);
		if (cFit_Info.bBest_Fit_With_Shell)
			bData_Valid &= !std::isnan(cFit_Info.dBest_Fit_HVF_Log_Tau) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Temp) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Vmin) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Vmax) && !std::isnan(cFit_Info.dBest_Fit_HVF_Ion_Aux);
	}
	bData_Valid &= (uiIon != -1);

	double	dNorm_WL;
	if (bData_Valid)
	{
        ES::Spectrum cTarget;
		if (lpszTarget_Complete_Path[0] != 0)
		{
			cTarget = ES::Spectrum::create_from_ascii_file( lpszTarget_Complete_Path );
			cFit_Info.cBest_Fit_Spectrum = cTarget;
			cFit_Info.cBest_Fit_Spectrum.zero_flux();
//			cSpectrum_No_PS = cTarget;
//			cSpectrum_No_PS.zero_flux();
//			cSpectrum_No_HVF = cTarget;
//			cSpectrum_No_HVF.zero_flux();
		}
		else
		{
			cTarget = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 5.0);
			cFit_Info.cBest_Fit_Spectrum = cTarget;
//			cSpectrum_No_PS = cTarget;
//			cSpectrum_No_PS.zero_flux();
//			cSpectrum_No_HVF = cTarget;
//			cSpectrum_No_HVF.zero_flux();
		}
		switch (eFit_Region)
		{
		case CANIR:
			dFit_Range_Min_WL = 7750.0;
			dFit_Range_Max_WL = 8400.0;
			dNorm_WL = 7750.0;
//			dWL_Ref = (8203.97 + 8251.07 + 8256.99) / 3.0;
			dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
			break;
		case CAHK:
			dNorm_WL = 3750.0;
			dFit_Range_Min_WL = 3500.0;
			dFit_Range_Max_WL = 3950.0;
			dWL_Ref = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
//			dWL_Ref = (3934.777  + 3969.592) * 0.5;
			break;
		case SI5968:
			dNorm_WL = 5750.0;
			dFit_Range_Min_WL = 5600.0;
			dFit_Range_Max_WL = 5950.0;
//			dWL_Ref = (5959.21  + 5980.59) * 0.5;
			dWL_Ref = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
			break;
		case SI6355:
			dNorm_WL = 5750.0;
			dFit_Range_Min_WL = 5700.0;
			dFit_Range_Max_WL = 6350.0;
			dWL_Ref = (6348.85 * pow(10.0,0.149)  + 6373.12 * pow(10.0,-0.082)) / (pow(10.0,0.149) + pow(10.0,-0.082));
			break;
		case OINIR:
			dNorm_WL = 7100.0;
			dFit_Range_Min_WL = 7100.0;
			dFit_Range_Max_WL = 7800.0;
			dWL_Ref = (7774.083 * pow(10.0,0.369) + 7776.305 * pow(10.0,0.223) + 7777.528 * pow(10.0,0.002)) / (pow(10.0,0.369) + pow(10.0,0.223) + pow(10.0,0.002));
//			dWL_Ref = (7774.083  + 7776.305 + 7777.528) / 3.0;
			break;
		case OTHER:
			dFit_Range_Min_WL = cTarget.wl(0);
			dFit_Range_Max_WL = cTarget.wl(cTarget.size() - 1);
			dNorm_WL = (cTarget.wl(0) + cTarget.wl(cTarget.size() - 1)) * 0.5;
			dWL_Ref = (dFit_Range_Max_WL * 2.0 + dFit_Range_Min_WL) / 3.0;
			break;
		}
		if (bNo_Fit && !bForce_Generate)
		{	// check to see if file is available with the fit.  This will save some processing time.
			sprintf(lpszOutput_Filename,"%s.user.csv",lpszOutput_File_Prefix);
			FILE * fileSpectrum = fopen(lpszOutput_Filename,"rt");
			if (fileSpectrum)
			{
				fclose(fileSpectrum);
				cFit_Info.cBest_Fit_Spectrum = ES::Spectrum::create_from_ascii_file( lpszOutput_Filename );
				bSpectrum_Pre_Loaded = true;
				printf("Pre loaded spectrum\n");
			}
		}


		if (cFit_Info.iBest_Fit_File == 0)
		{
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			XDATASET cOpacity_Map_Shell;
			XDATASET cOpacity_Map_Ejecta;
			switch (eFit_Region)
			{
			case CANIR:
			case CAHK:
			case SI6355:
			case SI5968:
				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",cFit_Info.lpszBest_Fit_File);
				break;
			case OINIR:
				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",cFit_Info.lpszBest_Fit_File);
				break;
			case OTHER:
				if (uiIon / 100 >= 14 && uiIon / 100 <= 20)
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Si.xdataset",cFit_Info.lpszBest_Fit_File);
				else if (uiIon / 100 == 6)
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.C.xdataset",cFit_Info.lpszBest_Fit_File);
				else if (uiIon / 100 == 8)
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.O.xdataset",cFit_Info.lpszBest_Fit_File);
				else if (uiIon / 100 >= 22)
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Fe.xdataset",cFit_Info.lpszBest_Fit_File);
				else if (uiIon / 100 >= 10 && uiIon / 100 <= 12)
					sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.Mg.xdataset",cFit_Info.lpszBest_Fit_File);
				break;
			}

			sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",cFit_Info.lpszBest_Fit_File);
			cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
			cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);
			bNo_Shell = cOpacity_Map_Shell.GetNumElements() == 0;
			printf("Starting: Scalar fitting %s\n",bDisallow_Scalar_Fit ? "Disabled" : "Enabled");
			if (!bDisable_P_Cygni_Dynamic_Inclusion)
			{ // dynamically increase the fit range to account for a significant portion of the local P Cygni emission
				unsigned int uiI = 0;
				while (cTarget.wl(uiI) < (dFit_Range_Max_WL - 100.0) && uiI < cTarget.size())
					uiI++;
				double	dMax_Flux = 0.0;
				double	dMax_Flux_WL = 0.0;
				unsigned int uiMax_Flux_Idx = 0;
				while (cTarget.wl(uiI) < (dFit_Range_Max_WL + 300.0) && uiI < cTarget.size())
				{
					if (cTarget.flux(uiI) > dMax_Flux)
					{
						dMax_Flux = cTarget.flux(uiI);
						dMax_Flux_WL = cTarget.wl(uiI);
						uiMax_Flux_Idx = uiI;
					}
					uiI++;
				}
				uiI = uiMax_Flux_Idx;
				dFit_Range_Max_WL = dMax_Flux_WL + 200.0;
			}


			CompareFits(cTarget, dTarget_MJD,cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, dFit_Range_Min_WL, dFit_Range_Max_WL, cFit_Info, bNo_Fit, bSpectrum_Pre_Loaded, bAllow_Ion_Temp_Fit, bDisallow_Scalar_Fit,dWL_Ref, dUpper_HVF_Scalar_Bound);
		}
		else if (cFit_Info.iBest_Fit_File == -1)
		{
			bNo_Shell = false;
			CompareFitsExp(cTarget, uiIon, true, cFit_Info.dBest_Fit_PS_Vel, cFit_Info.dBest_Fit_PS_Temp, cFit_Info.dBest_Fit_PS_Ion_Temp, cFit_Info.dBest_Fit_PS_Log_Tau, cFit_Info.dBest_Fit_PS_Ion_Vmin, cFit_Info.dBest_Fit_PS_Ion_Vmax, cFit_Info.dBest_Fit_PS_Ion_Aux, cFit_Info.dBest_Fit_HVF_Ion_Temp, cFit_Info.dBest_Fit_HVF_Log_Tau, cFit_Info.dBest_Fit_HVF_Ion_Vmin, cFit_Info.dBest_Fit_HVF_Ion_Vmax, cFit_Info.dBest_Fit_HVF_Ion_Aux,  cFit_Info.cBest_Fit_Spectrum, cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF, bNo_Fit,bSpectrum_Pre_Loaded);
		}
		else
		{
			bNo_Shell = true;
			CompareFitsExp(cTarget, uiIon, false, cFit_Info.dBest_Fit_PS_Vel, cFit_Info.dBest_Fit_PS_Temp, cFit_Info.dBest_Fit_PS_Ion_Temp, cFit_Info.dBest_Fit_PS_Log_Tau, cFit_Info.dBest_Fit_PS_Ion_Vmin, cFit_Info.dBest_Fit_PS_Ion_Vmax, cFit_Info.dBest_Fit_PS_Ion_Aux, cFit_Info.dBest_Fit_HVF_Ion_Temp, cFit_Info.dBest_Fit_HVF_Log_Tau, cFit_Info.dBest_Fit_HVF_Ion_Vmin, cFit_Info.dBest_Fit_HVF_Ion_Vmax, cFit_Info.dBest_Fit_HVF_Ion_Aux,  cFit_Info.cBest_Fit_Spectrum, cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF, bNo_Fit,bSpectrum_Pre_Loaded);
		}

		
		if (cFit_Info.iBest_Fit_File >= 0)
		{
			if (cFit_Info.bBest_Fit_With_Shell)
				sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f thv %.2f Thv %.2f",cFit_Info.lpszBest_Fit_File,cFit_Info.dBest_Fit_Day,cFit_Info.dBest_Fit_PS_Vel,cFit_Info.dBest_Fit_PS_Temp,cFit_Info.dBest_Fit_PS_Log_Tau,cFit_Info.dBest_Fit_PS_Ion_Temp,cFit_Info.dBest_Fit_HVF_Log_Tau,cFit_Info.dBest_Fit_HVF_Ion_Temp);
			else
				sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f",cFit_Info.lpszBest_Fit_File,cFit_Info.dBest_Fit_Day,cFit_Info.dBest_Fit_PS_Vel,cFit_Info.dBest_Fit_PS_Temp,cFit_Info.dBest_Fit_PS_Log_Tau,cFit_Info.dBest_Fit_PS_Ion_Temp);
		}
		else
		{
			if (cFit_Info.bBest_Fit_With_Shell)
				sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f thv %.2f Thv %.2f hvfvm %.2f hvfa %.2f",cFit_Info.lpszBest_Fit_File,cFit_Info.dBest_Fit_PS_Vel,cFit_Info.dBest_Fit_PS_Temp,cFit_Info.dBest_Fit_PS_Log_Tau,cFit_Info.dBest_Fit_PS_Ion_Temp,cFit_Info.dBest_Fit_PS_Ion_Vmin,cFit_Info.dBest_Fit_PS_Ion_Aux,cFit_Info.dBest_Fit_HVF_Log_Tau,cFit_Info.dBest_Fit_HVF_Ion_Temp,cFit_Info.dBest_Fit_HVF_Ion_Vmin,cFit_Info.dBest_Fit_HVF_Ion_Aux);
			else
				sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f",cFit_Info.lpszBest_Fit_File,cFit_Info.dBest_Fit_PS_Vel,cFit_Info.dBest_Fit_PS_Temp,cFit_Info.dBest_Fit_PS_Log_Tau,cFit_Info.dBest_Fit_PS_Ion_Temp,cFit_Info.dBest_Fit_PS_Ion_Vmin,cFit_Info.dBest_Fit_PS_Ion_Aux);
		}

		epsplot::PAGE_PARAMETERS	cPlot_Parameters;
		epsplot::DATA cPlot;

		cPlot_Parameters.m_uiNum_Columns = 1;
		cPlot_Parameters.m_uiNum_Rows = 1;
		cPlot_Parameters.m_dWidth_Inches = 11.0;
		cPlot_Parameters.m_dHeight_Inches = 8.5;

		unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
		unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Flux", false, false, false, 0.2, false, 2.0);
//		if (!bNo_Shell)
//			cPlot.Set_Num_Plots(4);
//		else
//			cPlot.Set_Num_Plots(2);

		cPlot.Set_Plot_Title(bNo_Title ? NULL : lpszTitle,18.0);
		double	dStipple[2] = {16,16}; // long long dash
		cPlot.Define_Custom_Stipple(epsplot::STPL_CUSTOM_1,dStipple,2);

		double dTarget_Minima_WL, dTarget_Minima_Flux, dFit_Minima_WL, dFit_Minima_Flux;

		// Find the minima of the line used for fitting
//		Find_Flux_Minimum(cTarget, dTarget_Minima_WL, dTarget_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
//		Find_Flux_At_WL(cFit_Info.cBest_Fit_Spectrum,dTarget_Minima_WL, dNormalization_Flux_Fit);
//		Find_Flux_Minimum(cFit_Info.cBest_Fit_Spectrum,dFit_Minima_WL, dFit_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);

		printf("Plot full\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.full.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux, cTarget.wl(0), cTarget.wl(cTarget.size()- 1),bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Ca NIR\n");
		epsplot::AXIS_PARAMETERS cAxis_Param_Save = cPlot.Get_Y_Axis_Parameters(uiY_Axis);
		epsplot::AXIS_PARAMETERS cAxis_Param_Mod = cAxis_Param_Save;
		cAxis_Param_Mod.m_dLower_Limit = 0.2;
		cAxis_Param_Mod.m_dUpper_Limit = 2.0;

		cPlot.Modify_Y_Axis_Parameters(uiY_Axis, cAxis_Param_Mod);
		Plot(cPlot_Parameters, cPlot, "%s.user.CaNIR.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,7500.0, 9000.0,bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Ca H&K\n");
		cPlot.Modify_Y_Axis_Parameters(uiY_Axis, cAxis_Param_Save);
		Plot(cPlot_Parameters, cPlot, "%s.user.CaHK.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,3000.0, 5000.0,bNo_Shell, uiX_Axis, uiY_Axis);

		printf("Plot Si 6355\n");
		Plot(cPlot_Parameters, cPlot, "%s.user.Si6355.eps", lpszOutput_File_Prefix, cTarget, cFit_Info.cBest_Fit_Spectrum,  cFit_Info.cBest_Fit_Spectrum_No_PS, cFit_Info.cBest_Fit_Spectrum_No_HVF,  g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux,5000.0, 7000.0,bNo_Shell, uiX_Axis, uiY_Axis);



		if (bCalc_Fit)
		{
			printf("Ca H&K Fit: %.2e\n",Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, 3700.0, 4500.0,2));
			printf("Si 6355 Fit: %.2e\n",Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, 5700.0, 6500.0,2));
			printf("Ca NIR Fit: %.2e\n",Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, 7700.0, 8500.0,2));
			printf("Overall Fit: %.2e\n",Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, cTarget.wl(0), cTarget.wl(cTarget.size() - 1),2));
			printf("Range Fit:  %.2e\n",Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, dFit_Range_Min_WL, dFit_Range_Max_WL,2));
			cFit_Info.dBest_Fit = Get_Fit(cTarget, cFit_Info.cBest_Fit_Spectrum, dFit_Range_Min_WL, dFit_Range_Max_WL,2);
		}
			
		sprintf(lpszOutput_Filename,"%s.user.data",lpszOutput_File_Prefix);
		sprintf(lpszOutput_Binary_Filename,"%s.user.databin",lpszOutput_File_Prefix);
		if (!bInhibit_Resave)
		{
			cFit_Info.Output(lpszOutput_Filename,false);
			cFit_Info.OutputBinary(lpszOutput_Binary_Filename);
		}
		sprintf(lpszOutput_Filename,"%s.user.csv",lpszOutput_File_Prefix);
		FILE * fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum.wl(uiI), cFit_Info.cBest_Fit_Spectrum.flux(uiI), cFit_Info.cBest_Fit_Spectrum.flux_error(uiI));
		fclose(fileOut);
		
		sprintf(lpszOutput_Filename,"%s.user.shell.csv",lpszOutput_File_Prefix);
		fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum_No_PS.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum_No_PS.wl(uiI), cFit_Info.cBest_Fit_Spectrum_No_PS.flux(uiI), cFit_Info.cBest_Fit_Spectrum_No_PS.flux_error(uiI));
		fclose(fileOut);

		sprintf(lpszOutput_Filename,"%s.user.shell.flattened.csv",lpszOutput_File_Prefix);
		fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum_No_PS.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum_No_PS.wl(uiI), cFit_Info.cBest_Fit_Spectrum_No_PS.flux(uiI) / cFit_Info.cBest_Fit_Spectrum_Continuum.flux(uiI), cFit_Info.cBest_Fit_Spectrum_No_PS.flux_error(uiI));
		fclose(fileOut);

		sprintf(lpszOutput_Filename,"%s.user.ejecta.csv",lpszOutput_File_Prefix);
		fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum_No_HVF.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum_No_HVF.wl(uiI), cFit_Info.cBest_Fit_Spectrum_No_HVF.flux(uiI), cFit_Info.cBest_Fit_Spectrum_No_HVF.flux_error(uiI));
		fclose(fileOut);
	
		sprintf(lpszOutput_Filename,"%s.user.ejecta.flattened.csv",lpszOutput_File_Prefix);
		fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum_No_HVF.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum_No_HVF.wl(uiI), cFit_Info.cBest_Fit_Spectrum_No_HVF.flux(uiI) / cFit_Info.cBest_Fit_Spectrum_Continuum.flux(uiI), cFit_Info.cBest_Fit_Spectrum_No_HVF.flux_error(uiI));
		fclose(fileOut);

		sprintf(lpszOutput_Filename,"%s.user.continuum.csv",lpszOutput_File_Prefix);
		fileOut = fopen(lpszOutput_Filename,"wt");
		for (unsigned int uiI = 0; uiI < cFit_Info.cBest_Fit_Spectrum_Continuum.size(); uiI++)
			fprintf(fileOut, "%f %f %f\n",cFit_Info.cBest_Fit_Spectrum_Continuum.wl(uiI), cFit_Info.cBest_Fit_Spectrum_Continuum.flux(uiI), cFit_Info.cBest_Fit_Spectrum_Continuum.flux_error(uiI));
		fclose(fileOut);
	}
	else
	{
		fprintf(stderr,"Failed\n");
		strcat(lpszOutput_Filename,".userprof.errfile");
		cFit_Info.Output(lpszOutput_Filename,false);
	}
	return 0;
}

