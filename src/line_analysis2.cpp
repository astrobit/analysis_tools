#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
#include <float.h>
#include <xflash.h>
#include <wordexp.h>

//#include <line_anal.h>

double g_dLog_Tau_Threshold = -3.0;
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
			if (cRHO.lpTail)
				lpTail = cRHO.lpTail;
		}
		else
		{
			lpHead = cRHO.lpHead;
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
				double dVminlcl = lpCurr->cData.dVmin;
				if (dVminlcl > 3.0e10) // limit to c
					dVminlcl = 3.0e10;
				if (bFirst)
					fprintf(fileOut,"%6.3f",dVminlcl * 1.0e-8); // 1.0e-8 = cm/s -> 1 Mm/s
				else
					fprintf(fileOut,", %6.3f",dVminlcl * 1.0e-8);
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
				if (dVmaxlcl > 3.0e10) // limit to c
					dVmaxlcl = 3.0e10;
				if (bFirst)
					fprintf(fileOut,"%6.3f",dVmaxlcl * 1.0e-8); // 1.0e-8 = cm/s -> 1 Mm/s
				else
					fprintf(fileOut,", %6.3f",dVmaxlcl * 1.0e-8);
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
                                double dEfolding = lpCurr->cData.dE_Folding * 1.0e-8;
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


void Output_YAML_File(const char * lpszFilename, const SYNIONDATA_LL	& cSynIonDataLL, const double & dData_Spacing, unsigned int uiDay, const double & dRef_Velocity, const double & dPhotosphere_Velocity, const double & dTemp)
{
	FILE * fileOut = fopen(lpszFilename,"wt");
	double	dVel_Max = 0.0;

	SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;
	while (lpCurr)
	{
		if (lpCurr->cData.dVmax > dVel_Max)
			dVel_Max = lpCurr->cData.dVmax;
		lpCurr = lpCurr->lpNext;
	}
	if (dVel_Max > 3.0e10)
	{
		printf("Warning: velmax > c.\n");
		dVel_Max = 3.0e10;
	}
	if (dVel_Max < dPhotosphere_Velocity)
		dVel_Max = dPhotosphere_Velocity + 1.0e5;

	if (fileOut)
	{
		fprintf(fileOut,"#-\n");
		fprintf(fileOut,"#- Day %i\n",uiDay);
		fprintf(fileOut,"#-\n");
		fprintf(fileOut,"---\n");
		fprintf(fileOut,"output :\n");
		fprintf(fileOut,"    min_wl      : 3000.0        # min. wavelength in AA\n");
		fprintf(fileOut,"    max_wl      : 10000.0        # max. wavelength in AA\n");
		fprintf(fileOut,"    wl_step     : %.1f           # wavelength spacing in AA\n",dData_Spacing);
		fprintf(fileOut,"grid :\n");
		fprintf(fileOut,"    bin_width   : %3.1f          # opacity bin size in kkm/s\n",0.1);
		fprintf(fileOut,"    v_size      : 100           # size of line-forming region grid\n");
		fprintf(fileOut,"    v_outer_max : %3.1f          # fastest ejecta velocity in kkm/s\n",dVel_Max * 1.0e-8);
		fprintf(fileOut,"opacity :\n");
//					fprintf(fileOut,"    line_dir    : /usr/local/share/es/lines     # path to atomic line data\n");
//					fprintf(fileOut,"    ref_file    : /usr/local/share/es/refs.dat  # path to ref. line data\n");
//@@For es-0.99 (my variant) the line dir and ref file are specified by environment variables
//		fprintf(fileOut,"    line_dir    : /data1/r900-3/bwmulligan/common/share/es/lines     # path to atomic line data\n");
//		fprintf(fileOut,"    ref_file    : /data1/r900-3/bwmulligan/common/share/es/refs.dat  # path to ref. line data\n");
		fprintf(fileOut,"    form        : exp           # parameterization (only exp for now)\n");
		fprintf(fileOut,"    v_ref       : %6.3f       # reference velocity for parameterization\n",dRef_Velocity * 1.0e-8);
		fprintf(fileOut,"    log_tau_min : %.1f          # opacity threshold\n",g_dLog_Tau_Threshold);
		fprintf(fileOut,"source :\n");
		fprintf(fileOut,"    mu_size     : 10            # number of angles for source integration\n");
		fprintf(fileOut,"spectrum :\n");
		fprintf(fileOut,"    p_size      : 60            # number of phot. impact parameters for spectrum\n");
		fprintf(fileOut,"    flatten     : No            # divide out continuum or not\n");
		fprintf(fileOut,"setups :\n");
		fprintf(fileOut,"    -   a0      :  1.0          # constant term\n");
		fprintf(fileOut,"        a1      :  0.0          # linear warp term\n");
		fprintf(fileOut,"        a2      :  0.0          # quadratic warp term\n");
		fprintf(fileOut,"        v_phot  :  %.1f         # velocity at photosphere (kkm/s)\n",dPhotosphere_Velocity * 1.0e-8);
		fprintf(fileOut,"        v_outer :  %.1f         # outer velocity of line forming region (kkm/s)\n",dVel_Max * 1.0e-8);
		fprintf(fileOut,"        t_phot  :  %.1f         # blackbody photosphere temperature (kK)\n", dTemp / 1000.0);

//		if (uiNumIons > 6)
//			uiNumIons = 6;
		printf("Writing ion parameters\n");
//		printf("Writing ion data : ION\n");
		WriteIonParameter(cSynIonDataLL,ION,fileOut,dVel_Max);
//		printf("Writing ion data : ACTIVE\n");
		WriteIonParameter(cSynIonDataLL,ACTIVE,fileOut,dVel_Max);
//		printf("Writing ion data : LOGTAU\n");
		WriteIonParameter(cSynIonDataLL,LOGTAU,fileOut,dVel_Max);
//		printf("Writing ion data : VMIN\n");
		WriteIonParameter(cSynIonDataLL,VMIN,fileOut,dVel_Max);
//		printf("Writing ion data : VMAX\n");
		WriteIonParameter(cSynIonDataLL,VMAX,fileOut,dVel_Max);
//		printf("Writing ion data : AUX\n");
		WriteIonParameter(cSynIonDataLL,AUX,fileOut,dVel_Max);
//		printf("Writing ion data : TEMP\n");
		WriteIonParameter(cSynIonDataLL,TEMP,fileOut,dVel_Max);
//		printf("Done writing ion data\n");

		fprintf(fileOut,"\n");
		fclose(fileOut);
	}
}
void Output_YAML_Data_To_CSV(const char * lpszFilename, const SYNIONDATA_LL	& cSynIonDataLL)
{
	
	double	dVel_Max = 0.0;
	FILE * fileOut = fopen(lpszFilename,"wt");
	if (fileOut != NULL)
	{
		SYNIONDATA_NODE * lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmax > dVel_Max)
				dVel_Max = lpCurr->cData.dVmax;
			lpCurr = lpCurr->lpNext;
		}
		if (dVel_Max < 3.0e10)
			dVel_Max = 3.0e10;
		lpCurr = cSynIonDataLL.lpHead;
		//printf("Writing CSV file\n");
		fprintf(fileOut,"ion");
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				fprintf(fileOut,",   %4i",lpCurr->cData.uiID);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\nenable");

		lpCurr = cSynIonDataLL.lpHead;

		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				if (lpCurr->cData.bEnable)
				{
					fprintf(fileOut,",    Yes");
				}
				else
				{
					fprintf(fileOut,",    No ");
				}
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\nlogtau");
		lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				fprintf(fileOut,",  %5.2f",lpCurr->cData.dLog_Tau);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\nvmin");
		lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				fprintf(fileOut,", %6.3f",lpCurr->cData.dVmin * 1.0e-8 + 0.001);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\nvmax");
		lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				double dVmaxlcl = lpCurr->cData.dVmax;
				if (dVmaxlcl > dVel_Max)
					dVmaxlcl = dVel_Max;
				fprintf(fileOut,", %6.3f",dVmaxlcl * 1.0e-8);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\naux");
		lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
		        double dEfolding = lpCurr->cData.dE_Folding * 1.0e-8;
		        if (dEfolding > 0.0 && dEfolding < 0.001)
		            dEfolding = 0.001;
		        else if (dEfolding < 0.0 && dEfolding > -0.001)
		            dEfolding = -0.001;
				fprintf(fileOut,", %6.3f",dEfolding);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\ntemp");
		lpCurr = cSynIonDataLL.lpHead;
		while (lpCurr)
		{
			if (lpCurr->cData.dVmin > 0.0 && lpCurr->cData.dVmin < dVel_Max && lpCurr->cData.dVmax > 0.0)// && lpCurr->cData.dLog_Tau > dTau_Threshold)
			{
				fprintf(fileOut,",  %4.1f",lpCurr->cData.dTemp * 1.0e-3);
			}
			lpCurr = lpCurr->lpNext;
		}
		fprintf(fileOut,"\n");

		fclose(fileOut);
	}
}


double GetAbundance(unsigned int i_uiRow, unsigned int i_uiElement_Idx, const XDATASET &cData)
{
	// FLASH seems to lower limit abundances to 10^-10; this forces a small abundance to be 0
	double	dAbd = cData.GetElement(i_uiElement_Idx,i_uiRow);
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
double * g_dIon_Density = NULL;
unsigned int g_uiNum_Tau_Points = 0;
void FillIonData(SYNIONDATA_LL &cIon_Data_LL, const XASTROLINE_ELEMENTDATA &cElement, const XASTROLINE_LINEDATA &cIonRefLine, unsigned int uiIon_State, const double &dTime, const double &dTimeRef, const double &dTemp_Ref, const double & dTemp_Alpha, const XDATASET &cData, unsigned int uiVel_Idx, unsigned int uiDens_Idx, unsigned int uiAbd_Idx, unsigned int uiRef_Vel_Idx, unsigned int uiPS_Vel_Idx, const double * lpdElectron_Density, bool bEnable)
{
    double  dTau_Threshold = pow(10.0,g_dLog_Tau_Threshold);
	unsigned int uiI,uiJ,uiIref;
	unsigned int uiNum_Points = cData.GetNumElements();
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

		if (g_dIon_Density)
			delete [] g_dIon_Density;
		g_dIon_Density = new double[uiNum_Tau_Points];
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


	double dRps = cData.GetElement(0,uiPS_Vel_Idx) + cData.GetElement(uiVel_Idx,uiPS_Vel_Idx) * (dTime - dTimeRef);
	//printf("Doing tau points %.1f %.1f %.1f %.1f\n",dTemp,dTime,dTimeRef,dIon_Temp);
	double dStoreA[5] = {0.0,0.0,0.0,0.0,0.0}, dStoreN = 0.0, dNionMax = 0.0;;
	for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
	{
		double dDens = cData.GetElement(uiDens_Idx,uiI + uiPS_Vel_Idx);
		if (dDens < 2.0e-10)
			dDens *= 1.0e-14;
		double dAbd = cData.GetElement(uiAbd_Idx,uiI + uiPS_Vel_Idx) * dN_Factor;
//		double dState_Abundance = 1.0;
		double dTempLcl = dTemp_Ref;
		if (dTemp_Alpha != 0.0)
		{
			double dRlcl = cData.GetElement(0,uiI + uiPS_Vel_Idx) + cData.GetElement(uiVel_Idx,uiI + uiPS_Vel_Idx) * (dTime - dTimeRef);
			dTempLcl *= pow(dRlcl / dRps,dTemp_Alpha);
		}
//		printf("%.2e,%.2e\t%.2e\t%.2f\t%.0f\t%.0f\n",dTemp_Alpha,dRlcl / dRps,pow(dRlcl / dRps,dTemp_Alpha),cData.GetElement(uiVel_Idx,uiI + uiPS_Vel_Idx),dTemp_Ref,dTempLcl);
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

			if (cElement.uiZ == 20 && uiI == 0)
			{
				printf("Ej = %.2e %.2e %.2e %.2e %.2e | kT = %.2e | gj = %.2e %.2e %.2e %.2e %.2e\n",
					lpcIon_Energy_Data->m_lpIonization_energies_erg[0] * g_XASTRO.k_deV_erg,
					lpcIon_Energy_Data->m_lpIonization_energies_erg[1] * g_XASTRO.k_deV_erg,
					lpcIon_Energy_Data->m_lpIonization_energies_erg[2] * g_XASTRO.k_deV_erg,
					lpcIon_Energy_Data->m_lpIonization_energies_erg[3] * g_XASTRO.k_deV_erg,
					lpcIon_Energy_Data->m_lpIonization_energies_erg[4] * g_XASTRO.k_deV_erg,
					dkT * g_XASTRO.k_deV_erg,
					lpPartition_Fn_Data->Get_Partition_Function(0,dTempLcl),
					lpPartition_Fn_Data->Get_Partition_Function(1,dTempLcl),
					lpPartition_Fn_Data->Get_Partition_Function(2,dTempLcl),
					lpPartition_Fn_Data->Get_Partition_Function(3,dTempLcl),
					lpPartition_Fn_Data->Get_Partition_Function(4,dTempLcl)
					);
			}

			for (uiJ = 0; uiJ <= 4; uiJ++) // assume triply ionized
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
				if (uiJ < (cElement.uiZ - 1))
					dA[uiJ] = dLambda_Fact * exp(-lpcIon_Energy_Data->m_lpIonization_energies_erg[uiJ] / dkT) * dG1 / dG2;
				else
					dA[uiJ] = 0.0;
//				printf("%.1e\t%.1e\n",lpcIon_Energy_DatcFlash_File.m_lpIonization_energies_erg[uiJ],g_XASTRO.k_deV_erg);
//				if (uiJ < lpcIon_Energy_DatcFlash_File.m_uiIonization_Energies_Count)
//					dIon_Temp += lpcIon_Energy_DatcFlash_File.m_lpIonization_energies_erg[uiJ];
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
		for (uiJ = 0; uiJ < uiIon_State - 1; uiJ++)
		{
			dNion *= dA[uiJ];
		}
		g_dIon_Density[uiI] = dNion;
		double	dExcess = dNion / (dAbd * dDens) - 1.0;
        if (dExcess > 1.0e-10)
            printf("Fault: n[X] > n[tot]: (%.2e > %.2e) %.2e %.2e %.2e %.2e %.2e %.2e\n",dNion,dAbd*dDens,dN1,dA[0],dA[1],dA[2],dA[3],dA[4]);
		if (cElement.uiZ == 20 && dNion > dNionMax)
		{
			dNionMax = dNion;
			dStoreA[0] = dA[0];
			dStoreA[1] = dA[1];
			dStoreA[2] = dA[2];
			dStoreA[3] = dA[3];
			dStoreA[4] = dA[4];
			dStoreN = dAbd * dDens;
		}
		
		g_lpdTau[uiI] = Optical_Depth(dNion,cIonRefLine,dTempLcl,dTime,dTimeRef);
        if (g_lpdTau[uiI] > 1.0e300)
            printf("Tau[i] fault\n");
		if (g_lpdTau[uiI] > dTau_Max)
			dTau_Max = g_lpdTau[uiI];
	}
//	double	dLine_Max = 0.0;
	if (cElement.uiZ == 20)
	{
		printf("a: %.2e %.2e %.2e %.2e %.2e | %i %.2e %.2e\n",dStoreA[1],dStoreA[2],dStoreA[3],dStoreA[4],dStoreA[5],uiIon_State,dStoreN,dNionMax);
	}
	printf("Max tau = %.2e\n",dTau_Max);

	uiIref = 0;
	cIon_Data.Clean(); // clear initial one
	double	dRef_Velocity = cData.GetElement(uiVel_Idx,uiRef_Vel_Idx);
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
			cIon_Data.uiID = cElement.uiZ * 100 + uiIon_State - 1;
			cIon_Data.bEnable = bEnable;
			dV[0] = cData.GetElement(uiVel_Idx,uiI + uiPS_Vel_Idx);
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
					dV[3] = cData.GetElement(uiVel_Idx,uiJ + uiPS_Vel_Idx); // maximum velocity
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
					if (g_lpdTau[uiIdx[0]] < dTau_Avg) // if the first point already is above the average, (usually this happens at the photosphere surface), skip the first points
					{
						bKeep_Going = true;
						for (uiJ = uiI + 1; uiJ < uiImax && bKeep_Going; uiJ++)
						{
							bKeep_Going = false;
							for (uiK = uiJ; uiK < uiNum_Tau_Points && uiK < (uiJ + 10) && !bKeep_Going; uiK++)
								bKeep_Going |= (g_lpdTau[uiK] < dTau_Avg);
						}
						uiIdx[1] = uiJ - 1;
						dV[1] = cData.GetElement(uiVel_Idx,uiJ + uiPS_Vel_Idx - 1); // velocity of
					}
					else
					{
						uiIdx[1] = uiIdx[0];
						dV[1] = dV[0];
					}
					bKeep_Going = true;
					for (; uiJ < uiImax && bKeep_Going; uiJ++)
					{
						bKeep_Going = false;
						for (uiK = uiJ; uiK < uiNum_Tau_Points && uiK < (uiJ + 10) && !bKeep_Going; uiK++)
							bKeep_Going |= (g_lpdTau[uiK] > dTau_Avg);
					}
					uiIdx[2] = uiJ - 1;
					dV[2] = cData.GetElement(uiVel_Idx,uiJ + uiPS_Vel_Idx - 1);

					// rising edge
					if (dV[1] > dV[0] + 0.01 && (g_lpdTau[uiIdx[0]] < dTau_Avg)) // skip the rising edge if it is really thin or if we are already above the average
					{
						dRlcl = cData.GetElement(0,(uiIdx[0] + uiIdx[1]) / 2 + uiPS_Vel_Idx) + cData.GetElement(uiVel_Idx,(uiIdx[0] + uiIdx[1]) / 2 + uiPS_Vel_Idx) * (dTime - dTimeRef);
						dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
						dAux[0] = (dV[0] - dV[1]) / log(g_lpdTau[uiIdx[1]]/g_lpdTau[uiIdx[0]]);
						dLog_Tau[0] = log10(exp((dV[1] - dRef_Velocity) / dAux[0])) + dLog_Tau_Avg;

						if (dLog_Tau[0] > -DBL_MAX && dLog_Tau[0] < DBL_MAX) // it is possible to generate large numbers.  If the tau value is too large, leave it out
						{
							cIon_Data.dVmax = dV[1];
							cIon_Data.dVmin = dV[0];
							cIon_Data.dE_Folding = dAux[0];
							cIon_Data.dLog_Tau = dLog_Tau[0];
							cIon_Data.dTemp = dTempLcl;
							cIon_Data_LL.Add(cIon_Data);
						}
					}
	
		

					//center
					unsigned int uiDelta_Idx = uiIdx[2] - uiIdx[1];
					if (uiDelta_Idx > 50)
					{
						unsigned int uiTest_Idx[2];
						double dLog_Tau_Test_1 = 0.0,dLog_Tau_Test_2 = 0.0;

						dRlcl = cData.GetElement(0,(uiIdx[1] + uiIdx[2]) / 2 + uiPS_Vel_Idx) + cData.GetElement(uiVel_Idx,(uiIdx[1] + uiIdx[2]) / 2 + uiPS_Vel_Idx) * (dTime - dTimeRef);
						dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
						uiTest_Idx[0] = uiDelta_Idx / 10 + uiIdx[1];
						uiTest_Idx[1] = (uiDelta_Idx / 10) * 9 + uiIdx[1];
						for (uiJ = uiIdx[1]; uiJ < uiTest_Idx[0]; uiJ++)
							dLog_Tau_Test_1 += log10(g_lpdTau[uiJ]);
						dLog_Tau_Test_1 /= (uiTest_Idx[0] - uiIdx[1]);
						for (uiJ = uiTest_Idx[1]; uiJ < uiIdx[2]; uiJ++)
							dLog_Tau_Test_2 += log10(g_lpdTau[uiJ]);
						dLog_Tau_Test_2 /= (uiIdx[2] - uiTest_Idx[1]);

						dAux[1] = (cData.GetElement(1,uiTest_Idx[1] + uiPS_Vel_Idx) - cData.GetElement(uiVel_Idx,uiTest_Idx[0] + uiPS_Vel_Idx)) / log(pow(10.0,-dLog_Tau_Test_2 + dLog_Tau_Test_1));
						dLog_Tau[1] = log10(exp((cData.GetElement(uiVel_Idx,uiIdx[1] + uiPS_Vel_Idx) - dRef_Velocity) / dAux[1])) + dLog_Tau_Test_1;

						if (dLog_Tau[1] > -DBL_MAX && dLog_Tau[1] < DBL_MAX) // it is possible to generate large numbers.  If the tau value is too large, leave it out
						{
							cIon_Data.dVmax = dV[2];
							cIon_Data.dVmin = dV[1];
							cIon_Data.dE_Folding = dAux[1];
							cIon_Data.dLog_Tau = dLog_Tau[1];
							cIon_Data.dTemp = dTempLcl;
							cIon_Data_LL.Add(cIon_Data);
						}
					}

					// falling edge
					dRlcl = cData.GetElement(0,(uiIdx[2] + uiIdx[3]) / 2 + uiPS_Vel_Idx) + cData.GetElement(uiVel_Idx,(uiIdx[2] + uiIdx[3]) / 2 + uiPS_Vel_Idx) * (dTime - dTimeRef);
					dTempLcl = pow(dRlcl / dRps,dTemp_Alpha) * dTemp_Ref;
					dAux[2] = (dV[2] - dV[3]) / log(g_lpdTau[uiIdx[3]]/g_lpdTau[uiIdx[2]]);
					dLog_Tau[2] = log10(exp((dV[2] - dRef_Velocity) / dAux[2])) + dLog_Tau_Avg;

					if (dLog_Tau[2] > -DBL_MAX && dLog_Tau[2] < DBL_MAX) // it is possible to generate large numbers.  If the tau value is too large, leave it out
					{
						cIon_Data.dVmax = dV[3];
						cIon_Data.dVmin = dV[2];
						cIon_Data.dE_Folding = dAux[2];
						cIon_Data.dLog_Tau = dLog_Tau[2];
						cIon_Data.dTemp = dTempLcl;
						cIon_Data_LL.Add(cIon_Data);
					}

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
//        if (cElement.uiZ == 8)
//        {
//            if (cIon_Data_LL.lpHead != 0)
//                printf("Added O data.\n");
//        }
}
class FIDE_CMN_PARAMETERS
{
public:
	double dTime;
	double dTimeRef;
	double dTemp_Ref;
	double dTemp_Alpha;
	const XDATASET * lpcData;
	const XDATASET * lpcRef_Line_Data;
	unsigned int uiDens_Idx;
	unsigned int uiVel_Idx;
	unsigned int uiRef_Vel_Idx;
	unsigned int uiPS_Vel_Idx;
	double	dPS_Velocity;
	double	dRef_Velocity;
	const double * lpdElectron_Density;
	unsigned int uiDay;
	unsigned int uiAlpha;
	bool bOutput_Intermediate;
	bool bForced_PS_Velocity;
};

void FillIonDataElement(const FIDE_CMN_PARAMETERS &cCmn, SYNIONDATA_LL &cSynIonDataLL,	const XASTROLINE_ELEMENTDATA &cElement, unsigned int uiAbd_Idx, const double & dTemp_Offset, const char * lpszElement, bool bEnable)
{
	SYNIONDATA_LL cTemp;
	unsigned int uiN;
	unsigned int uiNum_Points_Vel = cCmn.lpcData[0].GetNumElements();
	unsigned int uiNum_Tau_Points = uiNum_Points_Vel - cCmn.uiPS_Vel_Idx;// + 1;
	double dVelocityPrev;
	double dVelocity;
	double dVelocityNext;
	char lpszFilename[64];
	char lpszIon[16];
	unsigned int uiI;
	FILE * fileOut;
	double	dElement_Mass;
	double	dIon_Mass;
	double	dThreshold_Ion_Mass;
	double	dDetection_Threshold = pow(10.0,g_dLog_Tau_Threshold);
	unsigned int uiLine_Data_Ref_Idx;
	for (uiN = 0; uiN < cElement.uiZ; uiN++)
	{
		if (cElement.lpcIonData[uiN].uiNum_Lines > 0)// && (cElement.uiZ != 20 || uiN != 2))
		{
			// look up reference line for this ion
			uiLine_Data_Ref_Idx = cElement.lpcIonData[uiN].uiNum_Lines;
			if (cCmn.lpcRef_Line_Data != NULL)
			{
				unsigned int uiRef_Line_Idx = 0;
				unsigned int uiElement_ID = cElement.uiZ * 100 + uiN;
				unsigned int uiMax_Elements = cCmn.lpcRef_Line_Data->GetNumElements();
				double			dRef_Wavelength;
				double			dRef_Strength;
				while (uiRef_Line_Idx < uiMax_Elements && cCmn.lpcRef_Line_Data->GetElement(0,uiRef_Line_Idx) != uiElement_ID)
					uiRef_Line_Idx++;
				if (uiRef_Line_Idx < uiMax_Elements)
				{
					dRef_Wavelength = cCmn.lpcRef_Line_Data->GetElement(1,uiRef_Line_Idx);
					dRef_Strength = cCmn.lpcRef_Line_Data->GetElement(2,uiRef_Line_Idx);
					bool bDone = false;
					printf("Ref data: %.2f %.2f\n",dRef_Wavelength,dRef_Strength);
					double dThreshold = 0.1;
					while (!bDone && dThreshold < 6.5)
					{
						if (!bDone)
						{
//							printf("Searching with threshold = %.1f\n",dThreshold);
							uiLine_Data_Ref_Idx = 0;
						}
						while (uiLine_Data_Ref_Idx < cElement.lpcIonData[uiN].uiNum_Lines && !bDone)
						{
							// first check and see if the wavelength is within 5 Angstrom
							bDone = fabs(cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dWavelength_A - dRef_Wavelength) < dThreshold; 
							//if (bDone) // ensure the oscillator strength is accurate to within 20%
							//{
							//	printf("Found %.2f %.2f\n",cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dWavelength_A,cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dOscillator_Strength);
							//	bDone = fabs(cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dOscillator_Strength/dRef_Strength - 1.0) < 0.2;
							//}
							if (!bDone)
								uiLine_Data_Ref_Idx++;
						}
						dThreshold *= 2.0;
					}
					if (bDone)
					{
						printf("Found %.2f %.2f\n",cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dWavelength_A,cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx].dOscillator_Strength);
					}
					
				} 
			}
			if (uiLine_Data_Ref_Idx < cElement.lpcIonData[uiN].uiNum_Lines)
			{
				printf("\tIon %i\n",uiN);
				FillIonData(cTemp, cElement, cElement.lpcIonData[uiN].lpcLine_Data[uiLine_Data_Ref_Idx],cElement.lpcIonData[uiN].uiIon_State, cCmn.dTime, cCmn.dTimeRef, cCmn.dTemp_Ref + dTemp_Offset, cCmn.dTemp_Alpha, cCmn.lpcData[0], cCmn.uiVel_Idx, cCmn.uiDens_Idx, uiAbd_Idx, cCmn.uiRef_Vel_Idx, cCmn.uiPS_Vel_Idx,cCmn.lpdElectron_Density,bEnable);
				printf("Ion %i complete\n",uiN);

				if (cCmn.bOutput_Intermediate)
				{
					xRomanNumeralGenerator(lpszIon,uiN + 1);
					if (cCmn.bForced_PS_Velocity)
						sprintf(lpszFilename,"./results/tau/tau_d%i_T%.0f_a%i_ps%3.0f_%s_%s.csv",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,cCmn.dPS_Velocity * 1.0e-7,lpszElement,lpszIon);
					else
						sprintf(lpszFilename,"./results/tau/tau_d%i_T%.0f_a%i_%s_%s.csv",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,lpszElement,lpszIon);
					fileOut = fopen(lpszFilename,"wt");
					fprintf(fileOut,"radius [cm], velocity [cm/s], element density [g/cm^3], ion density [g/cm^3], computed optical depth, computed synow optical depth, shell mass [g]\n");

					dVelocityPrev = cCmn.lpcData[0].GetElement(cCmn.uiVel_Idx,cCmn.uiPS_Vel_Idx - 1); 
					dVelocity = cCmn.lpcData[0].GetElement(cCmn.uiVel_Idx,cCmn.uiPS_Vel_Idx);
				
					dElement_Mass = 0.0;
					dIon_Mass = 0.0;
					dThreshold_Ion_Mass = 0.0;
					for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
					{
						dVelocityNext = cCmn.lpcData[0].GetElement(cCmn.uiVel_Idx,cCmn.uiPS_Vel_Idx + uiI + 1);
						if (dVelocity < cCmn.dPS_Velocity)
							uiI = uiNum_Tau_Points; // skip to end - we are not within the ejecta anymore
						else
						{
							double	dTauV = 0.0;
							double dTime_Rel = cCmn.dTimeRef / cCmn.dTime;
							double dDens_Mult = dTime_Rel * dTime_Rel * dTime_Rel;
							double dDens = cCmn.lpcData[0].GetElement(cCmn.uiDens_Idx,uiI + cCmn.uiPS_Vel_Idx) * cCmn.lpcData[0].GetElement(uiAbd_Idx,uiI + cCmn.uiPS_Vel_Idx);// * dDens_Mult;
							double	dIon_Dens = g_dIon_Density[uiI] * dDens_Mult;
							double dRt0 = cCmn.lpcData[0].GetElement(0,uiI + cCmn.uiPS_Vel_Idx);
							double dRnextt0 = cCmn.lpcData[0].GetElement(0,uiI + cCmn.uiPS_Vel_Idx + 1);
							double dShell_Volume = 4.0 / 3.0 * (dRnextt0 * dRnextt0 * dRnextt0 - dRt0 * dRt0 * dRt0) * g_cConstants.dPi;
							double	dShell_Element_Mass = dDens * dShell_Volume;
							double	dShell_Ion_Mass = g_dIon_Density[uiI] * dShell_Volume * cElement.uiA * g_XASTRO.k_dmh;
							dElement_Mass += dShell_Element_Mass;
							dIon_Mass += dShell_Ion_Mass;
							if (g_lpdTau[uiI] > dDetection_Threshold)
								dThreshold_Ion_Mass += dShell_Ion_Mass;
							SYNIONDATA_NODE * lpCurr = cTemp.lpHead;
							while (lpCurr)
							{
								if (lpCurr->cData.dVmin <= dVelocity && lpCurr->cData.dVmax >= dVelocity)
								{
									dTauV += pow(10.0,lpCurr->cData.dLog_Tau) * exp((cCmn.dRef_Velocity - dVelocity) / lpCurr->cData.dE_Folding);
								}
				

								lpCurr = lpCurr->lpNext;
							}
			
	//						double dAux_Lcl = (dVelocityNext - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI + 1]);
	//						if (uiI != 0)
	//						{
	//								dAux_Lcl += (dVelocityPrev - dVelocity)  / log(g_lpdTau[uiI] / g_lpdTau[uiI - 1]);
	//								dAux_Lcl *= 0.5;
	//						}
	//						double dTau_Ref_Lcl = g_lpdTau[uiI] * exp((dVelocity - dPS_Velocity) / dAux_Lcl);
							fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",cCmn.lpcData[0].GetElement(0,uiI + cCmn.uiPS_Vel_Idx),dVelocity,dDens,dIon_Dens,g_lpdTau[uiI],dTauV,dShell_Ion_Mass);
						}
						dVelocityPrev = dVelocity;
						dVelocity = dVelocityNext;

					}
					fclose(fileOut);
					sprintf(lpszFilename,"./results/tau/mass_d%i_T%.0f_a%i_ps%3.0f_%s_%s.csv",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,cCmn.dPS_Velocity * 1.0e-7,lpszElement,lpszIon);
					fileOut = fopen(lpszFilename,"wt");
					fprintf(fileOut,"Day, Ion, Element Mass, Ion Mass, Detectable Ion Mass\n");
					fprintf(fileOut,"%i, %s %s, %.17e, %.17e, %.17e\n", cCmn.uiDay, lpszElement, lpszIon, dElement_Mass, dIon_Mass, dThreshold_Ion_Mass);
					fclose(fileOut);

				}
				if ((cElement.uiZ == 8 && uiN >= 3) ||  // O II, O III is causing problems?
					(cElement.uiZ == 10 && uiN == 2) || // Ne II is not in syn++ database
					(cElement.uiZ == 16 && uiN == 3) || // S IV is not in syn++ database
					(cElement.uiZ == 18 && uiN == 2) || // Ar III is not in syn++ database
					(cElement.uiZ == 18 && uiN == 3) || // Ar IV is not in syn++ database
					(cElement.uiZ == 22 && uiN == 3)) // Ti IV is not in syn++ database
					cTemp.Clean();
				else
				{
					cSynIonDataLL += cTemp;
					cTemp.Void();
				}
			}
			else if (false)//uiN <= 2) // force output for doubly ionized, but set all tau to 0
			{
				xRomanNumeralGenerator(lpszIon,uiN + 1);
				if (cCmn.bForced_PS_Velocity)
					sprintf(lpszFilename,"./results/tau/tau_d%i_T%.0f_a%i_ps%3.0f_%s_%s.csv",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,cCmn.dPS_Velocity * 1.0e-7,lpszElement,lpszIon);
				else
					sprintf(lpszFilename,"./results/tau/tau_d%i_T%.0f_a%i_%s_%s.csv",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,lpszElement,lpszIon);
				fileOut = fopen(lpszFilename,"wt");
				fprintf(fileOut,"radius [cm], velocity [cm/s], density [g/cm^3], computed optical depth, computed synow optical depth\n");
				for (uiI = 0; uiI < uiNum_Tau_Points; uiI++)
				{
					dVelocity = cCmn.lpcData[0].GetElement(cCmn.uiVel_Idx,cCmn.uiPS_Vel_Idx + uiI);
					if (dVelocity < cCmn.dPS_Velocity)
						uiI = uiNum_Tau_Points; // skip to end - we are not within the ejecta anymore
					else				
						fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e\n",cCmn.lpcData[0].GetElement(0,uiI + cCmn.uiPS_Vel_Idx),dVelocity,0.0,0.0,0.0,0.0);
				}
				fclose(fileOut);
			}
			else
				printf("Couldn't find ref line\n");
		}
	}
}


class PE_PARAMETERS
{
public:
	double	dData_Spacing;
	bool	bOutput_Intermediate_YAML;
};

void Process_Element(const FIDE_CMN_PARAMETERS & cCmn, const PE_PARAMETERS & cPE_Cmn, SYNIONDATA_LL &cSynIonDataLL,	const XASTROLINE_ELEMENTDATA &cElement, const double & dTemp_Offset, unsigned int uiAbd_Idx, bool bEnable, const char * lpszElement)
{
	SYNIONDATA_LL	cSynIonDataLLTemp;
	char lpszFilename[256];

	printf("Fill %s data\n",lpszElement);
	FillIonDataElement(cCmn, cSynIonDataLLTemp, cElement, uiAbd_Idx, dTemp_Offset, lpszElement, bEnable);
	if (cPE_Cmn.bOutput_Intermediate_YAML)
	{
		if (cCmn.bForced_PS_Velocity)
			sprintf(lpszFilename,"./results/yaml/run_d%i_T%.0f_a%i_%s_ps%3.0f.yaml",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,lpszElement,cCmn.dPS_Velocity * 1.0e-7);
		else
			sprintf(lpszFilename,"./results/yaml/run_d%i_T%.0f_a%i_%s.yaml",cCmn.uiDay,cCmn.dTemp_Ref,cCmn.uiAlpha,lpszElement);

		Output_YAML_File(lpszFilename,cSynIonDataLLTemp,cPE_Cmn.dData_Spacing,cCmn.uiDay,cCmn.dRef_Velocity, cCmn.dPS_Velocity, cCmn.dTemp_Ref);
	}
	cSynIonDataLL += cSynIonDataLLTemp;
	if (cSynIonDataLL.lpHead == NULL && cSynIonDataLLTemp.lpHead != NULL)
		printf("Fault 1\n");
	if (cSynIonDataLL.lpTail != cSynIonDataLLTemp.lpTail && cSynIonDataLLTemp.lpTail != NULL)
		printf("Fault 2\n");
	cSynIonDataLLTemp.Void();
}

void Generate_Decay_Products(const double & dTime_Ref, const double & dTime, XDATASET & io_cData, unsigned int i_uiParent_Idx, unsigned int i_uiParentOut_Idx, unsigned int i_uiChild1_Idx, unsigned int i_uiChild2_Idx, const double i_dHalflife[2], const double i_dNuclear_Mass[3])
{
	double	dLog2 = log(2.0);
	double	dTau[2] = {i_dHalflife[0] * dLog2,i_dHalflife[1] * dLog2};
	double	dDeltaT = dTime - dTime_Ref;
	double	dX[2] = {dDeltaT / dTau[0],dDeltaT / dTau[1]};
	unsigned int uiI;

	unsigned int uiNum_Elem = io_cData.GetNumElements();

	for (uiI = 0; uiI < uiNum_Elem; uiI++)
	{
		//double	dR = io_cData.GetElement(0,uiI);
		double	dParent0 = GetAbundance(uiI,i_uiParent_Idx, io_cData) / i_dNuclear_Mass[0];
		double dParent_t = dParent0 * exp(-dX[0]);
		double dChild_1 = (exp(-dX[1] - dX[0]) * (-exp(dX[1]) + exp(dX[0])) * dParent0 * dTau[1])/(dTau[1] - dTau[0]);
		double dChild_2 = dParent0 - dParent_t - dChild_1;

		io_cData.SetElement(i_uiParentOut_Idx,uiI,dParent_t * i_dNuclear_Mass[0]);
		io_cData.SetElement(i_uiChild1_Idx,uiI,dChild_1 * i_dNuclear_Mass[1]);
		io_cData.SetElement(i_uiChild2_Idx,uiI,dChild_2 * i_dNuclear_Mass[2]);
	}
}

unsigned int  Find_Variable_Idx(const XFLASH_File & cFile, const char * i_lpszVar_Name)
{
	unsigned int uiVariable_Idx = 0;
	unsigned int uiLen = strlen(i_lpszVar_Name);

	while (uiVariable_Idx < cFile.m_uiNum_Vars && (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) != 0 || (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) == 0 && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != ' ' && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != 0)))
	{
//		printf("%s:%s\n",cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name);
		uiVariable_Idx++;
	}
	
	return uiVariable_Idx;
}


void Load_Line_Data(XASTROLINE_ELEMENTDATA & o_cLine_Data, const char * i_lpszElement_ID)
{
	unsigned int uiI;
	char lpszRoman_Numeral[16];
	o_cLine_Data.ReadDataFile("../Data",i_lpszElement_ID);
	for (uiI = 0; uiI < o_cLine_Data.uiZ; uiI++)
	{
		if (o_cLine_Data.lpcIonData[uiI].uiNum_Lines == 0)
		{
			xRomanNumeralGenerator(lpszRoman_Numeral,uiI + 1);
			printf("No line data for %s %s\n",i_lpszElement_ID,lpszRoman_Numeral);
		}
	}
}

int main(int iArgC,const char * lpszArgV[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFile_To_Read[256];
	char	lpszExpanded_File_To_Read[256];
	double	dTimeRef;// = 50.0;
	double	dTemp;// = 12000.0;
//	double	dCaAbdAdj = 0.0;
//	double	dSiAbdAdj = 0.0;
//	double	dFeAbdAdj = 0.0;
	unsigned int uiK;
	unsigned int uiDmin,uiDmax,uiDstep;
	unsigned int uiTmin,uiTmax,uiTstep;
	unsigned int uiAmin,uiAmax,uiAstep;
	bool bEnable_H, bEnable_He, bEnable_C, bEnable_N, bEnable_O, bEnable_Ne, bEnable_Mg, bEnable_Si, bEnable_S, bEnable_Ar, bEnable_Ca, bEnable_Ti, bEnable_V, bEnable_Cr, bEnable_Mn, bEnable_Fe, bEnable_Co, bEnable_Ni;
	double dH_Temp_Offset, dHe_Temp_Offset, dC_Temp_Offset, dN_Temp_Offset, dO_Temp_Offset, dNe_Temp_Offset, dMg_Temp_Offset, dSi_Temp_Offset, dS_Temp_Offset, dAr_Temp_Offset, dCa_Temp_Offset, dTi_Temp_Offset, dV_Temp_Offset, dCr_Temp_Offset, dMn_Temp_Offset, dFe_Temp_Offset, dCo_Temp_Offset, dNi_Temp_Offset;
	bool	bEnable_Abundance_Output = false;
	double	dForce_PS_Vel = -1.0;
	bool	bEnable_Tau_Output;
	bool	bEnable_Elemental_Spectra_Output;

	unsigned int uiShell_Lower_Idx = 0;
	double	dRef_Velocity;
	unsigned int uiVel_Ref_Idx = 0;

    double		dPhotosphere_Velocity[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    unsigned int uiPS_Velocity_Idx[6] = {0,0,0,0,0,0};
	unsigned int uiPS_Vel_Idx; // the one to actually use
	double dPS_Velocity;// the one to actually use
	char lpszFilename[64];

	FIDE_CMN_PARAMETERS	cCmn;
	PE_PARAMETERS		cPE_Cmn;
	XDATASET	cData;
	XDATASET	cRef_Lines;

	unsigned int uiFlash_Idx[40]; // more than needed
	unsigned int uiCoord_Idx, uiZone_Size_Idx;
	unsigned int uiVel_Idx, uiDens_Idx, uiHyd_Idx, uiHe3_Idx, uiHe4_Idx;
	unsigned int uiC12_Idx, uiN14_Idx, uiO16_Idx, uiNe20_Idx, uiMg24_Idx, uiSi28_Idx;
	unsigned int uiS32_Idx, uiAr36_Idx, uiCa40_Idx, uiCa44_Idx, uiTi44_Idx, uiTi48_Idx;
	unsigned int uiCr48_Idx, uiCr52_Idx, uiFe52_Idx, uiFe56_Idx, uiNi56_Idx;

	unsigned int uiHeComb_Idx, uiCaComb_Idx, uiTiComb_Idx, uiCrComb_Idx, uiFeComb_Idx;

	unsigned int uiCa44Decay_Idx;
	unsigned int uiSc44Decay_Idx;
	unsigned int uiTi44Decay_Idx;
	unsigned int uiTi48Decay_Idx;
	unsigned int uiV48Decay_Idx;
	unsigned int uiCr48Decay_Idx;
	unsigned int uiCr52Decay_Idx;
	unsigned int uiMn52Decay_Idx;
	unsigned int uiFe52Decay_Idx;
	unsigned int uiFe56Decay_Idx;
	unsigned int uiCo56Decay_Idx;
	unsigned int uiNi56Decay_Idx;
//	XDATASET 	cCaHK_Line_Results;
//	XDATASET 	cCaHK_Spectra_Results;
	printf("Starting.\n");
	XASTRO_ATOMIC_IONIZATION_DATA	* lpcIon_Energy_Data[] = {
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(1),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(2),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(2),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(6),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(7),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(8),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(10),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(12),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(14),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(16),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(18),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(20),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(20),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(21),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(22),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(22),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(23),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(24),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(24),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(25),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(26),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(26),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(27),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(28)
	};
	unsigned int uiI,uiJ,uiL;
	unsigned int uiNumAbdIon = sizeof(lpcIon_Energy_Data) / sizeof(XASTRO_ATOMIC_IONIZATION_DATA	*);
	for (uiI = 0; uiI < uiNumAbdIon; uiI++)
	{
		if (lpcIon_Energy_Data[uiI] == NULL)
			printf("Unable to find ion %i.\n",uiI);
	}
	printf("Ions confirmed.\n");


	double		dNi56_Halflifes[2] = {6.075 * 86400.0,77.233 * 86400.0};
	double		dNi56_Nuclear_Mass[3] = {55.942132, 55.9398393, 55.9349375};

	double		dFe52_Halflifes[2] = {8.275 * 3600.0, 5.591 * 86400.0};
	double		dFe52_Nuclear_Mass[3] = {51.948114, 51.9455655, 51.9405075};

	double		dCr48_Halflifes[2] = {21.56 * 3600.0, 15.9735 * 86400.0};
	double		dCr48_Nuclear_Mass[3] = {47.954032, 47.9522537, 47.9479463};

	double		dTi44_Halflifes[2] = {60.0 * 86400.0 * 365.2425, 3.97 * 3600.0};
	double		dTi44_Nuclear_Mass[3] = {43.9596901, 43.9594028, 43.9554818};

	// Abundance values from Seitenzahl et al 2013 MNRAS, N100 model
	// Note these abundances sum all isotopes of each element, which will be used below to only determine the 
	// alpha series isotopes.  This is wrong, but the alpha series tends to be the most dominant isotope of each element
	// given that there still exists > 10% error in determining the elemental abundances, I think this is a reasonable estimation
	double	dLog_Abd_Mg[2] = {-2.4471971698, -1.8135824334}; // Ne20 and Mg24, respectively
	double	dLog_Abd_Si[6] = {-0.5415283333, -0.9378736444, -1.7067798935, -1.8312788521,-4.4942397241,-2.8417249788}; //Si,S,Ar,Ca,Ti,Cr
	double	dLog_Abd_Ni[2] = {-0.8921870067, -0.1574165722}; // Fe52,Ni56

	double	dTemp_Sum;
	double	dAbd_Mg[2];
	double	dAbd_Si[6];
	double	dAbd_Ni[2];

	dTemp_Sum = pow(10.0,dLog_Abd_Mg[0]) + pow(10.0,dLog_Abd_Mg[1]);
	dAbd_Mg[0] = pow(10.0,dLog_Abd_Mg[0]) / dTemp_Sum;
	dAbd_Mg[1] = pow(10.0,dLog_Abd_Mg[1]) / dTemp_Sum;

	unsigned int uiEntries = sizeof(dLog_Abd_Si) / sizeof(double);
	dTemp_Sum = 0.0;
	for (uiI = 0; uiI < uiEntries; uiI++)
		dTemp_Sum += pow(10.0,dLog_Abd_Si[uiI]);
	for (uiI = 0; uiI < uiEntries; uiI++)
		dAbd_Si[uiI] = pow(10.0,dLog_Abd_Si[uiI]) / dTemp_Sum;

	dTemp_Sum = pow(10.0,dLog_Abd_Ni[0]) + pow(10.0,dLog_Abd_Ni[1]);
	dAbd_Ni[0] = pow(10.0,dLog_Abd_Ni[0]) / dTemp_Sum;
	dAbd_Ni[1] = pow(10.0,dLog_Abd_Ni[1]) / dTemp_Sum;


	double	* lpdElectron_Density[6];
	double * lpdElectron_Density_Temp;
	double * lpdVolume_tref;

	SYNIONDATA_LL	cSynIonDataLL;
	unsigned int uiPS_Velocity_Idx_Forced = 0;
	unsigned int uiNum_Points;
	double	dElectron_Scaling = 1.0;
	double	dData_Spacing;
	wordexp_t  cResults;


	xParse_Command_Line_String(iArgC,lpszArgV,"--ref-line-file",lpszFile_To_Read,256,"");
	if (lpszFile_To_Read[0] != 0)
	{
		if (wordexp(lpszFile_To_Read,&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
			strcpy(lpszExpanded_File_To_Read,cResults.we_wordv[0]);
		else
			lpszExpanded_File_To_Read[0] = 0;
		wordfree(&cResults);
	}
	else
		strcpy(lpszExpanded_File_To_Read,getenv("SYNXX_REF_LINE_DATA_PATH"));
	printf("Using reference file %s\n",lpszExpanded_File_To_Read);
	cRef_Lines.ReadDataFile(lpszExpanded_File_To_Read,true,0,'\"',0);
	//@@DEBUG printf("%.1f %.2f\n",cRef_Lines.GetElement(0,0),cRef_Lines.GetElement(1,0));
	if (cRef_Lines.GetNumElements() == 0)
	{
		printf("Unable to find reference line file %s.  Exiting.\n",lpszExpanded_File_To_Read);
		exit(1);
	}

	xParse_Command_Line_String(iArgC,lpszArgV,"-file",lpszFile_To_Read,256,"");
	if (wordexp(lpszFile_To_Read,&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
		strcpy(lpszExpanded_File_To_Read,cResults.we_wordv[0]);
	else
		lpszExpanded_File_To_Read[0] = 0;
	wordfree(&cResults);
	printf("Filename is %s\n",lpszExpanded_File_To_Read);
//	printf("%x %x %x %x %x",lpszFile_To_Read[18],lpszFile_To_Read[19],lpszFile_To_Read[20],lpszFile_To_Read[21],lpszFile_To_Read[22]);
//	printf("%x %x %x %x %x",lpszArgV[1][18],lpszArgV[1][19],lpszArgV[1][20],lpszArgV[1][21],lpszArgV[1][22]);
	if (lpszExpanded_File_To_Read[0] == 0)
	{
		printf("No FLASH file specified.  Exiting...\n");
		exit(0);
	}
	uiDmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_start",0);
	uiDmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_end",25);
	uiDstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_step",1);
	uiTmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_start",7000);
	uiTmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_end",15000);
	uiTstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_step",500);
	uiAmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_start",0);
	uiAmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_end",10);
	uiAstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_step",1);

	dData_Spacing = xParse_Command_Line_Dbl(iArgC,lpszArgV,"-wavelength_spacing",5.0);

	bEnable_H  = xParse_Command_Line_Bool(iArgC,lpszArgV,"--H",true);
	bEnable_He = xParse_Command_Line_Bool(iArgC,lpszArgV,"--He",true);
	bEnable_C  = xParse_Command_Line_Bool(iArgC,lpszArgV,"--C",true);
	bEnable_N  = xParse_Command_Line_Bool(iArgC,lpszArgV,"--N",true);
	bEnable_O  = xParse_Command_Line_Bool(iArgC,lpszArgV,"--O",true);
	bEnable_Ne = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Ne",true);
	bEnable_Mg = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Mg",true);
	bEnable_Si = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Si",true);
	bEnable_S  = xParse_Command_Line_Bool(iArgC,lpszArgV,"--S",true);
	bEnable_Ar = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Ar",true);
	bEnable_Ca = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Ca",true);
	bEnable_Ti = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Ti",true);
	bEnable_V = xParse_Command_Line_Bool(iArgC,lpszArgV,"--V",true);
	bEnable_Cr = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Cr",true);
	bEnable_Mn = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Mn",true);
	bEnable_Fe = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Fe",true); // Iron is slow - disable by default
	bEnable_Co = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Co",true);
	bEnable_Ni = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Ni",true);

	bEnable_Abundance_Output = xParse_Command_Line_Bool(iArgC,lpszArgV,"--output_abd",false);

	bEnable_Tau_Output = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Tau_Output",false);
	bEnable_Elemental_Spectra_Output = xParse_Command_Line_Bool(iArgC,lpszArgV,"--Elem_Spec_Output",true);

	dForce_PS_Vel = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--forced_ps",-1.0);

	dH_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_H",0.0);
	dHe_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_He",0.0);
	dC_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_C",0.0);
	dN_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_N",0.0);
	dO_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_O",0.0);
	dNe_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Ne",0.0);
	dMg_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Mg",0.0);
	dSi_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Si",0.0);
	dS_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_S",0.0);
	dAr_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Ar",0.0);
	dCa_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Ca",0.0);
	dTi_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Ti",0.0);
	dV_Temp_Offset  = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_V",0.0);
	dCr_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Cr",0.0);
	dMn_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Mn",0.0);
	dFe_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Fe",0.0);
	dCo_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Co",0.0);
	dNi_Temp_Offset = xParse_Command_Line_Dbl(iArgC,lpszArgV,"--offset_temp_Ni",0.0);

	//printf("Si: %.0f\t S: %.0f\n",dSi_Temp_Offset,dS_Temp_Offset);
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

	Load_Line_Data(cH_Lines,"H");
	Load_Line_Data(cHe_Lines,"He");
	Load_Line_Data(cC_Lines,"C");
	Load_Line_Data(cN_Lines,"N");
	Load_Line_Data(cO_Lines,"O");
	Load_Line_Data(cNe_Lines,"Ne");
	Load_Line_Data(cMg_Lines,"Mg");
	Load_Line_Data(cSi_Lines,"Si");
	Load_Line_Data(cS_Lines,"S");
	Load_Line_Data(cAr_Lines,"Ar");
	Load_Line_Data(cCa_Lines,"Ca");
	Load_Line_Data(cTi_Lines,"Ti");
	Load_Line_Data(cV_Lines,"V");
	Load_Line_Data(cCr_Lines,"Cr");
	Load_Line_Data(cMn_Lines,"Mn");
	Load_Line_Data(cFe_Lines,"Fe");
	Load_Line_Data(cCo_Lines,"Co");
	Load_Line_Data(cNi_Lines,"Ni");

//	FILE * fileCombinedData = fopen("out.csv");
//	if (fileCombinedData) // there is a single combined data file, use it
//	{
//		XDATASET	cCombinedData;
//		cCombinedData.ReadDataFile("out.csv",false,false,',',1);
//		fgets(lpszFieldNames,
//	}
	XFLASH_File	cFlash_File;

	cFlash_File.Open(lpszExpanded_File_To_Read);
	dTimeRef = cFlash_File.m_dTime; //xParse_Command_Line_Dbl(iArgC,lpszArgV,"-time_ref",50.0);
	printf("Opened file %s.\n",lpszExpanded_File_To_Read);
	printf("Time ref set to %.1f s\n",dTimeRef);

	unsigned int uiLeaf_Blocks = 0;
//	for (uiI = 0; uiI < cFlash_File.m_uiNum_Vars; uiI++)
//		printf("%s\n",cFlash_File.m_lpszVar_Names[uiI]);

	for (uiI = 0; uiI < cFlash_File.m_uiNum_Blocks; uiI++)
	{
		if (cFlash_File.m_lpeBlock_Node_Type[uiI] == FR_LEAF_NODE)
			uiLeaf_Blocks++;
	}

	uiI = 0; uiCoord_Idx = uiI;
	uiI++; uiZone_Size_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"velx"); uiI++; uiVel_Idx = uiI;
	if (uiFlash_Idx[uiI - 2] >= cFlash_File.m_uiNum_Vars)
	{
		printf("Velocity missing. Exiting.\n");
		exit(1);
	}
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"dens"); uiI++; uiDens_Idx = uiI;
	if (uiFlash_Idx[uiI - 2] >= cFlash_File.m_uiNum_Vars)
	{
		printf("(%i %i %i) Density missing. Exiting.\n",uiI,uiFlash_Idx[uiI - 1],cFlash_File.m_uiNum_Vars);
		exit(1);
	}
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"hyd"); uiI++; uiHyd_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"he3"); uiI++; uiHe3_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"he4"); uiI++; uiHe4_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"c12"); uiI++; uiC12_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"n14"); uiI++; uiN14_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"o16"); uiI++; uiO16_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ne20"); uiI++; uiNe20_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"mg24"); uiI++; uiMg24_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"si28"); uiI++; uiSi28_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"s32"); uiI++; uiS32_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ar36"); uiI++; uiAr36_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ca40"); uiI++; uiCa40_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ca44"); uiI++; uiCa44_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ti44"); uiI++; uiTi44_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ti48"); uiI++; uiTi48_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"cr48"); uiI++; uiCr48_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"cr52"); uiI++; uiCr52_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"fe52"); uiI++; uiFe52_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"fe56"); uiI++; uiFe56_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ni56"); uiI++; uiNi56_Idx = uiI;
	unsigned int uiFlash_Vars = uiI - 1;


	uiI++; uiHeComb_Idx = uiI; 
	uiI++; uiCaComb_Idx = uiI;
	uiI++; uiTiComb_Idx = uiI;
	uiI++; uiCrComb_Idx = uiI;
	uiI++; uiFeComb_Idx = uiI;

	uiI++; uiNi56Decay_Idx = uiI;
	uiI++; uiCo56Decay_Idx = uiI;
	uiI++; uiFe56Decay_Idx = uiI;
	uiI++; uiFe52Decay_Idx = uiI;
	uiI++; uiMn52Decay_Idx = uiI;
	uiI++; uiCr52Decay_Idx = uiI;
	uiI++; uiCr48Decay_Idx = uiI;
	uiI++; uiV48Decay_Idx  = uiI;
	uiI++; uiTi48Decay_Idx = uiI;
	uiI++; uiTi44Decay_Idx = uiI;
	uiI++; uiSc44Decay_Idx = uiI;
	uiI++; uiCa44Decay_Idx = uiI;

	uiL = 0;
	cData.Allocate(uiI + 1,uiLeaf_Blocks * cFlash_File.m_uiBlock_Dimensions[0]);
	cData.Zero();

//	double dVelocity_Scale = 1.0e-9; // convert to 10,000 km/s

	for (uiI = 0; uiI < cFlash_File.m_uiNum_Blocks; uiI++)
	{
		if (cFlash_File.m_lpeBlock_Node_Type[uiI] == FR_LEAF_NODE)
		{
			int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiI;
			double ddx = 0.0,ddy = 0.0,ddz = 0.0;
			// compute the cell 
			ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
			if (cFlash_File.m_uiNum_Dimensions >= 2)
				ddy = cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 3] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 2];
			if (cFlash_File.m_uiNum_Dimensions >= 3)
				ddz = cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 5] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 4];
			ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];
			if (cFlash_File.m_uiNum_Dimensions >= 2)
				ddy /= (double)cFlash_File.m_uiBlock_Dimensions[1];
			if (cFlash_File.m_uiNum_Dimensions >= 3)
				ddz /= (double)cFlash_File.m_uiBlock_Dimensions[2];
			// Set coordinate
			for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
			{
				cData.SetElement(0,uiL + uiK,ddx * (uiK + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
				cData.SetElement(1,uiL + uiK,ddx);
			}
			// Fill variable data
			for (uiJ = 0; uiJ < uiFlash_Vars; uiJ++)
			{
				if (uiFlash_Idx[uiJ] < cFlash_File.m_uiNum_Vars)
				{
					XFLASH_Block * lpcBlock = cFlash_File.GetBlock(uiFlash_Idx[uiJ],uiI);
					
//					if (uiJ != 0) // velocity index
//					{
						for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
							cData.SetElement(uiJ + 2,uiL + uiK,lpcBlock->m_lpdData[uiK]);
//					}
//					else
//					{
//						for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
//							cData.SetElement(uiJ + 1,uiL + uiK,lpcBlock->m_lpdData[uiK] * dVelocity_Scale);
//					}
					cFlash_File.ReleaseBlock(lpcBlock);
				}
			}
			uiL += cFlash_File.m_uiBlock_Dimensions[0];
		}
	}
	cFlash_File.Close();
	uiNum_Points = uiL;
//	printf("I ran, %i elements\n",cData.GetNumElements());
//	cData.SaveDataFileCSV("out.csv",(const char **)cFlash_File.m_lpszVar_Names);
//	cData.SaveDataFileCSV("out.csv",NULL);
//	return 1;
	

	// Compute combined He (He-3 + He-4)
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		cData.SetElement(uiHeComb_Idx,uiI,cData.GetElement(uiHe3_Idx,uiI) + cData.GetElement(uiHe4_Idx,uiI));


		// "Fix" the group abundances
		if (GetAbundance(uiI,uiHyd_Idx,cData) == 0.0) // in the supernova
		{
			double dMgg = GetAbundance(uiI,uiMg24_Idx, cData) + GetAbundance(uiI,uiNe20_Idx,cData);
			cData.SetElement(uiMg24_Idx,uiI,dMgg * dAbd_Mg[0]);
			cData.SetElement(uiNe20_Idx,uiI,dMgg * dAbd_Mg[1]);

			double dSig = GetAbundance(uiI,uiSi28_Idx, cData) + GetAbundance(uiI,uiS32_Idx, cData) + GetAbundance(uiI,uiAr36_Idx, cData) + GetAbundance(uiI,uiCa40_Idx, cData) + GetAbundance(uiI,uiTi44_Idx, cData) + GetAbundance(uiI,uiCr48_Idx, cData);
			cData.SetElement(uiSi28_Idx,uiI,dSig * dAbd_Si[0]);
			cData.SetElement(uiS32_Idx, uiI,dSig * dAbd_Si[1]);
			cData.SetElement(uiAr36_Idx,uiI,dSig * dAbd_Si[2]);
			cData.SetElement(uiCa40_Idx,uiI,dSig * dAbd_Si[3]);
			cData.SetElement(uiTi44_Idx,uiI,dSig * dAbd_Si[4]);
			cData.SetElement(uiCr48_Idx,uiI,dSig * dAbd_Si[5]);

			double dNig = GetAbundance(uiI,uiFe52_Idx, cData) + GetAbundance(uiI,uiNi56_Idx, cData);
			cData.SetElement(uiFe52_Idx,uiI,dNig * dAbd_Ni[0]);
			cData.SetElement(uiNi56_Idx,uiI,dNig * dAbd_Ni[1]);
		}
	}

	lpdVolume_tref = new double[uiNum_Points];
	lpdElectron_Density[0] = new double[uiNum_Points];
	lpdElectron_Density[1] = new double[uiNum_Points];
	lpdElectron_Density[2] = new double[uiNum_Points];
	lpdElectron_Density[3] = new double[uiNum_Points];
	lpdElectron_Density[4] = new double[uiNum_Points];
	lpdElectron_Density[5] = new double[uiNum_Points];
	lpdElectron_Density_Temp = new double[uiNum_Points];


	double dVol_Const = 4.0 / 3.0 * g_cConstants.dPi;
	uiShell_Lower_Idx = 0;
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		if (GetAbundance(uiI,uiHe3_Idx, cData) > 0.0)
			uiShell_Lower_Idx = uiI;
		double dXim12 = cData.GetElement(uiCoord_Idx,uiI) - cData.GetElement(uiZone_Size_Idx,uiI) * 0.5;
		double dXip12 = cData.GetElement(uiCoord_Idx,uiI) + cData.GetElement(uiZone_Size_Idx,uiI) * 0.5;
		
		lpdVolume_tref[uiI] = dVol_Const * (dXip12 * dXip12 * dXip12 - dXim12 * dXim12 * dXim12); // volume of element at reference time
	}

	cCmn.dTimeRef = dTimeRef;
	cCmn.lpcData = &cData;
	cCmn.lpcRef_Line_Data = &cRef_Lines;
	cCmn.uiVel_Idx = uiVel_Idx;
	cCmn.uiDens_Idx = uiDens_Idx;
	cCmn.lpdElectron_Density = lpdElectron_Density_Temp;
	cCmn.bOutput_Intermediate = bEnable_Tau_Output;
	cCmn.bForced_PS_Velocity = dForce_PS_Vel > 0.0;
	if (dForce_PS_Vel > 0.0)
		dForce_PS_Vel *= 1.0e9;

	cPE_Cmn.dData_Spacing = dData_Spacing;
	cPE_Cmn.bOutput_Intermediate_YAML = bEnable_Elemental_Spectra_Output;

    for (uiJ = uiDmin; uiJ < uiDmax; uiJ += uiDstep)
    {
//		uiJ = 6; //@@DEBUG
        double dTime = 86500.0 * uiJ + 43250.0; // set time to day (J + 0.5)
		Generate_Decay_Products(dTimeRef, dTime, cData, uiNi56_Idx, uiNi56Decay_Idx, uiCo56Decay_Idx, uiFe56Decay_Idx, dNi56_Halflifes, dNi56_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiFe52_Idx, uiFe52Decay_Idx, uiMn52Decay_Idx, uiCr52Decay_Idx, dFe52_Halflifes, dFe52_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiCr48_Idx, uiCr48Decay_Idx, uiV48Decay_Idx, uiTi48Decay_Idx, dCr48_Halflifes, dCr48_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiTi44_Idx, uiTi44Decay_Idx, uiSc44Decay_Idx, uiCa44Decay_Idx, dTi44_Halflifes, dTi44_Nuclear_Mass);

		// Generate combined element groups (e.g. combine Ca-40 and Ca-44 into a single Ca group)
		for (uiI = 0; uiI < uiNum_Points; uiI++)
		{
			double dSum;
			//double	dR = cVelocity.GetElement(0,uiI);

			dSum = 0.0;
			dSum += cData.GetElement(uiCa40_Idx,uiI);
			dSum += cData.GetElement(uiCa44Decay_Idx,uiI);
			cData.SetElement(uiCaComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiTi44_Idx,uiI);
			dSum += cData.GetElement(uiTi48_Idx,uiI);
			dSum += cData.GetElement(uiTi48Decay_Idx,uiI);
			cData.SetElement(uiTiComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiCr52_Idx,uiI);
			dSum += cData.GetElement(uiCr48Decay_Idx,uiI);
			dSum += cData.GetElement(uiCr52Decay_Idx,uiI);
			cData.SetElement(uiCrComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiFe56_Idx,uiI);
			dSum += cData.GetElement(uiFe56Decay_Idx,uiI);
			dSum += cData.GetElement(uiFe52Decay_Idx,uiI);
			cData.SetElement(uiFeComb_Idx,uiI,dSum);
		}

		if (bEnable_Abundance_Output)
		{
			if (dForce_PS_Vel > 0)
				sprintf(lpszFilename,"./results/abd/run_d%i_ps%.0f.csv",uiJ,dForce_PS_Vel * 1.0e-7);
			else
				sprintf(lpszFilename,"./results/abd/run_d%i.csv",uiJ);
		
			FILE * fileOut = fopen(lpszFilename,"wt");
			if (fileOut)
			{
				fprintf(fileOut,"Velocity, H, He, C, N, O, Ne, Mg, Si, S, Ar, Ca, Ti, Cr, Mn, Fe, Co, Ni\n");
				for (uiI = 0; uiI < uiNum_Points; uiI++)
				{
					fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n",
						cData.GetElement(uiVel_Idx,uiI),
						GetAbundance(uiI,uiHyd_Idx, cData),
						GetAbundance(uiI,uiHeComb_Idx, cData),
						GetAbundance(uiI,uiC12_Idx, cData),
						GetAbundance(uiI,uiN14_Idx, cData),
						GetAbundance(uiI,uiO16_Idx, cData),
						GetAbundance(uiI,uiNe20_Idx, cData),
						GetAbundance(uiI,uiMg24_Idx, cData),
						GetAbundance(uiI,uiSi28_Idx, cData),
						GetAbundance(uiI,uiS32_Idx, cData),
						GetAbundance(uiI,uiAr36_Idx, cData),
						GetAbundance(uiI,uiCaComb_Idx, cData),
						GetAbundance(uiI,uiTiComb_Idx, cData),
						GetAbundance(uiI,uiCrComb_Idx, cData),
						GetAbundance(uiI,uiMn52Decay_Idx, cData),
						GetAbundance(uiI,uiFeComb_Idx, cData),
						GetAbundance(uiI,uiCo56Decay_Idx, cData),
						GetAbundance(uiI,uiNi56Decay_Idx, cData)
					);

				}
				fclose(fileOut);
			}
		}
		double dExterior_Density = (cData.GetElement(uiDens_Idx,uiNum_Points - 1) * 1.05); // 5% higher than density in outermost zone


//        if (dPhotosphere_Velocity[2] > dRef_Velocity)
//                dRef_Velocity = dPhotosphere_Velocity[2] + 0.2;
//        for (uiI = 0; uiI < uiNum_Points; uiI++)
//        {
//                if (cData.GetElement(uiVel_Idx,uiI) > dRef_Velocity && uiVel_Ref_Idx == 0)
//                        uiVel_Ref_Idx = uiI;
//        }

        for (uiL = uiAmin; uiL < uiAmax; uiL+=uiAstep) // loop for temp power law index
        {
//                        uiL = 0;//@@DEBUG
            double	dTemp_Alpha = uiL * -0.1; // T \propto 1/sqrt(r)
            
            for (uiK = uiTmin; uiK < uiTmax; uiK+=uiTstep) // loop for temp
            {
//                            uiK = 16; //@@DEBUG
                dTemp = uiK;// * 500.0 + 7000.0;
				double dkT = dTemp * g_XASTRO.k_dKb;
//				double	dkTev = dTemp * g_XASTRO.k_dKb_eV;

				double	dElectron_Optical_Depth_tref[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
				memset(lpdElectron_Density[0],0,sizeof(double) * uiNum_Points);
				memset(lpdElectron_Density[1],0,sizeof(double) * uiNum_Points);
				memset(lpdElectron_Density[2],0,sizeof(double) * uiNum_Points);
				memset(lpdElectron_Density[3],0,sizeof(double) * uiNum_Points);
				memset(lpdElectron_Density[4],0,sizeof(double) * uiNum_Points);
				memset(lpdElectron_Density[5],0,sizeof(double) * uiNum_Points);
				for (uiI = 0; uiI < uiNum_Points; uiI++)
				{
					double	dNumber_Density = cData.GetElement(uiDens_Idx,uiI);// * (dTimeRef * dTimeRef * dTimeRef);
					if (dNumber_Density < dExterior_Density) // make the true CSM very low density
						dNumber_Density *= (1.0e-24 / dExterior_Density);
					if (uiI < 10)
						printf("%.2e: %.2e\n",cData.GetElement(uiCoord_Idx,uiI),dNumber_Density);
					dNumber_Density /= g_XASTRO.k_dmh;
					double	dAbd[][2] = {
						{GetAbundance(uiI,uiHyd_Idx, cData),1.0},
						{GetAbundance(uiI,uiHe3_Idx, cData),3.0},
						{GetAbundance(uiI,uiHe4_Idx, cData),4.0},
						{GetAbundance(uiI,uiC12_Idx, cData),12.0},
						{GetAbundance(uiI,uiN14_Idx, cData),14.0},
						{GetAbundance(uiI,uiO16_Idx, cData),16.0},
						{GetAbundance(uiI,uiNe20_Idx, cData),20.0},
						{GetAbundance(uiI,uiMg24_Idx, cData),24.0},
						{GetAbundance(uiI,uiSi28_Idx, cData),28.0},
						{GetAbundance(uiI,uiS32_Idx, cData),32.0},
						{GetAbundance(uiI,uiAr36_Idx, cData),36.0},
						{GetAbundance(uiI,uiCa40_Idx, cData),40.0},
						{GetAbundance(uiI,uiCa44Decay_Idx, cData) + GetAbundance(uiI,uiCa44_Idx, cData),44.0},
						{GetAbundance(uiI,uiSc44Decay_Idx, cData),44.0},
						{GetAbundance(uiI,uiTi44Decay_Idx, cData) + GetAbundance(uiI,uiTi44_Idx, cData),44.0},
						{GetAbundance(uiI,uiTi48Decay_Idx, cData) + GetAbundance(uiI,uiTi48_Idx, cData),48.0},
						{GetAbundance(uiI,uiV48Decay_Idx, cData),48.0},
						{GetAbundance(uiI,uiCr48Decay_Idx, cData) + GetAbundance(uiI,uiCr48_Idx, cData),48.0},
						{GetAbundance(uiI,uiCr52Decay_Idx, cData) + GetAbundance(uiI,uiCr52_Idx, cData),52.0},
						{GetAbundance(uiI,uiMn52Decay_Idx, cData),52.0},
						{GetAbundance(uiI,uiFe52Decay_Idx, cData) + GetAbundance(uiI,uiFe52_Idx, cData),52.0},
						{GetAbundance(uiI,uiFe56Decay_Idx, cData) + GetAbundance(uiI,uiFe56_Idx, cData),56.0},
						{GetAbundance(uiI,uiCo56Decay_Idx, cData),56.0},
						{GetAbundance(uiI,uiNi56_Idx, cData),56.0}
						};


					unsigned int uiNum_Abd = sizeof(dAbd) / (2 * sizeof(double));

					for (unsigned int uiAbd = 0; uiAbd < uiNum_Abd; uiAbd++)
					{
						double dNj = dNumber_Density * dAbd[uiAbd][0] / dAbd[uiAbd][1];
						// estimate singly ionized ion density
						lpdElectron_Density[0][uiI] += dNj;

						if (uiAbd > 0) // Exclude Hydrogen
							lpdElectron_Density[1][uiI] += dNj; // doubly ionized
						if (uiAbd > 2) // Exclude Helium
						{
							lpdElectron_Density[2][uiI] += dNj; // triply ionized
							lpdElectron_Density[3][uiI] += 0.5 * dNumber_Density * dAbd[uiAbd][0]; // fully ionized
						}

						if (lpcIon_Energy_Data[uiAbd])
						{
							for (unsigned int uiN = 0; uiN < lpcIon_Energy_Data[uiAbd]->m_uiIonization_Energies_Count; uiN++)
							{
//								printf("%i (%i): %.2e\t%.2e\n",uiAbd,uiN,dkT, lpcIon_Energy_Data[uiAbd]->m_lpIonization_energies_erg[uiN]);
								if (dkT > (lpcIon_Energy_Data[uiAbd]->m_lpIonization_energies_erg[uiN] * 0.042)) // rough estimate for temperature at which ion stage is reached
									lpdElectron_Density[4][uiI] += dNj;
							}
						}


					}
					lpdElectron_Density[1][uiI] += lpdElectron_Density[0][uiI];
					lpdElectron_Density[2][uiI] += lpdElectron_Density[1][uiI];
					lpdElectron_Density[3][uiI] += dNumber_Density * dAbd[0][0]; // add Hydrogen
					lpdElectron_Density[3][uiI] += 0.5 * dNumber_Density * dAbd[1][0]; // add helium

					double dDeltaX = cData.GetElement(uiZone_Size_Idx,uiI);
					double	dConst_Terms = 0.66524574e-24; // electron scattering optical depth
					dElectron_Optical_Depth_tref[0] += lpdElectron_Density[0][uiI] * dConst_Terms * dDeltaX;
					dElectron_Optical_Depth_tref[1] += lpdElectron_Density[1][uiI] * dConst_Terms * dDeltaX;
					dElectron_Optical_Depth_tref[2] += lpdElectron_Density[2][uiI] * dConst_Terms * dDeltaX;
					dElectron_Optical_Depth_tref[3] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX;
					dElectron_Optical_Depth_tref[4] += lpdElectron_Density[4][uiI] * dConst_Terms * dDeltaX;
			//		double	dKappa_0 = lpdElectron_Density[0][uiI] * dConst_Terms
			//		printf("%.3e\t%.3e\t%.3e\t%.3e\t%.3e\t%.3e\n",dDeltaX,cDens.GetElement(1,uiI) / (12 * g_XASTRO.k_dmh),lpdElectron_Density[0][uiI],lpdElectron_Density[1][uiI],lpdElectron_Density[2][uiI],lpdElectron_Density[0][uiI] * 0.66524574e-24 * dDeltaX);
				}
				printf("---------------------\n%.3e\t%.3e\t%.3e\t%.3e\t%.3e\n--------------------\n",dElectron_Optical_Depth_tref[0],dElectron_Optical_Depth_tref[1],dElectron_Optical_Depth_tref[2],dElectron_Optical_Depth_tref[3],dElectron_Optical_Depth_tref[4]);
				double	dElectron_Optical_Depth[5] = {0.0,0.0,0.0,0.0,0.0};
				double	dElectron_Optical_Depth_Save = 0.0;
				double	dConst_Terms = 0.66524574e-24;// * pow(dTime / dTimeRef,-3.0);// * pow(dTime,-3.0);
				dPhotosphere_Velocity[0] = dPhotosphere_Velocity[1] = dPhotosphere_Velocity[2] = dPhotosphere_Velocity[3] = dPhotosphere_Velocity[4] = 0.0;
				uiPS_Velocity_Idx[0] = uiPS_Velocity_Idx[1] = uiPS_Velocity_Idx[2] = uiPS_Velocity_Idx[3] = uiPS_Velocity_Idx[4] = 0;
				if (dForce_PS_Vel > 0.0)
					memcpy(lpdElectron_Density_Temp,lpdElectron_Density[3],sizeof(double) * uiNum_Points);
				else
					memcpy(lpdElectron_Density_Temp,lpdElectron_Density[4],sizeof(double) * uiNum_Points);

				for (uiI = uiNum_Points - 1; uiI < uiNum_Points; uiI--)
				{
						// Get coordinates, box size and, velocity for zone and its neighbors
						double dXi = cData.GetElement(uiCoord_Idx,uiI);
				        double	dDeltaX = cData.GetElement(uiZone_Size_Idx,uiI);
						double dXim1 = 0.0;
						double dXip1 = dXi + dDeltaX;
						double	dVi = cData.GetElement(uiVel_Idx,uiI);
						double dVim1 = 0.0;
						double dVip1 = dVi;

						if (uiI != 0)
						{
							dXim1 = cData.GetElement(uiCoord_Idx,uiI - 1);
							dVim1 = cData.GetElement(uiVel_Idx,uiI - 1);
						}
						if (uiI != uiNum_Points - 1)
						{
							dXip1 = cData.GetElement(uiCoord_Idx,uiI + 1);
							dVip1 = cData.GetElement(uiVel_Idx,uiI + 1);
						}
						// compute position and velocity at zone boundaries (at time tref)
						double dXim12 = dXi - dDeltaX * 0.5;
						double dXip12 = dXi + dDeltaX * 0.5;
//						if (uiI == uiNum_Points - 5)
//							printf("v at point 200 (%.2e,%.2e): %.2e - %.2e = %.2e\n",dXi,dDeltaX,dXim12,dXip12,lpdVolume_tref[uiI]);
						double dVim12 = 0.0;// = dVi - (dVi - dVim1) / (dXi - dXim1) * (dXim12 - dXim1);
						if (uiI != 0)
							dVim12 = dVi - (dVi - dVim1) / (dXi - dXim1) * (dXim12 - dXim1);
						double dVip12 = dVi + (dVip1 - dVi) / (dXip1 - dXi) * (dXip12 - dXi);
						// compute zone boundaries at current time
						dXim12 += dVim12 * (dTime - dTimeRef);
						dXip12 += dVip12 * (dTime - dTimeRef);
						// compute volume of zone at current time		
						double dVolume = dVol_Const * (dXip12 * dXip12 * dXip12 - dXim12 * dXim12 * dXim12);
//						if (uiI == uiNum_Points - 5)
//							printf("v at point 200 (%.2e): %.2e - %.2e = %.2e\n",dVi,dXim12,dXip12,dVolume);
						// compute scaling factor for density between tref and now
						double dVolTerm = (lpdVolume_tref[uiI] / dVolume);
						dDeltaX = dXip12 - dXim12; // compute the delta x at current time
				        // compute optical depth for electron scattering, assuming partial ionizations
		//			printf("%.3e\t%.3e\t%.3e\n",lpdElectron_Density[0][uiI],dDeltaX,dElectron_Optical_Depth[0]);
				        dElectron_Optical_Depth[0] += lpdElectron_Density[0][uiI] * dConst_Terms * dDeltaX * dVolTerm;
				        if (dElectron_Optical_Depth[0] > 1.0 && dPhotosphere_Velocity[0] == 0.0)
				        {
				                dPhotosphere_Velocity[0] = cData.GetElement(uiVel_Idx,uiI);
				                uiPS_Velocity_Idx[0] = uiI;
				        }
				        dElectron_Optical_Depth[1] += lpdElectron_Density[1][uiI] * dConst_Terms * dDeltaX * dVolTerm;
				        if (dElectron_Optical_Depth[1] > 1.0 && dPhotosphere_Velocity[1] == 0.0)
				        {
				                dPhotosphere_Velocity[1] = cData.GetElement(uiVel_Idx,uiI);
				                uiPS_Velocity_Idx[1] = uiI;
				        }
				        dElectron_Optical_Depth[2] += lpdElectron_Density[2][uiI] * dConst_Terms * dDeltaX * dVolTerm;
				        if (dElectron_Optical_Depth[2] > 1.0 && dPhotosphere_Velocity[2] == 0.0)
				        {
				                dPhotosphere_Velocity[2] = cData.GetElement(uiVel_Idx,uiI);
				                uiPS_Velocity_Idx[2] = uiI;
				        }
				        dElectron_Optical_Depth[3] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX * dVolTerm;
				        if (dElectron_Optical_Depth[3] > 1.0 && dPhotosphere_Velocity[3] == 0.0)
				        {
				                dPhotosphere_Velocity[3] = cData.GetElement(uiVel_Idx,uiI);
				                uiPS_Velocity_Idx[3] = uiI;
				        }
				        dElectron_Optical_Depth[4] += lpdElectron_Density[4][uiI] * dConst_Terms * dDeltaX * dVolTerm;
				        if (dElectron_Optical_Depth[4] > 1.0 && dPhotosphere_Velocity[4] == 0.0)
				        {
				                dPhotosphere_Velocity[4] = cData.GetElement(uiVel_Idx,uiI);
				                uiPS_Velocity_Idx[4] = uiI;
				        }
				}
				//return 1;
				if (dForce_PS_Vel > 0.0)
				{
					uiI = 0;
					dElectron_Optical_Depth_Save = dElectron_Optical_Depth[3];
					dElectron_Optical_Depth[3] = 0.0;
					while (uiI < uiNum_Points && cData.GetElement(uiVel_Idx,uiI) < dForce_PS_Vel)
						    uiI++;
					uiPS_Velocity_Idx_Forced = uiI;
					printf("Forced PS Velocity = %.1f kkm/s (Idx %i)\n",dForce_PS_Vel * 1.0e-8,uiPS_Velocity_Idx_Forced);
					for (;uiI < uiNum_Points; uiI++)
					{
						    // compute optical depth for electron scattering, assuming partial ionizations
							double dXi = cData.GetElement(uiCoord_Idx,uiI);
						    double	dDeltaX = cData.GetElement(uiZone_Size_Idx,uiI);
							double dXim1 = 0.0;
							double dXip1 = dXi + dDeltaX;
							double	dVi = cData.GetElement(uiVel_Idx,uiI);
							double dVim1 = 0.0;
							double dVip1 = dVi;

							if (uiI != 0)
							{
								dXim1 = cData.GetElement(uiCoord_Idx,uiI - 1);
								dVim1 = cData.GetElement(uiVel_Idx,uiI - 1);
							}
							if (uiI != uiNum_Points - 1)
							{
								dXip1 = cData.GetElement(uiCoord_Idx,uiI + 1);
								dVip1 = cData.GetElement(uiVel_Idx,uiI + 1);
							}
							// compute position and velocity at zone boundaries (at time tref)
							double dXim12 = dXi - dDeltaX * 0.5;
							double dXip12 = dXi + dDeltaX * 0.5;
//							if (uiI == uiNum_Points - 5)
//								printf("v at point 200 (%.2e,%.2e): %.2e - %.2e = %.2e\n",dXi,dDeltaX,dXim12,dXip12,lpdVolume_tref[uiI]);
							double dVim12 = dVi - (dVi - dVim1) / (dXi - dXim1) * (dXim12 - dXim1);
							double dVip12 = dVi + (dVip1 - dVi) / (dXip1 - dXi) * (dXip12 - dXi);
							// compute zone boundaries at current time
							dXim12 += dVim12 * (dTime - dTimeRef);
							dXip12 += dVip12 * (dTime - dTimeRef);
							// compute volume of zone at current time		
							double dVolume = dVol_Const * (dXip12 * dXip12 * dXip12 - dXim12 * dXim12 * dXim12);
//							if (uiI == uiNum_Points - 5)
//								printf("v at point 200 (%.2e): %.2e - %.2e = %.2e\n",dVi,dXim12,dXip12,dVolume);
							// compute scaling factor for density between tref and now
							double dVolTerm = (lpdVolume_tref[uiI] / dVolume);
							dDeltaX = dXip12 - dXim12; // compute delta x at current time
			//			printf("%.3e\t%.3e\t%.3e\n",lpdElectron_Density[0][uiI],dDeltaX,dElectron_Optical_Depth[0]);
						    dElectron_Optical_Depth[3] += lpdElectron_Density[3][uiI] * dConst_Terms * dDeltaX * dVolTerm;
					}
					dElectron_Scaling = 1.0 / dElectron_Optical_Depth[3];
					if (dElectron_Scaling > 1.0)
						dElectron_Scaling = 1.0; // don't allow it to go > 1

					dPhotosphere_Velocity[3] = dForce_PS_Vel;// force
					uiPS_Velocity_Idx[3] = uiPS_Velocity_Idx_Forced;

					if (dElectron_Scaling < 1.0)
					{
						for (uiI = 0; uiI < uiNum_Points; uiI++)
						{
								lpdElectron_Density_Temp[uiI] *= dElectron_Scaling;
						}
					}
				}
				printf("%i   %.2e   %.2e   %.2e   %.2e   %.2e   | %.2e   %.2e   %.2e   %.2e (%.2e)   %.2e  | %.2e \n",uiJ,dPhotosphere_Velocity[0],dPhotosphere_Velocity[1],dPhotosphere_Velocity[2],dPhotosphere_Velocity[3],dPhotosphere_Velocity[4],dElectron_Optical_Depth[0],dElectron_Optical_Depth[1],dElectron_Optical_Depth[2],dElectron_Optical_Depth[3],dElectron_Optical_Depth_Save,dElectron_Optical_Depth[4],dElectron_Scaling);
		//	}
				//return 1;


				// Set reference velocity for photosphere clculations
				if (dForce_PS_Vel > 0.0)
				{
					uiVel_Ref_Idx = uiPS_Velocity_Idx[3];
					uiPS_Vel_Idx = uiPS_Velocity_Idx[3];
					dPS_Velocity = dPhotosphere_Velocity[3];
					dRef_Velocity = dPhotosphere_Velocity[3];
				}
				else
				{
					uiVel_Ref_Idx = uiPS_Velocity_Idx[0];
					uiPS_Vel_Idx = uiPS_Velocity_Idx[0];
					dPS_Velocity = dPhotosphere_Velocity[0];
					dRef_Velocity = dPhotosphere_Velocity[0];
				}
				printf("Ref velocity (1) = %.2e\n",dRef_Velocity);
				// round ref velocity to nearest km/s to ensure consistency
				dRef_Velocity -= fmod(dRef_Velocity,1.0e5);
//				dRef_Velocity *= 1.0e-5;
//				dRef_Velocity -= floor(dRef_Velocity * 1.0e-5);
//				dRef_Velocity *= 1.0e5;

				cCmn.dTime = dTime;
				cCmn.dTemp_Ref = dTemp;
				cCmn.dRef_Velocity = dRef_Velocity;
				printf("Ref velocity = %.2e\n",cCmn.dRef_Velocity);
				cCmn.dPS_Velocity = dPS_Velocity;
				cCmn.dTemp_Alpha = dTemp_Alpha;
				cCmn.uiDay = uiJ;
				cCmn.uiAlpha = uiL;
				cCmn.uiRef_Vel_Idx = uiVel_Ref_Idx;
				cCmn.uiPS_Vel_Idx = uiPS_Vel_Idx;

                cSynIonDataLL.Clean();
                printf("-------------Day %i Temp %.0f Alpha %.1f --------------------------------------\n",uiJ,dTemp,-dTemp_Alpha);
//			unsigned int uiSynIonDataIdx = 0;
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cH_Lines, dH_Temp_Offset, uiHyd_Idx,  bEnable_H, "H");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cHe_Lines, dHe_Temp_Offset, uiHeComb_Idx, bEnable_He, "He");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cC_Lines, dC_Temp_Offset, uiC12_Idx, bEnable_C, "C");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cN_Lines, dN_Temp_Offset, uiN14_Idx, bEnable_N, "N");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cO_Lines, dO_Temp_Offset, uiO16_Idx, bEnable_O, "O");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cNe_Lines, dNe_Temp_Offset, uiNe20_Idx, bEnable_Ne, "Ne");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cMg_Lines, dMg_Temp_Offset, uiMg24_Idx, bEnable_Mg, "Mg");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cSi_Lines, dSi_Temp_Offset, uiSi28_Idx, bEnable_Si, "Si");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cS_Lines, dS_Temp_Offset, uiS32_Idx, bEnable_S, "S");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cAr_Lines, dAr_Temp_Offset, uiAr36_Idx, bEnable_Ar, "Ar");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cCa_Lines, dCa_Temp_Offset, uiCaComb_Idx, bEnable_Ca, "Ca");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cTi_Lines, dTi_Temp_Offset, uiTiComb_Idx, bEnable_Ti, "Ti");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cV_Lines, dV_Temp_Offset, uiV48Decay_Idx, bEnable_V, "V");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cCr_Lines, dCr_Temp_Offset, uiCrComb_Idx, bEnable_Cr, "Cr");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cMn_Lines, dMn_Temp_Offset, uiMn52Decay_Idx, bEnable_Mn, "Mn");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cFe_Lines, dFe_Temp_Offset, uiFeComb_Idx, bEnable_Fe, "Fe");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cCo_Lines, dCo_Temp_Offset, uiCo56Decay_Idx, bEnable_Co, "Co");
				Process_Element(cCmn, cPE_Cmn, cSynIonDataLL, cNi_Lines, dNi_Temp_Offset, uiNi56Decay_Idx, bEnable_Ni, "Ni");

				printf("Fill done\n");

				if (dForce_PS_Vel > 0.0)
					sprintf(lpszFilename,"./results/yaml/run_d%i_T%.0f_a%i_ps%3.0f.yaml",uiJ,dTemp,uiL,dPS_Velocity *1.0e-7);
				else
					sprintf(lpszFilename,"./results/yaml/run_d%i_T%.0f_a%i.yaml",uiJ,dTemp,uiL);

				Output_YAML_File(lpszFilename,cSynIonDataLL,dData_Spacing, uiJ,dRef_Velocity, dPS_Velocity, dTemp);
				if (dForce_PS_Vel > 0.0)
					sprintf(lpszFilename,"./results/syn_csv/run_d%i_T%.0f_a%i_ps%3.0f.csv",uiJ,dTemp,uiL,dPS_Velocity * 1.0e-7);
				else
					sprintf(lpszFilename,"./results/syn_csv/run_d%i_T%.0f_a%i.csv",uiJ,dTemp,uiL);
				Output_YAML_Data_To_CSV(lpszFilename, cSynIonDataLL);


			}
		}
	}


//	delete [] cSynIonData;

	delete [] lpdElectron_Density[0];
	delete [] lpdElectron_Density[1];
	delete [] lpdElectron_Density[2];
	delete [] lpdElectron_Density[3];
	delete [] lpdElectron_Density[4];
	delete [] g_lpdTau;
	return 0;
}

