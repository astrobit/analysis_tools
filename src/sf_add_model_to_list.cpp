#include <iostream>
#include <fstream>
#include <json.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <cmath>
#include <xio.h>
#include <opacity_profile_data.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <specfit.h>
#include <line_routines.h>
#include <cstdio>
#include <iomanip>
#include <ios>
	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <eps_plot.h>

//------------------------------------------------------------------------------
//
//
//
//	Add_Model_To_List
//
//
//
//------------------------------------------------------------------------------
void specfit::Add_Model_To_List(std::map< unsigned int, specfit::model> &io_o_mModel_Data, unsigned int i_uiModel)
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
		if (io_o_mModel_Data.count(i_uiModel) == 0)
		{
			ossFile_Path <<  "/";
			specfit::model	mModel;
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

			mModel.m_uiModel_ID = i_uiModel;
			mModel.m_dsEjecta[OPACITY_PROFILE_DATA::CARBON].ReadDataFileBin(ossOpM_C.str().c_str(),true);
			mModel.m_dsEjecta[OPACITY_PROFILE_DATA::OXYGEN].ReadDataFileBin(ossOpM_O.str().c_str(),true);
			mModel.m_dsEjecta[OPACITY_PROFILE_DATA::MAGNESIUM].ReadDataFileBin(ossOpM_Mg.str().c_str(),true);
			mModel.m_dsEjecta[OPACITY_PROFILE_DATA::SILICON].ReadDataFileBin(ossOpM_Si.str().c_str(),true);
			mModel.m_dsEjecta[OPACITY_PROFILE_DATA::IRON].ReadDataFileBin(ossOpM_Fe.str().c_str(),true);
			mModel.m_dsShell.ReadDataFileBin(ossOpM_Shell.str().c_str(),true);
			mModel.m_opdOp_Profile_Data.Load(ossOp_Data.str().c_str());

			mModel.m_dsPhotosphere.ReadDataFile(ossPhoto.str().c_str(),false,false,',',1);
			mModel.m_dsEjecta_Photosphere.ReadDataFile(ossPhoto_EO.str().c_str(),false,false,',',1);

			io_o_mModel_Data[i_uiModel] = mModel;
		}
	}
}
