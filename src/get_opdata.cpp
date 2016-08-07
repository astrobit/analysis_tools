#include<cstdio>
#include<cstdlib>
#include<opacity_profile_data.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <sstream>

int main(int i_iArg_Count, char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA	cOpData;
	unsigned int uiError = 0;
	if (i_iArg_Count > 2)
	{
		bool bVel = false;
		bool bDens = false;
		bool bAbund = false;
		bool bScalar = false;
		bool bNormtime = false;
		bool bReftime = false;
		std::string szGroup;
		OPACITY_PROFILE_DATA::GROUP eGroup;
		unsigned int uiModel = -1;

		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			char * lpszCursor = strstr(i_lpszArg_Values[uiI],"--element");
			if (lpszCursor != nullptr)
			{
				lpszCursor+=9; // get to end
				if (lpszCursor[0] == 0 && uiI < (i_iArg_Count - 1))
				{
					uiI++;
					lpszCursor = i_lpszArg_Values[uiI];
				}
				if (lpszCursor[0] == '=')
					lpszCursor++; // bypass '='
				else
				{
					uiError = 2;
					std::cerr << "Element group format invalid" << std::endl;
				}

				if (lpszCursor[0] == 0 && uiI < (i_iArg_Count - 1))
				{
					uiI++;
					lpszCursor = i_lpszArg_Values[uiI];
				}
				if (lpszCursor[0] != 0)
					szGroup = lpszCursor; // bypass '='
				else
				{
					uiError = 2;
					std::cerr << "Element group format invalid" << std::endl;
				}
			}
			else
			{
				lpszCursor = strstr(i_lpszArg_Values[uiI],"--model");
				if (lpszCursor != nullptr)
				{
					lpszCursor+=7; // get to end
					if (lpszCursor[0] == 0 && uiI < (i_iArg_Count - 1))
					{
						uiI++;
						lpszCursor = i_lpszArg_Values[uiI];
					}
					if (lpszCursor[0] == '=')
						lpszCursor++; // bypass '='
					else
					{
						uiError = 6;
						std::cerr << "Model format invalid" << std::endl;
					}

					if (lpszCursor[0] == 0 && uiI < (i_iArg_Count - 1))
					{
						uiI++;
						lpszCursor = i_lpszArg_Values[uiI];
					}
					if (lpszCursor[0] != 0)
						uiModel = atoi(lpszCursor);
					else
					{
						uiError = 6;
						std::cerr << "Model format invalid" << std::endl;
					}
				}
				else if (strcmp(i_lpszArg_Values[uiI],"--velocity") == 0 || strcmp(i_lpszArg_Values[uiI],"-v") == 0)
					bVel = true;
				else if (strcmp(i_lpszArg_Values[uiI],"--density") == 0 || strcmp(i_lpszArg_Values[uiI],"-d") == 0)
					bDens = true;
				else if (strcmp(i_lpszArg_Values[uiI],"--abundance") == 0 || strcmp(i_lpszArg_Values[uiI],"-a") == 0)
					bAbund = true;
				else if (strcmp(i_lpszArg_Values[uiI],"--scalar") == 0 || strcmp(i_lpszArg_Values[uiI],"-s") == 0)
					bScalar = true;
				else if (strcmp(i_lpszArg_Values[uiI],"--normtime") == 0 || strcmp(i_lpszArg_Values[uiI],"-n") == 0)
					bNormtime = true;
				else if (strcmp(i_lpszArg_Values[uiI],"--reftime") == 0 || strcmp(i_lpszArg_Values[uiI],"-r") == 0)
					bReftime = true;
			}
		}
		if (uiError == 0)
		{
			std::string sFilename;
			if (uiModel == -1)
			{
				if (i_lpszArg_Values[1][0] != '-') // make sure that no
					sFilename = i_lpszArg_Values[1];
			}
			else
			{
				const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
				std::ostringstream ossFilepath;
				ossFilepath << lpszLA_Data_Path;
				ossFilepath << "/models/";
				ossFilepath << uiModel << "/opacity_map_scalars.opdata";
				sFilename = ossFilepath.str();

			}
			if (!sFilename.empty())
			{
				std::ifstream ifOpData;
				ifOpData.open(sFilename.c_str());
				if (ifOpData.is_open())
				{
					ifOpData.close();
					cOpData.Load(sFilename.c_str());
				}
				else
				{
					uiError = 8;
					std::cerr << "Could not open " << sFilename << "." << std::endl;
				}
			}
			else
			{
				uiError = 7;
				std::cerr << "No opdata file specified." << std::endl;
			}
		}
		if (uiError == 0)
		{
			if (szGroup.empty())
			{
				uiError = 3;
				std::cerr << "Element group format invalid or not specified" << std::endl;
			}
			if (!bVel && !bDens && !bAbund && !bScalar && !bNormtime && !bReftime)
			{
				uiError = 4;
				std::cerr << "No reports specified" << std::endl;
			}
		}
		if (uiError == 0)
		{
			if (bVel || bDens || bAbund || bScalar || bNormtime)
			{
				if (szGroup == "C")
					eGroup = OPACITY_PROFILE_DATA::CARBON;
				else if (szGroup == "O")
					eGroup = OPACITY_PROFILE_DATA::OXYGEN;
				else if (szGroup == "Mg")
					eGroup = OPACITY_PROFILE_DATA::MAGNESIUM;
				else if (szGroup == "Si")
					eGroup = OPACITY_PROFILE_DATA::SILICON;
				else if (szGroup == "Fe")
					eGroup = OPACITY_PROFILE_DATA::IRON;
				else if (szGroup == "Shell" || szGroup == "shell" || szGroup == "SHELL")
					eGroup = OPACITY_PROFILE_DATA::SHELL;
				else
				{
					uiError = 5;
					std::cerr << "Invalid element group " << szGroup << std::endl;
				}
			}
		}
		if (uiError == 0)
		{
			std::cout << std::scientific;
			std::cout << std::setprecision(5);
			if (bVel)
				std::cout << "Velocity: " << cOpData.Get_Velocity(eGroup) << " cm/s" << std::endl;
			if (bDens)
				std::cout << "Density: " << cOpData.Get_Density(eGroup) << " g/cm^3" << std::endl;
			if (bAbund)
				std::cout << "Abundance: " << cOpData.Get_Abundance(eGroup) << std::endl;
			if (bScalar)
				std::cout << "Scalar: " << cOpData.Get_Scalar(eGroup) << std::endl;
			std::cout << std::fixed;
			std::cout << std::setprecision(4);
			if (bNormtime)
				std::cout << "Norm. Time: " << cOpData.Get_Normalization_Time(eGroup) << " s" << std::endl;
			if (bReftime)
				std::cout << "Ref. Time: " << cOpData.Get_Reference_Time() << " s" << std::endl;
		}
	}
	else
		uiError = 1;

	if (uiError != 0)
	{
		std::cout << "Usage: " << i_lpszArg_Values[0] << " [<opdata file>][--model--element=<elementgroup> [options]" << std::endl;
		std::cout << "opdata file or --model must be specified. If --model is not specified," << std::endl;
		std::cout << "\tthe opdata file must be the first parameter." << std::endl;
		std::cout << "Element groups: C, O, Mg, Si, Fe, shell" << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "\t--velocity, -v: reports reference velocity for element group" << std::endl;
		std::cout << "\t--density, -d : reports reference density for element group" << std::endl;
		std::cout << "\t--abundance, -a: reports reference abundance for element group" << std::endl;
		std::cout << "\t--scalar, -s: reports reference scalar for element group" << std::endl;
		std::cout << "\t--normtime, -n: reports reference normtime for element group" << std::endl;
		std::cout << "\t--reftime, -r: reports reference time" << std::endl;
	}
	return 0;
}
