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
			floattype dMax_Val = 0.0;
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


			std::cout << "Generate Matrix List" << std::endl;
		

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
			for (auto iterI = mpdSparse_MatrixBZ.begin(); iterI != mpdSparse_MatrixBZ.end(); iterI++)
			{
				// now divide all items in row J by the computed Z; M&W Eq. 14 / 15 (B_{ij} = H_{ij}/Z_j, but here we use j and ii instead of i and j, so here it is B_{j,ii} = H_{j,ii}/Z_{j}
				floattype fVal = iterI->second / vdZ[iterI->first.second];
				mpdSparse_MatrixB[iterI->first] = fVal;
				if (dMax_Val < fVal)
					dMax_Val = fVal;
				// fill the matrix to be used for the eigenvector problem
				smMatrixB.Set(iterI->first.first,iterI->first.second,fVal);
			}
		}
	}
}
