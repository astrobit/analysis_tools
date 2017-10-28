#include <xio.h>
#include <xstdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <xastro.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
//#include <line_anal.h>

int main(int iArgC, const char * lpszArgV[])
{
	XDATASET	cSi;
	XDATASET	cCa;
	FILE * fileOut;
	char	lpszFilename[256];
	unsigned int uiDmin,uiDmax,uiDstep,uiDay;
	unsigned int uiTmin,uiTmax,uiTstep,uiTemp;
	unsigned int uiAmin,uiAmax,uiAstep,uiAlpha;
	unsigned int uiI,uiImin,uiNum_Elem;
	double dMin, dCa_HK_Min, dSi_6355_Min, dCa_IR_Min;
	
	uiDmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_start",2);
	uiDmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_end",25);
	uiDstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-day_step",3);
	uiTmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_start",8000);
	uiTmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_end",14000);
	uiTstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-temp_step",1000);
	uiAmin = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_start",0);
	uiAmax = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_end",0);
	uiAstep = xParse_Command_Line_UInt(iArgC,lpszArgV,"-alpha_step",1);

	for (uiTemp = uiTmin; uiTemp <= uiTmax; uiTemp += uiTstep)
	{
		for (uiAlpha = uiAmin; uiAlpha <= uiAmax; uiAlpha += uiAstep)
		{
			sprintf(lpszFilename,"velprof/run_T%i_a%i.csv",uiTemp,uiAlpha);
			fileOut = fopen(lpszFilename,"wt");
			fprintf(fileOut,"Day, Ca H&K [A], Si 6355 [A], Ca NIR [A], Ca H&K [v], Si 6355 [v], Ca NIR [v]\n");

			for (uiDay = uiDmin; uiDay <= uiDmax; uiDay += uiDstep)
			{
				sprintf(lpszFilename,"spectra/run_d%i_T%i_a%i_Ca.csv",uiDay,uiTemp,uiAlpha);
				cCa.ReadDataFile(lpszFilename, true, false);
//				printf("Read Ca file (%i,%i,%i)\n",uiDay,uiTemp,uiAlpha);
				sprintf(lpszFilename,"spectra/run_d%i_T%i_a%i_Si.csv",uiDay,uiTemp,uiAlpha);
				cSi.ReadDataFile(lpszFilename, true, false);
//				printf("Read Si file (%i,%i,%i)\n",uiDay,uiTemp,uiAlpha);

				uiNum_Elem = cCa.GetNumElements();
				if (uiNum_Elem != cSi.GetNumElements())
				{
					printf("Error - different numbers of data points.\n");
					return -1;
				}
				
				uiI = 0;
				while (cCa.GetElement(0,uiI) < 3400.0 && uiI < uiNum_Elem)
					uiI++;
				uiImin = 0;
				dMin = 1.0e15;
				while (cCa.GetElement(0,uiI) < 4100.0 && uiI < uiNum_Elem)
				{
					if (cCa.GetElement(1,uiI) < dMin)
					{
						dMin = cCa.GetElement(1,uiI);
						uiImin = uiI;
					}
					uiI++;
				}
				dCa_HK_Min = cCa.GetElement(0,uiImin);

				while (cSi.GetElement(0,uiI) < 5700.0 && uiI < uiNum_Elem)
					uiI++;
				uiImin = 0;
				dMin = 1.0e15;
				while (cSi.GetElement(0,uiI) < 6300.0 && uiI < uiNum_Elem)
				{
					if (cSi.GetElement(1,uiI) < dMin)
					{
						dMin = cSi.GetElement(1,uiI);
						uiImin = uiI;
					}
					uiI++;
				}
				dSi_6355_Min = cCa.GetElement(0,uiImin);



				while (cCa.GetElement(0,uiI) < 7900.0 && uiI < uiNum_Elem)
					uiI++;
				uiImin = 0;
				dMin = 1.0e15;
				while (cCa.GetElement(0,uiI) < 8300.0 && uiI < uiNum_Elem)
				{
					if (cCa.GetElement(1,uiI) < dMin)
					{
						dMin = cCa.GetElement(1,uiI);
						uiImin = uiI;
					}
					uiI++;
				}
				dCa_IR_Min = cCa.GetElement(0,uiImin);
				double	dCa_HK_Ref = (3933.66 + 3968.47) * 0.5;
				double	dCa_NIR_Ref = (8498.023 + 8542.091 + 8662.141) / 3.0;
				double	dSi_6355_Ref = (6348.864 + 6373.132) * 0.5;
				printf("(%i,%i,%i): %.1f, %.1f, %.1f\n",uiDay,uiTemp,uiAlpha,dCa_HK_Min,dSi_6355_Min,dCa_IR_Min);
				fprintf(fileOut,"%i, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", uiDay, dCa_HK_Min, dSi_6355_Min, dCa_IR_Min, dCa_HK_Min / dCa_HK_Ref * 3.0e2, dSi_6355_Min / dSi_6355_Ref * 3.0e2, dCa_IR_Min / dCa_NIR_Ref * 3.0e2);
			}
			fclose(fileOut);
		}
	}
}
