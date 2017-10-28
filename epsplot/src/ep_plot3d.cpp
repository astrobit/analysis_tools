#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <eps_plot.h>
#include <sstream>
#include <iostream>
using namespace epsplot;

///////////////////////////////////////////////////////////////////////////////
//
// Line Plots
//
///////////////////////////////////////////////////////////////////////////////


unsigned int	data::Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, const double * i_lpdZ_Values, unsigned int i_uiNum_Points, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_uiNum_Points > 0 && i_lpdX_Values != nullptr && i_lpdY_Values != nullptr && i_lpdZ_Values != nullptr)
	{
		plot_3d_item * lpPlot_Data = new plot_3d_item;

		if (lpPlot_Data != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			if (uiRet != 0)
				std::cout << "Warning: 3d plot data not first in list; may overwrite other plot information";
			m_vcPlot_Item_List.push_back(lpPlot_Data);
			Modify_Plot_Data(uiRet,i_lpdX_Values, i_lpdY_Values, i_lpdZ_Values, i_uiNum_Points, i_eInterpolation_Scheme, i_uiX_Axis_ID, i_uiY_Axis_ID, i_uiZ_Axis_ID);

		}
	}
	return uiRet;
}

unsigned int	data::Set_Plot_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const std::vector<double> &i_vdZ_Values, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size() || i_vdX_Values.size() != i_vdZ_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Set_Plot_Data (3d) called using std::vector, but X, Y, and Z vectors differ in size.\n");
	}
	else if (i_vdX_Values.size() != 0)
	{
		plot_3d_item * lpPlot_Data = new plot_3d_item;

		if (lpPlot_Data != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			if (uiRet != 0)
				std::cout << "Warning: 3d plot data not first in list; may overwrite other plot information";
			m_vcPlot_Item_List.push_back(lpPlot_Data);
			Modify_Plot_Data(uiRet,i_vdX_Values, i_vdY_Values, i_vdZ_Values, i_eInterpolation_Scheme, i_uiX_Axis_ID, i_uiY_Axis_ID, i_uiZ_Axis_ID);

		}
	}
	return uiRet;
}
unsigned int	data::Set_Plot_Data(const std::vector<eps_triplet> &i_vpValues, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_vpValues.size() != 0)
	{
		plot_3d_item * lpPlot_Data = new plot_3d_item;

		if (lpPlot_Data != nullptr)
		{
			uiRet = m_vcPlot_Item_List.size();
			if (uiRet != 0)
				std::cout << "Warning: 3d plot data not first in list; may overwrite other plot information";
			m_vcPlot_Item_List.push_back(lpPlot_Data);
			Modify_Plot_Data(uiRet,i_vpValues, i_eInterpolation_Scheme, i_uiX_Axis_ID, i_uiY_Axis_ID, i_uiZ_Axis_ID);

		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, const double * i_lpdZ_Values, unsigned int i_uiNum_Points, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)
{
	unsigned int uiRet = -1;
	plot_3d_item * lpPlot_Data = (plot_3d_item *)Get_Plot_Item(i_uiPlot_Data_ID);
	if (lpPlot_Data != nullptr && lpPlot_Data->m_eType == type_3d)
	{
		uiRet = i_uiPlot_Data_ID;
		lpPlot_Data->m_uiNum_Points = i_uiNum_Points;
		lpPlot_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
		lpPlot_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
		lpPlot_Data->m_uiPlot_Axes_To_Use[2] = i_uiZ_Axis_ID;
		lpPlot_Data->m_eInterpolation_Scheme = i_eInterpolation_Scheme;
		if (lpPlot_Data->m_lppData)
			delete [] lpPlot_Data->m_lppData;
		lpPlot_Data->m_lppData = nullptr;
		if (i_uiNum_Points > 0)
		{
			lpPlot_Data->m_lppData = new eps_triplet [i_uiNum_Points];
			for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
			{
				lpPlot_Data->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
				lpPlot_Data->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
				lpPlot_Data->m_lppData[uiI].m_dZ = i_lpdZ_Values[uiI];
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const std::vector<double> &i_vdZ_Values, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size() || i_vdX_Values.size() != i_vdZ_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Modify_Plot_Data called using std::vector, but X, Y, and Z vectors differ in size.\n");
	}
	else
	{
		plot_3d_item * lpPlot_Data = (plot_3d_item *)Get_Plot_Item(i_uiPlot_Data_ID);
		if (lpPlot_Data != nullptr && lpPlot_Data->m_eType == type_3d)
		{
			uiRet = i_uiPlot_Data_ID;
			size_t uiNum_Points = lpPlot_Data->m_uiNum_Points = i_vdX_Values.size();
			lpPlot_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpPlot_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpPlot_Data->m_uiPlot_Axes_To_Use[2] = i_uiZ_Axis_ID;
			lpPlot_Data->m_eInterpolation_Scheme = i_eInterpolation_Scheme;
			if (lpPlot_Data->m_lppData)
				delete [] lpPlot_Data->m_lppData;
			lpPlot_Data->m_lppData = nullptr;

			if (uiNum_Points > 0)
			{
				lpPlot_Data->m_lppData = new eps_triplet [uiNum_Points];
				for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
				{
					lpPlot_Data->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
					lpPlot_Data->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
					lpPlot_Data->m_lppData[uiI].m_dZ = i_vdZ_Values[uiI];
				}
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_triplet> &i_vpValues, z_axis_iterpolation_scheme i_eInterpolation_Scheme, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, unsigned int i_uiZ_Axis_ID)

{
	unsigned int uiRet = -1;
	plot_3d_item * lpPlot_Data = (plot_3d_item *)Get_Plot_Item(i_uiPlot_Data_ID);
	if (lpPlot_Data != nullptr && lpPlot_Data->m_eType == type_3d)
	{
		size_t uiNum_Points = lpPlot_Data->m_uiNum_Points = i_vpValues.size();
		lpPlot_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
		lpPlot_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
		lpPlot_Data->m_uiPlot_Axes_To_Use[2] = i_uiZ_Axis_ID;
		lpPlot_Data->m_eInterpolation_Scheme = i_eInterpolation_Scheme;
		if (lpPlot_Data->m_lppData)
			delete [] lpPlot_Data->m_lppData;
		lpPlot_Data->m_lppData = nullptr;

		if (uiNum_Points > 0)
		{
			lpPlot_Data->m_lppData = new eps_triplet [uiNum_Points];
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				lpPlot_Data->m_lppData[uiI] = i_vpValues[uiI];
			}
		}
	}
	return uiRet;
}

