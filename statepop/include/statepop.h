#pragma once

#include <cfloat>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <xastro.h>
#include <xmath.h>
#include <xstdlib.h>
//#include <arlnsmat.h>
//#include <arlsnsym.h>
#include <radiation.h>
#include <line_routines.h>
#include <kurucz_data.h>
#include <ios>
#include <opacity_project_pp.h>
#include <sp_config.h>
#include <velocity_function.h>
#include <unordered_map>
#include <xastroion.h>
#include <algorithm>
#include <xlinalgep.h>




class statepop
{
public:
	// floating point, vector, and matrix types; defined here for convenience
	typedef long double 			floattype;
	typedef xvector_long 			vector;
	typedef xsquare_matrix_long 	matrix;
private:

	class config
	{
	public:
		unsigned int m_uin;
		unsigned int m_uil;
	
		unsigned int m_uiS;
		long double		m_dL; // allows for some of the L=1/2 type states
		unsigned int m_uiP;

		config(void)
		{
			m_uin = m_uil = m_uiS = m_uiP = -1;
			m_dL = -1;
		}
		config(unsigned int i_uin, unsigned int i_uil, unsigned int i_uiS = -1, const long double &i_dL = -1.0, unsigned int i_uiP = -1)
		{
			m_uin = i_uin;
			m_uil = i_uil;
			m_uiS = i_uiS;
			m_dL = i_dL;
			m_uiP = i_uiP;
			if (m_uil >= m_uin)
				std::cerr << "Invalid state n=" << m_uin << " l=" << m_uil << std::endl;
		}
		inline bool operator != (const config & i_cRHO) const
		{
			if (m_uil >= m_uin)
				std::cerr << "Invalid state n=" << m_uin << " l=" << m_uil << std::endl;
			if (i_cRHO.m_uil >= i_cRHO.m_uin)
				std::cerr << "Invalid state n=" << i_cRHO.m_uin << " l=" << i_cRHO.m_uil << std::endl;
			return (m_uin != i_cRHO.m_uin || m_uil != i_cRHO.m_uil);
		}
		inline bool operator == (const config & i_cRHO) const
		{
			if (m_uil >= m_uin)
				std::cerr << "Invalid state n=" << m_uin << " l=" << m_uil << std::endl;
			if (i_cRHO.m_uil >= i_cRHO.m_uin)
				std::cerr << "Invalid state n=" << i_cRHO.m_uin << " l=" << i_cRHO.m_uil << std::endl;
			return (m_uin == i_cRHO.m_uin && m_uil == i_cRHO.m_uil);
		}

		inline bool operator < (const config & i_cRHO) const
		{
			if (m_uil >= m_uin)
				std::cerr << "Invalid state n=" << m_uin << " l=" << m_uil << std::endl;
			if (i_cRHO.m_uil >= i_cRHO.m_uin)
				std::cerr << "Invalid state n=" << i_cRHO.m_uin << " l=" << i_cRHO.m_uil << std::endl;
			unsigned int uiL = m_uin + m_uil;
			unsigned int uiR = i_cRHO.m_uin + i_cRHO.m_uil;

			return (uiL < uiR) ||
					(uiL == uiR && m_uin < i_cRHO.m_uin);
		}
		inline bool operator <= (const config & i_cRHO) const
		{
			return *this < i_cRHO || *this == i_cRHO;
		}
		inline bool operator > (const config & i_cRHO) const
		{
			return !((*this) <= i_cRHO);
		}
		inline bool operator >= (const config & i_cRHO) const
		{
			return !(*this < i_cRHO);
		}
				
	};
	class vecconfig
	{
	public:
		std::vector<config> vccData;

		bool operator == (const vecconfig & i_vccRHO) const;
		size_t size(void) const{return vccData.size();}
		void push_back(const config &i_cConfig){vccData.push_back(i_cConfig);}
		std::vector<config>::iterator begin(void){return vccData.begin();}
		std::vector<config>::iterator end(void){return vccData.begin();}
		std::vector<config>::const_iterator cbegin(void) const{return vccData.cbegin();}
		std::vector<config>::const_iterator cend(void) const{return vccData.cbegin();}
		config & operator[](size_t tIdx){return vccData[tIdx];}
		const config & operator[](size_t tIdx) const{return vccData[tIdx];}
		void clear(void){vccData.clear();}
		bool empty(void){return vccData.empty();}
	};

	void Print_Config_Vector(const vecconfig & i_vcConfig, std::ostream & io_out = std::cout);
	vecconfig Get_Default_Configuration(unsigned int i_uiNe);
	vecconfig Read_Kurucz_State(unsigned int i_uiNe, std::string   i_szLabel_Kurucz);
	vecconfig Read_OP_State(unsigned int i_uiNe, std::string   i_szLabel_OP);


	bool Ion_Recombine_Equivalence(const vecconfig &i_Upper_Ion, const vecconfig & i_Lower_Ion);

	class state_correlation
	{
	public:
		opacity_project_level_descriptor 				m_opld_Main_State;
		opacity_project_level_descriptor 				m_opld_Ionized_State;
		std::vector<opacity_project_level_descriptor> 	m_vopld_Recombined_States;
	};

	typedef std::map<opacity_project_level_descriptor, vecconfig > mcfg;
	typedef std::map<size_t, imklvd> mimkvld;
	typedef std::map<size_t, state_correlation > mcorr;


	opacity_project_level_descriptor Find_Equivalent_Level(const statepop::floattype & i_dElement_Code, const statepop::floattype & i_lpdEnergy_Level_cm, const statepop::floattype & i_dGamma, const std::string & i_sLabel, const mcfg &i_mcfgConfigs, const vecconfig & i_vcfgConfig, bool i_bQuiet = false);

	std::vector<opacity_project_level_descriptor> Find_Equivalent_Recombined_Levels(const mcfg &i_mcfgConfigs, const vecconfig & i_vcfgConfig, bool i_bQuiet = false);

public:
	class param
	{
	public:
		statepop::floattype		dN;
		statepop::floattype		dNe;
		statepop::floattype		dRadiation_Temperature_K;
		statepop::floattype		dPhotosphere_Velocity_km_s;
		statepop::floattype		dMaterial_Velocity_km_s;
		statepop::floattype		dElectron_Kinetic_Temperature_K;
		unsigned int 	uiElement_Z;
		unsigned int 	uiElement_Max_Ion_Species;
		unsigned int 	uiElement_Min_Ion_Species;
		size_t 			tMax_States;
	};
	class transition_data
	{
	public:
		size_t tID;
		statepop::floattype ldElement_Code;
		size_t tLower_Level_ID;
		std::string szLower_Level_State;
		statepop::floattype ldLower_Level_J;
		std::string szUpper_Level_State;
		statepop::floattype ldUpper_Level_J;
		statepop::floattype ldWavenegth_Angstroms;
		statepop::floattype ldEinstein_A;
		statepop::floattype ldEinstein_B;
		statepop::floattype ldEinstein_B_Spontaneous_Emission;
		statepop::floattype ldGamma;
		statepop::floattype ldH_absorption;
		statepop::floattype ldH_emission;
		statepop::floattype ldZ;
	};
	class level_data
	{
	public:
		size_t tID;
		statepop::floattype ldElement_Code;
		std::string szLabel;
		statepop::floattype ldJ;
		statepop::floattype ldEnergy_Level_Ryd;
		statepop::floattype ldGamma;
		statepop::floattype ldZ;
		size_t		tNumber_of_Absorption_Transitions;
		size_t		tNumber_of_Emission_Transitions;
		size_t		tOP_Project_Level_Correlation[4];
		level_data(void)
		{
			tID = -1;
			tOP_Project_Level_Correlation[0] = tOP_Project_Level_Correlation[1] = tOP_Project_Level_Correlation[2] = tOP_Project_Level_Correlation[3] = -1;
		}
	};
	class saha_info
	{
		public:
		size_t tIon_State;
		statepop::floattype ldIonization_Energy_Ryd;
		statepop::floattype ldLog_Pop;
		saha_info(void){;}
		saha_info(size_t i_tIon_State, const statepop::floattype & i_ldIonization_Energy_Ryd, const statepop::floattype & i_ldLog_Pop)
		{
			tIon_State = i_tIon_State;
			ldIonization_Energy_Ryd = i_ldIonization_Energy_Ryd;
			ldLog_Pop = i_ldLog_Pop;
		}
	};
	class boltzmann_info
	{
		public:
		size_t tLevel_ID;
		statepop::floattype ldLog_Pop;
		boltzmann_info(void){;}
		boltzmann_info(size_t i_tLevel_ID, const statepop::floattype & i_ldLog_Pop)
		{
			tLevel_ID = i_tLevel_ID;
			ldLog_Pop = i_ldLog_Pop;
		}
	};

private:
	param 															m_cParam;
	statepop::floattype														dRedshift;
	Planck_radiation_field 											rfPlanck;
	Maxwellian_velocity_function 									vfMaxwell;
	kurucz_derived_data 											kddData;
	opacity_project_element 										opElement;
	std::vector<statepop::floattype> 										vdOP_Ground_State;
	std::map<opacity_project_level_descriptor, vecconfig > mConfigs;
	mcorr 															mCorrelation;
	mimkvld 														mimkvldKurucz_Correlation;
	size_t 															tMatrix_Order;
	std::vector<statepop::floattype> 										vdZ;
	std::map<std::pair<size_t, size_t> , statepop::floattype> 				mpdSparse_MatrixBZ;
	std::map<std::pair<size_t, size_t> , statepop::floattype> 				mpdSparse_MatrixB;
	matrix				 											smMatrixB;
	std::vector<transition_data> 									vTransition_Data;
	std::vector<level_data> 										vLevel_Data;


public:
	statepop(void)
	{
		dRedshift = 0.0;
		tMatrix_Order = 0;
	}

	size_t Get_Num_Transitions(void) {return vTransition_Data.size();}
	transition_data Get_Transition(size_t i_tIndex){return vTransition_Data[i_tIndex];}
	size_t Get_Num_Levels(void) {return vLevel_Data.size();}
	level_data Get_Level(size_t i_tIndex){return vLevel_Data[i_tIndex];}

	void Set_Param(const param & i_cParam);

		
	vector Get_Populations(void);

	matrix Get_Matrix_B(void)
	{
		return smMatrixB;
	}
	
	matrix Get_Matrix_BZ(void)
	{
		matrix msqlBZ(tMatrix_Order);
		msqlBZ.Zero();
		for (auto iterI = mpdSparse_MatrixBZ.begin(); iterI != mpdSparse_MatrixBZ.end(); iterI++)
		{
			msqlBZ.Set(iterI->first.first,iterI->first.second,iterI->second);
		}
		return msqlBZ;
	}

	////////////////////////////////////////////////////////////////////////
	//
	// Compute Saha ion populations
	//
	////////////////////////////////////////////////////////////////////////

	std::map<size_t,saha_info> Get_Saha_Populations(void);
	std::map<size_t,boltzmann_info> Get_Boltzmann_Populations(void);

};
