#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	FILE * fileOut = fopen("dens_prof_scaling.csv","wt");
	FILE * fileIn;
	double	dShell_Scale, dEjecta_Scale;
	char	lpszBuffer[1024];
	for (unsigned int uiI = 0; uiI < i_iArg_Count; uiI++)
	{
		fileIn = fopen(i_lpszArg_Values[uiI],"rt");
		if (fileIn)
		{
			fgets(lpszBuffer,1024,fileIn);
			char * lpszCursor = lpszBuffer;
			if (strstr(lpszBuffer,"Ejecta scaling factor:") != NULL)
			{
				lpszCursor += strlen("Ejecta scaling factor: "); // cause I'm lazy and don't want to count
				dEjecta_Scale = atof(lpszCursor);
				fgets(lpszBuffer,1024,fileIn);
				lpszCursor = lpszBuffer;
				lpszCursor += strlen("Shell scaling factor: ");
				dShell_Scale = atof(lpszCursor);
			}
			fprintf(fileOut,"%s, %.17e, %.17e\n",i_lpszArg_Values[uiI],dEjecta_Scale,dShell_Scale);
			fclose(fileIn);
		}
	}

	fclose(fileOut);
	return 0;
}
