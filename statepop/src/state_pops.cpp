#include<cstdio>
#include<cstdlib>
#include <statepop.h>





int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// inputs
	//
	////////////////////////////////////////////////////////////////////////////////////////

	statepop::floattype	dN = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--n", -1.0);
	statepop::floattype	dLog_N = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--log-n", -9999.0);
	statepop::floattype	dNe = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ne", -1.0);
	statepop::floattype	dLog_Ne = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--log-ne", -9999.0);
	statepop::floattype	dRadiation_Temperature_K = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--rad-temp", 10000.0);
	statepop::floattype	dPhotosphere_Velocity_km_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--ps-vel", 10000.0);
	unsigned int uiElement_Z = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--elem", -1);//20);
	unsigned int uiElement_Max_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--max-ion", -1);//4);
	unsigned int uiElement_Min_Ion_Species = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--min-ion", -1);//4);
	unsigned int uiIon_Species_Only = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--only-ion", -1);//4);
	size_t tMax_States = xParse_Command_Line_UInt(i_iArg_Count, i_lpszArg_Values, "--max-states", -1);//4);

	statepop::floattype	dMaterial_Velocity_km_s = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--mat-vel", 25000.0);
	statepop::floattype	dElectron_Kinetic_Temperature_K = xParse_Command_Line_Dbl(i_iArg_Count, i_lpszArg_Values, "--e-temp", -1);

	if (i_iArg_Count == 1 ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"?") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"HELP") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"--help") ||
		xParse_Command_Line_Exists(i_iArg_Count, i_lpszArg_Values,"--help") ||
		(dN == -1.0 && dLog_Ne == -9999 && dNe == -1.0 && dLog_Ne == -9999) ||
		(dLog_N != -9999 && dN != -1) ||
		(dLog_Ne != -9999 && dNe != -1) ||
		uiElement_Z == -1 ||
		(uiElement_Max_Ion_Species != -1 && uiElement_Max_Ion_Species > uiElement_Z)
		)
	{
		std::cout << i_lpszArg_Values[0] << ": Compute the relative populations for all known electron configurations of a given element." << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << i_lpszArg_Values[0] << " <options>" << std::endl;
		std::cout << "Options / Parameters:" << std::endl;
		std::cout << "--ne=<float> : assumed electron density to use for recombination component, in cm^{-3}. Mutually exclusive with --log-ne." << std::endl;
		std::cout << "--log-ne=<float> : assumed electron density to use for recombination component, in cm^{-3}. Mutually exclusive with --ne." << std::endl;
		std::cout << "--n=<float> : assumed total ion density, in cm^{-3}. Mutually exclusive with --log-n." << std::endl;
		std::cout << "--log-n=<float> : assumed total ion density, in cm^{-3}. Mutually exclusive with --n. " << std::endl;
		std::cout << "\t If only one of --ne, --n, --log-ne, --log-n is specified, the value will be used for both ion and electron density. " << std::endl;
		std::cout << "--rad-temp=<float> : for blackbody radiation field, the blackbody temperature in K." << std::endl << "\tDefault is 10,000 K" << std::endl;
		std::cout << "--e-temp=<float> : Kinetic temperature to use for electron velocity field." << std::endl << "\tDefault value is the photosphere (radiation) temperature." << std::endl;
		std::cout << "--ps-vel=<float> : Velocity of photosphere in km/s." << std::endl << "    Default = 10,000 km/s" << std::endl;
		std::cout << "--mat-vel=<float> : Velocity of material of interest in km/s." << std::endl << "    Default is 25,000 km/s" << std::endl;
		std::cout << "--elem=<integer> : Atomic number of element of interest (e.g. Hydrogen = 1)." << std::endl;
		std::cout << "--max-ion=<integer> : Maximum ionization state to consider; 0 is neutral," << std::endl << "\tmaximum value is atomic number of element." << std::endl << "    Default is the maximum value." << std::endl;
		std::cout << "--min-ion=<integer> : Minimum ionization state to consider; 0 is neutral," << std::endl << "    maximum value is atomic number of element." << std::endl << "    Default is 0." << std::endl;
		std::cout << "--only-ion=<integer> : Single ionization state to consider; 0 is neutral," << std::endl << "    maximum value is atomic number of element." << std::endl << "    Equivalent to setting --max-ion and --min-ion to the same value." << std::endl;

		if (dLog_N != -9999 && dN != -1)
		{
			std::cerr << "--log-n and --n were both specified in the call. These are mutually exclusive." << std::endl;
		}
		if (dLog_Ne != -9999 && dNe != -1)
		{
			std::cerr << "--log-ne and --ne were both specified in the call. These are mutually exclusive." << std::endl;
		}
		return 1;
	}


	if (dLog_N != -9999)
		dN = pow(10.0,dLog_N);
	if (dLog_Ne != -9999)
		dNe = pow(10.0,dLog_Ne);
	if (dN == -1 && dNe != -1)
	{
		std::cout << "Using electron density for ion density." << std::endl;
		dN = dNe;
	}
	if (dNe == -1 && dN != -1)
	{
		std::cout << "Using ion density for electro density." << std::endl;
		dNe = dN;
	}
	if (uiElement_Max_Ion_Species != -1 && uiIon_Species_Only != -1)
	{
		std::cerr << "--only-ion and --max-ion both specified. --max-ion ignored." << std::endl;
	}
	else if (uiElement_Min_Ion_Species != -1 && uiIon_Species_Only != -1)
	{
		std::cerr << "--only-ion and --min-ion both specified. --min-ion ignored." << std::endl;
	}


	if (uiIon_Species_Only != -1)
	{
		uiElement_Min_Ion_Species = uiIon_Species_Only;
		uiElement_Max_Ion_Species = uiIon_Species_Only;
	}
	else
	{
		if (uiElement_Min_Ion_Species == -1)
			uiElement_Min_Ion_Species = 0;
		if (uiElement_Max_Ion_Species == -1)
			uiElement_Max_Ion_Species = uiElement_Z;
	}


	////////////////////////////////////////////////////////////////////////////
	//
	// Output run parameters
	//
	////////////////////////////////////////////////////////////////////////////
	
	FILE * fileParameters = fopen("parameters.txt","wt");
	if (fileParameters != nullptr)
	{
		fprintf(fileParameters,"log Electron density: %.3Lf [cm^{-3}]\n",log10(dNe));
		fprintf(fileParameters,"log Ion density: %.3Lf [cm^{-3}]\n",log10(dN));
		fprintf(fileParameters,"Radiation temperature: %.0Lf K\n",dRadiation_Temperature_K);
		fprintf(fileParameters,"Electron temperature: %.0Lf K\n",dElectron_Kinetic_Temperature_K);
		fprintf(fileParameters,"Photosphere velocity: %.0Lf km / sec\n",dPhotosphere_Velocity_km_s);
		fprintf(fileParameters,"Material velocity: %.0Lf km / sec\n",dMaterial_Velocity_km_s);
		fprintf(fileParameters,"Element: Z = %i\n",uiElement_Z);
		if (uiElement_Min_Ion_Species == uiElement_Max_Ion_Species)
			fprintf(fileParameters,"Ion: %i\n",uiElement_Max_Ion_Species);
		else
			fprintf(fileParameters,"Ions: %i -- %i\n",uiElement_Min_Ion_Species, uiElement_Max_Ion_Species);

		fclose(fileParameters);
	}

	////////////////////////////////////////////////////////////////////////////
	//
	// Initialize statepop
	//

	statepop cStatepop;
	statepop::param	cParam;

	cParam.dN = dN;
	cParam.dNe = dNe;
	cParam.dRadiation_Temperature_K = dRadiation_Temperature_K;
	cParam.dPhotosphere_Velocity_km_s = dPhotosphere_Velocity_km_s;
	cParam.dMaterial_Velocity_km_s = dMaterial_Velocity_km_s;
	cParam.dElectron_Kinetic_Temperature_K = dElectron_Kinetic_Temperature_K;
	cParam.uiElement_Z = uiElement_Z;
	cParam.uiElement_Max_Ion_Species = uiElement_Max_Ion_Species;
	cParam.uiElement_Min_Ion_Species = uiElement_Min_Ion_Species;
	cParam.tMax_States = tMax_States;

	cStatepop.Set_Param(cParam);

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output list of transitions to file trx.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////

	FILE * fileTrx = fopen("trx.csv","wt");
	if (fileTrx)
	{
		fprintf(fileTrx,"Trx ID, Element code, Level (Lower) ID, Level (lower), J (lower), Level (Upper) ID, Level (upper), J (upper), Wavelength [A], A, B, B(SE), Gamma (rad), Hab (%.1f kK), Hem (%.1f kK), Z\n",dRadiation_Temperature_K*1e-3,dRadiation_Temperature_K*1e-3);
		size_t tNum_Trx = cStatepop.Get_Num_Transitions();
		for (size_t tI = 0; tI < tNum_Trx; tI++)
		{
			statepop::transition_data cTrx = cStatepop.Get_Transition(tI);
			fprintf(fileTrx,"%i, %.2Lf, %i, %s, %.1Lf, %i, %s, %.1Lf, %.3Lf, %.3Le, %.3Le, %.3Le, %.3Le, %.3Le, %.3Le, %.3Le\n",
						cTrx.tID,
						cTrx.ldElement_Code,
						cTrx.tLower_Level_ID,
						cTrx.szLower_Level_State.c_str(),
						cTrx.ldLower_Level_J,
						cTrx.tUpper_Level_ID,
						cTrx.szUpper_Level_State.c_str(),
						cTrx.ldUpper_Level_J,
						cTrx.ldWavenegth_Angstroms,
						cTrx.ldEinstein_A,
						cTrx.ldEinstein_B,
						cTrx.ldEinstein_B_Spontaneous_Emission,
						cTrx.ldGamma,
						cTrx.ldH_absorption,
						cTrx.ldH_emission,
						cTrx.ldZ);
				
		}
		fclose(fileTrx);
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output list of levels to file levels.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////

	FILE * fileLevels = fopen("levels.csv","wt");
	if (fileLevels)
	{
		fprintf(fileLevels,"ID, Element code, Level , J, Energy (Ryd), Gamma (s^-1), Z (s^-1), Absorbtion lines, Emission Lines, OP Project State\n");
		size_t tNum_Level = cStatepop.Get_Num_Levels();
		for (size_t tI = 0; tI < tNum_Level; tI++)
		{
			statepop::level_data cLevel = cStatepop.Get_Level(tI);
			fprintf(fileLevels,"%i, %.2Lf, %s, %.1Lf, %.5Lf, %.3Le, %.3Le, %i, %i, ",
				cLevel.tID,
				cLevel.ldElement_Code,
				cLevel.szLabel.c_str(),
				cLevel.ldJ,
				cLevel.ldEnergy_Level_Ryd,
				cLevel.ldGamma,
				cLevel.ldZ,
				cLevel.tNumber_of_Absorption_Transitions,
				cLevel.tNumber_of_Emission_Transitions);
			if (cLevel.tOP_Project_Level_Correlation[0] != -1)
			{
				fprintf(fileLevels,"%i%i%i %i",
					cLevel.tOP_Project_Level_Correlation[0],
					cLevel.tOP_Project_Level_Correlation[1],
					cLevel.tOP_Project_Level_Correlation[2],
					cLevel.tOP_Project_Level_Correlation[3]);
			}
			fprintf(fileLevels,"\n");
		}
		fclose(fileLevels);
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output matrix to matrixa.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////
	FILE * fileMatA = fopen("matrixa.csv","wt");
	statepop::matrix smB = cStatepop.Get_Matrix_BZ();

	if (fileMatA != nullptr)
	{
		size_t tIdx = 0;
		for (size_t tIdx_I = 0; tIdx_I < smB.size(); tIdx_I++) // row
		{
			for (size_t tIdx_J = 0; tIdx_J < smB.size(); tIdx_J++)// column
			{
				if (tIdx_J > 0)
					fprintf(fileMatA,", ");
				fprintf(fileMatA,"%.20Le",smB.Get(tIdx_I,tIdx_J));
			}
			fprintf(fileMatA,"\n");
		}
		fclose(fileMatA);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output matrix to matrixa.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////
	FILE * fileMatB = fopen("matrixb.csv","wt");
	smB = cStatepop.Get_Matrix_B();

	if (fileMatB != nullptr)
	{
		size_t tIdx = 0;
		for (size_t tIdx_I = 0; tIdx_I < smB.size(); tIdx_I++) // row
		{
			for (size_t tIdx_J = 0; tIdx_J < smB.size(); tIdx_J++)// column
			{
				if (tIdx_J > 0)
					fprintf(fileMatB,", ");
				fprintf(fileMatB,"%.20Le",smB.Get(tIdx_I,tIdx_J));
			}
			fprintf(fileMatB,"\n");
		}
		fclose(fileMatB);
	}

	//xvector_long vX = smB.Get_Eigenvector(1.0);
	//printf("spX\n");
	//vX.Print();
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Get state populations
	//
	////////////////////////////////////////////////////////////////////////////////////////
	statepop::vector vEig = cStatepop.Get_Populations();

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Write population data to eigv.csv
	//
	////////////////////////////////////////////////////////////////////////////////////////
	FILE * fileOut = fopen("eigv.csv","wt");
	if (fileOut != nullptr)
	{
		fprintf(fileOut,"i, Element Code, State, J, Energy Level (Ryd), Relative Population\n");
 		// write out the normalized best eigenvector to eigv.csv, with state information
		unsigned int uiCount = 0;
		for (size_t tI = 0; tI < vEig.size(); tI++)
		{
			statepop::level_data cLevel = cStatepop.Get_Level(tI);
			std::cout << "v[" << tI << "] = " << vEig[tI] << std::endl;
			std::ostringstream ssEig;
			ssEig << std::scientific << std::setprecision(std::numeric_limits<statepop::floattype>::digits10) << vEig[tI];
			fprintf(fileOut,"%i, %.2Lf, %s, %.1Lf, %.5Lf, %.17Le\n",
					tI,
					cLevel.ldElement_Code,
					cLevel.szLabel.c_str(),
					cLevel.ldJ,
					cLevel.ldEnergy_Level_Ryd,
					vEig[tI]);
		}
		fclose(fileOut);
		std::cout << "Eigenvalue has been output to eigv.csv" << std::endl;
	}

	statepop::vector vLines = cStatepop.Get_Relative_Line_Strengths();
	//statepop::vector vB_Lines = cStatepop.Get_Boltzmann_Relative_Line_Strengths();
	FILE * fileLines = fopen("lines.csv","wt");
	if (fileLines != nullptr)
	{
		fprintf(fileLines,"Index, Element Code, Wavelength [A], Lower State, Lower State J, Upper State, Upper State J, Relative Strength, Relative Strength (Boltzmann)\n");
 		// write out the normalized best eigenvector to eigv.csv, with state information
		unsigned int uiCount = 0;
		for (size_t tI = 0; tI < vLines.size(); tI++)
		{
			statepop::transition_data cTrx = cStatepop.Get_Transition(tI);
			fprintf(fileLines,"%i, %.2Lf, %.3Lf, %s, %.1Lf, %s, %.1Lf, %.23Le\n",//, %.23Le\n",
					tI,
					cTrx.ldElement_Code,
					cTrx.ldWavenegth_Angstroms,
					cTrx.szLower_Level_State.c_str(),
					cTrx.ldLower_Level_J,
					cTrx.szUpper_Level_State.c_str(),
					cTrx.ldUpper_Level_J,
					vLines[tI]);
		//			vB_Lines[tI]);
		}
		fclose(fileOut);
		std::cout << "Line strengths have been output to lines.csv" << std::endl;
	}


}

