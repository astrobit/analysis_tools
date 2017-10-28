#pragma once
#include <iostream>
#include <vector>
#include <string>


class config
{
public:
	unsigned int m_uin;
	unsigned int m_uil;
	
	unsigned int m_uiS;
	double		m_dL; // allows for some of the L=1/2 type states
	unsigned int m_uiP;

	config(void)
	{
		m_uin = m_uil = m_uiS = m_uiP = -1;
		m_dL = -1;
	}
	config(unsigned int i_uin, unsigned int i_uil, unsigned int i_uiS = -1, const double &i_dL = -1.0, unsigned int i_uiP = -1)
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


const char * Pass_Integer(const char * i_lpszCursor);
const char * Pass_Whitespace(const char * i_lpszCursor);

void Print_Config_Vector(const std::vector<config> & i_vcConfig, std::ostream & io_out = std::cout);

unsigned int Ang_Mom_Term_To_L(char i_chTerm);

std::vector<config> Get_Default_Configuration(unsigned int i_uiNe);
std::vector<config> Read_Kurucz_State(unsigned int i_uiNe, std::string   i_szLabel_Kurucz);
std::vector<config> Read_OP_State(unsigned int i_uiNe, std::string   i_szLabel_OP);


bool Ion_Recombine_Equivalence(const std::vector<config> &i_Upper_Ion, const std::vector<config> & i_Lower_Ion);


