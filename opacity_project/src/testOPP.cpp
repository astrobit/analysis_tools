#include<cstdio>
#include<cstdlib>
#include <opacity_project_pp.h>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	long double ldNe = 1.0e12;

	Planck_radiation_field rfPlanck(16000.0);
	opacity_project_element cElem;
	cElem.Read_Element_Data(20);
	Maxwellian_velocity_function vfMaxwell(16000.0);


	opacity_project_level_descriptor cLevel;
	cLevel.m_uiZ = 20;
	cLevel.m_uiN = 19;
	cLevel.m_uiS = 2;
	cLevel.m_uiL = 0;
	cLevel.m_uiP = 0;
	cLevel.m_uiLvl_ID = 1;
	opacity_project_level_descriptor cLevel_Neut;
	cLevel_Neut.m_uiZ = 20;
	cLevel_Neut.m_uiN = 20;
	cLevel_Neut.m_uiS = 1;
	cLevel_Neut.m_uiL = 0;
	cLevel_Neut.m_uiP = 0;
	cLevel_Neut.m_uiLvl_ID = 1;
	long double ldIon = cElem.Get_Ionization_Rate(cLevel_Neut,6.11316 / g_XASTRO.k_dRy_eV,rfPlanck,0.0);
	long double ldRecombine = cElem.Get_Recombination_Rate(cLevel,cLevel_Neut,6.11316 / g_XASTRO.k_dRy_eV,vfMaxwell) * ldNe;
	printf("%Le %Le\n",ldIon,ldRecombine);


	opacity_project_element cH;
	rfPlanck.Set_Temperature(3000.0);
	vfMaxwell.Set_Temperature(3000.0);
	cH.Read_Element_Data(1);
	cLevel_Neut.m_uiZ = 1;
	cLevel_Neut.m_uiN = 1;
	cLevel_Neut.m_uiS = 2;
	cLevel_Neut.m_uiL = 0;
	cLevel_Neut.m_uiP = 0;
	cLevel_Neut.m_uiLvl_ID = 1;
	cLevel.m_uiZ = 1;
	cLevel.m_uiN = 0;
	cLevel.m_uiS = 2;
	cLevel.m_uiL = 0;
	cLevel.m_uiP = 0;
	cLevel.m_uiLvl_ID = 1;
	ldIon = cH.Get_Ionization_Rate(cLevel_Neut,13.605693009  / g_XASTRO.k_dRy_eV,rfPlanck,0.0);
	ldRecombine = cH.Get_Recombination_Rate(cLevel,cLevel_Neut,13.605693009 / g_XASTRO.k_dRy_eV,vfMaxwell) * ldNe;
	printf("H: %Le %Le\n",ldIon,ldRecombine);

	rfPlanck.Set_Temperature(15000.0);
	vfMaxwell.Set_Temperature(15000.0);
	ldIon = cH.Get_Ionization_Rate(cLevel_Neut,13.605693009  / g_XASTRO.k_dRy_eV,rfPlanck,0.0);
	ldRecombine = cH.Get_Recombination_Rate(cLevel,cLevel_Neut,13.605693009 / g_XASTRO.k_dRy_eV,vfMaxwell) * ldNe;
	printf("H: %Le %Le\n",ldIon,ldRecombine);



	


	return 0;
}
