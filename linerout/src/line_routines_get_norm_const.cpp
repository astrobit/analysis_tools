#include <line_routines.h>

double Get_Norm_Const(const ES::Spectrum & i_cTarget, const double & i_dNorm_Blue, const double & i_dNorm_Red)
{
	double dTgt_Norm = 0.0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		if (i_cTarget.wl(uiI) >= i_dNorm_Blue && i_cTarget.wl(uiI) <= i_dNorm_Red)
			dTgt_Norm += i_cTarget.flux(uiI);
	}
	return dTgt_Norm;
}

