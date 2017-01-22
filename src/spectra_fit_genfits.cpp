#include <specfit.h>
#include <model_spectra_db.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>
/*
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

bool xTest_Valid(const XVECTOR * i_lpvVector_List, unsigned int i_uiNum_Vectors)
{
	bool bValid = true;
	for (unsigned int uiI = 0; uiI <  i_uiNum_Vectors && bValid; uiI++)
	{
		bValid &= !i_lpvVector_List[uiI].is_nan() && !i_lpvVector_List[uiI].is_inf();
	}
	return bValid;
}
void xPerform_Fit_Bound_Simplex(const XVECTOR &i_vLower_Bound,const XVECTOR &i_vUpper_Bound, const XVECTOR & i_lpvFit_Threshold, QFunctionV i_fUser_Function, XVECTOR & o_vResult, void * i_lpvUser_Data, const std::string &i_lpszCache_Name)
{
	unsigned int uiNum_Parameters = i_vLower_Bound.Get_Size();
	if (uiNum_Parameters != i_vUpper_Bound.Get_Size() || uiNum_Parameters != i_lpvFit_Threshold.Get_Size())
		fprintf(stderr,"xPerform_Fit_Bound: bound and theshold size doesn't match\n");
	else if (i_vLower_Bound.is_nan() || i_vLower_Bound.is_inf() || i_vUpper_Bound.is_nan() || i_vUpper_Bound.is_inf())
		fprintf(stderr,"xPerform_Fit_Bound: bound have invalid values (inf of NaN)\n");
	else if (i_lpvFit_Threshold.is_nan() || i_lpvFit_Threshold.is_inf())
		fprintf(stderr,"xPerform_Fit_Bound: fit threshold has invalid values (inf of NaN)\n");
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
		std::ostringstream ossCache_Filename;
		ossCache_Filename << i_lpszCache_Name << ".cache";

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
						cTest.Set(uiJ,cBounds[1][uiJ]);
				}
				fprintf(stdout,"-");
				fflush(stdout);
				dTest_Fit = i_fUser_Function(cTest,i_lpvUser_Data);
				//fprintf(stdout,"%.2e\t",dTest_Fit);
				fflush(stdout);

				// Now see if the fit is better than one of the existing fits. This will generate a simplex with the 8 best points in the cuboid
				if (!std::isnan(dTest_Fit) && !std::isinf(dTest_Fit))
				{
					lpdVertex_Fits[uiI] = dTest_Fit;
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
			}
			fprintf(stdout,"P");
			fflush(stdout);
			bool * lpbParameter_Variation = new bool[uiNum_Parameters];
			memset(lpbParameter_Variation,0,sizeof(bool) * uiNum_Parameters);
			// check trhough parameters and make sure that the simplex spans the bounded space
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				for (unsigned int uiK = uiJ; uiK < uiNum_Points; uiK++)
				{
					for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
					{
						lpbParameter_Variation[uiL] |= (fabs(lpcTest_Points[uiJ][uiL] - lpcTest_Points[uiK][uiL]) > 0.0);
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
			// look for points that have identical fits.  Find the mode of the fits
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
			// If there is a mode to the fits, choose vertices that span the space and don't have identical fits
			if (uiNum_Parameters > (uiNum_Points- uiFit_Mode_Max))
			{
				// can't replace using best points; just make sure we span
				for (unsigned int uiL = 0; uiL < uiNum_Parameters; uiL++)
				{
					if (!lpbParameter_Variation[uiL])
					{
						double dVal = cBounds[0][uiL];
						if (lpcTest_Points[0][uiL] == dVal)
							dVal = cBounds[1][uiL];
						lpcTest_Points[uiL].Set(uiL,dVal); // set the Lth point to the opposite bound
					}
				}
			}
			else
			{ // try another method to improve fits for starting point
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
							lpcTest_Points[uiFit_Max_Point].Set(uiJ,cBounds[1][uiJ]);
						else
							lpcTest_Points[uiFit_Max_Point].Set(uiJ,cBounds[0][uiJ]);
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
								lpbParameter_Variation[uiL] |= (fabs(lpcTest_Points[uiJ][uiL] - lpcTest_Points[uiK][uiL]) > 0.0);
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

				SavePointsCache(ossCache_Filename.str().c_str(),lpcTest_Points,uiNum_Points,uiNum_Parameters,lpdFit);
			}
			uiFit_Max_Point = 0;
		}
		// The method used below is basically simplex optimization, with a few modifications to handle bad data points and a contrained parameter space
//		while ((uiFit_Max_Point < uiNum_Parameters) && !Convergence(lpcTest_Points, uiNum_Parameters , i_lpvFit_Threshold))
		bool bUnrecoverable_Fault = false;
		while (!xConvergence(lpcTest_Points, uiNum_Points , i_lpvFit_Threshold) && xTest_Valid(lpcTest_Points,uiNum_Points) && !bUnrecoverable_Fault)
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

			std::vector<unsigned int> vuiSimilar_Fit;
			// see if a bunch of the fits are identical (variation < 0.1 std. dev)
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				if (lpdFit[uiI] == dFit_Max)
				{
					vuiSimilar_Fit.push_back(uiI);
				}
			}
			if (vuiSimilar_Fit.size() > 0)
			{
				unsigned int uiRand_Idx = xrand();
				uiRand_Idx %= vuiSimilar_Fit.size();
				uiFit_Max_Point = vuiSimilar_Fit[uiRand_Idx];
			}
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
				for (unsigned int uiI = 0; uiI < uiNum_Points && !bUnrecoverable_Fault; uiI++)
				{
					XVECTOR	vTest_Point;
					XVECTOR vDelta0 = lpcTest_Points[uiI] - cCentroid;
					cDelta = vDelta0;
					double dTest_Fit;
					unsigned int uiCount = 0;
					do
					{
						cDelta *= 0.5;
						vTest_Point = cCentroid + (vDelta0 - cDelta);
						dTest_Fit = i_fUser_Function(vTest_Point,i_lpvUser_Data);
						if (std::isnan(dTest_Fit) || std::isinf(dTest_Fit))
						{
							fprintf(stdout,"N");
							fflush(stdout);
						}
						uiCount++;
					}
					while ((std::isnan(dTest_Fit) || std::isinf(dTest_Fit)) && uiCount < 16);
					if (std::isnan(dTest_Fit) || std::isinf(dTest_Fit))
					{
						fprintf(stdout,"Q");
						fflush(stdout);
						bUnrecoverable_Fault = true;
					}
					else
					{
						fprintf(stdout,"*");
						fflush(stdout);
						lpdFit[uiI] = dTest_Fit;
						lpcTest_Points[uiI] = vTest_Point;
					}
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
				while (!xBoundTest(cTest,cBounds) && !cTest.is_nan() && !cTest.is_inf() && !bUnrecoverable_Fault)
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
					if (cDeltaTemp.is_nan() || cDeltaTemp.is_inf())
					{
						fprintf(stderr," temp delta is invalid\n");
						bUnrecoverable_Fault = true;
					}
					else
					{
						double dScalar = 1.0;
						// scale delta so that point will be within the box
						if (cTest.Get(uiLargest_Whatever) < cBounds[0].Get(uiLargest_Whatever))
							dScalar = cCentroid.Get(uiLargest_Whatever) - cBounds[0].Get(uiLargest_Whatever);
						else if (cTest.Get(uiLargest_Whatever) > cBounds[1].Get(uiLargest_Whatever))
							dScalar = cBounds[1].Get(uiLargest_Whatever) - cCentroid.Get(uiLargest_Whatever);
						cDeltaTemp *= dScalar;
						cTest = cCentroid - cDeltaTemp;
					}
			
				}
				if (!bUnrecoverable_Fault)
				{
					dFit_Test = i_fUser_Function(cTest,i_lpvUser_Data);
					if (std::isnan(dFit_Test) || std::isinf(dFit_Test))
					{
						fprintf(stdout,"N");
						fflush(stdout);
						bUnrecoverable_Fault = true;
					}
					else
					{
						if (dFit_Test >= lpdFit[uiFit_Max_Point])
						{
							fprintf(stdout,"#");
							fflush(stdout);
							cTest = cCentroid + cDelta * 0.5;
							dFit_Test = i_fUser_Function(cTest,i_lpvUser_Data);
						}

						if (std::isnan(dFit_Test) || std::isinf(dFit_Test))
						{
							fprintf(stdout,"N");
							fflush(stdout);
							bUnrecoverable_Fault = true;
						}
						else
						{
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
								if (std::isnan(dFit_Test) || std::isinf(dFit_Test))
								{
									fprintf(stdout,"N");
									fflush(stdout);
									bUnrecoverable_Fault = true;
								}
								else if (dFit_Test < dBest_Fit) // Centroid is better fit than best of test points, contract toward centroid
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
								for (unsigned int uiI = 0; uiI < uiNum_Points && !bUnrecoverable_Fault; uiI++)
								{
									if (uiBest_Fit_Idx != uiI || dFit_Test < dBest_Fit)
									{
										cDelta = lpcTest_Points[uiI] - vRef_Point;
										lpcTest_Points[uiI] = vRef_Point + dScaling * cDelta;
										fprintf(stdout,"@");
										fflush(stdout);
										//printf("T: %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",lpcTest_Points[uiI].Get(0),lpcTest_Points[uiI].Get(1),lpcTest_Points[uiI].Get(2),lpcTest_Points[uiI].Get(3),lpcTest_Points[uiI].Get(4),lpcTest_Points[uiI].Get(5),lpcTest_Points[uiI].Get(6));
										dFit_Test = i_fUser_Function(lpcTest_Points[uiI],i_lpvUser_Data);
										if (std::isnan(dFit_Test) || std::isinf(dFit_Test))
										{
											fprintf(stdout,"N");
											fflush(stdout);
											bUnrecoverable_Fault = true;
										}
										else
											lpdFit[uiI] = dFit_Test;
									}
								}
							}
						}
					}
				}
			}
			SavePointsCache(ossCache_Filename.str().c_str(),lpcTest_Points,uiNum_Points,uiNum_Parameters,lpdFit);
		}
		fprintf(stdout,"X");
		fflush(stdout);
		o_vResult = xCompute_Centroid(lpcTest_Points, uiNum_Points);

	}
	fprintf(stdout,"\n");
	fflush(stdout);
}

*/

/*class spectra_fit_data_BASE
{
public:
	unsigned int uiIon;
	double dMin_WL;
	double dMax_WL;
	bool bChi2;
	const char * lpszModel_Name;

};*/



class spectra_fit_data
{
public:
	msdb::USER_PARAMETERS	m_cParam;
	msdb::USER_PARAMETERS	m_cContinuum_Band_Param;

	const ES::Spectrum *	m_lpcTarget;
	const XDATASET *				m_lpcOpacity_Map_A;
	const XDATASET *				m_lpcOpacity_Map_B;

	bool					m_bDebug;
	unsigned int			m_uiDebug_Idx;

	specfit::feature_parameters		m_fpTarget_Feature_Parameters;
	double					m_dOII_P_Cygni_Peak_WL;
	double 					m_dCaII_P_Cygni_Peak_WL;

	spectra_fit_data(void) : m_cParam()
	{
		m_lpcTarget = nullptr;
		m_lpcOpacity_Map_A = nullptr;
		m_lpcOpacity_Map_B = nullptr;
		m_bDebug = false;
		m_uiDebug_Idx = 1;
	}
};



#define FIT_BLUE_WL 4500.0
#define FIT_RED_WL 9000.0
void Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, double & o_dTarget_Flux, double & o_dGenerated_Flux)
{
	unsigned int uiIdx = 0;
	double	dLuminosity_Target = 0.0, dLuminosity_Synth = 0.0;
	double	dFit_Delta_Lambda;
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_BLUE_WL)
		uiIdx++;
	dFit_Delta_Lambda = -i_cTarget.wl(uiIdx);

	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_RED_WL)
	{
		double dDelta_Lambda;
		if (uiIdx > 0 && uiIdx  < (i_cTarget.size() - 1))
			dDelta_Lambda = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx - 1)) * 0.5;
		else if (uiIdx == 0)
			dDelta_Lambda = i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx);
		else if (uiIdx == (i_cTarget.size() - 1))
			dDelta_Lambda = i_cTarget.wl(uiIdx) - i_cTarget.wl(uiIdx - 1);

		dLuminosity_Target += i_cTarget.flux(uiIdx) * dDelta_Lambda;
		dLuminosity_Synth += i_cGenerated.flux(uiIdx) * dDelta_Lambda;
		uiIdx++;

	}
	uiIdx--;
	dFit_Delta_Lambda += i_cTarget.wl(uiIdx);

	dLuminosity_Target /= dFit_Delta_Lambda; 
	dLuminosity_Synth /= dFit_Delta_Lambda; 
	o_dTarget_Flux = dLuminosity_Target * 0.5;// * 1.1;
	o_dGenerated_Flux = dLuminosity_Synth * 0.5; //dSynth_Normalization;

}

double	g_dTarget_Normalization_Flux = -1;
double	g_dGenerated_Normalization_Flux = -1;

double	Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiMoment, bool i_bRegenerate_Normalization = true)
{
	double	dRet;
	std::vector<double> vFit_Raw;
	std::vector<double> vFit_Central;
	std::vector<double> vFit_Standardized;

	if (i_bRegenerate_Normalization)
		Get_Normalization_Fluxes(i_cTarget, i_cGenerated, FIT_BLUE_WL, FIT_RED_WL, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
//		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
//	{
//		if (i_cTarget.wl(uiI) == 0.0 || std::isnan(i_cTarget.flux(uiI)) || std::isnan(i_cGenerated.flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",i_cTarget.wl(uiI),i_cTarget.flux(uiI),i_cGenerated.flux(uiI));
//		}
//	}


	Get_Fit_Moments_2(i_cTarget, i_cGenerated, i_dMin_WL, i_dMax_WL, i_uiMoment, vFit_Raw, vFit_Central, vFit_Standardized, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	dRet = fabs(vFit_Raw[i_uiMoment - 1]);
	return dRet; // use mean
}
void msdb_load_generate(msdb::USER_PARAMETERS	&i_cParam, msdb::SPECTRUM_TYPE i_eSpectrum_Type, const ES::Spectrum &i_cTarget, const XDATASET * i_cOp_Map_A, const XDATASET * i_cOp_Map_B, ES::Spectrum & o_cOutput)
{
	msdb::DATABASE	cMSDB;
	if (i_cOp_Map_A != nullptr)
	{
		bool bShell = i_cOp_Map_B != nullptr && i_cOp_Map_B[0].GetNumElements() > 0;
		if (cMSDB.Get_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput) == 0)
		{
			XVECTOR cParameters;
			cParameters.Set_Size(bShell? 7 : 5);
			cParameters.Set(0,1.0); // 1d after explosion
			cParameters.Set(1,i_cParam.m_dPhotosphere_Velocity_kkms); // ps velocity
			cParameters.Set(2,i_cParam.m_dPhotosphere_Temp_kK); // ps temp

			cParameters.Set(3,i_cParam.m_dEjecta_Effective_Temperature_kK); // fix excitation temp
			if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::EJECTA_ONLY)
				cParameters.Set(4,i_cParam.m_dEjecta_Log_Scalar); // PVF scalar
			else
				cParameters.Set(4,-20.0); // PVF scalar

	//				printf("%.5e %.5e\n",dEjecta_Scalar,dEjecta_Scalar + log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref));
			if (bShell)
			{
				cParameters.Set(5,i_cParam.m_dShell_Effective_Temperature_kK); // fix excitation temp
				if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::SHELL_ONLY)
					cParameters.Set(6,i_cParam.m_dShell_Log_Scalar); // HVF scalar
				else
					cParameters.Set(6,-20.0); // HVF scalar
			}
			//cParameters.Print();
			Generate_Synow_Spectra(i_cTarget,i_cOp_Map_A[0],i_cOp_Map_B[0],i_cParam.m_uiIon,cParameters,o_cOutput,-2.0,-2.0);
			msdb::dbid dbidID = cMSDB.Add_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput);
		}
	}
}


double pEW_Fit(const ES::Spectrum &i_cGenerated, const specfit::feature_parameters & i_fpTarget_Feature, specfit::feature_parameters	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	bool bQuit = false;
	double dFlux_Max = -1.0;
	for (unsigned int uiI = 0; uiI < i_cGenerated.size() && !bQuit; uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL > 8900.0) // assume the peak is between 8400 and 8900 A.
			bQuit = true;
		else if (dWL > 8400.0)
		{
			double dFlux = i_cGenerated.flux(uiI);
			if (dFlux > dFlux_Max)
			{
				uiCaII_Idx = uiI;
				dCaII_P_Cygni_Peak_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}
	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(0);
	double dWL_O_Peak = i_cGenerated.wl(0);
	double dFlux_Ca_Peak = dFlux_Max;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	o_cModel_Data.Reset();
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ);
		double dFlux = i_cGenerated.flux(uiJ);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;

		o_cModel_Data.Process_pEW(dFlux_Eff / dContinuum_Flux,dDel_WL_Ang); // 
		o_cModel_Data.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination
	}

	double dpEW_Err = (o_cModel_Data.m_d_pEW - i_fpTarget_Feature.m_d_pEW);
	double dVel_Err = (o_cModel_Data.m_dVmin - i_fpTarget_Feature.m_dVmin);
	double dErr = dpEW_Err * dpEW_Err + dVel_Err * dVel_Err * 0.01; 
	// 0.01 is a weight applied to the velocity component so that it doesn't overpower pEW err
	/// pEW is a value of ~100, with expected error of order 50
	// velocity is a value of ~10000, with expected error of order 500
	return dErr;
}
double Continuum_Fit(const ES::Spectrum &i_cGenerated, const ES::Spectrum &i_cContinuum, specfit::feature_parameters	&o_cModel_Data)
{
	//printf(" [%.2e %.2e] ",i_cGenerated.flux(10),i_cContinuum.flux(10));
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	//FILE * fileTest = fopen("testcn.csv","wt");

	o_cModel_Data.Reset();
	for (unsigned int uiI = 0; uiI < i_cGenerated.size(); uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		double dFlux = i_cGenerated.flux(uiI);
		double dContinuum_Flux = 1.0; // flux has been flattened
		double dFlux_Eff = dContinuum_Flux - dFlux;

		o_cModel_Data.Process_pEW(dFlux,dDel_WL_Ang); // 
		o_cModel_Data.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination

		//fprintf(fileTest,"%.2f, %.17e, %.17e\n",dWL,dFlux,o_cModel_Data.m_d_pEW);
	}
	double dpEW_Err = o_cModel_Data.m_d_pEW; // target value is 0
	double dErr = dpEW_Err * dpEW_Err * 10.0; // increase weight by 10 to really enforce no absorption in this region 
	//fclose(fileTest);
	return dErr;
}

double Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
//	printf(".");fflush(stdout);
	double dFit = DBL_MAX;
	spectra_fit_data *lpcCall_Data = (spectra_fit_data *)i_lpvSpectra_Fit_Data;
	if (lpcCall_Data && lpcCall_Data->m_lpcTarget != nullptr && lpcCall_Data->m_lpcOpacity_Map_A != nullptr)
	{
		//std::cout << lpcCall_Data->m_lpcTarget->wl(0) << " " << lpcCall_Data->m_lpcTarget->flux(0) << std::endl;
		ES::Spectrum cOutput_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());

		ES::Spectrum cOutput_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Continuum_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Continuum_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTarget_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		bool bShell = lpcCall_Data->m_lpcOpacity_Map_B != nullptr && lpcCall_Data->m_lpcOpacity_Map_B->GetNumElements() > 0;
		// look to see if this particular vector has been processed

		msdb::USER_PARAMETERS cParam;

		switch (i_vX.size())
		{
		case 3:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;
			msdb_load_generate(cParam,msdb::COMBINED,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,nullptr,cOutput);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;

			msdb_load_generate(cParam,msdb::COMBINED,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum);
	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
			{
				cOutput_Continuum.flux(uiI) /= cTrue_Continuum.flux(uiI);
			}
			break;
		case 4:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = i_vX[3];
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;

			msdb_load_generate(cParam,msdb::COMBINED,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;

//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			msdb_load_generate(cParam,msdb::COMBINED,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum);
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum);
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
			{
				cOutput_Continuum.flux(uiI) /= cTrue_Continuum.flux(uiI);
			}
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			break;
		case 7:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;
			msdb_load_generate(cParam,msdb::EJECTA_ONLY,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Ej);

			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[3];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[4];
			cParam.m_dEjecta_Log_Scalar = -20.0;
			cParam.m_dShell_Log_Scalar = i_vX[5];
			msdb_load_generate(cParam,msdb::SHELL_ONLY,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Sh);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;

			msdb_load_generate(cParam,msdb::EJECTA_ONLY,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum_Ej);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum_Ej);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;
			cParam.m_dPhotosphere_Temp_kK = i_vX[3];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[4];
			cParam.m_dEjecta_Log_Scalar = -20.0;
			cParam.m_dShell_Log_Scalar = i_vX[5];

			msdb_load_generate(cParam,msdb::SHELL_ONLY,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum_Sh);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum_Sh);

	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum_Sh.size(); uiI++)
			{
	//			if (ofTemp.is_open())
	//			{
	//				ofTemp << cOutput_Continuum.wl(uiI) << ", " << cOutput_Continuum.flux(uiI) << ", " << cTrue_Continuum.flux(uiI) << std::endl;
	//			}
				cOutput_Continuum_Sh.flux(uiI) /= cTrue_Continuum_Sh.flux(uiI);
				cOutput_Continuum_Ej.flux(uiI) /= cTrue_Continuum_Ej.flux(uiI);
				cOutput_Continuum.flux(uiI) = cOutput_Continuum_Ej.flux(uiI) * (1.0 - i_vX[6]) + cOutput_Continuum_Sh.flux(uiI) * i_vX[6];
		
			}

			for (unsigned int uiI = 0; uiI < cOutput_Ej.size(); uiI++)
			{
				cOutput.wl(uiI) = cOutput_Ej.wl(uiI);
				cOutput.flux(uiI) = cOutput_Ej.flux(uiI) * (1.0 - i_vX[6]) + cOutput_Sh.flux(uiI) * i_vX[6];

			}
			break;
		}
		double dTgt_Norm, dGen_Norm;
		Get_Normalization_Fluxes(lpcCall_Data->m_lpcTarget[0], cOutput, FIT_BLUE_WL, FIT_RED_WL, dTgt_Norm, dGen_Norm);
		specfit::feature_parameters cfpModel,cfpCont_Model;
		printf("--");
		fflush(stdout);
		dFit = pEW_Fit(cOutput,lpcCall_Data->m_fpTarget_Feature_Parameters,cfpModel) + Continuum_Fit(cOutput_Continuum,cTrue_Continuum,cfpCont_Model);
		printf(" (%.2f %.1f -- %.2f) ",cfpModel.m_d_pEW,cfpModel.m_dVmin,cfpCont_Model.m_d_pEW);
		fflush(stdout);

		double dScale = lpcCall_Data->m_lpcTarget[0].flux(0) / dTgt_Norm;
		//scaling continuum for fit: the scaling factors for the main fitting region will be used, so scale
		// true continuum to the blue edge of the fit region, and scale the test spectrum to the blue edge / scaling factor for blue edge * scaling factor for actual test region
		// this makes sure that the continuum region gets scaled the same as the main region, so it has equal weight.
		for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
		{
			cOutput_Continuum.flux(uiI) *= dScale * dGen_Norm;
			cTarget_Continuum.flux(uiI) = lpcCall_Data->m_lpcTarget[0].flux(0);
		}
//		dFit = Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cParam.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cParam.m_dWavelength_Range_Upper_Ang,2,true) + Get_Fit(cTarget_Continuum, cOutput_Continuum, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang,2,false);

//		std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
		if (lpcCall_Data->m_bDebug)
		{
			std::ofstream fsDebug;
			std::ostringstream ossDebugFilepath;
			ossDebugFilepath << ".debug/";
			ossDebugFilepath << cParam.m_uiModel_ID;
			ossDebugFilepath << "_";
			ossDebugFilepath << lpcCall_Data->m_uiDebug_Idx;
			ossDebugFilepath << "_";
			ossDebugFilepath << std::setprecision(6);
			ossDebugFilepath << i_vX[0];
			ossDebugFilepath << "_";
			ossDebugFilepath << i_vX[1];
			ossDebugFilepath << "_";
			ossDebugFilepath << i_vX[2];
			if (i_vX.size() > 3)
			{
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[3];
			}
			if (i_vX.size() == 6)
			{
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[4];
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[5];
			}
			ossDebugFilepath << ".csv";

			fsDebug.open(ossDebugFilepath.str().c_str());
			if (fsDebug.is_open())
			{
				fsDebug << std::setprecision(6);
				for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
				{
					fsDebug.flags(std::ios::fixed);
					fsDebug << cOutput_Continuum.wl(uiI) << ", ";
					fsDebug.flags(std::ios::scientific);
					fsDebug << cTarget_Continuum.flux(uiI) / dTgt_Norm << ", ";
					fsDebug << cOutput_Continuum.flux(uiI) / dGen_Norm;
					fsDebug << std::endl;
				}
				for (unsigned int uiI = 0; uiI < lpcCall_Data->m_lpcTarget[0].size(); uiI++)
				{
					fsDebug.flags(std::ios::fixed);
					fsDebug << lpcCall_Data->m_lpcTarget[0].wl(uiI) << ", ";
					fsDebug.flags(std::ios::scientific);
					fsDebug << (lpcCall_Data->m_lpcTarget[0].flux(uiI) / dTgt_Norm) << ", ";
					fsDebug << (cOutput.flux(uiI) / dGen_Norm);
					fsDebug << std::endl;
				}
				fsDebug.close();
			}
			else
				std::cerr << xconsole::bold << xconsole::foreground_red << "Error:" << xconsole::reset << " unable to open file " << ossDebugFilepath.str() << std::endl;
		}
		lpcCall_Data->m_uiDebug_Idx++;
	}
//	fprintf(stdout,"_");
//	fflush(stdout);
	return dFit;
}


void Calc_Observables(const ES::Spectrum &i_cGenerated,const ES::Spectrum &i_cContinuum, specfit::fit_result	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	double dCaII_P_Cygni_Peak_flux;
	bool bQuit = false;
	double dFlux_Max = -1.0;
	for (unsigned int uiI = 0; uiI < i_cGenerated.size() && !bQuit; uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL > 8900.0) // assume the peak is between 8400 and 8900 A.
			bQuit = true;
		else if (dWL > 8400.0)
		{
			double dFlux = i_cGenerated.flux(uiI);
			if (dFlux > dFlux_Max)
			{
				uiCaII_Idx = uiI;
				dCaII_P_Cygni_Peak_WL = dWL;
				dFlux_Max = dFlux;
				
			}
		}
	}
	dCaII_P_Cygni_Peak_flux = dFlux_Max;
	unsigned int uiBlue_Idx;
	double	dBlue_WL;
	dFlux_Max = -1.0;
	bQuit = false;
	for (unsigned int uiI = uiCaII_Idx; uiI > 0 && !bQuit; uiI--)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL < 7000.0) // assume the blue edge is between 7000 and 8000 A.
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			uiBlue_Idx = uiI;
			bQuit = true;
			dBlue_WL = dWL;
			dFlux_Max = dFlux;
		}
		else if (dWL < 8000.0)
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			if (dFlux > 0.98)
			{
				uiBlue_Idx = uiI;
				bQuit = true;
				dBlue_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}


	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx - uiBlue_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(uiBlue_Idx);
	double dWL_O_Peak = i_cGenerated.wl(uiBlue_Idx);
	double dFlux_Ca_Peak = dCaII_P_Cygni_Peak_flux;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	xvector vA, vX, vY, vW, vSigma;
	double dSmin;
	gauss_fit_parameters * lpgfpParamters = &g_cgfpCaNIR;

	vY.Set_Size(uiNum_Points);
	vX.Set_Size(uiNum_Points);
	vW.Set_Size(uiNum_Points);
	o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Reset();
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ + uiBlue_Idx);
		double dFlux = i_cGenerated.flux(uiJ + uiBlue_Idx);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;
		double dNorm_Flux_Eff = dFlux_Eff / dContinuum_Flux;

		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_pEW(dNorm_Flux_Eff,dDel_WL_Ang); // 
		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination

		vX.Set(uiJ,dWL);
		vW.Set(uiJ,0.01); // arbitrary weight
		vY.Set(uiJ,dFlux_Eff);
	}
	
	o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
                    dDel_WL_Ang, o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit_Uncertainty,  
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Fit_S, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrSingle_Gaussian_Fit, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrDouble_Gaussian_Fit);

}

double specfit::GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit, bool i_bDebug, const param_set * i_lppsEjecta, const param_set * i_lppsShell)
{


	spectra_fit_data cCall_Data;

	bool bShell = i_cModel.m_dsShell.GetNumElements() != 0;

	XDATASET dsDummy;
	cCall_Data.m_lpcOpacity_Map_A = &dsDummy;
	cCall_Data.m_lpcOpacity_Map_B = &dsDummy;
	if (bShell)
	{
		cCall_Data.m_lpcOpacity_Map_B = &i_cModel.m_dsShell;
//		std::cout << "Shell map " << std::scientific << i_cModel.m_dsShell.GetElement(1,700) << std::endl;
	}

	cCall_Data.m_bDebug = i_bDebug;
	double dBlue_Edge_WL = 0.0;
	switch (i_cFit.m_eFeature)
	{
	case specfit::CaNIR:
		cCall_Data.m_cParam.m_eFeature = msdb::CaNIR;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		//std::cout << "ej map size " << i_cModel.m_dsEjecta[3].GetNumElements();
		//std::cout << "shell map size " << i_cModel.m_dsShell.GetNumElements();
		cCall_Data.m_cParam.m_uiIon = 2001;
		{
			bool bQuit = false;
			double dFlux_Max = -1.0;
			unsigned int uiIdx = 0;
			unsigned int uiO_Idx = -1;
			unsigned int uiCaII_Idx = -1;
			// between about 7500 and 8000 A is the O feature. Look for it's P cygni peak and only perform fit redward of that point
			for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end() && !bQuit; iterI++)
			{
				double dWL = std::get<0>(*iterI);
				if (dWL > 8000.0)
					bQuit = true;
				else if (dWL > 7500.0)
				{
					double dFlux = std::get<1>(*iterI);
					if (dFlux > dFlux_Max)
					{
						uiO_Idx = uiIdx;
						cCall_Data.m_dOII_P_Cygni_Peak_WL = dWL;
						dFlux_Max = dFlux;
					}
				}
				uiIdx++;
			}
			cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = cCall_Data.m_dOII_P_Cygni_Peak_WL;
			
			// now idenfity the P Cygni peak of the Ca NIR feature
			uiIdx = 0;
			bQuit = false;
			dFlux_Max = -1.0;
			for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end() && !bQuit; iterI++)
			{
				double dWL = std::get<0>(*iterI);
				if (dWL > 8900.0)
					bQuit = true;
				else if (dWL > 8400.0)
				{
					double dFlux = std::get<1>(*iterI);
					if (dFlux > dFlux_Max)
					{
						uiCaII_Idx = uiIdx;
						cCall_Data.m_dCaII_P_Cygni_Peak_WL = dWL;
						dFlux_Max = dFlux;
					}
				}
				uiIdx++;
			}
			//specfit::feature_parameters	cRaw_Data;
//			gauss_fit_parameters * lpgfpParamters = &g_cgfpCaNIR;
//			XVECTOR	vX, vY, vA, vW, vSigma;
//			double	dSmin_Single = DBL_MAX;
//			double	dSmin;
//			double	dSmin_Flat;
//			double	dSmin_Single_Flat;
			unsigned int uiNum_Points = uiCaII_Idx - uiO_Idx + 1;
//			gauss_fit_results	cSingle_Fit;
//			gauss_fit_results	cDouble_Fit;
//			vY.Set_Size(uiNum_Points);
//			vX.Set_Size(uiNum_Points);
//			vW.Set_Size(uiNum_Points);
			double dFlux_O_Peak = std::get<1>(i_cFit.m_vData[uiO_Idx]);
			double dWL_O_Peak = std::get<0>(i_cFit.m_vData[uiO_Idx]);
			double dFlux_Ca_Peak = std::get<1>(i_cFit.m_vData[uiCaII_Idx]);
			double dWL_Ca_Peak = std::get<0>(i_cFit.m_vData[uiCaII_Idx]);
			double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
			
			double dDel_WL_Ang = (std::get<0>(i_cFit.m_vData[uiO_Idx + 1]) - std::get<0>(i_cFit.m_vData[uiO_Idx]));
			o_cFit.m_cTarget_Observables.m_fpParameters.Reset();
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				double dWL = std::get<0>(i_cFit.m_vData[uiJ + uiO_Idx]);
				double dFlux = std::get<1>(i_cFit.m_vData[uiJ + uiO_Idx]);
				double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
				double dFlux_Eff = dContinuum_Flux - dFlux;
//				vX.Set(uiJ,dWL);
//				vW.Set(uiJ,0.01); // arbitrary weight
//				vY.Set(uiJ,dFlux_Eff);

				o_cFit.m_cTarget_Observables.m_fpParameters.Process_pEW(dFlux_Eff / dContinuum_Flux,dDel_WL_Ang); // 
				o_cFit.m_cTarget_Observables.m_fpParameters.Process_Vmin(dWL,dFlux,8542.09);
			}
//           vA = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
//                                dDel_WL_Ang, d_pEW_PVF, d_pEW_HVF, dV_PVF, dV_HVF, vSigma, dSmin, &cSingle_Fit,&cDouble_Fit);
			// Perform Gaussian fit to target feature
			printf("Target pEW = %.2f\tTarget Vel = %.1f\n",o_cFit.m_cTarget_Observables.m_fpParameters.m_d_pEW,o_cFit.m_cTarget_Observables.m_fpParameters.m_dVmin);
			cCall_Data.m_fpTarget_Feature_Parameters = o_cFit.m_cTarget_Observables.m_fpParameters;
			dBlue_Edge_WL = dWL_O_Peak;
		}
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 9000.0;
		break;
	case specfit::CaHK:
		cCall_Data.m_cParam.m_eFeature = msdb::CaHK;
		cCall_Data.m_cParam.m_uiIon = 2001;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 2500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 4000.0;
		break;
	case specfit::Si6355:
		cCall_Data.m_cParam.m_eFeature = msdb::Si6355;
		cCall_Data.m_cParam.m_uiIon = 1401;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 5500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7000.0;
		break;
	case specfit::O7773:
		cCall_Data.m_cParam.m_eFeature = msdb::O7773;
		cCall_Data.m_cParam.m_uiIon = 800;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[1];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 6000.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7500.0;
		break;
	}
//		std::cout << "Ejecta map: " << std::scientific << cCall_Data.m_lpcOpacity_Map_A[0].GetElement(1,250) << std::endl;
	//std::string szCache = ".fitcache";
	ES::Spectrum cTarget;
	ES::Spectrum cFull_Target;
	cCall_Data.m_lpcTarget = &cTarget;
	if (!i_cFit.m_vData.empty())
	{
		unsigned int uiCount =  0;
		spectraldata vSpec_Subset;
		double dWL_Min = DBL_MAX, dWL_Max =-DBL_MAX;
		for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end(); iterI++)
		{
			double dWL = std::get<0>(*iterI);
			if (dWL >= cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang &&
				dWL <= cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang)
			{
				vSpec_Subset.push_back(*iterI);
				if (dWL < dWL_Min)
					dWL_Min = dWL;
				if (dWL > dWL_Max)
					dWL_Max = dWL;
			}
		}

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = dWL_Min;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = dWL_Max;
		cTarget = ES::Spectrum::create_from_size(vSpec_Subset.size());
		cFull_Target = ES::Spectrum::create_from_size(i_cFit.m_vData.size());
		// fill target spectrum
		unsigned int uiIdx = 0;
		for (specfit::spectraldata::const_iterator iterI = vSpec_Subset.cbegin(); iterI != vSpec_Subset.end(); iterI++)
		{
			cTarget.wl(uiIdx) = std::get<0>(*iterI);
			cTarget.flux(uiIdx) = std::get<1>(*iterI);
			cTarget.flux_error(uiIdx) = std::get<2>(*iterI);
			uiIdx++;
		}
		uiIdx = 0;
		for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end(); iterI++)
		{
			cFull_Target.wl(uiIdx) = std::get<0>(*iterI);
			cFull_Target.flux(uiIdx) = std::get<1>(*iterI);
			cFull_Target.flux_error(uiIdx) = std::get<2>(*iterI);
			uiIdx++;
		}
	}

	cCall_Data.m_cParam.m_dTime_After_Explosion = 1.0; // 1d after explosion, since this is degenerate with log S
	
	cCall_Data.m_cParam.m_dWavelength_Delta_Ang = fabs(cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang - cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang) / cTarget.size();
	cCall_Data.m_cParam.m_dEjecta_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion
	cCall_Data.m_cParam.m_dShell_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion
	cCall_Data.m_cParam.m_uiModel_ID = i_cModel.m_uiModel_ID;



	unsigned int uiParameters = 3;
	if (bShell)
		uiParameters = 4;
	if (i_cFit.m_bUse_Two_Component_Fit)
		uiParameters = 7;

	XVECTOR	vStarting_Point;
	XVECTOR	vLog_S_Delta;
	XVECTOR	vVariations;
	XVECTOR	vEpsilon;
	XVECTOR	vLower_Bounds;
	XVECTOR	vUpper_Bounds;
	std::vector<bool> vLower_Bounds_Valid;
	std::vector<bool> vUpper_Bounds_Valid;
	double dDelta_Bmax = 1.0;
	if (!std::isnan(i_cFit.m_dMJD_Bmax))
		dDelta_Bmax = i_cFit.m_dMJD - i_cFit.m_dMJD_Bmax + i_cFit.m_dDelay_From_Explosion_To_Bmax;
	//printf("dbmax %f\n",dDelta_Bmax);
	if (dDelta_Bmax < 1.0)
		dDelta_Bmax = 1.0;
	vStarting_Point.Set_Size(uiParameters);
	vLog_S_Delta.Set_Size(uiParameters);

	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Temp))
		vStarting_Point.Set(0,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Temp);
	else
		vStarting_Point.Set(0,10.0);
	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel))
		vStarting_Point.Set(1,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel);
	else if (!std::isnan(i_cFit.m_dMJD_Bmax))
	{
		const XDATASET *lpvPS_Data = nullptr;
		if (i_cFit.m_bUse_Two_Component_Fit)
		{
			lpvPS_Data = &i_cModel.m_dsEjecta_Photosphere;
		}
		else
		{
			lpvPS_Data = &i_cModel.m_dsPhotosphere;
		}
		unsigned int uiI = 0;
		while (uiI < lpvPS_Data->GetNumRows() && lpvPS_Data->GetElement(0,uiI) < dDelta_Bmax)
			uiI++;
		double dPS_Vel;
		if (uiI == lpvPS_Data->GetNumRows())
			uiI = lpvPS_Data->GetNumRows() - 1; // extrapolate from the end of the table
		if (uiI != 0)
		{
			dPS_Vel = ((lpvPS_Data->GetElement(4,uiI) - lpvPS_Data->GetElement(4,uiI - 1)) / (lpvPS_Data->GetElement(0,uiI) - lpvPS_Data->GetElement(0,uiI - 1)) * (dDelta_Bmax - lpvPS_Data->GetElement(0,uiI - 1)) + lpvPS_Data->GetElement(4,uiI - 1)) * 1.0e-8;
			if (dPS_Vel < 1.0) // we expect the material to still be optically thick;  enforce a lower limit of about 1000 km/s
				dPS_Vel = 1.0;
		}
		else
			dPS_Vel = lpvPS_Data->GetElement(4,uiI) * 1.0e-8;
		vStarting_Point.Set(1,dPS_Vel);
	}
	else
		vStarting_Point.Set(1,15.0);
	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S))
		vStarting_Point.Set(2,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S);
	else
		vStarting_Point.Set(2,2.0);
	if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
		vLog_S_Delta.Set(2,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	if (uiParameters == 4) // shell and single component fit
	{
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S))
			vStarting_Point.Set(3,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S);
		else
			vStarting_Point.Set(3,1.7);
		if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
			vLog_S_Delta.Set(3,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	}
	else if (uiParameters == 7)
	{
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Temp))
			vStarting_Point.Set(3,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Temp);
		else
			vStarting_Point.Set(3,10.0);
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel))
			vStarting_Point.Set(4,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel);
		else if (!std::isnan(i_cFit.m_dMJD_Bmax))
		{
			const XDATASET *lpvPS_Data = nullptr;
			lpvPS_Data = &i_cModel.m_dsPhotosphere;
			unsigned int uiI = 0;
			while (uiI < lpvPS_Data->GetNumRows() && lpvPS_Data->GetElement(0,uiI) < dDelta_Bmax)
				uiI++;
			double dPS_Vel;
			if (uiI != 0)
			{
				dPS_Vel = ((lpvPS_Data->GetElement(4,uiI) - lpvPS_Data->GetElement(4,uiI - 1)) / (lpvPS_Data->GetElement(0,uiI) - lpvPS_Data->GetElement(0,uiI - 1)) * (dDelta_Bmax - lpvPS_Data->GetElement(0,uiI - 1)) + lpvPS_Data->GetElement(4,uiI - 1)) * 1.0e-8;
			}
			else
				dPS_Vel = lpvPS_Data->GetElement(4,uiI) * 1.0e-8;
			vStarting_Point.Set(4,dPS_Vel);
		}
		else
			vStarting_Point.Set(4,20.0);
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S))
			vStarting_Point.Set(5,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S);
		else
			vStarting_Point.Set(5,1.7);
		if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
			vLog_S_Delta.Set(5,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
		if (!std::isnan(i_cFit.m_cSuggested_Param.m_dMixing_Fraction))
			vStarting_Point.Set(6,i_cFit.m_cSuggested_Param.m_dMixing_Fraction);
		else
			vStarting_Point.Set(6,0.25);
	}
	//printf("logsdelta %f\n",vLog_S_Delta[2]);
	vStarting_Point += vLog_S_Delta;
	vEpsilon.Set_Size(uiParameters);
	vEpsilon.Set(0,0.05);
	vEpsilon.Set(1,0.05);
	vEpsilon.Set(2,0.05);
	if (uiParameters == 4)
	{
		vEpsilon.Set(3,0.05);
	}
	else if (uiParameters == 7)
	{
		vEpsilon.Set(3,0.05);
		vEpsilon.Set(4,0.05);
		vEpsilon.Set(5,0.05);
		vEpsilon.Set(6,0.025);
	}
	vVariations.Set_Size(uiParameters);
	vVariations.Set(0,1.0);
	vVariations.Set(1,1.0);
	vVariations.Set(2,1.0);
	if (uiParameters == 4)
	{
		vVariations.Set(3,1.0);
	}
	else if (uiParameters == 7)
	{
		vVariations.Set(3,1.0);
		vVariations.Set(4,1.0);
		vVariations.Set(5,1.0);
		vVariations.Set(6,0.2);
	}
	vLower_Bounds.Set_Size(uiParameters);
	vLower_Bounds_Valid.resize(uiParameters);
	vLower_Bounds.Set(0,-5.0);
	if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
		vLower_Bounds.Set(1,-2.0);
	else
		vLower_Bounds.Set(1,-5.0);
	vLower_Bounds.Set(2,-0.5);
	vLower_Bounds_Valid[0] = true;
	vLower_Bounds_Valid[1] = true;
	vLower_Bounds_Valid[2] = true;
	if (uiParameters == 4)
	{
		vLower_Bounds.Set(3,-0.5);
		vLower_Bounds_Valid[3] = true;
	}
	else if (uiParameters == 7)
	{
		vLower_Bounds.Set(3,-5.0);
		vLower_Bounds_Valid[3] = true;
		if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
			vLower_Bounds.Set(4,-3.0);
		else
			vLower_Bounds.Set(4,-10.0);
		vLower_Bounds_Valid[4] = true;
		vLower_Bounds.Set(5,-0.5);
		vLower_Bounds_Valid[5] = true;
		vLower_Bounds.Set(6,-0.25);
		vLower_Bounds_Valid[6] = true;
	}
	vUpper_Bounds.Set_Size(uiParameters);
	vUpper_Bounds_Valid.resize(uiParameters);
	vUpper_Bounds_Valid[0] = true;
	vUpper_Bounds_Valid[1] = true;
	vUpper_Bounds_Valid[2] = true;
	vUpper_Bounds.Set(0,10.0);
	if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
		vUpper_Bounds.Set(1,2.0);
	else
		vUpper_Bounds.Set(1,5.0);
	vUpper_Bounds.Set(2,0.5);
	if (uiParameters > 3)
	{
		vUpper_Bounds.Set(3,0.5);
		vUpper_Bounds_Valid[3] = true;
	}
	else if (uiParameters == 7)
	{
		vUpper_Bounds.Set(3,10.0);
		vUpper_Bounds_Valid[3] = true;
		if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
			vUpper_Bounds.Set(4,2.0);
		else
			vUpper_Bounds.Set(4,5.0);
		vUpper_Bounds_Valid[4] = true;
		vUpper_Bounds.Set(5,0.5);
		vUpper_Bounds_Valid[5] = true;
		vUpper_Bounds.Set(6,0.75);
		vUpper_Bounds_Valid[6] = true;
	}
	vUpper_Bounds += vStarting_Point;
	vLower_Bounds += vStarting_Point;

	cCall_Data.m_cContinuum_Band_Param = cCall_Data.m_cParam;
	switch (i_cFit.m_eFeature)
	{
	case specfit::CaNIR:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = dBlue_Edge_WL - 100.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang - 1500.0;
		break;
	case specfit::CaHK:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 5500.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 7000.0;
		break;
	case specfit::Si6355:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 7500.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 8500.0;
		break;
	case specfit::O7773:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 5000.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 6500.0;
		break;
	}
	cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Delta_Ang = fabs(cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang - cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang) / cTarget.size();


	if (vStarting_Point.is_nan())
	{
		std::cerr << "Error: starting point contains nan." << std::endl;
		return DBL_MAX;
	}
	else if (i_lppsEjecta != nullptr || i_lppsShell != nullptr) // the user has specified an exact set of parameters to use.
	{
		vStarting_Point.Set(0,i_lppsEjecta->m_dPS_Temp);
		vStarting_Point.Set(1,i_lppsEjecta->m_dPS_Vel);
		vStarting_Point.Set(2,i_lppsEjecta->m_dLog_S + vLog_S_Delta[2]);
		vStarting_Point.Set(3,i_lppsShell->m_dLog_S + vLog_S_Delta[3]);

		Fit_Function(vStarting_Point, &cCall_Data);
	}
	else
		XFIT_Simplex(vStarting_Point, vVariations, vEpsilon, Fit_Function, &cCall_Data, false, &vLower_Bounds, &vLower_Bounds_Valid, &vUpper_Bounds, &vUpper_Bounds_Valid);

//	xPerform_Fit_Bound_Simplex(cBounds[0],cBounds[1],cThreshold,Fit_Function,cResult,&cCall_Data,szCache.c_str());

//	printf("Generating\n");

	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Vel = vStarting_Point[1];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Temp = vStarting_Point[0];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dLog_S = vStarting_Point[2];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dExcitation_Temp = 10000.0;

	if (uiParameters == 7)
	{
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Vel = vStarting_Point[3];
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Temp = vStarting_Point[4];
		o_cFit.m_cParams[specfit::comp_shell].m_dLog_S = vStarting_Point[5];
		o_cFit.m_cParams[specfit::comp_shell].m_dExcitation_Temp = 10000.0;
		o_cFit.m_cParams.m_dMixing_Fraction = vStarting_Point[6];
	}
	else if (uiParameters == 4)
	{
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Vel = vStarting_Point[1];
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Temp = vStarting_Point[0];
		o_cFit.m_cParams[specfit::comp_shell].m_dLog_S = vStarting_Point[3];
		o_cFit.m_cParams[specfit::comp_shell].m_dExcitation_Temp = 10000.0;
	}

	o_cFit.m_dMJD = i_cFit.m_dMJD;
	o_cFit.m_eFeature = i_cFit.m_eFeature;
	o_cFit.m_szInstrument = i_cFit.m_szInstrument;
	o_cFit.m_szSource = i_cFit.m_szSource;
	o_cFit.m_uiModel = i_cModel.m_uiModel_ID;

	ES::Spectrum csFull_Result(cFull_Target);
	ES::Spectrum csResult(cTarget);
	ES::Spectrum cContinuum(cFull_Target);
	ES::Spectrum cContinuum_EO(cFull_Target);
	ES::Spectrum cContinuum_SO(cFull_Target);
	ES::Spectrum csResult_EO(cFull_Target);
	ES::Spectrum csResult_SO(cFull_Target);
	ES::Spectrum cFull_Result_EO(cFull_Target);
	ES::Spectrum cFull_Result_SO(cFull_Target);
	csResult.zero_flux();
	cContinuum.zero_flux();
	csResult_EO.zero_flux();
	csResult_SO.zero_flux();
	csFull_Result.zero_flux();
	cFull_Result_EO.zero_flux();
	cFull_Result_SO.zero_flux();
	cContinuum_EO.zero_flux();
	cContinuum_SO.zero_flux();

	cCall_Data.m_cParam.m_dTime_After_Explosion = 1.0;
	cCall_Data.m_cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cCall_Data.m_cParam.m_dShell_Effective_Temperature_kK = 10.0;

	
	if (uiParameters == 7)
	{
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = -20.0;
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult_EO);

		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[3];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[4];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = -20.0;
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[5];
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult_SO);
		for (unsigned int uiI = 0; uiI < csResult_EO.size(); uiI++)
		{
			csResult.wl(uiI) = csResult_EO.wl(uiI);
			csResult.flux(uiI) = csResult_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + csResult_SO.flux(uiI) * vStarting_Point[6];
		}

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = std::get<0>(*(i_cFit.m_vData.begin()));
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = std::get<0>(*(i_cFit.m_vData.rbegin()));
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = -20.0;
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum_EO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_EO);

		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[3];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[4];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = -20.0;
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[5];
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum_SO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_SO);

		for (unsigned int uiI = 0; uiI < cContinuum_EO.size(); uiI++)
		{
			cContinuum.wl(uiI) = cContinuum_EO.wl(uiI);
			cContinuum.flux(uiI) = cContinuum_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + cContinuum_SO.flux(uiI) * vStarting_Point[6];

			csFull_Result.wl(uiI) = cFull_Result_EO.wl(uiI);
			csFull_Result.flux(uiI) = cFull_Result_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + cFull_Result_SO.flux(uiI) * vStarting_Point[6];
		}
	}
	else
	{
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[3];
		msdb_load_generate(cCall_Data.m_cParam, msdb::COMBINED, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult);

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = std::get<0>(*(i_cFit.m_vData.begin()));
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = std::get<0>(*(i_cFit.m_vData.rbegin()));
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum);
		msdb_load_generate(cCall_Data.m_cParam, msdb::COMBINED, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csFull_Result);
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_EO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_SO);
	}

	//printf("...");fflush(stdout);
	Calc_Observables(csFull_Result,cContinuum,o_cFit);

	double dTarget_Flux, dGenerated_Flux;
	Get_Normalization_Fluxes(cFull_Target, csFull_Result, FIT_BLUE_WL, FIT_RED_WL, dTarget_Flux, dGenerated_Flux);

	double	dNorm = dTarget_Flux  / dGenerated_Flux;
	for (unsigned int uiI = 0; uiI < cFull_Target.size(); uiI++)
	{
		o_cFit.m_vpdSpectrum_Target.push_back(std::pair<double,double>(cFull_Target.wl(uiI),cFull_Target.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Ejecta_Only.push_back(std::pair<double,double>(cFull_Result_EO.wl(uiI),cFull_Result_EO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Shell_Only.push_back(std::pair<double,double>(cFull_Result_SO.wl(uiI),cFull_Result_SO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Continuum.push_back(std::pair<double,double>(cContinuum.wl(uiI),cContinuum.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic.push_back(std::pair<double,double>(csFull_Result.wl(uiI),csFull_Result.flux(uiI) * dNorm));
	}
	//feature_parameters cfpModel,cfpCont_Model;

	//dFit = pEW_Fit(cOutput,lpcCall_Data->m_fpTarget_Feature_Parameters,cfpModel) + Continuum_Fit(cOutput_Continuum,cTrue_Continuum,cfpCont_Model);

	std::vector<double> vError;
	for (unsigned int uiI = 0; uiI < cTarget.size(); uiI++)
		vError.push_back(cTarget.flux(uiI) - csResult.flux(uiI) * dNorm);
	
	Get_Raw_Moments(vError,6,o_cFit.m_vdRaw_Moments);
	Get_Central_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdCentral_Moments);
	Get_Standardized_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdStandardized_Moments);


	return o_cFit.m_vdRaw_Moments[1];
}

