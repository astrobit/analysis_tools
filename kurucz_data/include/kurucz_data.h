#pragma once
#include <xastro.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <radiation.h>
#include <iostream>
#include <iomanip>

class Kurucz_Level_Data
{
public:
	// Data from Kurucz table
	long double 	m_dEnergy_Level_cm;
	bool			m_bEnergy_Level_Extrapolated_Predicted; // flag to indicate levels that are predicted or extraploated rather than known
	long double 	m_dJ;
	std::string 	m_szLabel;
	unsigned int 	m_uiNLTE_Index;
	int				m_iHyperfine_Shift_mK;
//	int m_iHyperfine_F;	

	// derived data
	long double		m_dStat_Weight;
	long double		m_dZ; // Z coefficient (M&W 2016(?) Eq. 12)
	long double 	m_dEnergy_Level_eV;
	long double 	m_dEnergy_Level_Ryd;

	long double 	m_dGamma; // sum of spontaneous emission coefficents to all lower levels

	// duplicate data
	long double 	m_dElement_Code;
	unsigned int 	m_uiZ; // atomic number
	unsigned int 	m_uiIon_State; // 0 = neutral, 1 = singly ionized, 2 = doubly ionized, etc.

	bool operator == (const Kurucz_Level_Data & i_cRHO) const;
	inline bool operator != (const Kurucz_Level_Data & i_cRHO) const 
	{
		return !(*this == i_cRHO);
	}
	bool operator <  (const Kurucz_Level_Data & i_cRHO) const;
	inline bool operator <=  (const Kurucz_Level_Data & i_cRHO) const
	{
		return (*this == i_cRHO) || (*this < i_cRHO);
	}
	inline bool operator >  (const Kurucz_Level_Data & i_cRHO) const
	{
		return !(*this <= i_cRHO);
	}
	inline bool operator >=  (const Kurucz_Level_Data & i_cRHO) const
	{
		return !(*this < i_cRHO);
	}
};




class Kurucz_Line_Data
{
public:
	// data from the file
	long double 	m_dWavelength_nm;
	long double 	m_dLog_gf;
	long double 	m_dElement_Code;
	Kurucz_Level_Data	m_cLevel_Lower;
	Kurucz_Level_Data	m_cLevel_Upper;
	long double		m_dGamma_Rad;
	long double		m_dGamma_Stark;
	long double		m_dGamma_vdWaals;
	std::string		m_szReference;
	unsigned int 	m_uiIsotope;
	long double		m_dHyperfine_Fractional_Strength;
	unsigned int 	m_uiIsotope_Diatomics;
	long double		m_dIsotope_Abundance_Fraction;
	unsigned int 	m_uiLine_Strength_Class;
	std::string		m_szCode;
	int 			m_uiEven_Lande_g;
	int m_uiOdd_Lande_g;
	int m_uiIsotope_Shift_mA;

	// derived / calculated data
	long double 	m_dTransition_Energy_erg;
	long double 	m_dTransition_Energy_eV;
	long double 	m_dWavelength_cm;
	long double 	m_dFrequency_Hz;
	long double 	m_dOscillator_Strength; 
	long double 	m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	long double 	m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	long double 	m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

	long double 	m_dH_abs; // H coefficient (M&W 2017, Eq. 13) for j < i
	long double 	m_dH_em; // H coefficient (M&W 2017, Eq. 13) for j > i
	long double 	m_dH_StE;
private:
	std::string mysubstr(const std::string & i_szStr, size_t &tStart, size_t tLen) const;
	void Fix_Label(std::string & io_szLabel) const;

public:

	void Process_Datafile_Line(const std::string & i_szLine);
	void Compute_Z(const radiation_field & i_cRad, const long double & i_dRedshift = 0.0, const long double & i_dFlux_Dilution = 1.0);
	long double Calc_Exciting(const radiation_field & i_cRad, const long double &  i_dRedshift = 0.0) const;
};


typedef std::vector <Kurucz_Line_Data> vkld;
typedef std::vector <Kurucz_Line_Data>::iterator ivkld;
typedef std::vector <ivkld> vivkld;
typedef std::vector <ivkld>::iterator ivivkld;
typedef std::vector <vkld > vvkld;
class level_definition
{
public:
	long double			m_dEnergy_Level_cm;
	long double			m_dJ;
	size_t				m_tS;
	size_t				m_tL;
	size_t				m_tP;
	long double			m_dK;
	level_definition(void){m_dEnergy_Level_cm = -1; m_dJ = -1; m_tS = m_tL = m_tP = -1; m_dK = -1;}
//	level_definition(const long double & i_dEnergy_Level_cm, const long double & i_dJ, size_t i_tS, size_t i_tL){m_dEnergy_Level_cm = i_dEnergy_Level_cm; m_dJ = i_dJ; m_tS = i_tS; m_tL = i_tL; m_dK = -1.0; m_tP = -1;}
//	level_definition(const long double & i_dEnergy_Level_cm, const long double & i_dJ, const long double & i_dK){m_dEnergy_Level_cm = i_dEnergy_Level_cm; m_dJ = i_dJ; m_dK = i_dK;m_tS = m_tL = -1; m_tP = -1;}
	level_definition(const Kurucz_Level_Data & i_klvdLevel);

	bool operator ==  (const level_definition & i_cRHO) const;
	inline bool operator !=  (const level_definition & i_cRHO) const
	{
		return !(*this == i_cRHO);
	}
	bool operator <  (const level_definition & i_cRHO) const;
	inline bool operator <=  (const level_definition & i_cRHO) const
	{
		return (*this < i_cRHO || *this == i_cRHO);
	}
	inline bool operator >  (const level_definition & i_cRHO) const
	{
		return !(*this <= i_cRHO);
	}
	inline bool operator >=  (const level_definition & i_cRHO) const
	{
		return !(*this < i_cRHO);
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
	unsigned int 	m_uiN;
	unsigned int 	m_uiMax_Ion;
	unsigned int 	m_uiMin_Ion;
	vvkld 			m_vvkldLine_Data;

	void Load_Data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion);
	void	clear(void)
	{
		m_vvkldLine_Data.clear();
	}
};

class kurucz_derived_data
{
public:
	vmklvd 			m_vmklvdLevel_Data;
	kurucz_data		m_kdKurucz_Data;

private:
	enum transition_type {tt_emission, tt_absorption};

	void Process_Level(transition_type i_eType, ivkld &i_ivkldTransition, const Kurucz_Level_Data &i_kvldLevel_Data, mklvd &mklvdCurr_Ion_Level_Data);
public:
	kurucz_derived_data(void){;}
	kurucz_derived_data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion)
	{
		Initialize(i_uiN, i_uiMin_Ion, i_uiMax_Ion);
	}

	void clear(void)
	{
		m_vmklvdLevel_Data.clear();
		m_kdKurucz_Data.clear();
	}

	void Initialize(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion);
	void Compute_H_Z(const radiation_field & i_rfRad, const long double & i_dRedshift, const long double & i_dFlux_Dilution);
};
