#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <line_routines.h>
#include <eps_plot.h>
#include <sstream>

using namespace epsplot;


std::vector<axis_metadata> * data::Get_Axis_Metedata_Vector_Ptr(AXIS i_eAxis)
{
	std::vector<axis_metadata> * lpvcAxis_Data;
	switch (i_eAxis)
	{
	case X_AXIS:
		lpvcAxis_Data = &m_cX_Axis_Parameters;
		break;
	case Y_AXIS:
		lpvcAxis_Data = &m_cY_Axis_Parameters;
		break;
	case Z_AXIS:
		lpvcAxis_Data = &m_cZ_Axis_Parameters;
		break;
	default:
		lpvcAxis_Data = nullptr;
		break;
	}
	return lpvcAxis_Data;
}

const std::vector<axis_metadata> * data::Get_Axis_Metedata_Vector_Ptr_Const(AXIS i_eAxis) const
{
	const std::vector<axis_metadata> * lpvcAxis_Data;
	switch (i_eAxis)
	{
	case X_AXIS:
		lpvcAxis_Data = &m_cX_Axis_Parameters;
		break;
	case Y_AXIS:
		lpvcAxis_Data = &m_cY_Axis_Parameters;
		break;
	case Z_AXIS:
		lpvcAxis_Data = &m_cZ_Axis_Parameters;
		break;
	default:
		lpvcAxis_Data = nullptr;
		break;
	}
	return lpvcAxis_Data;
}
unsigned int	data::Set_Axis_Parameters(AXIS i_eAxis, const axis_parameters & i_cAxis_Parameters)
{
	axis_metadata	cMeta_Data;
	std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr(i_eAxis);
	if(lpvcAxis_Data)
	{
		cMeta_Data.m_uiIdentifier = lpvcAxis_Data->size();
		cMeta_Data.m_cParameters = i_cAxis_Parameters;
		lpvcAxis_Data->push_back(cMeta_Data);
	}

	return cMeta_Data.m_uiIdentifier;
}

unsigned int	data::Set_X_Axis_Parameters(const axis_parameters & i_cAxis_Parameters)
{
	return Set_Axis_Parameters(X_AXIS,i_cAxis_Parameters);
}
unsigned int	data::Set_Y_Axis_Parameters(const axis_parameters & i_cAxis_Parameters)
{
	return Set_Axis_Parameters(Y_AXIS,i_cAxis_Parameters);
}
unsigned int	data::Set_Z_Axis_Parameters(const axis_parameters & i_cAxis_Parameters)
{
	return Set_Axis_Parameters(Z_AXIS,i_cAxis_Parameters);
}
unsigned int	data::Set_X_Axis_Parameters(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax)
{
	axis_parameters	cAxis_Parameters(i_lpszAxis_Description, i_bLog_Axis, i_bInvert_Axis, i_bSet_Min, i_dMin, i_bSet_Max, i_dMax);
	return Set_Axis_Parameters(X_AXIS,cAxis_Parameters);
}
unsigned int	data::Set_Y_Axis_Parameters(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax)
{
	axis_parameters	cAxis_Parameters(i_lpszAxis_Description, i_bLog_Axis, i_bInvert_Axis, i_bSet_Min, i_dMin, i_bSet_Max, i_dMax);
	return Set_Axis_Parameters(Y_AXIS,cAxis_Parameters);
}
unsigned int	data::Modify_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	unsigned int uiRet = -1;
	std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr(i_eAxis);
	if (lpvcAxis_Data && i_uiWhich < lpvcAxis_Data->size())
	{
		lpvcAxis_Data[0][i_uiWhich].m_cParameters = i_cAxis_Parameters;
		uiRet = lpvcAxis_Data[0][i_uiWhich].m_uiIdentifier;
	}
	return uiRet;
}
unsigned int	data::Modify_X_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	Modify_Axis_Parameters(X_AXIS,i_uiWhich,i_cAxis_Parameters);
}
unsigned int	data::Modify_Y_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	Modify_Axis_Parameters(Y_AXIS,i_uiWhich,i_cAxis_Parameters);
}
unsigned int	data::Modify_Z_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	Modify_Axis_Parameters(Z_AXIS,i_uiWhich,i_cAxis_Parameters);
}
axis_parameters	data::Get_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich) const
{
	axis_parameters	cRet;
	const std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr_Const(i_eAxis);
	if (lpvcAxis_Data && i_uiWhich < lpvcAxis_Data->size())
	{
		cRet = lpvcAxis_Data[0][i_uiWhich].m_cParameters;
	}
	return cRet;
}

axis_parameters	data::Get_X_Axis_Parameters(unsigned int i_uiWhich) const
{
	return Get_Axis_Parameters(X_AXIS, i_uiWhich);
}
axis_parameters	data::Get_Y_Axis_Parameters(unsigned int i_uiWhich) const
{
	return Get_Axis_Parameters(Y_AXIS, i_uiWhich);
}
axis_parameters	data::Get_Z_Axis_Parameters(unsigned int i_uiWhich) const
{
	return Get_Axis_Parameters(Z_AXIS, i_uiWhich);
}

