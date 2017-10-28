#pragma once
#include <xio.h>
#include <assert.h>
#include <opacity_profile_data.h>
#include <xlinalg.h>
#include <line_routines.h>
#include <model_spectra_db.h>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <cmath>
#include <xio.h>
#include <opacity_profile_data.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <specfit.h>
#include <line_routines.h>
#include <cstdio>
#include <iomanip>
#include <ios>
	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <eps_plot.h>

#include <OSC.hpp>
#include <cfloat>

//#define FIT_BLUE_WL 4500.0
//#define FIT_RED_WL 9000.0
#define FIT_BLUE_WL 6500.0
#define FIT_RED_WL 7900.0

namespace specfit
{
	enum feature {feat_undef, CaNIR, CaHK, Si6355, O7773};
	enum component {comp_undef=0, comp_ejecta, comp_shell};

	class param_set
	{
	public:
		double	m_dPS_Vel;
		double	m_dPS_Temp;
		double	m_dLog_S;
		double	m_dExcitation_Temp;
		param_set(void)
		{
			m_dPS_Vel =
			m_dPS_Temp =
			m_dLog_S =
			m_dExcitation_Temp = nan("");
		}
	};
	class params_range
	{
	public:
		double	m_dBlue_WL;
		double	m_dRed_WL;

		params_range(const double & i_dBlue_WL = -1, const double & i_dRed_WL = -1)
		{
			m_dBlue_WL = i_dBlue_WL;
			m_dRed_WL = i_dRed_WL;
		}

	};

	class params
	{
	public:
		param_set	m_cParams[2];
		double	m_dMixing_Fraction;

		params(void)
		{
			m_dMixing_Fraction = nan("");
		}

		param_set & operator[](component i_eComponent)
		{
			assert(i_eComponent != comp_undef);
			return (m_cParams[(size_t)(i_eComponent - comp_ejecta)]);
		}
		const param_set operator[](component i_eComponent) const
		{
			assert(i_eComponent != comp_undef);
			return (m_cParams[(size_t)(i_eComponent - comp_ejecta)]);
		}
		
	};

	class fit
	{
	public:
		double						m_dMJD;
		double						m_dMJD_Bmax;
		bool						m_bScale_Starting_Log_S;
		double						m_dTime_Scale_Power_Law;
		double						m_dDelay_From_Explosion_To_Bmax;
		feature						m_eFeature;
		std::string					m_szDatafile;
		std::string					m_szInstrument;
		std::string 				m_szSource;
		std::vector <unsigned int> 	m_vuiModels;
		std::vector <std::string> 	m_vsModel_Lists;
		params						m_cSuggested_Param;
		params_range				m_cNorm_Range;
		params_range				m_cFit_Range;
		double						m_dE_BmV; // E (B-V) value to use for dereddening
		double						m_dRedshift; // redshift value to use for conversion to rest frame wavelength
		bool						m_bUse_JSON_ebv; // use the ebv data from the JSON datafile
		bool						m_bUse_Two_Component_Fit; // set true to have a separate ejecta and shell photosphere and a mixing ratio
		OSCspectrum 				m_vData;
		fit(void) : m_cSuggested_Param()
		{
			m_dMJD = nan("");
			m_dMJD_Bmax = nan("");
			m_eFeature = feat_undef;
			m_dE_BmV = 0.0;
			m_bUse_JSON_ebv = false;
			m_bScale_Starting_Log_S = true;
			m_dTime_Scale_Power_Law = -2.0;
			m_dDelay_From_Explosion_To_Bmax = 18.0;
			m_bUse_Two_Component_Fit = false;
		}
	};

	class feature_parameters
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
		feature_parameters(void) { Reset();}
	};
	class result_observables
	{
	public:
		feature_parameters						m_fpParameters;

		xvector									m_xvGaussian_Fit;
		xvector									m_xvGaussian_Fit_Uncertainty;
		double									m_dGaussian_Fit_S;
		gauss_fit_results						m_gfrSingle_Gaussian_Fit;
		gauss_fit_results						m_gfrDouble_Gaussian_Fit;

		double									m_dGaussian_pEW_PVF;
		double									m_dGaussian_pEW_HVF;
		double									m_dGaussian_V_PVF;
		double									m_dGaussian_V_HVF;

		double									m_dGaussian_Ref_WL_Blue;
		double									m_dGaussian_Ref_WL_Red;
	};
	class fit_result
	{
	public:
		params									m_cParams;
		double									m_dMJD;
		feature									m_eFeature;
		std::string								m_szInstrument;
		std::string 							m_szSource;
		unsigned int							m_uiModel;

		OSCspectrum 							m_vpdSpectrum_Synthetic;
		OSCspectrum 							m_vpdSpectrum_Synthetic_Ejecta_Only;
		OSCspectrum 							m_vpdSpectrum_Synthetic_Shell_Only;
		OSCspectrum 							m_vpdSpectrum_Synthetic_Continuum;
		OSCspectrum 							m_vpdSpectrum_Target;

		std::vector<double> 					m_vdRaw_Moments;
		std::vector<double> 					m_vdCentral_Moments;
		std::vector<double> 					m_vdStandardized_Moments;

		result_observables						m_cTarget_Observables;
		result_observables						m_cSynthetic_Observables;


		void clear();
		fit_result(void);
	};


	class spectra_fit_data
	{
	public:
		msdb::USER_PARAMETERS	m_cParam;
		msdb::USER_PARAMETERS	m_cContinuum_Band_Param;

		const ES::Spectrum *	m_lpcTarget;
		const XDATASET *				m_lpcOpacity_Map_A;
		const XDATASET *				m_lpcOpacity_Map_B;

		bool					m_bDebug;
		unsigned int			m_uiDebug_Idx;

		feature_parameters		m_fpTarget_Feature_Parameters;
		double					m_dOII_P_Cygni_Peak_WL;
		double 					m_dCaII_P_Cygni_Peak_WL;
		feature_parameters		m_fpResult_Feature_Parameters;

		params_range				m_cNorm_Range;
		params_range				m_cFit_Range;

		spectra_fit_data(void) : m_cParam()
		{
			m_lpcTarget = nullptr;
			m_lpcOpacity_Map_A = nullptr;
			m_lpcOpacity_Map_B = nullptr;
			m_bDebug = false;
			m_uiDebug_Idx = 1;
		}
	};

	typedef std::vector < fit_result > best_fit_results;

	void Output_Results(std::ofstream & io_fsBest_Fits, const best_fit_results & i_vResults);
	void Parse_XML(const std::string &i_szFilename,
		std::vector <fit> &o_vfitFits ,
		std::map< std::string, std::vector<unsigned int> > &o_mModel_Lists,
		std::map< std::string, std::string > &o_mDatafile_List,
		const params_range & i_cNorm_Range,
		const params_range & i_cFit_Range
		);
	void Load_Data_Files(const std::map< std::string, std::string > &i_mDatafile_List,
		std::map< std::string, OSCfile> &o_mJson_Data,
		std::map< std::string, OSCspectrum > &o_mNon_Json_Data
		);
	void Validate_JSON_Data(const std::map< std::string, OSCfile> &i_mJson_Data);
	void Load_Models(
		std::vector <fit> &i_vfitFits,
		std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists,
		std::map< unsigned int, model> &o_mModel_Data
		);
	void Load_Data(
		std::vector <fit> &io_vfitFits,
		std::map< std::string, OSCfile> &i_mJson_Data,
		std::map< std::string, OSCspectrum > &i_mNon_Json_Data);
	void Deredden(std::vector <fit> &io_vfitFits);
	void Unredshift(std::vector <fit> &io_vfitFits);
	void Output_Result_Header(std::ofstream & io_ofsFile); // located in sf_write_fit.cpp
	void Write_Fit(std::ofstream & io_ofsFile, const fit_result & i_cResult);
	void Output_Target_Result_Header(std::ofstream & io_ofsFile); // located in sf_write_target_fit.cpp
	void Write_Target_Fit(std::ofstream & io_ofsFile, const fit_result & i_cResult);

	void Perform_Fits(std::vector <fit> &i_vfitFits, const std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists, std::map< unsigned int, model> &i_mModel_Data, best_fit_results & o_vResults, bool i_bDebug = false, const param_set * i_lppsEjecta = nullptr, const param_set * i_lppsShell = nullptr, const bool * i_lpbPerform_Single_Fit = nullptr);







	double GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit, bool i_bDebug = false, const param_set * i_lppsEjecta = nullptr, const param_set * i_lppsShell = nullptr, const bool * i_lpbPerform_Single_Fit = nullptr);
	void Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const params_range &i_cNorm_Range, double & o_dTarget_Flux, double & o_dGenerated_Flux);

	double	Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const specfit::params_range &i_cNorm_Range, const specfit::params_range &i_cFit_Range, unsigned int i_uiMoment, bool i_bRegenerate_Normalization = true);
	double pEW_Fit(const ES::Spectrum &i_cGenerated, const feature_parameters & i_fpTarget_Feature, feature_parameters	&o_cModel_Data);
	double Continuum_Fit(const ES::Spectrum &i_cGenerated, const ES::Spectrum &i_cContinuum, feature_parameters	&o_cModel_Data);
	double Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data);
	void Calc_Observables(const ES::Spectrum &i_cGenerated,const ES::Spectrum &i_cContinuum, fit_result	&o_cModel_Data);
	void Inc_Index(unsigned int * io_plIndices, unsigned int i_uiNum_Indices, unsigned int i_uiWrap_Value);
	double Bracket_Temperature(double & io_dTemp_Low, double & io_dTemp_Hi, const ES::Spectrum & i_cFull_Target);

	extern double	g_dTarget_Normalization_Flux;
	extern double	g_dGenerated_Normalization_Flux;


};


