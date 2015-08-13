#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <xio.h>
#include <iostream>
#include <sstream>
#include <line_routines.h>


void Resample(const double & i_dVmax, const XDATASET & i_cData, XDATASET & io_cResampled, const ABUNDANCE_LIST & i_cAbundance, bool i_bGroup_Abundances)
{
	double	dVol_Const = 4.0 / 3.0 * acos(-1.0);

	for (unsigned int uiI = 0; uiI < i_cData.GetNumElements(); uiI++)
	{
		double dDens = i_cData.GetElement(8,uiI);
		double dR = i_cData.GetElement(0,uiI);
		double dDelta_R = i_cData.GetElement(1,uiI);
		double dRin = dR - 0.5 * dDelta_R;
		double dRout = dR + 0.5 * dDelta_R;
		double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
		if (dVol < 0.0)
			dVol *= -1.0;
		double dMass = dDens * dVol;
		double	dVin = i_cData.GetElement(26,uiI);
		double	dVout = i_cData.GetElement(27,uiI);
		double dV = i_cData.GetElement(25,uiI);
		double dVmin = dVin;
		double dVmax = dVout;
		if (dV < dVmin)
			dVmin = dV;
		if (dVout < dVmin)
			dVmin = dVout;
		if (dV > dVmax)
			dVmax = dV;
		if (dVin > dVmax)
			dVmax = dVin;
		double dVin_Idx = ((dVmin / i_dVmax) * 128);
		double dVout_Idx = ((dVmax / i_dVmax) * 128);
		unsigned int uiVin_Idx = (dVin_Idx - fmod(dVin_Idx,1.0) + 0.1);
		unsigned int uiVout_Idx = (dVout_Idx - fmod(dVout_Idx,1.0) + 0.1);
		double dVel_Frac = (1.0 / (uiVout_Idx - uiVin_Idx + 1));

		if (i_bGroup_Abundances)
		{
			double	dC = i_cData.GetElement(3,uiI);
			double	dO = i_cData.GetElement(19,uiI);
			double	dMg_Group = i_cData.GetElement(14,uiI) + i_cData.GetElement(17,uiI);
			double	dSi_Group = i_cData.GetElement(2,uiI) + i_cData.GetElement(4,uiI) + i_cData.GetElement(20,uiI) + i_cData.GetElement(21,uiI);
			double	dFe_Group = i_cData.GetElement(5,uiI) + i_cData.GetElement(6,uiI) + i_cData.GetElement(7,uiI) + i_cData.GetElement(9,uiI) + i_cData.GetElement(10,uiI) + i_cData.GetElement(15,uiI) + i_cData.GetElement(18,uiI) + i_cData.GetElement(22,uiI) + i_cData.GetElement(23,uiI) + i_cData.GetElement(24,uiI);
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				double dC_vel = io_cResampled.GetElement(6,uiJ);
				io_cResampled.SetElement(6,uiJ,dC_vel + dC * dVel_Frac * dMass);

				double dO_vel = io_cResampled.GetElement(8,uiJ);
				io_cResampled.SetElement(8,uiJ,dO_vel + dO * dVel_Frac * dMass);

				for (unsigned int uiK = 9; uiK <= 13; uiK++) // F to Al: Mg group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dMg_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
				for (unsigned int uiK = 14; uiK <= 20; uiK++) // Si - Ca: Si group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dSi_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
				for (unsigned int uiK = 21; uiK <= 28; uiK++) // Sc to Ni: Fe group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dSi_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
			}
		}
		else
		{
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				for (unsigned int uiK = 1; uiK <= 28; uiK++)
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
			}
		}
	}
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[32];
	XDATASET cEjecta;
	XDATASET cShell;
	XDATASET	cCombined_Data;
	ABUNDANCE_LIST	cShell_Abundance;
	ABUNDANCE_LIST	cEjecta_Abundance;

	if (i_iArg_Count == 2)
	{
		unsigned int uiFile_Num = atoi(i_lpszArg_Values[1]);
		cShell_Abundance.Read_Table(Solar);
		cEjecta_Abundance.Read_Table(Seitenzahl_N100_2013);
		cEjecta_Abundance.Normalize_Groups();

		sprintf(lpszFilename,"ejecta%04i.xdataset",uiFile_Num);
		cEjecta.ReadDataFileBin(lpszFilename);

		sprintf(lpszFilename,"shell%04i.xdataset",uiFile_Num);
		cShell.ReadDataFileBin(lpszFilename);

		double	dVmax = 0.0;
		if (cShell.GetNumElements() > 0)
		{
			for (unsigned int uiI = 0; uiI < cShell.GetNumElements(); uiI++)
			{
				if (dVmax < cShell.GetElement(27,uiI))
					dVmax = cShell.GetElement(27,uiI);
				if (dVmax < cShell.GetElement(25,uiI))
					dVmax = cShell.GetElement(25,uiI);
			}
		}
		else
		{
			for (unsigned int uiI = 0; uiI < cEjecta.GetNumElements(); uiI++)
			{
				if (dVmax < cEjecta.GetElement(27,uiI))
					dVmax = cEjecta.GetElement(27,uiI);
				if (dVmax < cEjecta.GetElement(25,uiI))
					dVmax = cEjecta.GetElement(25,uiI);
			}
		}

		cCombined_Data.Allocate(29,128);
		cCombined_Data.Zero(); // make sure all abundances are 0
		for (unsigned int uiI = 0; uiI < 128; uiI++)
		{
			cCombined_Data.SetElement(0,uiI,((uiI + 0.5) / 128.0 * dVmax));

		}
		Resample(dVmax,cEjecta,cCombined_Data,cEjecta_Abundance,true);
		Resample(dVmax,cShell,cCombined_Data,cShell_Abundance,false);
		cCombined_Data.SaveDataFileBin("resampled.xdataset");

		FILE * fileDensity = fopen("density.dat","wt");
		FILE * fileAbundance = fopen("abundance.dat","wt");

		fprintf(fileDensity,"1 days\n"); 
		double	dDelta_v = (cCombined_Data.GetElement(0,1) - cCombined_Data.GetElement(0,0)) * 0.5;
		double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
		for (unsigned int uiI = 0; uiI < 128; uiI++)
		{
			double	dMass = 0.0;
			for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
			{
				dMass += cCombined_Data.GetElement(uiJ,uiI);
			}
			double dV = cCombined_Data.GetElement(0,uiI) * 86400.0;
			double dVin = dV - dDelta_v;
			double dVout = dV + dDelta_v;
			double dRin = dVin * 86400.0;
			double dRout = dVout * 86400.0;
			double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
			fprintf(fileDensity,"%i %.1f %e\n",uiI + 1, cCombined_Data.GetElement(0,uiI) * 1.0e-5, dMass / dVol);
			
			fprintf(fileAbundance,"%i",uiI + 1);
			for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
			{
				if (dMass > 0.0)
					fprintf(fileAbundance," %.8e", cCombined_Data.GetElement(uiJ,uiI) / dMass);
				else if (uiJ == 1)
					fprintf(fileAbundance," 1.0");
				else
					fprintf(fileAbundance," 0.0");
			}
			fprintf(fileAbundance,"\n");
		}
	}
	return 0;
}

