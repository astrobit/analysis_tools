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
// Line Plots
//
///////////////////////////////////////////////////////////////////////////////

unsigned int	data::Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, COLOR i_eColor, STIPPLE i_eStipple, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type, const double & i_dLine_Width)
{
	line_parameters	cLine_Param;

	cLine_Param.m_eColor = i_eColor;
	cLine_Param.m_dWidth = i_dLine_Width;
	cLine_Param.m_eStipple = i_eStipple;
	return Set_Plot_Data(i_lpdX_Values,i_lpdY_Values,i_uiNum_Points, cLine_Param, i_uiX_Axis_Type, i_uiY_Axis_Type);
}

unsigned int	data::Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_uiNum_Points != 0 && i_lpdX_Values != nullptr && i_lpdY_Values != nullptr)
	{
		line_item * lpItem = new line_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Plot_Data(uiRet,i_lpdX_Values, i_lpdY_Values, i_uiNum_Points,i_cLine_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}
	return uiRet;
}

unsigned int	data::Set_Plot_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != 0 && i_vdX_Values.size() == i_vdY_Values.size())
	{
		line_item * lpItem = new line_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Plot_Data(uiRet,i_vdX_Values, i_vdY_Values, i_cLine_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}
	else if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Set_Plot_Data called using std::vector, but X and Y vectors differ in size.\n");
	}

	return uiRet;
}
unsigned int	data::Set_Plot_Data(const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vpValues.size() != 0)
	{
		line_item * lpItem = new line_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Plot_Data(uiRet, i_vpValues, i_cLine_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}

	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	line_item * lpItem = (line_item *)Get_Plot_Item(i_uiPlot_Data_ID);
	if (lpItem != nullptr && lpItem->m_eType == type_line)
	{
		uiRet = i_uiPlot_Data_ID;

		lpItem->m_uiNum_Points = i_uiNum_Points;
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
		lpItem->m_cPlot_Line_Info = i_cLine_Parameters;
		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpItem->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpItem->m_cPlot_Line_Info.m_dWidth = 1.0;
		if (lpItem->m_lppData)
			delete [] lpItem->m_lppData;
		lpItem->m_lppData = nullptr;
		if (i_uiNum_Points > 0)
		{
			lpItem->m_lppData = new eps_pair [i_uiNum_Points];
			for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
				lpItem->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Modify_Plot_Data called using std::vector, but X and Y vectors differ in size.\n");
	}
	else
	{
		line_item * lpItem = (line_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_line)
		{
			uiRet = i_uiPlot_Data_ID;

			lpItem->m_uiNum_Points = i_vdX_Values.size();
			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Line_Info = i_cLine_Parameters;
			if (i_cLine_Parameters.m_dWidth >= 0.0)
				lpItem->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
			else
				lpItem->m_cPlot_Line_Info.m_dWidth = 1.0;
			if (lpItem->m_lppData)
				delete [] lpItem->m_lppData;
			lpItem->m_lppData = nullptr;
			if (lpItem->m_uiNum_Points > 0)
			{
				lpItem->m_lppData = new eps_pair [lpItem->m_uiNum_Points];
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
					lpItem->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
				}
			}
		}
	}
	return uiRet;

}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	line_item * lpItem = (line_item *)Get_Plot_Item(i_uiPlot_Data_ID);
	if (lpItem != nullptr && lpItem->m_eType == type_line)
	{
		uiRet = i_uiPlot_Data_ID;

		lpItem->m_uiNum_Points = i_vpValues.size();
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
		lpItem->m_cPlot_Line_Info = i_cLine_Parameters;
		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpItem->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpItem->m_cPlot_Line_Info.m_dWidth = 1.0;
		if (lpItem->m_lppData)
			delete [] lpItem->m_lppData;
		lpItem->m_lppData = nullptr;
		if (lpItem->m_uiNum_Points > 0)
		{
			lpItem->m_lppData = new eps_pair [lpItem->m_uiNum_Points];
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI] = i_vpValues[uiI];
			}
		}
	}
	return uiRet;
}

