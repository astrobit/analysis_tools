#include <xastro.h>
#include <xio.h>


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	XDATASET	cVega_Data;

	cVega_Data.ReadDataFile("alpha_lyr_mod_002.dat",true,false,0,2);

	double dSwift_u = 0.0;
	double dSwift_v = 0.0;
	double dSwift_b = 0.0;
	double dSwift_uvw1 = 0.0;
	double dSwift_uvw2 = 0.0;
	double dSwift_uvm2 = 0.0;
	double dSwift_white = 0.0;

	double dSum_Flux = 0.0;
	double dSum_WL = 0.0;
	for (unsigned int uiI = 0; uiI < cVega_Data.GetNumElements(); uiI ++)
	{
		double	dWL = cVega_Data.GetElement(0,uiI);
		double dDelta_WL;
		if (uiI == 0)
			dDelta_WL = (cVega_Data.GetElement(0,1) - dWL);
		else if (uiI == (cVega_Data.GetNumElements()- 1))
			dDelta_WL = (dWL - cVega_Data.GetElement(0,uiI - 1));
		else
			dDelta_WL = (cVega_Data.GetElement(0,uiI + 1) - cVega_Data.GetElement(0,uiI - 1));

			
		double dFlux = cVega_Data.GetElement(1,uiI) * dDelta_WL;
		dSum_Flux += dFlux;
		dSum_WL += dDelta_WL;
	}
	
	double dLum_Vega = pow(10.0,1.6034) * g_XASTRO.k_dLsun; // Yoon et al. 2010 ApJ 708:71
	printf("Lvega = %.3e\n",dLum_Vega);
	for (unsigned int uiI = 0; uiI < cVega_Data.GetNumElements(); uiI ++)
	{
		double	dWL = cVega_Data.GetElement(0,uiI);
		double dDelta_WL;
		if (uiI == 0)
			dDelta_WL = (cVega_Data.GetElement(0,1) - dWL);
		else if (uiI == (cVega_Data.GetNumElements()- 1))
			dDelta_WL = (dWL - cVega_Data.GetElement(0,uiI - 1));
		else
			dDelta_WL = (cVega_Data.GetElement(0,uiI + 1) - cVega_Data.GetElement(0,uiI - 1));

			
		double dFlux = cVega_Data.GetElement(1,uiI) / dSum_Flux * dLum_Vega * dDelta_WL;
		dSwift_u += dFlux * (g_XASTRO_Filter_Swift_u << dWL);
		dSwift_v += dFlux * (g_XASTRO_Filter_Swift_v << dWL);
		dSwift_b += dFlux * (g_XASTRO_Filter_Swift_b << dWL);
		dSwift_uvw1 += dFlux * (g_XASTRO_Filter_Swift_uvw1 << dWL);
		dSwift_uvw2 += dFlux * (g_XASTRO_Filter_Swift_uvw2 << dWL);
		dSwift_uvm2 += dFlux * (g_XASTRO_Filter_Swift_uvm2 << dWL);
		dSwift_white += dFlux * (g_XASTRO_Filter_Swift_white << dWL);
	}

	printf("u: %.17e\n",dSwift_u);
	printf("b: %.17e\n",dSwift_b);
	printf("v: %.17e\n",dSwift_v);
	printf("uvw1: %.17e\n",dSwift_uvw1);
	printf("uvw2: %.17e\n",dSwift_uvw2);
	printf("uvm2: %.17e\n",dSwift_uvm2);
	printf("white: %.17e\n",dSwift_white);

}
