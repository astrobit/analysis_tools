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
// Symbols
//
///////////////////////////////////////////////////////////////////////////////
unsigned int	data::Set_Symbol_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_uiNum_Points != 0 && i_lpdX_Values != nullptr && i_lpdY_Values != nullptr)
	{
		symbol_item * lpItem = new symbol_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Symbol_Data(uiRet,i_lpdX_Values, i_lpdY_Values, i_uiNum_Points,i_cSymbol_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}
	return uiRet;
}
unsigned int	data::Set_Symbol_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Set_Symbol_Data called using std::vector, but X and Y vectors differ in size.\n");
	}
	else if (i_vdX_Values.size() > 0)
	{
		symbol_item * lpItem = new symbol_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Symbol_Data(uiRet,i_vdX_Values, i_vdY_Values, i_cSymbol_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}
	return uiRet;
}
unsigned int	data::Set_Symbol_Data(const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vpValues.size() > 0)
	{
		symbol_item * lpItem = new symbol_item;

		if (lpItem != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
			Modify_Symbol_Data(uiRet,i_vpValues, i_cSymbol_Parameters, i_uiX_Axis_ID, i_uiY_Axis_ID);

		}
	}
	return uiRet;
}

unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_lpdX_Values != nullptr && i_lpdY_Values != nullptr && i_uiNum_Points > 0)
	{
		symbol_item * lpItem = (symbol_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_symbol)
		{
			uiRet = i_uiPlot_Data_ID;

			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
	
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			if (lpItem->m_lppData && lpItem->m_uiNum_Points < i_uiNum_Points)
			{
				delete [] lpItem->m_lppData;
				lpItem->m_lppData = new eps_pair [i_uiNum_Points];
			}
			lpItem->m_uiNum_Points = i_uiNum_Points;
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
				lpItem->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != 0  && i_vdX_Values.size() == i_vdY_Values.size())
	{
		symbol_item * lpItem = (symbol_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_symbol)
		{
			uiRet = i_uiPlot_Data_ID;

			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
	
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			if (lpItem->m_lppData != nullptr)
				delete [] lpItem->m_lppData;
			lpItem->m_lppData = new eps_pair [i_vdX_Values.size()];
			lpItem->m_uiNum_Points = i_vdY_Values.size();
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
				lpItem->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vpValues.size() != 0)
	{
		symbol_item * lpItem = (symbol_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpItem != nullptr && lpItem->m_eType == type_symbol)
		{
			uiRet = i_uiPlot_Data_ID;

			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
	
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			if (lpItem->m_lppData != nullptr)
				delete [] lpItem->m_lppData;
			lpItem->m_lppData = new eps_pair [i_vpValues.size()];
			lpItem->m_uiNum_Points = i_vpValues.size();
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI] = i_vpValues[uiI];
			}
		}
	}
	return uiRet;
}
