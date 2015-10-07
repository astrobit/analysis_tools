#include <unistd.h>
#include <wordexp.h>
#include <xstdlib.h>
#include <xio.h>
#include <string>

void Process(const char * i_lpszType, const char ** i_lpszCol_Names)
{
	XDATASET	cFile;
	XDATASET	cCombined;
	wordexp_t cResults;

	char lpszSearchFile[64];
	char lpszOutputFile[64];
	unsigned int uiColors[8][2] =
	{
		{2,4}, // u - v
		{2,3}, // u - b
		{3,4}, // b - v
		{7,5}, // uvm2 - uvw1
		{6,5}, // uvw2 - uvw1
		{7,4}, // uvm2 - v
		{6,4}, // uw2 - v
		{5,4} // uw1 - v
	};


	sprintf(lpszSearchFile,"*%s*photometry.csv",i_lpszType);
	sprintf(lpszOutputFile,"%s_photometry.csv",i_lpszType);

	if (wordexp(lpszSearchFile,&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
	{
		if (cResults.we_wordc > 0)
		{
			printf("Count %i\n",cResults.we_wordc);
			unsigned int uiCount = cResults.we_wordc;
			for (unsigned int uiI = 0; uiI < cResults.we_wordc; uiI++)
				if (strcmp(cResults.we_wordv[uiI],lpszOutputFile) == 0)
					uiCount--;
			if (uiCount > 0)
				cCombined.Allocate(17,uiCount);
			uiCount = 0;
			for (unsigned int uiI = 0; uiI < cResults.we_wordc; uiI++)
			{
				if (strcmp(cResults.we_wordv[uiI],lpszOutputFile) != 0)
				{

					printf("Reading %s\n",cResults.we_wordv[uiI]);
					cFile.ReadDataFile(cResults.we_wordv[uiI],false,false,',',1);
					
					for(unsigned int uiJ = 0; uiJ < 9; uiJ++)
						cCombined.SetElement(uiJ,uiCount,cFile.GetElement(uiJ,0));
					for (unsigned int uiJ = 0; uiJ < 8; uiJ++)
						cCombined.SetElement(9 + uiJ,uiCount,cFile.GetElement(uiColors[uiJ][0],0) - cFile.GetElement(uiColors[uiJ][1],0));
					uiCount++;
				}
			}
			if (uiCount > 0)
				cCombined.SaveDataFileCSV(lpszOutputFile,i_lpszCol_Names);
			wordfree(&cResults);
		}
	}
}

int main(int i_uiArg_Count, const char * i_lpszArg_Values[])
{


	std::string szColumn_Names[17] = 
		{
			std::string("Day"),
			std::string("Model"),
			std::string("Swift u"),
			std::string("Swift b"),
			std::string("Swift v"),
			std::string("Swift uvw1"),
			std::string("Swift uvw2"),
			std::string("Swift uvm2"),
			std::string("Swift white"),
			std::string("Swift u-v"),
			std::string("Swift u-b"),
			std::string("Swift b-v"),
			std::string("Swift uvm2-uvw1"),
			std::string("Swift uvw2-uvw1"),
			std::string("Swift uvm2-v"),
			std::string("Swift uvw2-v"),
			std::string("Swift uvw1-v")
	};
	const char ** lpszColumn_Names = new const char *[17];
	for (unsigned int uiI = 0; uiI < 17; uiI++)
		lpszColumn_Names[uiI] = szColumn_Names[uiI].c_str();

	Process("Solar",lpszColumn_Names);
	Process("Seitenzahl",lpszColumn_Names);
	Process("CO_Rich",lpszColumn_Names);
	Process("Ca_Rich",lpszColumn_Names);

	delete [] lpszColumn_Names;
	return 0;
}
