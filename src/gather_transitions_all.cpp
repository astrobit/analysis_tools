#include<cstdio>
#include<cstdlib>
#include <dirent.h>
#include <cstring>
#include <sstream>
#include <map>
#include <string>
#include <xio.h>
#include <vector>

typedef std::map< unsigned int, unsigned int> mapmodtrs;
typedef std::map <std::string, mapmodtrs > maptrs;

void ReadDirRec(std::ostringstream &io_OssdirName, DIR * i_dirDir, maptrs &io_mapData)
{
	dirent * direntData;
	while ((direntData = readdir(i_dirDir)) != nullptr)
	{
		DIR * dirCurr;
		if (strcmp(direntData->d_name,"..") != 0 && strcmp(direntData->d_name,".") != 0)
		{
			std::ostringstream ossCurr;
			ossCurr << io_OssdirName.str() << "/" << direntData->d_name;
			dirCurr = opendir(ossCurr.str().c_str());
			if (dirCurr != nullptr)
			{
				printf("%s\n",ossCurr.str().c_str());
				ReadDirRec(ossCurr,dirCurr,io_mapData);

				ossCurr << "/transitions.csv";
				FILE * fileTransitions;
				fileTransitions = fopen(ossCurr.str().c_str(),"rt");
				if (fileTransitions != nullptr)
				{
					fclose(fileTransitions);
					XDATASET dsTransitions;
					dsTransitions.ReadDataFile(ossCurr.str().c_str(), false, false,',', 1);
					for (unsigned int uiI = 0; uiI < dsTransitions.GetNumRows(); uiI++)
					{
						io_mapData[ossCurr.str()][dsTransitions.GetElement(0,uiI)] = dsTransitions.GetElement(1,uiI);
					} 
				}
				closedir(dirCurr);
			}
		}
	}
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	DIR * dirTop;
	dirTop = opendir(".");
	std::ostringstream ossTop;
	ossTop << ".";
	maptrs mapData;

	ReadDirRec(ossTop,dirTop,mapData);
	closedir(dirTop);

	FILE * fileTrsAll = fopen("transitions_all.csv","wt");
	if (fileTrsAll != nullptr)
	{
		std::vector<unsigned int> vModel_List;
		fprintf(fileTrsAll,"Path, ");
		for (maptrs::iterator iterI = mapData.begin(); iterI != mapData.end(); iterI++)
		{
			
			for (mapmodtrs::iterator iterJ = iterI->second.begin(); iterJ != iterI->second.end(); iterJ++)
			{
				bool bFound = false;
				for (unsigned int uiK = 0; uiK < vModel_List.size() && !bFound; uiK++)
				{
					if (vModel_List[uiK] == iterJ->first)
						bFound = true;
				}
				if (!bFound)
				{
					vModel_List.push_back(iterJ->first);
					fprintf(fileTrsAll,", %i",iterJ->first);
				}
			}
		}
		fprintf(fileTrsAll,"\n");

		for (maptrs::iterator iterI = mapData.begin(); iterI != mapData.end(); iterI++)
		{
			std::vector<unsigned int> vTransitions;
			vTransitions.resize(vModel_List.size());
			for (unsigned int uiK = 0; uiK < vTransitions.size(); uiK++)
				vTransitions[uiK] = 0;
			for (mapmodtrs::iterator iterJ = iterI->second.begin(); iterJ != iterI->second.end(); iterJ++)
			{
				bool bFound = false;
				for (unsigned int uiK = 0; uiK < vModel_List.size() && !bFound; uiK++)
				{
					if (vModel_List[uiK] == iterJ->first)
					{
						bFound = true;
						vTransitions[uiK] = iterJ->second;
					}
				}
			}

			fprintf(fileTrsAll,"%s",iterI->first.c_str());
			for (std::vector<unsigned int>::iterator iterJ = vTransitions.begin(); iterJ != vTransitions.end(); iterJ++)
			{
				fprintf(fileTrsAll,", %i",*iterJ);
			}
			fprintf(fileTrsAll,"\n");
		}
		fclose(fileTrsAll);
	}
	else
		fprintf(stderr,"Unable to open transitions_all.csv\n");
	return 0;
}
