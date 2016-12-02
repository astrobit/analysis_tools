#include<cstdio>
#include<cstdlib>
#include <map>
#include <iostream>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <vector>
#include <opacity_project_pp.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	unsigned int uiN = 20, uiZeff = 20;
	opacity_project_ion opiArI(uiN,uiZeff);

	double dMin = DBL_MAX;
	double dMax = 0.0;
	for (imscs iterI = opiArI.m_mscsPI_Cross_Sections.begin(); iterI != opiArI.m_mscsPI_Cross_Sections.end(); iterI++)
	{
		for (imddcs iterJ = iterI->second.begin(); iterJ != iterI->second.end(); iterJ++)
		{
			double dE = log10(iterJ->first);
			if (dE < dMin)
				dMin = dE;
			if (dE > dMax)
				dMax = dE;
		}
	}

	double dBin = (dMax - dMin) / 1024.0;
	double lpdCS[1024];
	memset(lpdCS,0.0,sizeof(double) * 1024);
	
	for (imscs iterI = opiArI.m_mscsPI_Cross_Sections.begin(); iterI != opiArI.m_mscsPI_Cross_Sections.end(); iterI++)
	{
		for (imddcs iterJ = iterI->second.begin(); iterJ != iterI->second.end(); iterJ++)
		{
			double dE = log10(iterJ->first);
			unsigned int uiIdx = (dE - dMin) / dBin;
			if (uiIdx > 1024)
				uiIdx = 1023;
			lpdCS[uiIdx] += iterJ->second;
		}
	}

	char lpszFilename[64];
	sprintf(lpszFilename,"lvl%02i.%02i.csv",uiN,uiZeff);

	FILE * fileOutLvl = fopen(lpszFilename,"wt");
	if (fileOutLvl)
	{
		for (imscs iterI = opiArI.m_mscsPI_Cross_Sections.begin(); iterI != opiArI.m_mscsPI_Cross_Sections.end(); iterI++)
		{
			imddcs iterJ = iterI->second.begin();
			while (iterJ != iterI->second.end() && fabs(iterJ->first-iterI->first.m_dEnergy_eV) < 0.01)
				iterJ++;
			if (iterJ != iterI->second.end())
				fprintf(fileOutLvl,"%i, %i, %.17e, %.17e\n", iterI->first.m_uiS, iterI->first.m_uiL, iterJ->first,iterJ->second);
		}
		fclose(fileOutLvl);
	}

	sprintf(lpszFilename,"ccs%02i.%02i.csv",uiN,uiZeff);
	FILE * fileOutCS = fopen(lpszFilename,"wt");
	if (fileOutCS)
	{
		for (unsigned int uiI = 0; uiI < 1024; uiI++)
		{
			if (lpdCS[uiI] != 0.0)
				fprintf(fileOutCS,"%.17e, %.17e\n",pow(10.0,dMin + (uiI + 0.5) * dBin),lpdCS[uiI]);
		}
		fclose(fileOutCS);
	}

	return 0;
}


