#include <statepop.h>

std::map<size_t,statepop::boltzmann_info> statepop::Get_Boltzmann_Populations(void)
{
	std::map<size_t,boltzmann_info> mbiRet;
	size_t tLevel_ID = 0;
	floattype dInv_Kb_T = (floattype)(1.0) / (floattype)(g_XASTRO.k_dKb * m_cParam.dElectron_Kinetic_Temperature_K);
	floattype dLog_e = std::log10(std::exp((floattype)(1.0)));
	std::map<size_t,saha_info> mSaha = Get_Saha_Populations();

	for (size_t tIon = m_cParam.uiElement_Min_Ion_Species; tIon <= m_cParam.uiElement_Max_Ion_Species; tIon++)
	{
		std::vector<floattype> vldZ;
		floattype ldZsum = 0.0;
		size_t tLevel_ID_Start = tLevel_ID;
		for (auto iterJ = kddData.m_vmklvdLevel_Data[tIon].begin(); iterJ != kddData.m_vmklvdLevel_Data[tIon].end(); iterJ++)
		{ // do all pops relative to ground state
			double dZi = std::log10(iterJ->second.klvdLevel_Data.m_dStat_Weight) + iterJ->second.klvdLevel_Data.m_dEnergy_Level_Ryd * g_XASTRO.k_dRy * dInv_Kb_T * dLog_e;
			vldZ.push_back(dZi);
			ldZsum += std::pow(10.0,dZi);
			mbiRet[tLevel_ID].tLevel_ID = tLevel_ID;
			mbiRet[tLevel_ID].ldLog_Pop = dZi;
			tLevel_ID++;
		}
		ldZsum = std::log10(ldZsum);
		for (size_t tI = 0; tI < vldZ.size(); tI++)
		{
			mbiRet[tLevel_ID_Start + tI].ldLog_Pop -= ldZsum + mSaha[tIon].ldLog_Pop ;
		}
	}
	return mbiRet;
}
