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
	bool	bX_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-axis-log");
	bool	bY_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-axis-log");
	double	dX_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-min",nan(""));
	double	dX_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-max",nan(""));
	double	dY_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-min",nan(""));
	double	dY_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-max",nan(""));
	char lpszOutput_File[256];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output",lpszOutput_File,sizeof(lpszOutput_File),"");

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
				unsigned int uiNum_Elements = cData.GetNumElements();
				char lpszFilename[256];
				if (lpszOutput_File[0] != 0)
					strcpy(lpszFilename,lpszOutput_File);
				else
					sprintf(lpszFilename,"%s.eps",lpszDatafile);
					


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
				cX_Axis_Parameters.m_bLog = bX_Axis_Log;
				cY_Axis_Parameters.m_bLog = bY_Axis_Log;
				if (!isnan(dX_min))
					cX_Axis_Parameters.m_dLower_Limit = dX_min;
				if (!isnan(dX_max))
					cX_Axis_Parameters.m_dUpper_Limit = dX_max;
				if (!isnan(dY_min))
					cY_Axis_Parameters.m_dLower_Limit = dY_min;
				if (!isnan(dY_max))
					cY_Axis_Parameters.m_dUpper_Limit = dY_max;

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
				printf("Plot output to %s\n",lpszFilename);
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
	{
		fprintf(stderr,"Usage: genplot --file=<file> [OPTIONS]\n");
		fprintf(stderr,"Command line options:\n");
		fprintf(stderr,"\t--x-title=<string> : specify title of x-axis\n");
		fprintf(stderr,"\t--y-title=<string> : specify title of y-axis\n");
		fprintf(stderr,"\t--x-column=<col #> : specify which column to use as x axis\n");
		fprintf(stderr,"\t--y-columns=\"#,#,#,#,#,...,#\" : specify columns to use for plotting data.\n");
		fprintf(stderr,"\t--header-lines=# : specify how many rows of the data file to ignore\n\t\t(non numeric data will cause a fault)\n");
		fprintf(stderr,"\t--whitespace-separated : by default, the file is assumed to be comma\n\t\tseparated.  Use this if columns in the data files are separated by\n\t\tspaces or tabs.\n");
		fprintf(stderr,"\t--x-axis-log: use a log scale for the x axis\n\t\t[NOTE: this currently doesn't work well]\n");
		fprintf(stderr,"\t--y-axis-log: use a log scale for the y axis\n\t\t[NOTE: this currently doesn't work well]\n");
		fprintf(stderr,"\t--output=<outfile>: output plot to <outfile>.  Default file name for\n\t\toutput is <file>.eps\n");
		fprintf(stderr,"\t--x-min=#: Lower limit for x axis\n");
		fprintf(stderr,"\t--x-max=#: Upper limit for x axis\n");
		fprintf(stderr,"\t--y-min=#: Lower limit for y axis\n");
		fprintf(stderr,"\t--y-max=#: Upper limit for y axis\n");
	}
}
