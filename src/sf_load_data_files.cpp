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
	std::map< std::string, Json::Value> &o_mJson_Data,
	std::map< std::string, std::vector<std::tuple<double,double,double> > > &o_mNon_Json_Data
	)
{
	// Parse the list of datafiles, 
	for (std::map< std::string, std::string >::const_iterator iterFiles = i_mDatafile_List.cbegin(); iterFiles != i_mDatafile_List.cend(); iterFiles++)
	
	{
		if (iterFiles->second.find(".json") != std::string::npos)
		{
			Json::Value jsonRoot;   // starts as "null"; will contain the root value after parsing
			std::ifstream fsData_File;
			fsData_File.open(iterFiles->second.c_str(), std::ifstream::binary);
			if (fsData_File.is_open())
			{
				fsData_File >> jsonRoot;
				o_mJson_Data[iterFiles->first] = jsonRoot;
				fsData_File.close();
			}
			else 
				std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Failed to open " << iterFiles->second << std::endl;
		}
		else // if (iterFiles->second.find(".json") != std::string::npos)
		{ //Non-json data file.  
			//@@TODO
		}
	} // for (std::map< std::string, std::string >::const_iterator iterFiles = i_mDatafile_List.cbegin(); ...
}
