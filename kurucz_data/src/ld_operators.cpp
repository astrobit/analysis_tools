#include <kurucz_data.h>

bool level_definition::operator ==  (const level_definition & i_cRHO) const
{
	return (i_cRHO.m_dEnergy_Level_cm == m_dEnergy_Level_cm && i_cRHO.m_dJ == m_dJ &&
				(
				 (m_dK == -1 && i_cRHO.m_dK == -1 && i_cRHO.m_tS == m_tS && i_cRHO.m_tL == m_tL) ||
				 (m_dK == -1 && i_cRHO.m_dK == -1 && (i_cRHO.m_tS == -1 || m_tS == -1) && i_cRHO.m_tL == m_tL) ||
				 (m_dK == i_cRHO.m_dK && m_tL == -1 && i_cRHO.m_tS == m_tS && i_cRHO.m_tL == -1)
				)
			);
}
bool level_definition::operator <  (const level_definition & i_cRHO) const
{
	return (
				(i_cRHO.m_dEnergy_Level_cm > m_dEnergy_Level_cm) || 
				(m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ > m_dJ) ||
				(m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ == m_dJ &&  
					(
						(m_dK != -1 && m_dK == i_cRHO.m_dK && i_cRHO.m_tS > m_tS) ||
						(m_dK != -1 && i_cRHO.m_dK != -1 && i_cRHO.m_dK > m_dK) ||
						(m_dK == -1 && i_cRHO.m_dK != -1) ||
						(m_dK == -1 && i_cRHO.m_dK == -1 && i_cRHO.m_tL > m_tL) ||
						(m_dK == -1 && i_cRHO.m_dK == -1 && i_cRHO.m_tL == m_tL && i_cRHO.m_tS > m_tS)
					)
				)
			);
}

