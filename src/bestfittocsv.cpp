#include <best_fit_data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cBest_Fit;

	FILE * fileOut = fopen("bestfits.csv","wt");

	fprintf(fileOut,"Best Fit file, model, Shell?, Fit, Day, PS Velocity, PS Temp, PS log tau, PS ion temp, HVF log tau, HVF ion temp, PS ion Vmin, PS ion Vmax, PS ion aux, HVF ion Vmin, HVF ion Vmax, HVF ion aux\n");
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		cBest_Fit.ReadFromFile(i_lpszArg_Values[uiI]);
		fprintf(fileOut,"%s, %s, %s, %.17e, %.2f, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",
			i_lpszArg_Values[uiI],
			cBest_Fit.lpszBest_Fit_File,
			cBest_Fit.bBest_Fit_With_Shell ? "Shell" : "No Shell",
			cBest_Fit.dBest_Fit,
			cBest_Fit.dBest_Fit_Day,
			cBest_Fit.dBest_Fit_PS_Vel,
			cBest_Fit.dBest_Fit_PS_Temp,
			cBest_Fit.dBest_Fit_PS_Log_Tau,
			cBest_Fit.dBest_Fit_PS_Ion_Temp,
			cBest_Fit.dBest_Fit_HVF_Log_Tau,
			cBest_Fit.dBest_Fit_HVF_Ion_Temp,
			cBest_Fit.dBest_Fit_PS_Ion_Vmin,
			cBest_Fit.dBest_Fit_PS_Ion_Vmax,
			cBest_Fit.dBest_Fit_PS_Ion_Aux,
			cBest_Fit.dBest_Fit_HVF_Ion_Vmin,
			cBest_Fit.dBest_Fit_HVF_Ion_Vmax,
			cBest_Fit.dBest_Fit_HVF_Ion_Aux);
	}
	fclose(fileOut);

	return 0;
}
