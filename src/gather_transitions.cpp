#include<cstdio>
#include<cstdlib>
#include <xio.h>
#include <vector>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	XDATASET	cEjecta;
	XDATASET	cShell;
	cEjecta.ReadDataFile("pEW_Ejecta_data.csv", false, false,',', 1);
	cShell.ReadDataFile("pEW_Shell_data.csv", false, false,',', 1);

	if (cEjecta.GetNumRows() == cShell.GetNumRows() &&
		cEjecta.GetNumColumns() == cShell.GetNumColumns())
	{
		std::vector<unsigned int> vTransitions;
		std::vector<unsigned int> vTransitions_Models;
		std::vector<bool> vTransitions_Set;
		vTransitions_Models.resize(cEjecta.GetNumColumns() - 1);
		vTransitions.resize(cEjecta.GetNumColumns() - 1);
		vTransitions_Set.resize(cEjecta.GetNumColumns() - 1);
		for (unsigned int uiJ = 0; uiJ < cEjecta.GetNumColumns(); uiJ++)
			vTransitions_Set[uiJ] = false;
		for (unsigned int uiI = 0; uiI < cEjecta.GetNumRows(); uiI++)
		{
			for (unsigned int uiJ = 1; uiJ < cEjecta.GetNumColumns(); uiJ++)
			{
				if (cEjecta.GetElement(uiJ,uiI) > cShell.GetElement(uiJ,uiI))
				{
					if (!vTransitions_Set[uiJ - 1])
					{
						vTransitions_Set[uiJ - 1] = true;
						vTransitions[uiJ - 1] = cEjecta.GetElement(0,uiI);
					}
				}
			}
		}
		FILE * fileEjecta = fopen("pEW_Ejecta_data.csv","rt");
		if (fileEjecta)
		{
			char lpszBuffer[1024];
			fgets(lpszBuffer,sizeof(lpszBuffer),fileEjecta);
			char * lpszCursor = lpszBuffer;
			unsigned int uiCount = 0;

			while (lpszCursor[0] != 0)
			{
				while (lpszCursor[0] != ',' && lpszCursor[0] != 0)
					lpszCursor++;
				while (lpszCursor[0] != '\t' && lpszCursor[0] != ' ' && lpszCursor[0] != 0)
					lpszCursor++;
				if (lpszCursor[0] != 0)
				{
					vTransitions_Models[uiCount] = atoi(lpszCursor);
					uiCount++;
				}
			}
			fclose(fileEjecta);
		}
		FILE * fileTransitions = fopen("transitions.csv","wt");
		if (fileTransitions)
		{
			for (unsigned int uiJ = 1; uiJ < cEjecta.GetNumColumns(); uiJ++)
				fprintf(fileTransitions,"%i, %i\n",vTransitions_Models[uiJ - 1],vTransitions[uiJ - 1]);
			fclose(fileTransitions);
		}
	}
	return 0;
}
