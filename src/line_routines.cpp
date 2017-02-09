#include <math.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <xio.h>
#include <xmath.h>
#include <xlinalg.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <float.h>
#include <line_routines.h>
#include <stack>

void Find_Minimum(ES::Spectrum * i_lpcSpectra, unsigned int i_uiNum_Spectra, double * o_lpdMinima_WL, double * o_lpdMinima_Flux, const double & i_dMin_WL, const double & i_dMax_WL)
{
	if (o_lpdMinima_WL || o_lpdMinima_Flux)
	{
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
		{
			double	dMinimum_Lcl = DBL_MAX;
			if (o_lpdMinima_WL)
				o_lpdMinima_WL[uiJ] = 0.0;
			for (unsigned int uiK = 0; uiK < i_lpcSpectra[uiJ].size(); uiK++)
			{
				if ((i_dMin_WL < 0.0 || i_dMin_WL > i_dMax_WL || i_lpcSpectra[uiJ].wl(uiK) >= i_dMin_WL) && 
					(i_dMax_WL < 0.0 || i_dMax_WL < i_dMin_WL || i_lpcSpectra[uiJ].wl(uiK) <= i_dMax_WL))
				{
					if (i_lpcSpectra[uiJ].flux(uiK) < dMinimum_Lcl)
					{
						dMinimum_Lcl = i_lpcSpectra[uiJ].flux(uiK);
						if (o_lpdMinima_WL)
							o_lpdMinima_WL[uiJ] = i_lpcSpectra[uiJ].wl(uiK);
					}
				}
			}
			if (o_lpdMinima_Flux)
				o_lpdMinima_Flux[uiJ] = dMinimum_Lcl;
		}
	}
}

double	Equivalent_Width(const XDATASET & i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length)
{
	double	dEW = -1.0;
	double	dContinuum_Flux[2] = {0,0};
	unsigned int uiContinuum_Flux_Index[2] = {0,0};
	for (unsigned int uiI = 0; uiI < i_cData.GetNumElements() && (uiContinuum_Flux_Index[0] == 0 || uiContinuum_Flux_Index[1] == 0); uiI++)
	{
		if (i_cData.GetElement(0,uiI) >= io_dContinuum_WL_Blue && uiContinuum_Flux_Index[0] == 0)
			uiContinuum_Flux_Index[0] = uiI;
		if (i_cData.GetElement(0,uiI) >= io_dContinuum_WL_Red && uiContinuum_Flux_Index[1] == 0)
			uiContinuum_Flux_Index[1] = uiI;
	}
	i_uiAveraging_Length <<= 1; // divide by 2
	if (i_uiAveraging_Length <= 0)
		i_uiAveraging_Length = 1;
	if (uiContinuum_Flux_Index[0] > 0 && uiContinuum_Flux_Index[1] > 0) // potential problem here if first index is about equal to continuum....
	{
		if (uiContinuum_Flux_Index[0] < i_uiAveraging_Length)
			uiContinuum_Flux_Index[0] = i_uiAveraging_Length;
		if (uiContinuum_Flux_Index[1] > (i_cData.GetNumElements() - i_uiAveraging_Length - 1))
			uiContinuum_Flux_Index[1] = (i_cData.GetNumElements() - i_uiAveraging_Length - 1);
		io_dContinuum_WL_Blue = i_cData.GetElement(0,uiContinuum_Flux_Index[0]);
		io_dContinuum_WL_Red = i_cData.GetElement(0,uiContinuum_Flux_Index[1]);

		uiContinuum_Flux_Index[0] -= i_uiAveraging_Length;
		uiContinuum_Flux_Index[1] -= i_uiAveraging_Length;

		for (unsigned int uiI = 0; uiI < 2 * i_uiAveraging_Length; uiI++)
		{
			dContinuum_Flux[0] += i_cData.GetElement(1,uiI + uiContinuum_Flux_Index[0]);
			dContinuum_Flux[1] += i_cData.GetElement(1,uiI + uiContinuum_Flux_Index[1]);
		}
		dContinuum_Flux[0] /= (2.0 * i_uiAveraging_Length);
		dContinuum_Flux[1] /= (2.0 * i_uiAveraging_Length);

		dEW = 0.0;
		for (unsigned int uiI = uiContinuum_Flux_Index[0]; uiI <= uiContinuum_Flux_Index[1]; uiI++)
		{
			double dCont = ((dContinuum_Flux[1] - dContinuum_Flux[0]) * (i_cData.GetElement(0,uiI) - io_dContinuum_WL_Blue) / (io_dContinuum_WL_Red - io_dContinuum_WL_Blue) + dContinuum_Flux[0]);
			double	dDelta_Wl;
			if (uiI > 0)
				dDelta_Wl = i_cData.GetElement(0,uiI) - i_cData.GetElement(0,uiI - 1);
			else
				dDelta_Wl = i_cData.GetElement(0,uiI + 1) - i_cData.GetElement(0,uiI);

			dEW += (1.0 - i_cData.GetElement(1,uiI) / dCont) * dDelta_Wl;
		}
	}
	return dEW;
}


double	Equivalent_Width(const ES::Spectrum &i_cData, double & io_dContinuum_WL_Blue, double & io_dContinuum_WL_Red, unsigned int i_uiAveraging_Length)
{
	double	dEW = -1.0;
	double	dContinuum_Flux[2] = {0.0,0.0};
	unsigned int uiContinuum_Flux_Index[2] = {0,0};
	for (unsigned int uiI = 0; uiI < i_cData.size() && (uiContinuum_Flux_Index[0] == 0 || uiContinuum_Flux_Index[1] == 0); uiI++)
	{
		if (i_cData.wl(uiI) >= io_dContinuum_WL_Blue && uiContinuum_Flux_Index[0] == 0)
			uiContinuum_Flux_Index[0] = uiI;
		if (i_cData.wl(uiI) >= io_dContinuum_WL_Red && uiContinuum_Flux_Index[1] == 0)
			uiContinuum_Flux_Index[1] = uiI;
	}
	i_uiAveraging_Length <<= 1; // divide by 2
	if (i_uiAveraging_Length <= 0)
		i_uiAveraging_Length = 1;
	if (uiContinuum_Flux_Index[0] > 0 && uiContinuum_Flux_Index[1] > 0) // potential problem here if first index is about equal to continuum....
	{
		if (uiContinuum_Flux_Index[0] < i_uiAveraging_Length)
			uiContinuum_Flux_Index[0] = i_uiAveraging_Length;
		if (uiContinuum_Flux_Index[1] > (i_cData.size() - i_uiAveraging_Length - 1))
			uiContinuum_Flux_Index[1] = (i_cData.size() - i_uiAveraging_Length - 1);
		io_dContinuum_WL_Blue = i_cData.wl(uiContinuum_Flux_Index[0]);
		io_dContinuum_WL_Red = i_cData.wl(uiContinuum_Flux_Index[1]);

		uiContinuum_Flux_Index[0] -= i_uiAveraging_Length;
		uiContinuum_Flux_Index[1] -= i_uiAveraging_Length;

		for (unsigned int uiI = 0; uiI < 2 * i_uiAveraging_Length; uiI++)
		{
			dContinuum_Flux[0] += i_cData.flux(uiI + uiContinuum_Flux_Index[0]);
			dContinuum_Flux[1] += i_cData.flux(uiI + uiContinuum_Flux_Index[1]);
//			printf("Continuum at %i,%i: %.2e %.2e\n",uiI + uiContinuum_Flux_Index[0],uiI + uiContinuum_Flux_Index[1],i_cData.flux(uiI + uiContinuum_Flux_Index[0]),i_cData.flux(uiI + uiContinuum_Flux_Index[1]));
		}
		dContinuum_Flux[0] /= (2.0 * i_uiAveraging_Length);
		dContinuum_Flux[1] /= (2.0 * i_uiAveraging_Length);
//		printf("Continuum at min/max: %.2e %.2e\n",dContinuum_Flux[0],dContinuum_Flux[1]);
		dEW = 0.0;
		for (unsigned int uiI = uiContinuum_Flux_Index[0]; uiI <= uiContinuum_Flux_Index[1]; uiI++)
		{
			double dCont = ((dContinuum_Flux[1] - dContinuum_Flux[0]) * (i_cData.wl(uiI) - io_dContinuum_WL_Blue) / (io_dContinuum_WL_Red - io_dContinuum_WL_Blue) + dContinuum_Flux[0]);
			double	dDelta_Wl;
			if (uiI > 0)
				dDelta_Wl = i_cData.wl(uiI) - i_cData.wl(uiI - 1);
			else
				dDelta_Wl = i_cData.wl(uiI + 1) - i_cData.wl(uiI);

			dEW += (1.0 - i_cData.flux(uiI) / dCont) * dDelta_Wl;
		}
	}
	return dEW;
}


unsigned int g_uiVelocity_Map_Alloc_Size = 0;
double		* g_lpdVelocities = nullptr;
std::vector<std::vector<double> > g_lpdOpacity_Profile;
unsigned int	g_uiOP_Multi_Num_Ions = 0;
ES::Synow::Grid * g_lpcGrid = nullptr;
ES::Synow::Grid * g_lpcGrid_Exp = nullptr;
ES::Synow::Opacity * g_lpcOpacity = nullptr;
ES::Synow::Opacity * g_lpcOpacity_Exp = nullptr;
ES::Spectrum * g_lpcReference = nullptr;
ES::Synow::Source * g_lpcSource = nullptr;
ES::Synow::Source * g_lpcSource_Exp = nullptr;
ES::Synow::Spectrum * g_lpcSpectrum = nullptr;
ES::Synow::Spectrum * g_lpcSpectrum_Exp = nullptr;
ES::Spectrum * g_lpcOutput = nullptr;
bool		g_bMin_Delta_Init = false;
double		g_dMin_Delta = DBL_MAX;

void Allocate_Opacity_Profile(unsigned int i_uiNum_Ions, bool i_bForce = false)
{
	if (i_uiNum_Ions > g_uiOP_Multi_Num_Ions || i_bForce)
	{
//		printf("OP delete \n");
		g_lpdOpacity_Profile.clear();
//		printf("OP allocate \n");
		g_lpdOpacity_Profile.resize(i_uiNum_Ions);
		if (g_uiVelocity_Map_Alloc_Size > 0)
		{
			for (unsigned int uiI = 0; uiI < i_uiNum_Ions; uiI++)
				g_lpdOpacity_Profile[uiI].resize(g_uiVelocity_Map_Alloc_Size,0);
		}
		g_uiOP_Multi_Num_Ions = i_uiNum_Ions;
//		printf("OP done \n");

	}
//	printf("Alloc OP: %i ions\n",g_uiOP_Multi_Num_Ions);
}

void Allocate_Synow_Classes(const ES::Spectrum &i_cTarget, unsigned int uiNum_Elements, const XDATASET * i_lpOpacity_Map)
{
	if (i_lpOpacity_Map)
		uiNum_Elements = i_lpOpacity_Map->GetNumElements() - 1;

	for (unsigned int uiI = 1; uiI < i_cTarget.size() && !g_bMin_Delta_Init; uiI++)
	{
		if ((i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1)) < g_dMin_Delta)
			g_dMin_Delta = i_cTarget.wl(uiI) - i_cTarget.wl(uiI - 1);
	}
	bool bWL_Change = (g_lpcOutput != nullptr && (i_cTarget.wl(0) != g_lpcOutput->wl(0) || g_lpcOutput->wl(g_lpcOutput->size() - 1) != i_cTarget.wl(i_cTarget.size() - 1)));


	if (uiNum_Elements > g_uiVelocity_Map_Alloc_Size || bWL_Change)
	{
		//ES::ERROR::Inhibit_Error_Reporting();

//		printf("Realloc Vel\n"); fflush(stdout);
		if (g_lpdVelocities != nullptr)
			delete [] g_lpdVelocities;
		g_lpdVelocities = nullptr;
//		printf("Realloc Grid\n"); fflush(stdout);
		if (g_lpcGrid != nullptr)
			delete g_lpcGrid;
		g_lpcGrid = nullptr;
//		printf("Realloc Opacity\n"); fflush(stdout);
		if (g_lpcOpacity != nullptr)
			delete g_lpcOpacity;
		g_lpcOpacity = nullptr;
//		printf("Realloc Reference\n"); fflush(stdout);
		if (g_lpcReference != nullptr)
			delete g_lpcReference;
		g_lpcReference = nullptr;
//		printf("Realloc Source\n"); fflush(stdout);
		if (g_lpcSource != nullptr)
			delete g_lpcSource;
		g_lpcSource = nullptr;
//		printf("Realloc Spectrum\n"); fflush(stdout);
		if (g_lpcSpectrum != nullptr)
			delete g_lpcSpectrum;
		g_lpcSpectrum = nullptr;
//		printf("Realloc Ouput\n"); fflush(stdout);
		if (g_lpcOutput != nullptr)
			delete g_lpcOutput;
		g_lpcOutput = nullptr;
//		printf("Realloc Grid xp\n"); fflush(stdout);
		if (g_lpcGrid_Exp != nullptr)
			delete g_lpcGrid_Exp;
		g_lpcGrid_Exp = nullptr;
//		printf("Realloc Opacity xp\n"); fflush(stdout);
		if (g_lpcOpacity_Exp != nullptr)
			delete g_lpcOpacity_Exp;
		g_lpcOpacity_Exp = nullptr;
//		printf("Realloc Source xp\n"); fflush(stdout);
		if (g_lpcSource_Exp != nullptr)
			delete g_lpcSource_Exp;
		g_lpcSource_Exp = nullptr;
//		printf("Realloc Spectrum xp\n"); fflush(stdout);
		if (g_lpcSpectrum_Exp != nullptr)
			delete g_lpcSpectrum_Exp;
		g_lpcSpectrum_Exp = nullptr;

//		printf("Alloc Velocities\n"); fflush(stdout);
		g_uiVelocity_Map_Alloc_Size = uiNum_Elements;
		g_lpdVelocities = new double[uiNum_Elements];
//		printf("%i\n",i_lpOpacity_Map->GetNumElements());
//		printf("Alloc Output\n"); fflush(stdout);
		g_lpcOutput = new ES::Spectrum(i_cTarget);
//		printf("Alloc Reference\n"); fflush(stdout);
		g_lpcReference = new ES::Spectrum( g_lpcOutput[0] );
//		printf("Alloc OP\n"); fflush(stdout);
		Allocate_Opacity_Profile(g_uiOP_Multi_Num_Ions > 0 ? g_uiOP_Multi_Num_Ions : 2,true);

//		printf("Set Velocities\n"); fflush(stdout);
		if (i_lpOpacity_Map != nullptr)
		{
			for (unsigned int uiI = 1; uiI < i_lpOpacity_Map->GetNumElements(); uiI++)
			{
				g_lpdVelocities[uiI - 1] = i_lpOpacity_Map->GetElement(0,uiI) * 1e-8;
//				printf("%.2e\n",g_lpdVelocities[uiI - 1]);
			}
		}
		else
			for (unsigned int uiI = 0; uiI < uiNum_Elements; uiI++)
				g_lpdVelocities[uiI] = (128.0 / (uiNum_Elements + 1.0)) * (uiI + 1); // between 0 and 128


//		printf("es - grid\n");  fflush(stdout);
		g_lpcGrid = new ES::Synow::Grid(i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, uiNum_Elements, g_lpdVelocities,true);
//		printf("es - grid (exp)\n");  fflush(stdout);
		g_lpcGrid_Exp = new ES::Synow::Grid(i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), g_dMin_Delta, 2048, g_lpdVelocities,true);

//		printf("es - opacity\n");  fflush(stdout);
		g_lpcOpacity = new ES::Synow::Opacity( g_lpcGrid[0], getenv("SYNXX_LINES_DATA_PATH"), getenv("SYNXX_REF_LINE_DATA_PATH"), "user_profile", 10.0, -3.0 );
//		printf("es - opacity (exp)\n");  fflush(stdout);
		g_lpcOpacity_Exp = new ES::Synow::Opacity( g_lpcGrid_Exp[0], getenv("SYNXX_LINES_DATA_PATH"), getenv("SYNXX_REF_LINE_DATA_PATH"), "exp", 10.0, -3.0 );

//		printf("es - source\n");  fflush(stdout);
		g_lpcSource = new ES::Synow::Source (g_lpcGrid[0], 16.0);
//		printf("es - source (exp)\n");  fflush(stdout);
		g_lpcSource_Exp = new ES::Synow::Source (g_lpcGrid_Exp[0], 16.0);

//		printf("es - spectrum\n");  fflush(stdout);
		g_lpcSpectrum = new ES::Synow::Spectrum( g_lpcGrid[0], g_lpcOutput[0], g_lpcReference[0], 60, false);
//		printf("es - spectrum (exp)\n");  fflush(stdout);
		g_lpcSpectrum_Exp = new ES::Synow::Spectrum( g_lpcGrid_Exp[0], g_lpcOutput[0], g_lpcReference[0], 60, false);
//		std::cout << "alloc grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//		std::cout << "alloc grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	}
	else if (i_lpOpacity_Map)
	{
//		printf("velocity update\n");  fflush(stdout);
		for (unsigned int uiI = 1; uiI < i_lpOpacity_Map->GetNumElements(); uiI++)
		{
			g_lpdVelocities[uiI - 1] = i_lpOpacity_Map->GetElement(0,uiI) * 1e-8;
		}
//		std::cout << "alloc(opmap) grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//		std::cout << "alloc(opmap) grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	}
	else if (g_lpdVelocities)
	{
		
//		printf("velocity update (predef)\n");  fflush(stdout);
		for (unsigned int uiI = 0; uiI < uiNum_Elements; uiI++)
			g_lpdVelocities[uiI] = (128.0 / (uiNum_Elements + 1.0)) * (uiI + 1); // between 0 and 128
//		std::cout << "alloc(vel) grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//		std::cout << "alloc(vel) grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	}
}

void Generate_Synow_Spectra(const ES::Spectrum &i_cTarget, const XDATASET & i_cOpacity_Map_A, const XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput, const double & i_dPower_Law_A, const double & i_dPower_Law_B)
{
	//printf("allocate\n"); fflush(stdout);
	Allocate_Synow_Classes(i_cTarget,0,&i_cOpacity_Map_A);
//    o_cOutput = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());
//    ES::Spectrum output = ES::Spectrum::create_from_spectrum( o_cOutput );
//    ES::Spectrum reference = ES::Spectrum::create_from_spectrum( i_cTarget );
	//printf("vmax\n"); fflush(stdout);
//	printf("I: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
//	printf("%.3e\n",o_cOutput.flux(100));

	double	dVmax = i_cOpacity_Map_A.GetElement(0,i_cOpacity_Map_A.GetNumElements() - 1) * 1.0e-8;
    // Grid object.
	

    // Attach setups one by one.
		// Initialize Ion data
    ES::Synow::Setup cSetup;
	g_lpcOutput[0].zero_flux();

//	double	dT_ref = i_cParameters.Get(0) * (i_cOpacity_Map_A.GetElement(2,0) - i_cOpacity_Map_A.GetElement(1,0)) + i_cOpacity_Map_A.GetElement(1,0);
//	double	dT_Scalar = pow(i_cParameters.Get(0) / i_cOpacity_Map_A.GetElement(1,0),-2.0);
	double dT_ref = i_cParameters.Get(0) / i_cOpacity_Map_A.GetElement(1,0);
//	printf("t scalar = %.2e (%.4f %.4f)\n",dT_ref,i_cParameters.Get(0),i_cOpacity_Map_A.GetElement(1,0));
//	int iT_ref = (int )(i_cParameters.Get(0) + 0.5);
//	if (iT_ref < 0)
//		iT_ref = 0;
//	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
//		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
//	iT_ref++;

//	printf("setup\n"); fflush(stdout);
    cSetup.a0 = 1.0;
    cSetup.a1 = 0.0;
    cSetup.a2 = 0.0;
    cSetup.v_phot = i_cParameters.Get(1);
	//printf("Vphot = %.2f\n",cSetup.v_phot);
    cSetup.v_outer = dVmax;
	//printf("phot\n");
    cSetup.t_phot = i_cParameters.Get(2);
	//printf("ions\n");
	cSetup.ions.push_back(i_uiIon);
	//printf("active\n");
	cSetup.active.push_back(true);
	//printf("log_tau\n");
	cSetup.log_tau.push_back(i_cParameters.Get(4));
	//printf("temp\n");
	cSetup.temp.push_back(i_cParameters.Get(3));
	//printf("form\n");
	cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
//	printf("userprof\n");
	cSetup.user_profile = g_lpdOpacity_Profile;
	cSetup.additive_opacities = true;
	//printf("%i %i\n",i_cOpacity_Map_A.GetNumElements(),i_cOpacity_Map_B.GetNumElements());
	//std::cout << "setup grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//	std::cout << "setup grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
//	fflush(stdout);
	
	if (i_cOpacity_Map_B.GetNumElements() > 0)
		cSetup.user_profile.resize(2);
	else
		cSetup.user_profile.resize(1);

	cSetup.user_profile[0].resize(i_cOpacity_Map_A.GetNumElements() - 1,0);
	for (unsigned int uiI = 0; uiI < i_cOpacity_Map_A.GetNumElements() - 1; uiI++)
	{
//		cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(iT_ref,uiI + 1);
		if (!i_cOpacity_Map_A.IsElementEmpty(1,uiI + 1))
			cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(1,uiI + 1) * pow(dT_ref,i_dPower_Law_A);
		else
			cSetup.user_profile[0][uiI] = 0.0;
		//std::cout << uiI << " " << cSetup.user_profile[0][uiI] << std::endl;
	}
	if (i_cOpacity_Map_B.GetNumElements() > 0)
	{
		cSetup.ions.push_back(i_uiIon);
		cSetup.active.push_back(true);
		cSetup.log_tau.push_back(i_cParameters.Get(6));
		cSetup.temp.push_back(i_cParameters.Get(5));
		cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
		cSetup.user_profile[1].resize(i_cOpacity_Map_B.GetNumElements() - 1,0);
		for (unsigned int uiI = 0; uiI < i_cOpacity_Map_B.GetNumElements() - 1; uiI++)
		{
//			cSetup.user_profile[1][uiI] = i_cOpacity_Map_B.GetElement(iT_ref,uiI + 1);
			if (!i_cOpacity_Map_B.IsElementEmpty(1,uiI + 1))
				cSetup.user_profile[1][uiI] = i_cOpacity_Map_B.GetElement(1,uiI + 1) * pow(dT_ref,i_dPower_Law_B);
			else
				cSetup.user_profile[1][uiI] = 0.0;
			//std::cout << uiI << " " << cSetup.user_profile[1][uiI] << std::endl;
		}
	}

//	std::cout << "setup(2) grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//	std::cout << "setup(2) grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	//printf("reset\n"); fflush(stdout);
	g_lpcGrid->reset(cSetup);
//	std::cout << "reset grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//	std::cout << "reset grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	//printf("generate\n"); fflush(stdout);
    g_lpcGrid[0]( cSetup );
//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
//	std::cout << "generate grid wl " << g_lpcGrid->wl << std::endl; std::cout.flush();
//	std::cout << "generate grid v_user " << g_lpcGrid->v_user << std::endl; std::cout.flush();
	o_cOutput = g_lpcOutput[0];
	//printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

//	printf("%.3e\n",o_cOutput.flux(100));
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

}

void Generate_Synow_Multi_Ion_Spectra(const double & i_dT_days, const double & i_dPS_Temp_kK, const double & i_dPS_Velocity_kkms, ION_DATA * i_lpcIon_Data, unsigned int i_uiNum_Ions, ES::Spectrum &io_cOutput)
{
	if (io_cOutput.size() == 0)
	{
		fprintf(stderr,"Generate_Synow_Multi_Ion_Spectra: Output paramter has not been set up.  You must set the wavelength range and spacing before calling this function.\n");
		return;
	}
		
	io_cOutput.zero_out();
//	printf("allocate OP %i\n",i_uiNum_Ions);
	XDATASET * lpVelocity_Info_Map = nullptr;
	for (unsigned int uiI = 0; uiI < i_uiNum_Ions && lpVelocity_Info_Map == nullptr; uiI++)
		if (i_lpcIon_Data[uiI].m_lpcOpacity_Map && i_lpcIon_Data[uiI].m_lpcOpacity_Map->GetNumElements() > 0)
			lpVelocity_Info_Map = i_lpcIon_Data[uiI].m_lpcOpacity_Map;
	if (!lpVelocity_Info_Map)
	{
		fprintf(stderr,"Generate_Synow_Multi_Ion_Spectra: no valid velocity map.\n");
		return;
	}
	if (lpVelocity_Info_Map)
	{
		Allocate_Opacity_Profile(i_uiNum_Ions);
		Allocate_Synow_Classes(io_cOutput,0,lpVelocity_Info_Map);

		double	dVmax = lpVelocity_Info_Map->GetElement(0,lpVelocity_Info_Map->GetNumElements() - 1) * 1.0e-8;
		// Grid object.
	

	//	printf("setup\n");
		// Attach setups one by one.
			// Initialize Ion data
		ES::Synow::Setup cSetup;
		g_lpcOutput[0].zero_flux();

	//	double	dT_ref = i_cParameters.Get(0) * (i_cOpacity_Map_A.GetElement(2,0) - i_cOpacity_Map_A.GetElement(1,0)) + i_cOpacity_Map_A.GetElement(1,0);
	//	double	dT_Scalar = pow(i_dT_days / i_lpcIon_Data[0].m_lpcOpacity_Map->GetElement(1,0),-2.0);
	//	printf("t scalar = %.2e (%.4f %.4f)\n",dT_Scalar,i_cParameters.Get(0),i_cOpacity_Map_A.GetElement(1,0));
	//	int iT_ref = (int )(i_cParameters.Get(0) + 0.5);
	//	if (iT_ref < 0)
	//		iT_ref = 0;
	//	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
	//		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
	//	iT_ref++;

		cSetup.a0 = 1.0;
		cSetup.a1 = 0.0;
		cSetup.a2 = 0.0;
		cSetup.v_phot = i_dPS_Velocity_kkms;
	//	printf("Vphot = %.2f\n",cSetup.v_phot);
		cSetup.v_outer = dVmax;
		cSetup.t_phot = i_dPS_Temp_kK;

	//	printf("ion defs\n");
		for (unsigned int uiI = 0; uiI < i_uiNum_Ions; uiI++)
		{
			if (i_lpcIon_Data[uiI].m_lpcOpacity_Map && i_lpcIon_Data[uiI].m_lpcOpacity_Map->GetNumElements() > 0)
			{
				printf("Setting up ion %i, scalar %f\n",i_lpcIon_Data[uiI].m_uiIon,i_lpcIon_Data[uiI].m_dScalar);
				double	dT_Scalar = pow(i_dT_days / i_lpcIon_Data[uiI].m_lpcOpacity_Map->GetElement(1,0),i_lpcIon_Data[uiI].m_dTime_Power_Law);
				cSetup.ions.push_back(i_lpcIon_Data[uiI].m_uiIon);
				cSetup.active.push_back(true);
				cSetup.log_tau.push_back(i_lpcIon_Data[uiI].m_dScalar);
				cSetup.temp.push_back(i_lpcIon_Data[uiI].m_dExcitation_Temp);
				cSetup.form.push_back(ES::Synow::Setup::form_user_profile);
				cSetup.user_profile = g_lpdOpacity_Profile;
				for (unsigned int uiJ = 0; uiJ < i_lpcIon_Data[uiI].m_lpcOpacity_Map->GetNumElements() - 1; uiJ++)
				{
			//		cSetup.user_profile[0][uiI] = i_cOpacity_Map_A.GetElement(iT_ref,uiI + 1);
					if (!i_lpcIon_Data[uiI].m_lpcOpacity_Map->IsElementEmpty(1,uiJ + 1))
						cSetup.user_profile[uiI][uiJ] = i_lpcIon_Data[uiI].m_lpcOpacity_Map->GetElement(1,uiJ + 1) * dT_Scalar;
					else
						cSetup.user_profile[1][uiI] = 0.0;
				}
			}
		}
		g_lpcGrid->reset(cSetup);
	//	printf("generate\n");
		g_lpcGrid[0]( cSetup );
	//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
		io_cOutput = g_lpcOutput[0];
	//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

	//	printf("%.3e\n",o_cOutput.flux(100));
	//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
	}
}

void Generate_Synow_Spectra_Exp(const ES::Spectrum &i_cTarget, unsigned int i_uiIon, const XVECTOR & i_cParameters, ES::Spectrum &o_cOutput)
{
//    o_cOutput = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());
//    ES::Spectrum output = ES::Spectrum::create_from_spectrum( o_cOutput );
//    ES::Spectrum reference = ES::Spectrum::create_from_spectrum( i_cTarget );

//	printf("I: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));
//	printf("%.3e\n",o_cOutput.flux(100));

    // Grid object.
	//printf("Allocate %i\n",i_cTarget.size());
	Allocate_Synow_Classes(i_cTarget,2048,nullptr);

	//printf("Setup\n");
	double	dVmax = i_cParameters.Get(5);
	if (i_cParameters.Get_Size() > 7 && i_cParameters.Get(10) > dVmax)
		dVmax = i_cParameters.Get(10);

    // Attach setups one by one.
		// Initialize Ion data
    ES::Synow::Setup cSetup;
	g_lpcOutput[0].zero_flux();

    cSetup.a0 = 1.0;
    cSetup.a1 = 0.0;
    cSetup.a2 = 0.0;
    cSetup.v_phot = i_cParameters.Get(0);
	//printf("Vphot = %.2f\n",cSetup.v_phot);
    cSetup.v_outer = dVmax;
    cSetup.t_phot = i_cParameters.Get(1);
	cSetup.ions.push_back(i_uiIon);
	cSetup.active.push_back(true);
	cSetup.log_tau.push_back(i_cParameters.Get(2));
	cSetup.temp.push_back(i_cParameters.Get(3));
	cSetup.v_min.push_back(i_cParameters.Get(4));
	cSetup.v_max.push_back(i_cParameters.Get(5));
	cSetup.aux.push_back(i_cParameters.Get(6));
	cSetup.form.push_back(ES::Synow::Setup::form_exp);
	if (i_cParameters.Get_Size() > 7)
	{
		cSetup.ions.push_back(i_uiIon);
		cSetup.active.push_back(true);
		cSetup.log_tau.push_back(i_cParameters.Get(7));
		cSetup.temp.push_back(i_cParameters.Get(8));
		cSetup.v_min.push_back(i_cParameters.Get(9));
		cSetup.v_max.push_back(i_cParameters.Get(10));
		cSetup.aux.push_back(i_cParameters.Get(11));
		cSetup.form.push_back(ES::Synow::Setup::form_exp);
	}

	g_lpcGrid_Exp->reset(cSetup);
	//printf("Generate\n");
    g_lpcGrid_Exp[0]( cSetup );
//	printf("P: %.2e %.2e\n",output.wl(100),output.flux(100));
	o_cOutput = g_lpcOutput[0];
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

//	printf("%.3e\n",o_cOutput.flux(100));
//	printf("O: %.2e %.2e\n",o_cOutput.wl(100),o_cOutput.flux(100));

}


// Does a point (vector) fall fully inside a given set of bounds.  e.g. x_i < y_i for all i
bool xPoint_Within_Bound(const XVECTOR &i_lpVector, const XVECTOR &i_cThreshold)
{
	XVECTOR cTest;
	bool bRet = (i_cThreshold.Get_Size() == i_lpVector.Get_Size());
	if (!bRet)
		fprintf(stderr,"xPoint_Within_Bound: Convergence threshold size not the same as paramter vector size!\n");

	for (unsigned int uiK = 0; uiK < i_lpVector.Get_Size() && bRet; uiK++)
	{
		bRet = (fabs(i_lpVector.Get(uiK)) < i_cThreshold.Get(uiK));
	}
	return bRet;
}

// Given a set of points in n-space and a bound, is the space between all points less than the bound.  e.g. (|x_i,j - x_k,j| < y_j) for all i,j,k
bool xConvergence(const XVECTOR * i_lpVectors, unsigned int i_uiNum_Points,const XVECTOR &i_cThreshold, XVECTOR * o_lpcConvergence_Fault)
{
	XVECTOR cTest;
	bool bRet = true;
	for(unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
		bRet &= (i_cThreshold.Get_Size() == i_lpVectors[uiI].Get_Size());
	if (!bRet)
		fprintf(stderr,"xConvergence: Convergence threshold size not the same as paramter vector size!\n");

	for(unsigned int uiI = 0; uiI < i_uiNum_Points && bRet; uiI++)
	{
		for (unsigned int uiJ = uiI + 1; uiJ < i_uiNum_Points && bRet; uiJ++)
		{
			cTest = i_lpVectors[uiJ] - i_lpVectors[uiI];
			for (unsigned int uiK = 0; uiK < cTest.Get_Size() && bRet; uiK++)
			{
				bRet = (fabs(cTest.Get(uiK)) < i_cThreshold.Get(uiK));
//				if (!bRet)
//					printf("%i %.2e %.2e\n",uiK,cTest.Get(uiK),i_cThreshold.Get(uiK));
				if (!bRet && o_lpcConvergence_Fault)
				{
					o_lpcConvergence_Fault[0] = cTest;
				}
			}
		}
	}
//	printf("Conv: %c\n",bRet ? 'y' : 'n');
	return bRet;
}

// is a given point within a cuboid.  i_lpcBounds must be a 2 element array, with the 
bool xBoundTest(const XVECTOR & i_cTest_Point, const XVECTOR * i_lpcBounds)
{
	bool bRet;
	bRet = (i_cTest_Point.Get_Size() == i_lpcBounds[0].Get_Size() && i_cTest_Point.Get_Size() == i_lpcBounds[1].Get_Size());
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds size not the same as test point size! %i %i %i\n",i_cTest_Point.Get_Size(),i_lpcBounds[0].Get_Size(), i_lpcBounds[1].Get_Size());
	XVECTOR cTest = i_cTest_Point - i_lpcBounds[0];
	for (unsigned int uiI = 0; uiI < i_lpcBounds[0].Get_Size() && bRet; uiI++)
	{
		bRet = i_lpcBounds[0].Get(uiI) <= i_lpcBounds[0].Get(uiI);
	}
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds invalid.  Must be a two element array, with index 0 as the lower bound and index 1 as upper bound.\n");
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	cTest = i_lpcBounds[1] - i_cTest_Point;
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	return bRet;
}

XVECTOR xCompute_Centroid(XVECTOR * i_lpvPoints, unsigned int i_uiNum_Points)
{
	XVECTOR cCentroid(i_lpvPoints[0].Get_Size());
	for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
		cCentroid.Set(uiI,0.0);

	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		cCentroid += i_lpvPoints[uiI];
	}
	cCentroid /= (double)(i_uiNum_Points);
	return cCentroid;
}



bool ValidateChi2(const ES::Spectrum &i_cTarget,const double &i_dMin_WL, const double &i_dMax_WL)
{
	bool bChi2 = true;
	unsigned int uiSource_Idx = 0;
	bool	bInvalid_Error = true;
	// Find lower bound for source data
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) <= i_dMax_WL && bChi2)
	{
		bChi2 &= (i_cTarget.flux_error(uiSource_Idx) != 0);
		bInvalid_Error &= (i_cTarget.flux_error(uiSource_Idx) == 0.0 || i_cTarget.flux_error(uiSource_Idx) == 1.0);
		uiSource_Idx++;
	}
	bChi2 &= !bInvalid_Error; // if the flux error is uniformly 0 or 1, don't use it
	return bChi2;
}
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, double * o_dMoments) // moments must be large enough to fit results
{
	memset(o_dMoments,0,sizeof(double) * i_uiNum_Moments);
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		double dXprod = i_dX[uiI];
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_dMoments[uiM] += dXprod;
			dXprod *=  i_dX[uiI];
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_dMoments[uiM] /= i_uiNum_Points;
}
void Get_Raw_Moments(const double * i_dX, unsigned int i_uiNum_Points, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments)
{
	o_vdMoments.clear();
	o_vdMoments.resize(i_uiNum_Moments,0.0);
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		double dXprod = i_dX[uiI];
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_vdMoments[uiM] += dXprod;
			dXprod *=  i_dX[uiI];
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_vdMoments[uiM] /= i_uiNum_Points;
}
void Get_Raw_Moments(const std::vector<double> &i_vdX, unsigned int i_uiNum_Moments, std::vector<double> & o_vdMoments)
{
	o_vdMoments.clear();
	o_vdMoments.resize(i_uiNum_Moments,0.0);
	for (std::vector<double>::const_iterator iterI = i_vdX.begin(); iterI != i_vdX.end(); iterI++)
	{
		double dXprod = *iterI;
		for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		{
			o_vdMoments[uiM] += dXprod;
			dXprod *=  *iterI;
		}
	}
	for (unsigned int uiM = 0; uiM < i_uiNum_Moments; uiM++)
		o_vdMoments[uiM] /= i_vdX.size();
}
void Get_Central_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dCentral_Moments)
{
	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dCentral_Moments[5] = i_dMoments[5] + ((((-5.0 * i_dMoments[0] * i_dMoments[0] + 15.0 * i_dMoments[1]) * i_dMoments[0] - 20.0 * i_dMoments[2]) * i_dMoments[0] + 15.0 * i_dMoments[3]) * i_dMoments[0] - 6.0 * i_dMoments[4]) * i_dMoments[0];
	case 5:
		o_dCentral_Moments[4] = i_dMoments[4] + (((4.0 * i_dMoments[0] * i_dMoments[0] - 10.0 * i_dMoments[1]) * i_dMoments[0] + 10.0 * i_dMoments[2]) * i_dMoments[0] - 5.0 * i_dMoments[3]) * i_dMoments[0];
	case 4:
		o_dCentral_Moments[3] = i_dMoments[3] + ((-3.0 * i_dMoments[0] * i_dMoments[0]  + 6.0 * i_dMoments[1]) * i_dMoments[0] - 4.0 * i_dMoments[2]) * i_dMoments[0];
	case 3:
		o_dCentral_Moments[2] = i_dMoments[2] + (2.0 * i_dMoments[0] * i_dMoments[0]  - 3.0 * i_dMoments[1]) * i_dMoments[0];
	case 2:
		o_dCentral_Moments[1] = i_dMoments[1] - i_dMoments[0] * i_dMoments[0];
	case 1:
		o_dCentral_Moments[0] = 0.0;
	case 0:
		break;
	}
}
void Get_Central_Moments(const std::vector<double> & i_vdMoments, std::vector<double> & o_vdMoments)
{
	std::stack<double> stackMoments;
	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_vdMoments.size())
	{
	default:
	case 6:
		stackMoments.push(i_vdMoments[5] + ((((-5.0 * i_vdMoments[0] * i_vdMoments[0] + 15.0 * i_vdMoments[1]) * i_vdMoments[0] - 20.0 * i_vdMoments[2]) * i_vdMoments[0] + 15.0 * i_vdMoments[3]) * i_vdMoments[0] - 6.0 * i_vdMoments[4]) * i_vdMoments[0]);
	case 5:
		stackMoments.push(i_vdMoments[4] + (((4.0 * i_vdMoments[0] * i_vdMoments[0] - 10.0 * i_vdMoments[1]) * i_vdMoments[0] + 10.0 * i_vdMoments[2]) * i_vdMoments[0] - 5.0 * i_vdMoments[3]) * i_vdMoments[0]);
	case 4:
		stackMoments.push(i_vdMoments[3] + ((-3.0 * i_vdMoments[0] * i_vdMoments[0]  + 6.0 * i_vdMoments[1]) * i_vdMoments[0] - 4.0 * i_vdMoments[2]) * i_vdMoments[0]);
	case 3:
		stackMoments.push(i_vdMoments[2] + (2.0 * i_vdMoments[0] * i_vdMoments[0]  - 3.0 * i_vdMoments[1]) * i_vdMoments[0]);
	case 2:
		stackMoments.push(i_vdMoments[1] - i_vdMoments[0] * i_vdMoments[0]);
	case 1:
		stackMoments.push(0.0);
	case 0:
		break;
	}
	while (!stackMoments.empty())
	{
		o_vdMoments.push_back(stackMoments.top());
		stackMoments.pop();
	}

}
void Get_Standardized_Moments(unsigned int i_uiNum_Moments, const double * i_dMoments, double * o_dStandardized_Moments)
{
	double	* lpdCentral_Moments = new double[i_uiNum_Moments];
	Get_Central_Moments(i_uiNum_Moments, i_dMoments,lpdCentral_Moments);

	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_uiNum_Moments)
	{
	default:
	case 6:
		o_dStandardized_Moments[5] = lpdCentral_Moments[5] / pow(lpdCentral_Moments[1],3.0);
	case 5:
		o_dStandardized_Moments[4] = lpdCentral_Moments[4] / pow(lpdCentral_Moments[1],2.5);
	case 4:
		o_dStandardized_Moments[3] = lpdCentral_Moments[3] / pow(lpdCentral_Moments[1],2.0);
	case 3:
		o_dStandardized_Moments[2] = lpdCentral_Moments[2] / pow(lpdCentral_Moments[1],1.5);
	case 2:
		o_dStandardized_Moments[1] = 1.0;
	case 1:
		o_dStandardized_Moments[0] = 0.0;
	case 0:
		break;
	}
	delete [] lpdCentral_Moments;
}
void Get_Standardized_Moments(const std::vector<double> & i_vdRaw_Moments, std::vector<double> & o_vdMoments)
{
	std::stack<double> stackMoments;
	std::vector<double> vCentral_Moments;
	Get_Central_Moments(i_vdRaw_Moments,vCentral_Moments);

	// it is correct there is no break statements in the switch below.  Higher order requests also return lower order requests
	switch (i_vdRaw_Moments.size())
	{
	default:
	case 6:
		stackMoments.push(vCentral_Moments[5] / pow(vCentral_Moments[1],3.0));
	case 5:
		stackMoments.push(vCentral_Moments[4] / pow(vCentral_Moments[1],2.5));
	case 4:
		stackMoments.push(vCentral_Moments[3] / pow(vCentral_Moments[1],2.0));
	case 3:
		stackMoments.push(vCentral_Moments[2] / pow(vCentral_Moments[1],1.5));
	case 2:
		stackMoments.push(1.0);
	case 1:
		stackMoments.push(0.0);
	case 0:
		break;
	}
	while (!stackMoments.empty())
	{
		o_vdMoments.push_back(stackMoments.top());
		stackMoments.pop();
	}
}


void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments)
{
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);
		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];

		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			lpdErrors[uiI] = dError;
		}

		if (o_lpdRaw_Moments)
			delete [] o_lpdRaw_Moments;
		if (o_lpdCentral_Moments)
			delete [] o_lpdCentral_Moments;
		if (o_lpdStandardized_Moments)
			delete [] o_lpdStandardized_Moments;

		o_lpdRaw_Moments = new double [i_uiNum_Moments];
		o_lpdCentral_Moments = new double [i_uiNum_Moments];
		o_lpdStandardized_Moments = new double [i_uiNum_Moments];

		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
		Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
		Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

		delete [] lpdErrors;
	}
}

void Bracket_Wavelength(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int &o_uiLower_Bound, unsigned int &o_uiUpper_Bound, unsigned int i_uiSuggested_Starting_Idx)
{
	while (i_cData.wl(i_uiSuggested_Starting_Idx) <= i_dTarget_Wavelength && i_uiSuggested_Starting_Idx < i_cData.size())
		i_uiSuggested_Starting_Idx++;
//	if (i_uiSuggested_Starting_Idx == 0)
//		printf("%.1f %.1f\n",i_dTarget_Wavelength,i_cData.wl(i_uiSuggested_Starting_Idx));
//	printf("%i %i\t",i_uiSuggested_Starting_Idx,i_cData.size());
	if (i_uiSuggested_Starting_Idx < i_cData.size())
	{
		if (i_uiSuggested_Starting_Idx != 0)
		{
			o_uiLower_Bound = i_uiSuggested_Starting_Idx - 1;
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_uiSuggested_Starting_Idx;
		}
	}
	else
	{
		if (i_cData.wl(i_uiSuggested_Starting_Idx - 1) <= i_dTarget_Wavelength)
		{
			o_uiLower_Bound = i_cData.size() - 1;
			o_uiUpper_Bound = i_cData.size();
		}
		else
		{
			o_uiLower_Bound = 
			o_uiUpper_Bound = i_cData.size();
		}
	}
//	printf("%i %i\n",o_uiLower_Bound,o_uiUpper_Bound);
}

double Interpolate_Flux(const ES::Spectrum &i_cData, const double & i_dTarget_Wavelength, unsigned int i_uiLower_Bound, unsigned int i_uiUpper_Bound)
{
	double	dFlux = nan("");
	if (i_uiLower_Bound != i_uiUpper_Bound && i_uiUpper_Bound < i_cData.size())
	{ 
		double dSlope = (i_cData.flux(i_uiLower_Bound + 1) - i_cData.flux(i_uiLower_Bound)) / (i_cData.wl(i_uiLower_Bound + 1) - i_cData.wl(i_uiLower_Bound));
		dFlux = (i_dTarget_Wavelength - i_cData.wl(i_uiLower_Bound)) * dSlope + i_cData.flux(i_uiLower_Bound);
	}
//	else
//		printf("%i %i %i\n",i_uiLower_Bound,i_uiUpper_Bound,i_cData.size());
	return dFlux;
}

void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, double * &o_lpdRaw_Moments, double * &o_lpdCentral_Moments, double * &o_lpdStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	double * lpdErrors = new double[uiCompare_Count];
	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			lpdErrors[uiI] = dError;
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	if (o_lpdRaw_Moments)
		delete [] o_lpdRaw_Moments;
	if (o_lpdCentral_Moments)
		delete [] o_lpdCentral_Moments;
	if (o_lpdStandardized_Moments)
		delete [] o_lpdStandardized_Moments;

	o_lpdRaw_Moments = new double [i_uiNum_Moments];
	o_lpdCentral_Moments = new double [i_uiNum_Moments];
	o_lpdStandardized_Moments = new double [i_uiNum_Moments];

	Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiNum_Moments,o_lpdRaw_Moments);
	Get_Central_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdCentral_Moments);
	Get_Standardized_Moments(i_uiNum_Moments,o_lpdRaw_Moments,o_lpdStandardized_Moments);

	delete [] lpdErrors;
}
void Get_Fit_Moments_2(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiNum_Moments, std::vector<double> & o_vRaw_Moments, std::vector<double> & o_vCentral_Moments, std::vector<double> & o_vStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	std::vector<double> vErrors;

	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			vErrors.push_back(dError);
		else
			vErrors.push_back(0.0);
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	Get_Raw_Moments(vErrors,i_uiNum_Moments,o_vRaw_Moments);
	Get_Central_Moments(o_vRaw_Moments,o_vCentral_Moments);
	Get_Standardized_Moments(o_vRaw_Moments,o_vStandardized_Moments);

}

void Get_Fit_Moments(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiMax_Moments, std::vector<double> &o_vRaw_Moments, std::vector<double> &o_vCentral_Moments, std::vector<double> &o_vStandardized_Moments, const double & i_dTarget_Normalization_Flux, const double & i_dGenereted_Normalization_Flux)
{
	unsigned int uiSource_Idx = 0;
	unsigned int uiSource_Gen_Idx_Lower = 0;
	unsigned int uiSource_Gen_Idx_Upper = 0;
	unsigned int uiSource_Gen_Start = 0;
	while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
		uiSource_Idx++;
	while (uiSource_Gen_Start < i_cGenerated.size() && i_cGenerated.wl(uiSource_Gen_Start) < i_dMin_WL)
		uiSource_Gen_Start++;
	if (uiSource_Gen_Start > 0)
		uiSource_Gen_Start--;
	// In the event that the generated data doesn't have the same range as the target data, we want to make sure that we only fit over the mutual range
	if (i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
	{
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_cGenerated.wl(uiSource_Gen_Start))
			uiSource_Idx++;
	}

	double dSource_Normalization = 1.0 / i_dTarget_Normalization_Flux;
	double dGenerated_Normalization = 1.0 / i_dGenereted_Normalization_Flux;

	unsigned int uiCompare_Count = 0;
	while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count + uiSource_Idx) <= i_dMax_WL && i_cGenerated.wl(i_cGenerated.size() - 1))
		uiCompare_Count++;
	double * lpdErrors = new double[uiCompare_Count];
	for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
	{
		Bracket_Wavelength(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper,uiSource_Gen_Start);
//		if (uiSource_Gen_Idx_Lower == uiSource_Gen_Idx_Upper)
//			printf("%i %i %i %.1f %i %i\n",uiI,uiSource_Idx,uiI + uiSource_Idx,i_cTarget.wl(uiI + uiSource_Idx),uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper);
		double	dGenerated_Flux_Norm = Interpolate_Flux(i_cGenerated, i_cTarget.wl(uiI + uiSource_Idx), uiSource_Gen_Idx_Lower,uiSource_Gen_Idx_Upper) * dGenerated_Normalization;
		double	dSource_Flux_Norm = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization;
		double	dError = dSource_Flux_Norm - dGenerated_Flux_Norm;
		if (!isnan(dGenerated_Flux_Norm))
			lpdErrors[uiI] = dError;
		uiSource_Gen_Start = uiSource_Gen_Idx_Upper;
	}

	o_vRaw_Moments.clear();
	o_vCentral_Moments.clear();
	o_vStandardized_Moments.clear();

	Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiMax_Moments,o_vRaw_Moments);
	Get_Central_Moments(o_vRaw_Moments,o_vCentral_Moments);
	Get_Standardized_Moments(o_vRaw_Moments,o_vStandardized_Moments);

	delete [] lpdErrors;
}

double Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, FIT_TYPE i_eFit,unsigned int i_uiFit_Moment)
{
	if (i_uiFit_Moment > 0)
		i_uiFit_Moment--; // use moment is an index, so the 1st moment is index 0

	double dFit = nan("");
	double	*lpdMoments = new double [i_uiFit_Moment];
	double	*lpdExtended_Moments = new double [i_uiFit_Moment];
	if (i_cTarget.wl(0) == i_cGenerated.wl(0) && i_cTarget.size() >= i_cGenerated.size())
	{
		unsigned int uiSource_Idx = 0;
		while (uiSource_Idx < i_cTarget.size() && i_cTarget.wl(uiSource_Idx) < i_dMin_WL)
			uiSource_Idx++;
		unsigned int uiNP = 0;
		double dMin_Target_Flux = DBL_MAX;
		unsigned int uiNorm_Idx = 0;
		for (unsigned int uiI = uiSource_Idx; uiI < i_cTarget.size() && i_cTarget.wl(uiI) <= i_dMax_WL; uiI++)
		{
			if (i_cTarget.flux(uiI)> 0.0 && i_cTarget.flux(uiI) < dMin_Target_Flux)
			{
				dMin_Target_Flux = i_cTarget.flux(uiI);
				uiNorm_Idx = uiI;
			}
		}
		double dSource_Normalization = 1.0 / i_cTarget.flux(uiNorm_Idx);
		double dGenerated_Normalization = 1.0 / i_cGenerated.flux(uiNorm_Idx);

		unsigned int uiCompare_Count = uiSource_Idx;
		while (uiCompare_Count < i_cTarget.size() && i_cTarget.wl(uiCompare_Count) <= i_dMax_WL)
			uiCompare_Count++;
		uiCompare_Count -= uiSource_Idx;
		double * lpdErrors = new double[uiCompare_Count];
		double	dChi2 = 0.0;
		double dErr_Sum = 0.0;
		for (unsigned int uiI = 0; uiI < uiCompare_Count; uiI++)
		{
			double	dError = i_cTarget.flux(uiI + uiSource_Idx) * dSource_Normalization - i_cGenerated.flux(uiI + uiSource_Idx) * dGenerated_Normalization;
			if (i_eFit == FIT_CHI2)
				dChi2 += dError / (i_cTarget.flux_error(uiI + uiSource_Idx) * dSource_Normalization);
			lpdErrors[uiI] = dError;
			dErr_Sum += (dError * dError * dError);
		}
		Get_Raw_Moments(lpdErrors,uiCompare_Count,i_uiFit_Moment + 1,lpdMoments);
		switch (i_eFit)
		{
		case FIT_CHI2:
			dFit = dChi2;
			break;
		case FIT_RAW:
			dFit = lpdMoments[i_uiFit_Moment];
			break;
		case FIT_CENTRAL:
			Get_Central_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		case FIT_STANDARDIZED:
			Get_Standardized_Moments(i_uiFit_Moment,lpdMoments,lpdExtended_Moments);
			dFit = lpdExtended_Moments[i_uiFit_Moment];
			break;
		}
		

		delete [] lpdErrors;
	}
	else
		fprintf(stderr,"spectrafit: Mismatched target and generated files.\n");
	delete [] lpdMoments;
	delete [] lpdExtended_Moments;
	return dFit;
}



void	SPECTRAL_DATABASE::Set_Base_Data(unsigned int i_uiIon, const char * i_lpszModel_Name)
{
	if (i_uiIon != 0 && i_uiIon != m_uiIon && ((!i_lpszModel_Name && m_lpszModel_Name && m_lpszModel_Name[0] != 0) || (!m_lpszModel_Name && i_lpszModel_Name && i_lpszModel_Name[0] != 0) || (m_lpszModel_Name && i_lpszModel_Name && strcmp(i_lpszModel_Name,m_lpszModel_Name) != 0)))
		Deallocate(); // there has been a change in the ion or model
	m_uiIon = i_uiIon;
	if (i_lpszModel_Name && i_lpszModel_Name[0] != 0)
	{
		m_lpszModel_Name = new char[strlen(i_lpszModel_Name) + 1];
		strcpy(m_lpszModel_Name,i_lpszModel_Name);
	}
	if (m_lpszModel_Name && m_lpszModel_Name[0] != 0)
	{
		unsigned int i_uiLength = 30 + strlen(m_lpszModel_Name);
		m_lpszDatabase_Filename = new char[i_uiLength];
		sprintf(m_lpszDatabase_Filename,".spectradata.%s.%i.database",m_lpszModel_Name,m_uiIon);
	}
}
void	SPECTRAL_DATABASE::Get_Base_Data(unsigned int &o_uiIon, char * o_lpszModel_Name, unsigned int i_uiMax_Model_Name_Length)
{
	o_uiIon = m_uiIon;
	if (o_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
		o_lpszModel_Name[0] = 0;
	if (o_lpszModel_Name && m_lpszModel_Name && i_uiMax_Model_Name_Length > 0)
	{
		strncpy(o_lpszModel_Name,m_lpszModel_Name,i_uiMax_Model_Name_Length);
	}
}

unsigned int SPECTRAL_DATABASE::Read_Database(void)
{
	FILE * fileIn;
	unsigned int uiFault_Code = 0;
	unsigned int uiStrLen_Model_Name = 0;
	double			* lpdDouble_Buffer = nullptr;
	unsigned int	uiDouble_Buffer_Length = 0;
	char * 			lpszModel_Name_Buffer = nullptr;
	unsigned int	uiIon = 0;
	unsigned int	uiModel_Name_Length = 0;
	unsigned int	uiParameter_Vector_Length = 0;
	unsigned int	uiNum_Parameter_Vectors = 0;
	unsigned int	uiNum_Tuples = 0;
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiStrLen_Model_Name = strlen(m_lpszModel_Name) + 1;
		if (uiStrLen_Model_Name == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}
	// make sure that there is no data in the database
	Deallocate_Non_Base_Data();
	Zero_Non_Base_Data();

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"rb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiIon != m_uiIon)
			uiFault_Code = 405; // ion mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiModel_Name_Length != uiStrLen_Model_Name) // ensure that the current and saved model names are of the same length
			uiFault_Code = 505; // model name length mismatch
	}

	if (uiFault_Code == 0)
	{
		lpszModel_Name_Buffer = new char [uiModel_Name_Length];
		size_t iRead_Size = fread(lpszModel_Name_Buffer,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (strcmp(lpszModel_Name_Buffer,m_lpszModel_Name) != 0) // make sure the data in the file matches
			uiFault_Code = 515; // model name mismatch
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fread(&uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		if (uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}
	if (uiFault_Code == 0)
	{
		m_lpvParameter_Data = new XVECTOR[uiNum_Parameter_Vectors];
		if (!m_lpvParameter_Data)
			uiFault_Code = 800; // failed to allocate parameter vector data
	}
	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Heads)
			uiFault_Code = 900; // failed to allocate tuple list head array
	}

	if (uiFault_Code == 0)
	{
		m_lplpcTuple_List_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[uiNum_Parameter_Vectors];
		if (!m_lplpcTuple_List_Tails)
			uiFault_Code = 905; // failed to allocate tuple list tail array
	}

	if (uiFault_Code == 0)
	{
		lpdDouble_Buffer = new double[uiParameter_Vector_Length];
		uiDouble_Buffer_Length = uiParameter_Vector_Length;
		if (!lpdDouble_Buffer)
			uiFault_Code = 1000; // failed to allocate temporary buffer for double precision data
	}
	for(unsigned int uiI = 0; uiI < uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		
		m_lpvParameter_Data[uiI].Set_Size(uiParameter_Vector_Length);
		m_lplpcTuple_List_Heads[uiI] = m_lplpcTuple_List_Tails[uiI] = nullptr;
		size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiParameter_Vector_Length,fileIn);
		if (iRead_Size < uiParameter_Vector_Length)
			uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			if (uiDouble_Buffer_Length < (uiNum_Tuples * 2))
			{
				if (lpdDouble_Buffer)
					delete [] lpdDouble_Buffer;
				uiDouble_Buffer_Length = uiNum_Tuples * 2;
				lpdDouble_Buffer = new double[uiDouble_Buffer_Length];
				if (!lpdDouble_Buffer)
					uiFault_Code = 300000000 + uiI; // failed to reallocate double buffer during vector i. 
			}
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fread(lpdDouble_Buffer,sizeof(double),uiNum_Tuples,fileIn);
			if (iRead_Size != uiNum_Tuples)
				uiFault_Code = 400000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
		}
		if (uiFault_Code == 0)
		{
			for (unsigned int uiJ = 0; uiJ < uiNum_Tuples && uiFault_Code == 0; uiJ+=2)
			{
				unsigned int uiWL_Idx = uiJ * 2;
				unsigned int uiInt_Idx = uiWL_Idx + 1;
				SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (!lpCurr)
					uiFault_Code = 500000000 + uiI; // failed to allocate a tuple node for vector i
				if (uiFault_Code == 0)
				{
					lpCurr->m_dWavelength = lpdDouble_Buffer[uiWL_Idx];
					lpCurr->m_dIntensity = lpdDouble_Buffer[uiInt_Idx];
					if (m_lplpcTuple_List_Heads[uiI] == nullptr)
					{
						m_lplpcTuple_List_Heads[uiI] = lpCurr;
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
					else
					{
						lpCurr->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpCurr;
					}
				}
			}
		}
	}
	if (uiFault_Code != 0)
	{
		Deallocate();
	}
	// deallocate local temporary storage
	if (lpdDouble_Buffer)
		delete [] lpdDouble_Buffer;
	if (lpszModel_Name_Buffer)
		delete [] lpszModel_Name_Buffer;
	// return fault code
	return uiFault_Code;	
}
unsigned int SPECTRAL_DATABASE::Save_Database(void)
{
	FILE * fileIn = nullptr;
	unsigned int uiFault_Code = 0;
	unsigned int uiModel_Name_Length;
	unsigned int uiParameter_Vector_Length;
	
	if (!m_lpszModel_Name)
		uiFault_Code = 100; // no model name specified

	if (uiFault_Code == 0)
	{
		uiModel_Name_Length = strlen(m_lpszModel_Name) + 1;
		if (uiModel_Name_Length == 1)
			uiFault_Code = 100; // no model name specified
	}

	if (uiFault_Code == 0)
	{
		if (m_uiIon == 0 || m_uiIon > 9900)
			uiFault_Code = 200; // invalid ion
	}

	if (uiFault_Code == 0)
	{
		if (!m_lpszDatabase_Filename && m_lpszDatabase_Filename[0] == 0)
			uiFault_Code = 300; // invalid filename
	}
	if (uiFault_Code == 0)
	{
		fileIn = fopen(m_lpszDatabase_Filename,"wb");

		if (!fileIn)
			uiFault_Code = 305; // failed to open file
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiIon,sizeof(unsigned int),1,fileIn);

		if (iRead_Size != 1)
			uiFault_Code = 400; // failed to read ion.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiModel_Name_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 500; // failed to read model name length.  Unexpected EOF or file access error.
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(m_lpszModel_Name,sizeof(char),uiModel_Name_Length,fileIn);
		if (iRead_Size != uiModel_Name_Length)
			uiFault_Code = 510; // failed to read model name.  Unexpected EOF or file access error.
	}
	if (uiFault_Code == 0)
	{
		uiParameter_Vector_Length = m_lpvParameter_Data[0].Get_Size();
		if (uiParameter_Vector_Length == 0 || uiParameter_Vector_Length > 128)//@@WARNING: hard coded limit of 128 parameters
			uiFault_Code = 605; // invalid parameter vector length
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&uiParameter_Vector_Length,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 600; // failed to read parameter vector length.  Unexpected EOF or file access error.
	}


	if (uiFault_Code == 0)
	{
		if (m_uiNum_Parameter_Vectors == 0)
			uiFault_Code = 705; // invalid parameter vector number
	}

	if (uiFault_Code == 0)
	{
		size_t iRead_Size = fwrite(&m_uiNum_Parameter_Vectors,sizeof(unsigned int),1,fileIn);
		if (iRead_Size != 1)
			uiFault_Code = 700; // failed to read model name.  Unexpected EOF or file access error.
	}

	for(unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiFault_Code == 0; uiI++)
	{
		for( unsigned int uiJ = 0; uiJ < uiParameter_Vector_Length && uiFault_Code == 0; uiJ++)
		{
			double dBuffer = m_lpvParameter_Data[uiI].Get(uiJ);
			size_t iRead_Size = fwrite(&dBuffer,sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 100000000 + uiI; // failed to read parameter data for vector i. Unexpected end of file or file access error
		}
		unsigned int uiNum_Tuples = 0;
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiI];
		// count tuples
		while (lpCurr != nullptr)
		{
			uiNum_Tuples++;
			lpCurr = lpCurr->m_lpNext;
		}
		if (uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&uiNum_Tuples,sizeof(unsigned int),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 200000000 + uiI; // failed to read number of tuples for vector i. Unexpected end of file or file access error
		}
		// go through tuple list, writing each one to file
		lpCurr = m_lplpcTuple_List_Heads[uiI];
		while (lpCurr != nullptr && uiFault_Code == 0)
		{
			size_t iRead_Size = fwrite(&(lpCurr->m_dWavelength),sizeof(double),1,fileIn);
			if (iRead_Size != 1)
				uiFault_Code = 600000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			if (uiFault_Code == 0)
			{
				size_t iRead_Size = fwrite(&(lpCurr->m_dIntensity),sizeof(double),1,fileIn);
				if (iRead_Size != 1)
					uiFault_Code = 700000000 + uiI; // failed to read tuple data for vector i. Unexpected end of file or file access error
			}

			lpCurr = lpCurr->m_lpNext;
		}
		
	}
	if (uiFault_Code != 0)
	{
		if (fileIn)
			fclose(fileIn);
		if (uiFault_Code > 305)
		{
			char lpszCommand[256];
			sprintf(lpszCommand,"rm %s",m_lpszDatabase_Filename);
			system(lpszCommand);
		}
	}
	// return fault code
	return uiFault_Code;	
}


unsigned int		SPECTRAL_DATABASE::Add_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = Find_Parameter_Vector(i_vParameters);
	if (uiRet == -1)
	{// not found, add
		m_uiNum_Parameter_Vectors++;
		XVECTOR			* lpNew_List = new XVECTOR[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Heads = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		SPECTRAL_DATABASE_TUPLE_NODE ** lplpcNew_Tails = new SPECTRAL_DATABASE_TUPLE_NODE *[m_uiNum_Parameter_Vectors];
		m_uiNum_Parameter_Vectors--;
		if (lpNew_List && lplpcNew_Heads && lplpcNew_Tails)
		{
			for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors; uiI++)
			{
				lpNew_List[uiI] = m_lpvParameter_Data[uiI];
				lplpcNew_Heads[uiI] = m_lplpcTuple_List_Heads[uiI];
				lplpcNew_Tails[uiI] = m_lplpcTuple_List_Tails[uiI];
			}
			lpNew_List[m_uiNum_Parameter_Vectors] = i_vParameters;
			lplpcNew_Heads[m_uiNum_Parameter_Vectors] = nullptr;
			lplpcNew_Tails[m_uiNum_Parameter_Vectors] = nullptr;
			m_uiNum_Parameter_Vectors++;
			if (m_lpvParameter_Data)
				delete [] m_lpvParameter_Data;
			if (m_lplpcTuple_List_Heads)
				delete [] m_lplpcTuple_List_Heads;
			if (m_lplpcTuple_List_Tails)
				delete [] m_lplpcTuple_List_Tails;

			m_lpvParameter_Data = lpNew_List;
			m_lplpcTuple_List_Heads = lplpcNew_Heads;
			m_lplpcTuple_List_Tails = lplpcNew_Tails;
		}
	}
}
unsigned int		SPECTRAL_DATABASE::Find_Parameter_Vector(const XVECTOR & i_vParameters)
{
	unsigned int uiRet = -1;
	for (unsigned int uiI = 0; uiI < m_uiNum_Parameter_Vectors && uiRet == -1; uiI++)
	{
		if (i_vParameters == m_lpvParameter_Data[uiI])
			uiRet = uiI;
	}
	return uiRet;
}

void 	SPECTRAL_DATABASE::Add_Spectrum(const XVECTOR & i_vParameters, const ES::Spectrum &i_cSpectrum)
{
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (uiIdx == -1)
		uiIdx = Add_Parameter_Vector(i_vParameters);
	if (uiIdx != -1)
	{
		SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
		for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && uiIdx != -1; uiI++)
		{
			while (lpCurr && lpCurr->m_dWavelength <= i_cSpectrum.wl(uiI))
				lpCurr = lpCurr->m_lpNext;
			if (!lpCurr || lpCurr->m_dWavelength != i_cSpectrum.wl(uiI))
			{
				SPECTRAL_DATABASE_TUPLE_NODE * lpNew = new SPECTRAL_DATABASE_TUPLE_NODE;
				if (lpNew)
				{
					lpNew->m_dWavelength = i_cSpectrum.wl(uiI);
					lpNew->m_dIntensity = i_cSpectrum.flux(uiI);
					if (!m_lplpcTuple_List_Heads[uiI])
					{
						m_lplpcTuple_List_Heads[uiI] = lpNew;
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else if (!lpCurr)
					{
						m_lplpcTuple_List_Tails[uiI]->m_lpNext = lpNew;
						lpNew->m_lpPrev = m_lplpcTuple_List_Tails[uiI];
						m_lplpcTuple_List_Tails[uiI] = lpNew;
					}
					else
					{
						lpNew->m_lpNext = lpCurr;
						lpNew->m_lpPrev = lpCurr->m_lpPrev;
						lpCurr->m_lpPrev = lpNew;
						if (lpNew->m_lpPrev)
							lpNew->m_lpPrev->m_lpNext = lpNew;
						if (lpCurr == m_lplpcTuple_List_Heads[uiI]) // if we are pointing at the head, update the head.
							m_lplpcTuple_List_Heads[uiI] = lpNew;
					}
					lpCurr = lpNew; // set the current pointer to the newly added node
				}
				else
					uiIdx = -1;
			}
		}
	}
	if (uiIdx != -1)
	{
		unsigned int uiError = Save_Database();
		if (uiError != 0)
			fprintf(stderr,"Error %i while saving spectral database.\n",uiError);
	}
}


bool 	SPECTRAL_DATABASE::Find_Spectrum(const XVECTOR & i_vParameters, ES::Spectrum & io_cOutput, bool i_bAllow_Interpolate, bool i_bGenerate_Wavelength_List) // allow interpolate flag will return f(wl) interpolated between known nearby wl if the exact wl value is 
{
	bool bSuccess = false;
	unsigned int uiIdx = Find_Parameter_Vector(i_vParameters);
	if (i_bGenerate_Wavelength_List)
		io_cOutput.zero_out();
	else
		io_cOutput.zero_flux();
	if (uiIdx != -1)
	{
		if (i_bGenerate_Wavelength_List)
		{
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			unsigned int uiCount = 0;
			while(lpCurr)
			{
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			io_cOutput = ES::Spectrum::create_from_size(uiCount);

			lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			uiCount = 0;
			while(lpCurr)
			{
				io_cOutput.wl(uiCount) = lpCurr->m_dWavelength;
				io_cOutput.flux(uiCount) = lpCurr->m_dIntensity;
				uiCount++;
				lpCurr = lpCurr->m_lpNext;
			}
			bSuccess = true;
		}
		else
		{
			bSuccess = true;
			SPECTRAL_DATABASE_TUPLE_NODE * lpCurr = m_lplpcTuple_List_Heads[uiIdx];
			for (unsigned int uiI = 0; uiI < io_cOutput.size() && bSuccess; uiI++)
			{
				while (lpCurr && lpCurr->m_dWavelength < io_cOutput.wl(uiI))
					lpCurr = lpCurr->m_lpNext;
				if (lpCurr->m_dWavelength == io_cOutput.wl(uiI))
					io_cOutput.flux(uiI) = lpCurr->m_dIntensity;
				else if (i_bAllow_Interpolate && lpCurr && lpCurr->m_lpPrev)
				{
					// @@TODO more interpolation techniques
					double dSlope = (lpCurr->m_dIntensity - lpCurr->m_lpPrev->m_dIntensity) / (lpCurr->m_dWavelength - lpCurr->m_lpPrev->m_dWavelength);
					double	dX = (io_cOutput.wl(uiI) - lpCurr->m_lpPrev->m_dWavelength);
					io_cOutput.flux(uiI) = dX * dSlope + lpCurr->m_lpPrev->m_dIntensity;
				}
				else
					bSuccess = false;
			}
		}
	}
	return bSuccess;
}


void Get_Spectra_Data(const ES::Spectrum & i_cSpectrum, double * &o_lpdWavelengths, double * & o_lpdFluxes, unsigned int & o_uiNum_Points, const double & i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	unsigned int uiCount = 0;
	while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
		uiStart_Idx++;
	while ((uiStart_Idx + uiCount) < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx + uiCount) < i_dMax_WL)
		uiCount++;
	o_uiNum_Points = uiCount;
	if (o_lpdWavelengths)
		delete [] o_lpdWavelengths;
	if (o_lpdFluxes)
		delete [] o_lpdFluxes;
	o_lpdWavelengths = new double[uiCount];
	o_lpdFluxes = new double[uiCount];
	uiCount = 0;
	while ((uiStart_Idx + uiCount) < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx + uiCount) < i_dMax_WL)
	{
		o_lpdWavelengths[uiCount] = i_cSpectrum.wl(uiStart_Idx + uiCount);
		o_lpdFluxes[uiCount] = i_cSpectrum.flux(uiStart_Idx + uiCount);
		uiCount++;
	}
}

void Find_Flux_Minimum(const double * i_lpdWavelengths, const double * i_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_uiNum_Points && i_lpdWavelengths[uiStart_Idx] < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMinima_Flux = DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_uiNum_Points && (i_dMax_WL < 0 || i_lpdWavelengths[uiI] < i_dMax_WL); uiI++)
	{
		if (i_lpdFluxes[uiI] < o_dMinima_Flux)
		{
			o_dMinima_Flux = i_lpdFluxes[uiI];
			o_dMinima_WL = i_lpdWavelengths[uiI];
		}
	}
}

void Find_Flux_At_WL(const ES::Spectrum & i_cSpectrum, const double & i_dWL, double & o_dFlux)
{
	unsigned int uiIdx = 0;
	o_dFlux = nan("");
	while (uiIdx < i_cSpectrum.size() && i_cSpectrum.wl(uiIdx) < i_dWL)
		uiIdx++;
	if (uiIdx < i_cSpectrum.size())
		o_dFlux = i_cSpectrum.flux(uiIdx);

}

void Find_Flux_Minimum(const ES::Spectrum & i_cSpectrum, double & o_dMinima_WL, double & o_dMinima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMinima_Flux = DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_cSpectrum.size() && (i_dMax_WL < 0 || i_cSpectrum.wl(uiI) < i_dMax_WL); uiI++)
	{
		if (i_cSpectrum.flux(uiI) > 0.0 && i_cSpectrum.flux(uiI) < o_dMinima_Flux)
		{
			o_dMinima_Flux = i_cSpectrum.flux(uiI);
			o_dMinima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Flux_Global_Maximum(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	unsigned int uiStart_Idx = 0;
	if (i_dMin_WL > 0)
	{
		while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
			uiStart_Idx++;
	}
	o_dMaxima_Flux = -DBL_MAX;
	for (unsigned int uiI = uiStart_Idx; uiI < i_cSpectrum.size() && (i_dMax_WL < 0 || i_cSpectrum.wl(uiI) < i_dMax_WL); uiI++)
	{
		if (i_cSpectrum.flux(uiI) > 0.0 && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Blue_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(i_cSpectrum, dMinima_WL, dMinima_Flux,i_dMin_WL,i_dMax_WL);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && i_cSpectrum.wl(uiI) < dMinima_WL; uiI++)
	{
		if ((i_cSpectrum.wl(uiI) > (i_dMin_WL - 250.0) || i_dMin_WL < 0) && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Find_Red_Maxima(const ES::Spectrum & i_cSpectrum, double & o_dMaxima_WL, double & o_dMaxima_Flux, const double &i_dMin_WL, const double & i_dMax_WL)
{
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(i_cSpectrum, dMinima_WL, dMinima_Flux,i_dMin_WL,i_dMax_WL);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size() && (i_cSpectrum.wl(uiI) < i_dMax_WL || i_dMax_WL < 0); uiI++)
	{
		if (i_cSpectrum.wl(uiI) > (dMinima_WL + 250.0) && i_cSpectrum.flux(uiI) > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = i_cSpectrum.flux(uiI);
			o_dMaxima_WL = i_cSpectrum.wl(uiI);
		}
	}
}

void Normalize(double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_Flux)
{	// this routine normalizes to a given flux value
	double	dNormalization = 1.0 / i_dNormalization_Flux;
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		io_lpdFluxes[uiI] *= dNormalization;
	}
}

void Normalize_At_WL(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, const double & i_dNormalization_WL, double & o_dNormalization_Flux)
{	// this routine searches for a given wavelength and normalizes to the flux value at that wavelength
	unsigned int uiNorm_Idx = 0;
	while (uiNorm_Idx < i_uiNum_Points && io_lpdWavelengths[uiNorm_Idx] < i_dNormalization_WL)
		uiNorm_Idx++;
	o_dNormalization_Flux = io_lpdFluxes[uiNorm_Idx];
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dNormalization_Flux);
}

void Normalize_At_Minima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMinima_WL, double & o_dMinima_Flux)
{	// this routine searches for the globlal flux minimum over the given data range, then normalizes the data to that value
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,o_dMinima_WL, o_dMinima_Flux);
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMinima_Flux);
}
void Normalize_At_Blue_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux)
{	// this routine first searches for a minimum, then finds the maximum that is blueward of the minimum.  The peak of this maximum is used as the normalization factor
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,dMinima_WL, dMinima_Flux);
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = 0; uiI < i_uiNum_Points && io_lpdWavelengths[uiI] < dMinima_WL; uiI++)
	{
		if (io_lpdFluxes[uiI] > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = io_lpdFluxes[uiI];
			o_dMaxima_WL = io_lpdWavelengths[uiI];
		}
	}

	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMaxima_Flux);
}
void Normalize_At_Red_Maxima(double * io_lpdWavelengths, double * io_lpdFluxes, unsigned int i_uiNum_Points, double & o_dMaxima_WL, double & o_dMaxima_Flux)
{	// this routine first searches for a minimum, then finds the maximum that is redward of the minimum.  The peak of this maximum is used as the normalization factor
	double	dMinima_Flux;
	double	dMinima_WL;
	Find_Flux_Minimum(io_lpdWavelengths, io_lpdFluxes, i_uiNum_Points,dMinima_WL, dMinima_Flux);
	unsigned int uiStart_Idx = 0;
	while (uiStart_Idx < i_uiNum_Points && io_lpdWavelengths[uiStart_Idx] < dMinima_WL)
		uiStart_Idx++;
	o_dMaxima_Flux = 0;
	for (unsigned int uiI = uiStart_Idx; uiI < i_uiNum_Points; uiI++)
	{
		if (io_lpdFluxes[uiI] > o_dMaxima_Flux)
		{
			o_dMaxima_Flux = io_lpdFluxes[uiI];
			o_dMaxima_WL = io_lpdWavelengths[uiI];
		}
	}
	Normalize(io_lpdFluxes,i_uiNum_Points,1.0 / o_dMaxima_Flux);
}


double Get_Element_Number(const char * i_lpszNuclide)
{
	double dZ;
	char lpszElem[4] = {(char)toupper(i_lpszNuclide[0]),0,0,0};
	if (i_lpszNuclide[1] != 0)
		lpszElem[1] = toupper(i_lpszNuclide[1]);
	if (i_lpszNuclide[2] != 0 && lpszElem[0] == 'U' && lpszElem[1] == 'U' && ((i_lpszNuclide[2] >= 'a' && i_lpszNuclide[2] <= 'z') || (i_lpszNuclide[2] >= 'A' && i_lpszNuclide[2] <= 'Z')))
		lpszElem[2] = toupper(i_lpszNuclide[2]);
	if (strcmp(lpszElem,"H") == 0)
		dZ = 1.0;
	else if (strcmp(lpszElem,"HE") == 0)
		dZ = 2.0;
	else if (strcmp(lpszElem,"LI") == 0)
		dZ = 3.0;
	else if (strcmp(lpszElem,"BE") == 0)
		dZ = 4.0;
	else if (strcmp(lpszElem,"B") == 0)
		dZ = 5.0;
	else if (strcmp(lpszElem,"C") == 0)
		dZ = 6.0;
	else if (strcmp(lpszElem,"N") == 0)
		dZ = 7.0;
	else if (strcmp(lpszElem,"O") == 0)
		dZ = 8.0;
	else if (strcmp(lpszElem,"F") == 0)
		dZ = 9.0;
	else if (strcmp(lpszElem,"NE") == 0)
		dZ = 10.0;
	else if (strcmp(lpszElem,"NA") == 0)
		dZ = 11.0;
	else if (strcmp(lpszElem,"MG") == 0)
		dZ = 12.0;
	else if (strcmp(lpszElem,"AL") == 0)
		dZ = 13.0;
	else if (strcmp(lpszElem,"SI") == 0)
		dZ = 14.0;
	else if (strcmp(lpszElem,"P") == 0)
		dZ = 15.0;
	else if (strcmp(lpszElem,"S") == 0)
		dZ = 16.0;
	else if (strcmp(lpszElem,"CL") == 0)
		dZ = 17.0;
	else if (strcmp(lpszElem,"AR") == 0)
		dZ = 18.0;
	else if (strcmp(lpszElem,"K") == 0)
		dZ = 19.0;
	else if (strcmp(lpszElem,"CA") == 0)
		dZ = 20.0;
	else if (strcmp(lpszElem,"SC") == 0)
		dZ = 21.0;
	else if (strcmp(lpszElem,"TI") == 0)
		dZ = 22.0;
	else if (strcmp(lpszElem,"V") == 0)
		dZ = 23.0;
	else if (strcmp(lpszElem,"CR") == 0)
		dZ = 24.0;
	else if (strcmp(lpszElem,"MN") == 0)
		dZ = 25.0;
	else if (strcmp(lpszElem,"FE") == 0)
		dZ = 26.0;
	else if (strcmp(lpszElem,"CO") == 0)
		dZ = 27.0;
	else if (strcmp(lpszElem,"NI") == 0)
		dZ = 28.0;
	else if (strcmp(lpszElem,"CU") == 0)
		dZ = 29.0;
	else if (strcmp(lpszElem,"ZN") == 0)
		dZ = 30.0;
	else if (strcmp(lpszElem,"GA") == 0)
		dZ = 31.0;
	else if (strcmp(lpszElem,"GE") == 0)
		dZ = 32.0;
	else if (strcmp(lpszElem,"AS") == 0)
		dZ = 33.0;
	else if (strcmp(lpszElem,"SE") == 0)
		dZ = 34.0;
	else if (strcmp(lpszElem,"BR") == 0)
		dZ = 35.0;
	else if (strcmp(lpszElem,"KR") == 0)
		dZ = 36.0;
	else if (strcmp(lpszElem,"RB") == 0)
		dZ = 37.0;
	else if (strcmp(lpszElem,"SR") == 0)
		dZ = 38.0;
	else if (strcmp(lpszElem,"Y") == 0)
		dZ = 39.0;
	else if (strcmp(lpszElem,"ZR") == 0)
		dZ = 40.0;
	else if (strcmp(lpszElem,"NB") == 0)
		dZ = 41.0;
	else if (strcmp(lpszElem,"MO") == 0)
		dZ = 42.0;
	else if (strcmp(lpszElem,"TC") == 0)
		dZ = 43.0;
	else if (strcmp(lpszElem,"RU") == 0)
		dZ = 44.0;
	else if (strcmp(lpszElem,"RH") == 0)
		dZ = 45.0;
	else if (strcmp(lpszElem,"PD") == 0)
		dZ = 46.0;
	else if (strcmp(lpszElem,"AG") == 0)
		dZ = 47.0;
	else if (strcmp(lpszElem,"CD") == 0)
		dZ = 48.0;
	else if (strcmp(lpszElem,"IN") == 0)
		dZ = 49.0;
	else if (strcmp(lpszElem,"SN") == 0)
		dZ = 50.0;
	else if (strcmp(lpszElem,"SB") == 0)
		dZ = 51.0;
	else if (strcmp(lpszElem,"TE") == 0)
		dZ = 52.0;
	else if (strcmp(lpszElem,"I") == 0)
		dZ = 53.0;
	else if (strcmp(lpszElem,"XE") == 0)
		dZ = 54.0;
	else if (strcmp(lpszElem,"CS") == 0)
		dZ = 55.0;
	else if (strcmp(lpszElem,"BA") == 0)
		dZ = 56.0;
	else if (strcmp(lpszElem,"LA") == 0)
		dZ = 57.0;
	else if (strcmp(lpszElem,"CE") == 0)
		dZ = 58.0;
	else if (strcmp(lpszElem,"PR") == 0)
		dZ = 59.0;
	else if (strcmp(lpszElem,"ND") == 0)
		dZ = 60.0;
	else if (strcmp(lpszElem,"PM") == 0)
		dZ = 61.0;
	else if (strcmp(lpszElem,"SM") == 0)
		dZ = 62.0;
	else if (strcmp(lpszElem,"EU") == 0)
		dZ = 63.0;
	else if (strcmp(lpszElem,"GD") == 0)
		dZ = 64.0;
	else if (strcmp(lpszElem,"TB") == 0)
		dZ = 65.0;
	else if (strcmp(lpszElem,"DY") == 0)
		dZ = 66.0;
	else if (strcmp(lpszElem,"HO") == 0)
		dZ = 67.0;
	else if (strcmp(lpszElem,"ER") == 0)
		dZ = 68.0;
	else if (strcmp(lpszElem,"TM") == 0)
		dZ = 69.0;
	else if (strcmp(lpszElem,"YB") == 0)
		dZ = 70.0;
	else if (strcmp(lpszElem,"LU") == 0)
		dZ = 71.0;
	else if (strcmp(lpszElem,"HF") == 0)
		dZ = 72.0;
	else if (strcmp(lpszElem,"TA") == 0)
		dZ = 73.0;
	else if (strcmp(lpszElem,"W") == 0)
		dZ = 74.0;
	else if (strcmp(lpszElem,"RE") == 0)
		dZ = 75.0;
	else if (strcmp(lpszElem,"OS") == 0)
		dZ = 76.0;
	else if (strcmp(lpszElem,"IR") == 0)
		dZ = 77.0;
	else if (strcmp(lpszElem,"PT") == 0)
		dZ = 78.0;
	else if (strcmp(lpszElem,"AU") == 0)
		dZ = 79.0;
	else if (strcmp(lpszElem,"HG") == 0)
		dZ = 80.0;
	else if (strcmp(lpszElem,"TL") == 0)
		dZ = 81.0;
	else if (strcmp(lpszElem,"PB") == 0)
		dZ = 82.0;
	else if (strcmp(lpszElem,"BI") == 0)
		dZ = 83.0;
	else if (strcmp(lpszElem,"PO") == 0)
		dZ = 84.0;
	else if (strcmp(lpszElem,"AT") == 0)
		dZ = 85.0;
	else if (strcmp(lpszElem,"RN") == 0)
		dZ = 86.0;
	else if (strcmp(lpszElem,"FR") == 0)
		dZ = 87.0;
	else if (strcmp(lpszElem,"RA") == 0)
		dZ = 88.0;
	else if (strcmp(lpszElem,"AC") == 0)
		dZ = 89.0;
	else if (strcmp(lpszElem,"TH") == 0)
		dZ = 90.0;
	else if (strcmp(lpszElem,"PA") == 0)
		dZ = 91.0;
	else if (strcmp(lpszElem,"U") == 0)
		dZ = 92.0;
	else if (strcmp(lpszElem,"NP") == 0)
		dZ = 93.0;
	else if (strcmp(lpszElem,"PU") == 0)
		dZ = 94.0;
	else if (strcmp(lpszElem,"AM") == 0)
		dZ = 95.0;
	else if (strcmp(lpszElem,"CM") == 0)
		dZ = 96.0;
	else if (strcmp(lpszElem,"BK") == 0)
		dZ = 97.0;
	else if (strcmp(lpszElem,"CF") == 0)
		dZ = 98.0;
	else if (strcmp(lpszElem,"ES") == 0)
		dZ = 99.0;
	else if (strcmp(lpszElem,"FM") == 0)
		dZ = 100.0;
	else if (strcmp(lpszElem,"MD") == 0)
		dZ = 101.0;
	else if (strcmp(lpszElem,"NO") == 0)
		dZ = 102.0;
	else if (strcmp(lpszElem,"LR") == 0)
		dZ = 103.0;
	else if (strcmp(lpszElem,"RF") == 0)
		dZ = 104.0;
	else if (strcmp(lpszElem,"DB") == 0)
		dZ = 105.0;
	else if (strcmp(lpszElem,"SG") == 0)
		dZ = 106.0;
	else if (strcmp(lpszElem,"BH") == 0)
		dZ = 107.0;
	else if (strcmp(lpszElem,"HS") == 0)
		dZ = 108.0;
	else if (strcmp(lpszElem,"MT") == 0)
		dZ = 109.0;
	else if (strcmp(lpszElem,"DS") == 0)
		dZ = 110.0;
	else if (strcmp(lpszElem,"RG") == 0)
		dZ = 111.0;
	else if (strcmp(lpszElem,"CN") == 0)
		dZ = 112.0;
	else if (strcmp(lpszElem,"UUT") == 0)
		dZ = 113.0;
	else if (strcmp(lpszElem,"FL") == 0)
		dZ = 114.0;
	else if (strcmp(lpszElem,"UUP") == 0)
		dZ = 115.0;
	else if (strcmp(lpszElem,"LV") == 0)
		dZ = 116.0;
	else if (strcmp(lpszElem,"UUS") == 0)
		dZ = 117.0;
	else if (strcmp(lpszElem,"UUO") == 0)
		dZ = 118.0;

	return dZ;
}

void	ABUNDANCE_LIST::Read_Table(ABUNDANCE_TYPE i_eAbundance_Type)
{
	char * lpszData_Path = getenv("LINE_ANALYSIS_DATA_PATH");
	if (lpszData_Path)
	{
		char lpszFilename[256] = {0};
		switch (i_eAbundance_Type)
		{
		case Solar:
			sprintf(lpszFilename,"%s/Solar_Abundance.csv",lpszData_Path);
			break;
		case CO_Rich:
			sprintf(lpszFilename,"%s/CO_Rich_Abundance.csv",lpszData_Path);
			break;
		case Seitenzahl_N100_2013:
			sprintf(lpszFilename,"%s/Seitenzahl_N100_2013.csv",lpszData_Path);
			break;
		case Seitenzahl_Ca_Rich:
			sprintf(lpszFilename,"%s/Seitenzahl_N100_2013_Ca_Rich.csv",lpszData_Path);
			break;
		}
		if (lpszFilename[0] != 0)
			Read_Table(lpszFilename);
	}
	else
		fprintf(stderr,"LINE_ANALYSIS_DATA_PATH is not set.  Ensure this variable is set before \nusing this version of ABUNDANCE_LIST::Read_Table.\n");
}
void	ABUNDANCE_LIST::Read_Table(const char * i_lpszFilename)
{
	XDATASET_ADVANCED	cAbundance_File;
	XDATASET_ADVANCED::DATATYPE	lpeRecord_Descriptor[] = {XDATASET_ADVANCED::XDT_STRING, XDATASET_ADVANCED::XDT_UINT, XDATASET_ADVANCED::XDT_DOUBLE, XDATASET_ADVANCED::XDT_DOUBLE, XDATASET_ADVANCED::XDT_STRING};
	cAbundance_File.DescribeRecord(lpeRecord_Descriptor,5);
	cAbundance_File.ReadDataFile(i_lpszFilename,false,',','\"');
	memset(m_dAbundances,0,sizeof(m_dAbundances));
	if (cAbundance_File.GetNumRows() > 0)
	{
		double	dAbd_Sum = 0.0;
		for (unsigned int uiI = 0; uiI < cAbundance_File.GetNumRows(); uiI++)
		{
			double dZ = Get_Element_Number(cAbundance_File.GetElementString(0,uiI));
			unsigned int uiZ = (unsigned int)(dZ);
//			printf("Load %i %f\n",uiZ,cAbundance_File.GetElementDbl(2,uiI));
			if (uiZ <= 118)
			{
				double	dAbd_Curr = pow(10.0,cAbundance_File.GetElementDbl(2,uiI));
				m_dAbundances[uiZ] += dAbd_Curr;
				m_dUncertainties[uiZ] += pow(10.0,cAbundance_File.GetElementDbl(3,uiI));
				dAbd_Sum += dAbd_Curr;
			}
			else
				fprintf(stderr,"Could not find atomic number for element %s in file %s.\n",cAbundance_File.GetElementString(0,uiI), i_lpszFilename);
		}
		double	dInv_Abd_Total = 1.0 / dAbd_Sum;
		for (unsigned int uiZ = 0; uiZ < 128; uiZ++)
		{
			m_dAbundances[uiZ] *= dInv_Abd_Total;
			m_dUncertainties[uiZ] *= dInv_Abd_Total;
		}
	}
	else
		fprintf(stderr,"Could not open abundance file %s.\n",i_lpszFilename);
}

// Gamezo et al abundance information is group abundance.  Normalize the abundances for the individual groups instead of whoel abundances
void	ABUNDANCE_LIST::Normalize_Groups(void)
{
	// Mg group: F to Al
	double	dSum = 0.0;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
		dSum += m_dAbundances[uiZ];
	double	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Si group: Si to Mn
	dSum = 0.0;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Fe group: Fe to Uuo (really more like Fe to Zn or Ge)
	dSum = 0.0;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}
}


double	Compute_Velocity(const double & i_dObserved_Wavelength, const double & i_dRest_Wavelength)
{
	double	dz = i_dObserved_Wavelength / i_dRest_Wavelength;
	double	dz_sqr = dz * dz;
	return (2.99792458e5 * (dz_sqr - 1.0) / (dz_sqr + 1.0));
}

