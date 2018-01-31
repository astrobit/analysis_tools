#include <opacity_project_pp.h>

long double opacity_project_ion::Calc_Ionizing_Rate(const opacity_project_state &i_sState, const radiation_field & i_cRad, const long double & i_dRescale_Energy_Ry, const long double &  i_dRedshift) const
{
	long double dSum = 0.0;
	cimscs cimscsCS_Data = m_mscsPI_Cross_Sections.find(i_sState.m_opldDescriptor);
	long double dScaling = fabs(i_dRescale_Energy_Ry / i_sState.m_dEnergy_Ry);
	//printf("2here0 %Le %Le %i %i %i%i%i %i\n",i_dRescale_Energy_Ry,i_sState.m_dEnergy_Ry,i_sState.m_opldDescriptor.m_uiZ,i_sState.m_opldDescriptor.m_uiN,i_sState.m_opldDescriptor.m_uiS,i_sState.m_opldDescriptor.m_uiL,i_sState.m_opldDescriptor.m_uiP,i_sState.m_opldDescriptor.m_uiLvl_ID);
	if (cimscsCS_Data != m_mscsPI_Cross_Sections.end())
	{
		cimddcs iterEnd = cimscsCS_Data->second.end();
		cimddcs iterIm = iterEnd;
		cimddcs iterImm = iterEnd;
		cimddcs iterIp = cimscsCS_Data->second.begin();
		cimddcs iterIpp = iterIp;
		if (iterIpp != iterEnd)
			iterIpp++;
		//printf("2here1\n");
		for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
		{
			if (iterIp != iterEnd)
				iterIp++;
			if (iterIpp != iterEnd)
				iterIpp++;
			long double dE = iterI->first;
			//printf("2here2 %Le\n",dE);
			if (dE >= i_sState.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
			{
				long double dE_Res = dScaling * dE * g_XASTRO.k_dRy; // photon energy in erg
				long double dFrequency = dE_Res / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
				long double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
				long double dJ = i_cRad.Get_Energy_Flux_freq(dFrequency,i_dRedshift);
				long double dJ_Ry = dJ / g_XASTRO.k_dRy; // radiation field in Ry / (cm^2 s Hz)
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
				long double dDelta_Freq = fabs(dDelta_E * g_XASTRO.k_dRy * dScaling / g_XASTRO.k_dh); // need to scale here too

				long double dF = 4.0 * g_XASTRO.k_dpi * dJ / dE_Res * dCS * dDelta_Freq;
				//printf("2here3 %Le %Le %Le %Le %Le %Le\n",dE_Res,dCS,dJ,dDelta_Freq,dF,dSum);
//					printf("f(%.2e,%.2e): %.2e, %.2e %.2e %.2e %.2e %.2e %.2e %.2e\n",dE,dE_Res,dScaling,dJ_Ry,dFrequency,dCS,dE_Res,dDelta_Freq,dDelta_E,dF);
				dSum += dF;
			}
			iterImm = iterIm;
			iterIm = iterI;
		}
	}
	return dSum;
}
