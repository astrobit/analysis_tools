#include <line_routines.h>
#include <cmath>
#include <xastro.h>

// deredden a spectrum, using Cardelli, Jason A.; Clayton, Geoffrey C.; Mathis, John S. 1989 ApJ 345, 245
// with correction for optical by O'Donnell 1992 ApJ 422, 158


// version using ES spectrum
void CCM_dered(ES::Spectrum & i_cSpectrum, const double & i_dE_BmV, const double & i_dRv)
{
	std::vector<std::pair<double,double> > vSpec;
	for (size_t uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		vSpec.push_back(std::pair<double, double> (i_cSpectrum.wl(uiI),i_cSpectrum.flux(uiI)));
	}
	XA_CCM_dered(vSpec,i_dE_BmV,i_dRv);
	for (size_t uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		i_cSpectrum.flux(uiI) = vSpec[uiI].second;
	}
}

