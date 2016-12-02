#pragma once
#include <xastro.h>
#include <vector>
#include <map>
#include <cstdlib>


class state
{
public:
	unsigned int m_uiS;
	unsigned int m_uiL;
	unsigned int m_uiP;
	unsigned int m_uiLvl_ID;

	char m_chType; // T is the this state is an actual level, C if it is an approximation of a level (?)
	unsigned int m_uiType_Index; // some index that has something to do with the type if C
	unsigned int m_ui_n; // quantum number n
	unsigned int m_ui_l; // quantum number l

	double m_dEnergy_eV; // level energy relative to potential at infinity

	unsigned int m_uiStatistical_Weight; // derived quantity

	state(void) {m_uiS = -1; m_uiL = -1; m_uiP = -1; m_uiLvl_ID = -1; m_dEnergy_eV = 0.0;}
	state(unsigned int i_uiN, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID, double i_dEnergy_eV = 0.0) {m_uiS = i_uiN; m_uiL = i_uiL; m_uiP = i_uiP; m_uiLvl_ID = i_uiLvl_ID; m_dEnergy_eV = i_dEnergy_eV;}

	inline bool operator < (const state & i_cRHO) const
	{
	
		return (m_uiS < i_cRHO.m_uiS || (m_uiS == i_cRHO.m_uiS && m_uiL < i_cRHO.m_uiL) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP < i_cRHO.m_uiP) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID < i_cRHO.m_uiLvl_ID));
	}
	inline bool operator <= (const state & i_cRHO) const
	{
		return (*this < i_cRHO || *this == i_cRHO);
	}
	inline bool operator > (const state & i_cRHO) const
	{
		return (m_uiS > i_cRHO.m_uiS || (m_uiS == i_cRHO.m_uiS && m_uiL > i_cRHO.m_uiL) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP > i_cRHO.m_uiP) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID > i_cRHO.m_uiLvl_ID));
	}
	inline bool operator >= (const state & i_cRHO) const
	{
		return (*this > i_cRHO || *this == i_cRHO);
	}
	inline bool operator == (const state & i_cRHO) const
	{
		return (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID == i_cRHO.m_uiLvl_ID);
	}
	inline bool operator != (const state & i_cRHO) const
	{
		return (m_uiS != i_cRHO.m_uiS || m_uiL != i_cRHO.m_uiL || m_uiP != i_cRHO.m_uiP || m_uiLvl_ID != i_cRHO.m_uiLvl_ID);
	}


};

class transition
{
public:
	double	m_dOscillator_Strength;
	double m_dWeighted_Oscillator_Strength; // g2/g1 f?

	double m_dTransition_Energy_erg;
	double m_dTransition_Energy_eV;
	double m_dWavelength_cm;
	double m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	double m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	double m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

};

typedef std::vector<state> vs;
typedef vs::iterator ivs;
typedef vs::const_iterator civs;

typedef std::map<double, double> mddcs;
typedef mddcs::iterator imddcs;
typedef mddcs::const_iterator cimddcs;

typedef std::map<state, transition> mst;
typedef mst::iterator imst;
typedef mst::const_iterator cimst;
typedef std::map<state, mst > msmst;
typedef msmst::iterator imsmst;
typedef msmst::const_iterator cimsmst;

typedef std::map< state , mddcs> mscs;
typedef mscs::iterator imscs;
typedef mscs::const_iterator cimscs;
 



class opacity_project_ion
{
public:
	unsigned int m_uiN;
	unsigned int m_uiZ_eff;

	vs				m_vsStates;
	msmst			m_msmtTransitions;
	mscs			m_mscsPI_Cross_Sections;
	
	state Find_State(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiState_ID)
	{
		state stateI;
		bool bFound = false;
		for (civs iterI = m_vsStates.begin(); iterI != m_vsStates.end() && !bFound; iterI++)
		{
			if (iterI->m_uiS == i_uiState_S && iterI->m_uiL == i_uiState_L && iterI->m_uiP == i_uiState_P && iterI->m_uiLvl_ID == i_uiState_ID)
			{
				bFound = true;
				stateI = *iterI;
			}
		}
		if (!bFound)
		{
			std::cerr << "Failed to find state " << i_uiState_S << " " << i_uiState_L << " " << i_uiState_P << " " << i_uiState_ID << std::endl;
		}
		return stateI;
	}
private:

	char * NextNum(char * i_lpszCursor)
	{
		char * lpszRet = i_lpszCursor;
		while (lpszRet[0] != 0 && (lpszRet[0] != ' ' && lpszRet[0] != '\t'))
			lpszRet++;
		while (lpszRet[0] != 0 && (lpszRet[0] == ' ' || lpszRet[0] == '\t'))
			lpszRet++;
		return lpszRet;
	}

	msmst Read_Transition_Data(void)
	{
		msmst msmstData;
		if (m_uiZ_eff != -1 && m_uiN != -1)
		{
			char lpszFilename[256];
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else 
			{
				sprintf(lpszFilename,"%s/Opacity-Project/f%02i.%02i",lpszLA_Data_Path,m_uiN,m_uiZ_eff);
				FILE * fileEdata = fopen(lpszFilename,"rt");

				if (fileEdata)
				{
					unsigned int uiS1,uiS2,uiL1,uiL2,uiP1,uiP2;
					state sStateI;
					state sStateJ;
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
						unsigned int uiState_Count = atoi(lpszCursor);

						if (uiS1 != 0 || uiL1 != 0 || uiP1 != 0 || uiS2 != 0 || uiL2 != 0 || uiP2 != 0)
						{
							mst mstTransitions;
							for (unsigned int uiI = 0; uiI < uiState_Count; uiI++)
							{
								transition trxData;

								fgets(lpszData,64,fileEdata); // individual state info
								lpszCursor = NextNum(lpszData);
								unsigned int uiLvl1 = atoi(lpszCursor);
								lpszCursor = NextNum(lpszCursor);
								unsigned int uiLvl2 = atoi(lpszCursor);
								lpszCursor = NextNum(lpszCursor);
								trxData.m_dOscillator_Strength = atof(lpszCursor);
								lpszCursor = NextNum(lpszCursor);
								trxData.m_dWeighted_Oscillator_Strength = atof(lpszCursor);
				
								state stateLower;
								state stateUpper;

								if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
								{
									sStateI = Find_State(uiS1,uiL1,uiP1,uiLvl1);
									sStateJ = Find_State(uiS2,uiL2,uiP2,uiLvl2);
								}
								else
								{
									sStateI = Find_State(uiS2,uiL2,uiP2,uiLvl2);
									sStateJ = Find_State(uiS1,uiL1,uiP1,uiLvl1);
								}
								if (sStateI.m_dEnergy_eV < sStateJ.m_dEnergy_eV)
								{
									stateLower = sStateI;
									stateUpper = sStateJ;
								}
								else
								{
									stateLower = sStateJ;
									stateUpper = sStateI;
								}



								trxData.m_dTransition_Energy_eV = fabs(sStateI.m_dEnergy_eV - sStateJ.m_dEnergy_eV);
								trxData.m_dTransition_Energy_erg = trxData.m_dTransition_Energy_eV * g_XASTRO.k_derg_eV;
								trxData.m_dWavelength_cm = g_XASTRO.k_dh / (g_XASTRO.k_derg_eV * trxData.m_dTransition_Energy_erg);

		trxData.m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dme * g_XASTRO.k_dc * trxData.m_dWavelength_cm * trxData.m_dWavelength_cm) * trxData.m_dOscillator_Strength *  stateLower.m_uiS / (double)stateUpper.m_uiS;
//		m_dEinstein_B = m_dEinstein_A / (8 * g_XASTRO.k_dpi * m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm * g_XASTRO.k_dh);
//		m_dEinstein_B = m_dEinstein_A / (2 * m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm * g_XASTRO.k_dh * g_XASTRO.k_dc);
		trxData.m_dEinstein_B = 2.0 * g_XASTRO.k_dh * g_XASTRO.k_dc * trxData.m_dEinstein_A / (trxData.m_dWavelength_cm * trxData.m_dWavelength_cm * trxData.m_dWavelength_cm);
		trxData.m_dEinstein_B_SE = trxData.m_dEinstein_B * stateLower.m_uiS / (double)stateUpper.m_uiS;

								if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
									mstTransitions[sStateJ] = trxData;
								else
									mstTransitions[sStateI] = trxData;
							}
							if (uiS1 < uiS2 || (uiS1 == uiS2 && uiL1 < uiL2) || (uiS1 == uiS2 && uiL1 == uiL2 && uiP1 < uiP2))
								msmstData[sStateJ] = mstTransitions;
							else
								msmstData[sStateI] = mstTransitions;
						}
					} while (uiS1 != 0 || uiL1 != 0 || uiP1 != 0 || uiS2 != 0 || uiL2 != 0 || uiP2 != 0);
				}
			}
		}
		return msmstData;
	}


	vs Read_State_Data(void)
	{
		vs vsData;
		if (m_uiZ_eff != -1 && m_uiN != -1)
		{
			char lpszFilename[256];
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else
			{
				sprintf(lpszFilename,"%s/Opacity-Project/e%02i.%02i",lpszLA_Data_Path,m_uiN,m_uiZ_eff);
				FILE * fileEdata = fopen(lpszFilename,"rt");

				if (fileEdata)
				{
					state sState;
					char lpszData[64];
					char * lpszCursor = nullptr;

					fgets(lpszData,64,fileEdata); // general ion information that we don't care about right now

					do
					{
						fgets(lpszData,64,fileEdata); //S, L, P, and count info
						lpszCursor = NextNum(lpszData);
						sState.m_uiS = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_uiL = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						sState.m_uiP = atoi(lpszCursor);

						lpszCursor = NextNum(lpszCursor);
						unsigned int uiState_Count = atoi(lpszCursor);


						fgets(lpszData,64,fileEdata); //don't know what this data is

						for (unsigned int uiI = 0; uiI < uiState_Count; uiI++)
						{
							fgets(lpszData,64,fileEdata); // individual state info
							lpszCursor = NextNum(lpszData);
							sState.m_uiLvl_ID = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							sState.m_chType = lpszCursor[0];
							lpszCursor = NextNum(lpszCursor);
							sState.m_uiType_Index = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							sState.m_ui_n = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							sState.m_ui_l = atoi(lpszCursor);
							lpszCursor = NextNum(lpszCursor);
							sState.m_dEnergy_eV = atof(lpszCursor) * 13.60569300984;

							vsData.push_back(sState);
						}
					} while (sState.m_uiS != 0 || sState.m_uiL != 0 || sState.m_uiP != 0);
				}
			}
		}
		return vsData;
	}

	mscs Read_Opacity_PI_Data(void)
	{
		mscs mscsData;
		char lpszFilename[256];
		if (m_uiZ_eff != -1 && m_uiN != -1)
		{
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else
			{
				sprintf(lpszFilename,"%s/Opacity-Project/p%02i.%02i",lpszLA_Data_Path,m_uiN,m_uiZ_eff);

				FILE * filePIdata = fopen(lpszFilename,"rt");
				if (filePIdata)
				{
					char lpszData[64];
		


					fgets(lpszData,64,filePIdata); // general ion information that we don't care about right now
					char * lpszCursor = NextNum(lpszData);
					unsigned int uiState_Count = atoi(lpszCursor);
					std::cout << uiState_Count << std::endl;

					for (unsigned int uiI = 0; uiI < uiState_Count; uiI++)
					{
						fgets(lpszData,64,filePIdata);
						lpszCursor = NextNum(lpszData);
						unsigned int uiState_S = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						unsigned int uiState_L = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						unsigned int uiState_P = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						unsigned int uiState_ID = atoi(lpszCursor);

						state stateI = Find_State(uiState_S,uiState_L,uiState_P,uiState_ID);

						fgets(lpszData,64,filePIdata);
						lpszCursor = NextNum(lpszData);
						unsigned int uiEner_Count = atoi(lpszCursor);
						lpszCursor = NextNum(lpszCursor);
						unsigned int uiCS_Count = atoi(lpszCursor);


						mddcs mddcsState_Data;
						fgets(lpszData,64,filePIdata); // first one seems to be garbage?
						lpszCursor = NextNum(lpszData);
						double dState_Energy = atof(lpszCursor) * 13.60569300984; // Rydberg


						std::cout << uiState_S << " " << uiState_L << " " << uiState_P << " " << uiState_ID << " " << dState_Energy << " " << uiCS_Count << std::endl;
			
			//			if (uiEner_Count != uiCS_Count)
			//				std::cerr << "Something fishy at " << uiState_S << " " << uiState_L << " " << uiState_P << " " << uiState_ID  << std::endl;

						for (unsigned int uiJ = 0; uiJ < uiCS_Count; uiJ++)
						{
							fgets(lpszData,64,filePIdata);
							lpszCursor = NextNum(lpszData);
							double dEnergy = atof(lpszCursor) * 13.60569300984; // Rydberg
							lpszCursor = NextNum(lpszCursor);
							double dCross_Section = atof(lpszCursor);

							double dOscillator_Strength = g_XASTRO.k_dme * g_XASTRO.k_dc / (2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de) * dCross_Section;
							double dEnergy_f = dEnergy * dOscillator_Strength;
							//std::cout << dEnergy << " " << dOscillator_Strength << " " << dEnergy_f << std::endl;

							mddcsState_Data[dEnergy] = dCross_Section;
						}

						mscsData[stateI] = mddcsState_Data;
					}
					fclose(filePIdata);
				}
			}
		}
		return mscsData;
	}
public:
	void Read_Data(unsigned int i_uiN, unsigned int i_uiZ)
	{
		m_uiN = i_uiN;
		m_uiZ_eff = i_uiZ;
		m_vsStates = Read_State_Data();
		m_msmtTransitions = Read_Transition_Data();
		m_mscsPI_Cross_Sections = Read_Opacity_PI_Data();
	}
	opacity_project_ion(void)
	{
		m_uiN = m_uiZ_eff = -1;
	}
	opacity_project_ion(unsigned int i_uiN, unsigned int i_uiZ)
	{
		Read_Data(i_uiN,i_uiZ);
	}
};

