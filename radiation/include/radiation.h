#pragma once
#include <vector>
#include <utility> 
#include <xastro.h>

class radiation_field
{
public:
	virtual double Get_Photon_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const = 0;
	virtual double Get_Energy_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const = 0;
	virtual double Get_Photon_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0) const = 0;
	virtual double Get_Energy_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0) const = 0;

	double Get_Rest_Frame_Wavelength(const double & i_dWavelength_cm, const double & i_dRedshift) const{return i_dWavelength_cm / (1.0 + i_dRedshift);}
	double Get_Rest_Frame_Frequency(const double & i_dFrequency_Hz, const double & i_dRedshift) const{return i_dFrequency_Hz * (1.0 + i_dRedshift);}
};


class Planck_radiation_field : public radiation_field
{
private:
	double m_dTemperature_K;
public:

	Planck_radiation_field(void){m_dTemperature_K = 10000.0;}
	Planck_radiation_field(const double & i_dTemperature_K){m_dTemperature_K = i_dTemperature_K;}

	double Get_Temperature(void) const {return m_dTemperature_K;}
	void Set_Temperature(const double & i_dTemperature_K) {m_dTemperature_K = i_dTemperature_K;}


	inline double Get_Photon_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const
	{
// tested and verified 1 Dec 2016
		double dWavelength_Local = Get_Rest_Frame_Wavelength(i_dWavelength_cm,i_dRedshift);
		double dPlanck_Coeff = 2.0 * g_XASTRO.k_dc / (dWavelength_Local * dWavelength_Local * dWavelength_Local * dWavelength_Local);

		double dExp = 1.0 / (exp(g_XASTRO.k_dhc / (g_XASTRO.k_dKb * m_dTemperature_K * dWavelength_Local)) - 1.0);
		return (dPlanck_Coeff * dExp);
	}
	inline double Get_Energy_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const
	{
// tested and verified 1 Dec 2016
		double dWavelength_Local = Get_Rest_Frame_Wavelength(i_dWavelength_cm,i_dRedshift);
		return (Get_Photon_Flux_wl(i_dWavelength_cm) * g_XASTRO.k_dhc / dWavelength_Local);
	}
	inline double Get_Photon_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0) const
	{
		double dFrequency_Local = Get_Rest_Frame_Frequency(i_dFrequency_Hz,i_dRedshift);
		double dPlanck_Coeff = 2.0 * dFrequency_Local * dFrequency_Local / (g_XASTRO.k_dc * g_XASTRO.k_dc);

		double dExp = 1.0 / (exp(g_XASTRO.k_dh * dFrequency_Local / (g_XASTRO.k_dKb * m_dTemperature_K)) - 1.0);
		return (dPlanck_Coeff * dExp);
	}
	inline double Get_Energy_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0) const
	{
		double dFrequency_Local = Get_Rest_Frame_Frequency(i_dFrequency_Hz,i_dRedshift);
		return (Get_Photon_Flux_freq(i_dFrequency_Hz) * g_XASTRO.k_dh * dFrequency_Local);
	}

};
class user_radiation_field : public radiation_field
{
public:
	std::vector< std::pair<double, double> > m_vpddPhoton_Flux; // first = wavelength [cm]; second=energy flux [erg cm^-2 s^-1]

	inline double Get_Photon_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const
	{
		double dWavelength_Local = Get_Rest_Frame_Wavelength(i_dWavelength_cm,i_dRedshift);
		double dPhoton_energy = g_XASTRO.k_dh * g_XASTRO.k_dc / dWavelength_Local;
		double dRet = Get_Energy_Flux_wl(i_dWavelength_cm) / dPhoton_energy;
		return dRet;
	}
	inline double Get_Energy_Flux_wl(const double & i_dWavelength_cm, const double & i_dRedshift = 0.0) const
	{
		double dRet = 0.0;
		double dWavelength_Local = Get_Rest_Frame_Wavelength(i_dWavelength_cm,i_dRedshift);
		unsigned int uiIdx = -1;
		unsigned int uiIdx2 = -1;
		for (std::vector< std::pair<double, double> >::const_iterator iterI = m_vpddPhoton_Flux.begin(); iterI != m_vpddPhoton_Flux.end() && uiIdx2 == -1; iterI++)
		{
			uiIdx++;
			if (iterI->first < dWavelength_Local)
			{
				uiIdx2 = uiIdx;
				uiIdx--;
			}
		}
		if (uiIdx2 != -1)
		{
			double dSlope = (m_vpddPhoton_Flux[uiIdx2].second - m_vpddPhoton_Flux[uiIdx].second) / (m_vpddPhoton_Flux[uiIdx2].first - m_vpddPhoton_Flux[uiIdx].first);
			double dFlux = (dWavelength_Local - m_vpddPhoton_Flux[uiIdx].first) * dSlope + m_vpddPhoton_Flux[uiIdx].second;
			dRet = dFlux;
		}
		return dRet;
	}
	inline double Get_Photon_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0)
	{
		double dFrequency_Local = Get_Rest_Frame_Frequency(i_dFrequency_Hz,i_dRedshift);
		double dWavelength_Local = g_XASTRO.k_dc / dFrequency_Local;
		double dPhoton_energy = g_XASTRO.k_dh * dFrequency_Local;
		double dRet = Get_Energy_Flux_wl(dWavelength_Local,0.0) / dPhoton_energy * (dWavelength_Local * dWavelength_Local) / g_XASTRO.k_dc;
		return dRet;
	}
	inline double Get_Energy_Flux_freq(const double & i_dFrequency_Hz, const double & i_dRedshift = 0.0)
	{
		double dFrequency_Local = Get_Rest_Frame_Frequency(i_dFrequency_Hz,i_dRedshift);
		double dWavelength_Local = g_XASTRO.k_dc / dFrequency_Local;
		double dRet = Get_Energy_Flux_wl(dWavelength_Local,0.0) * (dWavelength_Local * dWavelength_Local) / g_XASTRO.k_dc;
		return dRet;
	}
};


