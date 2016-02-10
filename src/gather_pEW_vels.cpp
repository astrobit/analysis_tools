#include <xstdlib.h>
#include <xio.h>
#include <cstdio>
#include <map>
#include <vector>
#include <xlinalg.h>
#include <line_routines.h>

class	FIT_DATA
{
public:
	double	m_dpEW;
	double	m_dVel;
	
	FIT_DATA(void)
	{
		m_dpEW = -1.0;
		m_dVel = -1.0;
	}
};

class DATA_CONTAINER
{
public:
	unsigned int m_uiModel;
	FIT_DATA	m_cFlat_Raw;
	FIT_DATA	m_cFlat_Ejecta;
	FIT_DATA	m_cFlat_Shell;
	FIT_DATA	m_cJeff_SF;
	FIT_DATA	m_cJeff_DF;
	FIT_DATA	m_cJeff_Combined;
	FIT_DATA	m_cFlat_SF;
	FIT_DATA	m_cFlat_DF;
	FIT_DATA	m_cFlat_Combined;

	DATA_CONTAINER(void) : m_uiModel(0), m_cJeff_SF(), m_cJeff_DF(), m_cFlat_SF(), m_cFlat_DF(), m_cFlat_Raw(), m_cFlat_Ejecta(), m_cFlat_Shell(), m_cJeff_Combined(),  m_cFlat_Combined() {}
};


double Compute_pEW(XVECTOR & i_vA, const double & i_dWL_Min, const double & i_dWL_Max)
{
	double dpEW = 0.0;
	double	ddWL = (i_dWL_Max - i_dWL_Min) / 1024.0;
	for (double dWL = i_dWL_Min; dWL <= i_dWL_Max; dWL += ddWL)
	{
		dpEW += (-Gaussian(dWL,i_vA,&g_cgfpCaNIR).Get(0)) * ddWL;
	}
	return dpEW;
}

enum GROUP {RAW,EJECTA,SHELL,JEFF_FIT,FLAT_FIT,JEFF_COMBINED, FLAT_COMBINED, ALL};
enum COMPONENT {PEW, VELOCITY};
void Write_Datafile(const char * i_lpszFilename, GROUP i_eGroup, COMPONENT i_eComponent, const std::map<unsigned int, std::map<unsigned int, DATA_CONTAINER> > & i_cFull_Map)
{
	FILE * fileFile = fopen(i_lpszFilename,"wt");
	const FIT_DATA * lpCtr = NULL;


	if (i_eGroup != ALL)
	{
		fprintf(fileFile,"Day");
		for (std::map<unsigned int, DATA_CONTAINER>::const_iterator cIterJ = (i_cFull_Map.begin())->second.begin(); cIterJ != (i_cFull_Map.begin())->second.end(); cIterJ++)
		{
			fprintf(fileFile,", %i",cIterJ->first);
		}
		fprintf(fileFile,"\n");
	}
	else
		fprintf(fileFile,"Day, Model, v min (min - combined flat), v min (min - ejecta flat), v min (min - shell flat), v min (Jeff - single), v min (Jeff - double), v min (Flat - single), v min (Flat- double), pEW (combined flat), pEW (ejecta flat), pEW (shell flat), pEW (Jeff - single), pEW (Jeff - double), pEW (Flat - single), pEW (Flat- double)\n");
	for (std::map<unsigned int, std::map<unsigned int, DATA_CONTAINER> >::const_iterator cIterI = i_cFull_Map.begin(); cIterI != i_cFull_Map.end(); cIterI++)
	{
		if (i_eGroup != ALL)
		fprintf(fileFile,"%i",cIterI->first);
		for (std::map<unsigned int, DATA_CONTAINER>::const_iterator cIterJ = cIterI->second.begin(); cIterJ != cIterI->second.end(); cIterJ++)
		{
			if (i_eGroup == ALL)
			{
				fprintf(fileFile,"%i",cIterI->first);
				fprintf(fileFile,", %i",cIterJ->first);
				for (unsigned int uiI = 0; uiI < 2; uiI++)
				{
					for (unsigned int uiJ = 0; uiJ <=6; uiJ++)
					{
						switch (uiJ)
						{
						case 0: lpCtr = &cIterJ->second.m_cFlat_Raw; break;
						case 1: lpCtr = &cIterJ->second.m_cFlat_Ejecta; break;
						case 2: lpCtr = &cIterJ->second.m_cFlat_Shell; break;
						case 3: lpCtr = &cIterJ->second.m_cJeff_SF; break;
						case 4: lpCtr = &cIterJ->second.m_cJeff_DF; break;
						case 5: lpCtr = &cIterJ->second.m_cFlat_SF; break;
						case 6: lpCtr = &cIterJ->second.m_cFlat_DF; break;
						case 7: lpCtr = &cIterJ->second.m_cJeff_Combined; break;
						case 8: lpCtr = &cIterJ->second.m_cFlat_Combined; break;
						}
						if (uiI == 0 && lpCtr->m_dVel < 500. ||
							uiI == 0 && lpCtr->m_dVel > 40000.0 ||
							uiI != 0 && lpCtr->m_dpEW < 0.)
							fprintf(fileFile,", ");
						else if (uiI == 0)
							fprintf(fileFile,", %.0f",lpCtr->m_dVel);
						else
							fprintf(fileFile,", %.1f",lpCtr->m_dpEW);
					}
				}
				fprintf(fileFile,"\n");
			}
			else
			{
				switch (i_eGroup)
				{
				case RAW:
					lpCtr = &cIterJ->second.m_cFlat_Raw;
					break;
				case EJECTA:
					lpCtr = &cIterJ->second.m_cFlat_Ejecta;
					break;
				case SHELL:
					lpCtr = &cIterJ->second.m_cFlat_Shell;
					break;
				case JEFF_FIT:
					lpCtr = &cIterJ->second.m_cJeff_SF;
					break;
				case FLAT_FIT:
					lpCtr = &cIterJ->second.m_cFlat_SF;
					break;
				case JEFF_COMBINED:
					lpCtr = &cIterJ->second.m_cJeff_Combined;
					break;
				case FLAT_COMBINED:
					lpCtr = &cIterJ->second.m_cFlat_Combined;
					break;
				}
				if (i_eComponent == VELOCITY && lpCtr->m_dVel < 500. ||
					i_eComponent == VELOCITY && lpCtr->m_dVel > 40000.0 ||
					i_eComponent != VELOCITY && lpCtr->m_dpEW < 0.)
					fprintf(fileFile,", ");
				else if (i_eComponent == VELOCITY)
					fprintf(fileFile,", %.0f",lpCtr->m_dVel);
				else
					fprintf(fileFile,", %.1f",lpCtr->m_dpEW);
			}
		}
		if (i_eGroup != ALL)
			fprintf(fileFile,"\n");
	}
	fclose(fileFile);
}

void Process_Gaussians(const XDATASET & i_cDatafile, unsigned int i_uiRow, unsigned int i_uiIndices[6], FIT_DATA & o_fdSF, FIT_DATA & o_fdDF, FIT_DATA & o_fdCombined, const double & i_dWL_Ref)
{
	XVECTOR	vA[2];
	if (i_cDatafile.GetElement(i_uiIndices[0],i_uiRow) != -1)
	{
		vA[0] = XVECTOR(3);
		vA[0].Set(0,i_cDatafile.GetElement(i_uiIndices[0],i_uiRow));
		vA[0].Set(1,i_cDatafile.GetElement(i_uiIndices[1],i_uiRow));
		vA[0].Set(2,i_cDatafile.GetElement(i_uiIndices[2],i_uiRow));
	}
	if (i_cDatafile.GetElement(i_uiIndices[3],i_uiRow) != -1)
	{
		vA[1] = XVECTOR(3);
		vA[1].Set(0,i_cDatafile.GetElement(i_uiIndices[3],i_uiRow));
		vA[1].Set(1,i_cDatafile.GetElement(i_uiIndices[4],i_uiRow));
		vA[1].Set(2,i_cDatafile.GetElement(i_uiIndices[5],i_uiRow));
	}
	if (vA[0].Get_Size() == 3)
	{
		if (vA[0].Get_Size() == vA[1].Get_Size())
		{
			if (vA[0].Get(2) < vA[1].Get(2))
			{
				o_fdSF.m_dVel = -Compute_Velocity(vA[1].Get(2), i_dWL_Ref);
				o_fdSF.m_dpEW = Compute_pEW(vA[1],7500.0,9000.0);
				o_fdDF.m_dVel = -Compute_Velocity(vA[0].Get(2), i_dWL_Ref);
				o_fdDF.m_dpEW = Compute_pEW(vA[0],7500.0,9000.0);
			}
			else
			{
				o_fdSF.m_dVel = -Compute_Velocity(vA[0].Get(2), i_dWL_Ref);
				o_fdSF.m_dpEW = Compute_pEW(vA[0],7500.0,9000.0);
				o_fdDF.m_dVel = -Compute_Velocity(vA[1].Get(2), i_dWL_Ref);
				o_fdDF.m_dpEW = Compute_pEW(vA[1],7500.0,9000.0);
			}
			o_fdCombined.m_dpEW = o_fdSF.m_dpEW + o_fdDF.m_dpEW;
		}
		else
		{
			o_fdSF.m_dVel = -Compute_Velocity(vA[0].Get(2), i_dWL_Ref);
			o_fdSF.m_dpEW = Compute_pEW(vA[0],7500.0,9000.0);
			o_fdCombined.m_dpEW = o_fdSF.m_dpEW;
		}
	}
	
	
}
int main(int i_uiArg_Count, const char * i_lpszArg_Values[])
{
	std::map<unsigned int, DATA_CONTAINER>	cMap;
	std::map<unsigned int, std::map<unsigned int, DATA_CONTAINER> > cFull_Map;

	double dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));

	for (unsigned int uiI = 1; uiI < i_uiArg_Count; uiI++)
	{
		XDATASET	cDatafile;
		char * lpszCursor = strstr((char *)i_lpszArg_Values[uiI],".eps.data.csv");
		if (lpszCursor)
		{
			lpszCursor-=3; // back up to get
			if (strncmp(lpszCursor,"NRM",3) == 0)
				lpszCursor -= 4;
			if (lpszCursor[0] == 'd')
			{
				lpszCursor++;
				unsigned int uiDay = atoi(lpszCursor);
				cMap.clear();
				if (cFull_Map.count(uiDay) == 0)
					cFull_Map[uiDay] = cMap;
				printf("Reading %s\n",i_lpszArg_Values[uiI]);
				cDatafile.ReadDataFile(i_lpszArg_Values[uiI],false,false,',',1);
				for (unsigned int uiJ = 0; uiJ < cDatafile.GetNumElements(); uiJ++)
				{
					unsigned int uiModel = cDatafile.GetElement(0,uiJ);	
					printf("Considering data for model %i\n",uiModel);
					if (cFull_Map[uiDay].count(uiModel) == 0) 
					{
						printf("Gathering data for model %i\n",uiModel);
						DATA_CONTAINER cData;
						XVECTOR vA(3);
						cData.m_uiModel = uiModel;
						cData.m_cFlat_Raw.m_dpEW = cDatafile.GetElement(1,uiJ);
						cData.m_cFlat_Raw.m_dVel = cDatafile.GetElement(2,uiJ);
						cData.m_cFlat_Ejecta.m_dpEW = cDatafile.GetElement(3,uiJ);
						cData.m_cFlat_Ejecta.m_dVel = cDatafile.GetElement(4,uiJ);
						cData.m_cFlat_Shell.m_dpEW = cDatafile.GetElement(5,uiJ);
						cData.m_cFlat_Shell.m_dVel = cDatafile.GetElement(6,uiJ);
						unsigned int uiJeff_Fit_Data_Indices[6]= {15,17,19,21,23,25};
						unsigned int uiFlat_Fit_Data_Indices[6]= {27,29,31,33,35,37};
						Process_Gaussians(cDatafile,uiJ,uiJeff_Fit_Data_Indices,cData.m_cJeff_SF,cData.m_cJeff_DF,cData.m_cJeff_Combined,dWL_Ref);
						Process_Gaussians(cDatafile,uiJ,uiFlat_Fit_Data_Indices,cData.m_cFlat_SF,cData.m_cFlat_DF,cData.m_cFlat_Combined,dWL_Ref);
						(cFull_Map[uiDay])[uiModel] = cData;
					}
				}
			}
		}
	}

	for (std::map<unsigned int, std::map<unsigned int, DATA_CONTAINER> >::iterator cIterI = cFull_Map.begin(); cIterI != cFull_Map.end(); cIterI++)
	{
		for (std::map<unsigned int, DATA_CONTAINER>::iterator cIterJ = cIterI->second.begin(); cIterJ != cIterI->second.end(); cIterJ++)
		{
			for (std::map<unsigned int, std::map<unsigned int, DATA_CONTAINER> >::iterator cIterK = cFull_Map.begin(); cIterK != cFull_Map.end(); cIterK++)
			{
				if (cIterK != cIterI)
				{
					if (cIterK->second.count(cIterJ->second.m_uiModel) == 0)
					{
						DATA_CONTAINER cData;
						cData.m_uiModel = cIterJ->second.m_uiModel;
						(cFull_Map[cIterK->first])[cData.m_uiModel] = cData;
					}
				}
			}
		}
	}


	Write_Datafile("vel_pEW_data.csv",ALL,VELOCITY,cFull_Map);

	Write_Datafile("vel_Jeff_data.csv",JEFF_FIT,VELOCITY,cFull_Map);
	Write_Datafile("vel_Flat_data.csv",FLAT_FIT,VELOCITY,cFull_Map);
	Write_Datafile("vel_Raw_data.csv",RAW,VELOCITY,cFull_Map);
	Write_Datafile("vel_Ejecta_data.csv",EJECTA,VELOCITY,cFull_Map);
	Write_Datafile("vel_Shell_data.csv",SHELL,VELOCITY,cFull_Map);

	Write_Datafile("pEW_Jeff_data.csv",JEFF_FIT,PEW,cFull_Map);
	Write_Datafile("pEW_Jeff_Combined_data.csv",JEFF_COMBINED,PEW,cFull_Map);
	Write_Datafile("pEW_Flat_data.csv",FLAT_FIT,PEW,cFull_Map);
	Write_Datafile("pEW_Flat_Combined_data.csv",FLAT_COMBINED,PEW,cFull_Map);
	Write_Datafile("pEW_Raw_data.csv",RAW,PEW,cFull_Map);
	Write_Datafile("pEW_Ejecta_data.csv",EJECTA,PEW,cFull_Map);
	Write_Datafile("pEW_Shell_data.csv",SHELL,PEW,cFull_Map);


	return 0;
}
