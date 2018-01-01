#include <statepop.h>
#include <cmath>
/*
size_t statepop::Get_Relative_Level(size_t i_uiN, size_t i_uiL)
{
	size_t tLevel = 0;
	if (i_uiN > 0 && i_uiL < i_uiN)
	{
		size_t tComb = i_uiN + i_uiL;
	
		// sum for levels below (n + l)
		for (size_t tN = 1; tN < tComb; tN++)
			tLevel += ((tN + 1) >> 1); 
		// for the user specified state
		size_t tL = i_uiL + 1;
		for (size_t tN = i_uiN - 1; tN > tL; tN--)
		{
			tLevel++;
			tL++;
		}
	}	
	return tLevel;
}*/


statepop::vecconfig statepop::Get_Default_Configuration(unsigned int i_uiNe)
{
	vecconfig vRet;
	size_t tComb = 1;
	while (vRet.size() < i_uiNe)
	{
		for (size_t tN = ((tComb + 1) >> 1); tN <= tComb; tN++)
		{
			size_t tL = tComb - tN;
			if (tL < tN)
			{
				size_t tMult = 2 + (tL << 2);
				for (size_t tX = 0; tX < tMult && vRet.size() < i_uiNe; tX++)
					vRet.push_back(config(tN,tL));
			}
		}
		tComb++;
	}
	vRet.invert(); // highest energy state in index 0
	return vRet;
}

bool statepop::vecconfig::operator ==(const statepop::vecconfig & i_vccRHO) const
{
	bool bRet = true;
	bRet = (vccData.size() == i_vccRHO.size());
	for (unsigned int uiI = 0; uiI < vccData.size() && bRet; uiI++)
	{
		bRet &= (vccData[uiI].m_uin == i_vccRHO.vccData[uiI].m_uin);
		bRet &= (vccData[uiI].m_uil == i_vccRHO.vccData[uiI].m_uil);
		bRet &= (vccData[uiI].m_uiS == i_vccRHO.vccData[uiI].m_uiS || vccData[uiI].m_uiS == -1 || i_vccRHO.vccData[uiI].m_uiS == -1);
		bRet &= (vccData[uiI].m_uiL == i_vccRHO.vccData[uiI].m_uiL || vccData[uiI].m_uiL == -1 || i_vccRHO.vccData[uiI].m_uiL == -1);
		bRet &= (vccData[uiI].m_uiP == i_vccRHO.vccData[uiI].m_uiP || vccData[uiI].m_uiP == -1 || i_vccRHO.vccData[uiI].m_uiP == -1);
		bRet &= (vccData[uiI].m_dJ == i_vccRHO.vccData[uiI].m_dJ || vccData[uiI].m_dJ == -1.0 || i_vccRHO.vccData[uiI].m_dJ == -1.0);
		//@@TODO -- is it really OK to require K be identical?
		bRet &= (vccData[uiI].m_dK == i_vccRHO.vccData[uiI].m_dK);
	}
	return bRet;
}
