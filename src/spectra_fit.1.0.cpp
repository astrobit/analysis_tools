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
//	main
//
//
//
//------------------------------------------------------------------------------

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	using namespace specfit;
	std::vector<std::string> vCL_Arguments;
	std::vector<std::string> vFile_List;
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		vCL_Arguments.push_back(std::string(i_lpszArg_Values[uiI]));
	}
	bool bDebug = false;
	bool bSingle = false;
	bool bTry_Single_Fit = false;
	for (std::vector<std::string>::iterator iterI = vCL_Arguments.begin(); iterI != vCL_Arguments.end(); iterI++)
	{
		if (iterI->find(".xml") != std::string::npos)
			vFile_List.push_back(*iterI);
		else if (*iterI == "--debug")
			bDebug = true;
		else if ((iterI->substr(0,8) == "--ps-vel") || (iterI->substr(0,9) == "--ps-temp") || (iterI->substr(0,4) == "--Se") || (iterI->substr(0,4) == "--Ss"))
			bSingle = true;
		else  if (iterI->substr(0,5) == "--fit")
			bTry_Single_Fit = true;
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
		bTry_Single_Fit = xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "--fit");
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

	ifsTest.open("Results/target_fit_data.csv");
	if (!ifsTest.is_open())
	{
		std::ofstream ofsFile;
		ofsFile.open("Results/target_fit_data.csv",std::ios_base::app);
		assert(ofsFile.is_open());
		Output_Target_Result_Header(ofsFile);
		ofsFile.close();
	}
	else
		ifsTest.close();
	



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

		// generate header lines for model specific fit results files
		for (auto iterModels = mModel_Data.begin(); iterModels != mModel_Data.end(); iterModels++)
		{
			std::ostringstream ossModel_Fit_File;
			ossModel_Fit_File << "Results/model_fit_data_" << iterModels->first << ".csv";
			std::cout << ossModel_Fit_File.str() << std::endl;
			ifsTest.open(ossModel_Fit_File.str().c_str());
			if (!ifsTest.is_open())
			{
				std::ofstream ofsFile;
				ofsFile.open(ossModel_Fit_File.str().c_str(),std::ios_base::app);
				assert(ofsFile.is_open());
				Output_Result_Header(ofsFile);
				ofsFile.close();
			}
			else
				ifsTest.close();
		}

		// Deredden spectra if requested
		Deredden( vfitFits);
		// Done confirming and loading the data; begin the actual fitting
		if (bSingle)
			Perform_Fits( vfitFits, mModel_Lists, mModel_Data, vResults, bDebug, &cRef_Data_E, &cRef_Data_S, &bTry_Single_Fit);
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
