#include<cstdio>
#include<cstdlib>
#include <radiation.h>
#include <velocity_function.h>

long double Photoionization_Rate(const long double & i_dCross_Section_cm2, const long double & i_dIonization_Energy_erg,const radiation_field & i_cRadiation_Field, const long double & i_dRedshift)
{
	long double dFrequency_Ref = i_dIonization_Energy_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
	long double ldFrequency = dFrequency_Ref;
	long double ldFreq_Max = dFrequency_Ref * 16.0;
	long double dDelta_Freq = ldFrequency  / 4096.0;
	long double dSum = 0.0;
	do
	{
		long double dJ = i_cRadiation_Field.Get_Energy_Flux_freq(ldFrequency,i_dRedshift);
		// calculate step size in Ry
		long double ldE = ldFrequency * g_XASTRO.k_dh;
		long double ldf_fr = dFrequency_Ref / ldFrequency;
		long double dF = 4.0 * g_XASTRO.k_dpi * dJ / ldE * i_dCross_Section_cm2 * dDelta_Freq * ldf_fr * ldf_fr * ldf_fr;
		dSum += dF;
		//printf("%Le %Le %Le %Le %Le \n",ldFrequency,dF,dSum,ldf_fr,dJ);
		ldFrequency += dDelta_Freq;
	}
	while (ldFrequency < ldFreq_Max);

	return dSum;
}

long double Ion_Recombination_Rate(const long double & i_dCross_Section_cm2, const long double & i_dIonization_Energy_erg, const long double & i_dIonized_State_g, const long double & i_dRecombined_State_g, const velocity_function & i_cVelocity_Function, const long double & i_dRedshift)
{
	long double dFrequency_Ref = i_dIonization_Energy_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
	long double ldFrequency = dFrequency_Ref;
	long double ldFreq_Max = dFrequency_Ref * 16.0;
	long double dDelta_Freq = ldFrequency  / 4096.0;
	long double dSum = 0.0;
	do
	{
		long double ldE = ldFrequency * g_XASTRO.k_dh;
		//long double dDelta_E = dDelta_Freq * g_XASTRO.k_dh;
		long double dVelocity = std::sqrt(fabs(ldE - i_dIonization_Energy_erg) * 2.0 / g_XASTRO.k_dme);
		if (dVelocity > 0)
		{
			// Milne relation: cs(bf) = (m c v / h nu)^2 g+ / gn * ge / 2 cs(fb)
			// the E_rel term is (h nu / m c v)
			long double dE_Rel = ldE / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
			long double dDelta_Vel = dDelta_Freq * g_XASTRO.k_dh / (g_XASTRO.k_dme * dVelocity);
			long double ldf_fr = dFrequency_Ref / ldFrequency;
			long double dCS = i_dCross_Section_cm2 * ldf_fr * ldf_fr * ldf_fr * dE_Rel * dE_Rel * i_dRecombined_State_g / i_dIonized_State_g;;
			long double dMW = i_cVelocity_Function(dVelocity);
			long double dCS_V = dMW * dVelocity * dCS;
			long double dF = dCS_V * dDelta_Vel;
			//printf("%Le %Le %Le %Le %Le %Le %Le %Le\n",ldFrequency,dVelocity,dE_Rel,dMW,dCS,dDelta_Vel,dF,dSum);
			dSum += dF;
		}
		ldFrequency += dDelta_Freq;
	}
	while (ldFrequency < ldFreq_Max);

	return dSum;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	long double ldTemp = 16000.0;
	Maxwellian_velocity_function vfMaxwell(ldTemp);
	Planck_radiation_field rfPlanck(ldTemp);
	long double dCS_Central = 5e-18;
	long double dIonization_Energy = 50.9131*g_XASTRO.k_derg_eV;// 6.11316*g_XASTRO.k_derg_eV;
	long double ldIon = Photoionization_Rate(dCS_Central,dIonization_Energy,rfPlanck,0.0);
	long double ldRecombine = Ion_Recombination_Rate(dCS_Central,dIonization_Energy,1.0,1.0,vfMaxwell,0.0) * 1e12; // rough electron density

	printf("%Le %Le\n",ldIon,ldRecombine);
	return 0;
}
