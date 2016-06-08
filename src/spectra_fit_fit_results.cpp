#include <specfit.h>

using namespace specfit;

void fit_result::clear(void)
{

	m_vpdSpectrum_Synthetic.clear();
	m_vpdSpectrum_Synthetic_Ejecta_Only.clear();
	m_vpdSpectrum_Synthetic_Shell_Only.clear();
	m_vpdSpectrum_Synthetic_Continuum.clear();
	m_vpdSpectrum_Target.clear();

	m_vdRaw_Moments.clear();
	m_vdCentral_Moments.clear();
	m_vdStandardized_Moments.clear();

}
fit_result::fit_result(void)
{
	clear();
}

