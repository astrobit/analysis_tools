#include <kurucz_data.h>

void kurucz_derived_data::Process_Level(kurucz_derived_data::transition_type i_eType, ivkld &i_ivkldTransition, const Kurucz_Level_Data &i_kvldLevel_Data, mklvd &mklvdCurr_Ion_Level_Data)
{
	if (mklvdCurr_Ion_Level_Data.count(level_definition(i_kvldLevel_Data)) == 0)
	{
		kurucz_combined_data cdlvData;
		cdlvData.klvdLevel_Data = i_kvldLevel_Data;
		if (i_eType == tt_emission)
			cdlvData.vivkldEmission_Transition_Data.push_back(i_ivkldTransition);
		else
			cdlvData.vivkldAbsorption_Transition_Data.push_back(i_ivkldTransition);
		mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)] = cdlvData;
	}
	else if (mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data != i_kvldLevel_Data) // same label, different data
	{
		std::string sCurr_Level = mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_szLabel;
		std::string sTest_Level = i_kvldLevel_Data.m_szLabel;
		size_t tAsterisk_Curr = sCurr_Level.find('*');
		size_t tAsterisk_Test = sTest_Level.find('*');
		bool bFlag_S_Indeterminate = false;
		

		// the Kurucz data are inconsisent about labeling odd parity; strip the parity information and test again.
		size_t tAsterisk;
		if (tAsterisk_Curr != std::string::npos)
		{
			sCurr_Level[tAsterisk_Curr] = ' ';
			if (sCurr_Level[tAsterisk_Curr + 1] == ' ')
			{
				bFlag_S_Indeterminate = true;
			}
		}
		if (tAsterisk_Test != std::string::npos)
		{
			sTest_Level[tAsterisk_Test] = ' ';
			if (sTest_Level[tAsterisk_Curr + 1] == ' ')
			{
				bFlag_S_Indeterminate = true;
			}
		}
		
		// for cases e.g. Si III 6h *3H and 6h *1H J = 5, the S of a given transtion is indeterminate; ignore this diference and just consider these to be the same state by stripping the S information
		if (bFlag_S_Indeterminate)
		{
			if (tAsterisk_Curr != std::string::npos)
			{
				sCurr_Level[tAsterisk_Test + 1] = ' ';
			}
			if (tAsterisk_Test != std::string::npos)
			{
				sTest_Level[tAsterisk_Test + 1] = ' ';
			}
		}

		// strip all spaces out so that e.g. "3d  2P" and "3d 2P" are identified as the same thing
		size_t tSpace;
		while ((tSpace = sCurr_Level.find(' ')) != std::string::npos)
			sCurr_Level.erase(tSpace,1);
		while ((tSpace = sTest_Level.find(' ')) != std::string::npos)
			sTest_Level.erase(tSpace,1);

			
		if (sTest_Level == sCurr_Level ||
			(sCurr_Level.substr(1,sCurr_Level.size() - 1) == sTest_Level.substr(0,sCurr_Level.size() - 1)) ||
			(sCurr_Level.substr(0,sCurr_Level.size() - 1) == sTest_Level.substr(1,sCurr_Level.size() - 1)))
		{ // some configuration definitions have a quirk in which it is sometimes described as something line p4p and sometimes just 4p
			if (i_eType == tt_emission)
				mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldEmission_Transition_Data.push_back(i_ivkldTransition);
			else
				mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldAbsorption_Transition_Data.push_back(i_ivkldTransition);
		}
		else
		{
			std::cout << "Energy mismatch in " << i_kvldLevel_Data.m_dElement_Code << " " << i_ivkldTransition->m_dWavelength_nm << " nm :";
			std::cout << i_kvldLevel_Data.m_dEnergy_Level_cm << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_dEnergy_Level_cm << " ";
			std::cout << std::endl;
		}
	}
	else
	{
		if (i_eType == tt_emission)
			mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldEmission_Transition_Data.push_back(i_ivkldTransition);
		else
			mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldAbsorption_Transition_Data.push_back(i_ivkldTransition);
	}
}
