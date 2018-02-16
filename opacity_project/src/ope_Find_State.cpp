#include <opacity_project_pp.h>

opacity_project_state opacity_project_element::Find_State(const opacity_project_level_descriptor & i_poldLevel) const
{
	opacity_project_state stateI;
	if (m_uiZ == i_poldLevel.m_uiZ && i_poldLevel.m_uiN <= m_uiZ)
		stateI = m_vopiIon_Data[i_poldLevel.m_uiN - 1].Find_State(i_poldLevel);
	else
		std::cerr << "Failed to find state " << i_poldLevel.m_uiZ << " " << i_poldLevel.m_uiN << " " << i_poldLevel.m_uiS << " " << i_poldLevel.m_uiL << " " << i_poldLevel.m_uiP << " " << i_poldLevel.m_uiLvl_ID << std::endl;
	return stateI;
}



opacity_project_state opacity_project_element::Find_State(unsigned int i_uiN, unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
{
	return Find_State(opacity_project_level_descriptor(m_uiZ, i_uiN, i_uiState_S, i_uiState_L, i_uiState_P, i_uiID));
}
