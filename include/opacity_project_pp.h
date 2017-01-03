#pragma once
#include <xastro.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <radiation.h>
#include <sstream>
#include <velocity_function.h>

class opacity_project_level_descriptor
{
public:
	unsigned int m_uiZ;
	unsigned int m_uiN;

	unsigned int m_uiS;
	unsigned int m_uiL;
	unsigned int m_uiP;
	unsigned int m_uiLvl_ID;

	opacity_project_level_descriptor(void){m_uiN = m_uiZ = m_uiS = m_uiL = m_uiP = m_uiLvl_ID = -1;}
	opacity_project_level_descriptor(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID) {m_uiS = i_uiS; m_uiL = i_uiL; m_uiP = i_uiP; m_uiLvl_ID = i_uiLvl_ID; m_uiZ = i_uiZ; m_uiN = i_uiN;}

	inline bool operator > (const opacity_project_level_descriptor & i_cRHO) const
	{
		return (m_uiZ > i_cRHO.m_uiZ || (m_uiZ == i_cRHO.m_uiZ && m_uiN > i_cRHO.m_uiN) || (m_uiZ == i_cRHO.m_uiZ && m_uiN == i_cRHO.m_uiN &&
(m_uiS > i_cRHO.m_uiS || (m_uiS == i_cRHO.m_uiS && m_uiL > i_cRHO.m_uiL) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP > i_cRHO.m_uiP) || (m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID > i_cRHO.m_uiLvl_ID))));
	}
	inline bool operator >= (const opacity_project_level_descriptor & i_cRHO) const
	{
		return (*this > i_cRHO || *this == i_cRHO);
	}
	inline bool operator < (const opacity_project_level_descriptor & i_cRHO) const
	{
		return !(*this >= i_cRHO);
	}
	inline bool operator <= (const opacity_project_level_descriptor & i_cRHO) const
	{
		return !(*this < i_cRHO);
	}
	inline bool operator == (const opacity_project_level_descriptor & i_cRHO) const
	{
		return (m_uiZ == i_cRHO.m_uiZ && m_uiN == i_cRHO.m_uiN && m_uiS == i_cRHO.m_uiS && m_uiL == i_cRHO.m_uiL && m_uiP == i_cRHO.m_uiP && m_uiLvl_ID == i_cRHO.m_uiLvl_ID);
	}
	inline bool operator != (const opacity_project_level_descriptor & i_cRHO) const
	{
		return (m_uiZ != i_cRHO.m_uiZ || m_uiN != i_cRHO.m_uiN || m_uiS != i_cRHO.m_uiS || m_uiL != i_cRHO.m_uiL || m_uiP != i_cRHO.m_uiP || m_uiLvl_ID != i_cRHO.m_uiLvl_ID);
	}
};

class opacity_project_state
{
public:
	opacity_project_level_descriptor m_opldDescriptor;

	char m_chType; // T is the this opacity_project_state is an actual level, C if it is an approximation of a level (?)
	unsigned int m_uiType_Index; // some index that has something to do with the type if C
	unsigned int m_ui_n; // quantum number n
	unsigned int m_ui_l; // quantum number l

	double m_dEnergy_Ry; // level energy relative to potential at infinity
	double m_dEnergy_eV; // level energy relative to potential at infinity
	double m_dEnergy_erg; // level energy relative to potential at infinity

	double m_dGamma; // sum of all spontanous emission coefficients for this opacity_project_state

	unsigned int m_uiStatistical_Weight; // derived quantity

	opacity_project_state(void) {m_opldDescriptor.m_uiS = -1; m_opldDescriptor.m_uiL = -1; m_opldDescriptor.m_uiP = -1; m_opldDescriptor.m_uiLvl_ID = -1; m_dEnergy_eV = 0.0;}
	opacity_project_state(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID, double i_dEnergy_eV = 0.0) : m_opldDescriptor(i_uiZ, i_uiN, i_uiS,i_uiL,i_uiP,i_uiLvl_ID) {m_dEnergy_eV = i_dEnergy_eV;}

	inline bool operator < (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor < i_cRHO.m_opldDescriptor;
	}
	inline bool operator <= (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor <= i_cRHO.m_opldDescriptor;
	}
	inline bool operator > (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor > i_cRHO.m_opldDescriptor;
	}
	inline bool operator >= (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor >= i_cRHO.m_opldDescriptor;
	}
	inline bool operator == (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor == i_cRHO.m_opldDescriptor;
	}
	inline bool operator != (const opacity_project_state & i_cRHO) const
	{
		return m_opldDescriptor != i_cRHO.m_opldDescriptor;
	}


};

class opacity_project_transition
{
public:
	double	m_dOscillator_Strength;
	double m_dWeighted_Oscillator_Strength; // g2/g1 f?

	double m_dTransition_Energy_Ry;
	double m_dTransition_Energy_erg;
	double m_dTransition_Energy_eV;
	double m_dWavelength_cm;
	double m_dFrequency_Hz;
	double m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	double m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	double m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

	void Calculate_Derived_Quantities(const opacity_project_state & i_sState_Lower, const opacity_project_state & i_sState_Upper)
	{
		m_dTransition_Energy_Ry = i_sState_Upper.m_dEnergy_Ry - i_sState_Lower.m_dEnergy_Ry;
		m_dTransition_Energy_eV = m_dTransition_Energy_Ry * g_XASTRO.k_dRy_eV;
		m_dTransition_Energy_erg = m_dTransition_Energy_Ry * g_XASTRO.k_dRy;
		m_dWavelength_cm = g_XASTRO.k_dh / (g_XASTRO.k_derg_eV * m_dTransition_Energy_erg);

		m_dFrequency_Hz = g_XASTRO.k_dc / m_dWavelength_cm;
		m_dEinstein_B = 4.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dh * m_dFrequency_Hz * g_XASTRO.k_dme) * m_dOscillator_Strength / 100.0; // I don't know why this factor of 100 is here, but it seems to be needed to get the right order or magnitude for A
		m_dEinstein_B_SE = (i_sState_Upper.m_uiStatistical_Weight * m_dEinstein_B) / i_sState_Lower.m_uiStatistical_Weight;
		m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh / (m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm) * m_dEinstein_B_SE;
	}
};

typedef std::map<opacity_project_level_descriptor,opacity_project_state> ms;
typedef ms::iterator ims;
typedef ms::const_iterator cims;

typedef std::map<double, double> mddcs;
typedef mddcs::iterator imddcs;
typedef mddcs::const_iterator cimddcs;

typedef std::map<opacity_project_level_descriptor, opacity_project_transition> mst;
typedef mst::iterator imst;
typedef mst::const_iterator cimst;
typedef std::map<opacity_project_level_descriptor, mst > msmst;
typedef msmst::iterator imsmst;
typedef msmst::const_iterator cimsmst;

typedef std::map< opacity_project_level_descriptor , mddcs> mscs;
typedef mscs::iterator imscs;
typedef mscs::const_iterator cimscs;
 
class configuration_data
{
public:
	char m_chType;
	unsigned int m_uiIndex;
	unsigned int m_uiS;
	unsigned int m_uiL;
	unsigned int m_uiP;
	double		m_dLevel_Energy;
	std::string m_szState;
};

typedef std::vector<configuration_data> vcd;
typedef std::vector<configuration_data>::iterator ivcd;
typedef std::vector<configuration_data>::const_iterator civcd;



class opacity_project_ion
{
public:
	unsigned int m_uiN; // electron count
	unsigned int m_uiZ; // atomic number

	ms				m_msStates;
	msmst			m_msmtTransitions;
	mscs			m_mscsPI_Cross_Sections;
	vcd				m_cConfiguration_Data_Target;
	vcd				m_cConfiguration_Data_Continuum;
	
	opacity_project_state Find_State(const opacity_project_level_descriptor &i_opldDescriptor) const
	{
		opacity_project_state stateI;
		if (i_opldDescriptor.m_uiZ != m_uiZ || i_opldDescriptor.m_uiN != m_uiN)
			std::cerr << "Z,N mismatch " << i_opldDescriptor.m_uiZ << " " << i_opldDescriptor.m_uiN << " -- " << m_uiZ << " " << m_uiN << std::endl;
		if (m_msStates.count(i_opldDescriptor) == 1)
			stateI = m_msStates.at(i_opldDescriptor);
		else
			std::cerr << "Failed to find state " << i_opldDescriptor.m_uiS << " " << i_opldDescriptor.m_uiL << " " << i_opldDescriptor.m_uiP << " " << i_opldDescriptor.m_uiLvl_ID << " " << m_msStates.count(i_opldDescriptor) << std::endl;
		return stateI;
	}
	opacity_project_state Find_State(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
	{
		return Find_State(opacity_project_level_descriptor(m_uiZ,m_uiN,i_uiState_S,i_uiState_L,i_uiState_P,i_uiID));
	}

	double Calc_Ionizing_Rate(const opacity_project_state &i_sState, const radiation_field & i_cRad, const double & i_dRescale_Energy_Ry = -1.0, const double &  i_dRedshift = 0.0) const
	{
		double dSum = 0.0;
		cimscs cimscsCS_Data = m_mscsPI_Cross_Sections.find(i_sState.m_opldDescriptor);
		if (cimscsCS_Data != m_mscsPI_Cross_Sections.end())
		{
			cimddcs iterEnd = cimscsCS_Data->second.end();
			cimddcs iterIm = iterEnd;
			cimddcs iterImm = iterEnd;
			cimddcs iterIp = cimscsCS_Data->second.begin();
			cimddcs iterIpp = iterIp;
			if (iterIpp != iterEnd)
				iterIpp++;
			double dScaling = fabs(i_dRescale_Energy_Ry / i_sState.m_dEnergy_Ry);
			for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
			{
				if (iterIp != iterEnd)
					iterIp++;
				if (iterIpp != iterEnd)
					iterIpp++;
				double dE = iterI->first;
				if (dE >= -i_sState.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
				{
					double dE_Res = dScaling * dE * g_XASTRO.k_dRy;
					double dFrequency = dE_Res / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
					double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
					double dJ = i_cRad.Get_Energy_Flux_freq(dFrequency,i_dRedshift);
					double dJ_Ry = dJ / g_XASTRO.k_dRy; // radiation field in Ry / (cm^2 s Hz)
					// calculate step size in Ry
					double dDelta_E;
					if ((iterImm == iterEnd || iterIpp == iterEnd) && iterIm != iterEnd && iterIp != iterEnd)
					{
						dDelta_E = (iterIp->first - iterIm->first) * 0.5;
					}
					else if (iterIp == iterEnd && iterIm != iterEnd)
					{
						dDelta_E = (iterI->first - iterIm->first);
					}
					else if (iterIm == iterEnd && iterIp != iterEnd)
					{
						dDelta_E = (iterIp->first - iterI->first);
					}
					else
					{
						dDelta_E = (iterIpp->first + iterIp->first - iterIm->first - iterImm->first) / 6.0;
					}
					double dDelta_Freq = fabs(dDelta_E * g_XASTRO.k_dRy * dScaling / g_XASTRO.k_dh); // need to scale here too

					double dF = 4.0 * g_XASTRO.k_dpi * dJ_Ry * dCS / (dE_Res) * dDelta_Freq;
//					printf("f(%.2e,%.2e): %.2e, %.2e %.2e %.2e %.2e %.2e %.2e %.2e\n",dE,dE_Res,dScaling,dJ_Ry,dFrequency,dCS,dE_Res,dDelta_Freq,dDelta_E,dF);
					dSum += dF;
				}
				iterImm = iterIm;
				iterIm = iterI;
			}
		}
		return dSum;
	}
	double Calc_Recombination_Rate(const opacity_project_state &i_sIonized_State, const opacity_project_state &i_sRecombined_State, const velocity_function & i_cVel_Fn, const double & i_dRescale_Energy_Ry = -1.0) const
	{
		double dSum = 0.0;
		cimscs cimscsCS_Data = m_mscsPI_Cross_Sections.find(i_sRecombined_State.m_opldDescriptor);
		if (cimscsCS_Data != m_mscsPI_Cross_Sections.end())
		{
			cimddcs iterEnd = cimscsCS_Data->second.end();
			cimddcs iterIm = iterEnd;
			cimddcs iterImm = iterEnd;
			cimddcs iterIp = cimscsCS_Data->second.begin();
			cimddcs iterIpp = iterIp;
			if (iterIpp != iterEnd)
				iterIpp++;
			double dScaling = fabs(i_dRescale_Energy_Ry / i_sRecombined_State.m_dEnergy_Ry);
			for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
			{
				if (iterIp != iterEnd)
					iterIp++;
				if (iterIpp != iterEnd)
					iterIpp++;
				double dE = iterI->first;
				if (dE >= -i_sRecombined_State.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
				{
					double dE_erg = fabs(dScaling * dE * g_XASTRO.k_dRy);
					double dFrequency = dE_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
					double dVelocity = sqrt(fabs(dScaling * dE - i_dRescale_Energy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
					double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
					if (dVelocity > 0.0)
					{
						double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
						// calculate step size in Ry
						double dDelta_E;
						if ((iterImm == iterEnd || iterIpp == iterEnd) && iterIm != iterEnd && iterIp != iterEnd)
						{
							dDelta_E = (iterIp->first - iterIm->first) * 0.5;
						}
						else if (iterIp == iterEnd && iterIm != iterEnd)
						{
							dDelta_E = (iterI->first - iterIm->first);
						}
						else if (iterIm == iterEnd && iterIp != iterEnd)
						{
							dDelta_E = (iterIp->first - iterI->first);
						}
						else
						{
							dDelta_E = (iterIpp->first + iterIp->first - iterIm->first - iterImm->first) / 6.0;
						}
						//double dDelta_Freq = dDelta_E * dScaling / g_XASTRO.k_dh; // need to scale here too

						double dg_plus = i_sIonized_State.m_opldDescriptor.m_uiS + i_sIonized_State.m_opldDescriptor.m_uiL;
						double dg_neut = i_sRecombined_State.m_opldDescriptor.m_uiS + i_sRecombined_State.m_opldDescriptor.m_uiL;

						double dMW = i_cVel_Fn(dVelocity);
						double dF = dg_neut / (dg_plus * 4.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi) * dVelocity * i_cVel_Fn(dVelocity) * dCS * dE_Rel * dE_Rel * dDelta_E;
						//if (isnan(dF) || isinf(dF))
							printf("f(%.2e,%.2e): s%.2e, v%.2e f%.2e er%.2e m%.2e de%.2e f%.2e\n",dE,dE_erg,dScaling,dVelocity,dFrequency,dE_Rel,dMW,dDelta_E,dF);
						dSum += dF;
					}

				}
				iterImm = iterIm;
				iterIm = iterI;
			}
		}
		return dSum;
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
		if (m_uiZ != -1 && m_uiN != -1)
		{
			char lpszFilename[256];
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else 
			{
				sprintf(lpszFilename,"%s/Opacity-Project/f%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);
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
				}
			}
		}
		return msmstData;
	}


	ms Read_State_Data(void)
	{
		ms msData;
		if (m_uiZ != -1 && m_uiN != -1)
		{
			char lpszFilename[256];
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else
			{
				sprintf(lpszFilename,"%s/Opacity-Project/e%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);
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
				}
			}
		}
		return msData;
	}

	mscs Read_Opacity_PI_Data(void)
	{
		mscs mscsData;
		char lpszFilename[256];
		if (m_uiZ != -1 && m_uiN != -1)
		{
			const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
			if (lpszLA_Data_Path == nullptr)
				std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
			else
			{
				sprintf(lpszFilename,"%s/Opacity-Project/p%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);

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
			}
		}
		return mscsData;
	}
	void Read_Configuration_Data(void)
	{
		const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
		if (lpszLA_Data_Path == nullptr)
			std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
		else
		{
			char lpszFilename[256];
			sprintf(lpszFilename,"%s/Opacity-Project/c%02i.%02i",lpszLA_Data_Path,m_uiZ,m_uiN);

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
		}
	}
public:
	void Read_Data(unsigned int i_uiZ, unsigned int i_uiN)
	{
		m_uiZ = i_uiZ;
		m_uiN = i_uiN;
		m_msStates = Read_State_Data();
		m_msmtTransitions = Read_Transition_Data();
		m_mscsPI_Cross_Sections = Read_Opacity_PI_Data();
		Read_Configuration_Data();
	}
	opacity_project_ion(void)
	{
		m_uiN = m_uiZ = -1;
	}
	opacity_project_ion(unsigned int i_uiZ, unsigned int i_uiN)
	{
		Read_Data(i_uiZ,i_uiN);
	}
	std::string Get_State_Configuration(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID)
	{
		std::string szRet;
		opacity_project_state stateI = Find_State(i_uiState_S,i_uiState_L,i_uiState_P,i_uiID);
		if (stateI.m_opldDescriptor.m_uiS != -1)
		{
			std::ostringstream ossConfig;
			if (stateI.m_chType == 'T')
			{
				ossConfig << m_cConfiguration_Data_Target[stateI.m_uiType_Index - 1].m_szState;
				ossConfig << " " << stateI.m_ui_n;
				if (stateI.m_ui_l == 0)
					ossConfig << "s";
				else if (stateI.m_ui_l == 1)
					ossConfig << "p";
				else if (stateI.m_ui_l == 2)
					ossConfig << "d";
				else if (stateI.m_ui_l >= 3 && stateI.m_ui_l <= 6)
				{
					char chState[2] = {(char)('f' + stateI.m_ui_l - 3),0};
					ossConfig << chState;
				}
				else if (stateI.m_ui_l >= 7)
				{
					char chState[2] = {(char)('k' + stateI.m_ui_l - 7),0};
					ossConfig << chState;
				}
			}
			else
				ossConfig << m_cConfiguration_Data_Continuum[stateI.m_uiType_Index - 1].m_szState;
			ossConfig << " " << stateI.m_opldDescriptor.m_uiS;
			if (stateI.m_opldDescriptor.m_uiL == 0)
				ossConfig << "S";
			else if (stateI.m_opldDescriptor.m_uiL == 1)
				ossConfig << "P";
			else if (stateI.m_opldDescriptor.m_uiL == 2)
				ossConfig << "D";
			else if (stateI.m_opldDescriptor.m_uiL >= 3 && stateI.m_opldDescriptor.m_uiL <= 6)
			{
				char chState[2] = {(char)('F' + stateI.m_opldDescriptor.m_uiL - 3),0};
				ossConfig << chState;
			}
			else if (stateI.m_opldDescriptor.m_uiL >= 7)
			{
				char chState[2] = {(char)('K' + stateI.m_opldDescriptor.m_uiL - 7),0};
				ossConfig << chState;
			}
			if (stateI.m_opldDescriptor.m_uiP == 1)
				ossConfig << "*";
			szRet = ossConfig.str();
		}
		return szRet;
	}
};

class opacity_project_element
{
public:
	unsigned int m_uiZ;
	std::vector<opacity_project_ion> m_vopiIon_Data;
public:
	void Read_Element_Data(unsigned int i_uiZ)
	{
		m_uiZ = i_uiZ;
		for (unsigned int uiI = 1; uiI <= i_uiZ; uiI++)
		{
			opacity_project_ion opiIon_Data;
			opiIon_Data.Read_Data(i_uiZ,uiI);
			m_vopiIon_Data.push_back(opiIon_Data);
		}
	}
	opacity_project_state Find_State(const opacity_project_level_descriptor & i_poldLevel) const
	{
		opacity_project_state stateI;
		if (m_uiZ == i_poldLevel.m_uiZ && i_poldLevel.m_uiN <= m_uiZ)
			stateI = m_vopiIon_Data[i_poldLevel.m_uiN - 1].Find_State(i_poldLevel);
		else
			std::cerr << "Failed to find state " << i_poldLevel.m_uiZ << " " << i_poldLevel.m_uiN << " " << i_poldLevel.m_uiS << " " << i_poldLevel.m_uiL << " " << i_poldLevel.m_uiP << " " << i_poldLevel.m_uiLvl_ID << std::endl;
		return stateI;
	}
	opacity_project_state Find_State(unsigned int i_uiN, unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
	{
		return Find_State(opacity_project_level_descriptor(m_uiZ, i_uiN, i_uiState_S, i_uiState_L, i_uiState_P, i_uiID));
	}

	double Get_Ionization_Rate(const opacity_project_level_descriptor &i_opld_State, const double & i_dCorrected_Energy_Ry, const radiation_field & i_cRad, const double &  i_dRedshift = 0.0) const
	{
		opacity_project_state sState = Find_State(i_opld_State);
		return m_vopiIon_Data[i_opld_State.m_uiN - 1].Calc_Ionizing_Rate(sState, i_cRad, i_dCorrected_Energy_Ry, i_dRedshift);

	}
	double Get_Recombination_Rate(const opacity_project_level_descriptor &i_opld_Ionized_State, const opacity_project_level_descriptor &i_opld_Recombined_State, const double & i_dCorrected_Energy_Ry, const velocity_function & i_cVel) const
	{
		opacity_project_state sIonized_State = Find_State(i_opld_Ionized_State);
		opacity_project_state sRecombined_State = Find_State(i_opld_Recombined_State);
		return m_vopiIon_Data[i_opld_Recombined_State.m_uiN - 1].Calc_Recombination_Rate(sIonized_State, sRecombined_State, i_cVel, i_dCorrected_Energy_Ry);
		return 0.0;
	}

};

