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

void specfit::Parse_XML(const std::string &i_szFilename,
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

