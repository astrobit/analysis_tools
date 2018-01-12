#include <statepop.h>

statepop::vector statepop::Get_Relative_Line_Strengths(void)
{
	vector vEig = Get_Populations();
	vector vRet(vTransition_Data.size());

	for(size_t tI = 0; tI < vTransition_Data.size(); tI++)
	{
		transition_data cTrx = vTransition_Data[tI];
		// 10^-24 converts wavelengths in Angstrom to cm
//		floattype fLine_Strength = 1.0e-24 * cTrx.ldEinstein_A / (8.0 * g_XASTRO.k_dpi) * cTrx.ldWavenegth_Angstroms * cTrx.ldWavenegth_Angstroms * cTrx.ldWavenegth_Angstroms *
//			((2.0 * cTrx.ldUpper_Level_J + 1.0) / (2.0 * cTrx.ldLower_Level_J + 1.0) * vEig[cTrx.tLower_Level_ID] - vEig[cTrx.tUpper_Level_ID]);
		// 10^-8 = angstrom -> cm
		if (cTrx.tLower_Level_ID != -1 && cTrx.tUpper_Level_ID != -1)
		{			
			floattype fLine_Strength = cTrx.ldEinstein_B * g_XASTRO.k_dh * 
				(vEig[cTrx.tLower_Level_ID] - (2.0 * cTrx.ldLower_Level_J + 1.0) / (2.0 * cTrx.ldUpper_Level_J + 1.0) * vEig[cTrx.tUpper_Level_ID]);

			vRet[tI] = fLine_Strength;
		}
		else
		{
			std::cerr << "Transition " << tI << " has unidentified states" << std::endl;
			vRet[tI] = 0.0; // not sure what else to do
		}
	}
	return vRet;
}
statepop::vector statepop::Get_Boltzmann_Relative_Line_Strengths(void)
{
	std::map<size_t,boltzmann_info> vEig = Get_Boltzmann_Populations();
	vector vRet(vTransition_Data.size());

	for(size_t tI = 0; tI < vTransition_Data.size(); tI++)
	{
		transition_data cTrx = vTransition_Data[tI];

		floattype fLine_Strength = cTrx.ldEinstein_B * g_XASTRO.k_dh * 
			(std::pow(10.0,vEig[cTrx.tLower_Level_ID].ldLog_Pop) - (2.0 * cTrx.ldLower_Level_J + 1.0) / (2.0 * cTrx.ldUpper_Level_J + 1.0) * std::pow(10.0,vEig[cTrx.tUpper_Level_ID].ldLog_Pop));

//		floattype fLine_Strength = 1.0e-24 * cTrx.ldEinstein_A / (8.0 * g_XASTRO.k_dpi) * cTrx.ldWavenegth_Angstroms * cTrx.ldWavenegth_Angstroms * cTrx.ldWavenegth_Angstroms *
//			((2.0 * cTrx.ldUpper_Level_J + 1.0) / (2.0 * cTrx.ldLower_Level_J + 1.0) * std::pow(10.0,vEig[cTrx.tLower_Level_ID].ldLog_Pop) - std::pow(10.0,vEig[cTrx.tUpper_Level_ID].ldLog_Pop));

		vRet[tI] = fLine_Strength;
	}
	return vRet;
}

