#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xio.h>
#include <Plot_Utilities.h>
#include "ES_Synow.hh"
#include "ES_Generic_Error.hh"


class SERIESLISTNODE
{
public:
	char *  m_lpszFilename;
	double	m_dDay;
	epsplot::COLOR	m_eColor;

	SERIESLISTNODE * m_lpcNext;
	SERIESLISTNODE * m_lpcPrev;

	SERIESLISTNODE(void)
	{
		m_lpszFilename = NULL;
		m_dDay = -50.0;
		m_lpcNext = m_lpcPrev = NULL;
		m_eColor = epsplot::BLACK;
	}
};

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszOutput_Filename[64];
	char	lpszOutput_Dir[256];
	char	lpszTitle[128];
	unsigned int uiNum_Spectra;
    ES::Spectrum  * lpcSpectra;
	double			*lpdDay;
	bool			bLinear_Scale = true;
	SERIESLISTNODE			* lpcSeriesListHead = NULL;
	SERIESLISTNODE			* lpcSeriesListTail = NULL;
	char ** lplpszInput_Filenames;
	bool	bGenerate_Individual_Spectra = 	!xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-individual");
	epsplot::COLOR	* lpColors;


	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--title",lpszTitle,128,"");
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--output-dir",lpszOutput_Dir,128,"");
	if (lpszOutput_Dir[0] != 0)
		strcat(lpszOutput_Dir,".");
	if (xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values, "--serieslist"))
	{
		char lpszSeriesListFilename[128];
		xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--serieslist",lpszSeriesListFilename,128,"");
		FILE * fileIn = fopen(lpszSeriesListFilename,"rt");
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
					lpszCursor = strchr(lpszCursor,',');
					if (lpszCursor)
					{
						lpszCursor++;
						printf("%s\n",lpszCursor);
						if (strncmp(lpszCursor,"BLUE",4) == 0 || strncmp(lpszCursor,"blue",4) == 0 || strncmp(lpszCursor,"Blue",4) == 0)
							lpCurr->m_eColor = epsplot::BLUE;
						else if (strncmp(lpszCursor,"RED",3) == 0 || strncmp(lpszCursor,"red",3) == 0 || strncmp(lpszCursor,"Red",3) == 0)
							lpCurr->m_eColor = epsplot::RED;
						else if (strncmp(lpszCursor,"GREEN",5) == 0 || strncmp(lpszCursor,"green",5) == 0 || strncmp(lpszCursor,"Green",5) == 0)
							lpCurr->m_eColor = epsplot::GREEN;
						else if (strncmp(lpszCursor,"YELLOW",6) == 0 || strncmp(lpszCursor,"yellow",6) == 0 || strncmp(lpszCursor,"Yellow",6) == 0)
							lpCurr->m_eColor = epsplot::YELLOW;
						else if (strncmp(lpszCursor,"MAGENTA",7) == 0 || strncmp(lpszCursor,"magenta",7) == 0 || strncmp(lpszCursor,"Magenta",7) == 0)
							lpCurr->m_eColor = epsplot::MAGENTA;
						else if (strncmp(lpszCursor,"CYAN",4) == 0 || strncmp(lpszCursor,"cyan",4) == 0 || strncmp(lpszCursor,"Cyan",4) == 0)
							lpCurr->m_eColor = epsplot::CYAN;
					}

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
		}
	}
	else
	{
		for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			if (strstr(i_lpszArg_Values[uiI],"--title") == 0 &&
				strstr(i_lpszArg_Values[uiI],"--linear-scale") == 0 &&
				strstr(i_lpszArg_Values[uiI],"--output-dir") == 0)
			{
				uiNum_Spectra++;
				SERIESLISTNODE * lpCurr = new SERIESLISTNODE;
				lpCurr->m_lpszFilename = new char[strlen(i_lpszArg_Values[uiI]) + 1];
				strcpy(lpCurr->m_lpszFilename,i_lpszArg_Values[uiI]);
				uiI++;
				lpCurr->m_dDay = atof(i_lpszArg_Values[uiI]);
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
	}

	lpcSpectra = new ES::Spectrum[uiNum_Spectra];
	lpdDay = new double[uiNum_Spectra];
	lpColors = new epsplot::COLOR[uiNum_Spectra];
	lplpszInput_Filenames = new char * [uiNum_Spectra];
	double	dMin_WL = DBL_MAX, dMax_WL = 0.0;
	double	dMin_Flux = DBL_MAX, dMax_Flux = 0.0;
	unsigned int uiCurr_Idx = 0;
	SERIESLISTNODE * lpCurr = lpcSeriesListHead;
	while (lpCurr)
	{
		dMin_Flux = DBL_MAX; dMax_Flux = 0.0;
        lpcSpectra[uiCurr_Idx] = ES::Spectrum::create_from_ascii_file( lpCurr->m_lpszFilename );
		
		lpdDay[uiCurr_Idx] = lpCurr->m_dDay;
		if (lpcSpectra[uiCurr_Idx].size() > 0)
		{
			if (lpcSpectra[uiCurr_Idx].wl(0) < dMin_WL)
				dMin_WL = lpcSpectra[uiCurr_Idx].wl(0);
			if (lpcSpectra[uiCurr_Idx].wl(lpcSpectra[uiCurr_Idx].size() - 1) > dMax_WL)
				dMax_WL = lpcSpectra[uiCurr_Idx].wl(lpcSpectra[uiCurr_Idx].size() - 1);
		}
		else
			printf("File %s seems to be empty!\n",lpCurr->m_lpszFilename);
		for (unsigned int uiJ = 0; uiJ < lpcSpectra[uiCurr_Idx].size(); uiJ++)
		{
			if (lpcSpectra[uiCurr_Idx].flux(uiJ) < dMin_Flux)
				dMin_Flux = lpcSpectra[uiCurr_Idx].flux(0);
			if (lpcSpectra[uiCurr_Idx].flux(uiJ) > dMax_Flux)
				dMax_Flux = lpcSpectra[uiCurr_Idx].flux(uiJ);
		}
		lplpszInput_Filenames[uiCurr_Idx] = lpCurr->m_lpszFilename;
		lpColors[uiCurr_Idx] = lpCurr->m_eColor;
		uiCurr_Idx++;
		lpCurr = lpCurr->m_lpcNext;
		printf("%.0e %.0e\n",dMin_Flux,dMax_Flux);
	}
	printf("Plotting\n");
	sprintf(lpszOutput_Filename,"%sseries.full.eps",lpszOutput_Dir);
	PlotSeries(lpcSpectra, lpdDay, lpColors, uiNum_Spectra, lpszOutput_Filename, dMin_WL, dMax_WL, bLinear_Scale, lpszTitle, (dMin_WL + dMax_WL) * 0.5);
	sprintf(lpszOutput_Filename,"%sseries.CaNIR.eps",lpszOutput_Dir);
	PlotSeries(lpcSpectra, lpdDay, lpColors, uiNum_Spectra, lpszOutput_Filename, 7700.0, 9000.0, bLinear_Scale, lpszTitle, 7200.0);
	sprintf(lpszOutput_Filename,"%sseries.CaHK.eps",lpszOutput_Dir);
	PlotSeries(lpcSpectra, lpdDay, lpColors, uiNum_Spectra, lpszOutput_Filename, 3000.0, 4500.0, bLinear_Scale, lpszTitle, 3200.0);
	sprintf(lpszOutput_Filename,"%sseries.Si6355.eps",lpszOutput_Dir);
	PlotSeries(lpcSpectra, lpdDay, lpColors, uiNum_Spectra, lpszOutput_Filename, 5000.0, 6500.0, bLinear_Scale, lpszTitle, 5200.0);

	if (bGenerate_Individual_Spectra)
	{
		for (unsigned int uiI = 0; uiI < uiNum_Spectra; uiI++)
		{
			char lpszTitle[16];
			sprintf(lpszTitle,"%f",lpdDay[uiI]);
			sprintf(lpszOutput_Filename,"%s%s.full.eps",lpszOutput_Dir,lplpszInput_Filenames[uiI]);
			Plot_Spectrum(lpszOutput_Filename,dMin_WL, dMax_WL, lpszTitle, lpcSpectra[uiI]);
			sprintf(lpszOutput_Filename,"%s%s.CaNIR.eps",lpszOutput_Dir,lplpszInput_Filenames[uiI]);
			Plot_Spectrum(lpszOutput_Filename,7700.0, 9000.0, lpszTitle, lpcSpectra[uiI]);
			sprintf(lpszOutput_Filename,"%s%s.CaHK.eps",lpszOutput_Dir,lplpszInput_Filenames[uiI]);
			Plot_Spectrum(lpszOutput_Filename,3000.0, 4500.0, lpszTitle, lpcSpectra[uiI]);
			sprintf(lpszOutput_Filename,"%s%s.Si6355.eps",lpszOutput_Dir,lplpszInput_Filenames[uiI]);
			Plot_Spectrum(lpszOutput_Filename,5000.0, 6500.0, lpszTitle, lpcSpectra[uiI]);
		}
	}
	return 0;
}

