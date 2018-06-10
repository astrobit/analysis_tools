#include <eps_plot.h>

/*class stringpart
{
public:
	std::string sPart;
	
	stringpart * pcSuper;
	stringpart * pcSub;
	stringpart * pcNext;

	stringpart(void)
	{
		pcSuper = nullptr;
		pcSub = nullptr;
		pcNext = nullptr;
	}

};*/

double epsplot::data::Estimate_Text_Width(const std::string &i_sString,const epsplot::text_parameters & i_cText_Param) const
{//@@TODO: This is obviously really wrong, but computing the size of text, especially when there are superscripts, subscripts, and special characters, is non-trivial
// neeed to redo this once a proper parser for the string is written
	double dLength = i_cText_Param.m_iFont_Size * 0.8 * i_sString.size();
	return dLength;
}

//generic method for estimating height. If there is a character with a descent (e.g. 'g') make the height 33% bigger. If there is a super and/or subscript, make the line 50 bigger.

double epsplot::data::Estimate_Text_Height(const std::string &i_sString,const epsplot::text_parameters & i_cText_Param) const
{
	double dSize = i_cText_Param.m_iFont_Size;
	bool bHas_Superscript = false;
	bool bHas_Subscript = false;
	bool bHas_Descent = false;
	for (auto iterI = i_sString.cbegin(); iterI != i_sString.cend(); iterI++)
	{
		bHas_Descent |= (*iterI == 'g' || *iterI == 'p' || *iterI == 'y' || *iterI == 'j' || *iterI == 'q');
		bHas_Superscript |= (*iterI == '^');
		bHas_Subscript |= (*iterI == '_');
	}
	if (!bHas_Subscript && bHas_Descent)
		dSize += i_cText_Param.m_iFont_Size * 0.33;
	if (bHas_Subscript && !bHas_Descent)
		dSize += i_cText_Param.m_iFont_Size * 0.50;
	if (bHas_Superscript)
		dSize += i_cText_Param.m_iFont_Size * 0.50;

	return dSize;
}

