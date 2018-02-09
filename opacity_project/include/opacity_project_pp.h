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

	opacity_project_level_descriptor(void);
	opacity_project_level_descriptor(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID);

	bool operator > (const opacity_project_level_descriptor & i_cRHO) const;
	bool operator >= (const opacity_project_level_descriptor & i_cRHO) const;
	bool operator < (const opacity_project_level_descriptor & i_cRHO) const;
	bool operator <= (const opacity_project_level_descriptor & i_cRHO) const;
	bool operator == (const opacity_project_level_descriptor & i_cRHO) const;
	bool operator != (const opacity_project_level_descriptor & i_cRHO) const;
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

	opacity_project_state(void);
	opacity_project_state(unsigned int i_uiZ, unsigned int i_uiN, unsigned int i_uiS, unsigned int i_uiL, unsigned int i_uiP, unsigned int i_uiLvl_ID, long double i_dEnergy_eV = 0.0);

	bool operator < (const opacity_project_state & i_cRHO) const;
	bool operator <= (const opacity_project_state & i_cRHO) const;
	bool operator > (const opacity_project_state & i_cRHO) const;
	bool operator >= (const opacity_project_state & i_cRHO) const;
	bool operator == (const opacity_project_state & i_cRHO) const;
	bool operator != (const opacity_project_state & i_cRHO) const;


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

	void Calculate_Derived_Quantities(const opacity_project_state & i_sState_Lower, const opacity_project_state & i_sState_Upper);

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
	
	opacity_project_state Find_State(const opacity_project_level_descriptor &i_opldDescriptor) const;
	opacity_project_state Find_State(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const;

	long double Calc_Ionizing_Rate(const opacity_project_state &i_sState, const radiation_field & i_cRad, const long double & i_dRescale_Energy_Ry = -1.0, const long double &  i_dRedshift = 0.0) const;
	long double Calc_Recombination_Rate(const opacity_project_state &i_sIonized_State, const opacity_project_state &i_sRecombined_State, const velocity_function & i_cVel_Fn, const long double & i_dRescale_Energy_Ry = -1.0) const;
private:

	msmst Read_Transition_Data(void);
	ms Read_State_Data(void);
	mscs Read_Opacity_PI_Data(void);
	void Read_Configuration_Data(void);
public:
	void Read_Data(unsigned int i_uiZ, unsigned int i_uiN);
	opacity_project_ion(void);
	opacity_project_ion(unsigned int i_uiZ, unsigned int i_uiN);
	std::string Get_State_Configuration(unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const;
};

class opacity_project_element
{
public:
	unsigned int m_uiZ;
	std::vector<opacity_project_ion> m_vopiIon_Data;
public:
	void clear(void);

	void Read_Element_Data(unsigned int i_uiZ);
	opacity_project_state Find_State(const opacity_project_level_descriptor & i_poldLevel) const;
	opacity_project_state Find_State(unsigned int i_uiN, unsigned int i_uiState_S, unsigned int i_uiState_L, unsigned int i_uiState_P, unsigned int i_uiID) const;

	long double Get_Ionization_Rate(const opacity_project_level_descriptor &i_opld_State, const long double & i_dCorrected_Energy_Ry, const radiation_field & i_cRad, const long double &  i_dRedshift = 0.0) const;
	long double Get_Recombination_Rate(const opacity_project_level_descriptor &i_opld_Ionized_State, const opacity_project_level_descriptor &i_opld_Recombined_State, const long double & i_dCorrected_Energy_Ry, const velocity_function & i_cVel) const;

	opacity_project_transition Get_Transition(const opacity_project_level_descriptor &i_opld_Lower_State, const opacity_project_level_descriptor &i_opld_Upper_State) const;

};

