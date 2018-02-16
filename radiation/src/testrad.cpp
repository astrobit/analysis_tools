#include<cstdio>
#include<cstdlib>
#include <xastro.h>
#include <radiation.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	double dRad_temp = 5800.0;
	double dLuminosity = 0.0;
	double dPhoton_Luminosity = 0.0;
	double dRadius = 15000.0e5 * (18 * 24.0 * 3600.0);
	Planck_radiation_field cSun(15000.0);
 	size_t tI_Max = (1 << 24);
	for (size_t tI = 0; tI < tI_Max; tI++)
	{
		double dWL = tI * 1.0e-8 + 1.0e-8; // 10 ...  angstrom
		dLuminosity += cSun.Get_Energy_Flux_wl(dWL) * 1.0e-8;
		dPhoton_Luminosity += cSun.Get_Photon_Flux_wl(dWL) * 1.0e-8;
	}
	dLuminosity *= 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;
	dPhoton_Luminosity *= 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;
	printf("WL: %.3e %.3e\n",dLuminosity,dPhoton_Luminosity);

	dLuminosity = 0.0;
	dPhoton_Luminosity = 0.0;
	double dDel_Freq = (3.0e18/tI_Max);
	for (size_t tI = 0; tI < tI_Max; tI++)
	{

		double dFreq = (tI + 1) * dDel_Freq; // 1 angstrom
		dLuminosity += cSun.Get_Energy_Flux_freq(dFreq) * dDel_Freq;
		dPhoton_Luminosity += cSun.Get_Photon_Flux_freq(dFreq) * dDel_Freq;
	}
	dLuminosity *= 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;
	dPhoton_Luminosity *= 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;
	printf("Freq: %.3e %.3e\n",dLuminosity,dPhoton_Luminosity);

	return 0;
}
