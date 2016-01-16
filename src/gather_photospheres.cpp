#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <cmath>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	#define NUM_TABLES 28
	FILE * fileIonTable[NUM_TABLES][2];
	char	lpszFilename[128];
	bool bFault = false;
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		fileIonTable[uiI][0] = NULL;
		fileIonTable[uiI][1] = NULL;
	}
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		for (unsigned int uiJ = 0; uiJ < 2; uiJ++)
		{
			sprintf(lpszFilename,"psdata/ps_ion_%i_table_%i.tex",uiI+1,uiJ);
			fileIonTable[uiI][uiJ] = fopen(lpszFilename,"wt");
			if (!fileIonTable[uiI][uiJ])
			{
				fprintf(stderr,"Fault opening output file %s.\n",lpszFilename);
				bFault = true;
			}
			else
			{
				fprintf(fileIonTable[uiI][uiJ],"Model");
				for (unsigned int uiD = uiJ * 12 + 1; uiD < ((uiJ + 1) * 12 + 1); uiD++)
					fprintf(fileIonTable[uiI][uiJ]," & %i",uiD);
				fprintf(fileIonTable[uiI][uiJ]," \\\\\n");
			}
		}
	}
	if (!bFault)
	{
		for (unsigned int uiI = 1; uiI < 128; uiI++)
		{
			sprintf(lpszFilename,"run%i/photosphere.csv",uiI);
			FILE * fileTest = fopen(lpszFilename,"rt");
			if (fileTest)
			{
				char chTest;
				size_t uiData = 0;
				uiData = fread(&chTest,1,1,fileTest);
				fclose(fileTest);
				if (uiData > 0)			
				{
					XDATASET	xData;
					printf("Processing %s\n",lpszFilename);
					xData.ReadDataFile(lpszFilename,false,false,',',1);
					if (xData.GetNumRows() > 0)
					{
						for (unsigned int uiJ = 0; uiJ < NUM_TABLES; uiJ++)
						{
							for (unsigned int uiK = 0; uiK < 2; uiK++)
							{
								fprintf(fileIonTable[uiJ][uiK],"%i",uiI);
								for (unsigned int uiD = uiK * 12 + 1; uiD < ((uiK + 1) * 12 + 1); uiD++)
								{
									unsigned int uiCol = uiJ + 2;
									unsigned int uiRow = (uiD - 1) * 4 + 3;
									double dV = xData.GetElement(uiCol,uiRow) * 1e-8;
									if (!isnan(dV) && !isinf(dV) && !xData.IsElementEmpty(uiCol,uiRow))
										fprintf(fileIonTable[uiJ][uiK]," & %.1f",dV);
									else
										fprintf(fileIonTable[uiJ][uiK]," & \\ldots");
								}
								fprintf(fileIonTable[uiJ][uiK]," \\\\\n");
							}
						}
					}
				}
				//else
					//fprintf(stderr,"Error opening %s\n",lpszFilename);
			}
		}
	}
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		if (fileIonTable[uiI][0])
			fclose(fileIonTable[uiI][0]);
		if (fileIonTable[uiI][1])
			fclose(fileIonTable[uiI][1]);
	}
	
	return 0;
}
