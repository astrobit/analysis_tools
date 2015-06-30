
#include <best_fit_data.h>

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cDatafile;
	char	lpszFilename[256];

	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		strcpy(lpszFilename,i_lpszArg_Values[uiI]);
		cDatafile.ReadFromFile(lpszFilename);
		fprintf(stdout,"%s: ",lpszFilename);
		fflush(stdout);
		sprintf(lpszFilename,"%sbin",i_lpszArg_Values[uiI]);
		cDatafile.OutputBinary(lpszFilename);
		fprintf(stdout,"output to %s\n",lpszFilename);
	}


	return 0;
}

