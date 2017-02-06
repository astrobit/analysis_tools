#include <specfit.h>
#include <model_spectra_db.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>


class spectra_fit_data
{
public:
	msdb::USER_PARAMETERS	m_cParam;
	msdb::USER_PARAMETERS	m_cContinuum_Band_Param;

	const ES::Spectrum *	m_lpcTarget;
	const XDATASET *				m_lpcOpacity_Map_A;
	const XDATASET *				m_lpcOpacity_Map_B;

	bool					m_bDebug;
	unsigned int			m_uiDebug_Idx;

	specfit::feature_parameters		m_fpTarget_Feature_Parameters;
	double					m_dOII_P_Cygni_Peak_WL;
	double 					m_dCaII_P_Cygni_Peak_WL;
	specfit::feature_parameters		m_fpResult_Feature_Parameters;

	spectra_fit_data(void) : m_cParam()
	{
		m_lpcTarget = nullptr;
		m_lpcOpacity_Map_A = nullptr;
		m_lpcOpacity_Map_B = nullptr;
		m_bDebug = false;
		m_uiDebug_Idx = 1;
	}
};



//#define FIT_BLUE_WL 4500.0
//#define FIT_RED_WL 9000.0
#define FIT_BLUE_WL 6500.0
#define FIT_RED_WL 7900.0
void Get_Normalization_Fluxes(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, double & o_dTarget_Flux, double & o_dGenerated_Flux)
{
	unsigned int uiIdx = 0;
	double	dLuminosity_Target = 0.0, dLuminosity_Synth = 0.0;
	double	dFit_Delta_Lambda;
	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_BLUE_WL)
		uiIdx++;
	dFit_Delta_Lambda = -i_cTarget.wl(uiIdx);

	while (uiIdx < i_cTarget.size() && i_cTarget.wl(uiIdx) < FIT_RED_WL)
	{
		double dDelta_Lambda;
		if (uiIdx > 0 && uiIdx  < (i_cTarget.size() - 1))
			dDelta_Lambda = (i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx - 1)) * 0.5;
		else if (uiIdx == 0)
			dDelta_Lambda = i_cTarget.wl(uiIdx + 1) - i_cTarget.wl(uiIdx);
		else if (uiIdx == (i_cTarget.size() - 1))
			dDelta_Lambda = i_cTarget.wl(uiIdx) - i_cTarget.wl(uiIdx - 1);

		dLuminosity_Target += i_cTarget.flux(uiIdx) * dDelta_Lambda;
		dLuminosity_Synth += i_cGenerated.flux(uiIdx) * dDelta_Lambda;
		uiIdx++;

	}
	uiIdx--;
	dFit_Delta_Lambda += i_cTarget.wl(uiIdx);

	dLuminosity_Target /= dFit_Delta_Lambda; 
	dLuminosity_Synth /= dFit_Delta_Lambda; 
	o_dTarget_Flux = dLuminosity_Target * 0.5;// * 1.1;
	o_dGenerated_Flux = dLuminosity_Synth * 0.5; //dSynth_Normalization;

}

double	g_dTarget_Normalization_Flux = -1;
double	g_dGenerated_Normalization_Flux = -1;

double	Get_Fit(const ES::Spectrum &i_cTarget, const ES::Spectrum &i_cGenerated, const double &i_dMin_WL, const double &i_dMax_WL, unsigned int i_uiMoment, bool i_bRegenerate_Normalization = true)
{
	double	dRet;
	std::vector<double> vFit_Raw;
	std::vector<double> vFit_Central;
	std::vector<double> vFit_Standardized;

	if (i_bRegenerate_Normalization)
		Get_Normalization_Fluxes(i_cTarget, i_cGenerated, FIT_BLUE_WL, FIT_RED_WL, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
//		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
//	for (unsigned int uiI = 0; uiI < i_cTarget.size(); uiI++)
//	{
//		if (i_cTarget.wl(uiI) == 0.0 || std::isnan(i_cTarget.flux(uiI)) || std::isnan(i_cGenerated.flux(uiI)))
//		{
//			printf("%.2f\t%.2e\t%.2e\n",i_cTarget.wl(uiI),i_cTarget.flux(uiI),i_cGenerated.flux(uiI));
//		}
//	}


	Get_Fit_Moments_2(i_cTarget, i_cGenerated, i_dMin_WL, i_dMax_WL, i_uiMoment, vFit_Raw, vFit_Central, vFit_Standardized, g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	if (std::isnan(g_dTarget_Normalization_Flux) || std::isnan(g_dGenerated_Normalization_Flux))
		printf("Norm flux: %.2e\t%.2e\n",g_dTarget_Normalization_Flux, g_dGenerated_Normalization_Flux);
	dRet = fabs(vFit_Raw[i_uiMoment - 1]);
	return dRet; // use mean
}
void msdb_load_generate(msdb::USER_PARAMETERS	&i_cParam, msdb::SPECTRUM_TYPE i_eSpectrum_Type, const ES::Spectrum &i_cTarget, const XDATASET * i_cOp_Map_A, const XDATASET * i_cOp_Map_B, ES::Spectrum & o_cOutput)
{
	msdb::DATABASE	cMSDB;
	if (i_cOp_Map_A != nullptr)
	{
		bool bShell = i_cOp_Map_B != nullptr && i_cOp_Map_B[0].GetNumElements() > 0;
		if (cMSDB.Get_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput) == 0)
		{
			XVECTOR cParameters;
			cParameters.Set_Size(bShell? 7 : 5);
			cParameters.Set(0,1.0); // 1d after explosion
			cParameters.Set(1,i_cParam.m_dPhotosphere_Velocity_kkms); // ps velocity
			cParameters.Set(2,i_cParam.m_dPhotosphere_Temp_kK); // ps temp

			cParameters.Set(3,i_cParam.m_dEjecta_Effective_Temperature_kK); // fix excitation temp
			if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::EJECTA_ONLY)
				cParameters.Set(4,i_cParam.m_dEjecta_Log_Scalar); // PVF scalar
			else
				cParameters.Set(4,-20.0); // PVF scalar

	//				printf("%.5e %.5e\n",dEjecta_Scalar,dEjecta_Scalar + log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref));
			if (bShell)
			{
				cParameters.Set(5,i_cParam.m_dShell_Effective_Temperature_kK); // fix excitation temp
				if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::SHELL_ONLY)
					cParameters.Set(6,i_cParam.m_dShell_Log_Scalar); // HVF scalar
				else
					cParameters.Set(6,-20.0); // HVF scalar
			}
			//cParameters.Print();
			Generate_Synow_Spectra(i_cTarget,i_cOp_Map_A[0],i_cOp_Map_B[0],i_cParam.m_uiIon,cParameters,o_cOutput,-2.0,-2.0);
			msdb::dbid dbidID = cMSDB.Add_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput);
		}
	}
}


double pEW_Fit(const ES::Spectrum &i_cGenerated, const specfit::feature_parameters & i_fpTarget_Feature, specfit::feature_parameters	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	bool bQuit = false;
	double dFlux_Max = -1.0;
	for (unsigned int uiI = 0; uiI < i_cGenerated.size() && !bQuit; uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL > 8900.0) // assume the peak is between 8400 and 8900 A.
			bQuit = true;
		else if (dWL > 8400.0)
		{
			double dFlux = i_cGenerated.flux(uiI);
			if (dFlux > dFlux_Max)
			{
				uiCaII_Idx = uiI;
				dCaII_P_Cygni_Peak_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}
	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(0);
	double dWL_O_Peak = i_cGenerated.wl(0);
	double dFlux_Ca_Peak = dFlux_Max;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	o_cModel_Data.Reset();
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ);
		double dFlux = i_cGenerated.flux(uiJ);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;

		o_cModel_Data.Process_pEW(dFlux_Eff / dContinuum_Flux,dDel_WL_Ang); // 
		o_cModel_Data.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination
	}

	double dpEW_Err = (o_cModel_Data.m_d_pEW - i_fpTarget_Feature.m_d_pEW) * 10.0; // 10.0 = extra weight to pEW
	double dVel_Err = (o_cModel_Data.m_dVmin - i_fpTarget_Feature.m_dVmin);
	double dErr = dpEW_Err * dpEW_Err + dVel_Err * dVel_Err * 0.01; 
	// 0.01 is a weight applied to the velocity component so that it doesn't overpower pEW err
	/// pEW is a value of ~100, with expected error of order 50
	// velocity is a value of ~10000, with expected error of order 500
	return dErr;
}
double Continuum_Fit(const ES::Spectrum &i_cGenerated, const ES::Spectrum &i_cContinuum, specfit::feature_parameters	&o_cModel_Data)
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

double Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
//	printf(".");fflush(stdout);
	fflush(stdout);
	double dFit = DBL_MAX;
	spectra_fit_data *lpcCall_Data = (spectra_fit_data *)i_lpvSpectra_Fit_Data;
	if (lpcCall_Data && lpcCall_Data->m_lpcTarget != nullptr && lpcCall_Data->m_lpcOpacity_Map_A != nullptr)
	{
		//std::cout << lpcCall_Data->m_lpcTarget->wl(0) << " " << lpcCall_Data->m_lpcTarget->flux(0) << std::endl;
		ES::Spectrum cOutput_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_lpcTarget->wl(0), lpcCall_Data->m_lpcTarget->wl(lpcCall_Data->m_lpcTarget->size() - 1), lpcCall_Data->m_lpcTarget->size());

		ES::Spectrum cOutput_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Continuum_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Continuum_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTarget_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum_Ej  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cTrue_Continuum_Sh  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		bool bShell = lpcCall_Data->m_lpcOpacity_Map_B != nullptr && lpcCall_Data->m_lpcOpacity_Map_B->GetNumElements() > 0;
		// look to see if this particular vector has been processed

		msdb::USER_PARAMETERS cParam;

		switch (i_vX.size())
		{
		case 3:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;
			msdb_load_generate(cParam,msdb::COMBINED,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,nullptr,cOutput);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;

			msdb_load_generate(cParam,msdb::COMBINED,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum);
	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
			{
				cOutput_Continuum.flux(uiI) /= cTrue_Continuum.flux(uiI);
			}
			break;
		case 4:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = i_vX[3];
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;

			msdb_load_generate(cParam,msdb::COMBINED,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;

//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			msdb_load_generate(cParam,msdb::COMBINED,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum);
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum);
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
			{
				cOutput_Continuum.flux(uiI) /= cTrue_Continuum.flux(uiI);
			}
//			std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
			break;
		case 7:
			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;
			cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
			cParam.m_dShell_Effective_Temperature_kK = 10.0;
			msdb_load_generate(cParam,msdb::EJECTA_ONLY,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Ej);

			cParam = lpcCall_Data->m_cParam;
			cParam.m_dPhotosphere_Temp_kK = i_vX[3];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[4];
			cParam.m_dEjecta_Log_Scalar = -20.0;
			cParam.m_dShell_Log_Scalar = i_vX[5];
			msdb_load_generate(cParam,msdb::SHELL_ONLY,lpcCall_Data->m_lpcTarget[0],lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Sh);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;
			cParam.m_dPhotosphere_Temp_kK = i_vX[0];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[1];
			cParam.m_dEjecta_Log_Scalar = i_vX[2];
			cParam.m_dShell_Log_Scalar = -20.0;

			msdb_load_generate(cParam,msdb::EJECTA_ONLY,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum_Ej);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum_Ej);

			cParam.m_dWavelength_Range_Lower_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang;
			cParam.m_dWavelength_Range_Upper_Ang = lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang;
			cParam.m_dPhotosphere_Temp_kK = i_vX[3];
			cParam.m_dPhotosphere_Velocity_kkms = i_vX[4];
			cParam.m_dEjecta_Log_Scalar = -20.0;
			cParam.m_dShell_Log_Scalar = i_vX[5];

			msdb_load_generate(cParam,msdb::SHELL_ONLY,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cOutput_Continuum_Sh);
			msdb_load_generate(cParam,msdb::CONTINUUM,cTarget_Continuum,lpcCall_Data->m_lpcOpacity_Map_A,lpcCall_Data->m_lpcOpacity_Map_B,cTrue_Continuum_Sh);

	///		std::ofstream ofTemp;
	//		ofTemp.open("last_c.csv");
			for (unsigned int uiI = 0; uiI < cOutput_Continuum_Sh.size(); uiI++)
			{
	//			if (ofTemp.is_open())
	//			{
	//				ofTemp << cOutput_Continuum.wl(uiI) << ", " << cOutput_Continuum.flux(uiI) << ", " << cTrue_Continuum.flux(uiI) << std::endl;
	//			}
				cOutput_Continuum_Sh.flux(uiI) /= cTrue_Continuum_Sh.flux(uiI);
				cOutput_Continuum_Ej.flux(uiI) /= cTrue_Continuum_Ej.flux(uiI);
				cOutput_Continuum.flux(uiI) = cOutput_Continuum_Ej.flux(uiI) * (1.0 - i_vX[6]) + cOutput_Continuum_Sh.flux(uiI) * i_vX[6];
		
			}

			for (unsigned int uiI = 0; uiI < cOutput_Ej.size(); uiI++)
			{
				cOutput.wl(uiI) = cOutput_Ej.wl(uiI);
				cOutput.flux(uiI) = cOutput_Ej.flux(uiI) * (1.0 - i_vX[6]) + cOutput_Sh.flux(uiI) * i_vX[6];

			}
			break;
		}
		double dTgt_Norm, dGen_Norm;
		Get_Normalization_Fluxes(lpcCall_Data->m_lpcTarget[0], cOutput, FIT_BLUE_WL, FIT_RED_WL, dTgt_Norm, dGen_Norm);
		specfit::feature_parameters cfpModel,cfpCont_Model;
		printf("--");
		fflush(stdout);
		dFit = pEW_Fit(cOutput,lpcCall_Data->m_fpTarget_Feature_Parameters,cfpModel) + Continuum_Fit(cOutput_Continuum,cTrue_Continuum,cfpCont_Model);
		printf(" (%.2f %.1f -- %.2f) ",cfpModel.m_d_pEW,cfpModel.m_dVmin,cfpCont_Model.m_d_pEW);
		lpcCall_Data->m_fpResult_Feature_Parameters = cfpModel;
		fflush(stdout);

		double dScale = lpcCall_Data->m_lpcTarget[0].flux(0) / dTgt_Norm;
		//scaling continuum for fit: the scaling factors for t main fitting region will be used, so scale
		// true continuum to the blue edge of the fit region, and scale the test spectrum to the blue edge / scaling factor for blue edge * scaling factor for actual test region
		// this makes sure that the continuum region gets scaled the same as the main region, so it has equal weight.
		for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
		{
			cOutput_Continuum.flux(uiI) *= dScale * dGen_Norm;
			cTarget_Continuum.flux(uiI) = lpcCall_Data->m_lpcTarget[0].flux(0);
		}
		double dFlux_min = DBL_MAX;
		double dFlux_Min_Wl = 0.0;
		double dFlux_Max = 0.0;
		double dFlux_Max_Wl = 0.0;
		for (unsigned int uiI = 0; uiI < lpcCall_Data->m_lpcTarget[0].size(); uiI++)
		{
			double dFlux = lpcCall_Data->m_lpcTarget[0].flux(uiI);
			if (dFlux < dFlux_min)
			{
				dFlux_min = dFlux;
				dFlux_Min_Wl = lpcCall_Data->m_lpcTarget[0].wl(uiI);
			}
		}
		for (unsigned int uiI = 0; uiI < lpcCall_Data->m_lpcTarget[0].size(); uiI++)
		{
			double dWL = lpcCall_Data->m_lpcTarget[0].wl(uiI);
			if (dWL > dFlux_Min_Wl)
			{
				double dFlux = lpcCall_Data->m_lpcTarget[0].flux(uiI);
				if (dFlux > dFlux_Max)
				{
					dFlux_Max = dFlux;
					dFlux_Max_Wl = dWL;
				}
			}
		}
		dFlux_Min_Wl -= 50.0;
//		printf("%.1f %.1f\n",dFlux_Min_Wl,dFlux_Max_Wl);

		dFit = Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, dFlux_Min_Wl, dFlux_Max_Wl,2,true) ;
		dFit += Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cParam.m_dWavelength_Range_Lower_Ang, dFlux_Min_Wl,2,true) * 0.1;
		dFit += Get_Fit(cTarget_Continuum, cOutput_Continuum, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang,2,false);

//		std::cout << std::endl << cParam.m_dWavelength_Range_Lower_Ang << " " << cOutput_Continuum.wl(0) << " " << cTarget_Continuum.wl(0) << std::endl;
		if (lpcCall_Data->m_bDebug)
		{
			std::ofstream fsDebug;
			std::ostringstream ossDebugFilepath;
			ossDebugFilepath << ".debug/";
			ossDebugFilepath << cParam.m_uiModel_ID;
			ossDebugFilepath << "_";
			ossDebugFilepath << lpcCall_Data->m_uiDebug_Idx;
			ossDebugFilepath << "_";
			ossDebugFilepath << std::setprecision(6);
			ossDebugFilepath << i_vX[0];
			ossDebugFilepath << "_";
			ossDebugFilepath << i_vX[1];
			ossDebugFilepath << "_";
			ossDebugFilepath << i_vX[2];
			if (i_vX.size() > 3)
			{
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[3];
			}
			if (i_vX.size() == 6)
			{
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[4];
				ossDebugFilepath << "_";
				ossDebugFilepath << i_vX[5];
			}
			ossDebugFilepath << ".csv";

			fsDebug.open(ossDebugFilepath.str().c_str());
			if (fsDebug.is_open())
			{
				fsDebug << std::setprecision(6);
				for (unsigned int uiI = 0; uiI < cOutput_Continuum.size(); uiI++)
				{
					fsDebug.flags(std::ios::fixed);
					fsDebug << cOutput_Continuum.wl(uiI) << ", ";
					fsDebug.flags(std::ios::scientific);
					fsDebug << cTarget_Continuum.flux(uiI) / dTgt_Norm << ", ";
					fsDebug << cOutput_Continuum.flux(uiI) / dGen_Norm;
					fsDebug << std::endl;
				}
				for (unsigned int uiI = 0; uiI < lpcCall_Data->m_lpcTarget[0].size(); uiI++)
				{
					fsDebug.flags(std::ios::fixed);
					fsDebug << lpcCall_Data->m_lpcTarget[0].wl(uiI) << ", ";
					fsDebug.flags(std::ios::scientific);
					fsDebug << (lpcCall_Data->m_lpcTarget[0].flux(uiI) / dTgt_Norm) << ", ";
					fsDebug << (cOutput.flux(uiI) / dGen_Norm);
					fsDebug << std::endl;
				}
				fsDebug.close();
			}
			else
				std::cerr << xconsole::bold << xconsole::foreground_red << "Error:" << xconsole::reset << " unable to open file " << ossDebugFilepath.str() << std::endl;
		}
		lpcCall_Data->m_uiDebug_Idx++;
	}
//	fprintf(stdout,"_");
//	fflush(stdout);
	return dFit;
}


void Calc_Observables(const ES::Spectrum &i_cGenerated,const ES::Spectrum &i_cContinuum, specfit::fit_result	&o_cModel_Data)
{
	// idenfity the P Cygni peak of the Ca NIR feature
	unsigned int uiCaII_Idx = 0;
	double dCaII_P_Cygni_Peak_WL;
	double dCaII_P_Cygni_Peak_flux;
	bool bQuit = false;
	double dFlux_Max = -1.0;
	for (unsigned int uiI = 0; uiI < i_cGenerated.size() && !bQuit; uiI++)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL > 8900.0) // assume the peak is between 8400 and 8900 A.
			bQuit = true;
		else if (dWL > 8400.0)
		{
			double dFlux = i_cGenerated.flux(uiI);
			if (dFlux > dFlux_Max)
			{
				uiCaII_Idx = uiI;
				dCaII_P_Cygni_Peak_WL = dWL;
				dFlux_Max = dFlux;
				
			}
		}
	}
	dCaII_P_Cygni_Peak_flux = dFlux_Max;
	unsigned int uiBlue_Idx;
	double	dBlue_WL;
	dFlux_Max = -1.0;
	bQuit = false;
	for (unsigned int uiI = uiCaII_Idx; uiI > 0 && !bQuit; uiI--)
	{
		double dWL = i_cGenerated.wl(uiI);
		if (dWL < 7000.0) // assume the blue edge is between 7000 and 8000 A.
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			uiBlue_Idx = uiI;
			bQuit = true;
			dBlue_WL = dWL;
			dFlux_Max = dFlux;
		}
		else if (dWL < 8000.0)
		{
			double dFlux = i_cGenerated.flux(uiI) / i_cContinuum.flux(uiI);
			if (dFlux > 0.98)
			{
				uiBlue_Idx = uiI;
				bQuit = true;
				dBlue_WL = dWL;
				dFlux_Max = dFlux;
			}
		}
	}


	// calculate the P cygni slope
	unsigned int uiNum_Points = uiCaII_Idx - uiBlue_Idx + 1;
	double dFlux_O_Peak = i_cGenerated.flux(uiBlue_Idx);
	double dWL_O_Peak = i_cGenerated.wl(uiBlue_Idx);
	double dFlux_Ca_Peak = dCaII_P_Cygni_Peak_flux;
	double dWL_Ca_Peak = dCaII_P_Cygni_Peak_WL;
	double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
	double dDel_WL_Ang = (i_cGenerated.wl(1) - i_cGenerated.wl(0));
	// now find the P Cygni pEW and the velocity of the minimum of the feature
	xvector vA, vX, vY, vW, vSigma;
	double dSmin;
	gauss_fit_parameters * lpgfpParamters = &g_cgfpCaNIR;

	o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Blue = dWL_O_Peak;
	o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Ref_WL_Red = dWL_Ca_Peak;
	vY.Set_Size(uiNum_Points);
	vX.Set_Size(uiNum_Points);
	vW.Set_Size(uiNum_Points);
	o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Reset();
	for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
	{
		double dWL = i_cGenerated.wl(uiJ + uiBlue_Idx);
		double dFlux = i_cGenerated.flux(uiJ + uiBlue_Idx);
		double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
		double dFlux_Eff = dContinuum_Flux - dFlux;
		double dNorm_Flux_Eff = dFlux_Eff / dContinuum_Flux;

		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_pEW(dNorm_Flux_Eff,dDel_WL_Ang); // 
		o_cModel_Data.m_cSynthetic_Observables.m_fpParameters.Process_Vmin(dWL,dFlux,8542.09); /// use central line of CaIII triplet for velocity determination

		vX.Set(uiJ,dWL);
		vW.Set(uiJ,0.01); // arbitrary weight
		vY.Set(uiJ,dNorm_Flux_Eff);
	}
	
	o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
                    dDel_WL_Ang, o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_pEW_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_PVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_V_HVF, 
					o_cModel_Data.m_cSynthetic_Observables.m_xvGaussian_Fit_Uncertainty,  
					o_cModel_Data.m_cSynthetic_Observables.m_dGaussian_Fit_S, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrSingle_Gaussian_Fit, 
					&o_cModel_Data.m_cSynthetic_Observables.m_gfrDouble_Gaussian_Fit);

}

void Inc_Index(unsigned int * io_plIndices, unsigned int i_uiNum_Indices, unsigned int i_uiWrap_Value)
{
	if (i_uiNum_Indices != 0)
	{
		io_plIndices[0]++;
		if (io_plIndices[0] >= i_uiWrap_Value)
		{
			io_plIndices[0] -= i_uiWrap_Value;
			Inc_Index(&io_plIndices[1],i_uiNum_Indices - 1,i_uiWrap_Value);
		}
	}
}

double Bracket_Temperature(double & io_dTemp_Low, double & io_dTemp_Hi, const ES::Spectrum & i_cFull_Target)
{
	double dResult = 0.0;

	double dTemp_Low = io_dTemp_Low;
	double	dTemp_Hi = io_dTemp_Hi;
	double dFlux_Low = 0.0;
	double dFlux_Hi = 0.0;
	double dFlux_Target = 0.0;
	double dVariance_Hi = 0.0;
	double dVariance_Lo = 0.0;
	double dDelta_WL = (i_cFull_Target.wl(1) - i_cFull_Target.wl(0));
	for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
	{
		if (i_cFull_Target.wl(uiI) >= FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
		{
			double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);

			dFlux_Target += i_cFull_Target.flux(uiI) * dDelta_WL;
			dFlux_Low += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Low) * dDelta_WL * dE;
			dFlux_Hi += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Hi) * dDelta_WL * dE;

		}
	}
	for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
	{
		if (i_cFull_Target.wl(uiI) >= 4000)// && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
		{
			double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);
			double dErr_Lo = (dFlux_Target / dFlux_Low) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Low) * dE - i_cFull_Target.flux(uiI);
			double dErr_Hi = (dFlux_Target / dFlux_Hi) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Hi) * dE - i_cFull_Target.flux(uiI);
			dVariance_Lo += dErr_Lo * dErr_Lo;
			dVariance_Hi += dErr_Hi * dErr_Hi;

		}
	}

	unsigned int uiCount = 0;
	do
	{
		double dTemp_Test= (dTemp_Hi - dTemp_Low) * 0.5 + dTemp_Low;
		double dFlux_Test = 0.0;
		double dVariance_Test = 0.0;
		for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
		{
			if (i_cFull_Target.wl(uiI) >= FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
			{
				double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);

				dFlux_Test += XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Test) * dDelta_WL * dE;
			}
		}
		for (unsigned int uiI = 0; uiI < i_cFull_Target.size(); uiI++)
		{
			if (i_cFull_Target.wl(uiI) >= 4000)//FIT_BLUE_WL && i_cFull_Target.wl(uiI) <= FIT_RED_WL)
			{
				double dE = g_XASTRO.k_dh * g_XASTRO.k_dc / (i_cFull_Target.wl(uiI) * 1.0e-8);
				double dErr_Test = (dFlux_Target / dFlux_Test) * XA_Planck_Photon_Flux(i_cFull_Target.wl(uiI) * 1.0e-8,&dTemp_Test) * dE - i_cFull_Target.flux(uiI);
				dVariance_Test += dErr_Test * dErr_Test;

			}
		}
		dResult = dVariance_Test;
		if (dVariance_Lo < dVariance_Hi && dVariance_Test < dVariance_Hi)
		{
			dVariance_Hi = dVariance_Test;
			dTemp_Hi = dTemp_Test;
		}
		else if (dVariance_Lo > dVariance_Hi && dVariance_Test < dVariance_Lo)
		{
			dVariance_Lo = dVariance_Test;
			dTemp_Low = dTemp_Test;
		}
		else
		{
			double dTemp_Tests[2] = {dTemp_Test,dTemp_Test};
			double dTest_Low = Bracket_Temperature(dTemp_Low,dTemp_Tests[0],i_cFull_Target);
			double dTest_Hi = Bracket_Temperature(dTemp_Tests[1],dTemp_Hi,i_cFull_Target);
			if (dTest_Low < dTest_Hi)
			{
				dTemp_Hi = dTemp_Tests[0];
				dResult =  dTest_Low;
			}
			else
			{
				dTemp_Low = dTemp_Tests[1];
				dResult =  dTest_Hi;
			}
			uiCount = 9;
		}
		uiCount++;
	} while (uiCount < 8);

	io_dTemp_Low = dTemp_Low;
	io_dTemp_Hi = dTemp_Hi;
	return dResult;
}

double specfit::GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit, bool i_bDebug, const param_set * i_lppsEjecta, const param_set * i_lppsShell,const bool * i_lpbPerform_Single_Fit)
{


	spectra_fit_data cCall_Data;

	bool bShell = i_cModel.m_dsShell.GetNumElements() != 0;

	XDATASET dsDummy;
	cCall_Data.m_lpcOpacity_Map_A = &dsDummy;
	cCall_Data.m_lpcOpacity_Map_B = &dsDummy;
	if (bShell)
	{
		cCall_Data.m_lpcOpacity_Map_B = &i_cModel.m_dsShell;
//		std::cout << "Shell map " << std::scientific << i_cModel.m_dsShell.GetElement(1,700) << std::endl;
	}

	cCall_Data.m_bDebug = i_bDebug;
	double dBlue_Edge_WL = 0.0;
	switch (i_cFit.m_eFeature)
	{
	case specfit::CaNIR:
		cCall_Data.m_cParam.m_eFeature = msdb::CaNIR;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		//std::cout << "ej map size " << i_cModel.m_dsEjecta[3].GetNumElements();
		//std::cout << "shell map size " << i_cModel.m_dsShell.GetNumElements();
		cCall_Data.m_cParam.m_uiIon = 2001;
		{
			bool bQuit = false;
			double dFlux_Max = -1.0;
			unsigned int uiIdx = 0;
			unsigned int uiO_Idx = -1;
			unsigned int uiCaII_Idx = -1;
			// between about 7500 and 8000 A is the O feature. Look for it's P cygni peak and only perform fit redward of that point
			for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end() && !bQuit; iterI++)
			{
				double dWL = std::get<0>(*iterI);
				if (dWL > 8000.0)
					bQuit = true;
				else if (dWL > 7500.0)
				{
					double dFlux = std::get<1>(*iterI);
					if (dFlux > dFlux_Max)
					{
						uiO_Idx = uiIdx;
						cCall_Data.m_dOII_P_Cygni_Peak_WL = dWL;
						dFlux_Max = dFlux;
					}
				}
				uiIdx++;
			}
			cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = cCall_Data.m_dOII_P_Cygni_Peak_WL;
			
			// now idenfity the P Cygni peak of the Ca NIR feature
			uiIdx = 0;
			bQuit = false;
			dFlux_Max = -1.0;
			for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end() && !bQuit; iterI++)
			{
				double dWL = std::get<0>(*iterI);
				if (dWL > 8900.0)
					bQuit = true;
				else if (dWL > 8400.0)
				{
					double dFlux = std::get<1>(*iterI);
					if (dFlux > dFlux_Max)
					{
						uiCaII_Idx = uiIdx;
						cCall_Data.m_dCaII_P_Cygni_Peak_WL = dWL;
						dFlux_Max = dFlux;
					}
				}
				uiIdx++;
			}
			//specfit::feature_parameters	cRaw_Data;
			gauss_fit_parameters * lpgfpParamters = &g_cgfpCaNIR;
			XVECTOR	vX, vY, vA, vW, vSigma;
//			double	dSmin_Single = DBL_MAX;
//			double	dSmin;
//			double	dSmin_Flat;
//			double	dSmin_Single_Flat;
			unsigned int uiNum_Points = uiCaII_Idx - uiO_Idx + 1;
//			gauss_fit_results	cSingle_Fit;
//			gauss_fit_results	cDouble_Fit;
			vY.Set_Size(uiNum_Points);
			vX.Set_Size(uiNum_Points);
			vW.Set_Size(uiNum_Points);
			double dFlux_O_Peak = std::get<1>(i_cFit.m_vData[uiO_Idx]);
			double dWL_O_Peak = std::get<0>(i_cFit.m_vData[uiO_Idx]);
			double dFlux_Ca_Peak = std::get<1>(i_cFit.m_vData[uiCaII_Idx]);
			double dWL_Ca_Peak = std::get<0>(i_cFit.m_vData[uiCaII_Idx]);
			double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
			
			double dDel_WL_Ang = (std::get<0>(i_cFit.m_vData[uiO_Idx + 1]) - std::get<0>(i_cFit.m_vData[uiO_Idx]));
			o_cFit.m_cTarget_Observables.m_fpParameters.Reset();
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				double dWL = std::get<0>(i_cFit.m_vData[uiJ + uiO_Idx]);
				double dFlux = std::get<1>(i_cFit.m_vData[uiJ + uiO_Idx]);
				double dContinuum_Flux = (dWL - dWL_O_Peak) * dSlope + dFlux_O_Peak;
				double dFlux_Eff = dContinuum_Flux - dFlux;
				vX.Set(uiJ,dWL);
				vW.Set(uiJ,0.01); // arbitrary weight
				vY.Set(uiJ,dFlux_Eff / dContinuum_Flux);

				o_cFit.m_cTarget_Observables.m_fpParameters.Process_pEW(dFlux_Eff / dContinuum_Flux,dDel_WL_Ang); // 
				o_cFit.m_cTarget_Observables.m_fpParameters.Process_Vmin(dWL,dFlux,8542.09);
			}

			o_cFit.m_cTarget_Observables.m_dGaussian_Ref_WL_Blue = dWL_O_Peak;
			o_cFit.m_cTarget_Observables.m_dGaussian_Ref_WL_Red = dWL_Ca_Peak;

			o_cFit.m_cTarget_Observables.m_xvGaussian_Fit = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
				            dDel_WL_Ang, o_cFit.m_cTarget_Observables.m_dGaussian_pEW_PVF, 
							o_cFit.m_cTarget_Observables.m_dGaussian_pEW_HVF, 
							o_cFit.m_cTarget_Observables.m_dGaussian_V_PVF, 
							o_cFit.m_cTarget_Observables.m_dGaussian_V_HVF, 
							o_cFit.m_cTarget_Observables.m_xvGaussian_Fit_Uncertainty,  
							o_cFit.m_cTarget_Observables.m_dGaussian_Fit_S, 
							&o_cFit.m_cTarget_Observables.m_gfrSingle_Gaussian_Fit, 
							&o_cFit.m_cTarget_Observables.m_gfrDouble_Gaussian_Fit);

//           vA = Perform_Gaussian_Fit(vX, vY, vW, lpgfpParamters,
//                                dDel_WL_Ang, d_pEW_PVF, d_pEW_HVF, dV_PVF, dV_HVF, vSigma, dSmin, &cSingle_Fit,&cDouble_Fit);
			// Perform Gaussian fit to target feature
			printf("Target pEW = %.2f(%.1f %.1f) \tTarget Vel = %.1f (%.0f %.0f)\n",o_cFit.m_cTarget_Observables.m_fpParameters.m_d_pEW,
																		o_cFit.m_cTarget_Observables.m_dGaussian_pEW_HVF,
																		o_cFit.m_cTarget_Observables.m_dGaussian_pEW_PVF,
																		o_cFit.m_cTarget_Observables.m_fpParameters.m_dVmin,
																		o_cFit.m_cTarget_Observables.m_dGaussian_V_HVF,
																		o_cFit.m_cTarget_Observables.m_dGaussian_V_PVF
																		);
			cCall_Data.m_fpTarget_Feature_Parameters = o_cFit.m_cTarget_Observables.m_fpParameters;
			dBlue_Edge_WL = dWL_O_Peak;
		}
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 9000.0;
		break;
	case specfit::CaHK:
		cCall_Data.m_cParam.m_eFeature = msdb::CaHK;
		cCall_Data.m_cParam.m_uiIon = 2001;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 2500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 4000.0;
		break;
	case specfit::Si6355:
		cCall_Data.m_cParam.m_eFeature = msdb::Si6355;
		cCall_Data.m_cParam.m_uiIon = 1401;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[3];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 5500.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7000.0;
		break;
	case specfit::O7773:
		cCall_Data.m_cParam.m_eFeature = msdb::O7773;
		cCall_Data.m_cParam.m_uiIon = 800;
		cCall_Data.m_lpcOpacity_Map_A = &i_cModel.m_dsEjecta[1];
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = 6000.0;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = 7500.0;
		break;
	}
//		std::cout << "Ejecta map: " << std::scientific << cCall_Data.m_lpcOpacity_Map_A[0].GetElement(1,250) << std::endl;
	//std::string szCache = ".fitcache";
	ES::Spectrum cTarget;
	ES::Spectrum cFull_Target;
	cCall_Data.m_lpcTarget = &cTarget;
	if (!i_cFit.m_vData.empty())
	{
		unsigned int uiCount =  0;
		spectraldata vSpec_Subset;
		double dWL_Min = DBL_MAX, dWL_Max =-DBL_MAX;
		for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end(); iterI++)
		{
			double dWL = std::get<0>(*iterI);
			if (dWL >= cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang &&
				dWL <= cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang)
			{
				vSpec_Subset.push_back(*iterI);
				if (dWL < dWL_Min)
					dWL_Min = dWL;
				if (dWL > dWL_Max)
					dWL_Max = dWL;
			}
		}

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = dWL_Min;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = dWL_Max;
		cTarget = ES::Spectrum::create_from_size(vSpec_Subset.size());
		cFull_Target = ES::Spectrum::create_from_size(i_cFit.m_vData.size());
		// fill target spectrum
		unsigned int uiIdx = 0;
		for (specfit::spectraldata::const_iterator iterI = vSpec_Subset.cbegin(); iterI != vSpec_Subset.end(); iterI++)
		{
			cTarget.wl(uiIdx) = std::get<0>(*iterI);
			cTarget.flux(uiIdx) = std::get<1>(*iterI);
			cTarget.flux_error(uiIdx) = std::get<2>(*iterI);
			uiIdx++;
		}
		uiIdx = 0;
		for (specfit::spectraldata::const_iterator iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.end(); iterI++)
		{
			cFull_Target.wl(uiIdx) = std::get<0>(*iterI);
			cFull_Target.flux(uiIdx) = std::get<1>(*iterI);
			cFull_Target.flux_error(uiIdx) = std::get<2>(*iterI);
			uiIdx++;
		}
	}

	double dTemp_Low = 10000.0;
	double	dTemp_Hi = 25000.0;
	double dVar = Bracket_Temperature(dTemp_Low,dTemp_Hi,cFull_Target);
	printf("%.2e : (%.0f - %.0f)\n",dVar,dTemp_Low,dTemp_Hi);
	// debug: set temps to 5000
	//dTemp_Low = dTemp_Hi = 5000.0;

	cCall_Data.m_cParam.m_dTime_After_Explosion = 1.0; // 1d after explosion, since this is degenerate with log S
	
	cCall_Data.m_cParam.m_dWavelength_Delta_Ang = fabs(cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang - cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang) / cTarget.size();
	cCall_Data.m_cParam.m_dEjecta_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion
	cCall_Data.m_cParam.m_dShell_Scalar_Time_Power_Law = -2.0; // irrelevant for this application since all spectra will be generated at 1d after explosion
	cCall_Data.m_cParam.m_uiModel_ID = i_cModel.m_uiModel_ID;



	unsigned int uiParameters = 3;
	if (bShell)
		uiParameters = 4;
	if (i_cFit.m_bUse_Two_Component_Fit)
		uiParameters = 7;

	XVECTOR	vStarting_Point;
	XVECTOR	vLog_S_Delta;
	XVECTOR	vVariations;
	XVECTOR	vEpsilon;
	XVECTOR	vLower_Bounds;
	XVECTOR	vUpper_Bounds;
	std::vector<bool> vLower_Bounds_Valid;
	std::vector<bool> vUpper_Bounds_Valid;
	double dDelta_Bmax = 1.0;
	if (!std::isnan(i_cFit.m_dMJD_Bmax))
		dDelta_Bmax = i_cFit.m_dMJD - i_cFit.m_dMJD_Bmax + i_cFit.m_dDelay_From_Explosion_To_Bmax;
	//printf("dbmax %f\n",dDelta_Bmax);
	if (dDelta_Bmax < 1.0)
		dDelta_Bmax = 1.0;
	vStarting_Point.Set_Size(uiParameters);
	vLog_S_Delta.Set_Size(uiParameters);

	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Temp))
		vStarting_Point.Set(0,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Temp);
	else
		vStarting_Point.Set(0,10.0);
	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel))
		vStarting_Point.Set(1,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel);
	else if (!std::isnan(i_cFit.m_dMJD_Bmax))
	{
		const XDATASET *lpvPS_Data = nullptr;
		if (i_cFit.m_bUse_Two_Component_Fit)
		{
			lpvPS_Data = &i_cModel.m_dsEjecta_Photosphere;
		}
		else
		{
			lpvPS_Data = &i_cModel.m_dsPhotosphere;
		}
		unsigned int uiI = 0;
		while (uiI < lpvPS_Data->GetNumRows() && lpvPS_Data->GetElement(0,uiI) < dDelta_Bmax)
			uiI++;
		double dPS_Vel;
		if (uiI == lpvPS_Data->GetNumRows())
			uiI = lpvPS_Data->GetNumRows() - 1; // extrapolate from the end of the table
		if (uiI != 0)
		{
			dPS_Vel = ((lpvPS_Data->GetElement(4,uiI) - lpvPS_Data->GetElement(4,uiI - 1)) / (lpvPS_Data->GetElement(0,uiI) - lpvPS_Data->GetElement(0,uiI - 1)) * (dDelta_Bmax - lpvPS_Data->GetElement(0,uiI - 1)) + lpvPS_Data->GetElement(4,uiI - 1)) * 1.0e-8;
			if (dPS_Vel < 1.0) // we expect the material to still be optically thick;  enforce a lower limit of about 1000 km/s
				dPS_Vel = 1.0;
		}
		else
			dPS_Vel = lpvPS_Data->GetElement(4,uiI) * 1.0e-8;
		vStarting_Point.Set(1,dPS_Vel);
	}
	else
		vStarting_Point.Set(1,15.0);
	if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S))
		vStarting_Point.Set(2,i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dLog_S);
	else
		vStarting_Point.Set(2,2.0);
	if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
		vLog_S_Delta.Set(2,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	if (uiParameters == 4) // shell and single component fit
	{
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S))
			vStarting_Point.Set(3,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S);
		else
			vStarting_Point.Set(3,1.7);
		if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
			vLog_S_Delta.Set(3,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
	}
	else if (uiParameters == 7)
	{
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Temp))
			vStarting_Point.Set(3,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Temp);
		else
			vStarting_Point.Set(3,10.0);
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel))
			vStarting_Point.Set(4,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel);
		else if (!std::isnan(i_cFit.m_dMJD_Bmax))
		{
			const XDATASET *lpvPS_Data = nullptr;
			lpvPS_Data = &i_cModel.m_dsPhotosphere;
			unsigned int uiI = 0;
			while (uiI < lpvPS_Data->GetNumRows() && lpvPS_Data->GetElement(0,uiI) < dDelta_Bmax)
				uiI++;
			double dPS_Vel;
			if (uiI != 0)
			{
				dPS_Vel = ((lpvPS_Data->GetElement(4,uiI) - lpvPS_Data->GetElement(4,uiI - 1)) / (lpvPS_Data->GetElement(0,uiI) - lpvPS_Data->GetElement(0,uiI - 1)) * (dDelta_Bmax - lpvPS_Data->GetElement(0,uiI - 1)) + lpvPS_Data->GetElement(4,uiI - 1)) * 1.0e-8;
			}
			else
				dPS_Vel = lpvPS_Data->GetElement(4,uiI) * 1.0e-8;
			vStarting_Point.Set(4,dPS_Vel);
		}
		else
			vStarting_Point.Set(4,20.0);
		if (!std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S))
			vStarting_Point.Set(5,i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dLog_S);
		else
			vStarting_Point.Set(5,1.7);
		if (i_cFit.m_bScale_Starting_Log_S && !std::isnan(i_cFit.m_dMJD_Bmax) )
			vLog_S_Delta.Set(5,i_cFit.m_dTime_Scale_Power_Law * log10(dDelta_Bmax));
		if (!std::isnan(i_cFit.m_cSuggested_Param.m_dMixing_Fraction))
			vStarting_Point.Set(6,i_cFit.m_cSuggested_Param.m_dMixing_Fraction);
		else
			vStarting_Point.Set(6,0.25);
	}
	//printf("logsdelta %f\n",vLog_S_Delta[2]);
	vStarting_Point += vLog_S_Delta;
	vEpsilon.Set_Size(uiParameters);
	vEpsilon.Set(0,0.05);
	vEpsilon.Set(1,0.05);
	vEpsilon.Set(2,0.05);
	if (uiParameters == 4)
	{
		vEpsilon.Set(3,0.05);
	}
	else if (uiParameters == 7)
	{
		vEpsilon.Set(3,0.05);
		vEpsilon.Set(4,0.05);
		vEpsilon.Set(5,0.05);
		vEpsilon.Set(6,0.025);
	}
	vVariations.Set_Size(uiParameters);
	vVariations.Set(0,1.0);
	vVariations.Set(1,1.0);
	vVariations.Set(2,1.0);
	if (uiParameters == 4)
	{
		vVariations.Set(3,1.0);
	}
	else if (uiParameters == 7)
	{
		vVariations.Set(3,1.0);
		vVariations.Set(4,1.0);
		vVariations.Set(5,1.0);
		vVariations.Set(6,0.2);
	}
	vLower_Bounds.Set_Size(uiParameters);
	vLower_Bounds_Valid.resize(uiParameters);
	vLower_Bounds.Set(0,dTemp_Low * 1.0e-3);
	vLower_Bounds.Set(1,5.0);
//	if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
//		vLower_Bounds.Set(1,5.0);
//	else
//		vLower_Bounds.Set(1,-10.0);
	vLower_Bounds.Set(2,-0.5);
	vLower_Bounds_Valid[0] = true;
	vLower_Bounds_Valid[1] = true;
	vLower_Bounds_Valid[2] = true;
	if (uiParameters == 4)
	{
		vLower_Bounds.Set(3,-0.5);
		vLower_Bounds_Valid[3] = true;
	}
	else if (uiParameters == 7)
	{
		vLower_Bounds.Set(3,-5.0);
		vLower_Bounds_Valid[3] = true;
		if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
			vLower_Bounds.Set(4,-3.0);
		else
			vLower_Bounds.Set(4,-10.0);
		vLower_Bounds_Valid[4] = true;
		vLower_Bounds.Set(5,-0.5);
		vLower_Bounds_Valid[5] = true;
		vLower_Bounds.Set(6,-0.25);
		vLower_Bounds_Valid[6] = true;
	}
	vUpper_Bounds.Set_Size(uiParameters);
	vUpper_Bounds_Valid.resize(uiParameters);
	vUpper_Bounds_Valid[0] = true;
	vUpper_Bounds_Valid[1] = true;
	vUpper_Bounds_Valid[2] = true;
	vUpper_Bounds.Set(0,dTemp_Hi * 1.0e-3);
//	if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_ejecta].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
//		vUpper_Bounds.Set(1,4.0);
//	else
//		vUpper_Bounds.Set(1,10.0);
	vUpper_Bounds.Set(1,25.0);
	vUpper_Bounds.Set(2,3.0);
	if (uiParameters > 3)
	{
		vUpper_Bounds.Set(3,3.0);
		vUpper_Bounds_Valid[3] = true;
	}
	else if (uiParameters == 7)
	{
		vUpper_Bounds.Set(3,10.0);
		vUpper_Bounds_Valid[3] = true;
		if (std::isnan(i_cFit.m_cSuggested_Param[specfit::comp_shell].m_dPS_Vel) && !std::isnan(i_cFit.m_dMJD_Bmax))
			vUpper_Bounds.Set(4,2.0);
		else
			vUpper_Bounds.Set(4,5.0);
		vUpper_Bounds_Valid[4] = true;
		vUpper_Bounds.Set(5,0.5);
		vUpper_Bounds_Valid[5] = true;
		vUpper_Bounds.Set(6,0.75);
		vUpper_Bounds_Valid[6] = true;
	}
	printf("Temp: %.0f %.0f\n",vLower_Bounds.Get(0),vUpper_Bounds.Get(0));
	//vUpper_Bounds += vStarting_Point;
	//vLower_Bounds += vStarting_Point;

	cCall_Data.m_cContinuum_Band_Param = cCall_Data.m_cParam;
	switch (i_cFit.m_eFeature)
	{
	case specfit::CaNIR:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = dBlue_Edge_WL - 100.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang - 1500.0;
		break;
	case specfit::CaHK:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 5500.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 7000.0;
		break;
	case specfit::Si6355:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 7500.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 8500.0;
		break;
	case specfit::O7773:
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang = 5000.0;
		cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang = 6500.0;
		break;
	}
	cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Delta_Ang = fabs(cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang - cCall_Data.m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang) / cTarget.size();
	if (i_lpbPerform_Single_Fit != nullptr && i_lpbPerform_Single_Fit[0])
	{
		vStarting_Point.Set(0,i_lppsEjecta->m_dPS_Temp);
		vStarting_Point.Set(1,i_lppsEjecta->m_dPS_Vel);
		vStarting_Point.Set(2,i_lppsEjecta->m_dLog_S + vLog_S_Delta[2]);
		vStarting_Point.Set(3,i_lppsShell->m_dLog_S + vLog_S_Delta[3]);

		vLower_Bounds.Set(0,vStarting_Point.Get(0)-2.5);
		vLower_Bounds.Set(1,vStarting_Point.Get(1)-2.5);
		vLower_Bounds.Set(2,vStarting_Point.Get(2)-0.3);
		vLower_Bounds.Set(3,vStarting_Point.Get(3)-0.3);

		vUpper_Bounds.Set(0,vStarting_Point.Get(0)+2.5);
		vUpper_Bounds.Set(1,vStarting_Point.Get(1)+2.5);
		vUpper_Bounds.Set(2,vStarting_Point.Get(2)+0.3);
		vUpper_Bounds.Set(3,vStarting_Point.Get(3)+0.3);
	}


	if (vStarting_Point.is_nan())
	{
		std::cerr << "Error: starting point contains nan." << std::endl;
		return DBL_MAX;
	}
	else if ((i_lppsEjecta != nullptr || i_lppsShell != nullptr) && !i_lpbPerform_Single_Fit[0]) // the user has specified an exact set of parameters to use.
	{
		vStarting_Point.Set(0,i_lppsEjecta->m_dPS_Temp);
		vStarting_Point.Set(1,i_lppsEjecta->m_dPS_Vel);
		vStarting_Point.Set(2,i_lppsEjecta->m_dLog_S + vLog_S_Delta[2]);
		vStarting_Point.Set(3,i_lppsShell->m_dLog_S + vLog_S_Delta[3]);

		Fit_Function(vStarting_Point, &cCall_Data);
	}
	else
	{
		unsigned int uiRefines = 0;
		unsigned int uiRefines_Max = 4;
		unsigned int uiGrid_Size = 4;
		if ((i_lppsEjecta != nullptr || i_lppsShell != nullptr) && i_lpbPerform_Single_Fit[0]) // the user has specified an exact set of parameters to use.
		{
			//uiRefines_Max = 4;
			uiGrid_Size = 2;
		}

		do
		{
			std::vector<xvector> vxvGrid;
			unsigned int *lpuiIndices = new unsigned int[vLower_Bounds.size() - 1];
			memset(lpuiIndices,0,sizeof(unsigned int) * vLower_Bounds.size() - 1);
			xvector vDelta = vUpper_Bounds - vLower_Bounds;
			double dGrid_Side = 1.0 / uiGrid_Size;
	
			bool bDone = false;
			do
			{

				xvector vX(vDelta.size());
				vX.Set(0,0.0);
				for (unsigned int uiN = 1; uiN < vLower_Bounds.size(); uiN++)
				{
					vX.Set(uiN,vDelta.Get(uiN) * dGrid_Side * lpuiIndices[uiN - 1]);
				}
				vX += vLower_Bounds;
				vxvGrid.push_back(vX);
				Inc_Index(lpuiIndices,vLower_Bounds.size() - 1, uiGrid_Size + 1);
				// test for completion of grid: if all indeces have wrapped back to 0, quit
				bDone = true;
				for (unsigned int uiN = 0; uiN < vLower_Bounds.size() - 1 && bDone; uiN++)
				{
					bDone = lpuiIndices[uiN] == 0;
				}
			} while (!bDone);
			double dBest_Fit_Grid = DBL_MAX;
			xvector vStart_Point_Best;
			std::string szInst_File_Friendly = i_cFit.m_szInstrument;
			for (std::string::iterator iterI = szInst_File_Friendly.begin(); iterI != szInst_File_Friendly.end(); iterI++)
			{
				if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
					*iterI = '_';
			}
			std::string szSource_File_Friendly = i_cFit.m_szSource;
			for (std::string::iterator iterI = szSource_File_Friendly.begin(); iterI != szSource_File_Friendly.end(); iterI++)
			{
				if (*iterI == ' ' || *iterI == '\t' || *iterI == ',')
					*iterI = '_';
			}

			std::ostringstream ossGrid_Filename; 
			ossGrid_Filename << "Results/grid_fit_" << std::setprecision(7) << i_cFit.m_dMJD << "_" << szInst_File_Friendly << "_source" << szSource_File_Friendly << "_model_" << i_cModel.m_uiModel_ID << ".csv";

			FILE * fileGridFitInfo = fopen(ossGrid_Filename.str().c_str(),"wt");
			for (std::vector<xvector>::iterator iterI = vxvGrid.begin(); iterI != vxvGrid.end(); iterI++)
			{
				for (unsigned int uiK = 0; uiK < iterI->size(); uiK++)
				{
					printf("%.2f, ",iterI->Get(uiK));
				}
				double dFit = Fit_Function(*iterI, &cCall_Data);
				printf("%.2e\n",dFit);
				if (dFit < dBest_Fit_Grid)
				{
					vStart_Point_Best = *iterI;
					dBest_Fit_Grid = dFit;
				}
				if (fileGridFitInfo)
				{
					for (unsigned int uiK = 0; uiK < iterI->size(); uiK++)
					{
						fprintf(fileGridFitInfo,"%.2f, ",iterI->Get(uiK));
					}
					fprintf(fileGridFitInfo,"%.2e, %.2e, %.2e\n",dFit,cCall_Data.m_fpResult_Feature_Parameters.m_d_pEW,cCall_Data.m_fpResult_Feature_Parameters.m_dVmin);
				}
			}
			if (fileGridFitInfo)
			{
				fclose(fileGridFitInfo);
			}
			vStarting_Point = vStart_Point_Best;

			vDelta *= 0.5 * dGrid_Side;
			vUpper_Bounds = vStarting_Point + 0.5 * vDelta;
			vLower_Bounds = vStarting_Point - 0.5 * vDelta;
			uiRefines++;
		} while (uiRefines < uiRefines_Max);
		//XFIT_Simplex(vStarting_Point, vVariations, vEpsilon, Fit_Function, &cCall_Data, false, &vLower_Bounds, &vLower_Bounds_Valid, &vUpper_Bounds, &vUpper_Bounds_Valid);
		Fit_Function(vStarting_Point, &cCall_Data);
//		XFIT_Simplex(vStarting_Point, vVariations, vEpsilon, Fit_Function, &cCall_Data, false, &vLower_Bounds, &vLower_Bounds_Valid, &vUpper_Bounds, &vUpper_Bounds_Valid);
	}
//	xPerform_Fit_Bound_Simplex(cBounds[0],cBounds[1],cThreshold,Fit_Function,cResult,&cCall_Data,szCache.c_str());

//	printf("Generating\n");

	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Vel = vStarting_Point[1];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dPS_Temp = vStarting_Point[0];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dLog_S = vStarting_Point[2];
	o_cFit.m_cParams[specfit::comp_ejecta].m_dExcitation_Temp = 10000.0;

	if (uiParameters == 7)
	{
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Vel = vStarting_Point[3];
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Temp = vStarting_Point[4];
		o_cFit.m_cParams[specfit::comp_shell].m_dLog_S = vStarting_Point[5];
		o_cFit.m_cParams[specfit::comp_shell].m_dExcitation_Temp = 10000.0;
		o_cFit.m_cParams.m_dMixing_Fraction = vStarting_Point[6];
	}
	else if (uiParameters == 4)
	{
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Vel = vStarting_Point[1];
		o_cFit.m_cParams[specfit::comp_shell].m_dPS_Temp = vStarting_Point[0];
		o_cFit.m_cParams[specfit::comp_shell].m_dLog_S = vStarting_Point[3];
		o_cFit.m_cParams[specfit::comp_shell].m_dExcitation_Temp = 10000.0;
	}

	o_cFit.m_dMJD = i_cFit.m_dMJD;
	o_cFit.m_eFeature = i_cFit.m_eFeature;
	o_cFit.m_szInstrument = i_cFit.m_szInstrument;
	o_cFit.m_szSource = i_cFit.m_szSource;
	o_cFit.m_uiModel = i_cModel.m_uiModel_ID;

	ES::Spectrum csFull_Result(cFull_Target);
	ES::Spectrum csResult(cTarget);
	ES::Spectrum cContinuum(cFull_Target);
	ES::Spectrum cContinuum_EO(cFull_Target);
	ES::Spectrum cContinuum_SO(cFull_Target);
	ES::Spectrum csResult_EO(cFull_Target);
	ES::Spectrum csResult_SO(cFull_Target);
	ES::Spectrum cFull_Result_EO(cFull_Target);
	ES::Spectrum cFull_Result_SO(cFull_Target);
	csResult.zero_flux();
	cContinuum.zero_flux();
	csResult_EO.zero_flux();
	csResult_SO.zero_flux();
	csFull_Result.zero_flux();
	cFull_Result_EO.zero_flux();
	cFull_Result_SO.zero_flux();
	cContinuum_EO.zero_flux();
	cContinuum_SO.zero_flux();

	cCall_Data.m_cParam.m_dTime_After_Explosion = 1.0;
	cCall_Data.m_cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cCall_Data.m_cParam.m_dShell_Effective_Temperature_kK = 10.0;

	
	if (uiParameters == 7)
	{
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = -20.0;
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult_EO);

		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[3];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[4];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = -20.0;
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[5];
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult_SO);
		for (unsigned int uiI = 0; uiI < csResult_EO.size(); uiI++)
		{
			csResult.wl(uiI) = csResult_EO.wl(uiI);
			csResult.flux(uiI) = csResult_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + csResult_SO.flux(uiI) * vStarting_Point[6];
		}

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = std::get<0>(*(i_cFit.m_vData.begin()));
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = std::get<0>(*(i_cFit.m_vData.rbegin()));
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = -20.0;
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum_EO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_EO);

		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[3];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[4];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = -20.0;
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[5];
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum_SO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_SO);

		for (unsigned int uiI = 0; uiI < cContinuum_EO.size(); uiI++)
		{
			cContinuum.wl(uiI) = cContinuum_EO.wl(uiI);
			cContinuum.flux(uiI) = cContinuum_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + cContinuum_SO.flux(uiI) * vStarting_Point[6];

			csFull_Result.wl(uiI) = cFull_Result_EO.wl(uiI);
			csFull_Result.flux(uiI) = cFull_Result_EO.flux(uiI) * (1.0 - vStarting_Point[6]) + cFull_Result_SO.flux(uiI) * vStarting_Point[6];
		}
	}
	else
	{
		cCall_Data.m_cParam.m_dPhotosphere_Temp_kK = vStarting_Point[0];
		cCall_Data.m_cParam.m_dPhotosphere_Velocity_kkms = vStarting_Point[1];
		cCall_Data.m_cParam.m_dEjecta_Log_Scalar = vStarting_Point[2];
		cCall_Data.m_cParam.m_dShell_Log_Scalar = vStarting_Point[3];
		msdb_load_generate(cCall_Data.m_cParam, msdb::COMBINED, cTarget, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csResult);

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = std::get<0>(*(i_cFit.m_vData.begin()));
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = std::get<0>(*(i_cFit.m_vData.rbegin()));
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum);
		msdb_load_generate(cCall_Data.m_cParam, msdb::COMBINED, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csFull_Result);
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_EO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_SO);
	}

	//printf("...");fflush(stdout);
	Calc_Observables(csFull_Result,cContinuum,o_cFit);

	double dTarget_Flux, dGenerated_Flux;
	Get_Normalization_Fluxes(cFull_Target, csFull_Result, FIT_BLUE_WL, FIT_RED_WL, dTarget_Flux, dGenerated_Flux);

	double	dNorm = dTarget_Flux  / dGenerated_Flux;
	for (unsigned int uiI = 0; uiI < cFull_Target.size(); uiI++)
	{
		o_cFit.m_vpdSpectrum_Target.push_back(std::pair<double,double>(cFull_Target.wl(uiI),cFull_Target.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Ejecta_Only.push_back(std::pair<double,double>(cFull_Result_EO.wl(uiI),cFull_Result_EO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Shell_Only.push_back(std::pair<double,double>(cFull_Result_SO.wl(uiI),cFull_Result_SO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Continuum.push_back(std::pair<double,double>(cContinuum.wl(uiI),cContinuum.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic.push_back(std::pair<double,double>(csFull_Result.wl(uiI),csFull_Result.flux(uiI) * dNorm));
	}
	//feature_parameters cfpModel,cfpCont_Model;

	//dFit = pEW_Fit(cOutput,lpcCall_Data->m_fpTarget_Feature_Parameters,cfpModel) + Continuum_Fit(cOutput_Continuum,cTrue_Continuum,cfpCont_Model);

	std::vector<double> vError;
	for (unsigned int uiI = 0; uiI < cTarget.size(); uiI++)
		vError.push_back(cTarget.flux(uiI) - csResult.flux(uiI) * dNorm);
	
	Get_Raw_Moments(vError,6,o_cFit.m_vdRaw_Moments);
	Get_Central_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdCentral_Moments);
	Get_Standardized_Moments(o_cFit.m_vdRaw_Moments,o_cFit.m_vdStandardized_Moments);


	return o_cFit.m_vdRaw_Moments[1];
}

