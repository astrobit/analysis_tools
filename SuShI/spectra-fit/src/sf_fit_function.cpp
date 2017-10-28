#include <specfit.h>


double specfit::Fit_Function(const XVECTOR & i_vX, void * i_lpvSpectra_Fit_Data)
{
//	printf(".");fflush(stdout);
	fflush(stdout);
	double dFit = DBL_MAX;
	specfit::spectra_fit_data *lpcCall_Data = (specfit::spectra_fit_data *)i_lpvSpectra_Fit_Data;
	if (lpcCall_Data && lpcCall_Data->m_lpcTarget != nullptr && lpcCall_Data->m_lpcOpacity_Map_A != nullptr)
	{
		//std::cout << lpcCall_Data->m_lpcTarget->wl(0) << " " << lpcCall_Data->m_lpcTarget->flux(0) << std::endl;
		ES::Spectrum cOutput_Sh  = ES::Spectrum::create_from_spectrum( *lpcCall_Data->m_lpcTarget);
		ES::Spectrum cOutput_Ej  = ES::Spectrum::create_from_spectrum( *lpcCall_Data->m_lpcTarget);
		ES::Spectrum cOutput  = ES::Spectrum::create_from_spectrum( *lpcCall_Data->m_lpcTarget);

		ES::Spectrum cOutput_Continuum  = ES::Spectrum::create_from_range_and_size( lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang, lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang, lpcCall_Data->m_lpcTarget->size());
		ES::Spectrum cOutput_Continuum_Ej  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
		ES::Spectrum cOutput_Continuum_Sh  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
		ES::Spectrum cTarget_Continuum  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
		ES::Spectrum cTrue_Continuum  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
		ES::Spectrum cTrue_Continuum_Ej  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
		ES::Spectrum cTrue_Continuum_Sh  = ES::Spectrum::create_from_spectrum( cOutput_Continuum);
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
		Get_Normalization_Fluxes(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cNorm_Range, dTgt_Norm, dGen_Norm);
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
		specfit::params_range cParam_Range_Fit_Weak(cOutput.wl(0),dFlux_Min_Wl);
		specfit::params_range cParam_Range_Fit_Strong(dFlux_Min_Wl,cOutput.wl(cOutput.size() - 1));
		specfit::params_range cParam_Range_Fit_Continuum(lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Lower_Ang,lpcCall_Data->m_cContinuum_Band_Param.m_dWavelength_Range_Upper_Ang);

		if (lpcCall_Data->m_cFit_Range.m_dBlue_WL != -1 && lpcCall_Data->m_cFit_Range.m_dBlue_WL != -1)
		{
			dFit = Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cNorm_Range, lpcCall_Data->m_cFit_Range,2,true) ;
		}
		else
		{
			dFit = Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cNorm_Range, cParam_Range_Fit_Strong,2,true) ;
			dFit += Get_Fit(lpcCall_Data->m_lpcTarget[0], cOutput, lpcCall_Data->m_cNorm_Range, cParam_Range_Fit_Weak,2,false) * 0.1;
		}
		dFit += Get_Fit(cTarget_Continuum, cOutput_Continuum, lpcCall_Data->m_cNorm_Range, cParam_Range_Fit_Continuum, 2 ,false);

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
