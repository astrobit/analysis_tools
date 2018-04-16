#include<cstdio>
#include<cstdlib>
#include <OSC.hpp>
#include <xstdlib.h>
#include <iostream>
#include <sstream>

std::string filenamize(const std::string & i_sIn)
{
	std::string sOut;
	for (auto iterJ = i_sIn.begin(); iterJ != i_sIn.end(); iterJ++)
	{
		if (*iterJ == ' ')
			sOut.push_back('_');
		else if ((*iterJ >= 'a' && *iterJ <= 'z') ||
					(*iterJ >= 'A' && *iterJ <= 'Z') ||
					(*iterJ >= '0' && *iterJ <= '9'))
			sOut.push_back(*iterJ);
	}
	return sOut;
}
std::string filenamize(const double &i_dValue, const char* i_lpszFormat_Specifier="%.17f")
{
	char lpszValue[32];
	sprintf(lpszValue,i_lpszFormat_Specifier,i_dValue);
	std::string sOut;
	size_t tI = 0;
	while (lpszValue[tI] != 0)
	{
		if (lpszValue[tI] == ' ')
			sOut.push_back('_');
		else if ((lpszValue[tI] >= 'a' && lpszValue[tI] <= 'z') ||
					(lpszValue[tI] >= 'A' && lpszValue[tI] <= 'Z') ||
					(lpszValue[tI] >= '0' && lpszValue[tI] <= '9'))
			sOut.push_back(lpszValue[tI]);
		else if (lpszValue[tI] == '-')
			sOut.push_back('m');
		else if (lpszValue[tI] == '+')
			sOut.push_back('p');
		else if (lpszValue[tI] == '.')
			sOut.push_back('p');

		tI++;
	}
	return sOut;
}

std::vector<std::string> get_OSC_references(const std::string & i_szSource_List, const OSCfile & i_cOSC_File)
{
	std::vector <size_t> vRefs;
	size_t tPos = 0;
	std::string sSourcelist = i_szSource_List;
	while (tPos != std::string::npos && tPos < sSourcelist.size() && (sSourcelist[tPos] <= '0' || sSourcelist[tPos] >= '9'))
		tPos++;

	while (tPos != std::string::npos && tPos < sSourcelist.size() && (sSourcelist[tPos] >= '0' && sSourcelist[tPos] <= '9'))
	{
		vRefs.push_back(std::stoi(sSourcelist.substr(tPos,sSourcelist.size() - tPos)));
		tPos = sSourcelist.find(',',tPos);
		if (tPos != std::string::npos)
		{
			while (tPos != std::string::npos && tPos < sSourcelist.size() && (sSourcelist[tPos] <= '0' || sSourcelist[tPos] >= '9'))
				tPos++;
			if (tPos >= sSourcelist.size())
				tPos = std::string::npos;
		}
	}
	std::vector<std::string> vReferences;

	for (auto iterI = vRefs.begin(); iterI != vRefs.end(); iterI++)
	{
		vReferences.push_back(i_cOSC_File.m_vsReference_List[*iterI - 1].m_szReference);
	}
	return vReferences;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	if (i_iArg_Count > 1)
	{
		xstdlib::command_line_parse cCommand_Line(i_iArg_Count,i_lpszArg_Values);
		std::string sOSC_File;
		size_t tIdx = -1;
		std::vector<std::string> vKeys = cCommand_Line.Get_All_Keys();
		for (auto iterI = vKeys.begin(); iterI != vKeys.end(); iterI++)
		{
			if (iterI->at(0) != '-')
				sOSC_File = *iterI;
			else
			{
				tIdx = std::stoi(iterI->substr(1,iterI->size() - 1));
			}
		}
		OSCfile cFile;
		cFile.Load(sOSC_File);

		size_t tOut_Idx = 1;
		std::map<size_t, OSCspectra_id> mapIdx;

		for (auto iterI = cFile.m_mSpectra_List.begin(); iterI != cFile.m_mSpectra_List.end(); iterI++)
		{
			mapIdx[tOut_Idx] = iterI->first;
			tOut_Idx++;
		}

		if (tIdx == -1 || tIdx > cFile.m_mSpectra_List.size())
		{
			std::cout << "Available spectra:" << std::endl;
			for (auto iterI = mapIdx.begin(); iterI != mapIdx.end(); iterI++)
			{
				std::vector<std::string> vReferences = get_OSC_references(iterI->second.m_szSources,cFile);
				
				std::cout << iterI->first << " | |" << iterI->second.m_dDate_MJD << " | " << iterI->second.m_szInstrument;
				for (auto iterJ = vReferences.begin(); iterJ != vReferences.end(); iterJ++)
					std::cout << " | " << *iterJ;
				std::cout << std::endl;
			}
		}
		else
		{
//			cFile.Unredshift();
//			cFile.Deredden();

			std::cout << "EBV: " << cFile.m_dEBV_Mean << std::endl;
			std::cout << "Redshift: " << cFile.m_dRedshift_Weighted_Mean << std::endl;

			std::cout << "Index " << tIdx << std::endl;
			OSCspectrum cSpectrum = cFile.m_mSpectra_List[mapIdx[tIdx]];
			cSpectrum.Deredden(cFile.m_dEBV_Mean);
			cSpectrum.Unredshift(cFile.m_dRedshift_Weighted_Mean);

			std::vector<std::string> vReferences = get_OSC_references(mapIdx[tIdx].m_szSources,cFile);

			std::ostringstream ossFilename;
			ossFilename << filenamize(mapIdx[tIdx].m_dDate_MJD,"%.1f") << "_" << filenamize(mapIdx[tIdx].m_szInstrument);
			for (auto iterI = vReferences.begin(); iterI != vReferences.end(); iterI++)
			{
				ossFilename << "_" << filenamize(*iterI);
			}
			ossFilename << ".dat";



			FILE* fileOut=fopen(ossFilename.str().c_str(),"wt");
			if (fileOut != nullptr)
			{
				std::cout << "Min: " << cSpectrum.front().m_dWavelength << std::endl;
				std::cout << "Max: " << cSpectrum.back().m_dWavelength << std::endl;

				for (auto iterI = cSpectrum.begin(); iterI != cSpectrum.end(); iterI++)
				{
					fprintf(fileOut,"%.17f\t%.17e\n",iterI->m_dWavelength,iterI->m_dFlux);
				}
				fclose(fileOut);
			}
		}

		

	}
	return 0;
}
