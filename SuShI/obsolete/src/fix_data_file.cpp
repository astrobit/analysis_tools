#include <best_fit_data.h>


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{

	BEST_FIT_DATA	cData;
	char	lpszFilename[256];

	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		strcpy(lpszFilename,i_lpszArg_Values[uiI]);
		cData.ReadBinary(lpszFilename);
		cData.SetNumMoments(6);
		cData.OutputBinary(lpszFilename);
		size_t uiJ = strlen(lpszFilename);
		uiJ -= 3;
		lpszFilename[uiJ] = 0;
		cData.Output(lpszFilename,false);
	}
	return 0;
}
