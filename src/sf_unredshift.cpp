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
//	Deredden
//
//
//
//------------------------------------------------------------------------------

void specfit::Unredshift(std::vector <specfit::fit> &io_vfitFits)
{
	// process each model, and make sure that it has all of the needed info; fill the data points in the vector in the fit class for each one.
	for (std::vector <specfit::fit>::iterator iterFit = io_vfitFits.begin(); iterFit != io_vfitFits.end(); iterFit++)
	{
		if (!std::isnan(iterFit->m_dE_BmV) && iterFit->m_dE_BmV != 0.0 && !iterFit->m_vData.empty())
		{
			std::cout << "Computng rest frame wavelength for " << std::setprecision(7) << iterFit->m_dMJD << " from " << iterFit->m_szSource << " using " << iterFit->m_szInstrument << " with z = " << std::scientific << std::setprecision(4) << iterFit->m_dRedshift << std::endl;
			iterFit->m_vData.Unredshift(iterFit->m_dRedshift);
		}
	}
}

