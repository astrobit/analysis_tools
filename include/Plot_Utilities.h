#pragma once

#include <xio.h>
#include "ES_Synow.hh"
#include <float.h>
#include <eps_plot.h>

// Create_Plot does basic initialization for white background with black axes
//plstream * Create_Plot(const char * lpszFilename, const double &i_dXmin, const double & i_dXmax, const double & i_dYmin, const double & i_dYmax, const char * i_lpszX_label, const char * i_lpszY_label, const char * i_lpszTitle );


// Plot columns 0 and 1 from a dataset
void Plot(const XDATASET & i_cDataset, const char * i_lpszSourceFile, const char * i_lpszFilename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, bool bLinear_Scale, bool bSmart_Range);
// Plot a single spectrum, with no error bars
void PlotSingle(ES::Spectrum & i_cSpectrum, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, bool bLinear_Scale);
// Plot a series of spectra, with a gap in between and the day labels on the right side of the plot
void PlotSeries(ES::Spectrum * i_lpcSpectra, const double * i_lpdDay, epsplot::COLOR * i_lpColors, unsigned int i_uiNum_Spectra, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, bool bLinear_Scale, const char * i_lpszTitle, const double &i_dNormalization_WL = -1.0, bool i_bPlot_Minima_Curve = false, bool i_bY_Axis_Velocity = false, const double & i_dVelocity_WL_Reference = -1.0, ES::Spectrum * i_lpcReference_Spectra = NULL, const double * i_lpdReference_Day = NULL, unsigned int i_uiNum_Reference_Spectra = 0, const double & i_dMin_WL_For_Vel_Ev = -1.0, const double & i_dMax_WL_For_Vel_Ev = -1.0);

// Plot a target spectrum in black with error bars in grey, and a fit spectrum in red on top of it
void Plot(ES::Spectrum & i_cTarget, ES::Spectrum & i_cFit, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, const double & i_dNormalization_WL = -1.0, bool bLinear_Scale = false, bool bSmart_Range = true);


// Plot a spectrum with the given wavelengths and log intensities.
void Plot_Spectrum(const char * i_lpszFilename,const double & i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, unsigned int i_uiNum_Points, const double * i_lpdWavelength, const double * i_lpdLog_Intensity);
void Plot_Spectrum(const char * i_lpszFilename,const double & i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, const ES::Spectrum & i_cTarget);


class EPS_SPECTRA_PLOT_DATA
{
public:
	bool	m_bLog_Flux; // if true, y axis will be log flux; if false, y axis will be flux
	char	* m_lpszTitle;
	char 	* m_lpszFilename;
	ES::Spectrum ** m_lpcSpectra;
	unsigned int m_uiNum_Days;
	unsigned int m_uiNum_Plots;
	double *	m_lpdDays;
	double	m_dNormalization_WL; // if > 0, all fluxes will be normalized such that flux(norm WL) = 1.0
	double	m_dVelocity_Reference_WL; // if > 0, plot will be in velocity instead of wavelength space

	double	m_dMin_WL;
	double	m_dMax_WL;
	double	m_dMin_Flux; // if > 0, lower limit the flux for any particular day to this value 
	double	m_dMax_Flux; // if > 0, upper limit the flux for any particular day to this value
	double	m_dMin_Log_Flux; // if > 0, lower limit the flux for any particular day to this value 
	double	m_dMax_Log_Flux; // if > 0, upper limit the flux for any particular day to this value 

	bool	m_bPlot_Minima_Curve;
	double	m_dMin_WL_For_Minima;
	double	m_dMax_WL_For_Minima;

	bool	m_bPlot_EW_Curve;
	double	m_dMin_WL_For_EW;
	double	m_dMax_WL_For_EW;

	double			m_dLine_Width_Graph;
	double		m_dLine_Width_Axis;

	epsplot::color_triplet	m_cSpectrum_Color;
	epsplot::color_triplet	m_cAxis_Color;


	EPS_SPECTRA_PLOT_DATA(void)
	{
		m_bLog_Flux = false;
		m_lpszTitle= NULL;
		m_lpszFilename = NULL;
		m_lpcSpectra = NULL;
		m_uiNum_Days = 0;
		m_uiNum_Plots = 0;
		m_lpdDays = NULL;
		m_dNormalization_WL = -1.0;
		m_dVelocity_Reference_WL = -1.0;
		m_dMin_WL = -1.0;
		m_dMax_WL = -1.0;
		m_dMin_Flux = -1.0;
		m_dMax_Flux = -1.0;
		m_dMin_Log_Flux = -DBL_MAX;
		m_dMax_Log_Flux = DBL_MAX;

		m_bPlot_Minima_Curve = false;
		m_dMin_WL_For_Minima = -1;
		m_dMax_WL_For_Minima = -1;

		m_bPlot_EW_Curve = false;
		m_dMin_WL_For_EW = -1;
		m_dMax_WL_For_EW = -1;

		m_dLine_Width_Graph = 1.0;
		m_dLine_Width_Axis = 1.5;
		m_cSpectrum_Color.m_dRed = m_cSpectrum_Color.m_dGreen = m_cSpectrum_Color.m_dBlue = 0.0;
		m_cAxis_Color = m_cSpectrum_Color;
	}
	~EPS_SPECTRA_PLOT_DATA(void)
	{
	}
};

unsigned int	Set_Plot_Data(epsplot::data & i_cPlot, const ES::Spectrum & i_cSpectrum, const double & i_dMin_WL, const double & i_dMax_WL, const double & i_dNormalization_WL, const double & i_dVelocity_Reference_WL, const epsplot::line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type);


void Plot_EPS_Grid(const epsplot::page_parameters & i_cGrid, const EPS_SPECTRA_PLOT_DATA & i_cData);

