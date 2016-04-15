#pragma once

#include <xio.h>
#include <xlinalg.h>
#include "ES_Synow.hh"

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
void Generate_Synow_Spectra(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput, const double & i_dTime_Power_Law_A = -2.0, const double & i_dTime_Power_Law_B = -2.0);
void Generate_Synow_Spectra_Exp(const ES::Spectrum &i_cTarget, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput);


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
void Get_Central_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dCentral_Moments);
void Get_Standardized_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dStandardized_Moments);
// Get_Fit_Moments works on spectra that have the exact same size and wavelengths
void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments);
// Get_Fit_Moments works on spectra that have the different sizes, resolution, or wavelength data
void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments, const double & i_dTarget_Normalization_Flux = 1.0, const double & i_dGenereted_Normalization_Flux = 1.0);

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
		m_dH_Ratio_2 = i_dStrength_3 / i_dStrength_2;
	}
};
class GAUSS_FIT_RESULTS
{
public:
	XVECTOR	m_vA;
	double	m_dS;
	XSQUARE_MATRIX	m_mCovariance;
};

extern GAUSS_FIT_PARAMETERS	g_cgfpCaNIR;
extern GAUSS_FIT_PARAMETERS	g_cgfpCaHK;
extern GAUSS_FIT_PARAMETERS	g_cgfpOINIR;
extern GAUSS_FIT_PARAMETERS	g_cgfpSi6355;
extern GAUSS_FIT_PARAMETERS	g_cgfpSi5968;

double	Compute_Velocity(const double & i_dObserved_Wavelength, const double & i_dRest_Wavelength);
void Compute_Gaussian_Fit_pEW(const XVECTOR & i_vX, const XVECTOR &i_vA, const double & i_dWavelength_Delta_Ang, const GAUSS_FIT_PARAMETERS * i_lpgfpParameters, double & o_dpEW_PVF, double & o_dpEW_HVF);
XVECTOR Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData);
XVECTOR Multi_Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData);
XVECTOR Perform_Gaussian_Fit(const double & i_dMin_Flux_Flat, const double & i_dCentral_WL, const XVECTOR & i_vX, const XVECTOR & i_vY, const XVECTOR & i_vW, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters,
                    const double & i_dWavelength_Delta_Ang, double & o_dpEW_PVF, double & o_dpEW_HVF, double & o_dV_PVF, double & o_dV_HVF, XVECTOR & o_vSigmas, double & o_dS, GAUSS_FIT_RESULTS * io_lpSingle_Fit = NULL, GAUSS_FIT_RESULTS * io_lpDouble_Fit = NULL);



