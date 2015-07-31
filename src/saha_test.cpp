

#include <compositions.h>
#include <cstdio>
#include <line_routines.h>


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

	ABUNDANCE_LIST	cSolar_Abd;
	cSolar_Abd.Read_Table(Solar);
	COMPOSITION_SET	cComp_Solar;
	unsigned int uiSolar_Abd_Cnt = 0;
	for (unsigned int uiI = 0; uiI < 128; uiI++)
		if (cSolar_Abd.m_dAbundances[uiI] > 0.0)
			uiSolar_Abd_Cnt++;

	cComp_Solar.Allocate(uiSolar_Abd_Cnt);
	uiSolar_Abd_Cnt = 0;
	for (unsigned int uiI = 0; uiI < 128; uiI++)
		if (cSolar_Abd.m_dAbundances[uiI] > 0.0)
		{
			cComp_Solar.Initialize_Composition(uiSolar_Abd_Cnt,uiI, uiI == 1 ? 1 : (2 * uiI));
			cComp_Solar.Set_Composition(uiSolar_Abd_Cnt,cSolar_Abd.m_dAbundances[uiI]);
			uiSolar_Abd_Cnt++;
		}

	for (double dTemp = 5000.0; dTemp <= 30000.0; dTemp += 5000.0)
	{
		cComp.Zaghloul_Saha(dTemp, 3.414688e-9,10000);
		cComp_Solar.Zaghloul_Saha(dTemp, 3.414688e-9,10000);
		printf("Temp = %.0f\n",dTemp);
		printf("----- H/He -----\n");
		printf("H: %.2e %.2e\n",(cComp.Get_Composition(0)).m_lpdIon_Fraction[0],(cComp.Get_Composition(0)).m_lpdIon_Fraction[1]);
		printf("He: %.2e %.2e %.2e\n",(cComp.Get_Composition(1)).m_lpdIon_Fraction[0],(cComp.Get_Composition(1)).m_lpdIon_Fraction[1],(cComp.Get_Composition(1)).m_lpdIon_Fraction[2]);

		printf("----- Solar -----\n");
		for (unsigned int uiI = 0; uiI < cComp_Solar.Get_Num_Compositions(); uiI++)
		{
			unsigned int uiZmax = cComp_Solar.Get_Composition(uiI).m_uiZ;
			printf("[%i]:",uiZmax);
			uiZmax++; // neutral up to fully ionized
			if (uiZmax > 4)
				uiZmax = 4;
			for (unsigned int uiZ = 0; uiZ < uiZmax; uiZ++)
				printf("\t%.2e",(cComp_Solar.Get_Composition(uiI)).m_lpdIon_Fraction[uiZ]);
			printf("\n");
		}

	}

	return 0;
}
