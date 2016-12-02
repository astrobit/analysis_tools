#include <line_routines.h>
#include <xastro.h>
#include <radiation.h>

double Radiation_Field_Photon_Flux_wl(const double & i_dWavelength_cm, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Photon_Flux_wl(i_dWavelength_cm);
}
double Radiation_Field_Energy_Flux_wl(const double & i_dWavelength_cm, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Energy_Flux_wl(i_dWavelength_cm);
}
double Radiation_Field_Photon_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Photon_Flux_freq(i_dFrequency_Hz);
}
double Radiation_Field_Energy_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Energy_Flux_freq(i_dFrequency_Hz);
}

double Lorentz_Function(const double & i_dFrequency_Hz, const double & i_dFrequency_Reference_Hz, const double & i_dGamma_Hz)
{
// see http://www.phy.ohiou.edu/~mboett/astro401_fall12/broadening.pdf
// tested and verified 1 Dec 2016
	double dGamma = i_dGamma_Hz * 0.25 / g_XASTRO.k_dpi;
	double dDel_f = (i_dFrequency_Hz - i_dFrequency_Reference_Hz); 
	double dPhi_f = (dGamma / (dDel_f * dDel_f + dGamma * dGamma) / g_XASTRO.k_dpi);
	return dPhi_f;
}

double Line_Energy_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvData)
{
	lef_data * lpldData = (lef_data *) i_lpvData;

	double dRet = lpldData->m_lpdField->Get_Energy_Flux_freq(i_dFrequency_Hz,lpldData->m_dRedshift);
	return dRet * Lorentz_Function(i_dFrequency_Hz,lpldData->m_dReference_Frequency_Hz,lpldData->m_dGamma);
}

double Line_Photon_Flux_freq(const double & i_dFrequency_Hz, const void * i_lpvData)
{
	lef_data * lpldData = (lef_data *) i_lpvData;

	double dRet = lpldData->m_lpdField->Get_Photon_Flux_freq(i_dFrequency_Hz,lpldData->m_dRedshift);
	return dRet * Lorentz_Function(i_dFrequency_Hz,lpldData->m_dReference_Frequency_Hz,lpldData->m_dGamma);
}
