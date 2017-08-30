#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xio.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
//#include <plplot/plplot.h>
//#include <plplot/plstream.h>
#include <Plot_Utilities.h>
#include <time.h>
#include <unistd.h>
#include <line_routines.h>
#include <eps_plot.h>

/*
plstream * Create_Plot(const char * lpszFilename, const double &i_dXmin, const double & i_dXmax, const double & i_dYmin, const double & i_dYmax, const char * i_lpszX_label, const char * i_lpszY_label, const char * i_lpszTitle )
{
	plstream * lpcPLStream = new plstream;
	if (lpcPLStream)
	{
		lpcPLStream->scolbg(255,255,255); // set background color to white
		lpcPLStream->sdev("epscairo"); // select encapsulated postscript with color
		lpcPLStream->sfnam(lpszFilename); // set output filename
		lpcPLStream->sori(1);// supposed to be portrait, but comes out landscape	


		lpcPLStream->init();
		lpcPLStream->scol0(15,0,0,0); // since black was redefined to white, redefine white to black
		lpcPLStream->col0(15); // use black for axes
		lpcPLStream->env(i_dXmin,i_dXmax,i_dYmin,i_dYmax,0.0,0.0);

		lpcPLStream->lab(i_lpszX_label, i_lpszY_label, i_lpszTitle);
	}
	return lpcPLStream;
}*/

void Plot(const XDATASET & i_cDataset, const char * i_lpszSourceFile, const char * i_lpszFilename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, bool bLinear_Scale, bool bSmart_Range)
{
	double	dMin_Flux = 1e30;
	double	dMax_Flux = -1e30;
	for (unsigned int uiI = 0; uiI < i_cDataset.GetNumElements(); uiI++)
	{
		double dWL = i_cDataset.GetElement(0,uiI);
		double	dFlux = i_cDataset.GetElement(1,uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			if (dFlux < dMin_Flux)
				dMin_Flux = dFlux;
			if (dFlux > dMax_Flux)
				dMax_Flux = dFlux;
		}
	}
	if (bSmart_Range)
	{
		unsigned int	* lpuiBins = new unsigned int [256];
		double	dBin_Range = (dMax_Flux - dMin_Flux) / 255.0;
		unsigned int uiCount = 0;
		memset(lpuiBins,0,sizeof(unsigned int) * 256);
		for (unsigned int uiI = 0; uiI < i_cDataset.GetNumElements(); uiI++)
		{
			double dWL = i_cDataset.GetElement(0,uiI);
			if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
			{

				double	dFlux = i_cDataset.GetElement(1,uiI);
				unsigned int uiBin = (unsigned int)((dFlux - dMin_Flux) / dBin_Range);
				if (uiBin > 255)
					uiBin = 0;
				lpuiBins[uiBin]++;
				uiCount++;
			}
		}
		// find mode
		unsigned int uiPeak = 0;
		unsigned int uiPeak_Idx;
		for (unsigned int uiI = 0; uiI < 256; uiI++)
		{
			if (lpuiBins[uiI] > uiPeak)
			{
				uiPeak = lpuiBins[uiI];
				uiPeak_Idx = uiI;
			}
		}
		unsigned int uiThreshold = (uiCount * 0.01); // 1%
		if (uiThreshold == 0)
			uiThreshold = 1; // just to be practical
		unsigned int uiMin, uiMax;
		uiMin = uiPeak_Idx;
		uiMax = uiPeak_Idx;
		while (uiMin != 0 && lpuiBins[uiMin] > uiThreshold)
			uiMin--;
		while (uiMax < 256 && lpuiBins[uiMax] > uiThreshold)
			uiMax++;

		if (uiMin >= 2)
		{
			dMin_Flux += (uiMin - 2) * dBin_Range;
		}
		if (uiMax >= 254)
		{
			dMax_Flux -= (254 - uiMax) * dBin_Range;
		}
		delete [] lpuiBins;
	}

	epsplot::page_parameters	cPlot_Parameters;
	epsplot::data cPlot;
	epsplot::line_parameters	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, true, i_dMin_WL, true, i_dMax_WL);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( bLinear_Scale ? "Intensity" : "Log Intensity", false, false, true, dMin_Flux,true, dMax_Flux);

	if (i_lpszTitle)
		cPlot.Set_Plot_Title(i_lpszTitle,18.0);
	cPlot.Set_Plot_Filename(i_lpszFilename);

	if (i_cDataset.GetNumColumns() > 3)
	{
		cLine_Parameters.m_eColor = epsplot::GREY_50;
		cPlot.Set_Plot_Data(i_cDataset.GetElementArray(0), i_cDataset.GetElementArray(3), i_cDataset.GetNumElements(), cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
		cPlot.Set_Plot_Data(i_cDataset.GetElementArray(0), i_cDataset.GetElementArray(4), i_cDataset.GetNumElements(), cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	}
	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Set_Plot_Data(i_cDataset.GetElementArray(0), i_cDataset.GetElementArray(1), i_cDataset.GetNumElements(), cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
}

void PlotSingle(ES::Spectrum & i_cSpectrum, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, bool bLinear_Scale)
{
	double	dMin_Flux = 1e30;
	double	dMax_Flux = -1e30;
	unsigned int uiFit_Element_Count = 0;
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		double dWL = i_cSpectrum.wl(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiFit_Element_Count++;
		}
	}
	double	*lpdFit_WL = new double[uiFit_Element_Count];
	double	*lpdFit_Flux = new double[uiFit_Element_Count];
	uiFit_Element_Count = 0; // use it as an index
	for (unsigned int uiI = 0; uiI < i_cSpectrum.size(); uiI++)
	{
		double dWL = i_cSpectrum.wl(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			lpdFit_WL[uiFit_Element_Count] = dWL;
			lpdFit_Flux[uiFit_Element_Count] = log10(i_cSpectrum.flux(uiI));

			uiFit_Element_Count++;
		}
	}

	for (unsigned int uiI = 0; uiI < uiFit_Element_Count; uiI++)
	{
		if (lpdFit_Flux[uiI] < dMin_Flux)
			dMin_Flux = lpdFit_Flux[uiI];
		if (lpdFit_Flux[uiI] > dMax_Flux)
			dMax_Flux = lpdFit_Flux[uiI];
	}

	epsplot::page_parameters	cPlot_Parameters;
	epsplot::data cPlot;
	epsplot::line_parameters	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, true, i_dMin_WL, true, i_dMax_WL);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( bLinear_Scale ? "Intensity" : "Log Intensity", false, false, true, dMin_Flux,true, dMax_Flux);

	if (i_lpszTitle)
		cPlot.Set_Plot_Title(i_lpszTitle,18.0);
	cPlot.Set_Plot_Filename(i_lpszOutput_Filename);

	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiFit_Element_Count, cLine_Parameters, uiX_Axis_ID,uiY_Axis_ID);


	delete [] lpdFit_WL;
	delete [] lpdFit_Flux;
}

void PlotSeries(ES::Spectrum * i_lpcSpectra, const double * i_lpdDay, epsplot::COLOR * i_lpColors, unsigned int i_uiNum_Spectra, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, bool bLinear_Scale, const char * i_lpszTitle, const double &i_dNormalization_WL, bool i_bPlot_Minima_Curve, bool i_bY_Axis_Velocity, const double & i_dVelocity_WL_Reference, ES::Spectrum * i_lpcReference_Spectra, const double * i_lpdReference_Day, unsigned int i_uiNum_Reference_Spectra, const double & i_dMin_WL_For_Vel_Ev, const double & i_dMax_WL_For_Vel_Ev)
{
	double	dMin_X = i_dMin_WL;
	double	dMax_X = i_dMax_WL;
	double	dMin_Flux = 1e30;
	double	dMax_Flux = -1e30;
	unsigned int * uiElement_Count = new unsigned int [i_uiNum_Spectra];
	double	dNormalization_WL = i_dNormalization_WL;

	double ** lpdSpectra_WL = new double *[i_uiNum_Spectra];
	double ** lpdSpectra_Flux = new double *[i_uiNum_Spectra];
	double	* dNormalization = new double[i_uiNum_Spectra];
	memset(dNormalization,0,sizeof(double) * i_uiNum_Spectra);
	double	* lpdMinima_WL = NULL;
	double	* lpdMinima_Flux = NULL;

	double	* lpdReference_Minima_WL = NULL;
	double	* lpdReference_Minima_Flux = NULL;
	double ** lpdReference_Spectra_WL = NULL;
	double ** lpdReference_Spectra_Flux = NULL;
	double	* dReference_Normalization = NULL;
	unsigned int * uiReference_Element_Count = NULL;
	if (i_uiNum_Reference_Spectra > 0)
	{
//		printf("Alloc ref\n");
		uiReference_Element_Count = new unsigned int [i_uiNum_Reference_Spectra];
		lpdReference_Spectra_WL = new double *[i_uiNum_Reference_Spectra];
		lpdReference_Spectra_Flux = new double *[i_uiNum_Reference_Spectra];
		dReference_Normalization = new double[i_uiNum_Reference_Spectra];
		memset(dReference_Normalization,0,sizeof(double) * i_uiNum_Reference_Spectra);
	}
	if (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0)
	{
		dMin_X = (i_dMin_WL / i_dVelocity_WL_Reference - 1.0) * 300.0;
		dMax_X = (i_dMax_WL / i_dVelocity_WL_Reference - 1.0) * 300.0;
	}

	if (i_bPlot_Minima_Curve)
	{
		lpdMinima_WL = new double[i_uiNum_Spectra];
		lpdMinima_Flux = new double[i_uiNum_Spectra];
		if (i_uiNum_Reference_Spectra > 0)
		{
			lpdReference_Minima_WL = new double[i_uiNum_Reference_Spectra];
			lpdReference_Minima_Flux = new double[i_uiNum_Reference_Spectra];
		}
	}
	if (i_uiNum_Reference_Spectra > 0 && i_dNormalization_WL <= 0.0)
	{
		dNormalization_WL = (i_dMax_WL + i_dMin_WL) * 0.5;
	}

	if (dNormalization_WL > 0.0)
	{
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
		{
			for (unsigned int uiI = 0; uiI < i_lpcSpectra[uiJ].size(); uiI++)
			{
				if (dNormalization[uiJ] == 0.0 && i_lpcSpectra[uiJ].wl(uiI) > dNormalization_WL)
				{
					dNormalization[uiJ] = i_lpcSpectra[uiJ].flux(uiI);
				}
			}
		}
		//printf("Normalize ref\n");
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
		{
			for (unsigned int uiI = 0; uiI < i_lpcReference_Spectra[uiJ].size(); uiI++)
			{
				if (dReference_Normalization[uiJ] == 0.0 && i_lpcReference_Spectra[uiJ].wl(uiI) > dNormalization_WL)
				{
					dReference_Normalization[uiJ] = i_lpcReference_Spectra[uiJ].flux(uiI);
				}
			}
		}
	}
	else
	{
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
			dNormalization[uiJ] = 1.0;
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
			dReference_Normalization[uiJ] = 1.0;
	}
	//printf("Ref N = %.2e\n",dReference_Normalization[0]);
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
	{
		uiElement_Count[uiJ] = 0;
		for (unsigned int uiI = 0; uiI < i_lpcSpectra[uiJ].size(); uiI++)
		{
			double dWL = i_lpcSpectra[uiJ].wl(uiI);
			if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
			{
				uiElement_Count[uiJ]++;
			}
		}
		lpdSpectra_WL[uiJ] = new double[uiElement_Count[uiJ]];
		lpdSpectra_Flux[uiJ] = new double[uiElement_Count[uiJ]];
		uiElement_Count[uiJ] = 0;
		for (unsigned int uiI = 0; uiI < i_lpcSpectra[uiJ].size(); uiI++)
		{
			double dWL = i_lpcSpectra[uiJ].wl(uiI);
			double	dFlux = i_lpcSpectra[uiJ].flux(uiI) / dNormalization[uiJ];
			if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
			{
				if (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0)
					lpdSpectra_WL[uiJ][uiElement_Count[uiJ]] = (dWL / i_dVelocity_WL_Reference - 1.0) * 300.0;
				else
					lpdSpectra_WL[uiJ][uiElement_Count[uiJ]] = dWL;
				lpdSpectra_Flux[uiJ][uiElement_Count[uiJ]] = dFlux;
				if (dFlux >= 0.0 && dFlux < dMin_Flux)
					dMin_Flux = dFlux;
				if (dFlux > dMax_Flux)
					dMax_Flux = dFlux;					

				uiElement_Count[uiJ]++;
			}
		}
	}
		//printf("Normalize ref\n");
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
	{
		uiReference_Element_Count[uiJ] = 0;
		for (unsigned int uiI = 0; uiI < i_lpcReference_Spectra[uiJ].size(); uiI++)
		{
			double dWL = i_lpcReference_Spectra[uiJ].wl(uiI);
			if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
			{
				uiReference_Element_Count[uiJ]++;
			}
		}
		lpdReference_Spectra_WL[uiJ] = new double[uiReference_Element_Count[uiJ]];
		lpdReference_Spectra_Flux[uiJ] = new double[uiReference_Element_Count[uiJ]];
		uiReference_Element_Count[uiJ] = 0;
		for (unsigned int uiI = 0; uiI < i_lpcReference_Spectra[uiJ].size(); uiI++)
		{
			double dWL = i_lpcReference_Spectra[uiJ].wl(uiI);
			double	dFlux = i_lpcReference_Spectra[uiJ].flux(uiI) / dReference_Normalization[uiJ];
			if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
			{
				if (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0)
					lpdReference_Spectra_WL[uiJ][uiReference_Element_Count[uiJ]] = (dWL / i_dVelocity_WL_Reference - 1.0) * 300.0;
				else
					lpdReference_Spectra_WL[uiJ][uiReference_Element_Count[uiJ]] = dWL;
				lpdReference_Spectra_Flux[uiJ][uiReference_Element_Count[uiJ]] = dFlux;
				uiReference_Element_Count[uiJ]++;
			}
		}
	}

	double dRange;
	if (!bLinear_Scale)
		dRange = log10(dMax_Flux) - log10(dMin_Flux);
	else
		dRange = dMax_Flux - dMin_Flux;

	dMin_Flux = DBL_MAX;
	dMax_Flux = -DBL_MAX;					

	double	dDay_Min = DBL_MAX;
	double	dDay_Max = -DBL_MAX;
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
	{
		if (i_lpdDay[uiJ] < dDay_Min)
			dDay_Min = i_lpdDay[uiJ];
		if (i_lpdDay[uiJ] > dDay_Max)
			dDay_Max = i_lpdDay[uiJ];
	}


	for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
	{
		for (unsigned int uiI = 0; uiI < uiElement_Count[uiJ]; uiI++)
		{
			double dFlux = lpdSpectra_Flux[uiJ][uiI];
			if (dFlux > 0.0)
			{
				double	dGraph_Flux;
				if (bLinear_Scale)
					dGraph_Flux = dFlux + 0.5 * dRange * (dDay_Max - i_lpdDay[uiJ]);
				else
					dGraph_Flux = log10(dFlux) + 0.5 * dRange * (dDay_Max - i_lpdDay[uiJ]);

				if (dGraph_Flux < dMin_Flux)
					dMin_Flux = dGraph_Flux;
				if (dGraph_Flux > dMax_Flux)
					dMax_Flux = dGraph_Flux;
			}
		}
	}
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
	{
		for (unsigned int uiI = 0; uiI < uiElement_Count[uiJ]; uiI++)
		{
			double dFlux = lpdSpectra_Flux[uiJ][uiI];
			if (dFlux < 0.0)
				dFlux = dMin_Flux;
			if (bLinear_Scale)
				lpdSpectra_Flux[uiJ][uiI] = dFlux + 0.5 * dRange * (dDay_Max - i_lpdDay[uiJ]);
			else
				lpdSpectra_Flux[uiJ][uiI] = log10(dFlux) + 0.5 * dRange * (dDay_Max - i_lpdDay[uiJ]);
		}
	}
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
	{
		for (unsigned int uiI = 0; uiI < uiReference_Element_Count[uiJ]; uiI++)
		{
			double dFlux = lpdReference_Spectra_Flux[uiJ][uiI];
			if (dFlux <= 0.0)
				dFlux = dMin_Flux;
			if (bLinear_Scale)
				lpdReference_Spectra_Flux[uiJ][uiI] = dFlux + 0.5 * dRange * (dDay_Max - i_lpdReference_Day[uiJ]);
			else
				lpdReference_Spectra_Flux[uiJ][uiI] = log10(dFlux) + 0.5 * dRange * (dDay_Max - i_lpdReference_Day[uiJ]);

		}
	}
	if (i_bPlot_Minima_Curve)
	{
		double	dWL_Min = i_dMin_WL_For_Vel_Ev, dWL_Max = i_dMax_WL_For_Vel_Ev;
		if (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0)
		{
			dWL_Min = (i_dMin_WL_For_Vel_Ev / i_dVelocity_WL_Reference - 1.0) * 300.0;
			dWL_Max = (i_dMax_WL_For_Vel_Ev / i_dVelocity_WL_Reference - 1.0) * 300.0;
		}
		for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
		{
			lpdMinima_WL[uiJ] = 0.0;
			lpdMinima_Flux[uiJ] = DBL_MAX;
			for (unsigned int uiI = 0; uiI < uiElement_Count[uiJ]; uiI++)
			{
				if (i_dMax_WL_For_Vel_Ev <= 0.0 || i_dMin_WL_For_Vel_Ev <= 0.0 || (lpdSpectra_WL[uiJ][uiI] <= dWL_Max && lpdSpectra_WL[uiJ][uiI] >= dWL_Min))
				{
					if (lpdSpectra_Flux[uiJ][uiI] < lpdMinima_Flux[uiJ])
					{
						lpdMinima_Flux[uiJ] = lpdSpectra_Flux[uiJ][uiI];
						lpdMinima_WL[uiJ] = lpdSpectra_WL[uiJ][uiI];
					}
				}
			}
		}

		for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
		{
			lpdReference_Minima_WL[uiJ] = 0.0;
			lpdReference_Minima_Flux[uiJ] = DBL_MAX;
			for (unsigned int uiI = 0; uiI < uiReference_Element_Count[uiJ]; uiI++)
			{
				if (i_dMax_WL_For_Vel_Ev <= 0.0 || i_dMin_WL_For_Vel_Ev <= 0.0 || (lpdReference_Spectra_WL[uiJ][uiI] <= dWL_Max && lpdReference_Spectra_WL[uiJ][uiI] >= dWL_Min))
				{
					if (lpdReference_Spectra_Flux[uiJ][uiI] < lpdReference_Minima_Flux[uiJ])
					{
						lpdReference_Minima_Flux[uiJ] = lpdReference_Spectra_Flux[uiJ][uiI];
						lpdReference_Minima_WL[uiJ] = lpdReference_Spectra_WL[uiJ][uiI];
					}
				}
			}
		}
	}
	dMin_Flux -= dRange * 0.5;
	dMax_Flux += dRange * 0.5; // give a little extra space on top



	epsplot::page_parameters	cPlot_Parameters;
	epsplot::data cPlot;
	epsplot::line_parameters	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0) ? "Velocity [kkm/s] (Wavelength [Angstroms])" : "Wavelength [Angstroms]", false, false, true, dMin_X, true, dMax_X);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( bLinear_Scale ? "Flux + Const" : "Log Flux + Const", false, false, true, dMin_Flux,true, dMax_Flux);

	if (i_lpszTitle)
		cPlot.Set_Plot_Title(i_lpszTitle,18.0);
//	printf("Plotting to file %s\n",i_lpszOutput_Filename);
	cPlot.Set_Plot_Filename(i_lpszOutput_Filename);

	cLine_Parameters.m_eColor = epsplot::BLUE;
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Reference_Spectra; uiJ++)
	{
		char lpszString[32];
		sprintf(lpszString,"Day %.0f",i_lpdReference_Day[uiJ]);
		int iDay = i_lpdReference_Day[uiJ] < 0 ? (int)(i_lpdReference_Day[uiJ] - 0.5) : (int)(i_lpdReference_Day[uiJ] + 0.5);
	//@@TODO: implement text
//		lpcPLStream->ptex(dMax_X * 0.925 + dMin_X * 0.075,dMin_Flux + 0.5 * dRange * (dDay_Max - iDay + 1.0),1.0,0.0,0.5,lpszString);
		cPlot.Set_Plot_Data(lpdReference_Spectra_WL[uiJ], lpdReference_Spectra_Flux[uiJ], uiReference_Element_Count[uiJ], cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	}

	cLine_Parameters.m_eColor = epsplot::BLACK;
	for (unsigned int uiJ = 0; uiJ < i_uiNum_Spectra; uiJ++)
	{
		char lpszString[32];
		sprintf(lpszString,"Day %.0f",i_lpdDay[uiJ]);
		int iDay = i_lpdDay[uiJ] < 0 ? (int)(i_lpdDay[uiJ] - 0.5) : (int)(i_lpdDay[uiJ] + 0.5);
	//@@TODO: implement text
//		lpcPLStream->ptex(dMax_X * 0.925 + dMin_X * 0.075,dMin_Flux + 0.5 * dRange * (dDay_Max - iDay + 1.0),1.0,0.0,0.5,lpszString);
		if (i_lpColors)
			cLine_Parameters.m_eColor = i_lpColors[uiJ];
		cPlot.Set_Plot_Data(lpdSpectra_WL[uiJ], lpdSpectra_Flux[uiJ], uiElement_Count[uiJ], cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	}
	cLine_Parameters.m_eColor = epsplot::BLACK;
	if (i_bPlot_Minima_Curve)
	{
		cLine_Parameters.m_eColor = epsplot::RED;
		cLine_Parameters.m_eStipple = epsplot::SHORT_DASH;
		cPlot.Set_Plot_Data(lpdMinima_WL,lpdMinima_Flux, i_uiNum_Spectra, cLine_Parameters, 0,0);
	}
	if (i_bPlot_Minima_Curve && i_uiNum_Reference_Spectra > 0)
	{
		cLine_Parameters.m_eColor = epsplot::BLUE;
		cLine_Parameters.m_eStipple = epsplot::SHORT_DASH;
		cPlot.Set_Plot_Data(lpdReference_Minima_WL,lpdReference_Minima_Flux, i_uiNum_Reference_Spectra, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	}
	if (i_bY_Axis_Velocity && i_dVelocity_WL_Reference > 0.0)
	{
		char lpszString[8];
		// plot wavelengths as text
		cLine_Parameters.m_eColor = epsplot::BLACK;
		for (int iVel = 0; iVel > dMin_X; iVel -= 20)
		{
			double	dWL = floor((iVel / 300.0 + 1.0) * i_dVelocity_WL_Reference / 10.0) * 10.0;
			
			sprintf(lpszString,"%.0f",dWL);
	//@@TODO: implement text
//			lpcPLStream->ptex((PLFLT)iVel,dMin_Flux + 0.25 * dRange,1.0,0.0,0.5,lpszString);
		}
	}
	cPlot.Plot(cPlot_Parameters);

	for (unsigned int uiI = 0; uiI < i_uiNum_Spectra; uiI++)
	{
		delete [] lpdSpectra_WL[uiI];
		delete [] lpdSpectra_Flux[uiI];
	}
	delete [] lpdSpectra_WL;
	delete [] lpdSpectra_Flux;

	if (lpdReference_Spectra_WL)
	{
		for (unsigned int uiI = 0; uiI < i_uiNum_Reference_Spectra; uiI++)
		{
			delete [] lpdReference_Spectra_WL[uiI];
			delete [] lpdReference_Spectra_Flux[uiI];
		}
		delete [] lpdReference_Spectra_WL;
		delete [] lpdReference_Spectra_Flux;
	}
	if (lpdReference_Minima_WL)
		delete [] lpdReference_Minima_WL;
	if (lpdReference_Minima_Flux)
		delete [] lpdReference_Minima_Flux;
	if (lpdMinima_WL)
		delete [] lpdMinima_WL;
	if (lpdMinima_Flux)
		delete [] lpdMinima_Flux;
}

void Plot(ES::Spectrum & i_cTarget, ES::Spectrum & i_cFit, const char * i_lpszOutput_Filename, const double &i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, const double & i_dNormalization_WL, bool bLinear_Scale, bool bSmart_Range)
{
	double	dMin_Flux = 1e30;
	double	dMax_Flux = -1e30;
	unsigned int uiTarget_Element_Count = 0;
	unsigned int uiFit_Element_Count = 0;
	bool	bError_Invalid = true;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		double	dFlux = i_cTarget.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiTarget_Element_Count++;
		}
		bError_Invalid &= (i_cTarget.flux_error(uiI) == 0.0 || i_cTarget.flux_error(uiI) == 1.0);
	}
	for (unsigned int uiI = 0; uiI < i_cFit.size(); uiI++)
	{
		double dWL = i_cFit.wl(uiI);
		double	dFlux = i_cFit.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiFit_Element_Count++;
		}
	}
	double	*lpdTarget_WL = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux_Error_Upper = new double[uiTarget_Element_Count];
	double	*lpdTarget_Flux_Error_Lower = new double[uiTarget_Element_Count];
	double	*lpdFit_WL = new double[uiFit_Element_Count];
	double	*lpdFit_Flux = new double[uiFit_Element_Count];
	uiTarget_Element_Count = 0; // use it as an index
	double	dTarget_Normalization = 0.0;
	for (unsigned int uiI = 0; uiI < i_cTarget.size() && dTarget_Normalization == 0.0; uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		double	dFlux = i_cTarget.flux(uiI);
		double	dFlux_Error = i_cTarget.flux_error(uiI);
		if (i_dNormalization_WL <= i_cTarget.wl(i_cTarget.size() - 1) && i_dNormalization_WL != -1.0)
		{
			if (dWL >= i_dNormalization_WL && i_dNormalization_WL != -1.0 && dTarget_Normalization == 0.0 && dFlux > 0.0)
				dTarget_Normalization = dFlux;
		}
		else if (i_dMin_WL <= dWL && dWL <= i_dMax_WL && dTarget_Normalization == 0.0 && dFlux > 0.0)
			dTarget_Normalization = dFlux;
	}

	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		double	dFlux = i_cTarget.flux(uiI);
		double	dFlux_Error = i_cTarget.flux_error(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			lpdTarget_WL[uiTarget_Element_Count] = dWL;
			if (bLinear_Scale)
			{
				lpdTarget_Flux[uiTarget_Element_Count] = dFlux / dTarget_Normalization;
				if (!bError_Invalid)
				{
					lpdTarget_Flux_Error_Upper[uiTarget_Element_Count] = (dFlux + dFlux_Error) / dTarget_Normalization;
					double dFlux_Error_Lower = (dFlux - dFlux_Error) / dTarget_Normalization;
					lpdTarget_Flux_Error_Lower[uiTarget_Element_Count] = (dFlux - dFlux_Error) / dTarget_Normalization;
				}
			}
			else
			{
				lpdTarget_Flux[uiTarget_Element_Count] = log10(dFlux / dTarget_Normalization);
				if (!bError_Invalid)
				{
					lpdTarget_Flux_Error_Upper[uiTarget_Element_Count] = log10((dFlux + dFlux_Error) / dTarget_Normalization);
					double dFlux_Error_Lower = (dFlux - dFlux_Error) / dTarget_Normalization;
					if (dFlux_Error_Lower > 1e-10)
						lpdTarget_Flux_Error_Lower[uiTarget_Element_Count] = log10(dFlux_Error_Lower);
					else
						lpdTarget_Flux_Error_Lower[uiTarget_Element_Count] = -10.0;
				}
			}

			uiTarget_Element_Count++;
		}
	}
	uiFit_Element_Count = 0; // use it as an index
	double	dFit_Normalization = 0.0;
	for (unsigned int uiI = 0; uiI < i_cFit.size(); uiI++)
	{
		double dWL = i_cFit.wl(uiI);
		double	dFlux = i_cFit.flux(uiI);
		double	dFlux_Error = i_cFit.flux_error(uiI);
		if (i_dNormalization_WL <= i_cFit.wl(i_cFit.size() - 1) && i_dNormalization_WL != -1.0)
		{
			if (dWL >= i_dNormalization_WL && i_dNormalization_WL != -1.0 && dFit_Normalization == 0.0 && dFlux > 0.0)
				dFit_Normalization = dFlux;
		}
		else if (i_dMin_WL <= dWL && dWL <= i_dMax_WL && dFit_Normalization == 0.0 && dFlux > 0.0)
			dFit_Normalization = dFlux;
	}

	for (unsigned int uiI = 0; uiI < i_cFit.size(); uiI++)
	{
		double dWL = i_cFit.wl(uiI);
		double	dFlux = i_cFit.flux(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			if (dFit_Normalization == 0.0)
				dFit_Normalization = dFlux;
			lpdFit_WL[uiFit_Element_Count] = dWL;
			if (bLinear_Scale)
				lpdFit_Flux[uiFit_Element_Count] = dFlux / dFit_Normalization;
			else
				lpdFit_Flux[uiFit_Element_Count] = log10(dFlux / dFit_Normalization);

			uiFit_Element_Count++;
		}
	}
	double dThreshold = bLinear_Scale ? 0.0 : -15.0;
	for (unsigned int uiI = 0; uiI < uiTarget_Element_Count; uiI++)
	{
		if (lpdTarget_Flux[uiI] > dThreshold && lpdTarget_Flux[uiI] < dMin_Flux)
			dMin_Flux = lpdTarget_Flux[uiI];
		if (lpdTarget_Flux[uiI] > dMax_Flux)
			dMax_Flux = lpdTarget_Flux[uiI];

		if (!bError_Invalid)
		{
			if (lpdTarget_Flux_Error_Upper[uiI] > dThreshold && lpdTarget_Flux_Error_Upper[uiI] < dMin_Flux)
				dMin_Flux = lpdTarget_Flux_Error_Upper[uiI];
			if (lpdTarget_Flux_Error_Upper[uiI] > dMax_Flux)
				dMax_Flux = lpdTarget_Flux_Error_Upper[uiI];

			if (lpdTarget_Flux_Error_Lower[uiI] > dThreshold)
			{
				if (lpdTarget_Flux_Error_Lower[uiI] < dMin_Flux)
					dMin_Flux = lpdTarget_Flux_Error_Lower[uiI];
				if (lpdTarget_Flux_Error_Lower[uiI] > dMax_Flux)
					dMax_Flux = lpdTarget_Flux_Error_Lower[uiI];
			}
		}
	}


	if (bSmart_Range)
	{
		unsigned int	* lpuiBins = new unsigned int [256];
		double	dBin_Range = (dMax_Flux - dMin_Flux) / 255.0;
		unsigned int uiCount = 0;
		memset(lpuiBins,0,sizeof(unsigned int) * 256);
		for (unsigned int uiI = 0; uiI < uiTarget_Element_Count; uiI++)
		{
			unsigned int uiBin = (unsigned int)((lpdTarget_Flux[uiI] - dMin_Flux) / dBin_Range);
			if (uiBin > 255)
				uiBin = 0;
			lpuiBins[uiBin]++;
			uiCount++;
		}
		for (unsigned int uiI = 0; uiI < uiFit_Element_Count; uiI++)
		{
			unsigned int uiBin = (unsigned int)((lpdFit_Flux[uiI] - dMin_Flux) / dBin_Range);
			if (uiBin > 255)
				uiBin = 0;
			lpuiBins[uiBin]++;
			uiCount++;
		}
		// find mode
		unsigned int uiPeak = 0;
		unsigned int uiPeak_Idx;
		for (unsigned int uiI = 0; uiI < 256; uiI++)
		{
			if (lpuiBins[uiI] > uiPeak)
			{
				uiPeak = lpuiBins[uiI];
				uiPeak_Idx = uiI;
			}
		}
		unsigned int uiThreshold = (uiCount * 0.01); // 1%
		if (uiThreshold == 0)
			uiThreshold = 1; // just to be practical
		unsigned int uiMin, uiMax;
		uiMin = uiPeak_Idx;
		uiMax = uiPeak_Idx;
		while (uiMin != 0 && lpuiBins[uiMin] > uiThreshold)
			uiMin--;
		while (uiMax < 256 && lpuiBins[uiMax] > uiThreshold)
			uiMax++;

		if (uiMin >= 2)
		{
			dMin_Flux += (uiMin - 2) * dBin_Range;
		}
		if (uiMax >= 254)
		{
			dMax_Flux -= (254 - uiMax) * dBin_Range;
		}
		delete [] lpuiBins;
	}

	for (unsigned int uiI = 0; uiI < uiFit_Element_Count; uiI++)
	{
		if (lpdFit_Flux[uiI] > dThreshold && lpdFit_Flux[uiI]< dMin_Flux)
			dMin_Flux = lpdFit_Flux[uiI];
		if (lpdFit_Flux[uiI] > dMax_Flux)
			dMax_Flux = lpdFit_Flux[uiI];
	}


	epsplot::page_parameters	cPlot_Parameters;
	epsplot::data cPlot;
	epsplot::line_parameters	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, true, i_dMin_WL, true, i_dMax_WL);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( "Log Flux", false, false, true, dMin_Flux,true, dMax_Flux);

	if (i_lpszTitle)
		cPlot.Set_Plot_Title(i_lpszTitle,18.0);
	cPlot.Set_Plot_Filename(i_lpszOutput_Filename);

	if (!bError_Invalid)
	{
		cLine_Parameters.m_eColor = epsplot::GREY_50;
		cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux_Error_Upper, uiTarget_Element_Count, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
		cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux_Error_Lower, uiTarget_Element_Count, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	}
	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux, uiTarget_Element_Count, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	cLine_Parameters.m_eColor = epsplot::RED;
	cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiFit_Element_Count, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);

	delete [] lpdTarget_WL;
	delete [] lpdTarget_Flux;
	delete [] lpdTarget_Flux_Error_Upper;
	delete [] lpdTarget_Flux_Error_Lower;
	delete [] lpdFit_WL;
	delete [] lpdFit_Flux;
}

void Plot_Spectrum(const char * i_lpszFilename,const double & i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, unsigned int i_uiNum_Points, const double * i_lpdWavelength, const double * i_lpdLog_Intensity)
{
	double dMin_Intensity = 1.0e30;
	double	dMax_Intensity = -1.0e30;
	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		if (i_lpdWavelength[uiI] >= i_dMin_WL && i_lpdWavelength[uiI] <= i_dMax_WL)
		{
			if (i_lpdLog_Intensity[uiI] < dMin_Intensity)
				dMin_Intensity = i_lpdLog_Intensity[uiI];
			if (i_lpdLog_Intensity[uiI] > dMax_Intensity)
				dMax_Intensity = i_lpdLog_Intensity[uiI];
		}
	}

	epsplot::page_parameters	cPlot_Parameters;
	epsplot::data cPlot;
	epsplot::line_parameters	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, true, i_dMin_WL, true, i_dMax_WL);
	unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( "Log Intensity", false, false, true, dMin_Intensity, true, dMax_Intensity);

	if (i_lpszTitle)
		cPlot.Set_Plot_Title(i_lpszTitle,18.0);
	cPlot.Set_Plot_Filename(i_lpszFilename);

	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Set_Plot_Data(i_lpdWavelength, i_lpdLog_Intensity, i_uiNum_Points, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
	cPlot.Plot(cPlot_Parameters);

}
void Plot_Spectrum(const char * i_lpszFilename,const double & i_dMin_WL, const double & i_dMax_WL, const char * i_lpszTitle, const ES::Spectrum & i_cTarget)
{
	unsigned int uiElement_Count = 0;
	double * lpdSpectra_WL = NULL, * lpdSpectra_Flux = NULL;
	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
	{
		double dWL = i_cTarget.wl(uiI);
		if (i_dMin_WL <= dWL && dWL <= i_dMax_WL)
		{
			uiElement_Count++;
		}
	}
	if (uiElement_Count > 0)
	{
		lpdSpectra_WL = new double[uiElement_Count];
		lpdSpectra_Flux = new double[uiElement_Count];
		unsigned int uiI = 0;
		unsigned int uiJ = 0;
		while (i_cTarget.wl(uiI) < i_dMin_WL)
			uiI++;
		for (unsigned int uiJ = 0;uiJ < uiElement_Count; uiJ++)
		{
			lpdSpectra_WL[uiJ] = i_cTarget.wl(uiI + uiJ);
			lpdSpectra_Flux[uiJ] = i_cTarget.flux(uiI + uiJ);
		}

		epsplot::page_parameters	cPlot_Parameters;
		epsplot::data cPlot;
		epsplot::line_parameters	cLine_Parameters;

		epsplot::axis_parameters	cX_Axis_Parameters;
		epsplot::axis_parameters	cY_Axis_Parameters;

		cPlot_Parameters.m_uiNum_Columns = 1;
		cPlot_Parameters.m_uiNum_Rows = 1;
		cPlot_Parameters.m_dWidth_Inches = 11.0;
		cPlot_Parameters.m_dHeight_Inches = 8.5;

		cX_Axis_Parameters.Set_Title("Wavelength [A]");
		cY_Axis_Parameters.Set_Title("Flux");
		cY_Axis_Parameters.m_dLower_Limit = 0.0;

		unsigned int uiX_Axis_ID = cPlot.Set_X_Axis_Parameters( cX_Axis_Parameters);
		unsigned int uiY_Axis_ID = cPlot.Set_Y_Axis_Parameters( cY_Axis_Parameters);

		if (i_lpszTitle)
			cPlot.Set_Plot_Title(i_lpszTitle,18.0);
		cPlot.Set_Plot_Filename(i_lpszFilename);

		cLine_Parameters.m_eColor = epsplot::BLACK;
		cPlot.Set_Plot_Data(lpdSpectra_WL, lpdSpectra_Flux, uiElement_Count, cLine_Parameters, uiX_Axis_ID, uiY_Axis_ID);
		cPlot.Plot(cPlot_Parameters);

		delete [] lpdSpectra_WL;
		delete [] lpdSpectra_Flux;
	}

}


void Plot_EPS_Grid(const epsplot::page_parameters & i_cGrid, const EPS_SPECTRA_PLOT_DATA & i_cData)
{
	epsplot::color_triplet	cBlue(0.0,0.0,1.0);
	char	lpszText[32];
	double ** lpdMinima_WL = NULL, ** lpdMinima_Flux = NULL;
	double ** lpdEW = NULL;
	double lpdDash_Style[2] = {2,2};
	double lpdLong_Short_Dash_Style[4] = {4,2,2,2};
	epsplot::epsfile	cEPS("%.3f");// it seems this resolution is sufficient.  For higher resolution, use %.6f
	cEPS.Open_File(i_cData.m_lpszFilename, i_cData.m_lpszTitle, i_cGrid.m_dWidth_Inches, i_cGrid.m_dHeight_Inches);

	double	dGraph_Space_X = i_cGrid.m_dWidth_Inches * 72.0 * (1.0 - i_cGrid.m_dLeft_Axis_Margin - i_cGrid.m_dRight_Axis_Margin) - 2.0;
	double	dGraph_Space_Y = i_cGrid.m_dHeight_Inches * 72.0 * (1.0 - i_cGrid.m_dTop_Axis_Margin - i_cGrid.m_dBottom_Axis_Margin - i_cGrid.m_dTitle_Margin) - 2.0; // -2 to allow a little gap at the edge

	double	dGraph_Offset_X = i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0;
	double	dGraph_Offset_Y = i_cGrid.m_dHeight_Inches * i_cGrid.m_dBottom_Axis_Margin * 72.0;
	
	double	dSingle_Plot_Space_X = dGraph_Space_X / i_cGrid.m_uiNum_Columns;
	double	dSingle_Plot_Space_Y = dGraph_Space_Y / i_cGrid.m_uiNum_Rows;

	double	dDay_Min = DBL_MAX, dDay_Max = -DBL_MAX;

	for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
	{
		if (i_cData.m_lpdDays[uiJ] < dDay_Min)
			dDay_Min = i_cData.m_lpdDays[uiJ];
		if (i_cData.m_lpdDays[uiJ] > dDay_Max)
			dDay_Max = i_cData.m_lpdDays[uiJ];
	}
	double	dDay_Range = (dDay_Max - dDay_Min);
//	if (dDay_Range < 1.0)
//		dDay_Range += 2.0;
	double	**lpdNormalization = new double *[i_cData.m_uiNum_Plots];
		
	double	dXmin = DBL_MAX, dXmax = -DBL_MAX, dYmin = DBL_MAX, dYmax = -DBL_MAX;
	for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
	{
		lpdNormalization[uiI] = new double [i_cData.m_uiNum_Days];
		memset(lpdNormalization[uiI],0,sizeof(double) * i_cData.m_uiNum_Days);

		if (i_cData.m_dNormalization_WL > 0.0)
		{
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
			{
				for (unsigned int uiK = 0; uiK < i_cData.m_lpcSpectra[uiI][uiJ].size() && lpdNormalization[uiI][uiJ] == 0.0; uiK++)
				{
					if (i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) > i_cData.m_dNormalization_WL && lpdNormalization[uiI][uiJ] == 0.0)
						lpdNormalization[uiI][uiJ] = i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK);
				}
				if (lpdNormalization[uiI][uiJ] == 0.0)
					lpdNormalization[uiI][uiJ] = 1.0;
//				printf("%i %i %.2f\n",uiI,uiJ,lpdNormalization[uiI][uiJ]);
				lpdNormalization[uiI][uiJ] = 1.0 / lpdNormalization[uiI][uiJ];
			}
		}
		else
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
				lpdNormalization[uiI][uiJ] = 1.0;
//		printf("Compute Ymin\n");
		for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
		{
			for (unsigned int uiK = 0; uiK < i_cData.m_lpcSpectra[uiI][uiJ].size(); uiK++)
			{
				if (i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) < dXmin)
					dXmin = i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK);
				if (i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) > dXmax)
					dXmax = i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK);
				if (i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK) * lpdNormalization[uiI][uiJ] < dYmin)
					dYmin = i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK) * lpdNormalization[uiI][uiJ];
				if (i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK) * lpdNormalization[uiI][uiJ] > dYmax)
					dYmax = i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK)  * lpdNormalization[uiI][uiJ];
			}
		}
	}
	if (i_cData.m_dMin_WL > 0)// && dXmin < i_cData.m_dMin_WL)
		dXmin = i_cData.m_dMin_WL;
	if (i_cData.m_dMax_WL > 0)// && dXmax > i_cData.m_dMax_WL)
		dXmax = i_cData.m_dMax_WL;
	if (i_cData.m_dMin_Flux > 0)// && dYmin < i_cData.m_dMin_Flux)
		dYmin = i_cData.m_dMin_Flux;
	if (i_cData.m_dMax_Flux > 0)// && dYmax > i_cData.m_dMax_Flux)
		dYmax = i_cData.m_dMax_Flux;
	if (i_cData.m_bLog_Flux)
	{
		dYmin = log10(dYmin);
		dYmax = log10(dYmax);
		if (dYmin < i_cData.m_dMin_Log_Flux)
			dYmin = i_cData.m_dMin_Log_Flux;
		if (dYmax > i_cData.m_dMax_Log_Flux)
			dYmax = i_cData.m_dMax_Log_Flux;
	}
	double	dX_Range = (dXmax - dXmin);
	double	dY_Range = (dYmax - dYmin);

	if (i_cData.m_bPlot_Minima_Curve)
	{
		double	dWL_Min = i_cData.m_dMin_WL_For_Minima, dWL_Max = i_cData.m_dMax_WL_For_Minima;
		lpdMinima_WL = new double *[i_cData.m_uiNum_Plots];
		lpdMinima_Flux = new double *[i_cData.m_uiNum_Plots];
		for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
		{
			lpdMinima_WL[uiI] = new double [i_cData.m_uiNum_Days];
			lpdMinima_Flux[uiI] = new double [i_cData.m_uiNum_Days];
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
			{
				lpdMinima_WL[uiI][uiJ] = 0.0;
				lpdMinima_Flux[uiI][uiJ] = DBL_MAX;
				for (unsigned int uiK = 0; uiK < i_cData.m_lpcSpectra[uiI][uiJ].size(); uiK++)
				{
					if (i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) >= i_cData.m_dMin_WL_For_Minima && 
						i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) <= i_cData.m_dMax_WL_For_Minima)
					{
						if (i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK) < lpdMinima_Flux[uiI][uiJ])
						{
							lpdMinima_Flux[uiI][uiJ] = i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK);
							lpdMinima_WL[uiI][uiJ] = i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK);
						}
					}
				}
			}
		}
	}
	double	dEW_Min = DBL_MAX, dEW_Max = -DBL_MAX;
	if (i_cData.m_bPlot_EW_Curve && i_cData.m_dMin_WL_For_EW > 0.0 && i_cData.m_dMax_WL_For_EW > 0)
	{
		double dWL_Min = i_cData.m_dMin_WL_For_EW, dWL_Max = i_cData.m_dMax_WL_For_EW;
		lpdEW = new double *[i_cData.m_uiNum_Plots];
		for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
		{
			lpdEW[uiI] = new double [i_cData.m_uiNum_Days];
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
			{
				lpdEW[uiI][uiJ] = Equivalent_Width(i_cData.m_lpcSpectra[uiI][uiJ], dWL_Min,dWL_Max,2);
				//printf("%.2e %i %i\n",lpdEW[uiI][uiJ],uiI,uiJ);
				if (lpdEW[uiI][uiJ] < dEW_Min)
					dEW_Min = lpdEW[uiI][uiJ];
				if (lpdEW[uiI][uiJ] > dEW_Max)
					dEW_Max = lpdEW[uiI][uiJ];
			}
		}
	}
	double	dEW_Range = dEW_Max - dEW_Min;
	double	dEW_Scale = (lpdEW  != NULL) ? dSingle_Plot_Space_X / dEW_Range : 1.0;
	double	dX_Scale = dSingle_Plot_Space_X / dX_Range;
	double	dY_Scale = dSingle_Plot_Space_Y / (dY_Range * (dDay_Range + 2.0));
//	printf("%.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",dGraph_Offset_X, dSingle_Plot_Space_X, dGraph_Offset_Y, dSingle_Plot_Space_Y,dY_Range,dY_Scale,dDay_Range);
	for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
	{
		cEPS.State_Push();
 		cEPS.Translate(dGraph_Offset_X + (dSingle_Plot_Space_X * (uiI % i_cGrid.m_uiNum_Columns)) ,dGraph_Offset_Y + (dSingle_Plot_Space_Y * (uiI / i_cGrid.m_uiNum_Columns)));
		for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
		{
			bool bIn_Path = false;
			sprintf(lpszText,"Spectrum for %i %i Day %.1f",uiI % i_cGrid.m_uiNum_Columns, uiI / i_cGrid.m_uiNum_Columns, i_cData.m_lpdDays[uiJ]);
			cEPS.Comment(lpszText);
			cEPS.State_Push();
	 		cEPS.Translate(0.0,(dDay_Max - i_cData.m_lpdDays[uiJ] + 0.5) / (dDay_Range + 2.0) * dSingle_Plot_Space_Y);
//0.5, 1 = extra buffer space
			//printf("%.2f %.2f\n",dY_Range,dY_Scale);
			for (unsigned int uiK = 0; uiK < i_cData.m_lpcSpectra[uiI][uiJ].size(); uiK++)
			{
				if (i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) >= dXmin && i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) <= dXmax)
				{
					double dY = i_cData.m_lpcSpectra[uiI][uiJ].flux(uiK) * lpdNormalization[uiI][uiJ];
					if (i_cData.m_bLog_Flux)
						dY = log10(dY);
					dY -= dYmin;
					double dX = i_cData.m_lpcSpectra[uiI][uiJ].wl(uiK) - dXmin;

					dX *= dX_Scale;
					dY *= dY_Scale;
					if (!bIn_Path)
						cEPS.Move_To(dX,dY);
					else
						cEPS.Line_To(dX,dY);
					bIn_Path = true;
				}
				else
					bIn_Path = false;
			}
			cEPS.Set_RGB_Color(i_cData.m_cSpectrum_Color);
			cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
			cEPS.Stroke();
			cEPS.State_Pop();
		}
		if (i_cData.m_bPlot_Minima_Curve)
		{
			sprintf(lpszText,"Minima curve for %i %i",uiI % i_cGrid.m_uiNum_Columns, uiI / i_cGrid.m_uiNum_Columns);
			cEPS.Comment(lpszText);
			cEPS.State_Push();
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
			{
				double dY = lpdMinima_Flux[uiI][uiJ] * lpdNormalization[uiI][uiJ];
				if (i_cData.m_bLog_Flux)
					dY = log10(dY);
				dY -= dYmin;
				double dX = lpdMinima_WL[uiI][uiJ] - dXmin;

				dX *= dX_Scale;
				dY *= dY_Scale;
	 			dY += (dDay_Max - i_cData.m_lpdDays[uiJ] + 0.5) / (dDay_Range + 2.0) * dSingle_Plot_Space_Y;
				if (uiJ == 0)
					cEPS.Move_To(dX,dY);
				else
					cEPS.Line_To(dX,dY);
			}
			cEPS.Set_RGB_Color(epsplot::STD_RED);
			cEPS.Set_Dash(lpdDash_Style,2,0);
			cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
			cEPS.Stroke();
			cEPS.State_Pop();
		}
		if (lpdEW != NULL)
		{
			sprintf(lpszText,"EW curve for %i %i",uiI % i_cGrid.m_uiNum_Columns, uiI / i_cGrid.m_uiNum_Columns);
			cEPS.Comment(lpszText);
			cEPS.State_Push();
			for (unsigned int uiJ = 0; uiJ < i_cData.m_uiNum_Days; uiJ++)
			{
				double dY = (dDay_Max - i_cData.m_lpdDays[uiJ] + 0.5) / (dDay_Range + 2.0) * dSingle_Plot_Space_Y;
				double dX = lpdEW[uiI][uiJ] - dEW_Min;

				dX *= dEW_Scale;
				if (uiJ == 0)
					cEPS.Move_To(dX,dY);
				else
					cEPS.Line_To(dX,dY);
			}
			cEPS.Set_RGB_Color(epsplot::STD_BLUE);
			cEPS.Set_Dash(lpdLong_Short_Dash_Style,4,0);
			cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
			cEPS.Stroke();
			cEPS.State_Pop();
		}
		cEPS.State_Pop();
	}
//	printf("Grid\n");
//	sleep(1);
	// Draw grid
	cEPS.Comment("Grid");
	cEPS.State_Push();
	cEPS.Set_Line_Width(2.0);
	cEPS.Translate(dGraph_Offset_X ,dGraph_Offset_Y);
	cEPS.Move_To(0.0,0.0);
	cEPS.Line_To(0.0,dGraph_Space_Y);
	cEPS.Line_To(dGraph_Space_X,dGraph_Space_Y);
	cEPS.Line_To(dGraph_Space_X,0.0);
	cEPS.Close_Path();
	cEPS.Set_RGB_Color(i_cData.m_cAxis_Color);
	cEPS.Stroke();

	for (unsigned int uiI = 1; uiI < i_cGrid.m_uiNum_Columns; uiI++)
	{
		cEPS.Move_To((dGraph_Space_X / i_cGrid.m_uiNum_Columns) * uiI,0.0);
		cEPS.Line_To((dGraph_Space_X / i_cGrid.m_uiNum_Columns) * uiI,dGraph_Space_Y);
		cEPS.Stroke();
	}

	for (unsigned int uiI = 1; uiI < i_cGrid.m_uiNum_Rows; uiI++)
	{
		cEPS.Move_To(0.0,(dGraph_Space_Y / i_cGrid.m_uiNum_Rows) * uiI);
		cEPS.Line_To(dGraph_Space_X,(dGraph_Space_Y / i_cGrid.m_uiNum_Rows) * uiI);
		cEPS.Stroke();
	}
	cEPS.State_Pop();
	
//	printf("Axis labels\n");
	cEPS.Comment("Axis labels");
	cEPS.State_Push();
	cEPS.Text(epsplot::TIMES,false,false,36,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 + dGraph_Space_X * 0.5, 1.3 * i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 + dGraph_Space_Y, "Equivalent Width [Angstroms]");
	cEPS.Text(epsplot::TIMES,false,false,36,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 + dGraph_Space_X * 0.5, 0.3 * i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0, (i_cData.m_dVelocity_Reference_WL > 0.0) ? "Velocity [kkm/s]" : "Wavelength [Angstroms]");
	cEPS.Text(epsplot::TIMES,false,false,36,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.5,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0, "log Flux +  constant", 90.0);
	cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.75,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0, "Time", 90.0);


//	printf("Time arrow\n");
	cEPS.Comment("Time arrow");
	cEPS.State_Push();
	cEPS.Set_Line_Width(1.0);
	cEPS.Move_To( i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.9,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 + dSingle_Plot_Space_Y * 0.5);
	cEPS.Line_To( i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.9,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 - dSingle_Plot_Space_Y * 0.5);
	cEPS.Line_To( i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.95,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 - dSingle_Plot_Space_Y * 0.45);
	cEPS.Stroke();
	cEPS.Move_To( i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.9,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 - dSingle_Plot_Space_Y * 0.5);
	cEPS.Line_To( i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin * 72.0 * 0.85,dGraph_Space_Y * 0.5 +i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches * 72.0 - dSingle_Plot_Space_Y * 0.45);
	cEPS.Stroke();
	cEPS.Set_RGB_Color(i_cData.m_cAxis_Color);
	cEPS.State_Pop();


	// Grid lines
//	printf("Ticks\n");
	cEPS.Comment("Grid ticks");
	for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
	{
		cEPS.State_Push();
		cEPS.Set_RGB_Color(i_cData.m_cAxis_Color);
		cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
		cEPS.Translate(dGraph_Offset_X + (dSingle_Plot_Space_X * (uiI % i_cGrid.m_uiNum_Columns)) ,dGraph_Offset_Y + (dSingle_Plot_Space_Y * (uiI / i_cGrid.m_uiNum_Columns)));
			//printf("%.2f %.2f\n",dY_Range,dY_Scale);
		if (i_cData.m_dVelocity_Reference_WL > 0.0)
		{
			for (int iV = -100; iV <= 100; iV+=10)
			{
				double dWL = (iV / 300.0 + 1.0) * i_cData.m_dVelocity_Reference_WL;
				if (dWL >= dXmin && dWL <= dXmax)
				{
					double dX = (dWL - dXmin) * dX_Scale;
					double dY = i_cData.m_dLine_Width_Axis - 0.1;
					cEPS.Move_To(dX,dY);
					cEPS.Line_To(dX,dY + ((iV % 20 == 0) ? 4.0 : 3.0) * i_cData.m_dLine_Width_Axis);
					cEPS.Set_Line_Width((iV % 20 == 0) ? i_cData.m_dLine_Width_Graph : i_cData.m_dLine_Width_Graph * 0.5);
					cEPS.Stroke();
				}
			}
		}
		else
		{
			double	dWL_Range = (dXmax - dXmin);
			double dNum_Big = dWL_Range / 1000.0;
			double	dDelta;
			if (dNum_Big > 3.0)
				dDelta = 500.0;
			else if (dNum_Big > 1.5)
				dDelta = 250.0;
			else
				dDelta = 100.0;
			double dWL_Start = dXmin - fmod(dXmin,dDelta);
			if (dWL_Start < dXmin)
				dWL_Start += dDelta;
			
			for (double dWL = dWL_Start; dWL <= dXmax; dWL += dDelta)
			{
				double dX = (dWL - dXmin) * dX_Scale;
				double dY = i_cData.m_dLine_Width_Axis - 0.1;
				cEPS.Move_To(dX,dY);
				cEPS.Line_To(dX,dY + ((fmod(dWL,1000.0) == 0.0) ? 4.0 : 3.0) * i_cData.m_dLine_Width_Axis);
				cEPS.Set_Line_Width((fmod(dWL,1000.0) == 0.0) ? i_cData.m_dLine_Width_Graph : i_cData.m_dLine_Width_Graph * 0.5);
				cEPS.Stroke();
			}
		}
		cEPS.State_Pop();
	}
	for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
	{
		cEPS.State_Push();
		cEPS.Set_RGB_Color(i_cData.m_cAxis_Color);
		cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
		cEPS.Translate(dGraph_Offset_X + (dSingle_Plot_Space_X * (uiI % i_cGrid.m_uiNum_Columns)) ,dGraph_Offset_Y + (dSingle_Plot_Space_Y * (uiI / i_cGrid.m_uiNum_Columns)));
		double dX = dSingle_Plot_Space_X * 0.25;
		double dY = dSingle_Plot_Space_Y - 0.1;
		// line at top of graph
		cEPS.Move_To(dX,dY);
		cEPS.Line_To(dX,dY - 3.0 * i_cData.m_dLine_Width_Axis);
		cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
		cEPS.Stroke();
		dX = dSingle_Plot_Space_X * 0.50;
		cEPS.Move_To(dX,dY);
		cEPS.Line_To(dX,dY - 3.0 * i_cData.m_dLine_Width_Axis);
		cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
		cEPS.Stroke();
		dX = dSingle_Plot_Space_X * 0.75;
		cEPS.Move_To(dX,dY);
		cEPS.Line_To(dX,dY - 3.0 * i_cData.m_dLine_Width_Axis);
		cEPS.Set_Line_Width(i_cData.m_dLine_Width_Graph);
		cEPS.Stroke();
		cEPS.State_Pop();
	}
//	printf("Axis text\n");
	cEPS.Comment("X axis text");
	if (i_cData.m_dVelocity_Reference_WL > 0.0)
	{
		for (unsigned int uiI = 0; uiI < i_cGrid.m_uiNum_Columns; uiI++)
		{
			cEPS.State_Push();
			cEPS.Translate(dGraph_Offset_X + dSingle_Plot_Space_X * uiI ,dGraph_Offset_Y);
			for (int iV = -100; iV <= 100; iV+=20)
			{
				double dWL = (iV / 300.0 + 1.0) * i_cData.m_dVelocity_Reference_WL;
				if (dWL >= dXmin && dWL <= dXmax)
				{
					double dX = (dWL - dXmin) * dX_Scale;
					double dY = -i_cData.m_dLine_Width_Axis - 14.0;
					sprintf(lpszText,"%i",-iV);
					cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,dX,dY, lpszText);
				}
			}
			cEPS.State_Pop();
		}
	}
	else
	{
		double	dWL_Range = (dXmax - dXmin);
		double dNum_Big = dWL_Range / 1000.0;
		double	dDelta;
		if (dNum_Big > 3.0)
			dDelta = 2000.0;
		else if (dNum_Big > 1.5)
			dDelta = 1000.0;
		else
			dDelta = 500.0;
		double dWL_Start = dXmin - fmod(dXmin,dDelta * 0.5);
		if (dWL_Start < dXmin)
			dWL_Start += (dDelta * 0.5);
		for (unsigned int uiI = 0; uiI < i_cGrid.m_uiNum_Columns; uiI++)
		{
			cEPS.State_Push();
			cEPS.Translate(dGraph_Offset_X + dSingle_Plot_Space_X * uiI ,dGraph_Offset_Y);
		
			for (double dWL = dWL_Start; dWL <= dXmax; dWL += dDelta)
			{
				double dX = (dWL - dXmin) * dX_Scale;
				double dY = -i_cData.m_dLine_Width_Axis - 14.0;
				sprintf(lpszText,"%.0f",dWL);
				cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,dX,dY, lpszText);
			}
			cEPS.State_Pop();
		}
	}
	for (unsigned int uiI = 0; uiI < i_cGrid.m_uiNum_Columns; uiI++)
	{
		cEPS.State_Push();
		cEPS.Translate(dGraph_Offset_X + dSingle_Plot_Space_X * uiI ,dGraph_Offset_Y + dGraph_Space_Y + 4.0 + i_cData.m_dLine_Width_Axis);
	
		double dY = 0.0;
		double dX = dEW_Range * 0.25 * dEW_Scale;
		sprintf(lpszText,"%.1f",dEW_Range * 0.25 + dEW_Min);
		cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,dX,dY, lpszText);

		dX = dEW_Range * 0.50 * dEW_Scale;
		sprintf(lpszText,"%.1f",dEW_Range * 0.50 + dEW_Min);
		cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,dX,dY, lpszText);

		dX = dEW_Range * 0.75 * dEW_Scale;
		sprintf(lpszText,"%.1f",dEW_Range * 0.75 + dEW_Min);
		cEPS.Text(epsplot::TIMES,false,false,18,epsplot::CENTER,epsplot::BOTTOM,i_cData.m_cAxis_Color,dX,dY, lpszText);

		cEPS.State_Pop();
	}

//	printf("Done\n");
	cEPS.Close_File();


	if (lpdEW)
	{
		for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
			delete [] lpdEW[uiI];
		delete [] lpdEW;
	}
	if (lpdMinima_WL)
	{
		for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
			delete [] lpdMinima_WL[uiI];
		delete [] lpdMinima_WL;
	}
	if (lpdMinima_Flux)
	{
		for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
			delete [] lpdMinima_Flux[uiI];
		delete [] lpdMinima_Flux;
	}
	for (unsigned int uiI = 0; uiI < i_cData.m_uiNum_Plots; uiI++)
		delete [] lpdNormalization[uiI];
	delete [] lpdNormalization;
//	printf("Done\n");
}




unsigned int	Set_Plot_Data(epsplot::data & i_cPlot, const ES::Spectrum & i_cSpectrum, const double & i_dMin_WL, const double & i_dMax_WL, const double & i_dNormalization_WL, const double & i_dVelocity_Reference_WL, const epsplot::line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)
{
	unsigned int uiRet = 0;
	unsigned int uiNum_Points = 0;
	unsigned int uiStart_Idx = 0;
	double dNormalization = 1.0;
	if (i_dMin_WL < 0 || i_dMax_WL < 0)
		uiNum_Points = i_cSpectrum.size();
	else
	{
		while (uiStart_Idx < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx) < i_dMin_WL)
			uiStart_Idx++;
		while ((uiStart_Idx + uiNum_Points) < i_cSpectrum.size() && i_cSpectrum.wl(uiStart_Idx + uiNum_Points) < i_dMax_WL)
			uiNum_Points++;
	}
	if (i_dNormalization_WL > 0.0)
	{
		unsigned int uiI = 0;
		while (uiI < i_cSpectrum.size() && i_cSpectrum.wl(uiI) < i_dNormalization_WL)
			uiI++;
		if (uiI < i_cSpectrum.size())
		{
			dNormalization = 1.0 / i_cSpectrum.flux(uiI);
		}
	}

	if (uiNum_Points > 0)
	{
		double * lpdData_X = new double [uiNum_Points];
		double * lpdData_Y = new double [uiNum_Points];
		for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
		{
			if (i_dVelocity_Reference_WL > 0.0)
				lpdData_X[uiI] = i_cSpectrum.wl(uiI + uiStart_Idx) / i_dVelocity_Reference_WL * 299.792458;
			else
				lpdData_X[uiI] = i_cSpectrum.wl(uiI + uiStart_Idx);
			lpdData_Y[uiI] = i_cSpectrum.flux(uiI + uiStart_Idx) * dNormalization;
		}

		uiRet = i_cPlot.Set_Plot_Data(lpdData_X,lpdData_Y,uiNum_Points,i_cLine_Parameters, i_uiX_Axis_Type, i_uiY_Axis_Type);

		delete [] lpdData_X;
		delete [] lpdData_Y;
	}
	return uiRet;
}


