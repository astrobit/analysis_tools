#include <opacity_project_pp.h>

void opacity_project_element::Read_Element_Data(unsigned int i_uiZ)
{
	m_uiZ = i_uiZ;
	for (unsigned int uiI = 1; uiI <= i_uiZ; uiI++)
	{
		opacity_project_ion opiIon_Data;
		opiIon_Data.Read_Data(i_uiZ,uiI);
		m_vopiIon_Data.push_back(opiIon_Data);
	}
}
