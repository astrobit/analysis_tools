#include <specfit.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>

double	specfit::Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const specfit::params_range &i_cNorm_Range, const specfit::params_range &i_cFit_Range, unsigned int i_uiMoment, bool i_bRegenerate_Normalization)
{
	double	dRet;
	std::vector<double> vFit_Raw;
	std::vector<double> vFit_Central;
	std::vector<double> vFit_Standardized;

	if (i_bRegenerate_Normalization)
		Get_Normalization_Fluxes(i_cTarget, i_cGenerated, i_cNorm_Range, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
//	{
//		if (i_cTarget.wl(uiI) == 0.0 || std::isnan(i_cTarget.flux(uiI)) || std::isnan(i_cGenerated.flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",i_cTarget.wl(uiI),i_cTarget.flux(uiI),i_cGenerated.flux(uiI));
//		}
//	}


	Get_Fit_Moments_2(i_cTarget, i_cGenerated, i_cFit_Range.m_dBlue_WL, i_cFit_Range.m_dRed_WL, i_uiMoment, vFit_Raw, vFit_Central, vFit_Standardized, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	dRet = fabs(vFit_Raw[i_uiMoment - 1]);
	return dRet; // use mean
}

