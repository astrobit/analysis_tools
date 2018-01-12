#include <kurucz_data.h>

void Kurucz_Line_Data::Process_Datafile_Line(const std::string & i_szLine)
{
	size_t tPos = 0;
	m_dWavelength_nm = std::stod(mysubstr(i_szLine,tPos,11));
	m_dLog_gf = std::stod(mysubstr(i_szLine,tPos,7));
	m_dElement_Code = std::stod(mysubstr(i_szLine,tPos,6));
	m_cLevel_Lower.m_dEnergy_Level_cm = std::stod(mysubstr(i_szLine,tPos,12));
	m_cLevel_Lower.m_dJ = std::stod(mysubstr(i_szLine,tPos,5));
	tPos++; // blank space
	m_cLevel_Lower.m_szLabel = mysubstr(i_szLine,tPos,10);
	Fix_Label(m_cLevel_Lower.m_szLabel);

	m_cLevel_Upper.m_dEnergy_Level_cm = std::stod(mysubstr(i_szLine,tPos,12));
	m_cLevel_Upper.m_bEnergy_Level_Extrapolated_Predicted = (m_cLevel_Upper.m_dEnergy_Level_cm < 0);
	if (m_cLevel_Upper.m_bEnergy_Level_Extrapolated_Predicted)
		m_cLevel_Upper.m_dEnergy_Level_cm = std::fabs(m_cLevel_Upper.m_dEnergy_Level_cm);
	
	m_cLevel_Upper.m_dJ = std::stod(mysubstr(i_szLine,tPos,5));
	tPos++; // blank space
	m_cLevel_Upper.m_szLabel = mysubstr(i_szLine,tPos,10);
	Fix_Label(m_cLevel_Upper.m_szLabel);
	m_dGamma_Rad = std::stod(mysubstr(i_szLine,tPos,6));
	m_dGamma_Stark = std::stod(mysubstr(i_szLine,tPos,6));
	m_dGamma_vdWaals = std::stod(mysubstr(i_szLine,tPos,6));
	m_szReference = mysubstr(i_szLine,tPos,4);
	m_cLevel_Lower.m_uiNLTE_Index = std::stoi(mysubstr(i_szLine,tPos,2)); // the n value for levels when there are hyperfine transitions and the indicated level is the ensemble average
	m_cLevel_Upper.m_uiNLTE_Index = std::stoi(mysubstr(i_szLine,tPos,2));
	m_uiIsotope = std::stoi(mysubstr(i_szLine,tPos,3));
	m_dHyperfine_Fractional_Strength = std::stod(mysubstr(i_szLine,tPos,6));
	m_uiIsotope_Diatomics = std::stoi(mysubstr(i_szLine,tPos,3));
	m_dIsotope_Abundance_Fraction = std::stod(mysubstr(i_szLine,tPos,6));
	m_cLevel_Lower.m_iHyperfine_Shift_mK = std::stoi(mysubstr(i_szLine,tPos,5));
	m_cLevel_Upper.m_iHyperfine_Shift_mK = std::stoi(mysubstr(i_szLine,tPos,5));
	tPos += 7;

	// use the hyperfine level as state information
	if (m_cLevel_Lower.m_szLabel == "AVERAGE   " && m_cLevel_Upper.m_szLabel == "ENERGIES  ")
	{	// note: Hydrogen and He will still cause problems in level identification because of isotope information
		char lpszn[3];
		sprintf(lpszn,"%i",m_cLevel_Lower.m_uiNLTE_Index);
		m_cLevel_Lower.m_szLabel = lpszn;
		sprintf(lpszn,"%i",m_cLevel_Upper.m_uiNLTE_Index);
		m_cLevel_Upper.m_szLabel = lpszn;
	}
	else if	(m_cLevel_Upper.m_szLabel == "CONTINUUM ")
	{	// will usually be 99
		char lpszn[3];
		sprintf(lpszn,"%i",m_cLevel_Upper.m_uiNLTE_Index);
		m_cLevel_Upper.m_szLabel = lpszn;
	}
	// strip '?' from level label
	size_t tQpos;
	if ((tQpos = m_cLevel_Lower.m_szLabel.find('?')) != std::string::npos)
		m_cLevel_Lower.m_szLabel[tQpos] = ' ';
	if ((tQpos = m_cLevel_Upper.m_szLabel.find('?')) != std::string::npos)
		m_cLevel_Upper.m_szLabel[tQpos] = ' ';
		
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
	m_cLevel_Lower.m_uiZ = floor(m_dElement_Code);
	m_cLevel_Lower.m_uiIon_State = fmod(m_dElement_Code,1.0) * 100.0;

	m_cLevel_Upper.m_dElement_Code = m_dElement_Code;
	m_cLevel_Upper.m_uiZ = floor(m_dElement_Code);
	m_cLevel_Upper.m_uiIon_State = fmod(m_dElement_Code,1.0) * 100.0;

	if (m_cLevel_Upper.m_dEnergy_Level_cm < m_cLevel_Lower.m_dEnergy_Level_cm)
	{
		Kurucz_Level_Data cSwap = m_cLevel_Lower;
		m_cLevel_Lower = m_cLevel_Upper;
		m_cLevel_Upper = cSwap;
	}

	 //del E / (4 pi) n2 A  = pi q^2 / (me c) f
	//A = 4 pi^2 q^2 / (me c delE) f
	long double dgf = pow(10.0,m_dLog_gf);
	m_dOscillator_Strength = dgf / m_cLevel_Lower.m_dStat_Weight;

	//double dicm_eV = g_XASTRO.k_dh_eV * g_XASTRO.k_dh_h;
	long double	dicm_erg = g_XASTRO.k_dhc;
	m_dTransition_Energy_erg = fabs(m_cLevel_Upper.m_dEnergy_Level_cm - m_cLevel_Lower.m_dEnergy_Level_cm) * dicm_erg;
	m_dTransition_Energy_eV = g_XASTRO.k_deV_erg * m_dTransition_Energy_erg;

	m_cLevel_Upper.m_dEnergy_Level_eV = m_cLevel_Upper.m_dEnergy_Level_cm * dicm_erg;
	m_cLevel_Lower.m_dEnergy_Level_eV = m_cLevel_Lower.m_dEnergy_Level_cm * dicm_erg;

	m_cLevel_Upper.m_dEnergy_Level_Ryd = m_cLevel_Upper.m_dEnergy_Level_cm / g_XASTRO.k_dRy_cmm1;
	m_cLevel_Lower.m_dEnergy_Level_Ryd = m_cLevel_Lower.m_dEnergy_Level_cm / g_XASTRO.k_dRy_cmm1;

	m_dWavelength_cm = (m_dWavelength_nm * 1.0e-7);
	m_dFrequency_Hz = g_XASTRO.k_dc / m_dWavelength_cm;
//		m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dme * g_XASTRO.k_dc * m_dWavelength_cm * m_dWavelength_cm) * m_dOscillator_Strength *  m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;
//		m_dEinstein_B_SE = m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm  / (8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh)  * m_dEinstein_A;
//		m_dEinstein_B = m_dEinstein_B_SE * m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight;

	// BWM 2017-Nov-15: modified B and B_SE to match definition in Allen's (1976, p58)
	m_dEinstein_B = g_XASTRO.k_dpi * g_XASTRO.k_de * g_XASTRO.k_de / (g_XASTRO.k_dh * m_dFrequency_Hz * g_XASTRO.k_dme) * m_dOscillator_Strength;
	m_dEinstein_B_SE = m_cLevel_Lower.m_dStat_Weight / m_cLevel_Upper.m_dStat_Weight * m_dEinstein_B; // B21 = g1/g2 B12
	m_dEinstein_A = 8.0 * g_XASTRO.k_dpi * g_XASTRO.k_dh / (m_dWavelength_cm * m_dWavelength_cm * m_dWavelength_cm) * m_dEinstein_B_SE;

//		if (m_cLevel_Lower.m_dEnergy_Level_cm == 0.0 && fabs(m_cLevel_Upper.m_dEnergy_Level_cm - 82259.1) < 0.1)
//			std::cout << m_dElement_Code << " " << m_cLevel_Lower.m_dEnergy_Level_cm << "-" << m_cLevel_Upper.m_dEnergy_Level_cm << " " << m_dFrequency_Hz << " " << m_dOscillator_Strength << " " << m_dEinstein_A << " " << m_dEinstein_B << std::endl;

	m_dH_abs = 0.0;
	m_dH_em = 0.0;
	m_dH_StE = 0.0;
}
