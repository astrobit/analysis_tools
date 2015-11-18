#include <opacity_profile_data.h>
#include <cstdio>
#include <cstdlib>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA	cReference_OP_Data;

	cReference_OP_Data.Load("run57/opacity_map_scalars.opdata");
	FILE * fileOut = fopen("scalardata.csv","wt");
	fprintf(fileOut,"Model, C / C(57), C, O / O(57), O, Mg / Mg(57), Mg, Si / Si (57), Si, Fe / Fe(57), Fe, Shell / Shell(57), Shell\n");
	for (unsigned int uiI = 9; uiI < 82; uiI++)
	{
		OPACITY_PROFILE_DATA	cOP_Data;
		char lpszOp_Data_File[128];
		sprintf(lpszOp_Data_File,"run%i/opacity_map_scalars.opdata",uiI);
		cOP_Data.Load(lpszOp_Data_File);

		fprintf(fileOut,"%i",uiI);
		for (unsigned int uiElem = 0; uiElem < (OPACITY_PROFILE_DATA::SHELL - OPACITY_PROFILE_DATA::CARBON); uiElem++)
		{
			OPACITY_PROFILE_DATA::GROUP eGroup = (OPACITY_PROFILE_DATA::GROUP)(OPACITY_PROFILE_DATA::CARBON + uiElem);
			fprintf(fileOut,", %.3f, %.17e",log10(cOP_Data.Get_Scalar(eGroup) / cReference_OP_Data.Get_Scalar(eGroup)), cOP_Data.Get_Scalar(eGroup));
		}
		fprintf(fileOut,"\n");
	}

	return 0;
}
