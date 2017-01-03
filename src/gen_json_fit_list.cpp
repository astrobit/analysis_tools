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
#include <xastro.h>
#include <iomanip>

double Compute_MJD_Bmax(std::map<double, double> &i_mddPhotometry)
{
	//@@TODO: this is doing a really bad job of fitting due to noisy data
	// need to modifiy to try a better fitting method
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
		double	dWL_Min_Req = 7500.0;
		double	dWL_Max_Req = 9000.0;
		bool bCompute_MJD_Bmax = false;
		bool bUse_MJD_Max = false;
		std::string szTACC_Account;
		std::string szJob_Email;
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			std::string szCommand =  i_lpszArg_Values[uiI];
			if (szCommand[0] == '-') // this is a option, not a filename
			{
				if (szCommand.find("--TACC-account") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							szTACC_Account = szCommand.substr(sPos + 1);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								szTACC_Account = i_lpszArg_Values[uiI];
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
									szTACC_Account = szCommand.substr(sPos + 1);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										szTACC_Account = i_lpszArg_Values[uiI];
									}
								}
							}
						}
					}
				}
				else if (szCommand.find("--TACC-email") != std::string::npos)
				{
					if (szCommand.find("=") != std::string::npos)
					{
						size_t sPos =  szCommand.find("=");
						if (szCommand.size() > sPos)
						{ // the data is probably here
							szJob_Email = szCommand.substr(sPos + 1);
						}
						else
						{
							uiI++;
							if (uiI < i_iArg_Count)
							{
								szJob_Email = i_lpszArg_Values[uiI];
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
									szJob_Email = szCommand.substr(sPos + 1);
								}
								else
								{
									uiI++;
									if (uiI < i_iArg_Count)
									{
										szJob_Email = i_lpszArg_Values[uiI];
									}
								}
							}
						}
					}
				}
				else if (szCommand.find("--use-mjd-max") != std::string::npos || szCommand.find("--use-MJD-max") != std::string::npos)
				{
					bUse_MJD_Max = true;
				}
				else if (szCommand.find("--compute-mjd-bmax") != std::string::npos || szCommand.find("--Compute-MJD-Bmax") != std::string::npos)
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
							std::string szData = szCommand.substr(sPos + 1);
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
									std::string szData = szCommand.substr(sPos + 1);
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
									szFeature = szCommand.substr(sPos + 1);
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
					std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "unrecognized command line option " << szCommand << std::endl;
			}
			else
			{
				vsSource_Files.push_back(szCommand);
			}
		}

		// what is the requisite range for which spectral data must be available to try to fit?
		if (szFeature == "Si6355")
		{
			dWL_Max_Req = 7000.0;
			dWL_Min_Req = 5000.0;
		}
		//@@TODO other feature ranges
		std::ofstream fsOut_File;
		std::ofstream fsJob_File_Batch;

		if (!szTACC_Account.empty())
		{
			fsJob_File_Batch.open("job_fit_all");
			fsJob_File_Batch << "#!/bin/tcsh" << std::endl;
		}
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
		std::string szLA_Data_Path;
		if (lpszLA_Data_Path)
			szLA_Data_Path = lpszLA_Data_Path;
		fsOut_File.open("xml/fits.xml");
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
				unsigned int uiSpectra_Count = 1;
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
					if (bUse_MJD_Max)
					{
						if (jsonRoot[szID].isMember("maxdate")) // confirm that there is at least max date in this file
						{
							Json::Value jvMaxDate = jsonRoot[szID]["maxdate"];
							if (jvMaxDate.isArray())
							{
								double dMJD = 0.0;
								unsigned int uiNum_Terms = 0;
								for (Json::ValueConstIterator iterI = jvMaxDate.begin(); iterI != jvMaxDate.end(); iterI++)
								{
									if ((*iterI).isMember("value")) // confirm that there is at value within the max date
									{
										std::string szDate = (*iterI)["value"].asString();
										std::string szYear = szDate.substr(0,4);
										std::string szMonth = szDate.substr(5,2);
										std::string szDay = szDate.substr(8,2);
										calendar_date cDate;
										cDate.m_iYear = std::stoi(szYear);;
										cDate.m_uiMonth = std::stoi(szMonth);
										cDate.m_uiDay = std::stoi(szDay);
										std::cout << "max date " << cDate.m_uiDay << "/" << cDate.m_uiMonth << "/" << cDate.m_iYear << std::endl;
							
										dMJD += XA_MJD(cDate);
										uiNum_Terms++;
								}
									}
								if (uiNum_Terms == 0) 
									std::cerr << xconsole::bold << xconsole::foreground_yellow << "Warning: " << xconsole::reset << "could not find value for maxdate" << std::endl;
								else
								{
									dMJD_Bmax = dMJD / uiNum_Terms;
									std::cout << "Estimated B-band maximum on " << dMJD_Bmax << std::endl;
								}
							}
						}
						else 
							std::cerr << xconsole::bold << xconsole::foreground_yellow << "Warning: " << xconsole::reset << "could not find maxdate" << std::endl;
 					}
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
//											std::cout << "photometry->time" << std::endl;
											if (!(*iterI)["time"].isArray())
											{
												Json::Value vValue = iterI->get("time",Json::Value());
												if (vValue.isString())
													szTime = vValue.asString();
												else if (vValue.isNumeric())
												{
													double dVal;
													if (vValue.isDouble())
														dVal = vValue.asDouble();
													else if (vValue.isInt())
														dVal = vValue.asInt();
													else if (vValue.isUInt())
														dVal = vValue.asUInt();
													else
														std::cerr << "Could not identify numeric type for a photometry->time value." << std::endl;
													std::ostringstream ossVal;
													ossVal << dVal;
													szTime = ossVal.str();
												}
												else
													std::cerr << "Could not identify data type for a photometry->time value." << std::endl;
											}
											else
												szTime="0";
										}
										if (iterI->isMember("band"))
										{
//											std::cout << "photometry->band" << std::endl;
											Json::Value vValue = iterI->get("band",Json::Value());
											szBand = vValue.asString();
										}
										if (iterI->isMember("magnitude"))
										{
//											std::cout << "photometry->magnitude" << std::endl;
											Json::Value vValue = iterI->get("magnitude",Json::Value());
											szMagnitude = vValue.asString();
										}
										if (iterI->isMember("system"))
										{
//											std::cout << "photometry->system" << std::endl;
											Json::Value vValue = iterI->get("system",Json::Value());
											szSystem = vValue.asString();
										}
										if (iterI->isMember("u_time"))
										{
//											std::cout << "photometry->u_time" << std::endl;
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
									if (iterI->isMember("data"))
									{
										Json::Value jvSpec_Data = (*iterI)["data"];
										double dWL_Min = DBL_MAX;
										double dWL_Max = 0.0;
										if (jvSpec_Data.isArray())
										{
											for (Json::ValueConstIterator iterI = jvSpec_Data.begin(); iterI != jvSpec_Data.end(); iterI++)
											{
												Json::Value jvCurr = (*iterI);
												if (jvCurr.isArray())
												{
													double dWL = std::stod(std::string(jvCurr[0].asString()));
													if (dWL_Min > dWL)
														dWL_Min = dWL;
													if (dWL_Max < dWL)
														dWL_Max = dWL;
												}
											}
										}
										bInclude = (dWL_Min < dWL_Min_Req && dWL_Max > dWL_Max_Req);
									}
									if (bInclude)
									{
										std::ofstream fsOut_File_Single;
										std::ostringstream ossFilename;
										ossFilename << "xml/fit_" << szID << "_" << uiSpectra_Count << ".xml";

										if (!szTACC_Account.empty())
										{
											std::ostringstream ossSlurm_Filename;
											ossSlurm_Filename << "job/job_fit_" << szID << "_" << uiSpectra_Count;
											std::ofstream fsSlurm_File_Single;
											fsSlurm_File_Single.open(ossSlurm_Filename.str().c_str());
											fsSlurm_File_Single << "#!/bin/tcsh" << std::endl;
											fsSlurm_File_Single << "#SBATCH -J sf_fit_" << szID << "_" << uiSpectra_Count << "      #Job Name" << std::endl;
											fsSlurm_File_Single << "#SBATCH -o log/sf_fit_" << szID << "_" << uiSpectra_Count << ".o%j  #Output file name" << std::endl;
											fsSlurm_File_Single << "#SBATCH -N 1            # Allocate tasks on 1 nodes" << std::endl;
											fsSlurm_File_Single << "#SBATCH -n 1           # Number of MPI tasks, assumed to be 16 task/node" << std::endl;
											fsSlurm_File_Single << "#SBATCH -p normal        # Queue" << std::endl;
											fsSlurm_File_Single << "#SBATCH -t 8:00:00      # Run time (HH:mm:ss)" << std::endl;
											fsSlurm_File_Single << "#SBATCH -A " << szTACC_Account << "       # Account to charge" << std::endl;
											if (!szJob_Email.empty())
											{
												fsSlurm_File_Single << "#SBATCH --mail-user=" << szJob_Email << std::endl;
												fsSlurm_File_Single << "#SBATCH --mail-type=ALL" << std::endl;
											}
											fsSlurm_File_Single << "setenv OMP_NUM_THREADS 16 " << std::endl;
											fsSlurm_File_Single << "sf " << ossFilename.str() << std::endl;

											fsSlurm_File_Single.close();
											fsJob_File_Batch << "sbatch " << ossSlurm_Filename.str() << std::endl;
										}
										fsOut_File_Single.open(ossFilename.str().c_str());
										fsOut_File_Single << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
										fsOut_File_Single << "<!DOCTYPE SPECFIT SYSTEM \"" << szLA_Data_Path << "/specfit.dtd\">" << std::endl;
										fsOut_File_Single << "<SPECFIT>" << std::endl;
										fsOut_File_Single << "<MODELLIST id=\"mass\">" << std::endl;
										fsOut_File_Single << "\t<MODEL id=\"61\"/>" << std::endl;
										fsOut_File_Single << "\t<MODEL id=\"57\"/>" << std::endl;
										fsOut_File_Single << "\t<MODEL id=\"41\"/>" << std::endl;
										fsOut_File_Single << "\t<MODEL id=\"49\"/>" << std::endl;
										fsOut_File_Single << "\t<MODEL id=\"45\"/>" << std::endl;
										fsOut_File_Single << "</MODELLIST>" << std::endl;
										fsOut_File_Single << "<DATAFILE filepath=\"" << vsSource_Files[uiI] << "\" id=\"file" << uiI << "\" />" << std::endl;

										fsOut_File_Single << "<FIT";
										if (!szSources.empty())
											fsOut_File_Single << " source=\"" << szSources << "\"";
										if (!szMJD.empty())
											fsOut_File_Single << std::setprecision(7) << " mjd=\"" << szMJD << "\"";
										if (!szInstrument.empty())
											fsOut_File_Single << " instrument=\""<< szInstrument << "\"";
										fsOut_File_Single << " feature=\"" << szFeature << "\" datafileid=\"file" << uiI << "\"";
										if(bEBV_Avail)
											fsOut_File_Single << " usejsonebv=\"true\"";
										if (!std::isnan(dMJD_Bmax))
											fsOut_File_Single << std::setprecision(7) << " mjdbmax=\"" << dMJD_Bmax << "\" scalestartwithtime=\"true\"";
										fsOut_File_Single << " >" << std::endl;
										fsOut_File_Single << "\t<MODLIST id=\"mass\"/>" << std::endl;
										fsOut_File_Single << "</FIT>	" << std::endl;
										fsOut_File_Single << "</SPECFIT>" << std::endl;
										fsOut_File_Single.close();

										//output again to combined file
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
											fsOut_File << std::setprecision(7) << " mjdbmax=\"" << dMJD_Bmax << "\" scalestartwithtime=\"true\"";
										fsOut_File << " >" << std::endl;
										fsOut_File << "\t<MODLIST id=\"mass\"/>" << std::endl;
										fsOut_File << "</FIT>	" << std::endl;

										uiSpectra_Count++;
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
		fsOut_File.close();

		if (!szTACC_Account.empty())
		{
			std::ofstream fsSlurm_File_All;
			fsSlurm_File_All.open("job/job_fit");
			fsSlurm_File_All << "#!/bin/tcsh" << std::endl;
			fsSlurm_File_All << "#SBATCH -J sf_fit      #Job Name" << std::endl;
			fsSlurm_File_All << "#SBATCH -o log/sf_fit.o%j  #Output file name" << std::endl;
			fsSlurm_File_All << "#SBATCH -N 1            # Allocate tasks on 1 nodes" << std::endl;
			fsSlurm_File_All << "#SBATCH -n 1           # Number of MPI tasks, assumed to be 16 task/node" << std::endl;
			fsSlurm_File_All << "#SBATCH -p normal        # Queue" << std::endl;
			fsSlurm_File_All << "#SBATCH -t 48:00:00      # Run time (HH:mm:ss)" << std::endl;
			fsSlurm_File_All << "#SBATCH -A " << szTACC_Account << "       # Account to charge" << std::endl;
			if (!szJob_Email.empty())
			{
				fsSlurm_File_All << "#SBATCH --mail-user=" << szJob_Email << std::endl;
				fsSlurm_File_All << "#SBATCH --mail-type=ALL" << std::endl;
			}
			fsSlurm_File_All << "setenv OMP_NUM_THREADS 16" << std::endl;
			fsSlurm_File_All << "sf xml/fits.xml" << std::endl;

			fsSlurm_File_All.close();
			fsJob_File_Batch.close();
		}
	}

	return 0;
}
