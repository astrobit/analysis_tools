#include <iostream>
#include <fstream>
#include <OSC.hpp>
#include <xastro.h>
#include <xstdlib.h>
#include <cmath>

void OSCfile::Load(const std::string &i_sFilename)
{
	if (i_sFilename.find(".json") != std::string::npos)
	{
		std::ifstream fsData_File;
		fsData_File.open(i_sFilename.c_str(), std::ifstream::binary);
		if (fsData_File.is_open())
		{
			m_szFilename = i_sFilename;
			fsData_File >> m_jvDatafile;
			fsData_File.close();
			
			Json::Value::Members jmMember_List = m_jvDatafile.getMemberNames();
			std::string szID = jmMember_List[0];
			m_szSupernova_ID = szID;

			if (m_jvDatafile[szID].isMember("sources")) // confirm that there is at least one source in this file
			{ // display list of sources
				Json::Value vSources = m_jvDatafile[szID]["sources"];
				if (vSources.isArray())
				{
					for (Json::ValueConstIterator iterI = vSources.begin(); iterI != vSources.end(); iterI++)
					{
						if (iterI->isObject())
						{
							OSCreference cRef;
							if (iterI->isMember("alias"))
							{
								Json::Value vAlias = iterI->get("alias",Json::Value());
								cRef.m_szAlias = vAlias.asString();
							}
							else // if (iterI->isMember("alias"))
								cRef.m_szAlias = "X";
							if (iterI->isMember("reference"))
							{
								Json::Value vReference = iterI->get("reference",Json::Value());
								cRef.m_szReference = vReference.asString();
							} // if (iterI->isMember("reference"))
							if (iterI->isMember("name"))
							{
								Json::Value vReference = iterI->get("name",Json::Value());
								cRef.m_szName =  vReference.asString();
							} // if (iterI->isMember("name"))
							m_vsReference_List.push_back(cRef);
						} // if (iterI->isObject())
					} // for (Json::ValueConstIterator iterI = vSources.begin(); ...
				} // if (vSources.isArray())
			} // if (iterFiles->second[szID].isMember("sources"))

			if (m_jvDatafile[szID].isMember("ebv")) // confirm that there is at least one spectrum in this file
			{
				Json::Value jvEBV = m_jvDatafile[szID]["ebv"];
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
						if (dError == 0.0)
							dError = dValue * 0.001;
						double dWeight = 1.0 / (dError * dError);
						dEBV_Weighted_Sum += dValue * dWeight;
						dEBV_Error_Sum += dWeight;
					}
					m_dEBV_Mean = dEBV_Weighted_Sum / dEBV_Error_Sum;
				}
				else
				{
					Json::Value vValue = m_jvDatafile[szID]["ebv"].get("value",Json::Value());
					if (vValue.isConvertibleTo(Json::realValue))
						m_dEBV_Mean = vValue.asDouble();
					else
						m_dEBV_Mean = std::stod(std::string(vValue.asString()));
				}
			}
			if (m_jvDatafile[szID].isMember("redshift")) // confirm that there is at least one spectrum in this file
			{
				Json::Value jvEBV = m_jvDatafile[szID]["redshift"];
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
						if (dError == 0.0)
							dError = dValue * 0.001;
						double dWeight = 1.0 / (dError * dError);
						dEBV_Weighted_Sum += dValue * dWeight;
						dEBV_Error_Sum += dWeight;
					}
					m_dRedshift_Weighted_Mean = dEBV_Weighted_Sum / dEBV_Error_Sum;
				}
				else
				{
					Json::Value vValue = m_jvDatafile[szID]["redshift"].get("value",Json::Value());
					if (vValue.isConvertibleTo(Json::realValue))
						m_dRedshift_Weighted_Mean = vValue.asDouble();
					else
						m_dRedshift_Weighted_Mean = std::stod(std::string(vValue.asString()));
				}
			}
			if (m_jvDatafile[szID].isMember("spectra")) // confirm that there is at least one spectrum in this file
			{
				Json::Value jvSpectra = m_jvDatafile[szID]["spectra"];
				if (jvSpectra.isArray())
				{
					OSCspectra_id cID;
					for (Json::ValueConstIterator iterI = jvSpectra.begin(); iterI != jvSpectra.end(); iterI++)
					{
						if (iterI->isObject())
						{
							if (iterI->isMember("instrument"))
							{
								Json::Value vInstrument = iterI->get("instrument",Json::Value());
								cID.m_szInstrument = vInstrument.asString();
							}
							if (iterI->isMember("source"))
							{
								Json::Value vSource = iterI->get("source",Json::Value());
								cID.m_szSources = vSource.asString();
							}
							if (iterI->isMember("time"))
							{
								Json::Value vTime = iterI->get("time",Json::Value());
								if (vTime.isConvertibleTo(Json::realValue))
									cID.m_dDate_MJD = vTime.asDouble();
								else
									cID.m_dDate_MJD = std::stod(std::string(vTime.asString()));
							} // iterI->isMember("time")
							if (iterI->isMember("data"))
							{
								Json::Value jvSpec_Data = (*iterI)["data"];
								OSCspectrum cSpectrum;
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
											cSpectrum.push_back(OSCspectrum_dp(dWL,dFlux,dFlux_Error));
										}
									}
								}
								m_mSpectra_List[cID] = cSpectrum;
							} // if iterI->isMember("data")
						} //if (iterI->isObject())
					} // for (... iterI = jvSpectra.begin()...
				} //if (jvSpectra.isArray())
			} //if (jvData[szID].isMember("spectra")) // confirm that there is at least one spectrum in this file
		}
		else 
			std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << "Failed to open " << i_sFilename << std::endl;
	}
	else 
		std::cerr << xconsole::bold << xconsole::foreground_red << "Error: " << xconsole::reset << i_sFilename << " is not a .json file." << std::endl;
}

void OSCfile::Deredden(void)
{
	if (m_dEBV_Mean != 0.0 && !std::isnan(m_dEBV_Mean) && !std::isinf(m_dEBV_Mean))
	{
		for(std::map<OSCspectra_id, OSCspectrum >::iterator iterI = m_mSpectra_List.begin(); iterI != m_mSpectra_List.end(); iterI++)
		{
			iterI->second.Deredden(m_dEBV_Mean);
		}
	}
}


void OSCspectrum::Deredden(const double & i_dE_BmV)
{
	if (!std::isinf(i_dE_BmV) && !std::isnan(i_dE_BmV) && i_dE_BmV != 0.0 && !empty())
	{
		for (iterator iterI = begin(); iterI != end(); iterI++)
		{
			XA_CCM_dered(iterI->m_dWavelength, iterI->m_dFlux, i_dE_BmV);
		}
	}
}

void OSCfile::Unredshift(void)
{
	if (m_dRedshift_Weighted_Mean != 0.0 && !std::isnan(m_dRedshift_Weighted_Mean) && !std::isinf(m_dRedshift_Weighted_Mean))
	{
		for(std::map<OSCspectra_id, OSCspectrum >::iterator iterI = m_mSpectra_List.begin(); iterI != m_mSpectra_List.end(); iterI++)
		{
			iterI->second.Unredshift(m_dRedshift_Weighted_Mean);
		}
	}
}
void OSCspectrum::Unredshift(const double & i_dRedshift)
{
	if (!std::isinf(i_dRedshift) && !std::isnan(i_dRedshift) && i_dRedshift != 0.0 && !empty())
	{
		for (iterator iterI = begin(); iterI != end(); iterI++)
		{
			iterI->m_dWavelength = XA_Rest_Frame_Wavelength(iterI->m_dWavelength,1.0 + i_dRedshift);
		}
	}
}

OSCfile::OSCfile(void)
{
	m_dEBV_Mean = nan("");
	m_dRedshift_Weighted_Mean = nan("");
}

std::vector <std::tuple<double, double, double> > OSCspectrum::Get_Tuple_Vector(void) const
{
	std::vector <std::tuple<double, double, double> > vtRes;
	for(auto iterI = cbegin(); iterI != cend(); iterI++)
		vtRes.push_back(std::tuple<double, double, double> ( iterI->m_dWavelength, iterI->m_dFlux, iterI->m_dFlux_Error));
	return vtRes;
}

