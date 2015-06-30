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




char g_lpszVector_Data_Filename[256] = {""};
XDATASET	g_cFit_Data_Vectors;
XDATASET	g_cFit_Data_Output;
bool Read_Vector_Output_Cache(const XVECTOR & i_vX, SPECTRA_FIT_DATA_BASE * lpcCall_Data, ES::Spectrum &cOutput)
{
	bool bRet = false;
	char lpszVectorlist_Filename[256];
	char lpszOutput_Filename[256];
	sprintf(lpszVectorlist_Filename,".spectrafit.0.2.%s.%i.vectorlist",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	sprintf(lpszOutput_Filename,".spectrafit.0.2.%s.%i.outputdata",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
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
	sprintf(lpszVectorlist_Filename,".spectrafit.0.2.%s.%i.vectorlist",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
	sprintf(lpszOutput_Filename,".spectrafit.0.2.%s.%i.outputdata",lpcCall_Data->lpszModel_Name,lpcCall_Data->uiIon);
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

void Bounds(void)
{
	XVECTOR cBounds[2] = {XVECTOR(7),XVECTOR(7)};
	XVECTOR cTest(7);

	cBounds[0].Set(0,0.0); // time
	cBounds[0].Set(1,5.0); // PS velocity (kkm/s)
	cBounds[0].Set(2,5.0); // PS temp (kK)
	cBounds[0].Set(3,1.0); // PS ion temp (kK)
	cBounds[0].Set(4,-4.0); // PS ion log opacity scaling
	cBounds[0].Set(5,1.0); // HVF ion temp (kK)
	cBounds[0].Set(6,-4.0); // HVF ion log opacity scaling

	cBounds[1].Set(0,1.0); // time
	cBounds[1].Set(1,40.0); // PS velocity (kkm/s)
	cBounds[1].Set(2,30.0); // PS temp (kK)
	cBounds[1].Set(3,30.0); // PS ion temp (kK)
	cBounds[1].Set(4,5.0); // PS ion log opacity scaling
	cBounds[1].Set(5,30.0); // HVF ion temp (kK)
	cBounds[1].Set(6,3.0); // HVF ion log opacity scaling

	// permute the cache
	unsigned int uiNum_Parameters = cBounds[0].Get_Size();
	unsigned int uiNum_Vertices = 1 << uiNum_Parameters;
	for (unsigned int uiI = 0; uiI < uiNum_Vertices; uiI++)
	{
		cTest = cBounds[0];
		for (unsigned int uiJ = 0; uiJ < uiNum_Parameters; uiJ++)
		{
			if (uiI & (1 << uiJ)) 
				cTest.Set(uiJ,cBounds[1].Get(uiJ));
		}

