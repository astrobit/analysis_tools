#include <opacity_project_pp.h>

void opacity_project_ion::Read_Data(unsigned int i_uiZ, unsigned int i_uiN)
{
	m_uiZ = i_uiZ;
	m_uiN = i_uiN;
	m_msStates = Read_State_Data();
	m_msmtTransitions = Read_Transition_Data();
	m_mscsPI_Cross_Sections = Read_Opacity_PI_Data();
	Read_Configuration_Data();
}
