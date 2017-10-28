#include <specfit.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>

double specfit::Continuum_Fit(const ES::Spectrum &i_cGenerated, const ES::Spectrum &i_cContinuum, specfit::feature_parameters	&o_cModel_Data)
{
	//printf(" [%.2e %.2e] ",i_cGenerated.flux(10),i_cContinuum.flux(10));
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	//FILE * fileTest = fopen("testcn.csv","wt");

	o_cModel_Data.Reset();
	for (unsigned int uiI = 0; uiI < i_cGenerated.size(); uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		double dFlux = i_cGenerated.flux(uiI);
		double dContinuum_Flux = 1.0; // flux has been flattened
		double dFlux_Eff = dContinuum_Flux - dFlux;

		o_cModel_Data.Process_pEW(dFlux,dDel_WL_Ang); // 
		o_cModel_Data.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination

		//fprintf(fileTest,"%.2f, %.17e, %.17e\n",dWL,dFlux,o_cModel_Data.m_d_pEW);
	}
	double dpEW_Err = o_cModel_Data.m_d_pEW; // target value is 0
	double dErr = dpEW_Err * dpEW_Err * 10.0; // increase weight by 10 to really enforce no absorption in this region 
	//fclose(fileTest);
	return dErr;
}
