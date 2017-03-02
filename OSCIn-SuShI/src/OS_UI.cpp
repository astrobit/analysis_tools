#include <OSCIn-SuShI>
#include <line_routines.h>
#include <cfloat>
#include <ctime>

void OSCIn_SuShI_main::on_key_down(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
	switch (eKey_ID)
	{
	case KEY_ESCAPE:
	case KEY_Q:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(quit_request);
		m_csEvent_Queue.Unset();
		break;
	}
}

void OSCIn_SuShI_main::on_key_up(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
}
void OSCIn_SuShI_main::on_mouse_button_double_click(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void OSCIn_SuShI_main::on_mouse_button_down(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
	PAIR<unsigned int> tMouse = i_tMouse_Position;
	pane_id idMouse_Pane = Find_Pane(tMouse);
	QUAD<unsigned int> qPane_Position = Get_Pane_Position(idMouse_Pane);
	PAIR<unsigned int> tMouse_TR = tMouse - qPane_Position.m_tTR;
	tMouse -= qPane_Position.m_tBL;
	double dPane_Scale = 1.0 / (double) qPane_Position.Get_Size().m_tY;
	PAIR<double> tMouse_Scaled = tMouse * dPane_Scale;
	bool bProcessed = false;
	if (idMouse_Pane == m_idError_Pane)
	{
		for (std::map<button_id,BUTTON_INFO>::iterator cI = m_mError_Pane_Buttons.begin(); cI != m_mError_Pane_Buttons.end() && !bProcessed; cI++)
		{
			if (cI->second.TestHit(tMouse_Scaled))
			{
				m_csEvent_Queue.Set();
				m_qEvent_List.push_back((button_id)(cI->second.GetID()));
				m_csEvent_Queue.Unset();
				bProcessed = true;
			}
		}
	}
	else //if (idMouse_Pane == m_idPane)
	{
		for (std::map<button_id,BUTTON_INFO>::iterator cI = m_mMain_Pane_Buttons.begin(); cI != m_mMain_Pane_Buttons.end() && !bProcessed; cI++)
		{
			if (cI->second.TestHit(tMouse_Scaled))
			{
				m_csEvent_Queue.Set();
				m_qEvent_List.push_back((button_id)(cI->second.GetID()));
				m_csEvent_Queue.Unset();
				bProcessed = true;
			}
		}
	}
}
void OSCIn_SuShI_main::on_mouse_button_up(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void OSCIn_SuShI_main::on_mousemove(const PAIR<unsigned int> &i_tMouse_Position)
{
}
void OSCIn_SuShI_main::on_mouse_wheel(MOUSEBUTTON i_eWheel, int i_iWheel_Delta, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void OSCIn_SuShI_main::on_timer(unsigned int i_uiTimer_ID, const double & i_dDelta_Time_s)
{
	if (!m_vsError_Info.empty())
	{
		Change_Pane_Visibility(m_idError_Pane,true);
	}
	m_csEvent_Queue.Set();
	while (!m_qEvent_List.empty())
	{
		button_id eEvent = m_qEvent_List.front();
		m_qEvent_List.pop_front();
		switch (eEvent)
		{
		case error_ack:
			m_vsError_Info.clear();
			Change_Pane_Visibility(m_idError_Pane,false);
			Request_Refresh();
			break;
		case spectrum_select_up:
			if (m_iterSelected_ID != m_iterLast_ID)
				m_iterSelected_ID++;
			else
				m_iterSelected_ID = m_OSCfile.m_mSpectra_List.begin();
			Process_Selected_Spectrum();
			Normalize();
			Request_Refresh();
			break;
		case spectrum_select_down:
			if (m_iterSelected_ID != m_OSCfile.m_mSpectra_List.begin())
				m_iterSelected_ID--;
			else
				m_iterSelected_ID = m_iterLast_ID;
			Process_Selected_Spectrum();
			Normalize();
			Request_Refresh();
			break;
		case feature_select_up:
			{
				unsigned int uiFScurr = (unsigned int)m_eFeature_Select;
				uiFScurr++;
				if ((feature_select)uiFScurr >= fs_end)
				{
					uiFScurr = (unsigned int)fs_start + 1;
				}
				m_eFeature_Select = (feature_select)uiFScurr;
			}
			Request_Refresh();
			break;
		case feature_select_down:
			{
				unsigned int uiFScurr = (unsigned int)m_eFeature_Select;
				uiFScurr--;
				if ((feature_select)uiFScurr >= fs_end || (feature_select)uiFScurr == fs_start)
				{
					uiFScurr = (unsigned int)fs_end - 1;
				}
				m_eFeature_Select = (feature_select)uiFScurr;
			}
			Request_Refresh();
			break;
		case model_select_up:
			switch (m_uiModel)
			{
			case 61:
				m_uiModel = 57;
				break;
			case 57:
				m_uiModel = 41;
				break;
			case 41:
				m_uiModel = 49;
				break;
			case 49:
				m_uiModel = 45;
				break;
			case 45:
				m_uiModel = 17;
				break;
			case 17:
				m_uiModel = 61;
				break;
			}
			Request_Refresh();
			break;
		case model_select_down:
			switch (m_uiModel)
			{
			case 17:
				m_uiModel = 45;
				break;
			case 61:
				m_uiModel = 17;
				break;
			case 57:
				m_uiModel = 61;
				break;
			case 41:
				m_uiModel = 57;
				break;
			case 49:
				m_uiModel = 41;
				break;
			case 45:
				m_uiModel = 49;
				break;
			}
			Request_Refresh();
			break;
		case temp_select_up:
			if (m_dPS_Temp < 25.0)
				m_dPS_Temp += 0.5;
			Request_Refresh();
			break;
		case temp_select_down:
			if (m_dPS_Temp > 5.0)
				m_dPS_Temp -= 0.5;
			Request_Refresh();
			break;
		case temp_select_big_up:
			m_dPS_Temp += 5.0;
			if (m_dPS_Temp > 25.0)
				m_dPS_Temp = 25.0;
			Request_Refresh();
			break;
		case temp_select_big_down:
				m_dPS_Temp -= 5.0;
			if (m_dPS_Temp < 5.0)
				m_dPS_Temp=  5.0;
			Request_Refresh();
			break;
		case elem_select_up:
			if (m_uiElement < 28)
				m_uiElement++;
			Request_Refresh();
			break;
		case elem_select_down:
			if (m_uiElement > 6)
				m_uiElement--;
			if ((m_uiElement - 1) < m_uiIon)
				m_uiIon = m_uiElement;
			Request_Refresh();
			break;
		case ion_select_up:
			if (m_uiIon < (m_uiElement - 1))
				m_uiIon++;
			Request_Refresh();
			break;
		case ion_select_down:
			if (m_uiIon > 0)
				m_uiIon--;
			Request_Refresh();
			break;
		case exc_temp_select_up:
			m_dExc_Temp += 0.5;
			Request_Refresh();
			break;
		case exc_temp_select_down:
			if (m_dExc_Temp > 0.0)
				m_dExc_Temp -= 0.5;
			Request_Refresh();
			break;
		case exc_temp_select_big_up:
			m_dExc_Temp += 5.0;
			Request_Refresh();
			break;
		case exc_temp_select_big_down:
			m_dExc_Temp -= 5.0;
			if (m_dExc_Temp < 0.0)
				m_dExc_Temp = 0.0;
			Request_Refresh();
			break;
		case velocity_select_big_up:
			m_dPS_Velocity += 5.0;
			if (m_dPS_Velocity > 30.0)
				m_dPS_Velocity = 30.0;
			Request_Refresh();
			break;
		case velocity_select_big_down:
			m_dPS_Velocity -= 5.0;
			if (m_dPS_Velocity < 5.0)
				m_dPS_Velocity = 5.0;
			Request_Refresh();
			break;
		case velocity_select_up:
			if (m_dPS_Velocity < 30.0)
				m_dPS_Velocity += 0.5;
			Request_Refresh();
			break;
		case velocity_select_down:
			if (m_dPS_Velocity > 5.0)
				m_dPS_Velocity -= 0.5;
			Request_Refresh();
			break;
		case shell_scalar_big_up:
			m_dShell_Scalar += 1.0;
			Request_Refresh();
			break;
		case shell_scalar_big_down:
			m_dShell_Scalar -= 1.0;
			Request_Refresh();
			break;
		case ejecta_scalar_big_up:
			m_dEjecta_Scalar += 1.0;
			Request_Refresh();
			break;
		case ejecta_scalar_big_down:
			m_dEjecta_Scalar -= 1.0;
			Request_Refresh();
			break;
		case shell_scalar_up:
			m_dShell_Scalar += 0.05;
			Request_Refresh();
			break;
		case shell_scalar_down:
			m_dShell_Scalar -= 0.05;
			Request_Refresh();
			break;
		case ejecta_scalar_up:
			m_dEjecta_Scalar += 0.05;
			Request_Refresh();
			break;
		case ejecta_scalar_down:
			m_dEjecta_Scalar -= 0.05;
			Request_Refresh();
			break;
		case norm_wl_blue_up:
			if (m_dNorm_Blue < (m_dNorm_Red - 10.0))
				m_dNorm_Blue += 10.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_blue_down:
			m_dNorm_Blue -= 10.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_red_up:
			m_dNorm_Red += 10.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_red_down:
			if (m_dNorm_Red > (m_dNorm_Blue + 10.0))
				m_dNorm_Red -= 10.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_blue_big_up:
			if (m_dNorm_Blue < (m_dNorm_Red - 100.0))
				m_dNorm_Blue += 100.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_blue_big_down:
			m_dNorm_Blue -= 100.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_red_big_up:
			m_dNorm_Red += 100.0;
			Normalize();
			Request_Refresh();
			break;
		case norm_wl_red_big_down:
			if (m_dNorm_Red > (m_dNorm_Blue + 100.0))
				m_dNorm_Red -= 100.0;
			Normalize();
			Request_Refresh();
			break;
		case ref_fit_range_blue_up:
			if (m_dRefine_Blue < (m_dNorm_Red - 10.0))
				m_dRefine_Blue += 10.0;
			Request_Refresh();
			break;
		case ref_fit_range_blue_down:
			m_dRefine_Blue -= 10.0;
			Request_Refresh();
			break;
		case ref_fit_range_red_up:
			m_dRefine_Red += 10.0;
			Request_Refresh();
			break;
		case ref_fit_range_red_down:
			if (m_dRefine_Red > (m_dRefine_Blue + 10.0))
				m_dRefine_Red -= 10.0;
			Request_Refresh();
			break;
		case ref_fit_range_blue_big_up:
			if (m_dRefine_Blue < (m_dRefine_Red - 100.0))
				m_dRefine_Blue += 100.0;
			Request_Refresh();
			break;
		case ref_fit_range_blue_big_down:
			m_dRefine_Blue -= 100.0;
			Request_Refresh();
			break;
		case ref_fit_range_red_big_up:
			m_dRefine_Red += 100.0;
			Request_Refresh();
			break;
		case ref_fit_range_red_big_down:
			if (m_dRefine_Red > (m_dRefine_Blue + 100.0))
				m_dRefine_Red -= 100.0;
			Request_Refresh();
			break;
		case save_request:
			{
				Save_Result("results.csv",m_uiModel,m_sdGenerated_Spectrum,false);
			}
			break;
		case save_refine_request:
			if (!g_bRefine_In_Progress && m_sdRefine_Spectrum_Best.m_bValid)
			{
				Save_Result("refine_results.csv",m_uiModel,m_sdRefine_Spectrum_Best,true);
			}
			break;
		case gen_request:
			if (!g_bGen_In_Progress)
			{
				g_sdGen_Spectrum_Result.m_dPS_Temp = m_dPS_Temp;
				g_sdGen_Spectrum_Result.m_dPS_Velocity = m_dPS_Velocity;
				g_sdGen_Spectrum_Result.m_dShell_Scalar = m_dShell_Scalar;
				g_sdGen_Spectrum_Result.m_dEjecta_Scalar = m_dEjecta_Scalar;
				g_sdGen_Spectrum_Result.m_dExc_Temp = m_dExc_Temp;
				g_sdGen_Spectrum_Result.m_uiIon = m_uiElement * 100 + m_uiIon;//m_uiModel;
				g_sdGen_Spectrum_Result.m_dNorm_WL_Blue = m_dNorm_Blue;
				g_sdGen_Spectrum_Result.m_dNorm_WL_Red = m_dNorm_Red;
				g_sdGen_Spectrum_Result.m_dFit_WL_Blue = m_dRefine_Blue;
				g_sdGen_Spectrum_Result.m_dFit_WL_Red = m_dRefine_Red;
				g_lpmGen_Model = &m_mapModels[m_uiModel];
				g_sdGen_Spectrum_Result.m_specResult[0] = ES::Spectrum::create_copy_from_vector(m_specSelected_Spectrum.Get_Tuple_Vector());
				g_sdGen_Spectrum_Result.m_bValid = true;
				g_bGen_Process_Request = true;
			}
			break;
		case copy_refine_request:
			if (!g_bRefine_In_Progress && m_sdRefine_Spectrum_Best.m_bValid)
			{
				m_dPS_Temp = XRoundNearest(g_sdRefine_Result.m_dPS_Temp,0.01);
				m_dPS_Velocity = XRoundNearest(g_sdRefine_Result.m_dPS_Velocity,0.01);
				m_dShell_Scalar = XRoundNearest(g_sdRefine_Result.m_dShell_Scalar,0.01);
				m_dEjecta_Scalar = XRoundNearest(g_sdRefine_Result.m_dEjecta_Scalar,0.01);
			}
			break;
		case refine_request:
			if (!g_bRefine_In_Progress)
			{
				g_sdRefine_Result.m_dPS_Temp = m_dPS_Temp;
				g_sdRefine_Result.m_dPS_Velocity = m_dPS_Velocity;
				g_sdRefine_Result.m_dShell_Scalar = m_dShell_Scalar;
				g_sdRefine_Result.m_dEjecta_Scalar = m_dEjecta_Scalar;
				g_sdRefine_Result.m_dExc_Temp = m_dExc_Temp;
				g_sdRefine_Result.m_dFit_WL_Blue = m_dRefine_Blue;
				g_sdRefine_Result.m_dFit_WL_Red = m_dRefine_Red;
				g_sdRefine_Result.m_dNorm_WL_Blue = m_dNorm_Blue;
				g_sdRefine_Result.m_dNorm_WL_Red = m_dNorm_Red;
				g_sdRefine_Result.m_uiIon = m_uiElement * 100 + m_uiIon;//m_uiModel;
				g_lpmRefine_Model = &m_mapModels[m_uiModel];
				g_sdRefine_Result.m_specResult[0] = ES::Spectrum::create_copy_from_vector(m_specSelected_Spectrum.Get_Tuple_Vector());
				g_sdRefine_Result.m_bValid = true;
				g_bRefine_Process_Request = true;
			}
			break;
		case gauss_fit_data_range_blue_big_up:
			if (m_dGauss_Fit_Blue < (m_dGauss_Fit_Red - 100.0))
				m_dGauss_Fit_Blue += 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_blue_up:
			if (m_dGauss_Fit_Blue < (m_dGauss_Fit_Red - 5.0))
				m_dGauss_Fit_Blue += 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_blue_big_down:
			if (m_dGauss_Fit_Blue > 100.0)
				m_dGauss_Fit_Blue -= 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_blue_down:
			if (m_dGauss_Fit_Blue > 5.0)
				m_dGauss_Fit_Blue -= 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_red_big_up:
			if (m_dGauss_Fit_Red < 14900.0)
				m_dGauss_Fit_Red += 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_red_up:
			if (m_dGauss_Fit_Red < 14995.0)
				m_dGauss_Fit_Red += 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_red_big_down:
			if (m_dGauss_Fit_Red > (m_dGauss_Fit_Blue + 100.0))
				m_dGauss_Fit_Red -= 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_data_range_red_down:
			if (m_dGauss_Fit_Red > (m_dGauss_Fit_Blue + 5.0))
				m_dGauss_Fit_Red -= 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_blue_big_up:
			if (m_dGauss_Fit_Gen_Blue < (m_dGauss_Fit_Gen_Red - 100.0))
				m_dGauss_Fit_Gen_Blue += 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_blue_up:
			if (m_dGauss_Fit_Gen_Blue < (m_dGauss_Fit_Gen_Red - 5.0))
				m_dGauss_Fit_Gen_Blue += 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_blue_big_down:
			if (m_dGauss_Fit_Gen_Blue > 100.0)
				m_dGauss_Fit_Gen_Blue -= 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_blue_down:
			if (m_dGauss_Fit_Gen_Blue > 5.0)
				m_dGauss_Fit_Gen_Blue -= 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_red_big_up:
			if (m_dGauss_Fit_Gen_Red < 14900.0)
				m_dGauss_Fit_Gen_Red += 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_red_up:
			if (m_dGauss_Fit_Gen_Red < 14995.0)
				m_dGauss_Fit_Gen_Red += 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_red_big_down:
			if (m_dGauss_Fit_Gen_Red > (m_dGauss_Fit_Gen_Blue + 100.0))
				m_dGauss_Fit_Gen_Red -= 100.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case gauss_fit_gen_range_red_down:
			if (m_dGauss_Fit_Gen_Red > (m_dGauss_Fit_Gen_Blue + 5.0))
				m_dGauss_Fit_Gen_Red -= 5.0;
			Calc_Observed_pEW();
			Request_Refresh();
			break;
		case flux_zoom_in:
			m_dFlux_Zoom *= 2.0;
			Request_Refresh();
			break;
		case flux_zoom_out:
			m_dFlux_Zoom *= 0.5;
			Request_Refresh();
			break;
		case clear_request:
			m_sdGenerated_Spectrum_Prev.m_bValid = false;
			m_cTarget_Gaussian_Fit.m_bValid = false;
			m_cModel_Gaussian_Fit.m_bValid = false;
			break;
		case generated_fit_request:
			if (!g_bFit_In_Progress && (m_eFeature_Select == fs_CaHK || m_eFeature_Select == fs_Si6355 || m_eFeature_Select == fs_CaNIR) && !g_bRefine_In_Progress && !g_bGen_In_Progress)
			{
				g_cFit_Result.m_bType = true;
				g_cFit_Result.m_dRange_WL_Blue = m_dGauss_Fit_Gen_Blue;
				g_cFit_Result.m_dRange_WL_Red = m_dGauss_Fit_Gen_Red;
				g_cFit_Result.m_eFeature = m_eFeature_Select;
				g_cFit_Result.m_specResult = Copy(m_sdGenerated_Spectrum.m_specResult[0]);
				g_cFit_Result.m_bValid = true;
				g_bFit_Process_Request = true;
			}
			break;
		case data_fit_request:
			if (!g_bFit_In_Progress && (m_eFeature_Select == fs_CaHK || m_eFeature_Select == fs_Si6355 || m_eFeature_Select == fs_CaNIR))
			{
				g_cFit_Result.m_bType = false;
				g_cFit_Result.m_dRange_WL_Blue = m_dGauss_Fit_Blue;
				g_cFit_Result.m_dRange_WL_Red = m_dGauss_Fit_Red;
				g_cFit_Result.m_eFeature = m_eFeature_Select;
				g_cFit_Result.m_specResult = m_specSelected_Spectrum;
				g_cFit_Result.m_bValid = true;
				g_bFit_Process_Request = true;
			}
			break;
		case toggle_component_display_request:
			m_bDisplay_Components = !m_bDisplay_Components;
			break;
		case abort_request:
			if (g_bRefine_In_Progress)
				g_bAbort_Request = true;
			break;
		case quit_request:
			if (g_bRefine_In_Progress)
				g_bAbort_Request = true;
			g_bQuit_Thread = true;
			m_bQuit_Request_Pending = true;
			break;
		}
	}
	m_csEvent_Queue.Unset();

	if (m_bQuit_Request_Pending && !g_bGen_Thread_Running)
	{
		Request_Quit();
	}
	if (g_bGen_Done)
	{
		m_sdGenerated_Spectrum_Prev = m_sdGenerated_Spectrum;
		m_sdGenerated_Spectrum = g_sdGen_Spectrum_Result;
		Normalize();
		Calc_Observed_pEW();
		g_bGen_Done = false;
		Request_Refresh();
	}
	m_dTimer += i_dDelta_Time_s;
	bool bFlasher = fmod(m_dTimer,1.0) < 0.5;
	if (bFlasher != m_bFlasher_1s_50p)
		Request_Refresh();
	m_bFlasher_1s_50p = bFlasher;

	if (g_bRefine_Done)
	{
		m_sdRefine_Spectrum_Best = g_sdRefine_Result;
		m_sdRefine_Spectrum_Curr.m_bValid = false;
		Normalize();
		Calc_Observed_pEW();
		g_bRefine_Done = false;
		Request_Refresh();
	}
	if (g_bRefine_In_Progress && (g_uiRefine_Result_ID != m_uiRefine_Result_ID) && g_uiRefine_Result_ID != 0)
	{
		m_uiRefine_Result_ID = g_uiRefine_Result_ID;
		m_sdRefine_Spectrum_Best = g_sdRefine_Result_Curr_Best;
		m_sdRefine_Spectrum_Curr = g_sdRefine_Result_Curr;
		Normalize();
		Request_Refresh();
	}
	if (g_bFit_Done)
	{
		g_bFit_Done = false;
		if (g_cFit_Result.m_bValid)
		{
			if (g_cFit_Result.m_bType)
				m_cModel_Gaussian_Fit = g_cFit_Result;
			else
				m_cTarget_Gaussian_Fit = g_cFit_Result;
		}
		Calc_Observed_pEW();
		Request_Refresh();
	}

}

