#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xflash.h>


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	char	lpszFilename[32];
	unsigned int uiI;
	XFLASH_File	cFlash_File;
	unsigned int uiFile_ID;

	if (i_iArg_Count >= 2)
	{
		for (uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			if (i_lpszArg_Values[uiI][0] >= '0' && i_lpszArg_Values[uiI][0] <= '9')
			{
				uiFile_ID = atoi(i_lpszArg_Values[uiI]);
				sprintf(lpszFilename,"explos_hdf5_chk_%04i",uiFile_ID);
			}
			else
			{
				const char * lpszCursor = i_lpszArg_Values[uiI];
				strcpy(lpszFilename,i_lpszArg_Values[uiI]);
				lpszCursor += (strlen(i_lpszArg_Values[uiI]) - 4);
				uiFile_ID = atoi(lpszCursor);
			}
			cFlash_File.Open(lpszFilename);
			printf("Opened file %s.\n",lpszFilename);
			printf("Time for file is %.1f s\n",cFlash_File.m_dTime);
			cFlash_File.Close();
		} // loop through args
	} // if args >= 2
	return 0;
}

