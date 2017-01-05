#pragma once
#include <xio.h>
#include <best_fit_data.h>
#include <assert.h>
#include <tuple>
#include <opacity_profile_data.h>

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

	typedef std::vector <std::tuple<double, double, double> > spectraldata;
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
		double						m_dE_BmV; // E (B-V) value to use for dereddening
		bool						m_bUse_JSON_ebv; // use the ebv data from the JSON datafile
		bool						m_bUse_Two_Component_Fit; // set true to have a separate ejecta and shell photosphere and a mixing ratio
		spectraldata m_vData;
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

	class model
	{
	public:
		unsigned int m_uiModel_ID;
		XDATASET	m_dsEjecta[5]; // carbon=0,oxygen=1,mg group = 2, si group = 3, fe group = 4
		XDATASET	m_dsShell;
		OPACITY_PROFILE_DATA	m_opdOp_Profile_Data;
		XDATASET	m_dsPhotosphere;
		XDATASET	m_dsEjecta_Photosphere;
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

		std::vector< std::pair<double,double> > m_vpdSpectrum_Synthetic;
		std::vector< std::pair<double,double> > m_vpdSpectrum_Synthetic_Ejecta_Only;
		std::vector< std::pair<double,double> > m_vpdSpectrum_Synthetic_Shell_Only;
		std::vector< std::pair<double,double> > m_vpdSpectrum_Synthetic_Continuum;
		std::vector< std::pair<double,double> > m_vpdSpectrum_Target;

		std::vector<double> 					m_vdRaw_Moments;
		std::vector<double> 					m_vdCentral_Moments;
		std::vector<double> 					m_vdStandardized_Moments;

		void clear();
		fit_result(void);
	};

	double GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit, bool i_bDebug = false, const param_set * i_lppsEjecta = nullptr, const param_set * i_lppsShell = nullptr);


};

