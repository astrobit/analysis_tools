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
gauss_fit_parameters * g_lpgfpParamters = &g_cgfpCaNIR;
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

			double dSmin= 0.0;
			unsigned int uiFit_Attempt = 0;
			double	dCenter_WL_Perturb = -25.0;
			double dCenter_WL_Start = g_dSuggested_Center_WL;
			bool bForce_Quit = false;
			do
			{
				g_vFit_Results.Set_Size(0);
				g_dFit_Results_Smin = 0.0;
				g_mCovariance_Matrix.Set_Size(0);
				XSQUARE_MATRIX mCovariance_Matrix;
				XVECTOR	vA;
				vA.Set_Size(3);
				if (uiFit_Attempt > 0)
				{
					g_dSuggested_Center_WL += dCenter_WL_Perturb;
					if (g_dSuggested_Center_WL <= g_vX[0])
					{
						dCenter_WL_Perturb *= -1.0;
						g_dSuggested_Center_WL = dCenter_WL_Start+ dCenter_WL_Perturb;
					}
					if (g_dSuggested_Center_WL >= g_vX[g_vX.Get_Size() - 1])
						bForce_Quit = true;
				}
				if (!bForce_Quit)
				{
					vA.Set(2,g_dSuggested_Center_WL);
					// to find estimated HWHM, go up the red side until we reach half amplitude
					unsigned int uiI = 0;
					while (uiI < g_vX.Get_Size() && g_vX.Get(uiI) < g_dSuggested_Center_WL)
						uiI++;
					double dAmplitude;
					if (uiI < g_vX.Get_Size())
						dAmplitude = g_vY[uiI];
					vA.Set(0,dAmplitude); /// make sure that we have the right amplitude
					while (uiI < g_vX.Get_Size() && g_vY[uiI] < (vA[0] * 0.5))
						uiI++;
					double dHWHM_Est = g_vX[uiI] - vA[2];
					double dHWHM_Low = dHWHM_Est * 0.50;
					double dHWHM_High = dHWHM_Est;
					double dTest;
					unsigned int uiCount = 0;
					do
					{
						dHWHM_Est = (0.5 * (dHWHM_High + dHWHM_Low));
						vA.Set(0,1.0);
						vA.Set(1,dHWHM_Est); // HWHM
						dTest = Gaussian(g_vX[uiI],vA,g_lpgfpParamters)[0];
						if (dTest > 0.5)
						{
							dHWHM_High = dHWHM_Est;
						}
						else
						{
							dHWHM_Low = dHWHM_Est;
						}
						uiCount++;
					} while (fabs(dTest - 0.5) > 0.001 && (dHWHM_High / dHWHM_Low > 1.01) && uiCount < 64);
					dHWHM_Est = (0.5 * (dHWHM_High + dHWHM_Low));

					vA.Set(0,dAmplitude); /// make sure that we have the right amplitude
					vA.Set(1,dHWHM_Est); // HWHM
					if (g_dSuggested_Center_Second_WL != -1)
					{
						std::vector<double> vResiduals;
						double	dResid_Min = DBL_MAX;
						double dX_Resi_Min;
						for (unsigned int uiI = 0; uiI < g_vX.Get_Size(); uiI++)
						{
							double dY = Multi_Gaussian(g_vX[uiI],vA,g_lpgfpParamters).Get(0);
							double dR = g_vY[uiI] - dY;
							if (dResid_Min > dR)
							{
								dResid_Min = dR;
								dX_Resi_Min = g_vX[uiI];
							}
							vResiduals.push_back(g_vY[uiI] - dY);
						}
						if (uiFit_Attempt > 0)
						{
							g_dSuggested_Center_Second_WL = dX_Resi_Min;
						}

						vA.Set(2,g_dSuggested_Center_Second_WL);
						// to find estimated HWHM, go up the red side until we reach half amplitude
						uiI = 0;
						while (uiI < g_vX.Get_Size() && g_vX[uiI] < vA[2])
							uiI++;
						double dAmplitude_2 = 0.0;
						if (uiI < g_vX.Get_Size())
						{
							dAmplitude_2 = vResiduals[uiI];
						}
						while (uiI < g_vX.Get_Size() && vResiduals[uiI] < (dAmplitude_2 * 0.5))
							uiI++;
						double dHWHM_2_Est = g_vX[uiI] - vA[2];
						dHWHM_Low = dHWHM_2_Est * 0.50;
						dHWHM_High = dHWHM_2_Est;
						double dTest;
						unsigned int uiCount = 0;
						do
						{
							dHWHM_2_Est = (0.5 * (dHWHM_High + dHWHM_Low));
							vA.Set(0,1.0);
							vA.Set(1,dHWHM_2_Est); // HWHM
							dTest = Gaussian(g_vX[uiI],vA,g_lpgfpParamters)[0];
							if (dTest > 0.5)
							{
								dHWHM_High = dHWHM_2_Est;
							}
							else
							{
								dHWHM_Low = dHWHM_2_Est;
							}
							uiCount++;
						} while (fabs(dTest - 0.5) > 0.001 && (dHWHM_High / dHWHM_Low > 1.01) && uiCount < 64);
						dHWHM_2_Est = (0.5 * (dHWHM_High + dHWHM_Low));

						vA.Set_Size(6);
						vA.Set(0,dAmplitude);
						vA.Set(1,dHWHM_Est);
						vA.Set(2,g_dSuggested_Center_WL);
						vA.Set(3,dAmplitude_2);
						vA.Set(4,dHWHM_2_Est);
						vA.Set(5,g_dSuggested_Center_Second_WL);

					}
					printf("Start: %f %f %f",vA[0],vA[1],vA[2]);
					if (vA.Get_Size() == 6)
						printf(" - %f %f %f",vA[3],vA[4],vA[5]);
					printf("\n");
					if (GeneralFit(g_vX, g_vY ,g_vW, Multi_Gaussian, vA, mCovariance_Matrix, dSmin, (void *)g_lpgfpParamters,1024))
					{
						g_vFit_Results = vA;
						g_dFit_Results_Smin = dSmin;
						g_mCovariance_Matrix = mCovariance_Matrix;
					}
					printf("End: %f %f %f",vA[0],vA[1],vA[2]);
					if (vA.Get_Size() == 6)
						printf(" - %f %f %f",vA[3],vA[4],vA[5]);
					printf("\n");
					printf("Quality: %e\n",dSmin);
				}
				uiFit_Attempt++;
			} while (!bForce_Quit && dSmin == 0.0);
			g_bPerform_Fit = false;
			g_bFit_Results_Ready = true;
		}
		else
			usleep(100);		
	}
	g_bFit_Thread_Running = false;
}
