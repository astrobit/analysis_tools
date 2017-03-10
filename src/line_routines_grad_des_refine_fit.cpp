#include <line_routines.h>
#include <cfloat>
#include <unistd.h>

double g_dGrad_Des_Convergence_Threshold = 0.01;

double Set_Grad_Des_Convergence_Threshold(const double & i_dValue)
{
	return (g_dGrad_Des_Convergence_Threshold = i_dValue);
}
double Get_Grad_Des_Convergence_Threshold(void)
{
	return g_dGrad_Des_Convergence_Threshold;
}

void Grad_Des_Refine_Fit(
	unsigned int		&o_uiRefine_Result_ID,
	spectrum_data		&io_sdRefine_Result,
	const model *				i_lpmRefine_Model,
	spectrum_data		&o_sdRefine_Result_Curr,
	spectrum_data		&o_sdRefine_Result_Curr_Best,
	const double				&i_dStep_Size,
	bool				&i_bAbort_Request)
{
	ES::Spectrum cTarget;
	msdb::USER_PARAMETERS cParam;
	msdb::USER_PARAMETERS cParam_Curr;
	msdb::USER_PARAMETERS	cParam_Save;
	double dQuality_Save = DBL_MAX;
	opacity_profile_data::group eModel_Group = opacity_profile_data::carbon;
	bool bValid_Ion = false;

	o_uiRefine_Result_ID = 0;
	

	Refine_Prep(io_sdRefine_Result,i_lpmRefine_Model->m_uiModel_ID,cTarget,cParam,eModel_Group,bValid_Ion);
	double dNorm_Target = Get_Norm_Const(cTarget,io_sdRefine_Result.m_dNorm_WL_Blue,io_sdRefine_Result.m_dNorm_WL_Red);
	o_sdRefine_Result_Curr = io_sdRefine_Result;
	o_sdRefine_Result_Curr_Best = io_sdRefine_Result;

	unsigned int uiIdx = (unsigned int) (eModel_Group - opacity_profile_data::carbon);
	if (bValid_Ion)
	{


		double dQuality_Ref = 0.0;
		double dQuality_Last = 0.0;
		double dDeriv[4] = {0.0,0.0,0.0,0.0};
		double dMag_Deriv = 0.0;
		do
		{
			ES::Spectrum esResult;
			msdb_load_generate(cParam, msdb::COMBINED, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
			double dNorm_Gen = Get_Norm_Const(esResult,io_sdRefine_Result.m_dNorm_WL_Blue,io_sdRefine_Result.m_dNorm_WL_Red);
			double dSum_Err_2 = 0.0;
			unsigned int uiErr_Data_Count = 0;
			double dNorm = 0.0;
			for (unsigned int uiI = 0; uiI < esResult.size(); uiI++)
			{
				if (esResult.wl(uiI) >= io_sdRefine_Result.m_dFit_WL_Blue && esResult.wl(uiI) <= io_sdRefine_Result.m_dFit_WL_Red)
				{
					if ((cTarget.flux(uiI) / dNorm_Target) > dNorm)
						dNorm = cTarget.flux(uiI) / dNorm_Target;
				}
			}
			
			for (unsigned int uiI = 0; uiI < esResult.size(); uiI++)
			{
				if (esResult.wl(uiI) >= io_sdRefine_Result.m_dFit_WL_Blue && esResult.wl(uiI) <= io_sdRefine_Result.m_dFit_WL_Red)
				{
					double dErr = (esResult.flux(uiI) / dNorm_Gen - cTarget.flux(uiI) / dNorm_Target) / dNorm;
					dSum_Err_2 += dErr * dErr;
					uiErr_Data_Count++;
				}
			}
			dSum_Err_2 /= uiErr_Data_Count;
			dSum_Err_2 *= 0.5;

			o_sdRefine_Result_Curr_Best.m_dPS_Temp = cParam.m_dPhotosphere_Temp_kK;
			o_sdRefine_Result_Curr_Best.m_dPS_Velocity = cParam.m_dPhotosphere_Velocity_kkms;
			o_sdRefine_Result_Curr_Best.m_dEjecta_Scalar = cParam.m_dEjecta_Log_Scalar;
			o_sdRefine_Result_Curr_Best.m_dShell_Scalar = cParam.m_dShell_Log_Scalar;
			o_sdRefine_Result_Curr_Best.m_specResult[0] = esResult;
			o_sdRefine_Result_Curr_Best.m_dQuality_of_Fit = dSum_Err_2;
			o_sdRefine_Result_Curr = o_sdRefine_Result_Curr_Best;

			o_uiRefine_Result_ID++;
			dQuality_Ref = dSum_Err_2;

			for (unsigned int uiMode = 0; uiMode < 8 && !i_bAbort_Request; uiMode++)
			{
				cParam_Curr = cParam;
				switch (uiMode)
				{
				case 0:
					cParam_Curr.m_dPhotosphere_Temp_kK -= 0.01;
					break;
				case 1:
					cParam_Curr.m_dPhotosphere_Temp_kK += 0.01;
					break;
				case 2:
					cParam_Curr.m_dPhotosphere_Velocity_kkms -= 0.01;
					break;
				case 3:
					cParam_Curr.m_dPhotosphere_Velocity_kkms += 0.01;
					break;
				case 4:
					cParam_Curr.m_dEjecta_Log_Scalar -= 0.01;
					break;
				case 5:
					cParam_Curr.m_dEjecta_Log_Scalar += 0.01;
					break;
				case 6:
					cParam_Curr.m_dShell_Log_Scalar -= 0.01;
					break;
				case 7:
					cParam_Curr.m_dShell_Log_Scalar += 0.01;
					break;
				}

				msdb_load_generate(cParam_Curr, msdb::COMBINED, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
				dNorm_Gen = Get_Norm_Const(esResult,io_sdRefine_Result.m_dNorm_WL_Blue,io_sdRefine_Result.m_dNorm_WL_Red);
				dNorm = 0.0;
				for (unsigned int uiI = 0; uiI < esResult.size(); uiI++)
				{
					if (esResult.wl(uiI) >= io_sdRefine_Result.m_dFit_WL_Blue && esResult.wl(uiI) <= io_sdRefine_Result.m_dFit_WL_Red)
					{
						if ((cTarget.flux(uiI) / dNorm_Target) > dNorm)
							dNorm = cTarget.flux(uiI) / dNorm_Target;
					}
				}
				dSum_Err_2 = 0.0;
				uiErr_Data_Count = 0;
				for (unsigned int uiI = 0; uiI < esResult.size(); uiI++)
				{
					if (esResult.wl(uiI) >= io_sdRefine_Result.m_dFit_WL_Blue && esResult.wl(uiI) <= io_sdRefine_Result.m_dFit_WL_Red)
					{
						double dErr = (esResult.flux(uiI) / dNorm_Gen - cTarget.flux(uiI) / dNorm_Target) / dNorm;
						dSum_Err_2 += dErr * dErr;
						uiErr_Data_Count++;
					}
				}
				dSum_Err_2 /= uiErr_Data_Count;
				dSum_Err_2 *= 0.5;
				o_sdRefine_Result_Curr.m_dPS_Temp = cParam_Curr.m_dPhotosphere_Temp_kK;
				o_sdRefine_Result_Curr.m_dPS_Velocity = cParam_Curr.m_dPhotosphere_Velocity_kkms;
				o_sdRefine_Result_Curr.m_dEjecta_Scalar = cParam_Curr.m_dEjecta_Log_Scalar;
				o_sdRefine_Result_Curr.m_dShell_Scalar = cParam_Curr.m_dShell_Log_Scalar;
				o_sdRefine_Result_Curr.m_specResult[0] = esResult;
				o_sdRefine_Result_Curr.m_dQuality_of_Fit = dSum_Err_2;

				o_uiRefine_Result_ID++;
	//									std::cout << g_uiRefine_Result_ID << std::endl;
				sleep(1); // just in case the generate is already done, to give user a chance to see it.

				switch (uiMode)
				{
				case 0:
				case 2:
				case 4:
				case 6:
					dQuality_Last = dSum_Err_2;
					break;
				case 1:
				case 3:
					dDeriv[(uiMode - 1) / 2] = (dSum_Err_2 - dQuality_Last) / 0.02;
					break;
				case 5:
				case 7:
					dDeriv[(uiMode - 1) / 2] = (dSum_Err_2 - dQuality_Last) / 0.02;
					break;
				}
			}
			if (!i_bAbort_Request)
			{
				dMag_Deriv = sqrt(dDeriv[0] * dDeriv[0] + dDeriv[1] * dDeriv[1] + dDeriv[2] * dDeriv[2] + dDeriv[3] * dDeriv[3]);
				dMag_Deriv /= dSum_Err_2;
				printf("%.1e %.1e %.1e %.1e\n",dDeriv[0],dDeriv[1],dDeriv[2],dDeriv[3]);
				cParam.m_dPhotosphere_Temp_kK -= i_dStep_Size * dDeriv[0];
				cParam.m_dPhotosphere_Velocity_kkms -= i_dStep_Size * dDeriv[1];
				cParam.m_dEjecta_Log_Scalar -= i_dStep_Size * dDeriv[2];
				cParam.m_dShell_Log_Scalar -= i_dStep_Size * dDeriv[3];
			}
		} while (fabs(dMag_Deriv) > 0.01 && !i_bAbort_Request);
	}
	if (!i_bAbort_Request)
	{
		if (bValid_Ion)
		{
			ES::Spectrum esResult;
			msdb_load_generate(cParam, msdb::EJECTA_ONLY, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
			o_sdRefine_Result_Curr_Best.m_specResult[1] = esResult;
			msdb_load_generate(cParam, msdb::SHELL_ONLY, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
			o_sdRefine_Result_Curr_Best.m_specResult[2] = esResult;
			io_sdRefine_Result = o_sdRefine_Result_Curr_Best;
		}
	}
	else
	{
		io_sdRefine_Result.m_bValid  = false;
		o_sdRefine_Result_Curr.m_bValid  = false;
		o_sdRefine_Result_Curr_Best.m_bValid  = false;
	}
}
