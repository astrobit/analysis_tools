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
//	Load_Data_Files
//
//
//
//------------------------------------------------------------------------------
void specfit::Load_Data_Files(const std::map< std::string, std::string > &i_mDatafile_List,
	std::map< std::string, OSCfile> &o_mJson_Data,
	std::map< std::string, OSCspectrum > &o_mNon_Json_Data
	)
{
	// Parse the list of datafiles, 
	for (std::map< std::string, std::string >::const_iterator iterFiles = i_mDatafile_List.cbegin(); iterFiles != i_mDatafile_List.cend(); iterFiles++)
	
	{
		if (iterFiles->second.find(".json") != std::string::npos)
		{
			OSCfile jsonFile;   // starts as "null"; will contain the root value after parsing
			jsonFile.Load(iterFiles->second.c_str());
			o_mJson_Data[iterFiles->first] = jsonFile;
		}
		else // if (iterFiles->second.find(".json") != std::string::npos)
		{ //Non-json data file.  
			//@@TODO
		}
	} // for (std::map< std::string, std::string >::const_iterator iterFiles = i_mDatafile_List.cbegin(); ...
}
