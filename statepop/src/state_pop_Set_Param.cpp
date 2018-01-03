#include <statepop.h>
#include <sp_config.h>

void statepop::Reset_Param(const floattype & i_dElectron_Density_cm3, const statepop::floattype & i_dRadiation_Temperature_K, const statepop::floattype & i_dElectron_Kinetic_Temperature_K, const statepop::floattype & i_dMaterial_Velocity_km_s, const statepop::floattype & i_dPhotosphere_Velocity_km_s)
{
	param i_cParam = m_cParam;
	i_cParam.dNe = i_dElectron_Density_cm3;
	i_cParam.dMaterial_Velocity_km_s = i_dMaterial_Velocity_km_s;
	i_cParam.dPhotosphere_Velocity_km_s = i_dPhotosphere_Velocity_km_s;
	i_cParam.dRadiation_Temperature_K = i_dRadiation_Temperature_K;
	i_cParam.dElectron_Kinetic_Temperature_K = i_dElectron_Kinetic_Temperature_K;
	Reset_Param(i_cParam);
}
void statepop::Reset_Param(const statepop::param & i_cParam)
{
	if (i_cParam.uiElement_Min_Ion_Species != i_cParam.uiElement_Min_Ion_Species ||
		i_cParam.uiElement_Max_Ion_Species != i_cParam.uiElement_Max_Ion_Species ||
		i_cParam.uiElement_Z != m_cParam.uiElement_Z)
	{
		Set_Param(i_cParam);
	}
	else
	{
		m_cParam = i_cParam;

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
	//@@TODO: warning - if reset called I'm not sure that kurucz data will reload correctly
	kddData.Initialize(m_cParam.uiElement_Z,m_cParam.uiElement_Min_Ion_Species,m_cParam.uiElement_Max_Ion_Species);
	kddData.Compute_H_Z(rfPlanck,dRedshift,dVelocity_Ratio * dVelocity_Ratio);

	std::cout << "Initialize Opacity Project Data" << std::endl;

	// load Opacity Project data
	//@@TODO: warning - if reset called I'm not sure that opacity project data will reload correctly
	opElement.Read_Element_Data(m_cParam.uiElement_Z);


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// create mapping of Opacity Project states
	//
	////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Identify Opacity Project States" << std::endl;

	mConfigs.clear();
	// generate map linking level descriptors to full level definitions
	size_t tI = 0;
	for (std::vector<opacity_project_ion>::iterator iterI = opElement.m_vopiIon_Data.begin(); iterI != opElement.m_vopiIon_Data.end(); iterI++)
	{
		for (ims iterJ = iterI->m_msStates.begin(); iterJ != iterI->m_msStates.end(); iterJ++)
		{
			//std::cout << iterJ->first.m_uiZ << " " << iterJ->first.m_uiN << " ";
			mConfigs[iterJ->first] = Read_OP_State(iterI->m_uiN, iterI->Get_State_Configuration(iterJ->first.m_uiS,iterJ->first.m_uiL,iterJ->first.m_uiP,iterJ->first.m_uiLvl_ID));
			mapOPStates[tI] = iterJ->first; // state index for OP data
			mapOPStates_Inverse[iterJ->first] = tI; // correlate OP data configuration to state index
			tI++;
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
	// create numerical index for Kurucz levels -- this makes life easier all around; we can afterward refer to everything by the index in this map
	//
	////////////////////////////////////////////////////////////////////////////////////////
	tI = 0;
	for (auto iterI = kddData.m_vmklvdLevel_Data.begin(); iterI != kddData.m_vmklvdLevel_Data.end(); iterI++)
	{
		for (auto iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
		{
			mapKStates[tI] = iterJ->second;
			tI++;
		}
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

	for (auto iterI = mapKStates.begin(); iterI != mapKStates.end(); iterI++)
	{
		vecconfig vcK_Config;
		vcK_Config = Read_Kurucz_State((unsigned int)(floor(iterI->second.klvdLevel_Data.m_dElement_Code) - fmod(iterI->second.klvdLevel_Data.m_dElement_Code,1.0) * 100.0 + 0.001), iterI->second.klvdLevel_Data.m_szLabel, iterI->second.klvdLevel_Data.m_dJ);
		if (vcK_Config.size() == 0)
		{
			std::cerr << "fault in processing the Kurucz state." << std::endl;
			return;
		}
		vK_Configs[iterI->first] = vcK_Config; // save the configuration

		// Find the equivalent Opacity Project Level, if it exists
		opacity_project_level_descriptor opldOP_Level = Find_Equivalent_Level(iterI->second.klvdLevel_Data.m_dElement_Code, iterI->second.klvdLevel_Data.m_dEnergy_Level_cm, iterI->second.klvdLevel_Data.m_dGamma, iterI->second.klvdLevel_Data.m_szLabel, mConfigs,vcK_Config);

		state_correlation stCorr;
		stCorr.m_opld_Main_State = opldOP_Level; // may be (-1,-1,-1,-1) if not found


		
		mCorrelation[iterI->first] = stCorr;
	}

	std::cout << "Kurucz ionization correlation" << std::endl;
	// alternative ionization correlation; use the Kurucz configuration information to find the equivalent ionized state
	for (auto iterI = vK_Configs.begin(); iterI != vK_Configs.end(); iterI++)
	{
		// not the most elegant code. Why oh why didn't I implement the configuration as a stack
		// ok, really it needs to be a deque
		vecconfig vIon_Config = iterI->second;
		if (vIon_Config.size() > 0)
		{
			vIon_Config.pop_front();
			size_t tCorr = -1;
			for (auto iterJ = vK_Configs.begin(); iterJ != vK_Configs.end() && tCorr == -1; iterJ++)
			{
				if (vIon_Config == iterJ->second)
					tCorr = iterJ->first;
			}
			if (tCorr == -1) // not found; try again
			{
				//@@TODO
			}
			mK_Ionized_States[iterI->first] = tCorr;

			if (tCorr != -1) // not found; try again
				mCorrelation[iterI->first].m_opld_Ionized_State = mCorrelation[tCorr].m_opld_Main_State;
		}
	}
			


	std::cout << "Generate Level List" << std::endl;

	size_t uiCount = 0;
	double dIon_Code = 0.0;
	for (auto iterI = mapKStates.begin(); iterI != mapKStates.end(); iterI++)
	{
		if (iterI->second.klvdLevel_Data.m_dElement_Code != dIon_Code)
			std::cout << "Ion " << iterI->second.klvdLevel_Data.m_dElement_Code << ":" << std::endl;

		dIon_Code = iterI->second.klvdLevel_Data.m_dElement_Code;

		std::cout << "Level " << iterI->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1 (J = " << iterI->second.klvdLevel_Data.m_dJ << ") has " << (iterI->second.vivkldEmission_Transition_Data.size() + iterI->second.vivkldAbsorption_Transition_Data.size()) << " transitions." << std::endl;

		level_data cLevel;

		cLevel.tID = iterI->first;
		cLevel.ldElement_Code = iterI->second.klvdLevel_Data.m_dElement_Code;
		cLevel.szLabel = iterI->second.klvdLevel_Data.m_szLabel;
		cLevel.ldJ = iterI->second.klvdLevel_Data.m_dJ;
		cLevel.ldEnergy_Level_Ryd = iterI->second.klvdLevel_Data.m_dEnergy_Level_Ryd;
		cLevel.ldGamma = iterI->second.klvdLevel_Data.m_dGamma;
		cLevel.ldZ = iterI->second.klvdLevel_Data.m_dZ;
		cLevel.tNumber_of_Absorption_Transitions = iterI->second.vivkldAbsorption_Transition_Data.size();
		cLevel.tNumber_of_Emission_Transitions = iterI->second.vivkldEmission_Transition_Data.size();

		cLevel.tOP_Project_Level_Correlation[0] = mCorrelation[iterI->first].m_opld_Main_State.m_uiS;
		cLevel.tOP_Project_Level_Correlation[1] = mCorrelation[iterI->first].m_opld_Main_State.m_uiL;
		cLevel.tOP_Project_Level_Correlation[2] = mCorrelation[iterI->first].m_opld_Main_State.m_uiP;
		cLevel.tOP_Project_Level_Correlation[3] = mCorrelation[iterI->first].m_opld_Main_State.m_uiLvl_ID;

		cLevel.tOP_Project_Ionization_State[0] = mCorrelation[iterI->first].m_opld_Ionized_State.m_uiS;
		cLevel.tOP_Project_Ionization_State[1] = mCorrelation[iterI->first].m_opld_Ionized_State.m_uiL;
		cLevel.tOP_Project_Ionization_State[2] = mCorrelation[iterI->first].m_opld_Ionized_State.m_uiP;
		cLevel.tOP_Project_Ionization_State[3] = mCorrelation[iterI->first].m_opld_Ionized_State.m_uiLvl_ID;

		cLevel.cElectron_Configuration = vK_Configs[iterI->first];

		cLevel.tIonized_State_ID = mK_Ionized_States[iterI->first];
		vLevel_Data.push_back(cLevel);
	}

	std::cout << "Generate Transition List" << std::endl;
	vTransition_Data.clear();
	uiCount = 0;
	size_t tState_ID = 0;
	for (auto iterI = mapKStates.begin(); iterI != mapKStates.end(); iterI++)
	{
		for (auto iterK = iterI->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterI->second.vivkldAbsorption_Transition_Data.end(); iterK++)
		{

			Kurucz_Level_Data cLevel_Upper;
			if ((*iterK)->m_cLevel_Upper == iterI->second.klvdLevel_Data)
				cLevel_Upper = (*iterK)->m_cLevel_Lower;
			else
				cLevel_Upper = (*iterK)->m_cLevel_Upper;

			transition_data cTrx;
			cTrx.tID = uiCount;
			cTrx.ldElement_Code = iterI->second.klvdLevel_Data.m_dElement_Code;
			cTrx.tLower_Level_ID = iterI->first;
			cTrx.szLower_Level_State = iterI->second.klvdLevel_Data.m_szLabel;
			cTrx.ldLower_Level_J = iterI->second.klvdLevel_Data.m_dJ;
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
			cTrx.ldZ = iterI->second.klvdLevel_Data.m_dZ;

			vTransition_Data.push_back(cTrx);

			uiCount++;
		}
		tState_ID++;
	}

//	Generate_Matrix();

}
