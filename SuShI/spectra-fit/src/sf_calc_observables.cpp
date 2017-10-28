#include <specfit.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>

void specfit::Calc_Observables(const ES::Spectrum &i_cGenerated,const ES::Spectrum &i_cContinuum, specfit::fit_result	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	double dCaII_P_Cygni_Peak_flux;
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
	dCaII_P_Cygni_Peak_flux = dFlux_Max;
	unsigned int uiBlue_Idx;
	double	dBlue_WL;
	dFlux_Max = -1.0;
	bQuit = false;
	for (unsigned int uiI = uiCaII_Idx; uiI > 0 && !bQuit; uiI--)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL < 7000.0) // assume the blue edge is between 7000 and 8000 A.
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			uiBlue_Idx = uiI;
			bQuit = true;
			dBlue_WL = dWL;
			dFlux_Max = dFlux;
		}
		else if (dWL < 8000.0)
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			if (dFlux > 0.98)
			{
				uiBlue_Idx = uiI;
				bQuit = true;
				dBlue_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}


	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx - uiBlue_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(uiBlue_Idx);
	double dWL_O_Peak = i_cGenerated.wl(uiBlue_Idx);
	double dFlux_Ca_Peak = dCaII_P_Cygni_Peak_flux;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	xvector vA, vX, vY, vW, vSigma;
	double dSmin;
	gauss_fit_parameters * lpgfpParamters = &g_cgfpCaNIR;

	o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Blue = dWL_O_Peak;
	o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Red = dWL_Ca_Peak;
	vY.Set_Size(uiNum_Points);
	vX.Set_Size(uiNum_Points);
	vW.Set_Size(uiNum_Points);
	o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Reset();
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ + uiBlue_Idx);
		double dFlux = i_cGenerated.flux(uiJ + uiBlue_Idx);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;
		double dNorm_Flux_Eff = dFlux_Eff / dContinuum_Flux;

		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_pEW(dNorm_Flux_Eff,dDel_WL_Ang); // 
		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination

		vX.Set(uiJ,dWL);
		vW.Set(uiJ,0.01); // arbitrary weight
		vY.Set(uiJ,dNorm_Flux_Eff);
	}
	
	o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
                    dDel_WL_Ang, o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit_Uncertainty,  
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Fit_S, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrSingle_Gaussian_Fit, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrDouble_Gaussian_Fit);

}
