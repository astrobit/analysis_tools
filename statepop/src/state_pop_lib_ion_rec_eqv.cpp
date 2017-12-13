#include <statepop.h>

bool statepop::Ion_Recombine_Equivalence(const vecconfig &i_Source_Ion, const vecconfig & i_Candidate)
{ // the source ion may recombine to candidate
	bool bRet = false;
	// need size of source to be one smaller than the candidate
	if (i_Source_Ion.size() == (i_Candidate.size() - 1))
	{
		bRet = true;
//		std::cout << "src size " << i_Source_Ion.size() << " test size " << i_Candidate.size();
//		Print_Config_Vector(i_Candidate,std::cout);
		// check all electrons in the candidate to see if they match the source
		for (unsigned int uiI = 0; uiI < i_Source_Ion.size() && bRet; uiI++)
		{
			bRet = (i_Source_Ion[i_Source_Ion.size() - uiI - 1].m_uin == i_Candidate[i_Candidate.size() - uiI - 1].m_uin &&
					i_Source_Ion[i_Source_Ion.size() - uiI - 1].m_uil == i_Candidate[i_Candidate.size() - uiI - 1].m_uil);
//			if (!bRet)
//			{
//				std::cout << "[" << uiI << "]:(" << i_Source_Ion[i_Source_Ion.size() - uiI - 1].m_uin << "," << i_Source_Ion[i_Source_Ion.size() - uiI - 1].m_uil << ")-)" << 
//							i_Candidate[i_Candidate.size() - uiI - 1].m_uin << "," << i_Candidate[i_Candidate.size() - uiI - 1].m_uil << ")";
//			}
		}
	}
	return bRet;
}
