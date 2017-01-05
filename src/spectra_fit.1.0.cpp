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
	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
/*
class dictionarynode
{
public:
	std::string m_szNode_Name;
	std::map< std::string, dictionarynode> m_cChildren;

	dictionarynode(void){;}
	dictionarynode(const std::string & i_szNode_Name)
	{
		m_szNode_Name = i_szNode_Name;
	}
};
//------------------------------------------------------------------------------
//
//
//
//	Parse (dictionarynode ...
//
//
//
//------------------------------------------------------------------------------

void Parse(const dictionarynode & i_cDictionaryEntry, unsigned int i_uiTab_Level)
{
	i_uiTab_Level++;
	for (unsigned int uiI = 0; uiI < i_uiTab_Level - 1; uiI++)
		std::cout<<"----|";
	std::cout << i_cDictionaryEntry.m_szNode_Name;
	std::cout << std::endl;

	for (std::map< std::string, dictionarynode>::const_iterator cI = i_cDictionaryEntry.m_cChildren.begin(); cI != i_cDictionaryEntry.m_cChildren.end(); cI++)
	{
		Parse(cI->second,i_uiTab_Level);
	}
}
//------------------------------------------------------------------------------
//
//
//
//	Parse (Json::Value ...
//
//
//
//------------------------------------------------------------------------------

void Parse(const Json::Value & i_jvValue, unsigned int i_uiTab_Level, dictionarynode & o_cDictionaryEntry)
{
	i_uiTab_Level++;
	if (i_jvValue.isObject())
	{
		Json::Value::Members jmMember_List = i_jvValue.getMemberNames();
		for (Json::Value::Members::const_iterator iterJ = jmMember_List.begin(); iterJ != jmMember_List.end(); iterJ++)
		{
			if (o_cDictionaryEntry.m_cChildren.count(*iterJ) == 0)
				o_cDictionaryEntry.m_cChildren[*iterJ] = dictionarynode(*iterJ);
			Parse(i_jvValue[*iterJ],i_uiTab_Level,o_cDictionaryEntry.m_cChildren[*iterJ]);
		}
	}
	else if (i_jvValue.isArray())
	{
		for (Json::ValueConstIterator iterI = i_jvValue.begin(); iterI != i_jvValue.end(); iterI++)
		{
			if (iterI->isObject())
			{
				Parse((*iterI),i_uiTab_Level,o_cDictionaryEntry);
			}
		}
	}
}
*/

//------------------------------------------------------------------------------
//
//
//
//	Test_Attr_Content
//
//
//
//------------------------------------------------------------------------------
inline bool Test_Attr_Content(const xmlAttr * i_lpAttr)
{
	return (i_lpAttr && i_lpAttr->children && i_lpAttr->children->type == XML_TEXT_NODE);
}

//------------------------------------------------------------------------------
//
//
//
//	Attr_Get_String
//
//
//
//------------------------------------------------------------------------------
std::string Attr_Get_String(const xmlAttr * i_lpAttr)
{
	std::string szRet;
	if (Test_Attr_Content(i_lpAttr))
		szRet = (const char *) i_lpAttr->children->content;
	return szRet;
}

//------------------------------------------------------------------------------
//
//
//
//	Attr_Get_Uint
//
//
//
//------------------------------------------------------------------------------
unsigned int Attr_Get_Uint(const xmlAttr * i_lpAttr, unsigned int i_uiDefault)
{
	unsigned int uiRet = i_uiDefault;
	std::string szData = Attr_Get_String(i_lpAttr);
	if (!szData.empty())
		uiRet = std::stoi(szData);
	return uiRet;
}
//------------------------------------------------------------------------------
//
//
//
//	Attr_Get_Double
//
//
//
//------------------------------------------------------------------------------
double Attr_Get_Double(const xmlAttr * i_lpAttr, const double &i_dDefault)
{
	double dRet = i_dDefault;
	std::string szData = Attr_Get_String(i_lpAttr);
	if (!szData.empty())
		dRet = std::stod(szData);
	return dRet;
}

//------------------------------------------------------------------------------
//
//
//
//	Attr_Get_Bool
//
//
//
//------------------------------------------------------------------------------
bool Attr_Get_Bool(const xmlAttr * i_lpAttr, bool i_bDefault)
{
	bool bRet = i_bDefault;
	if (Test_Attr_Content(i_lpAttr))
		bRet = (strcmp((char *)i_lpAttr->children->content,"true") == 0);
	return bRet;
}
//------------------------------------------------------------------------------
//
//
//
//	Parse_XML
//
//
//
//------------------------------------------------------------------------------

void Parse_XML(const std::string &i_szFilename,
	std::vector <specfit::fit> &o_vfitFits ,
	std::map< unsigned int, specfit::model> &o_mModel_Data,
	std::map< std::string, std::vector<unsigned int> > &o_mModel_Lists,
	std::map< std::string, std::string > &o_mDatafile_List
	)
{
	xmlDocPtr doc = xmlReadFile(i_szFilename.c_str(), nullptr, XML_PARSE_DTDVALID);

	if (doc == nullptr)
	{
		std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Failed to parse " << i_szFilename << std::endl;
	}
	else
	{
		/*Get the root element node */
		xmlNodePtr xrnRoot = xmlDocGetRootElement(doc);
		// Parse the XML file itself; fill the data into maps and vectors with the relevant information in each
		if (xrnRoot && xrnRoot->type == XML_ELEMENT_NODE && std::string((const char *)xrnRoot->name) == "SPECFIT")
		{
			xmlNode * lpCurr_Node = xrnRoot->children;
			xmlAttr * lpCurr_Attr = nullptr;
			// first go through and identify all model lists
			while (lpCurr_Node)
			{
				switch (lpCurr_Node->type)
				{
				case XML_ELEMENT_NODE:
					if (std::string((const char *)lpCurr_Node->name) == "MODELLIST")
					{
						lpCurr_Attr = lpCurr_Node->properties;
						std::string szList_ID;
						std::vector<unsigned int> vModels;
						while (lpCurr_Attr)
						{
							if (std::string((const char *)lpCurr_Attr->name) == "id")
							{
								szList_ID = Attr_Get_String(lpCurr_Attr);
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
						xmlNode * lpCurr_Model_Node = lpCurr_Node->children;
						while (lpCurr_Model_Node)
						{
							switch (lpCurr_Node->type)
							{
							case XML_ELEMENT_NODE:
								if (std::string((const char *)lpCurr_Model_Node->name) == "MODEL")
								{
									lpCurr_Attr = lpCurr_Model_Node->properties;
									std::string szList_ID;
									while (lpCurr_Attr)
									{
										if (std::string((const char *)lpCurr_Attr->name) == "id")
										{
											vModels.push_back(Attr_Get_Uint(lpCurr_Attr,-1));
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								break;
							}
							lpCurr_Model_Node = lpCurr_Model_Node->next;
						}
						if (!vModels.empty() && !szList_ID.empty())
							o_mModel_Lists[szList_ID] = vModels;
					}
					else if (std::string((const char *)lpCurr_Node->name) == "DATAFILE")
					{
						lpCurr_Attr = lpCurr_Node->properties;
						std::string szFile_ID;
						std::string szPath;
						while (lpCurr_Attr)
						{
							if (std::string((const char *)lpCurr_Attr->name) == "filepath")
							{
								szPath = Attr_Get_String(lpCurr_Attr);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "id")
							{
								szFile_ID = Attr_Get_String(lpCurr_Attr);
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
					
						if (!szFile_ID.empty() && !szPath.empty())
							o_mDatafile_List[szFile_ID] = szPath;
					}
					else if (std::string((const char *)lpCurr_Node->name) == "FIT")
					{
						lpCurr_Attr = lpCurr_Node->properties;
						std::string szFile_ID;
						std::string szSource;
						std::string szMJD;
						std::string szFeature;
						specfit::fit	cFit;
						while (lpCurr_Attr)
						{
							if (std::string((const char *)lpCurr_Attr->name) == "datafileid")
							{
								cFit.m_szDatafile = Attr_Get_String(lpCurr_Attr);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "instrument")
							{
								cFit.m_szInstrument = Attr_Get_String(lpCurr_Attr);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "source")
							{
								cFit.m_szSource = Attr_Get_String(lpCurr_Attr);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "mjd")
							{
								cFit.m_dMJD = Attr_Get_Double(lpCurr_Attr,-1);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "ebv")
							{
								cFit.m_dE_BmV = Attr_Get_Double(lpCurr_Attr,0.0);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "mjdbmax")
							{
								cFit.m_dMJD_Bmax = Attr_Get_Double(lpCurr_Attr,nan(""));
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "bmaxdelayexpl")
							{
								cFit.m_dDelay_From_Explosion_To_Bmax = Attr_Get_Double(lpCurr_Attr,18.0);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "timepowerlaw")
							{
								cFit.m_dTime_Scale_Power_Law = Attr_Get_Double(lpCurr_Attr,-2.0);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "scalestartwithtime")
							{
								cFit.m_dTime_Scale_Power_Law = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "usejsonebv")
							{
								cFit.m_bUse_JSON_ebv = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "usetwocomponentfit")
							{
								cFit.m_bUse_Two_Component_Fit = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (std::string((const char *)lpCurr_Attr->name) == "feature")
							{
								std::string szFeature = Attr_Get_String(lpCurr_Attr);
								if (szFeature == "CaNIR")
									cFit.m_eFeature = specfit::CaNIR;
								else if (szFeature == "CaHK")
									cFit.m_eFeature = specfit::CaHK;
								else if (szFeature == "Si6355")
									cFit.m_eFeature = specfit::Si6355;
								else if (szFeature == "O7773")
									cFit.m_eFeature = specfit::O7773;
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
					
						xmlNode * lpCurr_Model_Node = lpCurr_Node->children;
						while (lpCurr_Model_Node)
						{
							switch (lpCurr_Model_Node->type)
							{
							case XML_ELEMENT_NODE:
								if (std::string((const char *)lpCurr_Model_Node->name) == "MODEL")
								{
									lpCurr_Attr = lpCurr_Model_Node->properties;
									std::string szList_ID;
									while (lpCurr_Attr)
									{
										if (std::string((const char *)lpCurr_Attr->name) == "id")
										{
											cFit.m_vuiModels.push_back(Attr_Get_Uint(lpCurr_Attr,-1));
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								else if (std::string((const char *)lpCurr_Model_Node->name) == "MODLIST")
								{
									lpCurr_Attr = lpCurr_Model_Node->properties;
									std::string szList_ID;
									while (lpCurr_Attr)
									{
										if (std::string((const char *)lpCurr_Attr->name) == "id")
										{
											cFit.m_vsModel_Lists.push_back(Attr_Get_String(lpCurr_Attr));
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								else if (std::string((const char *)lpCurr_Model_Node->name) == "SUGGESTPARAM")
								{
									lpCurr_Attr = lpCurr_Model_Node->properties;
									std::string szList_ID;
									while (lpCurr_Attr)
									{
										if (std::string((const char *)lpCurr_Attr->name) == "ps_temp")
										{
											cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Temp = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "ps_vel")
										{
											cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel = Attr_Get_Double(lpCurr_Attr,nan(""));
											cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel = cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel;
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "ejecta_ps_vel")
										{
											cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "shell_ps_vel")
										{
											cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "ejecta_temp")
										{
											cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dExcitation_Temp = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "ejecta_log_S")
										{
											cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "shell_temp")
										{
											cFit.m_cSuggested_Param[specfit::comp_shell].m_dExcitation_Temp = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "shell_log_S")
										{
											cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S = Attr_Get_Double(lpCurr_Attr,nan(""));
										}
										else if (std::string((const char *)lpCurr_Attr->name) == "ejecta_shell_mixing")
										{
											cFit.m_cSuggested_Param.m_dMixing_Fraction = Attr_Get_Double(lpCurr_Attr,nan(""));
										}


										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								break;
							}
							lpCurr_Model_Node = lpCurr_Model_Node->next;
						}
						o_vfitFits.push_back(cFit);
					}
					break;
				}
				lpCurr_Node = lpCurr_Node->next;
			}
		} //if (i_lpRoot_Element && i_lpRoot_Element->type == XML_ELEMENT_NODE && std::string((const char *)i_lpRoot_Element->name) == "SPECFIT")
	} // if (doc == nullptr)
} // void Parse_XML(...

//------------------------------------------------------------------------------
//
//
//
//	Load_Data_Files
//
//
//
//------------------------------------------------------------------------------
void Load_Data_Files(const std::map< std::string, std::string > &i_mDatafile_List,
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
//------------------------------------------------------------------------------
//
//
//
//	Validate_JSON_Data
//
//
//
//------------------------------------------------------------------------------
void Validate_JSON_Data(const std::map< std::string, Json::Value> &i_mJson_Data)
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

//------------------------------------------------------------------------------
//
//
//
//	Add_Model_To_List
//
//
//
//------------------------------------------------------------------------------
void Add_Model_To_List(std::map< unsigned int, specfit::model> &io_o_mModel_Data, unsigned int i_uiModel)
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
//------------------------------------------------------------------------------
//
//
//
//	Load_Models
//
//
//
//------------------------------------------------------------------------------
void Load_Models(
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
//------------------------------------------------------------------------------
//
//
//
//	Load_Data
//
//
//
//------------------------------------------------------------------------------

void Load_Data(
	std::vector <specfit::fit> &io_vfitFits,
	std::map< std::string, Json::Value> &i_mJson_Data,
	std::map< std::string, std::vector<std::tuple<double,double,double> > > &i_mNon_Json_Data)
{
	// process each model, and make sure that it has all of the needed info; fill the data points in the vector in the fit class for each one.
	for (std::vector <specfit::fit>::iterator iterFit = io_vfitFits.begin(); iterFit != io_vfitFits.end(); iterFit++)
	{
		if (i_mJson_Data.count(iterFit->m_szDatafile) > 0) /// looking at json data file
		{
			Json::Value jvData = i_mJson_Data[iterFit->m_szDatafile];
			Json::Value::Members jmMember_List = jvData.getMemberNames();
			std::string szID = jmMember_List[0];
			if (iterFit->m_bUse_JSON_ebv) // user has requested to use ebv data from the JSON file
			{
				if (jvData[szID].isMember("ebv")) // confirm that there is at least one spectrum in this file
				{
					Json::Value jvEBV = jvData[szID]["ebv"];
					if (jvEBV.isArray())
					{
						double dEBV_Weighted_Sum = 0.0;
						double dEBV_Error_Sum = 0.0;
						std::vector<Json::ValueConstIterator> viterFound;
						for (Json::ValueConstIterator iterI = jvEBV.begin(); iterI != jvEBV.end(); iterI++)
						{
							double	dValue;
							Json::Value vValue = iterI->get("value",Json::Value());
							if (vValue.isConvertibleTo(Json::realValue))
								dValue = vValue.asDouble();
							else
								dValue = std::stod(std::string(vValue.asString()));
							double dError = dValue;
							Json::Value vError = iterI->get("error",Json::Value());
							if (vError.isConvertibleTo(Json::realValue))
								dError = vError.asDouble();
							else
								dError = std::stod(std::string(vError.asString()));
							double dWeight = 1.0 / (dError * dError);
							dEBV_Weighted_Sum += dValue * dWeight;
							dEBV_Error_Sum += dWeight;
						}
						iterFit->m_dE_BmV = dEBV_Weighted_Sum / dEBV_Error_Sum;
					}
				}
				else
				{
					Json::Value vValue = jvData[szID]["ebv"].get("value",Json::Value());
					if (vValue.isConvertibleTo(Json::realValue))
						iterFit->m_dE_BmV = vValue.asDouble();
					else
						iterFit->m_dE_BmV = std::stod(std::string(vValue.asString()));
				}

			}
			if (jvData[szID].isMember("spectra")) // confirm that there is at least one spectrum in this file
			{
				Json::Value jvSpectra = jvData[szID]["spectra"];
				if (jvSpectra.isArray())
				{
					std::vector<Json::ValueConstIterator> viterFound;
					for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
					{
						if (iterI->isObject())
						{
							if (iterI->isMember("source"))
							{
								Json::Value vSource = iterI->get("source",Json::Value());
								std::string sSource = vSource.asString();

								if (sSource.find(iterFit->m_szSource) != std::string::npos)
								{
									double dTime = nan("");
									if (iterI->isMember("time"))
									{
										Json::Value vTime = iterI->get("time",Json::Value());
										if (vTime.isConvertibleTo(Json::realValue))
											dTime = vTime.asDouble();
										else
											dTime = std::stod(std::string(vTime.asString()));
									} // iterI->isMember("time")
									if (!isnan(dTime) && dTime == iterFit->m_dMJD)
									{
										viterFound.push_back(iterI);
									} // if (!isnan(dTime) && dTime == iterFit->m_dMJD)
								} // if (sSource.find(iterFit->m_szSource) != std::string::npos)
							} // if (iterI->isMember("source"))
						} // if (iterI->isObject())
					} // for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
					if (viterFound.size() > 1)
					{ // more than one found matching the desired MJD
						std::vector<Json::ValueConstIterator> viterInstrFound;
						for (std::vector<Json::ValueConstIterator>::iterator iterMJD = viterFound.begin(); iterMJD != viterFound.end(); iterMJD++)
						{
							if ((*iterMJD)->isMember("instrument"))
							{
								Json::Value vInstrument = (*iterMJD)->get("instrument",Json::Value());
								std::string szInstr = vInstrument.asString();
								if (szInstr == iterFit->m_szInstrument)
									viterInstrFound.push_back((*iterMJD));
							} // if ((*iterMJD)->isMember("instrument"))
						} // for (std::vector<Json::ValueConstIterator>::iterator iterMJD = viterFound.begin() ...
						if (viterInstrFound.size() > 1)
						{
							std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Multiple instruments found that match." << std::endl;
						} // if (viterInstrFound.size() > 1)
						else if (viterInstrFound.size() == 0)
						{
							std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "No instruments found that match." << std::endl;
						}
						
						viterFound = viterInstrFound;
					} // if (viterFound.size() > 1)
					if (viterFound.size() == 1)
					{ // exactly one spectra found with the matching 
						std::cout << "Found source " << iterFit->m_szSource << " with MJD " << std::setprecision(7) << iterFit->m_dMJD;
						if (!iterFit->m_szInstrument.empty())
						{
							if (viterFound[0]->isMember("instrument"))
							{
								Json::Value vInstrument = viterFound[0]->get("instrument",Json::Value());
								std::string szInstr = vInstrument.asString();
								if (szInstr == iterFit->m_szInstrument)
									std::cout << " and instrument " << iterFit->m_szInstrument;
								else
								{
									std::cout << " but instrument differs (req. " << iterFit->m_szInstrument << ", found " << szInstr << ")";
									iterFit->m_szInstrument= szInstr;
								}
							}
							else
								std::cout << " with no instrument specified (req. " << iterFit->m_szInstrument << ")";
						} // if (!iterFit->m_szInstrument.empty())
						else if (viterFound[0]->isMember("instrument"))
						{ // fill in the instrument if none is specified
							Json::Value vInstrument = viterFound[0]->get("instrument",Json::Value());
							iterFit->m_szInstrument = vInstrument.asString();
						}

						std::cout << std::endl;
						
						if (viterFound[0]->isMember("data"))
						{
							Json::Value jvSpec_Data = (*viterFound[0])["data"];
							if (jvSpec_Data.isArray())
							{
								for (Json::ValueConstIterator iterI = jvSpec_Data.begin(); iterI != jvSpec_Data.end(); iterI++)
								{
									Json::Value jvCurr = (*iterI);
									if (jvCurr.isArray())
									{
										double dWL = std::stod(std::string(jvCurr[0].asString()));
										double dFlux = std::stod(std::string(jvCurr[1].asString()));
										double dFlux_Error = 0.0;//std::stod(std::string(jvCurr[0].asString()));
										if (jvCurr.size() == 3)
										{
											dFlux_Error = std::stod(std::string(jvCurr[2].asString()));
										}
										iterFit->m_vData.push_back(std::tuple<double, double, double>(dWL,dFlux,dFlux_Error));
									}
								}
							}
						}
					} // if (viterFound.size() == 1)
					else
					{
						std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Unable to find source " << iterFit->m_szSource << " with MJD " << std::setprecision(7) << iterFit->m_dMJD;
						if (!iterFit->m_szInstrument.empty())
							std::cerr << " and instrument " << iterFit->m_szInstrument;
						std::cerr << std::endl;
						std::cerr << xconsole::bold << "MJD available for source: " << xconsole::reset << std::endl;
						for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
						{
							if (iterI->isObject())
							{
								if (iterI->isMember("source"))
								{
									Json::Value vSource = iterI->get("source",Json::Value());
									std::string sSource = vSource.asString();

									if (sSource.find(iterFit->m_szSource) != std::string::npos)
									{
										if (iterI->isMember("time"))
										{
											Json::Value vTime = iterI->get("time",Json::Value());
											std::cerr << vTime.asString();
										}
										if (iterI->isMember("instrument"))
										{
											Json::Value vTime = iterI->get("instrument",Json::Value());
											std::cerr << " --- " << vTime.asString();
										}
										std::cerr << std::endl;

									} /// if (sSource.find(iterFit->m_szSource) != std::string::npos)
								} // if (iterI->isMember("source"))
							} // if (iterI->isObject())
						} // for (Json::ValueConstIterator iterI = jvSpectra.begin(); ...
					} // if (viterFound.size() == 1) .. else
				} //if (jvSpectra.isArray())
			} //if (jvData[szID].isMember("spectra")) // confirm that there is at least one spectrum in this file
		} //if (mJson_Data.count(iterFit->m_szDatafile) > 0) /// looking at json data file
	}// for (std::vector <specfit::fit>::iterator iterFit = o_vfitFits.begin(); ...
}
//------------------------------------------------------------------------------
//
//
//
//	Load_Data
//
//
//
//------------------------------------------------------------------------------

void Deredden(std::vector <specfit::fit> &io_vfitFits)
{
	// process each model, and make sure that it has all of the needed info; fill the data points in the vector in the fit class for each one.
	for (std::vector <specfit::fit>::iterator iterFit = io_vfitFits.begin(); iterFit != io_vfitFits.end(); iterFit++)
	{
		if (!std::isnan(iterFit->m_dE_BmV) && iterFit->m_dE_BmV != 0.0 && !iterFit->m_vData.empty())
		{
			std::cout << "Dereddending " << std::setprecision(7) << iterFit->m_dMJD << " from " << iterFit->m_szSource << " using " << iterFit->m_szInstrument << " with E(B-v) = " << iterFit->m_dE_BmV << std::endl;
			CCM_dered(iterFit->m_vData, iterFit->m_dE_BmV);
		}
	}
}

//------------------------------------------------------------------------------
//
//
//
//	Write_Fit
//
//
//
//------------------------------------------------------------------------------

void Write_Fit(std::ofstream & io_ofsFile, const specfit::fit_result & i_cResult)
{
	io_ofsFile << std::setprecision(7) << i_cResult.m_dMJD;
	switch (i_cResult.m_eFeature)
	{
	case specfit::CaNIR:
		io_ofsFile << ", " << "Ca NIR";
		break;
	case specfit::CaHK:
		io_ofsFile << ", " << "CaHK";
		break;
	case specfit::Si6355:
		io_ofsFile << ", " << "Si 6355";
		break;
	case specfit::O7773:
		io_ofsFile << ", " << "O 7773";
		break;
	}

	io_ofsFile << ", \"" << i_cResult.m_szSource << "\"";
	io_ofsFile << ", " << i_cResult.m_szInstrument;
	io_ofsFile << ", " << i_cResult.m_uiModel;
	io_ofsFile << ", " << std::setprecision(4) << i_cResult.m_cParams[specfit::comp_ejecta].m_dPS_Vel;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dPS_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dLog_S;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dExcitation_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dPS_Vel;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dPS_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dLog_S;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dExcitation_Temp;

	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdRaw_Moments.cbegin(); iterI != i_cResult.m_vdRaw_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdCentral_Moments.cbegin(); iterI != i_cResult.m_vdCentral_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdStandardized_Moments.cbegin(); iterI != i_cResult.m_vdStandardized_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	io_ofsFile << std::endl;

	std::ostringstream ossSpectra_Data_File;
	std::string szInst_File_Friendly = i_cResult.m_szInstrument;
	for (std::string::iterator iterI = szInst_File_Friendly.begin(); iterI != szInst_File_Friendly.end(); iterI++)
	{
		if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
			*iterI = '_';
	}
	std::string szSource_File_Friendly = i_cResult.m_szSource;
	for (std::string::iterator iterI = szSource_File_Friendly.begin(); iterI != szSource_File_Friendly.end(); iterI++)
	{
		if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
			*iterI = '_';
	}
	ossSpectra_Data_File << "Results/spectra_" << std::setprecision(7) << i_cResult.m_dMJD << "_" << szInst_File_Friendly << "_source" << szSource_File_Friendly << "_model" << i_cResult.m_uiModel << ".csv";
	
	std::ofstream ofsSpectra;
	ofsSpectra.open(ossSpectra_Data_File.str().c_str());
	if (ofsSpectra.is_open())
	{
		for (unsigned int uiI = 0; uiI < i_cResult.m_vpdSpectrum_Target.size(); uiI++)
		{
			ofsSpectra << std::setprecision(10) << i_cResult.m_vpdSpectrum_Target[uiI].first;
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Target[uiI].second;
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic[uiI].second;
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Continuum[uiI].second;
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Ejecta_Only[uiI].second;
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Shell_Only[uiI].second;
			ofsSpectra << std::endl;
		}
		ofsSpectra.close();
	}
}

//------------------------------------------------------------------------------
//
//
//
//	Perform_Fits
//
//
//
//------------------------------------------------------------------------------
typedef std::vector < specfit::fit_result > best_fit_results;
void Perform_Fits(std::vector <specfit::fit> &i_vfitFits, const std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists, std::map< unsigned int, specfit::model> &i_mModel_Data, best_fit_results & o_vResults, bool i_bDebug = false, const specfit::param_set * i_lppsEjecta = nullptr, const specfit::param_set * i_lppsShell = nullptr)
{
	for (std::vector <specfit::fit>::iterator iterFit = i_vfitFits.begin(); iterFit != i_vfitFits.end(); iterFit++)
	{
		if (!iterFit->m_vData.empty())
		{
			specfit::fit_result	cBest_Fit_Result;
			double		m_dBest_Fit = DBL_MAX;
			std::cout << "Starting ";
			switch (iterFit->m_eFeature)
			{
			case specfit::CaNIR:
				std::cout << "Ca NIR";
				break;
			case specfit::CaHK:
				std::cout << "Ca H&K";
				break;
			case specfit::Si6355:
				std::cout << "Si 6355";
				break;
			case specfit::O7773:
				std::cout << "O 7773";
				break;
			}
			std::cout << " fit for MJD " <<std::setprecision(7) <<  iterFit->m_dMJD << " from Ref. " << iterFit->m_szSource << " using " << iterFit->m_szInstrument << std::endl;
			std::vector<specfit::fit_result> vFit_Results;

			// process individual models
			for (std::vector <unsigned int>::iterator 	iterMod = iterFit->m_vuiModels.begin(); iterMod != iterFit->m_vuiModels.end(); iterMod++)
			{
				if (i_mModel_Data.count(*iterMod) > 0 ) // make sure that this model exists
				{
					std::cout << "Model # " << *iterMod << std::endl;
					specfit::fit_result	cFit_Result;
					double dFit = GenerateFit(*iterFit, i_mModel_Data[*iterMod], cFit_Result, i_bDebug,i_lppsEjecta,i_lppsShell);
					if (dFit >= 0.0 && dFit < m_dBest_Fit)
					{
						cBest_Fit_Result = cFit_Result;
						m_dBest_Fit = dFit;
					}
					std::ofstream ofsIndividual_Fit_File;
					if (i_lppsEjecta != nullptr || i_lppsShell != nullptr)
						ofsIndividual_Fit_File.open("Results/individual_fit_data_single.csv",std::ios_base::app);
					else
						ofsIndividual_Fit_File.open("Results/individual_fit_data.csv",std::ios_base::app);
					Write_Fit(ofsIndividual_Fit_File,cFit_Result);
					ofsIndividual_Fit_File.close();
				}
			}
			// process model lists
			for (std::vector <std::string>::iterator 	iterModList = iterFit->m_vsModel_Lists.begin(); iterModList != iterFit->m_vsModel_Lists.end(); iterModList++)
			{
				if (i_mModel_Lists.count(*iterModList) > 0 ) // make sure that this model list exists
				{
					std::vector <unsigned int> vModels = i_mModel_Lists.at(*iterModList);
					for (std::vector <unsigned int>::const_iterator 	iterMod = vModels.cbegin(); iterMod != vModels.cend(); iterMod++)
					{
						if (i_mModel_Data.count(*iterMod) > 0 ) // make sure that this model exists
						{
							std::cout << "Model # " << *iterMod << std::endl;
							specfit::fit_result	cFit_Result;
							double dFit = GenerateFit(*iterFit, i_mModel_Data[*iterMod], cFit_Result, i_bDebug,i_lppsEjecta,i_lppsShell);
							if (dFit >= 0.0 && dFit < m_dBest_Fit)
							{
								cBest_Fit_Result = cFit_Result;
								m_dBest_Fit = dFit;
							}
							std::ofstream ofsIndividual_Fit_File;
							if (i_lppsEjecta != nullptr || i_lppsShell != nullptr)
								ofsIndividual_Fit_File.open("Results/individual_fit_data_single.csv",std::ios_base::app);
							else
								ofsIndividual_Fit_File.open("Results/individual_fit_data.csv",std::ios_base::app);
							Write_Fit(ofsIndividual_Fit_File,cFit_Result);
							ofsIndividual_Fit_File.close();
						}
					}
				}
			}
			o_vResults.push_back(cBest_Fit_Result);

			std::cout << "Fit complete for ";
			switch (iterFit->m_eFeature)
			{
			case specfit::CaNIR:
				std::cout << "Ca NIR";
				break;
			case specfit::CaHK:
				std::cout << "Ca H&K";
				break;
			case specfit::Si6355:
				std::cout << "Si 6355";
				break;
			case specfit::O7773:
				std::cout << "O 7773";
				break;
			}
			std::cout << " on MJD " << std::setprecision(7) << iterFit->m_dMJD << " from Ref. " << iterFit->m_szSource << " using " << iterFit->m_szInstrument << std::endl;
		} // if (!iterFit->m_vData.empty())
	}// for (std::vector <specfit::fit>::iterator iterFit = i_vfitFits.begin(); ...
}
//------------------------------------------------------------------------------
//
//
//
//	Output_Result_Header
//
//
//
//------------------------------------------------------------------------------

void Output_Result_Header(std::ofstream & io_ofsFile)
{
	io_ofsFile << "MJD, Feature, Sources, Instrument, Model #, PS Velocity (Ejecta), PS Temperature (Ejecta), Log S (Ejecta), Excitation Temp (Ejecta), PS Velocity (Ejecta), PS Temperature (Ejecta), Log S (Shell), Exctitation Temp (Shell)";
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Raw Fit Moment " << uiI;
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Central Fit Moment " << uiI;
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Standardized Fit Moment " << uiI;
	io_ofsFile <<  std::endl;
}
//------------------------------------------------------------------------------
//
//
//
//	Output_Results
//
//
//
//------------------------------------------------------------------------------

void Output_Results(std::ofstream & io_fsBest_Fits, const best_fit_results & i_vResults)
{

	for (best_fit_results::const_iterator iterI = i_vResults.cbegin(); iterI != i_vResults.cend(); iterI++)
	{
		Write_Fit(io_fsBest_Fits,*iterI);
	}

}


//------------------------------------------------------------------------------
//
//
//
//	main
//
//
//
//------------------------------------------------------------------------------

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	std::vector<std::string> vCL_Arguments;
	std::vector<std::string> vFile_List;
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		vCL_Arguments.push_back(std::string(i_lpszArg_Values[uiI]));
	}
	bool bDebug = false;
	bool bSingle = false;
	for (std::vector<std::string>::iterator iterI = vCL_Arguments.begin(); iterI != vCL_Arguments.end(); iterI++)
	{
		if (iterI->find(".xml") != std::string::npos)
			vFile_List.push_back(*iterI);
		else if (*iterI == "--debug")
			bDebug = true;
		else if ((iterI->substr(0,8) == "--ps-vel") || (iterI->substr(0,9) == "--ps-temp") || (iterI->substr(0,4) == "--Se") || (iterI->substr(0,4) == "--Ss"))
			bSingle = true;
		else
			std::cerr << "Unrecognized command line parameter " << *iterI << std::endl;
	}
	specfit::param_set cRef_Data_E;
	specfit::param_set cRef_Data_S;
	if (bSingle)
	{
		cRef_Data_E.m_dPS_Vel = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-vel", 20.0);
		cRef_Data_E.m_dPS_Temp = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-temp", 10.0);
		cRef_Data_E.m_dExcitation_Temp = 10000.0;
		cRef_Data_E.m_dLog_S = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--Se", 3.0);
		cRef_Data_S = cRef_Data_E;
		cRef_Data_S.m_dLog_S = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--Ss", 4.0);
	}
	if (bDebug)
	{
		DIR * fileDir = opendir(".debug");

		if (fileDir != nullptr)
		{
			closedir(fileDir);
			system("rm .debug/*");
		}
		else
		{
			if (mkdir(".debug",S_IRWXU) != 0)
			{
				std::cerr << xconsole::foreground_red << xconsole::bold << "Error: " << xconsole::reset << "Unable to create .debug directory" << std::endl;
				return 1;
			}
		}	
	}

	std::ifstream ifsTest;
	if (!bSingle)
	{
		ifsTest.open("Results/individual_fit_data.csv");
		if (!ifsTest.is_open())
		{
			std::ofstream ofsFile;
			ofsFile.open("Results/individual_fit_data.csv",std::ios_base::app);
			assert(ofsFile.is_open());
			Output_Result_Header(ofsFile);
			ofsFile.close();
		}
		else
			ifsTest.close();
	}
	else
	{
		ifsTest.open("Results/individual_fit_data_single.csv");
		if (!ifsTest.is_open())
		{
			std::ofstream ofsFile;
			ofsFile.open("Results/individual_fit_data_single.csv",std::ios_base::app);
			assert(ofsFile.is_open());
			Output_Result_Header(ofsFile);
			ofsFile.close();
		}
		else
			ifsTest.close();
	}

	

	if (!bSingle)
	{
		ifsTest.open("Results/best_fit_data.csv");
		if (!ifsTest.is_open())
		{
			std::ofstream ofsFile;
			ofsFile.open("Results/best_fit_data.csv",std::ios_base::app);
			assert(ofsFile.is_open());
			Output_Result_Header(ofsFile);
			ofsFile.close();
		}
		else
			ifsTest.close();
	}

	if (bDebug)
		std::cout << xconsole::bold << "Debug mode active" << xconsole::reset << std::endl;

	for (std::vector<std::string>::iterator iterI = vFile_List.begin(); iterI != vFile_List.end(); iterI++)
	{
		std::vector <specfit::fit> vfitFits;
		std::map< unsigned int, specfit::model> mModel_Data;
		std::map< std::string, std::vector<unsigned int> > mModel_Lists;
		std::map< std::string, std::string > mDatafile_List;
		std::map< std::string, Json::Value> mJson_Data;
		std::map< std::string, std::vector<std::tuple<double,double,double> > > mNon_Json_Data;
		best_fit_results vResults;
		std::vector < std::vector < std::pair< gauss_fit_results, gauss_fit_results > > > vGauss_Fits_Results;

		Parse_XML( *iterI, vfitFits, mModel_Data, mModel_Lists, mDatafile_List);
		Load_Data_Files( mDatafile_List, mJson_Data, mNon_Json_Data);
		Validate_JSON_Data( mJson_Data);
		Load_Models( vfitFits, mModel_Lists, mModel_Data);
		Load_Data( vfitFits, mJson_Data, mNon_Json_Data);

		
		// Deredden spectra if requested
		Deredden( vfitFits);
		// Done confirming and loading the data; begin the actual fitting
		if (bSingle)
			Perform_Fits( vfitFits, mModel_Lists, mModel_Data, vResults, bDebug, &cRef_Data_E, &cRef_Data_S);
		else
			Perform_Fits( vfitFits, mModel_Lists, mModel_Data, vResults, bDebug);

		// process the results to get Gaussian fits
//			Process_Results( vResults
//                  vA_Flat = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
//                                  cParam.m_dWavelength_Delta_Ang, dpEW_Flat_PVF, dpEW_Flat_HVF, dV_Flat_PVF, dV_Flat_HVF,
//									vSigma_Flat, dSmin_Flat,&cSingle_Flat_Fit,&cDouble_Flat_Fit);

		if (!bSingle)
		{
			std::ofstream ofsBest_Fits;
			ofsBest_Fits.open("Results/best_fit_data.csv",std::ios_base::app);
			Output_Results(ofsBest_Fits,vResults);
			if (ofsBest_Fits.is_open())
				ofsBest_Fits.close();
		}
	}
	return 0;
}
