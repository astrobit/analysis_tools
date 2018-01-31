#include <opacity_project_pp.h>

opacity_project_ion::opacity_project_ion(void)
{
	m_uiN = m_uiZ = -1;
}
opacity_project_ion::opacity_project_ion(unsigned int i_uiZ, unsigned int i_uiN)
{
	Read_Data(i_uiZ,i_uiN);
}

