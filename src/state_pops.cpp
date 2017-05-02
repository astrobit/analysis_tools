#include<cstdio>
#include<cstdlib>
#include <cfloat>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <xastro.h>
#include <xmath.h>
#include <xstdlib.h>
#include <arlnsmat.h>
#include <arlsnsym.h>
#include <radiation.h>
#include <line_routines.h>
#include <kurucz_data.h>
#include <ios>
#include<opacity_project_pp.h>
#include <state_pops.h>
#include <velocity_function.h>
#include <unordered_map>
#include <xastroion.h>


typedef std::map<opacity_project_level_descriptor, std::vector<config> > mcfg;



opacity_project_level_descriptor Find_Equivalent_Level(const double & i_dElement_Code, const double & i_lpdEnergy_Level_cm, const double & i_dGamma, const std::string & i_sLabel, const mcfg &i_mcfgConfigs, const std::vector<config> & i_vcfgConfig, bool i_bQuiet = false)
{
	opacity_project_level_descriptor opldEquivalent_Level;
	bool bFound = false;
	for(auto iterK = i_mcfgConfigs.cbegin(); iterK != i_mcfgConfigs.cend() && !bFound; iterK++)
	{
		if (i_vcfgConfig == iterK->second)
		{
			bFound = true;
			opldEquivalent_Level = iterK->first;
		}
	}
	if (!bFound && !i_bQuiet)
	{
		// try again with SLP info stripped - this will not be quite right - it could return a 1P state 
		std::vector<opacity_project_level_descriptor> vMatches;
		std::vector<config> vcTest = i_vcfgConfig;
		vcTest[0].m_uiS = vcTest[0].m_uiP = -1; vcTest[0].m_dL = -1.0;
		for(auto iterK = i_mcfgConfigs.cbegin(); iterK != i_mcfgConfigs.cend(); iterK++)
		{
			if (vcTest == iterK->second)
			{
				vMatches.push_back(iterK->first);
			}
		}
		std::cerr << "Unable to correlate Kurucz state (" << i_dElement_Code << ") " << i_sLabel << " [" << i_lpdEnergy_Level_cm << ", " << i_dGamma << "]" << std::endl;
		std::cerr << "Possible matches:" << std::endl;
		if (vMatches.size() >= 1)
		{
			for (auto iterK = vMatches.begin(); iterK != vMatches.end(); iterK++)
			{
				std::cerr << iterK->m_uiS << " " << iterK->m_uiL << " " << iterK->m_uiP << " " << iterK->m_uiLvl_ID << std::endl;
			}
		}
		else
			std::cerr << "None" << std::endl;
		Print_Config_Vector(i_vcfgConfig,std::cerr);

	}
	return opldEquivalent_Level;
}

std::vector<opacity_project_level_descriptor> Find_Equivalent_Recombined_Levels(const mcfg &i_mcfgConfigs, const std::vector<config> & i_vcfgConfig, bool i_bQuiet = false)
{
	std::vector<opacity_project_level_descriptor> vopldLevels;
//	if (i_vcfgConfig.size() != i_mcfgConfigs.begin()->first.m_uiZ)
//		Print_Config_Vector(i_vcfgConfig,std::cout);
	for(auto iterK = i_mcfgConfigs.begin(); iterK != i_mcfgConfigs.end(); iterK++)
	{
		if (i_vcfgConfig.size() != iterK->first.m_uiZ)
		{
//			if (i_vcfgConfig.size() == (iterK->first.m_uiN - 1))
//			{
//				std::cout << "test " << iterK->first.m_uiS << " " << iterK->first.m_uiL << " " << iterK->first.m_uiP << " " << iterK->first.m_uiLvl_ID;
//			}
			if (Ion_Recombine_Equivalence(i_vcfgConfig,iterK->second))
			{
				vopldLevels.push_back(iterK->first);
			}
//			if (i_vcfgConfig.size() == (iterK->first.m_uiN - 1))
//				std::cout << std::endl;
		}
	}
	return vopldLevels;
}
class state_correlation
{
public:
	opacity_project_level_descriptor 				m_opld_Main_State;
	opacity_project_level_descriptor 				m_opld_Ionized_State;
	std::vector<opacity_project_level_descriptor> 	m_vopld_Recombined_States;
};

typedef std::map<size_t, imklvd> mimkvld;
typedef std::map<size_t, state_correlation > mcorr;

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// inputs
	//
	////////////////////////////////////////////////////////////////////////////////////////

	double	dN = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--n", -1.0);
	double	dLog_N = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--log-n", -9999.0);
	double	dNe = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ne", -1.0);
	double	dLog_Ne = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--log-ne", -9999.0);
	double	dRadiation_Temperature_K = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--rad-temp", 10000.0);
	double	dPhotosphere_Velocity_km_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-vel", 10000.0);
	unsigned int uiElement_Z = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--elem", -1);//20);
	unsigned int uiElement_Max_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--max-ion", -1);//4);
	unsigned int uiElement_Min_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--min-ion", -1);//4);
	unsigned int uiIon_Species_Only = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--only-ion", -1);//4);

	double	dMaterial_Velocity_km_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--mat-vel", 25000.0);
	double	dElectron_Velocity_Temperature = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--e-temp", -1);

	if (i_iArg_Count == 1 ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"?") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"HELP") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"--help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"--help") ||
		(dN == -1.0 && dLog_Ne == -9999 && dNe == -1.0 && dLog_Ne == -9999) ||
		(dLog_N != -9999 && dN != -1) ||
		(dLog_Ne != -9999 && dNe != -1) ||
		uiElement_Z == -1 ||
		(uiElement_Max_Ion_Species != -1 && uiElement_Max_Ion_Species > uiElement_Z)
		)
	{
		std::cout << i_lpszArg_Values[0] << ": Compute the relative populations for all known electron configurations of a given element." << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << i_lpszArg_Values[0] << " <options>" << std::endl;
		std::cout << "Options / Parameters:" << std::endl;
		std::cout << "--ne=<float> : assumed electron density to use for recombination component, in cm^{-3}. Mutually exclusive with --log-ne." << std::endl;
		std::cout << "--log-ne=<float> : assumed electron density to use for recombination component, in cm^{-3}. Mutually exclusive with --ne." << std::endl;
		std::cout << "--n=<float> : assumed total ion density, in cm^{-3}. Mutually exclusive with --log-n." << std::endl;
		std::cout << "--log-n=<float> : assumed total ion density, in cm^{-3}. Mutually exclusive with --n. " << std::endl;
		std::cout << "\t If only one of --ne, --n, --log-ne, --log-n is specified, the value will be used for both ion and electron density. " << std::endl;
		std::cout << "--rad-temp=<float> : for blackbody radiation field, the blackbody temperature in K." << std::endl << "\tDefault is 10,000 K" << std::endl;
		std::cout << "--e-temp=<float> : Kinetic temperature to use for electron velocity field." << std::endl << "\tDefault value is the photosphere (radiation) temperature." << std::endl;
		std::cout << "--ps-vel=<float> : Velocity of photosphere in km/s." << std::endl << "    Default = 10,000 km/s" << std::endl;
		std::cout << "--mat-vel=<float> : Velocity of material of interest in km/s." << std::endl << "    Default is 25,000 km/s" << std::endl;
		std::cout << "--elem=<integer> : Atomic number of element of interest (e.g. Hydrogen = 1)." << std::endl;
		std::cout << "--max-ion=<integer> : Maximum ionization state to consider; 0 is neutral," << std::endl << "\tmaximum value is atomic number of element." << std::endl << "    Default is the maximum value." << std::endl;
		std::cout << "--min-ion=<integer> : Minimum ionization state to consider; 0 is neutral," << std::endl << "    maximum value is atomic number of element." << std::endl << "    Default is 0." << std::endl;
		std::cout << "--only-ion=<integer> : Single ionization state to consider; 0 is neutral," << std::endl << "    maximum value is atomic number of element." << std::endl << "    Equivalent to setting --max-ion and --min-ion to the same value." << std::endl;

		if (dLog_N != -9999 && dN != -1)
		{
			std::cerr << "--log-n and --n were both specified in the call. These are mutually exclusive." << std::endl;
		}
		if (dLog_Ne != -9999 && dNe != -1)
		{
			std::cerr << "--log-ne and --ne were both specified in the call. These are mutually exclusive." << std::endl;
		}
		return 1;
	}


	if (dLog_N != -9999)
		dN = pow(10.0,dLog_N);
	if (dLog_Ne != -9999)
		dNe = pow(10.0,dLog_Ne);
	if (dN == -1 && dNe != -1)
	{
		std::cout << "Using electron density for ion density." << std::endl;
		dN = dNe;
	}
	if (dNe == -1 && dN != -1)
	{
		std::cout << "Using ion density for electro density." << std::endl;
		dNe = dN;
	}

	double	dRedshift = (dMaterial_Velocity_km_s - dPhotosphere_Velocity_km_s) * 1.0e5 / g_XASTRO.k_dc;
	// generate radiation field
	Planck_radiation_field rfPlanck(dRadiation_Temperature_K);
	if (dElectron_Velocity_Temperature == -1)
		dElectron_Velocity_Temperature = dRadiation_Temperature_K;
	Maxwellian_velocity_function vfMaxwell(dElectron_Velocity_Temperature);

	std::map<opacity_project_level_descriptor, std::vector<config> > mConfigs;
	mcorr mCorrelation;
	mimkvld mimkvldKurucz_Correlation;
	if (uiElement_Max_Ion_Species != -1 && uiIon_Species_Only != -1)
	{
		std::cerr << "--only-ion and --max-ion both specified. --max-ion ignored." << std::endl;
	}
	else if (uiElement_Min_Ion_Species != -1 && uiIon_Species_Only != -1)
	{
		std::cerr << "--only-ion and --min-ion both specified. --min-ion ignored." << std::endl;
	}


	unsigned int uiMin_Ion = 0;
	if (uiElement_Min_Ion_Species != -1)
		uiMin_Ion = uiElement_Min_Ion_Species;
	if (uiIon_Species_Only != -1)
	{
		uiMin_Ion = uiIon_Species_Only;
		uiElement_Max_Ion_Species = uiIon_Species_Only;
	}

	////////////////////////////////////////////////////////////////////////////
	//
	// Output run parameters
	//
	////////////////////////////////////////////////////////////////////////////
	
	FILE * fileParameters = fopen("parameters.txt","wt");
	if (fileParameters != nullptr)
	{
		fprintf(fileParameters,"log Electron density: %.3f [cm^{-3}]\n",log10(dNe));
		fprintf(fileParameters,"log Ion density: %.3f [cm^{-3}]\n",log10(dN));
		fprintf(fileParameters,"Radiation temperature: %.0f K\n",dRadiation_Temperature_K);
		fprintf(fileParameters,"Electron temperature: %.0f K\n",dElectron_Velocity_Temperature);
		fprintf(fileParameters,"Photosphere velocity: %.0f km / sec\n",dPhotosphere_Velocity_km_s);
		fprintf(fileParameters,"Material velocity: %.0f km / sec\n",dMaterial_Velocity_km_s);
		fprintf(fileParameters,"Redshift: %.2e \n",dRedshift);
		fprintf(fileParameters,"Element: Z = %i\n",uiElement_Z);
		if (uiMin_Ion == uiElement_Max_Ion_Species)
			fprintf(fileParameters,"Ion: %i\n",uiMin_Ion);
		else
			fprintf(fileParameters,"Ions: %i -- %i\n",uiMin_Ion, uiElement_Max_Ion_Species);

		fclose(fileParameters);
	}

	// load Kurucz data
	kurucz_derived_data kddData(uiElement_Z,uiMin_Ion,uiElement_Max_Ion_Species,rfPlanck,dRedshift);

	// load Opacity Project data
	opacity_project_element opElement;
	opElement.Read_Element_Data(uiElement_Z);

	std::vector<double> vdOP_Ground_State;

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// create mapping of Opacity Project states
	//
	////////////////////////////////////////////////////////////////////////////////////////

	// generate map linking level descriptors to full level definitions
	for (std::vector<opacity_project_ion>::iterator iterI = opElement.m_vopiIon_Data.begin(); iterI != opElement.m_vopiIon_Data.end(); iterI++)
	{
		for (ims iterJ = iterI->m_msStates.begin(); iterJ != iterI->m_msStates.end(); iterJ++)
		{
			//std::cout << iterJ->first.m_uiZ << " " << iterJ->first.m_uiN << " ";
			mConfigs[iterJ->first] = Read_OP_State(iterI->m_uiN, iterI->Get_State_Configuration(iterJ->first.m_uiS,iterJ->first.m_uiL,iterJ->first.m_uiP,iterJ->first.m_uiLvl_ID));
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// find ground state of each ion
	//
	////////////////////////////////////////////////////////////////////////////////////////

	for (std::vector<opacity_project_ion>::iterator iterI = opElement.m_vopiIon_Data.begin(); iterI != opElement.m_vopiIon_Data.end(); iterI++)
	{
		double dEmin = DBL_MAX;
		for (ims iterJ = iterI->m_msStates.begin(); iterJ != iterI->m_msStates.end(); iterJ++)
		{
			//std::cout << iterJ->first.m_uiZ << " " << iterJ->first.m_uiN << " ";
			if (iterJ->second.m_dEnergy_Ry < dEmin)
				dEmin = iterJ->second.m_dEnergy_Ry;
		}
		printf("Ground state %.2e\n",dEmin);
		vdOP_Ground_State.push_back(fabs(dEmin));
	}



	////////////////////////////////////////////////////////////////////////////////////////
	//
	// link Kurucz levels to Opacity Project Levels by their full configuration
	//
	////////////////////////////////////////////////////////////////////////////////////////

	size_t tCorrelation_Idx = 0;;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			std::vector<config> vcK_Config = Read_Kurucz_State((unsigned int)(floor(iterJ->second.klvdLevel_Data.m_dElement_Code) - fmod(iterJ->second.klvdLevel_Data.m_dElement_Code,1.0) * 100.0 + 0.001), iterJ->second.klvdLevel_Data.m_szLabel);
			opacity_project_level_descriptor opldOP_Level = Find_Equivalent_Level(iterJ->second.klvdLevel_Data.m_dElement_Code, iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm, iterJ->second.klvdLevel_Data.m_dGamma, iterJ->second.klvdLevel_Data.m_szLabel, mConfigs,vcK_Config);
			if (opldOP_Level.m_uiS != -1)
			{
				state_correlation stCorr;
				stCorr.m_opld_Main_State = opldOP_Level;

				// now identify level of ionized states
				std::vector<config> vIon_Config;
				auto iterK = vcK_Config.begin();
				iterK++; // the highest state electron has been lost
				for (; iterK != vcK_Config.end(); iterK++)
				{
					vIon_Config.push_back(*iterK);
				}
				std::string szIon_Label = iterJ->second.klvdLevel_Data.m_szLabel + "--";
				stCorr.m_opld_Ionized_State = Find_Equivalent_Level(iterJ->second.klvdLevel_Data.m_dElement_Code, iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm, iterJ->second.klvdLevel_Data.m_dGamma,szIon_Label,mConfigs,vIon_Config);
				// now identify the list of recombined states
				stCorr.m_vopld_Recombined_States = Find_Equivalent_Recombined_Levels(mConfigs,vcK_Config);
				std::cout << "RC: " << iterJ->second.klvdLevel_Data.m_szLabel << " has " << stCorr.m_vopld_Recombined_States.size() << std::endl;
				mCorrelation[tCorrelation_Idx] = stCorr;
				mimkvldKurucz_Correlation[tCorrelation_Idx] = iterJ;
				tCorrelation_Idx++;
			}
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output list of levels to file levels.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////

	FILE * fileLevels = fopen("levels.csv","wt");
	if (fileLevels)
		fprintf(fileLevels,"ID, Element code, Level , J, Energy (Ryd), Gamma (s^-1), Z (s^-1), Absorbtion lines, Emission Lines, OP Project State\n");


	unsigned int uiCount = 0;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		std::cout << "Ion " << kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin()->second.klvdLevel_Data.m_dElement_Code << ":" << std::endl;
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			std::cout << "Level " << iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1 (J = " << iterJ->second.klvdLevel_Data.m_dJ << ") has " << (iterJ->second.vivkldEmission_Transition_Data.size() + iterJ->second.vivkldAbsorption_Transition_Data.size()) << " transitions." << std::endl;

			if (fileLevels)
			{
				fprintf(fileLevels,"%i, %.2f, %s, %.1f, %.5f, %.3e, %.3e, %i, %i, ",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_Ryd,iterJ->second.klvdLevel_Data.m_dGamma,iterJ->second.klvdLevel_Data.m_dZ,iterJ->second.vivkldAbsorption_Transition_Data.size(),iterJ->second.vivkldEmission_Transition_Data.size());
				bool bFound = false;
				for (size_t tI = 0; tI < tCorrelation_Idx && !bFound; tI++)
				{
					if (mimkvldKurucz_Correlation[tI] == iterJ)
					{
						bFound = true;
						if (mCorrelation.count(tI) > 0)
						{
							fprintf(fileLevels,"%i%i%i %i",mCorrelation[tI].m_opld_Main_State.m_uiS,mCorrelation[tI].m_opld_Main_State.m_uiL,mCorrelation[tI].m_opld_Main_State.m_uiP,mCorrelation[tI].m_opld_Main_State.m_uiLvl_ID);
						}
					}
				}
				
				fprintf(fileLevels,"\n");
				uiCount++;
			}

		}
	}
	if (fileLevels)
		fclose(fileLevels);


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output list of transitions to file trx.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////

	FILE * fileTrx = fopen("trx.csv","wt");
	if (fileTrx)
		fprintf(fileTrx,"Trx ID, Element code, Level (Lower) ID, Level (lower), Level (upper), Wavelength [A], A, B, B(SE), Hab (%.1f kK), Hem (%.1f kK), Z\n",dRadiation_Temperature_K*1e-3,dRadiation_Temperature_K*1e-3);


	if (fileTrx)
	{
		unsigned int uiCount = 0;
		size_t tState_ID = 0;
		for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
		{
			for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
			{
				for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
				{

					Kurucz_Level_Data cLevel_Upper;
					if ((*iterK)->m_cLevel_Upper == iterJ->second.klvdLevel_Data)
						cLevel_Upper = (*iterK)->m_cLevel_Lower;
					else
						cLevel_Upper = (*iterK)->m_cLevel_Upper;

					fprintf(fileTrx,"%i, %.2f, %i, %s %.1f, %s %.1f, %.3f, %.3e, %.3e, %.3e, %.3e, %.3e, %.3e\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,tState_ID,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ, cLevel_Upper.m_szLabel.c_str(), cLevel_Upper.m_dJ, (*iterK)->m_dWavelength_cm * 1e8, (*iterK)->m_dEinstein_A, (*iterK)->m_dEinstein_B, (*iterK)->m_dEinstein_B_SE, (*iterK)->m_dH_abs, (*iterK)->m_dH_em, iterJ->second.klvdLevel_Data.m_dZ);
					uiCount++;
				}
				tState_ID++;
			}
		}
		fclose(fileTrx);
	}


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Generate Matrix
	//
	////////////////////////////////////////////////////////////////////////////////////////
	//Decide on matrix size based on element
	// for the moment, let's just do this for a single ion
	size_t tMatrix_Order = 0;
	if (kddData.m_vmklvdLevel_Data.size() > 0)
	{
		unsigned int uiNum_Ions = kddData.m_kdKurucz_Data.m_vvkldLine_Data.size();
		for (vmklvd::iterator iterI = kddData.m_vmklvdLevel_Data.begin(); iterI != kddData.m_vmklvdLevel_Data.end(); iterI++)
		{
			tMatrix_Order += iterI->size();
		}
		std::cout << "Matrix size will be " << tMatrix_Order << std::endl;
		std::map<std::pair<size_t, size_t> , double> mpdSparse_Matrix;

		size_t tIdx_I = 0; // row
		size_t tIdx_J = 0; // column
		size_t tIdx_Total = 0;
		double dMax_Val = 0.0;
		for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
		{
			for (imklvd iterSt_I = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterSt_I != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterSt_I++)
			{
				for (size_t tIdx_Ion_J = 0; tIdx_Ion_J < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_J++)
				{
					for (imklvd iterSt_J = kddData.m_vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != kddData.m_vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
					{
						// calculate Z (Eq 12) for Level i

						// -- processing section
						if (tIdx_I != tIdx_J)
						{
							if (tIdx_Ion_J == tIdx_Ion_I) // same ion
							{
								double dH = 0.0;
								bool bFound = false;
								if (tIdx_I > tIdx_J)//iterSt_J->second.klvdLevel_Data < iterSt_I->second.klvdLevel_Data)
								{
								// first find the transition in the list for j
									for (ivivkld iterK = iterSt_J->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterSt_J->second.vivkldAbsorption_Transition_Data.end() && !bFound; iterK++)
									{
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH  = (*iterK)->m_dH_abs;
											bFound = true;
										}
				
									}
								}
								else //if (kddData.m_vmklvdLevel_Data[tIdx_Ion_J].klvdLevel_Data > kddData.m_vmklvdLevel_Data[tIdx_Ion_I].klvdLevel_Data)
								{
								// first find the transition in the list for j
									for (ivivkld iterK = iterSt_J->second.vivkldEmission_Transition_Data.begin(); iterK != iterSt_J->second.vivkldEmission_Transition_Data.end() && !bFound; iterK++)
									{
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH = (*iterK)->m_dH_em;
											bFound = true;
										}
				
									}
								}
								if (bFound)
								{
									mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
								}
							}
							else if (tIdx_Ion_J == (tIdx_Ion_I - 1)) // potential photoionization term
							{
								bool bFound = false;
								for (size_t tJ = 0; tJ < mimkvldKurucz_Correlation.size() && !bFound; tJ++)
								{
									if (mimkvldKurucz_Correlation[tJ] == iterSt_J)
									{
										for (size_t tI = 0; tI < mimkvldKurucz_Correlation.size() && !bFound; tI++)
										{
											if (mimkvldKurucz_Correlation[tI] == iterSt_I)
											{
												bFound = true;
												if (mCorrelation[tJ].m_opld_Ionized_State == mCorrelation[tI].m_opld_Main_State)
												{
													XASTRO_ATOMIC_IONIZATION_DATA xaiIon_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data(mCorrelation[tI].m_opld_Main_State.m_uiZ);
													double dIon_Thresh_Ryd = xaiIon_Data.Get_Ion_State_Potential(mCorrelation[tJ].m_opld_Main_State.m_uiZ - mCorrelation[tJ].m_opld_Main_State.m_uiN) / g_XASTRO.k_dRy;
													double dGround_Ryd = vdOP_Ground_State[mCorrelation[tJ].m_opld_Main_State.m_uiN - 1];
													opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tJ].m_opld_Main_State);
													double dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
													double dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
							
													double dH = opElement.Get_Ionization_Rate(mCorrelation[tJ].m_opld_Main_State,dTrue_Ion_Thresh,rfPlanck,dRedshift);
													mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
												}
											}
										}
									}
								}
							}
							else if (tIdx_Ion_J == (tIdx_Ion_I + 1)) // potential recombination term
							{
								bool bFound = false;
								for (size_t tJ = 0; tJ < mimkvldKurucz_Correlation.size() && !bFound; tJ++)
								{
									if (mimkvldKurucz_Correlation[tJ] == iterSt_J)
									{
										for (size_t tI = 0; tI < mimkvldKurucz_Correlation.size() && !bFound; tI++)
										{
											if (mimkvldKurucz_Correlation[tI] == iterSt_I)
											{
												for (auto iterK = mCorrelation[tJ].m_vopld_Recombined_States.begin(); iterK != mCorrelation[tJ].m_vopld_Recombined_States.end() && !bFound; iterK++)
												{
													if ((*iterK) == mCorrelation[tI].m_opld_Main_State)
													{
														XASTRO_ATOMIC_IONIZATION_DATA xaiIon_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data(mCorrelation[tI].m_opld_Main_State.m_uiZ);
														double dIon_Thresh_Ryd = xaiIon_Data.Get_Ion_State_Potential(mCorrelation[tI].m_opld_Main_State.m_uiZ - mCorrelation[tI].m_opld_Main_State.m_uiN) / g_XASTRO.k_dRy;
														bFound = true;
														double dGround_Ryd = vdOP_Ground_State[mCorrelation[tI].m_opld_Main_State.m_uiN - 1];
														opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tI].m_opld_Main_State);
														double dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
														double dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
														double dH = opElement.Get_Recombination_Rate(mCorrelation[tJ].m_opld_Main_State, mCorrelation[tI].m_opld_Main_State,dTrue_Ion_Thresh,vfMaxwell) * dNe;
														mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
													}
												}
												bFound = true; //bFound is being used in two different ways here: to control the inner iterK loop, and to control the outer tI and tJ loops. Even if the inner iterK loop doesn't find anything, the tI and tJ loops should terminate.
											}
										}
									}
								}
							}
							// else H = 0 (handled by memset above)
							// -- end of processing section
						}
						tIdx_J++;
						tIdx_Total++;
					}
				}
				tIdx_I++;
				tIdx_J = 0; // new row
			}
		}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output matrix to matrixa.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////
		FILE * fileMatA = fopen("matrixa.csv","wt");

		if (fileMatA != nullptr)
		{
			size_t tIdx = 0;
			for (size_t tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++) // row
			{
				for (size_t tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)// column
				{
					std::pair<size_t,size_t> pPos(tIdx_J,tIdx_I);
					if (tIdx_J > 0)
						fprintf(fileMatA,", ");
					if (mpdSparse_Matrix.count(pPos) > 0)
						fprintf(fileMatA,"%.17e",mpdSparse_Matrix[pPos]);
					else
						fprintf(fileMatA,"0.0");
					tIdx++;
				}
				fprintf(fileMatA,"\n");
			}
			fclose(fileMatA);
		}

		//size_t tMatrix_Size = tIdx_I;
		////////////////////////////////////////////////////////////////////////////////////
		//
		// Generate Z and divide matrix entries by z
		//
		////////////////////////////////////////////////////////////////////////////////////
		std::vector<double> vdZ;
		for (tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)
		{
			double dZ = 0;
			// compute Z for column j. M&W Eq. 13
			for (tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++)
			{
				if (mpdSparse_Matrix.count(std::pair<size_t,size_t>(tIdx_J,tIdx_I)) > 0)
				{
					dZ += mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_J,tIdx_I)];
				}
			}
			vdZ.push_back(dZ);
		}

		for (tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)
		{
			// now divide all items in row J by the computed Z; M&W Eq. 14 / 15 (B_{ij} = H_{ij}/Z_j, but here we use j and ii instead of i and j, so here it is B_{j,ii} = H_{j,ii}/Z_{j}
			for (size_t tIdx_II = 0; tIdx_II < tMatrix_Order; tIdx_II++)
			{
				if (mpdSparse_Matrix.count(std::pair<size_t,size_t>(tIdx_II,tIdx_J)) > 0)
				{
					mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_II,tIdx_J)] /= vdZ[tIdx_J];
					if (dMax_Val < mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_II,tIdx_J)])
						dMax_Val = mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_II,tIdx_J)];
				}
			}
		}

		
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output matrix to matrixb.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////
		FILE * fileMatB = fopen("matrixb.csv","wt");

		if (fileMatB != nullptr)
		{
			size_t tIdx = 0;
			for (size_t tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++) // row
			{
				for (size_t tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)// column
				{
					std::pair<size_t,size_t> pPos(tIdx_J,tIdx_I);
					if (tIdx_J > 0)
						fprintf(fileMatB,", ");
					if (mpdSparse_Matrix.count(pPos) > 0)
						fprintf(fileMatB,"%.17e",mpdSparse_Matrix[pPos]);// / dMax_Val);
					else
						fprintf(fileMatB,"0.0");
					tIdx++;
				}
				fprintf(fileMatB,"\n");
			}
			fclose(fileMatB);
		}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Generate compact sparse matrix for arpack++
	//
	////////////////////////////////////////////////////////////////////////////////////////
		double * lpdValues = new double[mpdSparse_Matrix.size()];
		int * lpiRow_Idx = new int[mpdSparse_Matrix.size()];
		int * lpiCol_Idx = new int[tMatrix_Order + 1];
		size_t tIdx = 0, tColIdx = 0;
		size_t tCol = -1;
		for (std::map<std::pair<size_t, size_t> , double>::iterator iterI = mpdSparse_Matrix.begin(); iterI != mpdSparse_Matrix.end(); iterI++)
		{
			//std::cout << iterI->first.first << " " << iterI->first.second << " " << iterI->second / dMax_Val << std::endl;
			lpdValues[tIdx] = iterI->second / dMax_Val;
			lpiRow_Idx[tIdx] = iterI->first.second;
			if (tCol != iterI->first.first)
			{
				lpiCol_Idx[tColIdx] = (int)tIdx;
				tColIdx++;
				tCol = iterI->first.first;
			}
			tIdx++;
		}
		lpiCol_Idx[tMatrix_Order] = mpdSparse_Matrix.size();

		FILE * fileCSC = fopen("csc.csv","wt");
		for (unsigned int uiI = 0; uiI < mpdSparse_Matrix.size(); uiI++)
		{
			fprintf(fileCSC,"%i, %i, %e\n",uiI,lpiRow_Idx[uiI],lpdValues[uiI]);
		}
		fclose(fileCSC);
		FILE * fileCSCcol = fopen("csccol.csv","wt");
		for (unsigned int uiI = 0; uiI < (tMatrix_Order + 1); uiI++)
		{
			fprintf(fileCSCcol,"%i, %i\n",uiI,lpiCol_Idx[uiI]);
		}
		fclose(fileCSCcol);

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Perform arpack++ routines to find the eigenvectors
	//
	////////////////////////////////////////////////////////////////////////////////////////
		// define the compact sparse matrix used by arpack++
		ARluNonSymMatrix<double, double> cscmA(tMatrix_Order,mpdSparse_Matrix.size(),lpdValues,lpiRow_Idx,lpiCol_Idx);
		// define the problem for arpack++; look for 3 largest eigenvalues (for now)
		ARluNonSymStdEig<double> cProb(4,cscmA);
		// allow up to 10000 iterations for convergence
		cProb.ChangeMaxit(10000);
		// solve for the eignevectrs
		cProb.FindEigenvectors();

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Print the eigenvalues and determine the "best" value
	//
	////////////////////////////////////////////////////////////////////////////////////////
		size_t tEigValMaxIdx = -1;
		double dEig_Val_Err_Min = DBL_MAX;
		// print out the eigenvalues that were found, and identify the one that is closest to 1
		for (size_t tI = 0; tI < cProb.ConvergedEigenvalues(); tI++)
		{
			if (fabs(cProb.Eigenvalue(tI) - 1.0) < dEig_Val_Err_Min)
			{
				dEig_Val_Err_Min = fabs(cProb.Eigenvalue(tI) - 1.0);
				tEigValMaxIdx = tI;
			}
			std::cout << "Eigenvalue[" << tI << "] = " << cProb.Eigenvalue(tI) << std::endl;
		}
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output best eigenvector to console and file
	//
	////////////////////////////////////////////////////////////////////////////////////////
		if (tEigValMaxIdx != -1)
		{
			double dMax_Val = 0.0;

			// write out best eigenvector to console, and find the maximum value
			std::cout << "Eigenvalue = " << cProb.Eigenvalue(tEigValMaxIdx) << std::endl;
			for (size_t tJ = 0; tJ < cProb.GetN(); tJ++)
			{
				if (fabs(cProb.Eigenvector(tEigValMaxIdx,tJ).real()) > fabs(dMax_Val))
					dMax_Val = cProb.Eigenvector(tEigValMaxIdx,tJ).real();
				std::cout << "v[" << tJ << "] = " << cProb.Eigenvector(tEigValMaxIdx,tJ) << std::endl;
			}

			std::cout << dMax_Val << std::endl;
			double dNorm = 1.0 / dMax_Val;

			double dSum = 0.0;
			// normalize, ignoring small values
			std::vector<double> vdEig; // put the resultsinto this vector
			for (size_t tJ = 0; tJ < cProb.GetN(); tJ++)
			{
				double dVal = cProb.Eigenvector(tEigValMaxIdx,tJ).real() * dNorm;
				if (fabs(dVal) < 1e-10) // approximate threshold for uncertainty)
					dVal = 0.0;
				dSum += dVal;
				vdEig.push_back(dVal);
			}
			std::cout << dSum << std::endl;
			dNorm = 1.0 / dSum;
			for (size_t tJ = 0; tJ < vdEig.size(); tJ++)
			{
				vdEig[tJ] *= dNorm;
			}

			// write out the normalized best eigenvector to eigv.csv, with state information
			FILE * fileOut = fopen("eigv.csv","wt");
			if (fileOut)
				fprintf(fileOut,"i, Element Code, State, J, Wavenumber (cm^1), Relative Population\n");
			unsigned int uiCount = 0;
			for (vmklvd::iterator iterI = kddData.m_vmklvdLevel_Data.begin(); iterI != kddData.m_vmklvdLevel_Data.end(); iterI++)
			{
				for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
				{
					if (fileOut)
					{
						fprintf(fileOut,"%i, %.2f, %s, %.1f, %.5f, %.17e\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm,vdEig[uiCount]);
						uiCount++;
					}

				}
			}
			if (fileOut)
			{
				fclose(fileOut);
				std::cout << "Eigenvalue has been output to eigv.csv" << std::endl;
			}


			FILE * fileTrx = fopen("trxs.csv","wt");
			if (fileTrx)
				fprintf(fileTrx,"Trx ID, Element code, Level (Lower) ID, Level (lower), Level (upper), Wavelength [A], A, B, B(SE), Hab (%.1f kK), Hem (%.1f kK), Z, X(ul)\n",dRadiation_Temperature_K*1e-3,dRadiation_Temperature_K*1e-3);


			if (fileTrx)
			{
				unsigned int uiCount = 0;
				size_t tState_ID = 0;
				size_t tIon_Ref = 0;
				for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
				{
					for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
					{
						for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
						{
							Kurucz_Level_Data cLevel_Upper;
							size_t tJ = tIon_Ref;
							if ((*iterK)->m_cLevel_Upper == iterJ->second.klvdLevel_Data)
							{
								bool bFound = false;
								for (imklvd iterL = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterL != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end() && !bFound; iterL++)
								{
									if ((*iterK)->m_cLevel_Lower != iterL->second.klvdLevel_Data)
										tJ++;
									else
										bFound = true;
								}
								cLevel_Upper = (*iterK)->m_cLevel_Lower;
							}
							else
							{
								size_t tJ = 0;
								bool bFound = false;
								for (imklvd iterL = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterL != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end() && !bFound; iterL++)
								{
									if ((*iterK)->m_cLevel_Upper != iterL->second.klvdLevel_Data)
										tJ++;
									else
										bFound = true;
								}
								cLevel_Upper = (*iterK)->m_cLevel_Upper;
							}
							double dNup = vdEig[tJ];
							double dNlow = vdEig[tState_ID];
							double dVal = 1e-11;
							if (dNlow > 0 && dNup > 0)
								dVal = dNlow - dNup * iterJ->second.klvdLevel_Data.m_dStat_Weight / cLevel_Upper.m_dStat_Weight;
							if (dVal < 0.0)
								dVal = 1e-11;
							double dX = 8.0 * g_XASTRO.k_dpi * (*iterK)->m_dFrequency_Hz * (*iterK)->m_dFrequency_Hz / (g_XASTRO.k_dc * g_XASTRO.k_dc * (*iterK)->m_dEinstein_A * dN * dVal);

							fprintf(fileTrx,"%i, %.2f, %i, %s %.1f, %s %.1f, %.3f, %.3e, %.3e, %.3e, %.3e, %.3e, %.3e, %.3e\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,tState_ID,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ, cLevel_Upper.m_szLabel.c_str(), cLevel_Upper.m_dJ, (*iterK)->m_dWavelength_cm * 1e8, (*iterK)->m_dEinstein_A, (*iterK)->m_dEinstein_B, (*iterK)->m_dEinstein_B_SE, (*iterK)->m_dH_abs, (*iterK)->m_dH_em, iterJ->second.klvdLevel_Data.m_dZ,dX);
							uiCount++;
						}
						tState_ID++;
					}
					tIon_Ref = tState_ID;
				}
				fclose(fileTrx);
			}

			
		}
		else
			std::cerr << "Failed to generate an eigenvector." << std::endl;


		////////////////////////////////////////////////////////////////////////
		//
		// Compute Saha ion populations
		//
		////////////////////////////////////////////////////////////////////////
		FILE * fileSaha = fopen("saha.csv","wt");
		if (fileSaha)
		{
			double dInv_Kb_T = 1.0 / (g_XASTRO.k_dKb * dElectron_Velocity_Temperature);
			double dLog_e = log10(exp(1.0));
			fprintf(fileSaha,"Ion, E_{ion} [Ryd] , log n_i / n_i-1\n");
			for (size_t uiState = 0; uiState < vdOP_Ground_State.size(); uiState++)
			{
				double dLambda = g_XASTRO.k_dh * g_XASTRO.k_dh / (2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dme) * dInv_Kb_T;
				//printf("ground state %.2e\n",vdOP_Ground_State[uiState]);
				double dExponential_Term = -dInv_Kb_T * fabs(vdOP_Ground_State[uiState] * g_XASTRO.k_dRy);
				double dLog_Rel_Pop = log10(2.0 / (dLambda * sqrt(dLambda))) + dExponential_Term * dLog_e - log10(dNe);
				fprintf(fileSaha,"%i, %.2e, %.3e\n",uiState + uiMin_Ion,vdOP_Ground_State[uiState],dLog_Rel_Pop);
			}
		}
		////////////////////////////////////////////////////////////////////////
		//
		// Compute Boltzman state populations
		//
		////////////////////////////////////////////////////////////////////////
		FILE * fileBoltzmann = fopen("boltzmann.csv","wt");
		size_t tLevel_ID = 0;
		if (fileBoltzmann)
		{
			double dInv_Kb_T = 1.0 / (g_XASTRO.k_dKb * dElectron_Velocity_Temperature);
			double dLog_e = log10(exp(1.0));
			for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
			{
				imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin();
				imklvd iterJlast = iterJ;
				if (iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end())
					iterJ++;
				for (; iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
				{ // do all pops relative to ground state
					double dRel_Pop = log10(iterJ->second.klvdLevel_Data.m_dStat_Weight / iterJlast->second.klvdLevel_Data.m_dStat_Weight) + (iterJlast->second.klvdLevel_Data.m_dEnergy_Level_Ryd - iterJ->second.klvdLevel_Data.m_dEnergy_Level_Ryd) * g_XASTRO.k_dRy * dInv_Kb_T * dLog_e;
					fprintf(fileBoltzmann,"%i, %.3e\n",tLevel_ID,dRel_Pop);
//					iterJlast = iterJ;
					tLevel_ID++;
				}
			}
		}
	}
	return 0;
}
