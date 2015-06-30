#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xflash.h>
#include <xio.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <Plot_Utilities.h>

unsigned int  Find_Variable_Idx(const XFLASH_File & cFile, const char * i_lpszVar_Name)
{
	unsigned int uiVariable_Idx = 0;
	unsigned int uiLen = strlen(i_lpszVar_Name);

	while (uiVariable_Idx < cFile.m_uiNum_Vars && (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) != 0 || (strncmp(cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name,uiLen) == 0 && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != ' ' && cFile.m_lpszVar_Names[uiVariable_Idx][uiLen] != 0)))
	{
//		printf("%s:%s\n",cFile.m_lpszVar_Names[uiVariable_Idx],i_lpszVar_Name);
		uiVariable_Idx++;
	}
	
	return uiVariable_Idx;
}
class BOUNDS
{
public:
	unsigned int uiNum_Bounds;
	double	* lpdMin;
	double  * lpdMax;

private:
	void Delete(void)
	{
		uiNum_Bounds = 0;
		if (lpdMin)
			delete [] lpdMin;
		if (lpdMax)
			delete [] lpdMax;
		lpdMin = lpdMax = NULL;
	}
	void Copy(const BOUNDS & cRHO)
	{
		if (cRHO.uiNum_Bounds != uiNum_Bounds)
			Delete();
		if (cRHO.uiNum_Bounds > 0)
		{
			if (cRHO.uiNum_Bounds != uiNum_Bounds)
			{
				lpdMin = new double [cRHO.uiNum_Bounds];
				lpdMax = new double [cRHO.uiNum_Bounds];
				uiNum_Bounds = cRHO.uiNum_Bounds;
			}
			memcpy(lpdMin,cRHO.lpdMin,sizeof(double) * cRHO.uiNum_Bounds);
			memcpy(lpdMax,cRHO.lpdMax,sizeof(double) * cRHO.uiNum_Bounds);
		}
	}
public:
	BOUNDS(void)
	{
		uiNum_Bounds = 0;
		lpdMin = lpdMax = NULL;
	}
	BOUNDS(const BOUNDS & cRHO)
	{
		uiNum_Bounds = 0;
		lpdMin = lpdMax = NULL;
		Copy(cRHO);
	}
	BOUNDS & operator = (const BOUNDS & cRHO)
	{
		Copy(cRHO);
		return *this;
	}
	~BOUNDS(void)
	{
		Delete();
	}

	void DetermineBounds(const XDATASET &cDataset)
	{
		Delete(); // clear existing bounds
		uiNum_Bounds = cDataset.GetNumColumns();
		lpdMin = new double[uiNum_Bounds];
		lpdMax = new double[uiNum_Bounds];
		for (unsigned int uiI = 0; uiI < uiNum_Bounds; uiI++)
		{
			lpdMin[uiI] = DBL_MAX;
			lpdMax[uiI] = DBL_MIN;
		}
		for (unsigned int uiI = 0; uiI < uiNum_Bounds; uiI++)
			for (unsigned int uiJ = 0; uiJ < cDataset.GetNumElements(); uiJ++)
			{
				double dVal = cDataset.GetElement(uiI,uiJ);
				if (dVal < lpdMin[uiI])
					lpdMin[uiI] = dVal;
				if (dVal > lpdMax[uiI])
					lpdMax[uiI] = dVal;
			}
	}
				
};

enum OPERATION	{op_none,op_log,op_mult};

void Process_Block(const XFLASH_File & cFlash_File, unsigned int uiBlock_ID, unsigned int uiVar_Idx, const XFLASH_Block * lpcHydBlock, const XFLASH_Block * lpcHe3Block, XDATASET & cDataEjecta, XDATASET &cDataShell, XDATASET & cDataCombined, unsigned int uiColumn_Idx, unsigned int uiRow_Idx_Ejecta, unsigned int uiRow_Idx_Shell, unsigned int uiRow_Idx_Combined, unsigned int &uiShell_Row_Offset, unsigned int &uiEjecta_Row_Offset, unsigned int &uiCombined_Row_Offset, OPERATION eOper, const double & dScalar = 1.0)
{
	XFLASH_Block * lpcBlock;
	uiShell_Row_Offset = 0;
	uiEjecta_Row_Offset = 0;
	uiCombined_Row_Offset = 0;
	bool bIn_Shell = false;
	bool bEntered_Shell = false;
	lpcBlock = cFlash_File.GetBlock(uiVar_Idx,uiBlock_ID);
	double ddx;
	// compute the cell 
	int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiBlock_ID;
	ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
	ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];
	for (unsigned int uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
	{
		double dX = ddx * (uiL + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0];
		bIn_Shell = lpcHe3Block->m_lpdData[uiL] > 1e-4;
		double	dData;
		switch (eOper)
		{
		case op_none:
			dData = lpcBlock->m_lpdData[uiL];
			break;
		case op_log:
			dData = log10(lpcBlock->m_lpdData[uiL]);
			break;
		case op_mult:
			dData = lpcBlock->m_lpdData[uiL] * dScalar;
			break;
		}

		if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
		{
			if (uiColumn_Idx == 1)
			{
				cDataEjecta.SetElement(0,uiEjecta_Row_Offset + uiRow_Idx_Ejecta,dX);
			}
			cDataEjecta.SetElement(uiColumn_Idx,uiEjecta_Row_Offset + uiRow_Idx_Ejecta,dData);
			uiEjecta_Row_Offset++;
		}
		else if (bIn_Shell) // in shell
		{
			if (uiColumn_Idx == 1)
			{
				cDataShell.SetElement(0,uiShell_Row_Offset + uiRow_Idx_Shell,dX);
			}
			cDataShell.SetElement(uiColumn_Idx,uiShell_Row_Offset + uiRow_Idx_Shell,dData);
			uiShell_Row_Offset++;
		}

		bEntered_Shell |= bIn_Shell;
		if (!bEntered_Shell || bIn_Shell)
		{
			if (uiColumn_Idx == 1)
			{
				cDataCombined.SetElement(0,uiCombined_Row_Offset + uiRow_Idx_Combined,dX);
			}
			cDataCombined.SetElement(uiColumn_Idx,uiCombined_Row_Offset + uiRow_Idx_Combined,dData);
			uiCombined_Row_Offset++;
		}

	}
	cFlash_File.ReleaseBlock(lpcBlock);
}
void Plot_vs_r(XDATASET & cDataset, const char * lpszFilename)
{
	BOUNDS cBounds;				
	PLINT lpiOptions_Array[2];
	PLINT lpiLegend_Text_Colors[2];
	const char *lplpszLegend_Test[2];
	PLINT lpiLegend_Line_Colors[2];
	PLINT lpiLegend_Line_Styles[2];
	PLFLT lpfLegend_Line_Widths[2];
	PLFLT fLgnd_Width,fLgnd_Height;

	cBounds.DetermineBounds(cDataset);
	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;
	epsplot::LINE_PARAMETERS	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Radius [cm]", false, false, true, cBounds.lpdMin[0], true, cBounds.lpdMax[0]);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Velocity [10 kkm/s], log Density [g/cm#u3#d]", false, false, true, std::min(cBounds.lpdMin[1],cBounds.lpdMin[2]), true, std::max(cBounds.lpdMax[1],cBounds.lpdMax[2]));

	cPlot.Set_Plot_Filename(lpszFilename);

	cLine_Parameters.m_eColor = epsplot::RED;
	cPlot.Set_Plot_Data(cDataset.GetElementArray(0), cDataset.GetElementArray(1), cDataset.GetNumElements(), cLine_Parameters, uiX_Axis, uiY_Axis);
	cLine_Parameters.m_eColor = epsplot::BLUE;
	cPlot.Set_Plot_Data(cDataset.GetElementArray(0), cDataset.GetElementArray(2), cDataset.GetNumElements(), cLine_Parameters, uiX_Axis, uiY_Axis);

	/*
	//@@TODO: Generate Legend
	lpiOptions_Array[0]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[0] = 1;
	lplpszLegend_Test[0]        = "Density";
	lpiLegend_Line_Colors[0] = 1;
	lpiLegend_Line_Styles[0] = 1;
	lpfLegend_Line_Widths[0] = 1.0;
	lpiOptions_Array[1]   = PL_LEGEND_LINE;
	lpiLegend_Text_Colors[1] = 9;
	lplpszLegend_Test[1]        = "Velocity";
	lpiLegend_Line_Colors[1] = 9;
	lpiLegend_Line_Styles[1] = 1;
	lpfLegend_Line_Widths[1] = 1.0;
	lpcPLStream->legend( &fLgnd_Width,&fLgnd_Height,
		PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX, 0,
		0.0, 0.0, 0.1, 
		0, 15, 1, 0, 0,
		2, lpiOptions_Array,
		1.0, 1.0, 2.0,
		0., lpiLegend_Text_Colors, (const char * const *) lplpszLegend_Test,
		NULL, NULL, NULL, NULL,
		lpiLegend_Line_Colors, lpiLegend_Line_Styles, lpfLegend_Line_Widths,
		NULL, NULL, NULL, NULL );
	&*/
//	delete lpcPLStream;
}
void Plot_rho_vs_v(XDATASET & cDataset, const char * lpszFilename)
{
	BOUNDS cBounds;				
	cBounds.DetermineBounds(cDataset);

	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
	epsplot::DATA cPlot;
	epsplot::LINE_PARAMETERS	cLine_Parameters;

	cPlot_Parameters.m_uiNum_Columns = 1;
	cPlot_Parameters.m_uiNum_Rows = 1;
	cPlot_Parameters.m_dWidth_Inches = 11.0;
	cPlot_Parameters.m_dHeight_Inches = 8.5;

	unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( "Velocity [10 kkm/s]", false, false, true, cBounds.lpdMin[2], true, cBounds.lpdMax[2]);
	unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( "Density [g/cm#u3#d]", false, false, true, cBounds.lpdMin[1], true, cBounds.lpdMax[1]);

	cPlot.Set_Plot_Filename(lpszFilename);

	cLine_Parameters.m_eColor = epsplot::BLACK;
	cPlot.Set_Plot_Data(cDataset.GetElementArray(2), cDataset.GetElementArray(1), cDataset.GetNumElements(), cLine_Parameters, uiX_Axis, uiY_Axis);

}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[32];
	char	lpszEjecta_Filename[32];
	char	lpszEjecta_Bin_Filename[32];
	char	lpszShell_Filename[32];
	char	lpszShell_Bin_Filename[32];
	XFLASH_File	cFlash_File;
	XFLASH_Block * lpcBlock;
	XFLASH_Block * lpcHydBlock;
	XFLASH_Block * lpcHe3Block;
//	FILE * fileOut = fopen("photosphere.csv","wt");
	XDATASET cDataEjecta, cDataShell, cDataCombined;

	std::stringstream cHeader_String;

	unsigned int uiI,uiJ,uiK,uiL;
	unsigned int uiHe3_Idx,uiHyd_Idx;
	unsigned int uiVel_Idx;
	unsigned int uiDens_Idx;

	unsigned int uiFile_ID;

	if (i_iArg_Count >= 2)
	{
		for (uiI = 1; uiI < i_iArg_Count; uiI++)
		{
			if (i_lpszArg_Values[uiI][0] >= '0' && i_lpszArg_Values[uiI][0] <= '9')
			{
				uiFile_ID = atoi(i_lpszArg_Values[uiI]);
				sprintf(lpszFilename,"explos_hdf5_chk_%04i",uiFile_ID);
			}
			else
			{
				const char * lpszCursor = i_lpszArg_Values[uiI];
				strcpy(lpszFilename,i_lpszArg_Values[uiI]);
				lpszCursor += (strlen(i_lpszArg_Values[uiI]) - 4);
				uiFile_ID = atoi(lpszCursor);
			}
			cFlash_File.Open(lpszFilename);
			printf("Opened file %s.\n",lpszFilename);
			printf("Time for file is %.1f s\n",cFlash_File.m_dTime);

			uiHe3_Idx = Find_Variable_Idx(cFlash_File,"he3");
			uiHyd_Idx = Find_Variable_Idx(cFlash_File,"hyd");
			uiVel_Idx = Find_Variable_Idx(cFlash_File,"velx");
			uiDens_Idx = Find_Variable_Idx(cFlash_File,"dens");
			unsigned int uiShell_Rows = 0;
			unsigned int uiEjecta_Rows = 0;
			unsigned int uiCombined_Rows = 0;
			bool bIn_Shell = false;
			bool bEntered_Shell = false;
			// first count how many rows we will need
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
					for (uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
					{
						bIn_Shell = (lpcHe3Block->m_lpdData[uiL] > 1e-4); // in shell
						if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
							uiEjecta_Rows++;
						else if (bIn_Shell) // in shell
							uiShell_Rows++;
						bEntered_Shell |= bIn_Shell;
						if (!bEntered_Shell || bIn_Shell)
							uiCombined_Rows++;
					}
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
				}
			}
			if (uiEjecta_Rows > 0)
				cDataEjecta.Allocate(3,uiEjecta_Rows);
			if (uiShell_Rows > 0)
				cDataShell.Allocate(3,uiShell_Rows);
			if (uiCombined_Rows > 0)
				cDataCombined.Allocate(3,uiCombined_Rows);

			unsigned int uiRow_Idx_Shell = 0;
			unsigned int uiRow_Idx_Ejecta = 0;
			unsigned int uiRow_Idx_Combined = 0;
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiJ;
					// Copy variable data
					unsigned int uiColumn_Idx = 1;
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
					unsigned int uiShell_Row_Offset = 0;
					unsigned int uiEjecta_Row_Offset = 0;
					unsigned int uiCombined_Row_Offset = 0;
					uiColumn_Idx = 1;
					lpcBlock = cFlash_File.GetBlock(uiK,uiJ);

					uiShell_Row_Offset = 0;
					uiEjecta_Row_Offset = 0;
					uiCombined_Row_Offset = 0;

					Process_Block(cFlash_File, uiJ, uiDens_Idx, lpcHydBlock, lpcHe3Block, cDataEjecta, cDataShell, cDataCombined, 1, uiRow_Idx_Ejecta, uiRow_Idx_Shell, uiRow_Idx_Combined, uiShell_Row_Offset, uiEjecta_Row_Offset, uiCombined_Row_Offset,op_log,1.0);
					Process_Block(cFlash_File, uiJ, uiVel_Idx, lpcHydBlock, lpcHe3Block, cDataEjecta, cDataShell, cDataCombined, 2, uiRow_Idx_Ejecta, uiRow_Idx_Shell, uiRow_Idx_Combined, uiShell_Row_Offset, uiEjecta_Row_Offset, uiCombined_Row_Offset,op_mult,1.0e-9);

					uiRow_Idx_Shell += uiShell_Row_Offset;
					uiRow_Idx_Ejecta += uiEjecta_Row_Offset;
					uiRow_Idx_Combined += uiCombined_Row_Offset;
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
				} // end if leaf node
			} // loop through blocks
			cFlash_File.Close();
			if (cDataCombined.GetNumElements() > 0)
			{
				sprintf(lpszFilename,"pltvsr_combined_%04i.eps",uiFile_ID);
				Plot_vs_r(cDataCombined, lpszFilename);
				sprintf(lpszFilename,"pltvsv_combined_%04i.eps",uiFile_ID);
				Plot_rho_vs_v(cDataCombined, lpszFilename);
			}
			if (cDataShell.GetNumElements() > 0)
			{
				sprintf(lpszFilename,"pltvsr_shell_%04i.eps",uiFile_ID);
				Plot_vs_r(cDataShell, lpszFilename);
				sprintf(lpszFilename,"pltvsv_shell_%04i.eps",uiFile_ID);
				Plot_rho_vs_v(cDataShell, lpszFilename);
			}
			if (cDataEjecta.GetNumElements() > 0)
			{
				sprintf(lpszFilename,"pltvsr_ejecta_%04i.eps",uiFile_ID);
				Plot_vs_r(cDataEjecta, lpszFilename);
				sprintf(lpszFilename,"pltvsv_ejecta_%04i.eps",uiFile_ID);
				Plot_rho_vs_v(cDataEjecta, lpszFilename);
			}
		} // loop through args
	} // if args >= 2
	return 0;
}

