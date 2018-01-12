#pragma once
#include <xastro.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <radiation.h>
#include <sstream>
#include <velocity_function.h>
#include <iostream>
#include <xstdlib.h>

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

	long double m_dEnergy_Ry; // level energy relative to potential at infinity
	long double m_dEnergy_eV; // level energy relative to potential at infinity
	long double m_dEnergy_erg; // level energy relative to potential at infinity

	long double m_dGamma; // sum of all spontanous emission coefficients for this opacity_project_state

	unsigned int m_uiStatistical_Weight; // derived quantity

	opacity_project_state(void) {m_opldDescriptor.m_uiS = -1; m_opldDescriptor.m_uiL = -1; m_opldDescriptor.m_uiP = -1; m_opldDescriptor.m_uiLvl_ID = -1; m_dEnergy_eV = 0.0;}
	opacity_project_state(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID, long double i_dEnergy_eV = 0.0) : m_opldDescriptor(i_uiZ, i_uiN, i_uiS,i_uiL,i_uiP,i_uiLvl_ID) {m_dEnergy_eV = i_dEnergy_eV;}

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
	long double	m_dOscillator_Strength;
	long double m_dWeighted_Oscillator_Strength; // g2/g1 f?

	long double m_dTransition_Energy_Ry;
	long double m_dTransition_Energy_erg;
	long double m_dTransition_Energy_eV;
	long double m_dWavelength_cm;
	long double m_dFrequency_Hz;
	long double m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	long double m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	long double m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

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

typedef std::map<long double, long double> mddcs;
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
	long double		m_dLevel_Energy;
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
		{
			char lpszSymb[4];
			char lpszNum[32];
			xGet_Element_Symbol(m_uiZ,lpszSymb);
			xRomanNumeralGenerator(lpszNum,m_uiZ - m_uiN + 1);
			std::cerr << "Failed to find state " << lpszSymb << " " << lpszNum << ":" << i_opldDescriptor.m_uiS << " " << i_opldDescriptor.m_uiL << " " << i_opldDescriptor.m_uiP << " " << i_opldDescriptor.m_uiLvl_ID << " " << m_msStates.count(i_opldDescriptor) << std::endl;
		}
		return stateI;
	}
	opacity_project_state Find_State(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const
	{
		return Find_State(opacity_project_level_descriptor(m_uiZ,m_uiN,i_uiState_S,i_uiState_L,i_uiState_P,i_uiID));
	}

	long double Calc_Ionizing_Rate(const opacity_project_state &i_sState, const radiation_field & i_cRad, const long double & i_dRescale_Energy_Ry = -1.0, const long double &  i_dRedshift = 0.0) const
	{
		long double dSum = 0.0;
		cimscs cimscsCS_Data = m_mscsPI_Cross_Sections.find(i_sState.m_opldDescriptor);
		long double dScaling = fabs(i_dRescale_Energy_Ry / i_sState.m_dEnergy_Ry);
		//printf("2here0 %Le %Le %i %i %i%i%i %i\n",i_dRescale_Energy_Ry,i_sState.m_dEnergy_Ry,i_sState.m_opldDescriptor.m_uiZ,i_sState.m_opldDescriptor.m_uiN,i_sState.m_opldDescriptor.m_uiS,i_sState.m_opldDescriptor.m_uiL,i_sState.m_opldDescriptor.m_uiP,i_sState.m_opldDescriptor.m_uiLvl_ID);
		if (cimscsCS_Data != m_mscsPI_Cross_Sections.end())
		{
			cimddcs iterEnd = cimscsCS_Data->second.end();
			cimddcs iterIm = iterEnd;
			cimddcs iterImm = iterEnd;
			cimddcs iterIp = cimscsCS_Data->second.begin();
			cimddcs iterIpp = iterIp;
			if (iterIpp != iterEnd)
				iterIpp++;
			//printf("2here1\n");
			for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
			{
				if (iterIp != iterEnd)
					iterIp++;
				if (iterIpp != iterEnd)
					iterIpp++;
				long double dE = iterI->first;
				//printf("2here2 %Le\n",dE);
				if (dE >= i_sState.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
				{
					long double dE_Res = dScaling * dE * g_XASTRO.k_dRy; // photon energy in erg
					long double dFrequency = dE_Res / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
					long double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
					long double dJ = i_cRad.Get_Energy_Flux_freq(dFrequency,i_dRedshift);
					long double dJ_Ry = dJ / g_XASTRO.k_dRy; // radiation field in Ry / (cm^2 s Hz)
					// calculate step size in Ry
					long double dDelta_E;
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
					long double dDelta_Freq = fabs(dDelta_E * g_XASTRO.k_dRy * dScaling / g_XASTRO.k_dh); // need to scale here too

					long double dF = 4.0 * g_XASTRO.k_dpi * dJ / dE_Res * dCS * dDelta_Freq;
					//printf("2here3 %Le %Le %Le %Le %Le %Le\n",dE_Res,dCS,dJ,dDelta_Freq,dF,dSum);
//					printf("f(%.2e,%.2e): %.2e, %.2e %.2e %.2e %.2e %.2e %.2e %.2e\n",dE,dE_Res,dScaling,dJ_Ry,dFrequency,dCS,dE_Res,dDelta_Freq,dDelta_E,dF);
					dSum += dF;
				}
				iterImm = iterIm;
				iterIm = iterI;
			}
		}
		return dSum;
	}
	long double Calc_Recombination_Rate(const opacity_project_state &i_sIonized_State, const opacity_project_state &i_sRecombined_State, const velocity_function & i_cVel_Fn, const long double & i_dRescale_Energy_Ry = -1.0) const
	{
		long double dSum = 0.0;
		long double dScaling = fabs(i_dRescale_Energy_Ry / i_sRecombined_State.m_dEnergy_Ry);
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
			//printf("1here1\n");
			for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
			{
				if (iterIp != iterEnd)
					iterIp++;
				if (iterIpp != iterEnd)
					iterIpp++;
				long double dE = iterI->first;
				if (dE >= -i_sRecombined_State.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
				{
					long double dE_erg = fabs(dScaling * dE * g_XASTRO.k_dRy);
					long double dFrequency = dE_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
					long double dVelocity = sqrt(fabs(dScaling * dE - i_dRescale_Energy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
					long double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
					//printf("1here2 %Le %Le %Le %Le %e %e\n",dE,dVelocity,dScaling,i_dRescale_Energy_Ry,g_XASTRO.k_dRy,g_XASTRO.k_dme);
					if (dVelocity > 0.0)
					{
						// calculate step size in Ry
						long double dDelta_E;
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
						long double ldDelta_Vel = dDelta_E * g_XASTRO.k_dRy / (g_XASTRO.k_dme * dVelocity);

						long double dg_plus = i_sIonized_State.m_opldDescriptor.m_uiS + i_sIonized_State.m_opldDescriptor.m_uiL;
						long double dg_neut = i_sRecombined_State.m_opldDescriptor.m_uiS + i_sRecombined_State.m_opldDescriptor.m_uiL;
						long double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
						long double dMW = i_cVel_Fn(dVelocity);
						long double dF = dE_Rel * dE_Rel * dMW * dVelocity * dCS * ldDelta_Vel;
						//printf("%Le %Le %Le %Le %Le %Le %Le %Le\n",dFrequency,dVelocity,dE_Rel,dMW,dCS,ldDelta_Vel,dF,dSum);

						dSum += dF;
					}

				}
				iterImm = iterIm;
				iterIm = iterI;
			}
		}
		else
		{
			//printf("1ahere1\n");
			long double dDelta_E = 0.1 * -i_sRecombined_State.m_dEnergy_Ry;
			for (unsigned int uiI = 0; uiI < 100; uiI++)
			{
				long double dE = -i_sRecombined_State.m_dEnergy_Ry + uiI * dDelta_E;
				long double dE_erg = fabs(dScaling * dE * g_XASTRO.k_dRy);
				long double dFrequency = dE_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
				long double dVelocity = sqrt(fabs(dScaling * dE - i_dRescale_Energy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
				long double dCS = 0.01 * 1e-18 * (1.0 - uiI / 99.0); // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
				if (dVelocity > 0.0)
				{
					// calculate step size in Ry
					//double dDelta_Freq = dDelta_E * dScaling / g_XASTRO.k_dh; // need to scale here too

					long double dg_plus = i_sIonized_State.m_opldDescriptor.m_uiS + i_sIonized_State.m_opldDescriptor.m_uiL;
					long double dg_neut = i_sRecombined_State.m_opldDescriptor.m_uiS + i_sRecombined_State.m_opldDescriptor.m_uiL;
					long double ldDelta_Vel = dDelta_E / (g_XASTRO.k_dme * dVelocity);
					long double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
					long double dFv = i_cVel_Fn(dVelocity);
					long double dF = dE_Rel * dE_Rel * dFv * dVelocity * dCS * ldDelta_Vel;
					//if (isnan(dF) || isinf(dF))
					//	printf("f(%.2e,%.2e): s%.2e, v%.2e f%.2e er%.2e m%.2e de%.2e f%.2e\n",dE,dE_erg,dScaling,dVelocity,dFrequency,dE_Rel,dMW,dDelta_E,dF);
					dSum += dF;
				}
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

	msmst Read_Transition_Data(void);
	ms Read_State_Data(void);
	mscs Read_Opacity_PI_Data(void);
	void Read_Configuration_Data(void);
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
	void clear(void)
	{
		m_vopiIon_Data.clear();
	}

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

	long double Get_Ionization_Rate(const opacity_project_level_descriptor &i_opld_State, const long double & i_dCorrected_Energy_Ry, const radiation_field & i_cRad, const long double &  i_dRedshift = 0.0) const
	{
		opacity_project_state sState = Find_State(i_opld_State);
		return m_vopiIon_Data[i_opld_State.m_uiN - 1].Calc_Ionizing_Rate(sState, i_cRad, i_dCorrected_Energy_Ry, i_dRedshift);

	}
	long double Get_Recombination_Rate(const opacity_project_level_descriptor &i_opld_Ionized_State, const opacity_project_level_descriptor &i_opld_Recombined_State, const long double & i_dCorrected_Energy_Ry, const velocity_function & i_cVel) const
	{
		opacity_project_state sIonized_State = Find_State(i_opld_Ionized_State);
		opacity_project_state sRecombined_State = Find_State(i_opld_Recombined_State);
		return m_vopiIon_Data[i_opld_Recombined_State.m_uiN - 1].Calc_Recombination_Rate(sIonized_State, sRecombined_State, i_cVel, i_dCorrected_Energy_Ry);
	}

};

