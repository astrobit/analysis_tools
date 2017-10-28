#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
//#include <line_anal.h>



double GetAbundance(unsigned int uiI,const XDATASET &cAbdData)
{
	double	dAbd = cAbdData.GetElement(1,uiI);
	if (dAbd < 5.0e-10)
		dAbd = 0.0;
	return dAbd;
}
// Abundance of a radioatively decaying substance which is only a parent (not a decay product).  e.g. Ni 56
double GetAbundance(unsigned int i_uiI,const XDATASET &i_cAbdData,const double &i_dTime, const double &i_dHalflife)
{
	double	i_dAbd = cAbdData.GetElement(1,i_uiI);
	if (i_dAbd < 5.0e-10)
		i_dAbd = 0.0;
	dAbd *= exp(-g_cConstants.dLn2 * i_dTime / i_dHalflife);
	return dAbd;
}

// Abundance of a radioactively decaying substance which has a single parent, e.g. Co 56.  Halflife 1 refers to parents halflife, Halflife 2 refers to this nucleus
double GetAbundance(unsigned int i_uiI,const XDATASET &i_cAbdData,const double &i_dTime, const double &i_dHalflife_1, const double &i_dHalflife_2)
{
	double	i_dAbd = cAbdData.GetElement(1,i_uiI);
	if (i_dAbd < 5.0e-10)
		i_dAbd = 0.0;
	double	dTau1, dTau2;
	dTau1 = i_dHalflife_1 / g_cConstants.dLn2;
	dTau2 = i_dHalflife_2 / g_cConstants.dLn2;
	dInvTau1 = i_dHalflife_1 / g_cConstants.dLn2;
	dInvTau2 = i_dHalflife_2 / g_cConstants.dLn2;
	dAbd *= exp(-i_dTime * (dInvTau1 + dInvTau2)) * (exp(i_dTime * dInvTau1) - exp(i_dTime * dInvTau2) * dTau2 / (dTau2 - dTau1);
	return dAbd;
}

// Abundance of a non-radioactively decaying substance which is the result of a cascade from a single parent nucleus, and a single intermediate nucleus, e.g. Fe 56.  Halflife 1 refers to parents halflife, Halflife 2 refers to intermediate nucleus
double GetAbundanceFinal(unsigned int i_uiI,const XDATASET &i_cAbdData,const double &i_dTime, const double &i_dHalflife_1, const double &i_dHalflife_2)
{
	double	i_dAbd = cAbdData.GetElement(1,i_uiI);
	if (i_dAbd < 5.0e-10)
		i_dAbd = 0.0;
	double	dTau1, dTau2;
	dTau1 = i_dHalflife_1 / g_cConstants.dLn2;
	dTau2 = i_dHalflife_2 / g_cConstants.dLn2;
	dInvTau1 = i_dHalflife_1 / g_cConstants.dLn2;
	dInvTau2 = i_dHalflife_2 / g_cConstants.dLn2;
	dAbd *= (dTau1 * (exp(-dTime * dInvTau1) - 1.0) + dTau2 * (1.0 - exp(-dTime * dInvTau2))) / (dTau2 - dTau1);
	return dAbd;
}



unsigned int CountIonStates(const XASTROLINE_ELEMENTDATA &cElement)
{
	unsigned int uiI, uiNumIons = 0;
	for (uiI = 0; uiI < cElement.uiZ; uiI++)
	{
		if (cElement.lpcIonData[uiI].uiNum_Lines > 0)
			uiNumIons++;
	}
	return uiNumIons;
}

// Sobolev optical depth computation, using line data
double Sobolev_Optical_Depth(const double & i_dNumber_Density_Ref, const XASTROLINE_LINEDATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double &i_dTime_Ref, const double &i_dIon_Temp)
{
	return i_dNumber_Density_Ref * i_cLine_Data.SobolevTauCoeff() * i_dTime_Ref * i_dTime_Ref * i_dTime_Ref / (i_dTime * i_dTime) * exp(-(i_dIon_Temp + i_cLine_Data.cLower_State.dEnergy_eV / g_XASTRO.k_dKb_eV) / i_dFluid_Temp);
}

// Compton (electron scattering) optical depth
double Compton_Optical_Depth(const double & i_dElectron_Density_cm3, const double &i_dLength_cm)
{

	return i_dElectron_Density_cm3 * i_dLength_cm * 0.66524574e-24;
}


// A method for estimating ionization state by assuming that any state with a thermal energy > 4% of the ionization energy will be
// fully ionized
double	Electron_Density(const double & i_dkTzone_eV, XASTRO_ATOMIC_IONIZATION_DATA * lpIon_Energies, const double &i_dSpecies_Density)
{
	double	dSpecies_Electron_Density = 0.0;
	unsigned int uiI
	for (uiI = 0; uiI < lpIon_Energies->m_uiIonization_Energies_Count; uiI++)
	{
		if (dkTzone_eV > 0.042 * lpIon_Energies->m_lpIonization_energies_erg[uiI]) // rough estimate based on calcium ionization under 'normal' conditions [e.g. stellar atmosphere densities]
			dSpecies_Electron_Density += i_dSpecies_Density;
	}
	return dSpecies_Electron_Density;
}

int main(void)
{
	// Inputs: Wavelength limits
	double	dLambda_Max = 6350.0;
	double	dLambda_Min = 5700.0;
	double	dDelta_Lambda = 1.0; // data resolution

	double	dTimeRef = 50.0;
	double	dTime = 86400.0 * 10;
	double	dTemp = 12000.0;
	double	dKT = dTemp * g_XASTRO.k_dKb;
	double	dKT_eV = dTemp * g_XASTRO.k_dKb_eV;
	double	dCaAbdAdj = 0.0;
	double	dSiAbdAdj = 0.0;
	double	dFeAbdAdj = 0.0;

	unsigned int uiNum_Lambda_Bins = (unsigned int)(dLambda_Min - dLambda_Min) / dDelta_Lambda;
	double	*lpdDataBins = new double[uiNum_Lambda_Bins];

	XASTROLINE_ELEMENTDATA	cH_Lines;
	XASTROLINE_ELEMENTDATA	cHe_Lines;
	XASTROLINE_ELEMENTDATA	cC_Lines;
	XASTROLINE_ELEMENTDATA	cO_Lines;
	XASTROLINE_ELEMENTDATA	cSi_Lines;
	XASTROLINE_ELEMENTDATA	cFe_Lines;
	XASTROLINE_ELEMENTDATA	cCa_Lines;

	cH_Lines.ReadDataFile("./Data","H");
	cHe_Lines.ReadDataFile("./Data","He");
	cC_Lines.ReadDataFile("./Data","C");
	cO_Lines.ReadDataFile("./Data","O");
	cSi_Lines.ReadDataFile("./Data","Si");
	cCa_Lines.ReadDataFile("./Data","Ca");
	cFe_Lines.ReadDataFile("./Data","Fe");


	XDATASET	cVelocity;
	XDATASET	cDens;
	XDATASET	cCa;
	XDATASET	cC;
	XDATASET	cO;
	XDATASET	cSi;
	XDATASET	cNi56;
	XDATASET	cHe3;
	XDATASET	cHyd;
	XDATASET	cMgg;
	XDATASET	cTemp;
	XDATASET	cPres;

	XDATASET * lpSpecies[] =
		{
			&cHyd,
			&cHe3,
			&cHe4,
			&cC,
			&cN,
			&cO,
			&cNe,
			&cMg,
			&cSi,
			&cCa
		};

	unsigned int uiNum_Ordered_Species = sizeof (lpSpecies) / sizeof(XDATASET *);

	cVelocity.ReadDataFile("velx.txt", true, false);
	cDens.ReadDataFile("dens.txt", true, false);
	cCa.ReadDataFile("ca40.txt", true, false);
	cSi.ReadDataFile("sig.txt", true, false);
	cNi56.ReadDataFile("fe.txt",true,false);
	cHe3.ReadDataFile("he3.txt",true,false);
	cHe4.ReadDataFile("he4.txt",true,false);
	cHyd.ReadDataFile("hyd.txt",true,false);
	cMgg.ReadDataFile("mgg.txt",true,false);
	cC.ReadDataFile("c12.txt",true,false);
	cO.ReadDataFile("o16.txt",true,false);
	cTemp.ReadDataFile("temp.txt",true,false);
//	cPres.ReadDataFile("pres.txt",true,false);

	unsigned int uiNum_Data = cVelocity.GetNumElements();
	double	*lpdElectron_Density = new double[uiNum_Data];
	double	*lpdElectron_Optical_Depth = new double[uiNum_Data];
	double	*lpdZone_Energy = new double[uiNum_Data];
	double	*lpdZone_Temp = new double[uiNum_Data];

	double	dHalf_Lifes[2] = {77.23626 * 86400.0,6.0751 * 86400.0}; //0 = Cobolt 56, 1 = Nickel 56

	XASTRO_ATOMIC_IONIZATION_DATA * lpIon_Energies[12];
	
	lpIon_Energies[0] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(1); // Hydrogen
	lpIon_Energies[1] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(2); // Helium
	lpIon_Energies[2] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(6); // Carbon
	lpIon_Energies[3] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(7); // Nitrogen
	lpIon_Energies[4] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(8); // Oxygen
	lpIon_Energies[5] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(10); // Neon
	lpIon_Energies[6] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(12); // Magnesium
	lpIon_Energies[7] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(14); // Silicon
	lpIon_Energies[8] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(20); // Calcium
	lpIon_Energies[9] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(26); // Iron
	lpIon_Energies[10] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(27); // Cobalt
	lpIon_Energies[11] = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(28); // Nickel

	// Initialize zone temperatures
	for (uiI = 0; uiI < uiNum_Data; uiI++)
	{
		lpdZone_Temp[uiI] = cTemp.GetElement(1,uiI);
	}

	for (uiI = 0; uiI < uiNum_Data; uiI++)
	{
		double	dLocal_Density ;
		double	dAbundance;
		double	dSpecies_Density;
		double	dZone_Temp = lpdZone_Temp[uiI];
		double	dkTzone_eV = dZone_Temp * g_XASTRO.k_dKb_eV;
		double	dZone_Dens = cDens.GetElement(1,uiI) * pow(dTimeRef/dTime,3.0);
		double	dZone_Length;
		if (uiI > 0)
			dZone_Length += (cDens.GetElement(0,uiI) - cDens.GetElement(0,uiI - 1)) * 0.5;
		else
			dZone_Length += cDens.GetElement(0,uiI) * 0.5;
		if (uiI > (uiNum_Data - 1))
			dZone_Length += (cDens.GetElement(0,uiI + 1) - cDens.GetElement(0,uiI)) * 0.5;
		else
			dZone_Length += (cDens.GetElement(0,uiI) - cDens.GetElement(0,uiI - 1)) * 0.5; // arbitrary

		lpdElectron_Density[uiI] = 0.0;

		// For stable nuclei
		for (uiJ = 0; uiJ < uiNum_Ordered_Species; uiJ++)
		{
			dSpecies_Density = GetAbundance(uiI,*lpSpecies[uiJ]) * dZone_Dens;
			lpdElectron_Density[uiI] += Electron_Density(i_dkTzone_eV, lpIon_Energies[uiJ], i_dSpecies_Density);
		}

		// For radioactively decaying nuclei - Ni 56
		dSpecies_Density = GetAbundance(uiI,cNi56,dTime,dHalf_Lifes[1]) * dZone_Dens;
		lpdElectron_Density[uiI] += Electron_Density(i_dkTzone_eV, lpIon_Energies[11], i_dSpecies_Density);

		// For radioactively decaying nuclei - Co 56
		dSpecies_Density = GetAbundance(uiI,cNi56,dTime,dHalf_Lifes[1],dHalf_Lifes[0]) * dZone_Dens;
		lpdElectron_Density[uiI] += Electron_Density(i_dkTzone_eV, lpIon_Energies[10], i_dSpecies_Density);

		// For Fe 56
		dSpecies_Density = GetAbundanceFinal(uiI,cNi56,dTime,dHalf_Lifes[1],dHalf_Lifes[0]) * dZone_Dens;
		lpdElectron_Density[uiI] += Electron_Density(i_dkTzone_eV, lpIon_Energies[9], i_dSpecies_Density);

		lpdElectron_Optical_Depth[uiI] = Compton_Optical_Depth(lpdElectron_Density[uiI],dZone_Length);
	}

	return 0;
}


