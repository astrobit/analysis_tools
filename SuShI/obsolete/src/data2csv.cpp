
#include <best_fit_data.h>

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit_Data;
	char	lpszFilename[256];

	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		strcpy(lpszFilename,i_lpszArg_Values[uiI]);
		cFit_Data.ReadFromFile(lpszFilename);
		fprintf(stdout,"%s: ",lpszFilename);
		fflush(stdout);
		sprintf(lpszFilename,"%s.csv",i_lpszArg_Values[uiI]);
		FILE * fileOut = fopen(lpszFilename,"wt");
		fprintf(fileOut,"%s, %.2f, %.2f, %.4f, %.4f, %.4f, %.4f, %.17e, %.17e, %.17e, %.4f, %.4f",lpszModel,cFit_Data.dSource_Data_MJD, cFit_Data.dBest_Fit_Day, cFit_Data.dBest_Fit_PS_Vel, cFit_Data.dBest_Fit_PS_Temp, cFit_Data.dBest_Fit_PS_Log_Tau, cFit_Data.dBest_Fit_HVF_Log_Tau, cFit_Data.m_dPS_Absorption_Peak_Velocity, cFit_Data.m_dHVF_Absorption_Peak_Velocity);
		for (unsigned int uiI = 0; uiI < cFit_Data.GetNumMoments(); uiI++)
		{
			double dRaw,dCentral,dStandardized;
			cFit_Data.GetMoments(uiI + 1,dRaw,dCentral,dStandardized);
			fprintf(fileOut,", %.17e, %.17e, %.17e",dRaw,dCentral,dStandardized);
		}
		fprintf(fileOut,"\n");
		fclose(fileOut);
		fprintf(stdout,"output to %s\n",lpszFilename);
	}


	return 0;
}

