#include <eps_plot.h>

int main(void)
{
	using namespace epsplot;

	AXIS_PARAMETERS	cX_Axis;
	AXIS_PARAMETERS	cY_Axis;
	DATA	cPlot;
	PAGE_PARAMETERS	cPage;

	cX_Axis.Set_Title("X");
	cX_Axis.m_dLower_Limit = -1.0;
	cX_Axis.m_dUpper_Limit = 2.0;

	cY_Axis.Set_Title("Y");
	cY_Axis.m_dLower_Limit = -1.0;
	cY_Axis.m_dUpper_Limit = 2.0;
	
	double	lpdX_Values[256];
	double	lpdY_Values[256];

	for (unsigned int uiI = 0; uiI < 256; uiI++)
	{
		lpdX_Values[uiI] = -1.0 + uiI * 2.0 / 255.0;
		lpdY_Values[uiI] = lpdX_Values[uiI] * lpdX_Values[uiI] *  lpdX_Values[uiI];
	}

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters(cX_Axis);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters(cY_Axis);

	cPage.m_uiNum_Columns = 1;
	cPage.m_uiNum_Rows = 1;
	cPage.m_dWidth_Inches = 11.0;
	cPage.m_dHeight_Inches = 8.5;

	cPlot.Set_Plot_Filename("test.eps");
	cPlot.Set_Plot_Data(lpdX_Values, lpdY_Values, 256, LINE_PARAMETERS(), uiX_Axis_ID, uiY_Axis_ID);
	cPlot.Plot(cPage);

	cX_Axis.m_bInvert = true;
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cPlot.Set_Plot_Filename("test_inv_x.eps");
	cPlot.Plot(cPage);

	cX_Axis.m_bInvert = false;
	cPlot.Modify_X_Axis_Parameters(uiX_Axis_ID,cX_Axis);
	cY_Axis.m_bInvert = true;
	cPlot.Modify_Y_Axis_Parameters(uiY_Axis_ID,cY_Axis);
	cPlot.Set_Plot_Filename("test_inv_y.eps");
	cPlot.Plot(cPage);

	return 0;
}
