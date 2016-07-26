#include<cstdio>
#include<cstdlib>
#include <xstdlib.h>
#include <json.h>
#include <string>
#include <fstream>
#include <ostream>
#include <iostream>
#include <limits>
#include <cfloat>
#include <cmath>
#include <xmath.h>

double Compute_MJD_Bmax(std::map<double, double> &i_mddPhotometry)
{
	double dMJD_Bmax = nan("");
	if (i_mddPhotometry.size() > 3)
	{
		double dLast = i_mddPhotometry.rbegin()->first;
		XSPLINE_DATA cSpline(i_mddPhotometry);
		double dMJD_min = i_mddPhotometry.begin()->first;
		double	dMag_Bmax = DBL_MAX;
		for (unsigned int uiI = 0 ; uiI < 50; uiI++)
		{
			double dMJD = dMJD_min + uiI * 0.5;
			if (dMJD < dLast)
			{
				if (cSpline.Interpolate(dMJD) < dMag_Bmax)
				{
					dMag_Bmax = cSpline.Interpolate(dMJD);
					dMJD_Bmax = dMJD;
				}
			}
		}
	}
	return dMJD_Bmax;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (i_iArg_Count > 1)
	{
		double dMJD_Start = 0;
		double dMJD_End = DBL_MAX;
		double dMJD_Bmax = nan("");
		std::vector<std::string> vsSource_Files;
		std::string szFeature = "CaNIR";
		bool bCompute_MJD_Bmax = false;
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			std::string szCommand =  i_lpszArg_Values[uiI];
			if (szCommand[0] == '-') // this is a option, not a filename
			{
				if (szCommand.find("--compute-mjd-bmax") != std::string::npos || szCommand.find("--Compute-MJD-Bmax") != std::string::npos)
				{
					bCompute_MJD_Bmax = true;
				}
				else if (szCommand.find("--mjd-bmax") != std::string::npos || szCommand.find("--MJD-Bmax") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							std::string szData = szCommand.substr(sPos + 1);
							dMJD_Bmax = std::stod(szData);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								std::string szData = i_lpszArg_Values[uiI];
								dMJD_Bmax = std::stod(szData);
							}
						}
					}
					else
					{
						uiI++;
						if (uiI < i_iArg_Count)
						{
							szCommand = i_lpszArg_Values[uiI];
							if (szCommand.find("=") != std::string::npos)
							{
								size_t sPos =  szCommand.find("=");
								if (szCommand.size() > sPos)
								{ // the data is probably here
									std::string szData = szCommand.substr(sPos + 1);
									dMJD_Bmax = std::stod(szData);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										std::string szData = i_lpszArg_Values[uiI];
										dMJD_Bmax = std::stod(szData);
									}
								}
							}
						}
					}
				}
				else if (szCommand.find("--max-MJD") != std::string::npos || szCommand.find("--max-mjd") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							std::string szData = szCommand.substr(sPos + 1);
							dMJD_End = std::stod(szData);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								std::string szData = i_lpszArg_Values[uiI];
								dMJD_End = std::stod(szData);
							}
						}
					}
					else
					{
						uiI++;
						if (uiI < i_iArg_Count)
						{
							szCommand = i_lpszArg_Values[uiI];
							if (szCommand.find("=") != std::string::npos)
							{
								size_t sPos =  szCommand.find("=");
								if (szCommand.size() > sPos)
								{ // the data is probably here
									std::string szData = szCommand.substr(sPos + 1);
									dMJD_End = std::stod(szData);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										std::string szData = i_lpszArg_Values[uiI];
										dMJD_End = std::stod(szData);
									}
								}
							}
						}
					}
				}
				else if (szCommand.find("--min-MJD") != std::string::npos || szCommand.find("--min-mjd") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							std::string szData = &(szCommand[sPos]);
							dMJD_Start = std::stod(szData);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								std::string szData = i_lpszArg_Values[uiI];
								dMJD_Start = std::stod(szData);
							}
						}
					}
					else
					{
						uiI++;
						if (uiI < i_iArg_Count)
						{
							szCommand = i_lpszArg_Values[uiI];
							if (szCommand.find("=") != std::string::npos)
							{
								size_t sPos =  szCommand.find("=");
								if (szCommand.size() > sPos)
								{ // the data is probably here
									std::string szData = &(szCommand[sPos]);
									dMJD_Start = std::stod(szData);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										std::string szData = i_lpszArg_Values[uiI];
										dMJD_Start = std::stod(szData);
									}
								}
							}
						}
					}
				}
				else if (szCommand.find("--feature") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							szFeature = &(szCommand[sPos]);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								szFeature = i_lpszArg_Values[uiI];
							}
						}
					}
					else
					{
						uiI++;
						if (uiI < i_iArg_Count)
						{
							szCommand = i_lpszArg_Values[uiI];
							if (szCommand.find("=") != std::string::npos)
							{
								size_t sPos =  szCommand.find("=");
								if (szCommand.size() > sPos)
								{ // the data is probably here
									szFeature = &(szCommand[sPos]);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										szFeature = i_lpszArg_Values[uiI];
									}
								}
							}
						}
					}
				}
				else
					std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "unreconized command line option " << szCommand << std::endl;
			}
			else
			{
				vsSource_Files.push_back(szCommand);
			}
		}
		std::ofstream fsOut_File;
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
		std::string szLA_Data_Path;
		if (lpszLA_Data_Path)
			szLA_Data_Path = lpszLA_Data_Path;
		fsOut_File.open("fits.xml");
		fsOut_File << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		fsOut_File << "<!DOCTYPE SPECFIT SYSTEM \"" << szLA_Data_Path << "/specfit.dtd\">" << std::endl;
		fsOut_File << "<SPECFIT>" << std::endl;
		fsOut_File << "<MODELLIST id=\"mass\">" << std::endl;
		fsOut_File << "\t<MODEL id=\"61\"/>" << std::endl;
		fsOut_File << "\t<MODEL id=\"57\"/>" << std::endl;
		fsOut_File << "\t<MODEL id=\"41\"/>" << std::endl;
		fsOut_File << "\t<MODEL id=\"49\"/>" << std::endl;
		fsOut_File << "\t<MODEL id=\"45\"/>" << std::endl;
		fsOut_File << "</MODELLIST>" << std::endl;
		std::string szFilename =  i_lpszArg_Values[1];
		for (unsigned int uiI = 0; uiI < vsSource_Files.size(); uiI++)
		{
			if (vsSource_Files[uiI].find(".json") != std::string::npos)
			{
				Json::Value jsonRoot;   // starts as "null"; will contain the root value after parsing
				std::ifstream fsData_File;
				fsData_File.open(vsSource_Files[uiI].c_str(), std::ifstream::binary);
				if (fsData_File.is_open())
				{
					fsData_File >> jsonRoot;
					fsData_File.close();
					Json::Value::Members jmMember_List = jsonRoot.getMemberNames();

					fsOut_File << "<DATAFILE filepath=\"" << vsSource_Files[uiI] << "\" id=\"file" << uiI << "\" />" << std::endl;
					std::string szID = jmMember_List[0];
					bool bEBV_Avail = jsonRoot[szID].isMember("ebv"); // check if the E(B-V) value is available
					std::map< double, double > mddPhotometry_Vega;
					std::map< double, double > mddPhotometry_CSP;
					std::map< double, double > mddPhotometry_Unknown;
					std::string szPhotometry_System;
					if (bCompute_MJD_Bmax)
					{
						if (jsonRoot[szID].isMember("photometry")) // confirm that there is at least one source in this file
						{
							Json::Value jvPhotometry = jsonRoot[szID]["photometry"];
							if (jvPhotometry.isArray())
							{
								for (Json::ValueConstIterator iterI = jvPhotometry.begin(); iterI != jvPhotometry.end(); iterI++)
								{
									if (iterI->isObject())
									{
										std::string szTime;
										std::string szBand;
										std::string szMagnitude;
										std::string szTimeUnit;
										std::string szSystem;

										if (iterI->isMember("time"))
										{
											Json::Value vValue = iterI->get("time",Json::Value());
											szTime = vValue.asString();
										}
										if (iterI->isMember("band"))
										{
											Json::Value vValue = iterI->get("band",Json::Value());
											szBand = vValue.asString();
										}
										if (iterI->isMember("magnitude"))
										{
											Json::Value vValue = iterI->get("magnitude",Json::Value());
											szMagnitude = vValue.asString();
										}
										if (iterI->isMember("system"))
										{
											Json::Value vValue = iterI->get("system",Json::Value());
											szSystem = vValue.asString();
										}
										if (iterI->isMember("u_time"))
										{
											Json::Value vValue = iterI->get("u_time",Json::Value());
											szTimeUnit = vValue.asString();
										}
												
										if (!szTime.empty() && !szBand.empty() && !szMagnitude.empty() && !szTimeUnit.empty())
										{
											if ( szTimeUnit == "MJD" && szBand == "B")
											{
												double dMJD = std::stod(szTime);
												double dMagnitude = std::stod(szMagnitude);
												if (szSystem == "Vega")
												{
													mddPhotometry_Vega[dMJD] = dMagnitude;
												}
												else if (szSystem == "CSP")
												{
													mddPhotometry_CSP[dMJD] = dMagnitude;
												}
												else if (szSystem.empty())
												{
													mddPhotometry_Unknown[dMJD] = dMagnitude;
												}
												else
													std::cout << "B-band photometry available in alternate system (" << szSystem << ")." << std::endl;
											}
										}
									}
								}
							}
						}

						double dBmax_Vega = Compute_MJD_Bmax(mddPhotometry_Vega);
						if (!std::isnan(dBmax_Vega))
							std::cout << "Estimated Vega B-band maximum on " << dBmax_Vega << std::endl;

						double dBmax_CSP = Compute_MJD_Bmax(mddPhotometry_CSP);
						if (!std::isnan(dBmax_CSP))
							std::cout << "Estimated CSP B-band maximum on " << dBmax_CSP << std::endl;

						double dBmax_Unknown = Compute_MJD_Bmax(mddPhotometry_Unknown);
						if (!std::isnan(dBmax_Unknown))
							std::cout << "Estimated B-band maximum (unknown system(s)) on " << dBmax_Unknown << std::endl;

						unsigned int uiMag_Count = 0;
						dMJD_Bmax = 0.0;
						if (!std::isnan(dBmax_Vega)) // Vega system is weighted more heavily
						{
							dMJD_Bmax += dBmax_Vega * 2.0;
							uiMag_Count+=2;
						}
						if (!std::isnan(dBmax_CSP))
						{
							dMJD_Bmax += dBmax_CSP;
							uiMag_Count++;
						}
						if (!std::isnan(dBmax_Unknown))
						{
							dMJD_Bmax += dBmax_Unknown;
							uiMag_Count++;
						}

						if (uiMag_Count > 0)
						{
							dMJD_Bmax /= uiMag_Count;
							dMJD_End = dMJD_Bmax + 5.0;
							dMJD_Start = dMJD_Bmax - 25.0;
						}
						else
						{
							std::cout << "B-band maximum could not be computed. Exiting." << std::endl;
							return -1;
						}

					}

					if (jsonRoot[szID].isMember("spectra")) // confirm that there is at least one source in this file
					{
						Json::Value jvSpectra = jsonRoot[szID]["spectra"];
						if (jvSpectra.isArray())
						{
							for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
							{
								if (iterI->isObject())
								{
									std::string szSources;
									std::string szInstrument;
									std::string szMJD;

									if (iterI->isMember("source"))
									{
										Json::Value vValue = iterI->get("source",Json::Value());
										szSources = vValue.asString();
							
									} // if (iterI->isMember("source"))
									if (iterI->isMember("instrument"))
									{
										Json::Value vValue = iterI->get("instrument",Json::Value());
										szInstrument = vValue.asString();
							
									} // if (iterI->isMember("source"))
									if (iterI->isMember("time"))
									{
										Json::Value vValue = iterI->get("time",Json::Value());
										szMJD = vValue.asString();
							
									} // if (iterI->isMember("time"))

									bool bInclude = true;
									if (!szMJD.empty())
									{
										double dMJD = std::stod(szMJD);
										bInclude = (dMJD >= dMJD_Start && dMJD <= dMJD_End);
									}
									if (bInclude)
									{
										fsOut_File << "<FIT";
										if (!szSources.empty())
											fsOut_File << " source=\"" << szSources << "\"";
										if (!szMJD.empty())
											fsOut_File << " mjd=\"" << szMJD << "\"";
										if (!szInstrument.empty())
											fsOut_File << " instrument=\""<< szInstrument << "\"";
										fsOut_File << " feature=\"" << szFeature << "\" datafileid=\"file" << uiI << "\"";
										if(bEBV_Avail)
											fsOut_File << " usejsonebv=\"true\"";
										if (!std::isnan(dMJD_Bmax))
											fsOut_File << " mjdbmax=\"" << dMJD_Bmax << "\" scalestartwithtime=\"true\"";
										fsOut_File << " >" << std::endl;
										fsOut_File << "\t<MODLIST id=\"mass\"/>" << std::endl;
										fsOut_File << "</FIT>	" << std::endl;
									} // if (bInclude)
								} // if (iterI->isObject())
							} // for (Json::ValueConstIterator iterI = jvSpectra.begin(); ...
						} // if (jvSpectra.isArray())
					}
					else
						std::cerr << xconsole::bold << xconsole::foreground_red << "Warning: " << xconsole::reset << "No spectra in file." << std::endl;
				
				}
				else 
					std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Failed to open " << szFilename << std::endl;
			}
		}
		fsOut_File << "</SPECFIT>" << std::endl;
	}

	return 0;
}
