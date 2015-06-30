#include <yaml-cpp/yaml.h>
#include <xstdlib.h>
#include <xastroline.h>
#include <xastroion.h>
#include <xmath.h>
#include <xio.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <wordexp.h>
#include <iostream>

class SYNOW_LINE_PROFILE
{
public:
	double	dVref_Mmps; // Reference velocity for exponential profile [1000 km/s]
	double	dVmin_Mmps; // minimum velocity of line generating region (LGR) [1000 km/s]
	double	dVmax_Mmps; // maximum velocity of LGR [1000 km/s]
	double	dTau_Ref; // Optical depth at reference velocity
	double	dVprof_Mmps; // characteristic velocity for an e-folding of tau [1000 km/s]
	double	dIon_Temperature_K; // temperature of the ion, for state populations
	unsigned int uiIon_ID; // NNii, where NN is atomic number and ii is ion state, ii = 0 is neutral, 1 is singly ionized, etc.
	bool	bActive;
	
	SYNOW_LINE_PROFILE(void)
	{
		dVref_Mmps =
		dVmin_Mmps =
		dVmax_Mmps =
		dTau_Ref =
		dVprof_Mmps =
		dIon_Temperature_K = 0.0;
		uiIon_ID = 0;
		bActive = false;
	}

	// Tau(v) = Tau_Ref * exp( (v - vref) / vprof)

	double	Tau(const double &dVelocity)
	{
		double dTau = 0.0;
		if (dVelocity >= dVmin_Mmps && dVelocity <= dVmax_Mmps && dVref_Mmps > 0.0 && dVprof_Mmps != 0.0 && dTau_Ref > 0.0)
		{
			dTau = dTau_Ref * exp (-(dVelocity - dVref_Mmps) / dVprof_Mmps);
		}
		return dTau;
	}
	double	Number_Density(const double &i_dVelocity, const double & i_dTime, const double & i_dTau_Const)
	{
		// Compute the number density based on the line optical depth arising from a localized region, using the Sobolev approximation
		double	dDensity = 0.0;
		double dTau = Tau(i_dVelocity);
		if (dTau > 0.0)
		{
			dDensity = dTau / (i_dTau_Const * i_dTime);
		}
		return dDensity;
	}
};

double	Mass(const double &dVelocity, const double & dDelta_Velocity, const double &i_dNumber_Density, const double &i_dTime, const double & i_dIon_Atomic_Weight)
{
	double dRinner = dVelocity * i_dTime;
	double	dRouter = dRinner + dDelta_Velocity * i_dTime;
	double	dShell_Volume = 4.0 / 3.0 * g_cConstants.dPi * (dRouter * dRouter * dRouter - dRinner * dRinner * dRinner);
	double dMass = dShell_Volume * i_dNumber_Density * g_XASTRO.k_dmh * i_dIon_Atomic_Weight;

	return dMass;
}


std::string Resolve_Path(const YAML::Node &i_cYaml, const std::string &i_lpszFirst, const std::string &i_lpszSecond, const std::string &i_szEnv_Variable)
{
	std::string cszTemp;
	std::string cszResult_File;

	try{cszTemp = i_cYaml[ i_lpszFirst ][ i_lpszSecond    ].as<std::string>();}
	catch(YAML::Exception& e){;} // we don't care if it isn't included in the file at this point

	if (cszTemp.empty())
	{
		char * lpszEnv_Variable = getenv(i_szEnv_Variable.c_str());
		if (lpszEnv_Variable)
			cszResult_File = lpszEnv_Variable;
	}
	else
	{
		size_t i = 0;
		while ((i = cszTemp.find_first_of ("\\", i)) != std::string::npos)
		{
			cszTemp.erase(i, 1);
		}
		wordexp_t cResults;
		if (wordexp(cszTemp.c_str(),&cResults,WRDE_NOCMD|WRDE_UNDEF) == 0)
		{
			cszResult_File = cResults.we_wordv[0];
			wordfree(&cResults);
		}
	}
    if( cszResult_File.empty() )
    {
        std::cerr << "syn++: Unable to resolve " + i_lpszSecond << std::endl;
        exit( 1 );
    }
	return cszResult_File;
}

std::string Get_Ion_ID_String(unsigned int i_uiIon_ID)
{
	std::string cszRet;
	char lpszBuffer[16];

	xGet_Element_Symbol(i_uiIon_ID / 100,lpszBuffer);

	cszRet = lpszBuffer;
	xRomanNumeralGenerator(lpszBuffer,i_uiIon_ID % 100 + 1);
	cszRet += " ";
	cszRet += lpszBuffer;
	return cszRet;
}

std::string Get_Element_ID_String(unsigned int i_uiIon_ID)
{
	std::string cszRet;
	char lpszBuffer[16];

	xGet_Element_Symbol(i_uiIon_ID / 100,lpszBuffer);
	cszRet = lpszBuffer;
	return cszRet;
}


int main(int i_uiArg_Count, const char * i_lpszArg_Values[])
{
	unsigned int 			uiI, uiJ;

	XDATASET	cRef_Lines;
	YAML::Node				cSyn_File;
	SYNOW_LINE_PROFILE		*lpcLine_Profiles;
	unsigned int			uiNum_Profiles;
	unsigned int 			uiNum_Ions;

	char					lpszSyn_File[256];
	char					lpszOutputPrefix[256];
	char					lpszFilename[256];

	XASTROLINE_ELEMENTDATA * lpcElement_Line_Data;
	bool					bElements[118];
	unsigned int			uiElem_Idx[118];
	unsigned int			uiNum_Elem = 0;
	double					dDelta_V;
	double					dTime_days;
	double					dTime_s;

//	bool					bScale_CaII_To_NIR; // flag to rescale the Ca II optical depths to assume a fit to the NIR rather than the H&K feature

	// Read command line parameters here
	
	xParse_Command_Line_String(i_uiArg_Count,i_lpszArg_Values,"--prof_data_file",lpszSyn_File,256,"");
	if (lpszSyn_File[0] == 0)
		xParse_Command_Line_String(i_uiArg_Count,i_lpszArg_Values,"--prof-data-file",lpszSyn_File,256,"");
	if (lpszSyn_File[0] == 0)
		xParse_Command_Line_String(i_uiArg_Count,i_lpszArg_Values,"--prof-data-file",lpszSyn_File,256,"");
	dDelta_V = xParse_Command_Line_Dbl(i_uiArg_Count,i_lpszArg_Values,"--delta-v",0.1);
	dTime_days = xParse_Command_Line_Dbl(i_uiArg_Count,i_lpszArg_Values,"--day",-1.0);
	xParse_Command_Line_String(i_uiArg_Count,i_lpszArg_Values,"--output-prefix",lpszOutputPrefix,256,"");
//	bScale_CaII_To_NIR = xParse_Command_Line_Bool(i_uiArg_Count,i_lpszArg_Values,"--use-CaII-NIR",false);
	if(dTime_days < 0.0)
	{
		printf("Error - day number not specified.  Exiting.\n");
		exit(1);
	}
	if (lpszSyn_File[0] == 0)
	{
		printf("Error - data file not specified.  Exiting.\n");
		exit(1);
	}

	dTime_s = dTime_days * 3600.0 * 24.0;

	printf("Opening %s\n",lpszSyn_File);
	cSyn_File = YAML::LoadFile(lpszSyn_File);


	// Parse syn++.yaml file
	std::string	csRef_File = Resolve_Path(cSyn_File,"opacity","ref_file","SYNXX_REF_LINE_DATA_PATH");
	cRef_Lines.ReadDataFile(csRef_File.c_str(),true,0,'\"',0);

	

    const YAML::Node & cSetups = cSyn_File["setups"];
    for( YAML::const_iterator iterI = cSetups.begin(); iterI != cSetups.end(); ++ iterI )
	{
	    const YAML::Node cIterNode = *iterI;
	    const YAML::Node cIons = cIterNode["ions"];
//	unsigned int uiIon = 0;//cSyn_File["setups"]["ions"][1].as<double>();
		uiNum_Ions = cIons.size();
		lpcLine_Profiles = new SYNOW_LINE_PROFILE[uiNum_Ions];
//        uiNum_Ions++;
//    for( YAML::Iterator iter = setups.begin(); iter != setups.end(); ++ iter )
		printf("%i ions specified.\n",uiNum_Ions);
		for (uiI = 0; uiI < uiNum_Ions; uiI++)
		{
			lpcLine_Profiles[uiI].uiIon_ID = cIterNode["ions"][uiI].as<unsigned int>();//
			lpcLine_Profiles[uiI].dVprof_Mmps = cIterNode["aux"][uiI].as<double>();//
			lpcLine_Profiles[uiI].dVmin_Mmps = cIterNode["v_min"][uiI].as<double>();//
			lpcLine_Profiles[uiI].dVmax_Mmps = cIterNode["v_max"][uiI].as<double>();//
			lpcLine_Profiles[uiI].dTau_Ref = pow(10.0,cIterNode["log_tau"][uiI].as<double>());//
			lpcLine_Profiles[uiI].dIon_Temperature_K = cIterNode["temp"][uiI].as<double>() * 1000.0;//
			lpcLine_Profiles[uiI].dVref_Mmps = cSyn_File["opacity"]["v_ref"].as<double>();//
			lpcLine_Profiles[uiI].bActive = cIterNode["active"][uiI].as<bool>();//

			printf("Ion: %i (%.1f-%.1f) T = %.1f",lpcLine_Profiles[uiI].uiIon_ID,lpcLine_Profiles[uiI].dVmin_Mmps,lpcLine_Profiles[uiI].dVmax_Mmps,lpcLine_Profiles[uiI].dIon_Temperature_K);
			if (!lpcLine_Profiles[uiI].bActive)
				printf("\t[Inactive]");
			printf("\n");
		}
	}

	printf("Vref = %.1f\n",lpcLine_Profiles[0].dVref_Mmps);
	// Identify which elements are included
	memset(uiElem_Idx, -1 , sizeof(uiElem_Idx));
	memset(bElements, 0 , sizeof(bElements));
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (lpcLine_Profiles[uiI].bActive)
		{
			unsigned int uiAtomic_Number = lpcLine_Profiles[uiI].uiIon_ID / 100;
			if (uiAtomic_Number < 118 && uiAtomic_Number > 0)
				bElements[uiAtomic_Number - 1] = true;
		}
	}
	for (uiI = 0; uiI < 118; uiI++)
	{
		if (bElements[uiI])
		{
			uiElem_Idx[uiI] = uiNum_Elem;
			uiNum_Elem++;
		}
	}

	lpcElement_Line_Data = new XASTROLINE_ELEMENTDATA[uiNum_Elem];
	// Read elemental line data

	for (uiI = 0; uiI < 116; uiI++)
	{
		if (uiElem_Idx[uiI] < 116)
		{
			char lpszAtomic_Symbol[4];
			xGet_Element_Symbol(uiI + 1,lpszAtomic_Symbol);
			lpcElement_Line_Data[uiElem_Idx[uiI]].ReadDataFile(NULL,lpszAtomic_Symbol);
		}
	}


	double	dVelocity;
	double	dTotal_Mass = 0.0;
	double	* dTotal_Ion_Masses = new double[uiNum_Ions];
	memset(dTotal_Ion_Masses,0,sizeof(double) * uiNum_Ions);
	double	* dTau_Const = new double[uiNum_Ions];
	memset(dTau_Const,0,sizeof(double) * uiNum_Ions);
	double	dTau;
	double	dMass;
	double	dNumber_Density;

	double	dVmin = 1.0e300;
	double	dVmax = 0.0;

	double	dTau_Max = 0.0;
	double	dEner_Tau_Max;
	double	dRef_Lambda;
	double	dRef_Oscillator_Strength;

	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (lpcLine_Profiles[uiI].bActive)
		{
			printf("%i: %i\n",uiI,lpcLine_Profiles[uiI].uiIon_ID);
			if (lpcLine_Profiles[uiI].dVmin_Mmps < dVmin)
				dVmin = lpcLine_Profiles[uiI].dVmin_Mmps;

			if (lpcLine_Profiles[uiI].dVmax_Mmps > dVmax)
				dVmax = lpcLine_Profiles[uiI].dVmax_Mmps;

			unsigned int uiElement = lpcLine_Profiles[uiI].uiIon_ID / 100;
			unsigned int uiIon = lpcLine_Profiles[uiI].uiIon_ID % 100;
			XASTROLINE_ELEMENTDATA * lpcElement_Lines = &lpcElement_Line_Data[uiElem_Idx[uiElement - 1]];
			if (lpcElement_Lines)
			{
				XASTROLINE_IONDATA * lpcIon_Data = &lpcElement_Lines->lpcIonData[uiIon];
				if (lpcIon_Data)
				{
					// Find referense line for the element
					unsigned int uiIon_Ref_Idx = 0;
					while (uiIon_Ref_Idx < cRef_Lines.GetNumElements() &&
							cRef_Lines.GetElement(0,uiIon_Ref_Idx) != lpcLine_Profiles[uiI].uiIon_ID)
						uiIon_Ref_Idx++;

					if (uiIon_Ref_Idx >= cRef_Lines.GetNumElements())
					{
						printf("Ion %i not found in reference list.\n",lpcLine_Profiles[uiI]);
						exit(1);
					}
					double dRef_Line_Wavelength = cRef_Lines.GetElement(1,uiIon_Ref_Idx);
					double dRef_Line_Oscillator_Strength = cRef_Lines.GetElement(2,uiIon_Ref_Idx);
/*					double	dVmin_Effect =  (1 - lpcLine_Profiles[uiI].dVmin_Mmps / 300.0);
					double	dVmax_Effect =  (1 - lpcLine_Profiles[uiI].dVmax_Mmps / 300.0);
					double	dMax_Wavelength_For_Effect = dRef_Line_Wavelength * dVmin_Effect;
					double	dMin_Wavelength_For_Effect = dRef_Line_Wavelength * dVmax_Effect;
					double	dDelta_Wavelength = (dMax_Wavelength_For_Effect - dMin_Wavelength_For_Effect);
					double	dDelta_Range = 0.05 * dDelta_Wavelength;
					dTau_Const[uiI] = 0.0;
					printf("%.2f %.2f\n",dMax_Wavelength_For_Effect,dMin_Wavelength_For_Effect);
//@@DEBUG			printf("Ref for %i: %.7f\n",lpcLine_Profiles[uiI].uiIon_ID,dRef_Line_Wavelength);
					uiJ = 0;
					if (lpcIon_Data->uiNum_Lines > 0)
					{
						while (uiJ < lpcIon_Data->uiNum_Lines &&
							!(((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMax_Wavelength_For_Effect) < dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMin_Wavelength_For_Effect) > 0.0) ||
							 ((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMin_Wavelength_For_Effect) > -dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMax_Wavelength_For_Effect) < 0.0)))

						{
	//@@DEBUG				printf("%.3f failed\n",lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A);
							uiJ++;
						}
						printf("Lines available: %i\n",lpcIon_Data->uiNum_Lines);
						printf("Starting at %i for %i (%.2f)\n",uiJ,lpcLine_Profiles[uiI].uiIon_ID,lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A);
						while (uiJ < lpcIon_Data->uiNum_Lines &&
							(((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMax_Wavelength_For_Effect) < dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMin_Wavelength_For_Effect) > 0.0) ||
							 ((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMin_Wavelength_For_Effect) > -dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMax_Wavelength_For_Effect) < 0.0)))
						{
							double dTau_Const_Sobolev = lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff();
							if (lpcIon_Data->lpcLine_Data[uiJ].cUpper_State.uiStatistical_Weight > 0 &&
								lpcIon_Data->lpcLine_Data[uiJ].cLower_State.uiStatistical_Weight > 0)
							{
								printf("Stat weights for %.1f A: %i(u) %i(l)\n",lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A, lpcIon_Data->lpcLine_Data[uiJ].cUpper_State.uiStatistical_Weight, lpcIon_Data->lpcLine_Data[uiJ].cLower_State.uiStatistical_Weight);
							}

							double dLower_State_Energy = lpcIon_Data->lpcLine_Data[uiJ].cLower_State.dEnergy_eV;
							dTau_Const[uiI] += dTau_Const_Sobolev * exp((dLower_State_Energy / (g_XASTRO.k_dKb_eV * lpcLine_Profiles[uiI].dIon_Temperature_K)));
							uiJ++;
						}
						printf("Ended at %i for %i (%.2f)\n",uiJ,lpcLine_Profiles[uiI].uiIon_ID,lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A);
					}
					printf("Line ref: %i : %.1f : %.2e\n",lpcLine_Profiles[uiI].uiIon_ID,dRef_Line_Wavelength,dTau_Const[uiI]);
*/					
					if (uiJ < lpcIon_Data->uiNum_Lines)
					{
//						printf("Line ref: %i : %.7f : %.3f\n",lpcLine_Profiles[uiI].uiIon_ID,dRef_Line_Wavelength,lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A);
//						printf("%.2e\n",lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff());
						double dTau_Const_Sobolev = lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff();
						double dLower_State_Energy = lpcIon_Data->lpcLine_Data[uiJ].cLower_State.dEnergy_eV;
						dTau_Const[uiI] = dTau_Const_Sobolev * exp((dLower_State_Energy / (g_XASTRO.k_dKb_eV * lpcLine_Profiles[uiI].dIon_Temperature_K)));
					}
					else
					{
						//determine optical depth constant for the ion
						dTau_Max = 0.0; // reset maximum constant
						for (unsigned int uiJ = 0; uiJ < lpcIon_Data->uiNum_Lines; uiJ++)
						{
							if (lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff() > dTau_Max)
							{
								dTau_Max = lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff();
								dEner_Tau_Max = lpcIon_Data->lpcLine_Data[uiJ].cLower_State.dEnergy_eV;
								dRef_Lambda = lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A;
								dRef_Oscillator_Strength = lpcIon_Data->lpcLine_Data[uiJ].dOscillator_Strength;
							}
						}
						dTau_Const[uiI] = dTau_Max * exp((dEner_Tau_Max / (g_XASTRO.k_dKb_eV * lpcLine_Profiles[uiI].dIon_Temperature_K)));
						// rescale from reference oscillator strength to our oscillator strength
						dTau_Const[uiI] *= ((dRef_Line_Wavelength * dRef_Line_Oscillator_Strength) / (dRef_Oscillator_Strength * dRef_Lambda));
					}
//					printf("For Ion %i: Using %.2e (l = %.1f Angstrom) for Tau Const\n",lpcLine_Profiles[uiI].uiIon_ID,dTau_Const[uiI],dRef_Lambda);*/
				}
				else
				{
					printf("Unable to find ion line data for ion ID %i\n",lpcLine_Profiles[uiI].uiIon_ID);
				}
			}
			else
			{
				printf("Unable to find element line data for ion ID %i\n",lpcLine_Profiles[uiI].uiIon_ID);
			}
		}
	}

	/*
	if (bScale_CaII_To_NIR)
	{
		if (
		unsigned int uiIon_Ref_Idx = 0;
		while (uiIon_Ref_Idx < cRef_Lines.GetNumElements() &&
				cRef_Lines.GetElement(0,uiIon_Ref_Idx) != 2001) // Ca II
			uiIon_Ref_Idx++;

		if (uiIon_Ref_Idx >= cRef_Lines.GetNumElements())
		{
			printf("Ca II not found in reference list but rescaling requested.\n",lpcLine_Profiles[uiI]);
			exit(1);
		}
		double dRef_Line_Wavelength = cRef_Lines.GetElement(1,uiIon_Ref_Idx);
		XASTROLINE_ELEMENTDATA * lpcElement_Lines = &lpcElement_Line_Data[uiElem_Idx[19]]; // Ca
		if (lpcElement_Lines)
		{
			XASTROLINE_IONDATA * lpcIon_Data = &lpcElement_Lines->lpcIonData[1]; // Ca II

			if (lpcIon_Data)
			{
				if (dRef_Line_Wavelength < 8300.0) // make sure it isn't already using something in the NIR
				{
					double dRef_Line_Wavelength = cRef_Lines.GetElement(1,uiIon_Ref_Idx);
					double	dVmin_Effect =  (1 - lpcLine_Profiles[uiI].dVmin_Mmps / 300.0);
					double	dVmax_Effect =  (1 - lpcLine_Profiles[uiI].dVmax_Mmps / 300.0);
					double	dMax_Wavelength_For_Effect = 8662.141 * dVmin_Effect; // Longest wavelength of NIR feature
					double	dMin_Wavelength_For_Effect = 8498.023 * dVmax_Effect; // shortest wavelength of NIR feature
					double	dDelta_Wavelength = (dMax_Wavelength_For_Effect - dMin_Wavelength_For_Effect);
					double	dDelta_Range = 0.05 * dDelta_Wavelength;
					double	dTau_Const_NIR = 0.0;
					uiJ = 0;
					if (lpcIon_Data->uiNum_Lines > 0)
					{
						while (uiJ < lpcIon_Data->uiNum_Lines &&
							!(((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMax_Wavelength_For_Effect) < dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMin_Wavelength_For_Effect) > 0.0) ||
							 ((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMin_Wavelength_For_Effect) > -dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMax_Wavelength_For_Effect) < 0.0)))

						{
		//@@DEBUG				printf("%.3f failed\n",lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A);
							uiJ++;
						}
						while (uiJ < lpcIon_Data->uiNum_Lines &&
							(((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMax_Wavelength_For_Effect) < dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmin_Effect - dMin_Wavelength_For_Effect) > 0.0) ||
							 ((lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMin_Wavelength_For_Effect) > -dDelta_Range &&
							  (lpcIon_Data->lpcLine_Data[uiJ].dWavelength_A * dVmax_Effect - dMax_Wavelength_For_Effect) < 0.0)))
						{
							double dTau_Const_Sobolev = lpcIon_Data->lpcLine_Data[uiJ].SobolevTauCoeff();

							double dLower_State_Energy = lpcIon_Data->lpcLine_Data[uiJ].cLower_State.dEnergy_eV;
							dTau_Const_NIR += dTau_Const_Sobolev * exp((dLower_State_Energy / (g_XASTRO.k_dKb_eV * lpcLine_Profiles[uiI].dIon_Temperature_K)));
							uiJ++;
						}
					}
				}
			}
		}
	}*/

//	printf("Beginning mass measurement (%.1f-%.1f)\n",dVmin,dVmax);
	FILE * fileOut;
	if (lpszOutputPrefix[0] != 0)
		sprintf(lpszFilename,"%s.reverse.d%.0f.csv",lpszOutputPrefix,dTime_days);
	else
		sprintf(lpszFilename,"reverse.d%.0f.csv",dTime_days);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"v");
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
		if (lpcLine_Profiles[uiI].bActive)
			fprintf(fileOut,",tau(%i), n(%i), m(%i)",lpcLine_Profiles[uiI].uiIon_ID,lpcLine_Profiles[uiI].uiIon_ID,lpcLine_Profiles[uiI].uiIon_ID);
	fprintf(fileOut,"\n");
	for (dVelocity = dVmin; dVelocity < dVmax; dVelocity += dDelta_V)
	{
		fprintf(fileOut,"%.2f",dVelocity);
		for (uiI = 0; uiI < uiNum_Ions; uiI++)
		{
			if (lpcLine_Profiles[uiI].bActive)
			{
				dNumber_Density = 0.0;
				dMass = 0.0;
				unsigned int uiElement = lpcLine_Profiles[uiI].uiIon_ID / 100;
				XASTROLINE_ELEMENTDATA * lpcElement_Lines = &lpcElement_Line_Data[uiElem_Idx[uiElement - 1]];
				if (lpcElement_Lines && dTau_Const[uiI] > 0.0)
				{
					dTau = lpcLine_Profiles[uiI].Tau(dVelocity);
					dNumber_Density = lpcLine_Profiles[uiI].Number_Density(dVelocity,dTime_s, dTau_Const[uiI]);
					dMass = Mass(dVelocity * 1.0e8, dDelta_V * 1.0e8, dNumber_Density,dTime_s,lpcElement_Lines->uiA);
					dTotal_Mass += dMass;
					dTotal_Ion_Masses[uiI] += dMass;
				}
//				else
//				{
//					printf("Unable to find element line data for ion ID %i\n",lpcLine_Profiles[uiI].uiIon_ID);
//				}
				fprintf(fileOut,",%.17e, %.17e, %.17e",dTau,dNumber_Density,dMass);
			}
		}
		fprintf(fileOut,"\n");
	}
	fclose(fileOut);



	double	* dElement_Mass = new double[uiNum_Ions];
	memset(dElement_Mass,0,sizeof(double) * uiNum_Ions);

	printf("Masses by component:\n");
	if (lpszOutputPrefix[0] != 0)
		sprintf(lpszFilename,"%s.mass.comp.d%.0f.csv",lpszOutputPrefix,dTime_days);
	else
		sprintf(lpszFilename,"mass.comp.d%.0f.csv",dTime_days);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"Ion, Mass [Msun]\n");
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (lpcLine_Profiles[uiI].bActive)
		{
			dElement_Mass[uiI] += dTotal_Ion_Masses[uiI];
		}
	}
			
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (dElement_Mass[uiI] > 0.0)
		{
			std::string cszIon = Get_Ion_ID_String(lpcLine_Profiles[uiI].uiIon_ID);
			printf("Total mass in % s: %.3e\n",cszIon.c_str(),dElement_Mass[uiI] / g_XASTRO.k_dMsun);
			fprintf(fileOut,"% s, %.17e\n",cszIon.c_str(),dElement_Mass[uiI] / g_XASTRO.k_dMsun);
		}
	}
	fclose(fileOut);

	if (lpszOutputPrefix[0] != 0)
		sprintf(lpszFilename,"%s.mass.ion.d%.0f.csv",lpszOutputPrefix,dTime_days);
	else
		sprintf(lpszFilename,"mass.ion.d%.0f.csv",dTime_days);
	fileOut = fopen(lpszFilename,"wt");
	printf("Masses by ion:\n");	
	memset(dElement_Mass,0,sizeof(double) * uiNum_Ions);
	fprintf(fileOut,"Ion, Mass [Msun]\n");
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (lpcLine_Profiles[uiI].bActive)
		{
			bool bFound = false;
			for (uiJ = 0; uiJ < uiI && !bFound; uiJ++)
			{
				if (lpcLine_Profiles[uiI].uiIon_ID == lpcLine_Profiles[uiJ].uiIon_ID)
				{
					bFound = true;
					dElement_Mass[uiJ] += dTotal_Ion_Masses[uiI];
				}
			}
			if (!bFound)
				dElement_Mass[uiI] += dTotal_Ion_Masses[uiI];
		}
	}
			
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (dElement_Mass[uiI] > 0.0)
		{
			std::string cszIon = Get_Ion_ID_String(lpcLine_Profiles[uiI].uiIon_ID);
			printf("Total mass in % s: %.3e\n",cszIon.c_str(),dElement_Mass[uiI] / g_XASTRO.k_dMsun);
			fprintf(fileOut,"% s, %.17e\n",cszIon.c_str(),dElement_Mass[uiI] / g_XASTRO.k_dMsun);
		}
	}
	fclose(fileOut);

	printf("Masses by element:\n");	
	if (lpszOutputPrefix[0] != 0)
		sprintf(lpszFilename,"%s.mass.elem.d%.0f.csv",lpszOutputPrefix,dTime_days);
	else
		sprintf(lpszFilename,"mass.elem.d%.0f.csv",dTime_days);
	fileOut = fopen(lpszFilename,"wt");
	fprintf(fileOut,"Element, Mass [Msun]\n");
	memset(dElement_Mass,0,sizeof(double) * uiNum_Ions);
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (lpcLine_Profiles[uiI].bActive)
		{
			bool bFound = false;
			unsigned int uiElement = lpcLine_Profiles[uiI].uiIon_ID / 100;
//			printf("I = %i, searching for elem %i\n",uiI,uiElement);
			for (uiJ = 0; uiJ < uiI && !bFound; uiJ++)
			{
				unsigned int uiElement_J = (lpcLine_Profiles[uiJ].uiIon_ID / 100);
//				printf("J = %i, comparing to %i\n",uiJ,uiElement_J);
				if (uiElement == uiElement_J)
				{
					bFound = true;
//					printf("Found!\n");
					dElement_Mass[uiJ] += dTotal_Ion_Masses[uiI];
				}
			}
			if (!bFound)
				dElement_Mass[uiI] += dTotal_Ion_Masses[uiI];
		}
	}
			
	for (uiI = 0; uiI < uiNum_Ions; uiI++)
	{
		if (dElement_Mass[uiI] > 0.0)
		{
			std::string cszElement = Get_Element_ID_String(lpcLine_Profiles[uiI].uiIon_ID);
			printf("Total mass in %s: %.3e (%i)\n",cszElement.c_str(),dElement_Mass[uiI],uiI);
			fprintf(fileOut,"%s, %.17e\n",cszElement.c_str(),dElement_Mass[uiI] / g_XASTRO.k_dMsun);
		}
	}
	fclose(fileOut);
	printf("Total mass measured: %.3e\n",dTotal_Mass);

	delete [] lpcLine_Profiles;
	delete [] dTotal_Ion_Masses;
	delete [] dTau_Const;
	delete [] lpcElement_Line_Data;
	return 0;
}


