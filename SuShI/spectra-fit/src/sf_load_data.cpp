#include <specfit.h>

//------------------------------------------------------------------------------
//
//
//
//	Load_Data
//
//
//
//------------------------------------------------------------------------------

void specfit::Load_Data(
	std::vector <specfit::fit> &io_vfitFits,
	std::map< std::string, OSCfile> &i_mJson_Data,
	std::map< std::string, OSCspectrum > &i_mNon_Json_Data)
{
	// process each model, and make sure that it has all of the needed info; fill the data points in the vector in the fit class for each one.
	for (std::vector <specfit::fit>::iterator iterFit = io_vfitFits.begin(); iterFit != io_vfitFits.end(); iterFit++)
	{
		if (i_mJson_Data.count(iterFit->m_szDatafile) > 0) /// looking at json data file
		{
			OSCfile jvData = i_mJson_Data[iterFit->m_szDatafile];
			std::string szID = jvData.m_szSupernova_ID;
			if (iterFit->m_bUse_JSON_ebv) // user has requested to use ebv data from the JSON file
			{
				iterFit->m_dE_BmV = jvData.m_dEBV_Mean;
			}
			iterFit->m_dRedshift = jvData.m_dRedshift_Weighted_Mean;
			OSCspectra_id odcidID;
			odcidID.m_dDate_MJD = iterFit->m_dMJD;
			odcidID.m_szInstrument = iterFit->m_szInstrument;
			odcidID.m_szSources = iterFit->m_szSource;

			if (i_mJson_Data[iterFit->m_szDatafile].m_mSpectra_List.count(odcidID) > 0)
			{
				iterFit->m_vData = i_mJson_Data[iterFit->m_szDatafile].m_mSpectra_List[odcidID];
			}
		} //if (mJson_Data.count(iterFit->m_szDatafile) > 0) /// looking at json data file
	}// for (std::vector <specfit::fit>::iterator iterFit = o_vfitFits.begin(); ...
}
