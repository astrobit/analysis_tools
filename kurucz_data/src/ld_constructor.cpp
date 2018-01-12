#include <kurucz_data.h>

level_definition::level_definition(const Kurucz_Level_Data & i_klvdLevel)
{
	m_tS = m_tL = m_tP = -1;
	m_dK = -1.0L;
	m_dEnergy_Level_cm = i_klvdLevel.m_dEnergy_Level_cm;
	m_dJ = i_klvdLevel.m_dJ;
//		std::cout << m_dEnergy_Level_cm << " " << m_dJ << " " << i_klvdLevel.m_szLabel << std::endl;
	std::string szLabel = i_klvdLevel.m_szLabel;
	while (szLabel.size() > 0 && szLabel.back() == ' ')
		szLabel.pop_back();
	if (szLabel.size() > 0)
	{
		if (szLabel.back() >= 'A' && szLabel.back() <= 'Z')
		{
			char chL = szLabel.back();
			szLabel.pop_back();
			if (chL == 'S')
				m_tL = 1;
			else if (chL == 'P')
				m_tL = 2;
			else if (chL == 'D')
				m_tL = 3;
			else// if (chL < 'J')
			{
				m_tL = (chL - 'F') + 3;
				if (chL > 'J') // J is skipped
					m_tL--;
				if (chL > 'P') // P is L = 2
					m_tL--;
				if (chL > 'S') // S is L = 1
					m_tL--;
			}
			if (szLabel.size() > 0 && szLabel.back() >= '0' && szLabel.back() <= '9')
				m_tS = (szLabel.back() - '0');
			if (szLabel.size() > 0)
				szLabel.pop_back();
			if (szLabel.size() > 0 && szLabel.back() == '*')
				m_tP = 1;
		}
		else if (szLabel.back() == ']') // J K coupling term
		{
			szLabel.pop_back();
			if (szLabel.size() > 0 && szLabel.back() == '+')
			{
				szLabel.pop_back();
				m_dK = 0.5;
				if (szLabel.size() > 0 && szLabel.back() >= '0' && szLabel.back() <= '9')
				{
					m_dK += (szLabel.back() - '0');
					szLabel.pop_back();
				}
				if (szLabel.size() > 0)
					szLabel.pop_back(); // brace
				if (szLabel.size() > 0 && szLabel.back() >= '0' && szLabel.back() <= '9')
				{
					m_tS = szLabel.back() - '0';
				}					
			}
			else
			{
				if (szLabel.size() > 0 && szLabel.back() >= '0' && szLabel.back() <= '9')
				{
					m_dK = (szLabel.back() - '0') * 0.5;
					szLabel.pop_back();
				}
				if (szLabel.size() > 0)
					szLabel.pop_back(); // brace
				if (szLabel.size() > 0 && szLabel.back() >= '0' && szLabel.back() <= '9')
				{
					m_tS = szLabel.back() - '0';
				}
			}					
		}
	}
}
