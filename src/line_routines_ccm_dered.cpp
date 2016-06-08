#include <line_routines.h>
#include <cmath>

void CCM_dered(ES::Spectrum & i_cSpectrum, const double & i_dE_BmV, const double & i_dRv)
{
	std::vector<std::pair<double,double> > vSpec;
	for (size_t uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		vSpec.push_back(std::pair<double, double> (i_cSpectrum.wl(uiI),i_cSpectrum.flux(uiI)));
	}
	CCM_dered(vSpec,i_dE_BmV,i_dRv);
	for (size_t uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		i_cSpectrum.flux(uiI) = vSpec[uiI].second;
	}
}
void CCM_dered(std::vector<std::tuple<double,double,double> > &io_vSpectrum, const double & i_dE_BmV, const double & i_dRv)
{
	std::vector<std::pair<double,double> > vSpec;
	for (size_t uiI = 0; uiI < io_vSpectrum.size(); uiI++)
	{
			vSpec.push_back(std::pair<double, double> (std::get<0>(io_vSpectrum[uiI]),std::get<1>(io_vSpectrum[uiI])));
	}
	CCM_dered(vSpec,i_dE_BmV,i_dRv);
	for (size_t uiI = 0; uiI < io_vSpectrum.size(); uiI++)
	{
		std::get<1>(io_vSpectrum[uiI]) = vSpec[uiI].second;
	}
}

// deredden a spectrum, using Cardelli, Jason A.; Clayton, Geoffrey C.; Mathis, John S. 1989 ApJ 345, 245
// with correction for optical by O'Donnell 1992 ApJ 422, 158
void CCM_dered(std::vector<std::pair<double,double> > &io_vSpectrum, const double & i_dE_BmV, const double & i_dRv)
{
	if (i_dE_BmV != 0.0 && i_dRv != 0.0)
	{
		double dInv_Rv = 1.0 / i_dRv;
		for (std::vector<std::pair<double,double> >::iterator iterI = io_vSpectrum.begin(); iterI != io_vSpectrum.end(); iterI++)
		{
			double dWL = iterI->first;
			double	dX = 10000.0 / dWL; // convert wl to microns and invert.  Assumes that wl is in Angstroms
			double dA = 1.0, dB = 1.0;
			if (dX > 0.3)
			{
				if (dX < 1.1) // Mid-IR
				{
					double dPow_wl = pow(dX,1.61);
					dA = 0.574 * dPow_wl;
					dB = -0.527 * dPow_wl;
				}
				else if (dX < 3.3) // Near-IR, Optical, Near-UV
				{
					double dY = dX - 1.82; 
					// Original CCM coefficients, 7th order polynomial
	//				dA = 1.0 + dY * (0.17699 + dY * (-0.50447 + dY * (-0.02427 + dY * ( 0.72085 + dY * ( 0.01979 + dY * (-0.77530 + dY * 0.32999))))));
	//				dB =       dY * (1.41338 + dY * ( 2.28305 + dY * ( 1.07233 + dY * (-5.38434 + dY * (-0.62251 + dY * ( 5.32060 + dY * -2.09002))))));
					// O'Donnell coefficients, 8th order polynomial
					dA = 1.0 + dY * (0.104 + dY * (-0.609 + dY * ( 0.701 + dY * ( 1.137 + dY * (-1.718 + dY * (-0.827 + dY * (1.647 + dY * -0.505)))))));
					dB =       dY * (1.952 + dY * ( 2.908 + dY * (-3.989 + dY * (-7.985 + dY * (11.102 + dY * ( 5.491 + dY * (-10.805 + dY * 3.347)))))));
				}
				else if (dX < 8.0) // Mid-UV
				{
					double dY = dX - 4.67;
					double dZ = dX - 4.62;
					double dW = dX - 5.9;
					double dFa = 0.0, dFb = 0.0;
					if (dW > 0.0)
					{
						dFa = dW * dW * (-0.04473 - 0.09779 * dW);
						dFb = dW * dW * (0.2130 + 0.1207 * dW);
					}

					dA = 1.752 - 0.316 * dX - 0.104 / (dY * dY + 0.341) + dFa;
					dB = -3.090  + 1.825 * dX + 1.206 / (dZ * dZ + 0.263) + dFb;
				}
				else if (dX < 10.0) // Far-UV
				{
					double dY = dX - 8.0;
					dA = -1.073 + dY * (-0.628 + dY * ( 0.137 + dY * -0.070));
					dB = 13.670 + dY * ( 4.257 + dY * (-0.420 + dY *  0.374));
				}
				double dAv = i_dRv * i_dE_BmV;
				double dAlambda = dAv * dA + dB * dInv_Rv;
	 			iterI->second *= pow(10.0,0.4 * dAlambda);
			}
		
		}
	}
}

