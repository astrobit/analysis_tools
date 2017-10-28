#pragma once
#include <xastro.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <radiation.h>
#include <iostream>
#include <iomanip>
#include <line_routines.h>
#include <xmath.h>

class Kurucz_Level_Data
{
public:
	// Data from Kurucz table
	double m_dEnergy_Level_cm;
	double m_dJ;
	std::string   m_szLabel;
	unsigned int m_uiNLTE_Index;
	int	m_iHyperfine_Shift_mK;
//	int m_iHyperfine_F;	

	// derived data
	double	m_dStat_Weight;
	double	m_dZ; // Z coefficient (M&W 2016(?) Eq. 12)
	double m_dEnergy_Level_eV;
	double m_dEnergy_Level_Ryd;

	double m_dGamma; // sum of spontaneous emission coefficents to all lower levels

	// duplicate data
	double m_dElement_Code;

	inline bool operator == (const Kurucz_Level_Data & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm == m_dEnergy_Level_cm &&
			i_cRHO.m_dJ == m_dJ &&
//			i_cRHO.m_uiNLTE_Index == m_uiNLTE_Index &&
//			i_cRHO.m_iHyperfine_Shift_mK == m_iHyperfine_Shift_mK &&
//			i_cRHO.m_iHyperfine_F == m_iHyperfine_F &&
			i_cRHO.m_szLabel == m_szLabel);
		
	}
	inline bool operator != (const Kurucz_Level_Data & i_cRHO) const
	{
		return !(*this == i_cRHO);
	}
	inline bool operator <  (const Kurucz_Level_Data & i_cRHO) const
	{
		return (m_dEnergy_Level_cm < i_cRHO.m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && m_dJ < i_cRHO.m_dJ));
	}
	inline bool operator <=  (const Kurucz_Level_Data & i_cRHO) const
	{
		return ((m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && m_dJ == i_cRHO.m_dJ) || (*this < i_cRHO));
	}
	inline bool operator >  (const Kurucz_Level_Data & i_cRHO) const
	{
		return (m_dEnergy_Level_cm > i_cRHO.m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && m_dJ > i_cRHO.m_dJ));
	}
	inline bool operator >=  (const Kurucz_Level_Data & i_cRHO) const
	{
		return ((m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && m_dJ == i_cRHO.m_dJ) || (*this > i_cRHO));
	}
};




class Kurucz_Line_Data
{
public:
	// data from the file
	double m_dWavelength_nm;
	double m_dLog_gf;
	double m_dElement_Code;
	Kurucz_Level_Data	m_cLevel_Lower;
	Kurucz_Level_Data	m_cLevel_Upper;
	double	m_dGamma_Rad;
	double	m_dGamma_Stark;
	double	m_dGamma_vdWaals;
	std::string	m_szReference;
	unsigned int m_uiIsotope;
	double	m_dHyperfine_Fractional_Strength;
	unsigned int m_uiIsotope_Diatomics;
	double	m_dIsotope_Abundance_Fraction;
	unsigned int m_uiLine_Strength_Class;
	std::string	m_szCode;
	int m_uiEven_Lande_g;
	int m_uiOdd_Lande_g;
	int m_uiIsotope_Shift_mA;

	// derived / calculated data
	double m_dTransition_Energy_erg;
	double m_dTransition_Energy_eV;
	double m_dWavelength_cm;
	double m_dFrequency_Hz;
	double m_dOscillator_Strength; 
	double m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	double m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	double m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

	double m_dH_abs; // H coefficient (M&W 2017, Eq. 13) for j < i
	double m_dH_em; // H coefficient (M&W 2017, Eq. 13) for j > i
private:
	std::string mysubstr(const std::string & i_szStr, size_t &tStart, size_t tLen)
	{
		std::string szRet = i_szStr.substr(tStart,tLen);
		tStart += tLen;
		return szRet;
	}
public:

	void Process_Datafile_Line(const std::string & i_szLine)
	{
		size_t tPos = 0;
		m_dWavelength_nm = std::stod(mysubstr(i_szLine,tPos,11));
		m_dLog_gf = std::stod(mysubstr(i_szLine,tPos,7));
		m_dElement_Code = std::stod(mysubstr(i_szLine,tPos,6));
		m_cLevel_Lower.m_dEnergy_Level_cm = std::stod(mysubstr(i_szLine,tPos,12));
		m_cLevel_Lower.m_dJ = std::stod(mysubstr(i_szLine,tPos,5));
		tPos++; // blank space
		m_cLevel_Lower.m_szLabel = mysubstr(i_szLine,tPos,10);
		m_cLevel_Upper.m_dEnergy_Level_cm = std::stod(mysubstr(i_szLine,tPos,12));
		m_cLevel_Upper.m_dJ = std::stod(mysubstr(i_szLine,tPos,5));
		tPos++; // blank space
		m_cLevel_Upper.m_szLabel = mysubstr(i_szLine,tPos,10);
		m_dGamma_Rad = std::stod(mysubstr(i_szLine,tPos,6));
		m_dGamma_Stark = std::stod(mysubstr(i_szLine,tPos,6));
		m_dGamma_vdWaals = std::stod(mysubstr(i_szLine,tPos,6));
		m_szReference = mysubstr(i_szLine,tPos,4);
		m_cLevel_Lower.m_uiNLTE_Index = std::stoi(mysubstr(i_szLine,tPos,2));
		m_cLevel_Upper.m_uiNLTE_Index = std::stoi(mysubstr(i_szLine,tPos,2));
		m_uiIsotope = std::stoi(mysubstr(i_szLine,tPos,3));
		m_dHyperfine_Fractional_Strength = std::stod(mysubstr(i_szLine,tPos,6));
		m_uiIsotope_Diatomics = std::stoi(mysubstr(i_szLine,tPos,3));
		m_dIsotope_Abundance_Fraction = std::stod(mysubstr(i_szLine,tPos,6));
		m_cLevel_Lower.m_iHyperfine_Shift_mK = std::stoi(mysubstr(i_szLine,tPos,5));
		m_cLevel_Upper.m_iHyperfine_Shift_mK = std::stoi(mysubstr(i_szLine,tPos,5));
		tPos += 7;
//		tPos++; // symbol "F" for legibility
//		m_cLevel_Lower.m_iHyperfine_F = std::stoi(mysubstr(i_szLine,tPos,1));
//		tPos++; // Hyperfine note
//		tPos++; // symbol "-" for legibility
//		std::cout << tPos << " " << i_szLine.substr(tPos,1) << std::endl;
//		m_cLevel_Upper.m_iHyperfine_F = std::stoi(mysubstr(i_szLine,tPos,1));
//		tPos++; // Hyperfine note
//		m_uiLine_Strength_Class = std::stoi(mysubstr(i_szLine,tPos,1));
		m_szCode = mysubstr(i_szLine,tPos,3);
		m_uiEven_Lande_g = std::stoi(mysubstr(i_szLine,tPos,5));
		m_uiOdd_Lande_g = std::stoi(mysubstr(i_szLine,tPos,5));
//		std::cout << tPos << " " << i_szLine.substr(tPos,5) << std::endl;
//		m_uiIsotope_Shift_mA = std::stoi(mysubstr(i_szLine,tPos,5));

		m_cLevel_Lower.m_dStat_Weight = 2 * m_cLevel_Lower.m_dJ + 1;
		m_cLevel_Upper.m_dStat_Weight = 2 * m_cLevel_Upper.m_dJ + 1;

		// duplicate element code data into level (handy for later)
		m_cLevel_Lower.m_dElement_Code = m_dElement_Code;
		m_cLevel_Upper.m_dElement_Code = m_dElement_Code;

		 //del E / (4 pi) n2 A  = pi q^2 / (me c) f
		//A = 4 pi^2 q^2 / (me c delE) f
		double dgf = pow(10.0,m_dLog_gf);
		m_dOscillator_Strength = dgf / m_cLevel_Lower.m_dStat_Weight;

		//double dicm_eV = g_XASTRO.k_dh_eV * g_XASTRO.k_dh_h;
		double	dicm_erg = g_XASTRO.k_dhc;
		m_dTransition_Energy_erg = fabs(m_cLevel_Upper.m_dEnergy_Level_cm - m_cLevel_Lower.m_dEnergy_Level_cm) * dicm_erg;
		m_dTransition_Energy_eV = g_XASTRO.k_deV_erg * m_dTransition_Energy_erg;

		m_cLevel_Upper.m_dEnergy_Level_eV = m_cLevel_Upper.m_dEnergy_Level_cm * dicm_erg;
		m_cLevel_Lower.m_dEnergy_Level_eV = m_cLevel_Lower.m_dEnergy_Level_cm * dicm_erg;

		m_cLevel_Upper.m_dEnergy_Level_Ryd = m_cLevel_Upper.m_dEnergy_Level_cm / 109737.31568508;
		m_cLevel_Lower.m_dEnergy_Level_Ryd = m_cLevel_Lower.m_dEnergy_Level_cm / 109737.31568508;

		m_dWavelength_cm = (m_dWavelength_nm * 1.0e-7);
		m_dFrequency_Hz = g_XASTRO.k_dc / m_dWavelength_cm;
//		m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dme * g_XASTRO.k_dc * m_dWavelength_cm * m_dWavelength_cm) * m_dOscillator_Strength *  m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;
//		m_dEinstein_B_SE = m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm  / (8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh)  * m_dEinstein_A;
//		m_dEinstein_B = m_dEinstein_B_SE * m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;

		m_dEinstein_B = 4.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dh * m_dFrequency_Hz * g_XASTRO.k_dme) * m_dOscillator_Strength / 100.0; // I don't know why this factor of 100 is here, but it seems to be needed to get the right order or magnitude for A
		m_dEinstein_B_SE = m_cLevel_Upper.m_dStat_Weight / m_cLevel_Lower.m_dStat_Weight * m_dEinstein_B;
		m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh / (m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm) * m_dEinstein_B_SE;

//		if (m_cLevel_Lower.m_dEnergy_Level_cm == 0.0 && fabs(m_cLevel_Upper.m_dEnergy_Level_cm - 82259.1) < 0.1)
//			std::cout << m_dElement_Code << " " << m_cLevel_Lower.m_dEnergy_Level_cm << "-" << m_cLevel_Upper.m_dEnergy_Level_cm << " " << m_dFrequency_Hz << " " << m_dOscillator_Strength << " " << m_dEinstein_A << " " << m_dEinstein_B << std::endl;

		m_dH_abs = 0.0;
		m_dH_em = 0.0;
	}
	void Compute_Z(const radiation_field & i_cRad, const double & i_dRedshift = 0.0)
	{
		double dEnergy_Flux = Calc_Exciting(i_cRad,i_dRedshift);
		m_dH_abs = m_dEinstein_B * dEnergy_Flux;
		m_dH_em = m_dEinstein_A + m_dEinstein_B_SE * dEnergy_Flux;
//		std::cout << std::fixed << m_dElement_Code << " " << m_cLevel_Lower.m_dEnergy_Level_cm << "-" << m_cLevel_Upper.m_dEnergy_Level_cm << " " << m_dWavelength_cm * 1e8 << " " << std::scientific << m_dH_abs << " " << m_dH_em << " " << m_dTransition_Energy_eV << " " << m_dEinstein_A << " " << m_dEinstein_B << std::endl;
	}

	double Calc_Exciting(const radiation_field & i_cRad, const double &  i_dRedshift = 0.0) const
	{
		//double dWavelength_cm = m_dWavelength_cm * (1.0 + i_dRedshift);

	//	std::cout << dMaximum_wavelength_cm << std::endl;
		lef_data cldRef_Data;
		cldRef_Data.m_lpdField  = &i_cRad;
		cldRef_Data.m_dReference_Frequency_Hz = m_dFrequency_Hz;//g_XASTRO.k_dc / m_dWavelength_cm;
		cldRef_Data.m_dRedshift = i_dRedshift;
		cldRef_Data.m_dGamma = m_cLevel_Upper.m_dGamma + m_cLevel_Lower.m_dGamma;

		double dFreq_Low = cldRef_Data.m_dReference_Frequency_Hz - 2048.0 * cldRef_Data.m_dGamma;
		double dFreq_Hi = cldRef_Data.m_dReference_Frequency_Hz + 2048.0 * cldRef_Data.m_dGamma;

		double dF_n = XM_Simpsons_Integration(Line_Energy_Flux_freq,dFreq_Low,dFreq_Hi,1 << 16,(const void *)&cldRef_Data);
		
		return dF_n;// * g_XASTRO.k_dc / (m_dWavelength_cm * m_dWavelength_cm); // convert from per Hz to per cm
	}

};


typedef std::vector <Kurucz_Line_Data> vkld;
typedef std::vector <Kurucz_Line_Data>::iterator ivkld;
typedef std::vector <ivkld> vivkld;
typedef std::vector <ivkld>::iterator ivivkld;
typedef std::vector <vkld > vvkld;
class level_definition
{
public:
	double			m_dEnergy_Level_cm;
	double			m_dJ;
	level_definition(void){m_dEnergy_Level_cm = -1; m_dJ = -1;}
	level_definition(const double & i_dEnergy_Level_cm, const double & i_dJ){m_dEnergy_Level_cm = i_dEnergy_Level_cm; m_dJ = i_dJ;}
	level_definition(const Kurucz_Level_Data & i_klvdLevel){m_dEnergy_Level_cm = i_klvdLevel.m_dEnergy_Level_cm; m_dJ = i_klvdLevel.m_dJ;}

	inline bool operator ==  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm == m_dEnergy_Level_cm && i_cRHO.m_dJ == m_dJ);
	}
	inline bool operator !=  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm != m_dEnergy_Level_cm && i_cRHO.m_dJ != m_dJ);
	}
	inline bool operator <  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm > m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ > m_dJ));
	}
	inline bool operator <=  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm >= m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ >= m_dJ));
	}
	inline bool operator >  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm < m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ < m_dJ));
	}
	inline bool operator >=  (const level_definition & i_cRHO) const
	{
		return (i_cRHO.m_dEnergy_Level_cm <= m_dEnergy_Level_cm || (m_dEnergy_Level_cm == i_cRHO.m_dEnergy_Level_cm && i_cRHO.m_dJ <= m_dJ));
	}
};

class kurucz_combined_data
{
public:
	Kurucz_Level_Data	klvdLevel_Data;
	vivkld				vivkldAbsorption_Transition_Data;
	vivkld				vivkldEmission_Transition_Data;
};
typedef std::map <level_definition,kurucz_combined_data> mklvd;
typedef std::map <level_definition,kurucz_combined_data>::iterator imklvd;
typedef std::vector <mklvd > vmklvd;


class kurucz_data
{
public:
	unsigned int m_uiN;
	unsigned int m_uiMax_Ion;
	unsigned int m_uiMin_Ion;
	vvkld m_vvkldLine_Data;

	void Load_Data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion);
};

class kurucz_derived_data
{
public:
	vmklvd m_vmklvdLevel_Data;
	kurucz_data	m_kdKurucz_Data;

private:
	enum transition_type {tt_emission, tt_absorption};

	void Process_Level(transition_type i_eType, ivkld &i_ivkldTransition, const Kurucz_Level_Data &i_kvldLevel_Data, mklvd &mklvdCurr_Ion_Level_Data)
	{
		if (mklvdCurr_Ion_Level_Data.count(level_definition(i_kvldLevel_Data)) == 0)
		{
			kurucz_combined_data cdlvData;
			cdlvData.klvdLevel_Data = i_kvldLevel_Data;
			if (i_eType == tt_emission)
				cdlvData.vivkldEmission_Transition_Data.push_back(i_ivkldTransition);
			else
				cdlvData.vivkldAbsorption_Transition_Data.push_back(i_ivkldTransition);
			mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)] = cdlvData;
		}
		else if (mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data != i_kvldLevel_Data) // same label, different data
		{
			std::cout << "Mismatch in " << i_kvldLevel_Data.m_dElement_Code << " " << i_kvldLevel_Data.m_szLabel << " data." << std::endl;
			std::cout << i_kvldLevel_Data.m_dEnergy_Level_cm << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_dEnergy_Level_cm << " " << std::scientific << (i_kvldLevel_Data.m_dEnergy_Level_cm - mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_dEnergy_Level_cm) << std::fixed << std::endl;
			std::cout << i_kvldLevel_Data.m_dJ << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_dJ << std::endl;
			std::cout << i_kvldLevel_Data.m_uiNLTE_Index << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_uiNLTE_Index << std::endl;
			std::cout << i_kvldLevel_Data.m_iHyperfine_Shift_mK << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_iHyperfine_Shift_mK << std::endl;
			std::cout << i_kvldLevel_Data.m_szLabel << " " << mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].klvdLevel_Data.m_szLabel << std::endl;

		}
		else
		{
			if (i_eType == tt_emission)
				mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldEmission_Transition_Data.push_back(i_ivkldTransition);
			else
				mklvdCurr_Ion_Level_Data[level_definition(i_kvldLevel_Data)].vivkldAbsorption_Transition_Data.push_back(i_ivkldTransition);
		}
	}
public:
	kurucz_derived_data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion, const radiation_field & i_rfRad, const double & i_dRedshift)
	{
		m_kdKurucz_Data.Load_Data(i_uiN,i_uiMin_Ion,i_uiMax_Ion);


		// Build list of levels and the associated transitions
		for (vvkld::iterator iterI = m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterI != m_kdKurucz_Data.m_vvkldLine_Data.end(); iterI++)
		{
			mklvd mklvdCurr_Ion_Level_Data;
			double dIon_State = (iterI->begin()->m_dElement_Code - i_uiN) * 100.0; // 0.01 for rounding error
			if (dIon_State <= (i_uiMax_Ion + 0.02))
			{
				for (ivkld iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
				{

					transition_type tLU_Type = tt_absorption;
					transition_type tUL_Type = tt_emission;

					if (iterJ->m_cLevel_Lower > iterJ->m_cLevel_Upper)
					{
						tLU_Type = tt_emission;
						tUL_Type = tt_absorption;
					}
					Process_Level(tLU_Type,iterJ,iterJ->m_cLevel_Lower,mklvdCurr_Ion_Level_Data);
					Process_Level(tUL_Type,iterJ,iterJ->m_cLevel_Upper,mklvdCurr_Ion_Level_Data);
				}
				m_vmklvdLevel_Data.push_back(mklvdCurr_Ion_Level_Data);
			}

		}
		for (vmklvd::iterator iterI = m_vmklvdLevel_Data.begin(); iterI != m_vmklvdLevel_Data.end(); iterI++)
		{
			for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
			{
				// calculate the gamma for each level; this is the sum of Einstein A coefficients for all transitions
				// this is required for determining the width of the Lorentzian wings
				iterJ->second.klvdLevel_Data.m_dGamma = 0.0;
				for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
				{
					iterJ->second.klvdLevel_Data.m_dGamma += (*iterK)->m_dEinstein_A;
				}
				// fill in the data stored in the transition data for this particular level
				for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
				{
					(*iterK)->m_cLevel_Upper.m_dGamma = iterJ->second.klvdLevel_Data.m_dGamma;
				}
				for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
				{
					(*iterK)->m_cLevel_Lower.m_dGamma = iterJ->second.klvdLevel_Data.m_dGamma;
				}
			}
		}
		for (vvkld::iterator iterI = m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterI != m_kdKurucz_Data.m_vvkldLine_Data.end(); iterI++)
		{
			double dIon_State = (iterI->begin()->m_dElement_Code - i_uiN) * 100.0; // 0.01 for rounding error
			if (dIon_State <= (i_uiMax_Ion + 0.02))
			{
				for (ivkld iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
				{
					iterJ->Compute_Z(i_rfRad,i_dRedshift);
				}
			}
		}

		for (vmklvd::iterator iterI = m_vmklvdLevel_Data.begin(); iterI != m_vmklvdLevel_Data.end(); iterI++)
		{
			for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
			{
				// calculate Z (Eq 12) for Level j
				iterJ->second.klvdLevel_Data.m_dZ = 0.0;

				for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
				{
					iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dH_abs;
				
				}
				for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
				{
					iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dH_em;
				}
			}
		}
	}
};
