#include <opacity_project_pp.h>

bool opacity_project_level_descriptor::operator > (const opacity_project_level_descriptor & i_cRHO) const
{
	return (m_uiZ > i_cRHO.m_uiZ || (m_uiZ == i_cRHO.m_uiZ && m_uiN > i_cRHO.m_uiN) || (m_uiZ == i_cRHO.m_uiZ && m_uiN == i_cRHO.m_uiN &&
(m_uiS > i_cRHO.m_uiS || (m_uiS == i_cRHO.m_uiS && m_uiL > i_cRHO.m_uiL) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP > i_cRHO.m_uiP) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID > i_cRHO.m_uiLvl_ID))));
}
bool opacity_project_level_descriptor::operator >= (const opacity_project_level_descriptor & i_cRHO) const
{
	return (*this > i_cRHO || *this == i_cRHO);
}
bool opacity_project_level_descriptor::operator < (const opacity_project_level_descriptor & i_cRHO) const
{
	return !(*this >= i_cRHO);
}
bool opacity_project_level_descriptor::operator <= (const opacity_project_level_descriptor & i_cRHO) const
{
	return !(*this < i_cRHO);
}
bool opacity_project_level_descriptor::operator == (const opacity_project_level_descriptor & i_cRHO) const
{
	return (m_uiZ == i_cRHO.m_uiZ && m_uiN == i_cRHO.m_uiN && m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID == i_cRHO.m_uiLvl_ID);
}
bool opacity_project_level_descriptor::operator != (const opacity_project_level_descriptor & i_cRHO) const
{
	return (m_uiZ != i_cRHO.m_uiZ || m_uiN != i_cRHO.m_uiN || m_uiS != i_cRHO.m_uiS || m_uiL != i_cRHO.m_uiL || m_uiP != i_cRHO.m_uiP || m_uiLvl_ID != i_cRHO.m_uiLvl_ID);
}
