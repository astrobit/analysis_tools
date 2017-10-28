#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xstdlib.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <float.h>
#include <best_fit_data.h>

double	Compute_Velocity(const double &i_dMeasured_WL, const double & i_dReference_WL)
{
	double	dZ = i_dMeasured_WL / i_dReference_WL - 1.0;
	double	dZ_p1_sqr = (dZ + 1.0) * (dZ + 1.0);
	double dVelocity = (dZ_p1_sqr - 1) / (dZ_p1_sqr + 1) * 300.0;
	return dVelocity;
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	BEST_FIT_DATA	cFit;
	char	lpszFit_Region[16];
	int 	iDate;
	char	lpszInstrument[16];
	char	lpszSupernova_ID[32];
	int		iRun;
	double	dMin_WL,dMax_WL,dRef_WL;
	
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--SNid", lpszSupernova_ID, 32, NULL);
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--fit-region", lpszFit_Region, 16, NULL);
	iDate = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--date", 0);
	xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--instrument", lpszInstrument, 16, NULL);
	iRun = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--run", 0);
	
	bool bFit_Region_Valid = (strcmp(lpszFit_Region,"CaNIR") == 0 || strcmp(lpszFit_Region,"CaHK") == 0 || strcmp(lpszFit_Region,"Si6355") == 0 || strcmp(lpszFit_Region,"OI8446") == 0);
	bool bDate_Valid = (iDate / 10000) > 1900 && (iDate / 10000) < 2100 && (iDate % 100) <= 31 && ((iDate / 100) % 100) >= 1 && ((iDate / 100) % 100) <= 12;
	if (lpszSupernova_ID[0] != 0 && bFit_Region_Valid && bDate_Valid && lpszInstrument[0] != 0)
	{
		char lpszFilename[256];
		sprintf(lpszFilename, "%s/%s-%i-%s.dat.fit.run%i.%s.data.user.databin", lpszFit_Region, lpszSupernova_ID, iDate, lpszInstrument, iRun, lpszFit_Region);
		if (strcmp(lpszFit_Region,"CaNIR") == 0)
		{
			dMin_WL = 7300.0;
			dMax_WL = 8300.0;
//			dRef_WL = (8203.97 * pow(10.0,0.31) + 8251.07 * pow(10,0.57) + 8257.0 * pow(10.0,-0.39)) / (pow(10.0,0.31) + pow(10.0,0.57) + pow(10.0,-0.39));
			dRef_WL = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		}
		else if (strcmp(lpszFit_Region,"CaHK") == 0)
		{
			dMin_WL = 3000.0;
			dMax_WL = 4000.0;
			dRef_WL = (3934.777 * pow(10.0,0.135) + 3969.592 * pow(10,-0.18)) / (pow(10.0,0.135) + pow(10.0,-0.18));
		}
		else if (strcmp(lpszFit_Region,"Si6355") == 0)
		{
			dMin_WL = 5400.0;
			dMax_WL = 6400.0;
			dRef_WL = (5959.21 * pow(10.0,-0.225) + 5980.59 * pow(10,0.084)) / (pow(10.0,-0.225) + pow(10.0,0.084));
		}

		cFit.ReadBinary(lpszFilename);

		if ((cFit.cBest_Fit_Spectrum.size() > 0) &&
			(cFit.cBest_Fit_Spectrum_No_PS.size() > 0) &&
			(cFit.cBest_Fit_Spectrum_No_HVF.size() > 0)) // confirm that this has the no PSF and no HVF spectra available
		{
			sprintf(lpszFilename, "Temp/%s-%i-%s.dat.tempfit.csv", lpszSupernova_ID, iDate, lpszInstrument);
			ES::Spectrum cContinuum = ES::Spectrum::create_from_ascii_file( lpszFilename );
		

			if (cContinuum.wl(0) == cFit.cBest_Fit_Spectrum_No_PS.wl(0) &&
				cContinuum.wl(0) == cFit.cBest_Fit_Spectrum_No_HVF.wl(0) &&
				cContinuum.size() == cFit.cBest_Fit_Spectrum_No_PS.size() &&
				cContinuum.size() == cFit.cBest_Fit_Spectrum_No_HVF.size())
			{ // check to ensure size and 1st wl entry are identical.  Probably safe to assume everything else is good after this
				sprintf(lpszFilename, "%s/%s-%i-%s.dat.fit.run%i.%s.data.user.flattened.csv", lpszFit_Region, lpszSupernova_ID, iDate, lpszInstrument, iRun, lpszFit_Region);
				FILE * fileOut = fopen(lpszFilename,"wt");
				if (fileOut)
				{
					fprintf(fileOut,"wl, Cont, PSF, HVF, PSF flat, HVF flat\n");
					double	dMin_Flux[4] = {DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX},dMin_Flux_WL[4];
					for (unsigned int uiI = 0; uiI < cContinuum.size(); uiI++)
					{
						double dCont_Flux = cContinuum.flux(uiI);
						double dInv_Cont_Flux = 1.0 / dCont_Flux;
						fprintf(fileOut, "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f\n",cContinuum.wl(uiI),cContinuum.flux(uiI),cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI), cFit.cBest_Fit_Spectrum_No_PS.flux(uiI), cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI) * dInv_Cont_Flux,cFit.cBest_Fit_Spectrum_No_PS.flux(uiI) * dInv_Cont_Flux);
						if (cFit.cBest_Fit_Spectrum_No_HVF.wl(uiI) >= dMin_WL && cFit.cBest_Fit_Spectrum_No_HVF.wl(uiI) <= dMax_WL)
						{
							if (cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI) < dMin_Flux[0])
							{
								dMin_Flux[0] = cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI);
								dMin_Flux_WL[0] = cFit.cBest_Fit_Spectrum_No_HVF.wl(uiI);
							}
							if (cFit.cBest_Fit_Spectrum_No_PS.flux(uiI) < dMin_Flux[1])
							{
								dMin_Flux[1] = cFit.cBest_Fit_Spectrum_No_PS.flux(uiI);
								dMin_Flux_WL[1] = cFit.cBest_Fit_Spectrum_No_PS.wl(uiI);
							}
							if (cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI) * dInv_Cont_Flux < dMin_Flux[2])
							{
								dMin_Flux[2] = cFit.cBest_Fit_Spectrum_No_HVF.flux(uiI) * dInv_Cont_Flux;
								dMin_Flux_WL[2] = cFit.cBest_Fit_Spectrum_No_HVF.wl(uiI);
							}
							if (cFit.cBest_Fit_Spectrum_No_PS.flux(uiI) * dInv_Cont_Flux < dMin_Flux[3])
							{
								dMin_Flux[3] = cFit.cBest_Fit_Spectrum_No_PS.flux(uiI) * dInv_Cont_Flux;
								dMin_Flux_WL[3] = cFit.cBest_Fit_Spectrum_No_PS.wl(uiI);
							}
						}
					}
					fclose(fileOut);
					printf("C PSF: %.4f %.4f %.2f\n",dMin_Flux_WL[0],dMin_Flux[0],Compute_Velocity(dMin_Flux_WL[0],dRef_WL));
					printf("C HVF: %.4f %.4f %.2f\n",dMin_Flux_WL[1],dMin_Flux[1],Compute_Velocity(dMin_Flux_WL[1],dRef_WL));
					printf("F PSF: %.4f %.4f %.2f\n",dMin_Flux_WL[2],dMin_Flux[2],Compute_Velocity(dMin_Flux_WL[2],dRef_WL));
					printf("F HVF: %.4f %.4f %.2f\n",dMin_Flux_WL[3],dMin_Flux[3],Compute_Velocity(dMin_Flux_WL[3],dRef_WL));
				}
			}
		}
	}
	else
	{
		fprintf(stderr,"Usage: genfs --fit-region=<fit region> --date=yyyymmdd --instrument=<instrument> --SNid=<supernova ID> --run=<run #>\n\tValid fit regions: CaNIR, CaHK, Si6355, OI8446\n");
	}
	return 0;
}
