#include <xio.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	XDATASET	cCombined_Results;

	XDATASET	cDatafile;
	char lpszHeader[256];
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		printf("Reading %s\n",i_lpszArg_Values[uiI]);
		cDatafile.ReadDataFile(i_lpszArg_Values[uiI],false,false,',');
		if (uiI == 1)
		{
			cCombined_Results.Allocate(2*cDatafile.GetNumElements() + 1,i_iArg_Count - 1);
			strcpy(lpszHeader,"Day");
			for (unsigned int uiJ = 0; uiJ < cDatafile.GetNumElements(); uiJ++)
			{
				char lpszTemp[32];
				sprintf(lpszTemp,", PVF (%.0f), HVF (%.0f)",cDatafile.GetElement(0,uiJ),cDatafile.GetElement(0,uiJ));
				strcat(lpszHeader,lpszTemp);
			}
			strcat(lpszHeader,"\n");
			cCombined_Results.SetHeaderLine(lpszHeader);
		}
		const char * lpszCursor = strstr(i_lpszArg_Values[uiI],".eps");
		lpszCursor--;
		while (lpszCursor > i_lpszArg_Values[uiI] && lpszCursor[0] != 'd')
			lpszCursor--;
		lpszCursor++;
		double dDay = atof(lpszCursor);

		cCombined_Results.SetElement(0,uiI - 1,dDay);
		for (unsigned int uiJ = 0; uiJ < cDatafile.GetNumElements(); uiJ++)
		{
			double dPVF = 0.0, dHVF = 0.0;
			if (cDatafile.GetElement(3,uiJ) > 1.0)
				dPVF = cDatafile.GetElement(4,uiJ);
			if (cDatafile.GetElement(5,uiJ) > 1.0)
				dHVF = cDatafile.GetElement(6,uiJ);

			cCombined_Results.SetElement(1 + uiJ * 2,uiI - 1,dPVF);
			cCombined_Results.SetElement(2 + uiJ * 2,uiI - 1,dHVF);
		}
	}
	cCombined_Results.SaveDataFileCSV("vel_plot.csv",NULL);
	return 0;
}
