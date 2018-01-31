#include <opacity_project_pp.h>

opacity_project_state opacity_project_ion::Find_State(const opacity_project_level_descriptor &i_opldDescriptor) const
{
	opacity_project_state stateI;
	if (i_opldDescriptor.m_uiZ != m_uiZ || i_opldDescriptor.m_uiN != m_uiN)
		std::cerr << "Z,N mismatch " << i_opldDescriptor.m_uiZ << " " << i_opldDescriptor.m_uiN << " -- " << m_uiZ << " " << m_uiN << std::endl;
	if (m_msStates.count(i_opldDescriptor) == 1)
		stateI = m_msStates.at(i_opldDescriptor);
	else
	{
		char lpszSymb[4];
		char lpszNum[32];
		xGet_Element_Symbol(m_uiZ,lpszSymb);
		xRomanNumeralGenerator(lpszNum,m_uiZ - m_uiN + 1);
		std::cerr << "Failed to find state " << lpszSymb << " " << lpszNum << ":" << i_opldDescriptor.m_uiS << " " << i_opldDescriptor.m_uiL << " " << i_opldDescriptor.m_uiP << " " << i_opldDescriptor.m_uiLvl_ID << " " << m_msStates.count(i_opldDescriptor) << std::endl;
	}
	return stateI;
}
opacity_project_state opacity_project_ion::Find_State(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
{
	return Find_State(opacity_project_level_descriptor(m_uiZ,m_uiN,i_uiState_S,i_uiState_L,i_uiState_P,i_uiID));
}

