#include <opacity_project_pp.h>

long double opacity_project_ion::Calc_Recombination_Rate(const opacity_project_state &i_sIonized_State, const opacity_project_state &i_sRecombined_State, const velocity_function & i_cVel_Fn, const long double & i_dRescale_Energy_Ry) const
{
	long double dSum = 0.0;
	long double dScaling = fabs(i_dRescale_Energy_Ry / i_sRecombined_State.m_dEnergy_Ry);
	cimscs cimscsCS_Data = m_mscsPI_Cross_Sections.find(i_sRecombined_State.m_opldDescriptor);
	if (cimscsCS_Data != m_mscsPI_Cross_Sections.end())
	{
		cimddcs iterEnd = cimscsCS_Data->second.end();
		cimddcs iterIm = iterEnd;
		cimddcs iterImm = iterEnd;
		cimddcs iterIp = cimscsCS_Data->second.begin();
		cimddcs iterIpp = iterIp;
		if (iterIpp != iterEnd)
			iterIpp++;
		//printf("1here1\n");
		for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
		{
			if (iterIp != iterEnd)
				iterIp++;
			if (iterIpp != iterEnd)
				iterIpp++;
			long double dE = iterI->first;
			if (dE >= -i_sRecombined_State.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
			{
				long double dE_erg = fabs(dScaling * dE * g_XASTRO.k_dRy);
				long double dFrequency = dE_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
				long double dVelocity = sqrt(fabs(dScaling * dE - i_dRescale_Energy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
				long double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
				//printf("1here2 %Le %Le %Le %Le %e %e\n",dE,dVelocity,dScaling,i_dRescale_Energy_Ry,g_XASTRO.k_dRy,g_XASTRO.k_dme);
				if (dVelocity > 0.0)
				{
					// calculate step size in Ry
					long double dDelta_E;
					if ((iterImm == iterEnd || iterIpp == iterEnd) && iterIm != iterEnd && iterIp != iterEnd)
					{
						dDelta_E = (iterIp->first - iterIm->first) * 0.5;
					}
					else if (iterIp == iterEnd && iterIm != iterEnd)
					{
						dDelta_E = (iterI->first - iterIm->first);
					}
					else if (iterIm == iterEnd && iterIp != iterEnd)
					{
						dDelta_E = (iterIp->first - iterI->first);
					}
					else
					{
						dDelta_E = (iterIpp->first + iterIp->first - iterIm->first - iterImm->first) / 6.0;
					}
					//double dDelta_Freq = dDelta_E * dScaling / g_XASTRO.k_dh; // need to scale here too
					long double ldDelta_Vel = dDelta_E * g_XASTRO.k_dRy / (g_XASTRO.k_dme * dVelocity);

					long double dg_plus = i_sIonized_State.m_opldDescriptor.m_uiS + i_sIonized_State.m_opldDescriptor.m_uiL;
					long double dg_neut = i_sRecombined_State.m_opldDescriptor.m_uiS + i_sRecombined_State.m_opldDescriptor.m_uiL;
					long double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
					long double dMW = i_cVel_Fn(dVelocity);
					long double dF = dE_Rel * dE_Rel * dMW * dVelocity * dCS * ldDelta_Vel;
					//printf("%Le %Le %Le %Le %Le %Le %Le %Le\n",dFrequency,dVelocity,dE_Rel,dMW,dCS,ldDelta_Vel,dF,dSum);

					dSum += dF;
				}

			}
			iterImm = iterIm;
			iterIm = iterI;
		}
	}
	else
	{
		//printf("1ahere1\n");
		long double dDelta_E = 0.1 * -i_sRecombined_State.m_dEnergy_Ry;
		for (unsigned int uiI = 0; uiI < 100; uiI++)
		{
			long double dE = -i_sRecombined_State.m_dEnergy_Ry + uiI * dDelta_E;
			long double dE_erg = fabs(dScaling * dE * g_XASTRO.k_dRy);
			long double dFrequency = dE_erg / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
			long double dVelocity = sqrt(fabs(dScaling * dE - i_dRescale_Energy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
			long double dCS = 0.01 * 1e-18 * (1.0 - uiI / 99.0); // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
			if (dVelocity > 0.0)
			{
				// calculate step size in Ry
				//double dDelta_Freq = dDelta_E * dScaling / g_XASTRO.k_dh; // need to scale here too

				long double dg_plus = i_sIonized_State.m_opldDescriptor.m_uiS + i_sIonized_State.m_opldDescriptor.m_uiL;
				long double dg_neut = i_sRecombined_State.m_opldDescriptor.m_uiS + i_sRecombined_State.m_opldDescriptor.m_uiL;
				long double ldDelta_Vel = dDelta_E / (g_XASTRO.k_dme * dVelocity);
				long double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
				long double dFv = i_cVel_Fn(dVelocity);
				long double dF = dE_Rel * dE_Rel * dFv * dVelocity * dCS * ldDelta_Vel;
				//if (isnan(dF) || isinf(dF))
				//	printf("f(%.2e,%.2e): s%.2e, v%.2e f%.2e er%.2e m%.2e de%.2e f%.2e\n",dE,dE_erg,dScaling,dVelocity,dFrequency,dE_Rel,dMW,dDelta_E,dF);
				dSum += dF;
			}
		}
	}
	return dSum;
}
