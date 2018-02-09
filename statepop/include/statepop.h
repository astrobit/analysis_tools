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
//#include <sp_config.h>
#include <velocity_function.h>
#include <unordered_map>
#include <xastroion.h>
#include <algorithm>
#include <xlinalgep.h>
#include <deque>




class statepop
{
public:
	// floating point, vector, and matrix types; defined here for convenience
	typedef long double 			floattype;
	typedef xvector_long 			vector;
	typedef xsquare_matrix_long 	matrix;
public:

	class config
	{
	public:
		unsigned int m_uin;
		unsigned int m_uil;
	
		unsigned int m_uiS;
		unsigned int m_uiL;
		unsigned int m_uiP;
		floattype	m_dJ;
		floattype	m_dK;

		config(void)
		{
			m_uin = m_uil = m_uiS = m_uiP = -1;
			m_uiL = -1;
			m_dJ = -1;
			m_dK = -1;
		}
		config(unsigned int i_uin, unsigned int i_uil, unsigned int i_uiS = -1, const unsigned int &i_uiL = -1, unsigned int i_uiP = -1, const floattype &i_dJ = -1.0, const floattype &i_dK = -1.0)
		{
			m_uin = i_uin;
			m_uil = i_uil;
			m_uiS = i_uiS;
			m_uiL = i_uiL;
			m_uiP = i_uiP;
			m_dJ = i_dJ;
			m_dK = i_dK;
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
		std::deque<config> vccData;

		bool operator == (const vecconfig & i_vccRHO) const;
		size_t size(void) const{return vccData.size();}
		void push_back(const config &i_cConfig){vccData.push_back(i_cConfig);}
		void push_front(const config &i_cConfig){vccData.push_front(i_cConfig);}
		config front(void) const { return vccData.front();}
		config back(void) const { return vccData.back();}
		void pop_front(void) { return vccData.pop_front();}
		void pop_back(void) { return vccData.pop_back();}

		std::deque<config>::iterator begin(void){return vccData.begin();}
		std::deque<config>::iterator end(void){return vccData.end();}
		std::deque<config>::const_iterator cbegin(void) const{return vccData.cbegin();}
		std::deque<config>::const_iterator cend(void) const{return vccData.cend();}
		config & operator[](size_t tIdx){return vccData[tIdx];}
		const config & operator[](size_t tIdx) const{return vccData[tIdx];}
		void clear(void){vccData.clear();}
		bool empty(void){return vccData.empty();}
		void invert(void)
		{
			std::deque<config> qCurr = vccData;
			vccData.clear();
			for (auto iterI = qCurr.rbegin(); iterI != qCurr.rend(); iterI++)
				vccData.push_back(*iterI);
		}
	};
private:
	void Print_Config_Vector(const vecconfig & i_vcConfig, std::ostream & io_out = std::cout);
	vecconfig Get_Default_Configuration(unsigned int i_uiNe);
	vecconfig Read_Kurucz_State(unsigned int i_uiNe, std::string   i_szLabel_Kurucz, const floattype & i_dJ);
	vecconfig Read_OP_State(unsigned int i_uiNe, std::string   i_szLabel_OP);


	bool Ion_Recombine_Equivalence(const vecconfig &i_Upper_Ion, const vecconfig & i_Lower_Ion);

	class state_correlation
	{
	public:
		opacity_project_level_descriptor 				m_opld_Main_State;
		opacity_project_level_descriptor 				m_opld_Ionized_State;
	};

	typedef std::map<opacity_project_level_descriptor, vecconfig > mcfg;
	typedef std::map<size_t, imklvd> mimkvld;
	typedef std::map<size_t, state_correlation > mcorr;


	std::map<size_t, size_t> mapK_OP_State_Correlation;


	opacity_project_level_descriptor Find_Equivalent_Level(const floattype & i_dElement_Code, const floattype & i_lpdEnergy_Level_cm, const floattype & i_dGamma, const std::string & i_sLabel, const mcfg &i_mcfgConfigs, const vecconfig & i_vcfgConfig, bool i_bQuiet = false);

	std::vector<opacity_project_level_descriptor> Find_Equivalent_Recombined_Levels(const mcfg &i_mcfgConfigs, const vecconfig & i_vcfgConfig, bool i_bQuiet = false);

public:
	class param
	{
	public:
		floattype		dN;
		floattype		dNe;
		floattype		dRadiation_Temperature_K;
		floattype		dPhotosphere_Velocity_km_s;
		floattype		dMaterial_Velocity_km_s;
		floattype		dElectron_Kinetic_Temperature_K;
		unsigned int 	uiElement_Z;
		unsigned int 	uiElement_Max_Ion_Species;
		unsigned int 	uiElement_Min_Ion_Species;
		size_t 			tMax_States;
	};
	class transition_data
	{
	public:
		size_t tID;
		floattype ldElement_Code;
		size_t tLower_Level_ID;
		std::string szLower_Level_State;
		floattype ldLower_Level_J;
		size_t tUpper_Level_ID;
		std::string szUpper_Level_State;
		floattype ldUpper_Level_J;
		floattype ldWavenegth_Angstroms;
		floattype ldEinstein_A;
		floattype ldEinstein_B;
		floattype ldEinstein_B_Spontaneous_Emission;
		floattype ldGamma;
		floattype ldH_absorption;
		floattype ldH_emission;
		floattype ldZ;
	};
	class level_data
	{
	public:
		size_t tID;
		floattype ldElement_Code;
		std::string szLabel;
		floattype ldJ;
		floattype ldEnergy_Level_Ryd;
		floattype ldGamma;
		floattype ldZ;
		size_t		tNumber_of_Absorption_Transitions;
		size_t		tNumber_of_Emission_Transitions;
		size_t		tOP_Project_Level_Correlation[4];
		size_t		tOP_Project_Ionization_State[4];
		size_t tIonized_State_ID;

		vecconfig cElectron_Configuration;
		level_data(void)
		{
			tID = -1;
			tOP_Project_Level_Correlation[0] = tOP_Project_Level_Correlation[1] = tOP_Project_Level_Correlation[2] = tOP_Project_Level_Correlation[3] = -1;
			tOP_Project_Ionization_State[0] = tOP_Project_Ionization_State[1] = tOP_Project_Ionization_State[2] = tOP_Project_Ionization_State[3] = -1;
			tIonized_State_ID = -1;
		}
	};
	class saha_info
	{
		public:
		size_t tIon_State;
		floattype ldIonization_Energy_Ryd;
		floattype ldLog_Pop;
		saha_info(void){;}
		saha_info(size_t i_tIon_State, const floattype & i_ldIonization_Energy_Ryd, const floattype & i_ldLog_Pop)
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
		floattype ldLog_Pop;
		boltzmann_info(void){;}
		boltzmann_info(size_t i_tLevel_ID, const floattype & i_ldLog_Pop)
		{
			tLevel_ID = i_tLevel_ID;
			ldLog_Pop = i_ldLog_Pop;
		}
	};

private:
	param 															m_cParam;
	floattype														dRedshift;
	floattype						 								dVelocity_Ratio;
	Planck_radiation_field 											rfPlanck;
	Maxwellian_velocity_function 									vfMaxwell;
	kurucz_derived_data 											kddData;
	opacity_project_element 										opElement;
	std::vector<floattype> 											vdOP_Ground_State;
	std::map<opacity_project_level_descriptor, vecconfig > 			mConfigs;
	mcorr 															mCorrelation;
	mimkvld 														mimkvldKurucz_Correlation;
	size_t 															tMatrix_Order;
	std::vector<floattype> 											vdZ;
	std::map<std::pair<size_t, size_t> , floattype> 				mpdSparse_MatrixBZ;
	std::map<std::pair<size_t, size_t> , floattype> 				mpdSparse_MatrixB;
	matrix				 											smMatrixB;
	std::vector<transition_data> 									vTransition_Data;
	std::vector<level_data> 										vLevel_Data;

	std::map<size_t,kurucz_combined_data>							mapKStates;	 // for easy lookup of Kurucz states by level ID
	//std::map<size_t,Kurucz_Line_Data>								mapKLines;
	std::map<size_t, vecconfig>										vK_Configs;
	std::map<size_t, size_t>										mK_Ionized_States;
	std::map<size_t,opacity_project_level_descriptor>				mapOPStates;	 // for easy lookup of Kurucz states by level ID
	std::map<opacity_project_level_descriptor, size_t>				mapOPStates_Inverse;
	std::map<size_t,size_t>											mapK_OP;	 // for easy lookup of Kurucz states by level ID

	size_t Find_Level_ID(floattype i_ldElement_Code, const std::string & i_szLabel, const floattype & i_dJ, const floattype & i_dEnergy_Level_Ryd)
	{
		size_t tRet = -1;

		for (auto iterI = mapKStates.begin(); iterI != mapKStates.end() && tRet == -1; iterI++)
		{
			if (iterI->second.klvdLevel_Data.m_dElement_Code == i_ldElement_Code &&
				iterI->second.klvdLevel_Data.m_szLabel == i_szLabel &&
				iterI->second.klvdLevel_Data.m_dJ == i_dJ &&
				iterI->second.klvdLevel_Data.m_dEnergy_Level_Ryd == i_dEnergy_Level_Ryd)
				tRet = iterI->first;
		}
		return tRet;

	}
	void Generate_Matrix(void);

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
	void Reset_Param(const param & i_cParam);
	void Reset_Param(const floattype & i_dElectron_Density_cm3, const floattype & i_dRadiation_Temperature_K, const floattype & i_dElectron_Kinetic_Temperature_K, const floattype & i_dMaterial_Velocity_km_s, const floattype & i_dPhotosphere_Velocity_km_s);
	param Get_Param(void) const { return m_cParam;}

		
	vector Get_Populations(void);
	vector Get_Relative_Line_Strengths(void);
	vector Get_Boltzmann_Relative_Line_Strengths(void);
	vector Get_Relaxational_Populations(size_t tNum_Iterations, const vector * i_lpStart_Condition = nullptr);

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
			msqlBZ.Set(iterI->first.second,iterI->first.first,iterI->second);
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
