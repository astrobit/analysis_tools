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

///////////////////////////////////////////////////////////////////////////////
//
// Rectangle
//
///////////////////////////////////////////////////////////////////////////////

unsigned int	data::Set_Rectangle_Data(const rectangle & i_cArea, bool i_bFill, COLOR i_eFill_Color, bool i_bBorder, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	rectangle_item * lpItem = new rectangle_item;

	if (lpItem != nullptr)
	{
		uiRet = m_vcPlot_Item_List.size();
		m_vcPlot_Item_List.push_back(lpItem);
		Modify_Rectangle_Data(uiRet,i_cArea, i_bFill, i_eFill_Color, i_bBorder, i_cLine_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

	}

	return uiRet;
}

unsigned int	data::Modify_Rectangle_Data(unsigned int i_uiPlot_Data_ID, const rectangle & i_cArea, bool i_bFill, COLOR i_eFill_Color, bool i_bBorder, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	rectangle_item * lpItem = (rectangle_item *)Get_Plot_Item(i_uiPlot_Data_ID);
	if (lpItem != nullptr && lpItem->m_eType == type_rectangle)
	{
		uiRet = i_uiPlot_Data_ID;

		lpItem->m_cPlot_Rectangle_Info = i_cArea;
		lpItem->m_bArea_Fill = i_bFill;
		lpItem->m_ePlot_Area_Fill_Color = i_eFill_Color;
		lpItem->m_bDraw_Border = i_bBorder;
		lpItem->m_cPlot_Border_Info = i_cLine_Parameters;
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;

		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpItem->m_cPlot_Border_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpItem->m_cPlot_Border_Info.m_dWidth = 1.0;

	}
	return uiRet;
}
