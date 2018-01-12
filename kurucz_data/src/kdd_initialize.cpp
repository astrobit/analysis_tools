#include <kurucz_data.h>

void kurucz_derived_data::Initialize(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion)
{
	m_kdKurucz_Data.Load_Data(i_uiN,i_uiMin_Ion,i_uiMax_Ion);
	// Build list of levels and the associated transitions
	for (vvkld::iterator iterI = m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterI != m_kdKurucz_Data.m_vvkldLine_Data.end(); iterI++)
	{
		mklvd mklvdCurr_Ion_Level_Data;
		long double dIon_State = (iterI->begin()->m_dElement_Code - i_uiN) * 100.0; // 0.01 for rounding error
		if (dIon_State <= (i_uiMax_Ion + 0.02)) // 0.02: rounding error
		{
			for (ivkld iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
			{

				transition_type tLU_Type = tt_absorption;
				transition_type tUL_Type = tt_emission;

				if (iterJ->m_cLevel_Lower > iterJ->m_cLevel_Upper)
				{
					// 22 Dec. 2017: verified that some Kurucz data is inverted
//						std::cout << "lower - upper reverse for trx " << iterJ->m_dElement_Code << " " << iterJ->m_cLevel_Lower.m_szLabel << " -- " << iterJ->m_cLevel_Upper.m_szLabel << " (" << iterJ->m_dWavelength_nm << ") [" << iterJ->m_cLevel_Lower.m_dEnergy_Level_cm << " -- " << iterJ->m_cLevel_Upper.m_dEnergy_Level_cm << "]" << std::endl;
					tLU_Type = tt_emission;
					tUL_Type = tt_absorption;
				}
				Process_Level(tLU_Type,iterJ,iterJ->m_cLevel_Lower,mklvdCurr_Ion_Level_Data);
				Process_Level(tUL_Type,iterJ,iterJ->m_cLevel_Upper,mklvdCurr_Ion_Level_Data);
			}
			m_vmklvdLevel_Data.push_back(mklvdCurr_Ion_Level_Data);
		}

	}
	for (vmklvd::iterator iterI = m_vmklvdLevel_Data.begin(); iterI != m_vmklvdLevel_Data.end(); iterI++)
	{
		for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
		{
			// calculate the gamma for each level; this is the sum of Einstein A coefficients for all transitions
			// this is required for determining the width of the Lorentzian wings
			iterJ->second.klvdLevel_Data.m_dGamma = 0.0;
			for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
			{
				iterJ->second.klvdLevel_Data.m_dGamma += (*iterK)->m_dEinstein_A;
			}
			// fill in the data stored in the transition data for this particular level
			for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
			{
				(*iterK)->m_cLevel_Upper.m_dGamma = iterJ->second.klvdLevel_Data.m_dGamma;
			}
			for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
			{
				(*iterK)->m_cLevel_Lower.m_dGamma = iterJ->second.klvdLevel_Data.m_dGamma;
			}
		}
	}
}
