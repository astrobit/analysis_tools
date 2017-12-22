#include<cstdio>
#include<cstdlib>
#include <opacity_project_pp.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	long double ldNe = 1.0e12;

	Planck_radiation_field rfPlanck(16000.0);
	opacity_project_element cElem;
	cElem.Read_Element_Data(20);
	Maxwellian_velocity_function vfMaxwell(16000.0);


	opacity_project_level_descriptor cLevel;
	cLevel.m_uiZ = 20;
	cLevel.m_uiN = 19;
	cLevel.m_uiS = 2;
	cLevel.m_uiL = 2;
	cLevel.m_uiP = 0;
	cLevel.m_uiLvl_ID = 1;
	opacity_project_state sState = cElem.Find_State(cLevel);
	FILE * fileOP = fopen("testop.csv","wt");
	long double ldSumI = 0.0, ldSumR = 0.0;
	
	if (fileOP != nullptr)
	{
		fprintf(fileOP,"E (Ryd), Freq (Hz), Delta Freq, velocity (cm/s), Delta velocity, Wavelength (Angstrom), J (photons/cm^2/s/Hz), R, CS, Vel\n");

		cimscs cimscsCS_Data = cElem.m_vopiIon_Data[cLevel.m_uiN - 1].m_mscsPI_Cross_Sections.find(sState.m_opldDescriptor);
		if (cimscsCS_Data !=  cElem.m_vopiIon_Data[cLevel.m_uiN - 1].m_mscsPI_Cross_Sections.end())
		{
			cimddcs iterEnd = cimscsCS_Data->second.end();
			cimddcs iterIm = iterEnd;
			cimddcs iterImm = iterEnd;
			cimddcs iterIp = cimscsCS_Data->second.begin();
			cimddcs iterIpp = iterIp;
			if (iterIpp != iterEnd)
				iterIpp++;
			for (cimddcs iterI = cimscsCS_Data->second.begin(); iterI != iterEnd; iterI++)
			{
				if (iterIp != iterEnd)
					iterIp++;
				if (iterIpp != iterEnd)
					iterIpp++;
				long double dE = iterI->first;
				if (dE >= -sState.m_dEnergy_Ry) // energies below ionization threshold are included in order to approximate transitions to the continuum of states just below ionization
				{
					long double dE_Res = dE * g_XASTRO.k_dRy; // photon energy in erg
					long double dFrequency = dE_Res / g_XASTRO.k_dh; // rescale energy here to determine frequency of the photons we want
					long double dCS = iterI->second * 1e-18; // calculate cross-section in cm^2/Ry; 1e-18 cm^2/Mb
					long double dJ = rfPlanck.Get_Energy_Flux_freq(dFrequency,0.0);
					long double dJ_Ry = dJ / g_XASTRO.k_dRy; // radiation field in Ry / (cm^2 s Hz)
					long double dJ_n = dJ / dE_Res; // radiation field in photons / (cm^2 s Hz)

					long double dE_erg = fabs(dE * g_XASTRO.k_dRy);
					long double dVelocity = sqrt(fabs(dE+sState.m_dEnergy_Ry) * 2.0 * g_XASTRO.k_dRy / g_XASTRO.k_dme);
					long double ldR = 0.0;
					long double dDelta_E;
					// calculate step size in Ry
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
					long double dFv = vfMaxwell(dVelocity);
					long double dDelta_Freq = fabs(dDelta_E * g_XASTRO.k_dRy / g_XASTRO.k_dh); // need to scale here too
					long double ldDelta_Vel = dDelta_E / (g_XASTRO.k_dme * dVelocity);
					long double dE_Rel = dE_erg / (g_XASTRO.k_dme * g_XASTRO.k_dc * dVelocity);
					ldR = 0.0;
					if (dVelocity > 0.0)
					{
						ldR = dE_Rel * dE_Rel * dFv * dVelocity;
					}

					long double dI = 4.0 * g_XASTRO.k_dpi * dJ_n;
					fprintf(fileOP,"%.17Le, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le, %.17Le\n",
						dE, dFrequency, dDelta_Freq, dVelocity, ldDelta_Vel, g_XASTRO.k_dc / dFrequency * 1.0e8, dI, ldR, dCS,dFv,dE_Rel);
					dI *= dCS * dDelta_Freq;
					ldR *= dCS * ldDelta_Vel;

	//					printf("f(%.2e,%.2e): %.2e, %.2e %.2e %.2e %.2e %.2e %.2e %.2e\n",dE,dE_Res,dScaling,dJ_Ry,dFrequency,dCS,dE_Res,dDelta_Freq,dDelta_E,dF);
					ldSumI += dI;
					ldSumR += ldR;
				}
				iterImm = iterIm;
				iterIm = iterI;
			}
		}
		printf("%.4Le\t%.4Le\n",ldSumI,ldSumR);
	}

	return 0;
}
