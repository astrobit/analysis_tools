#include <iostream>
#include <fstream>
#include <json.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <cmath>
#include <xio.h>
#include <opacity_profile_data.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <specfit.h>
#include <line_routines.h>
#include <cstdio>
#include <iomanip>
#include <ios>
	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <eps_plot.h>


//------------------------------------------------------------------------------
//
//
//
//	Output_Result_Header
//
//
//
//------------------------------------------------------------------------------

void specfit::Output_Result_Header(std::ofstream & io_ofsFile)
{
	io_ofsFile << "MJD, Feature, Sources, Instrument, Model #, PS Velocity (Ejecta), PS Temperature (Ejecta), Log S (Ejecta), Excitation Temp (Ejecta), PS Velocity (Ejecta), PS Temperature (Ejecta), Log S (Shell), Exctitation Temp (Shell)";
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Raw Fit Moment " << uiI;
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Central Fit Moment " << uiI;
	for (unsigned int uiI = 1; uiI <= 6; uiI++)
		io_ofsFile <<  ", Standardized Fit Moment " << uiI;
	io_ofsFile <<  ",	Observed pEW, Observed Vmin (km/s), Fit pEW, Fit Vmin (km/s), Gaussian Fit Amplitude (PVF), Gaussian Fit Width (PVF), Gaussian Fit Central Wavelength (PVF), Gaussian Fit Amplitude (HVF), Gaussian Fit Width (HVF), Gaussian Fit Central Wavelength (HVF), Gaussian Fit pEW (PVF), Gaussian Fit pEW (HVF), Gaussian Fit Velocity (PVF), Gaussian Fit Velocity (HVF)";
	io_ofsFile <<  std::endl;
}

//------------------------------------------------------------------------------
//
//
//
//	Write_Fit
//
//
//
//------------------------------------------------------------------------------

void specfit::Write_Fit(std::ofstream & io_ofsFile, const specfit::fit_result & i_cResult)
{
	double dPlot_WL_Start = -1;
	double dPlot_WL_End = -1;
	int iDef_Precision = io_ofsFile.precision();
	std::ios_base::fmtflags fFlags = io_ofsFile.flags();
	io_ofsFile << std::setprecision(7) << i_cResult.m_dMJD;
	gauss_fit_parameters * lpgfpGF_Param = nullptr;
	switch (i_cResult.m_eFeature)
	{
	case specfit::CaNIR:
		io_ofsFile << ", " << "Ca NIR";
		dPlot_WL_Start = 7000.0;
		dPlot_WL_End = 9000.0;
		lpgfpGF_Param = &g_cgfpCaNIR;
		break;
	case specfit::CaHK:
		io_ofsFile << ", " << "CaHK";
		dPlot_WL_Start = 2000.0;
		dPlot_WL_End = 4000.0;
		lpgfpGF_Param = &g_cgfpCaHK;
		break;
	case specfit::Si6355:
		io_ofsFile << ", " << "Si 6355";
		dPlot_WL_Start = 5000.0;
		dPlot_WL_End = 7000.0;
		lpgfpGF_Param = &g_cgfpSi6355;
		break;
	case specfit::O7773:
		io_ofsFile << ", " << "O 7773";
		dPlot_WL_Start = 6000.0;
		dPlot_WL_End = 8000.0;
		lpgfpGF_Param = &g_cgfpOINIR;
		break;
	}

	io_ofsFile << ", \"" << i_cResult.m_szSource << "\"";
	io_ofsFile << ", " << i_cResult.m_szInstrument;
	io_ofsFile << ", " << i_cResult.m_uiModel;
	io_ofsFile << ", " << std::setprecision(4) << i_cResult.m_cParams[specfit::comp_ejecta].m_dPS_Vel;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dPS_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dLog_S;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_ejecta].m_dExcitation_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dPS_Vel;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dPS_Temp;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dLog_S;
	io_ofsFile << ", " << std::setprecision(4) <<i_cResult.m_cParams[specfit::comp_shell].m_dExcitation_Temp;
	io_ofsFile << std::setprecision(17) << std::scientific;
	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdRaw_Moments.cbegin(); iterI != i_cResult.m_vdRaw_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdCentral_Moments.cbegin(); iterI != i_cResult.m_vdCentral_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	for (std::vector<double>::const_iterator iterI = i_cResult.m_vdStandardized_Moments.cbegin(); iterI != i_cResult.m_vdStandardized_Moments.cend(); iterI++)
	{
		io_ofsFile << ", " << *iterI;
	}
	io_ofsFile << ", " << i_cResult.m_cTarget_Observables.m_fpParameters.m_d_pEW;
	io_ofsFile << ", " << i_cResult.m_cTarget_Observables.m_fpParameters.m_dVmin;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_fpParameters.m_d_pEW;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_fpParameters.m_dVmin;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[0];
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[1];
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[2];
	if (i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit.size() > 3)
	{
		io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[3];
		io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[4];
		io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_xvGaussian_Fit[5];
	}
	else
	{
		io_ofsFile << ", " << "-";
		io_ofsFile << ", " << "-";
		io_ofsFile << ", " << "-";
	}
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_dGaussian_pEW_PVF;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_dGaussian_pEW_HVF;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_dGaussian_V_PVF;
	io_ofsFile << ", " << i_cResult.m_cSynthetic_Observables.m_dGaussian_V_HVF;
	io_ofsFile << std::setprecision(iDef_Precision);
	io_ofsFile.flags(fFlags);
//	io_ofsFile << std::defaultfloat 
	io_ofsFile << std::endl;

	std::ostringstream ossSpectra_Data_File;
	std::string szInst_File_Friendly = i_cResult.m_szInstrument;
	for (std::string::iterator iterI = szInst_File_Friendly.begin(); iterI != szInst_File_Friendly.end(); iterI++)
	{
		if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
			*iterI = '_';
	}
	std::string szSource_File_Friendly = i_cResult.m_szSource;
	for (std::string::iterator iterI = szSource_File_Friendly.begin(); iterI != szSource_File_Friendly.end(); iterI++)
	{
		if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
			*iterI = '_';
	}
	ossSpectra_Data_File << "Results/spectra_" << std::setprecision(7) << i_cResult.m_dMJD << "_" << szInst_File_Friendly << "_source" << szSource_File_Friendly << "_model" << i_cResult.m_uiModel << ".csv";
	
	std::ofstream ofsSpectra;
	ofsSpectra.open(ossSpectra_Data_File.str().c_str());
	if (ofsSpectra.is_open())
	{
		for (unsigned int uiI = 0; uiI < i_cResult.m_vpdSpectrum_Target.size(); uiI++)
		{
			ofsSpectra << std::setprecision(10) << i_cResult.m_vpdSpectrum_Target[uiI].wl();
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Target[uiI].flux();
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic[uiI].flux();
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Continuum[uiI].flux();
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Ejecta_Only[uiI].flux();
			ofsSpectra << "," << std::setprecision(17) << i_cResult.m_vpdSpectrum_Synthetic_Shell_Only[uiI].flux();
			ofsSpectra << std::endl;
		}
		ofsSpectra.close();
	}

	std::ostringstream ossSpectra_Data_Plot_File_Full;
	ossSpectra_Data_Plot_File_Full << "Results/Plots/spectra_full_" << std::setprecision(7) << i_cResult.m_dMJD << "_" << szInst_File_Friendly << "_source" << szSource_File_Friendly << "_model" << i_cResult.m_uiModel << ".eps";
	

	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;
	epsplot::AXIS_PARAMETERS	cX_Axis_Parameters;
	epsplot::AXIS_PARAMETERS	cY_Axis_Parameters;


	cX_Axis_Parameters.Set_Title("Wavelength [A]");
	cX_Axis_Parameters.m_dMajor_Label_Size = 24.0;
	cY_Axis_Parameters.Set_Title("Flux");
	cY_Axis_Parameters.m_dMajor_Label_Size = 24.0;
	cY_Axis_Parameters.m_dLower_Limit = 0.0;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	cPlot.Set_Plot_Filename(ossSpectra_Data_Plot_File_Full.str().c_str());

	epsplot::LINE_PARAMETERS cLine_Parameters;

	std::vector<double> vX;
	std::vector<double> vY_Tgt;
	std::vector<double> vY_Synth;
	std::vector<double> vY_Synth_GF;

	unsigned int uiBlue_Idx = -1;
	unsigned int uiRed_Idx = -1;
	for (unsigned int uiI = 0; uiI < i_cResult.m_vpdSpectrum_Target.size() && (uiBlue_Idx == -1 || uiRed_Idx == -1); uiI++)
	{
		if (i_cResult.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Blue == i_cResult.m_vpdSpectrum_Synthetic[uiI].wl())
			uiBlue_Idx = uiI;
		if (i_cResult.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Red == i_cResult.m_vpdSpectrum_Synthetic[uiI].wl())
			uiRed_Idx = uiI;
	}

	double dWL_Ref = 0.0;
	double dSlope = 0.0;
	double dFlux_Ref = 0.0;
	if (uiBlue_Idx != -1 && uiRed_Idx != -1)
	{
		dSlope = (i_cResult.m_vpdSpectrum_Synthetic[uiBlue_Idx].flux() - i_cResult.m_vpdSpectrum_Synthetic[uiRed_Idx].flux()) / (i_cResult.m_vpdSpectrum_Synthetic[uiBlue_Idx].wl() - i_cResult.m_vpdSpectrum_Synthetic[uiRed_Idx].wl());
		dWL_Ref = i_cResult.m_vpdSpectrum_Synthetic[uiBlue_Idx].wl();
		dFlux_Ref = i_cResult.m_vpdSpectrum_Synthetic[uiBlue_Idx].flux();
	}
	for (unsigned int uiI = 0; uiI < i_cResult.m_vpdSpectrum_Target.size(); uiI++)
	{
		double dWL = i_cResult.m_vpdSpectrum_Target[uiI].wl();
		vX.push_back(dWL);
		vY_Tgt.push_back(i_cResult.m_vpdSpectrum_Target[uiI].flux());
		vY_Synth.push_back(i_cResult.m_vpdSpectrum_Synthetic[uiI].flux());
		XVECTOR vZ = Multi_Gaussian(i_cResult.m_vpdSpectrum_Target[uiI].wl(),i_cResult.m_cTarget_Observables.m_xvGaussian_Fit,lpgfpGF_Param);
		double dY = vZ[0];
		double dY_cont = (dWL - dWL_Ref) * dSlope + dFlux_Ref;
		vY_Synth_GF.push_back((1.0 - dY) * dY_cont);
	}
	cLine_Parameters.m_eColor = epsplot::BLACK;
	cLine_Parameters.m_eStipple = epsplot::SOLID;
	cPlot.Set_Plot_Data(vX, vY_Tgt, cLine_Parameters, uiX_Axis, uiY_Axis);
	cLine_Parameters.m_eColor = epsplot::RED;
	cLine_Parameters.m_eStipple = epsplot::LONG_DASH;
	cPlot.Set_Plot_Data(vX, vY_Synth, cLine_Parameters, uiX_Axis, uiY_Axis);

	cPlot.Plot(cPlot_Parameters);

	cLine_Parameters.m_eColor = epsplot::BLUE;
	cLine_Parameters.m_eStipple = epsplot::LONG_SHORT_DASH;
	cPlot.Set_Plot_Data(vX, vY_Synth_GF, cLine_Parameters, uiX_Axis, uiY_Axis);
	if (dPlot_WL_End != -1 && dPlot_WL_Start != -1)
	{
		std::ostringstream ossSpectra_Data_Plot_File;
		ossSpectra_Data_Plot_File << "Results/Plots/spectra_" << std::setprecision(7) << i_cResult.m_dMJD << "_" << szInst_File_Friendly << "_source" << szSource_File_Friendly << "_model" << i_cResult.m_uiModel << ".eps";
	

		//cX_Axis_Parameters.Set_Title("Wavelength [A]");
		//cX_Axis_Parameters.m_dMajor_Label_Size = 24.0;
		cX_Axis_Parameters.m_dUpper_Limit = dPlot_WL_End;
		cX_Axis_Parameters.m_dLower_Limit = dPlot_WL_Start;

		cPlot.Modify_X_Axis_Parameters(uiX_Axis, cX_Axis_Parameters);


		cPlot.Set_Plot_Filename(ossSpectra_Data_Plot_File.str().c_str());

		cPlot.Plot(cPlot_Parameters);
	}

	
}
