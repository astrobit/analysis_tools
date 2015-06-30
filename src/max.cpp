#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <unistd.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	double	dMax = -DBL_MAX;
	if (i_iArg_Count > 1)
	{
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			double dVal = atof(i_lpszArg_Values[uiI]);
			if (dMax < dVal)
				dMax = dVal;
		}
	}
	else if (!isatty(STDIN_FILENO))
	{
		char lpszBuffer[257];
		while (fgets(lpszBuffer,sizeof(lpszBuffer),stdin))
		{
			double dVal = atof(lpszBuffer);
			if (dMax < dVal)
				dMax = dVal;
		}
	}
	else
	{
		printf("Enter Q or a blank line to quit\n");
		char lpszBuffer[257];
		do
		{
			fgets(lpszBuffer,sizeof(lpszBuffer),stdin);
			if (lpszBuffer[0] != 'Q' && lpszBuffer[0] != 'q' && lpszBuffer[0] != 0 && lpszBuffer[0] != 10 && lpszBuffer[0] != 13)
			{
				double dVal = atof(lpszBuffer);
				if (dMax < dVal)
					dMax = dVal;
			}
		} while (lpszBuffer[0] != 'Q' && lpszBuffer[0] != 'q' && lpszBuffer[0] != 0 && lpszBuffer[0] != 10 && lpszBuffer[0] != 13);
	}
	printf("Max = %.17e\n",dMax);
	return 0;
}

