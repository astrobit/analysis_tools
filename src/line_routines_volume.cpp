
#include <xlinalg.h>

////////////////////////////////////////////////////////////////////////////////
//
// These functions should be moved into xlibs
// Functions in this file are related to convergence of a set of vectors,
// testing if a point is within a defined volumne, or computing the centroid of
// a volume (defined by its vertices)
//
////////////////////////////////////////////////////////////////////////////////


// Does a point (vector) fall fully inside a given set of bounds.  e.g. x_i < y_i for all i
bool xPoint_Within_Bound(const XVECTOR &i_lpVector, const XVECTOR &i_cThreshold)
{
	XVECTOR cTest;
	bool bRet = (i_cThreshold.Get_Size() == i_lpVector.Get_Size());
	if (!bRet)
		fprintf(stderr,"xPoint_Within_Bound: Convergence threshold size not the same as paramter vector size!\n");

	for (unsigned int uiK = 0; uiK < i_lpVector.Get_Size() && bRet; uiK++)
	{
		bRet = (fabs(i_lpVector.Get(uiK)) < i_cThreshold.Get(uiK));
	}
	return bRet;
}

// Given a set of points in n-space and a bound, is the space between all points less than the bound.  e.g. (|x_i,j - x_k,j| < y_j) for all i,j,k
bool xConvergence(const XVECTOR * i_lpVectors, unsigned int i_uiNum_Points,const XVECTOR &i_cThreshold, XVECTOR * o_lpcConvergence_Fault)
{
	XVECTOR cTest;
	bool bRet = true;
	for(unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
		bRet &= (i_cThreshold.Get_Size() == i_lpVectors[uiI].Get_Size());
	if (!bRet)
		fprintf(stderr,"xConvergence: Convergence threshold size not the same as paramter vector size!\n");

	for(unsigned int uiI = 0; uiI < i_uiNum_Points && bRet; uiI++)
	{
		for (unsigned int uiJ = uiI + 1; uiJ < i_uiNum_Points && bRet; uiJ++)
		{
			cTest = i_lpVectors[uiJ] - i_lpVectors[uiI];
			for (unsigned int uiK = 0; uiK < cTest.Get_Size() && bRet; uiK++)
			{
				bRet = (fabs(cTest.Get(uiK)) < i_cThreshold.Get(uiK));
//				if (!bRet)
//					printf("%i %.2e %.2e\n",uiK,cTest.Get(uiK),i_cThreshold.Get(uiK));
				if (!bRet && o_lpcConvergence_Fault)
				{
					o_lpcConvergence_Fault[0] = cTest;
				}
			}
		}
	}
//	printf("Conv: %c\n",bRet ? 'y' : 'n');
	return bRet;
}

// is a given point within a cuboid.  i_lpcBounds must be a 2 element array, with the 
bool xBoundTest(const XVECTOR & i_cTest_Point, const XVECTOR * i_lpcBounds)
{
	bool bRet;
	bRet = (i_cTest_Point.Get_Size() == i_lpcBounds[0].Get_Size() && i_cTest_Point.Get_Size() == i_lpcBounds[1].Get_Size());
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds size not the same as test point size! %i %i %i\n",i_cTest_Point.Get_Size(),i_lpcBounds[0].Get_Size(), i_lpcBounds[1].Get_Size());
	XVECTOR cTest = i_cTest_Point - i_lpcBounds[0];
	for (unsigned int uiI = 0; uiI < i_lpcBounds[0].Get_Size() && bRet; uiI++)
	{
		bRet = i_lpcBounds[0].Get(uiI) <= i_lpcBounds[0].Get(uiI);
	}
	if (!bRet)
		fprintf(stderr,"xBoundTest: Bounds invalid.  Must be a two element array, with index 0 as the lower bound and index 1 as upper bound.\n");
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	cTest = i_lpcBounds[1] - i_cTest_Point;
	for (unsigned int uiI = 0; uiI < i_cTest_Point.Get_Size() && bRet; uiI++)
	{
		bRet &= (cTest.Get(uiI) >= 0.0);
	}
	return bRet;
}

XVECTOR xCompute_Centroid(XVECTOR * i_lpvPoints, unsigned int i_uiNum_Points)
{
	XVECTOR cCentroid(i_lpvPoints[0].Get_Size());
	for (unsigned int uiI = 0; uiI < cCentroid.Get_Size(); uiI++)
		cCentroid.Set(uiI,0.0);

	for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
	{
		cCentroid += i_lpvPoints[uiI];
	}
	cCentroid /= (double)(i_uiNum_Points);
	return cCentroid;
}
