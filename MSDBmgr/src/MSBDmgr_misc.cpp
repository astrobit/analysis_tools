#include <MSDBmgr.hpp>

void MSDB_MGR_MAIN::Refresh_Spectra_List(void)
{
	m_vdbidSpectra_List.clear();
	if (m_cDB.Database_Exists())
	{	
		std::vector<msdb::dbid> vdbidList = m_cDB.Get_Spectra_List();
		if (!vdbidList.empty())
		{
			for (std::vector<msdb::dbid>::iterator cI = vdbidList.begin(); cI != vdbidList.end(); cI++)
			{
				msdb::PARAMETERS cParams;
				m_cDB.Get_Parameters(*cI,cParams);
				m_vdbidSpectra_List.push_back(std::pair<msdb::dbid,msdb::PARAMETERS>(*cI,cParams));		

			}
		}
	}
	if (!m_vdbidSpectra_List.empty())
		m_uiSelected_Spectrum = 0;
	else
		m_uiSelected_Spectrum = -1;
}

void MSDB_MGR_MAIN::Delete_Spectra(void)
{
	if (!m_vdbidSpectra_List.empty() && m_uiSelected_Spectrum < m_vdbidSpectra_List.size())
	{
		msdb::SPECTRUM_TYPE eType = Identify_Spectrum_Type(m_vdbidSpectra_List[m_uiSelected_Spectrum].second);

		m_cDB.Delete_Spectrum(m_vdbidSpectra_List[m_uiSelected_Spectrum].first,eType);
	}
}

void MSDB_MGR_MAIN::Regenerate_Spectra(void)
{
}

void MSDB_MGR_MAIN::Select_Next_Spectra(void)
{
	if (!m_vdbidSpectra_List.empty())
	{
		m_uiSelected_Spectrum++;
		if (m_uiSelected_Spectrum >= m_vdbidSpectra_List.size())
			m_uiSelected_Spectrum = 0;
		msdb::SPECTRUM_TYPE eType = Identify_Spectrum_Type(m_vdbidSpectra_List[m_uiSelected_Spectrum].second);
		m_cDB.Get_Spectrum(m_vdbidSpectra_List[m_uiSelected_Spectrum].first,eType,m_specCurr_Spectrum);
	}
	else
		m_uiSelected_Spectrum = -1;
}
void MSDB_MGR_MAIN::Select_Prev_Spectra(void)
{
	if (!m_vdbidSpectra_List.empty())
	{
		m_uiSelected_Spectrum--;
		if (m_uiSelected_Spectrum >= m_vdbidSpectra_List.size())
			m_uiSelected_Spectrum = m_vdbidSpectra_List.size() - 1;
		msdb::SPECTRUM_TYPE eType = Identify_Spectrum_Type(m_vdbidSpectra_List[m_uiSelected_Spectrum].second);
		m_cDB.Get_Spectrum(m_vdbidSpectra_List[m_uiSelected_Spectrum].first,eType,m_specCurr_Spectrum);
	}
	else
		m_uiSelected_Spectrum = -1;
}

