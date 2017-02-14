#include <math.h>
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
#include <float.h>
#include <line_routines.h>
#include <stack>

///////////////////////////////////////////////////////////////////////////////
//
// functions that compute the statistical moments of a fit
//
///////////////////////////////////////////////////////////////////////////////

bool ValidateChi2(const ES::Spectrum &i_cTarget,const double &i_dMin_WL, const double &i_dMax_WL)
{
	bool bChi2 = true;
	unsigned int uiSource_Idx = 0;
	bool	bInvalid_Error = true;
	// Find lower bound for source data
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) <= i_dMax_WL && bChi2)
	{
		bChi2 &= (i_cTarget.flux_error(uiSource_Idx) != 0);
		bInvalid_Error &= (i_cTarget.flux_error(uiSource_Idx) == 0.0 || i_cTarget.flux_error(uiSource_Idx) == 1.0);
		uiSource_Idx++;
	}
	bChi2 &= !bInvalid_Error; // if the flux error is uniformly 0 or 1, don't use it
	return bChi2;
}
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, double * o_dMoments) // moments must be large enough to fit results
{
	memset(o_dMoments,0,sizeof(double) * i_uiNum_Moments);
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		double dXprod = i_dX[uiI];
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_dMoments[uiM] += dXprod;
			dXprod *=  i_dX[uiI];
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_dMoments[uiM] /= i_uiNum_Points;
}
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments)
{
	o_vdMoments.clear();
	o_vdMoments.resize(i_uiNum_Moments,0.0);
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		double dXprod = i_dX[uiI];
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_vdMoments[uiM] += dXprod;
			dXprod *=  i_dX[uiI];
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_vdMoments[uiM] /= i_uiNum_Points;
}
void Get_Raw_Moments(const std::vector<double> &i_vdX, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments)
{
	o_vdMoments.clear();
	o_vdMoments.resize(i_uiNum_Moments,0.0);
	for (std::vector<double>::const_iterator iterI = i_vdX.begin(); iterI != i_vdX.end(); iterI++)
	{
		double dXprod = *iterI;
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_vdMoments[uiM] += dXprod;
			dXprod *=  *iterI;
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_vdMoments[uiM] /= i_vdX.size();
}
void Get_Central_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dCentral_Moments)
{
	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dCentral_Moments[5] = i_dMoments[5] + ((((-5.0 * i_dMoments[0] * i_dMoments[0] + 15.0 * i_dMoments[1]) * i_dMoments[0] - 20.0 * i_dMoments[2]) * i_dMoments[0] + 15.0 * i_dMoments[3]) * i_dMoments[0] - 6.0 * i_dMoments[4]) * i_dMoments[0];
	case 5:
		o_dCentral_Moments[4] = i_dMoments[4] + (((4.0 * i_dMoments[0] * i_dMoments[0] - 10.0 * i_dMoments[1]) * i_dMoments[0] + 10.0 * i_dMoments[2]) * i_dMoments[0] - 5.0 * i_dMoments[3]) * i_dMoments[0];
	case 4:
		o_dCentral_Moments[3] = i_dMoments[3] + ((-3.0 * i_dMoments[0] * i_dMoments[0]  + 6.0 * i_dMoments[1]) * i_dMoments[0] - 4.0 * i_dMoments[2]) * i_dMoments[0];
	case 3:
		o_dCentral_Moments[2] = i_dMoments[2] + (2.0 * i_dMoments[0] * i_dMoments[0]  - 3.0 * i_dMoments[1]) * i_dMoments[0];
	case 2:
		o_dCentral_Moments[1] = i_dMoments[1] - i_dMoments[0] * i_dMoments[0];
	case 1:
		o_dCentral_Moments[0] = 0.0;
	case 0:
		break;
	}
}
void Get_Central_Moments(const std::vector<double> & i_vdMoments, std::vector<double> & o_vdMoments)
{
	std::stack<double> stackMoments;
	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_vdMoments.size())
	{
	default:
	case 6:
		stackMoments.push(i_vdMoments[5] + ((((-5.0 * i_vdMoments[0] * i_vdMoments[0] + 15.0 * i_vdMoments[1]) * i_vdMoments[0] - 20.0 * i_vdMoments[2]) * i_vdMoments[0] + 15.0 * i_vdMoments[3]) * i_vdMoments[0] - 6.0 * i_vdMoments[4]) * i_vdMoments[0]);
	case 5:
		stackMoments.push(i_vdMoments[4] + (((4.0 * i_vdMoments[0] * i_vdMoments[0] - 10.0 * i_vdMoments[1]) * i_vdMoments[0] + 10.0 * i_vdMoments[2]) * i_vdMoments[0] - 5.0 * i_vdMoments[3]) * i_vdMoments[0]);
	case 4:
		stackMoments.push(i_vdMoments[3] + ((-3.0 * i_vdMoments[0] * i_vdMoments[0]  + 6.0 * i_vdMoments[1]) * i_vdMoments[0] - 4.0 * i_vdMoments[2]) * i_vdMoments[0]);
	case 3:
		stackMoments.push(i_vdMoments[2] + (2.0 * i_vdMoments[0] * i_vdMoments[0]  - 3.0 * i_vdMoments[1]) * i_vdMoments[0]);
	case 2:
		stackMoments.push(i_vdMoments[1] - i_vdMoments[0] * i_vdMoments[0]);
	case 1:
		stackMoments.push(0.0);
	case 0:
		break;
	}
	while (!stackMoments.empty())
	{
		o_vdMoments.push_back(stackMoments.top());
		stackMoments.pop();
	}

}
void Get_Standardized_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dStandardized_Moments)
{
	double	* lpdCentral_Moments = new double[i_uiNum_Moments];
	Get_Central_Moments(i_uiNum_Moments, i_dMoments,lpdCentral_Moments);

	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dStandardized_Moments[5] = lpdCentral_Moments[5] / pow(lpdCentral_Moments[1],3.0);
	case 5:
		o_dStandardized_Moments[4] = lpdCentral_Moments[4] / pow(lpdCentral_Moments[1],2.5);
	case 4:
		o_dStandardized_Moments[3] = lpdCentral_Moments[3] / pow(lpdCentral_Moments[1],2.0);
	case 3:
		o_dStandardized_Moments[2] = lpdCentral_Moments[2] / pow(lpdCentral_Moments[1],1.5);
	case 2:
		o_dStandardized_Moments[1] = 1.0;
	case 1:
		o_dStandardized_Moments[0] = 0.0;
	case 0:
		break;
	}
	delete [] lpdCentral_Moments;
}
void Get_Standardized_Moments(const std::vector<double> & i_vdRaw_Moments, std::vector<double> & o_vdMoments)
{
	std::stack<double> stackMoments;
	std::vector<double> vCentral_Moments;
	Get_Central_Moments(i_vdRaw_Moments,vCentral_Moments);

	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_vdRaw_Moments.size())
	{
	default:
	case 6:
		stackMoments.push(vCentral_Moments[5] / pow(vCentral_Moments[1],3.0));
	case 5:
		stackMoments.push(vCentral_Moments[4] / pow(vCentral_Moments[1],2.5));
	case 4:
		stackMoments.push(vCentral_Moments[3] / pow(vCentral_Moments[1],2.0));
	case 3:
		stackMoments.push(vCentral_Moments[2] / pow(vCentral_Moments[1],1.5));
	case 2:
		stackMoments.push(1.0);
	case 1:
		stackMoments.push(0.0);
	case 0:
		break;
	}
	while (!stackMoments.empty())
	{
		o_vdMoments.push_back(stackMoments.top());
		stackMoments.pop();
	}
}


void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments)
{
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);
		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];

		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			lpdErrors[uiI] = dError;
		}

		if (o_lpdRaw_Moments)
			delete [] o_lpdRaw_Moments;
		if (o_lpdCentral_Moments)
			delete [] o_lpdCentral_Moments;
		if (o_lpdStandardized_Moments)
			delete [] o_lpdStandardized_Moments;

		o_lpdRaw_Moments = new double [i_uiNum_Moments];
		o_lpdCentral_Moments = new double [i_uiNum_Moments];
		o_lpdStandardized_Moments = new double [i_uiNum_Moments];

		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
		Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
		Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

		delete [] lpdErrors;
	}
}


void Bracket_Wavelength(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int &o_uiLower_Bound, unsigned int &o_uiUpper_Bound, unsigned int i_uiSuggested_Starting_Idx)
{
	while (i_cData.wl(i_uiSuggested_Starting_Idx) <= i_dTarget_Wavelength && i_uiSuggested_Starting_Idx < i_cData.size())
		i_uiSuggested_Starting_Idx++;
//	if (i_uiSuggested_Starting_Idx == 0)
//		printf("%.1f %.1f\n",i_dTarget_Wavelength,i_cData.wl(i_uiSuggested_Starting_Idx));
//	printf("%i %i\t",i_uiSuggested_Starting_Idx,i_cData.size());
	if (i_uiSuggested_Starting_Idx < i_cData.size())
	{
		if (i_uiSuggested_Starting_Idx != 0)
		{
			o_uiLower_Bound = i_uiSuggested_Starting_Idx - 1;
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
	}
	else
	{
		if (i_cData.wl(i_uiSuggested_Starting_Idx - 1) <= i_dTarget_Wavelength)
		{
			o_uiLower_Bound = i_cData.size() - 1;
			o_uiUpper_Bound = i_cData.size();
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_cData.size();
		}
	}
//	printf("%i %i\n",o_uiLower_Bound,o_uiUpper_Bound);
}

double Interpolate_Flux(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int i_uiLower_Bound, unsigned int i_uiUpper_Bound)
{
	double	dFlux = nan("");
	if (i_uiLower_Bound != i_uiUpper_Bound && i_uiUpper_Bound < i_cData.size())
	{ 
		double dSlope = (i_cData.flux(i_uiLower_Bound + 1) - i_cData.flux(i_uiLower_Bound)) / (i_cData.wl(i_uiLower_Bound + 1) - i_cData.wl(i_uiLower_Bound));
		dFlux = (i_dTarget_Wavelength - i_cData.wl(i_uiLower_Bound)) * dSlope + i_cData.flux(i_uiLower_Bound);
	}
//	else
//		printf("%i %i %i\n",i_uiLower_Bound,i_uiUpper_Bound,i_cData.size());
	return dFlux;
}

void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	double * lpdErrors = new double[uiCompare_Count];
	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			lpdErrors[uiI] = dError;
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	if (o_lpdRaw_Moments)
		delete [] o_lpdRaw_Moments;
	if (o_lpdCentral_Moments)
		delete [] o_lpdCentral_Moments;
	if (o_lpdStandardized_Moments)
		delete [] o_lpdStandardized_Moments;

	o_lpdRaw_Moments = new double [i_uiNum_Moments];
	o_lpdCentral_Moments = new double [i_uiNum_Moments];
	o_lpdStandardized_Moments = new double [i_uiNum_Moments];

	Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
	Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
	Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

	delete [] lpdErrors;
}
void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, std::vector<double> & o_vRaw_Moments, std::vector<double> & o_vCentral_Moments, std::vector<double> & o_vStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	std::vector<double> vErrors;

	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			vErrors.push_back(dError);
		else
			vErrors.push_back(0.0);
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	Get_Raw_Moments(vErrors,i_uiNum_Moments,o_vRaw_Moments);
	Get_Central_Moments(o_vRaw_Moments,o_vCentral_Moments);
	Get_Standardized_Moments(o_vRaw_Moments,o_vStandardized_Moments);

}

void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiMax_Moments, std::vector<double> &o_vRaw_Moments, std::vector<double> &o_vCentral_Moments, std::vector<double> &o_vStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	double * lpdErrors = new double[uiCompare_Count];
	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			lpdErrors[uiI] = dError;
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	o_vRaw_Moments.clear();
	o_vCentral_Moments.clear();
	o_vStandardized_Moments.clear();

	Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiMax_Moments,o_vRaw_Moments);
	Get_Central_Moments(o_vRaw_Moments,o_vCentral_Moments);
	Get_Standardized_Moments(o_vRaw_Moments,o_vStandardized_Moments);

	delete [] lpdErrors;
}

double Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, FIT_TYPE i_eFit,unsigned int i_uiFit_Moment)
{
	if (i_uiFit_Moment > 0)
		i_uiFit_Moment--; // use moment is an index, so the 1st moment is index 0

	double dFit = nan("");
	double	*lpdMoments = new double [i_uiFit_Moment];
	double	*lpdExtended_Moments = new double [i_uiFit_Moment];
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);

		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];
		double	dChi2 = 0.0;
		double dErr_Sum = 0.0;
		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			if (i_eFit == FIT_CHI2)
				dChi2 += dError / (i_cTarget.flux_error(uiI + uiSource_Idx) * dSource_Normalization);
			lpdErrors[uiI] = dError;
			dErr_Sum += (dError * dError * dError);
		}
		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiFit_Moment + 1,lpdMoments);
		switch (i_eFit)
		{
		case FIT_CHI2:
			dFit = dChi2;
			break;
		case FIT_RAW:
			dFit = lpdMoments[i_uiFit_Moment];
			break;
		case FIT_CENTRAL:
			Get_Central_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		case FIT_STANDARDIZED:
			Get_Standardized_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		}
		

		delete [] lpdErrors;
	}
	else
		fprintf(stderr,"spectrafit: Mismatched target and generated files.\n");
	delete [] lpdMoments;
	delete [] lpdExtended_Moments;
	return dFit;
}


