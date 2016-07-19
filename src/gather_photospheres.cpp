#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <cmath>
#include <set>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	#define NUM_TABLES 28
	FILE * fileIonTable[NUM_TABLES][2];
	char	lpszFilename[128];
	bool bFault = false;
	std::set<unsigned int> setModel_List;
	
	setModel_List.insert(10);
	setModel_List.insert(17);
	setModel_List.insert(18);
	setModel_List.insert(41);
	setModel_List.insert(45);
	setModel_List.insert(49);
	setModel_List.insert(53);
	setModel_List.insert(54);
	setModel_List.insert(55);
	setModel_List.insert(56);
	setModel_List.insert(57);
	setModel_List.insert(61);
	setModel_List.insert(62);
	setModel_List.insert(63);
	setModel_List.insert(64);
	setModel_List.insert(65);
	setModel_List.insert(66);
	setModel_List.insert(67);
	setModel_List.insert(68);
	setModel_List.insert(69);
	setModel_List.insert(70);
	setModel_List.insert(71);
	setModel_List.insert(72);
	setModel_List.insert(73);
	setModel_List.insert(74);
	//setModel_List.insert(75); removed due to not being in free expansion
	setModel_List.insert(80);
	setModel_List.insert(81);

	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		fileIonTable[uiI][0] = NULL;
		fileIonTable[uiI][1] = NULL;
	}
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		for (unsigned int uiJ = 0; uiJ < 2; uiJ++)
		{
			sprintf(lpszFilename,"psdata/ps_ion_%i_table_%i.tex",uiI+1,uiJ);
			fileIonTable[uiI][uiJ] = fopen(lpszFilename,"wt");
			if (!fileIonTable[uiI][uiJ])
			{
				fprintf(stderr,"Fault opening output file %s.\n",lpszFilename);
				bFault = true;
			}
			else
			{
				fprintf(fileIonTable[uiI][uiJ],"& \\multicolumn{12}{c}{Day after explosion} \\\\\n");
				fprintf(fileIonTable[uiI][uiJ],"\\hline\n");
				fprintf(fileIonTable[uiI][uiJ],"Model");
				for (unsigned int uiD = uiJ * 12 + 1; uiD < ((uiJ + 1) * 12 + 1); uiD++)
					fprintf(fileIonTable[uiI][uiJ]," & %i",uiD);
				fprintf(fileIonTable[uiI][uiJ]," \\\\\n");
			}
		}
	}
	if (!bFault)
	{
		for (unsigned int uiI = 1; uiI < 128; uiI++)
		{
			if (setModel_List.count(uiI) > 0)
				{
				sprintf(lpszFilename,"run%i/photosphere.csv",uiI);
				FILE * fileTest = fopen(lpszFilename,"rt");
				if (fileTest)
				{
					char chTest;
					size_t uiData = 0;
					uiData = fread(&chTest,1,1,fileTest);
					fclose(fileTest);
					if (uiData > 0)			
					{
						XDATASET	xData;
						printf("Processing %s\n",lpszFilename);
						xData.ReadDataFile(lpszFilename,false,false,',',1);
						if (xData.GetNumRows() > 0)
						{
							for (unsigned int uiJ = 0; uiJ < NUM_TABLES; uiJ++)
							{
								for (unsigned int uiK = 0; uiK < 2; uiK++)
								{
									fprintf(fileIonTable[uiJ][uiK],"%i",uiI);
									for (unsigned int uiD = uiK * 12 + 1; uiD < ((uiK + 1) * 12 + 1); uiD++)
									{
										unsigned int uiCol = uiJ + 2;
										unsigned int uiRow = (uiD - 1) * 4 + 3;
										double dV = xData.GetElement(uiCol,uiRow) * 1e-8;
										if (!std::isnan(dV) && !std::isinf(dV) && !xData.IsElementEmpty(uiCol,uiRow))
											fprintf(fileIonTable[uiJ][uiK]," & %.1f",dV);
										else
											fprintf(fileIonTable[uiJ][uiK]," & \\ldots");
									}
									fprintf(fileIonTable[uiJ][uiK]," \\\\\n");
								}
							}
						}
					}
					//else
						//fprintf(stderr,"Error opening %s\n",lpszFilename);
				}
			}
		}
	}
	for (unsigned int uiI =0; uiI < NUM_TABLES; uiI++)
	{
		fprintf(fileIonTable[uiI][0],"\\hline\n");
		if (fileIonTable[uiI][0])
			fclose(fileIonTable[uiI][0]);
		fprintf(fileIonTable[uiI][1],"\\hline\n");
		if (fileIonTable[uiI][1])
			fclose(fileIonTable[uiI][1]);
	}
	
	return 0;
}
