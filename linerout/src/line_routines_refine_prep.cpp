#include <line_routines.h>

void Refine_Prep(const spectrum_data & i_cData, unsigned int i_uiModel_ID, ES::Spectrum & o_cTarget, msdb::USER_PARAMETERS &o_cUser_Param, opacity_profile_data::group & o_eModel_Group, bool & o_bIon_Valid)
{
	o_cUser_Param.m_uiModel_ID = i_uiModel_ID;
	o_cUser_Param.m_uiIon = i_cData.m_uiIon;

	o_cUser_Param.m_dTime_After_Explosion = 1.0;
	o_cUser_Param.m_dPhotosphere_Velocity_kkms = i_cData.m_dPS_Velocity;
	o_cUser_Param.m_dPhotosphere_Temp_kK = i_cData.m_dPS_Temp;
	o_cUser_Param.m_dEjecta_Log_Scalar = i_cData.m_dEjecta_Scalar;
	o_cUser_Param.m_dShell_Log_Scalar = i_cData.m_dShell_Scalar;
	o_cUser_Param.m_dEjecta_Effective_Temperature_kK = i_cData.m_dExc_Temp;
	o_cUser_Param.m_dShell_Effective_Temperature_kK = i_cData.m_dExc_Temp;
	o_cUser_Param.m_dWavelength_Range_Lower_Ang = i_cData.m_specResult[0].wl(0);
	o_cUser_Param.m_dWavelength_Range_Upper_Ang = i_cData.m_specResult[0].wl(i_cData.m_specResult[0].size() - 1);
	o_cUser_Param.m_dWavelength_Delta_Ang = i_cData.m_specResult[0].wl(1) - i_cData.m_specResult[0].wl(0);
	o_cUser_Param.m_dEjecta_Scalar_Time_Power_Law = -2.0;
	o_cUser_Param.m_dShell_Scalar_Time_Power_Law = -2.0;

	unsigned int uiElem = o_cUser_Param.m_uiIon / 100;
	o_bIon_Valid = false;
	switch (uiElem)
	{
	case 6:
		o_eModel_Group = opacity_profile_data::carbon;
		o_bIon_Valid = true;
		break;
	case 8:
		o_eModel_Group = opacity_profile_data::oxygen;
		o_bIon_Valid = true;
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		o_eModel_Group = opacity_profile_data::magnesium;
		o_bIon_Valid = true;
		break;
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		o_eModel_Group = opacity_profile_data::silicon;
		o_bIon_Valid = true;
		break;
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
		o_eModel_Group = opacity_profile_data::iron;
		o_bIon_Valid = true;
		break;
	}
	o_cUser_Param.m_eFeature = msdb::Generic;
		

	o_cTarget = i_cData.m_specResult[0];
}
