#include <eps_plot.h>

unsigned int	epsplot::data::Set_Legend(const epsplot::legend_parameters & i_cLegend_Parameters)
{
	unsigned int uiRet = -1;
	if (m_cLegends.empty())
	{
		uiRet = 1;
		m_cLegends[1] = i_cLegend_Parameters;
	}
	else
	{
		uiRet = m_cLegends.crbegin()->first + 1;
		m_cLegends[uiRet] = i_cLegend_Parameters;
	}
	return uiRet;
}


unsigned int	epsplot::data::Modify_Legend(unsigned int i_uiLegend_ID, const legend_parameters & i_cLegend_Parameters)
{
	unsigned int uiRet = -1;
	if (m_cLegends.count(i_uiLegend_ID) != 0)
	{
		m_cLegends[i_uiLegend_ID] = i_cLegend_Parameters;
	}
	return uiRet;
}


std::vector<size_t> epsplot::data::Get_Legend_Entries(unsigned int i_uiLegend_ID) const
{
	std::vector<size_t> vLE;
	for (auto iterI = m_cLegend_Entry_Crossref.cbegin(); iterI != m_cLegend_Entry_Crossref.cend(); iterI++)
	{
		if (iterI->second == i_uiLegend_ID)
		{
			vLE.push_back(iterI->first);
		}
	}
	return vLE;
}

epsplot::legend_parameters epsplot::data::Preprocess_Legend(unsigned int i_uiLegend_ID) const
{
	legend_parameters cRet;
	if (m_cLegends.count(i_uiLegend_ID) != 0)
	{
		cRet = m_cLegends.at(i_uiLegend_ID);
		if ((cRet.m_tNum_Col == -1 || cRet.m_tNum_Col == 0) && (cRet.m_tNum_Row == -1 || cRet.m_tNum_Row == 0))
		{
			cRet.m_tNum_Col = 1;
		}

		std::vector<size_t> vLE = Get_Legend_Entries(i_uiLegend_ID);

		if (cRet.m_tNum_Col == -1 || cRet.m_tNum_Col == 0)
		{
			cRet.m_tNum_Col = vLE.size() / cRet.m_tNum_Row;
			if (cRet.m_tNum_Row > 1 && (vLE.size() % cRet.m_tNum_Row) > 0)
				cRet.m_tNum_Col++;
		}
		else if (cRet.m_tNum_Row == -1 || cRet.m_tNum_Row == 0)
		{
			cRet.m_tNum_Row = vLE.size() / cRet.m_tNum_Col;
			if (cRet.m_tNum_Col > 1 && (vLE.size() % cRet.m_tNum_Col) > 0)
				cRet.m_tNum_Row++;
		}

		if (cRet.m_dLine_Length == -1.0)
		{
			double dMax_Length = 36.0;
			for (auto iterJ = vLE.cbegin(); iterJ != vLE.cend(); iterJ++)
			{
				double dLength = 0.0;
				unsigned int uiStipple_Size;
				const double * i_lpdStipple = Get_Stipple(m_cLegend_Entries.at(*iterJ).m_cLine_Parameters.m_eStipple,uiStipple_Size);
				for (unsigned int uiI = 0; uiI < uiStipple_Size; uiI++)
				{
					dLength += i_lpdStipple[uiI];
				}
				if (uiStipple_Size > 0)
					dLength += i_lpdStipple[0]; // repeat first section
				if (dLength > dMax_Length)
					dMax_Length = dLength;
			}
			cRet.m_dLine_Length = dMax_Length;
		}

		if (cRet.m_dGap_Width < 0.0)
			cRet.m_dGap_Width = 12.0;

		if (cRet.m_dColumn_Width <= 0.0)
		{
			double dWidth = cRet.m_dLine_Length + cRet.m_dGap_Width;
			double dMax_Text_Width = 0.0;
			for (auto iterJ = vLE.cbegin(); iterJ != vLE.cend(); iterJ++)
			{
				double dText_Width = Estimate_Text_Width(m_cLegend_Entries.at(*iterJ).m_szEntry_Text,cRet.m_cText_Parameters);
				if (dMax_Text_Width < dText_Width)
					dMax_Text_Width = dText_Width;
			}
			cRet.m_dColumn_Width = dWidth + dMax_Text_Width;
		}

		if (cRet.m_dRow_Height <= 0.0)
		{
			double dMax_Text_Height = 0.0;
			for (auto iterJ = vLE.cbegin(); iterJ != vLE.cend(); iterJ++)
			{
				double dText_Height = Estimate_Text_Height(m_cLegend_Entries.at(*iterJ).m_szEntry_Text,cRet.m_cText_Parameters);
				if (dMax_Text_Height < dText_Height)
					dMax_Text_Height = dText_Height;
			}
			cRet.m_dRow_Height = dMax_Text_Height * 1.1; // 10% larger for reasonable spacing
		}
	}
	return cRet;
}



