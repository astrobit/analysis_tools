#include<cstdio>
#include<cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <lapacke/lapacke.h>
#include <xastro.h>
#include <xmath.h>
#include <xstdlib.h>

class pepn_data
{
public:
	double	dTemperature_K;
	double	dTransition_Wavelength_cm;
};


double Planck_Exciting_Photon_Number(const double & i_dX, const void * i_lpvData)
{
	pepn_data * lpppndData = (pepn_data *)i_lpvData;
	double dTemperature_K = lpppndData[0].dTemperature_K;
	double dFreq = g_XASTRO.k_dc / i_dX;
	double dFreq_Ref = g_XASTRO.k_dc / lpppndData[0].dTransition_Wavelength_cm;
	double dNorm_Delta_Freq = (dFreq - dFreq_Ref) / dFreq_Ref;
	double	dLorentz = 1.0 / (1.0 + dNorm_Delta_Freq * dNorm_Delta_Freq);

	double dPlanck_Coeff = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (i_dX * i_dX * i_dX * i_dX);

	double dExp = 1.0 / (exp(g_XASTRO.k_dhc / (g_XASTRO.k_dKb * dTemperature_K * i_dX)) - 1.0);
	return (dPlanck_Coeff * dExp * dLorentz);
}


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
	double	m_dZ; // Mulligan Z coefficient (M&W 2016(?) Eq. 12)

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

std::string mysubstr(const std::string & i_szStr, size_t &tStart, size_t tLen)
{
	std::string szRet = i_szStr.substr(tStart,tLen);
	tStart += tLen;
	return szRet;
}

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
	double m_dOscillator_Strength; 
	double m_dEinstein_A; // spontaneous emission - transition from upper to lower, in [s^-1]
	double m_dEinstein_B; // absorption - transition from lower to upper, in [s^-1 / (erg s^-1 cm^-2 cm^1)]
	double m_dEinstein_B_SE; // stimulated emission - transition from upper to lower, in [s^-1 / (erg s^-1 cm^-2 cm^1) ]

	double m_dZ_abs;
	double m_dZ_em;

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

		m_dWavelength_cm = (m_dWavelength_nm * 1.0e-7);
		m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dme * g_XASTRO.k_dc * m_dWavelength_cm * m_dWavelength_cm) * m_dOscillator_Strength *  m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;
//		m_dEinstein_B = m_dEinstein_A / (8 * g_XASTRO.k_dpi * m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm * g_XASTRO.k_dh);
//		m_dEinstein_B = m_dEinstein_A / (2 * m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm * g_XASTRO.k_dh * g_XASTRO.k_dc);
		m_dEinstein_B = 2.0 * g_XASTRO.k_dh * g_XASTRO.k_dc * m_dEinstein_A / (m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm);
		m_dEinstein_B_SE = m_dEinstein_B * m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;

		//std::cout << m_dWavelength_nm << " " << m_dIsotope_Abundance_Fraction << std::endl;

		m_dZ_abs = 0.0;
		m_dZ_em = 0.0;
	}
	void Compute_Z(const double & i_dTemp_K, const double & i_dRedshift = 0.0)
	{
		double dPhoton_Flux = Calc_Exciting(i_dTemp_K,i_dRedshift) * m_dTransition_Energy_erg;
		m_dZ_abs = m_dEinstein_B * dPhoton_Flux;
		m_dZ_em = m_dEinstein_A + m_dEinstein_B_SE * dPhoton_Flux;
//		std::cout << std::fixed << m_dElement_Code << " " << m_cLevel_Lower.m_dEnergy_Level_cm << "-" << m_cLevel_Upper.m_dEnergy_Level_cm << " " << m_dWavelength_cm * 1e8 << " " << std::scientific << m_dZ_abs << " " << m_dZ_em << " " << m_dTransition_Energy_eV << " " << m_dEinstein_A << " " << m_dEinstein_B << std::endl;
	}

	double Calc_Exciting(const double & i_dTemp_K, const double & i_dRedshift = 0.0)
	{
		pepn_data cppnData;
		cppnData.dTemperature_K = i_dTemp_K;
		cppnData.dTransition_Wavelength_cm = m_dWavelength_cm * (1.0 + i_dRedshift);

	//	std::cout << dMaximum_wavelength_cm << std::endl;
		double dF_n = XM_Simpsons_Integration(Planck_Exciting_Photon_Number,cppnData.dTransition_Wavelength_cm - 100.0 * 1e-8,cppnData.dTransition_Wavelength_cm + 100.0 * 1e-8,1 << 16,&cppnData);
		return dF_n;
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

class combined_data
{
public:
	Kurucz_Level_Data	klvdLevel_Data;
	vivkld				vivkldAbsorption_Transition_Data;
	vivkld				vivkldEmission_Transition_Data;
};
typedef std::map <level_definition,combined_data> mklvd;
typedef std::map <level_definition,combined_data>::iterator imklvd;
typedef std::vector <mklvd > vmklvd;

enum transition_type {tt_emission, tt_absorption};

void Process_Level(transition_type i_eType, ivkld &i_ivkldTransition, const Kurucz_Level_Data &i_kvldLevel_Data, mklvd &mklvdCurr_Ion_Level_Data)
{
	if (mklvdCurr_Ion_Level_Data.count(level_definition(i_kvldLevel_Data)) == 0)
	{
		combined_data cdlvData;
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


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	vvkld vvkldLine_Data;
	vmklvd vmklvdLevel_Data;
	// inputs
	double	dRadiation_Temperature_K = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--rad-temp", 10000.0);
	double	dPhotosphere_Velocity_kkm_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-vel", 10000.0);
	unsigned int uiElement_Z = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--elem", 48);//20);
	unsigned int uiElement_Max_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--max-ion", 0); //4);
	double	dMaterial_Velocity_kkm_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--mat-vel", 25000.0);

	double	dRedshift = (dMaterial_Velocity_kkm_s - dPhotosphere_Velocity_kkm_s) * 1.0e5 / g_XASTRO.k_dc;
//	double	dMaterial_Density_g_cm3;
	// outputs: state populations

	
	// For element, load all state informtion from the Kurucz lists

	for (unsigned int uiI = 0; uiI <= uiElement_Z; uiI++)
	{
		std::ostringstream ossFilename;
		ossFilename << "share/Kurucz/gf";
//		ossFilename.fill('0');
//		ossFilename.width(2);
		ossFilename << std::setfill('0') << std::setw(2) << uiElement_Z;
		ossFilename << std::setfill('0') << std::setw(2) << uiI;
		ossFilename << ".all";
//		std::cout << ossFilename.str() << std::endl;
		vkld vkldIon_Line_Data;

		FILE * fileElem = fopen(ossFilename.str().c_str(),"rt");
		if (fileElem)
		{
			while (feof(fileElem) == 0)
			{
				char lpszLine[256];
				fgets(lpszLine,sizeof(lpszLine),fileElem);
				Kurucz_Line_Data kldCurr_Line;
				std::string szLine = lpszLine;
				kldCurr_Line.Process_Datafile_Line(szLine);
				kldCurr_Line.Compute_Z(dRadiation_Temperature_K,dRedshift);
				vkldIon_Line_Data.push_back(kldCurr_Line);
			// Load photoionization data for the element
			// @@TODO
			}
			vvkldLine_Data.push_back(vkldIon_Line_Data);
		}
	}
	// Build list of levels and the associated transitions
	for (vvkld::iterator iterI = vvkldLine_Data.begin(); iterI != vvkldLine_Data.end(); iterI++)
	{
		mklvd mklvdCurr_Ion_Level_Data;
		double dIon_State = (iterI->begin()->m_dElement_Code - uiElement_Z) * 100.0; // 0.01 for rounding error
		if (dIon_State <= (uiElement_Max_Ion_Species + 0.02))
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
			vmklvdLevel_Data.push_back(mklvdCurr_Ion_Level_Data);
		}

	}
	
	FILE * fileLevels = fopen("levels.csv","wt");
	if (fileLevels)
		fprintf(fileLevels,"ID, Element code, Level , J, Wavenumber (cm^-1), Z (s^-1)\n");


	unsigned int uiCount = 0;
	for (vmklvd::iterator iterI = vmklvdLevel_Data.begin(); iterI != vmklvdLevel_Data.end(); iterI++)
	{
		std::cout << "Ion " << iterI->begin()->second.klvdLevel_Data.m_dElement_Code << ":" << std::endl;
		for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
		{
			std::cout << "Level " << iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm << " cm^-1 (J = " << iterJ->second.klvdLevel_Data.m_dJ << ") has " << (iterJ->second.vivkldEmission_Transition_Data.size() + iterJ->second.vivkldAbsorption_Transition_Data.size()) << " transitions." << std::endl;

			// calculate Z (Eq 12) for Level j
			iterJ->second.klvdLevel_Data.m_dZ = 0.0;
			for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
			{
				iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dZ_abs;
				
			}
			for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
			{
				iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dZ_em;
			}

			if (fileLevels)
			{
				fprintf(fileLevels,"%i, %.2f, %s, %.1f, %.1f, %.3e, %i, %i\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm,iterJ->second.klvdLevel_Data.m_dZ,iterJ->second.vivkldAbsorption_Transition_Data.size(),iterJ->second.vivkldEmission_Transition_Data.size());
				uiCount++;
			}

		}
	}
	if (fileLevels)
		fclose(fileLevels);

/*	FILE * fileLevelsTrxAbs = fopen("lev-trx-abs.csv","wt");
	if (fileLevelsTrxAbs)
		fprintf(fileLevelsTrxAbs,"Level ID, Element code, Lower Level , Lower Level J, Lower Level Wavenumber (cm^-1), Upper Level, Upper Level J, Upper Level Wavenumber (cm^-1)\n");
	uiCount = 0;
	for (size_t tIdx_Ion_J = 0; tIdx_Ion_J < vmklvdLevel_Data.size(); tIdx_Ion_J++)
	{
		for (imklvd iterSt_J = vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
		{
			for (ivivkld iterK = iterSt_J->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterSt_J->second.vivkldAbsorption_Transition_Data.end() && !bFound; iterK++)
			{
				fprintf(fileLevelsTrxAbs,"%i, %.2f, %s, %.1f, %.1f, %s, %.1f, %.1f\n",uiCount,iterJ->second.klvdLevel_Data.m_dElement_Code,iterJ->second.klvdLevel_Data.m_szLabel.c_str(),iterJ->second.klvdLevel_Data.m_dJ,iterJ->second.klvdLevel_Data.m_dEnergy_Level_cm,(*iterK)->m_szLabel.c_str(),(*iterK)->m_dJ,(*iterK)->m_dEnergy_Level_cm);
			}
			uiCount++;
		}
	}
	if (fileLevelsTrxAbs)
		fclose(fileLevelsTrxAbs);
*/
	//Decide on matrix size based on element
	// for the moment, let's just do this for a single ion
	size_t tMatrix_Order = 0;
	if (vmklvdLevel_Data.size() > 0)
	{
		unsigned int uiNum_Ions = vvkldLine_Data.size();
		for (vmklvd::iterator iterI = vmklvdLevel_Data.begin(); iterI != vmklvdLevel_Data.end(); iterI++)
		{
			tMatrix_Order += iterI->size();
		}
		std::cout << "Matrix size will be " << tMatrix_Order << std::endl;
		// allocate space for matrices to be passed on to LAPACK
		double * lpdA = new double [tMatrix_Order * tMatrix_Order];
		double * lpdB = new double [tMatrix_Order * tMatrix_Order];
		double * lpdE = new double [tMatrix_Order * tMatrix_Order];
		double * lpdLe = new double [tMatrix_Order * tMatrix_Order];
		double * lpdRe = new double [tMatrix_Order * tMatrix_Order];
		double * lpdWork = new double [6 * tMatrix_Order];
		double * lpdL_Scale = new double [tMatrix_Order]; // dggbal output
		double * lpdR_Scale = new double [tMatrix_Order]; // dggbal output
		int		iIlo = 0;
		int 	iIhi = 0;

		// zero output data (just in case)
		memset(lpdL_Scale,0,tMatrix_Order * sizeof(double));
		memset(lpdR_Scale,0,tMatrix_Order * sizeof(double));

		memset(lpdE,0,tMatrix_Order * tMatrix_Order * sizeof(double));
		memset(lpdWork,0,tMatrix_Order * 6 * sizeof(double));
		memset(lpdLe,0,tMatrix_Order * tMatrix_Order * sizeof(double));
		memset(lpdRe,0,tMatrix_Order * tMatrix_Order * sizeof(double));

		// make B the identity matrix
		memset(lpdB,0,tMatrix_Order * tMatrix_Order * sizeof(double));
		for (unsigned int uiI = 0; uiI < tMatrix_Order; uiI++)
		{
			lpdB[uiI * (tMatrix_Order + 1)] = 1.0;
		}

		// zero A
		memset(lpdA,0,tMatrix_Order * tMatrix_Order * sizeof(double));
		// generate A (see Mulligan & Wheeler 2016, Eq 12-14

		size_t tIdx_I = 0; // column
		size_t tIdx_J = 0; // row
		size_t tIdx_Total = 0;
		for (size_t tIdx_Ion_J = 0; tIdx_Ion_J < vmklvdLevel_Data.size(); tIdx_Ion_J++)
		{
			//for (size_t tIdx_State_J = 0; tIdx_State_J < vmklvdLevel_Data[tIdx_Ion_J].size(); tIdx_State_J++)
			for (imklvd iterSt_J = vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
			{
				for (size_t tIdx_Ion_I = 0; tIdx_Ion_I < vmklvdLevel_Data.size(); tIdx_Ion_I++)
				{
					for (imklvd iterSt_I = vmklvdLevel_Data[tIdx_Ion_I].begin(); iterSt_I != vmklvdLevel_Data[tIdx_Ion_I].end(); iterSt_I++)
//					for (size_t tIdx_State_I = 0; tIdx_State_I < vmklvdLevel_Data[tIdx_Ion_I].size(); tIdx_State_I++;)
					{
						// calculate Z (Eq 12) for Level i

						// -- processing section
						if (tIdx_I != tIdx_J)
						{
							if (tIdx_Ion_J == tIdx_Ion_I) // same ion
							{
								double dH = 0.0;
								bool bFound = false;
								if (tIdx_I > tIdx_J)//iterSt_J->second.klvdLevel_Data < iterSt_I->second.klvdLevel_Data)
								{
								// first find the transition in the list for j
									for (ivivkld iterK = iterSt_J->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterSt_J->second.vivkldAbsorption_Transition_Data.end() && !bFound; iterK++)
									{
//										if ((*iterK)->m_cLevel_Lower == iterSt_J->second.klvdLevel_Data &&
//											(*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data)
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH  = (*iterK)->m_dZ_abs;//(*iterK)->m_dEinstein_B * (*iterK)->Calc_Exciting(dRadiation_Temperature_K,dRedshift);
											bFound = true;
										}
				
									}
								}
								else //if (vmklvdLevel_Data[tIdx_Ion_J].klvdLevel_Data > vmklvdLevel_Data[tIdx_Ion_I].klvdLevel_Data)
								{
								// first find the transition in the list for j
									for (ivivkld iterK = iterSt_J->second.vivkldEmission_Transition_Data.begin(); iterK != iterSt_J->second.vivkldEmission_Transition_Data.end() && !bFound; iterK++)
									{
//										if ((*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data &&
//											(*iterK)->m_cLevel_Upper == iterSt_J->second.klvdLevel_Data)
										if ((*iterK)->m_cLevel_Upper == iterSt_I->second.klvdLevel_Data || (*iterK)->m_cLevel_Lower == iterSt_I->second.klvdLevel_Data)
										{
											dH = (*iterK)->m_dZ_em;//(*iterK)->m_dEinstein_A + (*iterK)->m_dEinstein_B_SE * (*iterK)->Calc_Exciting(dRadiation_Temperature_K,dRedshift);
											bFound = true;
										}
				
									}
								}
								if (bFound)
									lpdA[tIdx_Total] = dH / iterSt_I->second.klvdLevel_Data.m_dZ;
								if (tIdx_I == 1 && bFound)
									std::cout << tIdx_J << " " << dH << std::endl;

							}
							else if (tIdx_Ion_J == (tIdx_Ion_I - 1)) // potential recombination term
							{
								//@@TODO 
								//lpdA[tIdx_Total] = ....
							}
							else if (tIdx_Ion_J == (tIdx_Ion_I + 1)) // potential photoionization term
							{
								//@@TODO 
								//lpdA[tIdx_Total] = ....
							}
							// else H = 0 (handled by memset above)
							// -- end of processing section
						}
						tIdx_I++;
						tIdx_Total++;
					}
				}
				tIdx_J++;
				tIdx_I = 0; // new row
			}
		}
		
		FILE * fileMatA = fopen("matrixa.csv","wt");

		if (fileMatA)
		{
			size_t tIdx = 0;
			for (size_t tIdx_I = 0; tIdx_I < tMatrix_Order; tIdx_I++)// column
			{
				for (size_t tIdx_J = 0; tIdx_J < tMatrix_Order; tIdx_J++) // row
				{
					if (tIdx_J > 0)
						fprintf(fileMatA,", ");
					fprintf(fileMatA,"%.17e",lpdA[tIdx]);
					tIdx++;
				}
				fprintf(fileMatA,"\n");
			}
			fclose(fileMatA);
		}

		int iMatrix_Order_lapack = (int)tMatrix_Order;


		int iSuccess = LAPACKE_dggbal( LAPACK_COL_MAJOR , 'B', iMatrix_Order_lapack, lpdA,
                           iMatrix_Order_lapack, lpdB, iMatrix_Order_lapack,
                           &iIlo, &iIhi, lpdL_Scale, lpdR_Scale);

		if (iSuccess == 0)
		{
			char chJob = 'B';
			char chSide = 'R';
			int iNum_E = 0;
			LAPACK_dtgevc(
					&chSide, &chJob, nullptr,
                    &iMatrix_Order_lapack, lpdA, &iMatrix_Order_lapack,
                    lpdB, &iMatrix_Order_lapack, lpdLe,
                    &iMatrix_Order_lapack, lpdRe, &iMatrix_Order_lapack,
                    &iMatrix_Order_lapack, &iNum_E, lpdWork,&iSuccess );

			if (iSuccess == 0)
			{
			
				LAPACK_dggbak( &chJob, &chSide, &iMatrix_Order_lapack, &iIlo,
		                &iIhi, lpdL_Scale, lpdR_Scale,
		                &iNum_E, lpdLe, &iMatrix_Order_lapack,
		                &iSuccess );
				if (iSuccess == 0)
				{
					unsigned int uiJ = 0;
					for (size_t tIdx_Ion_J = 0; tIdx_Ion_J < vmklvdLevel_Data.size(); tIdx_Ion_J++)
					{
						//for (size_t tIdx_State_J = 0; tIdx_State_J < vmklvdLevel_Data[tIdx_Ion_J].size(); tIdx_State_J++)
						for (imklvd iterSt_J = vmklvdLevel_Data[tIdx_Ion_J].begin(); iterSt_J != vmklvdLevel_Data[tIdx_Ion_J].end(); iterSt_J++)
						{
							std::cout << std::fixed << std::setw(4) << iterSt_J->second.klvdLevel_Data.m_dElement_Code << " " << iterSt_J->second.klvdLevel_Data.m_szLabel << " " << std::fixed << std::setw(2) << iterSt_J->second.klvdLevel_Data.m_dJ << " " << std::scientific << lpdLe[uiJ];
							std::cout << " " << iIlo;
							std::cout << " " << iIhi;
							std::cout << " " << iNum_E;
							std::cout << std::endl;
							uiJ++;
						}
						std::cout << "dggbak" << std::endl;
						for (unsigned int uiI = 0; uiI < iMatrix_Order_lapack * iMatrix_Order_lapack; uiI++)
							std::cout << uiI << " " << std::scientific << lpdA[uiI] << " " << lpdB[uiI] << " " << lpdL_Scale[uiI] << " " << lpdR_Scale[uiI] << std::endl;
					}
				}
				else
				{
					std::cerr << "Matrix get eigenvectors (LAPACK_dggbak) failed at element " << -iSuccess << "." << std::endl;
				}
			}
			else
			{
				std::cerr << "Matrix get eigenvectors (LAPACK_dtgevc) failed at element " << -iSuccess << "." << std::endl;
			}
		}
		else
		{
			std::cerr << "Matrix balance failed at element " << -iSuccess << "." << std::endl;
		}

	}
	return 0;
}
