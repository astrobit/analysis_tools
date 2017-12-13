#include <statepop.h>

std::map<size_t,statepop::saha_info> statepop::Get_Saha_Populations(void)
{
	std::map<size_t,saha_info> vsiInfo;
	statepop::floattype dKb_T = (g_XASTRO.k_dKb * m_cParam.dElectron_Kinetic_Temperature_K);
	statepop::floattype dInv_Kb_T = (statepop::floattype)(1.0) / dKb_T;
	statepop::floattype dLog_e = std::log10(std::exp((statepop::floattype)(1.0)));
	statepop::floattype dLambda = g_XASTRO.k_dh * g_XASTRO.k_dh / (2.0 * g_XASTRO.k_dpi * g_XASTRO.k_dme) * dInv_Kb_T;
	statepop::floattype dTot_Pop = 1.0;
	statepop::floattype dLog_Rel_Pop_Sum = 0.0;
	for (size_t uiState = vdOP_Ground_State.size(); uiState < vdOP_Ground_State.size(); uiState--)
	{
		statepop::floattype dExponential_Term = -dInv_Kb_T * std::fabs(vdOP_Ground_State[uiState] * g_XASTRO.k_dRy);
		statepop::floattype dLog_Rel_Pop = std::log10(2.0 / (dLambda * std::sqrt(dLambda))) + dExponential_Term * dLog_e - std::log10(m_cParam.dNe);
		dLog_Rel_Pop_Sum += dLog_Rel_Pop;
		vsiInfo[m_cParam.uiElement_Z - (uiState + m_cParam.uiElement_Min_Ion_Species)] = saha_info(m_cParam.uiElement_Z - (uiState + m_cParam.uiElement_Min_Ion_Species),vdOP_Ground_State[uiState],dLog_Rel_Pop_Sum);
		dTot_Pop += std::pow(10.0,dLog_Rel_Pop_Sum);
	}
	vsiInfo[m_cParam.uiElement_Min_Ion_Species] = saha_info(m_cParam.uiElement_Min_Ion_Species,vdOP_Ground_State[0],0.0);
	dTot_Pop = std::log10(dTot_Pop);
	dLog_Rel_Pop_Sum = 0.0;
	for (size_t uiState = vdOP_Ground_State.size(); uiState < vdOP_Ground_State.size(); uiState--)
	{
		vsiInfo[m_cParam.uiElement_Z - (uiState + m_cParam.uiElement_Min_Ion_Species)].ldLog_Pop -= dTot_Pop;
	}
	return vsiInfo;
}
