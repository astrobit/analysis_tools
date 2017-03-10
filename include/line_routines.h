#pragma once

#include <xio.h>
#include <xlinalg.h>
#include "ES_Synow.hh"
#include <utility>
#include <vector>
#include <radiation.h>
#include <model_spectra_db.h>
#include <opacity_profile_data.h>

double	Equivalent_Width(const XDATASET & i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length);
double	Equivalent_Width(const ES::Spectrum &i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length);


class ION_DATA
{
public:
	unsigned int m_uiIon;
	XDATASET * m_lpcOpacity_Map;
	double		m_dScalar;
	double		m_dExcitation_Temp;
	double		m_dTime_Power_Law;
	ION_DATA(void)
	{
		m_uiIon = 2001;
		m_lpcOpacity_Map = NULL;
		m_dScalar = 1.0;
		m_dExcitation_Temp = 10.0;
		m_dTime_Power_Law = -2.0;
	}
};


void Generate_Synow_Multi_Ion_Spectra(const double & i_dT_days, const double & i_dPS_Temp_kK, const double & i_dPS_Velocity_kkms, ION_DATA * i_lpcIon_Data, unsigned int i_uiNum_Ions, ES::Spectrum &io_cOutput);
// xvector parameters for Generate_Synow_Spectra:
// 0: time after explosion in days
// 1: photosphere velocity in 1000 km/s
// 2: photosphere temperature in 1000 K
// 3: excitation temperature for PVF  in 1000 K
// 4: log_{10} S for PVF
// 5: excitation temperature for HVF  in 1000 K
// 6: log_{10} S for HVF
void Generate_Synow_Spectra(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput, const double & i_dTime_Power_Law_A = -2.0, const double & i_dTime_Power_Law_B = -2.0);
void Generate_Synow_Spectra_Exp(const ES::Spectrum &i_cTarget, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput);
void msdb_load_generate(msdb::USER_PARAMETERS	&i_cParam, msdb::SPECTRUM_TYPE i_eSpectrum_Type, const ES::Spectrum &i_cTarget, const xdataset * i_cOp_Map_A, const xdataset * i_cOp_Map_B, ES::Spectrum & o_cOutput);


void Find_Minimum(ES::Spectrum * i_lpcSpectra, unsigned int i_uiNum_Spectra, double * o_lpdMinima_WL, double * o_lpdMinima_Flux, const double & i_dMin_WL = -1.0, const double & i_dMax_WL = -1.0);

void Get_Spectra_Data(const ES::Spectrum & i_cSpectrum, double * &o_lpdWavelengths, double * & o_lpdFluxes, unsigned int & o_uiNum_Points, const double & i_dMin_WL, const double & i_dMax_WL);
void Find_Flux_Minimum(const double * i_lpdWavelengths, const double * i_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL = -1.0, const double & i_dMax_WL = -1.0);
void Find_Flux_Minimum(const ES::Spectrum & i_cSpectrum, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL = -1.0, const double & i_dMax_WL = -1.0);
void Find_Flux_Global_Maximum(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL);
void Find_Flux_At_WL(const ES::Spectrum & i_cSpectrum, const double & i_dWL, double & o_dMinima_Flux);
void Find_Blue_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL = -1.0, const double & i_dMax_WL = -1.0);
void Find_Red_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL = -1.0, const double & i_dMax_WL = -1.0);


void Normalize(double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_Flux);
void Normalize_At_WL(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_WL, double & o_dNormalization_Flux);
void Normalize_At_Minima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux);
void Normalize_At_Blue_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux);
void Normalize_At_Red_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux);


bool xPoint_Within_Bound(const XVECTOR &i_lpVector, const XVECTOR &i_cThreshold);
bool xConvergence(const XVECTOR * i_lpVectors, unsigned int i_uiNum_Points,const XVECTOR &i_cThreshold, XVECTOR * o_lpcConvergence_Fault = NULL);
bool xBoundTest(const XVECTOR & i_cTest_Point, const XVECTOR * i_lpcBounds);
XVECTOR xCompute_Centroid(XVECTOR * i_lpvPoints, unsigned int i_uiNum_Points);


enum FIT_TYPE {FIT_CHI2,FIT_RAW,FIT_CENTRAL,FIT_STANDARDIZED};
// Fit types refer to:
// Chi^2 will do (s(x) - g(x)) / e(x) [s = source, g = generated, e = source error]
// raw will return the raw moment r = sum(s(x) - g(x))^n/N [n = selected moment, N = # of tested points]
// central will return the c(n) = sum(s(x) - g(x) - u)^n/N [u = mean error sum(s(x) - g(x)) / N]
// standardized will return the c(n)/o^n [c = central moment, o = sqrt(c(2) - c(1)^2) = standard deviation]
// At present, central and standardized moment of values > 6 will return 0 @@TODO
// Note central and standardized moment of 1 [c(1)] = 0, standardized moment of 2 [c(2)] = 1 (by definition)
bool ValidateChi2(const ES::Spectrum &i_cTarget,const double &i_dMin_WL, const double &i_dMax_WL);
double Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, FIT_TYPE i_eFit = FIT_CENTRAL,unsigned int i_uiFit_Moment=2);

void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, double * o_dMoments); // moments must be large enough to fit results
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments);
void Get_Raw_Moments(const std::vector<double> &i_vdX, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments);
void Get_Central_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dCentral_Moments);
void Get_Central_Moments(const std::vector<double> & i_vdMoments, std::vector<double> & o_vdMoments);
void Get_Standardized_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dStandardized_Moments);
void Get_Standardized_Moments(const std::vector<double> & i_vdMoments, std::vector<double> & o_vdMoments);
// Get_Fit_Moments works on spectra that have the exact same size and wavelengths

void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments);
// Get_Fit_Moments works on spectra that have the different sizes, resolution, or wavelength data
void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments, const double & i_dTarget_Normalization_Flux = 1.0, const double & i_dGenereted_Normalization_Flux = 1.0);
void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, std::vector<double> & o_vRaw_Moments, std::vector<double> & o_vCentral_Moments, std::vector<double> & o_vStandardized_Moments, const double & i_dTarget_Normalization_Flux = 1.0, const double & i_dGenereted_Normalization_Flux = 1.0);

double Interpolate_Flux(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int i_uiLower_Bound, unsigned int i_uiUpper_Bound);
void Bracket_Wavelength(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int &o_uiLower_Bound, unsigned int &o_uiUpper_Bound, unsigned int i_uiSuggested_Starting_Idx = 0);


class SPECTRAL_DATABASE_TUPLE_NODE
{
public:
	double				m_dWavelength;
	double				m_dIntensity;

	SPECTRAL_DATABASE_TUPLE_NODE * m_lpPrev;
	SPECTRAL_DATABASE_TUPLE_NODE * m_lpNext;

	SPECTRAL_DATABASE_TUPLE_NODE(void)
	{
		m_dWavelength = -1.0;
		m_dIntensity = -1.0;
		m_lpPrev = m_lpNext = NULL;
	}
};


class SPECTRAL_DATABASE
{
private:
	unsigned int		m_uiIon;
	char *				m_lpszModel_Name;
	char *				m_lpszDatabase_Filename;
	unsigned int 		m_uiNum_Parameter_Vectors;
	XVECTOR			* 	m_lpvParameter_Data;
	SPECTRAL_DATABASE_TUPLE_NODE ** m_lplpcTuple_List_Heads;
	SPECTRAL_DATABASE_TUPLE_NODE ** m_lplpcTuple_List_Tails;

	unsigned int		Add_Parameter_Vector(const XVECTOR & i_vParameters);
	unsigned int		Find_Parameter_Vector(const XVECTOR & i_vParameters);
	void				Deallocate_Non_Base_Data(void)
	{
		if (m_lpvParameter_Data)
			delete [] m_lpvParameter_Data;
		if (m_lplpcTuple_List_Heads)
		{
			for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors; uiI++)
			{
				SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiI];
				while (lpCurr)
				{
					SPECTRAL_DATABASE_TUPLE_NODE * lpNext = lpCurr->m_lpNext;
					delete lpCurr;
					lpCurr = lpNext;
				}
			}
		}
	}
	void				Deallocate(void)
	{
		if (m_lpszModel_Name)
			delete [] m_lpszModel_Name;
		if (m_lpszDatabase_Filename)
			delete [] m_lpszDatabase_Filename;
		Deallocate_Non_Base_Data();
		Zero(); // make sure this is done after deallocation
	}
	void				Zero(void)
	{
		m_uiIon = 0;
		m_lpszModel_Name = NULL;
		m_lpszDatabase_Filename = NULL;
		Zero_Non_Base_Data();
	}
	void				Zero_Non_Base_Data(void)
	{
		m_uiNum_Parameter_Vectors = 0;
		m_lpvParameter_Data = NULL;
		m_lplpcTuple_List_Heads = NULL;
		m_lplpcTuple_List_Tails = NULL;
	}
	unsigned int		Read_Database(void); // returns fault code (see function for descriptions).  0 = no fault
	unsigned int		Save_Database(void);
public:
	void	Set_Base_Data(unsigned int i_uiIon, const char * i_lpszModel_Name);
	void	Get_Base_Data(unsigned int &o_uiIon, char * o_lpszModel_Name, unsigned int i_uiMax_Model_Name_Length);
	void 	Add_Spectrum(const XVECTOR & i_vParameters, const ES::Spectrum &i_cOutput);
	bool 	Find_Spectrum(const XVECTOR & i_vParameters, ES::Spectrum & io_cOutput, bool i_bAllow_Interpolate, bool i_bGenerate_Wavelength_List); // allow interpolate flag will return f(wl) interpolated between known nearby wl if the exact wl value is not available.  Generate wavelength list flag will fill io_cOutput with all available data, ignoring the wavelength list that is sent as input.
	SPECTRAL_DATABASE(void)
	{
		Zero();
	}
	~SPECTRAL_DATABASE(void)
	{
		Deallocate();
	}
};


class SERIESLISTNODE
{
public:
	char *  m_lpszFilename;
	double	m_dDay;

	SERIESLISTNODE * m_lpcNext;
	SERIESLISTNODE * m_lpcPrev;

	SERIESLISTNODE(void)
	{
		m_lpszFilename = NULL;
		m_dDay = -50.0;
		m_lpcNext = m_lpcPrev = NULL;
	}
};
class SERIESLIST
{ // container class for listing multiple source data
public:
	SERIESLISTNODE * m_lpHead;
	SERIESLISTNODE * m_lpTail;
	SERIESLIST(void)
	{
		m_lpHead = m_lpTail = NULL;
	}
};

enum ABUNDANCE_TYPE {Solar,Seitenzahl_N100_2013,CO_Rich,Seitenzahl_Ca_Rich};

class ABUNDANCE_LIST
{
public:
	double	m_dAbundances[128]; // ordered by z; neutrons not included; no differentiation between isotopes
	double	m_dUncertainties[128];

	void	Read_Table(const char * i_lpszFilename);
	void	Read_Table(ABUNDANCE_TYPE i_eType);
	void	Normalize_Groups(void);
};

 // parameters for fitting a gaussian to a spectral absorption feature

class gauss_fit_parameters
{
public:
	double	m_dWl[3];
	double	m_dStr[3];
	double	m_dW_Ratio_1;
	double	m_dW_Ratio_2;
	
	double	m_dH_Ratio_1;
	double	m_dH_Ratio_2;

	bool operator == (const gauss_fit_parameters & i_RHO)
	{
		return (m_dWl[0] == i_RHO.m_dWl[0] && m_dWl[1] == i_RHO.m_dWl[1] && m_dWl[2] == i_RHO.m_dWl[2] &&
				m_dStr[0] == i_RHO.m_dStr[0] && m_dStr[1] == i_RHO.m_dStr[1] && m_dStr[2] == i_RHO.m_dStr[2]);

	}
	bool operator != (const gauss_fit_parameters & i_RHO)
	{
		return (m_dWl[0] != i_RHO.m_dWl[0] || m_dWl[1] != i_RHO.m_dWl[1] || m_dWl[2] != i_RHO.m_dWl[2] &&
				m_dStr[0] != i_RHO.m_dStr[0] || m_dStr[1] != i_RHO.m_dStr[1] || m_dStr[2] != i_RHO.m_dStr[2]);

	}
	gauss_fit_parameters(void)
	{
		m_dW_Ratio_1 = m_dW_Ratio_2 = m_dH_Ratio_1 = m_dH_Ratio_2 = 0.0;
	}

	gauss_fit_parameters(const double & i_dWL_1, const double & i_dStrength_1, const double & i_dWL_2, const double & i_dStrength_2, const double & i_dWL_3 = 0.0, const double & i_dStrength_3 = 0.0)
	{
		if (i_dStrength_3 > 0.0)
		{
			if (i_dStrength_1 < i_dStrength_2 && i_dStrength_3 < i_dStrength_2)
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
				m_dH_Ratio_2 = i_dStrength_3 / i_dStrength_2;
			}
			else if (i_dStrength_2 < i_dStrength_1 && i_dStrength_3 < i_dStrength_1)
			{
				m_dWl[0] = i_dWL_2;
				m_dWl[1] = i_dWL_1;
				m_dWl[2] = i_dWL_3;

				m_dStr[0] = i_dStrength_2;
				m_dStr[1] = i_dStrength_1;
				m_dStr[2] = i_dStrength_3;

				m_dW_Ratio_1 = i_dWL_2 / i_dWL_1;
				m_dW_Ratio_2 = i_dWL_3 / i_dWL_1;
				m_dH_Ratio_1 = i_dStrength_2 / i_dStrength_1;
				m_dH_Ratio_2 = i_dStrength_3 / i_dStrength_2;
			}
			else
			{
				m_dWl[0] = i_dWL_1;
				m_dWl[1] = i_dWL_3;
				m_dWl[2] = i_dWL_2;

				m_dStr[0] = i_dStrength_1;
				m_dStr[1] = i_dStrength_3;
				m_dStr[2] = i_dStrength_2;

				m_dW_Ratio_1 = i_dWL_1 / i_dWL_3;
				m_dW_Ratio_2 = i_dWL_2 / i_dWL_3;
				m_dH_Ratio_1 = i_dStrength_1 / i_dStrength_3;
				m_dH_Ratio_2 = i_dStrength_2 / i_dStrength_3;
			}
		}
		else if (i_dStrength_1 < i_dStrength_2)
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
			m_dH_Ratio_2 = i_dStrength_3 / i_dStrength_2;
		}
		else
		{
			m_dWl[0] = i_dWL_2;
			m_dWl[1] = i_dWL_1;
			m_dWl[2] = i_dWL_3;

			m_dStr[0] = i_dStrength_2;
			m_dStr[1] = i_dStrength_1;
			m_dStr[2] = i_dStrength_3;

			m_dW_Ratio_1 = i_dWL_2 / i_dWL_1;
			m_dW_Ratio_2 = i_dWL_3 / i_dWL_1;
			m_dH_Ratio_1 = i_dStrength_2 / i_dStrength_1;
			m_dH_Ratio_2 = i_dStrength_3 / i_dStrength_1;
		}
	}
};
class gauss_fit_results
{
public:
	XVECTOR	m_vA;
	double	m_dS;
	XSQUARE_MATRIX	m_mCovariance;

	gauss_fit_results(void) : m_vA(), m_dS(-1.), m_mCovariance()	{;};
};

extern gauss_fit_parameters	g_cgfpCaNIR;
extern gauss_fit_parameters	g_cgfpCaHK;
extern gauss_fit_parameters	g_cgfpOINIR;
extern gauss_fit_parameters	g_cgfpSi6355;
extern gauss_fit_parameters	g_cgfpSi5968;

double	Compute_Velocity(const double & i_dObserved_Wavelength, const double & i_dRest_Wavelength);
void Compute_Gaussian_Fit_pEW(const XVECTOR & i_vX, const XVECTOR &i_vA, const double & i_dWavelength_Delta_Ang, const gauss_fit_parameters * i_lpgfpParameters, double & o_dpEW_PVF, double & o_dpEW_HVF);
XVECTOR Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData);
XVECTOR Multi_Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData);
XVECTOR Estimate_Gaussian_Fit(const XVECTOR & i_vX, const XVECTOR & i_vY, const gauss_fit_parameters * i_lpgfpParamters);
XVECTOR Perform_Gaussian_Fit(const XVECTOR & i_vX, const XVECTOR & i_vY, const XVECTOR & i_vW, const gauss_fit_parameters * i_lpgfpParamters,
                    const double & i_dWavelength_Delta_Ang, double & o_dpEW_PVF, double & o_dpEW_HVF, double & o_dV_PVF, double & o_dV_HVF, XVECTOR & o_vSigmas, double & o_dS, gauss_fit_results * io_lpSingle_Fit = NULL, gauss_fit_results * io_lpDouble_Fit = NULL);

void CCM_dered(ES::Spectrum & i_cSpectrum, const double & i_dE_BmV, const double & i_dRv = 3.1);
void CCM_dered(std::vector<std::pair<double,double> > &io_vSpectrum, const double & i_dE_BmV, const double & i_dRv = 3.1);
void CCM_dered(std::vector<std::tuple<double,double,double> > &io_vSpectrum, const double & i_dE_BmV, const double & i_dRv = 3.1);
void CCM_dered(const double & i_dWavelength, double & io_dFlux, const double & i_dE_BmV, const double & i_dRv = 3.1);


class lef_data
{
public:
	const radiation_field * m_lpdField;
	double m_dReference_Frequency_Hz;
	double m_dRedshift;
	double m_dGamma;
};


double Radiation_Field_Photon_Flux_wl(const double & i_dWavelength_cm, const void * i_lpvRadiation_Field);
double Radiation_Field_Energy_Flux_wl(const double & i_dWavelength_cm, const void * i_lpvRadiation_Field);
double Radiation_Field_Photon_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvRadiation_Field);
double Radiation_Field_Energy_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvRadiation_Field);
double Lorentz_Function(const double & i_dFrequency_Hz, const double & i_dFrequency_Reference_Hz, const double & i_dGamma_Hz);
double Line_Energy_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvData);
double Line_Photon_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvData);

double Get_Element_Number(const char * i_lpszNuclide);


class spectrum_data
{
public:
	bool			m_bValid;
	double			m_dPS_Temp;
	double			m_dPS_Velocity;
	double			m_dEjecta_Scalar;
	double			m_dShell_Scalar;
	unsigned int	m_uiIon;
	double			m_dExc_Temp;

	double			m_dFit_WL_Blue;
	double			m_dFit_WL_Red;

	double			m_dNorm_WL_Blue;
	double			m_dNorm_WL_Red;

	double			m_dQuality_of_Fit;


	ES::Spectrum	m_specResult[3]; // 0 = combined, 1 = ejecta, 2 = shell

	spectrum_data(void)
	{
		m_bValid = false;
	}
};
class model
{
public:
	unsigned int m_uiModel_ID;
	xdataset	m_dsEjecta[5]; // carbon=0,oxygen=1,mg group = 2, si group = 3, fe group = 4
	xdataset	m_dsShell;
	OPACITY_PROFILE_DATA	m_opdOp_Profile_Data;
	xdataset	m_dsPhotosphere;
	xdataset	m_dsEjecta_Photosphere;


	void Load_Model(unsigned int i_uiModel);

	model(void){m_uiModel_ID = -1;}
	model(unsigned int i_uiModel){Load_Model(i_uiModel);}
};

void Set_Grid_Refine_Level(unsigned int i_uiRefine_Level_Max);
void Set_Grid_Refine_Size(const xvector & i_vVector);
enum grid_refine_parameter {grp_ps_temp,grp_ps_vel,grp_Ss,grp_Se};
void Set_Grid_Refine_Parameter(grid_refine_parameter i_eParameter, const double & i_dValue);
void Grid_Refine_Fit(
	unsigned int		&o_uiRefine_Result_ID,
	spectrum_data		&io_sdRefine_Result,
	const model *				i_lpmRefine_Model,
	spectrum_data		&o_sdRefine_Result_Curr,
	spectrum_data		&o_sdRefine_Result_Curr_Best,
	unsigned int		&o_uiRefine_Max,
	bool				&i_bAbort_Request);
void Refine_Prep(const spectrum_data & i_cData, unsigned int i_uiModel_ID, ES::Spectrum & o_cTarget, msdb::USER_PARAMETERS &o_cUser_Param, opacity_profile_data::group & o_eModel_Group, bool & o_bIon_Valid);
double Get_Norm_Const(const ES::Spectrum & i_cTarget, const double & i_dNorm_Blue, const double & i_dNorm_Red);

double Set_Grad_Des_Convergence_Threshold(const double & i_dValue);
double Get_Grad_Des_Convergence_Threshold(void);
void Grad_Des_Refine_Fit(
	unsigned int		&o_uiRefine_Result_ID,
	spectrum_data		&io_sdRefine_Result,
	const model *				i_lpmRefine_Model,
	spectrum_data		&o_sdRefine_Result_Curr,
	spectrum_data		&o_sdRefine_Result_Curr_Best,
	const double				&i_dStep_Size,
	bool				&i_bAbort_Request);



