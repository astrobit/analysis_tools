#include <statepop.h>

std::vector<opacity_project_level_descriptor> statepop::Find_Equivalent_Recombined_Levels(const mcfg &i_mcfgConfigs, const vecconfig & i_vcfgConfig, bool i_bQuiet)
{
	std::vector<opacity_project_level_descriptor> vopldLevels;
//	if (i_vcfgConfig.size() != i_mcfgConfigs.begin()->first.m_uiZ)
//		Print_Config_Vector(i_vcfgConfig,std::cout);
	for(auto iterK = i_mcfgConfigs.begin(); iterK != i_mcfgConfigs.end(); iterK++)
	{
		if (i_vcfgConfig.size() != iterK->first.m_uiZ)
		{
//			if (i_vcfgConfig.size() == (iterK->first.m_uiN - 1))
//			{
//				std::cout << "test " << iterK->first.m_uiS << " " << iterK->first.m_uiL << " " << iterK->first.m_uiP << " " << iterK->first.m_uiLvl_ID;
//			}
			if (Ion_Recombine_Equivalence(i_vcfgConfig,iterK->second))
			{
				vopldLevels.push_back(iterK->first);
			}
//			if (i_vcfgConfig.size() == (iterK->first.m_uiN - 1))
//				std::cout << std::endl;
		}
	}
	return vopldLevels;
}
