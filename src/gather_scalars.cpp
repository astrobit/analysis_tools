#include <opacity_profile_data.h>
#include <cstdio>
#include <cstdlib>
#include <xio.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA	cReference_OP_Data;

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

		if (uiShell_Type == 0)
			fprintf(fileOutTex,"%i & %s & ? & \\ldots & \\ldots & \\ldots & \\ldots & ",uiI,szGam_Expl_Model.c_str(),dShell_Mass,strType.c_str(),dShell_Inner,dShell_Outer);
		else
			fprintf(fileOutTex,"%i & %s & ? & %.3f & %2s & %.3f & %.3f & ",uiI,szGam_Expl_Model.c_str(),dShell_Mass,strType.c_str(),dShell_Inner,dShell_Outer);
		fprintf(fileOut,"%i",uiI);
		for (unsigned int uiElem = 0; uiElem <= (OPACITY_PROFILE_DATA::SHELL - OPACITY_PROFILE_DATA::CARBON); uiElem++)
		{
			OPACITY_PROFILE_DATA::GROUP eGroup = (OPACITY_PROFILE_DATA::GROUP)(OPACITY_PROFILE_DATA::CARBON + uiElem);
			double dLog_Rel = log10(cOP_Data.Get_Scalar(eGroup) / cReference_OP_Data.Get_Scalar(eGroup));
			fprintf(fileOut,", %.3f, %.17e",dLog_Rel, cOP_Data.Get_Scalar(eGroup));
			if (eGroup == OPACITY_PROFILE_DATA::SILICON)
				fprintf(fileOutTex,"%.3f & ",dLog_Rel);
			else if (eGroup == OPACITY_PROFILE_DATA::SHELL)
			{
				if (uiShell_Type == 0)
					fprintf(fileOutTex,"\\ldots\\\\\n");
				else
					fprintf(fileOutTex,"%.3f\\\\\n",dLog_Rel);
			}
		}
		fprintf(fileOut,"\n");
	}
	fclose(fileOut);
	fclose(fileOutTex);

	return 0;
}
