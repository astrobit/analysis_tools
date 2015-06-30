#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <string>
#include <ostream>
#include <iostream>
#include <list>
#include <xio.h>
#include <xstdlib.h>

class COMPONENT_DATA
{
public:
	std::string 	m_cszComponent;
	unsigned int	m_uiComponent_Counter;
	double	*		m_dMasses;
};

bool Component_Data_Compare (const COMPONENT_DATA * i_lpLeft, const COMPONENT_DATA * i_lpRight)
{
	return (i_lpLeft->m_cszComponent < i_lpRight->m_cszComponent || (i_lpLeft->m_cszComponent == i_lpRight->m_cszComponent && i_lpLeft->m_uiComponent_Counter < i_lpRight->m_uiComponent_Counter));
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	if (xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "-help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "--help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "-h") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "--h") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "?") ||
		i_iArg_Count == 1)
	{
		std::cout << "rlamc: utility for combining mass results from reverse." << std::endl;
		std::cout << "Usage: rlamc [FILE]" << std::endl;
	}
	else
	{
		unsigned int uiI, uiJ;
		const char ** lpszFilenames = NULL;
		unsigned int uiFile_Count = 0;
		std::cout << i_iArg_Count << std::endl;
		if (i_iArg_Count == 2) // user has used a wildcard to specify the files
		{
			wordexp_t sResults;
			wordexp(i_lpszArg_Values[1],&sResults,0);
			uiFile_Count = sResults.we_wordc;


			if (sResults.we_wordc > 0)
			{
				lpszFilenames = new const char *[uiFile_Count];
				for (uiI = 0; uiI < sResults.we_wordc; uiI++)
				{
					lpszFilenames[uiI] = sResults.we_wordv[uiI];
				}
			}
		}
		else if (i_iArg_Count > 2) // user has explicitly specified the files
		{
			uiFile_Count = i_iArg_Count - 1;
			lpszFilenames = new const char *[uiFile_Count];
			for (uiI = 0; uiI < uiFile_Count; uiI++)
			{
				printf("%s\n",i_lpszArg_Values[uiI + 1]);
				lpszFilenames[uiI] = i_lpszArg_Values[uiI + 1];
			}
		}
		if (uiFile_Count > 0)
		{
			XDATASET_ADVANCED *	lpcExisting_Files;
			XDATASET_ADVANCED::DATATYPE eColumn_Descriptors[2] = {XDATASET_ADVANCED::XDT_STRING,XDATASET_ADVANCED::XDT_DOUBLE};
			unsigned int uiMax_Rows = 0;
			bool bError = false;

			lpcExisting_Files = new XDATASET_ADVANCED[uiFile_Count];
			for (uiI = 0; uiI < uiFile_Count; uiI++)
			{
				std::cout << "Loading " << lpszFilenames[uiI] << std::endl;
				lpcExisting_Files[uiI].DescribeRecord(eColumn_Descriptors,2);
				lpcExisting_Files[uiI].ReadDataFile(lpszFilenames[uiI],false,',');
				if (lpcExisting_Files[uiI].GetNumRows() > uiMax_Rows)
					uiMax_Rows = lpcExisting_Files[uiI].GetNumRows();
				if (lpcExisting_Files[uiI].GetNumRows() == 0)
				{
					std::cerr << "rlamc: Failed to load file " << lpszFilenames[uiI] << std::endl;
					bError = true;
				}
			}

			std::list<COMPONENT_DATA *> cComponent_Data_List;

			if (!bError)
			{
				// Create the data grid:
				for (uiI = 0; uiI < uiFile_Count; uiI++)
				{
					unsigned int uiNum_Rows_Lcl = lpcExisting_Files[uiI].GetNumRows();
					for (uiJ = 0; uiJ < uiNum_Rows_Lcl; uiJ++)
					{
						const char * lpszCurr_Component = lpcExisting_Files[uiI].GetElementString(0, uiJ);
						unsigned int uiComponent_Counter = 0;
						bool bFound = false;
						COMPONENT_DATA * lpComponent_Data_Curr = NULL;
						for (std::list<COMPONENT_DATA *>::iterator itIter = cComponent_Data_List.begin(); itIter != cComponent_Data_List.end() && !bFound; itIter++)
						{
							lpComponent_Data_Curr = *itIter;
							
							if (lpComponent_Data_Curr[0].m_cszComponent == lpszCurr_Component && lpComponent_Data_Curr[0].m_dMasses[uiI] == 0.0)
								bFound = true;
							if (lpComponent_Data_Curr[0].m_cszComponent == lpszCurr_Component && !bFound)
								uiComponent_Counter++;
							
						}
						if (!bFound || cComponent_Data_List.empty())
						{
							lpComponent_Data_Curr = new COMPONENT_DATA;
							lpComponent_Data_Curr[0].m_cszComponent = lpszCurr_Component;
							lpComponent_Data_Curr[0].m_uiComponent_Counter = uiComponent_Counter;
							lpComponent_Data_Curr[0].m_dMasses = new double[uiFile_Count];
							memset(lpComponent_Data_Curr[0].m_dMasses,0.0,sizeof(double) * uiFile_Count);
							cComponent_Data_List.push_back(lpComponent_Data_Curr);
						}

						lpComponent_Data_Curr[0].m_dMasses[uiI] = lpcExisting_Files[uiI].GetElementDbl(1, uiJ);
						 						
					}
				}
				// Sort the list by component
				cComponent_Data_List.sort(Component_Data_Compare);
				// Parse the filenames to get the dates
				int * lpiDay_Number = new int[uiFile_Count];
				memset(lpiDay_Number,0,sizeof(int) * uiFile_Count);
				for (uiI = 0; uiI < uiFile_Count; uiI++)
				{
					const char * lpszCursor = lpszFilenames[uiI];
					while (lpszCursor[0] != 0 && strcmp(lpszCursor,".csv") != 0)
						lpszCursor++;
					if (lpszCursor[0] != 0)
					{
						while (lpszCursor >= lpszFilenames[uiI] && strncmp(lpszCursor,".d",2) != 0)
						{
							lpszCursor--;
//							std::cout << lpszCursor << std::endl;
						}
						if (strncmp(lpszCursor,".d",2) == 0)
						{
							lpszCursor += 2;
							lpiDay_Number[uiI] = atoi(lpszCursor);
						}
					}
					if (lpiDay_Number[uiI] == 0)
					{
						std::cerr << "rlamc: Failed to parse day number in filename " << lpszFilenames[uiI] << std::endl;
						bError = true;
					}
				}

					
				// # of files = # of rows in the resulting database
				if (!bError)
				{
					FILE * fileOut = fopen("rlamc.csv","wt");
					fprintf(fileOut,"Day Number");
					for (std::list<COMPONENT_DATA *>::iterator itIter = cComponent_Data_List.begin(); itIter != cComponent_Data_List.end(); itIter++)
					{
						COMPONENT_DATA * lpCurr = *itIter;
						fprintf(fileOut,", %s",lpCurr->m_cszComponent.c_str());
						switch(lpCurr->m_uiComponent_Counter)
						{
						case 0:
							fprintf(fileOut," (PS)");
							break;
						default:
							fprintf(fileOut," (HV%i)",lpCurr->m_uiComponent_Counter);
							break;
						}
					}
					fprintf(fileOut,"\n");
					for (uiI = 0; uiI < uiFile_Count; uiI++)
					{
						fprintf(fileOut,"%i",lpiDay_Number[uiI] - 20);
						for (std::list<COMPONENT_DATA *>::iterator itIter = cComponent_Data_List.begin(); itIter != cComponent_Data_List.end(); itIter++)
						{
							COMPONENT_DATA * lpCurr = *itIter;
							fprintf(fileOut,", %.17e",lpCurr->m_dMasses[uiI]);
						}
						fprintf(fileOut,"\n");
					}
					fclose(fileOut);
				}

			}
		}
	}


	
	return 0;
}

