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
//	Validate_JSON_Data
//
//
//
//------------------------------------------------------------------------------
void specfit::Validate_JSON_Data(const std::map< std::string, OSCfile> &i_mJson_Data)
{
	for (std::map< std::string, OSCfile >::const_iterator iterFiles = i_mJson_Data.cbegin(); iterFiles != i_mJson_Data.cend(); iterFiles++)
	{
		std::cout << " ------------ References (" << iterFiles->first << ") ------------" << std::endl;
		for (auto iterRef = iterFiles->second.m_vsReference_List.cbegin(); iterRef != iterFiles->second.m_vsReference_List.cend(); iterRef++)
		{
			std::cout << iterRef->m_szAlias;
			std::cout << ": " << iterRef->m_szReference;
			std::cout << " [" << iterRef->m_szName << "]" << std::endl;

		}


		if (iterFiles->second.m_mSpectra_List.size() > 0)
		{
			std::cout << " ------------ Spectra (" << iterFiles->first << ") ------------" << std::endl;
			unsigned int uiCount = iterFiles->second.m_mSpectra_List.size();
			unsigned int uiRows = (uiCount + 1) >> 1; // 1/2 count, adding 1 in case it is odd
			std::ostringstream ozHeaderLeft;

			ozHeaderLeft << "MJD" << " [instrument] (sources)";

			std::cout << ozHeaderLeft.str();
			unsigned int uiSpace = 40 - ozHeaderLeft.str().size();
			if (uiSpace > 40)
				uiSpace = 0;
			while (uiSpace > 0)
			{
				std::cout << " ";
				uiSpace--;
			}
			std::cout << "MJD" << " [instrument] (sources)";
			std::cout << std::endl;

			uiSpace = 79;
			while (uiSpace > 0)
			{
				std::cout << "-";
				uiSpace--;
			}
			std::cout << std::endl;
			std::map<OSCspectra_id, OSCspectrum >::const_iterator iterSpecL = iterFiles->second.m_mSpectra_List.cbegin();
			std::map<OSCspectra_id, OSCspectrum >::const_iterator iterSpecR = iterFiles->second.m_mSpectra_List.cbegin();

			for (unsigned int uiI = 0; uiI < uiRows; uiI++)
				iterSpecR++;

			for (unsigned int uiI = 0; uiI < uiRows; uiI++)
			{
				std::ostringstream ozLeftCol;

				if (iterSpecL != iterFiles->second.m_mSpectra_List.cend())
					ozLeftCol << iterSpecL->first.m_dDate_MJD << " [" << iterSpecL->first.m_szInstrument << "] (" << iterSpecL->first.m_szSources << ")";

				std::cout << ozLeftCol.str();
				unsigned int uiSpace = 40 - ozLeftCol.str().size();
				if (uiSpace > 40)
					uiSpace = 0;
				while (uiSpace > 0)
				{
					std::cout << " ";
					uiSpace--;
				}
				if (iterSpecR != iterFiles->second.m_mSpectra_List.cend())
					std::cout << iterSpecR->first.m_dDate_MJD << " [" << iterSpecR->first.m_szInstrument << "] (" << iterSpecR->first.m_szSources << ")";
				std::cout << std::endl;

				if (iterSpecL != iterFiles->second.m_mSpectra_List.cend())
					iterSpecL++;
				if (iterSpecR != iterFiles->second.m_mSpectra_List.cend())
					iterSpecR++;
			}
		}
		else // if (iterFiles->second[szID].isMember("spectra"))
			std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "No spectra available in this .json file." << std::endl;
	} // for (std::map< std::string, Json::Value >::iterator iterFiles = mJson_Data.begin(); ...
}
