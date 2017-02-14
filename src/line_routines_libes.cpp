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


////////////////////////////////////////////////////////////////////////////////
//
// Routines related to generating a spectrum using libes
//
////////////////////////////////////////////////////////////////////////////////




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


//
//
// Get_Spectra_Data: extract ES (synow) data to floating point arrays 
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

