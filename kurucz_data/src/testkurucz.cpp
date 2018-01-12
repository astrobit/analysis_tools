#include<cstdio>
#include<cstdlib>
#include <kurucz_data.h>

#include <ios>
#include <iomanip>
#include <xstdlib.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	Planck_radiation_field rfPlanck(15000.0);
	Planck_radiation_field rfPlanck2(15000.0);
	//Maxwellian_velocity_function vfMaxwell(5800.0);
	kurucz_derived_data kddData(20,0,5); // Ca I
	long double ldVps = 18000.0;
	long double ldVmat = 20000.0;
	long double ldVel_Ratio = ldVps / ldVmat;
	long double ldRedshift = (ldVps - ldVmat) / g_XASTRO.k_dc * 1.0e5;
	printf("here\n");
	fflush(stdout);
	FILE * fileKDl = fopen("testkdl.csv","wt");
	if (fileKDl != nullptr)
	{
		for (auto iterIon = kddData.m_vmklvdLevel_Data.begin(); iterIon != kddData.m_vmklvdLevel_Data.end(); iterIon++)
		{
			for (auto iterS = iterIon->begin(); iterS != iterIon->end(); iterS++)
			{
				fprintf(fileKDl,"%.2Lf, %.3Lf, %s, %.1Lf, %.4Le, %i, %i\n", 
					iterS->second.klvdLevel_Data.m_dElement_Code,
					iterS->second.klvdLevel_Data.m_dEnergy_Level_cm,
					iterS->second.klvdLevel_Data.m_szLabel.c_str(),
					iterS->second.klvdLevel_Data.m_dJ,
					iterS->second.klvdLevel_Data.m_dGamma,
					iterS->second.vivkldAbsorption_Transition_Data.size(),
					iterS->second.vivkldEmission_Transition_Data.size()

);

			}
		}
		fclose(fileKDl);
		fileKDl = nullptr;
	}

	FILE * fileKDln = fopen("testkdln.csv","wt");
	if (fileKDln != nullptr)
	{
		for (auto iterIon = kddData.m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterIon != kddData.m_kdKurucz_Data.m_vvkldLine_Data.end(); iterIon++)
		{
			for (auto iterS = iterIon->begin(); iterS != iterIon->end(); iterS++)
			{
				level_definition cLwr(iterS->m_cLevel_Lower);
				level_definition cUpr(iterS->m_cLevel_Upper);

				fprintf(fileKDln,"%.2Lf, %.3Lf, %.3Lf, %s, %.1Lf, %.3Lf, %.1Lf, %i, %i, %i, %.1Lf, %.3Lf, %s, %.1Lf, %.3Lf, %.1Lf, %i, %i, %i, %.1Lf, %.3Le\n", 
					iterS->m_dElement_Code,
					iterS->m_dWavelength_nm,
					iterS->m_cLevel_Lower.m_dEnergy_Level_cm,
					iterS->m_cLevel_Lower.m_szLabel.c_str(),
					iterS->m_cLevel_Lower.m_dJ,
					cLwr.m_dEnergy_Level_cm,
					cLwr.m_dJ,
					cLwr.m_tS,
					cLwr.m_tL,
					cLwr.m_tP,
					cLwr.m_dK,
					iterS->m_cLevel_Upper.m_dEnergy_Level_cm,
					iterS->m_cLevel_Upper.m_szLabel.c_str(),
					iterS->m_cLevel_Upper.m_dJ,
					cUpr.m_dEnergy_Level_cm,
					cUpr.m_dJ,
					cUpr.m_tS,
					cUpr.m_tL,
					cUpr.m_tP,
					cUpr.m_dK,
					iterS->m_dEinstein_A);



			}
		}
		fclose(fileKDln);
		fileKDln = nullptr;
	}

//	return 0;//@@DEBUG


	kddData.Compute_H_Z(rfPlanck,ldRedshift,ldVel_Ratio * ldVel_Ratio);
	printf("here2\n");
	fflush(stdout);
/*	FILE * fileKD = fopen("testkd.csv","wt");
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
	}*/

	//return 0; //@@DEBUG
	for (size_t tZ = 3; tZ <= 92; tZ++)
	{
		kurucz_derived_data * PKddData = new kurucz_derived_data(tZ,0,tZ - 1);;
		char lpszElem[4] = {0,0,0,0};
		xGet_Element_Symbol(tZ,lpszElem);

		std::cout << lpszElem << " energy test" << std::endl;
		for (auto iterIon = PKddData->m_vmklvdLevel_Data.begin(); iterIon != PKddData->m_vmklvdLevel_Data.end(); iterIon++)
		{
			for (auto iterS = iterIon->begin(); iterS != iterIon->end(); iterS++)
			{
				for (auto iterT = iterIon->begin(); iterT != iterIon->end(); iterT++)
				{
					if (iterS != iterT)
					{
						if (iterS->second.klvdLevel_Data.m_szLabel == iterT->second.klvdLevel_Data.m_szLabel && std::fabs(iterS->second.klvdLevel_Data.m_dJ - iterT->second.klvdLevel_Data.m_dJ) < 0.1)
						{
							if (iterS->second.klvdLevel_Data.m_dEnergy_Level_cm < iterT->second.klvdLevel_Data.m_dEnergy_Level_cm) // so each one is only listed once
							{
								std::cerr << (iterS->second.klvdLevel_Data.m_uiZ + iterS->second.klvdLevel_Data.m_uiIon_State * 0.01 + 0.001) << ": Probable duplicate states " << iterS->second.klvdLevel_Data.m_szLabel << " " << iterS->second.klvdLevel_Data.m_dJ << " " << iterS->second.klvdLevel_Data.m_dEnergy_Level_cm << " & " << iterT->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1." << std::endl;
							}
						}
					}
				}
			}
		}

		for (auto iterIon = PKddData->m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterIon != PKddData->m_kdKurucz_Data.m_vvkldLine_Data.end(); iterIon++)
		{
			for (auto iterS = iterIon->begin(); iterS != iterIon->end(); iterS++)
			{
				long double dWL = 1.0e7L / std::fabs(iterS->m_cLevel_Upper.m_dEnergy_Level_cm - iterS->m_cLevel_Lower.m_dEnergy_Level_cm);
				long double ldFrac_Error = std::fabs(iterS->m_dWavelength_nm - dWL) /  dWL;
				if (ldFrac_Error > 0.001) // relatively large energe difference in wavelength vs. transition
				{ // 
					long double dUpper, dLower;
					if (iterS->m_cLevel_Lower.m_dEnergy_Level_cm < iterS->m_cLevel_Upper.m_dEnergy_Level_cm)
					{
						dUpper = 1.0e7L / iterS->m_dWavelength_nm + iterS->m_cLevel_Lower.m_dEnergy_Level_cm;
						dLower = iterS->m_cLevel_Upper.m_dEnergy_Level_cm - 1.0e7L / iterS->m_dWavelength_nm;
					}
					else
					{
						dLower = 1.0e7L / iterS->m_dWavelength_nm + iterS->m_cLevel_Lower.m_dEnergy_Level_cm;
						dUpper = iterS->m_cLevel_Upper.m_dEnergy_Level_cm - 1.0e7L / iterS->m_dWavelength_nm;
					}

					std::cerr << iterS->m_dElement_Code << " " << iterS->m_dWavelength_nm << " energy difference error. Try " << dLower << " cm^-1, " << dUpper << " cm^-1, or " << dWL << " nm." << std::endl;
				}
			}
		}
		delete PKddData;
		PKddData = nullptr;

	}
	return 0;
}
