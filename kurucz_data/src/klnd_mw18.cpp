#include <kurucz_data.h>
#include <line_routines.h>
#include <xmath.h>


void Kurucz_Line_Data::Compute_Z(const radiation_field & i_cRad, const long double & i_dRedshift, const long double & i_dFlux_Dilution)
{
//	long double dEnergy_Flux = Calc_Exciting(i_cRad,i_dRedshift);
	long double dPhoton_Flux = i_cRad.Get_Photon_Flux_freq(m_dFrequency_Hz,i_dRedshift);
	m_dH_abs = dPhoton_Flux * g_XASTRO.k_dh / g_XASTRO.k_dc * m_dEinstein_B * m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight * i_dFlux_Dilution;
	//m_dH_abs = m_dEinstein_B * dEnergy_Flux * i_dFlux_Dilution;
	m_dH_StE = dPhoton_Flux * g_XASTRO.k_dh / g_XASTRO.k_dc * m_dEinstein_B * i_dFlux_Dilution;
//	m_dH_StE = m_dEinstein_B_SE * dEnergy_Flux * i_dFlux_Dilution; // mostly just for debugging purposes
	m_dH_em = m_dEinstein_A + m_dH_StE;
//		std::cout << std::fixed << m_dElement_Code << " " << m_cLevel_Lower.m_dEnergy_Level_cm << "-" << m_cLevel_Upper.m_dEnergy_Level_cm << " " << m_dWavelength_cm * 1e8 << " " << std::scientific << m_dH_abs << " " << m_dH_em << " " << m_dTransition_Energy_eV << " " << m_dEinstein_A << " " << m_dEinstein_B << std::endl;
}

long double Kurucz_Line_Data::Calc_Exciting(const radiation_field & i_cRad, const long double &  i_dRedshift) const
{
	//double dWavelength_cm = m_dWavelength_cm * (1.0 + i_dRedshift);

//	std::cout << dMaximum_wavelength_cm << std::endl;
	lef_data cldRef_Data;
	cldRef_Data.m_lpdField  = &i_cRad;
	cldRef_Data.m_dReference_Frequency_Hz = m_dFrequency_Hz;//g_XASTRO.k_dc / m_dWavelength_cm;
	cldRef_Data.m_dRedshift = i_dRedshift;
	//if (m_dGamma_Rad != 0.0)
	//	cldRef_Data.m_dGamma = m_dGamma_Rad;//m_cLevel_Upper.m_dGamma + m_cLevel_Lower.m_dGamma;
	//else
		cldRef_Data.m_dGamma = m_cLevel_Upper.m_dGamma + m_cLevel_Lower.m_dGamma;

	long double dF_n = 0.0;
	if (!std::isnan(cldRef_Data.m_dGamma) && !std::isnan(cldRef_Data.m_dReference_Frequency_Hz))
	{
		long double dFreq_Low = cldRef_Data.m_dReference_Frequency_Hz - 2048.0 * cldRef_Data.m_dGamma;
		long double dFreq_Hi = cldRef_Data.m_dReference_Frequency_Hz + 2048.0 * cldRef_Data.m_dGamma;

		dF_n = XM_Simpsons_Integration_LD(Line_Energy_Flux_freq,dFreq_Low,dFreq_Hi,1 << 16,(const void *)&cldRef_Data);
	}
	
	return dF_n;// * g_XASTRO.k_dc / (m_dWavelength_cm * m_dWavelength_cm); // convert from per Hz to per cm
}

