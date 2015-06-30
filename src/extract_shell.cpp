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

bool Is_Variable_To_Skip(const char i_lpszVariables_To_Skip[][5], unsigned int i_uiNum_Variables_To_Skip, const char * i_lpszVariable)
{
	bool bRet = false;
	unsigned int uiI;
	for (uiI = 0; uiI < i_uiNum_Variables_To_Skip && !bRet; uiI++)
	{
		bRet = (strcmp(i_lpszVariables_To_Skip[uiI],i_lpszVariable) == 0);
	}
	return bRet;
}

void Compute_Face_V(XDATASET & i_cDataset, unsigned int i_uiVelocity_Idx, unsigned int i_uiV_Lower_Idx, unsigned int i_uiV_Upper_Idx, unsigned int i_uiDDX_Idx = 1)
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
		i_cDataset.SetElement(i_uiV_Upper_Idx,uiJ,dVupper);
		dVlower = dVupper;
	}
	double	dDelta_V = i_cDataset.GetElement(i_uiV_Upper_Idx,0) - i_cDataset.GetElement(i_uiVelocity_Idx,0);
	dVlower = i_cDataset.GetElement(i_uiVelocity_Idx,0) - dDelta_V;
	if (dVlower > 0.0)
		i_cDataset.SetElement(i_uiV_Lower_Idx,0,dVlower);
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
	XDATASET cDataEjecta, cDataShell;
	std::stringstream cHeader_String;

	unsigned int uiI, uiJ, uiK, uiL;
	unsigned int uiHe3_Idx, uiHyd_Idx, uiVel_Idx;

	const char lpszVariables_To_Skip[][5] = 
	{
		"eint",
		"ener",
		"gamc",
		"game",
		"gpol",
		"gpot",
		"pres",
		"temp",
		"vely",
		"velz"
	};
	unsigned int uiNum_Variables_To_Skip = sizeof(lpszVariables_To_Skip) / 5;
	unsigned int uiNum_Variables_To_Use = 0;

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
			sprintf(lpszEjecta_Filename,"ejecta%04i.csv",uiFile_ID);
			sprintf(lpszShell_Filename,"shell%04i.csv",uiFile_ID);
			sprintf(lpszEjecta_Bin_Filename,"ejecta%04i.xdataset",uiFile_ID);
			sprintf(lpszShell_Bin_Filename,"shell%04i.xdataset",uiFile_ID);
			cHeader_String.str("");
			cHeader_String.clear();
			cHeader_String << "Radius [cm]";
			cHeader_String << ", Delta Radius [cm]";

			uiNum_Variables_To_Use = 0;
			uiVel_Idx = 0;
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Vars; uiJ++)
			{
				if (!Is_Variable_To_Skip(lpszVariables_To_Skip, uiNum_Variables_To_Skip, cFlash_File.m_lpszVar_Names[uiJ]))
				{
					uiNum_Variables_To_Use++;
					if (strcmp(cFlash_File.m_lpszVar_Names[uiJ],"dens") == 0)
						cHeader_String << ", Density [gm/cm^3]";
					else if (strcmp(cFlash_File.m_lpszVar_Names[uiJ],"velx") == 0)
					{
						cHeader_String << ", Velocity [cm/s]";
						uiVel_Idx = uiNum_Variables_To_Use + 1;
					}
					else
						cHeader_String << ", " << cFlash_File.m_lpszVar_Names[uiJ];
				}
			}
			cHeader_String << ", Inner Face Velocity [cm/s]";
			cHeader_String << ", Outer Face Velocity [cm/s]";
			cHeader_String << std::endl;
			uiHe3_Idx = Find_Variable_Idx(cFlash_File,"he3");
			uiHyd_Idx = Find_Variable_Idx(cFlash_File,"hyd");
			unsigned int uiShell_Rows = 0;
			unsigned int uiEjecta_Rows = 0;
			// first count how many rows we will need
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
					for (uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
					{
						if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
							uiEjecta_Rows++;
						else if (lpcHe3Block->m_lpdData[uiL] > 1e-4) // in shell
							uiShell_Rows++;
					}
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
				}
			}
			if (uiEjecta_Rows > 0)
				cDataEjecta.Allocate(uiNum_Variables_To_Use + 4,uiEjecta_Rows);
			if (uiShell_Rows > 0)
				cDataShell.Allocate(uiNum_Variables_To_Use + 4,uiShell_Rows);

			unsigned int uiRow_Idx_Shell = 0;
			unsigned int uiRow_Idx_Ejecta = 0;
			for (uiJ = 0; uiJ < cFlash_File.m_uiNum_Blocks; uiJ++)
			{
				if (cFlash_File.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiJ;
					double ddx;
					// compute the cell 
					ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
					ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];
					// Copy variable data
					unsigned int uiColumn_Idx = 1;
					lpcHydBlock = cFlash_File.GetBlock(uiHyd_Idx,uiJ);
					lpcHe3Block = cFlash_File.GetBlock(uiHe3_Idx,uiJ);
					unsigned int uiShell_Row_Offset = 0;
					unsigned int uiEjecta_Row_Offset = 0;
					uiColumn_Idx = 2;
					for (uiK = 0; uiK < cFlash_File.m_uiNum_Vars; uiK++)
					{
						if (!Is_Variable_To_Skip(lpszVariables_To_Skip, uiNum_Variables_To_Skip, cFlash_File.m_lpszVar_Names[uiK]))
						{
							lpcBlock = cFlash_File.GetBlock(uiK,uiJ);
							uiShell_Row_Offset = 0;
							uiEjecta_Row_Offset = 0;
							for (uiL = 0; uiL < cFlash_File.m_uiBlock_Dimensions[0]; uiL++)
							{
								double dX = ddx * (uiL + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0];
								if (lpcHydBlock->m_lpdData[uiL] < 0.01) // in ejecta
								{
									if (uiColumn_Idx == 2)
									{
										cDataEjecta.SetElement(0,uiEjecta_Row_Offset + uiRow_Idx_Ejecta,dX);
										cDataEjecta.SetElement(1,uiEjecta_Row_Offset + uiRow_Idx_Ejecta,ddx);
									}
									cDataEjecta.SetElement(uiColumn_Idx,uiEjecta_Row_Offset + uiRow_Idx_Ejecta,lpcBlock->m_lpdData[uiL]);
									uiEjecta_Row_Offset++;
								}
								else if (lpcHe3Block->m_lpdData[uiL] > 1e-4) // in shell
								{
									if (uiColumn_Idx == 2)
									{
										cDataShell.SetElement(0,uiShell_Row_Offset + uiRow_Idx_Shell,dX);
										cDataShell.SetElement(1,uiShell_Row_Offset + uiRow_Idx_Shell,ddx);
									}
									cDataShell.SetElement(uiColumn_Idx,uiShell_Row_Offset + uiRow_Idx_Shell,lpcBlock->m_lpdData[uiL]);
									uiShell_Row_Offset++;
								}
							}
							uiColumn_Idx++;
							cFlash_File.ReleaseBlock(lpcBlock);
						}
					}
					uiRow_Idx_Shell += uiShell_Row_Offset;
					uiRow_Idx_Ejecta += uiEjecta_Row_Offset;
					cFlash_File.ReleaseBlock(lpcHydBlock);
					cFlash_File.ReleaseBlock(lpcHe3Block);
				} // end if leaf node
			} // loop through blocks
			cFlash_File.Close();
			// compute the 
			Compute_Face_V(cDataShell,uiVel_Idx,uiNum_Variables_To_Use + 2,uiNum_Variables_To_Use + 3);
			Compute_Face_V(cDataEjecta,uiVel_Idx,uiNum_Variables_To_Use + 2,uiNum_Variables_To_Use + 3);
			if (cDataShell.GetNumElements() > 0)
			{
				cDataShell.SetHeaderLine((cHeader_String.str()).c_str());
				cDataShell.SaveDataFileCSV(lpszShell_Filename,NULL);
				cDataShell.SaveDataFileBin(lpszShell_Bin_Filename);
				cDataShell.Deallocate();
			}
			if (cDataEjecta.GetNumElements() > 0)
			{
				cDataEjecta.SetHeaderLine((cHeader_String.str()).c_str());
				cDataEjecta.SaveDataFileCSV(lpszEjecta_Filename,NULL);
				cDataEjecta.SaveDataFileBin(lpszEjecta_Bin_Filename);
				cDataEjecta.Deallocate();
			}
		} // loop through args
	} // if args >= 2
	return 0;
}

