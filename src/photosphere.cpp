#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
#include <float.h>
#include <xflash.h>
/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  photosphere
//
//  Given a 1-D (spherical) FLASH checkpoint file
//  This computes the location of the photosphere assuming singly through quadruply ionized material
//
//////////////////////////////////////////////////////////////////////////////////////////////////

double GetAbundance(unsigned int i_uiRow, unsigned int i_uiElement_Idx, const XDATASET &cData)
{
	// FLASH seems to lower limit abundances to 10^-10; this forces a small abundance to be 0
	double	dAbd = cData.GetElement(i_uiElement_Idx,i_uiRow);
	if (dAbd < 5.0e-10)
		dAbd = 0.0;
	return dAbd;
}

void Generate_Decay_Products(const double & dTime_Ref, const double & dTime, XDATASET & io_cData, unsigned int i_uiParent_Idx, unsigned int i_uiParentOut_Idx, unsigned int i_uiChild1_Idx, unsigned int i_uiChild2_Idx, const double i_dHalflife[2], const double i_dNuclear_Mass[3])
{
	double	dLog2 = log(2.0);
	double	dTau[2] = {i_dHalflife[0] * dLog2,i_dHalflife[1] * dLog2};
	double	dDeltaT = dTime - dTime_Ref;
	double	dX[2] = {dDeltaT / dTau[0],dDeltaT / dTau[1]};
	unsigned int uiI;

	unsigned int uiNum_Elem = io_cData.GetNumElements();

	for (uiI = 0; uiI < uiNum_Elem; uiI++)
	{
		//double	dR = io_cData.GetElement(0,uiI);
		double	dParent0 = GetAbundance(uiI,i_uiParent_Idx, io_cData) / i_dNuclear_Mass[0];
		double dParent_t = dParent0 * exp(-dX[0]);
		double dChild_1 = (exp(-dX[1] - dX[0]) * (-exp(dX[1]) + exp(dX[0])) * dParent0 * dTau[1])/(dTau[1] - dTau[0]);
		double dChild_2 = dParent0 - dParent_t - dChild_1;

		io_cData.SetElement(i_uiParentOut_Idx,uiI,dParent_t * i_dNuclear_Mass[0]);
		io_cData.SetElement(i_uiChild1_Idx,uiI,dChild_1 * i_dNuclear_Mass[1]);
		io_cData.SetElement(i_uiChild2_Idx,uiI,dChild_2 * i_dNuclear_Mass[2]);
	}
}

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

int main(int iArgC,const char * lpszArgV[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFile_To_Read[256];
	double	dTimeRef;// = 50.0;
	double	dTemp;// = 12000.0;

	unsigned int uiShell_Lower_Idx = 0;
	double	dRef_Velocity;
	unsigned int uiVel_Ref_Idx = 0;

	char lpszFilename[256];

	//printf("%s\n",lpszArgV[1]);
	XDATASET	cData;

	unsigned int uiFlash_Idx[40]; // more than needed
	unsigned int uiCoord_Idx, uiZone_Size_Idx;
	unsigned int uiVel_Idx, uiDens_Idx, uiHyd_Idx, uiHe3_Idx, uiHe4_Idx;
	unsigned int uiC12_Idx, uiN14_Idx, uiO16_Idx, uiNe20_Idx, uiMg24_Idx, uiSi28_Idx;
	unsigned int uiS32_Idx, uiAr36_Idx, uiCa40_Idx, uiCa44_Idx, uiTi44_Idx, uiTi48_Idx;
	unsigned int uiCr48_Idx, uiCr52_Idx, uiFe52_Idx, uiFe56_Idx, uiNi56_Idx;

	unsigned int uiHeComb_Idx, uiCaComb_Idx, uiTiComb_Idx, uiCrComb_Idx, uiFeComb_Idx;

	unsigned int uiCa44Decay_Idx;
	unsigned int uiSc44Decay_Idx;
	unsigned int uiTi44Decay_Idx;
	unsigned int uiTi48Decay_Idx;
	unsigned int uiV48Decay_Idx;
	unsigned int uiCr48Decay_Idx;
	unsigned int uiCr52Decay_Idx;
	unsigned int uiMn52Decay_Idx;
	unsigned int uiFe52Decay_Idx;
	unsigned int uiFe56Decay_Idx;
	unsigned int uiCo56Decay_Idx;
	unsigned int uiNi56Decay_Idx;
//	XDATASET 	cCaHK_Line_Results;
//	XDATASET 	cCaHK_Spectra_Results;
	printf("Starting.\n");
	XASTRO_ATOMIC_IONIZATION_DATA	* lpcIon_Energy_Data[] = {
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(1),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(2),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(2),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(6),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(7),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(8),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(10),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(12),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(14),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(16),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(18),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(20),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(20),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(21),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(22),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(22),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(23),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(24),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(24),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(25),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(26),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(26),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(27),
		g_xAstro_Ionization_Energy_Data.Get_Ionization_Data_Ptr(28)
	};
	unsigned int uiI,uiJ,uiK,uiL,uiM;
	unsigned int uiNumAbdIon = sizeof(lpcIon_Energy_Data) / sizeof(XASTRO_ATOMIC_IONIZATION_DATA	*);
	for (uiI = 0; uiI < uiNumAbdIon; uiI++)
	{
		if (lpcIon_Energy_Data[uiI] == NULL)
			printf("Unable to find ion %i.\n",uiI);
	}
	printf("Ions confirmed.\n");


	double		dNi56_Halflifes[2] = {6.075 * 86400.0,77.233 * 86400.0};
	double		dNi56_Nuclear_Mass[3] = {55.942132, 55.9398393, 55.9349375};

	double		dFe52_Halflifes[2] = {8.275 * 3600.0, 5.591 * 86400.0};
	double		dFe52_Nuclear_Mass[3] = {51.948114, 51.9455655, 51.9405075};

	double		dCr48_Halflifes[2] = {21.56 * 3600.0, 15.9735 * 86400.0};
	double		dCr48_Nuclear_Mass[3] = {47.954032, 47.9522537, 47.9479463};

	double		dTi44_Halflifes[2] = {60.0 * 86400.0 * 365.2425, 3.97 * 3600.0};
	double		dTi44_Nuclear_Mass[3] = {43.9596901, 43.9594028, 43.9554818};

	// Abundance values from Seitenzahl et al 2013 MNRAS, N100 model
	// Note these abundances sum all isotopes of each element, which will be used below to only determine the 
	// alpha series isotopes.  This is wrong, but the alpha series tends to be the most dominant isotope of each element
	// given that there still exists > 10% error in determining the elemental abundances, I think this is a reasonable estimation
	double	dLog_Abd_Mg[2] = {-2.4471971698, -1.8135824334}; // Ne20 and Mg24, respectively
	double	dLog_Abd_Si[6] = {-0.5415283333, -0.9378736444, -1.7067798935, -1.8312788521,-4.4942397241,-2.8417249788}; //Si,S,Ar,Ca,Ti,Cr
	double	dLog_Abd_Ni[2] = {-0.8921870067, -0.1574165722}; // Fe52,Ni56

	double	dTemp_Sum;
	double	dAbd_Mg[2];
	double	dAbd_Si[6];
	double	dAbd_Ni[2];

	dTemp_Sum = pow(10.0,dLog_Abd_Mg[0]) + pow(10.0,dLog_Abd_Mg[1]);
	dAbd_Mg[0] = pow(10.0,dLog_Abd_Mg[0]) / dTemp_Sum;
	dAbd_Mg[1] = pow(10.0,dLog_Abd_Mg[1]) / dTemp_Sum;

	unsigned int uiEntries = sizeof(dLog_Abd_Si) / sizeof(double);
	dTemp_Sum = 0.0;
	for (uiI = 0; uiI < uiEntries; uiI++)
		dTemp_Sum += pow(10.0,dLog_Abd_Si[uiI]);
	for (uiI = 0; uiI < uiEntries; uiI++)
		dAbd_Si[uiI] = pow(10.0,dLog_Abd_Si[uiI]) / dTemp_Sum;

	dTemp_Sum = pow(10.0,dLog_Abd_Ni[0]) + pow(10.0,dLog_Abd_Ni[1]);
	dAbd_Ni[0] = pow(10.0,dLog_Abd_Ni[0]) / dTemp_Sum;
	dAbd_Ni[1] = pow(10.0,dLog_Abd_Ni[1]) / dTemp_Sum;


	unsigned int uiMax_Ion = 28;
	double	* lpdElectron_Density[28];
	double	* lpdElectron_Density_EO[28];
    double		dPhotosphere_Velocity[28];
    double		dPhotosphere_Velocity_EO[28];
    unsigned int uiPS_Velocity_Idx[28];
    unsigned int uiPS_Velocity_Idx_EO[28];
	double	dElectron_Optical_Depth[28];
	double	dElectron_Optical_Depth_EO[28];
	double * lpdElectron_Density_Temp;
	double * lpdElectron_Density_Temp_EO;
	double * lpdVolume_tref;

	unsigned int uiNum_Points;
	if (xParse_Command_Line_Exists(iArgC,lpszArgV,"--filenum"))
	{
		unsigned int uiFile = xParse_Command_Line_UInt(iArgC,lpszArgV,"--filenum");
		sprintf(lpszFile_To_Read,"explos_hdf5_chk_%04i",uiFile);
	}
	else if (xParse_Command_Line_Exists(iArgC,lpszArgV,"--file"))
	{
		xParse_Command_Line_String(iArgC,lpszArgV,"--file",lpszFile_To_Read,256);
	}
	else
	{
		fprintf(stderr,"Usage: photosphere --file=<filename>\n or:   photosphere --filenum=<number>\n The filenum option will load file explos_hdf5_chk_X; uses format %%04i, so user does not need to specify leading zeros.\n");
		exit(0);
	}

	if (lpszFile_To_Read[0] == 0)
	{
		printf("No FLASH file specified.  Exiting...\n");
		exit(0);
	}
	bool bMetal_Shell = xParse_Command_Line_Exists(iArgC,lpszArgV,"--convert-shell-to-metals");
//	printf("%s\n",lpszFile_To_Read);
//	dTemp = xParse_Command_Line_Dbl(iArgC, lpszArgV, "--temp",10000.0);


//	FILE * fileCombinedData = fopen("out.csv");
//	if (fileCombinedData) // there is a single combined data file, use it
//	{
//		XDATASET	cCombinedData;
//		cCombinedData.ReadDataFile("out.csv",false,false,',',1);
//		fgets(lpszFieldNames,
//	}
	XFLASH_File	cFlash_File;
	FILE * fileOut = fopen("photosphere.csv","wt");
	FILE * fileOutEO = fopen("photosphere_eo.csv","wt");
	FILE * fileOutTest = fopen("photosphere_test.csv","wt");

	cFlash_File.Open(lpszFile_To_Read);
	dTimeRef = cFlash_File.m_dTime; //xParse_Command_Line_Dbl(iArgC,lpszArgV,"-time_ref",50.0);
	printf("Opened file %s.\n",lpszFile_To_Read);
	printf("Time ref set to %.1f s\n",dTimeRef);

	unsigned int uiLeaf_Blocks = 0;
//	for (uiI = 0; uiI < cFlash_File.m_uiNum_Vars; uiI++)
//		printf("%s\n",cFlash_File.m_lpszVar_Names[uiI]);

	for (uiI = 0; uiI < cFlash_File.m_uiNum_Blocks; uiI++)
	{
		if (cFlash_File.m_lpeBlock_Node_Type[uiI] == FR_LEAF_NODE)
			uiLeaf_Blocks++;
	}

	uiI = 0; uiCoord_Idx = uiI;
	uiI++; uiZone_Size_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"velx"); uiI++; uiVel_Idx = uiI;
	if (uiFlash_Idx[uiI - 2] >= cFlash_File.m_uiNum_Vars)
	{
		printf("Velocity missing. Exiting.\n");
		exit(1);
	}
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"dens"); uiI++; uiDens_Idx = uiI;
	if (uiFlash_Idx[uiI - 2] >= cFlash_File.m_uiNum_Vars)
	{
		printf("(%i %i %i) Density missing. Exiting.\n",uiI,uiFlash_Idx[uiI - 1],cFlash_File.m_uiNum_Vars);
		exit(1);
	}
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"hyd"); uiI++; uiHyd_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"he3"); uiI++; uiHe3_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"he4"); uiI++; uiHe4_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"c12"); uiI++; uiC12_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"n14"); uiI++; uiN14_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"o16"); uiI++; uiO16_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ne20"); uiI++; uiNe20_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"mg24"); uiI++; uiMg24_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"si28"); uiI++; uiSi28_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"s32"); uiI++; uiS32_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ar36"); uiI++; uiAr36_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ca40"); uiI++; uiCa40_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ca44"); uiI++; uiCa44_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ti44"); uiI++; uiTi44_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ti48"); uiI++; uiTi48_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"cr48"); uiI++; uiCr48_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"cr52"); uiI++; uiCr52_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"fe52"); uiI++; uiFe52_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"fe56"); uiI++; uiFe56_Idx = uiI;
	uiFlash_Idx[uiI - 1] = Find_Variable_Idx(cFlash_File,"ni56"); uiI++; uiNi56_Idx = uiI;
	unsigned int uiFlash_Vars = uiI - 1;


	uiI++; uiHeComb_Idx = uiI; 
	uiI++; uiCaComb_Idx = uiI;
	uiI++; uiTiComb_Idx = uiI;
	uiI++; uiCrComb_Idx = uiI;
	uiI++; uiFeComb_Idx = uiI;

	uiI++; uiNi56Decay_Idx = uiI;
	uiI++; uiCo56Decay_Idx = uiI;
	uiI++; uiFe56Decay_Idx = uiI;
	uiI++; uiFe52Decay_Idx = uiI;
	uiI++; uiMn52Decay_Idx = uiI;
	uiI++; uiCr52Decay_Idx = uiI;
	uiI++; uiCr48Decay_Idx = uiI;
	uiI++; uiV48Decay_Idx  = uiI;
	uiI++; uiTi48Decay_Idx = uiI;
	uiI++; uiTi44Decay_Idx = uiI;
	uiI++; uiSc44Decay_Idx = uiI;
	uiI++; uiCa44Decay_Idx = uiI;

	uiL = 0;
	cData.Allocate(uiI + 1,uiLeaf_Blocks * cFlash_File.m_uiBlock_Dimensions[0]);
	cData.Zero();

//	double dVelocity_Scale = 1.0e-9; // convert to 10,000 km/s

	for (uiI = 0; uiI < cFlash_File.m_uiNum_Blocks; uiI++)
	{
		if (cFlash_File.m_lpeBlock_Node_Type[uiI] == FR_LEAF_NODE)
		{
			int iOffset = 2 * cFlash_File.m_uiNum_Dimensions * uiI;
			double ddx = 0.0,ddy = 0.0,ddz = 0.0;
			// compute the cell 
			ddx = (cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 1] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]);
			if (cFlash_File.m_uiNum_Dimensions >= 2)
				ddy = cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 3] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 2];
			if (cFlash_File.m_uiNum_Dimensions >= 3)
				ddz = cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 5] - cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 4];
			ddx /= (double)cFlash_File.m_uiBlock_Dimensions[0];
			if (cFlash_File.m_uiNum_Dimensions >= 2)
				ddy /= (double)cFlash_File.m_uiBlock_Dimensions[1];
			if (cFlash_File.m_uiNum_Dimensions >= 3)
				ddz /= (double)cFlash_File.m_uiBlock_Dimensions[2];
			// Set coordinate
			for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
			{
				cData.SetElement(0,uiL + uiK,ddx * (uiK + 0.5) + cFlash_File.m_lpdBlock_Bounding_Box[iOffset + 0]); // coordinate
				cData.SetElement(1,uiL + uiK,ddx); // cell size
			}
			// Fill variable data
			for (uiJ = 0; uiJ < uiFlash_Vars; uiJ++)
			{
				if (uiFlash_Idx[uiJ] < cFlash_File.m_uiNum_Vars)
				{
					XFLASH_Block * lpcBlock = cFlash_File.GetBlock(uiFlash_Idx[uiJ],uiI);
					
//					if (uiJ != 0) // velocity index
//					{
						for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
							cData.SetElement(uiJ + 2,uiL + uiK,lpcBlock->m_lpdData[uiK]);
//					}
//					else
//					{
//						for (uiK = 0; uiK < cFlash_File.m_uiBlock_Dimensions[0]; uiK++)
//							cData.SetElement(uiJ + 1,uiL + uiK,lpcBlock->m_lpdData[uiK] * dVelocity_Scale);
//					}
					cFlash_File.ReleaseBlock(lpcBlock);
				}
			}
			uiL += cFlash_File.m_uiBlock_Dimensions[0];
		}
	}
	cFlash_File.Close();
	uiNum_Points = uiL;
//	printf("I ran, %i elements\n",cData.GetNumElements());
//	cData.SaveDataFileCSV("out.csv",(const char **)cFlash_File.m_lpszVar_Names);
//	cData.SaveDataFileCSV("out.csv",NULL);
//	return 1;
	

	// Compute combined He (He-3 + He-4)
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		cData.SetElement(uiHeComb_Idx,uiI,cData.GetElement(uiHe3_Idx,uiI) + cData.GetElement(uiHe4_Idx,uiI));


		// "Fix" the group abundances
		if (GetAbundance(uiI,uiHyd_Idx,cData) == 0.0) // in the supernova
		{
			double dMgg = GetAbundance(uiI,uiMg24_Idx, cData) + GetAbundance(uiI,uiNe20_Idx,cData);
			cData.SetElement(uiMg24_Idx,uiI,dMgg * dAbd_Mg[0]);
			cData.SetElement(uiNe20_Idx,uiI,dMgg * dAbd_Mg[1]);

			double dSig = GetAbundance(uiI,uiSi28_Idx, cData) + GetAbundance(uiI,uiS32_Idx, cData) + GetAbundance(uiI,uiAr36_Idx, cData) + GetAbundance(uiI,uiCa40_Idx, cData) + GetAbundance(uiI,uiTi44_Idx, cData) + GetAbundance(uiI,uiCr48_Idx, cData);
			cData.SetElement(uiSi28_Idx,uiI,dSig * dAbd_Si[0]);
			cData.SetElement(uiS32_Idx, uiI,dSig * dAbd_Si[1]);
			cData.SetElement(uiAr36_Idx,uiI,dSig * dAbd_Si[2]);
			cData.SetElement(uiCa40_Idx,uiI,dSig * dAbd_Si[3]);
			cData.SetElement(uiTi44_Idx,uiI,dSig * dAbd_Si[4]);
			cData.SetElement(uiCr48_Idx,uiI,dSig * dAbd_Si[5]);

			double dNig = GetAbundance(uiI,uiFe52_Idx, cData) + GetAbundance(uiI,uiNi56_Idx, cData);
			cData.SetElement(uiFe52_Idx,uiI,dNig * dAbd_Ni[0]);
			cData.SetElement(uiNi56_Idx,uiI,dNig * dAbd_Ni[1]);
		}
		if (bMetal_Shell && GetAbundance(uiI,uiHyd_Idx,cData) > 0.0 && GetAbundance(uiI,uiHe3_Idx,cData) > 0.0001)
		{
			//enhance carbon & oxygen
			double	dHyd = GetAbundance(uiI,uiHyd_Idx,cData);
			double	dHe3 = GetAbundance(uiI,uiHe3_Idx,cData);
			double	dHe4 = GetAbundance(uiI,uiHe4_Idx,cData);
			double dNon_Metals = dHyd + dHe3 + dHe4;
			double	dC = GetAbundance(uiI,uiC12_Idx,cData) + 0.5 * dNon_Metals;
			double	dO = GetAbundance(uiI,uiO16_Idx,cData) + 0.5 * dNon_Metals;
//			printf("Zone %i setting C to %.2e O to %.2e (%.2e,%.2e,%.2e)\n",uiI,dC,dO,dHyd,dHe3,dHe4);
			cData.SetElement(uiHyd_Idx,uiI,0.0);
			cData.SetElement(uiHe3_Idx,uiI,0.0);
			cData.SetElement(uiHe4_Idx,uiI,0.0);
			cData.SetElement(uiHeComb_Idx,uiI,0.0);
			cData.SetElement(uiO16_Idx,uiI,dO);
			cData.SetElement(uiC12_Idx,uiI,dC);
			cData.SetElement(uiO16_Idx,uiI,dO);
		}			
	}

	lpdVolume_tref = new double[uiNum_Points];
	for (uiI = 0; uiI < uiMax_Ion; uiI++)
		lpdElectron_Density[uiI] = new double[uiNum_Points];
	for (uiI = 0; uiI < uiMax_Ion; uiI++)
		lpdElectron_Density_EO[uiI] = new double[uiNum_Points];
	lpdElectron_Density_Temp = new double[uiNum_Points];
	lpdElectron_Density_Temp_EO = new double[uiNum_Points];

	double dVol_Const = 4.0 / 3.0 * g_cConstants.dPi;
	uiShell_Lower_Idx = 0;
	for (uiI = 0; uiI < uiNum_Points; uiI++)
	{
		if (GetAbundance(uiI,uiHe3_Idx, cData) > 0.0)
			uiShell_Lower_Idx = uiI;
		double dXim12 = cData.GetElement(uiCoord_Idx,uiI) - cData.GetElement(uiZone_Size_Idx,uiI) * 0.5;
		double dXip12 = cData.GetElement(uiCoord_Idx,uiI) + cData.GetElement(uiZone_Size_Idx,uiI) * 0.5;
		
		lpdVolume_tref[uiI] = dVol_Const * (dXip12 * dXip12 * dXip12 - dXim12 * dXim12 * dXim12); // volume of element at reference time
	}

	fprintf(fileOut, "Day, Vps (kappa=0.2)");
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOut,", Vel-ps %s",lpszRoman);
	}
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOut,", Tau-ps %s",lpszRoman);
	}
    fprintf(fileOut,"\n");

	fprintf(fileOutEO, "Day, Vps (kappa=0.2)");
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOutEO,", Vel-ps %s",lpszRoman);
	}
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOutEO,", Tau-ps %s",lpszRoman);
	}
    fprintf(fileOut,"\n");

	fprintf(fileOutTest,"Day, i, t/tref^3, v/vref, Dens(t) [g/cc], Vel [cm/s], dx");
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOutTest,", ne [/cc] %s",lpszRoman);
	}
	for (uiM = 0; uiM < uiMax_Ion; uiM++)
	{
		char lpszRoman[8];
		xRomanNumeralGenerator(lpszRoman,uiM + 2);
		fprintf(fileOutTest,", tau_e %s",lpszRoman);
	}
    fprintf(fileOutTest,"\n");

	double dExterior_Density = 0.0;
	for (uiI = uiNum_Points - 1; uiI > uiNum_Points - 100; uiI--)
		dExterior_Density += cData.GetElement(uiDens_Idx,uiI) * 0.0105;
	printf("Using exterior density %.2e\n",dExterior_Density);
//    printf("h mass %.5e\n",g_XASTRO.k_dmh);
    for (uiJ = 0; uiJ < 120; uiJ ++)
    {
		double	dDay = uiJ * 0.25 + 0.25;
//		uiJ = 6; //@@DEBUG
        double dTime = 86400.0 * dDay; // set time in seconds
		Generate_Decay_Products(dTimeRef, dTime, cData, uiNi56_Idx, uiNi56Decay_Idx, uiCo56Decay_Idx, uiFe56Decay_Idx, dNi56_Halflifes, dNi56_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiFe52_Idx, uiFe52Decay_Idx, uiMn52Decay_Idx, uiCr52Decay_Idx, dFe52_Halflifes, dFe52_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiCr48_Idx, uiCr48Decay_Idx, uiV48Decay_Idx, uiTi48Decay_Idx, dCr48_Halflifes, dCr48_Nuclear_Mass);
		Generate_Decay_Products(dTimeRef, dTime, cData, uiTi44_Idx, uiTi44Decay_Idx, uiSc44Decay_Idx, uiCa44Decay_Idx, dTi44_Halflifes, dTi44_Nuclear_Mass);
		// Generate combined element groups (e.g. combine Ca-40 and Ca-44 into a single Ca group)
		for (uiI = 0; uiI < uiNum_Points; uiI++)
		{
			double dSum;
			//double	dR = cVelocity.GetElement(0,uiI);

			dSum = 0.0;
			dSum += cData.GetElement(uiCa40_Idx,uiI);
			dSum += cData.GetElement(uiCa44Decay_Idx,uiI);
			cData.SetElement(uiCaComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiTi44_Idx,uiI);
			dSum += cData.GetElement(uiTi48_Idx,uiI);
			dSum += cData.GetElement(uiTi48Decay_Idx,uiI);
			cData.SetElement(uiTiComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiCr52_Idx,uiI);
			dSum += cData.GetElement(uiCr48Decay_Idx,uiI);
			dSum += cData.GetElement(uiCr52Decay_Idx,uiI);
			cData.SetElement(uiCrComb_Idx,uiI,dSum);

			dSum = 0.0;
			dSum += cData.GetElement(uiFe56_Idx,uiI);
			dSum += cData.GetElement(uiFe56Decay_Idx,uiI);
			dSum += cData.GetElement(uiFe52Decay_Idx,uiI);
			cData.SetElement(uiFeComb_Idx,uiI,dSum);

		}
//		double dExterior_Density = (cData.GetElement(uiDens_Idx,uiNum_Points - 1) * 1.05); // 5% higher than density in outermost zone
//		double dkT = dTemp * g_XASTRO.k_dKb;

		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			memset(lpdElectron_Density[uiM],0,sizeof(double) * uiNum_Points);
		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			memset(lpdElectron_Density_EO[uiM],0,sizeof(double) * uiNum_Points);
		for (uiI = 0; uiI < uiNum_Points; uiI++)
		{
			double	dMass_Density = cData.GetElement(uiDens_Idx,uiI);
			double	dNumber_Density;
			if (dMass_Density < dExterior_Density) // make the true CSM very low density
				dMass_Density *= (1.0e-24 / dExterior_Density);
			dNumber_Density = dMass_Density;
//			if (uiI < 10)
//				printf("%.2e: %.2e\n",cData.GetElement(uiCoord_Idx,uiI),dNumber_Density);
			dNumber_Density /= g_XASTRO.k_dmh;
			double	dAbd[][3] = {
				{GetAbundance(uiI,uiHyd_Idx, cData),1.0,1.0},
				{GetAbundance(uiI,uiHe3_Idx, cData),3.0,2.0},
				{GetAbundance(uiI,uiHe4_Idx, cData),4.0,2.0},
				{GetAbundance(uiI,uiC12_Idx, cData),12.0,6.0},
				{GetAbundance(uiI,uiN14_Idx, cData),14.0,7.0},
				{GetAbundance(uiI,uiO16_Idx, cData),16.0,8.0},
				{GetAbundance(uiI,uiNe20_Idx, cData),20.0,10.0},
				{GetAbundance(uiI,uiMg24_Idx, cData),24.0,12.0},
				{GetAbundance(uiI,uiSi28_Idx, cData),28.0,14.0},
				{GetAbundance(uiI,uiS32_Idx, cData),32.0,16.0},
				{GetAbundance(uiI,uiAr36_Idx, cData),36.0,18.0},
				{GetAbundance(uiI,uiCa40_Idx, cData),40.0,20.0},
				{GetAbundance(uiI,uiCa44Decay_Idx, cData) + GetAbundance(uiI,uiCa44_Idx, cData),44.0,20.0},
				{GetAbundance(uiI,uiSc44Decay_Idx, cData),44.0,21.0},
				{GetAbundance(uiI,uiTi44Decay_Idx, cData) + GetAbundance(uiI,uiTi44_Idx, cData),44.0,22.0},
				{GetAbundance(uiI,uiTi48Decay_Idx, cData) + GetAbundance(uiI,uiTi48_Idx, cData),48.0,22.0},
				{GetAbundance(uiI,uiV48Decay_Idx, cData),48.0,23.0},
				{GetAbundance(uiI,uiCr48Decay_Idx, cData) + GetAbundance(uiI,uiCr48_Idx, cData),48.0,24.0},
				{GetAbundance(uiI,uiCr52Decay_Idx, cData) + GetAbundance(uiI,uiCr52_Idx, cData),52.0,24.0},
				{GetAbundance(uiI,uiMn52Decay_Idx, cData),52.0,25.0},
				{GetAbundance(uiI,uiFe52Decay_Idx, cData) + GetAbundance(uiI,uiFe52_Idx, cData),52.0,26.0},
				{GetAbundance(uiI,uiFe56Decay_Idx, cData) + GetAbundance(uiI,uiFe56_Idx, cData),56.0,26.0},
				{GetAbundance(uiI,uiCo56Decay_Idx, cData),56.0,27.0},
				{GetAbundance(uiI,uiNi56_Idx, cData),56.0,28.0}
				};

			bool bEjecta_Zone = GetAbundance(uiI,uiHyd_Idx, cData) < 1.0e-9;
			unsigned int uiNum_Abd = sizeof(dAbd) / (3 * sizeof(double));
            double dAbd_Tot = 0.0;
			for (unsigned int uiAbd = 0; uiAbd < uiNum_Abd; uiAbd++)
			{
				dAbd_Tot += dAbd[uiAbd][0];
				if (dNumber_Density < 0.0 || dAbd[uiAbd][0] < 0.0)
					printf("- data for zone %i, element %i (%.2e %.2e)\n",uiI,uiAbd,dNumber_Density, dAbd[uiAbd][0]);
				double dNj = dNumber_Density * dAbd[uiAbd][0] / dAbd[uiAbd][1];
				// estimate singly ionized ion density
				for (uiM = 0; uiM < uiMax_Ion; uiM++)
				{
					if ((uiM + 1) <= dAbd[uiAbd][2])
						lpdElectron_Density[uiM][uiI] += dNj;
					if (bEjecta_Zone) // ejecta zone
						lpdElectron_Density_EO[uiM][uiI] += dNj;
				}
			}
			if (dAbd_Tot < 0.99)
			{
				printf("Zone %i has abd %.3e\n",uiI,dAbd_Tot);
				for (unsigned int uiAbd = 0; uiAbd < uiNum_Abd; uiAbd++)
				{
					if (uiAbd != 0)
						printf ("\t");
					printf ("%.2e",dAbd[uiAbd][0]);
				}
				printf ("\n");
			}
//			double dDeltaX = cData.GetElement(uiZone_Size_Idx,uiI);
//			double	dConst_Terms = 0.66524574e-24; // electron scattering optical depth

			for (uiM = 0; uiM < uiMax_Ion - 1; uiM++)
			{
				lpdElectron_Density[uiM + 1][uiI] += lpdElectron_Density[uiM][uiI];
				lpdElectron_Density_EO[uiM + 1][uiI] += lpdElectron_Density_EO[uiM][uiI];
			}
//			printf("Electron densities done\n");
//			for (uiM = 0; uiM < 28; uiM++)
//				dElectron_Optical_Depth_tref[uiM] += lpdElectron_Density[uiM][uiI] * dConst_Terms * dDeltaX;
	//		double	dKappa_0 = lpdElectron_Density[0][uiI] * dConst_Terms
	//		printf("%.3e\t%.3e\t%.3e\t%.3e\t%.3e\t%.3e\n",dDeltaX,cDens.GetElement(1,uiI) / (12 * g_XASTRO.k_dmh),lpdElectron_Density[0][uiI],lpdElectron_Density[1][uiI],lpdElectron_Density[2][uiI],lpdElectron_Density[0][uiI] * 0.66524574e-24 * dDeltaX);
		}
		memset(dElectron_Optical_Depth,0,sizeof(dElectron_Optical_Depth));
		memset(dPhotosphere_Velocity,0,sizeof(dPhotosphere_Velocity));
		memset(uiPS_Velocity_Idx,0,sizeof(uiPS_Velocity_Idx));
		memset(dElectron_Optical_Depth_EO,0,sizeof(dElectron_Optical_Depth_EO));
		memset(dPhotosphere_Velocity_EO,0,sizeof(dPhotosphere_Velocity_EO));
		memset(uiPS_Velocity_Idx_EO,0,sizeof(uiPS_Velocity_Idx_EO));
		double	dConst_Terms = 0.66524574e-24;// * pow(dTime / dTimeRef,-3.0);// * pow(dTime,-3.0);
		memcpy(lpdElectron_Density_Temp,lpdElectron_Density[uiMax_Ion - 1],sizeof(double) * uiNum_Points);
		memcpy(lpdElectron_Density_Temp_EO,lpdElectron_Density_EO[uiMax_Ion - 1],sizeof(double) * uiNum_Points);
		double dGeneric_Photosphere_Optical_Depth = 0.0;
		double dGeneric_Photosphere_Velocity = 0.0;
		double dGeneric_Photosphere_Optical_Depth_EO = 0.0;
		double dGeneric_Photosphere_Velocity_EO = 0.0;
		unsigned int uiGeneric_PS_Velocity_Idx = 0;
		unsigned int uiGeneric_PS_Velocity_Idx_EO = 0;
		for (uiI = uiNum_Points - 1; uiI < uiNum_Points; uiI--)
		{
			double	dMass_Density = cData.GetElement(uiDens_Idx,uiI);
			if (dMass_Density < dExterior_Density) // make the true CSM very low density
				dMass_Density *= (1.0e-24 / dExterior_Density);
			bool bEjecta_Zone = GetAbundance(uiI,uiHyd_Idx, cData) < 1.0e-9;
			// Get coordinates, box size and, velocity for zone and its neighbors
			double dXi = cData.GetElement(uiCoord_Idx,uiI);
	        double	dDeltaX = cData.GetElement(uiZone_Size_Idx,uiI);
			double dXim1 = 0.0;
			double dXip1 = dXi + dDeltaX;
			double	dVi = cData.GetElement(uiVel_Idx,uiI);
			double dVim1 = 0.0;
			double dVip1 = dVi;

			if (uiI != 0)
			{
				dXim1 = cData.GetElement(uiCoord_Idx,uiI - 1);
				dVim1 = cData.GetElement(uiVel_Idx,uiI - 1);
			}
			if (uiI != uiNum_Points - 1)
			{
				dXip1 = cData.GetElement(uiCoord_Idx,uiI + 1);
				dVip1 = cData.GetElement(uiVel_Idx,uiI + 1);
			}
			// compute position and velocity at zone boundaries (at time tref)
			double dXim12 = dXi - dDeltaX * 0.5;
			double dXip12 = dXi + dDeltaX * 0.5;
//						if (uiI == uiNum_Points - 5)
//							printf("v at point 200 (%.2e,%.2e): %.2e - %.2e = %.2e\n",dXi,dDeltaX,dXim12,dXip12,lpdVolume_tref[uiI]);
			double dVim12 = 0.0;// = dVi - (dVi - dVim1) / (dXi - dXim1) * (dXim12 - dXim1);
			if (uiI != 0)
				dVim12 = dVi - (dVi - dVim1) / (dXi - dXim1) * (dXim12 - dXim1);
				
			double dVip12 = dVi + (dVip1 - dVi) / (dXip1 - dXi) * (dXip12 - dXi);
			// compute zone boundaries at current time
			dXim12 += dVim12 * (dTime - dTimeRef);
			dXip12 += dVip12 * (dTime - dTimeRef);
			// compute volume of zone at current time		
			double dVolume = dVol_Const * (dXip12 * dXip12 * dXip12 - dXim12 * dXim12 * dXim12);
			if (dVolume < 0.0)
				dVolume *= -1.0;
			dDeltaX = dXip12 - dXim12; // compute delta x at time
			if (dDeltaX < 0.0)
				dDeltaX *= -1.0;
//						if (uiI == uiNum_Points - 5)
//							printf("v at point 200 (%.2e): %.2e - %.2e = %.2e\n",dVi,dXim12,dXip12,dVolume);
			// compute scaling factor for density between tref and now
			double dVolTerm = (lpdVolume_tref[uiI] / dVolume);
	        // compute optical depth for electron scattering, assuming partial ionizations
//			printf("%.3e\t%.3e\t%.3e\n",lpdElectron_Density[0][uiI],dDeltaX,dElectron_Optical_Depth[0]);
			if ((uiI%5000) == 0)
			{
				fprintf(fileOutTest,"%.2f, %i, %.2e, %.2e, %.2e, %.2e, %.2e",
					dDay,
					uiI,
                    pow(dTime / dTimeRef,-3.0),
					dVolTerm,
					cData.GetElement(uiDens_Idx,uiI) * dVolTerm,
					cData.GetElement(uiVel_Idx,uiI),
					dDeltaX);
					for (uiM = 0; uiM < uiMax_Ion; uiM++)
						fprintf(fileOutTest,", %.2e",lpdElectron_Density[uiM][uiI] * dVolTerm);
					for (uiM = 0; uiM < uiMax_Ion; uiM++)
						fprintf(fileOutTest,", %.2e",lpdElectron_Density[uiM][uiI] * dConst_Terms * dDeltaX * dVolTerm);
					fprintf(fileOutTest,"\n");
			}

			for (uiM = 0; uiM < uiMax_Ion; uiM++)
			{
			    dElectron_Optical_Depth[uiM] += lpdElectron_Density[uiM][uiI] * dConst_Terms * dDeltaX * dVolTerm;
			    dElectron_Optical_Depth_EO[uiM] += lpdElectron_Density_EO[uiM][uiI] * dConst_Terms * dDeltaX * dVolTerm;
			    if (dElectron_Optical_Depth[uiM] > 0.66 && dPhotosphere_Velocity[uiM] == 0.0)
			    {
			            dPhotosphere_Velocity[uiM] = cData.GetElement(uiVel_Idx,uiI);
			            uiPS_Velocity_Idx[uiM] = uiI;
			    }
			    if (dElectron_Optical_Depth_EO[uiM] > 0.66 && dPhotosphere_Velocity_EO[uiM] == 0.0)
			    {
			            dPhotosphere_Velocity_EO[uiM] = cData.GetElement(uiVel_Idx,uiI);
			            uiPS_Velocity_Idx_EO[uiM] = uiI;
			    }
			}
			// assume opacity of 0.2 cm^2/g
			dGeneric_Photosphere_Optical_Depth += 0.2 * dMass_Density * dDeltaX * dVolTerm;
			if (bEjecta_Zone)
				dGeneric_Photosphere_Optical_Depth_EO += 0.2 * dMass_Density * dDeltaX * dVolTerm;
//				if (uiGeneric_PS_Velocity_Idx == 0)
//			        printf("%.2f %i %.2e %.2e %.2e %.2e\n",dDay,uiI,dGeneric_Photosphere_Optical_Depth,dVolTerm,dDeltaX,cData.GetElement(uiDens_Idx,uiI));
			if (dGeneric_Photosphere_Optical_Depth > 0.66 && uiGeneric_PS_Velocity_Idx == 0)
			{
		        dGeneric_Photosphere_Velocity = cData.GetElement(uiVel_Idx,uiI);
		        uiGeneric_PS_Velocity_Idx = uiI;
			}
			if (dGeneric_Photosphere_Optical_Depth_EO > 0.66 && uiGeneric_PS_Velocity_Idx_EO == 0)
			{
		        dGeneric_Photosphere_Velocity_EO = cData.GetElement(uiVel_Idx,uiI);
		        uiGeneric_PS_Velocity_Idx_EO = uiI;
			}
		}
		//return 1;
		fprintf(fileOut, "%.2f",dDay);
		fprintf(fileOut, ", %.3e",dGeneric_Photosphere_Velocity);
		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			fprintf(fileOut,", %.3e",dPhotosphere_Velocity[uiM]);
		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			fprintf(fileOut,", %.3e",dElectron_Optical_Depth[uiM]);
		fprintf(fileOut,"\n");

		fprintf(fileOutEO, "%.2f",dDay);
		fprintf(fileOutEO, ", %.3e",dGeneric_Photosphere_Velocity_EO);
		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			fprintf(fileOutEO,", %.3e",dPhotosphere_Velocity_EO[uiM]);
		for (uiM = 0; uiM < uiMax_Ion; uiM++)
			fprintf(fileOutEO,", %.3e",dElectron_Optical_Depth_EO[uiM]);
		fprintf(fileOutEO,"\n");
	} // end of time loop

	fclose(fileOut);

//	delete [] cSynIonData;

	delete [] lpdVolume_tref;
	for (uiI = 0; uiI < uiMax_Ion; uiI++)
		delete [] lpdElectron_Density[uiI];
	for (uiI = 0; uiI < uiMax_Ion; uiI++)
		delete [] lpdElectron_Density_EO[uiI];
	delete [] lpdElectron_Density_Temp;
	delete [] lpdElectron_Density_Temp_EO;
	return 0;
}

