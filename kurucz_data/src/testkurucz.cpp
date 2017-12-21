#include<cstdio>
#include<cstdlib>
#include <kurucz_data.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	Planck_radiation_field rfPlanck(15000.0);
	//Maxwellian_velocity_function vfMaxwell(5800.0);
	kurucz_derived_data kddData(20,0,2); // Hydrogen -- Ca I and Ca II
	long double ldVps = 18000.0;
	long double ldVmat = 20000.0;
	long double ldVel_Ratio = ldVps / ldVmat;
	long double ldRedshift = (ldVps - ldVmat) / g_XASTRO.k_dc * 1.0e5;
	kddData.Compute_H_Z(rfPlanck,ldRedshift,ldVel_Ratio * ldVel_Ratio);
	printf("here\n");
	fflush(stdout);
	FILE * fileKD = fopen("testkd.csv","wt");
	if (fileKD != nullptr)
	{
		fprintf(fileKD,"Freq (Hz), Wavelength (Angstrom), planck*Lorentz, Plank, Lorentz, BPLdf, BPLdf (SE)\n");
		auto iterI = kddData.m_kdKurucz_Data.m_vvkldLine_Data.begin();
		if (iterI != kddData.m_kdKurucz_Data.m_vvkldLine_Data.end())
			iterI++;
		if (iterI != kddData.m_kdKurucz_Data.m_vvkldLine_Data.end())
		//for (auto iterI = kddData.m_kdKurucz_Data.begin(); iterI != kddData.m_kdKurucz_Data.end(); iterI++)
		{
			auto iterJ = iterI->begin();//
	//		for (auto iterJ = iterI->begin(); iterJ != iterI->end(); iterI++)
			{
				for (size_t tI = 0; tI < 227; tI++)//while (iterJ != iterI->end() && iterJ->m_cLevel_Lower.m_szLabel != "4s 2S")// && iterJ->m_cLevel_Upper.m_szLabel != "4p 2P")
				{
						iterJ++;
				}
				if (iterJ != iterI->end())
				{
					lef_data cldRef_Data;
					cldRef_Data.m_lpdField  = &rfPlanck;
					cldRef_Data.m_dReference_Frequency_Hz = iterJ->m_dFrequency_Hz;//g_XASTRO.k_dc / m_dWavelength_cm;
					cldRef_Data.m_dRedshift = 0.0;
					cldRef_Data.m_dGamma = iterJ->m_cLevel_Upper.m_dGamma + iterJ->m_cLevel_Lower.m_dGamma;


					long double dFreq_Low = cldRef_Data.m_dReference_Frequency_Hz - 256.0 * cldRef_Data.m_dGamma;
					long double dFreq_Hi = cldRef_Data.m_dReference_Frequency_Hz + 256.0 * cldRef_Data.m_dGamma;
					long double ldDelta = (dFreq_Hi - dFreq_Low) / (1 << 17);
					printf("wl: %.2Lf %.4Lf %.4Lf (%.3Le) -- %.3Le (%s %.1Lf -- %s %.1Lf) %.3Le %.3Le\n",iterJ->m_dElement_Code,iterJ->m_dWavelength_nm,g_XASTRO.k_dc * 1.0e8 / cldRef_Data.m_dReference_Frequency_Hz, iterJ->m_dFrequency_Hz,cldRef_Data.m_dGamma,
							iterJ->m_cLevel_Lower.m_szLabel.c_str(),iterJ->m_cLevel_Lower.m_dJ,
							iterJ->m_cLevel_Upper.m_szLabel.c_str(),iterJ->m_cLevel_Upper.m_dJ,
							iterJ->m_dEinstein_B,
							iterJ->m_dEinstein_B_SE);
					fflush(stdout);
					long double ldBJtot = 0.0;
					long double ldBJSEtot = 0.0;
					long double ldJmax = 0.0;
					for (long double ldFreq = dFreq_Low; ldFreq <= dFreq_Hi; ldFreq += ldDelta)
					{
						long double ldPlanck = rfPlanck.Get_Energy_Flux_freq(ldFreq);
						long double dLorentz = Lorentz_Function(ldFreq,iterJ->m_dFrequency_Hz,cldRef_Data.m_dGamma);
						long double dVal = Line_Energy_Flux_freq(ldFreq,&cldRef_Data);
						long double ldBJ = iterJ->m_dEinstein_B * dVal * ldDelta;
						long double ldBJSE = iterJ->m_dEinstein_B_SE * dVal * ldDelta;
						if (dLorentz > ldJmax)
							ldJmax = dLorentz;
						ldBJtot += ldBJ;
						ldBJSEtot += ldBJSE;
						fprintf(fileKD,"%.17Le, %.17Lf, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le\n",ldFreq,g_XASTRO.k_dc * 1.0e8 / ldFreq,dVal,ldPlanck,dLorentz,ldBJ,ldBJSE);
					}
					printf("%.4Le %.4Le %.4Le %.4Le\n",iterJ->m_dEinstein_A,ldBJtot,ldBJSEtot,ldJmax);
				}
			}
		}
	}
	return 0;
}
