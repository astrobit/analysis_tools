#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <cmath>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	#define NUM_TABLES 28
	FILE * fileIonTable[NUM_TABLES];
	char	lpszFilename[128];
	bool bFault = false;
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
		fileIonTable[uiI] = NULL;
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		sprintf(lpszFilename,"ion%itable.tex",uiI+1);
		fileIonTable[uiI] = fopen(lpszFilename,"wt");
		if (!fileIonTable[uiI])
		{
			fprintf(stderr,"Fault opening output file %s.\n",lpszFilename);
			bFault = true;
		}
		else
		{
			fprintf(fileIonTable[uiI],"Model");
			for (unsigned int uiD = 0; uiD < 24; uiD++)
				fprintf(fileIonTable[uiI]," & Day %i",uiD);
			fprintf(fileIonTable[uiI]," \\\\\n");
		}
	}
	if (!bFault)
	{
		for (unsigned int uiI = 1; uiI < 128; uiI++)
		{
			sprintf(lpszFilename,"run%i/photosphere.csv");
			FILE * fileTest = fopen(lpszFilename,"rt");
			if (fileTest)
			{
				fclose(fileTest);
				XDATASET	xData;
				xData.ReadDataFile(lpszFilename,false,false,',',1);

				for (unsigned int uiJ = 0; uiJ < NUM_TABLES; uiJ++)
				{
					fprintf(fileIonTable[uiJ],"%i",uiI);
					for (unsigned int uiD = 0; uiD < 24; uiD++)
					{
						unsigned int uiCol = uiJ + 2;
						unsigned int uiRow = uiD * 4 + 3;
						double dV = xData.GetElement(uiCol,uiRow) * 1e-8;
						if (!isnan(dV) && !isinf(dV) && !xData.IsElementEmpty(uiCol,uiRow))
							fprintf(fileIonTable[uiJ]," & %.1f",dV);
						else
							fprintf(fileIonTable[uiJ]," & \\ldots");
					}
					fprintf(fileIonTable[uiJ]," \\\\\n");
				}
			}
		}
	}
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		if (fileIonTable[uiI])
			fclose(fileIonTable[uiI]);
	}
	
	return 0;
}
