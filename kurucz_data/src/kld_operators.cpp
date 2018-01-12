#include <kurucz_data.h>

bool Kurucz_Level_Data::operator == (const Kurucz_Level_Data & i_cRHO) const
{
	return (i_cRHO.m_dEnergy_Level_cm == m_dEnergy_Level_cm &&
		i_cRHO.m_dJ == m_dJ &&
//			i_cRHO.m_uiNLTE_Index == m_uiNLTE_Index &&
//			i_cRHO.m_iHyperfine_Shift_mK == m_iHyperfine_Shift_mK &&
//			i_cRHO.m_iHyperfine_F == m_iHyperfine_F &&
		(i_cRHO.m_szLabel == m_szLabel || i_cRHO.m_szLabel == "AVERAGE   " || i_cRHO.m_szLabel == "ENERGIES  " || m_szLabel == "AVERAGE   " || m_szLabel == "ENERGIES  "));
	
}
bool Kurucz_Level_Data::operator <  (const Kurucz_Level_Data & i_cRHO) const
{
	return (m_dEnergy_Level_cm < i_cRHO.m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && m_dJ < i_cRHO.m_dJ));
}

