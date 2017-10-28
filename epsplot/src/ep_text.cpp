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


unsigned int	data::Set_Text_Data(const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_lpszText != nullptr)
	{
		text_item * lpItem = new text_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Text_Data(uiRet,i_dX, i_dY, i_lpszText,i_cLine_Parameters, i_cText_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}

	return uiRet;
}

unsigned int	data::Modify_Text_Data(unsigned int i_uiPlot_Data_ID, const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_lpszText != nullptr)
	{
		text_item * lpItem = (text_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_text)
		{
			lpItem->Set_Text(i_lpszText);
			lpItem->m_cLine_Parameters = i_cLine_Parameters;
			lpItem->m_cText_Parameters = i_cText_Parameters;
			lpItem->m_dX = i_dX;
			lpItem->m_dY = i_dY;
			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
		}
	}
}
