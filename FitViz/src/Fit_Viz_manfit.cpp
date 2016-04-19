#include <FitViz.hpp>
#include <line_routines.h>
#include <unistd.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include <xfit.h>


bool	g_bFit_Thread_Running = false;
bool	g_bQuit_Thread = false;
bool	g_bPerform_Fit = false;
bool	g_bFit_Results_Ready = false;
XVECTOR	g_vFit_Results;
XVECTOR g_vX;
XVECTOR	g_vY;
XVECTOR g_vW;
GAUSS_FIT_PARAMETERS * g_lpgfpParamters = &g_cgfpCaNIR;
double g_dSuggested_Center_WL;
double g_dSuggested_Center_Flux;
double g_dSuggested_Center_Second_WL = -1.0;
double g_dSuggested_Center_Second_Flux = -1.0;
double g_dFit_Results_Smin;
XSQUARE_MATRIX	g_mCovariance_Matrix;

void Perform_Fit(void)
{
	double dpEW_PVF, dpEW_HVF, dV_PVF, dV_HVF, dSmin;
	XVECTOR vSigma;
	g_bFit_Thread_Running = true;
	while (!g_bQuit_Thread)
	{
		if (g_bPerform_Fit)
		{
			g_vFit_Results.Set_Size(0);
			g_dFit_Results_Smin = 0.0;
			g_mCovariance_Matrix.Set_Size(0);
			if (g_dSuggested_Center_Second_WL == -1.0)
			{
				XSQUARE_MATRIX mCovariance_Matrix;
				XVECTOR	vA;
				double dSmin;
				vA.Set_Size(3);
				vA.Set(2,g_dSuggested_Center_WL);
				// to find estimated HWHM, go up the red side until we reach half amplitude
				unsigned int uiI = 0;
				while (uiI < g_vX.Get_Size() && g_vX.Get(uiI) < g_dSuggested_Center_WL)
					uiI++;
				if (uiI < g_vX.Get_Size())
					vA.Set(0,g_vY[uiI]); /// make sure that we have the right amplitude
				while (uiI < g_vX.Get_Size() && g_vY[uiI] < (vA[0] * 0.5))
					uiI++;
				vA.Set(1,g_vX[uiI] - vA[2]); // HWHM
				// Perform LSQ fit
				printf("Start: %f %f %f\n",vA[0],vA[1],vA[2]);
				if (GeneralFit(g_vX, g_vY ,g_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)g_lpgfpParamters,256))
				{
					g_vFit_Results = vA;
					g_dFit_Results_Smin = dSmin;
					g_mCovariance_Matrix = mCovariance_Matrix;
				}
				printf("End: %f %f %f\n",vA[0],vA[1],vA[2]);
				printf("Quality: %e\n",dSmin);

			}
			else
			{
				XSQUARE_MATRIX mCovariance_Matrix;
				XVECTOR	vA;
				double dSmin;
				vA.Set_Size(6);
				// first determine which one is the reddest center
				if (g_dSuggested_Center_WL > g_dSuggested_Center_Second_WL)
				{
					vA.Set(2,g_dSuggested_Center_WL);
					vA.Set(5,g_dSuggested_Center_Second_WL);
				}
				else
				{
					vA.Set(2,g_dSuggested_Center_Second_WL);
					vA.Set(5,g_dSuggested_Center_WL);
				}
				unsigned int uiI = 0;
				while (uiI < g_vX.Get_Size() && g_vX.Get(uiI) < vA[2])
					uiI++;
				if (uiI < g_vX.Get_Size())
					vA.Set(0,g_vY[uiI]); /// make sure that we have the right amplitude
				while (uiI < g_vX.Get_Size() && g_vY[uiI] < (vA[0] * 0.5))
					uiI++;
				vA.Set(1,g_vX[uiI] - vA[2]); // HWHM

				uiI = 0;
				while (uiI < g_vX.Get_Size() && g_vX.Get(uiI) < vA[5])
					uiI++;
				XVECTOR vATemp;
				vATemp.Set_Size(3);
				vATemp.Set(0,vA[0]);
				vATemp.Set(1,vA[1]);
				vATemp.Set(2,vA[2]);
				if (uiI < g_vX.Get_Size())
				{
					double dY = Multi_Gaussian(g_vX[uiI],vATemp,(void*)g_lpgfpParamters).Get(0);
					vA.Set(3,g_vY[uiI] - dY); /// make sure that we have the right amplitude
				}
				uiI--;
				double dR = g_vY[uiI] - Multi_Gaussian(g_vX[uiI],vATemp,(void*)g_lpgfpParamters).Get(0);
				while (uiI > 0 && dR < (vA[3] * 0.5))
				{
					dR = g_vY[uiI] - Multi_Gaussian(g_vX[uiI],vATemp,(void*)g_lpgfpParamters).Get(0);
					uiI--;
				}
				vA.Set(4,g_vX[uiI] - vA[5]); // HWHM

				printf("Start: %f %f %f - %f %f %f\n",vA[0],vA[1],vA[2],vA[3],vA[4],vA[5]);
				if (GeneralFit(g_vX, g_vY ,g_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)g_lpgfpParamters,256))
				{
					g_vFit_Results = vA;
					g_dFit_Results_Smin = dSmin;
					g_mCovariance_Matrix = mCovariance_Matrix;
				}
				printf("End: %f %f %f - %f %f %f\n",vA[0],vA[1],vA[2],vA[3],vA[4],vA[5]);
				printf("Quality: %e\n",dSmin);
			}
			g_bPerform_Fit = false;
			g_bFit_Results_Ready = true;
		}
		else
			usleep(100);		
	}
	g_bFit_Thread_Running = false;
}
