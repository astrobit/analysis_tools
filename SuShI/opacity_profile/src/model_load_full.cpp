#include <opacity_profile_data.h>
#include <unistd.h>
#include <xstdlib.h>
#include <dirent.h>
#include <sstream>
#include <iostream>

//------------------------------------------------------------------------------
//
//
//
//	Add_Model_To_List
//
//
//
//------------------------------------------------------------------------------
void model::Load_Model_Full_Data(void)
{
	const char * lpszData_Dir = DATADIR;
	if (lpszData_Dir != nullptr)
	{
		std::ostringstream ossFile_Path;
		ossFile_Path << lpszData_Dir;
		ossFile_Path << "/";
		ossFile_Path <<  m_uiModel_ID;

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
			std::ostringstream ossFD_Shell;
			std::ostringstream ossFD_Ejecta;

			ossFD_Shell << ossFile_Path.str() << "full_data_shell.xdataset";
			ossFD_Ejecta << ossFile_Path.str() << "full_data_ejecta.xdataset";

			m_dsShell_Full_Data.Read_xdataset(ossFD_Shell.str().c_str());
			m_dsEjecta_Full_Data.Read_xdataset(ossFD_Ejecta.str().c_str());

			if (m_dsEjecta_Full_Data.Get_Num_Rows() == 0)
				std::cerr << "Failed to load full data for model " << m_uiModel_ID << std::endl;
		}
	}
}
