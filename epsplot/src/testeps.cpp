#include <eps_plot.h>

epsplot::eps_triplet Fill_Triplet(const double & i_dX, const double & i_dY)
{
	return epsplot::eps_triplet(i_dX,i_dY,std::sqrt(i_dX * i_dX + i_dY * i_dY));
}

int main(void)
{
	using namespace epsplot;

	axis_parameters	cX_Axis;
	axis_parameters	cY_Axis;
	axis_parameters	cZ_Axis;
	data	cPlot;
	page_parameters	cPage;

	cX_Axis.Set_Title("X");
	cX_Axis.m_dLower_Limit = -1.0;
	cX_Axis.m_dUpper_Limit = 2.0;

	cY_Axis.Set_Title("Y");
	cY_Axis.m_dLower_Limit = -1.0;
	cY_Axis.m_dUpper_Limit = 6.0;
	
	cZ_Axis.Set_Title("Z");
	cZ_Axis.m_dLower_Limit = 0.0;
	cZ_Axis.m_dUpper_Limit = std::sqrt(4.0 + 36.0);
	cZ_Axis.m_eScheme = epsplot::dark_to_light;//epsplot::rainbow;

	double	lpdX_Values[256];
	double	lpdY_Values[256];

	for (unsigned int uiI = 0; uiI < 256; uiI++)
	{
		lpdX_Values[uiI] = -1.0 + uiI * 2.0 / 255.0;
		lpdY_Values[uiI] = lpdX_Values[uiI] * lpdX_Values[uiI] *  lpdX_Values[uiI];
	}

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters(cX_Axis);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters(cY_Axis);
	unsigned int uiZ_Axis_ID = cPlot.Set_Z_Axis_Parameters(cZ_Axis);

	cPage.m_uiNum_Columns = 1;
	cPage.m_uiNum_Rows = 1;
	cPage.m_dWidth_Inches = 11.0;
	cPage.m_dHeight_Inches = 8.5;

	cPlot.Set_Plot_Filename("test.eps");
	cPlot.Set_Plot_Data(lpdX_Values, lpdY_Values, 256, line_parameters(), uiX_Axis_ID, uiY_Axis_ID);
	cPlot.Plot(cPage);

	cX_Axis.m_bInvert = true;
	cX_Axis.Set_Title("invert X");
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cPlot.Set_Plot_Filename("test_inv_x.eps");
	cPlot.Plot(cPage);

	cX_Axis.m_bInvert = false;
	cX_Axis.Set_Title("X");
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cY_Axis.Set_Title("invert Y");
	cY_Axis.m_bInvert = true;
	cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
	cPlot.Set_Plot_Filename("test_inv_y.eps");
	cPlot.Plot(cPage);

	cY_Axis.Set_Title("Y");
	cY_Axis.m_bInvert = false;
	cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
	cPlot.Clear_Plots();
	std::vector<eps_triplet> vetData;
	std::vector<eps_pair> vepData;
	for (double dX = -1.0; dX <= 2.250; dX += 0.250)
	{
		for (double dY = -1.0; dY <= 6.250; dY += 0.250)
		{
			printf("%f %f\n",dX,dY);
			vetData.push_back(Fill_Triplet(dX,dY));
			vepData.push_back(eps_pair(dX,dY));
		}
	}

	symbol_parameters cSymb;

	cSymb.m_eColor = MAGENTA;
	cSymb.m_bFilled = true;
	cSymb.m_eType = SQUARE;
	cSymb.m_dSize = 1.0;

	cPlot.Set_Plot_Data(vetData,epsplot::nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
	cPlot.Set_Symbol_Data(vepData, cSymb, uiX_Axis_ID,uiY_Axis_ID);
	cPlot.Set_Plot_Filename("test_image.eps");
	cPlot.Plot(cPage);

	cPlot.Clear_Plots();
	cPlot.Set_Plot_Filename("test_image_2.eps");
	vetData.clear();
	vepData.clear();
	for (unsigned int uiI = 0; uiI < 25; uiI++)
	{
		double dX = rand() * 3.25 / RAND_MAX - 1;
		double dY = rand() * 7.25 / RAND_MAX - 1;
		double dZ = rand() * std::sqrt(4.0 + 36.0) / RAND_MAX;

		vetData.push_back(eps_triplet(dX,dY,dZ));
		vepData.push_back(eps_pair(dX,dY));
	}
	cPlot.Set_Plot_Data(vetData,epsplot::nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
	cPlot.Set_Symbol_Data(vepData, cSymb, uiX_Axis_ID,uiY_Axis_ID);
	cPlot.Plot(cPage);

	cPlot.Clear_Plots();

	cPage.m_dWidth_Inches = 8.5;
	cX_Axis.m_dLower_Limit = -1.0;
	cX_Axis.m_dUpper_Limit = 2.0;
	cY_Axis.m_dLower_Limit = -1.0;
	cY_Axis.m_dUpper_Limit = 2.0;
	cZ_Axis.m_dLower_Limit = 0.0;
	cZ_Axis.m_dUpper_Limit = 3.0;
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
	cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);
	cPlot.Set_Plot_Filename("test_image_3.eps");
	vetData.clear();
	vepData.clear();
	double dCoord_X[2] = {-0.5,1.5};
	double dCoord_Y[2] = {-0.5,1.5};

	for (unsigned int uiI = 0; uiI < 2; uiI++)
		for (unsigned int uiJ = 0; uiJ < 2; uiJ++)
		{
			vetData.push_back(eps_triplet(dCoord_X[uiI],dCoord_Y[uiJ],uiI + uiJ * 2));
			vepData.push_back(eps_pair(dCoord_X[uiI],dCoord_Y[uiJ]));
		}
	vetData[3].m_dX -= 0.5;
	vetData[3].m_dY -= 0.5;
	vepData[3].m_dX -= 0.5;
	vepData[3].m_dY -= 0.5;

	cPlot.Set_Plot_Data(vetData,epsplot::nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
	cPlot.Set_Symbol_Data(vepData, cSymb, uiX_Axis_ID,uiY_Axis_ID);
	cPlot.Plot(cPage);

	cPage.m_dWidth_Inches = 11.0;
	cPlot.Set_Plot_Filename("test_image_3r.eps");
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
	cPlot.Modify_Z_Axis_Parameters(uiZ_Axis_ID,cZ_Axis);
	cPlot.Clear_Plots();

	cPlot.Set_Plot_Data(vetData,epsplot::nearest,uiX_Axis_ID,uiY_Axis_ID,uiZ_Axis_ID);
	cPlot.Set_Symbol_Data(vepData, cSymb, uiX_Axis_ID,uiY_Axis_ID);
	cPlot.Plot(cPage);

	return 0;
}





