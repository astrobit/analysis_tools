#include <specfit.h>


//------------------------------------------------------------------------------
//
//
//
//	Load_Models
//
//
//
//------------------------------------------------------------------------------
void specfit::Load_Models(
	std::vector <specfit::fit> &i_vfitFits,
	std::map< std::string, std::vector<unsigned int> > &i_mModel_Lists,
	std::map< unsigned int, model> &o_mModel_Data
	)
{
	// process model lists

	for (std::map< std::string, std::vector<unsigned int> >::iterator iterML = i_mModel_Lists.begin(); iterML != i_mModel_Lists.end(); iterML++)
	{
		for (std::vector<unsigned int>::iterator iterMod = iterML->second.begin(); iterMod != iterML->second.end(); iterMod++)
		{
			o_mModel_Data[*iterMod] = model(*iterMod);
		} // for (std::vector<unsigned int>::iterator iterMod = iterML->second.begin(); ...
	} // for (std::map< std::string, std::vector<unsigned int> >::iterator iterML = mModel_Lists.begin(); ...

	// process each model, and make sure that it has all of the needed info
	for (std::vector <specfit::fit>::iterator iterFit = i_vfitFits.begin(); iterFit != i_vfitFits.end(); iterFit++)
	{
		// process individual models
		for (std::vector <unsigned int>::iterator 	iterMod = iterFit->m_vuiModels.begin(); iterMod != iterFit->m_vuiModels.end(); iterMod++)
		{
			o_mModel_Data[*iterMod] = model(*iterMod);
		} // for (std::vector <unsigned int>::iterator 	iterMod = iterFit->m_vuiModels.begin(); ...
	}
}
