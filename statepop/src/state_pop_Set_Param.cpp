#include <statepop.h>
#include <sp_config.h>

void statepop::Reset_Param(const floattype & i_dElectron_Density_cm3, const statepop::floattype & i_dRadiation_Temperature_K, const statepop::floattype & i_dElectron_Kinetic_Temperature_K, const statepop::floattype & i_dMaterial_Velocity_km_s, const statepop::floattype & i_dPhotosphere_Velocity_km_s)
{
	m_cParam.dNe = i_dElectron_Density_cm3;
	m_cParam.dMaterial_Velocity_km_s = i_dMaterial_Velocity_km_s;
	m_cParam.dPhotosphere_Velocity_km_s = i_dPhotosphere_Velocity_km_s;
	m_cParam.dRadiation_Temperature_K = i_dRadiation_Temperature_K;
	m_cParam.dElectron_Kinetic_Temperature_K = i_dElectron_Kinetic_Temperature_K;

	dRedshift = (m_cParam.dMaterial_Velocity_km_s - m_cParam.dPhotosphere_Velocity_km_s) * 1.0e5 / g_XASTRO.k_dc;
	dVelocity_Ratio = m_cParam.dPhotosphere_Velocity_km_s / m_cParam.dMaterial_Velocity_km_s; // also relative ratio of radii of material and photosphere
	// generate radiation field
	rfPlanck.Set_Temperature(m_cParam.dRadiation_Temperature_K);
	if (m_cParam.dElectron_Kinetic_Temperature_K == -1)
		m_cParam.dElectron_Kinetic_Temperature_K = m_cParam.dRadiation_Temperature_K;
	// set temperature for electrons
	vfMaxwell.Set_Temperature(m_cParam.dElectron_Kinetic_Temperature_K);
	// compute H and Z for each level / transition
	kddData.Compute_H_Z(rfPlanck,dRedshift,dVelocity_Ratio * dVelocity_Ratio);
	// regenerate the matrix
	Generate_Matrix();
}

void statepop::Set_Param(const statepop::param & i_cParam)
{
	m_cParam = i_cParam;

	if (m_cParam.uiElement_Min_Ion_Species == -1)
		m_cParam.uiElement_Min_Ion_Species = 0;
	if (m_cParam.uiElement_Max_Ion_Species == -1)
		m_cParam.uiElement_Max_Ion_Species = m_cParam.uiElement_Z;

	dRedshift = (m_cParam.dMaterial_Velocity_km_s - m_cParam.dPhotosphere_Velocity_km_s) * 1.0e5 / g_XASTRO.k_dc;
	dVelocity_Ratio = m_cParam.dPhotosphere_Velocity_km_s / m_cParam.dMaterial_Velocity_km_s; // also relative ratio of radii of material and photosphere
	// generate radiation field
	rfPlanck.Set_Temperature(m_cParam.dRadiation_Temperature_K);
	if (m_cParam.dElectron_Kinetic_Temperature_K == -1)
		m_cParam.dElectron_Kinetic_Temperature_K = m_cParam.dRadiation_Temperature_K;
	vfMaxwell.Set_Temperature(m_cParam.dElectron_Kinetic_Temperature_K);

	std::cout << "Initialize Kurucz Data" << std::endl;


	// load Kurucz data
	kddData.Initialize(m_cParam.uiElement_Z,m_cParam.uiElement_Min_Ion_Species,m_cParam.uiElement_Max_Ion_Species);
	kddData.Compute_H_Z(rfPlanck,dRedshift,dVelocity_Ratio * dVelocity_Ratio);

	std::cout << "Initialize Opacity Project Data" << std::endl;

	// load Opacity Project data
	opElement.Read_Element_Data(m_cParam.uiElement_Z);


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// create mapping of Opacity Project states
	//
	////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Identify Opacity Project States" << std::endl;

	mConfigs.clear();
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
	std::cout << "Find Ground States" << std::endl;

	for (std::vector<opacity_project_ion>::iterator iterI = opElement.m_vopiIon_Data.begin(); iterI != opElement.m_vopiIon_Data.end(); iterI++)
	{
		statepop::floattype dEmin = LDBL_MAX;
		for (ims iterJ = iterI->m_msStates.begin(); iterJ != iterI->m_msStates.end(); iterJ++)
		{
			//std::cout << iterJ->first.m_uiZ << " " << iterJ->first.m_uiN << " ";
			if (iterJ->second.m_dEnergy_Ry < dEmin)
				dEmin = iterJ->second.m_dEnergy_Ry;
		}
		printf("Ground state %.2Le\n",dEmin);
		vdOP_Ground_State.push_back(fabs(dEmin));
	}



	////////////////////////////////////////////////////////////////////////////////////////
	//
	// link Kurucz levels to Opacity Project Levels by their full configuration
	//
	////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Clearing correlation data" << std::endl;

	mimkvldKurucz_Correlation.clear();
	mCorrelation.clear();

	std::cout << "Mapping Kurucz -- Opacity Project" << std::endl;
	size_t tCorrelation_Idx = 0;;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			std::cout << "Read Kurucz State Config" << std::endl;
			vecconfig vcK_Config = Read_Kurucz_State((unsigned int)(floor(iterJ->second.klvdLevel_Data.m_dElement_Code) - fmod(iterJ->second.klvdLevel_Data.m_dElement_Code,1.0) * 100.0 + 0.001), iterJ->second.klvdLevel_Data.m_szLabel);
			if (vcK_Config.size() == 0)
			{
				std::cerr << "fault" << std::endl;
				return;
			}
			std::cout << "Find Equivalent OP level" << std::endl;
			opacity_project_level_descriptor opldOP_Level = Find_Equivalent_Level(iterJ->second.klvdLevel_Data.m_dElement_Code, iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm, iterJ->second.klvdLevel_Data.m_dGamma, iterJ->second.klvdLevel_Data.m_szLabel, mConfigs,vcK_Config);
			if (opldOP_Level.m_uiS != -1)
			{
				state_correlation stCorr;
				stCorr.m_opld_Main_State = opldOP_Level;

				std::cout << "Identify ionized state" << std::endl;
				// now identify level of ionized states
				vecconfig vIon_Config;
				auto iterK = vcK_Config.begin();
				if (iterK != vcK_Config.end())
				{
					iterK++; // the highest state electron has been lost
					for (; iterK != vcK_Config.end(); iterK++)
					{
						vIon_Config.push_back(*iterK);
					}
					std::string szIon_Label = iterJ->second.klvdLevel_Data.m_szLabel + "--";
					stCorr.m_opld_Ionized_State = Find_Equivalent_Level(iterJ->second.klvdLevel_Data.m_dElement_Code, iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm, iterJ->second.klvdLevel_Data.m_dGamma,szIon_Label,mConfigs,vIon_Config);
					// now identify the list of recombined states
					std::cout << "Identify recombined states" << std::endl;
					stCorr.m_vopld_Recombined_States = Find_Equivalent_Recombined_Levels(mConfigs,vcK_Config);
					//std::cout << "RC: " << iterJ->second.klvdLevel_Data.m_szLabel << " has " << stCorr.m_vopld_Recombined_States.size() << std::endl;
					mCorrelation[tCorrelation_Idx] = stCorr;
					mimkvldKurucz_Correlation[tCorrelation_Idx] = iterJ;
					tCorrelation_Idx++;
				}
				else
					std::cerr << "Error no K configuration data for " << iterJ->second.klvdLevel_Data.m_szLabel << " " << iterJ->second.klvdLevel_Data.m_dElement_Code << std::endl;
			}
		}
	}

	std::cout << "Generate Level List" << std::endl;

	size_t uiCount = 0;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		std::cout << "Ion " << kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin()->second.klvdLevel_Data.m_dElement_Code << ":" << std::endl;
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			std::cout << "Level " << iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1 (J = " << iterJ->second.klvdLevel_Data.m_dJ << ") has " << (iterJ->second.vivkldEmission_Transition_Data.size() + iterJ->second.vivkldAbsorption_Transition_Data.size()) << " transitions." << std::endl;

			level_data cLevel;

			cLevel.tID = uiCount;
			cLevel.ldElement_Code = iterJ->second.klvdLevel_Data.m_dElement_Code;
			cLevel.szLabel = iterJ->second.klvdLevel_Data.m_szLabel;
			cLevel.ldJ = iterJ->second.klvdLevel_Data.m_dJ;
			cLevel.ldEnergy_Level_Ryd = iterJ->second.klvdLevel_Data.m_dEnergy_Level_Ryd;
			cLevel.ldGamma = iterJ->second.klvdLevel_Data.m_dGamma;
			cLevel.ldZ = iterJ->second.klvdLevel_Data.m_dZ;
			cLevel.tNumber_of_Absorption_Transitions = iterJ->second.vivkldAbsorption_Transition_Data.size();
			cLevel.tNumber_of_Emission_Transitions = iterJ->second.vivkldEmission_Transition_Data.size();



			bool bFound = false;
			for (size_t tI = 0; tI < tCorrelation_Idx && !bFound; tI++)
			{
				if (mimkvldKurucz_Correlation[tI] == iterJ)
				{
					bFound = true;
					if (mCorrelation.count(tI) > 0)
					{
						cLevel.tOP_Project_Level_Correlation[0] = mCorrelation[tI].m_opld_Main_State.m_uiS;
						cLevel.tOP_Project_Level_Correlation[1] = mCorrelation[tI].m_opld_Main_State.m_uiL;
						cLevel.tOP_Project_Level_Correlation[2] = mCorrelation[tI].m_opld_Main_State.m_uiP;
						cLevel.tOP_Project_Level_Correlation[3] = mCorrelation[tI].m_opld_Main_State.m_uiLvl_ID;
					}
				}
			}
			vLevel_Data.push_back(cLevel);
			uiCount++;

		}
	}
	std::cout << "Generate Transition List" << std::endl;
	vTransition_Data.clear();
	uiCount = 0;
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

				transition_data cTrx;
				cTrx.tID = uiCount;
				cTrx.ldElement_Code = iterJ->second.klvdLevel_Data.m_dElement_Code;
				cTrx.tLower_Level_ID = tState_ID;
				cTrx.szLower_Level_State = iterJ->second.klvdLevel_Data.m_szLabel;
				cTrx.ldLower_Level_J = iterJ->second.klvdLevel_Data.m_dJ;
				cTrx.tUpper_Level_ID = Find_Level_ID(cLevel_Upper.m_dElement_Code,cLevel_Upper.m_szLabel,cLevel_Upper.m_dJ,cLevel_Upper.m_dEnergy_Level_Ryd);
				cTrx.szUpper_Level_State = cLevel_Upper.m_szLabel;
				cTrx.ldUpper_Level_J = cLevel_Upper.m_dJ;
				cTrx.ldWavenegth_Angstroms = (*iterK)->m_dWavelength_cm * 1e8;
				cTrx.ldEinstein_A = (*iterK)->m_dEinstein_A;
				cTrx.ldEinstein_B = (*iterK)->m_dEinstein_B;
				cTrx.ldEinstein_B_Spontaneous_Emission = (*iterK)->m_dEinstein_B_SE;
				cTrx.ldGamma = (*iterK)->m_dGamma_Rad;
				cTrx.ldH_absorption = (*iterK)->m_dH_abs;
				cTrx.ldH_emission = (*iterK)->m_dH_em;
				cTrx.ldZ = iterJ->second.klvdLevel_Data.m_dZ;

				vTransition_Data.push_back(cTrx);

				uiCount++;
			}
			tState_ID++;
		}
	}
	Generate_Matrix();

}
