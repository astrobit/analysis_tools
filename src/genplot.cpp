#include <Plot_Utilities.h>
#include <xio.h>
#include <xstdlib.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	char * lpszX_Axis_Title = NULL;
	char * lpszY_Axis_Title = NULL;
	unsigned int * lpuiData_Columns = NULL;
	double * lpdX_Values = NULL;
	double ** lpdY_Values = NULL;
	const char * lpszDatafile = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--file");
	const char * lpszColumnList = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-columns");
	unsigned int uiX_Axis_Column = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-column",0);
	unsigned int uiHeader_Lines  = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--header-lines",0);
	bool	bWhitespace_Separated = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--whitespace-separated");

	XDATASET	cData;


	if (lpszDatafile)
	{
		cData.ReadDataFile(lpszDatafile,bWhitespace_Separated,false,bWhitespace_Separated ? 0 : ',', uiHeader_Lines);
		if (cData.GetNumElements() > 0)
		{
			lpdX_Values = cData.GetElementArray(uiX_Axis_Column);
			unsigned int uiY_Count = 0;
			if (lpszColumnList)
			{
				uiY_Count = 0;
				while (lpszColumnList[0] != 0)
				{
					// bypass initial junk
					while (lpszColumnList[0] != 0 && (lpszColumnList[0] < '0' || lpszColumnList[0] > '9'))
						lpszColumnList++;
					if (lpszColumnList[0] >= '0' && lpszColumnList[0] <= '9')
						uiY_Count++;
					while (lpszColumnList[0] != 0 && lpszColumnList[0] >= '0' && lpszColumnList[0] <= '9')
						lpszColumnList++;
				}
				lpdY_Values = new double * [uiY_Count];
				uiY_Count = 0;
				while (lpszColumnList[0] != 0)
				{
					// bypass initial junk
					while (lpszColumnList[0] != 0 && (lpszColumnList[0] < '0' || lpszColumnList[0] > '9'))
						lpszColumnList++;
					if (lpszColumnList[0] >= '0' && lpszColumnList[0] <= '9')
					{
						lpdY_Values[uiY_Count] = cData.GetElementArray(atoi(lpszColumnList));
						uiY_Count++;
					}
					while (lpszColumnList[0] != 0 && lpszColumnList[0] >= '0' && lpszColumnList[0] <= '9')
						lpszColumnList++;
				}
				
			}
			else
			{
				unsigned int uiNum_Col = cData.GetNumColumns();
				if (uiNum_Col > 1)
				{
					lpdY_Values = new double * [uiNum_Col - 1];
					uiY_Count = 0;
					for (unsigned int uiI = 0; uiI < uiNum_Col; uiI++)
					{
						if (uiI != uiX_Axis_Column)
						{
							lpdY_Values[uiY_Count] = cData.GetElementArray(uiI);
							uiY_Count++;
						}
					}
				}
			}


			if (lpdX_Values && lpdY_Values && uiY_Count > 0)
			{
				char lpszFilename[256];
				sprintf(lpszFilename,"%s.eps",lpszDatafile);

				unsigned int uiNum_Elements = cData.GetNumElements();


				epsplot::PAGE_PARAMETERS	cPlot_Parameters;
				epsplot::DATA cPlot;
				epsplot::AXIS_PARAMETERS	cX_Axis_Parameters;
				epsplot::AXIS_PARAMETERS	cY_Axis_Parameters;
				epsplot::TEXT_PARAMETERS	cText_Paramters;
				epsplot::LINE_PARAMETERS cLine_Parameters;
				cLine_Parameters.m_dWidth = 1.0;
				cLine_Parameters.m_eColor = epsplot::BLACK;
				cLine_Parameters.m_eStipple = epsplot::SOLID;


				cPlot_Parameters.m_uiNum_Columns = 1;
				cPlot_Parameters.m_uiNum_Rows = 1;
				cPlot_Parameters.m_dWidth_Inches = 11.0;
				cPlot_Parameters.m_dHeight_Inches = 8.5;

				if (lpszX_Axis_Title)
					cX_Axis_Parameters.Set_Title(lpszX_Axis_Title);
				cX_Axis_Parameters.m_dMajor_Label_Size = 24.0;
				if (lpszY_Axis_Title)
					cY_Axis_Parameters.Set_Title(lpszY_Axis_Title);

				unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
				unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);

				for (unsigned int uiI = 0; uiI < uiY_Count; uiI++)
				{
					cLine_Parameters.m_eColor = epsplot::COLOR(epsplot::BLACK + (uiI % 7));

					cPlot.Set_Plot_Data(lpdX_Values, lpdY_Values[uiI], uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
				}

				cPlot.Set_Plot_Filename(lpszFilename);
				cPlot.Plot(cPlot_Parameters);
			}
		}	
	}
}
