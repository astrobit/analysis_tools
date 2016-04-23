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

XVECTOR Estimate_Gaussian_Fit(const XVECTOR & i_vX, const XVECTOR & i_vY, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters)
{
    XVECTOR vA;

	double	dYmin = DBL_MAX;
	double	dXmin = 0;
	unsigned int uiXmin = 0;
	double	dYmax = -DBL_MAX;
	double	dXmax = 0;
	unsigned int uiXmax = 0;
	double	dSum = 0.0;
	//std::vector<unsigned int> vuiMinima;
	std::vector<double> vSmoothed_Data;

	int iSmooth_Size = 15;
	double dSmooth_Coeff = 1.0 / (2 * iSmooth_Size + 1);
	for (unsigned int uiI = 0; uiI < iSmooth_Size; uiI++)
	{
		vSmoothed_Data.push_back(i_vY[0]);
	}
	for (unsigned int uiI = iSmooth_Size; uiI < (i_vY.Get_Size() - iSmooth_Size); uiI++)
	{
		double dVal = 0.0;
		for (int iJ = -iSmooth_Size; iJ <= iSmooth_Size; iJ++)
			dVal += i_vY[uiI + iJ] * dSmooth_Coeff;
		vSmoothed_Data.push_back(dVal);
	}
	for (unsigned int uiI = 0; uiI < iSmooth_Size; uiI++)
	{
		vSmoothed_Data.push_back(i_vY[i_vY.Get_Size() - 1]);
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
	double	dDbl_Center;
	double	dDbl_Amplitude;
	double	dDbl_HWHM;
	unsigned int uiI;

	unsigned int uiXcenter = uiXmax;
	if (fabs(dYmax) < fabs(dYmin))
	{
		printf("fitting minimum %i\n",uiXcenter);
		dAmplitude *= -1;
		dCenter = dXmin;
		uiXcenter = uiXmin;
		uiI = uiXcenter;
		while (uiI < vSmoothed_Data.size() && vSmoothed_Data[uiI] < 0.5*dAmplitude)
			uiI++;
		if (uiI == vSmoothed_Data.size())
			uiI--;
		printf("%i\n",uiI);
		double dXright = i_vX.Get(uiI);
		uiI = uiXcenter;
		while (uiI < 0 && vSmoothed_Data[uiI] < 0.5*dAmplitude)
			uiI--;
		printf("%i\n",uiI);
		double dXleft = i_vX.Get(uiI);
		dHWHM = fabs(dXright - dXleft);
	}
	else
	{
		printf("fitting maximum %i\n",uiXcenter);
		uiI = uiXcenter;
		while (uiI < vSmoothed_Data.size() && vSmoothed_Data[uiI] > 0.5*dAmplitude)
			uiI++;
		if (uiI == vSmoothed_Data.size())
			uiI--;
		printf("%i\n",uiI);
		double dXright = i_vX.Get(uiI);
		uiI = uiXcenter;
		while (uiI > 0 && vSmoothed_Data[uiI] > 0.5*dAmplitude)
			uiI--;
		printf("%i\n",uiI);
		double dXleft = i_vX.Get(uiI);
		dHWHM = fabs(dXright - dXleft);
	}
	double dHWHM_Low = dHWHM * 0.50;
	double dHWHM_High = dHWHM;
	double dTest;
	unsigned int uiCount = 0;
    vA.Set_Size(3);
	vA.Set(0,1.0);
	vA.Set(2,dCenter);
	do
	{
		dHWHM = (0.5 * (dHWHM_High + dHWHM_Low));
		vA.Set(1,dHWHM); // HWHM
		dTest = Gaussian(i_vX[uiI],vA,(void*)i_lpgfpParamters)[0];
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

	printf("%f %f %f\n",dAmplitude,dHWHM,dCenter);
    // try single gaussian fit
    // some rough initial guesses for the parameters

	double dVariance_Min = DBL_MAX;
	unsigned int uiVar_Min_Idx;
    vA.Set(0,dAmplitude);
    vA.Set(1,dHWHM);
    vA.Set(2,dCenter);
	for (unsigned int uiI = 0; uiI < vSmoothed_Data.size(); uiI++)
	{
		double dR = vSmoothed_Data[uiI] - Gaussian(i_vX[uiI],vA,(void*)i_lpgfpParamters).Get(0);
		if (dR < dVariance_Min)
		{
			dVariance_Min = dR;
			uiVar_Min_Idx = uiI;
		}
	}
	dDbl_Center = i_vX[uiVar_Min_Idx];
	dDbl_Amplitude = dVariance_Min;
	while (uiVar_Min_Idx > 0 && (vSmoothed_Data[uiVar_Min_Idx] - Multi_Gaussian(i_vX[uiVar_Min_Idx],vA,(void*)i_lpgfpParamters).Get(0)) < (0.5 * dDbl_Amplitude))
		uiVar_Min_Idx--;
	vA.Set(0,1.0);
	vA.Set(2,dDbl_Center);
	dDbl_HWHM = fabs(i_vX[uiVar_Min_Idx] - dDbl_Center);
	dHWHM_Low = dDbl_HWHM * 0.50;
	dHWHM_High = dDbl_HWHM;
	uiCount = 0;
	do
	{
		dDbl_HWHM = (0.5 * (dHWHM_High + dHWHM_Low));
		vA.Set(1,dDbl_HWHM); // HWHM
		dTest = Gaussian(i_vX[uiVar_Min_Idx],vA,(void*)i_lpgfpParamters)[0];
		if (dTest > 0.5)
		{
			dHWHM_High = dDbl_HWHM;
		}
		else
		{
			dHWHM_Low = dDbl_HWHM;
		}
		uiCount++;
	} while (fabs(dTest - 0.5) > 0.001 && (dHWHM_High / dHWHM_Low > 1.01) && uiCount < 64);
	dDbl_HWHM = (0.5 * (dHWHM_High + dHWHM_Low));

	printf("PGd: %f %f %f - %f %f %f\n",dAmplitude,dHWHM,dCenter,dDbl_Amplitude,dDbl_HWHM,dDbl_Center);

	vA.Set_Size(6);
	if (dCenter > dDbl_Center)
	{
		vA.Set(0,dAmplitude);
		vA.Set(1,dHWHM);
		vA.Set(2,dCenter);
		vA.Set(3,dDbl_Amplitude);
		vA.Set(4,dDbl_HWHM);
		vA.Set(5,dDbl_Center);
	}
	else
	{
		vA.Set(0,dDbl_Amplitude);
		vA.Set(1,dDbl_HWHM);
		vA.Set(2,dDbl_Center);
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
	XSQUARE_MATRIX mCovariance_Matrix;
	XSQUARE_MATRIX mCovariance_Matrix_Single;
    double dSmin_Single, dSmin;

	vA_Double = Estimate_Gaussian_Fit(i_vX,i_vY,i_lpgfpParamters);
    // Perform LSQ fit
	vA.Set_Size(3);
    vA.Set(0,vA_Double[0]);
    vA.Set(1,vA_Double[1]);
    vA.Set(2,vA_Double[2]);
    // Perform LSQ fit
    if (GeneralFit(i_vX, i_vY ,i_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,256))
    {
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
	vA = vA_Double;
    mCovariance_Matrix.Zero();
    if (GeneralFit(i_vX, i_vY ,i_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,256))
    {
		printf("PGd-r: %f %f %f - %f %f %f\n",vA[0],vA[1],vA[2],vA[3],vA[4],vA[5]);
		if (io_lpDouble_Fit)
		{
			io_lpDouble_Fit->m_vA = vA;
			io_lpDouble_Fit->m_dS = dSmin;
			io_lpDouble_Fit->m_mCovariance = mCovariance_Matrix;
		}
        // if the single guassian fit is better, use those results
        if (dSmin > dSmin_Single)
        {
            vA = vA_Single;
            dSmin = dSmin_Single;
            mCovariance_Matrix = mCovariance_Matrix_Single;
        }
    }
    else
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

