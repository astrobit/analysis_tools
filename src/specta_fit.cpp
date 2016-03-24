
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
#include "ES_Generic_Error.hh"
#include <Plot_Utilities.h>
#include <best_fit_data.h>
#include <float.h>
#include <line_routines.h>

#define VERSION "0.2"
void Usage(const char * i_lpszError_Text)
{
		fprintf(stderr,"spectrafit v%s\n",VERSION);
		if (i_lpszError_Text)
			fprintf(stderr,"\nError: %s\n\n",i_lpszError_Text);
		fprintf(stderr,"Usage: \n\tspectrafit <Source_spectrum> <Fit_region> <Test_spectra> [options]\n");
		fprintf(stderr,"Source_spectrum: file containing wavelength, flux, and flux error.\n");
		fprintf(stderr,"\tMust be space, tab, or column separated.  If flux error is zero,\n");
		fprintf(stderr,"\tvariance will be used instead of chi^2.\n");
		fprintf(stderr,"Fit_region: Line for which to do the fit.  Options are 'CaNIR', 'CaHK',\n");
		fprintf(stderr,"\tor 'Si6355'.\n");
//		fprintf(stderr,"\tor 'Si6355' or 'Full'.\n");  // Full currently disabled
		fprintf(stderr,"Test_spectra: List of opactiy map(s) to use.\n");
		fprintf(stderr,"\n");
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t--time=X\n\t\tSpecify the suggested time after the explosion for which to fit the spectra\n");
		fprintf(stderr,"Output: name of opacity map with best fit, as well as the chi^2 or variance\n");
		fprintf(stderr,"\tof the fit, the day number, photosphere temperature, ion excitation\n");
		fprintf(stderr,"\tand the log opacity reference used.\n");
}




void SavePointsCache(const char * i_lpszFilename, const XVECTOR * lpvPoints, unsigned int uiNum_Points, unsigned int uiNum_Parameters, const double * lpdFit)
{
	if (i_lpszFilename && i_lpszFilename[0] != 0)
	{
		XDATASET	cDataset;
		cDataset.Allocate(uiNum_Parameters + 1,uiNum_Points);
		for (unsigned int uiI = 0; uiI < uiNum_Parameters; uiI++)
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
				cDataset.SetElement(uiI,uiJ,lpvPoints[uiJ].Get(uiI));
		for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			cDataset.SetElement(uiNum_Parameters,uiJ,lpdFit[uiJ]);
		cDataset.SaveDataFileBin(i_lpszFilename,true);
	}
}
bool ReadPointsCache(const char * i_lpszFilename, XVECTOR * lpvPoints, unsigned int uiNum_Points, unsigned int uiNum_Parameters, double * lpdFit)
{
	bool	bRet = false;
	if (i_lpszFilename && i_lpszFilename[0] != 0)
	{
		XDATASET	cDataset;
		cDataset.ReadDataFileBin(i_lpszFilename,true);
		if (uiNum_Points == cDataset.GetNumElements() && (uiNum_Parameters + 1) == cDataset.GetNumColumns())
		{
			bRet = true;
			for (unsigned int uiI = 0; uiI < uiNum_Parameters; uiI++)
				for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
				{
					//printf("%.1f\t",cDataset.GetElement(uiI,uiJ));
					if (uiI == 0)
						lpvPoints[uiJ].Set_Size(uiNum_Parameters);

					lpvPoints[uiJ].Set(uiI,cDataset.GetElement(uiI,uiJ));
				}
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
				lpdFit[uiJ] = cDataset.GetElement(uiNum_Parameters,uiJ);
		}
	}
	return bRet;
}


void xPerform_Fit_Bound_Simplex(const XVECTOR &i_vLower_Bound,const XVECTOR &i_vUpper_Bound, const XVECTOR & i_lpvFit_Threshold, QFunctionV i_fUser_Function, XVECTOR & o_vResult, void * i_lpvUser_Data, const char * i_lpszCache_Name)
{
	unsigned int uiNum_Parameters = i_vLower_Bound.Get_Size();
	if (uiNum_Parameters != i_vUpper_Bound.Get_Size() || uiNum_Parameters != i_lpvFit_Threshold.Get_Size())
		fprintf(stderr,"xPerform_Fit_Bound: bound and theshold size doesn't match\n");
	else
	{
		double	dFit_Min = DBL_MAX;
		double	dFit_Max = 0.0;
		unsigned int uiFit_Max_Point = 0;
		double	*lpdFit;
		unsigned int uiNum_Points = uiNum_Parameters + 1;
		// define tetrahedron spanning the search space
		XVECTOR *lpcTest_Points;
		XVECTOR cCentroid(uiNum_Parameters);
		XVECTOR cTestOuter(uiNum_Parameters);
		XVECTOR cTestInner(uiNum_Parameters);
		XVECTOR cTest(uiNum_Parameters);
		XVECTOR cDelta(uiNum_Parameters);
		XVECTOR	vRef_Point(uiNum_Parameters);
		double	dScaling;
		XVECTOR cBounds[2];
		double dTest_Fit = 0.0;
		char	lpszCache_Filename[256];
		sprintf(lpszCache_Filename,"%s.cache",i_lpszCache_Name);

		lpcTest_Points = new XVECTOR[uiNum_Points];
		lpdFit = new double[uiNum_Points];
		for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
		{
			lpcTest_Points[uiI].Set_Size(uiNum_Parameters);
		}

		cBounds[0] = i_vLower_Bound;
		cBounds[1] = i_vUpper_Bound;
		//if (!i_lpszCache_Name)// || !ReadPointsCache(lpszCache_Filename,lpcTest_Points,uiNum_Points,uiNum_Parameters,lpdFit))
		{
			unsigned int uiNum_Vertices = 1 << uiNum_Parameters;

			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
				lpdFit[uiI] = DBL_MAX;

			double	dFit_Sum = 0.0;
			double * lpdVertex_Fits = new double[uiNum_Vertices];
			// test all vertices of the cuboid that spans the search space
			for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
			{
				cTest = cBounds[0];
				for (unsigned int uiJ = 0; uiJ < uiNum_Parameters; uiJ++)
				{
					if (uiI & (1 << uiJ)) 
						cTest.Set(uiJ,cBounds[1].Get(uiJ));
				}
				fprintf(stdout,"-");
				fflush(stdout);
				lpdVertex_Fits[uiI] = dTest_Fit = i_fUser_Function(cTest,i_lpvUser_Data);
//				fprintf(stdout,"%.2e\t",dTest_Fit);
//				fflush(stdout);

				// Now see if the fit is better than one of the existing fits. This will generate a simplex with the 8 best points in the cuboid
				bool bDone = false;
				for (unsigned int uiJ = 0; uiJ < uiNum_Points && !bDone; uiJ++)
				{
					if (dTest_Fit < lpdFit[uiJ])
					{
						lpcTest_Points[uiJ] = cTest;
						lpdFit[uiJ] = dTest_Fit;
						bDone = true;
					}
				}
			}
//			fprintf(stdout,"P");
//			fflush(stdout);
			bool * lpbParameter_Variation = new bool[uiNum_Parameters];
			memset(lpbParameter_Variation,0,sizeof(bool) * uiNum_Parameters);
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				for (unsigned int uiK = uiJ; uiK < uiNum_Points; uiK++)
				{
					for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
					{
						lpbParameter_Variation[uiL] |= (fabs(lpcTest_Points[uiJ].Get(uiL) - lpcTest_Points[uiK].Get(uiL)) > 0.0);
					}
				}
			}
			unsigned int uiCount = 0;
			for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
			{
				if (!lpbParameter_Variation[uiL])
					uiCount++;
			}
			unsigned int uiFit_Mode_Max = 0;
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				unsigned int uiMode = 0;
				for (unsigned int uiK = 0; uiK < uiNum_Points; uiK++)
				{
				if (uiK != uiJ && lpdFit[uiK] == lpdFit[uiJ])
					uiMode++;
				}
				if (uiMode > uiFit_Mode_Max)
					uiFit_Mode_Max = uiMode;
			}
			//printf("%i %i %i\n",uiNum_Parameters, uiNum_Points, uiFit_Mode_Max);
			if (uiNum_Parameters > (uiNum_Points- uiFit_Mode_Max))
			{
				// can't replace using best points; just make sure we span
				for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
				{
					if (!lpbParameter_Variation[uiL])
					{
						double dVal = cBounds[0].Get(uiL);
						if (lpcTest_Points[0].Get(uiL) == dVal)
							dVal = cBounds[1].Get(uiL);
						lpcTest_Points[uiL].Set(uiL,dVal); // set the Lth point to the opposite bound
					}
				}
			}
			else
			{
				bool * lpbReplaced = new bool [uiNum_Points];
				memset(lpbReplaced,0,sizeof(bool) * uiNum_Points);
				while (uiCount != 0)
				{ 
//					printf("\tC%i",uiCount);
					dFit_Max = 0.0;
					for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
					{
						if (!lpbReplaced[uiI] && lpdFit[uiI] >= dFit_Max) // check points that haven't been replace for worst fit
						{
							dFit_Max = lpdFit[uiI];
							uiFit_Max_Point = uiI;
						}
					}
//					printf("\tM%i",uiFit_Max_Point);
					// replace the worst point with the best fit vertex that has variation in the desired parameters
					unsigned int uiI = 0;
					while (lpbParameter_Variation[uiI])
						uiI++;
//					printf("\tI%i",uiI);
					// uiI is now the index of the parameter that has no variation
					// find out if the parameter is limited to the lower or upper bound
					bool bLower = (lpcTest_Points[0].Get(uiI) == cBounds[0].Get(uiI));
					unsigned int uiBit_Isolate = 1 << uiI;
					unsigned int uiTest = bLower ? uiBit_Isolate : 0;
					double	dBest_Fit = DBL_MAX;
					unsigned int uiBest_Fit_Idx;
					for (unsigned int uiJ = 0; uiJ < uiNum_Vertices; uiJ++)
					{
						if (!((uiJ & uiBit_Isolate) ^ uiTest))
						{
							if (lpdVertex_Fits[uiJ] < dBest_Fit)
							{
								dBest_Fit = lpdVertex_Fits[uiJ];
								uiBest_Fit_Idx = uiJ;
							}
						}
					}
//					printf("\tB%i\n",uiBest_Fit_Idx);
					lpdFit[uiFit_Max_Point] = dBest_Fit;
					// Set the coordinates to the vertex
//					printf("P: %.3f %.3f %.3f %.3f %.3f %.1e\n",lpcTest_Points[uiFit_Max_Point].Get(0),lpcTest_Points[uiFit_Max_Point].Get(1),lpcTest_Points[uiFit_Max_Point].Get(2),lpcTest_Points[uiFit_Max_Point].Get(3),lpcTest_Points[uiFit_Max_Point].Get(4),dBest_Fit);
					for (unsigned int uiJ = 0; uiJ < uiNum_Parameters; uiJ++)
					{
						if (uiBest_Fit_Idx & (1 << uiJ)) 
							lpcTest_Points[uiFit_Max_Point].Set(uiJ,cBounds[1].Get(uiJ));
						else
							lpcTest_Points[uiFit_Max_Point].Set(uiJ,cBounds[0].Get(uiJ));
					}
//					printf("P: %.3f %.3f %.3f %.3f %.3f\n",lpcTest_Points[uiFit_Max_Point].Get(0),lpcTest_Points[uiFit_Max_Point].Get(1),lpcTest_Points[uiFit_Max_Point].Get(2),lpcTest_Points[uiFit_Max_Point].Get(3),lpcTest_Points[uiFit_Max_Point].Get(4));
					lpbReplaced[uiFit_Max_Point] = true;
					memset(lpbParameter_Variation,0,sizeof(bool) * uiNum_Parameters);
					for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
					{
						for (unsigned int uiK = uiJ; uiK < uiNum_Points; uiK++)
						{
							for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
							{
								lpbParameter_Variation[uiL] |= (fabs(lpcTest_Points[uiJ].Get(uiL) - lpcTest_Points[uiK].Get(uiL)) > 0.0);
							}
						}
					}
					uiCount = 0;
					for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
					{
						if (!lpbParameter_Variation[uiL])
							uiCount++;
					}
				}
				delete [] lpbReplaced;
				delete [] lpbParameter_Variation;
				delete [] lpdVertex_Fits;

				SavePointsCache(lpszCache_Filename,lpcTest_Points,uiNum_Points,uiNum_Parameters,lpdFit);
			}
			uiFit_Max_Point = 0;
		}
		// The method used below is basically simplex optimization, with a few modifications to handle bad data points and a contrained parameter space
//		while ((uiFit_Max_Point < uiNum_Parameters) && !Convergence(lpcTest_Points, uiNum_Parameters , i_lpvFit_Threshold))
		while (!xConvergence(lpcTest_Points, uiNum_Points , i_lpvFit_Threshold))
		{
			fprintf(stdout,".");
			fflush(stdout);
	//		fprintf(stdout,"F: %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",cConvergence_Fault.Get(0),cConvergence_Fault.Get(1),cConvergence_Fault.Get(2),cConvergence_Fault.Get(3),cConvergence_Fault.Get(4),cConvergence_Fault.Get(5),cConvergence_Fault.Get(6));

			// Find point with worst fit
			dFit_Max = 0.0;
			double	dFit_Near_Max = 0.0;
			double	dFit_Mean = 0.0;
			double 	dFit_Variance = 0.0;
			double	dFit_Std_Dev;
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				if (lpdFit[uiI] > dFit_Max)
				{
					dFit_Max = lpdFit[uiI];
					uiFit_Max_Point = uiI;
				}
				dFit_Mean += lpdFit[uiI];
				dFit_Variance += (lpdFit[uiI] * lpdFit[uiI]);
			}
			dFit_Mean /= uiNum_Points;
			dFit_Variance /= uiNum_Points;
			dFit_Std_Dev = sqrt(dFit_Variance - dFit_Mean * dFit_Mean);
			// find seceond worst fit - this is the threshold for adjusting the worst point
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				if (uiFit_Max_Point != uiI && lpdFit[uiI] > dFit_Near_Max)
				{
					dFit_Near_Max = lpdFit[uiI];
				}
			}
			//fprintf(stdout,"\nF: %.8e %.8e %.8e %.8e %.8e %.8e %.8e\n",lpdFit[0],lpdFit[1],lpdFit[2],lpdFit[3],lpdFit[4],lpdFit[5],lpdFit[6],lpdFit[7]);
			// If the fit is relatively uniform, contract
			if ((dFit_Max /  dFit_Mean - 1.0) < 0.005 || dFit_Mean < 10.0)
			{
				cCentroid = xCompute_Centroid(lpcTest_Points,uiNum_Points);
				// Scale the simplex by 1/2
				for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
				{
					cDelta = lpcTest_Points[uiI] - cCentroid;
					lpcTest_Points[uiI] = cCentroid + 0.5 * cDelta;
					fprintf(stdout,"*");
					fflush(stdout);
					lpdFit[uiI] = i_fUser_Function(lpcTest_Points[uiI],i_lpvUser_Data);
				}
			}
			else
			{
//				fprintf(stdout,"%i",uiFit_Max_Point);
//				fprintf(stdout,"\nF: %.8e %.8e %.8e %.8e %.8e %.8e %.8e\n",lpdFit[0],lpdFit[1],lpdFit[2],lpdFit[3],lpdFit[4],lpdFit[5],lpdFit[6],lpdFit[7]);
				// Compute centroid of remaining parameters
				for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
					cCentroid.Set(uiI,0.0);

				for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
				{
					if (uiI != uiFit_Max_Point)
						cCentroid += lpcTest_Points[uiI];
				}
				cCentroid /= (double)(uiNum_Points - 1);
	//			double dCentroid_Fit = i_fUser_Function(cCentroid,i_lpvUser_Data);
				// Generate new test position
				double	dFit_Min = DBL_MAX;
				for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
				{
					if (lpdFit[uiI] < dFit_Min)
					{
						dFit_Min = lpdFit[uiI];
					}
				}

				double	dMod = -0.5;
				double	dMod_Lcl;
				XVECTOR cDelta = lpcTest_Points[uiFit_Max_Point] - cCentroid;
				XVECTOR cDeltaTemp;
				double dFit_Test;
				XVECTOR cTest;
				cTest = cCentroid - cDelta;
				cDeltaTemp = cDelta;
				while (!xBoundTest(cTest,cBounds))
				{
					fprintf(stdout,"B");
					fflush(stdout);
					// try to fit the point within the bounds
					double	dMax_Whatever = 0.0;
					unsigned int uiLargest_Whatever;
					// figure out which point is most outside the box
					for (unsigned int uiI = 0; uiI < uiNum_Parameters; uiI++)
					{
						double	dRange = cBounds[1].Get(uiI) - cBounds[0].Get(uiI);
						double	dDelta = -1.0; // 
						if (cTest.Get(uiI) < cBounds[0].Get(uiI))
							dDelta = cBounds[0].Get(uiI) - cTest.Get(uiI);
						else if (cTest.Get(uiI) > cBounds[1].Get(uiI))
							dDelta = cTest.Get(uiI) - cBounds[1].Get(uiI);

						double	dWhatever = (dDelta / dRange);
						if (dWhatever > dMax_Whatever)
						{
							uiLargest_Whatever = uiI;
							dMax_Whatever = dWhatever;
						}
					}
					cDeltaTemp.Normalize();
					double dScalar = 1.0;
					// scale delta so that point will be within the box
					if (cTest.Get(uiLargest_Whatever) < cBounds[0].Get(uiLargest_Whatever))
						dScalar = cCentroid.Get(uiLargest_Whatever) - cBounds[0].Get(uiLargest_Whatever);
					else if (cTest.Get(uiLargest_Whatever) > cBounds[1].Get(uiLargest_Whatever))
						dScalar = cBounds[1].Get(uiLargest_Whatever) - cCentroid.Get(uiLargest_Whatever);
					cDeltaTemp *= dScalar;
					cTest = cCentroid - cDeltaTemp;
			
				}
				dFit_Test = i_fUser_Function(cTest,i_lpvUser_Data);

				if (dFit_Test >= lpdFit[uiFit_Max_Point])
				{
					fprintf(stdout,"#");
					fflush(stdout);
					cTest = cCentroid + cDelta * 0.5;
					dFit_Test = i_fUser_Function(cTest,i_lpvUser_Data);
				}

				if (dFit_Test < dFit_Near_Max)
				{
					fprintf(stdout,"+");
					fflush(stdout);
//					printf("%.2f",dFit_Test);
					lpdFit[uiFit_Max_Point] = dFit_Test;
					lpcTest_Points[uiFit_Max_Point] = cTest;
				}
				else
				{
					// no success; contract the simplex toward the best point
					double dBest_Fit = DBL_MAX;
					unsigned int uiBest_Fit_Idx;
					for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
					{
						if (lpdFit[uiI] < dBest_Fit)
						{
							dBest_Fit = lpdFit[uiI];
							uiBest_Fit_Idx = uiI;
						}
					}
					cCentroid = xCompute_Centroid(lpcTest_Points, uiNum_Points);
					dFit_Test = i_fUser_Function(cCentroid,i_lpvUser_Data);
					if (dFit_Test < dBest_Fit) // Centroid is better fit than best of test points, contract toward centroid
					{
						vRef_Point = cCentroid;
						dScaling = 0.5;
					}
					else
					{
						vRef_Point = lpcTest_Points[uiBest_Fit_Idx];
						dScaling = 0.75;
					}

					// Scale the simplex by 1/2
					for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
					{
						if (uiBest_Fit_Idx != uiI || dFit_Test < dBest_Fit)
						{
							cDelta = lpcTest_Points[uiI] - vRef_Point;
							lpcTest_Points[uiI] = vRef_Point + dScaling * cDelta;
							fprintf(stdout,"@");
							fflush(stdout);
							//printf("T: %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",lpcTest_Points[uiI].Get(0),lpcTest_Points[uiI].Get(1),lpcTest_Points[uiI].Get(2),lpcTest_Points[uiI].Get(3),lpcTest_Points[uiI].Get(4),lpcTest_Points[uiI].Get(5),lpcTest_Points[uiI].Get(6));
							lpdFit[uiI] = i_fUser_Function(lpcTest_Points[uiI],i_lpvUser_Data);
						}
					}
				}
			}
			SavePointsCache(lpszCache_Filename,lpcTest_Points,uiNum_Points,uiNum_Parameters,lpdFit);
		}
		fprintf(stdout,"X");
		fflush(stdout);
		o_vResult = xCompute_Centroid(lpcTest_Points, uiNum_Points);

	}
}


class SPECTRA_FIT_DATA_BASE
{
public:
	unsigned int uiIon;
	const ES::Spectrum *lpcTarget;
	double dMin_WL;
	double dMax_WL;
	bool bChi2;
	const char * lpszModel_Name;

};

class SPECTRA_FIT_DATA : public SPECTRA_FIT_DATA_BASE
{
public:
	XDATASET *lpcOpacity_Map_A;
	XDATASET *lpcOpacity_Map_B;
};

char g_lpszVector_Data_Filename[256] = {""};
XDATASET	g_cFit_Data_Vectors;
XDATASET	g_cFit_Data_Output;
bool Read_Vector_Output_Cache(const XVECTOR & i_vX, SPECTRA_FIT_DATA_BASE * lpcCall_Data, ES::Spectrum &cOutput)
{
	bool bRet = false;
	char lpszVectorlist_Filename[256];
	char lpszOutput_Filename[256];
	sprintf(lpszVectorlist_Filename,".sfdb/.spectrafit.0.2.%s.%i.vectorlist",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	sprintf(lpszOutput_Filename,".sfdb/.spectrafit.0.2.%s.%i.outputdata",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	if (strcmp(g_lpszVector_Data_Filename,lpszVectorlist_Filename) != 0)
	{
		strcpy(g_lpszVector_Data_Filename,lpszVectorlist_Filename);
		g_cFit_Data_Vectors.ReadDataFileBin(lpszVectorlist_Filename,true);
		g_cFit_Data_Output.ReadDataFileBin(lpszOutput_Filename,true);
		printf("Reading new vector file\n");
	}
	// Make sure that the vector data file and parameter set are the same size
	if (g_cFit_Data_Vectors.GetNumElements() != i_vX.Get_Size())
	{
		g_cFit_Data_Vectors.Deallocate();
		g_cFit_Data_Output.Deallocate();
	}
	// Make sure that the output data file and target set are the same size
	if (g_cFit_Data_Output.GetNumElements() != lpcCall_Data->lpcTarget[0].size())
	{
		g_cFit_Data_Vectors.Deallocate();
		g_cFit_Data_Output.Deallocate();
	}
	else
	{
		bool bTest = true;
		unsigned int uiI;
		for (uiI = 0; uiI < lpcCall_Data->lpcTarget[0].size() && bTest; uiI++)
		{
			bTest = (lpcCall_Data->lpcTarget[0].wl(uiI) == g_cFit_Data_Output.GetElement(0,uiI));
		}
		if (!bTest)
		{
			g_cFit_Data_Vectors.Deallocate();
			g_cFit_Data_Output.Deallocate();
		}
	}
	// check to see if this particular vector has already been processed.
	unsigned int uiFound = g_cFit_Data_Vectors.GetNumColumns();
	for (unsigned int uiI = 0; uiI < g_cFit_Data_Vectors.GetNumColumns() && uiFound == g_cFit_Data_Vectors.GetNumColumns(); uiI++)
	{
		bool bTest = true;
		for (unsigned int uiJ = 0; uiJ < i_vX.Get_Size() && bTest; uiJ++)
		{
			bTest = (i_vX.Get(uiJ) == g_cFit_Data_Vectors.GetElement(uiI,uiJ));
		}
		if (bTest)
			uiFound = uiI;	
	}
	if (uiFound < g_cFit_Data_Vectors.GetNumColumns())
	{
		for (unsigned int uiI = 0; uiI < cOutput.size(); uiI++)
		{
			cOutput.flux(uiI) = g_cFit_Data_Output.GetElement(uiFound + 1,uiI);
		}
		bRet = true;
	}
	return bRet;
}

void Save_Vector_Output_Cache(const XVECTOR & i_vX, SPECTRA_FIT_DATA_BASE * lpcCall_Data, ES::Spectrum &cOutput)
{
	char lpszVectorlist_Filename[256];
	char lpszOutput_Filename[256];
	sprintf(lpszVectorlist_Filename,".sfdb/.spectrafit.0.2.%s.%i.vectorlist",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	sprintf(lpszOutput_Filename,".sfdb/.spectrafit.0.2.%s.%i.outputdata",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	unsigned int uiAdd_Column = g_cFit_Data_Vectors.GetNumColumns();
	if (uiAdd_Column == 0)
	{	
		g_cFit_Data_Vectors.Allocate(1,i_vX.Get_Size());
		g_cFit_Data_Output.Allocate(2,lpcCall_Data->lpcTarget[0].size());
		for (unsigned int uiI = 0; uiI < lpcCall_Data->lpcTarget[0].size(); uiI++)
			g_cFit_Data_Output.SetElement(0,uiI,lpcCall_Data->lpcTarget[0].wl(uiI));
	}
	else
	{
		g_cFit_Data_Vectors.Add_Columns(1);
		g_cFit_Data_Output.Add_Columns(1);
	}
	for (unsigned int uiI = 0; uiI < i_vX.Get_Size(); uiI++)
		g_cFit_Data_Vectors.SetElement(uiAdd_Column,uiI,i_vX.Get(uiI));
	for (unsigned int uiI = 0; uiI < cOutput.size(); uiI++)
		g_cFit_Data_Output.SetElement(uiAdd_Column + 1,uiI,cOutput.flux(uiI));
	g_cFit_Data_Vectors.SaveDataFileBin(lpszVectorlist_Filename);
	g_cFit_Data_Output.SaveDataFileBin(lpszOutput_Filename);

}

double Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
	SPECTRA_FIT_DATA *lpcCall_Data = (SPECTRA_FIT_DATA *)i_lpvSpectra_Fit_Data;
	ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->lpcTarget->wl(0), lpcCall_Data->lpcTarget->wl(lpcCall_Data->lpcTarget->size() - 1), lpcCall_Data->lpcTarget->size());

	// look to see if this particular vector has been processed
	if (!Read_Vector_Output_Cache(i_vX, lpcCall_Data, cOutput))
	{
		XVECTOR vX;
		if (i_vX.Get_Size() == 5)
		{
			vX.Set_Size(7);
			vX.Set(5,i_vX.Get(3)); // HVF scalar
			vX.Set(6,i_vX.Get(2)); // HVF temp (use PS temp)
		}
		else
		{
			vX.Set_Size(5);
		}
		vX.Set(0,i_vX.Get(0)); // day
		vX.Set(1,i_vX.Get(1)); // PS vel
		vX.Set(2,i_vX.Get(2)); // PS temp
		vX.Set(3,i_vX.Get(4)); // PS LGR scalar
		vX.Set(4,i_vX.Get(2)); // PS LGR temp (use PS temp)

		Generate_Synow_Spectra(lpcCall_Data->lpcTarget[0],lpcCall_Data->lpcOpacity_Map_A[0],lpcCall_Data->lpcOpacity_Map_B[0],lpcCall_Data->uiIon,i_vX,cOutput);
		Save_Vector_Output_Cache(i_vX, lpcCall_Data, cOutput);
	}
//	fprintf(stdout,".");
//	fflush(stdout);
	double dFit = Fit(lpcCall_Data->lpcTarget[0], cOutput, lpcCall_Data->dMin_WL, lpcCall_Data->dMax_WL, lpcCall_Data->bChi2 ? FIT_CHI2 : FIT_CENTRAL , 2);
//	fprintf(stdout,"_");
//	fflush(stdout);
	return dFit;
}

double CompareFits(const char * i_lpszTarget_File, const char * i_lpszFileSource, const ES::Spectrum &i_cTarget, XDATASET & i_cOpacity_Map_A, XDATASET & i_cOpacity_Map_B, unsigned int i_uiIon, const double &i_dMin_WL, const double &i_dMax_WL, bool i_bChi2, BEST_FIT_DATA & o_cFit, const double &dSuggested_Time = 0.0)
{
	char lpszCache_Filename[256];
	ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( i_cTarget.wl(0), i_cTarget.wl(i_cTarget.size() - 1), i_cTarget.size());

	sprintf(lpszCache_Filename,".sfdb/.spectrafit.0.2.%s.%s.%i",i_lpszTarget_File,i_lpszFileSource,i_uiIon);
	SPECTRA_FIT_DATA cCall_Data;
	cCall_Data.lpszModel_Name = i_lpszFileSource;
	cCall_Data.lpcOpacity_Map_A = &i_cOpacity_Map_A;
	cCall_Data.lpcOpacity_Map_B = &i_cOpacity_Map_B;
	cCall_Data.uiIon = i_uiIon;
	cCall_Data.lpcTarget = &i_cTarget;
	cCall_Data.dMin_WL = i_dMin_WL;
	cCall_Data.dMax_WL = i_dMax_WL;
	cCall_Data.bChi2 = i_bChi2;



	unsigned int uiParameters = 5;
	if (i_cOpacity_Map_B.GetNumElements() == 0)
		uiParameters = 4; // don't look for HVF component separately

	XVECTOR cBounds[2] = {XVECTOR(uiParameters),XVECTOR(uiParameters)};
	XVECTOR cThreshold(uiParameters);
	XVECTOR cResult(uiParameters);
	double dTime_Lower_Bound, dTime_Upper_Bound;
	if (dSuggested_Time != 0.0)
	{
		dTime_Lower_Bound = 0.0;
		dTime_Upper_Bound = 0.0;
		unsigned int uiI = 1;
		while (uiI < i_cOpacity_Map_A.GetNumElements() && i_cOpacity_Map_A.GetElement(uiI,0) < (dSuggested_Time- 3.0))
			uiI++;
		if (uiI > 0)
			uiI--;
		dTime_Lower_Bound = uiI;
		while (uiI < i_cOpacity_Map_A.GetNumElements() && i_cOpacity_Map_A.GetElement(uiI,0) < (dSuggested_Time + 3.0))
			uiI++;
		if (uiI < i_cOpacity_Map_A.GetNumElements())
			dTime_Upper_Bound = uiI;
		else
			dTime_Upper_Bound = uiI - 1;
	}
	else
	{
		dTime_Lower_Bound = 0.0;
		dTime_Upper_Bound = i_cOpacity_Map_A.GetNumColumns() - 2;
	}


	cBounds[0].Set(0,dTime_Lower_Bound); // time
	cBounds[0].Set(1,3.0); // PS velocity (kkm/s)
	cBounds[0].Set(2,5.0); // PS temp (kK)
//	cBounds[0].Set(3,11.99); // PS ion temp (kK)
	cBounds[0].Set(3,-4.0); // PS ion log opacity scaling
	if (i_cOpacity_Map_B.GetNumElements() != 0)
	{
//		cBounds[0].Set(5,11.99); // HVF ion temp (kK)
		cBounds[0].Set(4,-4.0); // HVF ion log opacity scaling
	}
	cBounds[1].Set(0,dTime_Upper_Bound); // time
	cBounds[1].Set(1,40.0); // PS velocity (kkm/s)
	cBounds[1].Set(2,30.0); // PS temp (kK)
	//cBounds[1].Set(3,12.01); // PS ion temp (kK)
	cBounds[1].Set(3,5.0); // PS ion log opacity scaling
	if (i_cOpacity_Map_B.GetNumElements() != 0)
	{
//		cBounds[1].Set(5,12.01); // HVF ion temp (kK)
		cBounds[1].Set(4,8.0); // HVF ion log opacity scaling
	}

	cThreshold.Set(0,1.0);
	cThreshold.Set(1,0.5);
	cThreshold.Set(2,0.25);
//	cThreshold.Set(3,0.5);
	cThreshold.Set(3,0.1);
	if (i_cOpacity_Map_B.GetNumElements() != 0)
	{
//		cThreshold.Set(5,0.5);
		cThreshold.Set(4,0.1);
	}

	xPerform_Fit_Bound_Simplex(cBounds[0],cBounds[1],cThreshold,Fit_Function,cResult,&cCall_Data,lpszCache_Filename);
//	printf("\n");
//	cResult.Print();
	int iT_ref = (int )(cResult.Get(0) + 0.5);
	if (iT_ref < 0)
		iT_ref = 0;
	if (iT_ref > (i_cOpacity_Map_A.GetNumColumns() - 2))
		iT_ref = i_cOpacity_Map_A.GetNumColumns() - 2;
	iT_ref++;

	o_cFit.dBest_Fit_Day = i_cOpacity_Map_A.GetElement(iT_ref,0);
//	printf("%i %.2f\n",iT_ref,o_dDay);
	o_cFit.dBest_Fit_PS_Vel = cResult.Get(1);
	o_cFit.dBest_Fit_PS_Temp = cResult.Get(2);
	o_cFit.dBest_Fit_PS_Ion_Temp = cResult.Get(2);
	o_cFit.dBest_Fit_PS_Log_Tau = cResult.Get(3);
	o_cFit.bBest_Fit_With_Shell = (i_cOpacity_Map_B.GetNumElements() != 0);
	if (o_cFit.bBest_Fit_With_Shell)
	{
		o_cFit.dBest_Fit_HVF_Ion_Temp = cResult.Get(2);
		o_cFit.dBest_Fit_HVF_Log_Tau = cResult.Get(4);
	}
//	printf("Generating\n");

	XVECTOR vX;
	if (cResult.Get_Size() == 5)
	{
		vX.Set_Size(7);
		vX.Set(5,cResult.Get(4)); // HVF scalar
		vX.Set(6,cResult.Get(2)); // HVF temp (use PS temp)
	}
	else
	{
		vX.Set_Size(5);
	}
	vX.Set(0,cResult.Get(0)); // day
	vX.Set(1,cResult.Get(1)); // PS vel
	vX.Set(2,cResult.Get(2)); // PS temp
	vX.Set(3,cResult.Get(3)); // PS LGR scalar
	vX.Set(4,cResult.Get(2)); // PS LGR temp (use PS temp)

	Generate_Synow_Spectra(i_cTarget,i_cOpacity_Map_A,i_cOpacity_Map_B,i_uiIon,vX,cOutput);
	o_cFit.cBest_Fit_Spectrum = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
	o_cFit.dBest_Fit = Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2 ? FIT_CHI2 : FIT_CENTRAL , 2);
//	printf("%.2f\n",o_cOutput.flux(0));
	return o_cFit.dBest_Fit;
}

double Fit_Function_Exp(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
	SPECTRA_FIT_DATA_BASE *lpcCall_Data = (SPECTRA_FIT_DATA_BASE *)i_lpvSpectra_Fit_Data;
	ES::Spectrum cOutput(*lpcCall_Data->lpcTarget);
	// look to see if this particular vector has been processed
	if (!Read_Vector_Output_Cache(i_vX, lpcCall_Data, cOutput))
	{
		Generate_Synow_Spectra_Exp(lpcCall_Data->lpcTarget[0],lpcCall_Data->uiIon,i_vX,cOutput);
		Save_Vector_Output_Cache(i_vX, lpcCall_Data, cOutput);
	}
	return Fit(lpcCall_Data->lpcTarget[0], cOutput, lpcCall_Data->dMin_WL, lpcCall_Data->dMax_WL, lpcCall_Data->bChi2 ? FIT_CHI2 : FIT_CENTRAL , 2);
}

double CompareFitsExp(const char * i_lpszTarget_File, const char * i_lpszFileSource, const ES::Spectrum &i_cTarget, unsigned int i_uiIon, const double &i_dMin_WL, const double &i_dMax_WL, bool i_bChi2, bool i_bInclude_HVF, BEST_FIT_DATA & o_cFit)
{
	char lpszCache_Filename[256];
	ES::Spectrum cOutput(i_cTarget);

	sprintf(lpszCache_Filename,".sfdb/.spectrafit.0.2.%s.%s.%i",i_lpszTarget_File,i_lpszFileSource,i_uiIon);
	SPECTRA_FIT_DATA cCall_Data;
	cCall_Data.uiIon = i_uiIon;
	cCall_Data.lpcTarget = &i_cTarget;
	cCall_Data.dMin_WL = i_dMin_WL;
	cCall_Data.dMax_WL = i_dMax_WL;
	cCall_Data.bChi2 = i_bChi2;
	cCall_Data.lpszModel_Name = i_lpszFileSource;



	unsigned int uiParameters = 7;
	if (i_bInclude_HVF)
		uiParameters = 12;

	XVECTOR cBounds[2] = {XVECTOR(uiParameters),XVECTOR(uiParameters)};
	XVECTOR cThreshold(uiParameters);
	XVECTOR cResult(uiParameters);
	double dTime_Lower_Bound, dTime_Upper_Bound;


	cBounds[0].Set(0,5.0); // PS velocity (kkm/s)
	cBounds[0].Set(1,5.0); // PS temp (kK)
	cBounds[0].Set(2,-10.0); // PS ion log opacity scaling
	cBounds[0].Set(3,11.99); // PS ion temp (kK)
	cBounds[0].Set(4,5.0); // PS ion vmin
	cBounds[0].Set(5,20.0); // PS ion vmax
	cBounds[0].Set(6,-10.0); // PS ion aux
	if (i_bInclude_HVF)
	{
		cBounds[0].Set(7,-10.0); // PS ion log opacity scaling
		cBounds[0].Set(8,11.99); // PS ion temp (kK)
		cBounds[0].Set(9,20.0); // PS ion vmin
		cBounds[0].Set(10,100.0); // PS ion vmax
		cBounds[0].Set(11,-10.0); // PS ion aux
	}
	cBounds[1].Set(0,40.0); // PS velocity (kkm/s)
	cBounds[1].Set(1,30.0); // PS temp (kK)
	cBounds[1].Set(2,10.0); // PS ion log opacity scaling
	cBounds[1].Set(3,12.01); // PS ion temp (kK)
	cBounds[1].Set(4,20.0); // PS ion vmin
	cBounds[1].Set(5,50.0); // PS ion vmax
	cBounds[1].Set(6,50.0); // PS ion aux
	if (i_bInclude_HVF)
	{
		cBounds[1].Set(7,10.0); // PS ion log opacity scaling
		cBounds[1].Set(8,12.01); // PS ion temp (kK)
		cBounds[1].Set(9,50.0); // PS ion vmin
		cBounds[1].Set(10,200.0); // PS ion vmax
		cBounds[1].Set(11,50.0); // PS ion aux
	}

	cThreshold.Set(0,0.5);
	cThreshold.Set(1,0.25);
	cThreshold.Set(2,0.1);
	cThreshold.Set(3,0.5);
	cThreshold.Set(4,0.1);
	cThreshold.Set(5,0.1);
	cThreshold.Set(6,0.1);
	if (i_bInclude_HVF)
	{
		cThreshold.Set(7,0.1);
		cThreshold.Set(8,0.5);
		cThreshold.Set(9,0.1);
		cThreshold.Set(10,0.1);
		cThreshold.Set(11,0.1);
	}

	xPerform_Fit_Bound_Simplex(cBounds[0],cBounds[1],cThreshold,Fit_Function_Exp,cResult,&cCall_Data,lpszCache_Filename);
//	printf("\n");
//	cResult.Print();
//	printf("%i %.2f\n",iT_ref,o_dDay);
//	printf("%i %.2f\n",iT_ref,o_dDay);
	o_cFit.dBest_Fit_PS_Vel = cResult.Get(0);
	o_cFit.dBest_Fit_PS_Temp = cResult.Get(1);
	o_cFit.dBest_Fit_PS_Log_Tau = cResult.Get(2);
	o_cFit.dBest_Fit_PS_Ion_Temp = cResult.Get(3);
	o_cFit.dBest_Fit_PS_Ion_Vmin = cResult.Get(4);
	o_cFit.dBest_Fit_PS_Ion_Vmax = cResult.Get(5);
	o_cFit.dBest_Fit_PS_Ion_Aux = cResult.Get(6);
	o_cFit.bBest_Fit_With_Shell = i_bInclude_HVF;
	if (i_bInclude_HVF)
	{
		o_cFit.dBest_Fit_HVF_Log_Tau = cResult.Get(7);
		o_cFit.dBest_Fit_HVF_Ion_Temp = cResult.Get(8);
		o_cFit.dBest_Fit_HVF_Ion_Vmin = cResult.Get(9);
		o_cFit.dBest_Fit_HVF_Ion_Vmax = cResult.Get(10);
		o_cFit.dBest_Fit_HVF_Ion_Aux = cResult.Get(11);
	}


	Generate_Synow_Spectra_Exp(i_cTarget,i_uiIon,cResult,cOutput);
	o_cFit.cBest_Fit_Spectrum = cOutput; // not completely sure why this has to be done instead of passing o_cOutput as a parameter to Generate_Synow_Spectra, but if I do the latter it returns with flux set to zero...
//	printf("%.2f\n",o_cOutput.flux(0));
	return Fit(i_cTarget, cOutput, i_dMin_WL, i_dMax_WL, i_bChi2 ? FIT_CHI2 : FIT_CENTRAL , 2);
}




bool Read_Best_Fit_Cache(const char * i_lpszTarget_File, ES::Spectrum & i_cTarget, unsigned int uiIon, BEST_FIT_DATA & o_cBest_Fit, int &iStartFile, int i_iArg_Count, const char * i_lpszArg_Values[], bool bForce_Clean_Start)
{
	bool bRet = false;
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	char lpszFilename[256];
	sprintf(lpszFilename,".sfdb/.spectrafit.0.2.%s.filelist.cache",i_lpszTarget_File);
	FILE * fileCache = NULL;
	if (!bForce_Clean_Start)
		fileCache = fopen(lpszFilename,"rt");
	iStartFile = 3;
	if (fileCache)
	{
		char lpszBuffer[256];
		fgets(lpszBuffer,256,fileCache);
		iStartFile = atoi(lpszBuffer);
		fgets(lpszBuffer,256,fileCache);
		unsigned int uiNumFiles = atoi(lpszBuffer);
		bool bListTest = false;
		if (uiNumFiles == i_iArg_Count - 3)
		{
			bListTest = true;
			for (unsigned int uiI = 1; uiI < i_iArg_Count && bListTest; uiI++)
			{
				fgets(lpszBuffer,256,fileCache);
				// need to strip CR/LF
				char * lpszCursor = lpszBuffer;
				lpszCursor += strlen(lpszBuffer);
				lpszCursor--;
				if (lpszCursor[0] == 10 || lpszCursor[0] == 13)
					lpszCursor[0] = 0;
				lpszCursor--;
				if (lpszCursor[0] == 10 || lpszCursor[0] == 13)
					lpszCursor[0] = 0;
				bListTest = (strcmp(lpszBuffer,i_lpszArg_Values[uiI]) == 0);
			}
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Ion_Temp = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Temp = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_Day = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Log_Tau = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_HVF_Ion_Temp = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Vel = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_HVF_Log_Tau = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Ion_Vmin = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Ion_Vmax = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_PS_Ion_Aux = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_HVF_Ion_Vmin = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_HVF_Ion_Vmax = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.dBest_Fit_HVF_Ion_Aux = atof(lpszBuffer);
			fgets(lpszBuffer,256,fileCache);
			
			o_cBest_Fit.iBest_Fit_File = atoi(lpszBuffer);
			if (o_cBest_Fit.iBest_Fit_File == -1)
				o_cBest_Fit.lpszBest_Fit_File = "exphvf";
			else if (o_cBest_Fit.iBest_Fit_File == -2)
				o_cBest_Fit.lpszBest_Fit_File = "expps";
			else
				o_cBest_Fit.lpszBest_Fit_File = i_lpszArg_Values[o_cBest_Fit.iBest_Fit_File];
				
			fgets(lpszBuffer,256,fileCache);
			o_cBest_Fit.bBest_Fit_With_Shell = lpszBuffer[0] == 't';
//					printf("Generating spectra\n");
			// Generate the output for current best fit
			if (o_cBest_Fit.iBest_Fit_File == -1 || o_cBest_Fit.iBest_Fit_File == -2)
			{
				XVECTOR cTest;
				if (o_cBest_Fit.iBest_Fit_File == -2)
					cTest.Set_Size(7);
				else
					cTest.Set_Size(12);

				cTest.Set(0,o_cBest_Fit.dBest_Fit_PS_Vel); // PS velocity (kkm/s)
				cTest.Set(1,o_cBest_Fit.dBest_Fit_PS_Temp); // PS temp (kK)
				cTest.Set(2,o_cBest_Fit.dBest_Fit_PS_Log_Tau); // PS ion log opacity scaling
				cTest.Set(3,o_cBest_Fit.dBest_Fit_PS_Ion_Temp); // PS ion temp (kK)
				cTest.Set(4,o_cBest_Fit.dBest_Fit_PS_Ion_Vmin); // PS ion temp (kK)
				cTest.Set(5,o_cBest_Fit.dBest_Fit_PS_Ion_Vmax); // PS ion temp (kK)
				cTest.Set(6,o_cBest_Fit.dBest_Fit_PS_Ion_Aux); // PS ion temp (kK)
				if (o_cBest_Fit.iBest_Fit_File == -1)
				{
					cTest.Set(7,o_cBest_Fit.dBest_Fit_HVF_Log_Tau); // HVF ion log opacity scaling
					cTest.Set(8,o_cBest_Fit.dBest_Fit_HVF_Ion_Temp); // HVF ion temp (kK)
					cTest.Set(9,o_cBest_Fit.dBest_Fit_HVF_Ion_Vmin); // HVF ion temp (kK)
					cTest.Set(10,o_cBest_Fit.dBest_Fit_HVF_Ion_Vmax); // HVF ion temp (kK)
					cTest.Set(11,o_cBest_Fit.dBest_Fit_HVF_Ion_Aux); // HVF ion temp (kK)
				}
				Generate_Synow_Spectra_Exp(i_cTarget,uiIon,cTest,o_cBest_Fit.cBest_Fit_Spectrum);
			}
			else
			{
				XVECTOR cTest(7);
				char lpszOpacity_File_Ejecta[128];
				char lpszOpacity_File_Shell[128];

				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.xdataset",i_lpszArg_Values[o_cBest_Fit.iBest_Fit_File]);
				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",i_lpszArg_Values[o_cBest_Fit.iBest_Fit_File]);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell);

				for (unsigned int uiI = 1; uiI < cOpacity_Map_Ejecta.GetNumColumns(); uiI++)
					if (cOpacity_Map_Ejecta.GetElement(uiI,0) == o_cBest_Fit.dBest_Fit_Day)
						cTest.Set(0,uiI); // time
				cTest.Set(1,o_cBest_Fit.dBest_Fit_PS_Vel); // PS velocity (kkm/s)
				cTest.Set(2,o_cBest_Fit.dBest_Fit_PS_Temp); // PS temp (kK)
				cTest.Set(3,o_cBest_Fit.dBest_Fit_PS_Ion_Temp); // PS ion temp (kK)
				cTest.Set(4,o_cBest_Fit.dBest_Fit_PS_Log_Tau); // PS ion log opacity scaling
				cTest.Set(5,o_cBest_Fit.dBest_Fit_HVF_Ion_Temp); // HVF ion temp (kK)
				cTest.Set(6,o_cBest_Fit.dBest_Fit_HVF_Log_Tau); // HVF ion log opacity scaling

				Generate_Synow_Spectra(i_cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell,uiIon,cTest,o_cBest_Fit.cBest_Fit_Spectrum);
			}
			//printf("Done\n");
		}
		
		bRet = bListTest;
		if (!bListTest)
		{
			iStartFile = 3; // different file list - restart.
			char lpszCommand[256];
			sprintf(lpszCommand,"rm %s",lpszFilename);
			system(lpszCommand);
//					system("rm spectrafit.*.cache");
			o_cBest_Fit.dBest_Fit = DBL_MAX;
		}
		fclose(fileCache);
	}
	return bRet;
}

void Save_Best_Fit_Cache(const char * i_lpszTarget_File, const BEST_FIT_DATA & o_cBest_Fit, int iRunningFile, const char * i_lpszArg_Values[], int iNum_Files)
{
	bool bRet = false;
	char lpszFilename[256];
	sprintf(lpszFilename,".sfdb/.spectrafit.0.2.%s.filelist.cache",i_lpszTarget_File);
	FILE * fileCache = fopen(lpszFilename,"wt");
	if (fileCache)
	{
		char lpszBuffer[256];
		fprintf(fileCache,"%i",iRunningFile);
		fprintf(fileCache,"%i\n",iNum_Files);
		for (unsigned int uiJ = 0; uiJ < iNum_Files; uiJ++)
			fprintf(fileCache,"%s\n",i_lpszArg_Values[uiJ + 3]);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Ion_Temp);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Temp);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_Day);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Log_Tau);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_HVF_Ion_Temp);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Vel);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_HVF_Log_Tau);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Ion_Vmin);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Ion_Vmax);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_PS_Ion_Aux);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_HVF_Ion_Vmin);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_HVF_Ion_Vmax);
		fprintf(fileCache,"%.17e\n",o_cBest_Fit.dBest_Fit_HVF_Ion_Aux);
		fprintf(fileCache,"%i\n",o_cBest_Fit.iBest_Fit_File);
		fprintf(fileCache,"%c\n",o_cBest_Fit.bBest_Fit_With_Shell ? 't' : 'f');
		fclose(fileCache);
	}
}

double Process_CL_Dbl(int &io_iArg_Count,const char * i_lpszArg_Values[], const char * i_lpszParameter, const double & i_dDefault_Value);

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	xrand_Set_Type(XRT_K);
	xrsrand();

	char	lpszFilename[64];
	char	lpszBuffer[1024];
	char	lpszTargetFileStripped[256];
	char	lpszGroup_Data_File[4];
	XDATASET cDataSource;
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	char	chSeparator;
	double	dSuggested_Time;
	bool bNo_Exp;
	double		dSuggested_PS;
	double		dSuggested_PS_Temp;
	double		dSuggested_Ejecta_Scalar;
	double		dSuggested_Ejecta_Temp;
	double		dSuggested_Shell_Scalar;
	double		dSuggested_Shell_Temp;
	double		dSuggested_Normalization_WL;
	double		dFit_Max_WL;
	double		dFit_Min_WL;
	double		dNan = nan("");
	unsigned int uiNum_Files_To_Process;
	bool		bForce_Processing;
	double		dPlot_Max_WL,dPlot_Min_WL,dLine_Minimum;
//	const char * lpszFile_Lists
	if (bForce_Processing = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--ignore-cache"))
	{
		i_iArg_Count--; // dont' process the option as a file
		printf("Forced restart\n");
	}

	if (bNo_Exp = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--no-exp-fit"))
		i_iArg_Count--; // dont' process the option as a file

	if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--time"))
	{
		dSuggested_Time = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--time",0.0);
		i_iArg_Count--; // dont' process the option as a file
//		printf("%.0f\n",dSuggested_Time);
	}
	else if (xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--timebmax"))
	{
		dSuggested_Time = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--timebmax",-19.0);
		dSuggested_Time += 19.0;
//		printf("%.0f\n",dSuggested_Time);
		i_iArg_Count--; // dont' process the option as a file
	}
	dSuggested_PS = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--psvel",-1.0);
	dSuggested_PS_Temp = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--pstemp",dNan);
	dSuggested_Ejecta_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--Ce",dNan);
	dSuggested_Ejecta_Temp = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--Te",dNan);
	dSuggested_Shell_Scalar = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--Cs",dNan);
	dSuggested_Shell_Temp = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--Ts",dNan);
	dSuggested_Normalization_WL = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--wl_norm",dNan);
	dFit_Max_WL = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--wl_max",dNan);
	dFit_Min_WL = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--wl_min",dNan);

	if (i_iArg_Count < 4)
	{
		Usage("Missing parameters");
	}
	else
	{
		// generate the target spectrum
        ES::Spectrum cTarget = ES::Spectrum::create_from_ascii_file( i_lpszArg_Values[1] );
		double	dMin_WL,dMax_WL;
		unsigned int uiIon;

		const char * lpszTargetFileStripped = i_lpszArg_Values[1];
		if (strchr(lpszTargetFileStripped,'/'))
		{
			lpszTargetFileStripped += strlen(i_lpszArg_Values[1]);
			while (lpszTargetFileStripped[0] != '/')
				lpszTargetFileStripped--;
			lpszTargetFileStripped++;
		}
		// default to full range

		if (strcmp(i_lpszArg_Values[2],"CaNIR") == 0)
		{
			uiIon = 2001;
			dMin_WL = 7750.0;
			dMax_WL = 8800.0;
			dPlot_Min_WL = 7000.0;
			dPlot_Max_WL = 8500.0;
			dLine_Minimum = 8000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else if (strcmp(i_lpszArg_Values[2],"CaHK") == 0)
		{
			uiIon = 2001;
			dMin_WL = 3000.0;
			dMax_WL = 4500.0;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
			dLine_Minimum = 4000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else if (strcmp(i_lpszArg_Values[2],"Si6355") == 0)
		{
			uiIon = 1401;
			dMin_WL = 5000.0;
			dMax_WL = 6500.0;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
			dLine_Minimum = 6000.0;
			strcpy(lpszGroup_Data_File,"Si");
		}
		else //if (strcmp(i_lpszArg_Values[2],"Full") != 0)
		{
			uiIon = 0;
			dMin_WL = -1;
			dMax_WL = -1;
			dPlot_Min_WL = dMin_WL;
			dPlot_Max_WL = dMax_WL;
		}
//		else
//		{
//			uiIon = 2001; // need to decide how to handle this case
//			dMin_WL = cTarget.wl(0);
//			dMax_WL = cTarget.wl(target.size() - 1);
//		}
		double	dCoverage = 1.0;
		if (dMin_WL > cTarget.wl(0) && cTarget.wl(0) < dMax_WL && dMax_WL < cTarget.wl(cTarget.size() - 1))
		{
			// partial coverage - the target data starts in the middle of the range
			dCoverage = (dMax_WL - cTarget.wl(0)) / (dMax_WL - dMin_WL);
		}
		if (dMax_WL > cTarget.wl(cTarget.size() - 1) && dMin_WL < cTarget.wl(cTarget.size() - 1) && dMin_WL > cTarget.wl(0))
		{
			// partial coverage - the target data starts in the middle of the range
			dCoverage = (cTarget.wl(cTarget.size() - 1) - dMin_WL) / (dMax_WL - dMin_WL);
		}
		else if (cTarget.wl(0) > dMin_WL && cTarget.wl(cTarget.size() - 1) < dMax_WL)
		{
			dCoverage = (cTarget.wl(cTarget.size() - 1) - cTarget.wl(0)) / (dMax_WL - dMin_WL);
		}
		else if (dMin_WL > cTarget.wl(cTarget.size() - 1) || dMax_WL < cTarget.wl(0))
		{
			dCoverage = 0.0;
		}

		if (dCoverage < 0.75)
		{
			char lpszError[256];
			sprintf(lpszError,"Source spectrum does not cover desired range.  Requested %.2f - %.2f, Available %.2f - %.2f",dMin_WL,dMax_WL,cTarget.wl(0),cTarget.wl(cTarget.size() - 1));
			Usage(lpszError);
		}
		else if (dMin_WL > 0 && dMax_WL > 0)
		{
			if (dCoverage < 1.0)
			{
				if (dMin_WL > cTarget.wl(0) && cTarget.wl(0) < dMax_WL && dMax_WL < cTarget.wl(cTarget.size() - 1))
				{
					dMin_WL = cTarget.wl(0);
				}
				if (dMax_WL > cTarget.wl(cTarget.size() - 1) && dMin_WL < cTarget.wl(cTarget.size() - 1) && dMin_WL > cTarget.wl(0))
				{
					dMax_WL = cTarget.wl(cTarget.size() - 1);
				}
				else if (cTarget.wl(0) > dMin_WL && cTarget.wl(cTarget.size() - 1) < dMax_WL)
				{
					dMin_WL = cTarget.wl(0);
					dMax_WL = cTarget.wl(cTarget.size() - 1);
				}
			}
			unsigned int uiStart_Idx = 0;
			while (uiStart_Idx < cTarget.size() && cTarget.wl(uiStart_Idx) < dLine_Minimum)
				uiStart_Idx++;
			unsigned int uiTest_Idx = uiStart_Idx;
			double dLocal_Max = 0,dLocal_Max_WL;
			while (uiTest_Idx < cTarget.size() && cTarget.wl(uiTest_Idx) > dMin_WL)
			{
				if (cTarget.flux(uiTest_Idx) > dLocal_Max)
				{
					dLocal_Max = cTarget.flux(uiTest_Idx);
					dLocal_Max_WL = cTarget.wl(uiTest_Idx);
				}
				uiTest_Idx--;
			}
			dMin_WL = dLocal_Max_WL;
			uiTest_Idx = uiStart_Idx;
			dLocal_Max = 0;
			while (uiTest_Idx < cTarget.size() && cTarget.wl(uiTest_Idx) < dMax_WL)
			{
				if (cTarget.flux(uiTest_Idx) > dLocal_Max)
				{
					dLocal_Max = cTarget.flux(uiTest_Idx);
					dLocal_Max_WL = cTarget.wl(uiTest_Idx);
				}
				uiTest_Idx++;
			}
			dMax_WL = dLocal_Max_WL - 50.0; // go a little blueward of max on the red side

			ES::Spectrum cOutput(cTarget);
			ES::Spectrum cBest_Fit_Spectrum(cTarget);
			cOutput.zero_flux();
			cBest_Fit_Spectrum.zero_flux();
			bool bChi2 = ValidateChi2(cTarget,dMin_WL, dMax_WL);
			BEST_FIT_DATA	cBest_Fit;
			BEST_FIT_DATA	cFit;
			bool bRestart = false;
			char lpszOpacity_File_Ejecta[64], lpszOpacity_File_Shell[64];
			char lpszOutput_Filename[128];
			char lpszTitle[128];
			double	dPS_Vel, dPS_Temp, dPS_Ion_Temp, dPS_Log_Tau, dPS_Ion_Vmin, dPS_Ion_Vmax, dPS_Ion_Aux, dHVF_Ion_Temp, dHVF_Log_Tau, dHVF_Ion_Vmin, dHVF_Ion_Vmax, dHVF_Ion_Aux;
			int uiStartFile;

			bRestart = Read_Best_Fit_Cache(lpszTargetFileStripped,cTarget, uiIon, cBest_Fit, uiStartFile, i_iArg_Count, i_lpszArg_Values, bForce_Processing);
//			printf("%i\t%c\n",uiStartFile,bRestart?'t':'f');
			for (unsigned int uiI = uiStartFile; uiStartFile > 0 && uiI < i_iArg_Count; uiI++)
			{
				bRestart = false;
				printf("Reading %s",i_lpszArg_Values[uiI]);
				sprintf(lpszOpacity_File_Ejecta,"%s/opacity_map_ejecta.%s.xdataset",i_lpszArg_Values[uiI],lpszGroup_Data_File);
				sprintf(lpszOpacity_File_Shell,"%s/opacity_map_shell.xdataset",i_lpszArg_Values[uiI]);
				cOpacity_Map_Ejecta.ReadDataFileBin(lpszOpacity_File_Ejecta);
				cOpacity_Map_Shell.ReadDataFileBin(lpszOpacity_File_Shell,true);
				if (cOpacity_Map_Ejecta.GetNumElements() > 0)
				{
					double dDay;
					cFit.Reset();
					double dFit = CompareFits(lpszTargetFileStripped,i_lpszArg_Values[uiI], cTarget, cOpacity_Map_Ejecta,cOpacity_Map_Shell, uiIon, dMin_WL, dMax_WL, bChi2, cFit,dSuggested_Time);
					cFit.SetBestFitFile(i_lpszArg_Values[uiI]);
					cFit.iBest_Fit_File = uiI;
					sprintf(lpszOutput_Filename,"%s.fit.%s.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[uiI],i_lpszArg_Values[2]);
					cFit.Output(lpszOutput_Filename,bChi2);
					if (cFit.dBest_Fit != -1.0 && cFit.dBest_Fit < cBest_Fit.dBest_Fit)
					{
						cBest_Fit = cFit;
					}
					if (bChi2)
						printf("\tChi^2 Fit = %.2e\n",dFit);
					else
						printf("\tVar   Fit = %.2e\n",dFit);
					sprintf(lpszOutput_Filename,"%s.fit.%s.%s.eps",i_lpszArg_Values[1],i_lpszArg_Values[uiI],i_lpszArg_Values[2]);
					Plot(cTarget, cFit.cBest_Fit_Spectrum, lpszOutput_Filename, dPlot_Min_WL, dPlot_Max_WL, lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, true);
				}
				else
					printf("Invalid file\n");
				Save_Best_Fit_Cache(lpszTargetFileStripped,cBest_Fit,uiI,i_lpszArg_Values,i_iArg_Count - 3);

			}
			if (uiStartFile != -2 && !bNo_Exp)
			{
				printf("exphvf");

				cFit.Reset();
				double dFit = CompareFitsExp(lpszTargetFileStripped,"exphvf", cTarget, uiIon, dMin_WL, dMax_WL, bChi2, true, cFit);
				cFit.SetBestFitFile("exphvf");
				cFit.iBest_Fit_File = -1;
				sprintf(lpszOutput_Filename,"%s.fit.exphvf.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[2]);
				cFit.Output(lpszOutput_Filename,bChi2);
				if (cFit.dBest_Fit != -1.0 && cFit.dBest_Fit < cBest_Fit.dBest_Fit)
				{
					cBest_Fit = cFit;
				}
				if (bChi2)
					printf("\tChi^2 Fit = %.2e\n",dFit);
				else
					printf("\tVar   Fit = %.2e\n",dFit);
				Save_Best_Fit_Cache(lpszTargetFileStripped,cBest_Fit,-1,i_lpszArg_Values,i_iArg_Count - 3);
			}
			if (!bNo_Exp)
			{
				printf("expps");
				cFit.Reset();
				double dFit = CompareFitsExp(lpszTargetFileStripped,"expps", cTarget, uiIon, dMin_WL, dMax_WL, bChi2, false, cFit);
				cFit.SetBestFitFile("expps");
				cFit.iBest_Fit_File = -1;
				sprintf(lpszOutput_Filename,"%s.fit.expps.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[2]);
				cFit.Output(lpszOutput_Filename,bChi2);
				if (cFit.dBest_Fit != -1.0 && cFit.dBest_Fit < cBest_Fit.dBest_Fit)
				{
					cBest_Fit = cFit;
				}
				if (bChi2)
					printf("\tChi^2 Fit = %.2e\n",dFit);
				else
					printf("\tVar   Fit = %.2e\n",dFit);
				Save_Best_Fit_Cache(lpszTargetFileStripped,cBest_Fit,-2,i_lpszArg_Values,i_iArg_Count - 3);
			}

			//system("rm spectrafit.filelist.cache");// done - delete the cache
			fprintf(stdout,"spectrafit v%s\n",VERSION);
			cBest_Fit.Output(NULL,bChi2);
			if (cBest_Fit.iBest_Fit_File >= 0)
			{
				if (cBest_Fit.bBest_Fit_With_Shell)
					sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f thv %.2f Thv %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_Day,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_HVF_Log_Tau,cBest_Fit.dBest_Fit_HVF_Ion_Temp);
				else
					sprintf(lpszTitle,"%s: d %.2f Vps %.1f Tps %.2f tps %.2f Tpsi %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_Day,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp);
			}
			else
			{
				if (cBest_Fit.bBest_Fit_With_Shell)
					sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f thv %.2f Thv %.2f hvfvm %.2f hvfa %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_PS_Ion_Vmin,cBest_Fit.dBest_Fit_PS_Ion_Aux,cBest_Fit.dBest_Fit_HVF_Log_Tau,cBest_Fit.dBest_Fit_HVF_Ion_Temp,cBest_Fit.dBest_Fit_HVF_Ion_Vmin,cBest_Fit.dBest_Fit_HVF_Ion_Aux);
				else
					sprintf(lpszTitle,"%s: Vps %.1f Tps %.2f tps %.2f Tpsi %.2f psvm %.2f psa %.2f",cBest_Fit.lpszBest_Fit_File,cBest_Fit.dBest_Fit_PS_Vel,cBest_Fit.dBest_Fit_PS_Temp,cBest_Fit.dBest_Fit_PS_Log_Tau,cBest_Fit.dBest_Fit_PS_Ion_Temp,cBest_Fit.dBest_Fit_PS_Ion_Vmin,cBest_Fit.dBest_Fit_PS_Ion_Aux);
			}
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.full.eps",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			Plot(cTarget, cBest_Fit.cBest_Fit_Spectrum, lpszOutput_Filename, cTarget.wl(0), cTarget.wl(cTarget.size() - 1), lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, false);
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.eps",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			Plot(cTarget, cBest_Fit.cBest_Fit_Spectrum, lpszOutput_Filename, dPlot_Min_WL, dPlot_Max_WL, lpszTitle, (dPlot_Min_WL + dPlot_Max_WL) * 0.5, false, true);
			sprintf(lpszOutput_Filename,"%s.bestfit.%s.data",i_lpszArg_Values[1],i_lpszArg_Values[2]);
			cBest_Fit.Output(lpszOutput_Filename,bChi2);
			fprintf(stdout,"Best fit output to %s.\n",lpszOutput_Filename);
		}
		else
		{
			Usage("Invalid fit region");
		}
	}
	return 0;
}

