#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xio.h>
#include <Plot_Utilities.h>


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[256];
	char	lpszBuffer[1024];
	XDATASET cData;
	char	chSeparator;
	bool	bNo_Title = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-plot-title");
	bool	bLinear_Plot = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--linear-scale");

	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		if ((strcmp(i_lpszArg_Values[uiI],"--no-plot-title") != 0) &&
			(strcmp(i_lpszArg_Values[uiI],"--linear-scale") != 0))
		{
			FILE * fileIn = fopen(i_lpszArg_Values[uiI],"rt");
			if (fileIn)
			{
				if (fgets(lpszBuffer,1024,fileIn))
				{
					if (strchr(lpszBuffer,','))
						chSeparator = ',';
					else if (strchr(lpszBuffer,'\t'))
						chSeparator = '\t';
					else
						chSeparator = 0;
				}
				fclose(fileIn);
				cData.ReadDataFile(i_lpszArg_Values[uiI],chSeparator == 0, false,chSeparator,0);
				if (cData.GetNumColumns() == 3)
				{
					cData.Add_Columns(2);
					double	dMin_Flux = DBL_MAX;
					double	dMin_Flux_Error = DBL_MAX;
					double	dMax_Flux = 0.0;
					double	dMax_Flux_Error = 0.0;
					bool bBad_Error = true;
					for (unsigned int uiJ = 0; uiJ < cData.GetNumElements(); uiJ++)
					{
						double	dFlux = cData.GetElement(1,uiJ);
						double	dFlux_Error = cData.GetElement(2,uiJ);
						double	dFlux_Lower = dFlux - dFlux_Error;
						double	dFlux_Upper = dFlux + dFlux_Error;
						if (dFlux > 0.0 && dFlux < dMin_Flux)
							dMin_Flux = dFlux;
						if (dFlux_Lower > 0.0 && dFlux_Lower < dMin_Flux_Error)
							dMin_Flux_Error = dFlux_Lower;
						if (dFlux > dMax_Flux)
							dMax_Flux = dFlux;
						if (dFlux_Upper > dMax_Flux_Error)
							dMax_Flux_Error = dFlux_Upper;
						bBad_Error &= (dFlux_Error == 1.0 || dFlux_Error == 0.0);
						if (isnan(dFlux) || isnan(dFlux_Error) || isinf(dFlux) || isinf(dFlux_Error))
							printf("%s: Bad flux point at %.2f A\n",i_lpszArg_Values[uiI],cData.GetElement(0,uiJ));
					}
					if (!bBad_Error && dMin_Flux_Error < dMin_Flux)
						dMin_Flux = dMin_Flux_Error;
					if (!bBad_Error && dMax_Flux_Error < dMax_Flux)
						dMax_Flux = dMax_Flux_Error;
					printf("%s: Min Flux %.2e\tMax Flux %.2e\t%s\n",i_lpszArg_Values[uiI],dMin_Flux,dMax_Flux,bBad_Error?"Error not valid":"Error valid");
					for (unsigned int uiJ = 0; uiJ < cData.GetNumElements(); uiJ++)
					{
						double	dFlux = cData.GetElement(1,uiJ);
						double	dFlux_Error = cData.GetElement(2,uiJ);
						if (bBad_Error)
							dFlux_Error = 0.0;
							
						double	dUpper = dFlux + dFlux_Error;
						double	dLower = dFlux - dFlux_Error;
						if (dLower <= 0.0)
							dLower = dMin_Flux;
						if (dFlux <= 0.0)
							dFlux = dMin_Flux;
						if (dUpper <= 0.0)
							dUpper = dMin_Flux;
						if (bLinear_Plot)
						{
							cData.SetElement(3,uiJ,dUpper); // upper bound
							cData.SetElement(4,uiJ,dLower); // lower bound
						}
						else
						{
							cData.SetElement(3,uiJ,log10(dUpper)); // upper bound
							cData.SetElement(4,uiJ,log10(dLower)); // lower bound

							cData.SetElement(1,uiJ,log10(dFlux));
						}
					}
				}
				else
				{
					for (unsigned int uiJ = 0; uiJ < cData.GetNumElements(); uiJ++)
					{
						double	dFlux = cData.GetElement(1,uiJ);
						cData.SetElement(1,uiJ,log10(dFlux));
					}
				}
				sprintf(lpszFilename,"%s.eps",i_lpszArg_Values[uiI]);
				Plot(cData,i_lpszArg_Values[uiI],lpszFilename,2500.0,10000.0, bNo_Title ? NULL : i_lpszArg_Values[uiI], bLinear_Plot, true);
				sprintf(lpszFilename,"%s.NIR.eps",i_lpszArg_Values[uiI]);
				Plot(cData,i_lpszArg_Values[uiI],lpszFilename,7000.0,9000.0, bNo_Title ? NULL : i_lpszArg_Values[uiI], bLinear_Plot, true);
				sprintf(lpszFilename,"%s.HK.eps",i_lpszArg_Values[uiI]);
				Plot(cData,i_lpszArg_Values[uiI],lpszFilename,3000.0,5000.0, bNo_Title ? NULL : i_lpszArg_Values[uiI], bLinear_Plot, true);
				sprintf(lpszFilename,"%s.Si6355.eps",i_lpszArg_Values[uiI]);
				Plot(cData,i_lpszArg_Values[uiI],lpszFilename,5000.0,7000.0, bNo_Title ? NULL : i_lpszArg_Values[uiI], bLinear_Plot, true);
			}
		}
	}
	return 0;
}

