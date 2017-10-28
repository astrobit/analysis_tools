
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <eps_plot.h>
#include <Plot_Utilities.h>
#include <float.h>
#include <line_routines.h>

extern void Allocate_Synow_Classes(const ES::Spectrum &i_cTarget, unsigned int uiNum_Elements, const XDATASET * i_lpOpacity_Map);
extern unsigned int g_uiVelocity_Map_Alloc_Size;
extern double		* g_lpdVelocities;
extern std::vector<std::vector<double> > g_lpdOpacity_Profile;
extern ES::Synow::Grid * g_lpcGrid;
extern ES::Synow::Grid * g_lpcGrid_Exp;
extern ES::Synow::Opacity * g_lpcOpacity;
extern ES::Synow::Opacity * g_lpcOpacity_Exp;
extern ES::Spectrum * g_lpcReference;
extern ES::Synow::Source * g_lpcSource;
extern ES::Synow::Source * g_lpcSource_Exp;
extern ES::Synow::Spectrum * g_lpcSpectrum;
extern ES::Synow::Spectrum * g_lpcSpectrum_Exp;
extern ES::Spectrum * g_lpcOutput;
extern bool		g_bMin_Delta_Init;
extern double		g_dMin_Delta;


void Generate_Synow_Spectra(ES::Spectrum &o_cOutput)
{
	g_lpdOpacity_Profile.resize(2);
	g_lpdOpacity_Profile[0].resize(2048);
	g_lpdOpacity_Profile[1].resize(2048);
	Allocate_Synow_Classes(o_cOutput,2048,NULL);
	for (unsigned int uiI = 0; uiI < 2048; uiI++)
	{
//		g_lpdVelocities[uiI] = 0.0625 * (uiI + 1); // between 0 and 128
		g_lpdOpacity_Profile[0][uiI] = 0.0915 * exp(-pow((g_lpdVelocities[uiI] - 20.0) / 1.0,2.0));
//		g_lpdOpacity_Profile[0][uiI] = 3.0 * exp(-(g_lpdVelocities[uiI] - 10.0) / 1.0);
		g_lpdOpacity_Profile[1][uiI] = 0.0;
	}
    ES::Synow::Setup cSetup;
	// clear out the old results
	g_lpcOutput[0].zero_flux();

	// initialize setup
    cSetup.a0 = 1.0;
    cSetup.a1 = 0.0;
    cSetup.a2 = 0.0;
    cSetup.v_phot = 13.49;
    cSetup.v_outer = 128.0;
    cSetup.t_phot = 13.41;
	cSetup.ions.push_back(2001);
	cSetup.active.push_back(true);
	cSetup.log_tau.push_back(3.0);
	cSetup.temp.push_back(14.2);
	cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
	cSetup.user_profile = g_lpdOpacity_Profile;

	// reset the grid (makes sure all optical depths set to 0)
	g_lpcGrid->reset(cSetup);
	// generate the spectrum
    g_lpcGrid[0]( cSetup );
	// copy result to output
	o_cOutput = g_lpcOutput[0];

}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
    ES::Spectrum cTarget;
	char	lpszOutput_Filename[128];
	cTarget = ES::Spectrum::create_from_range_and_step(2500.0, 10000.0, 5.0);

	Generate_Synow_Spectra(cTarget);

	sprintf(lpszOutput_Filename,"gaussian.full.eps");
	PlotSingle(cTarget, lpszOutput_Filename, cTarget.wl(0), cTarget.wl(cTarget.size() - 1),NULL,false);
	sprintf(lpszOutput_Filename,"gaussian.CaNIR.eps");
	PlotSingle(cTarget, lpszOutput_Filename, 7000.0, 8500.0, NULL,false);
	sprintf(lpszOutput_Filename,"gaussian.CaHK.eps");
	PlotSingle(cTarget, lpszOutput_Filename, 3000.0, 4500.0, NULL,false);
	sprintf(lpszOutput_Filename,"gaussian.Si6355.eps");
	PlotSingle(cTarget, lpszOutput_Filename, 5000.0, 6500.0, NULL,false);
		

	return 0;
}

