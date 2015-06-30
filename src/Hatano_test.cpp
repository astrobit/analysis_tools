#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <line_anal.h>
#include <xastro.h>

 // roland thomas: SYNOW update


void Hatano_temp_analysis(const LINE_DATA &cLine, const char * i_lspzLine, const char * i_lpszFilename);
{
	
	FILE * fileOut = fopen(i_lpszFilename,"wt");
	for (uiI = 0; uiI = 5000; uiI < 20000; uiI += 100)
	{
		Optical
	}
}


int main(void)
{
	char lpszLine_Buffer[1024];
	double	dTRef = 0.0;
	double	dTemp = 0.0;
	double	dCaAbdAdj = 0.0;
	double	dSiAbdAdj = 0.0;
	double	dFeAbdAdj = 0.0;
	FILE * fileIn = fopen("params.txt","rt");
	if (fileIn)
	{
		fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn);
		dTRef = atof(lpszLine_Buffer);
		fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn);
		dTemp = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dCaAbdAdj = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dSiAbdAdj = atof(lpszLine_Buffer);
		if (fgets(lpszLine_Buffer,sizeof(lpszLine_Buffer),fileIn))
			dFeAbdAdj = atof(lpszLine_Buffer);
		fclose(fileIn);
	}
	else
	{
		printf("Unable to open params.txt\n");
		exit(1);
	}
	if (dTRef < 1.0 || dTemp < 30.0)
	{	
		printf("Invalid parameters.  Time = %.2e, Temp = %.2e\n",dTRef,dTemp);
		exit(1);
	}
                           //  Jlo   Jhi      A         lambda    A   E (eV)
	LINE_DATA	cCaH         ( 0.5 , 1.5 ,   1.466e8  , 3934.777, 40, 6.11316); // Hatano
	LINE_DATA	cCaK         ( 0.5 , 0.5 ,   1.444e8  , 3969.592, 40, 6.11316);
	LINE_DATA	cCaIIIR1     ( 1.5 , 1.5 ,   1.125e6  , 8498.023, 40, 1.692408 + 6.11316);
	LINE_DATA	cCaIIIR2     ( 2.5 , 1.5 ,   9.924e6  , 8542.091, 40, 1.692408 + 6.11316);
	LINE_DATA	cCaIIIR3     ( 1.5 , 0.5 ,   1.058e7  , 8662.141, 40, 1.692408 + 6.11316);
	LINE_DATA	cSiII6355_1  ( 0.5 , 1.5 ,   8.197e7  , 6348.864, 28, 8.121023 + 8.15168);
	LINE_DATA	cSiII6355_2  ( 0.5 , 0.5 ,   8.154e7  , 6373.132, 28, 8.121023 + 8.15168);
	LINE_DATA	cSiII5463    ( 0.5 , 1.5 ,   5.586e6  , 5462.145, 28, 12.877090 + 8.15168);
	LINE_DATA	cSiIII4560_1 ( 1.0 , 2.0 ,   9.759e7  , 4553.898, 28, 19.016329 + 16.34584 + 8.15168); // Hatano
	LINE_DATA	cSiIII4560_2 ( 1.0 , 1.0 ,   9.735e7  , 4569.120, 28, 19.016329 + 16.34584 + 8.15168);
	LINE_DATA	cSiIII4560_3 ( 1.0 , 0.0 ,   9.866e7  , 4576.039, 28, 19.016329 + 16.34584 + 8.15168);
	LINE_DATA	cFeII4925    ( 2.5 , 1.5 ,   3.290e6  , 4925.302, 56, 2.89101812 + 7.9024);
	LINE_DATA	cFeII5018    ( 2.5 , 2.5 ,   2.658e6  , 5019.840, 56, 2.89101812 + 7.9024); // Hatano
	LINE_DATA	cFeII5169    ( 2.5 , 3.5 ,   4.207e6  , 5170.473, 56, 2.89101812 + 7.9024);


	XDATASET	cVelocity;
	XDATASET	cDens;
	XDATASET	cCa;
	XDATASET	cSi;
	XDATASET	cNi;
	XDATASET	cHe3;
	XDATASET	cHyd;
	XDATASET 	cCaHK_Line_Results;
	XDATASET 	cCaHK_Spectra_Results;
	unsigned int uiI;
	double		dTemps[32];// = {1000.0,11,3000.0,5000.0,7000.0,9000.0,11000.0,13000.0};
	unsigned int uiNumTemps = sizeof(dTemps) / sizeof(double);
	for (uiI = 0; uiI < 20; uiI++)
	{
		dTemps[uiI] = 1000.0 + 	100.0 * uiI;
	}
	for (uiI = 20; uiI < 32; uiI++)
	{
		dTemps[uiI] = 3000.0 + (uiI - 19) * 1000.0;
	}


	cVelocity.ReadDataFile("velx.txt", true, false);
	cDens.ReadDataFile("dens.txt", true, false);
	cCa.ReadDataFile("ca40.txt", true, false);
	cSi.ReadDataFile("sig.txt", true, false);
	cNi.ReadDataFile("fe.txt",true,false);
	cHe3.ReadDataFile("he3.txt",true,false);
	cHyd.ReadDataFile("hyd.txt",true,false);


/*	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
		LineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
		LineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
		LineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
		LineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
		LineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
		LineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
		LineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
	}
	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		LineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}

	for (uiI = 0; uiI < 30; uiI++)
	{
		double	dTime = 86500.0 * (uiI + 1);
		printf("------------------------------------\n Day %i, log Tau = %.2f:\n",uiI+1,log10(dTRef * dTRef * dTRef / (dTime * dTime)) );
		BroadLineData(cVelocity,cCa,cDens,cCaH + cCaK,dTemp,dTime,dTRef,"Ca II H&K");
		BroadLineData(cVelocity,cCa,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTemp,dTime,dTRef,"Ca II IR Triplet");
		BroadLineData(cVelocity,cSi,cDens,cSiII6355_1 + cSiII6355_2,dTemp,dTime,dTRef,"Si II 6355");
		BroadLineData(cVelocity,cSi,cDens,cSiII5463,dTemp,dTime,dTRef,"Si II 5463");
		BroadLineData(cVelocity,cSi,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,-dTemp,dTime,dTRef,"Si III 4560");
		BroadLineData(cVelocity,cNi,cDens,cFeII4925,dTemp,dTime,dTRef,"Fe II 4925");
		BroadLineData(cVelocity,cNi,cDens,cFeII5018,dTemp,dTime,dTRef,"Fe II 5018");
		BroadLineData(cVelocity,cNi,cDens,cFeII5169,dTemp,dTime,dTRef,"Fe II 5169");
	}
*/
//	FILE * file
	BroadLineDataCSV(cVelocity, cCa, cHe3, cHyd, cDens, cCaH + cCaK, dTRef, "Ca H&K", dTemps, uiNumTemps, "CaHKLines.csv", "CaHKSpectra.csv", "CaHKtrx.csv");
	BroadLineDataCSV(cVelocity,cCa,cHe3,cHyd,cDens,cCaIIIR1 + cCaIIIR2 + cCaIIIR3,dTRef,"Ca IR Triplet",dTemps,uiNumTemps,"CaIRLines.csv","CaIRSpectra.csv","CaIRtrx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiII6355_1 + cSiII6355_2,dTRef,"Si II 6355",dTemps,uiNumTemps,"Si6355Lines.csv","Si6355Spectra.csv","Si6355trx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiII5463,dTRef,"Si II 5463",dTemps,uiNumTemps,"Si5463Lines.csv","Si5463Spectra.csv","Si5463trx.csv");
	BroadLineDataCSV(cVelocity,cSi,cHe3,cHyd,cDens,cSiIII4560_1 + cSiIII4560_2 + cSiIII4560_3,dTRef,"Si III 4560",dTemps,uiNumTemps,"Si4560Lines.csv","Si4560Spectra.csv","Si4560trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII4925,dTRef,"Fe II 4925",dTemps,uiNumTemps,"Fe4925Lines.csv","Fe4925Spectra.csv","Fe4925trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII5018,dTRef,"Fe II 5018",dTemps,uiNumTemps,"Fe5018Lines.csv","Fe5018Spectra.csv","Fe5018trx.csv");
	BroadLineDataCSV(cVelocity,cNi,cHe3,cHyd,cDens,cFeII5169,dTRef,"Fe II 5169",dTemps,uiNumTemps,"Fe5169Lines.csv","Fe5169Spectra.csv","Fe5169trx.csv");

}
