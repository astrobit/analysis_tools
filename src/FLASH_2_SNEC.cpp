#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <xflash.h>
#include <xio.h>
#include <iostream>
#include <sstream>

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


class INDICES
{
public:
	unsigned int uiRadius_Idx;
	unsigned int uiDelta_Radius_Idx;
	unsigned int uiDensity_Idx;
	unsigned int uiTemp_Idx;
	unsigned int uiVelocity_Idx;
	unsigned int uiInner_Face_Velocity_Idx;
	unsigned int uiHyd_Idx;
	unsigned int uiHe3_Idx;
	unsigned int uiHe4_Idx;
	unsigned int uiC12_Idx;
	unsigned int uiN14_Idx;
	unsigned int uiO16_Idx;
	unsigned int uiNe20_Idx;
	unsigned int uiMg24_Idx;
	unsigned int uiSi28_Idx;
	unsigned int uiS32_Idx;
	unsigned int uiAr36_Idx;
	unsigned int uiCa40_Idx;
	unsigned int uiTi44_Idx;
	unsigned int uiTi48_Idx;
	unsigned int uiV48_Idx;
	unsigned int uiCr48_Idx;
	unsigned int uiCr52_Idx;
	unsigned int uiMn52_Idx;
	unsigned int uiFe52_Idx;
	unsigned int uiFe56_Idx;
	unsigned int uiCo56_Idx;
	unsigned int uiNi56_Idx;
};
void Compute_Face_V(XDATASET & i_cDataset, unsigned int i_uiVelocity_Idx, unsigned int i_uiV_Lower_Idx, unsigned int i_uiDDX_Idx = 1)
{
	// compute the 
	double	dVupper;	
	double	dVlower = 0.0;
	for (unsigned int uiJ = 0; uiJ < i_cDataset.GetNumElements(); uiJ++)
	{
//		double dVlower;
//		double dVupper;
//		double	dDelta_V;

		double dVelX_i = i_cDataset.GetElement(i_uiVelocity_Idx,uiJ);
		double ddx_i = i_cDataset.GetElement(i_uiDDX_Idx,uiJ);
//		if (uiJ != 0)
//		{
//			double dVelX_im1 = i_cDataset.GetElement(i_uiVelocity_Idx,uiJ - 1);
//			double ddx_im1 = i_cDataset.GetElement(i_uiDDX_Idx,uiJ - 1);
//			dVlower = (dVelX_im1 - dVelX_i) / (1.0 + ddx_im1 / ddx_i) + dVelX_i;
//		}
//		else
//		{
//			double dVelX_ip1 = i_cDataset.GetElement(i_uiVelocity_Idx,uiJ + 1);
//			double ddx_ip1 = i_cDataset.GetElement(i_uiDDX_Idx,uiJ + 1);
//			dVlower = dVelX_i - (dVelX_ip1 - dVelX_i) / (1.0 + ddx_ip1 / ddx_i);
//		}


		if (uiJ != (i_cDataset.GetNumElements() - 1))
		{
			double dVelX_ip1 = i_cDataset.GetElement(i_uiVelocity_Idx,uiJ + 1);
			double ddx_ip1 = i_cDataset.GetElement(i_uiDDX_Idx,uiJ + 1);
			dVupper = (dVelX_ip1 - dVelX_i) / (1.0 + ddx_ip1 / ddx_i) + dVelX_i;
		}
		else
		{
			double dDelta_V = (dVelX_i - dVupper) * 2.0;
			dVupper += dDelta_V;
		}

//		dDelta_V = dVupper - dVlower;
		i_cDataset.SetElement(i_uiV_Lower_Idx,uiJ,dVlower);
		dVlower = dVupper;
	}
	double	dDelta_V = (i_cDataset.GetElement(i_uiV_Lower_Idx,1) - i_cDataset.GetElement(i_uiVelocity_Idx,0)) * 0.5;
	dVlower = i_cDataset.GetElement(i_uiVelocity_Idx,0) - dDelta_V;
	if (dVlower > 0.0)
		i_cDataset.SetElement(i_uiV_Lower_Idx,0,dVlower);
}


unsigned int Fill_Column(const XFLASH_File &cFlash_File, XDATASET & cData, unsigned int uiVar_Idx, unsigned int uiBlock_ID, unsigned int uiRow_Idx, unsigned int uiColumn_Idx)
{
	XFLASH_Block * lpcBlock = cFlash_File.GetBlock(uiVar_Idx,uiBlock_ID);
//	int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * iBlock_ID;
	unsigned int uiRow_Offset = 0;
	unsigned int uiMax_Elem = cData.GetNumElements();
	for (unsigned int uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
	{
		if ((uiL + uiRow_Idx) < uiMax_Elem)
		{
			cData.SetElement(uiColumn_Idx,uiRow_Offset + uiRow_Idx,lpcBlock->m_lpdData[uiL]);
			uiRow_Offset++;
		}
	}
	cFlash_File.ReleaseBlock(lpcBlock);
	return uiRow_Offset;
}
unsigned int Fill_Radius(const XFLASH_File &cFlash_File, XDATASET & cData, int iBlock_ID, unsigned int uiRow_Idx, unsigned int uiColumn_Idx)
{
	unsigned int uiRow_Offset = 0;
	int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * iBlock_ID;
	double ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
	ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];
	unsigned int uiMax_Elem = cData.GetNumElements();
	for (unsigned int uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
	{
		if ((uiL + uiRow_Idx) < uiMax_Elem)
		{
			double dX = ddx * (uiL + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0];
			cData.SetElement(uiColumn_Idx,uiL + uiRow_Idx,dX);
			cData.SetElement(uiColumn_Idx+ 1,uiL + uiRow_Idx,ddx);
			uiRow_Offset++;
		}
	}
	return uiRow_Offset;
}


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[32];
	char	lpszProfile_Filename[64];
	char	lpszIon_Filename[64];
	char	lpszGrid_Filename[64];
	XFLASH_File	cFlash_File;
//	FILE * fileOut = fopen("photosphere.csv","wt");
	XDATASET cData;

	unsigned int uiI, uiJ, uiK, uiL;
	INDICES	cFlash_Idx;
	INDICES	cData_Idx;

	double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
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
			sprintf(lpszProfile_Filename,"%04i.short",uiFile_ID);
			sprintf(lpszIon_Filename,"%04i.iso.dat",uiFile_ID);
			sprintf(lpszGrid_Filename,"%04i.GridPattern.dat",uiFile_ID);

			cFlash_Idx.uiDensity_Idx = Find_Variable_Idx(cFlash_File,"dens");
			cFlash_Idx.uiTemp_Idx = Find_Variable_Idx(cFlash_File,"temp");
			cFlash_Idx.uiVelocity_Idx = Find_Variable_Idx(cFlash_File,"velx");
			cFlash_Idx.uiHyd_Idx = Find_Variable_Idx(cFlash_File,"hyd");
			cFlash_Idx.uiHe3_Idx = Find_Variable_Idx(cFlash_File,"he3");
			cFlash_Idx.uiHe4_Idx = Find_Variable_Idx(cFlash_File,"he4");
			cFlash_Idx.uiC12_Idx = Find_Variable_Idx(cFlash_File,"c12");
			cFlash_Idx.uiN14_Idx = Find_Variable_Idx(cFlash_File,"n14");
			cFlash_Idx.uiO16_Idx = Find_Variable_Idx(cFlash_File,"o16");
			cFlash_Idx.uiNe20_Idx = Find_Variable_Idx(cFlash_File,"ne20");
			cFlash_Idx.uiMg24_Idx = Find_Variable_Idx(cFlash_File,"mg24");
			cFlash_Idx.uiSi28_Idx = Find_Variable_Idx(cFlash_File,"si28");
			cFlash_Idx.uiS32_Idx = Find_Variable_Idx(cFlash_File,"s32");
			cFlash_Idx.uiAr36_Idx = Find_Variable_Idx(cFlash_File,"ar36");
			cFlash_Idx.uiCa40_Idx = Find_Variable_Idx(cFlash_File,"ca40");
			cFlash_Idx.uiTi44_Idx = Find_Variable_Idx(cFlash_File,"ti44");
			cFlash_Idx.uiTi48_Idx = Find_Variable_Idx(cFlash_File,"ti48");
			cFlash_Idx.uiV48_Idx = Find_Variable_Idx(cFlash_File,"v48");
			cFlash_Idx.uiCr48_Idx = Find_Variable_Idx(cFlash_File,"cr48");
			cFlash_Idx.uiCr52_Idx = Find_Variable_Idx(cFlash_File,"cr52");
			cFlash_Idx.uiMn52_Idx = Find_Variable_Idx(cFlash_File,"mn52");
			cFlash_Idx.uiFe52_Idx = Find_Variable_Idx(cFlash_File,"fe52");
			cFlash_Idx.uiFe56_Idx = Find_Variable_Idx(cFlash_File,"fe56");
			cFlash_Idx.uiCo56_Idx = Find_Variable_Idx(cFlash_File,"co56");
			cFlash_Idx.uiNi56_Idx = Find_Variable_Idx(cFlash_File,"ni56");


			unsigned int uiNon_CSM_Rows = 0;
			bool bThrough_Shell = false;
			// first count how many rows we will need
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					XFLASH_Block * lpcHydBlock = cFlash_File.GetBlock(cFlash_Idx.uiHyd_Idx,uiJ);
					XFLASH_Block * lpcHe3Block = cFlash_File.GetBlock(cFlash_Idx.uiHe3_Idx,uiJ);
					for (uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
					{
						bThrough_Shell |= (lpcHydBlock->m_lpdData[uiL] > 0.01 && lpcHe3Block->m_lpdData[uiL] > 1e-4); // in shell
						if (!bThrough_Shell || (lpcHe3Block->m_lpdData[uiL] > 1e-4))
							uiNon_CSM_Rows++;
					}
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
				}
			}
			if (!bThrough_Shell) // no shell in this model; recount without CSM zones
			{
				uiNon_CSM_Rows = 0;
				for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
				{
					if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
					{
						XFLASH_Block * lpcHydBlock = cFlash_File.GetBlock(cFlash_Idx.uiHyd_Idx,uiJ);
						XFLASH_Block * lpcHe3Block = cFlash_File.GetBlock(cFlash_Idx.uiHe3_Idx,uiJ);
						for (uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
						{
							if (lpcHydBlock->m_lpdData[uiL] < 0.01)
								uiNon_CSM_Rows++;
						}
						cFlash_File.ReleaseBlock(lpcHydBlock);
						cFlash_File.ReleaseBlock(lpcHe3Block);
					}
				}
			}
			cData.Allocate(28,uiNon_CSM_Rows);

			unsigned int uiRow_Idx = 0;
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					unsigned int uiColumn_Idx = 0;
					cData_Idx.uiRadius_Idx = uiColumn_Idx;
					cData_Idx.uiDelta_Radius_Idx = uiColumn_Idx + 1;
					Fill_Radius(cFlash_File, cData, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx+=2; // 2 columns for radius and delta radius
					cData_Idx.uiDensity_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiDensity_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiVelocity_Idx = uiColumn_Idx;
//					cData_Idx.uiInner_Face_Velocity_Idx = uiColumn_Idx + 1;
					Fill_Column(cFlash_File, cData, cFlash_Idx.uiVelocity_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++; //uiColumn_Idx+=2; // save space for 
					cData_Idx.uiTemp_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiTemp_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiHyd_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiHyd_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiHe3_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiHe3_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiHe4_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiHe4_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiC12_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiC12_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiN14_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiN14_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiO16_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiO16_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiNe20_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiNe20_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiMg24_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiMg24_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiSi28_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiSi28_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiS32_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiS32_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiAr36_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiAr36_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiCa40_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiCa40_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiTi44_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiTi44_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiTi48_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiTi48_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiV48_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiV48_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiCr48_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiCr48_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiCr52_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiCr52_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiMn52_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiMn52_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiFe52_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiFe52_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiFe56_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiFe56_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiCo56_Idx = uiColumn_Idx; Fill_Column(cFlash_File, cData, cFlash_Idx.uiCo56_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;
					cData_Idx.uiNi56_Idx = uiColumn_Idx; uiRow_Idx += Fill_Column(cFlash_File, cData, cFlash_Idx.uiNi56_Idx, uiJ, uiRow_Idx, uiColumn_Idx); uiColumn_Idx++;

					} // end if leaf node
			} // loop through blocks
			cFlash_File.Close();
			// compute the inner face velocity for each cell
//			Compute_Face_V(cData,uiData_Velocity_Idx,uiData_Velocity_Idx + 1);

			FILE * fileProfile = fopen(lpszProfile_Filename,"wt");
			FILE * fileIons = fopen(lpszIon_Filename,"wt");
			FILE * fileGrid = fopen(lpszGrid_Filename,"wt");
			double	dMass = 0.0;
			double	dVel_Max = 0.0;
			double	dR_Vel_Max = 0.0;
			for (uiJ = 0; uiJ < uiNon_CSM_Rows; uiJ++)
			{
				double	dR = cData.GetElement(0,uiJ);
				double	dDelta_R = cData.GetElement(1,uiJ);
				double	dRinner = dR - 0.5 * dDelta_R;
				double	dRouter = dR + 0.5 * dDelta_R;
				double	dVolume_Router = dVol_Const * (pow(dRouter,3.0) - pow(dR,3.0));
				double	dVolume_R = dVol_Const * (pow(dR,3.0) - pow(dRinner,3.0));
				double	dDensity = cData.GetElement(2,uiJ);
//				double	dZone_Mass = dDensity * dVolume;

				dMass += dDensity * dVolume_R;
				if (uiJ != (uiNon_CSM_Rows - 1))
					dMass += dDensity * dVolume_Router;

				double	dVelocity = cData.GetElement(cData_Idx.uiVelocity_Idx,uiJ);
				if (dVel_Max < dVelocity)
				{
					dVel_Max = dVelocity;
					dR_Vel_Max = cData.GetElement(cData_Idx.uiRadius_Idx,uiJ);
				}
				
			}

			double	dVelocity_Scalar = dVel_Max / dR_Vel_Max;

			fprintf(fileProfile,"%i\n",uiNon_CSM_Rows);
//			fprintf(fileIons,"%i 11\n",uiNon_CSM_Rows);
			fprintf(fileIons,"%i 22\n",uiNon_CSM_Rows);
//			fprintf(fileIons,"1.0d0 4.0d0 12.0d0 16.0d0 24.0d0 28.0d0 32.0d0 40.0d0 52.0d0 56.0d0 56.0d0\n"); // atomic weights
//			fprintf(fileIons,"1.0d0 2.0d0 6.0d0 8.0d0 12.0d0 14.0d0 16.0d0 20.0d0 26.0d0 27.0d0 28.0d0\n"); // atomic numbers
			fprintf(fileIons,"1.0d0 3.0d0 4.0d0 12.0d0 14.0d0 16.0d0 20.0d0 24.0d0 28.0d0 32.0d0 36.0d0 40.0d0 44.0d0 48.0d0 48.0d0 48.0d0 52.0d0 52.0d0 52.0d0 56.0d0 56.0d0 56.0d0\n"); // atomic weights
			fprintf(fileIons,"1.0d0 2.0d0 2.0d0 6.0d0 7.0d0 8.0d0 10.0d0 12.0d0 14.0d0 16.0d0 18.0d0 20.0d0 22.0d0 22.0d0 23.0d0 24.0d0 24.0d0 25.0d0 26.0d0 26.0d0 27.0d0 28.0d0\n"); // atomic numbers
			double	dMass_Coordinate = 0.0;
			for (uiJ = 0; uiJ < uiNon_CSM_Rows; uiJ++)
			{
				double	dR = cData.GetElement(0,uiJ);
				double	dDelta_R = cData.GetElement(1,uiJ);
				double	dRinner = dR - 0.5 * dDelta_R;
				double	dRouter = dR + 0.5 * dDelta_R;
				double	dVolume_Router = dVol_Const * (pow(dRouter,3.0) - pow(dR,3.0));
				double	dVolume_R = dVol_Const * (pow(dR,3.0) - pow(dRinner,3.0));
				double	dDensity = cData.GetElement(2,uiJ);
//				double	dZone_Mass = dDensity * dVolume;

				dMass_Coordinate += dDensity * dVolume_R;
				fprintf(fileGrid,"%.17f\n",dMass_Coordinate / dMass);

				double	dVelocity = cData.GetElement(cData_Idx.uiVelocity_Idx,uiJ);
				if (false)
					dVelocity = dR * dVelocity_Scalar; // make velocity monotonic increasing with r

				double	dMg_Group = cData.GetElement(cData_Idx.uiN14_Idx,uiJ) + cData.GetElement(cData_Idx.uiNe20_Idx,uiJ) + cData.GetElement(cData_Idx.uiMg24_Idx,uiJ);
				double	dFe_Group = cData.GetElement(cData_Idx.uiTi44_Idx,uiJ) + cData.GetElement(cData_Idx.uiTi48_Idx,uiJ) + cData.GetElement(cData_Idx.uiV48_Idx,uiJ) + cData.GetElement(cData_Idx.uiCr48_Idx,uiJ) + cData.GetElement(cData_Idx.uiCr52_Idx,uiJ) + cData.GetElement(cData_Idx.uiFe52_Idx,uiJ) + cData.GetElement(cData_Idx.uiFe56_Idx,uiJ) + cData.GetElement(cData_Idx.uiCo56_Idx,uiJ);
				double	dCa_Group = cData.GetElement(cData_Idx.uiAr36_Idx,uiJ) + cData.GetElement(cData_Idx.uiCa40_Idx,uiJ);
				double	dOmega = 0.0; // Rotation?
				double	dYe = 0.4; // near full ioniazation; not really true, but close enough - could extract this if we really wanted to...
				// index mass radius temp density velocity Ye omega
				fprintf(fileProfile,"%i %.17e %.17e %.17e %.17e %.17e %.17e %.17e\n",uiJ,dMass_Coordinate,dR,cData.GetElement(cData_Idx.uiTemp_Idx,uiJ), cData.GetElement(cData_Idx.uiDensity_Idx,uiJ), cData.GetElement(cData_Idx.uiVelocity_Idx,uiJ), dYe, dOmega);

				fprintf(fileIons,"%.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e\n",dMass_Coordinate,dR,cData.GetElement(cData_Idx.uiHyd_Idx,uiJ),cData.GetElement(cData_Idx.uiHe3_Idx,uiJ),cData.GetElement(cData_Idx.uiHe4_Idx,uiJ),cData.GetElement(cData_Idx.uiC12_Idx,uiJ),cData.GetElement(cData_Idx.uiN14_Idx,uiJ),cData.GetElement(cData_Idx.uiO16_Idx,uiJ),cData.GetElement(cData_Idx.uiNe20_Idx,uiJ),cData.GetElement(cData_Idx.uiMg24_Idx,uiJ),cData.GetElement(cData_Idx.uiSi28_Idx,uiJ),cData.GetElement(cData_Idx.uiS32_Idx,uiJ),cData.GetElement(cData_Idx.uiAr36_Idx,uiJ),cData.GetElement(cData_Idx.uiCa40_Idx,uiJ),cData.GetElement(cData_Idx.uiTi44_Idx,uiJ),cData.GetElement(cData_Idx.uiTi48_Idx,uiJ),cData.GetElement(cData_Idx.uiV48_Idx,uiJ),cData.GetElement(cData_Idx.uiCr48_Idx,uiJ),cData.GetElement(cData_Idx.uiCr52_Idx,uiJ),cData.GetElement(cData_Idx.uiMn52_Idx,uiJ),cData.GetElement(cData_Idx.uiFe52_Idx,uiJ),cData.GetElement(cData_Idx.uiFe56_Idx,uiJ),cData.GetElement(cData_Idx.uiCo56_Idx,uiJ),cData.GetElement(cData_Idx.uiNi56_Idx,uiJ));
//				fprintf(fileIons,"%.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e %.17e\n",dMass_Coordinate,dR,cData.GetElement(cData_Idx.uiHyd_Idx,uiJ),cData.GetElement(cData_Idx.uiHe3_Idx,uiJ) + cData.GetElement(cData_Idx.uiHe4_Idx,uiJ),cData.GetElement(cData_Idx.uiC12_Idx,uiJ),cData.GetElement(cData_Idx.uiO16_Idx,uiJ),dMg_Group,cData.GetElement(cData_Idx.uiSi28_Idx,uiJ),cData.GetElement(cData_Idx.uiS32_Idx,uiJ),dCa_Group,dFe_Group,cData.GetElement(cData_Idx.uiCo56_Idx,uiJ),cData.GetElement(cData_Idx.uiNi56_Idx,uiJ));


				dMass_Coordinate += dDensity * dVolume_Router;
			}
			fprintf(fileGrid,"\n");


			cData.Deallocate();
		} // loop through args
	} // if args >= 2
	return 0;
}

