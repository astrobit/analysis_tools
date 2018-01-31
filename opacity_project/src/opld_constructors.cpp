#include <opacity_project_pp.h>

opacity_project_level_descriptor::opacity_project_level_descriptor(void)
{
	m_uiN = m_uiZ = m_uiS = m_uiL = m_uiP = m_uiLvl_ID = -1;
}

opacity_project_level_descriptor::opacity_project_level_descriptor(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID)
{
	m_uiS = i_uiS;
	m_uiL = i_uiL;
	m_uiP = i_uiP;
	m_uiLvl_ID = i_uiLvl_ID;
	m_uiZ = i_uiZ;
	m_uiN = i_uiN;
}

