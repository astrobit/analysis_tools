#include <best_fit_data.h>
#include <xstdlib.h>

void Report_Change(const double & i_dValue, const char * i_lpszDescription)
{
	if (fabs(i_dValue) > 0.0)
		printf("Changing %s by %f.\n",i_lpszDescription,i_dValue);
}

void Report_Set(const double & i_dValue, const char * i_lpszDescription)
{
	if (!isnan(i_dValue))
		printf("Setting %s to %f.\n",i_lpszDescription,i_dValue);
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	double	dNAN = nan("");
	double	dDay_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--Day-delta",0.0);
	double	dPS_v_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel-delta",0.0);
	double	dPS_T_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-temp-delta",0.0);
	double	dPVF_C_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PVF-scale-delta",0.0);
	double	dPVF_T_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PVF-temp-delta",0.0);
	double	dHVF_C_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-scale-delta",0.0);
	double	dHVF_T_delta = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-temp-delta",0.0);
	double	dDay_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--day-set",dNAN);
	double	dPS_v_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-vel-set",dNAN);
	double	dPS_T_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PS-temp-set",dNAN);
	double	dPVF_C_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PVF-scale-set",dNAN);
	double	dPVF_T_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--PVF-temp-set",dNAN);
	double	dHVF_C_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-scale-set",dNAN);
	double	dHVF_T_set = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--HVF-temp-set",dNAN);

	char	lpszModel_set[64] = {""};
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--model-set",lpszModel_set,sizeof(lpszModel_set),"");

	Report_Change(dDay_delta,"day");
	Report_Change(dPS_v_delta,"PS velocity");
	Report_Change(dPS_T_delta,"PS temperature");
	Report_Change(dPVF_C_delta,"PVF scalar");
	Report_Change(dPVF_T_delta,"PVF temperature");
	Report_Change(dHVF_C_delta,"HVF scalar");
	Report_Change(dHVF_T_delta,"HVF temperature");

	Report_Set(dDay_set,"day");
	Report_Set(dPS_v_set,"PS velocity");
	Report_Set(dPS_T_set,"PS temperature");
	Report_Set(dPVF_C_set,"PVF scalar");
	Report_Set(dPVF_T_set,"PVF temperature");
	Report_Set(dHVF_C_set,"HVF scalar");
	Report_Set(dHVF_T_set,"HVF temperature");

	if (lpszModel_set[0] != 0)
		printf("Setting model to %s.\n",lpszModel_set);

	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		if (i_lpszArg_Values[uiI][0] != '-')
		{
			BEST_FIT_DATA	cFit;
			printf("Reading file %s.\n",i_lpszArg_Values[uiI]);
			if (strstr(i_lpszArg_Values[uiI],"databin"))
				cFit.ReadBinary(i_lpszArg_Values[uiI]);
			else
				cFit.ReadFromFile(i_lpszArg_Values[uiI]);
			printf("Making requested changes.\n");

			if (lpszModel_set[0] != 0)
				cFit.SetBestFitFile(lpszModel_set);

			if (!isnan(dPVF_T_set))
				cFit.dBest_Fit_PS_Ion_Temp = dPVF_T_set;
			else
				cFit.dBest_Fit_PS_Ion_Temp += dPVF_T_delta;

			if (!isnan(dPS_T_set))
				cFit.dBest_Fit_PS_Temp = dPS_T_set;
			else
				cFit.dBest_Fit_PS_Temp += dPS_T_delta;

			if (!isnan(dDay_set))
				cFit.dBest_Fit_Day = dDay_set;
			else
				cFit.dBest_Fit_Day += dDay_delta;

			if (!isnan(dPVF_C_set))
				cFit.dBest_Fit_PS_Log_Tau = dPVF_C_set;
			else
				cFit.dBest_Fit_PS_Log_Tau += dPVF_C_delta;

			if (!isnan(dHVF_T_set))
				cFit.dBest_Fit_HVF_Ion_Temp = dHVF_T_set;
			else
				cFit.dBest_Fit_HVF_Ion_Temp += dHVF_T_delta;

			if (!isnan(dPS_v_set))
				cFit.dBest_Fit_PS_Vel = dPS_v_set;
			else
				cFit.dBest_Fit_PS_Vel += dPS_v_delta;

			if (!isnan(dHVF_C_set))
				cFit.dBest_Fit_HVF_Log_Tau = dHVF_C_set;
			else
				cFit.dBest_Fit_HVF_Log_Tau += dHVF_C_delta;

			printf("Outputting file %s.\n",i_lpszArg_Values[uiI]);
			if (strstr(i_lpszArg_Values[uiI],"databin"))
				cFit.OutputBinary(i_lpszArg_Values[uiI]);
			else
				cFit.Output(i_lpszArg_Values[uiI],false);
		}
	}
}
