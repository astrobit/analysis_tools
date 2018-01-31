#include <opacity_project_pp.h>

opacity_project_state::opacity_project_state(void)
{
	m_opldDescriptor.m_uiS = -1; 
	m_opldDescriptor.m_uiL = -1; 
	m_opldDescriptor.m_uiP = -1; 
	m_opldDescriptor.m_uiLvl_ID = -1; 
	m_dEnergy_eV = 0.0;
}
opacity_project_state::opacity_project_state(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID, long double i_dEnergy_eV) : m_opldDescriptor(i_uiZ, i_uiN, i_uiS,i_uiL,i_uiP,i_uiLvl_ID)
 {
	m_dEnergy_eV = i_dEnergy_eV;
}

