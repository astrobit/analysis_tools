#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <xio.h>
#include <iostream>
#include <sstream>
#include <line_routines.h>


#define NUM_ZONES	256

/// user data in YML file: log luminosity, time after explosion, photosphere velocity, outer velocity, abundance filename
const char lpszYML_File[] = {"#New configuration for TARDIS based on YAML\n#IMPORTANT any pure floats need to have a +/- after the e e.g. 2e+5\n#Hopefully pyyaml will fix this soon.\n---\n#Currently only simple1d is allowed\ntardis_config_version: v1.0\nsupernova:\n    luminosity_requested: %.7f log_lsun\n    time_explosion: %.2f day\n\n#atom_data: kurucz_atom_pure_simple.h5\natom_data: kurucz_cd23_chianti_H_He.h5\n\nmodel:\n            \n    structure:\n        type: file\n        filename: density.dat\n        filetype: simple_ascii\n        v_inner_boundary: %.1f km/s\n        v_outer_boundary: %.1f km/s\n\n    abundances:\n#         type: uniform\n#         H: 0.7\n#         He: 0.3\n        type: file\n        filename: %s\n        filetype: simple_ascii\n\nplasma:\n    initial_t_inner: 9500 K\n    initial_t_rads: 9500 K\n    disable_electron_scattering: no\n    ionization: nebular\n    excitation: dilute-lte\n    radiative_rates_type: dilute-blackbody\n    line_interaction_type: macroatom\n\nmontecarlo:\n    seed: 23111963\n    no_of_packets : 1.0e+5\n    iterations: 20\n    enable_reflective_inner_boundary: True\n    inner_boundary_albedo: 0.5\n\n    black_body_sampling:\n        start: 1 angstrom\n        stop: 1000000 angstrom\n        num: 1.0e+5\n\n    last_no_of_packets: 5.e+5\n    no_of_virtual_packets: 5\n\n    convergence_criteria:\n        type: specific\n        damping_constant: 1.0\n        threshold: 0.05\n        fraction: 0.8\n        hold: 3\n        t_inner:\n            damping_constant: 1.0\n    \nspectrum:\n    start : 2000 angstrom\n    stop : 10000 angstrom\n    num: 8000\n\n"};

class PEREIRA_DATA
{
public:
	double m_dEpoch;
	double m_dLog_Luminosity;
	PEREIRA_DATA(const double & i_dEpoch, const double & i_dLog_Luminosity)
	{
		m_dEpoch = i_dEpoch;
		m_dLog_Luminosity = i_dLog_Luminosity;
	}
};

/*PEREIRA_DATA	g_cPereira_Data[] = {
							PEREIRA_DATA(2.49, 7.9712932209),
							PEREIRA_DATA(3.39, 8.2662490688),
							PEREIRA_DATA(4.39, 8.4905376815),
							PEREIRA_DATA(5.39, 8.6960240873),
							PEREIRA_DATA(6.39, 8.8683090601),
							PEREIRA_DATA(7.39, 9.0071674775),
							PEREIRA_DATA(8.39, 9.1489900066),
							PEREIRA_DATA(9.39, 9.2108707374),
							PEREIRA_DATA(10.49, 9.3104132661),
							PEREIRA_DATA(11.39, 9.3629143399),
							PEREIRA_DATA(12.39, 9.4265611637),
							PEREIRA_DATA(16.39, 9.4857671829),
							PEREIRA_DATA(17.39, 9.4699865816),
							PEREIRA_DATA(18.39, 9.4653704764),
							PEREIRA_DATA(19.39, 9.453610882),
							PEREIRA_DATA(20.39, 9.4324414496),
							PEREIRA_DATA(21.39, 9.399067754),
							PEREIRA_DATA(24.39, 9.3250812657) };
*/
PEREIRA_DATA	g_cPereira_Data[] = {
							PEREIRA_DATA(2.49, 7.9712932209),
							PEREIRA_DATA(8.39, 9.1489900066),
							PEREIRA_DATA(18.39, 9.4653704764)};



void Resample(const double & i_dVmax, const XDATASET & i_cData, XDATASET & io_cResampled, const ABUNDANCE_LIST & i_cAbundance, bool i_bGroup_Abundances)
{
	double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
	double	dTotal_Mass = 0.0;
	for (unsigned int uiI = 0; uiI < i_cData.GetNumElements(); uiI++)
	{
		double dDens = i_cData.GetElement(8,uiI);
		double dR = i_cData.GetElement(0,uiI);
		double dDelta_R = i_cData.GetElement(1,uiI);
		double dRin = dR - 0.5 * dDelta_R;
		double dRout = dR + 0.5 * dDelta_R;
		double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
		if (dVol < 0.0)
			dVol *= -1.0;
		double dMass = dDens * dVol;
		dTotal_Mass += dMass;
		double	dVin = i_cData.GetElement(26,uiI);
		double	dVout = i_cData.GetElement(27,uiI);
		double dV = i_cData.GetElement(25,uiI);
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
			double	dC = i_cData.GetElement(3,uiI);
			double	dO = i_cData.GetElement(19,uiI);
			double	dMg_Group = i_cData.GetElement(14,uiI) + i_cData.GetElement(17,uiI);
			double	dSi_Group = i_cData.GetElement(2,uiI) + i_cData.GetElement(4,uiI) + i_cData.GetElement(20,uiI) + i_cData.GetElement(21,uiI);
			double	dFe_Group = i_cData.GetElement(5,uiI) + i_cData.GetElement(6,uiI) + i_cData.GetElement(7,uiI) + i_cData.GetElement(9,uiI) + i_cData.GetElement(10,uiI) + i_cData.GetElement(15,uiI) + i_cData.GetElement(18,uiI) + i_cData.GetElement(22,uiI) + i_cData.GetElement(23,uiI) + i_cData.GetElement(24,uiI);
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
				double dC_vel = io_cResampled.GetElement(6,uiJ);
				io_cResampled.SetElement(6,uiJ,dC_vel + dC * dVel_Frac * dMass);

				double dO_vel = io_cResampled.GetElement(8,uiJ);
				io_cResampled.SetElement(8,uiJ,dO_vel + dO * dVel_Frac * dMass);

				for (unsigned int uiK = 9; uiK <= 13; uiK++) // F to Al: Mg group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dMg_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
				for (unsigned int uiK = 14; uiK <= 20; uiK++) // Si - Ca: Si group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dSi_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
				for (unsigned int uiK = 21; uiK <= 28; uiK++) // Sc to Ni: Fe group
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dFe_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
				for (unsigned int uiK = 29; uiK <= 127; uiK++) // make everything else Ni
				{
					double dCurr = io_cResampled.GetElement(28,uiJ);
					io_cResampled.SetElement(28,uiJ,dCurr + dFe_Group * dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
			}
		}
		else
		{
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				for (unsigned int uiK = 1; uiK <= 28; uiK++)
				{
					double dCurr = io_cResampled.GetElement(uiK,uiJ);
					io_cResampled.SetElement(uiK,uiJ,dCurr + dVel_Frac * i_cAbundance.m_dAbundances[uiK] * dMass);
				}
			}
		}
	}
}

int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	char	lpszFilename[32];
	XDATASET cEjecta;
	XDATASET cShell;
	XDATASET	cCombined_Data;
	ABUNDANCE_LIST	cShell_Abundance[4];
	ABUNDANCE_LIST	cEjecta_Abundance;

	if (i_iArg_Count == 2)
	{
		unsigned int uiFile_Num = atoi(i_lpszArg_Values[1]);
		cShell_Abundance[0].Read_Table(Solar);
		cShell_Abundance[1].Read_Table(Seitenzahl_N100_2013);
		cShell_Abundance[2].Read_Table(CO_Rich);
		cShell_Abundance[3].Read_Table(Seitenzahl_Ca_Rich);
		cEjecta_Abundance.Read_Table(Seitenzahl_N100_2013);
		cEjecta_Abundance.Normalize_Groups();

		sprintf(lpszFilename,"ejecta%04i.xdataset",uiFile_Num);
		cEjecta.ReadDataFileBin(lpszFilename);


		if (cEjecta.GetNumElements() > 0)
		{
			sprintf(lpszFilename,"shell%04i.xdataset",uiFile_Num);
			cShell.ReadDataFileBin(lpszFilename);

			XDATASET cPhotosphere;
			strcpy(lpszFilename,"photosphere.csv");
			cPhotosphere.ReadDataFile(lpszFilename,false,false,',',1);

			double	dVmax = 0.0;
			if (cShell.GetNumElements() > 0)
			{
				for (unsigned int uiI = 0; uiI < cShell.GetNumElements(); uiI++)
				{
					if (dVmax < cShell.GetElement(27,uiI))
						dVmax = cShell.GetElement(27,uiI);
					if (dVmax < cShell.GetElement(25,uiI))
						dVmax = cShell.GetElement(25,uiI);
				}
			}
			else
			{
				for (unsigned int uiI = 0; uiI < cEjecta.GetNumElements(); uiI++)
				{
					if (dVmax < cEjecta.GetElement(27,uiI))
						dVmax = cEjecta.GetElement(27,uiI);
					if (dVmax < cEjecta.GetElement(25,uiI))
						dVmax = cEjecta.GetElement(25,uiI);
				}
			}

			cCombined_Data.Allocate(29,NUM_ZONES);
			unsigned int uiMax_Abd;
			bool bShell = (cShell.GetNumElements() != 0);
			if (bShell)
				uiMax_Abd = 4;
			else
				uiMax_Abd = 1;
			for (unsigned int uiAbd = 0; uiAbd < uiMax_Abd; uiAbd++)
			{
				cCombined_Data.Zero(); // make sure all abundances are 0
				for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
				{
					cCombined_Data.SetElement(0,uiI,((uiI + 0.5) / ((double)(NUM_ZONES)) * dVmax));

				}

				Resample(dVmax,cEjecta,cCombined_Data,cEjecta_Abundance,true);
				if (bShell)
					Resample(dVmax,cShell,cCombined_Data,cShell_Abundance[uiAbd],false);
				cCombined_Data.SaveDataFileBin("tardis/resampled.xdataset");

				FILE * fileDensity = NULL;
				FILE * fileAbundance = NULL;
				FILE * fileYml = NULL;
				switch (uiAbd)
				{
				case 0:
					fileDensity = fopen("tardis/density.dat","wt");
					if (bShell)
						fileAbundance = fopen("tardis/abundance_solar.dat","wt");
					else
						fileAbundance = fopen("tardis/abundance.dat","wt");
					break;
				case 1:
					fileAbundance = fopen("tardis/abundance_Seitenzahl.dat","wt");
					break;
				case 2:
					fileAbundance = fopen("tardis/abundance_CO_rich.dat","wt");
					break;
				case 3:
					fileAbundance = fopen("tardis/abundance_Ca_rich.dat","wt");
					break;
				}
				if (fileDensity)
					fprintf(fileDensity,"1 day\n"); 
				double	dDelta_v = (cCombined_Data.GetElement(0,1) - cCombined_Data.GetElement(0,0)) * 0.5;
				double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
				double dTotal_Mass = 0.0;
				for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
				{
					double	dMass = 0.0;
					for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
					{
						dMass += cCombined_Data.GetElement(uiJ,uiI);
					}
					dTotal_Mass += dMass;
					double dV = cCombined_Data.GetElement(0,uiI);
					double dVin = dV - dDelta_v;
					double dVout = dV + dDelta_v;
					double dRin = dVin * 86400.0;
					double dRout = dVout * 86400.0;
					double dVol = dVol_Const * (dRout * dRout * dRout - dRin * dRin * dRin);
					if (fileDensity)
						fprintf(fileDensity,"%i %.1f %e\n",uiI + 1, cCombined_Data.GetElement(0,uiI) * 1.0e-5, dMass / dVol);
			
					fprintf(fileAbundance,"%i",uiI + 1);
					for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
					{
						if (dMass > 0.0)
							fprintf(fileAbundance," %.8e", cCombined_Data.GetElement(uiJ,uiI) / dMass);
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
					switch (uiAbd)
					{
					case 0:
						if (bShell)
						{
							strcat(lpszFilename,"_Solar");
							strcat(lpszAbundance_File,"_solar");
						}
						break;
					case 1:
						strcat(lpszFilename,"_Seit");
						strcat(lpszAbundance_File,"_Seitenzahl");
						break;
					case 2:
						strcat(lpszFilename,"_CO_rich");
						strcat(lpszAbundance_File,"_CO_rich");
						break;
					case 3:
						strcat(lpszFilename,"_Ca_rich");
						strcat(lpszAbundance_File,"_Ca_rich");
						break;
					}
					strcat(lpszFilename,".yml");
					strcat(lpszAbundance_File,".dat");
					fileYml = fopen(lpszFilename,"wt");
					double	dPS_Vel = -1.0;
					for (unsigned int uiPS_Idx = 0; uiPS_Idx < cPhotosphere.GetNumElements() && dPS_Vel < 0.0; uiPS_Idx++)
					{
						if (g_cPereira_Data[uiDay].m_dEpoch <= cPhotosphere.GetElement(0,uiPS_Idx))
						{		// 3rd ion state
							if (uiPS_Idx > 0)
								dPS_Vel = (cPhotosphere.GetElement(3,uiPS_Idx) - cPhotosphere.GetElement(3,uiPS_Idx - 1)) * (g_cPereira_Data[uiDay].m_dEpoch - cPhotosphere.GetElement(0,uiPS_Idx - 1)) / (cPhotosphere.GetElement(0,uiPS_Idx) - cPhotosphere.GetElement(0,uiPS_Idx - 1)) + cPhotosphere.GetElement(3,uiPS_Idx - 1);
							else
								dPS_Vel = cPhotosphere.GetElement(3,uiPS_Idx);
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
	return 0;
}



