#include <iostream>
#include <fstream>
#include <json.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <cmath>
#include <xio.h>
#include <opacity_profile_data.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <specfit.h>
#include <line_routines.h>
#include <cstdio>
#include <iomanip>
#include <ios>
	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <eps_plot.h>

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

