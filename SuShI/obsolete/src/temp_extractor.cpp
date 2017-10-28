#include <xflash.h>
#include <stdio.h>
#include <cmath>

class DATA
{
public:
	double	m_dTemp;
	double	m_dVelocity;
	double	m_dRadius;
	double	m_dLuminosity;
	double	m_dEint;

	DATA(void)
	{
		m_dTemp = 0.0;
		m_dVelocity = 0.0;
		m_dRadius = 0.0;
		m_dEint = 0.0;
		m_dLuminosity = 0.0;
	}
};

double	g_dPi = 3.141592654;

void Test(DATA & io_cData, const double & i_dTemp, const double & i_dVelocity, const double & i_dRadius)
{
	if (io_cData.m_dTemp < i_dTemp)
	{
		io_cData.m_dTemp = i_dTemp;
		io_cData.m_dVelocity = i_dVelocity;
		io_cData.m_dRadius = i_dRadius;
		io_cData.m_dLuminosity = 0.5 * 5.670373e-5 * pow(i_dTemp,4.0) * 4.0 * g_dPi * i_dRadius * i_dRadius;
	}
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	XFLASH_File	cFile;
	unsigned int uiTemp_Idx;
	unsigned int uiHyd_Idx;
	unsigned int uiHe3_Idx;
	unsigned int uiVelX_Idx;
	unsigned int uiEint_Idx;
	unsigned int uiDens_Idx;
	FILE * fileOut = fopen("Temp_Data.csv","wt");
	fprintf(fileOut,"Time, Ejecta R, Ejecta V, Ejecta T, Ejecta L, Ejecta Eint, Shell R, Shell V, Shell T, Shell L, Shell Eint, CSM R, CSM V, CSM T, CSM L, CSM Eint\n");

	g_dPi = acos(-1.0);
	for (unsigned int uiI = 1; uiI < i_iArg_Count; uiI++)
	{
		printf("Reading %s\n",i_lpszArg_Values[uiI]);
		cFile.Open(i_lpszArg_Values[uiI]);

		uiTemp_Idx = cFile.Get_Variable_Idx_By_Name("temp");
		uiHyd_Idx = cFile.Get_Variable_Idx_By_Name("hyd ");
		uiHe3_Idx = cFile.Get_Variable_Idx_By_Name("he3 ");
		uiVelX_Idx = cFile.Get_Variable_Idx_By_Name("velx");
		uiEint_Idx = cFile.Get_Variable_Idx_By_Name("eint");
		uiDens_Idx = cFile.Get_Variable_Idx_By_Name("dens");

		DATA	cShell, cEjecta, cCSM;
		if (uiTemp_Idx != -1 && uiHyd_Idx != -1 && uiHe3_Idx != -1 && uiVelX_Idx != -1)
		{
			printf("Reading blocks\n");
			for (unsigned int uiJ = 0; uiJ < cFile.m_uiNum_Blocks; uiJ++)
			{
				if (cFile.m_lpeBlock_Node_Type[uiJ] == FR_LEAF_NODE)
				{
					XFLASH_Block * lpBlock_Temp = cFile.GetBlock(uiTemp_Idx,uiJ);
					XFLASH_Block * lpBlock_Hyd = cFile.GetBlock(uiHyd_Idx,uiJ);
					XFLASH_Block * lpBlock_He3 = cFile.GetBlock(uiHe3_Idx,uiJ);
					XFLASH_Block * lpBlock_VelX = cFile.GetBlock(uiVelX_Idx,uiJ);
					XFLASH_Block * lpBlock_Eint = cFile.GetBlock(uiEint_Idx,uiJ);
					XFLASH_Block * lpBlock_Dens = cFile.GetBlock(uiDens_Idx,uiJ);
					int iOffset = 2 * cFile.m_uiNum_Dimensions * uiJ;
					double ddx;
					// compute the cell 
					ddx = (cFile.m_lpdBlock_Bounding_Box[iOffset + 1] - cFile.m_lpdBlock_Bounding_Box[iOffset + 0]);
					ddx /= (double)cFile.m_uiBlock_Dimensions[0];
					for (unsigned int uiX = 0; uiX < lpBlock_Temp->m_uiSize[0]; uiX++)
					{
						double dX = ddx * (uiX + 0.5) + cFile.m_lpdBlock_Bounding_Box[iOffset + 0];
						double	dV = 4.0 * g_dPi * ((dX + ddx) * (dX + ddx) * (dX + ddx) - dX * dX * dX) / 3.0;
						if (lpBlock_Hyd->m_lpdData[uiX] > 3e-10 && lpBlock_He3->m_lpdData[uiX] > 3e-10)
						{ // shell material
							Test(cShell,lpBlock_Temp->m_lpdData[uiX],lpBlock_VelX->m_lpdData[uiX],dX);
							cShell.m_dEint += lpBlock_Eint->m_lpdData[uiX] * lpBlock_Dens->m_lpdData[uiX] * dV;
						}
						else if (lpBlock_Hyd->m_lpdData[uiX] > 3e-10)
						{ // circumstellar material
							Test(cCSM,lpBlock_Temp->m_lpdData[uiX],lpBlock_VelX->m_lpdData[uiX],dX);
							cCSM.m_dEint += lpBlock_Eint->m_lpdData[uiX] * lpBlock_Dens->m_lpdData[uiX] * dV;
						}
						else
						{ // ejecta
							Test(cEjecta,lpBlock_Temp->m_lpdData[uiX],lpBlock_VelX->m_lpdData[uiX],dX);
							cEjecta.m_dEint += lpBlock_Eint->m_lpdData[uiX] * lpBlock_Dens->m_lpdData[uiX] * dV;
						}
					}

					cFile.ReleaseBlock(lpBlock_Temp);
					cFile.ReleaseBlock(lpBlock_Hyd);
					cFile.ReleaseBlock(lpBlock_He3);
					cFile.ReleaseBlock(lpBlock_VelX);
					cFile.ReleaseBlock(lpBlock_Eint);
					cFile.ReleaseBlock(lpBlock_Dens);

				}
			}
			
			// output
			fprintf(fileOut,"%.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e, %.17e\n", cFile.m_dTime,cEjecta.m_dRadius,cEjecta.m_dVelocity,cEjecta.m_dTemp,cEjecta.m_dLuminosity,cEjecta.m_dEint, cShell.m_dRadius,cShell.m_dVelocity,cShell.m_dTemp,cShell.m_dLuminosity,cShell.m_dEint, cCSM.m_dRadius,cCSM.m_dVelocity,cCSM.m_dTemp,cCSM.m_dLuminosity,cCSM.m_dEint);
		}
		cFile.Close();
	}
	fclose(fileOut);
	return 0;
}
