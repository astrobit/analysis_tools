#include <opacity_project_pp.h>

long double opacity_project_element::Get_Recombination_Rate(const opacity_project_level_descriptor &i_opld_Ionized_State, const opacity_project_level_descriptor &i_opld_Recombined_State, const long double & i_dCorrected_Energy_Ry, const velocity_function & i_cVel) const
{
	opacity_project_state sIonized_State;
	if (i_opld_Ionized_State.m_uiN != 0)
		sIonized_State = Find_State(i_opld_Ionized_State);
	else
	{
		sIonized_State.m_opldDescriptor = i_opld_Ionized_State;
		sIonized_State.m_uiStatistical_Weight = i_opld_Ionized_State.m_uiL * 4 + 2;;
	}
	opacity_project_state sRecombined_State = Find_State(i_opld_Recombined_State);
	return m_vopiIon_Data[i_opld_Recombined_State.m_uiN - 1].Calc_Recombination_Rate(sIonized_State, sRecombined_State, i_cVel, i_dCorrected_Energy_Ry);
}
