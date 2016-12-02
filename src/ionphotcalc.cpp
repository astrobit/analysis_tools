#include <xastro.h>
#include <xmath.h>
#include <iostream>

double Planck_Photon_Number(const double & i_dX, const void * i_lpvData)
{
	double dTemperature = ((double *)i_lpvData)[0];

	double dPlanck_Coeff = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (i_dX * i_dX * i_dX * i_dX);
	double dExp = 1.0 / (exp(g_XASTRO.k_dhc / g_XASTRO.k_dKb / dTemperature / i_dX) - 1.0);
	return (dPlanck_Coeff * dExp);
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (i_iArg_Count < 3)
	{
		std::cout << "Usage: " << i_lpszArg_Values << " [ionization potential] [Temperature] [[radius]]" << std::endl;
		std::cout << "Computes the ionizing photon flux given an ionization potential and temperature." << std::endl;
		std::cout << "Ionization potential: ionization potential in eV." << std::endl;
		std::cout << "Temperature: value of temperature in Kelvin." << std::endl;
		std::cout << "Radius (optional): radius of BB in cm." << std::endl;
		std::cout << "Return value: in stdout, reports the ionizing photon flux." << std::endl;
	}
	else
	{
		double	dMaximum_wavelength_cm = g_XASTRO.k_dhc / (atof(i_lpszArg_Values[1]) * g_XASTRO.k_derg_eV);
		double	dTemperature = atof(i_lpszArg_Values[2]);
		std::cout << dMaximum_wavelength_cm << std::endl;
		double dRadius = 1.0;
		if (i_iArg_Count == 4)
			dRadius = atof(i_lpszArg_Values[3]);
		double dArea = dRadius * dRadius;
		if (i_iArg_Count == 4)
			dArea *= 4.0 * g_XASTRO.k_dpi;

		double dF_n = XM_Simpsons_Integration(XA_Planck_Photon_Flux,1.0e-8,dMaximum_wavelength_cm,1 << 24,&dTemperature);
		std::cout.precision(17);
		std::cout.setf(std::ios::scientific,std::ios::floatfield);
		std::cout << (dF_n * dArea) << std::endl;
	}
	return 0;
}
