#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <xio.h>
#include <iostream>
#include <sstream>
#include <abundance.h>
#include <xastro.h>
#include <opacity_profile_data.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>

#define NUM_ZONES	256

/// user data in YML file: log luminosity, time after explosion, photosphere velocity, outer velocity, abundance filename
const char lpszYML_File[] = {"#New configuration for TARDIS based on YAML\n#IMPORTANT any pure floats need to have a +/- after the e e.g. 2e+5\n#Hopefully pyyaml will fix this soon.\n---\n#Currently only simple1d is allowed\ntardis_config_version: v1.0\nsupernova:\n    luminosity_requested: %.7f log_lsun\n    time_explosion: %.2f day\n\n#atom_data: kurucz_atom_pure_simple.h5\natom_data: kurucz_cd23_chianti_H_He.h5\n\nmodel:\n            \n    structure:\n        type: file\n        filename: %s\n        filetype: simple_ascii\n        v_inner_boundary: %.1f km/s\n        v_outer_boundary: %.1f km/s\n\n    abundances:\n#         type: uniform\n        type: file\n        filename: %s\n        filetype: simple_ascii\n\n\nplasma:\n    initial_t_inner: %.0f K\n    initial_t_rad: %.0f K\n    disable_electron_scattering: no\n    ionization: nebular\n    excitation: dilute-lte\n    radiative_rates_type: detailed\n    line_interaction_type: macroatom\n#    nlte:\n#        species : [ 'Si 2', 'Ca 2', 'Mg 2', 'S 2', 'O 2', 'O 3', 'C 2']\n\nmontecarlo:\n    seed: 23111963\n    no_of_packets : %.1e\n    iterations: %i\n    enable_reflective_inner_boundary: True\n    inner_boundary_albedo: 0.5\n\n    black_body_sampling:\n        start: 1 angstrom\n        stop: %.0f angstrom\n        num: %.1e\n\n    last_no_of_packets: %.1e\n    no_of_virtual_packets: 5\n\n    convergence_criteria:\n        type: specific\n        damping_constant: 1.0\n        threshold: 0.05\n        fraction: 0.8\n        hold: 3\n#        t_inner:\n#            damping_constant: 1.0\n\nspectrum:\n    start : 2000 angstrom\n    stop : 10000 angstrom\n    num: 8000\n\n"};

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
	size_t tNRows = i_cData.Get_Num_Rows();
	//printf("nrows: %i\n",tNRows);
	for (size_t uiI = 0; uiI < tNRows; uiI++)
	{
		//printf("Row %i\n",uiI);
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
			if (uiVout_Idx >= NUM_ZONES)
				uiVout_Idx = NUM_ZONES - 1;
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				//printf("Velocity %i\n",uiJ);
				double dC_vel = io_cResampled.Get_Element_Double(uiJ,6);
				io_cResampled.Set_Element(uiJ,6,dC_vel + dC * dVel_Frac * dMass);

				double dO_vel = io_cResampled.Get_Element_Double(uiJ,8);
				io_cResampled.Set_Element(uiJ,8,dO_vel + dO * dVel_Frac * dMass);

				for (unsigned int uiK = 9; uiK <= 13; uiK++) // F to Al: Mg group
				{
					//printf("Element %i\n",uiK);
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dMg_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 14; uiK <= 20; uiK++) // Si - Ca: Si group
				{
					//printf("Element %i\n",uiK);
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dSi_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 21; uiK <= 28; uiK++) // Sc to Ni: Fe group
				{
					//printf("Element %i\n",uiK);
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dFe_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
				for (unsigned int uiK = 29; uiK <= 127; uiK++) // make everything else Ni
				{
					//printf("Element %i\n",uiK);
					double dCurr = io_cResampled.Get_Element_Double(uiJ,28);
					io_cResampled.Set_Element(uiJ,28,dCurr + dFe_Group * dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
			}
		}
		else
		{
			if (uiVout_Idx == NUM_ZONES)
				uiVout_Idx--;
			for (unsigned int uiJ = uiVin_Idx; uiJ <= uiVout_Idx; uiJ++) 
			{
				//printf("Velocity %i\n",uiJ);
				for (unsigned int uiK = 1; uiK <= 28; uiK++)
				{
					//printf("Element %i\n",uiK);
					double dCurr = io_cResampled.Get_Element_Double(uiJ,uiK);
					io_cResampled.Set_Element(uiJ,uiK,dCurr + dVel_Frac * i_cAbundance.get_Abundance(uiK) * dMass);
				}
			}
		}
	}
}
double Interpolate_From_Dataset_Elements(xdataset_improved & i_cDataset, const double & i_dDay, size_t i_tIdx_Lower, size_t i_tIdx_Upper)
{
	double dRet = -1;
	if (i_tIdx_Lower < i_cDataset.Get_Num_Rows() && i_tIdx_Upper < i_cDataset.Get_Num_Rows())
	{
		dRet = (i_cDataset.Get_Element_Double(i_tIdx_Upper,1) - i_cDataset.Get_Element_Double(i_tIdx_Lower,1)) * (i_dDay - i_cDataset.Get_Element_Double(i_tIdx_Lower,0)) / (i_cDataset.Get_Element_Double(i_tIdx_Upper,0) - i_cDataset.Get_Element_Double(i_tIdx_Lower,0)) + i_cDataset.Get_Element_Double(i_tIdx_Lower,1);
	}
	return dRet;
}
double Interpolate_From_Dataset(xdataset_improved & i_cDataset, const double & i_dDay)
{
	double dRet = -1.0;
	if (i_cDataset.Get_Num_Rows() > 0)
	{
		if (i_dDay < i_cDataset.Get_Element_Double(0,0))
		{
//			printf("Extrapolating from begining\n");
			dRet = Interpolate_From_Dataset_Elements(i_cDataset,i_dDay,0,1);
		}
		else if (i_dDay > i_cDataset.Get_Element_Double(i_cDataset.Get_Num_Rows() - 1,0))
		{
//			printf("Extrapolating from end\n");
			dRet = Interpolate_From_Dataset_Elements(i_cDataset,i_dDay,i_cDataset.Get_Num_Rows() - 2,i_cDataset.Get_Num_Rows() - 1);
		}
		else
		{
			for (size_t tIdx = 1; tIdx < i_cDataset.Get_Num_Rows() && dRet < 0.0; tIdx++)
			{
				if (i_dDay <= i_cDataset.Get_Element_Double(tIdx,0))
				{		// 3rd ion state
					dRet = Interpolate_From_Dataset_Elements(i_cDataset,i_dDay,tIdx - 1,tIdx);
				}
			}
		}
	}
	return dRet;
}

void Usage(const char * i_lpszCommand_Line)
{
	printf("Usage: %s --model=<#> --shell-abund=[all] --ejecta-abund=[Seitenzahl_2013_N100] [other options]\n",i_lpszCommand_Line);
	printf("Generate inputs for tardis for a supernova-shell interaction model\n");
	printf("Parameters:\n");
	printf("\t--model=<#>: specify the model for which the ejecta and shell datasets will be processed.\n");
	printf("\t--shell-abund=[all]: specify the abundance that will be used for the shell material.\n\t\tBy default, a tardis input will be generated for all abundances.\n");
	printf("\t--shell-abund-list=\"list of abundances\": specify a list of shell abundances that \n\t\twill be used. This option overrides --shell-abund. Entries may\n\t\tbe separated by spaces, tabs, or commas.");
	printf("\t--ejecta-abund=[all]: specify the abundance that will be used for the ejecta material.\n\t\tBy default, the Seitenzahl (2013) N100 abundance will be used.\n");
	printf("\t--day-start=[1]: Starting day (after explosion) for which output files will be generated.\n");
	printf("\t--day-end=[24]: Final day (after explosion) for which output files will be generated.\n");
	printf("\t--day=[#]: Only day (after explosion) for which output files will be generated. This\n\t\tOption overrides option --days, --day-start, and --day-end.\n");
	printf("\t--days=\"x y z ...\": list of specific days for which output files will be generated.\n\t\tThis option overrides , --day-start, and --day-end.\n");
	printf("\t--inhibit-shell: If the model contains a shell, don't process it - this allows seeing only\n\t\tthe effect of the ejecta.\n");
	printf("\t--temps-file=[file]: Temperatures to use at each epoch; default is 9500K at all epochs.\n\t\tFile should be in .csv format with column 0 = epoch after explosion in days,\n\t\tcol 1 = temperature in K\n");
	printf("\t--ps-file=[file]: Photosphere velocities to use at each epoch. File should be in\n\t\t.csv format with column 0 = epoch after explosion in days,\n\t\tcol 1 = photosphere velocity in 1000 km/s.\n\t\tDefault uses the photosphere derived from the model using electron\n\t\tscattering and Ye=3.\n");
	printf("\t--lum-file=[file]: Luminosity to use at each epoch. File should be in .csv format with\n\t\tcolumn 0 = epoch after explosion in days, col 1 = log10 of the luminosity in solar\n\t\tluminosities (e.g. 1 Lsun = 0, 10 Lsun = 1, etc.). Default uses the luminosities found\n\t\tby Pereira et al. 2013.\n");
	printf("\t--num-iter=[20]: Number of iterations to perform before producing final spectrum. A minimum\n\t\tof 20 iterations is recommended.\n");
	printf("\t--num-particles=[5e4]: Number of particles to use when attempting to converge.\n");
	printf("\t--num-particles-blackbody=[1e5]: Number of blackbody particles to use.\n");
	printf("\t--num-particles-final=[5e5]: Number of particles to use when generating final spectrum.\n");
	printf("Outputs:\n");
	printf("\tdX_E_ABD_S_ABD.yml: The parameters file for tardis for day X using shell abundance ABD.\n");
	printf("\tdensity.dat: The material density file generated from the ejecta and shell profile\n\t\t(chkpt) specified.\n");
	printf("\tabundance_E_ABD_S_ABD.dat: The material abundance file generated from the ejecta and shell profiles\n\t\tand abundance profiles specified. ABD is replaced by the abundance type;\n\t\tfor ejecta only, the file will be abundance.dat\n");
}


int main(int i_iArg_Count,const char * i_lpszArg_Values[])
{
//	char lpszLine_Buffer[1024];
	xdataset_improved cEjecta;
	xdataset_improved cShell;
	xdataset_improved	cEjecta_Combined_Data;
	xdataset_improved	cCombined_Data;
	xdataset_improved cPhotosphere;
	xdataset_improved cPhotosphere_Default;
	xdataset_improved cTemperature;
	xdataset_improved cLuminosity;
	xdataset_improved cPereira_Luminosity; // SN2011fe
	std::vector<std::string> vcShell_Abundance_Name;
	snatk_abundances::abundance_list	cEjecta_Abundance;

	model cModel;
	DIR * dirTardis = opendir("tardis");
	if (dirTardis == nullptr)
	{
		printf("Creating tardis directory\n");
		int iErr = mkdir("tardis", S_IRWXU);
		if (iErr == -1)
		{
			fprintf(stderr,"Error: unable to create tardis directory in current directory.\n");
			return -3;
		}
	}
	else
	{
		closedir(dirTardis);
	}
	dirTardis = opendir("tardis/results");
	if (dirTardis == nullptr)
	{
		printf("Creating tardis/results directory\n");
		int iErr = mkdir("tardis/results", S_IRWXU);
		if (iErr == -1)
		{
			fprintf(stderr,"Warning: unable to create tardis/results directory in current directory.\n");
		}
	}
	else
	{
		closedir(dirTardis);
	}
	dirTardis = opendir("tardis/log");
	if (dirTardis == nullptr)
	{
		printf("Creating tardis/log directory\n");
		int iErr = mkdir("tardis/log", S_IRWXU);
		if (iErr == -1)
		{
			fprintf(stderr,"Warning: unable to create tardis/log directory in current directory.\n");
		}
	}
	else
	{
		closedir(dirTardis);
	}

	dirTardis = nullptr;

	size_t tPereira_Data_Count = sizeof(g_cPereira_Data) / sizeof(pereira_data);
	cPereira_Luminosity.Allocate(tPereira_Data_Count,2);
	for (size_t tI = 0; tI < tPereira_Data_Count; tI++)
	{
		cPereira_Luminosity.Set_Element(tI,0,g_cPereira_Data[tI].m_dEpoch);
		cPereira_Luminosity.Set_Element(tI,1,g_cPereira_Data[tI].m_dLog_Luminosity);
	}

	std::vector<size_t> vDay_List;

	double dTemperature_K = 9500.0;
	size_t uiFile_Num = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--chkpt", -1);
	size_t uiModel = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--model", -1);
	std::string szShell_Abundance = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--shell-abund", std::string("all"));
	std::string szShell_Abundance_List = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--shell-abund-list");
	std::string szEjecta_Abundance = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--ejecta-abund", std::string("Seitenzahl_2013_N100"));
	size_t uiDay_Start = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--day-start", 1);
	size_t uiDay_End = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--day-end", 24);
	size_t uiDay_Only = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--day", -1);
	std::string szDay_List = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--days");
	bool bInhibit_Shell = xParse_Command_Line_Exists(i_iArg_Count,i_lpszArg_Values,"--inhibit-shell");
	std::string szTemp_File = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--temps-file");
	std::string szPhotosphere_File = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--ps-file");
	std::string szLuminosity_File = xParse_Command_Line_String(i_iArg_Count,i_lpszArg_Values,"--lum-file");
	size_t uiNum_Iter = xParse_Command_Line_Int(i_iArg_Count,i_lpszArg_Values,"--num-iter",20);
	double dNum_Particles = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--num-particles",5e4);
	double dNum_Particles_Blackbody = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--num-particles-blackbody",1e5);
	double dNum_Particles_Final = xParse_Command_Line_Dbl(i_iArg_Count,i_lpszArg_Values,"--num-particles-final",5e5);
	bool bUse_Calculated_Temp = true;

	std::vector<std::string> vsUser_Shell_Abund_List;
	if (!szShell_Abundance_List.empty())
	{
		std::string sAbund;
		bool bHas_Data = false;
		for (auto iterI = szShell_Abundance_List.begin(); iterI != szShell_Abundance_List.end(); iterI++)
		{
			if ((*iterI >= 'a' && *iterI <= 'z') || (*iterI >= 'A' && *iterI <= 'Z') || (*iterI >= '0' && *iterI <= '9') || *iterI == '@' || *iterI == '#' || *iterI == '&' || *iterI == '(' || *iterI == ')' || *iterI == '~' || *iterI == '_' || *iterI == '+' || *iterI == '-' || *iterI == '.' || *iterI == '=' || *iterI == ',')
			{
				sAbund.push_back(*iterI);
			}
			else if (!sAbund.empty())
			{
				vsUser_Shell_Abund_List.push_back(sAbund);
				sAbund.clear();
			}
		}
		if (!sAbund.empty())
		{
			vsUser_Shell_Abund_List.push_back(sAbund);
		}
	}
	else
		vsUser_Shell_Abund_List.push_back(szShell_Abundance);


	if (!szDay_List.empty())
	{
		size_t tDay = 0;
		bool bHas_Data = false;
		for (auto iterI = szDay_List.begin(); iterI != szDay_List.end(); iterI++)
		{
			if (*iterI >= '0' && *iterI <= '9')
			{
				tDay *= 10;
				tDay += (*iterI - '0');
				bHas_Data = true;
			}
			else if (bHas_Data)
			{
				vDay_List.push_back(tDay);
				if (tDay > 50)
					std::cerr << "Warning: selected day " << tDay << " is after day 50." << std::endl;
				bHas_Data = false;
				tDay = 0;
			}
		}
		if (bHas_Data)
		{
			vDay_List.push_back(tDay);
			if (tDay > 50)
				std::cerr << "Warning: selected day " << tDay << " is after day 50." << std::endl;
		}
	}
	else if (uiDay_Only != -1)
	{
		vDay_List.push_back(uiDay_Only);
		if (uiDay_Only > 50)
			std::cerr << "Warning: selected day " << uiDay_Only << " is after day 50." << std::endl;
	}
	else if (uiDay_End > uiDay_Start)
	{
		if (uiDay_Start > 50 || uiDay_End > 50)
			std::cerr << "Warning: selected day range (" << uiDay_Start << " -- " << uiDay_End << ") is after day 50." << std::endl;

		size_t tDay = uiDay_Start;
		while (tDay <= uiDay_End)
		{
			vDay_List.push_back(tDay);
			tDay++;
		}
	}
	else
	{
		std::cerr << "Error: invalid day range" << std::endl;
		Usage(i_lpszArg_Values[0]);
		return -1;
	}
	FILE * fileRegen = fopen("regentardis","rt");
	bool bIdentical_Line = false;
	if (fileRegen != nullptr)
	{
		char * lpszBuffer = new char[4096];
		while (feof(fileRegen) == 0 && !bIdentical_Line)
		{
			fgets(lpszBuffer,4096,fileRegen);
			std::vector<std::string> vLine;
			char * lpszCursor = lpszBuffer;
			std::string sString;
			bool bIn_Quotes = false;
			char chQuotes_Char = 0;
			while (lpszCursor[0] != 0 && lpszCursor[0] != 10 && lpszCursor[0] != 13 && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t')) // end of string, cr, lf
				lpszCursor++;

			bIdentical_Line = true;
			size_t tCommand_Idx = 0;
			while (lpszCursor[0] != 0 && lpszCursor[0] != 10 && lpszCursor[0] != 13 && bIdentical_Line && tCommand_Idx < i_iArg_Count) // end of string, cr, lf
			{
				if (!bIn_Quotes && (lpszCursor[0] == ' ' || lpszCursor[0] == '\t'))
				{
					if (tCommand_Idx > 0)
					{
						bIdentical_Line = (sString == i_lpszArg_Values[tCommand_Idx]);
					}
					tCommand_Idx++;
					sString.clear();
				}
				else
				{
					sString.push_back(lpszCursor[0]);
					
					if (!bIn_Quotes && lpszCursor[0] == '\'' || lpszCursor[0] == '\"')
					{
						chQuotes_Char = lpszCursor[0];
						bIn_Quotes = true;
					}
					else if (bIn_Quotes && lpszCursor[0] == chQuotes_Char);
					{
						bIn_Quotes = false;
						chQuotes_Char = 0;
					}
				}
				lpszCursor++;
			}
			bIdentical_Line = (tCommand_Idx < i_iArg_Count && sString == i_lpszArg_Values[tCommand_Idx]);
			tCommand_Idx++;
			//std::cout << tCommand_Idx
			bIdentical_Line = (bIdentical_Line && tCommand_Idx == i_iArg_Count);

			
		}
		delete [] lpszBuffer;
		fclose(fileRegen);
		fileRegen = nullptr;
	}
	if (!bIdentical_Line)
	{
		fileRegen = fopen("regentardis","at");
		if (fileRegen != nullptr)
		{
			fprintf(fileRegen,"%s",i_lpszArg_Values[0]);
			for (size_t tI = 1; tI < i_iArg_Count; tI++)
			{
				fprintf(fileRegen," %s",i_lpszArg_Values[tI]);
			}
			fprintf(fileRegen,"\n");
			std::cout << "generation command added to regentardis in local directory" << std::endl;
			fclose(fileRegen);
			fileRegen = nullptr;
			chmod("regentardis", S_IRWXU);
		}
	}

//	if (uiDay_Only != -1)
//	{
//		uiDay_Start = uiDay_End = uiDay_Only;
//	}
//	if (uiDay_End > 24)
//		uiDay_End = 24;

	if (uiModel != -1)
	{
		cModel.Load_Model(uiModel);
		cModel.Load_Model_Full_Data();

		cPhotosphere_Default.Allocate(cModel.m_dsPhotosphere.GetNumRows(),2);
		for (size_t tI = 0; tI < cModel.m_dsPhotosphere.GetNumRows();tI++)
		{
			cPhotosphere_Default.Set_Element(tI,0,cModel.m_dsPhotosphere.GetElement(0,tI));
			cPhotosphere_Default.Set_Element(tI,1,cModel.m_dsPhotosphere.GetElement(3,tI) * 1.0e-8);
		}

		cEjecta = cModel.m_dsEjecta_Full_Data;
		cShell = cModel.m_dsShell_Full_Data;
	}
	else if (uiFile_Num != -1)
	{
		char lspzFilenum[16];
		sprintf(lspzFilenum,"%04i",uiFile_Num);

		std::ostringstream ossEjecta_Filename;
		ossEjecta_Filename << "ejecta" << lspzFilenum << ".xdataset";

		cEjecta.Read_xdataset(ossEjecta_Filename.str().c_str());

		if (cEjecta.Get_Num_Rows() > 0)
		{
			std::ostringstream ossShell_Filename;
			ossShell_Filename << "shell" << lspzFilenum << ".xdataset";

			cShell.Read_xdataset(ossShell_Filename.str().c_str());
			if (cShell.Get_Num_Rows() == 0)
				printf("No shell\n");
		}

		xdataset_improved cPhotosphere_Temp;
		cPhotosphere_Temp.Read_Data_File("photosphere.csv",false,',',1);
		if (cPhotosphere_Temp.Get_Num_Rows() > 0)
		{
			cPhotosphere_Default.Allocate(cPhotosphere_Temp.Get_Num_Rows(),2);
			for (size_t tI = 0; tI < cPhotosphere_Temp.Get_Num_Rows();tI++)
			{
				cPhotosphere_Default.Set_Element(tI,0,cPhotosphere_Temp.Get_Element_Double(tI,0));
				cPhotosphere_Default.Set_Element(tI,1,cPhotosphere_Temp.Get_Element_Double(tI,3) * 1.0e-8);
			}
		}
	}

	if (cEjecta.Get_Num_Rows() > 0)
	{
		snatk_abundances::abundances cAbd;
		bool bPrint_Abundance_Types = false;
		bool bAll_Specfied = false;
		if (!bInhibit_Shell)
		{
			for (auto iterI = vsUser_Shell_Abund_List.begin(); iterI != vsUser_Shell_Abund_List.end() && !bAll_Specfied; iterI++)
			{
				if (*iterI == "all")
				{
					bAll_Specfied = true;
					vcShell_Abundance_Name = cAbd.Get_Type_List();
				}
			}
			if (!bAll_Specfied)
			{ // check each one for validity
				for (auto iterI = vsUser_Shell_Abund_List.begin(); iterI != vsUser_Shell_Abund_List.end() && !bAll_Specfied; iterI++)
				{
					if (cAbd.Check_List(*iterI))
					{
						vcShell_Abundance_Name.push_back(*iterI);
					}
					else
					{
						bPrint_Abundance_Types = true;
						fprintf(stderr,"%s does not name a valid abundance type\n",iterI->c_str());
					}
				}
			}
		}

		if (!bPrint_Abundance_Types && !bInhibit_Shell)
		{
			printf("Applying shell abundances:\n");
			for (auto iterI = vcShell_Abundance_Name.begin(); iterI != vcShell_Abundance_Name.end(); iterI++)
			{
				fprintf(stderr,"\t%s\n",iterI->c_str());
			}
		}

		if (cAbd.Check_List(szEjecta_Abundance))
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

		if (!szTemp_File.empty())
		{
			bUse_Calculated_Temp = false;
			printf("Using temperatures %s\n",szTemp_File.c_str());
			cTemperature.Read_Data_File(szTemp_File.c_str(),false,',',1);
		}
		else
		{
			printf("No temperature data -- will calculate based on luminosity and photosphere\n");
			// put dummy data in table anyway
			cTemperature.Allocate(2,2);
			cTemperature.Set_Element(0,0,0.0);
			cTemperature.Set_Element(0,1,9500.0);

			cTemperature.Set_Element(1,0,100000.0);
			cTemperature.Set_Element(1,1,9500.0);
		}

		if (!szPhotosphere_File.empty())
		{
			printf("Using photosphere %s\n",szPhotosphere_File.c_str());
			cPhotosphere.Read_Data_File(szPhotosphere_File.c_str(),false,',',1);
		}
		else
		{
			xdataset_improved cPhotosphere_Temp;
			if (cPhotosphere_Default.Get_Num_Rows() == 0)
			{
				printf("No photosphere data - defaulting to 10000 km/s\n");
				cPhotosphere.Allocate(2,2);
				cPhotosphere.Set_Element(0,0,0.0);
				cPhotosphere.Set_Element(0,1,10.0);

				cPhotosphere.Set_Element(1,0,100000.0);
				cPhotosphere.Set_Element(1,1,10.0);
			}
			else
			{
				printf("No photosphere data - using Thomson scattering photosphere with Ye = 3\n");
				cPhotosphere = cPhotosphere_Default;
			}			
		}
		if (!szLuminosity_File.empty())
		{
			printf("Using luminosity %s\n",szLuminosity_File.c_str());
			cLuminosity.Read_Data_File(szLuminosity_File.c_str(),false,',',1);
		}
		else
		{
			printf("No luminosoty data - using Pereira SN2011fe luminosity data\n");
			cLuminosity = cPereira_Luminosity;
		}

		double	dVmax = 0.0;
		if (!bInhibit_Shell && cShell.Get_Num_Rows() > 0)
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

		cEjecta_Combined_Data.Allocate(NUM_ZONES,29);
		cCombined_Data.Allocate(NUM_ZONES,29);
		cEjecta_Combined_Data.Zero(); // make sure all abundances are 0
		for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
		{
			for (unsigned int uiJ = 0; uiJ < 29; uiJ++)
				cEjecta_Combined_Data.Set_Element(uiI,uiJ,0.0);

		}
		for (unsigned int uiI = 0; uiI < NUM_ZONES; uiI++)
		{
			cEjecta_Combined_Data.Set_Element(uiI,0,((uiI + 0.5) / ((double)(NUM_ZONES)) * dVmax));
		}
		printf("Resampling ejecta\n");;fflush(stdout);
		Resample(dVmax,cEjecta,cEjecta_Combined_Data,cEjecta_Abundance,true);
		unsigned int uiMax_Abd;
		bool bShell = (cShell.Get_Num_Rows() != 0 && !bInhibit_Shell);
		if (bShell)
			uiMax_Abd = vcShell_Abundance_Name.size();
		else
			uiMax_Abd = 1;
		printf("Opening density file\n");
		std::ostringstream ossDensity_Filename;
		std::ostringstream ossDensity_Filepath;
		if (bInhibit_Shell)
			ossDensity_Filename << "density-noshell.dat";
		else
			ossDensity_Filename << "density.dat";
		ossDensity_Filepath << "tardis/" << ossDensity_Filename.str();
		FILE * fileDensity = fopen(ossDensity_Filepath.str().c_str(),"wt");
		for (unsigned int uiAbd = 0; uiAbd < uiMax_Abd; uiAbd++)
		{
			snatk_abundances::abundance_list cShell_Abundance;
			//printf("Checking abundance in list\n");fflush(stdout);
			//if (cAbd.Check_List(vcShell_Abundance_Name[uiAbd]))
			cCombined_Data = cEjecta_Combined_Data;
			if (bShell)
			{
				//printf("Getting abundance\n");fflush(stdout);
				printf("Processing Abundance Type %s (%i / %i)\n",vcShell_Abundance_Name[uiAbd].c_str(),uiAbd + 1, uiMax_Abd);
				cShell_Abundance = cAbd.Get(vcShell_Abundance_Name[uiAbd]);
				printf("Resampling shell\n");
				Resample(dVmax,cShell,cCombined_Data,cShell_Abundance,false);
			}
			//cCombined_Data.SaveDataFileBin("tardis/resampled.xdataset");

			FILE * fileAbundance = nullptr;
			FILE * fileScript = nullptr;
			std::ostringstream ossAbundance_Filename;
			std::ostringstream ossScript_Filename;
			
			if (!bShell)
			{
				ossAbundance_Filename << "abundance_E_" << szEjecta_Abundance << ".dat";
				ossScript_Filename << "run_E_" << szEjecta_Abundance;
			}
			else
			{
				//char lpsAbundance_Filename[256];
				ossAbundance_Filename << "abundance_E_" << szEjecta_Abundance << "_S_" << vcShell_Abundance_Name[uiAbd] << ".dat";
				ossScript_Filename << "run_E_" << szEjecta_Abundance << "_S_" << vcShell_Abundance_Name[uiAbd];
//				sprintf(lpsAbundance_Filename,"tardis/abundance_E_%s_S_%s.dat",szEjecta_Abundance.c_str(),vcShell_Abundance_Name[uiAbd].c_str());
//				fileAbundance = fopen(lpsAbundance_Filename,"wt");

			}
			std::ostringstream ossAbundance_Filename_Path;
			std::ostringstream ossScript_Filename_Path;
			ossAbundance_Filename_Path << "tardis/" << ossAbundance_Filename.str();
			ossScript_Filename_Path << "tardis/" << ossScript_Filename.str();
			fileAbundance = fopen(ossAbundance_Filename_Path.str().c_str(),"wt");
			if(fileAbundance == nullptr)
			{
				fprintf(stderr,"Gentardis: unable to open one or more output files.  Make sure that the `tardis' directory has been created.\n");
				if (fileDensity)
					fclose(fileDensity);
				fileDensity = nullptr;
				if (fileAbundance)
					fclose(fileAbundance);
				return -4;
			}
			if (fileDensity)
				fprintf(fileDensity,"1 day\n"); 
			double	dDelta_v = (cCombined_Data.Get_Element_Double(1,0) - cCombined_Data.Get_Element_Double(0,0)) * 0.5;
			double	dVol_Const = 4.0 / 3.0 * acos(-1.0);
			double dTotal_Mass = 0.0;
			printf("Writing density and abundance files\n");
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
				double dAbd_Norm = 0.0;
				for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
				{
					if (dMass > 0.0)
						dAbd_Norm += cCombined_Data.Get_Element_Double(uiI,uiJ) / dMass;
					else if (uiJ == 1)
						dAbd_Norm += 1.0;
				}
				double dAbd_Inv_Norm = 1.0 / dAbd_Norm;
				for (unsigned int uiJ = 1; uiJ <= 28; uiJ++)
				{
					if (dMass > 0.0)
						fprintf(fileAbundance," %.8e", cCombined_Data.Get_Element_Double(uiI,uiJ) / dMass * dAbd_Inv_Norm);
					else if (uiJ == 1)
						fprintf(fileAbundance," %.8e", dAbd_Inv_Norm);
					else
						fprintf(fileAbundance," 0.0");
				}
				fprintf(fileAbundance,"\n");
			}
 			// only need to write density file once.
			if (fileDensity)
				fclose(fileDensity);
			fileDensity = nullptr;
	
			fileScript = fopen(ossScript_Filename_Path.str().c_str(),"wt");

			printf("Starting day-by-day processing\n");

			for (auto iterDay = vDay_List.begin(); iterDay != vDay_List.end(); iterDay++)//unsigned int uiDay = uiDay_Start; uiDay <= uiDay_End; uiDay++)
			{
				size_t uiDay = *iterDay;
				std::ostringstream ossFilename;
				std::ostringstream ossFilebase;
				char lpszDay_String[16];
				sprintf(lpszDay_String,"%02i",uiDay);

				ossFilebase << "d" << lpszDay_String << "_E_" << szEjecta_Abundance;
				if (bShell)
				{
					ossFilebase << "_S_" << vcShell_Abundance_Name[uiAbd];
					//strcat(lpszAbundance_File,"_");
					//strcat(lpszAbundance_File,vcShell_Abundance_Name[uiAbd].c_str());
				}

				//char lpszAbundance_File[256];
				FILE * fileYml = nullptr;
				double dDay = uiDay;
				ossFilename << "tardis/" << ossFilebase.str() << ".yml";

				//strcat(lpszAbundance_File,".dat");
				double	dPS_Vel = Interpolate_From_Dataset(cPhotosphere,dDay);
				if (dPS_Vel != -1.0)
				{
					dPS_Vel *= 1e3;
				}
				double	dTemperature_K = Interpolate_From_Dataset(cTemperature,dDay);

				double dRadius_cm = dPS_Vel * uiDay * 1.0e5 * 3600.0 * 24.0;
				double dLuminosity_Calc = std::log10(dRadius_cm * dRadius_cm * g_XASTRO.k_dpi * 4.0 * g_XASTRO.k_dSigma_SB * dTemperature_K * dTemperature_K * dTemperature_K * dTemperature_K / g_XASTRO.k_dLsun);
				double dLuminosity = Interpolate_From_Dataset(cLuminosity,dDay);
				double dTemperature_Calc = std::pow(std::pow(10.0,dLuminosity) * g_XASTRO.k_dLsun / (dRadius_cm * dRadius_cm * g_XASTRO.k_dpi * 4.0 * g_XASTRO.k_dSigma_SB),0.25);
				if (bUse_Calculated_Temp)
					dTemperature_K = dTemperature_Calc;
				printf("Day %i -- ps %.1f t %.1f L(c) %.3f L %.3f t(c) %.1f\n",uiDay,dPS_Vel,dTemperature_K,dLuminosity_Calc, dLuminosity, dTemperature_Calc);
				/// user data in YML file: log luminosity, time after explosion, photosphere velocity, outer velocity, abundance filename
				if (dPS_Vel != -1.0 && dTemperature_K != -1.0 && dLuminosity != -1.0)
				{
					fileYml = fopen(ossFilename.str().c_str(),"wt");
					if (fileYml != nullptr)
					{
						double dVmaxLcl = cCombined_Data.Get_Element_Double(NUM_ZONES - 1,0) * 1.0e-5;
						//printf("Velocity %f\n",dVmaxLcl);
						fprintf(fileYml, lpszYML_File, dLuminosity, dDay, ossDensity_Filename.str().c_str(), dPS_Vel, dVmaxLcl, ossAbundance_Filename.str().c_str(), dTemperature_K, dTemperature_K, dNum_Particles, uiNum_Iter, dNum_Particles_Blackbody, dNum_Particles_Blackbody, dNum_Particles_Final);
						fclose(fileYml);
						if (fileScript != nullptr)
						{
							fprintf(fileScript,"tardis %s.yml results/%s.dat >& log/%s.log\n",
								ossFilebase.str().c_str(),
								ossFilebase.str().c_str(),
								ossFilebase.str().c_str());
						}
					}
				}
			}
			if (fileScript)
				fclose(fileScript);
			chmod(ossScript_Filename_Path.str().c_str(), S_IRWXU);
			if (fileAbundance)
				fclose(fileAbundance);

			printf("Total mass %f Msun\n",dTotal_Mass / 1.9891e33);
		}
		printf("Done!\n");
	}
	else
	{
		fprintf(stderr,"Error: model # must be specified\n\n");
		Usage(i_lpszArg_Values[0]);
		return -2;
	}


	return 0;
}


