#include <specfit.h>
#include <xlinalg.h>
#include <xmath.h>
#include <line_routines.h>
#include <sstream>
#include <xfit.h>
#include <fstream>
#include <iomanip>
#include <algorithm>

double	specfit::g_dTarget_Normalization_Flux = -1;
double	specfit::g_dGenerated_Normalization_Flux = -1;

double specfit::GenerateFit(const fit & i_cFit, const model & i_cModel, fit_result & o_cFit, bool i_bDebug, const param_set * i_lppsEjecta, const param_set * i_lppsShell,const bool * i_lpbPerform_Single_Fit)
{


	specfit::spectra_fit_data cCall_Data;

	cCall_Data.m_cNorm_Range = i_cFit.m_cNorm_Range;
	cCall_Data.m_cFit_Range = i_cFit.m_cFit_Range;

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
			for (auto iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.cend() && !bQuit; iterI++)
			{
				double dWL = iterI->wl();
				if (dWL > 8000.0)
					bQuit = true;
				else if (dWL > 7500.0)
				{
					double dFlux = iterI->flux();
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
			for (auto iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.cend() && !bQuit; iterI++)
			{
				double dWL = iterI->wl();
				if (dWL > 8900.0)
					bQuit = true;
				else if (dWL > 8400.0)
				{
					double dFlux = iterI->flux();
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
			double dFlux_O_Peak = i_cFit.m_vData[uiO_Idx].flux();
			double dWL_O_Peak = i_cFit.m_vData[uiO_Idx].wl();
			double dFlux_Ca_Peak = i_cFit.m_vData[uiCaII_Idx].flux();
			double dWL_Ca_Peak = i_cFit.m_vData[uiCaII_Idx].wl();
			double	dSlope = (dFlux_Ca_Peak - dFlux_O_Peak) / (dWL_Ca_Peak - dWL_O_Peak);
			
			double dDel_WL_Ang = (i_cFit.m_vData[uiO_Idx + 1].wl() - i_cFit.m_vData[uiO_Idx].wl());
			o_cFit.m_cTarget_Observables.m_fpParameters.Reset();
			for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
			{
				double dWL = i_cFit.m_vData[uiJ + uiO_Idx].wl();
				double dFlux = i_cFit.m_vData[uiJ + uiO_Idx].flux();
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
	double dRange_Min = -1.0;
	double dRange_Max = -1.0;
	if (i_cFit.m_cNorm_Range.m_dBlue_WL != -1 && i_cFit.m_cNorm_Range.m_dRed_WL != -1)
	{
		dRange_Min = i_cFit.m_cNorm_Range.m_dBlue_WL;
		dRange_Max = i_cFit.m_cNorm_Range.m_dRed_WL;

	}
	if (i_cFit.m_cFit_Range.m_dBlue_WL != -1 && i_cFit.m_cFit_Range.m_dRed_WL != -1)
	{
		if (dRange_Min == -1)
		{
			dRange_Min = i_cFit.m_cNorm_Range.m_dBlue_WL;
			dRange_Max = i_cFit.m_cNorm_Range.m_dRed_WL;
		}
		else
		{
			dRange_Min = std::min(i_cFit.m_cNorm_Range.m_dBlue_WL,dRange_Min);
			dRange_Max = std::max(i_cFit.m_cNorm_Range.m_dRed_WL,dRange_Max);
		}

	}
	if (dRange_Min != -1)
	{
		dBlue_Edge_WL = cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = dRange_Min;
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = dRange_Max;
	}
//		std::cout << "Ejecta map: " << std::scientific << cCall_Data.m_lpcOpacity_Map_A[0].GetElement(1,250) << std::endl;
	//std::string szCache = ".fitcache";
	ES::Spectrum cTarget;
	ES::Spectrum cFull_Target;
	cCall_Data.m_lpcTarget = &cTarget;
	if (!i_cFit.m_vData.empty())
	{
		unsigned int uiCount =  0;
		OSCspectrum vSpec_Subset;
		double dWL_Min = DBL_MAX, dWL_Max =-DBL_MAX;
		for (auto iterI = i_cFit.m_vData.cbegin(); iterI != i_cFit.m_vData.cend(); iterI++)
		{
			double dWL = iterI->wl();
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
		cTarget = ES::Spectrum::create_copy_from_vector(vSpec_Subset.Get_Tuple_Vector());
		cFull_Target = ES::Spectrum::create_copy_from_vector(i_cFit.m_vData.Get_Tuple_Vector());
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
		vStarting_Point.Set(0,(dTemp_Low + dTemp_Hi) * 0.5 * 1e-3);
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
	if (i_lpbPerform_Single_Fit == nullptr || !i_lpbPerform_Single_Fit[0])
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
		vStarting_Point.Set(2,i_lppsEjecta->m_dLog_S);
		vStarting_Point.Set(3,i_lppsShell->m_dLog_S);

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
		vStarting_Point.Set(2,i_lppsEjecta->m_dLog_S);
		vStarting_Point.Set(3,i_lppsShell->m_dLog_S);

		Fit_Function(vStarting_Point, &cCall_Data);
	}
	else
	{
		spectrum_data cResult;
		spectrum_data cCurr_Result;
		spectrum_data cBest_Result;
		unsigned int uiMax_Refine;
		bool bAbort = false;
		unsigned int uiResult_ID;


		cResult.m_bValid = true;
		cResult.m_dPS_Temp = vStarting_Point.Get(0);
		cResult.m_dPS_Velocity = vStarting_Point.Get(1);
		cResult.m_dEjecta_Scalar = vStarting_Point.Get(2);
		cResult.m_dShell_Scalar = vStarting_Point.Get(3);
		cResult.m_uiIon = cCall_Data.m_cParam.m_uiIon;

		cResult.m_dExc_Temp = 10.0;

		cResult.m_dFit_WL_Blue = i_cFit.m_cFit_Range.m_dBlue_WL;
		cResult.m_dFit_WL_Red = i_cFit.m_cFit_Range.m_dRed_WL;

		cResult.m_dNorm_WL_Blue = i_cFit.m_cNorm_Range.m_dRed_WL;
		cResult.m_dNorm_WL_Red = i_cFit.m_cNorm_Range.m_dRed_WL;
		cResult.m_specResult[0] = cTarget;

		Grid_Refine_Fit(
			uiResult_ID,
			cResult,
			&i_cModel,
			cCurr_Result,
			cBest_Result,
			uiMax_Refine,
			bAbort);
		vStarting_Point.Set(0,cResult.m_dPS_Temp);
		vStarting_Point.Set(1,cResult.m_dPS_Velocity);
		vStarting_Point.Set(2,cResult.m_dEjecta_Scalar);
		vStarting_Point.Set(2,cResult.m_dShell_Scalar);

	}

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

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = i_cFit.m_vData.cbegin()->wl();
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = i_cFit.m_vData.crbegin()->wl();
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

		cCall_Data.m_cParam.m_dWavelength_Range_Lower_Ang = i_cFit.m_vData.cbegin()->wl();
		cCall_Data.m_cParam.m_dWavelength_Range_Upper_Ang = i_cFit.m_vData.crbegin()->wl();
		msdb_load_generate(cCall_Data.m_cParam, msdb::CONTINUUM, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cContinuum);
		msdb_load_generate(cCall_Data.m_cParam, msdb::COMBINED, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, csFull_Result);
		msdb_load_generate(cCall_Data.m_cParam, msdb::EJECTA_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_EO);
		msdb_load_generate(cCall_Data.m_cParam, msdb::SHELL_ONLY, cFull_Target, cCall_Data.m_lpcOpacity_Map_A, cCall_Data.m_lpcOpacity_Map_B, cFull_Result_SO);
	}

	//printf("...");fflush(stdout);
	Calc_Observables(csFull_Result,cContinuum,o_cFit);

	double dTarget_Flux, dGenerated_Flux;
	Get_Normalization_Fluxes(cFull_Target, csFull_Result, i_cFit.m_cNorm_Range, dTarget_Flux, dGenerated_Flux);

	double	dNorm = dTarget_Flux  / dGenerated_Flux;
	for (unsigned int uiI = 0; uiI < cFull_Target.size(); uiI++)
	{
		o_cFit.m_vpdSpectrum_Target.push_back(OSCspectrum_dp(cFull_Target.wl(uiI),cFull_Target.flux(uiI)));
		o_cFit.m_vpdSpectrum_Synthetic_Ejecta_Only.push_back(OSCspectrum_dp(cFull_Result_EO.wl(uiI),cFull_Result_EO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Shell_Only.push_back(OSCspectrum_dp(cFull_Result_SO.wl(uiI),cFull_Result_SO.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic_Continuum.push_back(OSCspectrum_dp(cContinuum.wl(uiI),cContinuum.flux(uiI) * dNorm));
		o_cFit.m_vpdSpectrum_Synthetic.push_back(OSCspectrum_dp(csFull_Result.wl(uiI),csFull_Result.flux(uiI) * dNorm));
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

