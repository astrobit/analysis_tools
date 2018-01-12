#include <kurucz_data.h>

void kurucz_derived_data::Compute_H_Z(const radiation_field & i_rfRad, const long double & i_dRedshift, const long double & i_dFlux_Dilution)
{
	for (vvkld::iterator iterI = m_kdKurucz_Data.m_vvkldLine_Data.begin(); iterI != m_kdKurucz_Data.m_vvkldLine_Data.end(); iterI++)
	{
		long double dIon_State = (iterI->begin()->m_dElement_Code - m_kdKurucz_Data.m_uiN) * 100.0; // 0.01 for rounding error
		if (dIon_State <= (m_kdKurucz_Data.m_uiMax_Ion + 0.02))
		{
			for (ivkld iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
			{
				iterJ->Compute_Z(i_rfRad,i_dRedshift,i_dFlux_Dilution);
			}
		}
	}

	for (vmklvd::iterator iterI = m_vmklvdLevel_Data.begin(); iterI != m_vmklvdLevel_Data.end(); iterI++)
	{
		for (mklvd::iterator iterJ = iterI->begin(); iterJ != iterI->end(); iterJ++)
		{
			// calculate Z (Eq 12) for Level j
			iterJ->second.klvdLevel_Data.m_dZ = 0.0;

			for (ivivkld iterK = iterJ->second.vivkldAbsorption_Transition_Data.begin(); iterK != iterJ->second.vivkldAbsorption_Transition_Data.end(); iterK++)
			{
				iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dH_abs;
			
			}
			for (ivivkld iterK = iterJ->second.vivkldEmission_Transition_Data.begin(); iterK != iterJ->second.vivkldEmission_Transition_Data.end(); iterK++)
			{
				iterJ->second.klvdLevel_Data.m_dZ += (*iterK)->m_dH_em;
			}
		}
	}
}
