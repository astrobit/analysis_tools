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
//	Load_Data
//
//
//
//------------------------------------------------------------------------------

void specfit::Load_Data(
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
