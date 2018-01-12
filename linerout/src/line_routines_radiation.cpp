#include <line_routines.h>
#include <xastro.h>
#include <radiation.h>

long double Radiation_Field_Photon_Flux_wl(const long double & i_dWavelength_cm, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Photon_Flux_wl(i_dWavelength_cm);
}
long double Radiation_Field_Energy_Flux_wl(const long double & i_dWavelength_cm, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Energy_Flux_wl(i_dWavelength_cm);
}
long double Radiation_Field_Photon_Flux_freq(const long double & i_dFrequency_Hz, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Photon_Flux_freq(i_dFrequency_Hz);
}
long double Radiation_Field_Energy_Flux_freq(const long double & i_dFrequency_Hz, const void * i_lpvRadiation_Field)
{
	radiation_field * rfField = (radiation_field *)i_lpvRadiation_Field;
	return rfField->Get_Energy_Flux_freq(i_dFrequency_Hz);
}

long double Lorentz_Function(const long double & i_dFrequency_Hz, const long double & i_dFrequency_Reference_Hz, const long double & i_dGamma_Hz)
{
// see http://www.phy.ohiou.edu/~mboett/astro401_fall12/broadening.pdf
// tested and verified 1 Dec 2016
	long double ldFourth = 0.25L; // make sure that this is loaded as long double
	long double dGamma(i_dGamma_Hz * ldFourth / g_XASTRO.k_dpi);
	long double dDel_f(i_dFrequency_Hz - i_dFrequency_Reference_Hz); 
	long double dPhi_f(dGamma / (dDel_f * dDel_f + dGamma * dGamma) / g_XASTRO.k_dpi);
	if (dGamma == 0.0)
		std::cerr << "Gamma zero\n";
	return dPhi_f;
}

long double Line_Energy_Flux_freq(const long double & i_dFrequency_Hz, const void * i_lpvData)
{
	lef_data * lpldData = (lef_data *) i_lpvData;

	if (std::isnan(lpldData->m_dGamma))
		std::cerr << "Gamma nan\n";
	if (std::isnan(i_dFrequency_Hz) && !std::isnan(lpldData->m_dGamma))
		std::cerr << "Frequency nan\n";
	if (std::isnan(lpldData->m_dRedshift))
		std::cerr << "redshift nan\n";

	long double dRet = lpldData->m_lpdField->Get_Energy_Flux_freq(i_dFrequency_Hz,lpldData->m_dRedshift);
	if (std::isnan(dRet) && !std::isnan(i_dFrequency_Hz) && !std::isnan(lpldData->m_dRedshift))
		std::cerr << "energy flux nan\n";
	long double dLorentz = Lorentz_Function(i_dFrequency_Hz,lpldData->m_dReference_Frequency_Hz,lpldData->m_dGamma);
	if (std::isnan(dLorentz) && !std::isnan(i_dFrequency_Hz) && !std::isnan(lpldData->m_dReference_Frequency_Hz) && !isnan(lpldData->m_dGamma))
		std::cerr << "lorentz nan\n";
	dRet *= dLorentz;

	return dRet;
}

long double Line_Photon_Flux_freq(const long double & i_dFrequency_Hz, const void * i_lpvData)
{
	lef_data * lpldData = (lef_data *) i_lpvData;

	long double dRet = lpldData->m_lpdField->Get_Photon_Flux_freq(i_dFrequency_Hz,lpldData->m_dRedshift);
	return dRet * Lorentz_Function(i_dFrequency_Hz,lpldData->m_dReference_Frequency_Hz,lpldData->m_dGamma);
}
