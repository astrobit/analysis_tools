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
void specfit::Validate_JSON_Data(const std::map< std::string, Json::Value> &i_mJson_Data)
{
	for (std::map< std::string, Json::Value >::const_iterator iterFiles = i_mJson_Data.cbegin(); iterFiles != i_mJson_Data.cend(); iterFiles++)
	{
		std::string szID;
		Json::Value::Members jmMember_List = iterFiles->second.getMemberNames();
		szID = jmMember_List[0];
		if (iterFiles->second[szID].isMember("sources")) // confirm that there is at least one source in this file
		{ // display list of sources
			std::cout << " ------------ References (" << iterFiles->first << ") ------------" << std::endl;
			Json::Value vSources = iterFiles->second[szID]["sources"];
			if (vSources.isArray())
			{
				for (Json::ValueConstIterator iterI = vSources.begin(); iterI != vSources.end(); iterI++)
				{
					if (iterI->isObject())
					{
						if (iterI->isMember("alias"))
						{
							Json::Value vAlias = iterI->get("alias",Json::Value());
							std::cout << vAlias.asString();
						}
						else // if (iterI->isMember("alias"))
							std::cout << "X";
						if (iterI->isMember("reference"))
						{
							Json::Value vReference = iterI->get("reference",Json::Value());
							std::cout << ": " << vReference.asString();
						} // if (iterI->isMember("reference"))
						if (iterI->isMember("name"))
						{
							Json::Value vReference = iterI->get("name",Json::Value());
							std::cout << " [" << vReference.asString() << "]";
						} // if (iterI->isMember("name"))
						std::cout << std::endl;
					} // if (iterI->isObject())
				} // for (Json::ValueConstIterator iterI = vSources.begin(); ...
			} // if (vSources.isArray())
		} // if (iterFiles->second[szID].isMember("sources"))

		if (iterFiles->second[szID].isMember("spectra")) // confirm that there is at least one spectrum in this file
		{
			class spec_data
			{
			public:
				std::string m_szMJD;
				std::string m_szInstrument;
				std::string m_szSources;
			};
			std::vector<spec_data> vsdSpectra_Sources;

			Json::Value jvSpectra = iterFiles->second[szID]["spectra"];
			if (jvSpectra.isArray())
			{
				for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
				{
					if (iterI->isObject())
					{
						spec_data	cSpec_Data;
						if (iterI->isMember("source"))
						{
							Json::Value vValue = iterI->get("source",Json::Value());
							cSpec_Data.m_szSources = vValue.asString();
							
						} // if (iterI->isMember("source"))
						if (iterI->isMember("instrument"))
						{
							Json::Value vValue = iterI->get("instrument",Json::Value());
							cSpec_Data.m_szInstrument = vValue.asString();
							
						} // if (iterI->isMember("source"))
						if (iterI->isMember("time"))
						{
							Json::Value vValue = iterI->get("time",Json::Value());
							cSpec_Data.m_szMJD = vValue.asString();
							
						} // if (iterI->isMember("time"))
						vsdSpectra_Sources.push_back(cSpec_Data);
					} // if (iterI->isObject())
				} // for (Json::ValueConstIterator iterI = jvSpectra.begin(); ...
			} // if (jvSpectra.isArray())
			std::cout << " ------------ Spectra (" << iterFiles->first << ") ------------" << std::endl;
			unsigned int uiCount = vsdSpectra_Sources.size();
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

			for (unsigned int uiI = 0; uiI < uiRows; uiI++)
			{
				std::ostringstream ozLeftCol;

				ozLeftCol << vsdSpectra_Sources[uiI].m_szMJD << " [" << vsdSpectra_Sources[uiI].m_szInstrument << "] (" << vsdSpectra_Sources[uiI].m_szSources << ")";

				std::cout << ozLeftCol.str();
				unsigned int uiSpace = 40 - ozLeftCol.str().size();
				if (uiSpace > 40)
					uiSpace = 0;
				while (uiSpace > 0)
				{
					std::cout << " ";
					uiSpace--;
				}
				if ((uiI + uiRows) < uiCount)
					std::cout << vsdSpectra_Sources[uiI + uiRows].m_szMJD << " [" << vsdSpectra_Sources[uiI + uiRows].m_szInstrument << "] (" << vsdSpectra_Sources[uiI + uiRows].m_szSources << ")";
				std::cout << std::endl;
			}
		}
		else // if (iterFiles->second[szID].isMember("spectra"))
			std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "No spectra available in this .json file." << std::endl;
	} // for (std::map< std::string, Json::Value >::iterator iterFiles = mJson_Data.begin(); ...
}
