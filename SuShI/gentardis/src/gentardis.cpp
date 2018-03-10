#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <xio.h>
#include <iostream>
#include <sstream>
#include <abundance.h>


#define NUM_ZONES	256

/// user data in YML file: log luminosity, time after explosion, photosphere velocity, outer velocity, abundance filename
const char lpszYML_File[] = {"#New configuration for TARDIS based on YAML\n#IMPORTANT any pure floats need to have a +/- after the e e.g. 2e+5\n#Hopefully pyyaml will fix this soon.\n---\n#Currently only simple1d is allowed\ntardis_config_version: v1.0\nsupernova:\n    luminosity_requested: %.7f log_lsun\n    time_explosion: %.2f day\n\n#atom_data: kurucz_atom_pure_simple.h5\natom_data: kurucz_cd23_chianti_H_He.h5\n\nmodel:\n            \n    structure:\n        type: file\n        filename: density.dat\n        filetype: simple_ascii\n        v_inner_boundary: %.1f km/s\n        v_outer_boundary: %.1f km/s\n\n    abundances:\n#         type: uniform\n#         H: 0.7\n#         He: 0.3\n        type: file\n        filename: %s\n        filetype: simple_ascii\n\nplasma:\n    initial_t_inner: 9500 K\n    initial_t_rads: 9500 K\n    disable_electron_scattering: no\n    ionization: nebular\n    excitation: dilute-lte\n    radiative_rates_type: dilute-blackbody\n    line_interaction_type: macroatom\n\nmontecarlo:\n    seed: 23111963\n    no_of_packets : 1.0e+5\n    iterations: 20\n    enable_reflective_inner_boundary: True\n    inner_boundary_albedo: 0.5\n\n    black_body_sampling:\n        start: 1 angstrom\n        stop: 1000000 angstrom\n        num: 1.0e+5\n\n    last_no_of_packets: 5.e+5\n    no_of_virtual_packets: 5\n\n    convergence_criteria:\n        type: specific\n        damping_constant: 1.0\n        threshold: 0.05\n        fraction: 0.8\n        hold: 3\n        t_inner:\n            damping_constant: 1.0\n    \nspectrum:\n    start : 2000 angstrom\n    stop : 10000 angstrom\n    num: 8000\n\n"};

class pereira_data 
{
public:
	double m_dEpoch;
	double m_dLog_Luminosity;
	pereira_data(const double & i_dEpoch, const double & i_dLog_Luminosity)
	{
		m_dEpoch = i_dEpoch;
		m_dLog_Luminosity = i_dLog_Luminosity;
	}
};

pereira_data	g_cPereira_Data[] = {
							pereira_data(2.49, 7.9712932209),
							pereira_data(3.39, 8.2662490688),
							pereira_data(4.39, 8.4905376815),
							pereira_data(5.39, 8.6960240873),
							pereira_data(6.39, 8.8683090601),
							pereira_data(7.39, 9.0071674775),
							pereira_data(8.39, 9.1489900066),
							pereira_data(9.39, 9.2108707374),
							pereira_data(10.49, 9.3104132661),
							pereira_data(11.39, 9.3629143399),
							pereira_data(12.39, 9.4265611637),
							pereira_data(16.39, 9.4857671829),
							pereira_data(17.39, 9.4699865816),
							pereira_data(18.39, 9.4653704764),
							pereira_data(19.39, 9.453610882),
							pereira_data(20.39, 9.4324414496),
							pereira_data(21.39, 9.399067754),
							pereira_data(24.39, 9.3250812657) };

//pereira_data	g_cPereira_Data[] = {
//							pereira_data(2.49, 7.9712932209),
//							pereira_data(8.39, 9.1489900066),
//							pereira_data(18.39, 9.4653704764)};



void Resample(const double & i_dVmax, const xdataset_improved & i_cData, xdataset_improved & io_cResampled, const snatk_abundances::abundance_list & i_cAbundance, bool i_bGroup_Abundances)
{
	double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
	double	dTotal_Mass = 0.0;
	for (unsigned int uiI = 0; uiI < i_cData.Get_Num_Rows(); uiI++)
	{
		double dDens = i_cData.Get_Element_Double(uiI,8);
		double dR = i_cData.Get_Element_Double(uiI,0);
		double dDelta_R = i_cData.Get_Element_Double(uiI,1);
		double dRin = dR - 0.5 * dDelta_R;
		double dRout = dR + 0.5 * dDelta_R;
		double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
		if (dVol < 0.0)
			dVol *= -1.0;
		double dMass = dDens * dVol;
		dTotal_Mass += dMass;
		double	dVin = i_cData.Get_Element_Double(uiI,26);
		double	dVout = i_cData.Get_Element_Double(uiI,27);
		double dV = i_cData.Get_Element_Double(uiI,25);
		double dVmin = dVin;
		double dVmax = dVout;
		if (dV < dVmin)
			dVmin = dV;
		if (dVout < dVmin)
			dVmin = dVout;
		if (dV > dVmax)
			dVmax = dV;
		if (dVin > dVmax)
			dVmax = dVin;
		double dVin_Idx = ((dVmin / i_dVmax) * NUM_ZONES);
		double dVout_Idx = ((dVmax / i_dVmax) * NUM_ZONES);
		unsigned int uiVin_Idx = (dVin_Idx - fmod(dVin_Idx,1.0) + 0.1);
		unsigned int uiVout_Idx = (dVout_Idx - fmod(dVout_Idx,1.0) + 0.1);
		double dVel_Frac = (1.0 / (uiVout_Idx - uiVin_Idx + 1));

		if (i_bGroup_Abundances)
		{	// 11 = He3, 12 = He4, 13 = H, 16 = N
			double	dC = i_cData.Get_Element_Double(uiI,3);
			double	dO = i_cData.Get_Element_Double(uiI,19);
			double	dMg_Group = i_cData.Get_Element_Double(uiI,14) + i_cData.Get_Element_Double(uiI,17);
			double	dSi_Group = i_cData.Get_Element_Double(uiI,2) + i_cData.Get_Element_Double(uiI,4) + i_cData.Get_Element_Double(uiI,20) + i_cData.Get_Element_Double(uiI,21);
			double	dFe_Group = i_cData.Get_Element_Double(uiI,5) + i_cData.Get_Element_Double(uiI,6) + i_cData.Get_Element_Double(uiI,7) + i_cData.Get_Element_Double(uiI,9) + i_cData.Get_Element_Double(uiI,10) + i_cData.Get_Element_Double(uiI,15) + i_cData.Get_Element_Double(uiI,18) + i_cData.Get_Element_Double(uiI,22) + i_cData.Get_Element_Double(uiI,23) + i_cData.Get_Element_Double(uiI,24);
			double dTot_Abd = (dC + dO + dMg_Group + dSi_Group + dFe_Group);
//			if (dTot_Abd < 1.0)
			{
				// this can occur in the outer edges of the ejecta, where there is some mixing with CSM H and He.  
				// replace this with C and O.
				dC += (1.0 - dTot_Abd) * 0.5;
				dO += (1.0 - dTot_Abd) * 0.5;
			}
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				double dC_vel = io_cResampled.Get_Element_Double(uiJ,6);
				io_cResampled.Set_Element(uiJ,6,dC_vel + dC * dVel_Frac * dMass);

				double dO_vel = io_cResampled.Get_Element_Double(uiJ,8);
				io_cResampled.Set_Element(uiJ,8,dO_vel + dO * dVel_Frac * dMass);

				for (unsigned int uiK = 9; uiK <= 13; uiK++) // F to Al: Mg group
				{
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dMg_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 14; uiK <= 20; uiK++) // Si - Ca: Si group
				{
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dSi_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 21; uiK <= 28; uiK++) // Sc to Ni: Fe group
				{
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dFe_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 29; uiK <= 127; uiK++) // make everything else Ni
				{
					double dCurr = io_cResampled.Get_Element_Double(uiJ,28);
					io_cResampled.Set_Element(uiJ,28,dCurr + dFe_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
			}
		}
		else
		{
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				for (unsigned int uiK = 1; uiK <= 28; uiK++)
				{
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
			}
		}
	}
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[32];
	xdataset_improved cEjecta;
	xdataset_improved cShell;
	xdataset_improved	cCombined_Data;
	std::vector<std::string> vcShell_Abundance_Name;
	snatk_abundances::abundance_list	cEjecta_Abundance;

	size_t uiFile_Num = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--chkpt", -1);
	std::string szShell_Abundance = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--shell-abund", std::string("all"));
	std::string szEjecta_Abundance = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--ejecta-abund", std::string("Seitenzahl_N100_2013"));


	if (uiFile_Num != -1)
	{
		snatk_abundances::abundances cAbd;
		bool bPrint_Abundance_Types = false;
		if (szShell_Abundance == "all")
		{
			vcShell_Abundance_Name = cAbd.Get_Type_List();
		}
		else if (cAbd.Check_List(szShell_Abundance))
		{
			vcShell_Abundance_Name.push_back(szShell_Abundance);
		}
		else
		{
			bPrint_Abundance_Types = true;
			fprintf(stderr,"%s does not name a valid abundance type\n",szShell_Abundance.c_str());
		}

		if (!cAbd.Check_List(szEjecta_Abundance))
		{
			cEjecta_Abundance = cAbd.Get(szEjecta_Abundance);
			cEjecta_Abundance.Normalize_Groups();
		}
		else
		{
			bPrint_Abundance_Types = true;
			fprintf(stderr,"%s does not name a valid abundance type\n",szEjecta_Abundance.c_str());
		}
		if (bPrint_Abundance_Types)
		{
			std::vector<std::string> vsTypes = cAbd.Get_Type_List();
			fprintf(stderr,"Available types:\n");
			fprintf(stderr,"\tall (generates a separate tardis input file for each type listed below)\n");

			for (auto iterI = vsTypes.begin(); iterI != vsTypes.end(); iterI++)
			{
				fprintf(stderr,"\t%s\n",iterI->c_str());
			}
			return -1;
		}

		sprintf(lpszFilename,"ejecta%04i.xdataset",uiFile_Num);
		cEjecta.Read_xdataset(lpszFilename);


		if (cEjecta.Get_Num_Rows() > 0)
		{
			sprintf(lpszFilename,"shell%04i.xdataset",uiFile_Num);
			cShell.Read_xdataset(lpszFilename);

			xdataset_improved cPhotosphere;
			strcpy(lpszFilename,"photosphere.csv");
			cPhotosphere.Read_Data_File(lpszFilename,false,',',1);

			double	dVmax = 0.0;
			if (cShell.Get_Num_Rows() > 0)
			{
				for (unsigned int uiI = 0; uiI < cShell.Get_Num_Rows(); uiI++)
				{
					if (dVmax < cShell.Get_Element_Double(uiI,27))
						dVmax = cShell.Get_Element_Double(uiI,27);
					if (dVmax < cShell.Get_Element_Double(uiI,25))
						dVmax = cShell.Get_Element_Double(uiI,25);
				}
			}
			else
			{
				for (unsigned int uiI = 0; uiI < cEjecta.Get_Num_Rows(); uiI++)
				{
					if (dVmax < cEjecta.Get_Element_Double(uiI,27))
						dVmax = cEjecta.Get_Element_Double(uiI,27);
					if (dVmax < cEjecta.Get_Element_Double(uiI,25))
						dVmax = cEjecta.Get_Element_Double(uiI,25);
				}
			}

			cCombined_Data.Allocate(29,NUM_ZONES);
			unsigned int uiMax_Abd;
			bool bShell = (cShell.Get_Num_Rows() != 0);
			if (bShell)
				uiMax_Abd = vcShell_Abundance_Name.size();
			else
				uiMax_Abd = 1;
			for (unsigned int uiAbd = 0; uiAbd < uiMax_Abd; uiAbd++)
			{
				snatk_abundances::abundance_list cShell_Abundance = cAbd.Get(vcShell_Abundance_Name[uiAbd]);
				cCombined_Data.Zero(); // make sure all abundances are 0
				for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
				{
					cCombined_Data.Set_Element(uiI,0,((uiI + 0.5) / ((double)(NUM_ZONES)) * dVmax));

				}

				Resample(dVmax,cEjecta,cCombined_Data,cEjecta_Abundance,true);
				if (bShell)
					Resample(dVmax,cShell,cCombined_Data,cShell_Abundance,false);
				//cCombined_Data.SaveDataFileBin("tardis/resampled.xdataset");

				FILE * fileDensity = nullptr;
				FILE * fileAbundance = nullptr;
				FILE * fileYml = nullptr;
				if (uiAbd == 0)
					fileDensity = fopen("tardis/density.dat","wt");
				if (!bShell)
					fileAbundance = fopen("tardis/abundance.dat","wt");
				else
				{
					char lpsAbundance_Filename[256];
					sprintf(lpsAbundance_Filename,"tardis/abundance_%s.dat",vcShell_Abundance_Name[uiAbd].c_str());
					fileAbundance = fopen(lpsAbundance_Filename,"wt");

				}
				if(!fileAbundance)
				{
					fprintf(stderr,"Gentardis: unable to open one or more output files.  Make sure that the `tardis' directory has been created.\n");
					if (fileDensity)
						fclose(fileDensity);
					if (fileAbundance)
						fclose(fileAbundance);
					exit(-1);
				}
				if (fileDensity)
					fprintf(fileDensity,"1 day\n"); 
				double	dDelta_v = (cCombined_Data.Get_Element_Double(1,0) - cCombined_Data.Get_Element_Double(0,0)) * 0.5;
				double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
				double dTotal_Mass = 0.0;
				for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
				{
					double	dMass = 0.0;
					for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
					{
						dMass += cCombined_Data.Get_Element_Double(uiI,uiJ);
					}
					dTotal_Mass += dMass;
					double dV = cCombined_Data.Get_Element_Double(uiI,0);
					double dVin = dV - dDelta_v;
					double dVout = dV + dDelta_v;
					double dRin = dVin * 86400.0;
					double dRout = dVout * 86400.0;
					double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
					if (fileDensity)
						fprintf(fileDensity,"%i %.1f %e\n",uiI + 1, cCombined_Data.Get_Element_Double(uiI,0) * 1.0e-5, dMass / dVol);
			
					fprintf(fileAbundance,"%i",uiI + 1);
					for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
					{
						if (dMass > 0.0)
							fprintf(fileAbundance," %.8e", cCombined_Data.Get_Element_Double(uiI,uiJ) / dMass);
						else if (uiJ == 1)
							fprintf(fileAbundance," 1.0");
						else
							fprintf(fileAbundance," 0.0");
					}
					fprintf(fileAbundance,"\n");
				}
				for (unsigned int uiDay = 0; uiDay < 3; uiDay++)
				{
					char lpszFilename[32];
					char lpszAbundance_File[32];
					FILE * fileYml = NULL;
					unsigned int uiEpoch_Int = (unsigned int)(g_cPereira_Data[uiDay].m_dEpoch);
					sprintf(lpszFilename,"tardis/d%02i",uiEpoch_Int);
					strcpy(lpszAbundance_File,"abundance");
					if (bShell)
					{
						strcat(lpszFilename,"_");
						strcat(lpszFilename,vcShell_Abundance_Name[uiAbd].c_str());
						strcat(lpszAbundance_File,"_");
						strcat(lpszAbundance_File,vcShell_Abundance_Name[uiAbd].c_str());
					}
					strcat(lpszFilename,".yml");
					strcat(lpszAbundance_File,".dat");
					fileYml = fopen(lpszFilename,"wt");
					double	dPS_Vel = -1.0;
					for (unsigned int uiPS_Idx = 0; uiPS_Idx < cPhotosphere.Get_Num_Rows() && dPS_Vel < 0.0; uiPS_Idx++)
					{
						if (g_cPereira_Data[uiDay].m_dEpoch <= cPhotosphere.Get_Element_Double(uiPS_Idx,0))
						{		// 3rd ion state
							if (uiPS_Idx > 0)
								dPS_Vel = (cPhotosphere.Get_Element_Double(uiPS_Idx,3) - cPhotosphere.Get_Element_Double(uiPS_Idx - 1,3)) * (g_cPereira_Data[uiDay].m_dEpoch - cPhotosphere.Get_Element_Double(uiPS_Idx - 1,0)) / (cPhotosphere.Get_Element_Double(uiPS_Idx,0) - cPhotosphere.Get_Element_Double(uiPS_Idx - 1,0)) + cPhotosphere.Get_Element_Double(uiPS_Idx - 1,3);
							else
								dPS_Vel = cPhotosphere.Get_Element_Double(uiPS_Idx,3);
							dPS_Vel *= 1e-5;
						}
					}
					/// user data in YML file: log luminosity, time after explosion, photosphere velocity, outer velocity, abundance filename
					fprintf(fileYml,lpszYML_File,g_cPereira_Data[uiDay].m_dLog_Luminosity,g_cPereira_Data[uiDay].m_dEpoch,dPS_Vel,dVmax*1e-5,lpszAbundance_File);
					fclose(fileYml);
				}
				if (fileDensity)
					fclose(fileDensity);
				fclose(fileAbundance);

				printf("Total mass %f Msun\n",dTotal_Mass / 1.9891e33);
			}
		}
	}
	else
	{
		printf("Usage: %s --chkpt=<#> --shell-abund=[all] --ejecta-abund[Seitenzahl_N100_2013]\n",i_lpszArg_Values[0]);
		printf("Generate inputs for tardis for a supernova-shell interaction model\n");
		printf("Parameters:\n");
		printf("\t--chkpt=<#>: specify the checkpoint number for which the ejecta and shell datasets will be processed.\n");
		printf("\t--shell-abund=[all]: specify the abundance that will be used for the shell material. By default, a tardis input will be generated for all abundances.\n");
		printf("\t--ejecta-abund=[all]: specify the abundance that will be used for the ejecta material. By default, the Seitenzahl (2013) N100 abundance will be used.\n");
		printf("Outputs:\n");
		printf("\tdX_ABD.yaml: The parameters file for tardis for day X using shell abundance ABD.\n");
		printf("\tdensity.dat: The material density file generated from the ejecta and shell profile (chkpt) specified.\n");
		printf("\abundance_ABD.dat: The material abundance file generated from the ejecta and shell profiles and abundance profiles specified.\n");
	}


	return 0;
}



