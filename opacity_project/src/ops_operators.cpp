#include <opacity_project_pp.h>

bool opacity_project_state::operator < (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor < i_cRHO.m_opldDescriptor;
}
bool opacity_project_state::operator <= (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor <= i_cRHO.m_opldDescriptor;
}
bool opacity_project_state::operator > (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor > i_cRHO.m_opldDescriptor;
}
bool opacity_project_state::operator >= (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor >= i_cRHO.m_opldDescriptor;
}
bool opacity_project_state::operator == (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor == i_cRHO.m_opldDescriptor;
}
bool opacity_project_state::operator != (const opacity_project_state & i_cRHO) const
{
	return m_opldDescriptor != i_cRHO.m_opldDescriptor;
}
