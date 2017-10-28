#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <eps_plot.h>
#include <sstream>

using namespace epsplot;

///////////////////////////////////////////////////////////////////////////////
//
// Errorbars
//
///////////////////////////////////////////////////////////////////////////////
unsigned int	data::Set_Errorbar_Data(const errorbar_parameters & i_cErrorbar_Parameters, const std::vector<double> &i_vdValues, const line_parameters & i_cLine_Parameters)
{
	unsigned int uiRet = -1;
	line_item * lpLine_Item = (line_item *)Get_Plot_Item(i_cErrorbar_Parameters.m_uiAssociated_Plot);
	symbol_item * lpSymbol = (symbol_item *) lpLine_Item;
	if (lpLine_Item != nullptr && (lpLine_Item->m_eType == type_symbol || lpLine_Item->m_eType == type_line))
	{
		errorbar_item * lpItem = new errorbar_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Errorbar_Data(uiRet,i_cErrorbar_Parameters, i_vdValues, i_cLine_Parameters);

		}
	}
	return uiRet;
}

unsigned int	data::Modify_Errorbar_Data(unsigned int i_uiPlot_Data_ID, const errorbar_parameters & i_cErrorbar_Parameters, const std::vector<double> &i_vdValues, const line_parameters & i_cLine_Parameters)
{
	unsigned int uiRet = -1;
	line_item * lpLine_Item = (line_item *)Get_Plot_Item(i_cErrorbar_Parameters.m_uiAssociated_Plot);
	if (lpLine_Item != nullptr && (lpLine_Item->m_eType == type_symbol || lpLine_Item->m_eType == type_line))
	{
		errorbar_item * lpItem = (errorbar_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_errorbar)
		{
			lpItem->m_uiPlot_Axes_To_Use[0] = lpLine_Item->m_uiPlot_Axes_To_Use[0];
			lpItem->m_uiPlot_Axes_To_Use[1] = lpLine_Item->m_uiPlot_Axes_To_Use[1];
			lpItem->m_cErrorbar_Info = i_cErrorbar_Parameters;
			lpItem->m_uiNum_Points = i_vdValues.size();
			lpItem->m_cPlot_Line_Info = i_cLine_Parameters;
			if (lpItem->m_lppData != nullptr)
				delete [] lpItem->m_lppData;
			lpItem->m_lppData = new double[lpItem->m_uiNum_Points];
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI] = i_vdValues[uiI];
			}
		}
	}
	return uiRet;
}
