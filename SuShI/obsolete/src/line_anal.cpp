#include <line_anal.h>
#include <math.h>
#include <xastro.h>

XASTRO_ATOMIC_IONIZATION_DATA_LIST	g_xAstro_Ionization_Energy_Data;
XASTRO_ATOMIC_PARTITION_FUNCTION_DATA_LIST	g_xAstro_Partition_Fn_Data;

double Optical_Depth(const double & i_dMass_Density_Ref, const LINE_DATA & i_cLine_Data, const double & i_dFluid_Temp, const double & i_dTime, const double &i_dTime_Ref)
{
	return i_dMass_Density_Ref* i_cLine_Data.dTau_Coeff * i_dTime_Ref * i_dTime_Ref * i_dTime_Ref / (i_cLine_Data.uiAtomic_Number * g_XASTRO.k_dmh * i_dTime * i_dTime) * exp(-i_cLine_Data.dExcit_Temp_K / i_dFluid_Temp);
}

