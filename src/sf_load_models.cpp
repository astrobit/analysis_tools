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
//	Load_Models
//
//
//
//------------------------------------------------------------------------------
void specfit::Load_Models(
	std::vector <specfit::fit> &i_vfitFits,
	std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists,
	std::map< unsigned int, specfit::model> &o_mModel_Data
	)
{
	// process model lists
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
	if (!lpszLA_Data_Path)
		std::cerr << xconsole::bold << xconsole::foreground_yellow << "Warning: " << xconsole::reset << "LINE_ANALYSIS_DATA_PATH not specified. Probably won't find model data." << std::endl;

	for (std::map< std::string, std::vector<unsigned int> >::iterator iterML = i_mModel_Lists.begin(); iterML != i_mModel_Lists.end(); iterML++)
	{
		for (std::vector<unsigned int>::iterator iterMod = iterML->second.begin(); iterMod != iterML->second.end(); iterMod++)
		{
			Add_Model_To_List(o_mModel_Data,*iterMod);
		} // for (std::vector<unsigned int>::iterator iterMod = iterML->second.begin(); ...
	} // for (std::map< std::string, std::vector<unsigned int> >::iterator iterML = mModel_Lists.begin(); ...

	// process each model, and make sure that it has all of the needed info
	for (std::vector <specfit::fit>::iterator iterFit = i_vfitFits.begin(); iterFit != i_vfitFits.end(); iterFit++)
	{
		// process individual models
		for (std::vector <unsigned int>::iterator 	iterMod = iterFit->m_vuiModels.begin(); iterMod != iterFit->m_vuiModels.end(); iterMod++)
		{
			Add_Model_To_List(o_mModel_Data,*iterMod);
		} // for (std::vector <unsigned int>::iterator 	iterMod = iterFit->m_vuiModels.begin(); ...
	}
}
