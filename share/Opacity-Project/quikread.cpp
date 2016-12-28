#include<cstdio>
#include<cstdlib>
#include <map>
#include <iostream>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <vector>
#include <opacity_project_pp.h>
#include <xstdlib.h>
#include <sstream>

typedef std::map<double, opacity_project_state> mds;
typedef std::map<double, opacity_project_state>::iterator imds;


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	unsigned int uiN = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--elem", 20);
	unsigned int uiZeff = uiN - xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--ion", 0);//20);	
	if (uiZeff > uiN)
		uiZeff = uiN;
	opacity_project_ion opiArI(uiN,uiZeff);
	mds mdsState_Info;

	for (ims iterI = opiArI.m_msStates.begin(); iterI != opiArI.m_msStates.end(); iterI++)
	{
		mdsState_Info[iterI->second.m_dEnergy_eV] = iterI->second;
	}
	char lpszFilename[256];
	sprintf(lpszFilename,"e%02i.%02i.csv",uiN,uiZeff);
	unsigned int uiIdx = 0;
	FILE * fileEnerOrg = fopen(lpszFilename,"wt");
	if (fileEnerOrg != nullptr)
	{
		fprintf(fileEnerOrg,"Z, N, i, S, L, P, i, T, i, n, l, E (eV), Config\n");
		for (imds iterI = mdsState_Info.begin(); iterI != mdsState_Info.end(); iterI++)
		{
			uiIdx++;
			std::ostringstream ossConfiguration;
			if (iterI->second.m_chType == 'T')
			{
				ossConfiguration << opiArI.m_cConfiguration_Data_Target[iterI->second.m_uiType_Index - 1].m_szState;
				ossConfiguration << " " << iterI->second.m_ui_n;
				switch (iterI->second.m_ui_l)
				{
				case 0:
					ossConfiguration << "s";
					break;
				case 1:
					ossConfiguration << "p";
					break;
				default:
					{
						char chL = iterI->second.m_ui_l + 'd';
						if (chL >= 'e')
							chL++;
						if (chL >= 'j')
							chL++;
						ossConfiguration << chL;
					}
					break;
				}


			}
			else
				ossConfiguration << opiArI.m_cConfiguration_Data_Continuum[iterI->second.m_uiType_Index - 1].m_szState;


			fprintf(fileEnerOrg,"%i, %i, %i, %i, %i, %i, %i, %c, %i, %i, %i, %.5f, %s\n",
				uiN,
				uiZeff,
				uiIdx,
				iterI->second.m_opldDescriptor.m_uiS,
				iterI->second.m_opldDescriptor.m_uiL,
				iterI->second.m_opldDescriptor.m_uiP,
				iterI->second.m_opldDescriptor.m_uiLvl_ID,

				iterI->second.m_chType, // T is the this state is an actual level, C if it is an approximation of a level (?)
				iterI->second.m_uiType_Index, // some index that has something to do with the type if C
				iterI->second.m_ui_n, // quantum number n
				iterI->second.m_ui_l, // quantum number l

				iterI->second.m_dEnergy_eV, // level energy relative to potential at infinity

				ossConfiguration.str().c_str()
			
				);
		}
		fclose(fileEnerOrg);
	}
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

/*	sprintf(lpszFilename,"lvl%02i.%02i.csv",uiN,uiZeff);

	FILE * fileOutLvl = fopen(lpszFilename,"wt");
	if (fileOutLvl)
	{
		for (imscs iterI = opiArI.m_mscsPI_Cross_Sections.begin(); iterI != opiArI.m_mscsPI_Cross_Sections.end(); iterI++)
		{
			imddcs iterJ = iterI->second.begin();
			while (iterJ != iterI->second.end() && fabs(iterJ->first.m_dEnergy_eV) < 0.01)
				iterJ++;
			if (iterJ != iterI->second.end())
				fprintf(fileOutLvl,"%i, %i, %.17e, %.17e\n", iterI->first.m_opldDescriptor.m_uiS, iterI->first.m_opldDescriptor.m_uiL, iterJ->first,iterJ->second);
		}
		fclose(fileOutLvl);
	}*/

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

	sprintf(lpszFilename,"ccsrel%02i.%02i.csv",uiN,uiZeff);
	FILE * fileOutCSrel = fopen(lpszFilename,"wt");
	if (fileOutCSrel)
	{
		fprintf(fileOutCSrel,"n, l, n+l, E, E(cs), CS, E(ref), CS (ref)\n");
		for (ims iterI = opiArI.m_msStates.begin(); iterI != opiArI.m_msStates.end(); iterI++)
		{
			if (iterI->second.m_chType == 'T')
			{
				mddcs mddcsRelations = opiArI.m_mscsPI_Cross_Sections[iterI->first];
				double dMax = 0.0;
				double dMax_E = 0.0;
				double dRef = 0.0;
				double dRef_E = 0.0;
				for (imddcs iterJ = mddcsRelations.begin(); iterJ != mddcsRelations.end(); iterJ++)
				{
					if (dMax < iterJ->second)
					{
						dMax = iterJ->second;
						dMax_E = iterJ->first;
					}
					if (iterJ->first > fabs(iterI->second.m_dEnergy_Ry) && dRef_E == 0.0)
					{
						printf("%e %e\n",iterJ->first,iterI->second.m_dEnergy_Ry);
						dRef = iterJ->second;
						dRef_E = iterJ->first;
					}
				}
				if (dMax != 0.0)
					fprintf(fileOutCSrel,"%i, %i, %i, %.8e, %.8e, %.8e, %.8e, %.8e\n",iterI->second.m_ui_n,iterI->second.m_ui_l,iterI->second.m_ui_n+iterI->second.m_ui_l,iterI->second.m_dEnergy_Ry,dMax_E,dMax,dRef_E,dRef);
			}

		}
		fclose(fileOutCSrel);
	}	

	sprintf(lpszFilename,"ccsraw%02i.%02i.csv",uiN,uiZeff);
	FILE * fileOutCSraw = fopen(lpszFilename,"wt");
	if (fileOutCSraw)
	{
		fprintf(fileOutCSraw,"E, CS\n");
		imscs iterI = opiArI.m_mscsPI_Cross_Sections.begin();
		imscs iterJ = iterI;
		iterJ++;
		unsigned int uiMax = iterI->second.size();
		if (iterJ->second.size() > uiMax)
			uiMax = iterJ->second.size();
		imddcs iterII = iterI->second.begin();
		imddcs iterJJ = iterJ->second.begin();
		
 
		for (unsigned int uiI = 0; uiI < uiMax; uiI++)
		{
			if (iterII != iterI->second.end())
			{
				fprintf(fileOutCSraw,"%.8e, %.8e",iterII->first,iterII->second);
				iterII++;
			}
			else
				fprintf(fileOutCSraw," ,  ");
			if (iterJJ != iterJ->second.end())
			{
				iterJJ++;
				fprintf(fileOutCSraw,", %.8e, %.8e",iterJJ->first,iterJJ->second);
			}
			else
				fprintf(fileOutCSraw,",  ,  ");
			fprintf(fileOutCSraw,"\n");
		}
		fclose(fileOutCSraw);
	}
	return 0;
}


