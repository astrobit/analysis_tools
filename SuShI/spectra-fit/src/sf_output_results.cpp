#include <specfit.h>


//------------------------------------------------------------------------------
//
//
//
//	Output_Results
//
//
//
//------------------------------------------------------------------------------

void specfit::Output_Results(std::ofstream & io_fsBest_Fits, const best_fit_results & i_vResults)
{

	for (best_fit_results::const_iterator iterI = i_vResults.cbegin(); iterI != i_vResults.cend(); iterI++)
	{
		Write_Fit(io_fsBest_Fits,*iterI);
	}

}

