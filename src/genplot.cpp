#include <Plot_Utilities.h>
#include <xio.h>
#include <xstdlib.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	const char * lpszX_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-title");
	const char * lpszY_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-title");
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
				const char * lpszCursor = lpszColumnList;
				while (lpszCursor[0] != 0)
				{
					// bypass initial junk
					while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
						lpszCursor++;
					if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						uiY_Count++;
					while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						lpszCursor++;
				}
				lpdY_Values = new double * [uiY_Count];
				uiY_Count = 0;
				lpszCursor = lpszColumnList;
				while (lpszCursor[0] != 0)
				{
					// bypass initial junk
					while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
						lpszCursor++;
					if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					{
						lpdY_Values[uiY_Count] = cData.GetElementArray(atoi(lpszCursor));
						uiY_Count++;
					}
					while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						lpszCursor++;
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
					switch (cLine_Parameters.m_eColor)
					{
					case epsplot::BLACK:
						printf("Graph %i: black\n",uiI);
						break;
					case epsplot::RED:
						printf("Graph %i: red\n",uiI);
						break;
					case epsplot::GREEN:
						printf("Graph %i: green\n",uiI);
						break;
					case epsplot::BLUE:
						printf("Graph %i: blue\n",uiI);
						break;
					case epsplot::CYAN:
						printf("Graph %i: cyan\n",uiI);
						break;
					case epsplot::MAGENTA:
						printf("Graph %i: magenta\n",uiI);
						break;
					case epsplot::YELLOW:
						printf("Graph %i: yellow\n",uiI);
						break;
					}
					cPlot.Set_Plot_Data(lpdX_Values, lpdY_Values[uiI], uiNum_Elements, cLine_Parameters, uiX_Axis, uiY_Axis);
				}

				cPlot.Set_Plot_Filename(lpszFilename);
				cPlot.Plot(cPlot_Parameters);
			}
			else
			{
				if (!lpdX_Values)
					fprintf(stderr,"No x values\n");
				if (!lpdY_Values)
					fprintf(stderr,"No y values\n");
				if (uiY_Count == 0)
					fprintf(stderr,"y count = 0\n");
			}
		}	
		else
			fprintf(stderr,"No data in file\n");
	}
	else
		fprintf(stderr,"Data file not specified\n");
}
