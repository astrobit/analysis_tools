#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xstdlib.h>
#include <xio.h>
#include <xmath.h>
#include <xastro.h>
#include <xflash.h>
#include <math.h>

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
	unsigned int uiIon = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values,"--ion");
	double		dEjecta_Scaling = pow(10.0,xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values,"--ejecta",nan("")));
	double		dShell_Scaling = pow(10.0,xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values,"--shell",nan("")));
	char lpszModel[128] = {""};
	double	dScalar_Factors[5];
	char	lpszBuffer[1024];
	char lpszScalar_Datafile[64];
	double	dIon_Scalar;

	xParse_Command_Line_String(i_iArg_Count, i_lpszArg_Values,"--model", lpszModel, sizeof(lpszModel));

	if (!isnan(dEjecta_Scaling) && uiIon > 0 && uiIon < 11699 && lpszModel[0] != 0)
	{
		double	dConst_Terms = g_XASTRO.k_dme * g_XASTRO.k_dc / (g_cConstants.dPi * g_XASTRO.k_de * g_XASTRO.k_de);

		XDATASET	cRef_Lines;

		cRef_Lines.ReadDataFile(getenv("SYNXX_REF_LINE_DATA_PATH"),true,0,'\"',0);

	//	printf("Read ref lines\n");
		// Find referense line for the element
		unsigned int uiIon_Ref_Idx = 0;
		while (uiIon_Ref_Idx < cRef_Lines.GetNumElements() &&
				cRef_Lines.GetElement(0,uiIon_Ref_Idx) != uiIon)
			uiIon_Ref_Idx++;

		if (uiIon_Ref_Idx >= cRef_Lines.GetNumElements())
		{
			printf("Ion %i not found in reference list.\n",uiIon);
			exit(1);
		}
	//	else
	//		printf("Ion %i found at %i of %i\n",uiIon,uiIon_Ref_Idx,cRef_Lines.GetNumElements());
		double dLambda = cRef_Lines.GetElement(1,uiIon_Ref_Idx);
		double dOscillator_Strength = cRef_Lines.GetElement(2,uiIon_Ref_Idx);
		double	dElement_Mass = (uiIon / 100) * g_XASTRO.k_dAMU_gm;

		sprintf(lpszScalar_Datafile,"%s/opacity_map_scalars.txt",lpszModel);
		FILE * fileScalars = fopen(lpszScalar_Datafile,"rt");
		double dTref;
		if (fileScalars)
		{
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
	//		printf("Processing %s\n",lpszBuffer);
			dTref = xGetNumberFromEndOfString(lpszBuffer);
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[0] = xGetNumberFromEndOfString(lpszBuffer); // C
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[1] = xGetNumberFromEndOfString(lpszBuffer); // O
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[2] = xGetNumberFromEndOfString(lpszBuffer); // Mg
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[3] = xGetNumberFromEndOfString(lpszBuffer); // Si
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[4] = xGetNumberFromEndOfString(lpszBuffer); // Fe
			fgets(lpszBuffer,sizeof(lpszBuffer),fileScalars);
			dScalar_Factors[5] = xGetNumberFromEndOfString(lpszBuffer); // Shell
			fclose(fileScalars);
	//		printf("Read scalars\n");
		}
		else
		{
			printf("Unable to open %s.\n",lpszScalar_Datafile);
			exit(1);
		}

		switch (uiIon / 100)
		{
		case 6:
			dIon_Scalar = dScalar_Factors[0];
			break;
		case 8:
			dIon_Scalar = dScalar_Factors[1];
			break;
		case 10:
		case 12:
			dIon_Scalar = dScalar_Factors[2];
			break;
		case 14: // Si
		case 16: // S
		case 18: // Ar
		case 20: // Ca
		case 22: // Ti 
		case 23: // V
		case 24: // Cr
		case 25: // Mn
			dIon_Scalar = dScalar_Factors[3];
			break;
		case 26:
		case 27:
		case 28:
			dIon_Scalar = dScalar_Factors[4];
			break;
		}

		double	dIonization_Fraction = dConst_Terms * dEjecta_Scaling * dElement_Mass / (dOscillator_Strength * dLambda * dTref * dTref * dTref * dIon_Scalar);
		double	dAbd_Shell = dShell_Scaling * dScalar_Factors[5] / (dEjecta_Scaling * dIon_Scalar);

		printf("Ionization fraction: %.2e\n",dIonization_Fraction);
		printf("Shell abundance: %.2e\n",dAbd_Shell);
	}
	else
	{
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"ionabddet --ion=X --ejecta=X [--shell=X] --model=SSSS\n");
		fprintf(stderr,"\t--ion : integer ion ID: atomic # * 100 + ion state.  E.g. Neutral carbon = 600\n\t\t C I = 601, C II = 602, etc.\n");
		fprintf(stderr,"\t--ejecta : log scaling factor for ejecta component, used to determine ionization fraction\n");
		fprintf(stderr,"\t--shell : log scaling factor for shell component, used to determine shell abundance\n");
		fprintf(stderr,"\t--model : model to use - scaling factors will be loaded from SSSS/opacity_map_scaling.txt\n");
	}
	return 0;
}
