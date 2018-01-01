#include <statepop.h>

void statepop::Generate_Matrix(void)
{
	std::cout << "Generate Matrix" << std::endl;

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

		tMatrix_Order = mapKStates.size();
		std::cout << "Matrix size will be " << tMatrix_Order << std::endl;
		for (auto iterI = mapKStates.begin(); iterI != mapKStates.end(); iterI++)
		{
			size_t tIdx_I = iterI->first;
			size_t tIdx_Ion_I = (iterI->second.klvdLevel_Data.m_dElement_Code - std::floor(iterI->second.klvdLevel_Data.m_dElement_Code) + 0.001) * 100.0;
			for (auto iterJ = mapKStates.begin(); iterJ != mapKStates.end(); iterJ++)
			{
				size_t tIdx_J = iterJ->first;
				size_t tIdx_Ion_J = (iterJ->second.klvdLevel_Data.m_dElement_Code - std::floor(iterJ->second.klvdLevel_Data.m_dElement_Code) + 0.001) * 100.0;

				//printf("%i,%i (%i,%i)\n",tIdx_I,tIdx_J,tIdx_Ion_I,tIdx_Ion_J);
				floattype dMax_Val = 0.0;

				// -- processing section
				if (tIdx_Ion_J == tIdx_Ion_I) // same ion
				{
					if (tIdx_I != tIdx_J)
					{
						statepop::floattype dH = 0.0;
						bool bFound = false;
						auto itStart = iterJ->second.vivkldAbsorption_Transition_Data.begin();
						auto itEnd = iterJ->second.vivkldAbsorption_Transition_Data.end();
						if (tIdx_I < tIdx_J)
						{
							itStart = iterJ->second.vivkldEmission_Transition_Data.begin();
							itEnd = iterJ->second.vivkldEmission_Transition_Data.end();
						}
						for (auto iterK = itStart; iterK != itEnd && !bFound; iterK++)
						{
							if ((*iterK)->m_cLevel_Upper == iterI->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterI->second.klvdLevel_Data)
							{
								if (tIdx_I < tIdx_J)
									dH  = (*iterK)->m_dH_em;
								else
									dH  = (*iterK)->m_dH_abs;
								bFound = true;
							}
						}

						if (bFound)
						{
							mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
						}
					}
				}
				else if (tIdx_I > tIdx_J && (tIdx_Ion_J + 1) == tIdx_Ion_I) // photoionization term
				{
					//printf("here0\n");
					if (mK_Ionized_States[tIdx_J] == tIdx_I)
					{
						statepop::floattype dH = 0.0;
						Kurucz_Level_Data cKurucz_Level = mapKStates[tIdx_J].klvdLevel_Data; // get the state that is ionizing
						//printf("here0a\n");
						//printf("here\n");
						//fflush(stdout);
						XASTRO_ATOMIC_IONIZATION_DATA xaiIon_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data(cKurucz_Level.m_uiZ);
						statepop::floattype dIon_Thresh_erg = xaiIon_Data.Get_Ion_State_Potential(cKurucz_Level.m_uiIon_State);
						statepop::floattype dIon_Thresh_Ryd = dIon_Thresh_erg / g_XASTRO.k_dRy;
						//printf("here2\n");
						//fflush(stdout);
						if (mCorrelation[tIdx_J].m_opld_Main_State.m_uiS != -1)
						{
							//printf("here3\n");
							//fflush(stdout);
							statepop::floattype dGround_Ryd = vdOP_Ground_State[mCorrelation[tIdx_J].m_opld_Main_State.m_uiN - 1];
							//printf("here4\n");
							//fflush(stdout);
							opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tIdx_J].m_opld_Main_State);
							//printf("here5\n");
							//fflush(stdout);
							statepop::floattype dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
							//printf("here6\n");
							//fflush(stdout);
							statepop::floattype dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
							//printf("here7\n");
							//fflush(stdout);
							dH = opElement.Get_Ionization_Rate(mCorrelation[tIdx_J].m_opld_Main_State,dTrue_Ion_Thresh,rfPlanck,dRedshift) * dVelocity_Ratio * dVelocity_Ratio; // velocity ratio ^2 accounts for reduction in flux due to relative radii of the gas and the photosphere
						}
						else
						{
							dH = Photoionization_Rate(5.0e-18, dIon_Thresh_erg,rfPlanck,dRedshift) * dVelocity_Ratio * dVelocity_Ratio;
						}
						//printf("here8\n");
						//fflush(stdout);
						mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
					}
				}
				else if (tIdx_Ion_J == (tIdx_Ion_I + 1))// j < i: recombination term
				{
					//printf("0here0\n");
					if (mK_Ionized_States[tIdx_I] == tIdx_J)
					{
						//printf("0here1\n");
						statepop::floattype dH = 0.0;
						Kurucz_Level_Data cKurucz_Level = mapKStates[tIdx_I].klvdLevel_Data; // get the state that is ionizing
						Kurucz_Level_Data cIonized_Level = mapKStates[tIdx_J].klvdLevel_Data; // get the state that is ionizing
						//printf("here0a\n");
						//printf("here\n");
						//fflush(stdout);
						XASTRO_ATOMIC_IONIZATION_DATA xaiIon_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data(cKurucz_Level.m_uiZ);
						statepop::floattype dIon_Thresh_erg = xaiIon_Data.Get_Ion_State_Potential(cKurucz_Level.m_uiIon_State);
						statepop::floattype dIon_Thresh_Ryd = dIon_Thresh_erg / g_XASTRO.k_dRy;
						if (mCorrelation[tIdx_I].m_opld_Main_State.m_uiS != -1)
						{
							//printf("0here4 %Le\n",dIon_Thresh_Ryd);
							statepop::floattype dGround_Ryd = vdOP_Ground_State[mCorrelation[tIdx_I].m_opld_Main_State.m_uiN - 1];
							//printf("0here5 %Le\n",dGround_Ryd);
							opacity_project_state stOP_State = opElement.Find_State(mCorrelation[tIdx_I].m_opld_Main_State);
							//printf("0here6\n");
							statepop::floattype dState_E_Ryd = fabs(stOP_State.m_dEnergy_Ry);
							//printf("0here7 %Le\n",dState_E_Ryd);
							statepop::floattype dTrue_Ion_Thresh = dIon_Thresh_Ryd * dState_E_Ryd / dGround_Ryd;
							//printf("0here8 %Le\n",dTrue_Ion_Thresh);
							dH = opElement.Get_Recombination_Rate(mCorrelation[tIdx_I].m_opld_Main_State, mCorrelation[tIdx_J].m_opld_Main_State,dTrue_Ion_Thresh,vfMaxwell) * m_cParam.dNe * dVelocity_Ratio * dVelocity_Ratio; // velocity ratio ^2 accounts for reduction in flux due to relative radii of the gas and the photosphere
							//printf("0here9 %Le %Le %Le\n",dH,m_cParam.dNe,dVelocity_Ratio);
						}
						else
						{
							dH = Ion_Recombination_Rate(5.0e-18, dIon_Thresh_erg, cIonized_Level.m_dStat_Weight, cKurucz_Level.m_dStat_Weight, vfMaxwell) * m_cParam.dNe * dVelocity_Ratio * dVelocity_Ratio; // velocity ratio ^2 accounts for reduction in flux due to relative radii of the gas and the photosphere
						}

						mpdSparse_MatrixBZ[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dH;
					}
				}
			}
		}
	}


	std::cout << "Generate Matrix List" << std::endl;


	//size_t tMatrix_Size = tIdx_I;
	////////////////////////////////////////////////////////////////////////////////////
	//
	// Generate Z and divide matrix entries by z
	//
	////////////////////////////////////////////////////////////////////////////////////
	for (size_t tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++)
	{
		floattype dZ = 0;
		// compute Z for column j. M&W Eq. 13
		for (size_t tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++)
		{
			std::pair<size_t,size_t> pIdx(tIdx_J,tIdx_I);
			if (mpdSparse_MatrixBZ.count(pIdx) > 0)
			{
				dZ += mpdSparse_MatrixBZ[pIdx];
			}
		}
		vdZ.push_back(dZ);
	}
	smMatrixB.Set_Size(tMatrix_Order);
	smMatrixB.Zero();
	floattype dMax_Val;
	for (auto iterI = mpdSparse_MatrixBZ.begin(); iterI != mpdSparse_MatrixBZ.end(); iterI++)
	{
		// now divide all items in row i by the computed Z_i for that row; M&W Eq. 14 / 15 (B_{ij} = H_{ij}/Z_i
		// note that the sparse matrix is stored with (column, row) format
		floattype fVal = iterI->second / vdZ[iterI->first.second];
		mpdSparse_MatrixB[iterI->first] = fVal;
		if (dMax_Val < fVal)
			dMax_Val = fVal;
		// fill the matrix to be used for the eigenvector problem
		// note that the square matrix class uses (row, column) format
		smMatrixB.Set(iterI->first.second,iterI->first.first,fVal);
	}
}
