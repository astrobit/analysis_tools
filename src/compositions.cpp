#include <xastro.h>
#include <xastroion.h>
#include <cmath>
#include <cstdio>
#include <compositions.h>

//------------------------------------------------------------------------------
//
// compositions: this is mostly for the Saha routine
//
//
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// based on the work of Zaghloul et al., J. Phys. D: Appl. Phys. 33:977 (2000)
// here the state number 0 is neutral and the total number of states is Z+1
// adapted for multi-species (see below) by Brian W. Mulligan, July 2015

void	COMPOSITION_SET::Zaghloul_Saha(const double & i_dTemperature_K, const double & i_dDensity_g_cm3, unsigned int ITMAX)
{
	const double dZav_Tol = 1.0e-15;
	const double dZav_Tol_Inv = 1.0e15;
	double	dT_eff = i_dTemperature_K;

	double	dZav = m_dZav; // use previous average Z

	
	if (m_dDensity_Ref != i_dDensity_g_cm3 || m_bMass_Fraction_Change) // density for which Nn and Ni is computed has changed
	{
		m_dNn = 0.0;
		for (unsigned int uiJ = 0; uiJ < m_uiNum_Compositions; uiJ++)
		{
			m_dNn += i_dDensity_g_cm3 * m_lpdCompositions[uiJ].m_dMass_Fraction / (m_lpdCompositions[uiJ].m_uiA * g_XASTRO.k_dmp);
		}
		m_bMass_Fraction_Change = false;
		m_dDensity_Ref == i_dDensity_g_cm3;
	}

	if(dT_eff < 0.0) // make sure temperature is positive
		dT_eff = 1.0e-6; // minimum temp is 1 microKelvin

	double	dKt_erg = dT_eff * g_XASTRO.k_dKb;
	double dKbT_Inv = 1.0/dKt_erg;
	double dCoef = m_dSaha_Coeff * pow(dT_eff,1.5);

	if (dZav < 1.0e-6)
		dZav = 1.0e-6; // minimum value for Z_av is 10^-6


//	printf("here\n");
	for (unsigned int uiJ = 0; uiJ < m_uiNum_Compositions; uiJ++)
	{
		// warning: f(i+1) is acually being computed here.  the indices used here are for convencience, later uses
		// of f use i as ion state and f(i-1) is the appropriate index to use
		// e.g. for singly ionized, use i = 1, and f(i-1) = f(0)
//			printf("here a\n");
		for (unsigned int uiI = 0; uiI < m_lpdCompositions[uiJ].m_uiZ; uiI++)
		{
//			printf("here b\n");
			double dU_i = m_lpdCompositions[uiJ].Get_Partition_Function(uiI, i_dTemperature_K, m_dNn);
			double dU_ip1 = m_lpdCompositions[uiJ].Get_Partition_Function(uiI + 1, i_dTemperature_K, m_dNn);
//			printf("%f %f %e %e %e\n",dU_i,dU_ip1,dCoef,m_lpdCompositions[uiJ].Get_Ion_Energy(uiI),dKbT_Inv);
			m_lpdF[uiJ][uiI] = dU_ip1 / dU_i * dCoef * exp(-m_lpdCompositions[uiJ].Get_Ion_Energy(uiI)*dKbT_Inv + 10.0*i_dDensity_g_cm3);
			if (isnan(m_lpdF[uiJ][uiI]) || isinf(m_lpdF[uiJ][uiI]))
				m_lpdF[uiJ][uiI] = 1.0e+15; // something large

			if(uiI == 0)
				m_lpdProd_F[uiJ][uiI] = m_lpdF[uiJ][uiI];
			else
				m_lpdProd_F[uiJ][uiI] = m_lpdProd_F[uiJ][uiI - 1] * m_lpdF[uiJ][uiI];
		}
	}
//	printf("here\n");

//	printf("%e\n",m_lpdF[0][0]);
//	printf("%e\n",m_lpdProd_F[0][0]);
//	if (m_uiNum_Compositions > 1)
//	{
//		printf("%e %e\n",m_lpdF[1][0],m_lpdF[1][1]);
//		printf("%e %e\n",m_lpdProd_F[1][0],m_lpdProd_F[1][1]);
//	}
	// Zaghloul et al. eq. (9) is for a single species.  Here we adapt this technique to a multi-species plasma, and change eq (5) to eq (5.1):
	// Zav = Sum_s=1...N(Sum_r=1...Z_s(r \alpha_(s,r))), where N is # of species, and subscript s refers to species
	// Eq. (4) holds for each individual species [Sum_r=1...Z_s(\alpha_(s,r) = 1 for all s=1...N]
 	// Eq (8) can no longer holds generally, but instead becomes [eq 8.1]
	//    Zav = Sum_s=1...N(Sum_r=1...Z_s((r Prod_j=1...i f_j)/(Zav n_H)^i) \alpha_(s,0))
	// Since Zav is a function of all \alpha_(s,0), we use eq (4) and eq (7) for each species
    //    Sum_r=1...Z_s((Prod_j=1...i f_j)/(Zav n_H)^i) \alpha_(s,0)) = 1
	// to solve for \alpha_(s,0) for each species, leading to (4.2)
    //    \alpha_(s,0) = (Sum_r=1...Z_s((Prod_j=1...i f_j)/(Zav n_H)^i))^-1
	// and subsittuting into (8.1) above, we get (eq. 9.1)
	//    Zav = Sum_s=1...N(Sum_r=1...Z_s((r Prod_j=1...i f_j)/(Zav n_H)^i) (Sum_r=1...Z_s((Prod_j=1...i f_j)/(Zav n_H)^i))^-1)
	// we now have a transcendental equation similiar to (9), that can be solved by any appropriate solver.

	double	dDelta_Zav = 0.0;
	double	dValley_Jump = 0.01; // adjust Zav by 1% if we are stuck in a valley
	unsigned int uiIter = 0;
	do
	{
		dZav += dDelta_Zav;
		double dZav_Star_Nn_Inv = 1.0/(dZav*m_dNn);
		double	dF = 0.0;//-dZav;
		double	ddF = 0.0;
		double	dZav_Star_Nn_Accum;
		for (unsigned int uiJ = 0; uiJ < m_uiNum_Compositions; uiJ++)
		{
			double dGs = 0.0;
			double dHs = 0.0;
			double dKs = 0.0;
			dZav_Star_Nn_Accum = dZav_Star_Nn_Inv;
			// produce sums of:
			// g_s: Sum_r=1...Z_s((    Prod_j=1...i f_j)/(Zav n_H)^i)
			// h_s: Sum_r=1...Z_s((r   Prod_j=1...i f_j)/(Zav n_H)^i)
			// k_s: Sum_r=1...Z_s((r^2 Prod_j=1...i f_j)/(Zav n_H)^i)
			for (unsigned int uiI = 1; uiI <= m_lpdCompositions[uiJ].m_uiZ; uiI++)
			{
				double	dCommon = m_lpdProd_F[uiJ][uiI - 1] * dZav_Star_Nn_Accum;
				dGs += dCommon;
				dHs += uiI * dCommon;
				dKs += (uiI * uiI) * dCommon;
				dZav_Star_Nn_Accum *= dZav_Star_Nn_Inv;
			}
			// add neutral state to Sum1:
			double dCommon = (dHs / (1.0 + dGs));
			// f = Sum_s=1...N (h_s / (1 + g_s)) - z
			dF += dCommon;

			// df/dz = (1/(Zav n_H))Sum_s=1...N (k_s/(1 + g_s) + (h_s/(1 + g_s))^2)) - 1; 1/(Zav n_H)and -1 at the end are handled below
			ddF += (dCommon * dCommon - dKs / (1.0 + dGs));
		}
		ddF *= dZav_Star_Nn_Inv; // the 1/z part noted above
//		printf("a:f = %e dF = %e  Z = %e   dZ = %e\n",dF,ddF,dZav,dDelta_Zav);
		ddF -= 1.0; // the -1 part
		dF -= dZav;
		if (fabs(dF) > 1.0e-15)
		{
			if (fabs(ddF) > 1.0e-15)
				dDelta_Zav = -dF / ddF;
			else
			{
				dDelta_Zav = dValley_Jump * dF; // we're in a valley - jump to try to get out
				dValley_Jump *= -1.0; // if we get stuck again, try jumping the other way
			}
		}
		else
		{
			dDelta_Zav = 0.0; // close enough!
		}
//		printf("b:f = %e dF = %e  Z = %e dZ = %e (%e)\n",dF,ddF,dZav,dDelta_Zav,dDelta_Zav / dZav);
		uiIter++;
	}
	while(fabs(dDelta_Zav/dZav) > dZav_Tol && uiIter < ITMAX);
	dZav += dDelta_Zav;
	//printf("%i\n",uiIter);

	m_dNe = dZav * m_dNn;
	double dZav_Star_Nn_Inv = 1.0/m_dNe;
	//printf("%e %e\n",dZav,m_dNe);
	// now fill the ion fractions with the results
	for (unsigned int uiJ = 0; uiJ < m_uiNum_Compositions; uiJ++)
	{
		// \alpha_(s,0) : eq (4.2) from above
		double dGr = 0.0;
		double dZav_Star_Nn_Accum = dZav_Star_Nn_Inv;
		for (unsigned int uiI = 1; uiI <= m_lpdCompositions[uiJ].m_uiZ; uiI++)
		{
			dGr += m_lpdProd_F[uiJ][uiI - 1] * dZav_Star_Nn_Accum;
			dZav_Star_Nn_Accum *= dZav_Star_Nn_Inv;
		}
		m_lpdCompositions[uiJ].m_lpdIon_Fraction[0] = 1.0 / (dGr + 1);
		// recursion relation : eq (7)
		for (unsigned int uiI = 1; uiI <= m_lpdCompositions[uiJ].m_uiZ; uiI++)
		{
			m_lpdCompositions[uiJ].m_lpdIon_Fraction[uiI] = m_lpdF[uiJ][uiI - 1] * dZav_Star_Nn_Inv * m_lpdCompositions[uiJ].m_lpdIon_Fraction[uiI - 1];
		}
	}

	// save final Zav and ne
	m_dZav = dZav; 
}

