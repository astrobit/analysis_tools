#include<cstdio>
#include<cstdlib>
#include <velocity_function.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	Maxwellian_velocity_function	vfMax(10000.0);
	Fermi_Dirac_velocity_function	vfFermi(10000.0);
	Bose_Einstein_velocity_function	vfBE(10000.0);

	printf("Vel, Max, Fermi, BE\n");
	for (double dVel = 1e-10; dVel < 1e10; dVel *= 1.25)
	{
		printf("%.2e, %.2e, %.2e, %.2e\n",dVel,vfMax(dVel),vfFermi(dVel),vfBE(dVel));
	}
	
	return 0;
}
