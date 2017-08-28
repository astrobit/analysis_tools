
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <line_routines.h>
#include <eps_plot.h>

bool Is_Leap_Year(int i_iYear)
{
	return  ((i_iYear % 4) == 0 && ((i_iYear % 400 == 0) || (i_iYear % 100 != 0)));
}

int Get_Day_Of_Year(int i_iYear, int i_iMonth, int i_iDay)
{
	int iDay = i_iDay;
	switch (i_iMonth)
	{
	case 12:
		iDay += 30; // November
	case 11:
		iDay += 31; // October
	case 10:
		iDay += 30; // September
	case 9:
		iDay += 31; // August
	case 8:
		iDay += 31; // July
	case 7:
		iDay += 30; // June
	case 6:
		iDay += 31; // May
	case 5:
		iDay += 30; // April
	case 4:
		iDay += 31; // March
	case 3:
		iDay += 28; // February
		if (Is_Leap_Year(i_iYear))
			iDay ++; // February - leap year
	case 2:
		iDay += 31; // January
	}
	return iDay;
}

void Plot(epsplot::page_parameters	& i_cPlot_Parameters, epsplot::data &i_cPlot, const char * i_lpszFilename_Format, const char * i_lpszOutput_File_Prefix, const ES::Spectrum & i_cTarget, const ES::Spectrum & i_cFit, const double & i_dTarget_Normalization_Flux, const double & i_dFit_Normalization_Flux,const double &i_dPlot_Min_WL, const double & i_dPlot_Max_WL, unsigned int uiX_Axis, unsigned int uiY_Axis)
{
	// Make sure that both spectra are within the desired plot range, otherwise don't bother plotting.
	if (((i_cTarget.wl(0) >= i_dPlot_Min_WL && i_cTarget.wl(0) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(i_cTarget.size() - 1)>= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cTarget.wl(0) <= i_dPlot_Min_WL && i_cTarget.wl(i_cTarget.size() - 1) >= i_dPlot_Max_WL)) &&
	   ((i_cFit.wl(0) >= i_dPlot_Min_WL && i_cFit.wl(0) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(i_cFit.size() - 1)>= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) <= i_dPlot_Max_WL) ||
		(i_cFit.wl(0) <= i_dPlot_Min_WL && i_cFit.wl(i_cFit.size() - 1) >= i_dPlot_Max_WL)))
	{
		double * lpdTarget_WL = NULL, *lpdTarget_Flux = NULL, *lpdFit_WL = NULL, *lpdFit_Flux = NULL;;
		unsigned int uiNum_Points_Target, uiNum_Points_Fit;
		char lpszFilename[256];

		// Get spectral data in the desired range
		Get_Spectra_Data(i_cTarget, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, i_dPlot_Min_WL, i_dPlot_Max_WL);
		Get_Spectra_Data(i_cFit, lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, i_dPlot_Min_WL, i_dPlot_Max_WL);

		// normalize spectra
	//		printf("%.2e\t%.2e\n",i_dTarget_Normalization_Flux,i_dFit_Normalization_Flux);
		Normalize(lpdTarget_Flux, uiNum_Points_Target,i_dTarget_Normalization_Flux);
		Normalize(lpdFit_Flux, uiNum_Points_Fit, i_dFit_Normalization_Flux);

		sprintf(lpszFilename,i_lpszFilename_Format,i_lpszOutput_File_Prefix);
		i_cPlot.Set_Plot_Filename(lpszFilename);

		i_cPlot.Set_Plot_Data(lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, epsplot::BLACK, epsplot::SOLID,uiX_Axis, uiY_Axis, -1.0);
		i_cPlot.Set_Plot_Data(lpdFit_WL, lpdFit_Flux, uiNum_Points_Fit, epsplot::RED, epsplot::SOLID, uiX_Axis, uiY_Axis, -1.0);
		i_cPlot.Plot(i_cPlot_Parameters);

		delete [] lpdTarget_WL;
		delete [] lpdTarget_Flux;
		delete [] lpdFit_WL;
		delete [] lpdFit_Flux;
	}
	else
	{
		printf("No plot: %.1f %.1f %.1f %.1f %.1f %.1f\n",i_dPlot_Min_WL, i_dPlot_Max_WL,i_cTarget.wl(0),i_cTarget.wl(i_cTarget.size() - 1), i_cFit.wl(0),i_cFit.wl(i_cFit.size() - 1));
	}
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	if (i_iArg_Count > 1)
	{
		if (!(i_iArg_Count & 0x01)) // even number of parameters
		{
			int iYear_BMax = atoi(i_lpszArg_Values[1]) / 10000;
			int iMonth_BMax = (atoi(i_lpszArg_Values[1]) % 10000) / 100;
			int iDay_BMax = (atoi(i_lpszArg_Values[1]) % 100);
			int iDay_Of_Year_BMax = Get_Day_Of_Year(iYear_BMax,iMonth_BMax,iDay_BMax);

			FILE * fileOut = fopen("fit.user.other.csv","wt");
			fprintf(fileOut,"model, Date, Days (Bmax), Model day, PS velocity, PS temp, PS scaling, HVF scaling");
			for (unsigned int uiI = 1; uiI <= 6; uiI++)
			{
				fprintf(fileOut,", Fit Raw Moment %i, Fit Central Moment %i, Fit Standardized Moment %i",uiI,uiI,uiI);
			}
			fprintf(fileOut,"\n");

			for (unsigned int uiI = 2; uiI < i_iArg_Count; uiI+=2)
			{
				const char * lpszDate_Cursor = i_lpszArg_Values[uiI] + 9;
				char lpszDate_String[9];
				strncpy(lpszDate_String,lpszDate_Cursor,8);
				lpszDate_String[8] = 0;
				int iYear_Curr = atoi(lpszDate_String) / 10000;
				int iMonth_Curr = (atoi(lpszDate_String) % 10000) / 100;
				int iDay_Curr = (atoi(lpszDate_String) % 100);
				int iDay_Of_Year_Curr = Get_Day_Of_Year(iYear_Curr,iMonth_Curr,iDay_Curr);

				ES::Spectrum cTarget = ES::Spectrum::create_from_ascii_file( i_lpszArg_Values[uiI] );
				ES::Spectrum cGenerated = ES::Spectrum::create_from_ascii_file( i_lpszArg_Values[uiI + 1] );
//				printf("%i %i\n",cTarget.size(),cGenerated.size());
				double * lpdRaw_Moments = NULL, *lpdCentral_Moments = NULL, *lpdStandardized_Moments = NULL;
				double dFit_Range_Min_WL = 7750.0;
				double dFit_Range_Max_WL = 8400.0;
				double dTarget_Minima_WL, dTarget_Minima_Flux, dFit_Minima_WL, dFit_Minima_Flux;

				// Find the minima of the line used for fitting
//				Find_Flux_Minimum(cTarget, dTarget_Minima_WL, dTarget_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
				Find_Blue_Maxima(cTarget, dTarget_Minima_WL, dTarget_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
		//		Find_Flux_At_WL(cFit_Info.cBest_Fit_Spectrum,dTarget_Minima_WL, dNormalization_Flux_Fit);
//				Find_Flux_Minimum(cGenerated,dFit_Minima_WL, dFit_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);
				Find_Blue_Maxima(cGenerated,dFit_Minima_WL, dFit_Minima_Flux,dFit_Range_Min_WL, dFit_Range_Max_WL);

				Get_Fit_Moments_2(cTarget, cGenerated, dFit_Range_Min_WL,dFit_Range_Max_WL, 6, lpdRaw_Moments, lpdCentral_Moments, lpdStandardized_Moments,dTarget_Minima_Flux,dFit_Minima_Flux);

				unsigned int uiStart_Idx_Target =  0, uiStart_Idx_Generated = 0;
				double * lpdTarget_WL = NULL, *lpdTarget_Flux = NULL, *lpdGenerated_WL = NULL, *lpdGenerated_Flux = NULL;
				unsigned int uiNum_Points_Target, uiNum_Points_Generated;
				Get_Spectra_Data(cTarget, lpdTarget_WL, lpdTarget_Flux, uiNum_Points_Target, dFit_Range_Min_WL, dFit_Range_Max_WL);
				Get_Spectra_Data(cGenerated, lpdGenerated_WL, lpdGenerated_Flux, uiNum_Points_Generated, dFit_Range_Min_WL, dFit_Range_Max_WL);
				while (lpdGenerated_WL[uiStart_Idx_Generated] < lpdTarget_WL[0] && uiStart_Idx_Generated < uiNum_Points_Generated)
					uiStart_Idx_Generated++;
				while (lpdTarget_WL[uiStart_Idx_Target] < lpdGenerated_WL[0] && uiStart_Idx_Target < uiNum_Points_Target)
					uiStart_Idx_Target++;
				unsigned int uiNum_Points_Error = uiNum_Points_Target - uiStart_Idx_Target;
				double * lpdError = new double[uiNum_Points_Error];
				for (unsigned int uiJ = 0; uiJ < 9; uiJ++)
				{
					double dNormalization_Target,dNormalization_Generated;
					double	dTarget_Minima_WL_Lcl, dTarget_Minima_Flux_Lcl,dFit_Minima_WL_Lcl,dFit_Minima_Flux_Lcl;
					//printf("Calc Norm\n");
					switch (uiJ)
					{
					case 0: // Individual at blue individual max
						Find_Blue_Maxima(cTarget, dTarget_Minima_WL_Lcl, dTarget_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Blue_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					case 1: // target flux at generated blue max
						Find_Blue_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_At_WL(cTarget,dFit_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dTarget_Minima_Flux_Lcl;
						break;
					case 2: // individual flux at generated blue max
						Find_Blue_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_At_WL(cTarget,dFit_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					case 3: // Individual at individual minima
						Find_Flux_Minimum(cTarget, dTarget_Minima_WL_Lcl, dTarget_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_Minimum(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					case 4: // target flux at target minimum
						Find_Flux_Minimum(cTarget,dTarget_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dTarget_Minima_Flux_Lcl;
						break;
					case 5: // individual flux at generated blue max
						Find_Flux_Minimum(cTarget,dTarget_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_At_WL(cGenerated,dTarget_Minima_WL_Lcl, dFit_Minima_Flux_Lcl);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					case 6: // Individual at blue individual max
						Find_Red_Maxima(cTarget, dTarget_Minima_WL_Lcl, dTarget_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Red_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					case 7: // target flux at generated blue max
						Find_Red_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_At_WL(cTarget,dFit_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dTarget_Minima_Flux_Lcl;
						break;
					case 8: // individual flux at generated blue max
						Find_Red_Maxima(cGenerated,dFit_Minima_WL_Lcl, dFit_Minima_Flux_Lcl,dFit_Range_Min_WL, dFit_Range_Max_WL);
						Find_Flux_At_WL(cTarget,dFit_Minima_WL_Lcl,dTarget_Minima_Flux_Lcl);
						dNormalization_Target = 1.0 / dTarget_Minima_Flux_Lcl;
						dNormalization_Generated = 1.0 / dFit_Minima_Flux_Lcl;
						break;
					}
					//printf("Norm\n");
					unsigned int uiGen_Start_Index = uiStart_Idx_Generated;
					unsigned int uiLower_Bound,uiUpper_Bound;
					for (unsigned int uiK = 0; uiK < uiNum_Points_Error; uiK++)
					{
					//printf("Bracket\n");
						Bracket_Wavelength(cGenerated, lpdTarget_WL[uiK + uiStart_Idx_Target], uiLower_Bound, uiUpper_Bound, uiGen_Start_Index);
						uiGen_Start_Index = uiUpper_Bound;
					//printf("Target\n");
						double dTarget = lpdTarget_Flux[uiK + uiStart_Idx_Target] * dNormalization_Target;
					//printf("Generated\n");
						double dGenerated = Interpolate_Flux(cGenerated, lpdTarget_WL[uiK + uiStart_Idx_Target], uiLower_Bound, uiUpper_Bound) * dNormalization_Generated;
						lpdError[uiK] = dTarget - dGenerated;
					}
					double lpdMoments[3];
					Get_Raw_Moments(lpdError, uiNum_Points_Error, 3, lpdMoments);
					printf("%i: %.2e %.2e %.2e\n",uiJ,fabs(lpdMoments[0]),fabs(lpdMoments[1]),fabs(lpdMoments[2]));
				}
				delete [] lpdError;
				delete [] lpdTarget_WL;
				delete [] lpdTarget_Flux;
				delete [] lpdGenerated_WL;
				delete [] lpdGenerated_Flux;

				double	dDay_Rel = (iDay_Of_Year_Curr - iDay_Of_Year_BMax);
				while (iYear_Curr < iYear_BMax)
				{
					if (Is_Leap_Year(iYear_Curr))
						dDay_Rel -= 366;
					else
						dDay_Rel -= 365;
					iYear_Curr++;
				}
				while (iYear_Curr > iYear_BMax)
				{
					if (Is_Leap_Year(iYear_Curr))
						dDay_Rel += 366;
					else
						dDay_Rel += 365;
					iYear_Curr--;
				}

				fprintf(fileOut,"Jeff, %s, %.0f, n/a, n/a, n/a, n/a, n/a",lpszDate_String,dDay_Rel);
				for (unsigned int uiJ = 0; uiJ < 6; uiJ++)
					fprintf(fileOut,", %.17e, %.17e, %.17e",lpdRaw_Moments[uiJ], lpdCentral_Moments[uiJ], lpdStandardized_Moments[uiJ]);
				fprintf(fileOut,"\n");

			epsplot::page_parameters	cPlot_Parameters;
			epsplot::data cPlot;

			cPlot_Parameters.m_uiNum_Columns = 1;
			cPlot_Parameters.m_uiNum_Rows = 1;
			cPlot_Parameters.m_dWidth_Inches = 11.0;
			cPlot_Parameters.m_dHeight_Inches = 8.5;

			unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Wavelength [A]", false, false, false, 0.0, false, 0.0);
			unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Intensity", false, false, false, 5.0, false, 60.0);
//			cPlot.Set_Num_Plots(2);
			cPlot.Set_Plot_Title(NULL);//bNo_Title ? NULL : lpszTitle,18.0);


//			printf("Plot full\n");
			Plot(cPlot_Parameters, cPlot, "%s.full.eps", i_lpszArg_Values[uiI + 1], cTarget, cGenerated, dTarget_Minima_Flux, dFit_Minima_Flux,cTarget.wl(0), cTarget.wl(cTarget.size() - 1), uiX_Axis, uiY_Axis);

//			printf("Plot Ca NIR\n");
			Plot(cPlot_Parameters, cPlot, "%s.CaNIR.eps", i_lpszArg_Values[uiI + 1], cTarget, cGenerated, dTarget_Minima_Flux, dFit_Minima_Flux,7000.0, 9000.0, uiX_Axis, uiY_Axis);

//			printf("Plot Ca H&K\n");
			Plot(cPlot_Parameters, cPlot, "%s.CaHK.eps", i_lpszArg_Values[uiI + 1], cTarget, cGenerated, dTarget_Minima_Flux, dFit_Minima_Flux,3000.0, 5000.0, uiX_Axis, uiY_Axis);

//			printf("Plot Si 6355\n");
			Plot(cPlot_Parameters, cPlot, "%s.Si6355.eps", i_lpszArg_Values[uiI + 1], cTarget, cGenerated, dTarget_Minima_Flux, dFit_Minima_Flux,5000.0, 7000.0, uiX_Axis, uiY_Axis);
			}
			fclose(fileOut);
		}
	}
	return 0;
}

