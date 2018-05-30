#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <vector>
#include <eps_plot.h>
#include <xastro.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	xdataset_improved	cAbundance;
	xdataset_improved	cVelocity_Density;

	cAbundance.Read_Data_File(i_lpszArg_Values[1], true, 0, 0);
	cVelocity_Density.Read_Data_File(i_lpszArg_Values[2], true, 0, 1);


	std::vector<double> vMass_Coordinate;
	std::vector<double> vVelocity_Coordinate;
	std::vector<double> vDensity_Data;
	double dRadius_Inner = 0;
	double dVolume_Inner = 0;
	double dTotal_Mass = 0.0;
	double dFour_Thirds_Pi = 4.0 * g_XASTRO.k_dpi / 3.0;

	for (size_t tI = 0; tI < cVelocity_Density.Get_Num_Rows(); tI++)
	{
		double dVelocity = cVelocity_Density.Get_Element_Double(tI,1);
		double dDensity = cVelocity_Density.Get_Element_Double(tI,2);
		vVelocity_Coordinate.push_back(dVelocity);
		double dRadius_Outer;
		if (tI != (cVelocity_Density.Get_Num_Rows() - 1))
		{
			double dVelocityp1 = cVelocity_Density.Get_Element_Double(tI + 1,1);
			dRadius_Outer = (dVelocityp1 + dVelocity) * 0.5 * 86400 * 100000.0; // 86400 = 1 day
		}
		else
		{
			double dVelocitym1 = cVelocity_Density.Get_Element_Double(tI - 1,1);
			dRadius_Outer = (2.0 * dVelocity - dVelocitym1) * 86400 * 100000.0; // 86400 = 1 day
		}
		double dVolume_Outer = dRadius_Outer * dRadius_Outer * dRadius_Outer;
		double dVolume = dFour_Thirds_Pi * (dVolume_Outer - dVolume_Inner);
		dVolume_Inner = dVolume_Outer;
		dRadius_Inner = dRadius_Outer;
		double dMass_Zone = dVolume * cVelocity_Density.Get_Element_Double(tI,2);
		double dMass_Coord = dTotal_Mass + 0.5 * dMass_Zone;
		dTotal_Mass += dMass_Zone;
		vMass_Coordinate.push_back(dMass_Coord * 0.5e-33);
		vDensity_Data.push_back(std::log10(dDensity));
		//printf("%i %.2e\n",tI, dMass_Coord * 0.5e-33);
	}
	std::vector<epsplot::eps_pair> vAbd_Data[2][28];
	std::vector<epsplot::eps_pair> vDens_Data[2][28];
	for (size_t tI = 0; tI < cAbundance.Get_Num_Rows(); tI++)
	{
		for (size_t tJ = 1; tJ <= 28; tJ++)
		{
			double dAbd = cAbundance.Get_Element_Double(tI,tJ);
			if (dAbd > 0)
			{
				
				dAbd = std::log10(dAbd);
				vAbd_Data[0][tJ - 1].push_back(epsplot::eps_pair(vVelocity_Coordinate[tI],dAbd));
				vDens_Data[0][tJ - 1].push_back(epsplot::eps_pair(vVelocity_Coordinate[tI],dAbd + vDensity_Data[tI]));
				vAbd_Data[1][tJ - 1].push_back(epsplot::eps_pair(vMass_Coordinate[tI],dAbd));
				vDens_Data[1][tJ - 1].push_back(epsplot::eps_pair(vMass_Coordinate[tI],dAbd + vDensity_Data[tI]));

			}
		}
	}
	
	epsplot::data cPlot;
	epsplot::page_parameters	cPlot_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;
	epsplot::axis_parameters	cAxis;

	cPlot.Define_Custom_Color(epsplot::CLR_CUSTOM_1, epsplot::color_triplet(0.8,0.8,0.0));

	unsigned int uiX_Axis;
	unsigned int uiY_Axis;
	for (size_t tPlot = 0; tPlot < 4; tPlot++)
	{
		cPlot.Clear_Plots();
		cAxis.m_bLog = false;
		switch (tPlot)
		{
		case 0:
			cPlot.Set_Plot_Filename("velocity-abd.eps");
			cAxis.Set_Title("Velocity (km/s)");
			cAxis.m_dLower_Limit = 0.0;
			cAxis.m_dUpper_Limit = 100000.0;
			uiX_Axis = cPlot.Set_X_Axis_Parameters( cAxis);
			cAxis.Set_Title("Log Abundance (by mass)");
			cAxis.m_dLower_Limit = -10;
			cAxis.m_dUpper_Limit = 1.0;
			uiY_Axis = cPlot.Set_Y_Axis_Parameters( cAxis);
			break;
		case 1:
			cPlot.Set_Plot_Filename("mass-abd.eps");
			cAxis.Set_Title("Mass (Solar Masses)");
			cAxis.m_dLower_Limit = 0.0;
			cAxis.m_dUpper_Limit = 1.5;
			cPlot.Modify_X_Axis_Parameters(uiX_Axis,cAxis);
			break;
		case 2:
			cPlot.Set_Plot_Filename("velocity-dens.eps");
			cAxis.Set_Title("Velocity (km/s)");
			cAxis.m_dLower_Limit = 0.0;
			cAxis.m_dUpper_Limit = 100000.0;
			cPlot.Modify_X_Axis_Parameters(uiX_Axis, cAxis);
			cAxis.Set_Title("Log Density [g cm^{-3}  ]");
			cAxis.m_dLower_Limit = -25;
			cAxis.m_dUpper_Limit = -5.0;
			cPlot.Modify_Y_Axis_Parameters(uiY_Axis, cAxis);
			break;
		case 3:
			cPlot.Set_Plot_Filename("mass-dens.eps");
			cAxis.Set_Title("Mass (Solar Masses)");
			cAxis.m_dLower_Limit = 0.0;
			cAxis.m_dUpper_Limit = 1.5;
			cPlot.Modify_X_Axis_Parameters(uiX_Axis,cAxis);
			break;
		}

		epsplot::line_parameters cLine;
		epsplot::text_parameters cText;
		cText.m_iFont_Size = 16;
		cText.m_eVertical_Justification = epsplot::MIDDLE;

		std::vector<epsplot::eps_pair> vLegend;
		double dLine_Width = 0.05;
		double dText_Offset = 0.1;

		for (size_t tV = 0; tV < 3; tV++)
		{
			double dX;
			switch (tPlot)
			{
			case 0:
			case 2:
				dX = tV * 2.5e4 + 1.0e4;
				break;
			case 1:
			case 3:
				dX = tV * 0.325 + 0.35;
				break;
			}
			for (size_t tA = 0; tA < 3; tA++)
			{
				double dY;
				switch (tPlot)
				{
				case 0:
				case 1:
				 	dY = -8.0 - tA / 3.0;
					break;
				case 2:
				 	dY = -6.0 - tA;
					break;
				case 3:
				 	dY = -21.0 - tA;
					break;
				}
				vLegend.push_back(epsplot::eps_pair(dX,dY));
			}
		}
		switch (tPlot)
		{
		case 0:
		case 2:
			dLine_Width = 1.25e4;
			dText_Offset = 1.5e4;
			break;
		case 1:
		case 3:
			dLine_Width = 0.2;
			dText_Offset = 0.225;
		}
		size_t tLegend_Idx = 0;
		for (size_t tI = 0; tI < 28; tI++)
		{
			bool bPlot = false;
			switch (tI)
			{
			case 0:
				bPlot = true;
				cLine.m_eColor = epsplot::RED;
				cLine.m_eStipple = epsplot::SOLID;
				break;
			case 1:
				bPlot = true;
				cLine.m_eColor = epsplot::MAGENTA;
				cLine.m_eStipple = epsplot::LONG_DASH;
				break;
			case 5:
				bPlot = true;
				cLine.m_eColor = epsplot::BLACK;
				cLine.m_eStipple = epsplot::LONG_SHORT_DASH;
				break;
			case 7:
				bPlot = true;
				cLine.m_eColor = epsplot::GREEN; // oxygen
				cLine.m_eStipple = epsplot::DOTTED;
				break;
			case 13:
				bPlot = true;
				cLine.m_eColor = epsplot::BLUE; // silicon
				cLine.m_eStipple = epsplot::SHORT_DASH_DOTTED;
				break;
			case 15:
				bPlot = true;
				cLine.m_eColor = epsplot::CLR_CUSTOM_1; // sulfur
				cLine.m_eStipple = epsplot::LONG_DASH_DOTTED;
				break;
			case 19:
				bPlot = true;
				cLine.m_eColor = epsplot::CYAN; // calcium
				cLine.m_eStipple = epsplot::LONG_SHORT_DASH_DOTTED;
				break;
			case 25:
				bPlot = true;
				cLine.m_eColor = epsplot::GRAY_75; // iron
				cLine.m_eStipple = epsplot::LONG_LONG_DASH;
				break;
			case 27:
				bPlot = true;
				cLine.m_eColor = epsplot::GRAY_25; // nickel
				cLine.m_eStipple = epsplot::SOLID;
				break;
			}
			if (bPlot && vAbd_Data[tPlot][tI].size() > 0)
			{
				if (tPlot == 0 || tPlot == 1)
					cPlot.Set_Plot_Data(vAbd_Data[tPlot][tI], cLine, uiX_Axis, uiY_Axis);
				else
					cPlot.Set_Plot_Data(vDens_Data[tPlot % 2][tI], cLine, uiX_Axis, uiY_Axis);


				std::vector<epsplot::eps_pair> vLegend_Line;
				epsplot::eps_pair pLegend_Start = vLegend[tLegend_Idx];
				epsplot::eps_pair pLegend_Line_End = vLegend[tLegend_Idx];

				pLegend_Line_End.m_dX += dLine_Width;
				vLegend_Line.push_back(pLegend_Start);
				vLegend_Line.push_back(pLegend_Line_End);

				cPlot.Set_Plot_Data(vLegend_Line, cLine, uiX_Axis, uiY_Axis);

				char lpszElem[8];
				xGet_Element_Symbol(tI + 1, lpszElem);
				cPlot.Set_Text_Data(pLegend_Start.m_dX + dText_Offset, pLegend_Start.m_dY, lpszElem, cLine, cText, uiX_Axis, uiY_Axis);
				tLegend_Idx++;

			}
		}
		cPlot.Plot(cPlot_Parameters);
	}
	return 0;
}
