#include <opacity_project_pp.h>

opacity_project_transition opacity_project_element::Get_Transition(const opacity_project_level_descriptor &i_opld_Lower_State, const opacity_project_level_descriptor &i_opld_Upper_State) const
{
	opacity_project_transition cRet;
	if (i_opld_Lower_State.m_uiZ == i_opld_Upper_State.m_uiZ &&
		i_opld_Lower_State.m_uiN == i_opld_Upper_State.m_uiN &&
		i_opld_Upper_State.m_uiN > 0)
	{
	
		//opacity_project_state sState_Lower = Find_State(i_opld_Lower_State);
		//opacity_project_state sState_Upper = Find_State(i_opld_Upper_State);
		msmst::const_iterator iterL = m_vopiIon_Data[i_opld_Lower_State.m_uiN - 1].m_msmtTransitions.find(i_opld_Lower_State);
		if (iterL != m_vopiIon_Data[i_opld_Lower_State.m_uiN - 1].m_msmtTransitions.end())
		{
			mst::const_iterator iterU = iterL->second.find(i_opld_Upper_State);
			if (iterU != iterL->second.end())
			{
				
				cRet = iterU->second;
			}
		}
	}
	return cRet;
}

