#include <statepop.h>
#include <sp_config.h>

void statepop::Set_Param(const statepop::param & i_cParam)
{
	m_cParam = i_cParam;

	if (m_cParam.uiElement_Min_Ion_Species == -1)
		m_cParam.uiElement_Min_Ion_Species = 0;
	if (m_cParam.uiElement_Max_Ion_Species == -1)
		m_cParam.uiElement_Max_Ion_Species = m_cParam.uiElement_Z;

	dRedshift = (m_cParam.dMaterial_Velocity_km_s - m_cParam.dPhotosphere_Velocity_km_s) * 1.0e5 / g_XASTRO.k_dc;
	// generate radiation field
	rfPlanck.Set_Temperature(m_cParam.dRadiation_Temperature_K);
	if (m_cParam.dElectron_Kinetic_Temperature_K == -1)
		m_cParam.dElectron_Kinetic_Temperature_K = m_cParam.dRadiation_Temperature_K;
	vfMaxwell.Set_Temperature(m_cParam.dElectron_Kinetic_Temperature_K);



	// load Kurucz data
	kddData.Initialize(m_cParam.uiElement_Z,m_cParam.uiElement_Min_Ion_Species,m_cParam.uiElement_Max_Ion_Species,rfPlanck,dRedshift);

	// load Opacity Project data
	opElement.Read_Element_Data(m_cParam.uiElement_Z);


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// create mapping of Opacity Project states
	//
	////////////////////////////////////////////////////////////////////////////////////////

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

	mimkvldKurucz_Correlation.clear();
	mCorrelation.clear();

	size_t tCorrelation_Idx = 0;;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			vecconfig vcK_Config = Read_Kurucz_State((unsigned int)(floor(iterJ->second.klvdLevel_Data.m_dElement_Code) - fmod(iterJ->second.klvdLevel_Data.m_dElement_Code,1.0) * 100.0 + 0.001), iterJ->second.klvdLevel_Data.m_szLabel);
			opacity_project_level_descriptor opldOP_Level = Find_Equivalent_Level(iterJ->second.klvdLevel_Data.m_dElement_Code, iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm, iterJ->second.klvdLevel_Data.m_dGamma, iterJ->second.klvdLevel_Data.m_szLabel, mConfigs,vcK_Config);
			if (opldOP_Level.m_uiS != -1)
			{
				state_correlation stCorr;
				stCorr.m_opld_Main_State = opldOP_Level;

				// now identify level of ionized states
				vecconfig vIon_Config;
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
				//std::cout << "RC: " << iterJ->second.klvdLevel_Data.m_szLabel << " has " << stCorr.m_vopld_Recombined_States.size() << std::endl;
				mCorrelation[tCorrelation_Idx] = stCorr;
				mimkvldKurucz_Correlation[tCorrelation_Idx] = iterJ;
				tCorrelation_Idx++;
			}
		}
	}


	vTransition_Data.clear();
	size_t uiCount = 0;
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

	uiCount = 0;
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

	////////////////////////////////////////////////////////////////////////////
	//
	// Convergence loop
	//
	// Build a series of matrices with incresing number of components. Look for convergence of results for lowest energy states for each ion
	//
	////////////////////////////////////////////////////////////////////////////
	bool bDone = false;
//	if (tMax_States == -1)
//		tMax_States = 500;

	//for (size_t tMax_States = 4; tMax_States < 500 && !bDone; tMax_States++)
	{

		////////////////////////////////////////////////////////////////////////////////////////
		//
		// Generate Matrix
		//
		////////////////////////////////////////////////////////////////////////////////////////
		//Decide on matrix size based on element

		// Matrix schematic:
		// i = row, j = column

		//   |----- ion J, state j ------ | --- ion J + 1, state j --- | --- ion J + 2, state j --- | ...
		//   +----------------------------+----------------------------+----------------------------|    
		// | |0                           |                            |                            |
		// | | 0                          |                            |                            |
		// | |  0                         |                            |                            |
		// | |   0         stimulated     |                            |                            |
		// | |    0            &          |                            |                            |
		//   |     0       spontaneous    |                            |                            |
		// i |      0       emission      |                            |                            |
		// o |       0                    |                            |                            |
		// n |        0                   |       recombination        |   zeros                    |
		//   |         0                  |                            |                            |
		// I |          0                 |                            |                            |
		// , |           0                |                            |                            |
		//   |            0               |                            |                            |
		// s |             0              |                            |                            |
		// t |              0             |                            |                            |
		// a |               0            |                            |                            |
		// t |                0           |                            |                            |
		// e |                 0          |                            |                            |
		//   |                  0         |                            |                            |
		// i |                   0        |                            |                            |
		//   |                    0       |                            |                            |
		// | |                     0      |                            |                            |
		// | |                      0     |                            |                            |
		// | |   absorption          0    |                            |                            |
		// | |                        0   |                            |                            |
		// | |                         0  |                            |                            |
		// | |                          0 |                            |                            |
		//   |                           0|                            |                            |
		// --+----------------------------+----------------------------+----------------------------+
		//   |                            |0                           |                            |
		// | |                            | 0                          |                            |
		// | |                            |  0        stimulated       |                            |
		// | |                            |   0           &            |                            |
		//   |                            |    0      spontaneous      |                            |
		// i |                            |     0       emission       |                            |
		// o |                            |      0                     |                            |
		// n |                            |       0                    |                            |
		//   |                            |        0                   |                            |
		// I |      photoionization       |         0                  |    recombination           |
		//   |                            |          0                 |                            |
		// + |                            |           0                |                            |
		//   |                            |            0               |                            |
		// 1 |                            |             0              |                            |
		// , |                            |              0             |                            |
		//   |                            |               0            |                            |
		// s |                            |                0           |                            |
		// t |                            |                 0          |                            |
		// a |                            |                  0         |                            |
		// t |                            |                   0        |                            |
		// e |                            |                    0       |                            |
		//   |                            |                     0      |                            |
		// i |                            |                      0     |                            |
		//   |                            |                       0    |                            |
		// | |                            |                        0   |                            |
		// | |                            |                         0  |                            |
		// | |                            |                          0 |                            |
		//   |                            |                           0|                            |
		// --+----------------------------+----------------------------+----------------------------+

		tMatrix_Order = 0;
		if (kddData.m_vmklvdLevel_Data.size() > 0)
		{
			unsigned int uiNum_Ions = kddData.m_kdKurucz_Data.m_vvkldLine_Data.size();
			for (vmklvd::iterator iterI = kddData.m_vmklvdLevel_Data.begin(); iterI != kddData.m_vmklvdLevel_Data.end(); iterI++)
			{
				//tMatrix_Order += std::min(iterI->size(),tMax_States);
				tMatrix_Order += iterI->size();
			}
			std::cout << "Matrix size will be " << tMatrix_Order << std::endl;

			size_t tIdx_I = 0; // row
			size_t tIdx_J = 0; // column
			size_t tIdx_Total = 0;
			statepop::floattype dMax_Val = 0.0;
			for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
			{
				size_t tNum_States_I = 0;
//					for (imklvd iterSt_I = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterSt_I != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end() && tNum_States_I < tMax_States; iterSt_I++)
				for (imklvd iterSt_I = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterSt_I != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterSt_I++)
				{
					tNum_States_I++;
					for (size_t tIdx_Ion_J = 0; tIdx_Ion_J < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_J++)
					{
						size_t tNum_States_J = 0;
//							for (imklvd iterSt_J = kddData.m_vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != kddData.m_vmklvdLevel_Data[tIdx_Ion_J].end() && tNum_States_J < tMax_States; iterSt_J++)
						for (imklvd iterSt_J = kddData.m_vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != kddData.m_vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
						{
							tNum_States_J++;
							// calculate Z (Eq 12) for Level i

							// -- processing section
							if (tIdx_I != tIdx_J)
							{
								if (tIdx_Ion_J == tIdx_Ion_I) // same ion
								{
									statepop::floattype dH = 0.0;
									bool bFound = false;
									if (tIdx_I > tIdx_J)
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
									else
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
										mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
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
														statepop::floattype dIon_Thresh_Ryd = xaiIon_Data.Get_Ion_State_Potential(mCorrelation[tJ].m_opld_Main_State.m_uiZ - mCorrelation[tJ].m_opld_Main_State.m_uiN) / g_XASTRO.k_dRy;
														statepop::floattype dGround_Ryd = vdOP_Ground_State[mCorrelation[tJ].m_opld_Main_State.m_uiN - 1];
														opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tJ].m_opld_Main_State);
														statepop::floattype dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
														statepop::floattype dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
						
														statepop::floattype dH = opElement.Get_Ionization_Rate(mCorrelation[tJ].m_opld_Main_State,dTrue_Ion_Thresh,rfPlanck,dRedshift);
														mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
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
															statepop::floattype dIon_Thresh_Ryd = xaiIon_Data.Get_Ion_State_Potential(mCorrelation[tI].m_opld_Main_State.m_uiZ - mCorrelation[tI].m_opld_Main_State.m_uiN) / g_XASTRO.k_dRy;
															bFound = true;
															statepop::floattype dGround_Ryd = vdOP_Ground_State[mCorrelation[tI].m_opld_Main_State.m_uiN - 1];
															opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tI].m_opld_Main_State);
															statepop::floattype dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
															statepop::floattype dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
															statepop::floattype dH = opElement.Get_Recombination_Rate(mCorrelation[tJ].m_opld_Main_State, mCorrelation[tI].m_opld_Main_State,dTrue_Ion_Thresh,vfMaxwell) * m_cParam.dNe;
															mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
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


		

			//size_t tMatrix_Size = tIdx_I;
			////////////////////////////////////////////////////////////////////////////////////
			//
			// Generate Z and divide matrix entries by z
			//
			////////////////////////////////////////////////////////////////////////////////////
			for (tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)
			{
				double dZ = 0;
				// compute Z for column j. M&W Eq. 13
				for (tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++)
				{
					if (mpdSparse_MatrixBZ.count(std::pair<size_t,size_t>(tIdx_J,tIdx_I)) > 0)
					{
						dZ += mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)];
					}
				}
				vdZ.push_back(dZ);
			}
			smMatrixB.Set_Size(tMatrix_Order);
			for (tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)
			{
				// now divide all items in row J by the computed Z; M&W Eq. 14 / 15 (B_{ij} = H_{ij}/Z_j, but here we use j and ii instead of i and j, so here it is B_{j,ii} = H_{j,ii}/Z_{j}
				for (size_t tIdx_II = 0; tIdx_II < tMatrix_Order; tIdx_II++)
				{
					std::pair<size_t,size_t> pIdx(tIdx_II,tIdx_J);
					if (mpdSparse_MatrixBZ.count(pIdx) > 0)
					{
						mpdSparse_MatrixB[pIdx] /= vdZ[tIdx_J];
						if (dMax_Val < mpdSparse_MatrixB[pIdx])
							dMax_Val = mpdSparse_MatrixB[pIdx];
						smMatrixB.Set(tIdx_I,tIdx_J,mpdSparse_MatrixB[pIdx]);
					}
				}
			}
		}
	}
}
