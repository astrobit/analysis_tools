
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
#include <Plot_Utilities.h>
#include <float.h>
#include <line_routines.h>






int main(void)
{
	unsigned int uiNum_Spectra;
    ES::Spectrum  * lpcSpectra;
	double			*lpdDay;
	bool			bLinear_Scale;
	SERIESLISTNODE			* lpcSeriesListHead = NULL;
	SERIESLISTNODE			* lpcSeriesListTail = NULL;
	epsplot::DATA cPlot;
	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	unsigned int uiCurr_Idx = 0;
	FILE * fileIn = fopen("serieslist.csv","rt");
	if (fileIn)
	{
		char * lpszBuffer = new char [1024];
		while (fgets(lpszBuffer,sizeof(char) * 1024, fileIn))
		{
			char * lpszCursor = strchr(lpszBuffer,',');
			if (lpszBuffer[0] != 0 && lpszCursor)
			{
				lpszCursor--;
				while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
					lpszCursor--;
				lpszCursor++;
				lpszCursor[0] = 0;
				lpszCursor++;
				while (lpszCursor[0] != 0 && !((lpszCursor[0] >= '0' && lpszCursor[0] <= '9') || lpszCursor[0] == '-' || lpszCursor[0] == '+'))
					lpszCursor++;
				SERIESLISTNODE * lpCurr = new SERIESLISTNODE;
				lpCurr->m_dDay = atof(lpszCursor);
				lpszCursor = lpszBuffer;
				while (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')
					lpszCursor++;
				lpCurr->m_lpszFilename = new char[strlen(lpszCursor) + 1];
				strcpy(lpCurr->m_lpszFilename,lpszCursor);


				uiNum_Spectra++;
				if (!lpcSeriesListTail)
				{
					lpcSeriesListTail = lpcSeriesListHead = lpCurr;
				}
				else
				{
					lpcSeriesListTail->m_lpcNext = lpCurr;
					lpCurr->m_lpcPrev = lpcSeriesListTail;
					lpcSeriesListTail = lpCurr;
				}
			}
		}
		fclose(fileIn);
	}
	lpcSpectra = new ES::Spectrum[uiNum_Spectra];
	lpdDay = new double[uiNum_Spectra];
	SERIESLISTNODE * lpCurr = lpcSeriesListHead;
	while (lpCurr)
	{
        lpcSpectra[uiCurr_Idx] = ES::Spectrum::create_from_ascii_file( lpCurr->m_lpszFilename );
		
		lpdDay[uiCurr_Idx] = lpCurr->m_dDay;
		if (lpcSpectra[uiCurr_Idx].size() == 0)
			printf("File %s seems to be empty!\n",lpCurr->m_lpszFilename);
		uiCurr_Idx++;
		lpCurr = lpCurr->m_lpcNext;
	}

	double	*lpdEquivalent_Width;
	double	*lpdMinima_WL;
	lpdEquivalent_Width = new double[uiNum_Spectra];
	lpdMinima_WL = new double[uiNum_Spectra];
	Find_Minimum(lpcSpectra, uiNum_Spectra, lpdMinima_WL, NULL, 7400.0, 8300.0);
	for (unsigned int uiJ = 0; uiJ < uiNum_Spectra; uiJ++)
	{
		double	dWL_Min = 7600.0, dWL_Max = 8600.0;
		lpdMinima_WL[uiJ] = (1.0 - lpdMinima_WL[uiJ] / 8579.0) * 300.0; // convert to velocity [kkm/s]
		lpdEquivalent_Width[uiJ] = Equivalent_Width(lpcSpectra[uiJ], dWL_Min, dWL_Max, 4);
		if (isnan(lpdEquivalent_Width[uiJ]))
			printf("nan at %.1f (%i)\n",lpdDay[uiJ],uiJ);
	}
	cPlot.Set_Plot_Filename("series.paper.full.eps");
	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Time [days]", false, false, false, 0.0, false, 0.0);
	unsigned int uiY_Axis_Vel = cPlot.Set_Y_Axis_Parameters( "Velocity [kkm/s]", false, false, true, 5.0, true, 60.0);
	unsigned int uiY_Axis_pEW = cPlot.Set_Y_Axis_Parameters( "Equivalent Width [Angstroms]", false, false, true, 0.0, true, 1000.0);

	cPlot.Set_Plot_Data( lpdDay, lpdMinima_WL, uiNum_Spectra, epsplot::RED, epsplot::SOLID, uiX_Axis, uiY_Axis_Vel, -1.0);
	cPlot.Set_Plot_Data( lpdDay, lpdEquivalent_Width, uiNum_Spectra, epsplot::BLUE, epsplot::SOLID, uiX_Axis, uiY_Axis_pEW, -1.0);
	cPlot.Plot(cPlot_Parameters);

	delete [] lpdEquivalent_Width;
	delete [] lpdMinima_WL;
	delete [] lpcSpectra;
	delete [] lpdDay;
	lpCurr = lpcSeriesListHead;
	while (lpCurr)
	{
		SERIESLISTNODE			* lpNext = lpCurr->m_lpcNext;
		delete lpCurr;
		lpCurr = lpNext;
	}

}

