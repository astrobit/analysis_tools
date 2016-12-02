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


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	// inputs
	double	dRadiation_Temperature_K = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--rad-temp", 10000.0);
	double	dPhotosphere_Velocity_kkm_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-vel", 10000.0);
	unsigned int uiElement_Z = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--elem", 4);//20);
	unsigned int uiElement_Max_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--max-ion", 2);//4);
	double	dMaterial_Velocity_kkm_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--mat-vel", 25000.0);

	double	dRedshift = (dMaterial_Velocity_kkm_s - dPhotosphere_Velocity_kkm_s) * 1.0e5 / g_XASTRO.k_dc;
	Planck_radiation_field rfPlanck(dRadiation_Temperature_K);

	kurucz_derived_data kddData(uiElement_Z,uiElement_Max_Ion_Species,rfPlanck,dRedshift);

	std::vector<opacity_project_ion > vopIons;

	for (unsigned int uiI = 0; uiI < uiElement_Max_Ion_Species; uiI++)
	{
		opacity_project_ion opionCurr;
		opionCurr.Read_Data(uiElement_Z,uiElement_Z - uiI);
		vopIons.push_back(opionCurr);
	}

	FILE * fileLevels = fopen("levels.csv","wt");
	if (fileLevels)
		fprintf(fileLevels,"ID, Element code, Level , J, Energy (Ryd), Gamma (s^-1), Z (s^-1), Absorbtion lines, Emission Lines\n");


	unsigned int uiCount = 0;
	for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < kddData.m_vmklvdLevel_Data.size(); tIdx_Ion_I++)
	{
		std::cout << "Ion " << kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin()->second.klvdLevel_Data.m_dElement_Code << ":" << std::endl;
		for (imklvd iterJ = kddData.m_vmklvdLevel_Data[tIdx_Ion_I].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIdx_Ion_I].end(); iterJ++)
		{
			std::cout << "Level " << iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1 (J = " << iterJ->second.klvdLevel_Data.m_dJ << ") has " << (iterJ->second.vivkldEmission_Transition_Data.size() + iterJ->second.vivkldAbsorption_Transition_Data.size()) << " transitions." << std::endl;

			if (fileLevels)
			{
				fprintf(fileLevels,"%i, %.2f, %s, %.1f, %.5f, %.3e, %.3e, %i, %i\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_Ryd,iterJ->second.klvdLevel_Data.m_dGamma,iterJ->second.klvdLevel_Data.m_dZ,iterJ->second.klvdLevel_Data.m_dZ,iterJ->second.vivkldAbsorption_Transition_Data.size(),iterJ->second.vivkldEmission_Transition_Data.size());
				uiCount++;
			}

		}
	}
	if (fileLevels)
		fclose(fileLevels);

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
					//for (size_t tIdx_State_J = 0; tIdx_State_J < kddData.m_vmklvdLevel_Data[tIdx_Ion_J].size(); tIdx_State_J++)
					for (imklvd iterSt_J = kddData.m_vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != kddData.m_vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
					{
//					for (size_t tIdx_State_I = 0; tIdx_State_I < kddData.m_vmklvdLevel_Data[tIdx_Ion_I].size(); tIdx_State_I++;)
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
//										if ((*iterK)->m_cLevel_Lower == iterSt_J->second.klvdLevel_Data &&
//											(*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data)
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH  = (*iterK)->m_dH_abs;//(*iterK)->m_dEinstein_B * (*iterK)->Calc_Exciting(dRadiation_Temperature_K,dRedshift);
											//std::cout << tIdx_I << "<-" << tIdx_J << " (a) " << dH / iterSt_I->second.klvdLevel_Data.m_dZ << std::endl;
											bFound = true;
										}
				
									}
								}
								else //if (kddData.m_vmklvdLevel_Data[tIdx_Ion_J].klvdLevel_Data > kddData.m_vmklvdLevel_Data[tIdx_Ion_I].klvdLevel_Data)
								{
								// first find the transition in the list for j
									for (ivivkld iterK = iterSt_J->second.vivkldEmission_Transition_Data.begin(); iterK != iterSt_J->second.vivkldEmission_Transition_Data.end() && !bFound; iterK++)
									{
//										if ((*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data &&
//											(*iterK)->m_cLevel_Upper == iterSt_J->second.klvdLevel_Data)
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH = (*iterK)->m_dH_em;//(*iterK)->m_dEinstein_A + (*iterK)->m_dEinstein_B_SE * (*iterK)->Calc_Exciting(dRadiation_Temperature_K,dRedshift);
											//std::cout << tIdx_I << "<-" << tIdx_J << " (e) " << dH / iterSt_I->second.klvdLevel_Data.m_dZ << std::endl;
											bFound = true;
										}
				
									}
								}
								if (bFound)
								{
									double dVal = dH / iterSt_I->second.klvdLevel_Data.m_dZ;
									if (dVal > dMax_Val)
										dMax_Val = dVal;
									mpdSparse_Matrix[std::pair<size_t,size_t>(tIdx_J,tIdx_I)] = dVal;
								}
								//else
								//	std::cout << tIdx_I << "<->" << tIdx_J << std::endl;
							}
							else if (tIdx_Ion_J == (tIdx_Ion_I - 1)) // potential recombination term
							{
								//@@TODO 
								//lpdA[tIdx_Total] = ....
							}
							else if (tIdx_Ion_J == (tIdx_Ion_I + 1)) // potential photoionization term
							{
								//@@TODO 
								//lpdA[tIdx_Total] = ....
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
		
		FILE * fileMatA = fopen("matrixa.csv","wt");

		if (fileMatA)
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
						fprintf(fileMatA,"%.17e",mpdSparse_Matrix[pPos] / dMax_Val);
					else
						fprintf(fileMatA,"0.0");
					tIdx++;
				}
				fprintf(fileMatA,"\n");
			}
			fclose(fileMatA);
		}

		// generate compact spare matrix as required by arpack++

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

/*		FILE * fileCSC = fopen("csc.csv","wt");
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
		fclose(fileCSCcol);*/

		// define the compact sparse matrix used by arpack++
		ARluNonSymMatrix<double, double> cscmA(tMatrix_Order,mpdSparse_Matrix.size(),lpdValues,lpiRow_Idx,lpiCol_Idx);
		// define the problem for arpack++; look for 3 largest eigenvalues (for now)
		ARluNonSymStdEig<double> cProb(4,cscmA);
		// allow up to 10000 iterations for convergence
		cProb.ChangeMaxit(10000);
		// solve for the eignevectrs
		cProb.FindEigenvectors();

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
		if (tEigValMaxIdx != -1)
		{
			double dSum = 0.0;
			// write out best eigenvector to console
			std::cout << "Eigenvalue = " << cProb.Eigenvalue(tEigValMaxIdx) << std::endl;
			for (size_t tJ = 0; tJ < cProb.GetN(); tJ++)
			{
				dSum += cProb.Eigenvector(tEigValMaxIdx,tJ).real();
				std::cout << "v[" << tJ << "] = " << cProb.Eigenvector(tEigValMaxIdx,tJ) << std::endl;
			}

			// write out the normalized best eigenvector to eigv.csv, with state information
			double dNorm = 1.0 / dSum;
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
						fprintf(fileOut,"%i, %.2f, %s, %.1f, %.5f, %.17e\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm,cProb.Eigenvector(tEigValMaxIdx,uiCount).real() * dNorm);
						uiCount++;
					}

				}
			}
			if (fileOut)
				fclose(fileOut);
		}
	}
	return 0;
}
