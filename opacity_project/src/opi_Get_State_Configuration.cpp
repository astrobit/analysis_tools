#include <opacity_project_pp.h>

std::string opacity_project_ion::Get_State_Configuration(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
{
	std::string szRet;
	opacity_project_state stateI = Find_State(i_uiState_S,i_uiState_L,i_uiState_P,i_uiID);
	if (stateI.m_opldDescriptor.m_uiS != -1)
	{
		std::ostringstream ossConfig;
		if (stateI.m_chType == 'T')
		{
			ossConfig << m_cConfiguration_Data_Target[stateI.m_uiType_Index - 1].m_szState;
			ossConfig << " " << stateI.m_ui_n;
			if (stateI.m_ui_l == 0)
				ossConfig << "s";
			else if (stateI.m_ui_l == 1)
				ossConfig << "p";
			else if (stateI.m_ui_l == 2)
				ossConfig << "d";
			else if (stateI.m_ui_l >= 3 && stateI.m_ui_l <= 6)
			{
				char chState[2] = {(char)('f' + stateI.m_ui_l - 3),0};
				ossConfig << chState;
			}
			else if (stateI.m_ui_l >= 7)
			{
				char chState[2] = {(char)('k' + stateI.m_ui_l - 7),0};
				ossConfig << chState;
			}
		}
		else
			ossConfig << m_cConfiguration_Data_Continuum[stateI.m_uiType_Index - 1].m_szState;
		ossConfig << " " << stateI.m_opldDescriptor.m_uiS;
		if (stateI.m_opldDescriptor.m_uiL == 0)
			ossConfig << "S";
		else if (stateI.m_opldDescriptor.m_uiL == 1)
			ossConfig << "P";
		else if (stateI.m_opldDescriptor.m_uiL == 2)
			ossConfig << "D";
		else if (stateI.m_opldDescriptor.m_uiL >= 3 && stateI.m_opldDescriptor.m_uiL <= 6)
		{
			char chState[2] = {(char)('F' + stateI.m_opldDescriptor.m_uiL - 3),0};
			ossConfig << chState;
		}
		else if (stateI.m_opldDescriptor.m_uiL >= 7)
		{
			char chState[2] = {(char)('K' + stateI.m_opldDescriptor.m_uiL - 7),0};
			ossConfig << chState;
		}
		if (stateI.m_opldDescriptor.m_uiP == 1)
			ossConfig << "*";
		szRet = ossConfig.str();
	}
	return szRet;
}
