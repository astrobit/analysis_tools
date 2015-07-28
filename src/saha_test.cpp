

#include <compositions.h>
#include <cstdio>

int main(void)
{
	COMPOSITION_SET	cComp;
//	cComp.Allocate(1);
	cComp.Allocate(2);
	cComp.Initialize_Composition(0, 1, 1); // H
	cComp.Initialize_Composition(1, 2, 4); // He

//	cComp.Set_Composition(0, 1.00);
	cComp.Set_Composition(0, 0.75);
	cComp.Set_Composition(1, 0.25);


	for (double dTemp = 5000.0; dTemp <= 30000.0; dTemp += 5000.0)
	{
		cComp.Zaghloul_Saha(dTemp, 0.0000001,10000);
		printf("Temp = %.0f\n",dTemp);
		printf("H: %.2e %.2e\n",(cComp.Get_Composition(0)).m_lpdIon_Fraction[0],(cComp.Get_Composition(0)).m_lpdIon_Fraction[1]);
		printf("He: %.2e %.2e %.2e\n",(cComp.Get_Composition(1)).m_lpdIon_Fraction[0],(cComp.Get_Composition(1)).m_lpdIon_Fraction[1],(cComp.Get_Composition(1)).m_lpdIon_Fraction[2]);
	}

	return 0;
}
