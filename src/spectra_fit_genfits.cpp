#include <specfit.h>
#include <model_spectra_db.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>

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

	const ES::Spectrum *	m_lpcTarget;
	const XDATASET *				m_lpcOpacity_Map_A;
	const XDATASET *				m_lpcOpacity_Map_B;

	spectra_fit_data(void) : m_cParam()
	{
		m_lpcTarget = nullptr;
		m_lpcOpacity_Map_A = nullptr;
		m_lpcOpacity_Map_B = nullptr;
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

double Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
	msdb::DATABASE	cMSDB;
	double dFit = DBL_MAX;
	spectra_fit_data *lpcCall_Data = (spectra_fit_data *)i_lpvSpectra_Fit_Data;
	if (lpcCall_Data && lpcCall_Data->m_lpcTarget != nullptr && lpcCall_Data->m_lpcOpacity_Map_A != nullptr)
	{
		//std::cout << lpcCall_Data->m_lpcTarget->wl(0) << " " << lpcCall_Data->m_lpcTarget->flux(0) << std::endl;
		ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());

		bool bShell = lpcCall_Data->m_lpcOpacity_Map_B != nullptr && lpcCall_Data->m_lpcOpacity_Map_B->GetNumElements() > 0;
		// look to see if this particular vector has been processed

		lpcCall_Data->m_cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
		lpcCall_Data->m_cParam.m_dEjecta_Log_Scalar = i_vX[3];
		lpcCall_Data->m_cParam.m_dShell_Log_Scalar = i_vX[4];
		lpcCall_Data->m_cParam.m_dPhotosphere_Temp_kK = i_vX[2];
		lpcCall_Data->m_cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
		lpcCall_Data->m_cParam.m_dShell_Effective_Temperature_kK = 10.0;

		if (cMSDB.Get_Spectrum(lpcCall_Data->m_cParam, msdb::COMBINED, cOutput) == 0)
		{
			XVECTOR cParameters;
			cParameters.Set_Size(bShell? 7 : 5);
			cParameters.Set(0,1.0); // 1d after explosion
			cParameters.Set(1,lpcCall_Data->m_cParam.m_dPhotosphere_Velocity_kkms); // ps velocity
			cParameters.Set(2,lpcCall_Data->m_cParam.m_dPhotosphere_Temp_kK); // ps temp

			cParameters.Set(3,lpcCall_Data->m_cParam.m_dEjecta_Effective_Temperature_kK); // fix excitation temp
			cParameters.Set(4,lpcCall_Data->m_cParam.m_dEjecta_Log_Scalar); // PVF scalar
	//				printf("%.5e %.5e\n",dEjecta_Scalar,dEjecta_Scalar + log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref));
			if (bShell)
			{
				cParameters.Set(5,lpcCall_Data->m_cParam.m_dShell_Effective_Temperature_kK); // fix excitation temp
				cParameters.Set(6,lpcCall_Data->m_cParam.m_dShell_Log_Scalar); // HVF scalar
			}
			Generate_Synow_Spectra(lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A[0],lpcCall_Data->m_lpcOpacity_Map_B[0],lpcCall_Data->m_cParam.m_uiIon,cParameters,cOutput,-2.0,-2.0);
			msdb::dbid dbidID = cMSDB.Add_Spectrum(lpcCall_Data->m_cParam, msdb::COMBINED, cOutput);
		}
	//	fprintf(stdout,".");
	//	fflush(stdout);
		dFit = Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cParam.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cParam.m_dWavelength_Range_Upper_Ang,2,true);
	}
//	fprintf(stdout,"_");
//	fflush(stdout);
	return dFit;
}

double specfit::GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit)
{
	std::string szCache = ".fitcache";
	ES::Spectrum cTarget;
	if (!i_cFit.m_vData.empty())
	{
		double dWL_low = std::get<0>(i_cFit.m_vData[0]);
		double dWL_High = std::get<0>(i_cFit.m_vData[i_cFit.m_vData.size() - 1]);
		size_t uiNum = i_cFit.m_vData.size();
		cTarget = ES::Spectrum::create_from_range_and_size(dWL_low , dWL_High ,uiNum);
		// fill target spectrum
		for (unsigned int uiI = 0; uiI < i_cFit.m_vData.size(); uiI++)
		{
			cTarget.wl(uiI) = std::get<0>(i_cFit.m_vData[uiI]);
			cTarget.flux(uiI) = std::get<1>(i_cFit.m_vData[uiI]);
			cTarget.flux_error(uiI) = std::get<2>(i_cFit.m_vData[uiI]);
		}
	}


	spectra_fit_data cCall_Data;

	bool bShell = i_cModel.m_dsShell.GetNumElements() != 0;

	XDATASET dsDummy;
	cCall_Data.m_lpcTarget = &cTarget;
	cCall_Data.m_lpcOpacity_Map_A = &dsDummy;
	cCall_Data.m_lpcOpacity_Map_B = &dsDummy;
	if (bShell)
	{
		cCall_Data.m_lpcOpacity_Map_B = &i_cModel.m_dsShell;
	}

	unsigned int	m_uiModel_ID;
	switch (i_cFit.m_eFeature)
	{
	case specfit::CaNIR:
		cCall_Data.m_cParam.m_eFeature = msdb::CaNIR;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		//std::cout << "ej map size " << i_cModel.m_dsEjecta[3].GetNumElements();
		//std::cout << "shell map size " << i_cModel.m_dsShell.GetNumElements();
		cCall_Data.m_cParam.m_uiIon = 2001;
		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 7500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 9000.0;
		break;
	case specfit::CaHK:
		cCall_Data.m_cParam.m_eFeature = msdb::CaHK;
		cCall_Data.m_cParam.m_uiIon = 2001;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 2500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 4000.0;
		break;
	case specfit::Si6355:
		cCall_Data.m_cParam.m_eFeature = msdb::Si6355;
		cCall_Data.m_cParam.m_uiIon = 1401;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 5500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7000.0;
		break;
	case specfit::O7773:
		cCall_Data.m_cParam.m_eFeature = msdb::O7773;
		cCall_Data.m_cParam.m_uiIon = 800;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[1];
		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 6000.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7500.0;
		break;
	}
	cCall_Data.m_cParam.m_dTime_After_Explosion = 1.0; // 1d after explosion, since this is degenerate with log S
	
	cCall_Data.m_cParam.m_dWavelength_Delta_Ang = fabs(cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang - cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang) / cTarget.size();
	cCall_Data.m_cParam.m_dEjecta_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion
	cCall_Data.m_cParam.m_dShell_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion




	unsigned int uiParameters = 3;
	if (bShell)
		uiParameters = 4;

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
		dDelta_Bmax = i_cFit.m_dMJD_Bmax - i_cFit.m_dMJD - i_cFit.m_dDelay_From_Explosion_To_Bmax;
	printf("dbmax %f\n",dDelta_Bmax);
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
	else
		vStarting_Point.Set(1,15.0);
	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S))
		vStarting_Point.Set(2,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S);
	else
		vStarting_Point.Set(2,3.5);
	if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
		vLog_S_Delta.Set(2,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	if (uiParameters > 3)
	{
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S))
			vStarting_Point.Set(3,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S);
		else
			vStarting_Point.Set(3,4.5);
		if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
			vLog_S_Delta.Set(3,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	}
	printf("logsdelta %f\n",vLog_S_Delta[2]);
	vStarting_Point += vLog_S_Delta;
	vEpsilon.Set_Size(uiParameters);
	vEpsilon.Set(0,0.05);
	vEpsilon.Set(1,0.05);
	vEpsilon.Set(2,0.05);
	if (uiParameters > 3)
	{
		vEpsilon.Set(3,0.05);
	}
	vVariations.Set_Size(uiParameters);
	vVariations.Set(0,1.0);
	vVariations.Set(1,1.0);
	vVariations.Set(2,1.0);
	if (uiParameters > 3)
	{
		vVariations.Set(3,1.0);
	}
	vLower_Bounds.Set_Size(uiParameters);
	vLower_Bounds_Valid.resize(uiParameters);
	vLower_Bounds.Set(0,-5.0);
	vLower_Bounds.Set(1,-10.0);
	vLower_Bounds.Set(2,-1.0);
	vLower_Bounds_Valid[0] = true;
	vLower_Bounds_Valid[1] = true;
	vLower_Bounds_Valid[2] = true;
	if (uiParameters > 3)
	{
		vLower_Bounds.Set(3,-1.0);
		vLower_Bounds_Valid[3] = true;
	}
	vUpper_Bounds.Set_Size(uiParameters);
	vUpper_Bounds_Valid.resize(uiParameters);
	vUpper_Bounds_Valid[0] = true;
	vUpper_Bounds_Valid[1] = true;
	vUpper_Bounds_Valid[2] = true;
	vUpper_Bounds.Set(0,10.0);
	vUpper_Bounds.Set(1,10.0);
	vUpper_Bounds.Set(2,1.0);
	if (uiParameters > 3)
	{
		vUpper_Bounds.Set(3,1.0);
		vUpper_Bounds_Valid[3] = true;
	}
	vUpper_Bounds += vStarting_Point;
	vLower_Bounds += vStarting_Point;


	XFIT_Simplex(vStarting_Point, vVariations, vEpsilon, Fit_Function, &cCall_Data, false, &vLower_Bounds, &vLower_Bounds_Valid, &vUpper_Bounds, &vUpper_Bounds_Valid);

//	xPerform_Fit_Bound_Simplex(cBounds[0],cBounds[1],cThreshold,Fit_Function,cResult,&cCall_Data,szCache.c_str());

//	printf("Generating\n");

	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Vel = vStarting_Point[0];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Temp = vStarting_Point[1];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dLog_S = vStarting_Point[2];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dExcitation_Temp = 10000.0;

	o_cFit.m_cParams[specfit::comp_shell].m_dPS_Vel = vStarting_Point[0];
	o_cFit.m_cParams[specfit::comp_shell].m_dPS_Temp = vStarting_Point[1];
	o_cFit.m_cParams[specfit::comp_shell].m_dLog_S = vStarting_Point[3];
	o_cFit.m_cParams[specfit::comp_shell].m_dExcitation_Temp = 10000.0;

	o_cFit.m_dMJD = i_cFit.m_dMJD;
	o_cFit.m_eFeature = i_cFit.m_eFeature;
	o_cFit.m_szInstrument = i_cFit.m_szInstrument;
	o_cFit.m_szSource = i_cFit.m_szSource;
	o_cFit.m_uiModel = i_cModel.m_uiModel_ID;

	XVECTOR cContinuum_Parameters(7);
	XVECTOR cParameters(vStarting_Point);
	cContinuum_Parameters.Set(0,vStarting_Point[0]);
	cContinuum_Parameters.Set(1,vStarting_Point[1]);
	cContinuum_Parameters.Set(2,-20.0); // PS ion log tau
	cContinuum_Parameters.Set(3,10.0); // PS ion exctication temp
	cContinuum_Parameters.Set(4,vStarting_Point[0]); // PS ion vmin
	cContinuum_Parameters.Set(5,80.0); // PS ion vmax
	cContinuum_Parameters.Set(6,1.0); // PS ion vscale

	ES::Spectrum csResult  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
	ES::Spectrum cContinuum  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
	ES::Spectrum cResult_EO  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
	ES::Spectrum cResult_SO  = ES::Spectrum::create_from_range_and_size( cTarget.wl(0), cTarget.wl(cTarget.size() - 1), cTarget.size());
	msdb::DATABASE cMSDB(false);

	cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
	cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[3];
	cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[4];
	cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[2];
	cCall_Data.m_cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cCall_Data.m_cParam.m_dShell_Effective_Temperature_kK = 10.0;

	//cParam.m_dPhotosphere_Velocity_kkms = cContinuum_Parameters.Get(0);
	if (cMSDB.Get_Spectrum(cCall_Data.m_cParam, msdb::CONTINUUM, cContinuum) == 0)
	{
		//printf("generating continuua\n");
		Generate_Synow_Spectra_Exp(cContinuum,2001,cContinuum_Parameters,cContinuum); // ion irrelevant for this 
		//printf("Adding to db\n");
		cMSDB.Add_Spectrum(cCall_Data.m_cParam, msdb::CONTINUUM, cContinuum);
		//printf("done\n");
	}
	if (cMSDB.Get_Spectrum(cCall_Data.m_cParam, msdb::COMBINED, csResult) == 0)
	{
		Generate_Synow_Spectra(csResult, cCall_Data.m_lpcOpacity_Map_A[0], cCall_Data.m_lpcOpacity_Map_B[0], cCall_Data.m_cParam.m_uiIon, cParameters, csResult,-2.0,-2.0);
		msdb::dbid dbidID = cMSDB.Add_Spectrum(cCall_Data.m_cParam, msdb::COMBINED, csResult);
		if (!bShell)
		{
			cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, csResult);
		}
		else
		{
			cParameters.Set(6,-40.0);
			Generate_Synow_Spectra(cResult_EO, cCall_Data.m_lpcOpacity_Map_A[0], cCall_Data.m_lpcOpacity_Map_B[0], cCall_Data.m_cParam.m_uiIon, cParameters, cResult_EO,-2.0,-2.0);
			cMSDB.Add_Spectrum(dbidID, msdb::EJECTA_ONLY, cResult_EO);
			cParameters.Set(6,vStarting_Point[3]);

			cParameters.Set(4,-40.0);
			Generate_Synow_Spectra(cResult_SO, cCall_Data.m_lpcOpacity_Map_A[0], cCall_Data.m_lpcOpacity_Map_B[0], cCall_Data.m_cParam.m_uiIon, cParameters, cResult_SO,-2.0,-2.0);
			cMSDB.Add_Spectrum(dbidID, msdb::SHELL_ONLY, cResult_SO);
			cParameters.Set(4,vStarting_Point[2]);
		}
	}
	else
	{
		if (cMSDB.Get_Spectrum(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cResult_EO) == 0)
		{
			cParameters.Set(6,-40.0);
			Generate_Synow_Spectra(cResult_EO, cCall_Data.m_lpcOpacity_Map_A[0], cCall_Data.m_lpcOpacity_Map_B[0], cCall_Data.m_cParam.m_uiIon, cParameters, cResult_EO,-2.0,-2.0);
			msdb::dbid dbidID = cMSDB.Add_Spectrum(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cResult_EO);
			cParameters.Set(6,vStarting_Point[3]);
		}
		if (bShell)
		{
			if (cMSDB.Get_Spectrum(cCall_Data.m_cParam, msdb::SHELL_ONLY, cResult_SO) == 0)
			{
				cParameters.Set(4,-40.0);
				Generate_Synow_Spectra(cResult_SO, cCall_Data.m_lpcOpacity_Map_A[0], cCall_Data.m_lpcOpacity_Map_B[0], cCall_Data.m_cParam.m_uiIon, cParameters, cResult_SO,-2.0,-2.0);
				msdb::dbid dbidID = cMSDB.Add_Spectrum(cCall_Data.m_cParam, msdb::SHELL_ONLY, cResult_SO);
				cParameters.Set(4,vStarting_Point[2]);
			}
		}
	}


	for (unsigned int uiI = 0; uiI < cTarget.size(); uiI++)
	{
		o_cFit.m_vpdSpectrum_Target.push_back(std::pair<double,double>(cTarget.wl(uiI),cTarget.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Ejecta_Only.push_back(std::pair<double,double>(cResult_EO.wl(uiI),cResult_EO.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Shell_Only.push_back(std::pair<double,double>(cResult_SO.wl(uiI),cResult_SO.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Continuum.push_back(std::pair<double,double>(cContinuum.wl(uiI),cContinuum.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic.push_back(std::pair<double,double>(csResult.wl(uiI),csResult.flux(uiI)));
	}
	double * lpdX = new double[cTarget.size()];
	for (unsigned int uiI = 0; uiI < cTarget.size(); uiI++)
		lpdX[uiI] = cTarget.flux(uiI) - csResult.flux(uiI);
	
	Get_Raw_Moments(lpdX,cTarget.size(),6,o_cFit.m_vdRaw_Moments);
	Get_Central_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdCentral_Moments);
	Get_Standardized_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdStandardized_Moments);
	delete [] lpdX;

	return o_cFit.m_vdRaw_Moments[1];
}


