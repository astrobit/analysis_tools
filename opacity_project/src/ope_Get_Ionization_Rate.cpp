#include <opacity_project_pp.h>

long double opacity_project_element::Get_Ionization_Rate(const opacity_project_level_descriptor &i_opld_State, const long double & i_dCorrected_Energy_Ry, const radiation_field & i_cRad, const long double &  i_dRedshift) const
{
	opacity_project_state sState = Find_State(i_opld_State);
	return m_vopiIon_Data[i_opld_State.m_uiN - 1].Calc_Ionizing_Rate(sState, i_cRad, i_dCorrected_Energy_Ry, i_dRedshift);

}
