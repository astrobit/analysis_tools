#include <opacity_project_pp.h>

void opacity_project_transition::Calculate_Derived_Quantities(const opacity_project_state & i_sState_Lower, const opacity_project_state & i_sState_Upper)
{
	m_dTransition_Energy_Ry = i_sState_Upper.m_dEnergy_Ry - i_sState_Lower.m_dEnergy_Ry;
	m_dTransition_Energy_eV = m_dTransition_Energy_Ry * g_XASTRO.k_dRy_eV;
	m_dTransition_Energy_erg = m_dTransition_Energy_Ry * g_XASTRO.k_dRy;
	m_dWavelength_cm = g_XASTRO.k_dh / (g_XASTRO.k_derg_eV * m_dTransition_Energy_erg);

	m_dFrequency_Hz = g_XASTRO.k_dc / m_dWavelength_cm;

	long double dTrue_Oscillator_Strength = m_dOscillator_Strength * i_sState_Upper.m_uiStatistical_Weight / ((long double) i_sState_Lower.m_uiStatistical_Weight);

	m_dEinstein_B = g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dh * m_dFrequency_Hz * g_XASTRO.k_dme) * dTrue_Oscillator_Strength;
	m_dEinstein_B_SE = ((long double) i_sState_Lower.m_uiStatistical_Weight) / i_sState_Upper.m_uiStatistical_Weight * m_dEinstein_B; // B21 = g1/g2 B12
	m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh / (m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm) * m_dEinstein_B_SE;

}
