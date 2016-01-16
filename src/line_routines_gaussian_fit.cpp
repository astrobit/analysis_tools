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
	vOut.Set(0,i_vA.Get(0) * (dExp_1 + dExp_2 + dExp_3));
	vOut.Set(1,dExp_1 + dExp_2 + dExp_3);
	vOut.Set(2,i_vA.Get(0) * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3 * dDel_X_Sigma_3));
	vOut.Set(3,i_vA.Get(0) * dInv_Sigma * (dExp_1 * dDel_X_Sigma_1 + dExp_2 * dDel_X_Sigma_2 + dExp_3 * dDel_X_Sigma_3));
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

XVECTOR Perform_Gaussian_Fit(const double & i_dMin_Flux_Flat, const double & i_dCentral_WL, const XVECTOR & i_vX, const XVECTOR & i_vY, const XVECTOR & i_vW, const GAUSS_FIT_PARAMETERS * i_lpgfpParamters,
                    const double & i_dWavelength_Delta_Ang, double & o_dpEW_PVF, double & o_dpEW_HVF, double & o_dV_PVF, double & o_dV_HVF)
{
    XVECTOR vRet;
    XVECTOR vA,vA_Single;
	XSQUARE_MATRIX mCovariance_Matrix;
	XSQUARE_MATRIX mCovariance_Matrix_Single;
    double dSmin_Single, dSmin;
    
    // try single gaussian fit
    // some rough initial guesses for the parameters
    vA.Set_Size(3);
    vA.Set(0,-i_dMin_Flux_Flat/3.0);
    vA.Set(1,200.0);
    vA.Set(2,i_dCentral_WL);//lpdSpectra_WL[uiI][uiMin_Flux_Idx]);

    // Perform LSQ fit
    if (GeneralFit(i_vX, i_vY ,i_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,100))
    {
        vA_Single = vA;
        dSmin_Single = dSmin;
        mCovariance_Matrix_Single = mCovariance_Matrix;
    }
    // find the minimum of the flux error
    double dMin_Flux_WL = 0.0;
    double dMin_Flux_Err = DBL_MAX;
    if (vA_Single.Get_Size() == 3)
    {
        for (unsigned int uiJ = 0; uiJ < i_vX.Get_Size(); uiJ++)
        {
            double dFlux_Error = i_vY.Get(uiJ) - Multi_Gaussian(i_vX.Get(uiJ), vA_Single, (void *)i_lpgfpParamters).Get(0);
            if (dFlux_Error < dMin_Flux_Err)
            {
                dMin_Flux_WL = i_vX.Get(uiJ);
                dMin_Flux_Err = dFlux_Error;
            }
        }
        // try double gaussian fit
        vA.Set_Size(6);
        vA.Set(0,-dMin_Flux_Err);
        vA.Set(1,vA_Single.Get(1));
        vA.Set(2,dMin_Flux_WL);
        vA.Set(3,vA_Single.Get(0));
        vA.Set(4,vA_Single.Get(1));
        vA.Set(5,vA_Single.Get(2));
    }
    else
    {
        vA.Set_Size(6);
        vA.Set(0,-i_dMin_Flux_Flat / 6.0);
        vA.Set(1,200.0);
        vA.Set(2,i_dCentral_WL - 200.0);
        vA.Set(3,vA.Get(0));
        vA.Set(4,200.0);
        vA.Set(5,i_dCentral_WL);
    }
    // Perform LSQ fit
    //if (bVerbose)
    //    printf("Performing double unflat fit\n");
    mCovariance_Matrix.Zero();
    if (GeneralFit(i_vX, i_vY ,i_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)i_lpgfpParamters,100))
    {
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
    o_dpEW_HVF = 0.0;
    o_dpEW_PVF = 0.0;
    for (unsigned int uiJ = 0; uiJ < i_vX.Get_Size(); uiJ++)
    {
        XVECTOR vF = Multi_Gaussian(i_vX.Get(uiJ), vA, (void *)i_lpgfpParamters);
        if (vA.Get_Size() == 6)
        {
            XVECTOR vAlcl;
            vAlcl.Set_Size(3);
            vAlcl.Set(0,vA.Get(0));
            vAlcl.Set(1,vA.Get(1));
            vAlcl.Set(2,vA.Get(2));

            Gaussian(i_vX.Get(uiJ), vAlcl, (void *)i_lpgfpParamters);
            o_dpEW_HVF -= vF.Get(0) * i_dWavelength_Delta_Ang; // - sign to keep pEW positive

            vAlcl.Set(0,vA.Get(3));
            vAlcl.Set(1,vA.Get(4));
            vAlcl.Set(2,vA.Get(5));

            Gaussian(i_vX.Get(uiJ), vAlcl, (void *)i_lpgfpParamters);
            o_dpEW_PVF -= vF.Get(0) * i_dWavelength_Delta_Ang;
        }
        else
        {
            Gaussian(i_vX.Get(uiJ), vA, (void *)i_lpgfpParamters);
            o_dpEW_PVF -= vF.Get(0) * i_dWavelength_Delta_Ang; // - sign to keep pEW positive
        }
    }
    vRet = vA;
    return vRet;
}