#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xio.h>
#include <Plot_Utilities.h>
#include <line_routines.h>

#define VERSION "0.1"
void Usage(const char * i_lpszError_Text)
{
		fprintf(stderr,"equivwidth v%s\n",VERSION);
		if (i_lpszError_Text && i_lpszError_Text[0] != 0)
			fprintf(stderr,"\nError: %s\n\n",i_lpszError_Text);
		fprintf(stderr,"Usage: \n\tequivwidth --red=X.X --blue=X.X --file=<source data file> [--avg=[10]]\n");
		fprintf(stderr,"\t--file: file containing the spectrum data.\n");
		fprintf(stderr,"\tMust be space, tab, or column separated.\n");
		fprintf(stderr,"\t--red=X.X\tWavelength of flux to use for red end of continuum.\n");
		fprintf(stderr,"\t--blue=X.X\tWavelength of flux to use for blue end of continuum.\n");
		fprintf(stderr,"\t--avg=[10]\tNumber of data points to sample to determine flux of continuum.\n");
		fprintf(stderr,"Output: Equivalent width of specifed line region in angstroms\n");
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[256];
	char	lpszBuffer[1024];
	XDATASET cData;
	char	chSeparator;
	unsigned int uiAveraging_Length;
	double	dCont_WL[2];
	double	dEW;
	if (i_iArg_Count == 1 || i_lpszArg_Values[0][0] == '?' ||
		(i_lpszArg_Values[0][0] == '-' && i_lpszArg_Values[0][1] == 'h') ||
		(i_lpszArg_Values[0][0] == '-' && i_lpszArg_Values[0][1] == 'H') ||
		(i_lpszArg_Values[0][0] == '-' && i_lpszArg_Values[0][1] == '-' && i_lpszArg_Values[0][2] == 'h') ||
		(i_lpszArg_Values[0][0] == '-' && i_lpszArg_Values[0][1] == '-' && i_lpszArg_Values[0][2] == 'H'))
	{
		Usage("");
	}
	else
	{
		dCont_WL[0] =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--continuum-wl-blue",-1.0);
		if (dCont_WL[0] == -1.0)
			dCont_WL[0] =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--blue",-1.0);
		dCont_WL[1] =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--continuum-wl-red",-1.0);
		if (dCont_WL[1] == -1.0)
			dCont_WL[1] =	xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--red",-1.0);
		uiAveraging_Length = xParse_Command_Line_UInt(i_iArg_Count,i_lpszArg_Values,"--continuum-averaging-length",10);
		if (uiAveraging_Length == 10)
			uiAveraging_Length = xParse_Command_Line_UInt(i_iArg_Count,i_lpszArg_Values,"--avg",10);
		lpszFilename[0] = 0;
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--datafile",lpszFilename,sizeof(lpszFilename),NULL);
		if (!lpszFilename || lpszFilename[0] == 0)
			xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--file",lpszFilename,sizeof(lpszFilename),NULL);
		if (dCont_WL[0] > 0.0 && dCont_WL[1] > 0.0)
		{
			FILE * fileIn = fopen(lpszFilename,"rt");
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
				cData.ReadDataFile(lpszFilename,chSeparator == 0, false,chSeparator,0);
				if (cData.GetNumElements() > 0)
				{
					double	dEW = Equivalent_Width(cData,dCont_WL[0],dCont_WL[1],uiAveraging_Length);

					printf("The EW for %s for the feature between %.2f and %.2f is %.3e\n",lpszFilename, dCont_WL[0],dCont_WL[1],dEW);
				}
				else
					Usage("No data in file");
			}
			else
				Usage("Couldn't open file");
		}
		else
			Usage("Continuum WL bounds not specified");
	}	



	return 0;
}

