#include <specfit.h>





double specfit::pEW_Fit(const ES::Spectrum &i_cGenerated, const specfit::feature_parameters & i_fpTarget_Feature, specfit::feature_parameters	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	bool bQuit = false;
	double dFlux_Max = -1.0;
	for (unsigned int uiI = 0; uiI < i_cGenerated.size() && !bQuit; uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL > 8900.0) // assume the peak is between 8400 and 8900 A.
			bQuit = true;
		else if (dWL > 8400.0)
		{
			double dFlux = i_cGenerated.flux(uiI);
			if (dFlux > dFlux_Max)
			{
				uiCaII_Idx = uiI;
				dCaII_P_Cygni_Peak_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}
	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(0);
	double dWL_O_Peak = i_cGenerated.wl(0);
	double dFlux_Ca_Peak = dFlux_Max;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	o_cModel_Data.Reset();
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ);
		double dFlux = i_cGenerated.flux(uiJ);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;

		o_cModel_Data.Process_pEW(dFlux_Eff / dContinuum_Flux,dDel_WL_Ang); // 
		o_cModel_Data.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination
	}

	double dpEW_Err = (o_cModel_Data.m_d_pEW - i_fpTarget_Feature.m_d_pEW) * 10.0; // 10.0 = extra weight to pEW
	double dVel_Err = (o_cModel_Data.m_dVmin - i_fpTarget_Feature.m_dVmin);
	double dErr = dpEW_Err * dpEW_Err + dVel_Err * dVel_Err * 0.01; 
	// 0.01 is a weight applied to the velocity component so that it doesn't overpower pEW err
	/// pEW is a value of ~100, with expected error of order 50
	// velocity is a value of ~10000, with expected error of order 500
	return dErr;
}
