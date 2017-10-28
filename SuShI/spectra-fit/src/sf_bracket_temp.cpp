#include <specfit.h>


double specfit::Bracket_Temperature(double & io_dTemp_Low, double & io_dTemp_Hi, const ES::Spectrum & i_cFull_Target)
{
	double dResult = 0.0;

	double dTemp_Low = io_dTemp_Low;
	double	dTemp_Hi = io_dTemp_Hi;
	double dFlux_Low = 0.0;
	double dFlux_Hi = 0.0;
	double dFlux_Target = 0.0;
	double dVariance_Hi = 0.0;
	double dVariance_Lo = 0.0;
	double dDelta_WL = (i_cFull_Target.wl(1) - i_cFull_Target.wl(0));
	for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
	{
		if (i_cFull_Target.wl(uiI) >= FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
		{
			double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);

			dFlux_Target += i_cFull_Target.flux(uiI) * dDelta_WL;
			dFlux_Low += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Low) * dDelta_WL * dE;
			dFlux_Hi += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Hi) * dDelta_WL * dE;

		}
	}
	for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
	{
		if (i_cFull_Target.wl(uiI) >= 4000)// && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
		{
			double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);
			double dErr_Lo = (dFlux_Target / dFlux_Low) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Low) * dE - i_cFull_Target.flux(uiI);
			double dErr_Hi = (dFlux_Target / dFlux_Hi) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Hi) * dE - i_cFull_Target.flux(uiI);
			dVariance_Lo += dErr_Lo * dErr_Lo;
			dVariance_Hi += dErr_Hi * dErr_Hi;

		}
	}

	unsigned int uiCount = 0;
	do
	{
		double dTemp_Test= (dTemp_Hi - dTemp_Low) * 0.5 + dTemp_Low;
		double dFlux_Test = 0.0;
		double dVariance_Test = 0.0;
		for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
		{
			if (i_cFull_Target.wl(uiI) >= FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
			{
				double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);

				dFlux_Test += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Test) * dDelta_WL * dE;
			}
		}
		for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
		{
			if (i_cFull_Target.wl(uiI) >= 4000)//FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
			{
				double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);
				double dErr_Test = (dFlux_Target / dFlux_Test) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Test) * dE - i_cFull_Target.flux(uiI);
				dVariance_Test += dErr_Test * dErr_Test;

			}
		}
		dResult = dVariance_Test;
		if (dVariance_Lo < dVariance_Hi && dVariance_Test < dVariance_Hi)
		{
			dVariance_Hi = dVariance_Test;
			dTemp_Hi = dTemp_Test;
		}
		else if (dVariance_Lo > dVariance_Hi && dVariance_Test < dVariance_Lo)
		{
			dVariance_Lo = dVariance_Test;
			dTemp_Low = dTemp_Test;
		}
		else
		{
			double dTemp_Tests[2] = {dTemp_Test,dTemp_Test};
			double dTest_Low = Bracket_Temperature(dTemp_Low,dTemp_Tests[0],i_cFull_Target);
			double dTest_Hi = Bracket_Temperature(dTemp_Tests[1],dTemp_Hi,i_cFull_Target);
			if (dTest_Low < dTest_Hi)
			{
				dTemp_Hi = dTemp_Tests[0];
				dResult =  dTest_Low;
			}
			else
			{
				dTemp_Low = dTemp_Tests[1];
				dResult =  dTest_Hi;
			}
			uiCount = 9;
		}
		uiCount++;
	} while (uiCount < 8);

	io_dTemp_Low = dTemp_Low;
	io_dTemp_Hi = dTemp_Hi;
	return dResult;
}
