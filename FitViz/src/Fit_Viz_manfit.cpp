#include <FitViz.hpp>
#include <line_routines.h>
#include <unistd.h>

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

void Perform_Fit(void)
{
	double dpEW_PVF, dpEW_HVF, dV_PVF, dV_HVF, dSmin;
	XVECTOR vSigma;
	g_bFit_Thread_Running = true;
	while (!g_bQuit_Thread)
	{
		if (g_bPerform_Fit)
		{
	        g_vFit_Results = Perform_Gaussian_Fit(g_dSuggested_Center_Flux, g_dSuggested_Center_WL, g_vX, g_vY, g_vW, g_lpgfpParamters,
	                        g_vX.Get(1) - g_vX.Get(0), dpEW_PVF, dpEW_HVF, dV_PVF, dV_HVF, vSigma,dSmin);
			g_bPerform_Fit = false;
			g_bFit_Results_Ready = true;
		}
		else
			usleep(100);		
	}
	g_bFit_Thread_Running = false;
}
