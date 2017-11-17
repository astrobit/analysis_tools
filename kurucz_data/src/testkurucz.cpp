#include<cstdio>
#include<cstdlib>
#include <kurucz_data.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	Planck_radiation_field rfPlanck(5800.0);
	//Maxwellian_velocity_function vfMaxwell(5800.0);
	kurucz_derived_data kddData(1,0,0,rfPlanck,0.0); // Hydrogen -- Ca I and Ca II
	

	return 0;
}
