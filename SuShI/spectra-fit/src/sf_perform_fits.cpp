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
//	Perform_Fits
//
//
//
//------------------------------------------------------------------------------
void specfit::Perform_Fits(std::vector <specfit::fit> &i_vfitFits, const std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists, std::map< unsigned int, model> &i_mModel_Data, best_fit_results & o_vResults, bool i_bDebug, const specfit::param_set * i_lppsEjecta, const specfit::param_set * i_lppsShell, const bool * i_lpbPerform_Single_Fit)
{
	bool bTarget_Data_Written = false;
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
					double dFit = GenerateFit(*iterFit, i_mModel_Data[*iterMod], cFit_Result, i_bDebug,i_lppsEjecta,i_lppsShell,i_lpbPerform_Single_Fit);
					if (dFit >= 0.0 && dFit < m_dBest_Fit)
					{
								//std::cout << "save best fit" << std::endl;
								//std::cout.flush();
						cBest_Fit_Result = cFit_Result;
						m_dBest_Fit = dFit;
								//std::cout << "save best fit done" << std::endl;
								//std::cout.flush();
					}
					std::ofstream ofsIndividual_Fit_File;
					if (i_lppsEjecta != nullptr || i_lppsShell != nullptr)
						ofsIndividual_Fit_File.open("Results/individual_fit_data_single.csv",std::ios_base::app);
					else
						ofsIndividual_Fit_File.open("Results/individual_fit_data.csv",std::ios_base::app);
					Write_Fit(ofsIndividual_Fit_File,cFit_Result);
					ofsIndividual_Fit_File.close();

					//std::cout << "model fit data start" << std::endl;
					std::ofstream ofsModel_Fit_File;
					std::ostringstream ossModel_Fit_File;
					ossModel_Fit_File << "Results/model_fit_data_" << cFit_Result.m_uiModel << ".csv";
					ofsModel_Fit_File.open(ossModel_Fit_File.str().c_str(),std::ios_base::app);
					Write_Fit(ofsModel_Fit_File,cFit_Result);
					ofsModel_Fit_File.close();
					//std::cout << "model fit data done" << std::endl;
					if (!bTarget_Data_Written)
					{
					//std::cout << "target fit data start" << std::endl;
						std::ofstream ofsTarget_Data_File;
						ofsTarget_Data_File.open("Results/target_fit_data.csv",std::ios_base::app);
						Write_Target_Fit(ofsTarget_Data_File,cFit_Result);
						ofsTarget_Data_File.close();
						bTarget_Data_Written = true;
					}
					//std::cout << "target fit data done" << std::endl;
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
							double dFit = GenerateFit(*iterFit, i_mModel_Data[*iterMod], cFit_Result, i_bDebug,i_lppsEjecta,i_lppsShell,i_lpbPerform_Single_Fit);
							if (dFit >= 0.0 && dFit < m_dBest_Fit)
							{
								//std::cout << "save best fit" << std::endl;
								//std::cout.flush();
								cBest_Fit_Result = cFit_Result;
								m_dBest_Fit = dFit;
								//std::cout << "save best fit done" << std::endl;
								//std::cout.flush();
							}
							std::ofstream ofsIndividual_Fit_File;
							if (i_lppsEjecta != nullptr || i_lppsShell != nullptr)
								ofsIndividual_Fit_File.open("Results/individual_fit_data_single.csv",std::ios_base::app);
							else
								ofsIndividual_Fit_File.open("Results/individual_fit_data.csv",std::ios_base::app);
							Write_Fit(ofsIndividual_Fit_File,cFit_Result);
							ofsIndividual_Fit_File.close();

					//std::cout << "model fit data start" << std::endl;
							std::ofstream ofsModel_Fit_File;
							std::ostringstream ossModel_Fit_File;
							ossModel_Fit_File << "Results/model_fit_data_" << cFit_Result.m_uiModel << ".csv";
							ofsModel_Fit_File.open(ossModel_Fit_File.str().c_str(),std::ios_base::app);
							Write_Fit(ofsModel_Fit_File,cFit_Result);
							ofsModel_Fit_File.close();
					//std::cout << "model fit data done" << std::endl;

							if (!bTarget_Data_Written)
							{
					//std::cout << "target fit data start" << std::endl;
								std::ofstream ofsTarget_Data_File;
								ofsTarget_Data_File.open("Results/target_fit_data.csv",std::ios_base::app);
								Write_Target_Fit(ofsTarget_Data_File,cFit_Result);
								ofsTarget_Data_File.close();
								bTarget_Data_Written = true;
							}
					//std::cout << "target fit data done" << std::endl;
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
