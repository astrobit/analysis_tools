#include <state_pops.h>

bool Ion_Recombine_Equivalence(const std::vector<config> &i_Source_Ion, const std::vector<config> & i_Candidate)
{ // the source ion may recombine to candidate
	bool bRet = false;
	unsigned int uiI = 0;
	// need size of source to be one smaller than the candidate
	if (i_Source_Ion.size() == (i_Candidate.size() - 1))
	{
		bool bRet = true;
		// check all electrons in the candidate to see if they match the source
		for (unsigned int uiI = 0; uiI < i_Source_Ion.size() && bRet; uiI++)
		{
			bRet = (i_Source_Ion[i_Source_Ion.size() - uiI - 1] == i_Candidate[i_Candidate.size() - uiI - 1]);
		}
	}
	return bRet;
}
