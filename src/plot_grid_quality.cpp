#include <eps_plot.h>
#include <xio.h>
#include <map>
#include <vector>
#include <cfloat>

int main(int i_iNum_Params, const char * i_lpszParams[])
{
	using namespace epsplot;

	axis_parameters	cX_Axis;
	axis_parameters	cY_Axis;
	axis_parameters	cZ_Axis;
	data	cPlot;
	page_parameters	cPage;
	xdataset_improved xData;
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

	std::vector<size_t> vtRef_Points;

	// identify the grid points used in the search
	for (size_t tI = 1; tI < 9; tI++)
	{
		size_t tRef_Idx = tI * 81 + 41;
		size_t tStart_Idx = (tI - 1) * 81;
		size_t tEnd_Idx = tI * 81;
		for (size_t tJ = tStart_Idx; tJ < tEnd_Idx; tJ++)
		{
			if (xData.Get_Element_Double(tRef_Idx,2) == xData.Get_Element_Double(tJ,2) &&
				xData.Get_Element_Double(tRef_Idx,3) == xData.Get_Element_Double(tJ,3) &&
				xData.Get_Element_Double(tRef_Idx,4) == xData.Get_Element_Double(tJ,4) &&
				xData.Get_Element_Double(tRef_Idx,5) == xData.Get_Element_Double(tJ,5))
			{
				vtRef_Points.push_back(tJ);
			}
		} 
	}
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
	vtRef_Points.push_back(tBest);

	cZ_Axis.Set_Title("log(Quality)");
	cZ_Axis.m_dTitle_Size = 12.0;
	cZ_Axis.m_eScheme = epsplot::two_color_transition;
	cZ_Axis.m_ctColor_Upper = color_triplet(0,0,0.25); // only used for Z axis
	cZ_Axis.m_ctColor_Lower = color_triplet(0,0,1.0); // only used for Z axis

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters(cX_Axis);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters(cY_Axis);
	unsigned int uiZ_Axis_ID = cPlot.Set_Z_Axis_Parameters(cZ_Axis);
	cPage.m_uiNum_Columns = 1;
	cPage.m_uiNum_Rows = 1;
	cPage.m_dWidth_Inches = 11.0;
	cPage.m_dHeight_Inches = 8.5;
	for (size_t tAxis_X = 2; tAxis_X < 6; tAxis_X++)
	{
		size_t tX_Offset;
		size_t tY_Offset;
		for (size_t tAxis_Y = tAxis_X + 1; tAxis_Y < 6; tAxis_Y++)
		{
			switch (tAxis_X)
			{
			case 2:
				cX_Axis.Set_Title("T_{PS}");
				break;
			case 3:
				cX_Axis.Set_Title("v_{PS}");
				break;
			case 4:
				cX_Axis.Set_Title("S^{E}");
				break;
			case 5:
				cX_Axis.Set_Title("S^{S}");
				break;
			}
			switch (tAxis_Y)
			{
			case 2:
				cY_Axis.Set_Title("T_{PS}");
				break;
			case 3:
				cY_Axis.Set_Title("v_{PS}");
				break;
			case 4:
				cY_Axis.Set_Title("S^{E}");
				break;
			case 5:
				cY_Axis.Set_Title("S^{S}");
				break;
			}


			cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
			cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
			//cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);


			std::vector<eps_triplet> vetData;
			std::vector<eps_pair> vepPoint_Data;
			cPlot.Clear_Plots();
			double dQ_Min = DBL_MAX;
			double dQ_Max = 0.0;
			element cElem;
			for (size_t tRefine = 0; tRefine < 9; tRefine++)
			{
				size_t tTemp = (vtRef_Points[tRefine] % 81) / 27;
				size_t tVel = (vtRef_Points[tRefine] % 27) / 9;
				size_t tSe = (vtRef_Points[tRefine] % 9) / 3;
				size_t tSs = vtRef_Points[tRefine] % 3;
				printf("%i %i %i %i -- %.2f %.2f %.2f %.2f\n",tTemp,tVel,tSe,tSs,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dTemp,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dVel,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dSe,mQuality_Data[tRefine][tTemp][tVel][tSe][tSs].m_dSs);

				for (size_t tAxis_A = 0; tAxis_A < 3; tAxis_A++)
				{
					for (size_t tAxis_B = 0; tAxis_B < 3; tAxis_B++)
					{
						switch (tAxis_X)
						{
						case 2:
							switch (tAxis_Y)
							{
							case 3:
								cElem = mQuality_Data[tRefine][tAxis_A][tAxis_B][tSe][tSs];
								vetData.push_back(eps_triplet(cElem.m_dTemp,cElem.m_dVel,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dVel));
								break;
							case 4:
								cElem = mQuality_Data[tRefine][tAxis_A][tVel][tAxis_B][tSs];
								vetData.push_back(eps_triplet(cElem.m_dTemp,cElem.m_dSe,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dSe));
								break;
							case 5:
								cElem = mQuality_Data[tRefine][tAxis_A][tVel][tSe][tAxis_B];
								vetData.push_back(eps_triplet(cElem.m_dTemp,cElem.m_dSs,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dSs));
								break;
							}
							break;
						case 3:
							switch (tAxis_Y)
							{
							case 4:
								cElem = mQuality_Data[tRefine][tTemp][tAxis_A][tAxis_B][tSs];
								vetData.push_back(eps_triplet(cElem.m_dVel,cElem.m_dSe,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dVel,cElem.m_dSe));
								break;
							case 5:
								cElem = mQuality_Data[tRefine][tTemp][tAxis_A][tSe][tAxis_B];
								vetData.push_back(eps_triplet(cElem.m_dVel,cElem.m_dSs,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dVel,cElem.m_dSs));
								break;
							}
							break;
						case 4:
							switch (tAxis_Y)
							{
							case 5:
								cElem = mQuality_Data[tRefine][tTemp][tVel][tAxis_A][tAxis_B];
								vetData.push_back(eps_triplet(cElem.m_dSe,cElem.m_dSs,std::log10(cElem.m_dQ)));
								vepPoint_Data.push_back(eps_pair(cElem.m_dSe,cElem.m_dSs));
								break;
							}
							break;
						}
						if (dQ_Max < cElem.m_dQ)
							dQ_Max = cElem.m_dQ;
						if (dQ_Min > cElem.m_dQ)
							dQ_Min = cElem.m_dQ;
					}
				}
			}
			cZ_Axis.m_dLower_Limit = std::log10(dQ_Min);
			cZ_Axis.m_dUpper_Limit = std::log10(dQ_Max);
			cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);
			std::string szFilename_Data = i_lpszParams[1];
			size_t tPos = szFilename_Data.find(".csv");
			szFilename_Data[tPos + 0] = '_';
			szFilename_Data[tPos + 1] = '0' + (tAxis_X - 2);
			szFilename_Data[tPos + 2] = '_';
			szFilename_Data[tPos + 3] = '0' + (tAxis_Y - 2);
			szFilename_Data.push_back('.');
			szFilename_Data.push_back('c');
			szFilename_Data.push_back('s');
			szFilename_Data.push_back('v');
			FILE * fileData = fopen(szFilename_Data.c_str(),"wt");
			if (fileData != nullptr)
			{
				for (auto iterI = vetData.begin(); iterI != vetData.end(); iterI++)
				{
					fprintf(fileData,"%.4f, %.4f, %.17e\n",iterI->m_dX,iterI->m_dY,iterI->m_dZ);
				}
				fclose(fileData);
			}				


			cPlot.Set_Plot_Data(vetData,nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
			std::string szFilename = i_lpszParams[1];
			tPos = szFilename.find(".csv");
			szFilename[tPos + 0] = '_';
			szFilename[tPos + 1] = '0' + (tAxis_X - 2);
			szFilename[tPos + 2] = '_';
			szFilename[tPos + 3] = '0' + (tAxis_Y - 2);
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
			switch (tAxis_X)
			{
			case 2:
				switch (tAxis_Y)
				{
				case 3:
					vpBest_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dVel));
					break;
				case 4:
					vpBest_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dSe));
					break;
				case 5:
					vpBest_Data.push_back(eps_pair(cElem.m_dTemp,cElem.m_dSs));
					break;
				}
				break;
			case 3:
				switch (tAxis_Y)
				{
				case 4:
					vpBest_Data.push_back(eps_pair(cElem.m_dVel,cElem.m_dSe));
					break;
				case 5:
					vpBest_Data.push_back(eps_pair(cElem.m_dVel,cElem.m_dSs));
					break;
				}
				break;
			case 4:
				switch (tAxis_Y)
				{
				case 5:
					vpBest_Data.push_back(eps_pair(cElem.m_dSe,cElem.m_dSs));
					break;
				}
				break;
			}
			symbol_parameters cSymb;
			cSymb.m_eColor = YELLOW;
			cSymb.m_bFilled = false;
			cSymb.m_eType = CIRCLE;
			cSymb.m_dSize = 12.0;
			cPlot.Set_Symbol_Data(vpBest_Data, cSymb, uiX_Axis_ID,uiY_Axis_ID);

			cSymb.m_eColor = MAGENTA;
			cSymb.m_bFilled = true;
			cSymb.m_eType = SQUARE;
			cSymb.m_dSize = 1.0;
			cPlot.Set_Symbol_Data(vepPoint_Data, cSymb, uiX_Axis_ID,uiY_Axis_ID);

			cPlot.Set_Plot_Filename(szFilename.c_str());
			cPlot.Plot(cPage);
		}
	}
	return 0;
}
