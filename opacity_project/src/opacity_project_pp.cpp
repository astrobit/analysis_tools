#include <opacity_project_pp.h>

msmst opacity_project_ion::Read_Transition_Data(void)
{
	msmst msmstData;
	if (m_uiZ != -1 && m_uiN != -1)
	{
		char lpszFilename[256];
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
		// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
		const char lpszData_Dir[] = {DATADIR};
		if (lpszLA_Data_Path == nullptr)
		{
			lpszLA_Data_Path = lpszData_Dir;
		}
#endif
		if (lpszLA_Data_Path == nullptr)
			std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
		else 
		{
			sprintf(lpszFilename,"%s/f%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);
			FILE * fileEdata = fopen(lpszFilename,"rt");

			if (fileEdata)
			{
				unsigned int uiS1,uiS2,uiL1,uiL2,uiP1,uiP2;
				opacity_project_state sstateI;
				opacity_project_state sstateJ;
				char lpszData[64];
				char * lpszCursor = nullptr;

				fgets(lpszData,64,fileEdata); // general ion information that we don't care about right now

				do
				{
					fgets(lpszData,64,fileEdata); //S, L, P, and count info
					lpszCursor = NextNum(lpszData);
					uiS1 = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					uiL1 = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					uiP1 = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					uiS2 = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					uiL2 = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					uiP2 = atoi(lpszCursor);

					lpszCursor = NextNum(lpszCursor);
					unsigned int uiopacity_project_state_Count = atoi(lpszCursor);

					if (uiS1 != 0 || uiL1 != 0 || uiP1 != 0 || uiS2 != 0 || uiL2 != 0 || uiP2 != 0)
					{
						mst mstTransitions;
						for (unsigned int uiI = 0; uiI < uiopacity_project_state_Count; uiI++)
						{
							opacity_project_transition trxData;

							fgets(lpszData,64,fileEdata); // individual opacity_project_state info
							lpszCursor = NextNum(lpszData);
							unsigned int uiLvl1 = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							unsigned int uiLvl2 = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							trxData.m_dOscillator_Strength = atof(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							trxData.m_dWeighted_Oscillator_Strength = atof(lpszCursor);
			
							opacity_project_state stateLower;
							opacity_project_state stateUpper;

							if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
							{
								sstateI = Find_State(uiS1,uiL1,uiP1,uiLvl1);
								sstateJ = Find_State(uiS2,uiL2,uiP2,uiLvl2);
							}
							else
							{
								sstateI = Find_State(uiS2,uiL2,uiP2,uiLvl2);
								sstateJ = Find_State(uiS1,uiL1,uiP1,uiLvl1);
							}
							if (sstateI.m_dEnergy_eV < sstateJ.m_dEnergy_eV)
							{
								stateLower = sstateI;
								stateUpper = sstateJ;
							}
							else
							{
								stateLower = sstateJ;
								stateUpper = sstateI;
							}


							trxData.Calculate_Derived_Quantities(stateLower,stateUpper);



							if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
								mstTransitions[sstateJ.m_opldDescriptor] = trxData;
							else
								mstTransitions[sstateI.m_opldDescriptor] = trxData;
						}
						if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
							msmstData[sstateJ.m_opldDescriptor] = mstTransitions;
						else
							msmstData[sstateI.m_opldDescriptor] = mstTransitions;
					}
				} while (uiS1 != 0 || uiL1 != 0 || uiP1 != 0 || uiS2 != 0 || uiL2 != 0 || uiP2 != 0);
				fclose(fileEdata);
			}
			else
				std::cerr << "Couldn't open " << lpszFilename << std::endl;
		}
	}
	return msmstData;
}


ms opacity_project_ion::Read_State_Data(void)
{
	ms msData;
	if (m_uiZ != -1 && m_uiN != -1)
	{
		char lpszFilename[256];
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
		// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
		const char lpszData_Dir[] = {DATADIR};
		if (lpszLA_Data_Path == nullptr)
		{
			lpszLA_Data_Path = lpszData_Dir;
		}
#endif
		if (lpszLA_Data_Path == nullptr)
			std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
		else
		{
			sprintf(lpszFilename,"%s/e%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);
			FILE * fileEdata = fopen(lpszFilename,"rt");

			if (fileEdata)
			{
				opacity_project_state sState;
				char lpszData[64];
				char * lpszCursor = nullptr;

				fgets(lpszData,64,fileEdata); // general ion information that we don't care about right now

				do
				{
					sState.m_opldDescriptor.m_uiN = m_uiN;
					sState.m_opldDescriptor.m_uiZ = m_uiZ;
					fgets(lpszData,64,fileEdata); //S, L, P, and count info
					lpszCursor = NextNum(lpszData);
					sState.m_opldDescriptor.m_uiS = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					sState.m_opldDescriptor.m_uiL = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					sState.m_opldDescriptor.m_uiP = atoi(lpszCursor);

					lpszCursor = NextNum(lpszCursor);
					unsigned int uiopacity_project_state_Count = atoi(lpszCursor);


					fgets(lpszData,64,fileEdata); //don't know what this data is

					for (unsigned int uiI = 0; uiI < uiopacity_project_state_Count; uiI++)
					{
						fgets(lpszData,64,fileEdata); // individual opacity_project_state info
						lpszCursor = NextNum(lpszData);
						sState.m_opldDescriptor.m_uiLvl_ID = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_chType = lpszCursor[0];
						lpszCursor = NextNum(lpszCursor);
						sState.m_uiType_Index = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_ui_n = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_ui_l = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_dEnergy_Ry = atof(lpszCursor);
						sState.m_dEnergy_eV = sState.m_dEnergy_Ry * g_XASTRO.k_dRy_eV;

						sState.m_uiStatistical_Weight = sState.m_opldDescriptor.m_uiL + sState.m_opldDescriptor.m_uiS;

						msData[sState.m_opldDescriptor] = sState;
					}
				} while (sState.m_opldDescriptor.m_uiS != 0 || sState.m_opldDescriptor.m_uiL != 0 || sState.m_opldDescriptor.m_uiP != 0);
				fclose(fileEdata);
			}
			else
				std::cerr << "Couldn't open " << lpszFilename << std::endl;
		}
	}
	return msData;
}

mscs opacity_project_ion::Read_Opacity_PI_Data(void)
{
	mscs mscsData;
	char lpszFilename[256];
	if (m_uiZ != -1 && m_uiN != -1)
	{
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
		// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
		const char lpszData_Dir[] = {DATADIR};
		if (lpszLA_Data_Path == nullptr)
		{
			lpszLA_Data_Path = lpszData_Dir;
		}
#endif
		if (lpszLA_Data_Path == nullptr)
			std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
		else
		{
			sprintf(lpszFilename,"%s/p%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);

			FILE * filePIdata = fopen(lpszFilename,"rt");
			if (filePIdata)
			{
				char lpszData[64];
	


				fgets(lpszData,64,filePIdata); // general ion information that we don't care about right now
				char * lpszCursor = NextNum(lpszData);
				unsigned int uiopacity_project_state_Count = atoi(lpszCursor);
				//std::cout << uiopacity_project_state_Count << std::endl;

				for (unsigned int uiI = 0; uiI < uiopacity_project_state_Count; uiI++)
				{
					fgets(lpszData,64,filePIdata);
					lpszCursor = NextNum(lpszData);
					unsigned int uiS = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					unsigned int uiL = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					unsigned int uiP = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					unsigned int uiID = atoi(lpszCursor);

					opacity_project_level_descriptor opldState(m_uiZ,m_uiN,uiS,uiL,uiP,uiID);

					fgets(lpszData,64,filePIdata);
					lpszCursor = NextNum(lpszData);
					unsigned int uiEner_Count = atoi(lpszCursor);
					lpszCursor = NextNum(lpszCursor);
					unsigned int uiCS_Count = atoi(lpszCursor);


					mddcs mddcsState_Data;
					fgets(lpszData,64,filePIdata); // first one seems to be garbage?
					lpszCursor = NextNum(lpszData);
					double dEnergy = atof(lpszCursor);// * g_XASTRO.k_dRy_eV; // Rydberg


					//std::cout << uiS << " " << uiL << " " << uiP << " " << uiID << " " << dEnergy << " " << uiCS_Count << std::endl;
		
		//			if (uiEner_Count != uiCS_Count)
		//				std::cerr << "Something fishy at " << uiopacity_project_state_S << " " << uiopacity_project_state_L << " " << uiopacity_project_state_P << " " << uiopacity_project_state_ID  << std::endl;

					for (unsigned int uiJ = 0; uiJ < uiCS_Count; uiJ++)
					{
						fgets(lpszData,64,filePIdata);
						lpszCursor = NextNum(lpszData);
						double dEnergy = atof(lpszCursor);// * g_XASTRO.k_dRy_eV; // Rydberg
						lpszCursor = NextNum(lpszCursor);
						double dCross_Section = atof(lpszCursor);

						double dOscillator_Strength = g_XASTRO.k_dme * g_XASTRO.k_dc / (2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de) * dCross_Section;
						double dEnergy_f = dEnergy * dOscillator_Strength;
						//std::cout << dEnergy << " " << dOscillator_Strength << " " << dEnergy_f << std::endl;

						mddcsState_Data[dEnergy] = dCross_Section;
					}

					mscsData[opldState] = mddcsState_Data;
				}
				fclose(filePIdata);
			}
			else
				std::cerr << "Couldn't open " << lpszFilename << std::endl;
		}
	}
	return mscsData;
}
void opacity_project_ion::Read_Configuration_Data(void)
{
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
		// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
	const char lpszData_Dir[] = {DATADIR};
	if (lpszLA_Data_Path == nullptr)
	{
		lpszLA_Data_Path = lpszData_Dir;
	}
#endif
	if (lpszLA_Data_Path == nullptr)
		std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
	else
	{
		char lpszFilename[256];
		sprintf(lpszFilename,"%s/c%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);

		FILE * fileConfigData = fopen(lpszFilename,"rt");
		if (fileConfigData)
		{
			char lpszBuffer[256];

			fgets(lpszBuffer,sizeof(lpszBuffer),fileConfigData);
			while (feof(fileConfigData) == 0)
			{
				configuration_data cConfig;

				char * lpszCursor = lpszBuffer;

				cConfig.m_chType = lpszCursor[0];
				lpszCursor += 5;
				if (lpszCursor[0] == ' ')
					lpszCursor++;
				cConfig.m_uiIndex = atoi(lpszCursor);
				lpszCursor++;
				if (cConfig.m_uiIndex > 9)
					lpszCursor++;
				lpszCursor+=4;
				char lpszopacity_project_state[64];
				char * lpszopacity_project_state_Cursor = lpszopacity_project_state;
				while (lpszCursor != nullptr && lpszCursor[0] != 0 && lpszCursor[0] != 'X')
				{
					lpszopacity_project_state_Cursor[0] = lpszCursor[0];
					lpszopacity_project_state_Cursor++;
					lpszCursor++;
				}
				lpszopacity_project_state_Cursor[0] = 0;
				cConfig.m_szState = lpszopacity_project_state;


				if (cConfig.m_chType == 'T')
				{
					lpszCursor += 3;
					if (lpszCursor[0] == ' ')
						lpszCursor++;
					cConfig.m_uiS = atoi(lpszCursor);
					lpszCursor++;
					if (cConfig.m_uiS > 9)
						lpszCursor++;
					lpszCursor+=2;
					if (lpszCursor[0] == ' ')
						lpszCursor++;
					cConfig.m_uiL = atoi(lpszCursor);
					lpszCursor++;
					if (cConfig.m_uiL > 9)
						lpszCursor++;
					lpszCursor+=2;
					if (lpszCursor[0] == ' ')
						lpszCursor++;
					cConfig.m_uiP = atoi(lpszCursor);
					lpszCursor++;
					if (cConfig.m_uiP > 9)
						lpszCursor++;
					lpszCursor+=2;
					if (lpszCursor[0] == ' ')
						lpszCursor++;
					cConfig.m_dLevel_Energy = atof(lpszCursor);

					m_cConfiguration_Data_Target.push_back(cConfig);
				}
				else
					m_cConfiguration_Data_Continuum.push_back(cConfig);
				fgets(lpszBuffer,sizeof(lpszBuffer),fileConfigData);
			}
			fclose(fileConfigData);
		}
		else
			std::cerr << "Couldn't open " << lpszFilename << std::endl;
	}
}

