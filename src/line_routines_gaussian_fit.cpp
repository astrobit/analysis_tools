#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include <xfit.h>
#include "ES_Synow.hh"
#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>

GAUSS_FIT_PARAMETERS	g_cgfpCaNIR(8662.14,160.0,8542.09,170.0,8498.02,130.0);
GAUSS_FIT_PARAMETERS	g_cgfpCaHK(3934.77,230.0,3969.59,220.0);
GAUSS_FIT_PARAMETERS	g_cgfpOINIR(774.08,870.0,776.31,810.0,7777.53,750.0);
GAUSS_FIT_PARAMETERS	g_cgfpSi6355(6348.85,1000.0,6373.12,1000.0);
GAUSS_FIT_PARAMETERS	g_cgfpSi5968(5959.21,500.0,5980.59,500.0);

GAUSS_FIT_PARAMETERS	lg_cUser_Paramters;
double					lg_dUser_Sigma = 0.0;
double					lg_dNorm;
 
XVECTOR Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData)
{
	XVECTOR vOut;

	if (i_lpvData)
	{
		vOut.Set_Size(4); // function, and derivatives wrt each a parameter
		GAUSS_FIT_PARAMETERS *lpvParam = (GAUSS_FIT_PARAMETERS *) i_lpvData;
		double	dDelx_X, dDel_X_Sigma_1, dDel_X_Sigma_2, dDel_X_Sigma_3 = 0.0;
		double	dExp_1, dExp_2, dExp_3 = 0.0;
		double	dDel_X;
		double	dInv_Sigma;

		dInv_Sigma = 1.0 / i_vA.Get(1);

		if (lg_cUser_Paramters != lpvParam[0] || lg_dUser_Sigma != i_vA.Get(1))
		{ // calculate the normalization coefficient; it's a bit wonky because of how the 'x' component of the Gaussian is calclated
			lg_dUser_Sigma = i_vA.Get(1);
			lg_cUser_Paramters = lpvParam[0];

			dDel_X_Sigma_2 = 0.0;

			dDel_X = i_vA.Get(2) * (1.0 / lpvParam->m_dW_Ratio_1 - 1.0);
			dDel_X_Sigma_1 = dDel_X * dInv_Sigma;

			dExp_1 = exp(-0.5 * dDel_X_Sigma_1 * dDel_X_Sigma_1) * lpvParam->m_dH_Ratio_1;
			dExp_2 = exp(-0.5 * dDel_X_Sigma_2 * dDel_X_Sigma_2);
			dExp_3 = 0.0;

			if (lpvParam->m_dW_Ratio_2 != 0.0)
			{
				dDel_X = i_vA.Get(2) * (1.0 / lpvParam->m_dW_Ratio_2 - 1.0);
				dDel_X_Sigma_3 = dDel_X * dInv_Sigma;
				dExp_3 = exp(-0.5 * dDel_X_Sigma_3 * dDel_X_Sigma_3) * lpvParam->m_dH_Ratio_2;
			}
			lg_dNorm = 1.0 / (dExp_1 + dExp_2 + dExp_3);
		}

		dDel_X = i_dX - i_vA.Get(2);
		dDel_X_Sigma_2 = dDel_X * dInv_Sigma;


		dDel_X = i_dX / lpvParam->m_dW_Ratio_1 - i_vA.Get(2);
		dDel_X_Sigma_1 = dDel_X * dInv_Sigma;

		dExp_1 = exp(-0.5 * dDel_X_Sigma_1 * dDel_X_Sigma_1) * lpvParam->m_dH_Ratio_1;
		dExp_2 = exp(-0.5 * dDel_X_Sigma_2 * dDel_X_Sigma_2);
		dExp_3 = 0.0;

		if (lpvParam->m_dW_Ratio_2 != 0.0)
		{
			dDel_X = i_dX / lpvParam->m_dW_Ratio_2 - i_vA.Get(2);
			dDel_X_Sigma_3 = dDel_X * dInv_Sigma;
			dExp_3 = exp(-0.5 * dDel_X_Sigma_3 * dDel_X_Sigma_3) * lpvParam->m_dH_Ratio_2;
		}
		vOut.Set(0,i_vA.Get(0) * (dExp_1 + dExp_2 + dExp_3) * lg_dNorm);
		vOut.Set(1,(dExp_1 + dExp_2 + dExp_3) * lg_dNorm);
		vOut.Set(2,i_vA.Get(0) * lg_dNorm * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3 * dDel_X_Sigma_3));
		vOut.Set(3,i_vA.Get(0) * lg_dNorm * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3));
	}
	return vOut;
}
XVECTOR Multi_Gaussian(const double & i_dX, const XVECTOR & i_vA, void * i_lpvData)
{
	XVECTOR vOut;
	XVECTOR vOut1;
	XVECTOR vOut2;

	XVECTOR vA_lcl;
	if (i_vA.Get_Size() == 6)
	{
		vOut.Set_Size(7);
		vA_lcl.Set_Size(3);
		vA_lcl.Set(0,i_vA.Get(3));
		vA_lcl.Set(1,i_vA.Get(4));
		vA_lcl.Set(2,i_vA.Get(5));

		vOut1 = Gaussian(i_dX,i_vA,i_lpvData);
		vOut2 = Gaussian(i_dX,vA_lcl,i_lpvData);

		vOut.Set(0,vOut1.Get(0) + vOut2.Get(0));
		vOut.Set(1,vOut1.Get(1));
		vOut.Set(2,vOut1.Get(2));
		vOut.Set(3,vOut1.Get(3));
		vOut.Set(4,vOut2.Get(1));
		vOut.Set(5,vOut2.Get(2));
		vOut.Set(6,vOut2.Get(3));
	}
	else if (i_vA.Get_Size() == 3)
	{
		vOut = Gaussian(i_dX,i_vA,i_lpvData);
	}
	return vOut;
}
void Compute_Gaussian_Fit_pEW(const XVECTOR & i_vX, const XVECTOR &i_vA, const double & i_dWavelength_Delta_Ang, const GAUSS_FIT_PARAMETERS * i_lpgfpParameters, double & o_dpEW_PVF, double & o_dpEW_HVF)
{
	o_dpEW_HVF = 0.0;
	o_dpEW_PVF = 0.0;
	for (unsigned int uiJ = 0; uiJ < i_vX.Get_Size(); uiJ++)
	{
		XVECTOR vF = Multi_Gaussian(i_vX.Get(uiJ), i_vA, (void *)i_lpgfpParameters);
		if (i_vA.Get_Size() == 6)
		{
			XVECTOR vAlcl;
			vAlcl.Set_Size(3);
			vAlcl.Set(0,i_vA.Get(0));
			vAlcl.Set(1,i_vA.Get(1));
			vAlcl.Set(2,i_vA.Get(2));

			vF = Gaussian(i_vX.Get(uiJ), vAlcl, (void *)i_lpgfpParameters);
			o_dpEW_HVF -= vF.Get(0) * i_dWavelength_Delta_Ang; // - sign to keep pEW positive

			vAlcl.Set(0,i_vA.Get(3));
			vAlcl.Set(1,i_vA.Get(4));
			vAlcl.Set(2,i_vA.Get(5));

			vF = Gaussian(i_vX.Get(uiJ), vAlcl, (void *)i_lpgfpParameters);
			o_dpEW_PVF -= vF.Get(0) * i_dWavelength_Delta_Ang;
		}
		else
		{
			Gaussian(i_vX.Get(uiJ), i_vA, (void *)i_lpgfpParameters);
			o_dpEW_PVF -= vF.Get(0) * i_dWavelength_Delta_Ang; // - sign to keep pEW positive
		}
	}
}

double Determine_HWHM(const double & i_dHWHM_Estimate, const double & i_dCenter, const double & i_dHWHM_X, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters)
{
//	printf("DHWHM: %f %f %f\n",i_dHWHM_Estimate,i_dCenter,i_dHWHM_X);
	double dHWHM_Low = i_dHWHM_Estimate * 0.50;
	double dHWHM_High = i_dHWHM_Estimate;
	double	dHWHM;
	double dTest;
	unsigned int uiCount = 0;
	XVECTOR vA;
    vA.Set_Size(3);
	vA.Set(0,1.0);
	vA.Set(2,i_dCenter);
	do
	{
		dHWHM = (0.5 * (dHWHM_High + dHWHM_Low));
		vA.Set(1,dHWHM); // HWHM
		dTest = Gaussian(i_dHWHM_X,vA,(void*)i_lpgfpParamters)[0];
		if (dTest > 0.5)
		{
			dHWHM_High = dHWHM;
		}
		else
		{
			dHWHM_Low = dHWHM;
		}
		uiCount++;
	} while (fabs(dTest - 0.5) > 0.001 && (dHWHM_High / dHWHM_Low > 1.01) && uiCount < 64);
	dHWHM = (0.5 * (dHWHM_High + dHWHM_Low));
	return dHWHM;
}

XVECTOR Estimate_Second_Gaussian(const XVECTOR & i_vA, const XVECTOR & i_vX, const XVECTOR & i_vY, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters)
{
	double	dDbl_Center;
	double	dDbl_Amplitude;
	double	dDbl_HWHM;
	unsigned int uiI;
	XVECTOR vRet;
	double dVariance_Min = DBL_MAX;
	unsigned int uiVar_Min_Idx;
	for (unsigned int uiI = 0; uiI < i_vY.Get_Size(); uiI++)
	{
		double dR = i_vY[uiI] - Gaussian(i_vX[uiI],i_vA,(void*)i_lpgfpParamters).Get(0);
		if (dR < dVariance_Min)
		{
			dVariance_Min = dR;
			uiVar_Min_Idx = uiI;
		}
	}
	dDbl_Center = i_vX[uiVar_Min_Idx];
	dDbl_Amplitude = dVariance_Min;
	while (uiVar_Min_Idx > 0 && (i_vY[uiVar_Min_Idx] - Multi_Gaussian(i_vX[uiVar_Min_Idx],i_vA,(void*)i_lpgfpParamters).Get(0)) < (0.5 * dDbl_Amplitude))
		uiVar_Min_Idx--;

	dDbl_HWHM = Determine_HWHM(fabs(i_vX[uiVar_Min_Idx] - dDbl_Center),dDbl_Center,i_vX[uiVar_Min_Idx],i_lpgfpParamters);
	vRet.Set_Size(3);
	vRet.Set(0,dDbl_Amplitude);
	vRet.Set(1,dDbl_HWHM);
	vRet.Set(2,dDbl_Center);

	return vRet;
}
XVECTOR Estimate_Gaussian_Fit(const XVECTOR & i_vX, const XVECTOR & i_vY, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters)
{
    XVECTOR vA;
    XVECTOR vA2;

	double	dYmin = DBL_MAX;
	double	dXmin = 0;
	unsigned int uiXmin = 0;
	double	dYmax = -DBL_MAX;
	double	dXmax = 0;
	unsigned int uiXmax = 0;
	double	dSum = 0.0;
	//std::vector<unsigned int> vuiMinima;
	XVECTOR vSmoothed_Data;

	vSmoothed_Data.Set_Size(i_vY.Get_Size());
	int iSmooth_Size = 15;
	double dSmooth_Coeff = 1.0 / (2 * iSmooth_Size + 1);
	for (unsigned int uiI = 0; uiI < iSmooth_Size; uiI++)
	{
		vSmoothed_Data.Set(uiI,i_vY[0]);
	}
	for (unsigned int uiI = iSmooth_Size; uiI < (i_vY.Get_Size() - iSmooth_Size); uiI++)
	{
		double dVal = 0.0;
		for (int iJ = -iSmooth_Size; iJ <= iSmooth_Size; iJ++)
			dVal += i_vY[uiI + iJ] * dSmooth_Coeff;
		vSmoothed_Data.Set(uiI,dVal);
	}
	for (unsigned int uiI = 0; uiI < iSmooth_Size; uiI++)
	{
		vSmoothed_Data.Set(uiI,i_vY[i_vY.Get_Size() - 1]);
	}


	for (unsigned int uiI = 0; uiI < vSmoothed_Data.size(); uiI++)
	{
		dSum += vSmoothed_Data[uiI];
		if (vSmoothed_Data[uiI] < dYmin)
		{
			dYmin = vSmoothed_Data[uiI];
			dXmin = i_vX.Get(uiI);
			uiXmin = uiI;
		}
		if (vSmoothed_Data[uiI] > dYmax)
		{
			dYmax = vSmoothed_Data[uiI];
			dXmax = i_vX.Get(uiI);
			uiXmax = uiI;
		}
	}
	double dAmplitude = (dYmax - dYmin);
	double dCenter = dXmax;
	double	dHWHM;
	unsigned int uiI;

	unsigned int uiXcenter = uiXmax;
	if (fabs(dYmax) < fabs(dYmin))
	{
///		printf("fitting minimum %i\n",uiXcenter);
		dAmplitude *= -1;
		dCenter = dXmin;
		uiXcenter = uiXmin;
		uiI = uiXcenter;
		while (uiI < vSmoothed_Data.size() && vSmoothed_Data[uiI] < 0.5*dAmplitude)
			uiI++;
		if (uiI == vSmoothed_Data.size())
			uiI--;
//		printf("%i\n",uiI);
		double dXright = i_vX.Get(uiI);
		uiI = uiXcenter;
		while (uiI < 0 && vSmoothed_Data[uiI] < 0.5*dAmplitude)
			uiI--;
//		printf("%i\n",uiI);
		double dXleft = i_vX.Get(uiI);
		dHWHM = fabs(dXright - dXleft);
	}
	else
	{
//		printf("fitting maximum %i\n",uiXcenter);
		uiI = uiXcenter;
		while (uiI < vSmoothed_Data.size() && vSmoothed_Data[uiI] > 0.5*dAmplitude)
			uiI++;
		if (uiI == vSmoothed_Data.size())
			uiI--;
//		printf("%i\n",uiI);
		double dXright = i_vX.Get(uiI);
		uiI = uiXcenter;
		while (uiI > 0 && vSmoothed_Data[uiI] > 0.5*dAmplitude)
			uiI--;
//		printf("%i\n",uiI);
		double dXleft = i_vX.Get(uiI);
		dHWHM = fabs(dXright - dXleft);
	}
	dHWHM = Determine_HWHM(dHWHM,dCenter,i_vX[uiI],i_lpgfpParamters);

    // try single gaussian fit
    // some rough initial guesses for the parameters
	vA.Set_Size(3);
	vA.Set(0,dAmplitude);
	vA.Set(1,dHWHM);
	vA.Set(2,dCenter);
	vA2 = Estimate_Second_Gaussian(vA,i_vX,vSmoothed_Data,i_lpgfpParamters);
//	printf("PGd: %f %f %f - %f %f %f\n",dAmplitude,dHWHM,dCenter,vA2[0],vA2[1],vA2[2]);

	vA.Set_Size(6);
	if (dCenter > vA2[2])
	{
		vA.Set(0,dAmplitude);
		vA.Set(1,dHWHM);
		vA.Set(2,dCenter);
		vA.Set(3,vA2[0]);
		vA.Set(4,vA2[1]);
		vA.Set(5,vA2[2]);
	}
	else
	{
		vA.Set(0,vA2[0]);
		vA.Set(1,vA2[1]);
		vA.Set(2,vA2[2]);
		vA.Set(3,dAmplitude);
		vA.Set(4,dHWHM);
		vA.Set(5,dCenter);
	}
	return vA;
}
XVECTOR Perform_Gaussian_Fit(const XVECTOR & i_vX, const XVECTOR & i_vY, const XVECTOR & i_vW, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters,
                    const double & i_dWavelength_Delta_Ang, double & o_dpEW_PVF, double & o_dpEW_HVF, double & o_dV_PVF, double & o_dV_HVF,
 					XVECTOR & o_vSigmas, double & o_dS, GAUSS_FIT_RESULTS * io_lpSingle_Fit, GAUSS_FIT_RESULTS * io_lpDouble_Fit)
{
    XVECTOR vRet;
    XVECTOR vA,vA_Double,vA_Single;
	XVECTOR vAest;
	
	XSQUARE_MATRIX mCovariance_Matrix;
	XSQUARE_MATRIX mCovariance_Matrix_Single;
    double dSmin_Single = 0.0, dSmin;

	vA_Double = Estimate_Gaussian_Fit(i_vX,i_vY,i_lpgfpParamters);
    // Perform LSQ fit
	vA.Set_Size(3);
    vA.Set(0,vA_Double[0]);
    vA.Set(1,vA_Double[1]);
    vA.Set(2,vA_Double[2]);
    // Perform LSQ fit
	dSmin = 0.0;
	double	dPerturb = 10.0;
	unsigned int uiCount = 0;
	while (dSmin == 0.0 && vA[2] >= i_vX[0] && vA[2] <= i_vX[i_vX.Get_Size() - 1])
	{
//		printf("PGs: %f %f %f\n",vA[0],vA[1],vA[2]);
		mCovariance_Matrix.Zero();
		if (GeneralFit(i_vX, i_vY ,i_vW, Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,1024,-20,NULL,&vAest))
		{
//			printf("PGs-r: %f %f %f\n",vA[0],vA[1],vA[2]);
		    vA_Single = vA;
		    dSmin_Single = dSmin;
		    mCovariance_Matrix_Single = mCovariance_Matrix;
			if (io_lpSingle_Fit)
			{
				io_lpSingle_Fit->m_vA = vA;
				io_lpSingle_Fit->m_dS = dSmin;
				io_lpSingle_Fit->m_mCovariance = mCovariance_Matrix;
			}
		}
		else
		{
//			printf("PGs-e: %f %f %f\n",vAest[0],vAest[1],vAest[2]);
			if (dPerturb < 0)
				dPerturb *= -2;
			else
				dPerturb *= -1;
		    vA.Set(2,dPerturb + vA_Double[2]);
		}
	}
	dPerturb = 10.0;
	vA = vA_Double;
	dSmin = 0.0;
	while (dSmin == 0.0 && vA[2] >= i_vX[0] && vA[2] <= i_vX[i_vX.Get_Size() - 1])
	{
		mCovariance_Matrix.Zero();
//		printf("PGd-s: %f %f %f - %f %f %f\n",vA[0],vA[1],vA[2],vA[3],vA[4],vA[5]);
		if (GeneralFit(i_vX, i_vY ,i_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,512,-30,NULL,&vAest))
		{
//			printf("PGd-r: %f %f %f - %f %f %f\n",vA[0],vA[1],vA[2],vA[3],vA[4],vA[5]);
			if (io_lpDouble_Fit)
			{
				io_lpDouble_Fit->m_vA = vA;
				io_lpDouble_Fit->m_dS = dSmin;
				io_lpDouble_Fit->m_mCovariance = mCovariance_Matrix;
			}
		    // if the single guassian fit is better, use those results
		    if (dSmin > dSmin_Single && dSmin_Single != 0.0)
		    {
		        vA = vA_Single;
		        dSmin = dSmin_Single;
		        mCovariance_Matrix = mCovariance_Matrix_Single;
		    }
			else if (dSmin_Single != 0.0)
			{
				// look for a fit that is more likely an artifact than a true double Gaussian set
				// identify the weaker component of the 2 component fit, assume amplitude is negative
				if ((vA[0] < 0 && vA[3] > 0) ||
					(vA[3] < 0 && vA[0] > 0)) // look for amplitudes with opposing signs
				{
					//emission component - use single Gaussian fit
				    vA = vA_Single;
				    dSmin = dSmin_Single;
				    mCovariance_Matrix = mCovariance_Matrix_Single;
				}
				else if (vA[0] > (vA[3] * 0.05) && fabs(vA[1]) > 2.0 * fabs(vA[4]))  // first is weaker and more than 2x as wide as the second
				{
				    vA = vA_Single;
				    dSmin = dSmin_Single;
				    mCovariance_Matrix = mCovariance_Matrix_Single;
				}
				else if (vA[3] > (vA[0] * 0.05) && fabs(vA[4]) > 2.0 * fabs(vA[1]))  // second is weaker and more than 2x as wide as the first
				{
				    vA = vA_Single;
				    dSmin = dSmin_Single;
				    mCovariance_Matrix = mCovariance_Matrix_Single;
				}
			}
		}
		else
		{
//			printf("PGd-e: %f %f %f - %f %f %f\n",vAest[0],vAest[1],vAest[2],vAest[3],vAest[4],vAest[5]);
			if (dPerturb < 0)
				dPerturb *= -2;
			else
				dPerturb *= -1;
		    vA.Set(2,dPerturb + vA_Double[2]);
			XVECTOR vAt;
			vAt.Set_Size(3);
			vAt.Set(0,vA[0]);
			vAt.Set(1,vA[1]);
			vAt.Set(2,vA[2]);

			XVECTOR vA2 = Estimate_Second_Gaussian(vAt, i_vX, i_vY, i_lpgfpParamters);
			vA.Set(3,vA2[0]);
			vA.Set(4,vA2[1]);
			vA.Set(5,vA2[2]);

		}
	}
	if (dSmin == 0.0)
	{
        // if the double guassian fit fails, use single gaussian results
        vA = vA_Single;
        dSmin = dSmin_Single;
        mCovariance_Matrix = mCovariance_Matrix_Single;
    }

    if (vA.Get_Size() == 6)
    {
        o_dV_HVF = Compute_Velocity(vA.Get(2),i_lpgfpParamters->m_dWl[1]);
        o_dV_PVF = Compute_Velocity(vA.Get(5),i_lpgfpParamters->m_dWl[1]);
    }
    else
    {
        o_dV_PVF = Compute_Velocity(vA.Get(2),i_lpgfpParamters->m_dWl[1]);
        o_dV_HVF = 0.0;
    }
	Compute_Gaussian_Fit_pEW(i_vX,vA,i_dWavelength_Delta_Ang,i_lpgfpParamters,o_dpEW_PVF,o_dpEW_HVF);

    vRet = vA;
    o_vSigmas.Set_Size(vA.Get_Size());
    for (unsigned int uiI = 0; uiI < vA.Get_Size(); uiI++)
        o_vSigmas.Set(uiI,sqrt(mCovariance_Matrix.Get(uiI,uiI)));
    o_dS = dSmin;
    return vRet;
}

