#include <specfit.h>



void specfit::Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const params_range &i_cNorm_Range, double & o_dTarget_Flux, double & o_dGenerated_Flux)
{

	o_dTarget_Flux = Get_Norm_Const(i_cTarget,i_cNorm_Range.m_dBlue_WL,i_cNorm_Range.m_dRed_WL);
	o_dGenerated_Flux = Get_Norm_Const(i_cGenerated,i_cNorm_Range.m_dBlue_WL,i_cNorm_Range.m_dRed_WL);

}
