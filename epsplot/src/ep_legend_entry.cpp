#include <eps_plot.h>

		std::map<size_t,size_t>	cLegend_Entry_Crossref;

unsigned int	epsplot::data::Set_Legend_Entry(unsigned int i_uiLegend_ID, const legend_entry_parameters & i_cLegend_Parameters)
{
	unsigned int uiRet = -1;
	if (m_cLegends.count(i_uiLegend_ID) != 0)
	{
		if (m_cLegend_Entries.empty())
		{
			uiRet = 1;
			m_cLegend_Entries[1] = i_cLegend_Parameters;
			m_cLegend_Entry_Crossref[1] = i_uiLegend_ID;
		}
		else
		{
			uiRet = m_cLegend_Entries.crbegin()->first + 1;
			m_cLegend_Entries[uiRet] = i_cLegend_Parameters;
			m_cLegend_Entry_Crossref[uiRet] = i_uiLegend_ID;
		}
	}
	return uiRet;
}
unsigned int	epsplot::data::Modify_Legend_Entry(unsigned int i_uiLegend_Entry_ID, unsigned int i_uiLegend_ID, const legend_entry_parameters & i_cLegend_Parameters)
{
	unsigned int uiRet = -1;
	if (m_cLegends.count(i_uiLegend_ID) != 0 && m_cLegend_Entries.count(i_uiLegend_Entry_ID) != 0)
	{
		uiRet = i_uiLegend_Entry_ID;
		m_cLegend_Entries[i_uiLegend_Entry_ID] = i_cLegend_Parameters;
		m_cLegend_Entry_Crossref[i_uiLegend_Entry_ID] = i_uiLegend_ID;
	}
	return uiRet;
}
