#include <OSCIn-SuShI>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
}
ES::Spectrum Get_ES_Spectrum(const OSCspectrum & i_cRHO)
{
	FILE * fileTemp = fopen("temp.spc","wt");
	for (unsigned int uiI = 0; uiI < i_cRHO.size(); uiI++)
	{
		fprintf(fileTemp,"%.2f %.17e %.17e\n",i_cRHO.wl(uiI),i_cRHO.flux(uiI),i_cRHO.flux_error(uiI));
	}
	fclose(fileTemp);

	ES::Spectrum i_cLHO = ES::Spectrum::create_from_ascii_file("temp.spc");
	
	return i_cLHO;
}


void OSCIn_SuShI_main::Normalize(void)
{
	double dTgt_Norm = 0.0, dGen_Norm = 0.0;
	for (unsigned int uiI = 0; uiI < m_specSelected_Spectrum.size(); uiI++)
	{
		if (m_specSelected_Spectrum.wl(uiI) >= m_dNorm_Blue && m_specSelected_Spectrum.wl(uiI) <= m_dNorm_Red)
			dTgt_Norm += m_specSelected_Spectrum.flux(uiI);
	}
	for (unsigned int uiI = 0; uiI < g_sdGen_Spectrum_Result.m_specResult.size(); uiI++)
	{
		if (g_sdGen_Spectrum_Result.m_specResult.wl(uiI) >= m_dNorm_Blue && g_sdGen_Spectrum_Result.m_specResult.wl(uiI) <= m_dNorm_Red)
			dGen_Norm += g_sdGen_Spectrum_Result.m_specResult.flux(uiI);
	}
	m_dGenerated_Spectrum_Norm = dTgt_Norm / dGen_Norm;
	dGen_Norm = 0.0;
	for (unsigned int uiI = 0; uiI < m_sdGenerated_Spectrum_Prev.m_specResult.size(); uiI++)
	{
		if (m_sdGenerated_Spectrum_Prev.m_specResult.wl(uiI) >= m_dNorm_Blue && m_sdGenerated_Spectrum_Prev.m_specResult.wl(uiI) <= m_dNorm_Red)
			dGen_Norm += m_sdGenerated_Spectrum_Prev.m_specResult.flux(uiI);
	}
	m_dGenerated_Spectrum_Prev_Norm = dTgt_Norm / dGen_Norm;
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

bool				g_bQuit_Thread = false;
bool 				g_bGen_Process_Request = false;
bool				g_bGen_In_Progress = false;
bool				g_bGen_Done = false;
bool				g_bGen_Thread_Running = false;
spectrum_data		g_sdGen_Spectrum_Result;
model *				g_lpmGen_Model = nullptr;

void Process_Generate_Requests(void)
{
	g_bGen_Thread_Running = true;
	do
	{
		if (g_bGen_Process_Request && g_sdGen_Spectrum_Result.m_bValid)
		{
			g_bGen_Done = false;
			g_bGen_In_Progress = true;
			g_bGen_Process_Request = false;
			
			msdb::USER_PARAMETERS cParam;
			opacity_profile_data::group eModel_Group = opacity_profile_data::carbon;
			cParam.m_uiModel_ID = g_lpmGen_Model->m_uiModel_ID;
			cParam.m_uiIon = g_sdGen_Spectrum_Result.m_uiIon;
			cParam.m_eFeature = msdb::CaNIR;
			cParam.m_dTime_After_Explosion = 1.0;
			cParam.m_dPhotosphere_Velocity_kkms = g_sdGen_Spectrum_Result.m_dPS_Velocity;
			cParam.m_dPhotosphere_Temp_kK = g_sdGen_Spectrum_Result.m_dPS_Temp;
			cParam.m_dEjecta_Log_Scalar = g_sdGen_Spectrum_Result.m_dEjecta_Scalar;
			cParam.m_dShell_Log_Scalar = g_sdGen_Spectrum_Result.m_dShell_Scalar;
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;
			cParam.m_dWavelength_Range_Lower_Ang = g_sdGen_Spectrum_Result.m_specResult.wl(0);
			cParam.m_dWavelength_Range_Upper_Ang = g_sdGen_Spectrum_Result.m_specResult.wl(g_sdGen_Spectrum_Result.m_specResult.size() - 1);
			cParam.m_dWavelength_Delta_Ang = g_sdGen_Spectrum_Result.m_specResult.wl(1) - g_sdGen_Spectrum_Result.m_specResult.wl(0);
			cParam.m_dEjecta_Scalar_Time_Power_Law = -2.0;
			cParam.m_dShell_Scalar_Time_Power_Law = -2.0;

			unsigned int uiElem = cParam.m_uiIon / 100;
			bool bValid_Ion = false;
			switch (uiElem)
			{
			case 6:
				eModel_Group = opacity_profile_data::carbon;
				bValid_Ion = true;
				break;
			case 8:
				eModel_Group = opacity_profile_data::magnesium;
				bValid_Ion = true;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				eModel_Group = opacity_profile_data::magnesium;
				bValid_Ion = true;
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
				eModel_Group = opacity_profile_data::silicon;
				bValid_Ion = true;
				break;
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
				eModel_Group = opacity_profile_data::iron;
				bValid_Ion = true;
				break;
			}


			ES::Spectrum cTarget = g_sdGen_Spectrum_Result.m_specResult;
			g_sdGen_Spectrum_Result.m_specResult.zero_flux();
			if (bValid_Ion)
			{
				unsigned int uiIdx = (unsigned int) (eModel_Group - opacity_profile_data::carbon);
				msdb_load_generate(cParam, msdb::COMBINED, cTarget, &g_lpmGen_Model->m_dsEjecta[uiIdx], &g_lpmGen_Model->m_dsShell, g_sdGen_Spectrum_Result.m_specResult);
			}
			g_bGen_Done = true;
			g_bGen_In_Progress = false;
		}
	} while (!g_bQuit_Thread);
	g_bGen_Thread_Running = false;
}
