#pragma once
#include <vector>
#include <utility> 
#include <xastro.h>

class velocity_function
{
public:
	virtual long double Get_Probability(const long double & i_dVelocity_cm_s) const = 0;
	long double operator()(const long double & i_dVelocity_cm_s) const {return Get_Probability(i_dVelocity_cm_s);};
};


class Maxwellian_velocity_function : public velocity_function
{
private:
	long double m_dTemperature_K;
public:


	long double Get_Probability(const long double & i_dVelocity_cm_s) const
	{
		long double d2kt = 2.0 * g_XASTRO.k_dKb * m_dTemperature_K;
		long double dP = g_XASTRO.k_dme / (g_XASTRO.k_dpi * d2kt);
		return 4.0 * g_XASTRO.k_dpi * dP * sqrt(dP) * i_dVelocity_cm_s * i_dVelocity_cm_s * exp(-g_XASTRO.k_dme * i_dVelocity_cm_s * i_dVelocity_cm_s / d2kt);
	}

	Maxwellian_velocity_function(void){m_dTemperature_K = 10000.0;}
	Maxwellian_velocity_function(const double & i_dTemperature_K){m_dTemperature_K = i_dTemperature_K;}

	long double Get_Temperature(void) const {return m_dTemperature_K;}
	void Set_Temperature(const long double & i_dTemperature_K) {m_dTemperature_K = i_dTemperature_K;}



};
class Fermi_Dirac_velocity_function : public velocity_function
{
private:
	long double m_dTemperature_K;
public:


	long double Get_Probability(const long double & i_dVelocity_cm_s) const
	{
		long double d2kt = 2.0 * g_XASTRO.k_dKb * m_dTemperature_K;
		long double dP = g_XASTRO.k_dme / (g_XASTRO.k_dpi * d2kt);
		return 4.0 * g_XASTRO.k_dpi * dP * sqrt(dP) * i_dVelocity_cm_s * i_dVelocity_cm_s / (exp(g_XASTRO.k_dme * i_dVelocity_cm_s * i_dVelocity_cm_s / d2kt) + 1);
	}

	Fermi_Dirac_velocity_function(void){m_dTemperature_K = 10000.0;}
	Fermi_Dirac_velocity_function(const long double & i_dTemperature_K){m_dTemperature_K = i_dTemperature_K;}

	long double Get_Temperature(void) const {return m_dTemperature_K;}
	void Set_Temperature(const long double & i_dTemperature_K) {m_dTemperature_K = i_dTemperature_K;}


};
class Bose_Einstein_velocity_function : public velocity_function
{
private:
	long double m_dTemperature_K;
public:


	long double Get_Probability(const long double & i_dVelocity_cm_s) const
	{
		long double d2kt = 2.0 * g_XASTRO.k_dKb * m_dTemperature_K;
		long double dP = g_XASTRO.k_dme / (g_XASTRO.k_dpi * d2kt);
		return 4.0 * g_XASTRO.k_dpi * dP * sqrt(dP) * i_dVelocity_cm_s * i_dVelocity_cm_s / (exp(g_XASTRO.k_dme * i_dVelocity_cm_s * i_dVelocity_cm_s / d2kt) - 1);
	}

	Bose_Einstein_velocity_function(void){m_dTemperature_K = 10000.0;}
	Bose_Einstein_velocity_function(const long double & i_dTemperature_K){m_dTemperature_K = i_dTemperature_K;}

	long double Get_Temperature(void) const {return m_dTemperature_K;}
	void Set_Temperature(const long double & i_dTemperature_K) {m_dTemperature_K = i_dTemperature_K;}


};
class user_velocity_function : public velocity_function
{
public:
	std::vector< std::pair<long double, long double> > m_vpddVelocity_Function; // first = veocity [cm/2]; second = probability per unit velocity [cm^-1 s^1]

	inline long double Get_Probability(const long double & i_dVelocity_cm_s) const
	{
		long double dRet = 0.0;
		unsigned int uiIdx = -1;
		unsigned int uiIdx2 = -1;
		for (std::vector< std::pair<long double, long double> >::const_iterator iterI = m_vpddVelocity_Function.begin(); iterI != m_vpddVelocity_Function.end() && uiIdx2 == -1; iterI++)
		{
			uiIdx++;
			if (iterI->first < i_dVelocity_cm_s)
			{
				uiIdx2 = uiIdx;
				uiIdx--;
			}
		}
		if (uiIdx2 != -1)
		{
			long double dSlope = (m_vpddVelocity_Function[uiIdx2].second - m_vpddVelocity_Function[uiIdx].second) / (m_vpddVelocity_Function[uiIdx2].first - m_vpddVelocity_Function[uiIdx].first);
			dRet = (i_dVelocity_cm_s - m_vpddVelocity_Function[uiIdx].first) * dSlope + m_vpddVelocity_Function[uiIdx].second;
		}
		return dRet;
	}
};


