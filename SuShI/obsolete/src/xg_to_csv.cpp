#include <cstdio>
#include <cmath>
#include <cstdlib>

int main(int i_iArg_Count, char * i_lpszArg_Values[])
{
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		char lpszBuffer[1024];
		FILE * fileIn = fopen(i_lpszArg_Values[uiI],"rt");
		char lpszFilename[64];
		sprintf(lpszFilename,"%s.csv",i_lpszArg_Values[uiI]);
		FILE * fileOut = fopen(lpszFilename,"wt");

		unsigned int uiCol = 0;
		unsigned int uiRow = 0;
		unsigned int uiNum_Rows;
		while (!feof(fileIn))
		{
			do
			{
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
			} while (!feof(fileIn) && lpszBuffer[1] != '"' && lpszBuffer[2] != 'T');
//			dTime = atof(&lpszBuffer[12]);
			if (!feof(fileIn))
			{
				uiRow = 0;
				do
				{
					fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
					if (lpszBuffer[2]>='0' && lpszBuffer[2]<='9')
					{
	//					dValue = atof(&lpszBuffer[31];

	//					if (uiCol == 0)
	//					{
	//						dMass = atof(&lpszBuffer[2]);
	//					}
						uiRow++;
					}
				}
				while(!feof(fileIn) && (lpszBuffer[2]>='0' && lpszBuffer[2]<='9'));
//		        printf("%i\n",uiRow);
				uiCol++;
				uiNum_Rows = uiRow;
			}
		}
		uiRow = uiNum_Rows;
		printf("%i %i\n",uiRow,uiCol);
		double* lpdTimes = new double[uiCol];
		double *lpdMasses = new double[uiRow];
		double	** lpdValues = new double*[uiCol];
		for (unsigned int uiJ = 0; uiJ < uiCol; uiJ++)
		{
			lpdValues[uiJ] = new double[uiRow];
		}
		fclose(fileIn);
		fileIn = fopen(i_lpszArg_Values[uiI],"rt");
		//fseek(fileIn,SEEK_SET,0);// rewind to start of file
		uiCol = 0;
		while (!feof(fileIn))
		{
			do
			{
				fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
			} while (!feof(fileIn) && lpszBuffer[1] != '"' && lpszBuffer[2] != 'T');
			lpdTimes[uiCol] = atof(&lpszBuffer[12]);
			if (!feof(fileIn))
			{
				uiRow = 0;
				do
				{
					fgets(lpszBuffer,sizeof(lpszBuffer),fileIn);
					if (lpszBuffer[2]>='0' && lpszBuffer[2]<='9')
					{
						lpdValues[uiCol][uiRow] = atof(&lpszBuffer[31]);

						if (uiCol == 0)
						{
							lpdMasses[uiRow] = atof(&lpszBuffer[2]);
						}
						uiRow++;
					}
				}
				while(!feof(fileIn) && (lpszBuffer[2]>='0' && lpszBuffer[2]<='9'));
				uiCol++;
			}
		}
		printf("%i %i\n",uiRow,uiCol);
		fprintf(fileOut,"Mass");
		for (unsigned int uiJ = 0; uiJ < uiCol; uiJ++)
			fprintf(fileOut,", %.17e",lpdTimes[uiJ]);
		fprintf(fileOut,"\n");
		for (unsigned int uiJ = 0; uiJ < uiRow; uiJ++)
		{
			fprintf(fileOut,"%.17e",lpdMasses[uiJ]);
			for (unsigned int uiK = 0; uiK < uiCol; uiK++)
				fprintf(fileOut,", %.17e",lpdValues[uiK][uiJ]);
			fprintf(fileOut,"\n");
		}
		fclose(fileOut);
		fclose(fileIn);
		delete [] lpdTimes;
		delete [] lpdMasses;
		for (unsigned int uiJ = 0; uiJ < uiCol; uiJ++)
			delete [] lpdValues[uiJ];
		delete [] lpdValues;
	}
}
