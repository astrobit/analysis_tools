#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
//#include <line_anal.h>

double g_dLog_Tau_Threshold = -2.0;
class SYNIONDATA
{
public:
	bool			bEnable;
	unsigned int 	uiID;
	double			dLog_Tau;
	double			dVmin;
	double			dVmax;
	double			dE_Folding;
	double			dTemp;


	void Clean(void)
	{
		bEnable = true;
		dLog_Tau = 0.0;
		dVmin = 0.0;
		dVmax = 0.0;
		dE_Folding = 0.0;
		dTemp = 0.0;
	}

	SYNIONDATA(void)
	{
		uiID = 0;
		Clean();
	}
};
class SYNIONDATA_NODE
{
public:
	SYNIONDATA		cData;
	SYNIONDATA_NODE	*lpNext;
	SYNIONDATA_NODE *lpPrev;

	SYNIONDATA_NODE(void) : cData()
	{
		lpNext = NULL;
		lpPrev = NULL;
	}
	~SYNIONDATA_NODE(void)
	{
		lpNext = NULL;
		lpPrev = NULL;
	}
};
class SYNIONDATA_LL
{
public:
	SYNIONDATA_NODE * lpHead;
	SYNIONDATA_NODE * lpTail;

	inline void	Void(void)
	{
		lpHead = lpTail = NULL;
	}
	SYNIONDATA_LL(void)
	{
		Void();
	}
	void Add(const SYNIONDATA &cData)
	{
		SYNIONDATA_NODE * lpNew = new SYNIONDATA_NODE;
		lpNew->cData = cData;
		lpNew->lpPrev = lpTail;
		if (lpTail)
			lpTail->lpNext = lpNew;
		lpTail = lpNew;
		if (!lpHead)
			lpHead = lpNew;
	}
	void	Clean(void)
	{
		SYNIONDATA_NODE * lpCurr = lpHead;
		while (lpCurr)
		{
			lpHead = lpCurr->lpNext;
			delete lpCurr;
			lpCurr = lpHead;
		}
		Void();
	}
	~SYNIONDATA_LL(void)
	{
		Clean();
	}
	SYNIONDATA_LL & operator += (const SYNIONDATA_LL & cRHO)
	{
		if (lpTail)
		{
			lpTail->lpNext = cRHO.lpHead;
			if (cRHO.lpHead)
				cRHO.lpHead->lpPrev = lpTail;
			lpTail = cRHO.lpTail;
		}
		return *this;
	}
};

enum SYNIONPARAMETER {ION,ACTIVE,LOGTAU,VMIN,VMAX,AUX,TEMP};
void WriteIonParameter(const SYNIONDATA_LL &cSynIonDataLL, SYNIONPARAMETER eParameter, FILE * fileOut, const double &dVmax)
{
//	double	dTau_Threshold = g_dLog_Tau_Threshold;
//	unsigned int uiI;
	bool bFirst = true;
	SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;
	
	switch (eParameter)
	{
	case ION:
		fprintf(fileOut,"        ions    :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (bFirst)
					fprintf(fileOut,"  %4i",lpCurr->cData.uiID);
				else
					fprintf(fileOut,",   %4i",lpCurr->cData.uiID);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case ACTIVE:
		fprintf(fileOut,"        active  :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (lpCurr->cData.bEnable)
				{
					if (bFirst)
						fprintf(fileOut,"   Yes");
					else
						fprintf(fileOut,",    Yes");
				}
				else
				{
					if (bFirst)
						fprintf(fileOut,"   No ");
					else
						fprintf(fileOut,",    No ");
				}

				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case LOGTAU:
		fprintf(fileOut,"        log_tau :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (bFirst)
					fprintf(fileOut," %5.2f",lpCurr->cData.dLog_Tau);
				else
					fprintf(fileOut,",  %5.2f",lpCurr->cData.dLog_Tau);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case VMIN:
		fprintf(fileOut,"        v_min   :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (bFirst)
					fprintf(fileOut,"%6.3f",lpCurr->cData.dVmin * 10.0 + 0.001);
				else
					fprintf(fileOut,", %6.3f",lpCurr->cData.dVmin * 10.0 + 0.001);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case VMAX:
		fprintf(fileOut,"        v_max   :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				double dVmaxlcl = lpCurr->cData.dVmax;
				if (dVmaxlcl > dVmax)
					dVmaxlcl = dVmax;
				if (bFirst)
					fprintf(fileOut,"%6.3f",dVmaxlcl * 10.0);
				else
					fprintf(fileOut,", %6.3f",dVmaxlcl * 10.0);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case AUX:
		fprintf(fileOut,"        aux     :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
                                double dEfolding = lpCurr->cData.dE_Folding * 10.0;
                                if (dEfolding > 0.0 && dEfolding < 0.001)
                                    dEfolding = 0.001;
                                else if (dEfolding < 0.0 && dEfolding > -0.001)
                                    dEfolding = -0.001;
				if (bFirst)
					fprintf(fileOut,"%6.3f",dEfolding);
				else
					fprintf(fileOut,", %6.3f",dEfolding);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	case TEMP:
		fprintf(fileOut,"        temp    :  [ ");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVmax && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (bFirst)
					fprintf(fileOut,"  %4.1f",lpCurr->cData.dTemp * 1.0e-3);
				else
					fprintf(fileOut,",   %4.1f",lpCurr->cData.dTemp * 1.0e-3);
				bFirst = false;
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut," ]\n");
		break;
	}
}


double GetAbundance(unsigned int uiI,const XDATASET &cAbdData)
{
	// FLASH seems to lower limit abundances to 10^-10; this forces a small abundance to be 0
	double	dAbd = cAbdData.GetElement(1,uiI);
	if (dAbd < 5.0e-10)
		dAbd = 0.0;
	return dAbd;
}


/*unsigned int CountIonStates(const XASTROLINE_ELEMENTDATA &cElement)
{
	unsigned int uiI, uiNumIons = 0;
	for (uiI = 0; uiI < cElement.uiZ; uiI++)
	{
		if (cElement.lpcIonData[uiI].uiNum_Lines > 0)
			uiNumIons++;
	}
	return uiNumIons;
}*/

double Optical_Depth(const double & i_dNumber_Density_Ref, const XASTROLINE_LINEDATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double &i_dTime_Ref)
{
	return i_dNumber_Density_Ref * i_cLine_Data.SobolevTauCoeff() * i_dTime_Ref * i_dTime_Ref * i_dTime_Ref / (i_dTime * i_dTime) * exp(-(i_cLine_Data.cLower_State.dEnergy_eV / g_XASTRO.k_dKb_eV) / i_dFluid_Temp);
}

double * g_lpdTau = NULL;
unsigned int g_uiNum_Tau_Points = 0;
void FillIonData(SYNIONDATA_LL &cIon_Data_LL, const XASTROLINE_ELEMENTDATA &cElement, const XASTROLINE_IONDATA &cIon, const double &dTime, const double &dTimeRef, const double &dTemp_Ref, const double & dTemp_Alpha, const XDATASET &cVelocity, const XDATASET &cEle_Abd, const XDATASET &cDens, unsigned int uiRef_Vel_Idx, unsigned int uiPS_Vel_Idx, const double * lpdElectron_Density, bool bEnable)
{
        double  dTau_Threshold = pow(10.0,g_dLog_Tau_Threshold);
	unsigned int uiI,uiJ,uiIref;
	unsigned int uiNum_Points = cVelocity.GetNumElements();
	unsigned int uiNum_Tau_Points = uiNum_Points - uiPS_Vel_Idx;// + 1;
	//printf("Num tau points %i\n",uiNum_Tau_Points);
	double	dN_Factor = 1.0  / (cElement.uiA * g_XASTRO.k_dmh);
	//double	* lpdTau;
	double	dTau_Max = 0.0;
	double	dTime_Effect = 1.0 / (dTime * dTime * dTime);
	if (!g_lpdTau || uiNum_Tau_Points > g_uiNum_Tau_Points)
	{
		if (g_lpdTau)
			delete [] g_lpdTau;
		g_lpdTau = new double[uiNum_Tau_Points];
		g_uiNum_Tau_Points = uiNum_Tau_Points;
	}

	memset(g_lpdTau,0,sizeof(double) * uiNum_Tau_Points);
	SYNIONDATA cIon_Data;

	XASTRO_ATOMIC_IONIZATION_DATA	* lpcIon_Energy_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(cElement.uiZ);
	XASTRO_ATOMIC_PARTITION_FUNCTION_DATA_IRWIN *	lpPartition_Fn_Data = g_xAstro_Partition_Fn_Data_Irwin.Get_PF_Data_Ptr(cElement.uiZ);
	if (lpPartition_Fn_Data == NULL)
	{
		printf("Failed to find partition funciton data\n");
		return;
	}

//	double	dIon_Temp = 0.0;
	printf("T = %.0f\t Alpha = %.2f\n",dTemp_Ref,dTemp_Alpha);


	double dRps = cVelocity.GetElement(0,uiPS_Vel_Idx) + cVelocity.GetElement(1,uiPS_Vel_Idx) * 1.0e9 * (dTime - dTimeRef);
	//printf("Doing tau points %.1f %.1f %.1f %.1f\n",dTemp,dTime,dTimeRef,dIon_Temp);
	for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
	{
		double dDens = cDens.GetElement(1,uiI + uiPS_Vel_Idx);
		if (dDens < 2.0e-10)
			dDens *= 1.0e-14;
		double dAbd = cEle_Abd.GetElement(1,uiI + uiPS_Vel_Idx) * dN_Factor;
//		double dState_Abundance = 1.0;
		double dRlcl = cVelocity.GetElement(0,uiI + uiPS_Vel_Idx) + cVelocity.GetElement(1,uiI + uiPS_Vel_Idx) * 1.0e9 * (dTime - dTimeRef);
		double dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
//		printf("%.2e,%.2e\t%.2e\t%.2f\t%.0f\t%.0f\n",dTemp_Alpha,dRlcl / dRps,pow(dRlcl / dRps,dTemp_Alpha),cVelocity.GetElement(1,uiI + uiPS_Vel_Idx),dTemp_Ref,dTempLcl);
		double dkT = g_XASTRO.k_dKb * dTempLcl;
//		printf("kT = %.2e\n",dkT);
		double dDeBroglie_Wavelength = g_XASTRO.k_dh / sqrt(2.0 * g_cConstants.dPi * g_XASTRO.k_dme * dkT);
		double ddB_W_3 = 1.0 / (dDeBroglie_Wavelength * dDeBroglie_Wavelength * dDeBroglie_Wavelength);
		double	dA[5] = {0.0,0.0,0.0,0.0,0.0};
//		double	dAsum = 0.0;
		double	dLambda_Fact = ddB_W_3 / lpdElectron_Density[uiI + uiPS_Vel_Idx] / dTime_Effect;
		if (lpcIon_Energy_Data && lpPartition_Fn_Data)
		{
			double dG1,dG2;

			for (uiJ = 0; uiJ < 4; uiJ++) // assume triply ionized
			{
				if (uiJ < 2)
				{
					dG1 = lpPartition_Fn_Data->Get_Partition_Function(uiJ + 1,dTempLcl);
					dG2 = lpPartition_Fn_Data->Get_Partition_Function(uiJ + 2,dTempLcl);
				}
				else
				{
					dG1 = dG2 = 1.0;
				}
				
				dA[uiJ] = dLambda_Fact * exp(-lpcIon_Energy_Data->m_lpIonization_energies_erg[uiJ] / dkT) * dG1 / dG2;
//				printf("%.1e\t%.1e\n",lpcIon_Energy_Data->m_lpIonization_energies_erg[uiJ],g_XASTRO.k_deV_erg);
//				if (uiJ < lpcIon_Energy_Data->m_uiIonization_Energies_Count)
//					dIon_Temp += lpcIon_Energy_Data->m_lpIonization_energies_erg[uiJ];
//				dAsum += dA[uiJ];
			}
//			for (uiJ = 0; uiJ < 4; uiJ++) // assume triply ionized
//			{
//				dA[uiJ] /= dAsum;
//			}
//			dIon_Temp /= g_XASTRO.k_dKb;
//			printf("%.1e\n",dIon_Temp);
		}
		double	dN1 = dAbd * dDens / (1.0 + dA[0] * (1.0 + dA[1] * (1.0 + dA[2] * (1.0 + dA[3] * (1.0 + dA[4])))));
		double	dNion = dN1;
		for (uiJ = 0; uiJ < cIon.uiIon_State - 1; uiJ++)
		{
			dNion *= dA[uiJ];
		}
                if (dNion > (dAbd * dDens))
                    printf("Fault: n[X] > n[tot]\n");
//		if (cElement.uiZ == 20)
//		{
//			printf("a: %.2e %.2e %.2e %.2e %.2e\n",dA[1],dA[2],dA[3],dA[4],dA[5]);
//		}
		
		


		double	dTau_Line_Max = 0.0;
		double	dLine_Max_Lambda = 0.0;
		for (uiJ = 0; uiJ < cIon.uiNum_Lines; uiJ++)
		{
			double dTauLine = Optical_Depth(dNion,cIon.lpcLine_Data[uiJ],dTempLcl,dTime,dTimeRef);
                        if (dTauLine > 1.0e300)
                            printf("Tau fault\n");
			if (dTauLine > dTau_Max)
				dTau_Max = dTauLine;
			if (dTauLine > dTau_Line_Max)
			{
				dLine_Max_Lambda = cIon.lpcLine_Data[uiJ].dWavelength_A;
				dTau_Line_Max = dTauLine;
			}
//			printf("%.1e\n",dTauLine);
//			lpdTau[uiI] += dTauLine;
		}
//		if (cElement.uiZ == 20)
//		{
//			printf("%.1f %.1e %.1e %.1e %.1e %.1f\n",cVelocity.GetElement(1,uiI + uiPS_Vel_Idx),dNion,dDens,dAbd * dDens,dTau_Line_Max,dLine_Max_Lambda);
	//		printf("%.2e,%.2e\t%.2e\t%.2f\t%.0f\t%.0f\t%.1e\n",dTemp_Alpha,dRlcl / dRps,pow(dRlcl / dRps,dTemp_Alpha),cVelocity.GetElement(1,uiI + uiPS_Vel_Idx),dTemp_Ref,dTempLcl,dTau_Line_Max);
//		}
		g_lpdTau[uiI] += dTau_Line_Max;
                if (g_lpdTau[uiI] > 1.0e300)
                    printf("Tau[i] fault\n");
	}
//	double	dLine_Max = 0.0;
	uiIref = 0;
	cIon_Data.Clean(); // clear initial one
	double	dRef_Velocity = cVelocity.GetElement(1,uiRef_Vel_Idx);
	for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
	{
		if (g_lpdTau[uiI] > dTau_Threshold)
		{
			double	dV[4] = {0.0,0.0,0.0,0.0};
			unsigned int uiIdx[4] = {0,0,0,0};
			double	dAux[3];
			double	dLog_Tau[3];
			double	dTau_Avg;
			unsigned int uiK;
			double dRlcl;
			double dTempLcl;
			cIon_Data.uiID = cElement.uiZ * 100 + cIon.uiIon_State - 1;
			cIon_Data.bEnable = bEnable;
			dV[0] = cVelocity.GetElement(1,uiI + uiPS_Vel_Idx);
			uiIdx[0] = uiI;

			double	dLog_Tau_Avg = log10(g_lpdTau[uiI]);
			unsigned int uiN = 1;
			bool bKeep_Going = true;
			for (uiJ = uiI + 1; uiJ < uiNum_Tau_Points && bKeep_Going; uiJ++)
			{
				bKeep_Going = false;
				for (uiK = uiJ; uiK < uiNum_Tau_Points && uiK < (uiJ + 10) && !bKeep_Going; uiK++)
					bKeep_Going |= (g_lpdTau[uiK] > dTau_Threshold);
					
				if (bKeep_Going)
				{
					dLog_Tau_Avg += log10(g_lpdTau[uiJ]);
					dV[3] = cVelocity.GetElement(1,uiJ + uiPS_Vel_Idx); // maximum velocity
					uiIdx[3] = uiJ;
					uiN++;
				}
			}
//			printf("%i points\n",uiN);
			if (uiN > 500) // make sure we have some reasonable size, N = 500 is about 500 km/s in v
			{
				dLog_Tau_Avg /= uiN;
				if (dLog_Tau_Avg > -2.0) // make sure that this is a line worth working on
				{
					dTau_Avg = pow(10.0,dLog_Tau_Avg);
					unsigned int uiImax = uiJ;
					bKeep_Going = true;
					for (uiJ = uiI + 1; uiJ < uiImax && bKeep_Going; uiJ++)
					{
						bKeep_Going = false;
						for (uiK = uiJ; uiK < uiNum_Tau_Points && uiK < (uiJ + 10) && !bKeep_Going; uiK++)
							bKeep_Going |= (g_lpdTau[uiK] < dTau_Avg);
					}
					uiIdx[1] = uiJ - 1;
					dV[1] = cVelocity.GetElement(1,uiJ + uiPS_Vel_Idx - 1); // velocity of
					bKeep_Going = true;
					for (; uiJ < uiImax && bKeep_Going; uiJ++)
					{
						bKeep_Going = false;
						for (uiK = uiJ; uiK < uiNum_Tau_Points && uiK < (uiJ + 10) && !bKeep_Going; uiK++)
							bKeep_Going |= (g_lpdTau[uiK] > dTau_Avg);
					}
					uiIdx[2] = uiJ - 1;
					dV[2] = cVelocity.GetElement(1,uiJ + uiPS_Vel_Idx - 1);

					// rising edge
					dRlcl = cVelocity.GetElement(0,(uiIdx[0] + uiIdx[1]) / 2 + uiPS_Vel_Idx) + cVelocity.GetElement(1,(uiIdx[0] + uiIdx[1]) / 2 + uiPS_Vel_Idx) * 1.0e9 * (dTime - dTimeRef);
					dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
					dAux[0] = (dV[0] - dV[1]) / log(g_lpdTau[uiIdx[1]]/g_lpdTau[uiIdx[0]]);
					dLog_Tau[0] = log10(exp((dV[1] - dRef_Velocity) / dAux[0])) + dLog_Tau_Avg;

					if (dV[1] > dV[0] + 0.01) // skip the rising edge if it is really thin
					{
						cIon_Data.dVmax = dV[1];
						cIon_Data.dVmin = dV[0];
						cIon_Data.dE_Folding = dAux[0];
						cIon_Data.dLog_Tau = dLog_Tau[0];
						cIon_Data.dTemp = dTempLcl;
						cIon_Data_LL.Add(cIon_Data);
					}
		

					//center
					unsigned int uiDelta_Idx = uiIdx[2] - uiIdx[1];
					if (uiDelta_Idx > 50)
					{
						unsigned int uiTest_Idx[2];
						double dLog_Tau_Test_1 = 0.0,dLog_Tau_Test_2 = 0.0;

						dRlcl = cVelocity.GetElement(0,(uiIdx[1] + uiIdx[2]) / 2 + uiPS_Vel_Idx) + cVelocity.GetElement(1,(uiIdx[1] + uiIdx[2]) / 2 + uiPS_Vel_Idx) * 1.0e9 * (dTime - dTimeRef);
						dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
						uiTest_Idx[0] = uiDelta_Idx / 10 + uiIdx[1];
						uiTest_Idx[1] = (uiDelta_Idx / 10) * 9 + uiIdx[1];
						for (uiJ = uiIdx[1]; uiJ < uiTest_Idx[0]; uiJ++)
							dLog_Tau_Test_1 += log10(g_lpdTau[uiJ]);
						dLog_Tau_Test_1 /= (uiTest_Idx[0] - uiIdx[1]);
						for (uiJ = uiTest_Idx[1]; uiJ < uiIdx[2]; uiJ++)
							dLog_Tau_Test_2 += log10(g_lpdTau[uiJ]);
						dLog_Tau_Test_2 /= (uiIdx[2] - uiTest_Idx[1]);

						dAux[1] = (cVelocity.GetElement(1,uiTest_Idx[1] + uiPS_Vel_Idx) - cVelocity.GetElement(1,uiTest_Idx[0] + uiPS_Vel_Idx)) / log(pow(10.0,-dLog_Tau_Test_2 + dLog_Tau_Test_1));
						dLog_Tau[1] = log10(exp((cVelocity.GetElement(1,uiIdx[1] + uiPS_Vel_Idx) - dRef_Velocity) / dAux[1])) + dLog_Tau_Test_1;

						cIon_Data.dVmax = dV[2];
						cIon_Data.dVmin = dV[1];
						cIon_Data.dE_Folding = dAux[1];
						cIon_Data.dLog_Tau = dLog_Tau[1];
						cIon_Data.dTemp = dTempLcl;
						cIon_Data_LL.Add(cIon_Data);
					}

					// falling edge
					dRlcl = cVelocity.GetElement(0,(uiIdx[2] + uiIdx[3]) / 2 + uiPS_Vel_Idx) + cVelocity.GetElement(1,(uiIdx[2] + uiIdx[3]) / 2 + uiPS_Vel_Idx) * 1.0e9 * (dTime - dTimeRef);
					dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
					dAux[2] = (dV[2] - dV[3]) / log(g_lpdTau[uiIdx[3]]/g_lpdTau[uiIdx[2]]);
					dLog_Tau[2] = log10(exp((dV[2] - dRef_Velocity) / dAux[2])) + dLog_Tau_Avg;

					cIon_Data.dVmax = dV[3];
					cIon_Data.dVmin = dV[2];
					cIon_Data.dE_Folding = dAux[2];
					cIon_Data.dLog_Tau = dLog_Tau[2];
					cIon_Data.dTemp = dTempLcl;
					cIon_Data_LL.Add(cIon_Data);

				}
			}
			uiI += (uiN - 1); // advance to end
		}
	}
	//printf("%i %i\n",uiRef_Vel_Idx,uiPS_Vel_Idx);
//	uiIref = (uiRef_Vel_Idx - uiPS_Vel_Idx);
//	cIon_Data.dLog_Tau = log10(lpdTau[uiIref]);
	//printf("%.1f\n",cIon_Data.dLog_Tau);

//	delete [] lpdTau;
	//printf("%.1f folding\n",cIon_Data.dE_Folding);
}

void FillIonDataElement(SYNIONDATA_LL &cSynIonDataLL,	const XASTROLINE_ELEMENTDATA &cElement,const double &dTime, const double &dTimeRef, const double &dTemp_Ref, const double &dTemp_Alpha, const XDATASET &cVelocity, const XDATASET &cEle_Abd, const XDATASET &cDens, unsigned int uiRef_Vel_Idx, unsigned int uiPS_Vel_Idx, const double * lpdElectron_Density, bool bEnable = true)
{
//	SYNIONDATA cTemp;
	unsigned int uiN;
	for (uiN = 0; uiN < cElement.uiZ; uiN++)
	{
		if (cElement.lpcIonData[uiN].uiNum_Lines > 0 && (cElement.uiZ != 20 || uiN != 2))
		{
			printf("\tIon %i\n",uiN);
			FillIonData(cSynIonDataLL, cElement, cElement.lpcIonData[uiN], dTime, dTimeRef, dTemp_Ref, dTemp_Alpha, cVelocity, cEle_Abd, cDens, uiRef_Vel_Idx, uiPS_Vel_Idx,lpdElectron_Density,bEnable);
		}
	}
}


double Parse_Command_Line_Dbl(int iArgC,const char * lpszArgV[],const char * i_lpszOption, const double &dDefault_Value = -1.0e300)
{ // if the command line option is say "data", this is what input option should be
// e.g. command data = 10
// if option is -data, (e.g. command -data 10) then pass "-data" as option input
// = sign is optional (-data=10, -data = 10, -data 10 will all be read)
	double	dRet = dDefault_Value; 
	bool	bFound = false;
	unsigned int uiI;
	size_t iOption_Len = strlen(i_lpszOption);
	for (uiI = 1; uiI < iArgC && !bFound; uiI++) // start from 1 - 0 is the command
	{
		if (strncmp(lpszArgV[uiI],i_lpszOption,iOption_Len) == 0)
		{
			const char * lpszCursor = lpszArgV[uiI];
			lpszCursor += iOption_Len; // advance past option string
			if (lpszCursor[0] == '=')
			{
				lpszCursor++;
				if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
				{
					lpszCursor = lpszArgV[uiI + 1];
				}
			}
			else if (lpszCursor[0] == 0 && uiI < (iArgC - 1))
			{
				lpszCursor = lpszArgV[uiI + 1];
				if (lpszCursor[0] == '=')
				{
					lpszCursor++;
					if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
						lpszCursor = lpszArgV[uiI + 2];
				}
			}
			else
				continue;

			if (xIsANumber(lpszCursor))
			{
				dRet = atof(lpszCursor);
				bFound = true;
			}
			else
			{
				printf("Command line option %s contains an invalid value ('%s').\n",i_lpszOption,lpszCursor);
			}
		}
	}
	return dRet;
}

bool Parse_Command_Line_Bool(int iArgC,const char * lpszArgV[],const char * i_lpszOption, const double &bDefault_Value = false)
{ // if the command line option is say "data", this is what input option should be
// e.g. command data = 10
// if option is -data, (e.g. command -data 10) then pass "-data" as option input
// = sign is optional (-data=10, -data = 10, -data 10 will all be read)
	bool	bRet = bDefault_Value; 
	bool	bFound = false;
	unsigned int uiI;
	size_t iOption_Len = strlen(i_lpszOption);
	char lpszTemp[7];
	for (uiI = 1; uiI < iArgC && !bFound; uiI++) // start from 1 - 0 is the command
	{
		if (strncmp(lpszArgV[uiI],i_lpszOption,iOption_Len) == 0)
		{
			const char * lpszCursor = lpszArgV[uiI];
			lpszCursor += iOption_Len; // advance past option string
			if (lpszCursor[0] == '=')
			{
				lpszCursor++;
				if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
				{
					lpszCursor = lpszArgV[uiI + 1];
				}
			}
			else if (lpszCursor[0] == 0 && uiI < (iArgC - 1))
			{
				lpszCursor = lpszArgV[uiI + 1];
				if (lpszCursor[0] == '=')
				{
					lpszCursor++;
					if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
						lpszCursor = lpszArgV[uiI + 2];
				}
			}
			else
				continue;

			lpszTemp[6] = 0;
			lpszTemp[0] = tolower(lpszCursor[0]);
			lpszTemp[1] = tolower(lpszCursor[1]);
			lpszTemp[2] = tolower(lpszCursor[2]);
			lpszTemp[3] = tolower(lpszCursor[3]);
			lpszTemp[4] = tolower(lpszCursor[4]);
			lpszTemp[5] = tolower(lpszCursor[5]);
			if ((strcmp(lpszTemp,"yes") == 0) || (strcmp(lpszTemp,"true") == 0) || (strcmp(lpszTemp,".true.") == 0) || (strcmp(lpszTemp,"y") == 0))
			{
				bRet = true;
				bFound = true;
			}
			else if ((strcmp(lpszTemp,"no") == 0) || (strcmp(lpszTemp,"false") == 0) || (strcmp(lpszTemp,".false.") == 0) || (strcmp(lpszTemp,"n") == 0))
			{
				bRet = false;
				bFound = true;
			}
			else
			{
				printf("Command line option %s contains an invalid value ('%s').\n",i_lpszOption,lpszCursor);
			}
		}
	}
	return bRet;
}

unsigned int Parse_Command_Line_UInt(int iArgC,const char * lpszArgV[],const char * i_lpszOption, unsigned int i_uiDefault_Value = -1)
{ // if the command line option is say "data", this is what input option should be
// e.g. command data = 10
// if option is -data, (e.g. command -data 10) then pass "-data" as option input
// = sign is optional (-data=10, -data = 10, -data 10 will all be read)
	unsigned int	uiRet = i_uiDefault_Value; 
	bool	bFound = false;
	unsigned int uiI;
	size_t iOption_Len = strlen(i_lpszOption);
	for (uiI = 1; uiI < iArgC && !bFound; uiI++) // start from 1 - 0 is the command
	{
		if (strncmp(lpszArgV[uiI],i_lpszOption,iOption_Len) == 0)
		{
			const char * lpszCursor = lpszArgV[uiI];
			lpszCursor += iOption_Len; // advance past option string
			if (lpszCursor[0] == '=')
			{
				lpszCursor++;
				if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
				{
					lpszCursor = lpszArgV[uiI + 1];
				}
			}
			else if (lpszCursor[0] == 0 && uiI < (iArgC - 1))
			{
				lpszCursor = lpszArgV[uiI + 1];
				if (lpszCursor[0] == '=')
				{
					lpszCursor++;
					if (lpszCursor[0] == 0 && (uiI < (iArgC - 1)))
						lpszCursor = lpszArgV[uiI + 2];
				}
			}
			else
				continue;

			if (xIsANumber(lpszCursor))
			{
				uiRet = atoi(lpszCursor);
				bFound = true;
			}
			else
			{
				printf("Command line option %s contains an invalid value ('%s').\n",i_lpszOption,lpszCursor);
			}
		}
	}
	return uiRet;
}

void Report_Element_Tau(SYNIONDATA_LL &cSynIonDataLL,	const XASTROLINE_ELEMENTDATA &cElement,const double &dTime, const double &dTimeRef, const double &dTemp_Ref, const double &dTemp_Alpha, const XDATASET &cVelocity, const XDATASET &cEle_Abd, const XDATASET &cDens, unsigned int uiRef_Vel_Idx, unsigned int uiPS_Vel_Idx, const double * lpdElectron_Density, bool bEnable, const char * lpszElement_ID)
{
	unsigned int uiN;
	char lpszFilename[64];
	FILE * fileOut;
	SYNIONDATA_LL cTemp;
	SYNIONDATA_NODE * lpCurr;
	for (uiN = 0; uiN < cElement.uiZ; uiN++)
	{
		if (cElement.lpcIonData[uiN].uiNum_Lines > 0 && (cElement.uiZ != 20 || uiN != 2))
		{
			printf("\tIon %i\n",uiN);
			FillIonData(cTemp, cElement, cElement.lpcIonData[uiN], dTime, dTimeRef, dTemp_Ref, dTemp_Alpha, cVelocity, cEle_Abd, cDens, uiRef_Vel_Idx, uiPS_Vel_Idx,lpdElectron_Density,bEnable);

			sprintf(lpszFilename,"./results/tau/tau_d%i_T%.0f_a%i_ps%3.0f_%s_%s.csv",uiJ,dTemp,uiL,dPhotosphere_Velocity[2] * 100.0,lpszElement_ID);
			fileOut = fopen(lpszFilename,"wt");
			lpCurr = cSynIonDataLL.lpHead;

			unsigned int uiNum_Points_Vel = cVelocity.GetNumElements();
			unsigned int uiNum_Tau_Points = uiNum_Points_Vel - uiPS_Velocity_Idx[2];// + 1;
			double dVelocityPrev = cVelocity.GetElement(1,uiPS_Velocity_Idx[2] - 1); 
			double dVelocity = cVelocity.GetElement(1,uiPS_Velocity_Idx[2]);
			double dVelocityNext;// = cVelocity.GetElement(1,uiPS_Velocity_Idx_Forced + 1);
			for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
			{
				dVelocityNext = cVelocity.GetElement(1,uiPS_Velocity_Idx[2] + uiI + 1);
				if (dVelocity < dPhotosphere_Velocity[2])
					uiI = uiNum_Tau_Points;
				else
				{
					double	dTauV = 0.0;
					SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;
					while (lpCurr)
					{
						if (lpCurr->cData.dVmin <= dVelocity && lpCurr->cData.dVmax >= dVelocity)
						{
							dTauV += pow(10.0,lpCurr->cData.dLog_Tau) * exp((dRef_Velocity - dVelocity) / lpCurr->cData.dE_Folding);
						}
				

						lpCurr = lpCurr->lpNext;
					}
			
					double dAux_Lcl = (dVelocityNext - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI + 1]);
					if (uiI != 0)
					{
							dAux_Lcl += (dVelocityPrev - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI - 1]);
							dAux_Lcl *= 0.5;
					}
					double dTau_Ref_Lcl = g_lpdTau[uiI] * exp((dVelocity - dRef_Velocity) / dAux_Lcl);
					fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",cVelocity.GetElement(0,uiI + uiPS_Velocity_Idx[2]),dVelocity,g_lpdTau[uiI],dTauV,dTau_Ref_Lcl,dAux_Lcl);
				}
				dVelocityPrev = dVelocity;
				dVelocity = dVelocityNext;

			}
			fclose(fileOut);
			double	dLog_Tau_Avg = 0.0;
			unsigned int uiN = 0;
			lpCurr = cSynIonDataLL.lpHead;
			while (lpCurr)
			{
				printf("min = %.2f\tmax = %.2f\taux = %.2e\ttau = %.2e:\t",lpCurr->cData.dVmin,lpCurr->cData.dVmax,lpCurr->cData.dE_Folding,lpCurr->cData.dLog_Tau);
				for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
				{
					dVelocity = cVelocity.GetElement(1,uiPS_Velocity_Idx_Forced + uiI);
					if (lpCurr->cData.dVmin <= dVelocity && lpCurr->cData.dVmax >= dVelocity)
					{
						dLog_Tau_Avg += log10(g_lpdTau[uiI]);
						uiN++;
					}
				}
				dLog_Tau_Avg /= uiN;
				printf("Estimate %.2e avg tau\n",pow(10.0,dLog_Tau_Avg));
				dLog_Tau_Avg = 0.0;
				uiN = 0;

				lpCurr = lpCurr->lpNext;
			}
		}
	}
}
int main(int iArgC,const char * lpszArgV[])
{
//	char lpszLine_Buffer[1024];
	double	dTimeRef;// = 50.0;
	double	dTemp;// = 12000.0;
//	double	dCaAbdAdj = 0.0;
//	double	dSiAbdAdj = 0.0;
//	double	dFeAbdAdj = 0.0;
	unsigned int uiK;
	unsigned int uiDmin,uiDmax,uiDstep;
	unsigned int uiTmin,uiTmax,uiTstep;
	unsigned int uiAmin,uiAmax,uiAstep;
	bool bEnable_H, bEnable_He, bEnable_C, bEnable_N, bEnable_O, bEnable_Ne, bEnable_Mg, bEnable_Si, bEnable_S, bEnable_Ar, bEnable_Ca, bEnable_Ti, bEnable_Cr, bEnable_Fe, bEnable_Co, bEnable_Ni;
	double	dForce_PS_Vel = -1.0;

	unsigned int uiShell_Lower_Idx = 0;
	double	dVel_Max = 0.0;
	double	dRef_Velocity;
	unsigned int uiVel_Ref_Idx = 0;

    double		dPhotosphere_Velocity[4] = {0.0,0.0,0.0,0.0};
    unsigned int uiPS_Velocity_Idx[4] = {0,0,0,0};

	XDATASET	cVelocity;
	XDATASET	cDens;
	XDATASET	cHyd;
	XDATASET	cHe3;
	XDATASET	cHe4;
	XDATASET	cHeComb;
	XDATASET	cC;
	XDATASET	cN;
	XDATASET	cO;
	XDATASET	cNe;
	XDATASET	cMg;
	XDATASET	cSi;
	XDATASET	cS;
	XDATASET	cAr;
	XDATASET	cCa;
	XDATASET	cTi44;
	XDATASET	cTi48;
	XDATASET	cTiComb; // Ti 44 and Ti 48 (decay of Cr 48)
	XDATASET	cCr48;
	XDATASET	cCr52;
	XDATASET	cCrComb;
	XDATASET	cFe52;
	XDATASET	cFe56;
	XDATASET	cFeComb;
	XDATASET	cNi56;
//	XDATASET 	cCaHK_Line_Results;
//	XDATASET 	cCaHK_Spectra_Results;
	unsigned int uiI,uiJ,uiL;

	double	* lpdElectron_Density[4];
	double * lpdElectron_Density_Temp;

	SYNIONDATA_LL	cSynIonDataLL;
	unsigned int uiPS_Velocity_Idx_Forced = 0;
	unsigned int uiNum_Points;
	double	dElectron_Scaling = 1.0;

	dTimeRef = Parse_Command_Line_Dbl(iArgC,lpszArgV,"-time_ref",50.0);
	printf("Time ref set to %.1f s\n",dTimeRef);
	uiDmin = Parse_Command_Line_UInt(iArgC,lpszArgV,"-day_start",0);
	uiDmax = Parse_Command_Line_UInt(iArgC,lpszArgV,"-day_end",25);
	uiDstep = Parse_Command_Line_UInt(iArgC,lpszArgV,"-day_step",1);
	uiTmin = Parse_Command_Line_UInt(iArgC,lpszArgV,"-temp_start",7000);
	uiTmax = Parse_Command_Line_UInt(iArgC,lpszArgV,"-temp_end",15000);
	uiTstep = Parse_Command_Line_UInt(iArgC,lpszArgV,"-temp_step",500);
	uiAmin = Parse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_start",0);
	uiAmax = Parse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_end",10);
	uiAstep = Parse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_step",1);

	bEnable_H  = Parse_Command_Line_Bool(iArgC,lpszArgV,"--H",true);
	bEnable_He = Parse_Command_Line_Bool(iArgC,lpszArgV,"--He",true);
	bEnable_C  = Parse_Command_Line_Bool(iArgC,lpszArgV,"--C",true);
	bEnable_N  = Parse_Command_Line_Bool(iArgC,lpszArgV,"--N",true);
	bEnable_O  = Parse_Command_Line_Bool(iArgC,lpszArgV,"--O",true);
	bEnable_Ne = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Ne",true);
	bEnable_Mg = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Mg",true);
	bEnable_Si = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Si",true);
	bEnable_S  = Parse_Command_Line_Bool(iArgC,lpszArgV,"--S",true);
	bEnable_Ar = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Ar",true);
	bEnable_Ca = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Ca",true);
	bEnable_Ti = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Ti",true);
	bEnable_Cr = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Cr",true);
	bEnable_Fe = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Fe",false); // Iron is slow - disable by default
	bEnable_Co = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Co",true);
	bEnable_Ni = Parse_Command_Line_Bool(iArgC,lpszArgV,"--Ni",true);

	dForce_PS_Vel = Parse_Command_Line_Dbl(iArgC,lpszArgV,"--forced_ps",-1.0);

	uiDmax++;
	uiTmax++;
	uiAmax++;

	XASTROLINE_ELEMENTDATA	cH_Lines;
	XASTROLINE_ELEMENTDATA	cHe_Lines;
	XASTROLINE_ELEMENTDATA	cC_Lines;
	XASTROLINE_ELEMENTDATA	cN_Lines;
	XASTROLINE_ELEMENTDATA	cO_Lines;
	XASTROLINE_ELEMENTDATA	cNe_Lines;
	XASTROLINE_ELEMENTDATA	cMg_Lines;
	XASTROLINE_ELEMENTDATA	cSi_Lines;
	XASTROLINE_ELEMENTDATA	cS_Lines;
	XASTROLINE_ELEMENTDATA	cAr_Lines;
	XASTROLINE_ELEMENTDATA	cCa_Lines;
	XASTROLINE_ELEMENTDATA	cTi_Lines;
	XASTROLINE_ELEMENTDATA	cV_Lines;
	XASTROLINE_ELEMENTDATA	cCr_Lines;
	XASTROLINE_ELEMENTDATA	cMn_Lines;
	XASTROLINE_ELEMENTDATA	cFe_Lines;
	XASTROLINE_ELEMENTDATA	cCo_Lines;
	XASTROLINE_ELEMENTDATA	cNi_Lines;

	cH_Lines.ReadDataFile("../Data","H");
	cHe_Lines.ReadDataFile("../Data","He");
	cC_Lines.ReadDataFile("../Data","C");
	cN_Lines.ReadDataFile("../Data","N");
	cO_Lines.ReadDataFile("../Data","O");
	cNe_Lines.ReadDataFile("../Data","Ne");
	cMg_Lines.ReadDataFile("../Data","Mg");
	cSi_Lines.ReadDataFile("../Data","Si");
	cS_Lines.ReadDataFile("../Data","S");
	cAr_Lines.ReadDataFile("../Data","Ar");
	cCa_Lines.ReadDataFile("../Data","Ca");
	cTi_Lines.ReadDataFile("../Data","Ti");
	cV_Lines.ReadDataFile("../Data","V");
	cCr_Lines.ReadDataFile("../Data","Cr");
	cMn_Lines.ReadDataFile("../Data","Mn");
	cFe_Lines.ReadDataFile("../Data","Fe");
	cCo_Lines.ReadDataFile("../Data","Co");
	cNi_Lines.ReadDataFile("../Data","Ni");


	cVelocity.ReadDataFile("velx.txt", true, false);
	cDens.ReadDataFile("dens.txt", true, false);
	cHyd.ReadDataFile("hyd.txt",true,false);
	cHe3.ReadDataFile("he3.txt",true,false);
    cHe4.ReadDataFile("he4.txt",true,false);
	cC.ReadDataFile("c12.txt",true,false);
	cN.ReadDataFile("n14.txt",true,false);
	cO.ReadDataFile("o16.txt",true,false);
	cNe.ReadDataFile("ne20.txt",true,false);
	cMg.ReadDataFile("mg24.txt",true,false);
	cSi.ReadDataFile("si28.txt", true, false);
	cS.ReadDataFile("s32.txt",true,false);
	cAr.ReadDataFile("ar36.txt",true,false);
	cCa.ReadDataFile("ca40.txt", true, false);
	cTi44.ReadDataFile("ti44.txt", true, false);
	cTi48.ReadDataFile("ti48.txt", true, false);
	cCr48.ReadDataFile("cr48.txt", true, false);
	cCr52.ReadDataFile("cr52.txt", true, false);
	cFe52.ReadDataFile("fe52.txt", true, false);
	cFe56.ReadDataFile("fe56.txt", true, false);
	cNi56.ReadDataFile("ni56.txt",true,false);

	uiNum_Points = cVelocity.GetNumElements();

    cHeComb.Allocate(2,uiNum_Points);
    cTiComb.Allocate(2,uiNum_Points);
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		double dSum;		
		cHeComb.SetElement(0,uiI,cVelocity.GetElement(0,uiI));
		cHeComb.SetElement(1,uiI,cHe3.GetElement(1,uiI) + cHe4.GetElement(1,uiI));


		dSum = 0.0;
		if (cTi44.GetNumElements() == uiNum_Points)
			dSum += cTi44.GetElement(1,uiI);
		if (cTi48.GetNumElements() == uiNum_Points)
			dSum += cTi48.GetElement(1,uiI);
		if (cCr48.GetNumElements() == uiNum_Points)
			dSum += cCr48.GetElement(1,uiI);
		cTiComb.SetElement(0,uiI,cVelocity.GetElement(0,uiI));
		cTiComb.SetElement(1,uiI,dSum);

		dSum = 0.0;
		if (cCr52.GetNumElements() == uiNum_Points)
			dSum += cCr52.GetElement(1,uiI);
		if (cFe52.GetNumElements() == uiNum_Points)
			dSum += cFe52.GetElement(1,uiI);
		cCrComb.SetElement(0,uiI,cVelocity.GetElement(0,uiI));
		cCrComb.SetElement(1,uiI,dSum);

		dSum = 0.0;
		if (cFe56.GetNumElements() == uiNum_Points)
			dSum += cFe56.GetElement(1,uiI);
		if (cNi56.GetNumElements() == uiNum_Points)
			dSum += cNi56.GetElement(1,uiI);
		cFeComb.SetElement(0,uiI,cVelocity.GetElement(0,uiI));
		cFeComb.SetElement(1,uiI,dSum);
	}

	lpdElectron_Density[0] = new double[uiNum_Points];
	lpdElectron_Density[1] = new double[uiNum_Points];
	lpdElectron_Density[2] = new double[uiNum_Points];
	lpdElectron_Density[3] = new double[uiNum_Points];
	lpdElectron_Density_Temp = new double[uiNum_Points];

	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		if (cDens.GetElement(1,uiI) > 1.0e-7) // exclude low density stuff
		{
			if (cVelocity.GetElement(1,uiI) > dVel_Max)
				dVel_Max = cVelocity.GetElement(1,uiI);
		}
	}

	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		double	dNumber_Density = cDens.GetElement(1,uiI) * (50.0 * 50.0 * 50.0); // 50 = time of sample
		if (dNumber_Density < 1.0e-9) // make the true CSM very low density
			dNumber_Density *= 1e-14;
		dNumber_Density /= g_XASTRO.k_dmh;
		double	dAbd[][2] = {
			{GetAbundance(uiI,cHyd),1.0},
			{GetAbundance(uiI,cHe3),3.0},
			{GetAbundance(uiI,cHe4),4.0},
			{GetAbundance(uiI,cC),12.0},
			{GetAbundance(uiI,cN),14.0},
			{GetAbundance(uiI,cO),16.0},
			{GetAbundance(uiI,cNe),20.0},
			{GetAbundance(uiI,cMg),24.0},
			{GetAbundance(uiI,cSi),28.0},
			{GetAbundance(uiI,cS),32.0},
			{GetAbundance(uiI,cAr),36.0},
			{GetAbundance(uiI,cCa)  ,40.0},
			{GetAbundance(uiI,cTi44),44.0},
			{GetAbundance(uiI,cTi48),48.0},
			{GetAbundance(uiI,cCr48),48.0},
			{GetAbundance(uiI,cCr52),52.0},
			{GetAbundance(uiI,cFe52),52.0},
			{GetAbundance(uiI,cFe56),56.0},
			{GetAbundance(uiI,cNi56),56.0}
			};

		unsigned int uiNum_Abd = sizeof(dAbd) / (2 * sizeof(double));

		for (uiJ = 0; uiJ < uiNum_Abd; uiJ++)
		{
			// estimate singly ionized ion density
			lpdElectron_Density[0][uiI] += dNumber_Density * dAbd[uiJ][0] / dAbd[uiJ][1];

			// doubly ionized ion density
			if (uiJ != 0)
			{
				lpdElectron_Density[1][uiI] = dNumber_Density * dAbd[uiJ][0] / dAbd[uiJ][1]; // singly ionized
				lpdElectron_Density[2][uiI] = dNumber_Density * dAbd[uiJ][0] / dAbd[uiJ][1]; // double ionized
				lpdElectron_Density[3][uiI] += 0.5 * dNumber_Density * dAbd[uiJ][0]; // fully ionized
			}

		}
		lpdElectron_Density[1][uiI] += lpdElectron_Density[0][uiI];
		lpdElectron_Density[2][uiI] += lpdElectron_Density[1][uiI];
		lpdElectron_Density[3][uiI] += dNumber_Density * dAbd[0][0];
	}
	double	dElectron_Optical_Depth_t0[4] = {0.0,0.0,0.0,0.0};
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		double dDeltaX;
		if (uiI != uiNum_Points - 1)
		{
			double dX1 = cVelocity.GetElement(0,uiI+1) - cVelocity.GetElement(1,uiI+1) * 50.0;
			double dX2 = cVelocity.GetElement(0,uiI) - cVelocity.GetElement(1,uiI) * 50.0;
			dDeltaX = dX1 - dX2;
		}
		else
			dDeltaX = 0.0;
		double	dConst_Terms = 0.66524574e-24;
		dElectron_Optical_Depth_t0[0] += lpdElectron_Density[0][uiI] * dConst_Terms * dDeltaX;
		dElectron_Optical_Depth_t0[1] += lpdElectron_Density[1][uiI] * dConst_Terms * dDeltaX;
		dElectron_Optical_Depth_t0[2] += lpdElectron_Density[2][uiI] * dConst_Terms * dDeltaX;
		dElectron_Optical_Depth_t0[3] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX;
//		double	dKappa_0 = lpdElectron_Density[0][uiI] * dConst_Terms
//		printf("%.3e\t%.3e\t%.3e\t%.3e\t%.3e\t%.3e\n",dDeltaX,cDens.GetElement(1,uiI) / (12 * g_XASTRO.k_dmh),lpdElectron_Density[0][uiI],lpdElectron_Density[1][uiI],lpdElectron_Density[2][uiI],lpdElectron_Density[0][uiI] * 0.66524574e-24 * dDeltaX);
	}
	printf("------------------------------------\n%.3e\t%.3e\t%.3e\t%.3e\n--------------------------------------\n",dElectron_Optical_Depth_t0[0],dElectron_Optical_Depth_t0[1],dElectron_Optical_Depth_t0[2],dElectron_Optical_Depth_t0[3]);

	uiShell_Lower_Idx = 0;
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		if (GetAbundance(uiI,cHe3) > 0.0)
			uiShell_Lower_Idx = uiI;
	}

    for (uiJ = uiDmin; uiJ < uiDmax; uiJ += uiDstep)
    {
//		uiJ = 20; //@@DEBUG
        double dTime = 86500.0 * uiJ + 43250.0;
        double	dElectron_Optical_Depth[5] = {0.0,0.0,0.0,0.0,0.0};
        double	dConst_Terms = 0.66524574e-24 * pow(dTime,-3.0);
        dPhotosphere_Velocity[0] = dPhotosphere_Velocity[1] = dPhotosphere_Velocity[2] = dPhotosphere_Velocity[3] = 0.0;
        uiPS_Velocity_Idx[0] = uiPS_Velocity_Idx[1] = uiPS_Velocity_Idx[2] = uiPS_Velocity_Idx[3] = 0;
        memcpy(lpdElectron_Density_Temp,lpdElectron_Density[3],sizeof(double) * uiNum_Points);

        for (uiI = uiNum_Points - 1; uiI < uiNum_Points; uiI--)
        {
                // compute optical depth for electron scattering, assuming partial ionizations
                double	dDeltaX;
                if (uiI < uiNum_Points - 1)
                {
                        double dX1 = cVelocity.GetElement(0,uiI+1) + cVelocity.GetElement(1,uiI+1) * 1000000000.0 * (dTime - 50.0);
                        double dX2 = cVelocity.GetElement(0,uiI) + cVelocity.GetElement(1,uiI) * 1000000000.0 * (dTime - 50.0);
                        dDeltaX = dX1 - dX2;
                }
                else
                        dDeltaX = 0;
//			printf("%.3e\t%.3e\t%.3e\n",lpdElectron_Density[0][uiI],dDeltaX,dElectron_Optical_Depth[0]);
                dElectron_Optical_Depth[0] += lpdElectron_Density[0][uiI] * dConst_Terms * dDeltaX;
                if (dElectron_Optical_Depth[0] > 1.0 && dPhotosphere_Velocity[0] == 0.0)
                {
                        dPhotosphere_Velocity[0] = cVelocity.GetElement(1,uiI);
                        uiPS_Velocity_Idx[0] = uiI;
                }
                dElectron_Optical_Depth[1] += lpdElectron_Density[1][uiI] * dConst_Terms * dDeltaX;
                if (dElectron_Optical_Depth[1] > 1.0 && dPhotosphere_Velocity[1] == 0.0)
                {
                        dPhotosphere_Velocity[1] = cVelocity.GetElement(1,uiI);
                        uiPS_Velocity_Idx[1] = uiI;
                }
                dElectron_Optical_Depth[2] += lpdElectron_Density[2][uiI] * dConst_Terms * dDeltaX;
                if (dElectron_Optical_Depth[2] > 1.0 && dPhotosphere_Velocity[2] == 0.0)
                {
                        dPhotosphere_Velocity[2] = cVelocity.GetElement(1,uiI);
                        uiPS_Velocity_Idx[2] = uiI;
                }
                dElectron_Optical_Depth[3] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX;
                if (dElectron_Optical_Depth[3] > 1.0 && dPhotosphere_Velocity[3] == 0.0)
                {
                        dPhotosphere_Velocity[3] = cVelocity.GetElement(1,uiI);
                        uiPS_Velocity_Idx[3] = uiI;
                }
        }

		if (dForce_PS_Vel > 0.0)
		{
		    uiI = 0;
		    while (cVelocity.GetElement(1,uiI) < dForce_PS_Vel)
		            uiI++;
		    uiPS_Velocity_Idx_Forced = uiI;
		    printf("Forced PS Velocity = %.1f (Idx %i)\n",dForce_PS_Vel,uiPS_Velocity_Idx_Forced);
		    for (;uiI < uiNum_Points; uiI++)
		    {
		            // compute optical depth for electron scattering, assuming partial ionizations
		            double	dDeltaX;
		            if (uiI < uiNum_Points - 1)
		            {
		                    double dX1 = cVelocity.GetElement(0,uiI+1) + cVelocity.GetElement(1,uiI+1) * 1000000000.0 * (dTime - 50.0);
		                    double dX2 = cVelocity.GetElement(0,uiI) + cVelocity.GetElement(1,uiI) * 1000000000.0 * (dTime - 50.0);
		                    dDeltaX = dX1 - dX2;
		            }
		            else
		                    dDeltaX = 0;
	//			printf("%.3e\t%.3e\t%.3e\n",lpdElectron_Density[0][uiI],dDeltaX,dElectron_Optical_Depth[0]);
		            dElectron_Optical_Depth[4] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX;
		    }
		    double	dElectron_Scaling = 1.0 / dElectron_Optical_Depth[4];// * 1.0e5;// 10^5 to correct for 
			if (dElectron_Scaling > 1.0)
				dElectron_Scaling = 1.0; // don't allow it to go > 1

	        dPhotosphere_Velocity[2] = dForce_PS_Vel;// force
			uiPS_Velocity_Idx[2] = uiPS_Velocity_Idx_Forced;

			if (dElectron_Scaling < 1.0)
			{
				for (uiI = 0; uiI < uiNum_Points; uiI++)
				{
				        lpdElectron_Density_Temp[uiI] *= dElectron_Scaling;
				}
			}
		}
        printf("%i   %.2e   %.2e   %.2e   %.2e   %.2e   %.2e   %.2e   %.2e   %.2e\n",uiJ,dPhotosphere_Velocity[0],dPhotosphere_Velocity[1],dPhotosphere_Velocity[2],dPhotosphere_Velocity[3],dElectron_Optical_Depth[0],dElectron_Optical_Depth[1],dElectron_Optical_Depth[2],dElectron_Optical_Depth[3],dElectron_Scaling);
//	}


		// Set reference velocity for photosphere clculations
        dRef_Velocity = dPhotosphere_Velocity[2];
		uiVel_Ref_Idx = uiPS_Velocity_Idx[2];

        for (uiL = uiAmin; uiL < uiAmax; uiL+=uiAstep)
        {
//                        uiL = 0;//@@DEBUG
            double	dTemp_Alpha = uiL * -0.1; // T \propto 1/sqrt(r)
            
            for (uiK = uiTmin; uiK < uiTmax; uiK+=uiTstep)
			{
                dTemp = uiK;// * 500.0 + 7000.0;
                cSynIonDataLL.Clean();
                printf("-------------Day %i Temp %.0f Alpha %.1f --------------------------------------\n",uiJ,dTemp,-dTemp_Alpha);
	//			unsigned int uiSynIonDataIdx = 0;
/*				printf("Fill H data\n");
				FillIonDataElement(cSynIonDataLL, cH_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cHyd, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,true);
				printf("Fill He data\n");
				FillIonDataElement(cSynIonDataLL, cHe_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cHe3, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,true);
				printf("Fill C data\n");
				FillIonDataElement(cSynIonDataLL, cC_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cC, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,true);
				printf("Fill O data\n");
				FillIonDataElement(cSynIonDataLL, cO_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cO, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,false); // disabled because O tends to cause NaN faults in synow.
				printf("Fill Si data\n");
				FillIonDataElement(cSynIonDataLL, cSi_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cSi, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,true);
				printf("Fill Ca data\n");
				FillIonDataElement(cSynIonDataLL, cCa_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cCa, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,true);
*/
				printf("\tIon %i\n",1);
				FillIonData(cSynIonDataLL, cSi_Lines, cSi_Lines.lpcIonData[1], dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cSi, cDens, uiVel_Ref_Idx,  uiPS_Velocity_Idx[2],lpdElectron_Density_Temp,true);

	/*			printf("Fill Fe data\n");
				FillIonDataElement(cSynIonDataLL, cFe_Lines, dTime, dTimeRef, dTemp, dTemp_Alpha, cVelocity, cNi, cDens, uiVel_Ref_Idx, uiPS_Velocity_Idx_Forced,lpdElectron_Density_Temp,false); // Iron make synow really slow; exclude if possible
	*/
				char lpszFilename[64];
				sprintf(lpszFilename,"./results/tau_d%i_T%.0f_a%i_ps%3.0f.csv",uiJ,dTemp,uiL,dPhotosphere_Velocity[2] * 100.0);
				FILE * fileOut = fopen(lpszFilename,"wt");
				SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;

				unsigned int uiNum_Points_Vel = cVelocity.GetNumElements();
				unsigned int uiNum_Tau_Points = uiNum_Points_Vel - uiPS_Velocity_Idx[2];// + 1;
				double dVelocityPrev = cVelocity.GetElement(1,uiPS_Velocity_Idx[2] - 1); 
				double dVelocity = cVelocity.GetElement(1,uiPS_Velocity_Idx[2]);
				double dVelocityNext;// = cVelocity.GetElement(1,uiPS_Velocity_Idx_Forced + 1);
				for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
				{
					dVelocityNext = cVelocity.GetElement(1,uiPS_Velocity_Idx[2] + uiI + 1);
					if (dVelocity < dPhotosphere_Velocity[2])
						uiI = uiNum_Tau_Points;
					else
					{
						double	dTauV = 0.0;
						SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;
						while (lpCurr)
						{
							if (lpCurr->cData.dVmin <= dVelocity && lpCurr->cData.dVmax >= dVelocity)
							{
								dTauV += pow(10.0,lpCurr->cData.dLog_Tau) * exp((dRef_Velocity - dVelocity) / lpCurr->cData.dE_Folding);
							}
							

							lpCurr = lpCurr->lpNext;
						}
						
						double dAux_Lcl = (dVelocityNext - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI + 1]);
						if (uiI != 0)
						{
								dAux_Lcl += (dVelocityPrev - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI - 1]);
								dAux_Lcl *= 0.5;
						}
						double dTau_Ref_Lcl = g_lpdTau[uiI] * exp((dVelocity - dRef_Velocity) / dAux_Lcl);
						fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",cVelocity.GetElement(0,uiI + uiPS_Velocity_Idx[2]),dVelocity,g_lpdTau[uiI],dTauV,dTau_Ref_Lcl,dAux_Lcl);
					}
					dVelocityPrev = dVelocity;
					dVelocity = dVelocityNext;

				}
				fclose(fileOut);
				double	dLog_Tau_Avg = 0.0;
				unsigned int uiN = 0;
				lpCurr = cSynIonDataLL.lpHead;
				while (lpCurr)
				{
					printf("min = %.2f\tmax = %.2f\taux = %.2e\ttau = %.2e:\t",lpCurr->cData.dVmin,lpCurr->cData.dVmax,lpCurr->cData.dE_Folding,lpCurr->cData.dLog_Tau);
					for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
					{
						dVelocity = cVelocity.GetElement(1,uiPS_Velocity_Idx_Forced + uiI);
						if (lpCurr->cData.dVmin <= dVelocity && lpCurr->cData.dVmax >= dVelocity)
						{
							dLog_Tau_Avg += log10(g_lpdTau[uiI]);
							uiN++;
						}
					}
					dLog_Tau_Avg /= uiN;
					printf("Estimate %.2e avg tau\n",pow(10.0,dLog_Tau_Avg));
					dLog_Tau_Avg = 0.0;
					uiN = 0;

					lpCurr = lpCurr->lpNext;
				}


			}
		}
	}


//	delete [] cSynIonData;

	delete [] lpdElectron_Density[0];
	delete [] lpdElectron_Density[1];
	delete [] lpdElectron_Density[2];
	delete [] lpdElectron_Density[3];
	delete [] g_lpdTau;
	return 0;
}


