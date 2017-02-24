#include <OSCIn-SuShI>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cfloat>

void OSCIn_SuShI_main::Process_Selected_Spectrum(void)
{
	m_idSelected_ID = m_iterSelected_ID->first;
	m_specSelected_Spectrum = m_iterSelected_ID->second;
	memset(m_dSelected_Flux_Max,0,sizeof(m_dSelected_Flux_Max));
	for (auto iterI = m_specSelected_Spectrum.begin(); iterI != m_specSelected_Spectrum.end(); iterI++)
	{
		if (iterI->m_dWavelength >= 3000.0 && iterI->m_dWavelength <= 5000.0)
		{
			if (iterI->m_dFlux > m_dSelected_Flux_Max[1])
				m_dSelected_Flux_Max[1] = iterI->m_dFlux;
		}
		else if (iterI->m_dWavelength >= 5000.0 && iterI->m_dWavelength <= 7000.0)
		{
			if (iterI->m_dFlux > m_dSelected_Flux_Max[2])
				m_dSelected_Flux_Max[2] = iterI->m_dFlux;
		}
		else if (iterI->m_dWavelength >= 7000.0 && iterI->m_dWavelength <= 9000.0)
		{
			if (iterI->m_dFlux > m_dSelected_Flux_Max[3])
				m_dSelected_Flux_Max[3] = iterI->m_dFlux;
		}
		if (iterI->m_dFlux > m_dSelected_Flux_Max[0])
			m_dSelected_Flux_Max[0] = iterI->m_dFlux;
	}
	Normalize();
	Calc_Observed_pEW();
}

double Get_Norm_Const(const OSCspectrum & i_cTarget, const double & i_dNorm_Blue, const double & i_dNorm_Red)
{
	double dTgt_Norm = 0.0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		if (i_cTarget.wl(uiI) >= i_dNorm_Blue && i_cTarget.wl(uiI) <= i_dNorm_Red)
			dTgt_Norm += i_cTarget.flux(uiI);
	}
	return dTgt_Norm;
}
double Get_Norm_Const(const ES::Spectrum & i_cTarget, const double & i_dNorm_Blue, const double & i_dNorm_Red)
{
	double dTgt_Norm = 0.0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		if (i_cTarget.wl(uiI) >= i_dNorm_Blue && i_cTarget.wl(uiI) <= i_dNorm_Red)
			dTgt_Norm += i_cTarget.flux(uiI);
	}
	return dTgt_Norm;
}


void OSCIn_SuShI_main::Normalize(void)
{
	double dTgt_Norm = Get_Norm_Const(m_specSelected_Spectrum,m_dNorm_Blue, m_dNorm_Red);
	if (m_sdGenerated_Spectrum.m_bValid)
	{
		double dGen_Norm = Get_Norm_Const(m_sdGenerated_Spectrum.m_specResult,m_dNorm_Blue, m_dNorm_Red);
		m_dGenerated_Spectrum_Norm = dTgt_Norm / dGen_Norm;
	}
	if (m_sdGenerated_Spectrum_Prev.m_bValid)
	{
		double dGen_Norm_Prev = Get_Norm_Const(m_sdGenerated_Spectrum_Prev.m_specResult,m_dNorm_Blue, m_dNorm_Red);
		m_dGenerated_Spectrum_Prev_Norm = dTgt_Norm / dGen_Norm_Prev;
	}
	if (m_sdRefine_Spectrum_Curr.m_bValid)
	{
		double dGen_Norm = Get_Norm_Const(m_sdRefine_Spectrum_Curr.m_specResult,m_dNorm_Blue, m_dNorm_Red);
		m_dRefine_Spectrum_Norm = dTgt_Norm / dGen_Norm;
	}
	if (m_sdRefine_Spectrum_Best.m_bValid)
	{
		double dGen_Norm = Get_Norm_Const(m_sdRefine_Spectrum_Best.m_specResult,m_dNorm_Blue, m_dNorm_Red);
		m_dRefine_Spectrum_Best_Norm = dTgt_Norm / dGen_Norm;
	}

}

//------------------------------------------------------------------------------
//
//
//
//	Add_Model_To_List
//
//
//
//------------------------------------------------------------------------------
void model::Load_Model(unsigned int i_uiModel)
{
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
	std::string szLA_Data_Path;
	if (lpszLA_Data_Path)
		szLA_Data_Path = lpszLA_Data_Path;
//	else
//		std::cerr << "Warning: LINE_ANALYSIS_DATA_PATH not specified. Probably won't find model data." << std::endl;
	std::ostringstream ossFile_Path;
	ossFile_Path << szLA_Data_Path;
	ossFile_Path << "/models/";
	ossFile_Path <<  i_uiModel;

	DIR* dir = opendir(ossFile_Path.str().c_str());
	if (dir == nullptr)
	{
		std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Unable to find path " << ossFile_Path.str() << std::endl;
		/* Directory exists. */
	}
	else
	{
		closedir(dir);
		ossFile_Path <<  "/";
		std::ostringstream ossOpM_C;
		std::ostringstream ossOpM_O;
		std::ostringstream ossOpM_Mg;
		std::ostringstream ossOpM_Si;
		std::ostringstream ossOpM_Fe;
		std::ostringstream ossOpM_Shell;
		std::ostringstream ossPhoto;
		std::ostringstream ossPhoto_EO;
		std::ostringstream ossOp_Data;

		ossOpM_C << ossFile_Path.str() << "opacity_map_ejecta.C.xdataset";
		ossOpM_O << ossFile_Path.str() << "opacity_map_ejecta.O.xdataset";
		ossOpM_Mg << ossFile_Path.str() << "opacity_map_ejecta.Mg.xdataset";
		ossOpM_Si << ossFile_Path.str() << "opacity_map_ejecta.Si.xdataset";
		ossOpM_Fe << ossFile_Path.str() << "opacity_map_ejecta.Fe.xdataset";
		ossOpM_Shell << ossFile_Path.str() << "opacity_map_shell.xdataset";
		ossPhoto << ossFile_Path.str() << "photosphere.csv";
		ossPhoto_EO << ossFile_Path.str() << "photosphere_eo.csv";
		ossOp_Data << ossFile_Path.str() << "opacity_map_scalars.opdata";

		m_uiModel_ID = i_uiModel;
		m_dsEjecta[opacity_profile_data::carbon].ReadDataFileBin(ossOpM_C.str().c_str(),true);
		m_dsEjecta[opacity_profile_data::oxygen].ReadDataFileBin(ossOpM_O.str().c_str(),true);
		m_dsEjecta[opacity_profile_data::magnesium].ReadDataFileBin(ossOpM_Mg.str().c_str(),true);
		m_dsEjecta[opacity_profile_data::silicon].ReadDataFileBin(ossOpM_Si.str().c_str(),true);
		m_dsEjecta[opacity_profile_data::iron].ReadDataFileBin(ossOpM_Fe.str().c_str(),true);
		m_dsShell.ReadDataFileBin(ossOpM_Shell.str().c_str(),true);
		m_opdOp_Profile_Data.Load(ossOp_Data.str().c_str());

		if (m_dsEjecta[opacity_profile_data::silicon].GetNumRows() == 0)
			std::cerr << "Failed to load data for model " << i_uiModel << std::endl;

		m_dsPhotosphere.ReadDataFile(ossPhoto.str().c_str(),false,false,',',1);
		m_dsEjecta_Photosphere.ReadDataFile(ossPhoto_EO.str().c_str(),false,false,',',1);
	}
}

bool				g_bAbort_Request = false;
bool				g_bQuit_Thread = false;
bool 				g_bGen_Process_Request = false;
bool				g_bGen_In_Progress = false;
bool				g_bGen_Done = false;
bool				g_bGen_Thread_Running = false;
spectrum_data		g_sdGen_Spectrum_Result;
model *				g_lpmGen_Model = nullptr;

void Prep(const spectrum_data & i_cData, unsigned int i_uiModel_ID, ES::Spectrum & o_cTarget, msdb::USER_PARAMETERS &o_cUser_Param, opacity_profile_data::group & o_eModel_Group, bool & o_bIon_Valid)
{
	o_cUser_Param.m_uiModel_ID = i_uiModel_ID;
	o_cUser_Param.m_uiIon = i_cData.m_uiIon;
	o_cUser_Param.m_eFeature = msdb::CaNIR;
	o_cUser_Param.m_dTime_After_Explosion = 1.0;
	o_cUser_Param.m_dPhotosphere_Velocity_kkms = i_cData.m_dPS_Velocity;
	o_cUser_Param.m_dPhotosphere_Temp_kK = i_cData.m_dPS_Temp;
	o_cUser_Param.m_dEjecta_Log_Scalar = i_cData.m_dEjecta_Scalar;
	o_cUser_Param.m_dShell_Log_Scalar = i_cData.m_dShell_Scalar;
	o_cUser_Param.m_dEjecta_Effective_Temperature_kK = i_cData.m_dExc_Temp;
	o_cUser_Param.m_dShell_Effective_Temperature_kK = i_cData.m_dExc_Temp;
	o_cUser_Param.m_dWavelength_Range_Lower_Ang = i_cData.m_specResult.wl(0);
	o_cUser_Param.m_dWavelength_Range_Upper_Ang = i_cData.m_specResult.wl(i_cData.m_specResult.size() - 1);
	o_cUser_Param.m_dWavelength_Delta_Ang = i_cData.m_specResult.wl(1) - i_cData.m_specResult.wl(0);
	o_cUser_Param.m_dEjecta_Scalar_Time_Power_Law = -2.0;
	o_cUser_Param.m_dShell_Scalar_Time_Power_Law = -2.0;

	unsigned int uiElem = o_cUser_Param.m_uiIon / 100;
	o_bIon_Valid = false;
	switch (uiElem)
	{
	case 6:
		o_eModel_Group = opacity_profile_data::carbon;
		o_bIon_Valid = true;
		break;
	case 8:
		o_eModel_Group = opacity_profile_data::magnesium;
		o_bIon_Valid = true;
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		o_eModel_Group = opacity_profile_data::magnesium;
		o_bIon_Valid = true;
		break;
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		o_eModel_Group = opacity_profile_data::silicon;
		o_bIon_Valid = true;
		break;
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
		o_eModel_Group = opacity_profile_data::iron;
		o_bIon_Valid = true;
		break;
	}


	o_cTarget = i_cData.m_specResult;
}
void Process_Generate_Requests(void)
{
	g_bGen_Thread_Running = true;
	do
	{
		if (g_bGen_Process_Request && g_sdGen_Spectrum_Result.m_bValid)
		{
			ES::Spectrum cTarget;
			msdb::USER_PARAMETERS cParam;
			opacity_profile_data::group eModel_Group = opacity_profile_data::carbon;
			bool bValid_Ion = false;

			g_bGen_Done = false;
			g_bGen_In_Progress = true;
			g_bGen_Process_Request = false;
			

			Prep(g_sdGen_Spectrum_Result,g_lpmGen_Model->m_uiModel_ID,cTarget,cParam,eModel_Group,bValid_Ion);

			g_sdGen_Spectrum_Result.m_specResult.zero_flux();
			if (bValid_Ion)
			{
				unsigned int uiIdx = (unsigned int) (eModel_Group - opacity_profile_data::carbon);
				msdb_load_generate(cParam, msdb::COMBINED, cTarget, &g_lpmGen_Model->m_dsEjecta[uiIdx], &g_lpmGen_Model->m_dsShell, g_sdGen_Spectrum_Result.m_specResult);
				double dNorm_Target = Get_Norm_Const(cTarget,g_sdGen_Spectrum_Result.m_dNorm_WL_Blue,g_sdGen_Spectrum_Result.m_dNorm_WL_Red);
				double dNorm_Gen = Get_Norm_Const(g_sdGen_Spectrum_Result.m_specResult,g_sdGen_Spectrum_Result.m_dNorm_WL_Blue,g_sdGen_Spectrum_Result.m_dNorm_WL_Red);
				double dNorm = dNorm_Target / dNorm_Gen;
				double dSum_Err_2 = 0.0;
				unsigned int uiErr_Data_Count = 0;
				for (unsigned int uiI = 0; uiI < g_sdGen_Spectrum_Result.m_specResult.size(); uiI++)
				{
					if (g_sdGen_Spectrum_Result.m_specResult.wl(uiI) >= g_sdGen_Spectrum_Result.m_dFit_WL_Blue && g_sdGen_Spectrum_Result.m_specResult.wl(uiI) <= g_sdGen_Spectrum_Result.m_dFit_WL_Red)
					{
						double dErr = g_sdGen_Spectrum_Result.m_specResult.flux(uiI) * dNorm - cTarget.flux(uiI);
						dSum_Err_2 += dErr * dErr;
						uiErr_Data_Count++;
					}
				}
				dSum_Err_2 /= uiErr_Data_Count;

				g_sdGen_Spectrum_Result.m_dQuality_of_Fit = dSum_Err_2;
				

			}
			g_bGen_Done = true;
			g_bGen_In_Progress = false;
		}
		usleep(10); // hand process back to the OS for a bit to reduce CPU load
	} while (!g_bQuit_Thread);
	g_bGen_Thread_Running = false;
}

bool				g_bRefine_Process_Request = false;
bool				g_bRefine_In_Progress = false;
bool				g_bRefine_Done = false;
bool				g_bRefine_Thread_Running = false;
unsigned int		g_uiRefine_Result_ID = 0;
spectrum_data		g_sdRefine_Result;
model *				g_lpmRefine_Model = nullptr;
spectrum_data		g_sdRefine_Result_Curr;
spectrum_data		g_sdRefine_Result_Curr_Best;

void Process_Refine_Requests(void)
{
	g_bRefine_Thread_Running = true;
	do
	{
		if (g_bRefine_Process_Request && g_sdRefine_Result.m_bValid)
		{
			ES::Spectrum cTarget;
			msdb::USER_PARAMETERS cParam;
			msdb::USER_PARAMETERS cParam_Curr;
			msdb::USER_PARAMETERS	cParam_Save;
			double dQuality_Save = DBL_MAX;
			opacity_profile_data::group eModel_Group = opacity_profile_data::carbon;
			bool bValid_Ion = false;

			g_bRefine_Done = false;
			g_bRefine_In_Progress = true;
			g_bRefine_Process_Request = false;
			g_uiRefine_Result_ID = 0;
			
			xvector xvRefine_Params(4);
			xvRefine_Params.Set(0,5.0);
			xvRefine_Params.Set(1,2.0);
			xvRefine_Params.Set(2,0.3);
			xvRefine_Params.Set(3,0.3);
			
			Prep(g_sdRefine_Result,g_lpmGen_Model->m_uiModel_ID,cTarget,cParam,eModel_Group,bValid_Ion);
			double dNorm_Target = Get_Norm_Const(cTarget,g_sdRefine_Result.m_dNorm_WL_Blue,g_sdRefine_Result.m_dNorm_WL_Red);
			g_sdRefine_Result_Curr = g_sdRefine_Result;
			g_sdRefine_Result_Curr_Best = g_sdRefine_Result;

			if (bValid_Ion)
			{
				if (cParam.m_dPhotosphere_Temp_kK < xvRefine_Params.Get(0))
					cParam.m_dPhotosphere_Temp_kK = xvRefine_Params.Get(0);

				for (unsigned int uiRef_Level = 0; uiRef_Level < 6 && !g_bAbort_Request; uiRef_Level++)
				{
					for (int iTemp = -1; iTemp < 2 && !g_bAbort_Request; iTemp++)
						for (int iVel = -1; iVel < 2 && !g_bAbort_Request; iVel++)
							for (int iSs = -1; iSs < 2 && !g_bAbort_Request; iSs++)
								for (int iSe = -1; iSe < 2 && !g_bAbort_Request; iSe++)
								{
									cParam_Curr = cParam;
									ES::Spectrum esResult;
									cParam_Curr.m_dPhotosphere_Temp_kK += iTemp * xvRefine_Params.Get(0);
									cParam_Curr.m_dPhotosphere_Velocity_kkms += iVel * xvRefine_Params.Get(1);
									cParam_Curr.m_dEjecta_Log_Scalar += iSs * xvRefine_Params.Get(2);
									cParam_Curr.m_dShell_Log_Scalar += iSe * xvRefine_Params.Get(3);
									unsigned int uiIdx = (unsigned int) (eModel_Group - opacity_profile_data::carbon);
									msdb_load_generate(cParam_Curr, msdb::COMBINED, cTarget, &g_lpmRefine_Model->m_dsEjecta[uiIdx], &g_lpmRefine_Model->m_dsShell, esResult);
									double dNorm_Gen = Get_Norm_Const(esResult,g_sdRefine_Result.m_dNorm_WL_Blue,g_sdRefine_Result.m_dNorm_WL_Red);
									double dNorm = dNorm_Target / dNorm_Gen;
									double dSum_Err_2 = 0.0;
									unsigned int uiErr_Data_Count = 0;
									for (unsigned int uiI = 0; uiI < esResult.size(); uiI++)
									{
										if (esResult.wl(uiI) >= g_sdRefine_Result.m_dFit_WL_Blue && esResult.wl(uiI) <= g_sdRefine_Result.m_dFit_WL_Red)
										{
											double dErr = esResult.flux(uiI) * dNorm - cTarget.flux(uiI);
											dSum_Err_2 += dErr * dErr;
											uiErr_Data_Count++;
										}
									}
									dSum_Err_2 /= uiErr_Data_Count;
									if (dSum_Err_2 < dQuality_Save)
									{
										dQuality_Save = dSum_Err_2;
										cParam_Save = cParam_Curr;
										g_sdRefine_Result_Curr_Best.m_dPS_Temp = cParam_Curr.m_dPhotosphere_Temp_kK;
										g_sdRefine_Result_Curr_Best.m_dPS_Velocity = cParam_Curr.m_dPhotosphere_Velocity_kkms;
										g_sdRefine_Result_Curr_Best.m_dEjecta_Scalar = cParam_Curr.m_dEjecta_Log_Scalar;
										g_sdRefine_Result_Curr_Best.m_dShell_Scalar = cParam_Curr.m_dShell_Log_Scalar;
										g_sdRefine_Result_Curr_Best.m_specResult = esResult;
										g_sdRefine_Result_Curr_Best.m_dQuality_of_Fit = dSum_Err_2;
									}
									g_sdRefine_Result_Curr.m_dPS_Temp = cParam_Curr.m_dPhotosphere_Temp_kK;
									g_sdRefine_Result_Curr.m_dPS_Velocity = cParam_Curr.m_dPhotosphere_Velocity_kkms;
									g_sdRefine_Result_Curr.m_dEjecta_Scalar = cParam_Curr.m_dEjecta_Log_Scalar;
									g_sdRefine_Result_Curr.m_dShell_Scalar = cParam_Curr.m_dShell_Log_Scalar;
									g_sdRefine_Result_Curr.m_specResult = esResult;
									g_sdRefine_Result_Curr.m_dQuality_of_Fit = dSum_Err_2;

									g_uiRefine_Result_ID++;
//									std::cout << g_uiRefine_Result_ID << std::endl;
									sleep(1); // just in case the generate is already done, to give user a chance to see it.
									
								}
					cParam = cParam_Save;
					xvRefine_Params *= 0.5;
				}
			}
			
			if (!g_bAbort_Request)
			{
				g_sdRefine_Result = g_sdRefine_Result_Curr_Best;

				g_bRefine_Done = true;
				g_bRefine_In_Progress = false;
			}
			else
			{
				g_sdRefine_Result.m_bValid  = false;
				g_sdRefine_Result_Curr.m_bValid  = false;
				g_sdRefine_Result_Curr_Best.m_bValid  = false;
				g_bRefine_Done = false;
				g_bRefine_In_Progress = false;

				g_bAbort_Request = false;
			}
		}
		usleep(10); // hand process back to the OS for a bit to reduce CPU load
	} while (!g_bQuit_Thread);
	g_bRefine_Thread_Running = false;
}

void OSCIn_SuShI_main::Save_Result(const std::string &i_szFilename, unsigned int i_uiModel, const spectrum_data & i_cSpectrum, bool i_bRefined_Data)
{
	if (i_cSpectrum.m_bValid)
	{
		unsigned int uiID = 1;
		FILE * fileSaves = fopen(i_szFilename.c_str(),"rt");
		if (fileSaves == nullptr)
		{
			fileSaves = fopen(i_szFilename.c_str(),"wt");
			fprintf(fileSaves,"#, dd-mm-yyyy, hh:mm:ss, SN, MJD, Instrument, Sources, Model #, Element, Ion, Norm. Blue WL, Norm. Red WL, Fit Blue WL, Fit Red WL, PS Temp, PS Velocity, Ss, Se, Quality, Gauss Fit WL Blue (Model), Gauss Fit WL Red (Model), Gausss Fit WL Blue (Target), Gauss Fit WL Red (Target)\n");
			fclose(fileSaves);
		}
		else
		{
			char lpszBuffer[512] = {0};
			char lpszBufferLast[512] = {0};
			while (!feof(fileSaves))
			{
				strcpy(lpszBufferLast,lpszBuffer);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileSaves);
			}
			uiID = atoi(lpszBufferLast) + 1;
			fclose(fileSaves);
		}
		fileSaves = fopen(i_szFilename.c_str(),"at");
		if (fileSaves != nullptr)
		{
			time_t timer;
			tm * lpTime;
			time(&timer);  /* get current time; same as: timer = time(NULL)  */
			lpTime = gmtime(&timer);

			fprintf(fileSaves,"%i, %02i-%02i-%04i, %02i:%02i:%02i, %s, %.1f, %s, \"%s\", %i, %i, %i, %.0f, %.0f, %.0f, %.0f, %.17e, %.17e, %.17e, %.17e, %.17e, %.0f, %.0f, %.0f, %.0f\n",
							uiID,
							lpTime->tm_mday, 
							lpTime->tm_mon+1, 
							lpTime->tm_year + 1900, 
							lpTime->tm_hour, 
							lpTime->tm_min, 
							lpTime->tm_sec,
							m_OSCfile.m_szSupernova_ID.c_str(),
							m_idSelected_ID.m_dDate_MJD,
							m_idSelected_ID.m_szInstrument.c_str(),
							m_idSelected_ID.m_szSources.c_str(),
							i_uiModel, // assume they haven't changed it
							i_cSpectrum.m_uiIon / 100,
							i_cSpectrum.m_uiIon % 100,
							i_cSpectrum.m_dNorm_WL_Blue,
							i_cSpectrum.m_dNorm_WL_Red,
							i_cSpectrum.m_dFit_WL_Blue,
							i_cSpectrum.m_dFit_WL_Red,
							i_cSpectrum.m_dPS_Temp,
							i_cSpectrum.m_dPS_Velocity,
							i_cSpectrum.m_dShell_Scalar,
							i_cSpectrum.m_dEjecta_Scalar,
							i_cSpectrum.m_dQuality_of_Fit,
							m_dGauss_Fit_Gen_Blue,
							m_dGauss_Fit_Gen_Red,
							m_dGauss_Fit_Blue,
							m_dGauss_Fit_Red);
			fclose(fileSaves);
		}
		std::ostringstream ossJobFile;
		if (i_bRefined_Data)
			ossJobFile << "job_refined_" << m_OSCfile.m_szSupernova_ID << "_" << uiID;
		else
			ossJobFile << "job_" << m_OSCfile.m_szSupernova_ID << "_" << uiID;
		FILE * fileJob = fopen(ossJobFile.str().c_str(),"wt");
		if (fileJob != nullptr)
		{
			fprintf(fileJob,"sf <xml> --ps-vel=%.3f --ps-temp=%.3f --Se=%.5f --Ss=%.5f --norm-wl-blue=%.0f --norm-wl-red=%.0f --fit-wl-blue=%.0f --fit-wl-red=%.0f --fit\n",
							i_cSpectrum.m_dPS_Velocity,
							i_cSpectrum.m_dPS_Temp,
							i_cSpectrum.m_dEjecta_Scalar,
							i_cSpectrum.m_dShell_Scalar,
							i_cSpectrum.m_dNorm_WL_Blue,
							i_cSpectrum.m_dNorm_WL_Red,
							i_cSpectrum.m_dFit_WL_Blue,
							i_cSpectrum.m_dFit_WL_Red);
			fclose(fileJob);
		}
	}
}


bool				g_bFit_Process_Request = false;
bool				g_bFit_In_Progress = false;
bool				g_bFit_Done = false;
bool				g_bFit_Thread_Running = false;
gaussian_fit_data	g_cFit_Result;

class linear_interpolation
{
private:
	double 	m_dSlope;
	double	m_dX0;
	double	m_dY0;

public:
	linear_interpolation(const std::pair<double, double> & i_pddX1, const std::pair<double, double> & i_pddX2)
	{
		m_dSlope = (i_pddX2.second - i_pddX1.second) / (i_pddX2.first - i_pddX1.first);
		m_dX0 = i_pddX1.first;
		m_dY0 = i_pddX1.second;
	}
	inline double Interpolate(const double & i_dX) const
	{
		return (i_dX - m_dX0) * m_dSlope + m_dY0;
	}
	inline double operator << (const double & i_dX) const {return Interpolate(i_dX);}
};

void Process_Fit_Requests(void)
{
	g_bFit_Thread_Running = true;
	do
	{
		if (g_bFit_Process_Request && g_cFit_Result.m_bValid && 
			(g_cFit_Result.m_eFeature == fs_CaHK ||
			g_cFit_Result.m_eFeature == fs_Si6355 ||
			g_cFit_Result.m_eFeature == fs_CaNIR)
			)
		{
			g_bFit_Done = false;
			g_bFit_In_Progress = true;
			g_bFit_Process_Request = false;

			OSCspectrum cTarget = g_cFit_Result.m_specResult;

			std::vector<double> vdWL;
			std::vector<double> vdFlux;
			std::vector<double> vdFlux_Error;

			std::pair<double,double> pddLeft;
			unsigned int uiIdx_Blue = -1;
			std::pair<double,double> pddRight;
			unsigned int uiIdx_Red = -1;

			unsigned int uiI = 0;
			while (uiI < g_cFit_Result.m_specResult.size() && g_cFit_Result.m_specResult.wl(uiI) < g_cFit_Result.m_dRange_WL_Blue)
				uiI++;
			if (uiI < g_cFit_Result.m_specResult.size())
			{
				uiIdx_Blue = uiI;
				pddLeft.first = g_cFit_Result.m_specResult.wl(uiI);
				pddLeft.second = g_cFit_Result.m_specResult.flux(uiI);

				while (uiI < g_cFit_Result.m_specResult.size() && g_cFit_Result.m_specResult.wl(uiI) < g_cFit_Result.m_dRange_WL_Red)
					uiI++;
				if (uiI < g_cFit_Result.m_specResult.size())
				{
					pddRight.first = g_cFit_Result.m_specResult.wl(uiI);
					pddRight.second = g_cFit_Result.m_specResult.flux(uiI);
					uiIdx_Red = uiI;
					linear_interpolation cLI(pddLeft,pddRight);
			

					for (uiI = uiIdx_Blue; uiI <= uiIdx_Red; uiI++)
					{
						double dWL = g_cFit_Result.m_specResult.wl(uiI);
						double dFlux_Cont = cLI << dWL;
						double dFlux_Fit = 1.0 - g_cFit_Result.m_specResult.flux(uiI) / dFlux_Cont;
						double dFlux_Fit_Error = g_cFit_Result.m_specResult.flux_error(uiI) / dFlux_Cont;
						if (dFlux_Fit_Error == 0.0)
							dFlux_Fit_Error = dFlux_Fit * 0.01;
						vdWL.push_back(dWL);
						vdFlux.push_back(dFlux_Fit);
						vdFlux_Error.push_back(dFlux_Fit_Error);
					}
					gauss_fit_parameters * lpgfpParamters;
					switch (g_cFit_Result.m_eFeature)
					{
					case fs_CaHK:
						lpgfpParamters = &g_cgfpCaHK;
						break;
					case fs_Si6355:
						lpgfpParamters = &g_cgfpSi6355;
						break;
					case fs_CaNIR:
						lpgfpParamters = &g_cgfpCaNIR;
						break;
					}

					double dS;
					xvector vSigmas;
					xvector vRes = Perform_Gaussian_Fit(xvector(vdWL), xvector(vdFlux), xvector(vdFlux_Error), lpgfpParamters,
										vdWL[1] - vdWL[0], g_cFit_Result.m_d_pEW[0], g_cFit_Result.m_d_pEW[1], 
										g_cFit_Result.m_dVelocity[0], g_cFit_Result.m_dVelocity[1], vSigmas, g_cFit_Result.m_dQuality_of_Fit);


					g_cFit_Result.m_specResult.clear();
					for (unsigned int uiI = 0; uiI < vdWL.size(); uiI++)
					{
						double dWL = vdWL[uiI];
						double dFlux_Cont = cLI << dWL;

						xvector vY = Multi_Gaussian(dWL,vRes,lpgfpParamters);
						double dFlux_Fit = (1.0 - vY.Get(0)) * dFlux_Cont;
						double dFlux_Error = dFlux_Fit - cTarget.flux(uiIdx_Blue + uiI);
						g_cFit_Result.m_specResult.push_back(OSCspectrum_dp(dWL,dFlux_Fit,dFlux_Error));
					}
					
				}
			}
			g_bFit_In_Progress = false;
			g_bFit_Done = true;
		}			
		usleep(10); // hand process back to the OS for a bit to reduce CPU load
	} while (!g_bQuit_Thread);
	g_bFit_Thread_Running = false;
}

OSCspectrum Copy(const ES::Spectrum & i_cRHO)
{
	OSCspectrum cRet;
	for (unsigned int uiI = 0; uiI < i_cRHO.size(); uiI++)
	{
		cRet.push_back(OSCspectrum_dp(i_cRHO.wl(uiI),i_cRHO.flux(uiI),i_cRHO.flux_error(uiI)));
	}
	return cRet;
}

void OSCIn_SuShI_main::Calc_Observed_pEW(void)
{
	m_dDirect_Measure_pEW = Calc_pEW(m_specSelected_Spectrum,m_dGauss_Fit_Blue,m_dGauss_Fit_Red);
	m_dGen_Direct_Measure_pEW = 0.0;
	m_dRefine_Direct_Measure_pEW = 0.0;
	if (m_sdGenerated_Spectrum.m_bValid)
		m_dGen_Direct_Measure_pEW = Calc_pEW(Copy(m_sdGenerated_Spectrum.m_specResult),m_dGauss_Fit_Gen_Blue,m_dGauss_Fit_Gen_Red);
	if (m_sdRefine_Spectrum_Best.m_bValid)
		m_dRefine_Direct_Measure_pEW = Calc_pEW(Copy(m_sdRefine_Spectrum_Best.m_specResult),m_dGauss_Fit_Gen_Blue,m_dGauss_Fit_Gen_Red);

	m_dDirect_Measure_Vel = Calc_Vel(m_specSelected_Spectrum,m_dGauss_Fit_Blue,m_dGauss_Fit_Red);
	if (m_sdGenerated_Spectrum.m_bValid)
		m_dGen_Direct_Measure_Vel = Calc_Vel(Copy(m_sdGenerated_Spectrum.m_specResult),m_dGauss_Fit_Gen_Blue,m_dGauss_Fit_Gen_Red);
	if (m_sdRefine_Spectrum_Best.m_bValid)
		m_dRefine_Direct_Measure_Vel = Calc_Vel(Copy(m_sdRefine_Spectrum_Best.m_specResult),m_dGauss_Fit_Gen_Blue,m_dGauss_Fit_Gen_Red);
}


double OSCIn_SuShI_main::Calc_pEW(const OSCspectrum & i_cSpectrum, const double & i_dBlue_WL, const double & i_dRed_WL)
{
	double d_pEW = 0.0;

	std::pair<double,double> pddLeft;
	unsigned int uiIdx_Blue = -1;
	std::pair<double,double> pddRight;
	unsigned int uiIdx_Red = -1;

	unsigned int uiI = 0;
	while (uiI < i_cSpectrum.size() && i_cSpectrum.wl(uiI) < i_dBlue_WL)
		uiI++;
	if (uiI < i_cSpectrum.size())
	{
		uiIdx_Blue = uiI;
		pddLeft.first = i_cSpectrum.wl(uiI);
		pddLeft.second = i_cSpectrum.flux(uiI);

		while (uiI < i_cSpectrum.size() && i_cSpectrum.wl(uiI) < i_dRed_WL)
			uiI++;
		if (uiI < i_cSpectrum.size())
		{
			pddRight.first = i_cSpectrum.wl(uiI);
			pddRight.second = i_cSpectrum.flux(uiI);
			uiIdx_Red = uiI;
			linear_interpolation cLI(pddLeft,pddRight);

			double dWL_last;
			if (uiIdx_Blue > 0)
			{
				dWL_last = i_cSpectrum.wl(uiIdx_Blue - 1);
			}
			else
			{
				dWL_last = 2.0 * i_cSpectrum.wl(0) - i_cSpectrum.wl(1);
			}

			for (uiI = uiIdx_Blue; uiI <= uiIdx_Red && uiI != -1; uiI++)
			{
				double dWL = i_cSpectrum.wl(uiI);
				double dFlux_Cont = cLI << dWL;
				double dFlux_Fit = 1.0 - i_cSpectrum.flux(uiI) / dFlux_Cont;
				double dDel = dWL - dWL_last;
				d_pEW += dFlux_Fit * dDel;
				dWL_last = dWL;
			}
		}
	}
	return d_pEW;
}

double OSCIn_SuShI_main::Calc_Vel(const OSCspectrum & i_cSpectrum, const double & i_dBlue_WL, const double & i_dRed_WL)
{
	double dMin_Flux = DBL_MAX;
	double dMin_WL;

	for (unsigned int uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		if (i_cSpectrum.wl(uiI) >= i_dBlue_WL && i_cSpectrum.wl(uiI) <= i_dRed_WL)
		{
			if (i_cSpectrum.flux(uiI) < dMin_Flux)
			{
				dMin_Flux = i_cSpectrum.flux(uiI);
				dMin_WL = i_cSpectrum.wl(uiI);
			}
		}
	}
	return Compute_Velocity(dMin_WL,g_cgfpCaNIR.m_dWl[1]);
}
