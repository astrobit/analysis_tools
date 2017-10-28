#include <specfit.h>

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
