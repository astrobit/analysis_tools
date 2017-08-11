#include <line_routines.h>
#include <cfloat>
#include <unistd.h>
#include <iomanip>
#include <fstream>
#include <limits>

////////////////////////////////////////////////////////////////////////////////
//
// Grid_Refine_Fit
//
// Fits a user specified target spectrum with a given normalization range
// and fit range. 
//
// this function is designed as thread safe, and updates the curr and curr_best
// spectrum data as the refinement proceeds. o_uiRefine_Result_ID is an integer
// indicating what step the refinement is at (of o_uiRefine_Max total steps)
// when o_uiRefine_Result_ID changes value, curr and curr_best may be safely read
// though if the reading thread is slow compared to the refinement thread there
// is still risk of thrashing.
//
// If the user wishes to abort the refinement in the middle, i_bAbort_Request should
// be signalled. This value is essentially ignored in serial operation (but not if the user flags it upon function entry
//
//
// Final result is in io_sdRefine_Result.
//
////////////////////////////////////////////////////////////////////////////////

// maybe not the most straightforward way to initialize an xvector, but it works
xvector g_xvRefine_Params(std::vector<double>(std::initializer_list<double>({5.0,2.5,0.4,0.4})));
unsigned int g_uiRef_Level_Max = 9;

void Set_Grid_Refine_Level(unsigned int i_uiRefine_Level_Max)
{
	g_uiRef_Level_Max = i_uiRefine_Level_Max;
}

void Set_Grid_Refine_Size(const xvector & i_vVector)
{
	if (i_vVector.size() == 4)
		g_xvRefine_Params = i_vVector;
}


void Set_Grid_Refine_Parameter(grid_refine_parameter i_eParameter, const double & i_dValue)
{
	unsigned int uiIdx = i_eParameter - grp_ps_temp;
	if (uiIdx < 4)
		g_xvRefine_Params.Set(uiIdx,i_dValue);
}


void Grid_Refine_Fit(
	unsigned int		&o_uiRefine_Result_ID,
	spectrum_data		&io_sdRefine_Result,
	const model *				i_lpmRefine_Model,
	spectrum_data		&o_sdRefine_Result_Curr,
	spectrum_data		&o_sdRefine_Result_Curr_Best,
	unsigned int		&o_uiRefine_Max,
	bool				&i_bAbort_Request,
	refine_options		*i_lpRefine_Options)
{
	o_uiRefine_Max = 3 * 3 * 3 * 3 * g_uiRef_Level_Max;
	ES::Spectrum cTarget;
	msdb::USER_PARAMETERS cParam;
	msdb::USER_PARAMETERS cParam_Curr;
	msdb::USER_PARAMETERS	cParam_Save;
	double dQuality_Save = DBL_MAX;
	opacity_profile_data::group eModel_Group = opacity_profile_data::carbon;
	bool bValid_Ion = false;
	refine_options		cRefine_Options;
	if (i_lpRefine_Options != nullptr)
		cRefine_Options = *i_lpRefine_Options;

	o_uiRefine_Result_ID = 0;
	

	xvector xvRefine_Params = g_xvRefine_Params;
	if (cRefine_Options.m_bAppend_Report_To_File)
	{
		std::ofstream ofFile;
		ofFile.open(cRefine_Options.m_sFilename.c_str(), std::ofstream::out);
		ofFile << "Step, Refine Level, Temp, Vel, Se, Ss, Q" << std::endl;
		ofFile.close();
	}

	Refine_Prep(io_sdRefine_Result,i_lpmRefine_Model->m_uiModel_ID,cTarget,cParam,eModel_Group,bValid_Ion);
	double dNorm_Target = Get_Norm_Const(cTarget,io_sdRefine_Result.m_dNorm_WL_Blue,io_sdRefine_Result.m_dNorm_WL_Red);
	o_sdRefine_Result_Curr = io_sdRefine_Result;
	o_sdRefine_Result_Curr_Best = io_sdRefine_Result;

	unsigned int uiIdx = (unsigned int) (eModel_Group - opacity_profile_data::carbon);
	if (bValid_Ion)
	{
		if (cParam.m_dPhotosphere_Temp_kK < xvRefine_Params.Get(0) * 1.125)
			cParam.m_dPhotosphere_Temp_kK = xvRefine_Params.Get(0) * 1.125;
		if (cParam.m_dPhotosphere_Velocity_kkms < xvRefine_Params.Get(1) * 1.125)
			cParam.m_dPhotosphere_Velocity_kkms = xvRefine_Params.Get(1) * 1.125;

		for (unsigned int uiRef_Level = 0; uiRef_Level < g_uiRef_Level_Max && !i_bAbort_Request; uiRef_Level++)
		{
			for (int iTemp = -1; iTemp < 2 && !i_bAbort_Request; iTemp++)
				for (int iVel = -1; iVel < 2 && !i_bAbort_Request; iVel++)
					for (int iSs = -1; iSs < 2 && !i_bAbort_Request; iSs++)
						for (int iSe = -1; iSe < 2 && !i_bAbort_Request; iSe++)
						{
							cParam_Curr = cParam;
							ES::Spectrum esResult;
							cParam_Curr.m_dPhotosphere_Temp_kK += iTemp * xvRefine_Params.Get(0);
							cParam_Curr.m_dPhotosphere_Velocity_kkms += iVel * xvRefine_Params.Get(1);
							cParam_Curr.m_dEjecta_Log_Scalar += iSs * xvRefine_Params.Get(2);
							cParam_Curr.m_dShell_Log_Scalar += iSe * xvRefine_Params.Get(3);
							msdb_load_generate(cParam_Curr, msdb::COMBINED, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
							double dNorm_Gen = Get_Norm_Const(esResult,io_sdRefine_Result.m_dNorm_WL_Blue,io_sdRefine_Result.m_dNorm_WL_Red);
							double dSum_Err_2 = 0.0;
							unsigned int uiErr_Data_Count = 0;
							double dNorm = 0.0;

							for (unsigned int uiI = 0; uiI < cTarget.size(); uiI++)
							{
								if (!isnan(cTarget.flux(uiI)) && !isinf(cTarget.flux(uiI)))
								{
									if (cTarget.wl(uiI) >= io_sdRefine_Result.m_dFit_WL_Blue && cTarget.wl(uiI) <= io_sdRefine_Result.m_dFit_WL_Red)
									{
										if ((cTarget.flux(uiI) / dNorm_Target) > dNorm)
											dNorm = cTarget.flux(uiI) / dNorm_Target;
									}
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
							if (uiErr_Data_Count != 0)
							{
								dSum_Err_2 *= 0.5;
								dSum_Err_2 /= uiErr_Data_Count;
							}
							else
								std::cerr << "No quality data" << std::endl;
							if (dSum_Err_2 < dQuality_Save)
							{
								dQuality_Save = dSum_Err_2;
								cParam_Save = cParam_Curr;
								o_sdRefine_Result_Curr_Best.m_dPS_Temp = cParam_Curr.m_dPhotosphere_Temp_kK;
								o_sdRefine_Result_Curr_Best.m_dPS_Velocity = cParam_Curr.m_dPhotosphere_Velocity_kkms;
								o_sdRefine_Result_Curr_Best.m_dEjecta_Scalar = cParam_Curr.m_dEjecta_Log_Scalar;
								o_sdRefine_Result_Curr_Best.m_dShell_Scalar = cParam_Curr.m_dShell_Log_Scalar;
								o_sdRefine_Result_Curr_Best.m_specResult[0] = esResult;
								o_sdRefine_Result_Curr_Best.m_dQuality_of_Fit = dSum_Err_2;
							}
							o_sdRefine_Result_Curr.m_dPS_Temp = cParam_Curr.m_dPhotosphere_Temp_kK;
							o_sdRefine_Result_Curr.m_dPS_Velocity = cParam_Curr.m_dPhotosphere_Velocity_kkms;
							o_sdRefine_Result_Curr.m_dEjecta_Scalar = cParam_Curr.m_dEjecta_Log_Scalar;
							o_sdRefine_Result_Curr.m_dShell_Scalar = cParam_Curr.m_dShell_Log_Scalar;
							o_sdRefine_Result_Curr.m_specResult[0] = esResult;
							o_sdRefine_Result_Curr.m_dQuality_of_Fit = dSum_Err_2;

							o_uiRefine_Result_ID++;
							if (cRefine_Options.m_bReport_To_Console)
							{
								std::cout << "Step " << o_uiRefine_Result_ID;
								std::cout << " Temp  " << o_sdRefine_Result_Curr.m_dPS_Temp;
								std::cout << " Vel " << o_sdRefine_Result_Curr.m_dPS_Velocity;
								std::cout << " Se " << o_sdRefine_Result_Curr.m_dEjecta_Scalar;
								std::cout << " Ss " << o_sdRefine_Result_Curr.m_dShell_Scalar;
								std::cout << " Q " << o_sdRefine_Result_Curr.m_dQuality_of_Fit << std::endl;
							}
							if (cRefine_Options.m_bAppend_Report_To_File)
							{
								std::ofstream ofFile;
								ofFile.open(cRefine_Options.m_sFilename.c_str(), std::ofstream::out | std::ofstream::app);
								ofFile << o_uiRefine_Result_ID << ", " << uiRef_Level;
								ofFile << ",  " << o_sdRefine_Result_Curr.m_dPS_Temp;
								ofFile << ", " << o_sdRefine_Result_Curr.m_dPS_Velocity;
								ofFile << ", " << o_sdRefine_Result_Curr.m_dEjecta_Scalar;
								ofFile << ", " << o_sdRefine_Result_Curr.m_dShell_Scalar;
								ofFile << ", " << std::scientific << std::setprecision(std::numeric_limits<double>::digits10 + 1) << o_sdRefine_Result_Curr.m_dQuality_of_Fit << std::endl;
								ofFile.close();
							}
//									std::cout << o_uiRefine_Result_ID << std::endl;
							if (cRefine_Options.m_bSleep_Each_Step)
								sleep(1); // just in case the generate is already done, to give user a chance to see it.
							
						}
			cParam = cParam_Save;
			xvRefine_Params *= 0.5;
		}
	}
	
	if (!i_bAbort_Request)
	{
		if (bValid_Ion)
		{
			ES::Spectrum esResult;
			msdb_load_generate(cParam_Save, msdb::EJECTA_ONLY, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
			o_sdRefine_Result_Curr_Best.m_specResult[1] = esResult;
			msdb_load_generate(cParam_Save, msdb::SHELL_ONLY, cTarget, &i_lpmRefine_Model->m_dsEjecta[uiIdx], &i_lpmRefine_Model->m_dsShell, esResult);
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

