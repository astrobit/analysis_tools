#include <eps_plot.h>
#include <xio.h>
#include <map>
#include <vector>
#include <cfloat>

void Set_Axis_Title(unsigned int i_uiAxis_Index, epsplot::axis_parameters &io_Axis)
{
	switch (i_uiAxis_Index)
	{
	case 2:
		io_Axis.Set_Title("v_{PS}  (1000 km/s )");
		break;
	case 3:
		io_Axis.Set_Title("T_{PS}  (1000 K)");
		break;
	case 4:
		io_Axis.Set_Title("log S^{E}");
		break;
	case 5:
		io_Axis.Set_Title("log S^{S}");
		break;
	}
}

int main(int i_iNum_Params, const char * i_lpszParams[])
{
	using namespace epsplot;

	axis_parameters	cX_Axis;
	axis_parameters	cY_Axis;
	axis_parameters	cZ_Axis;
	data	cPlot;
	page_parameters	cPage;
	xdataset_improved xData;

	cX_Axis.m_dMajor_Label_Size = 14.0; // Points
	cX_Axis.m_dMinor_Label_Size = 14.0; // Points
	cX_Axis.m_dMinor_Tick_Length = 6.0;
	cX_Axis.m_dMajor_Tick_Length = 12.0;
	cX_Axis.m_dTitle_Size = 24.0; // Points
	cY_Axis = cX_Axis;
	cZ_Axis = cX_Axis;

	class element
	{
	public:
		double m_dTemp;
		double m_dVel;
		double m_dSe;
		double m_dSs;
		double m_dQ;

		element(void){};
		element(
		double i_dTemp,
		double i_dVel,
		double i_dSe,
		double i_dSs,
		double i_dQ
		)
		{
			m_dTemp = i_dTemp;
			m_dVel = i_dVel;
			m_dSe = i_dSe;
			m_dSs = i_dSs;
			m_dQ = i_dQ;
		}
	};

	std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, std::map<unsigned int, element> > > > > mQuality_Data;

	xData.Read_Data_File(i_lpszParams[1],false,',',1);
//	printf("%i\n",xData.Get_Num_Rows());

	std::vector<size_t> vtRef_Points;

	// identify the grid points used in the search
	// this searches for the point from level i that is used as the 
	// central point of level i+1
	// These points are added to the vector vtRef_Points
	for (size_t tI = 1; tI < 9; tI++)
	{
		size_t tRef_Idx = tI * 81 + 40;
//		printf("%i -- %.2f %.2f %.2f %.2f\n",tRef_Idx,xData.Get_Element_Double(tRef_Idx,2),xData.Get_Element_Double(tRef_Idx,3),xData.Get_Element_Double(tRef_Idx,4),xData.Get_Element_Double(tRef_Idx,5));
		size_t tStart_Idx = (tI - 1) * 81;
		size_t tEnd_Idx = tI * 81;
		for (size_t tJ = tStart_Idx; tJ < tEnd_Idx; tJ++)
		{
//			printf("%i %i -- %.2f %.2f %.2f %.2f\n",tRef_Idx,tJ,xData.Get_Element_Double(tJ,2),xData.Get_Element_Double(tJ,3),xData.Get_Element_Double(tJ,4),xData.Get_Element_Double(tJ,5));
			if (xData.Get_Element_Double(tRef_Idx,2) == xData.Get_Element_Double(tJ,2) &&
				xData.Get_Element_Double(tRef_Idx,3) == xData.Get_Element_Double(tJ,3) &&
				xData.Get_Element_Double(tRef_Idx,4) == xData.Get_Element_Double(tJ,4) &&
				xData.Get_Element_Double(tRef_Idx,5) == xData.Get_Element_Double(tJ,5))
			{
				vtRef_Points.push_back(tJ);
			}
		} 
	}
//	printf("Ref points %i\n",vtRef_Points.size());
	// Retreive the parameter values and quality of fit for data reference point, and index them by the reference level, and a simple index in each axis
	// Also determine the quality of fit for the best fitting point as well as its index
	double dBest = DBL_MAX;
	size_t tBest = 0;
	for (size_t tI = 0; tI < xData.Get_Num_Rows(); tI++)
	{
		size_t tTemp = (tI % 81) / 27;
		size_t tVel = (tI % 27) / 9;
		size_t tSe = (tI % 9) / 3;
		size_t tSs = tI % 3;

		mQuality_Data[xData.Get_Element_Int(tI,1)][tTemp][tVel][tSe][tSs] = element(xData.Get_Element_Double(tI,2),xData.Get_Element_Double(tI,3),xData.Get_Element_Double(tI,4),xData.Get_Element_Double(tI,5),xData.Get_Element_Double(tI,6));

		if (xData.Get_Element_Double(tI,6) < dBest)
		{
			dBest = xData.Get_Element_Double(tI,6);
			tBest = tI + 1;
		}
	}
	// make sure that the best point is added to the list of reference points if it isn't already there
	if (vtRef_Points.back() != tBest)
		vtRef_Points.push_back(tBest);

	cZ_Axis.Set_Title("log(J)");
	cZ_Axis.m_eScheme = epsplot::two_color_transition;
//	cZ_Axis.m_ctColor_Upper = color_triplet(0,0,0.25); // only used for Z axis
//	cZ_Axis.m_ctColor_Lower = color_triplet(0,0,1.0); // only used for Z axis
	cZ_Axis.m_ctColor_Upper = color_triplet(1,1,1); // only used for Z axis
	cZ_Axis.m_ctColor_Lower = color_triplet(0,0,0.0); // only used for Z axis

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters(cX_Axis);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters(cY_Axis);
	unsigned int uiZ_Axis_ID = cPlot.Set_Z_Axis_Parameters(cZ_Axis);
	cPage.m_uiNum_Columns = 1;
	cPage.m_uiNum_Rows = 1;
	// generate plots for each axis pair
	for (size_t tAxis_X = 2; tAxis_X < 6; tAxis_X++)
	{
		size_t tX_Offset;
		size_t tY_Offset;
		for (size_t tAxis_Y = tAxis_X + 1; tAxis_Y < 6; tAxis_Y++)
		{
			Set_Axis_Title(tAxis_X,cX_Axis);
			Set_Axis_Title(tAxis_Y,cY_Axis);

			cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
			cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
			//cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);


			std::vector<eps_triplet> vetData;
			std::vector<eps_pair> vepPoint_Data;
			cPlot.Clear_Plots();
			double dQ_Min = DBL_MAX;
			double dQ_Max = 0.0;
			element cElem;
		// loop through each level of refinement
			for (size_t tRefine = 0; tRefine < 9; tRefine++)
			{
//	printf("Here\n");
				size_t tTemp = (vtRef_Points[tRefine] % 81) / 27;
				size_t tVel = (vtRef_Points[tRefine] % 27) / 9;
				size_t tSe = (vtRef_Points[tRefine] % 9) / 3;
				size_t tSs = vtRef_Points[tRefine] % 3;
				//printf("%i %i %i %i -- %.2f %.2f %.2f %.2f\n",tTemp,tVel,tSe,tSs,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dTemp,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dVel,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dSe,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dSs);

				for (size_t tAxis_A = 0; tAxis_A < 3; tAxis_A++)
				{
					for (size_t tAxis_B = 0; tAxis_B < 3; tAxis_B++)
					{
						size_t tTemp_Lcl = tTemp;
						size_t tVel_Lcl = tVel;
						size_t tSe_Lcl = tSe;
						size_t tSs_Lcl = tSs;
						switch (tAxis_X)
						{
						case 2:
							tVel_Lcl = tAxis_A;
							break;
						case 3:
							tTemp_Lcl = tAxis_A;
							break;
						case 4:
							tSe_Lcl = tAxis_A;
							break;
						case 5:
							tSs_Lcl = tAxis_A;
							break;
						}
						switch (tAxis_Y)
						{
						case 2:
							tVel_Lcl = tAxis_B;
							break;
						case 3:
							tTemp_Lcl = tAxis_B;
							break;
						case 4:
							tSe_Lcl = tAxis_B;
							break;
						case 5:
							tSs_Lcl = tAxis_B;
							break;
						}

						// only include the point if it is not the point that was chosen for next level refinement
						if (tTemp_Lcl != tTemp ||
							tVel_Lcl != tVel ||
							tSe_Lcl != tSe ||
							tSs_Lcl != tSs ||
							tRefine == 8)
						{
//printf("%i %i %i %i %i\n",tRefine,tTemp_Lcl,tVel_Lcl,tSe_Lcl,tSs_Lcl);
							cElem = mQuality_Data[tRefine][tTemp_Lcl][tVel_Lcl][tSe_Lcl][tSs_Lcl];
							double dX,dY;
							switch (tAxis_X)
							{
							case 2:
								dX = cElem.m_dVel;
								break;
							case 3:
								dX = cElem.m_dTemp;
								break;
							case 4:
								dX = cElem.m_dSe;
								break;
							case 5:
								dX = cElem.m_dSs;
								break;
							}
							switch (tAxis_Y)
							{
							case 2:
								dY = cElem.m_dVel;
								break;
							case 3:
								dY = cElem.m_dTemp;
								break;
							case 4:
								dY = cElem.m_dSe;
								break;
							case 5:
								dY = cElem.m_dSs;
								break;
							}
							vetData.push_back(eps_triplet(dX,dY,std::log10(cElem.m_dQ)));
							vepPoint_Data.push_back(eps_pair(dX,dY));
							if (dQ_Max < cElem.m_dQ)
								dQ_Max = cElem.m_dQ;
							if (dQ_Min > cElem.m_dQ)
								dQ_Min = cElem.m_dQ;
						}
					}
				}
			}
			cZ_Axis.m_dLower_Limit = std::log10(dQ_Min);
			cZ_Axis.m_dUpper_Limit = std::log10(dQ_Max);
			cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);
			

			double dX_Min = DBL_MAX, dX_Max = -DBL_MAX, dY_Min = DBL_MAX, dY_Max = -DBL_MAX;

			for (auto iterK = vepPoint_Data.begin(); iterK != vepPoint_Data.end(); iterK++)
			{
				if (dX_Min > iterK->m_dX)
					dX_Min = iterK->m_dX;
				if (dX_Max < iterK->m_dX)
					dX_Max = iterK->m_dX;
				if (dY_Min > iterK->m_dY)
					dY_Min = iterK->m_dY;
				if (dY_Max < iterK->m_dY)
					dY_Max = iterK->m_dY;
			}
			double dX_Range = std::fabs(dX_Max - dX_Min);
			double dY_Range = std::fabs(dY_Max - dY_Min);

			cX_Axis.m_dLower_Limit = dX_Min - 0.1 * dX_Range;
			cX_Axis.m_dUpper_Limit = dX_Max + 0.1 * dX_Range;
			dX_Range = cX_Axis.m_dUpper_Limit - cX_Axis.m_dLower_Limit;
			cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);

			cY_Axis.m_dLower_Limit = dY_Min - 0.1 * dY_Range;
			cY_Axis.m_dUpper_Limit = dY_Max + 0.1 * dY_Range;
			dY_Range = cY_Axis.m_dUpper_Limit - cY_Axis.m_dLower_Limit;
			cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);


			cPlot.Set_Plot_Data(vetData,nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
			std::string szFilename = i_lpszParams[1];
			size_t tPos = szFilename.find(".csv");
			szFilename[tPos + 0] = '_';
			switch (tAxis_X)
			{
			case 2:
				szFilename[tPos + 1] = 'v';
				break;
			case 3:
				szFilename[tPos + 1] = 'T';
				break;
			case 4:
				szFilename[tPos + 1] = 'E';
				break;
			case 5:
				szFilename[tPos + 1] = 'S';
				break;
			}
			szFilename[tPos + 2] = '_';
			switch (tAxis_Y)
			{
			case 2:
				szFilename[tPos + 3] = 'v';
				break;
			case 3:
				szFilename[tPos + 3] = 'T';
				break;
			case 4:
				szFilename[tPos + 3] = 'E';
				break;
			case 5:
				szFilename[tPos + 3] = 'S';
				break;
			}
			szFilename.push_back('.');
			szFilename.push_back('e');
			szFilename.push_back('p');
			szFilename.push_back('s');

			std::vector<eps_pair> vpBest_Data;
			size_t tTemp = (vtRef_Points[8] % 81) / 27;
			size_t tVel = (vtRef_Points[8] % 27) / 9;
			size_t tSe = (vtRef_Points[8] % 9) / 3;
			size_t tSs = vtRef_Points[8] % 3;
			cElem = mQuality_Data[8][tTemp][tVel][tSe][tSs];
			double dX,dY;
			switch (tAxis_X)
			{
			case 2:
				dX = cElem.m_dVel;
				break;
			case 3:
				dX = cElem.m_dTemp;
				break;
			case 4:
				dX = cElem.m_dSe;
				break;
			case 5:
				dX = cElem.m_dSs;
				break;
			}
			switch (tAxis_Y)
			{
			case 2:
				dY = cElem.m_dVel;
				break;
			case 3:
				dY = cElem.m_dTemp;
				break;
			case 4:
				dY = cElem.m_dSe;
				break;
			case 5:
				dY = cElem.m_dSs;
				break;
			}
			vpBest_Data.push_back(eps_pair(dX, dY));

			symbol_parameters cSymb;

			cSymb.m_eColor = MAGENTA;
			cSymb.m_bFilled = true;
			cSymb.m_eType = SQUARE;
			cSymb.m_dSize = 3.0;
			cPlot.Set_Symbol_Data(vepPoint_Data, cSymb, uiX_Axis_ID,uiY_Axis_ID);

			cSymb.m_eColor = YELLOW;
			cSymb.m_bFilled = false;
			cSymb.m_eType = CIRCLE;
			cSymb.m_dSize = 4.0;
			cSymb.m_dLine_Width = 0.5;
			cPlot.Set_Symbol_Data(vpBest_Data, cSymb, uiX_Axis_ID,uiY_Axis_ID);

			cPlot.Set_Plot_Filename(szFilename.c_str());

			cPage.m_dWidth_Inches = 3.35 * 2.0;
			cPage.m_dHeight_Inches = -1.0;
			cPage.m_dSide_Unprintable_Margins_Inches = 0.0; // Inches
			cPage.m_dTop_Bottom_Unprintable_Margins_Inches = 4.0/72.0; // 4 points (in inches)
//			cPage.m_dZ_Axis_Margin_Inches = 1.5; // Inches
			cPage.m_bLandscape = false;

			cPlot.Plot(cPage);
		}
	}
	return 0;
}
