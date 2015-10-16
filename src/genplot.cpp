#include <Plot_Utilities.h>
#include <xio.h>
#include <xstdlib.h>

class PLOTS
{
public:
	unsigned int m_uiX_Column;
	unsigned int m_uiY_Column;
	epsplot::COLOR	m_eColor;
	bool					m_bUse_Stipple;
	epsplot::STIPPLE	m_eStipple;
	bool					m_bUse_Symbol;
	epsplot::SYMBOL_TYPE	m_eSymbol;
};

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	const char * lpszX_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-title");
	const char * lpszY_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-title");
	PLOTS * lpcPlots = NULL;
	const char * lpszDatafile = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--file");
	const char * lpszColumnList = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-columns");
	unsigned int uiX_Axis_Column = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-column",-1);
	const char * lpszX_Columns = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-columns");
	unsigned int uiHeader_Lines  = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--header-lines",0);
	bool	bWhitespace_Separated = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--whitespace-separated");
	bool	bX_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-axis-log");
	bool	bY_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-axis-log");
	double	dX_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-min",nan(""));
	double	dX_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-max",nan(""));
	double	dY_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-min",nan(""));
	double	dY_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-max",nan(""));
	bool	bCycle_Line_Style = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--cycle-line-style");
	char lpszOutput_File[256];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output",lpszOutput_File,sizeof(lpszOutput_File),"");

	XDATASET	cData;




	if (lpszDatafile)
	{
		cData.ReadDataFile(lpszDatafile,bWhitespace_Separated,false,bWhitespace_Separated ? 0 : ',', uiHeader_Lines);
		if (cData.GetNumElements() > 0)
		{
			unsigned int uiY_Count = 0;
			unsigned int uiX_Count = 0;
			if (lpszColumnList)
			{
				uiY_Count = 0;
				uiX_Count = 0;
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
				// parse the X column list
				if (uiX_Axis_Column == -1)
				{
					lpszCursor = lpszX_Columns;
					while (lpszCursor && lpszCursor[0] != 0)
					{
						// bypass initial junk
						while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
							lpszCursor++;
						if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
							uiX_Count++;
						while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
							lpszCursor++;
					}
				}
				if (uiX_Axis_Column == -1 && uiX_Count != uiY_Count)
				{
					fprintf(stderr,"X column has not been specified and number of Y columns specified does not equate the number of X columns specified.\n");
					exit(1);
				}
				lpcPlots = new PLOTS [uiY_Count];
				uiY_Count = 0;
				lpszCursor = lpszColumnList;
				while (lpszCursor[0] != 0)
				{
					// bypass initial junk
					while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
						lpszCursor++;
					if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					{
						lpcPlots[uiY_Count].m_uiY_Column = atoi(lpszCursor);
						if (uiX_Axis_Column != -1)
							lpcPlots[uiY_Count].m_uiX_Column = uiX_Axis_Column;
						uiY_Count++;
					}
					while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						lpszCursor++;
				}
				if (uiX_Axis_Column == -1)
				{
					uiX_Count = 0;
					lpszCursor = lpszX_Columns;
					while (lpszCursor[0] != 0)
					{
						// bypass initial junk
						while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
							lpszCursor++;
						if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						{
							lpcPlots[uiX_Count].m_uiX_Column = atoi(lpszCursor);
							uiX_Count++;
						}
						while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
							lpszCursor++;
					}
				}
				
			}
			else
			{
				unsigned int uiNum_Col = cData.GetNumColumns();
				if (uiNum_Col > 1)
				{
					lpcPlots = new PLOTS [uiNum_Col - 1];
					uiY_Count = 0;
					for (unsigned int uiI = 0; uiI < uiNum_Col; uiI++)
					{
						if (uiI != uiX_Axis_Column)
						{
							lpcPlots[uiY_Count].m_uiX_Column = uiX_Axis_Column;
							lpcPlots[uiY_Count].m_uiY_Column = uiI;
							uiY_Count++;
						}
					}
				}
			}
			if (uiY_Count > 0)
			{
				std::vector<epsplot::eps_pair> * lpvData = new std::vector<epsplot::eps_pair>[uiY_Count];
				if (lpvData)
				{
					unsigned int uiNum_Rows = cData.GetNumRows();
					for (unsigned int uiI = 0; uiI < uiY_Count; uiI++)
					{
						for (unsigned int uiJ =0; uiJ < uiNum_Rows; uiJ++)
						{
							if (!cData.IsElementEmpty(lpcPlots[uiI].m_uiY_Column,uiJ) && !cData.IsElementEmpty(lpcPlots[uiI].m_uiX_Column,uiJ))
							{
								lpvData[uiI].push_back(epsplot::eps_pair(cData.GetElement(lpcPlots[uiI].m_uiX_Column,uiJ),cData.GetElement(lpcPlots[uiI].m_uiY_Column,uiJ)));
							}
						}
					}

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
						printf("Graph %i: ",uiI);
						printf("(%i,%i) ",lpcPlots[uiI].m_uiX_Column,lpcPlots[uiI].m_uiY_Column);
						cLine_Parameters.m_eColor = epsplot::COLOR(epsplot::BLACK + (uiI % 7));
						switch (cLine_Parameters.m_eColor)
						{
						case epsplot::BLACK:
							printf("black");
							break;
						case epsplot::RED:
							printf("red");
							break;
						case epsplot::GREEN:
							printf("green");
							break;
						case epsplot::BLUE:
							printf("blue");
							break;
						case epsplot::CYAN:
							printf("cyan");
							break;
						case epsplot::MAGENTA:
							printf("magenta");
							break;
						case epsplot::YELLOW:
							printf("yellow");
							break;
						}
						if (bCycle_Line_Style)
						{
							cLine_Parameters.m_eStipple = epsplot::STIPPLE(epsplot::SOLID + (uiI % 8));
							switch (cLine_Parameters.m_eStipple)
							{
							case epsplot::SOLID:
								printf(", -----");
								break;
							case epsplot::SHORT_DASH:
								printf(", - - ");
								break;
							case epsplot::LONG_DASH:
								printf(", -- -- ");
								break;
							case epsplot::LONG_SHORT_DASH:
								printf(", -- - -- ");
								break;
							case epsplot::DOTTED:
								printf(", . . .");
								break;
							case epsplot::SHORT_DASH_DOTTED:
								printf(", - . - ");
								break;
							case epsplot::LONG_DASH_DOTTED:
								printf(", -- . --");
								break;
							}
						}
						printf("\n",uiI);
						cPlot.Set_Plot_Data(lpvData[uiI], cLine_Parameters, uiX_Axis, uiY_Axis);
					}

					cPlot.Set_Plot_Filename(lpszFilename);
					cPlot.Plot(cPlot_Parameters);
					printf("Plot output to %s\n",lpszFilename);
				}
				else
				{
					if (uiY_Count == 0)
						fprintf(stderr,"y count = 0\n");
				}
			}
		}	
		else
			fprintf(stderr,"No data in file\n");
		if (lpcPlots)
			delete [] lpcPlots;
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
