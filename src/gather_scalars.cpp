#include <opacity_profile_data.h>
#include <cstdio>
#include <cstdlib>
#include <xio.h>
#include <xflash.h>
#include <set>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA	cReference_OP_Data;

	std::set<unsigned int> setModel_List;
	
	setModel_List.insert(10);
	setModel_List.insert(17);
	setModel_List.insert(18);
	setModel_List.insert(41);
	setModel_List.insert(45);
	setModel_List.insert(49);
	setModel_List.insert(53);
	setModel_List.insert(54);
	setModel_List.insert(55);
	setModel_List.insert(56);
	setModel_List.insert(57);
	setModel_List.insert(61);
//	setModel_List.insert(62); <- Helmholz mass set, removed as generally unnecessary
//	setModel_List.insert(63); <- Helmholz mass set, removed as generally unnecessary
	setModel_List.insert(64);
//	setModel_List.insert(65); <- Helmholz mass set, removed as generally unnecessary
//	setModel_List.insert(66); <- Helmholz mass set, removed as generally unnecessary
//	setModel_List.insert(67); <- Helmholz mass set, removed as generally unnecessary
//	setModel_List.insert(68); <- Helmholz mass set, removed as generally unnecessary
//	setModel_List.insert(69); <- Helmholz mass set, removed as generally unnecessary
	setModel_List.insert(70);
	setModel_List.insert(71);
	setModel_List.insert(72);
	setModel_List.insert(73);
	setModel_List.insert(74);
	//setModel_List.insert(75); removed due to not being in free expansion
	setModel_List.insert(80);
	setModel_List.insert(81);

	cReference_OP_Data.Load("run57/opacity_map_scalars.opdata");
	FILE * fileOut = fopen("scalardata.csv","wt");
	FILE * fileOutTex = fopen("scalardata.tex","wt");
	fprintf(fileOut,"Model, C / C(57), C, O / O(57), O, Mg / Mg(57), Mg, Si / Si (57), Si, Fe / Fe(57), Fe, Shell / Shell(57), Shell\n");
	for (unsigned int uiI = 9; uiI <= 82; uiI++)
	{
		OPACITY_PROFILE_DATA	cOP_Data;
		XMAP	cParameters;
		char lpszOp_Data_File[64];
		char lpszParameters_File[64];
		sprintf(lpszOp_Data_File,"run%i/opacity_map_scalars.opdata",uiI);
		sprintf(lpszParameters_File,"run%i/flash.par",uiI);
		cOP_Data.Load(lpszOp_Data_File);
		cParameters.Read_File(lpszParameters_File);
		double dTime = -1.0;
		unsigned int uiLast_File = -1;
		for (unsigned int uiJ = 0; uiJ <= 500; uiJ++)
		{
			char lpszFlashFilename[64];
			sprintf(lpszFlashFilename,"run%i/explos_hdf5_chk_%04i",uiI,uiJ);
			FILE * fileFlash = fopen(lpszFlashFilename,"rb");
			if (fileFlash)
			{
				uiLast_File = uiJ;
				fclose(fileFlash);
			}
		}
		if (uiLast_File != -1)
		{
			XFLASH_File	cFlash_File;
			char lpszFlashFilename[64];
			sprintf(lpszFlashFilename,"run%i/explos_hdf5_chk_%04i",uiI,uiLast_File);
			cFlash_File.Open(lpszFlashFilename);
			dTime = cFlash_File.m_dTime;
		}
		char lpszLogFilename[64];
		sprintf(lpszLogFilename,"run%i/SNexplos.log",uiI);
		FILE * fileLog = fopen(lpszLogFilename,"rt");
		enum EOSTYPE {UNKNOWN,HELM,GAMMA};
		EOSTYPE eEos_Type = UNKNOWN;
		if (fileLog)
		{
			while (eEos_Type == UNKNOWN && !feof(fileLog))
			{
				char lpszBuffer[4096];
				fgets(lpszBuffer,sizeof(lpszBuffer),fileLog);
				char * lpCursor = NULL;
				if ((lpCursor = strstr(lpszBuffer,"/home/ts3/bwmulligan/FLASH4.0.1/SNexsh1d")) != NULL)
				{
					lpCursor += 41;
					if (lpCursor[0] == 'g')
						eEos_Type = GAMMA;
					else
						eEos_Type = HELM;
				}
				else if ((lpCursor = strstr(lpszBuffer,"helm_table.bdat")) != NULL)
					eEos_Type = HELM;
			}
			fclose(fileLog);
		}
		if (uiI >= 17 && eEos_Type == UNKNOWN)
			eEos_Type = GAMMA;

		fprintf(fileOut,"%i",uiI);
		std::string szExpl_Model = cParameters.Get_Value_String("sim_Explosion_Model_1D_Datafile");
		std::string strType;
		std::string szGam_Expl_Model;
		if (szExpl_Model == "det251D.dat")
			szGam_Expl_Model = "c";
		else if (szExpl_Model == "det331D.dat")
			szGam_Expl_Model = "b";
		else// if (szExpl_Model == "flame1D.dat")
			szGam_Expl_Model = "a";
		unsigned int uiShell_Type = cParameters.Get_Value_Uint("sim_CSS_Type");
		double dShell_Radius = cParameters.Get_Value_Double("sim_CSS_Cent_Rad") * 1.436075951e-11;
		double dShell_Width = cParameters.Get_Value_Double("sim_CSS_Width") * 1.436075951e-11;
		double	dMin_Radius = 5.35e8 * 1.436075951e-11;
		double	dShell_Inner = dShell_Radius - 0.5 * dShell_Width;
		double	dShell_Outer = dShell_Radius + 0.5 * dShell_Width;

		switch (uiShell_Type)
		{
		case 0:
			strType = "\\ldots";
			break;
		case 1:
			strType = "T";
			break;
		case 2:
			strType = "G";
			dShell_Inner = dShell_Radius - 2.0 * dShell_Width;
			dShell_Outer = dShell_Radius + 2.0 * dShell_Width;
			if (dShell_Inner < dMin_Radius)
				dShell_Inner = dMin_Radius;
			break;
		case 3:
			strType = "S";
			break;
		case 4:
			strType = "RS";
			break;
		}
		double dShell_Mass = cParameters.Get_Value_Double("sim_CSS_Mass") * 5.028789822e-34;

		if (setModel_List.count(uiI) != 0)
		{
			fprintf(fileOutTex,"%i & %.1f & %s & %s & ",uiI,dTime, szGam_Expl_Model.c_str(),eEos_Type == HELM ? "H" : (eEos_Type == GAMMA ? "$\\gamma$" : "?"));

			if (uiShell_Type == 0)
				 fprintf(fileOutTex,"\\ldots & \\ldots & \\ldots & \\ldots & ");
			else if (strType == "G")
				fprintf(fileOutTex,"%.3f & %2s & %.3f\\tablenotemark{d} & %.3f\\tablenotemark{d} & ",dShell_Mass,strType.c_str(),dShell_Inner,dShell_Outer);
			else
				fprintf(fileOutTex,"%.3f & %2s & %.3f & %.3f & ",dShell_Mass,strType.c_str(),dShell_Inner,dShell_Outer);
		}
		fprintf(fileOut,"%i",uiI);
		for (unsigned int uiElem = 0; uiElem <= (OPACITY_PROFILE_DATA::SHELL - OPACITY_PROFILE_DATA::CARBON); uiElem++)
		{
			OPACITY_PROFILE_DATA::GROUP eGroup = (OPACITY_PROFILE_DATA::GROUP)(OPACITY_PROFILE_DATA::CARBON + uiElem);
			double dLog_Rel = log10(cOP_Data.Get_Scalar(eGroup) / cReference_OP_Data.Get_Scalar(eGroup));
			fprintf(fileOut,", %.3f, %.17e",dLog_Rel, cOP_Data.Get_Scalar(eGroup));
			if (setModel_List.count(uiI) != 0)
			{
				if (eGroup == OPACITY_PROFILE_DATA::SILICON)
					fprintf(fileOutTex,"%.3f & ",dLog_Rel);
				else if (eGroup == OPACITY_PROFILE_DATA::SHELL)
				{
					if (uiShell_Type == 0)
						fprintf(fileOutTex,"\\ldots & ");
					else
						fprintf(fileOutTex,"%.3f & ",dLog_Rel);
				}
			}
		}
		if (uiShell_Type == 0)
			fprintf(fileOutTex,"\\ldots ");
		else
			fprintf(fileOutTex,"%.3e ", cOP_Data.Get_Density(OPACITY_PROFILE_DATA::SHELL) / cOP_Data.Get_Density(OPACITY_PROFILE_DATA::SILICON));

		fprintf(fileOutTex,"\\\\\n");
		fprintf(fileOut,"\n");
	}
	fclose(fileOut);
	fclose(fileOutTex);

	return 0;
}
