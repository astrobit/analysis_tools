#include <xastro.h>
#include <xmath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

class pipn_data
{
public:
	double	dTemperature;
	double	dReference_Wavelength;
};

double Planck_Ionizing_Photon_Number(const double & i_dX, const void * i_lpvData)
{
	pipn_data * lpppndData = (pipn_data *)i_lpvData;
	double dTemperature = lpppndData[0].dTemperature;
	double dRel_Wl = i_dX / lpppndData[0].dReference_Wavelength;
	if (dRel_Wl > 1.0)
		dRel_Wl = 0.0; // 
	double dBFCS  = dRel_Wl * dRel_Wl * dRel_Wl;

	//double dPlanck_Coeff = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (i_dX * i_dX * i_dX * i_dX);

	double dPlanck_Coeff_BFCS = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (i_dX * lpppndData[0].dReference_Wavelength * lpppndData[0].dReference_Wavelength * lpppndData[0].dReference_Wavelength);
	double dExp = 1.0 / (exp(g_XASTRO.k_dhc / g_XASTRO.k_dKb / dTemperature / i_dX) - 1.0);
	return (dPlanck_Coeff_BFCS * dExp); //(dPlanck_Coeff * dExp * dBFCS);
}

double Calc_Ionizing(const double & i_dIon_Pot_eV, const double & i_dTemp)
{
	pipn_data cppnData;
	double	dMaximum_wavelength_cm = g_XASTRO.k_dhc / (i_dIon_Pot_eV * g_XASTRO.k_derg_eV);
	cppnData.dTemperature = i_dTemp;
	cppnData.dReference_Wavelength = dMaximum_wavelength_cm;
//	std::cout << dMaximum_wavelength_cm << std::endl;
	double dRadius = 1.0e15;
	double dArea = 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;

	double dF_n = XM_Simpsons_Integration(Planck_Ionizing_Photon_Number,1.0e-8,dMaximum_wavelength_cm,1 << 24,&cppnData);
	return dF_n * dArea;
}

class pepn_data
{
public:
	double	dTemperature;
	double	dTransition_Wavelength_cm;
	double	dGamma_lower;
	double	dGamma_upper;
};
double Planck_Exciting_Photon_Number(const double & i_dX, const void * i_lpvData)
{
	pepn_data * lpppndData = (pepn_data *)i_lpvData;
	double dTemperature = lpppndData[0].dTemperature;
	double dFreq = g_XASTRO.k_dc / i_dX;
	double dFreq_Ref = g_XASTRO.k_dc / lpppndData[0].dTransition_Wavelength_cm;
	double dGamma_1 = lpppndData[0].dGamma_lower;
	double dGamma_2 = lpppndData[0].dGamma_upper;
	double dDelta_Freq = (dFreq - dFreq_Ref);

	double dPhi_1 = 1.0 / (dDelta_Freq * dDelta_Freq / dGamma_1 * 4.0 * g_XASTRO.k_dpi + 1);
	double dPhi_2 = 1.0 / (dDelta_Freq * dDelta_Freq / dGamma_2 * 4.0 * g_XASTRO.k_dpi + 1);

	double dPlanck_Coeff = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (i_dX * i_dX * i_dX * i_dX);

	double dExp = 1.0 / (exp(g_XASTRO.k_dhc / (g_XASTRO.k_dKb * dTemperature * i_dX)) - 1.0);
	return (dPlanck_Coeff * dExp * (dPhi_1 + dPhi_2));
}

double Calc_Exciting(const double & i_dTransition_energy_eV, const double & i_dTemp, const double & i_dGamma_lower, const double & i_dGamma_upper)
{
	pepn_data cppnData;
	double	dWavelength_cm = g_XASTRO.k_dhc / (i_dTransition_energy_eV * g_XASTRO.k_derg_eV);
	cppnData.dTemperature = i_dTemp;
	cppnData.dTransition_Wavelength_cm = dWavelength_cm;
	cppnData.dGamma_lower = i_dGamma_lower;
	cppnData.dGamma_upper = i_dGamma_upper;

//	std::cout << dMaximum_wavelength_cm << std::endl;
	double dRadius = 1.0e15;
	double dArea = 4.0 * g_XASTRO.k_dpi * dRadius * dRadius;

	double dF_n = XM_Simpsons_Integration(Planck_Exciting_Photon_Number,cppnData.dTransition_Wavelength_cm - 100.0 * 1e-8,cppnData.dTransition_Wavelength_cm + 100.0 * 1e-8,1 << 24,&cppnData);
	return dF_n * dArea;
}

double Calc_Exciting_Simple(const double & i_dTransition_energy_eV, const double & i_dTemp)
{ // basically assume photons within 2% of the central frequency are absorbed
	double	dWavelength_cm = g_XASTRO.k_dhc / (i_dTransition_energy_eV * g_XASTRO.k_derg_eV);

	double dPlanck_Coeff = 2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dc / (dWavelength_cm * dWavelength_cm * dWavelength_cm * dWavelength_cm);

	double dExp = 1.0 / (exp(g_XASTRO.k_dhc / (g_XASTRO.k_dKb * i_dTemp * dWavelength_cm)) - 1.0);
	return (dPlanck_Coeff * dExp * 0.02 * dWavelength_cm);

}

inline double min(const double & i_dA, const double & i_dB)
{
	return ((i_dA < i_dB) ? i_dA : i_dB);
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	double dN_E = 1e54;

	double dTemp_Min = 5000.0;
	double dTemp_Max = 25000.0;

	std::map<double,double> mddNII;
	std::map<double,double> mddNIII;
	std::map<double,double> mddN3p65p;

	double dGamma_Upper_32 = 1.47e8 + 1.1e6 + 9.9e6;
	double dGamma_Upper_12 = 1.4e8 + 1.06e7;
	double dGamma_Lower_32 = 1.3;
	double dGamma_Lower_52 = 1.3;
	// assume that transition probabilities for related spin states are essentially identical

	
	std::ifstream ifsTemp_Table;
	ifsTemp_Table.open("temptable.csv");
	if (ifsTemp_Table.is_open())
	{
		double dTemp, dNII, dNexc, dNIII;
		while (ifsTemp_Table >> dTemp >> dNII >> dNexc >> dNIII)
		{
			mddNII[dTemp] = dNII;
			mddNIII[dTemp] = dNIII;
			mddN3p65p[dTemp] = dNexc;
		}


		ifsTemp_Table.close();
	}
	else
	{
		std::ofstream ofsTemp_Table;
		ofsTemp_Table.open("temptable.csv");
		// calculate NII and NIII for each temperature
		for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
		{
			double dNII = Calc_Ionizing(6.1132,dTempS);
			double dNexc1 = Calc_Exciting(3.150984 - 1.692408,dTempS,dGamma_Lower_32,dGamma_Upper_32); // these values are wrong
			double dNexc2 = Calc_Exciting(3.150984 - 1.699932 ,dTempS,dGamma_Lower_52,dGamma_Upper_32); // these values are wrong
			double dNexc3 = Calc_Exciting(3.123349 - 1.692408,dTempS,dGamma_Lower_32,dGamma_Upper_12); // these values are wrong
			double dNexc = dNexc1 + dNexc2 + dNexc3;
//			double dNexc = Calc_Exciting_Simple(7.505138 - 1.692408,dTempS);
			double dNIII = Calc_Ionizing(11.87172,dTempS);
			mddNII[dTempS] = dNII;
			mddNIII[dTempS] = dNIII;
			mddN3p65p[dTempS] = dNexc;
			ofsTemp_Table << dTempS << " " << dNII << " " << dNexc << " " << dNIII << std::endl;
		}
		ofsTemp_Table.close();
	}

	std::vector<double> vdMass;
	std::vector<double> vdConst;
	std::vector<double> vdDensity;
	vdMass.push_back(0.003);
	vdMass.push_back(0.005);
	vdMass.push_back(0.008);
	vdMass.push_back(0.010);
	vdMass.push_back(0.012);
	vdMass.push_back(0.020);

	vdConst.push_back(22.46);
	vdConst.push_back(9.979);
	vdConst.push_back(6.761);
	vdConst.push_back(5.233);
	vdConst.push_back(3.903);
	vdConst.push_back(2.784);

	vdDensity.push_back(2.24819e-2);
	vdDensity.push_back(4.07052e-2);
	vdDensity.push_back(7.56584e-2);
	vdDensity.push_back(9.64721e-2);
	vdDensity.push_back(1.32798e-1);
	vdDensity.push_back(2.54219e-1);


	for (unsigned int uiM = 0; uiM < vdMass.size(); uiM++)
	{
		double dN_Sh = vdMass[uiM] * 1.99E+033/(40*1.67E-024); // # of Ca atoms in shell if shell is entirely Ca
		double dXref= 3.2e-2 * vdConst[uiM];
		std::ostringstream ossTableName;
		ossTableName << "Abd_table_M" << vdMass[uiM] << ".csv";
		std::ofstream ofsTable;
		ofsTable.open(ossTableName.str().c_str());
		// construct first line of table (header line)
		ofsTable << "TE/TS";
		for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
			ofsTable << ", " << dTempS;
		ofsTable << std::endl;

		std::ostringstream ossTableName2;
		ossTableName2 << "Abd_table_2_M" << vdMass[uiM] << ".csv";
		std::ofstream ofsTable2;
		ofsTable2.open(ossTableName2.str().c_str());

		double dC_sh = 1996.9846860964 * (86400/50.) * (86400/50.)/ ((2.17530134368732E-06 + 2.03464296795567E-06 + 2.18935826969984E-07) * vdDensity[uiM] / (40*1.67E-024));
		ofsTable2 << "T, X, Texc, FII, FIII" << std::endl;

		for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
		{
			double dXmin = mddNIII[dTempS] / dN_Sh;
//			double dX = mddNIII[dTempS] / dN_Sh / (1.0 - dC_sh * dN_Sh / mddN3p65p[dTempS]);
			double dX = mddNIII[dTempS] / dN_Sh / (1.0 - dC_sh * (dN_Sh - mddNIII[dTempS]) / mddN3p65p[dTempS]);
			if (dX < 0.0 || (dX * dN_Sh < mddN3p65p[dTempS]))
			{
				dX = dC_sh - mddNIII[dTempS] / dN_Sh;
			}
			if (dX < dXmin)
				dX = dXmin;
			ofsTable2 << dTempS << ", " << dX;
			// calculate the excitation temperaure
			double dTexc = -7.5 / g_XASTRO.k_dKb_eV / log(mddN3p65p[dTempS] / (dX * dN_Sh));
			ofsTable2 << ", " << dTexc;
			ofsTable2 << ", " << mddNII[dTempS] / (dX * dN_Sh);
			ofsTable2 << ", " << mddNIII[dTempS] / (dX * dN_Sh);
			ofsTable2 << std::endl;
		}
		ofsTable2.close();

		for (double dTempE = dTemp_Min; dTempE < dTemp_Max; dTempE += 500.0)
		{
			ofsTable << dTempE;

			double dNIIE = mddNII[dTempE];
			double dNIIIE = mddNIII[dTempE];
			double dFEII, dFEIII;
			if (dNIIE > dN_E)
				dNIIE = dN_E;
			if (dNIIIE > dNIIE)
				dNIIIE = dNIIE;
			dNIIE -= dNIIIE;
			dFEII = dNIIE / dN_E;
			dFEIII = dNIIIE / dN_E;

			if (dFEII > 0.0 && dFEII < 1.0)
			{ // if the ejecta is completely in the neutral or III state, so FE/FS is very small and very little Ca is required in the shell to get the desired ratio
				// or, the ejecta is completely in the II state, so FE/FS >= 1, requiring abundance = 1
				for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
				{
					double dX;

					double dNIIS = mddNII[dTempS];
					double dNIIIS = mddNIII[dTempS];

					if (dNIIS >= dN_Sh && dNIIIS >= dN_Sh) // all atoms will be in III state, no matter what
						ofsTable << ", *"; // "no" ions are in II state; essentially drives FE/FS to be extremely large
					else if (dNIIS >= dN_Sh && dNIIIS < dN_Sh)
					{ // all Ca are certain to be Ca II or higher
						dX = dFEII * dXref + dNIIIS / dN_Sh;
						ofsTable << ", " << dX;
					}
					else if (dNIIS > dNIIIS)
					{
						double dFSmin = 1.0e-4;
						// assume NII > X Ns (all Ca in the II state)
						dX = dFEII * dXref + dNIIIS / dN_Sh;
						if (dNIIS > dX * dN_Sh)
							ofsTable << ", " << dX; // assumption has held, X can be determined
						else if (dNIIS / dN_Sh < dFSmin)
							ofsTable << ", " << ", &"; // there may not be enough II photons to achieve the requisite opacity
						else
							ofsTable << ", " << dNIIIS / dN_Sh << "@"; // the abundance must be higher than the relative fraction of III photons - if the abundance is below this 
					}
					else
						ofsTable << ", " << "X"; // there are more III photons than II photons: any Ca will be in III state, leading to Fs = 0

	//				double dX = dNIIIS / dN_Sh;
	//				std::cout << dTempE << " " << dTempS << " " << dX << " " << dFEII << " " <<  dFS << std::endl;
				}
			}
			else
			{
				if (dFEII == 0.0)
					for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
						ofsTable << ", +";
				else
					for (double dTempS = dTemp_Min; dTempS < dTemp_Max; dTempS += 500.0)
						ofsTable << ", 0";
			}

			ofsTable << std::endl;
		}
		ofsTable.close();
	}
}


