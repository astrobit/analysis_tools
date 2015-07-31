#pragma once
#include <xastroion.h>
#include <xmath.h>
#include <cstdio>
#include <cstring>

class COMPOSITION
{
public:
	unsigned int	m_uiZ;
	unsigned int	m_uiA;
	double			m_dMass_Fraction;
	double *		m_lpdIon_Fraction; // ordered 0->Z (I, II, ... Z + I)
private:
	const XASTRO_ATOMIC_IONIZATION_DATA * m_lpcIon_Data;
	const XASTRO_ATOMIC_PARTITION_FUNCTION_DATA * m_lpcPF_Data;
	const XASTRO_ATOMIC_PARTITION_FUNCTION_DATA_IRWIN * m_lpcPF_Data_Irwin;


	void Copy(const COMPOSITION & i_cRHO)
	{
//		printf("Copying %i\n",i_cRHO.m_uiZ);
		m_uiZ = i_cRHO.m_uiZ;
		m_uiA = i_cRHO.m_uiA;
		m_dMass_Fraction = i_cRHO.m_dMass_Fraction;
//		printf("delete ion fraction\n");
		if (m_lpdIon_Fraction)
			delete [] m_lpdIon_Fraction;
		m_lpdIon_Fraction = new double[m_uiZ + 1];
//		printf("copy ion fraction\n");
		//@@TODO: should really check to confirm that ion fraction really did get allocated, but this is only a problem if we are running out of memory anyway
//		if (!m_lpdIon_Fraction)
//			printf("Warning : did not allocate ion fraction!\n");
//		if (!i_cRHO.m_lpdIon_Fraction)
//			printf("Warning : RHO doesn't have an ion fraction array!\n");
		memcpy(m_lpdIon_Fraction,i_cRHO.m_lpdIon_Fraction,sizeof(double) * (m_uiZ + 1));
//		printf("copy atomic data pointers\n");
		m_lpcIon_Data =  i_cRHO.m_lpcIon_Data;
		m_lpcPF_Data =  i_cRHO.m_lpcPF_Data;
		m_lpcPF_Data_Irwin =  i_cRHO.m_lpcPF_Data_Irwin;
	}
public:
	double Get_Partition_Function(unsigned int i_uiIon_State, const double & i_dTemperature_K, const double & i_dNumber_Density_cm3)
	{
		double dRet = 1.0;
		if (m_lpcPF_Data)
			dRet = m_lpcPF_Data->Get_Partition_Function(i_uiIon_State, i_dTemperature_K,i_dNumber_Density_cm3);
		else if (m_lpcPF_Data_Irwin)
			dRet = m_lpcPF_Data_Irwin->Get_Partition_Function(i_uiIon_State, i_dTemperature_K);
		return dRet;
	}
	double	Get_Ion_Energy(unsigned int i_uiIon_State)
	{
		double	dRet = 1.0e-12; // ~10^12 eV - not a realistic value, but forces it into neutral state
		if (m_lpcIon_Data && i_uiIon_State < m_uiZ)
			dRet = m_lpcIon_Data->Get_Ion_State_Potential(i_uiIon_State);
		else if (i_uiIon_State == 0)
			dRet = 1.0e-6; // lower potential for 1st ionized state if data not available
		if (m_uiZ == 1)
			dRet = 13.6 * g_XASTRO.k_derg_eV;
		else
			dRet = 26 * (i_uiIon_State + 1) * g_XASTRO.k_derg_eV;
		return dRet;
	}
	COMPOSITION & operator = (const COMPOSITION & i_cRHO) { Copy(i_cRHO); return *this;}
	void	Set_Isotope(unsigned int i_uiZ, unsigned int i_uiA)
	{
		m_uiZ = i_uiZ;
		m_uiA = i_uiA;
		if (m_lpdIon_Fraction)
			delete [] m_lpdIon_Fraction;
		m_lpdIon_Fraction = new double[m_uiZ + 1];
		m_lpcIon_Data = g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(m_uiZ);
		m_lpcPF_Data = g_xAstro_Partition_Fn_Data.Get_PF_Data_Ptr(m_uiZ);
		m_lpcPF_Data_Irwin = g_xAstro_Partition_Fn_Data_Irwin.Get_PF_Data_Ptr(m_uiZ);
		if (m_lpcIon_Data == NULL)
			fprintf(stderr,"COMPOSITION::Set_Isotope: Warning - ionization energy not available for Z = %i A = %i.\n",m_uiZ,m_uiA);
		if (m_lpcPF_Data == NULL && m_lpcPF_Data_Irwin == NULL)
			fprintf(stderr,"COMPOSITION::Set_Isotope: Warning - partition function not available for Z = %i A = %i.\n",m_uiZ,m_uiA);
	}
	COMPOSITION(void)
	{
		m_uiZ = m_uiA = 1;
		m_dMass_Fraction = 0.0;
		m_lpdIon_Fraction = NULL;
		m_lpcIon_Data = NULL;
		m_lpcPF_Data = NULL;
		m_lpcPF_Data_Irwin = NULL;
	}
	COMPOSITION(unsigned int i_uiZ, unsigned int i_uiA)
	{
		m_dMass_Fraction = 0.0;
		m_lpdIon_Fraction = NULL;
		m_lpcIon_Data = NULL;
		m_lpcPF_Data = NULL;
		m_lpcPF_Data_Irwin = NULL;
		Set_Isotope(i_uiZ,i_uiA);
	}
	COMPOSITION(const COMPOSITION & i_cRHO)
	{
		m_dMass_Fraction = 0.0;
		m_lpdIon_Fraction = NULL;
		m_lpcIon_Data = NULL;
		m_lpcPF_Data = NULL;
		m_lpcPF_Data_Irwin = NULL;
		Copy(i_cRHO);
	}
	~COMPOSITION(void)
	{
		if (m_lpdIon_Fraction)
			delete [] m_lpdIon_Fraction;
		m_lpdIon_Fraction = NULL;
	}
};
class COMPOSITION_SET
{
private:
	COMPOSITION	m_cGeneric_Composition;

	double		m_dSaha_Coeff;
	double	** m_lpdF;
	double	** m_lpdProd_F;
	unsigned int m_uiNum_Compositions;
	COMPOSITION * m_lpdCompositions;
	double		m_dZav; // average effective atomic charge
	double		m_dNe; // electron density (cm^-3)
	double		m_dNn; // nucleon density (cm^-3)

	double		m_dDensity_Ref; // density for which Nn and Ni is computed
	bool		m_bMass_Fraction_Change;

	void	Copy(const COMPOSITION_SET & i_cRHO)
	{
		Allocate(i_cRHO.m_uiNum_Compositions);
		for (unsigned int uiI = 0; uiI < m_uiNum_Compositions; uiI++)
		{
			// call inizalize to make sure that F gets set up correctly
			Initialize_Composition(uiI,i_cRHO.m_lpdCompositions[uiI].m_uiZ,i_cRHO.m_lpdCompositions[uiI].m_uiA);
			m_lpdCompositions[uiI] = i_cRHO.m_lpdCompositions[uiI]; // copy ion fractions
		}

		m_dZav = i_cRHO.m_dZav; // average effective atomic charge
		m_dNe = i_cRHO.m_dNe; // average effective atomic charge
		m_dNn = i_cRHO.m_dNn; // average effective atomic charge

		m_dDensity_Ref = 0.0;//i_cRHO.m_dDensity_Ref; // density for which Nn and Ni is computed
		m_bMass_Fraction_Change = true; // will need to recompute everything
	}

public:
	COMPOSITION_SET & operator = (const COMPOSITION_SET & i_cRHO) { Copy(i_cRHO); return *this;}
	void Allocate(unsigned int i_uiNum_Compositions)
	{
		if (m_uiNum_Compositions > 0)
		{
			delete [] m_lpdCompositions;
			for (unsigned int uiI = 0; uiI < m_uiNum_Compositions; uiI++)
			{
				if (m_lpdF[uiI])
					delete [] m_lpdF[uiI];
				if (m_lpdProd_F[uiI])
					delete [] m_lpdProd_F[uiI];
			}
			delete [] m_lpdF;
			delete [] m_lpdProd_F;
			m_lpdCompositions = NULL;
			m_lpdF = NULL;
			m_lpdProd_F = NULL;
		}
		
		if (i_uiNum_Compositions > 0)
		{
			m_lpdCompositions = new COMPOSITION[i_uiNum_Compositions];
			m_lpdF = new double * [i_uiNum_Compositions];
			m_lpdProd_F = new double * [i_uiNum_Compositions];
			for (unsigned int uiI = 0; uiI < i_uiNum_Compositions; uiI++)
			{
				m_lpdF[uiI] = NULL;
				m_lpdProd_F[uiI] = NULL;
			}
		}
		m_uiNum_Compositions = i_uiNum_Compositions;
	}
	void Initialize_Composition(unsigned int i_uiIdx, unsigned int i_uiZ, unsigned int i_uiA)
	{
		if (i_uiIdx < m_uiNum_Compositions)
		{
			m_lpdCompositions[i_uiIdx].Set_Isotope(i_uiZ, i_uiA);
			if (m_lpdF[i_uiIdx])
				delete [] m_lpdF[i_uiIdx];
			m_lpdF[i_uiIdx] = new double[i_uiZ + 1];
			if (m_lpdProd_F[i_uiIdx])
				delete [] m_lpdProd_F[i_uiIdx];
			m_lpdProd_F[i_uiIdx] = new double[i_uiZ + 1];
		}
	}
	void Set_Composition(unsigned int i_uiIdx, double i_dMass_Fraction)
	{
		if (i_uiIdx < m_uiNum_Compositions)
		{
			if (m_lpdCompositions[i_uiIdx].m_dMass_Fraction != i_dMass_Fraction)
				m_bMass_Fraction_Change = true;
			m_lpdCompositions[i_uiIdx].m_dMass_Fraction = i_dMass_Fraction;
		}
	}
	const COMPOSITION & Get_Composition(unsigned int i_uiIdx)
	{
		if (i_uiIdx < m_uiNum_Compositions)
			return m_lpdCompositions[i_uiIdx];
		else
			return m_cGeneric_Composition;
	}
	unsigned int Find_Comp_Idx(unsigned int i_uiZ, unsigned int i_uiA)
	{
		unsigned int uiRet = -1;
		for (unsigned int uiI = 0; uiI < m_uiNum_Compositions && uiRet == -1; uiI++)
		{
			if (m_lpdCompositions[uiI].m_uiZ == i_uiZ && m_lpdCompositions[uiI].m_uiA == i_uiA)
				uiRet = uiI;
		}
		return uiRet;
	}
	unsigned int Get_Num_Compositions(void) const {return m_uiNum_Compositions;}
	
	void	Zaghloul_Saha(const double & i_dTemperature_K, const double & i_dDensity_g_cm3, unsigned int ITMAX = 20000); // 10000 seems to be the right # of iters; 20000 to be safe.

	COMPOSITION_SET(void)
	{
		m_lpdF = NULL;
		m_lpdProd_F = NULL;
		m_uiNum_Compositions = 0;
		m_lpdCompositions = NULL;
		m_dZav = 1.0e-3; // rough guess for starting value.  Probably will want to scale this with temp & density?
		m_dNe = 0.0;
		m_dNn = 0.0;
		m_dDensity_Ref = 0.0;
		m_bMass_Fraction_Change = false;
		m_dSaha_Coeff = 2.0 * pow(2.0 * g_cConstants.dPi * g_XASTRO.k_dme * g_XASTRO.k_dKb / (g_XASTRO.k_dh * g_XASTRO.k_dh),1.5);
	}
	COMPOSITION_SET(const COMPOSITION_SET & i_cRHO)
	{
		m_lpdF = NULL;
		m_lpdProd_F = NULL;
		m_uiNum_Compositions = 0;
		m_lpdCompositions = NULL;

		m_dSaha_Coeff = 2.0 * pow(2.0 * g_cConstants.dPi * g_XASTRO.k_dme * g_XASTRO.k_dKb / (g_XASTRO.k_dh * g_XASTRO.k_dh),1.5);

		Copy(i_cRHO);
	}

};

