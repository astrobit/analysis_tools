#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <eps_plot.h>
#include <sstream>
#include <xstdlib.h>

using namespace epsplot;

class symbol_container
{
public:
	std::vector<eps_pair>	m_vSquare;
	std::vector<eps_pair>	m_vCircle;
	std::vector<eps_pair>	m_vTriangle_Up;
	std::vector<eps_pair>	m_vTriangle_Down;
	std::vector<eps_pair>	m_vTriangle_Left;
	std::vector<eps_pair>	m_vTriangle_Right;
	std::vector<eps_pair>	m_vDiamond;
	std::vector<eps_pair>	m_vTimes;
	std::vector<eps_pair>	m_vPlus;
	std::vector<eps_pair>	m_vDash;
	std::vector<eps_pair>	m_vAsterisk;
	std::vector<eps_pair>	m_vStar_4;
	std::vector<eps_pair>	m_vStar_5;
	std::vector<eps_pair>	m_vStar_6;
	std::vector<eps_pair>	m_vUser_Symbols[16];

	symbol_container(void)
	{
		m_vSquare.push_back(eps_pair(-0.5,-0.5));
		m_vSquare.push_back(eps_pair(-0.5, 0.5));
		m_vSquare.push_back(eps_pair( 0.5, 0.5));
		m_vSquare.push_back(eps_pair( 0.5,-0.5));
		double	dPi = acos(-1.0);
		double dAngle;
		for (dAngle = 0.0; dAngle < (2. * dPi); dAngle += ((2. * dPi) / 32.))
		{
			m_vCircle.push_back(eps_pair(0.5*sin(dAngle),0.5*cos(dAngle)));
		}

		m_vTriangle_Up.push_back(eps_pair(-0.5,-0.5));
		m_vTriangle_Up.push_back(eps_pair( 0.0, 0.5));
		m_vTriangle_Up.push_back(eps_pair( 0.5,-0.5));

		m_vTriangle_Down.push_back(eps_pair( 0.5, 0.5));
		m_vTriangle_Down.push_back(eps_pair( 0.0,-0.5));
		m_vTriangle_Down.push_back(eps_pair(-0.5, 0.5));

		m_vTriangle_Left.push_back(eps_pair( 0.5,-0.5));
		m_vTriangle_Left.push_back(eps_pair(-0.5, 0.0));
		m_vTriangle_Left.push_back(eps_pair( 0.5, 0.5));

		m_vTriangle_Right.push_back(eps_pair(-0.5, 0.5));
		m_vTriangle_Right.push_back(eps_pair( 0.5, 0.0));
		m_vTriangle_Right.push_back(eps_pair(-0.5,-0.5));

		m_vDiamond.push_back(eps_pair(-0.5, 0.0));
		m_vDiamond.push_back(eps_pair( 0.0, 0.5));
		m_vDiamond.push_back(eps_pair( 0.5, 0.0));
		m_vDiamond.push_back(eps_pair( 0.0,-0.5));

		m_vTimes.push_back(eps_pair(-0.5			, 0.5 - 1./12.	));
		m_vTimes.push_back(eps_pair(-0.5 + 1/12.	, 0.5			));
		m_vTimes.push_back(eps_pair(0.0			, 1/12.			));
		m_vTimes.push_back(eps_pair( 0.5 - 1/12.	, 0.5			));
		m_vTimes.push_back(eps_pair( 0.5			, 0.5 - 1./12.	));
		m_vTimes.push_back(eps_pair(1/12.		, 0.0			));
		m_vTimes.push_back(eps_pair( 0.5			,-0.5 + 1./12.	));
		m_vTimes.push_back(eps_pair( 0.5 - 1/12.	,-0.5			));
		m_vTimes.push_back(eps_pair(0.0			, -1/12.		));
		m_vTimes.push_back(eps_pair(-0.5 + 1/12.	,-0.5			));
		m_vTimes.push_back(eps_pair(-0.5			,-0.5 + 1./12.	));
		m_vTimes.push_back(eps_pair(-1/12.		, 0.0			));

		m_vDash.push_back(eps_pair(-0.5			, 1/12.			));
		m_vDash.push_back(eps_pair( 0.5			, 1/12.			));
		m_vDash.push_back(eps_pair( 0.5			,-1/12.			));
		m_vDash.push_back(eps_pair(-0.5			,-1/12.			));

		m_vPlus.push_back(eps_pair(-0.5			, 1/12.			));
		m_vPlus.push_back(eps_pair(-1/12.		, 1/12.			));
		m_vPlus.push_back(eps_pair(-1/12.		, 0.5			));
		m_vPlus.push_back(eps_pair( 1/12.		, 0.5			));
		m_vPlus.push_back(eps_pair( 1/12.		, 1/12.			));
		m_vPlus.push_back(eps_pair( 0.5			, 1/12.			));
		m_vPlus.push_back(eps_pair( 0.5			,-1/12.			));
		m_vPlus.push_back(eps_pair( 1/12.		,-1/12.			));
		m_vPlus.push_back(eps_pair( 1/12.		,-0.5			));
		m_vPlus.push_back(eps_pair(-1/12.		,-0.5			));
		m_vPlus.push_back(eps_pair(-1/12.		,-1/12.			));
		m_vPlus.push_back(eps_pair(-0.5			,-1/12.			));

		for (unsigned int uiArm = 0; uiArm < 8; uiArm++)
		{
			dAngle = uiArm * dPi * 0.25;
			m_vAsterisk.push_back(eps_pair(-0.5 * cos(dAngle) - 1/12. * sin(dAngle),-1/12.*cos(dAngle) + 0.5 * sin(dAngle)));
			m_vAsterisk.push_back(eps_pair(-0.5 * cos(dAngle) + 1/12. * sin(dAngle), 1/12.*cos(dAngle) + 0.5 * sin(dAngle)));
			m_vAsterisk.push_back(eps_pair( 0.0, 0.0));
		}

		for (unsigned int uiArm = 0; uiArm < 4; uiArm++)
		{
			dAngle = uiArm * dPi * 0.5;
			m_vStar_4.push_back(eps_pair(-0.5 * cos(dAngle), 0.5 * sin(dAngle)));
			m_vStar_4.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
		}

		for (unsigned int uiArm = 0; uiArm < 5; uiArm++)
		{
			dAngle = uiArm * dPi * 0.4;
			m_vStar_5.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
			m_vStar_5.push_back(eps_pair(0.5 * sin(dAngle), 0.5 * cos(dAngle)));
		}

		for (unsigned int uiArm = 0; uiArm < 6; uiArm++)
		{
			dAngle = uiArm * dPi / 3.;
			m_vStar_6.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
			m_vStar_6.push_back(eps_pair(0.5 * sin(dAngle), 0.5 * cos(dAngle)));
		}
	}
};

symbol_container	g_cSymbol_Containers;

//----------------------------------------------------------------------------
//
// data class members
//
//----------------------------------------------------------------------------


void data::Draw_Symbol(epsfile & io_cEPS, const double & i_dX, const double & i_dY, const symbol_parameters & i_cSymbol_Param)
{
	double dSize = i_cSymbol_Param.m_dSize;
	std::vector<eps_pair> *vSymbol;
	switch (i_cSymbol_Param.m_eType)
	{
	case SQUARE:
		vSymbol = &g_cSymbol_Containers.m_vSquare;
		break;
	case CIRCLE:
		vSymbol = &g_cSymbol_Containers.m_vCircle;
		break;
	case TRIANGLE_UP:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Up;
		break;
	case TRIANGLE_DOWN:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Down;
		break;
	case TRIANGLE_LEFT:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Left;
		break;
	case TRIANGLE_RIGHT:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Right;
		break;
	case DIAMOND:
		vSymbol = &g_cSymbol_Containers.m_vDiamond;
		break;
	case TIMES_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vTimes;
		break;
	case PLUS_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vPlus;
		break;
	case DASH_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vDash;
		break;
	case ASTERISK_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vAsterisk;
		break;
	case STAR4:
		vSymbol = &g_cSymbol_Containers.m_vStar_4;
		break;
	case STAR5:
		vSymbol = &g_cSymbol_Containers.m_vStar_5;
		break;
	case STAR6:
		vSymbol = &g_cSymbol_Containers.m_vStar_6;
		break;
	case SYMB_CUSTOM_1:
	case SYMB_CUSTOM_2:
	case SYMB_CUSTOM_3:
	case SYMB_CUSTOM_4:
	case SYMB_CUSTOM_5:
	case SYMB_CUSTOM_6:
	case SYMB_CUSTOM_7:
	case SYMB_CUSTOM_8:
	case SYMB_CUSTOM_9:
	case SYMB_CUSTOM_10:
	case SYMB_CUSTOM_11:
	case SYMB_CUSTOM_12:
	case SYMB_CUSTOM_13:
	case SYMB_CUSTOM_14:
	case SYMB_CUSTOM_15:
	case SYMB_CUSTOM_16:
		{
			unsigned int uiIdx = i_cSymbol_Param.m_eType - SYMB_CUSTOM_1;
			vSymbol = &(g_cSymbol_Containers.m_vUser_Symbols[uiIdx]);
		}
		break;
	default:
		vSymbol = nullptr;
		break;
	}
	if (vSymbol != nullptr)
	{
		if (!i_cSymbol_Param.m_bFilled && i_cSymbol_Param.m_dLine_Width != -1.0)
		{
			io_cEPS.Set_Line_Width(i_cSymbol_Param.m_dLine_Width);
		}
		for (unsigned int uiI = 0; uiI < vSymbol->size(); uiI++)
		{
			if (uiI == 0)
				io_cEPS.Move_To(vSymbol[0][uiI].m_dX * dSize + i_dX,vSymbol[0][uiI].m_dY * dSize + i_dY);
			else
				io_cEPS.Line_To(vSymbol[0][uiI].m_dX * dSize + i_dX,vSymbol[0][uiI].m_dY * dSize + i_dY);
		}
		if (i_cSymbol_Param.m_bFilled)
			io_cEPS.Fill();
		else
		{
			io_cEPS.Line_To(vSymbol[0][0].m_dX * dSize + i_dX,vSymbol[0][0].m_dY * dSize + i_dY);
			io_cEPS.Stroke();
		}
	}
}

void	data::Plot(const page_parameters & i_cGrid)
{
	color_triplet	cBlue(0.0,0.0,1.0);
	char	lpszText[32];
	double ** lpdMinima_WL = nullptr, ** lpdMinima_Flux = nullptr;
	double ** lpdEW = nullptr;
	double lpdDash_Style[2] = {2,2};
	double lpdLong_Short_Dash_Style[4] = {4,2,2,2};
	epsfile	cEPS("%.3f");// it seems this resolution is sufficient.  For higher resolution, use %.6f
	axis_metadata cX_Axis_Default;
	axis_metadata cY_Axis_Default;
	axis_metadata cZ_Axis_Default;
	double dPoints_To_Inches = 1.0 / 72.0;

	if (!m_szFilename.empty())
	{

		bool bHas_3d_plots = false;
		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end() && !bHas_3d_plots; cPlot_Item_Iter++)
		{
			bHas_3d_plots = ((*cPlot_Item_Iter)->m_eType == type_3d);
		}
		////////////////////////////////////////////////////////////////////////
		//
		// Determine the size of the plot title area
		//
		////////////////////////////////////////////////////////////////////////
		double dTitle_Margin = 0.0;
		if (i_cGrid.m_dTitle_Margin > 0.0 && i_cGrid.m_dHeight_Inches > 0.0)
			dTitle_Margin = i_cGrid.m_dTitle_Margin * i_cGrid.m_dHeight_Inches;
		else if (i_cGrid.m_dTitle_Margin_Inches > 0.0)
			dTitle_Margin = i_cGrid.m_dTitle_Margin_Inches;
		else if (!m_szTitle.empty())
			dTitle_Margin = m_dTitle_Size * 2.0 * dPoints_To_Inches;


		////////////////////////////////////////////////////////////////////////
		//
		// Determine the margins for the axes
		//
		////////////////////////////////////////////////////////////////////////
		size_t tSide = 0;
		double dX_Margin[2] = {0,0};
		if (i_cGrid.m_dX_Axis_Margin_Inches > 0.0)
		{
			dX_Margin[0] = i_cGrid.m_dX_Axis_Margin_Inches;
			if (m_cX_Axis_Parameters.size() > 1)
				dX_Margin[1] = i_cGrid.m_dX_Axis_Margin_Inches;
		}
		else
		{
			for (auto iterI = m_cX_Axis_Parameters.begin(); iterI != m_cX_Axis_Parameters.end(); iterI++)
			{
				double dSize = 0.0;
				if (!iterI->m_cParameters.m_sTitle.empty())
					dSize += iterI->m_cParameters.m_dTitle_Size * 2.0;
				double dLabel_Size = 0.0;
				if (iterI->m_cParameters.m_bLabel_Minor_Indices)
					dLabel_Size = iterI->m_cParameters.m_dMinor_Label_Size;
				if (iterI->m_cParameters.m_bLabel_Major_Indices && iterI->m_cParameters.m_dMajor_Label_Size > dLabel_Size)
					dLabel_Size = iterI->m_cParameters.m_dMajor_Label_Size;
				dLabel_Size *= 1.5;
				dSize += dLabel_Size;
				if (dX_Margin[tSide] < dSize)
					dX_Margin[tSide] = dSize;
				tSide++;
				tSide &= 1;
			}
			dX_Margin[0] *= dPoints_To_Inches;
			dX_Margin[1] *= dPoints_To_Inches;
		}

		// determine size of Y margins
		tSide = 0;
		double dY_Margin[2] = {0,0};
		if (i_cGrid.m_dY_Axis_Margin_Inches > 0.0)
		{
			dY_Margin[0] = i_cGrid.m_dY_Axis_Margin_Inches;
			if (m_cY_Axis_Parameters.size() > 1)
				dY_Margin[1] = i_cGrid.m_dY_Axis_Margin_Inches;
		}
		else
		{
			for (auto iterI = m_cY_Axis_Parameters.begin(); iterI != m_cY_Axis_Parameters.end(); iterI++)
			{
				double dSize = 0.0;
				if (!iterI->m_cParameters.m_sTitle.empty())
					dSize += iterI->m_cParameters.m_dTitle_Size * 2.0;
				if (dY_Margin[tSide] < dSize)
					dY_Margin[tSide] = dSize;
				tSide++;
				tSide &= 1;
			}
			dY_Margin[0] *= dPoints_To_Inches;
			dY_Margin[1] *= dPoints_To_Inches;
		}

		// determine size of Z margins
		double dZ_Margin = 0.0;
		if (bHas_3d_plots)
		{
			if (i_cGrid.m_dZ_Axis_Margin_Inches > 0.0)
				dZ_Margin = i_cGrid.m_dZ_Axis_Margin_Inches;
			else
			{
				for (auto iterI = m_cZ_Axis_Parameters.begin(); iterI != m_cZ_Axis_Parameters.end(); iterI++)
				{
					double dSize = iterI->m_cParameters.m_dBar_Width * 3.0;
					if (dSize > dZ_Margin)
						dZ_Margin = dSize;
				}
				dZ_Margin *= dPoints_To_Inches;
			}
		}
	
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Reset_Limits();
		}
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Reset_Limits();
		}
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cZ_Axis_Parameters.begin(); cAxis_Iter != m_cZ_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Reset_Limits();
		}

		////////////////////////////////////////////////////////////////////////
		//
		// Determine the limits for each axis, if the user hasn't explicitly 
		// specified them, by going through the data to the plotted and 
		// identifying minima and maxima
		//
		////////////////////////////////////////////////////////////////////////
		// x-axis
		////////////////////////////////////////////////////////////////////////
		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			line_item * lpcLine = nullptr;
			plot_3d_item * lpc3D = nullptr;
			symbol_item * lpcSymbol = nullptr;
			rectangle_item * lpcRectangle = nullptr;
			text_item * lpcText = nullptr;

//			double	dXminLcl = DBL_MAX, dXmaxLcl = -DBL_MAX;
			axis_metadata * lpX_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];

			switch (lpCurr->m_eType)
			{
			case type_3d:
				lpc3D = (plot_3d_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpc3D->m_uiNum_Points; uiJ++)
				{
					lpX_Axis->Adjust_Limits(lpc3D->m_lppData[uiJ].m_dX);
				}
				break;
			case type_line:
				lpcLine = (line_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
				{
					lpX_Axis->Adjust_Limits(lpcLine->m_lppData[uiJ].m_dX);
				}
				break;
			case type_symbol:
				lpcSymbol = (symbol_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
				{
					lpX_Axis->Adjust_Limits(lpcSymbol->m_lppData[uiJ].m_dX);
				}
				break;
			case type_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;
				lpX_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
				lpX_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
				break;
			case type_text:
//				lpcText = (text_item *) lpCurr;
				//Do not adjust axis limits to accomodate text.
				break;
			}
		}
		

		////////////////////////////////////////////////////////////////////////
		// y-axis and z-axis
		////////////////////////////////////////////////////////////////////////
		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			line_item * lpcLine = nullptr;
			symbol_item * lpcSymbol = nullptr;
			rectangle_item * lpcRectangle = nullptr;
			text_item * lpcText = nullptr;
			plot_3d_item * lpc3D = nullptr;

//			double	dYminLcl = DBL_MAX, dYmaxLcl = -DBL_MAX;
			axis_metadata * lpX_Axis;
			axis_metadata * lpY_Axis;
			axis_metadata * lpZ_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];
			if (m_cY_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[1] >= m_cY_Axis_Parameters.size())
				lpY_Axis = &cY_Axis_Default;
			else
				lpY_Axis = &m_cY_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[1]];
			if (m_cZ_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[2] >= m_cZ_Axis_Parameters.size())
				lpZ_Axis = &cZ_Axis_Default;
			else
				lpZ_Axis = &m_cZ_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[2]];

			switch (lpCurr->m_eType)
			{
			case type_3d:
				lpc3D = (plot_3d_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpc3D->m_uiNum_Points; uiJ++)
				{
					if (lpc3D->m_lppData[uiJ].m_dX >= lpX_Axis->m_dLower_Limit && lpc3D->m_lppData[uiJ].m_dX <= lpX_Axis->m_dUpper_Limit)
					{
						lpY_Axis->Adjust_Limits(lpc3D->m_lppData[uiJ].m_dY);
						lpZ_Axis->Adjust_Limits(lpc3D->m_lppData[uiJ].m_dZ);
					}
				}
				break;
			case type_line:
				lpcLine = (line_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
				{
					if (lpcLine->m_lppData[uiJ].m_dX >= lpX_Axis->m_dLower_Limit && lpcLine->m_lppData[uiJ].m_dX <= lpX_Axis->m_dUpper_Limit)
						lpY_Axis->Adjust_Limits(lpcLine->m_lppData[uiJ].m_dY);
				}
				break;
			case type_symbol:
				lpcSymbol = (symbol_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
				{
					if (lpcSymbol->m_lppData[uiJ].m_dX >= lpX_Axis->m_dLower_Limit && lpcSymbol->m_lppData[uiJ].m_dX <= lpX_Axis->m_dUpper_Limit)
						lpY_Axis->Adjust_Limits(lpcSymbol->m_lppData[uiJ].m_dY);
				}
				break;
			case type_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;
					if ((lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min >= lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min <= lpX_Axis->m_dUpper_Limit) ||
(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max >= lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max <= lpX_Axis->m_dUpper_Limit) ||
(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max < lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max > lpX_Axis->m_dUpper_Limit))
				{
					lpY_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					lpY_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
				}
				break;
			case type_text:
//				lpcText = (text_item *) lpCurr;
				//Do not adjust axis limits to accomodate text.
				break;
			}
		}


		////////////////////////////////////////////////////////////////////////
		// finalize x-axis
		////////////////////////////////////////////////////////////////////////
		cX_Axis_Default.Finalize_Limit();
		double dX_Range = 0.0;
		double dY_Range = 0.0;
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Finalize_Limit();

			double dRange = std::fabs(cAxis_Iter->m_dRange);
			if (cAxis_Iter->m_cParameters.m_bLog)
				dRange = std::log10(dRange);
			if (dRange > dX_Range)
				dX_Range = dRange;
			
		}


		////////////////////////////////////////////////////////////////////////
		// finalize y-axis
		////////////////////////////////////////////////////////////////////////
		cY_Axis_Default.Finalize_Limit();

		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Finalize_Limit();
			double dRange = std::fabs(cAxis_Iter->m_dRange);
			if (cAxis_Iter->m_cParameters.m_bLog)
				dRange = std::log10(dRange);
			if (dRange > dY_Range)
				dY_Range = dRange;
		}
		//printf("here\n");
		//////////////////////////////// determine length of the largest y-axis label text ////////////////////////////////////
		unsigned int uiI = 0;
		double dMaxLenY[2] = {0.0,0.0};
		if (m_cY_Axis_Parameters.size() == 0 && cY_Axis_Default.m_dRange != 0.0)
		{
			double dLogRange = log10(fabs(cY_Axis_Default.m_dRange));
			double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
			double	dMinor_Ticks = dMajor_Ticks * 0.1;
			unsigned int uiNum_Ticks = cY_Axis_Default.m_dRange / dMajor_Ticks;
			char lpszFormat[8];
			if (uiNum_Ticks > 10)
			{
				dMajor_Ticks *= 5.0; // e.g. units of 500
				dMinor_Ticks *= 10.0; // e.g. units of 100
			}
			else if (uiNum_Ticks > 5)
			{
				dMajor_Ticks *= 2.5; // e.g. units of 500
				dMinor_Ticks *= 5.0; // e.g. units of 100
			}
			else
			{
				dMajor_Ticks *= 1.0; // e.g. units of 500
				dMinor_Ticks *= 2.5; // e.g. units of 100
			}
			double	dLog_Major = floor(log10(dMajor_Ticks));
			bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
			if (dLog_Major < 0.0)
				sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
			else if (dLog_Major > 3)
				strcpy(lpszFormat,"%.2e");
			else
				strcpy(lpszFormat,"%.0f");
			double	dStart = XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			double	dEnd = XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
			if (cY_Axis_Default.m_cParameters.m_bInvert)
			{
				dStart =  XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
				dEnd =  XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			}
			for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
			{
				if (dVal >= cY_Axis_Default.m_dLower_Limit && dVal <= cY_Axis_Default.m_dUpper_Limit) // just to make sure
				{
					bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
					double dSize;
					if (bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Major_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMajor_Label_Size;
					}
					else if (!bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Minor_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMinor_Label_Size;
					}
					else
						dSize = 0.0;

					if (dSize > 0.0)
					{
						char lpszValue[16];
						if (bScientific_Notation)
						{
							double dLog10 = log10(dVal);
							double dPower = floor(dLog10);
							double	dMantissa = dVal * pow(10.0,-dPower);
							if (dVal == 0.0)
							{
								sprintf(lpszValue,"%.1f",dVal);
							}
							else if (dMantissa != 1.0 && dPower != 0.0)
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
								else
									sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
							}
							else if (dMantissa == 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"10^{%.0f}",dPower);
							}
							else
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2f",dMantissa);
								else
									sprintf(lpszValue,"%.1f",dMantissa);
							}
						}
						else
							sprintf(lpszValue,lpszFormat,dVal);
						
						double dLen = (strlen(lpszValue) + 0.5) * dSize;
						if (dLen > dMaxLenY[(uiI % 2)])
							dMaxLenY[(uiI % 2)] = dLen;
					}
				}
			}
		}
		else
		{
			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
			{
				// round minimum axis value to nearest major tick
				if (!(*cAxis_Iter).m_cParameters.m_bLog)
				{
					double dLogRange = log10(fabs((*cAxis_Iter).m_dRange));
					double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
					double	dMinor_Ticks = dMajor_Ticks * 0.1;
					unsigned int uiNum_Ticks = (*cAxis_Iter).m_dRange / dMajor_Ticks;
					char lpszFormat[8];
					if (uiNum_Ticks > 10)
					{
						dMajor_Ticks *= 5.0; // e.g. units of 500
						dMinor_Ticks *= 10.0; // e.g. units of 100
					}
					else if (uiNum_Ticks > 5)
					{
						dMajor_Ticks *= 2.5; // e.g. units of 500
						dMinor_Ticks *= 5.0; // e.g. units of 100
					}
					else
					{
						dMajor_Ticks *= 1.0; // e.g. units of 500
						dMinor_Ticks *= 2.5; // e.g. units of 100
					}
					double	dLog_Major = floor(log10(dMajor_Ticks));
					bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
					if (dLog_Major < 0.0)
						sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
					else if (dLog_Major > 3)
						strcpy(lpszFormat,"%.2e");
					else
						strcpy(lpszFormat,"%.0f");
					double	dStart = XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					double	dEnd = XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
					if ((*cAxis_Iter).m_cParameters.m_bInvert)
					{
						dStart =  XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
						dEnd =  XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					}

					for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
					{
						if (dVal >= (*cAxis_Iter).m_dLower_Limit && dVal <= (*cAxis_Iter).m_dUpper_Limit) // just to make sure
						{
							bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
							double dSize;
							if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
							}
							else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
							}
							else
								dSize = 0.0;

							if (dSize > 0.0)
							{
								char lpszValue[16];
								if (bScientific_Notation)
								{
									//printf("ys\n");
									double dLog10 = log10(dVal);
									double dPower = floor(dLog10);
									double	dMantissa = dVal * pow(10.0,-dPower);
									if (dVal == 0.0)
									{
										sprintf(lpszValue,"%.1f",dVal);
									}
									else if (dMantissa != 1.0 && dPower != 0.0)
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
										else
											sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
									}
									else if (dMantissa == 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"10^{%.0f}",dPower);
									}
									else
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2f",dMantissa);
										else
											sprintf(lpszValue,"%.1f",dMantissa);
									}
								}
								else
								{
									//printf("yns %s\n",lpszFormat);
									sprintf(lpszValue,lpszFormat,dVal);
								}
					
								double dLen = (strlen(lpszValue) + 0.5) * dSize;
								if (dLen > dMaxLenY[(uiI % 2)])
									dMaxLenY[(uiI % 2)] = dLen;
							}
						}
					}

				}
				else
				{
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					//double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
					double	dRange_Lower = (*cAxis_Iter).m_dStart;
					double	dRange_Upper = (*cAxis_Iter).m_dEnd;
					if (!std::isnan(dRange_Lower) && !std::isnan(dRange_Upper))
					{
						double dLower = floor(dRange_Lower) - 1.0;
						double dUpper = floor(dRange_Upper) + 1.0;
						for (double dPower = dLower; dPower <= dUpper; dPower += 1.0)
						{
							double dMult = pow(10.0,dPower);
							for (unsigned int uiScalar = 1; uiScalar < 10; uiScalar ++)
							{
								double dVal = dMult * uiScalar;
								bool bMajor_Test = (uiScalar == 1);
								if ((*cAxis_Iter).m_dLower_Limit <= dVal  && dVal <= (*cAxis_Iter).m_dUpper_Limit)
								{
									double dSize;
									epsplot::COLOR eColor;
									if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
									}
									else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
									}
									else
										dSize = 0.0;
									if (dSize > 0.0)
									{
										char lpszValue[16];
										double dLog10 = log10(dVal);
										double dPower = floor(dLog10);
										double	dMantissa = dVal * pow(10.0,-dPower);
										if (dVal == 0.0)
										{
											sprintf(lpszValue,"%.1f",dVal);
										}
										else if (dMantissa != 1.0 && dPower != 0.0)
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
											else
												sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
										}
										else if (dMantissa == 1.0 && dPower != 0.0)
										{
											sprintf(lpszValue,"10^{%.0f}",dPower);
										}
										else
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2f",dMantissa);
											else
												sprintf(lpszValue,"%.1f",dMantissa);
										}
					
										double dLen = (strlen(lpszValue) + 0.5) * dSize;
										if (dLen > dMaxLenY[(uiI % 2)])
											dMaxLenY[(uiI % 2)] = dLen;
									}
								}
							}
						}
					}
					else
						fprintf(stderr,"epsplot: y-axis is requested with log values, but range includes a negative number or zero (%f, %f).\n",dRange_Lower,dRange_Upper);
				}
				uiI++; // indicates axis at top or bottom of graph
			}
		}
		//printf("here\n");

		////////////////////////////////////////////////////////////////////////
		// finalize z-axis
		////////////////////////////////////////////////////////////////////////
		cZ_Axis_Default.Finalize_Limit();

		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cZ_Axis_Parameters.begin(); cAxis_Iter != m_cZ_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Finalize_Limit();
		}

		////////////////////////////////////////////////////////////////////////
		// if the user left the plot size open ended, compute the size of the
		//  graph space
		////////////////////////////////////////////////////////////////////////
		//printf("%.1f %.1f\n",dMaxLenY[0],dMaxLenY[1]);
		dMaxLenY[0] *= 5.0 / 7.0 * dPoints_To_Inches; // convert to inches and estimate relative width of text (5:7 is typical width:height ratio of text)
		dMaxLenY[1] *= 5.0 / 7.0 * dPoints_To_Inches;
		dY_Margin[0] += dMaxLenY[0];
		dY_Margin[1] += dMaxLenY[1];
		
		double dHoriz_Margins_Total = dY_Margin[0] + dY_Margin[1] + dZ_Margin + 2.0 * i_cGrid.m_dSide_Unprintable_Margins_Inches;
		double dVert_Margins_Total = dX_Margin[0] + dX_Margin[1] + dTitle_Margin + 2.0 * i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches;

		double dGraph_Space_X = 0.0;
		double dGraph_Space_Y = 0.0;

		if (i_cGrid.m_dWidth_Inches > 0.0)
		{
			dGraph_Space_X = i_cGrid.m_dWidth_Inches - dHoriz_Margins_Total;
			if (i_cGrid.m_dHeight_Inches > 0.0)
				dGraph_Space_Y = i_cGrid.m_dHeight_Inches - dVert_Margins_Total;
			else
				dGraph_Space_Y = dGraph_Space_X * (dY_Range / dX_Range);
		}
		else
		{
			dGraph_Space_Y = i_cGrid.m_dHeight_Inches - dVert_Margins_Total;
			if (i_cGrid.m_dWidth_Inches > 0.0)
				dGraph_Space_X = i_cGrid.m_dWidth_Inches - dHoriz_Margins_Total;
			else
				dGraph_Space_X = dGraph_Space_Y * (dX_Range / dY_Range);
		}
		double dWidth_Inches = dGraph_Space_X + dHoriz_Margins_Total;
		double dHeight_Inches = dVert_Margins_Total + dGraph_Space_Y;
		// convert to points
		dGraph_Space_X *= 72.0;
		dGraph_Space_Y *= 72.0;


		double	dGraph_Offset_X = (dY_Margin[0] + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0;
		double	dGraph_Offset_Y = (dX_Margin[0] + i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0;
		// Don't do multi plot for now.. @@TODO
		//double	dSingle_Plot_Space_X = dGraph_Space_X / i_cGrid.m_uiNum_Columns;
		//double	dSingle_Plot_Space_Y = dGraph_Space_Y / i_cGrid.m_uiNum_Rows;

		cEPS.Open_File(m_szFilename.c_str(), m_szTitle.c_str(), dWidth_Inches, dHeight_Inches, i_cGrid.m_bLandscape);

		////////////////////////////////////////////////////////////////////////
		// set the scaling factor for the axes
		////////////////////////////////////////////////////////////////////////
		cX_Axis_Default.Set_Scale(dGraph_Space_X);
		cY_Axis_Default.Set_Scale(dGraph_Space_Y);
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Set_Scale(dGraph_Space_X);
		}
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Set_Scale(dGraph_Space_Y);
		}

		////////////////////////////////////////////////////////////////////////
		// draw the boundaries of the graph
		////////////////////////////////////////////////////////////////////////


		COLOR eCurr_Color = BLACK;
		STIPPLE eCurr_Stipple = SOLID;
		double	dCurr_Line_Width = 1.0;
		// all scales and ranges worked out.. time to plot the data
		cEPS.State_Push();
		cEPS.Translate(dGraph_Offset_X,dGraph_Offset_Y);
		cEPS.Set_Line_Width(2.0);
		dCurr_Line_Width = 2.0;

		if (m_cX_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(-1.0,0.0);
		cEPS.Line_To(dGraph_Space_X + 1.0,0.0);
		cEPS.Stroke();

		if (m_cX_Axis_Parameters.size() > 1)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[1].m_cParameters.m_eColor));
		else if (m_cX_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(dGraph_Space_X + 1.0,dGraph_Space_Y);
		cEPS.Line_To(-1.0,dGraph_Space_Y);
		cEPS.Stroke();

		if (m_cY_Axis_Parameters.size() > 1)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[1].m_cParameters.m_eColor));
		else if (m_cY_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(dGraph_Space_X,0.0);
		cEPS.Line_To(dGraph_Space_X,dGraph_Space_Y);
		cEPS.Stroke();


		if (m_cY_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(0.0,dGraph_Space_Y);
		cEPS.Line_To(0.0,0.0);
		cEPS.Stroke();


		cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
		cEPS.Rect_Clip(0.0,0.0,dGraph_Space_X - 2.0,dGraph_Space_Y - 2.0);

		uiI = 0;

		if (m_vcPlot_Item_List.size() == 0)
		{
			fprintf(stderr,"No data to graph\n");
			fflush(stderr);
		}
		////////////////////////////////////////////////////////////////////////
		// 
		// Loop through all data and plot it
		// 
		////////////////////////////////////////////////////////////////////////
		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			//printf("plot item type %i\n",lpCurr->m_eType);
			//printf("Line Axes %i %i\n",lpCurr->m_uiPlot_Axes_To_Use[0],lpCurr->m_uiPlot_Axes_To_Use[1]);
			uiI++;
			line_item * lpcLine = nullptr;
			symbol_item * lpcSymbol = nullptr;
			rectangle_item * lpcRectangle = nullptr;
			text_item * lpcText = nullptr;
			plot_3d_item * lpcPlot_3d = nullptr;
			errorbar_item * lpcErrorbar = nullptr;
			axis_metadata * lpX_Axis;
			axis_metadata * lpY_Axis;
			axis_metadata * lpZ_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];
			if (m_cY_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[1] >= m_cY_Axis_Parameters.size())
				lpY_Axis = &cY_Axis_Default;
			else
				lpY_Axis = &m_cY_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[1]];
			if (m_cZ_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[2] >= m_cZ_Axis_Parameters.size())
				lpZ_Axis = &cZ_Axis_Default;
			else
				lpZ_Axis = &m_cZ_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[2]];

			switch (lpCurr->m_eType)
			{
			case type_3d:
				{
					lpcPlot_3d = (plot_3d_item *) lpCurr;
					size_t tRows = 0, tCols = 0;
					std::vector<color_triplet> vctColor_Data;
					//for (size_t tI = 0; tI < dGraph_Space_Y; tI++)
					//{
					//		double dY = lpY_Axis->Reverse_Scale(tI);
					//		printf(",%f",dY);
					//}
					//printf("\n");
					//printf("%f %f\n",lpZ_Axis->m_dStart,lpZ_Axis->m_dEnd);
					for (size_t tJ = 0; tJ < dGraph_Space_Y; tJ++)
					{
						tCols++;
						double dY = lpY_Axis->Reverse_Scale(tJ + 0.5);
						//printf("%f",dX);
						for (size_t tI = 0; tI < dGraph_Space_X + 1; tI++)
						{
							tRows++;
							double dX = lpX_Axis->Reverse_Scale(tI + 0.5);
							color_triplet ctCurr_Color;
							double dNearest_Range = DBL_MAX;
							double dSum_Weights[16] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
							double dSum_Weight_Val[16] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
							bool bExact = false;
							eps_triplet etNearest;
							for (size_t tK = 0; tK < lpcPlot_3d->m_uiNum_Points; tK++)
							{
								double dDel_X = dX - lpcPlot_3d->m_lppData[tK].m_dX;
								double dDel_Y = dY - lpcPlot_3d->m_lppData[tK].m_dY;
								double dRange = dDel_X * dDel_X + dDel_Y * dDel_Y;
								if (dRange == 0.0)
								{
									bExact = true;
									for (size_t tL = 0; tL < 16; tL++)
									{
										dSum_Weight_Val[tL] = lpcPlot_3d->m_lppData[tK].m_dZ;
										dSum_Weights[tL] = 1.0;
									}
								}
								if (!bExact && lpcPlot_3d->m_eInterpolation_Scheme != nearest) // don't waste time with this for a nearest neighbor search
								{
									double dInv_Sqrt_Range = 1.0 / sqrt(dRange);
									double dWeight = 1.0;

									for (size_t tL = 0; tL < 16; tL++)
									{
										dWeight *= dInv_Sqrt_Range;
										dSum_Weights[tL] += dWeight;
										dSum_Weight_Val[tL] += dWeight * lpcPlot_3d->m_lppData[tK].m_dZ;
									}
								}
								if (dRange < dNearest_Range)
								{	
									dNearest_Range = dRange;
									etNearest = lpcPlot_3d->m_lppData[tK];
								}
							}
							//printf(", %f",etNearest.m_dZ);
							switch (lpcPlot_3d->m_eInterpolation_Scheme)
							{
							case nearest:
								ctCurr_Color = lpZ_Axis->Get_Color(etNearest.m_dZ);
								//printf("%f %f %f\n",dX,dY,etNearest.m_dZ);
								break;
							case inverse_distance_weight_1:
							case inverse_distance_weight_2:
							case inverse_distance_weight_3:
							case inverse_distance_weight_4:
							case inverse_distance_weight_5:
							case inverse_distance_weight_6:
							case inverse_distance_weight_7:
							case inverse_distance_weight_8:
							case inverse_distance_weight_9:
							case inverse_distance_weight_10:
							case inverse_distance_weight_11:
							case inverse_distance_weight_12:
							case inverse_distance_weight_13:
							case inverse_distance_weight_14:
							case inverse_distance_weight_15:
							case inverse_distance_weight_16:
								{
									size_t tIdx = (lpcPlot_3d->m_eInterpolation_Scheme - inverse_distance_weight_1);
									ctCurr_Color = lpZ_Axis->Get_Color(dSum_Weight_Val[tIdx] / dSum_Weights[tIdx]);
								}
								break;
							}
							vctColor_Data.push_back(ctCurr_Color);

						}
					//printf("\n");

					}
					tRows /= tCols;
					//printf("%i %i\n",tRows,tCols);
					cEPS.ColorImage(dGraph_Offset_X, dGraph_Offset_Y, dGraph_Space_X, dGraph_Space_Y, 8, vctColor_Data, tRows);
					
				}
				break;
			case type_line:
				lpcLine = (line_item *) lpCurr;
				//printf("Num points in line %i\n",lpcLine->m_uiNum_Points);
				//printf("Line Axes %i %i\n",lpcLine->m_uiPlot_Axes_To_Use[0],lpcLine->m_uiPlot_Axes_To_Use[1]);
				if (lpcLine->m_uiNum_Points >= 2)
				{
					if (lpcLine->m_cPlot_Line_Info.m_eColor != eCurr_Color)
					{
						eCurr_Color = lpcLine->m_cPlot_Line_Info.m_eColor;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					if (lpcLine->m_cPlot_Line_Info.m_eStipple != eCurr_Stipple)
					{
						eCurr_Stipple = lpcLine->m_cPlot_Line_Info.m_eStipple;
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
					if (lpcLine->m_cPlot_Line_Info.m_dWidth != dCurr_Line_Width)
					{
						dCurr_Line_Width = lpcLine->m_cPlot_Line_Info.m_dWidth;
						cEPS.Set_Line_Width(dCurr_Line_Width);
					}

					{
						char lpszText[32];
						sprintf(lpszText,"Line %i\n",uiI);
						cEPS.Comment(lpszText);
						bool bFirst = true;
						for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
						{
							double dX = lpX_Axis->Scale(lpcLine->m_lppData[uiJ].m_dX);
							double dY = lpY_Axis->Scale(lpcLine->m_lppData[uiJ].m_dY);

							if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
							{
								if (bFirst)
									cEPS.Move_To(dX,dY);
								else
									cEPS.Line_To(dX,dY);
								bFirst = false;
							}
						}
						if (!bFirst) // don't draw if at least one valid point didn't show up
							cEPS.Stroke();
					}
				}
				break;
			case type_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;

				if (lpcRectangle->m_bArea_Fill)
				{
					char lpszText[32];
					sprintf(lpszText,"Rect Fill %i\n",uiI);
					cEPS.Comment(lpszText);

					if (lpcRectangle->m_ePlot_Area_Fill_Color != eCurr_Color)
					{
						eCurr_Color = lpcRectangle->m_ePlot_Area_Fill_Color;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					double	dX, dY;

					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Move_To(dX,dY);
					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					cEPS.Fill();
				}
				if (lpcRectangle->m_bDraw_Border)
				{
					sprintf(lpszText,"Rect Border %i\n",uiI);
					cEPS.Comment(lpszText);

					if (lpcRectangle->m_cPlot_Border_Info.m_eColor != eCurr_Color)
					{
						eCurr_Color = lpcRectangle->m_cPlot_Border_Info.m_eColor;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					if (lpcRectangle->m_cPlot_Border_Info.m_eStipple != eCurr_Stipple)
					{
						eCurr_Stipple = lpcRectangle->m_cPlot_Border_Info.m_eStipple;
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
					if (lpcRectangle->m_cPlot_Border_Info.m_dWidth != dCurr_Line_Width)
					{
						dCurr_Line_Width = lpcRectangle->m_cPlot_Border_Info.m_dWidth;
						cEPS.Set_Line_Width(dCurr_Line_Width);
					}


					double dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					double dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Move_To(dX,dY);
					dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					cEPS.Stroke();
				}
				break;
			case type_text:
				lpcText = (text_item *) lpCurr;
				if (lpcText->m_cLine_Parameters.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcText->m_cLine_Parameters.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				if (lpcText->m_cLine_Parameters.m_dWidth != dCurr_Line_Width)
				{
					dCurr_Line_Width = lpcText->m_cLine_Parameters.m_dWidth;
					cEPS.Set_Line_Width(dCurr_Line_Width);
				}

				{
					char lpszText[32];
					sprintf(lpszText,"Text %i (%s)\n",uiI,lpcText->Get_Text());
					cEPS.Comment(lpszText);
					double dX = lpX_Axis->Scale(lpcText->m_dX);
					double dY = lpY_Axis->Scale(lpcText->m_dY);

					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
					{
						cEPS.Text(lpcText->m_cText_Parameters.m_eFont, lpcText->m_cText_Parameters.m_bItalic, lpcText->m_cText_Parameters.m_bBold, lpcText->m_cText_Parameters.m_iFont_Size, lpcText->m_cText_Parameters.m_eHorizontal_Justification, lpcText->m_cText_Parameters.m_eVertical_Justification, Get_Color(eCurr_Color),dX, dY, lpcText->Get_Text(), lpcText->m_cText_Parameters.m_dRotation, lpcText->m_cLine_Parameters.m_dWidth);
					}
				}
				break;
			case type_symbol:
				lpcSymbol = (symbol_item *) lpCurr;
				if (lpcSymbol->m_cPlot_Symbol_Info.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcSymbol->m_cPlot_Symbol_Info.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				{
					char lpszText[32];
					sprintf(lpszText,"Symbol %i\n",uiI);
					cEPS.Comment(lpszText);
					if (eCurr_Stipple != SOLID)
					{
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(SOLID,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}

					for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
					{
						double dX = lpX_Axis->Scale(lpcSymbol->m_lppData[uiJ].m_dX);
						double dY = lpY_Axis->Scale(lpcSymbol->m_lppData[uiJ].m_dY);

						if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						{
							Draw_Symbol(cEPS,dX,dY,lpcSymbol->m_cPlot_Symbol_Info);
						}
					}
					if (eCurr_Stipple != SOLID)
					{
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
				}

				break;
			case type_errorbar:
				lpcErrorbar = (errorbar_item *) lpCurr;
				if (lpcErrorbar->m_cPlot_Line_Info.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcErrorbar->m_cPlot_Line_Info.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				if (lpcErrorbar->m_cPlot_Line_Info.m_dWidth != dCurr_Line_Width)
				{
					dCurr_Line_Width = lpcErrorbar->m_cPlot_Line_Info.m_dWidth;
					cEPS.Set_Line_Width(dCurr_Line_Width);
				}
				{
					
					char lpszText[32];
					sprintf(lpszText,"Errorbar %i (%i)\n",uiI,lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot);
					cEPS.Comment(lpszText);
					eps_pair * lpPair_List = nullptr;
					unsigned int uiNum_Points = 0;
					if (m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot]->m_eType ==  type_symbol)
					{
						lpcSymbol = (symbol_item *)m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot];
						uiNum_Points = lpcSymbol->m_uiNum_Points;
						lpPair_List = lpcSymbol->m_lppData;
					}
					else if (m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot]->m_eType ==  type_line)
					{
						lpcLine = (line_item *)m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot];
						uiNum_Points = lpcLine->m_uiNum_Points;
						lpPair_List = lpcLine->m_lppData;
					}
					if (lpPair_List != nullptr && uiNum_Points > 0)
					{
						for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
						{
							eps_pair p1(lpX_Axis->Scale(lpPair_List[uiJ].m_dX),lpY_Axis->Scale(lpPair_List[uiJ].m_dY));
							eps_pair p2,p3,p4;
							switch (lpcErrorbar->m_cErrorbar_Info.m_eDirection)
							{
							case ERRORBAR_X_LEFT:
								p2.m_dX = lpX_Axis->Scale(lpPair_List[uiJ].m_dX - lpcErrorbar->m_lppData[uiJ]);
								p2.m_dY = p1.m_dY;

								break;
							case ERRORBAR_X_RIGHT:
								p2.m_dX = lpX_Axis->Scale(lpPair_List[uiJ].m_dX + lpcErrorbar->m_lppData[uiJ]);
								p2.m_dY = p1.m_dY;

								break;
							case ERRORBAR_Y_UPPER:
								p2.m_dX = p1.m_dX;
								p2.m_dY = lpY_Axis->Scale(lpPair_List[uiJ].m_dY + lpcErrorbar->m_lppData[uiJ]);

								break;
							case ERRORBAR_Y_LOWER:
								p2.m_dX = p1.m_dX;
								p2.m_dY = lpY_Axis->Scale(lpPair_List[uiJ].m_dY - lpcErrorbar->m_lppData[uiJ]);

								break;
							}


							if (!std::isnan(p1.m_dX) && !std::isinf(p1.m_dX) && !std::isnan(p1.m_dY) && !std::isinf(p1.m_dY) &&
								!std::isnan(p2.m_dX) && !std::isinf(p2.m_dX) && !std::isnan(p2.m_dY) && !std::isinf(p2.m_dY) &&
								!std::isnan(p3.m_dX) && !std::isinf(p3.m_dX) && !std::isnan(p3.m_dY) && !std::isinf(p3.m_dY) &&
								!std::isnan(p4.m_dX) && !std::isinf(p4.m_dX) && !std::isnan(p4.m_dY) && !std::isinf(p4.m_dY))
							{
								if (lpcErrorbar->m_cPlot_Line_Info.m_eStipple != eCurr_Stipple)
								{
									eCurr_Stipple = lpcErrorbar->m_cPlot_Line_Info.m_eStipple;
									unsigned int uiStipple_Size;
									const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
									cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
								}
								cEPS.Move_To(p1.m_dX,p1.m_dY);
								cEPS.Line_To(p2.m_dX,p2.m_dY);
								cEPS.Stroke();
								// make the end-lines solid
								if (eCurr_Stipple != epsplot::SOLID)
								{
									eCurr_Stipple = epsplot::SOLID;
									unsigned int uiStipple_Size;
									const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
									cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
								}
								switch (lpcErrorbar->m_cErrorbar_Info.m_eDirection)
								{
								case ERRORBAR_X_LEFT:
								case ERRORBAR_X_RIGHT:
									p3.m_dX = p2.m_dX;
									p3.m_dY = p2.m_dY - lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;

									p4.m_dX = p2.m_dX;
									p4.m_dY = p2.m_dY + lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									break;
								case ERRORBAR_Y_UPPER:
								case ERRORBAR_Y_LOWER:
									p3.m_dX = p2.m_dX - lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									p3.m_dY = p2.m_dY;

									p4.m_dX = p2.m_dX + lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									p4.m_dY = p2.m_dY;
									break;
								}

								if (lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE ||
									lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE_AND_ARROW)
								{
									cEPS.Move_To(p3.m_dX,p3.m_dY);
									cEPS.Line_To(p4.m_dX,p4.m_dY);
									cEPS.Stroke();
								}
								if (lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_ARROW ||
									lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE_AND_ARROW)
								{
									cEPS.Move_To(p2.m_dX,p2.m_dY);
									cEPS.Line_To(p4.m_dX,p4.m_dY);
									cEPS.Stroke();

									cEPS.Move_To(p2.m_dX,p2.m_dY);
									cEPS.Line_To(p3.m_dX,p3.m_dY);
									cEPS.Stroke();
								}
							}
						}
					}
				}
				break;				
				
			default:
				break;
			}
		}

		cEPS.State_Pop();

		////////////////////////////////////////////////////////////////////////
		//
		// Draw the title of the plot if the user specified one
		//
		////////////////////////////////////////////////////////////////////////

		if (!m_szTitle.empty())
		{
			cEPS.State_Push();
			cEPS.Comment("Title");
			double dY = dGraph_Space_Y + dGraph_Offset_Y + 40;
			double dX = dWidth_Inches * 72.0 * 0.5;
			cEPS.Text(TIMES,false,false,m_dTitle_Size,CENTER,BOTTOM,Get_Color(m_eTitle_Color),dX,dY, m_szTitle.c_str(),0.0);
			cEPS.State_Pop();
		}

		////////////////////////////////////////////////////////////////////////
		//
		// Draw the title of each axis if the user specified one
		//
		////////////////////////////////////////////////////////////////////////

		cEPS.Comment("Axis titles");
		// write axis data
		cEPS.State_Push();
		uiI = 0;
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			double dX,dY;
			dX = dGraph_Space_X * 0.5 + dGraph_Offset_X;
			if (uiI & 1)
			{
				dY = dGraph_Space_Y + dGraph_Offset_Y;
				if (cAxis_Iter->m_cParameters.m_bLabel_Major_Indices)
					dY += cAxis_Iter->m_cParameters.m_dMajor_Label_Size * 2.0;
				else if (cAxis_Iter->m_cParameters.m_bLabel_Minor_Indices)
					dY += cAxis_Iter->m_cParameters.m_dMinor_Label_Size * 2.0;
				dY += 0.5 * cAxis_Iter->m_cParameters.m_dTitle_Size;
			}
			else			
			{
				dY = dGraph_Offset_Y;
				if (cAxis_Iter->m_cParameters.m_bLabel_Major_Indices)
					dY -= cAxis_Iter->m_cParameters.m_dMajor_Label_Size * 2.0;
				else if (cAxis_Iter->m_cParameters.m_bLabel_Minor_Indices)
					dY -= cAxis_Iter->m_cParameters.m_dMinor_Label_Size * 2.0;
				dY -= 0.5 * cAxis_Iter->m_cParameters.m_dTitle_Size;
			}

			cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dTitle_Size,CENTER,MIDDLE,Get_Color((*cAxis_Iter).m_cParameters.m_eTitle_Color),dX,dY, (*cAxis_Iter).m_cParameters.Get_Title());
			uiI++;
		}
		uiI = 0;
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			double	dX,dY;
			dY = dGraph_Offset_Y + 0.5 * dGraph_Space_Y;
			if (uiI & 1)
			{
				dX = dGraph_Space_X + dGraph_Offset_X + dMaxLenY[1] * 72.0;
				dX += 0.5 * cAxis_Iter->m_cParameters.m_dTitle_Size;
			}
			else			
			{
				dX = (dY_Margin[0] - dMaxLenY[0] + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0;
				dX -= 0.5 * cAxis_Iter->m_cParameters.m_dTitle_Size;
			}
			cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dTitle_Size,CENTER,MIDDLE,Get_Color((*cAxis_Iter).m_cParameters.m_eTitle_Color),dX,dY, (*cAxis_Iter).m_cParameters.Get_Title(),90.0);
			uiI++;
			
		}

		////////////////////////////////////////////////////////////////////////
		//
		// Draw the labels and ticks for each axis
		//
		////////////////////////////////////////////////////////////////////////
		cEPS.State_Pop();
		cEPS.Comment("Axis labels and ticks");
		cEPS.State_Push();
		cEPS.Translate(dGraph_Offset_X,dGraph_Offset_Y);
		cEPS.Set_Line_Width(2.0);
		uiI = 0;
		if (m_cX_Axis_Parameters.size() == 0 && cX_Axis_Default.m_dRange != 0.0)
		{
			double dLogRange = log10(fabs(cX_Axis_Default.m_dRange));
			double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
			double	dMinor_Ticks = dMajor_Ticks * 0.1;
			unsigned int uiNum_Ticks = cX_Axis_Default.m_dRange / dMajor_Ticks;
			char lpszFormat[8];
			if (uiNum_Ticks > 10)
			{
				dMajor_Ticks *= 5.0; // e.g. units of 500
				dMinor_Ticks *= 10.0; // e.g. units of 100
			}
			else if (uiNum_Ticks > 5)
			{
				dMajor_Ticks *= 2.5; // e.g. units of 500
				dMinor_Ticks *= 5.0; // e.g. units of 100
			}
			else
			{
				dMajor_Ticks *= 1.0; // e.g. units of 500
				dMinor_Ticks *= 2.5; // e.g. units of 100
			}
			double	dLog_Major = floor(log10(dMajor_Ticks));
			bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
			if (dLog_Major < 0.0)
				sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
			else
				strcpy(lpszFormat,"%.0f");
			double	dStart = XRoundNearest(cX_Axis_Default.m_dStart,dMinor_Ticks);
			double	dDeltaTickMinor = ((uiI % 2) == 0) ? cX_Axis_Default.m_cParameters.m_dMinor_Tick_Length : -(cX_Axis_Default.m_cParameters.m_dMinor_Tick_Length);
			double	dDeltaTickMajor = ((uiI % 2) == 0) ? cX_Axis_Default.m_cParameters.m_dMajor_Tick_Length : -(cX_Axis_Default.m_cParameters.m_dMajor_Tick_Length);
			//double	dDeltaText = ((uiI % 2) == 0? -(cX_Axis_Default.m_cParameters.m_dMajor_Label_Size * 0.5) : (cX_Axis_Default.m_cParameters.m_dMajor_Label_Size * 0.5);//8.0;
			for (double dVal = dStart; dVal <= cX_Axis_Default.m_dEnd; dVal += dMinor_Ticks)
			{
				if (dVal >= cX_Axis_Default.m_dLower_Limit && dVal <= cX_Axis_Default.m_dUpper_Limit) // just to make sure
				{
					double dX = cX_Axis_Default.Scale(dVal);
					double dY = (uiI % 2) * dGraph_Space_Y;
					double dText_Y ;
					if ((uiI % 2) & 1)
						dText_Y = dGraph_Space_Y + cX_Axis_Default.m_cParameters.m_dMajor_Label_Size;
					else
						dText_Y = -cX_Axis_Default.m_cParameters.m_dMajor_Label_Size;
					bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
					cEPS.Set_Line_Width(bMajor_Test ? cX_Axis_Default.m_cParameters.m_dMajor_Tick_Width : cX_Axis_Default.m_cParameters.m_dMinor_Tick_Width);
					cEPS.Move_To(dX,dY);
					if (bMajor_Test)
						cEPS.Line_To(dX,dY + dDeltaTickMajor);
					else
						cEPS.Line_To(dX,dY + dDeltaTickMinor);
					cEPS.Stroke();
					double dSize;
					epsplot::COLOR eColor;
					if (bMajor_Test && cX_Axis_Default.m_cParameters.m_bLabel_Major_Indices)
					{
						dSize = cX_Axis_Default.m_cParameters.m_dMajor_Label_Size;
						eColor = cX_Axis_Default.m_cParameters.m_eMajor_Label_Color;
					}
					else if (!bMajor_Test && cX_Axis_Default.m_cParameters.m_bLabel_Minor_Indices)
					{
						dSize = cX_Axis_Default.m_cParameters.m_dMinor_Label_Size;
						eColor = cX_Axis_Default.m_cParameters.m_eMinor_Label_Color;
					}
					else
						dSize = 0.0;

					if (dSize > 0.0)
					{
						char lpszValue[16];
						if (bScientific_Notation)
						{
							double dLog10 = log10(dVal);
							double dPower = floor(dLog10);
							double	dMantissa = dVal * pow(10.0,-dPower);\
							if (dVal == 0.0)
							{
								sprintf(lpszValue,"%.1f",dVal);
							}
							else if (dMantissa != 1.0 && dPower != 0.0)
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
								else
									sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
							}
							else if (dMantissa == 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"10^{%.0f}",dPower);
							}
							else
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2f",dMantissa);
								else
									sprintf(lpszValue,"%.1f",dMantissa);
							}
						}
						else
						{
							sprintf(lpszValue,lpszFormat,dVal);
						}
						cEPS.Text(TIMES,false,false,dSize,CENTER,MIDDLE,Get_Color(eColor),dX,dText_Y, lpszValue);
					}
				}
			}
		}
		else
		{
			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
			{
				// round minimum axis value to nearest major tick
				if (!(*cAxis_Iter).m_cParameters.m_bLog)
				{
					double dLogRange = log10(fabs((*cAxis_Iter).m_dRange));
					double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
					double	dMinor_Ticks = dMajor_Ticks * 0.1;
					unsigned int uiNum_Ticks = (*cAxis_Iter).m_dRange / dMajor_Ticks;
					char lpszFormat[8];
					if (uiNum_Ticks > 10)
					{
						dMajor_Ticks *= 5.0; // e.g. units of 500
						dMinor_Ticks *= 10.0; // e.g. units of 100
					}
					else if (uiNum_Ticks > 5)
					{
						dMajor_Ticks *= 2.5; // e.g. units of 500
						dMinor_Ticks *= 5.0; // e.g. units of 100
					}
					else
					{
						dMajor_Ticks *= 1.0; // e.g. units of 500
						dMinor_Ticks *= 2.5; // e.g. units of 100
					}
					double	dLog_Major = floor(log10(dMajor_Ticks));
					bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
					if (dLog_Major < 0.0)
						sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
					else if (dLog_Major > 3)
						strcpy(lpszFormat,"%.2e");
					else
						strcpy(lpszFormat,"%.0f");
					double	dStart = XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					double	dEnd = XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					//double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);//8.0;
					if ((*cAxis_Iter).m_cParameters.m_bInvert)
					{
						dStart =  XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
						dEnd =  XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					}
					for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
					{
						if (dVal >= (*cAxis_Iter).m_dLower_Limit && dVal <= (*cAxis_Iter).m_dUpper_Limit) // just to make sure
						{
							double dX = (*cAxis_Iter).Scale(dVal);
							double dY = (uiI % 2) * dGraph_Space_Y;
							double dText_Y;
							if ((uiI % 2) & 1)
								dText_Y = dGraph_Space_Y + (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
							else
								dText_Y = -(*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;

							bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
							cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
							cEPS.Move_To(dX,dY);
							if (bMajor_Test)
								cEPS.Line_To(dX,dY + dDeltaTickMajor);
							else
								cEPS.Line_To(dX,dY + dDeltaTickMinor);
							cEPS.Stroke();
							double dSize;
							epsplot::COLOR eColor;
							if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
							}
							else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
							}
							else
								dSize = 0.0;

							if (dSize > 0.0)
							{
								char lpszValue[16];
								if (bScientific_Notation)
								{
									double dLog10 = log10(dVal);
									double dPower = floor(dLog10);
									double	dMantissa = dVal * pow(10.0,-dPower);
									if (dVal == 0.0)
									{
										sprintf(lpszValue,"%.1f",dVal);
									}
									else if (dMantissa != 1.0 && dPower != 0.0)
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
										else
											sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
									}
									else if (dMantissa == 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"10^{%.0f}",dPower);
									}
									else
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2f",dMantissa);
										else
											sprintf(lpszValue,"%.1f",dMantissa);
									}
			
								}
								else
								{
									sprintf(lpszValue,lpszFormat,dVal);
								}
								cEPS.Text(TIMES,false,false,dSize,CENTER,MIDDLE,Get_Color(eColor),dX,dText_Y, lpszValue);
							}
				
						}
					}

				}
				else
				{
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					//double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
					double	dRange_Lower = (*cAxis_Iter).m_dStart;
					double	dRange_Upper = (*cAxis_Iter).m_dEnd;
					if (!std::isnan(dRange_Lower) && !std::isnan(dRange_Upper))
					{
						double dLower = floor(dRange_Lower) - 1.0;
						double dUpper = floor(dRange_Upper) + 1.0;
						for (double dPower = dLower; dPower <= dUpper; dPower += 1.0)
						{
							double dMult = pow(10.0,dPower);
							for (unsigned int uiScalar = 1; uiScalar < 10; uiScalar ++)
							{
								double dVal = dMult * uiScalar;
								double dX = (*cAxis_Iter).Scale(dVal);
								double dY = (uiI % 2) * dGraph_Space_Y;
								double dText_Y ;
								if ((uiI % 2) & 1)
									dText_Y = dGraph_Space_Y + (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								else
									dText_Y = -(*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								bool bMajor_Test = (uiScalar == 1);
								if ((*cAxis_Iter).m_dLower_Limit <= dVal  && dVal <= (*cAxis_Iter).m_dUpper_Limit)
								{
									cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
									cEPS.Move_To(dX,dY);
									if (bMajor_Test)
										cEPS.Line_To(dX,dY + dDeltaTickMajor);
									else
										cEPS.Line_To(dX,dY + dDeltaTickMinor);
									cEPS.Stroke();
									double dSize;
									epsplot::COLOR eColor;
									if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
									}
									else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
									}
									else
										dSize = 0.0;

									if (dSize > 0.0)
									{
										char lpszValue[16];
										double dLog10 = log10(dVal);
										double dPower = floor(dLog10);
										double	dMantissa = dVal * pow(10.0,-dPower);
										if (dVal == 0.0)
										{
											sprintf(lpszValue,"%.1f",dVal);
										}
										else if (dPower != 0.0 && dMantissa != 1.0)
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
											else
												sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
										}
										else if (dPower == 0.0 || dMantissa != 1.0)
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2f",dMantissa);
											else
												sprintf(lpszValue,"%.1f",dMantissa);
										}
										else
										{
											sprintf(lpszValue,"10^{%.0f}",dPower);
										}
										cEPS.Text(TIMES,false,false,dSize,RIGHT,MIDDLE,Get_Color(eColor),dX,dText_Y, lpszValue, 90.0);
									}
								}
							}
						}
					}
					else
						fprintf(stderr,"epsplot: x-axis is requested with log values, but range includes a negative number or zero (%f, %f).\n",dRange_Lower,dRange_Upper);
				}
				uiI++; // indicates axis at top or bottom of graph
			}
		}
		//////////////////////////////// y - axis ticks ////////////////////////////////////
		uiI = 0;
		if (m_cY_Axis_Parameters.size() == 0 && cY_Axis_Default.m_dRange != 0.0)
		{
			double dLogRange = log10(fabs(cY_Axis_Default.m_dRange));
			double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
			double	dMinor_Ticks = dMajor_Ticks * 0.1;
			unsigned int uiNum_Ticks = cY_Axis_Default.m_dRange / dMajor_Ticks;
			char lpszFormat[8];
			if (uiNum_Ticks > 10)
			{
				dMajor_Ticks *= 5.0; // e.g. units of 500
				dMinor_Ticks *= 10.0; // e.g. units of 100
			}
			else if (uiNum_Ticks > 5)
			{
				dMajor_Ticks *= 2.5; // e.g. units of 500
				dMinor_Ticks *= 5.0; // e.g. units of 100
			}
			else
			{
				dMajor_Ticks *= 1.0; // e.g. units of 500
				dMinor_Ticks *= 2.5; // e.g. units of 100
			}
			double	dLog_Major = floor(log10(dMajor_Ticks));
			bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
			if (dLog_Major < 0.0)
				sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
			else if (dLog_Major > 3)
				strcpy(lpszFormat,"%.2e");
			else
				strcpy(lpszFormat,"%.0f");
			double	dStart = XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			double	dEnd = XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
			double	dDeltaTickMinor = ((uiI % 2) == 0) ? cY_Axis_Default.m_cParameters.m_dMinor_Tick_Length : -(cY_Axis_Default.m_cParameters.m_dMinor_Tick_Length);
			double	dDeltaTickMajor = ((uiI % 2) == 0) ? cY_Axis_Default.m_cParameters.m_dMajor_Tick_Length : -(cY_Axis_Default.m_cParameters.m_dMajor_Tick_Length);
			//double	dDeltaText = ((uiI % 2) == 0) ? -(cY_Axis_Default.m_cParameters.m_dMajor_Label_Size * 0.5) : (cY_Axis_Default.m_cParameters.m_dMajor_Label_Size * 0.5);
			if (cY_Axis_Default.m_cParameters.m_bInvert)
			{
				dStart =  XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
				dEnd =  XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			}
			for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
			{
				if (dVal >= cY_Axis_Default.m_dLower_Limit && dVal <= cY_Axis_Default.m_dUpper_Limit) // just to make sure
				{
					double dY = cY_Axis_Default.Scale(dVal);
					double dX = (uiI % 2) * dGraph_Space_X;
					double dText_X;
					if ((uiI % 2) & 1)
						dText_X = dGraph_Space_X + 0.5 * cY_Axis_Default.m_cParameters.m_dMajor_Label_Size;
					else
						dText_X = -0.5 * cY_Axis_Default.m_cParameters.m_dMajor_Label_Size;
					bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
					cEPS.Set_Line_Width(bMajor_Test ? cY_Axis_Default.m_cParameters.m_dMajor_Tick_Width : cY_Axis_Default.m_cParameters.m_dMinor_Tick_Width);
					cEPS.Move_To(dX,dY);
					if (bMajor_Test)
						cEPS.Line_To(dX + dDeltaTickMajor,dY);
					else
						cEPS.Line_To(dX + dDeltaTickMinor,dY);
					cEPS.Stroke();
					double dSize;
					epsplot::COLOR eColor;
					if (bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Major_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMajor_Label_Size;
						eColor = cY_Axis_Default.m_cParameters.m_eMajor_Label_Color;
					}
					else if (!bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Minor_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMinor_Label_Size;
						eColor = cY_Axis_Default.m_cParameters.m_eMinor_Label_Color;
					}
					else
						dSize = 0.0;

					if (dSize > 0.0)
					{
						char lpszValue[16];
						if (bScientific_Notation)
						{
							double dLog10 = log10(dVal);
							double dPower = floor(dLog10);
							double	dMantissa = dVal * pow(10.0,-dPower);
							if (dVal == 0.0)
							{
								sprintf(lpszValue,"%.1f",dVal);
							}
							else if (dMantissa != 1.0 && dPower != 0.0)
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
								else
									sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
							}
							else if (dMantissa == 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"10^{%.0f}",dPower);
							}
							else
							{
								if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
									std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
									sprintf(lpszValue,"%.2f",dMantissa);
								else
									sprintf(lpszValue,"%.1f",dMantissa);
							}
						}
						else
							sprintf(lpszValue,lpszFormat,dVal);
						
						cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dText_X, dY, lpszValue, 0.0);
					}
				}
			}
		}
		else
		{
			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
			{
				// round minimum axis value to nearest major tick
				if (!(*cAxis_Iter).m_cParameters.m_bLog)
				{
					double dLogRange = log10(fabs((*cAxis_Iter).m_dRange));
					double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
					double	dMinor_Ticks = dMajor_Ticks * 0.1;
					unsigned int uiNum_Ticks = (*cAxis_Iter).m_dRange / dMajor_Ticks;
					char lpszFormat[8];
					if (uiNum_Ticks > 10)
					{
						dMajor_Ticks *= 5.0; // e.g. units of 500
						dMinor_Ticks *= 10.0; // e.g. units of 100
					}
					else if (uiNum_Ticks > 5)
					{
						dMajor_Ticks *= 2.5; // e.g. units of 500
						dMinor_Ticks *= 5.0; // e.g. units of 100
					}
					else
					{
						dMajor_Ticks *= 1.0; // e.g. units of 500
						dMinor_Ticks *= 2.5; // e.g. units of 100
					}
					double	dLog_Major = floor(log10(dMajor_Ticks));
					bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
					if (dLog_Major < 0.0)
						sprintf(lpszFormat,"%%.%.0ff",-dLog_Major + 1.0);
					else if (dLog_Major > 3)
						strcpy(lpszFormat,"%.2e");
					else
						strcpy(lpszFormat,"%.0f");
					double	dStart = XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					double	dEnd = XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					//double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
//					printf("Y %f %f %f (%f %f)\n",dStart,dEnd,dMinor_Ticks,(*cAxis_Iter).m_dLower_Limit,(*cAxis_Iter).m_dUpper_Limit);
					if ((*cAxis_Iter).m_cParameters.m_bInvert)
					{
						dStart =  XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
						dEnd =  XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					}

					for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
					{
						if (dVal >= (*cAxis_Iter).m_dLower_Limit && dVal <= (*cAxis_Iter).m_dUpper_Limit) // just to make sure
						{
							double dY = (*cAxis_Iter).Scale(dVal);
							double dX = (uiI % 2) * dGraph_Space_X;
							double dText_X;
							if ((uiI % 2) & 1)
								dText_X = dGraph_Space_X + 0.5 * cAxis_Iter->m_cParameters.m_dMajor_Label_Size;
							else
								dText_X = -0.5 * cAxis_Iter->m_cParameters.m_dMajor_Label_Size;
							bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
							cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
							cEPS.Move_To(dX,dY);
							if (bMajor_Test)
								cEPS.Line_To(dX + dDeltaTickMajor,dY);
							else
								cEPS.Line_To(dX + dDeltaTickMinor,dY);
							cEPS.Stroke();
//							printf("%f (%f %f) (%f %f)\n",dVal,dX,dY,dX + dDeltaTickMajor,dY);
							double dSize;
							epsplot::COLOR eColor;
							if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
							}
							else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
							}
							else
								dSize = 0.0;

							if (dSize > 0.0)
							{
								char lpszValue[16];
								if (bScientific_Notation)
								{
									//printf("ys\n");
									double dLog10 = log10(dVal);
									double dPower = floor(dLog10);
									double	dMantissa = dVal * pow(10.0,-dPower);
									if (dVal == 0.0)
									{
										sprintf(lpszValue,"%.1f",dVal);
									}
									else if (dMantissa != 1.0 && dPower != 0.0)
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
										else
											sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
									}
									else if (dMantissa == 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"10^{%.0f}",dPower);
									}
									else
									{
										if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
											std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
											sprintf(lpszValue,"%.2f",dMantissa);
										else
											sprintf(lpszValue,"%.1f",dMantissa);
									}
								}
								else
								{
									//printf("yns %s\n",lpszFormat);
									sprintf(lpszValue,lpszFormat,dVal);
								}
					
								cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dText_X, dY, lpszValue, 0.0);
							}
						}
					}

				}
				else
				{
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					//double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
					double	dRange_Lower = (*cAxis_Iter).m_dStart;
					double	dRange_Upper = (*cAxis_Iter).m_dEnd;
					if (!std::isnan(dRange_Lower) && !std::isnan(dRange_Upper))
					{
						double dLower = floor(dRange_Lower) - 1.0;
						double dUpper = floor(dRange_Upper) + 1.0;
						for (double dPower = dLower; dPower <= dUpper; dPower += 1.0)
						{
							double dMult = pow(10.0,dPower);
							for (unsigned int uiScalar = 1; uiScalar < 10; uiScalar ++)
							{
								double dVal = dMult * uiScalar;
								double dY = (*cAxis_Iter).Scale(dVal);
								double dX = (uiI % 2) * dGraph_Space_X;
								bool bMajor_Test = (uiScalar == 1);
								if ((*cAxis_Iter).m_dLower_Limit <= dVal  && dVal <= (*cAxis_Iter).m_dUpper_Limit)
								{
									cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
									cEPS.Move_To(dX,dY);
									if (bMajor_Test)
										cEPS.Line_To(dX + dDeltaTickMajor,dY);
									else
										cEPS.Line_To(dX + dDeltaTickMinor,dY);
									cEPS.Stroke();
		//							printf("%f (%f %f) (%f %f)\n",dVal,dX,dY,dX + dDeltaTickMajor,dY);
									double dSize;
									epsplot::COLOR eColor;
									if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
									}
									else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
									}
									else
										dSize = 0.0;
									if (dSize > 0.0)
									{
										char lpszValue[16];
										double dLog10 = log10(dVal);
										double dPower = floor(dLog10);
										double	dMantissa = dVal * pow(10.0,-dPower);
										if (dVal == 0.0)
										{
											sprintf(lpszValue,"%.1f",dVal);
										}
										else if (dMantissa != 1.0 && dPower != 0.0)
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
											else
												sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
										}
										else if (dMantissa == 1.0 && dPower != 0.0)
										{
											sprintf(lpszValue,"10^{%.0f}",dPower);
										}
										else
										{
											if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
												std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
												sprintf(lpszValue,"%.2f",dMantissa);
											else
												sprintf(lpszValue,"%.1f",dMantissa);
										}
										double dText_X;
										if ((uiI % 2) & 1)
											dText_X = dGraph_Space_X + 0.5 * cAxis_Iter->m_cParameters.m_dMajor_Label_Size;
										else
											dText_X = -0.5 * cAxis_Iter->m_cParameters.m_dMajor_Label_Size;
					
										cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dText_X, dY, lpszValue, 0.0);
									}
								}
							}
						}
					}
					else
						fprintf(stderr,"epsplot: y-axis is requested with log values, but range includes a negative number or zero (%f, %f).\n",dRange_Lower,dRange_Upper);
				}
				uiI++; // indicates axis at top or bottom of graph
			}
		}
		cEPS.State_Pop();
		//////////////////////////////////// z-axis label //////////////////////////////////////
		if (bHas_3d_plots)
		{
			cEPS.Comment("Z Axis labels");
			cEPS.State_Push();
			if (i_cGrid.m_dZ_Axis_Margin_Inches > 0.0)
			{
				cEPS.Translate((dWidth_Inches - i_cGrid.m_dZ_Axis_Margin_Inches* 0.5 - i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0, dGraph_Offset_Y);
			}
			else
			{
				cEPS.Translate(dGraph_Offset_X + dGraph_Space_X, dGraph_Offset_Y);
			}

			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cZ_Axis_Parameters.begin(); cAxis_Iter != m_cZ_Axis_Parameters.end(); cAxis_Iter++)
			{
				std::vector<color_triplet> vctColor_Data;
				for (size_t tI = 0; tI < 256; tI++)
				{
					color_triplet ctCurr = cAxis_Iter->Get_Color(tI / 255.0 * (cAxis_Iter->m_dEnd - cAxis_Iter->m_dStart) + cAxis_Iter->m_dStart);
					//printf("%.3f %.3f %.3f %.3f\n",tI/255.0,ctCurr.m_dRed,ctCurr.m_dGreen,ctCurr.m_dBlue);
					vctColor_Data.push_back(ctCurr);
				}
				double dLabel_Text_Height = 0.0;
				if (cAxis_Iter->m_cParameters.m_bLabel_Major_Indices)
					dLabel_Text_Height = cAxis_Iter->m_cParameters.m_dMajor_Label_Size; // top, bottom, and space
				//cEPS.ColorImage(dGraph_Offset_X + dGraph_Space_X + i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 0.5 * 72.0, dGraph_Offset_Y, 16.0, dGraph_Space_Y, 8, vctColor_Data, 256);
				cEPS.State_Push();
				cEPS.Translate(-cAxis_Iter->m_cParameters.m_dBar_Width * 0.5, dLabel_Text_Height * 2.0);
				cEPS.ColorImage(0.0,0.0, cAxis_Iter->m_cParameters.m_dBar_Width, dGraph_Space_Y - dLabel_Text_Height * 4.0 - (*cAxis_Iter).m_cParameters.m_dTitle_Size * 2.0, 8, vctColor_Data, 1);
				cEPS.State_Pop();
				if (cAxis_Iter->m_cParameters.m_bLabel_Major_Indices)
				{
					double dVal = cAxis_Iter->m_dStart;
					char lpszValue[64];
					if (dVal == 0.0)
						sprintf(lpszValue,"%.1f",dVal);
					else if (dVal > -1.0 && dVal <= -0.01)
						sprintf(lpszValue,"%.2f",dVal);
					else if (dVal < 1.0 && dVal >= 0.01)
						sprintf(lpszValue,"%.2f",dVal);
					else if (dVal >=  -100.0 && dVal <= 100.0)
						sprintf(lpszValue,"%.1f",dVal);
					else if (dVal >=  -10000.0 && dVal <= 10000.0)
						sprintf(lpszValue,"%.0f",dVal);
					else
					{
						double dLog10 = log10(fabs(dVal));
						double dPower = floor(dLog10);
						double	dMantissa = dVal * pow(10.0,-dPower);
						if (dMantissa != 1.0 && dPower != 0.0)
						{
							if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
								std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
								sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
							else
								sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
						}
						else if (dMantissa == 1.0 && dPower != 0.0)
						{
							sprintf(lpszValue,"10^{%.0f}",dPower);
						}
						else
						{
							if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
								std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
								sprintf(lpszValue,"%.2f",dMantissa);
							else
								sprintf(lpszValue,"%.1f",dMantissa);
						}
					}

					cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dMajor_Label_Size,CENTER,MIDDLE,Get_Color((*cAxis_Iter).m_cParameters.m_eMajor_Label_Color),0.0, dLabel_Text_Height, lpszValue, 0.0);

					dVal = cAxis_Iter->m_dEnd;
					if (dVal == 0.0)
						sprintf(lpszValue,"%.1f",dVal);
					else if (dVal > -1.0 && dVal <= -0.01)
						sprintf(lpszValue,"%.2f",dVal);
					else if (dVal < 1.0 && dVal >= 0.01)
						sprintf(lpszValue,"%.2f",dVal);
					else if (dVal >=  -100.0 && dVal <= 100.0)
						sprintf(lpszValue,"%.1f",dVal);
					else if (dVal >=  -10000.0 && dVal <= 10000.0)
						sprintf(lpszValue,"%.0f",dVal);
					else
					{
						double dLog10 = log10(fabs(dVal));
						double dPower = floor(dLog10);
						double	dMantissa = dVal * pow(10.0,-dPower);
						if (dMantissa != 1.0 && dPower != 0.0)
						{
							if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
								std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
								sprintf(lpszValue,"%.2fx10^{%.0f}",dMantissa,dPower);
							else
								sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
						}
							else if (dMantissa == 1.0 && dPower != 0.0)
						{
							sprintf(lpszValue,"10^{%.0f}",dPower);
						}
						else
						{
							if (std::fabs(std::fmod(dMantissa,1.0) - 0.25) < 0.001 ||
								std::fabs(std::fmod(dMantissa,1.0) - 0.75) < 0.001)
								sprintf(lpszValue,"%.2f",dMantissa);
							else
								sprintf(lpszValue,"%.1f",dMantissa);
						}
					}

					cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dMajor_Label_Size,CENTER,MIDDLE,Get_Color((*cAxis_Iter).m_cParameters.m_eMajor_Label_Color),0.0, dGraph_Space_Y-dLabel_Text_Height - (*cAxis_Iter).m_cParameters.m_dTitle_Size * 2.0, lpszValue, 0.0);
				}

				cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dTitle_Size,CENTER,MIDDLE,Get_Color((*cAxis_Iter).m_cParameters.m_eTitle_Color),0.0, dGraph_Space_Y-(*cAxis_Iter).m_cParameters.m_dTitle_Size * 0.5, cAxis_Iter->m_cParameters.Get_Title(), 0.0);
			}
			
			cEPS.State_Pop();
		}

		cEPS.Close_File();
	}
}

