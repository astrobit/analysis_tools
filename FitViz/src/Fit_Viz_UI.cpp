#include <FitViz.hpp>
#include <line_routines.h>

void FIT_VIZ_MAIN::on_key_down(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
	switch (eKey_ID)
	{
	case KEY_ESCAPE:
	case KEY_Q:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(QUIT_REQUEST);
		m_csEvent_Queue.Unset();
		break;
	case KEY_UP:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(MODEL_SELECT_UP);
		m_csEvent_Queue.Unset();
		break;
	case KEY_DOWN:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(MODEL_SELECT_DOWN);
		m_csEvent_Queue.Unset();
		break;
	case KEY_LEFT:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(DAY_SELECT_DOWN);
		m_csEvent_Queue.Unset();
		break;
	case KEY_RIGHT:
		m_csEvent_Queue.Set();
		m_qEvent_List.push_back(DAY_SELECT_UP);
		m_csEvent_Queue.Unset();
		break;
	case KEY_SPACE:
		m_csEvent_Queue.Set();
		if (m_eMan_Select_Mode != MS_OFF)
			m_qEvent_List.push_back(MODEL_DISPLAY_AREA);
		else
			m_qEvent_List.push_back(METHOD_TOGGLE);

		m_csEvent_Queue.Unset();
		break;
	case KEY_ENTER:
		m_csEvent_Queue.Set();
		if (m_eMan_Select_Mode != MS_OFF)
			m_qEvent_List.push_back(MODEL_DISPLAY_AREA);
		m_csEvent_Queue.Unset();
		break;
	}
}

void FIT_VIZ_MAIN::on_key_up(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
}
void FIT_VIZ_MAIN::on_mouse_button_double_click(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void FIT_VIZ_MAIN::on_mouse_button_down(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
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
	if (!bProcessed && m_eMan_Select_Mode != MS_OFF)
		m_eMan_Select_Mode = MS_OFF;
}
void FIT_VIZ_MAIN::on_mouse_button_up(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void FIT_VIZ_MAIN::on_mousemove(const PAIR<unsigned int> &i_tMouse_Position)
{
	if (m_eMan_Select_Mode != MS_OFF)
	{
		PAIR<unsigned int> tMouse = i_tMouse_Position;
		pane_id idMouse_Pane = Find_Pane(tMouse);
		QUAD<unsigned int> qPane_Position = Get_Pane_Position(idMouse_Pane);
		PAIR<unsigned int> tMouse_TR = tMouse - qPane_Position.m_tTR;
		tMouse -= qPane_Position.m_tBL;
		double dPane_Scale = 1.0 / (double) qPane_Position.Get_Size().m_tY;
		PAIR<double> tMouse_Scaled = tMouse * dPane_Scale;
		bool bProcessed = false;
		if (idMouse_Pane == m_idPane)
		{
			double dSize = Get_Pane_Aspect_Ratio(m_idPane);
			double dWL_scale = 0.90 * dSize / (m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]);
			double dWL = (tMouse_Scaled.m_tX - 0.1) / dWL_scale + m_vWavelength[0];
			
			unsigned int uiI = 0;
			while (uiI < m_vWavelength.size() && m_vWavelength[uiI] < dWL)
				uiI++;
			switch (m_eMan_Select_Mode)
			{
			case MS_BLUE:
				if (m_uiManual_Fit_Red_Idx == -1 && m_uiManual_Fit_Central_Idx == -1)
					m_uiManual_Fit_Blue_Idx = uiI;
				else if (m_uiManual_Fit_Central_Idx != -1)
				{
					if (m_uiManual_Fit_Central_Idx > uiI)
					{
						m_uiManual_Fit_Blue_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Central_Idx > 0)
							m_uiManual_Fit_Blue_Idx = m_uiManual_Fit_Central_Idx - 1;
						else
							m_uiManual_Fit_Blue_Idx = 0;
					}	
				}
				else //if (m_uiManual_Fit_Red_Idx != -1)
				{
					if (m_uiManual_Fit_Red_Idx > uiI)
					{
						m_uiManual_Fit_Blue_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Red_Idx > 0)
							m_uiManual_Fit_Blue_Idx = m_uiManual_Fit_Red_Idx - 1;
						else
							m_uiManual_Fit_Blue_Idx = 0;
					}	
				}
				break;
			case MS_RED:
				if (m_uiManual_Fit_Blue_Idx == -1 && m_uiManual_Fit_Central_Idx == -1)
					m_uiManual_Fit_Red_Idx = uiI;
				else if (m_uiManual_Fit_Central_Idx != -1)
				{
					if (m_uiManual_Fit_Central_Idx < uiI)
					{
						m_uiManual_Fit_Red_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Central_Idx < (m_vWavelength.size() - 2))
							m_uiManual_Fit_Red_Idx = m_uiManual_Fit_Central_Idx + 1;
						else
							m_uiManual_Fit_Red_Idx = m_uiManual_Fit_Central_Idx;
					}	
				}
				else //if (m_uiManual_Fit_Red_Idx != -1)
				{
					if (m_uiManual_Fit_Blue_Idx < uiI)
					{
						m_uiManual_Fit_Red_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Blue_Idx < (m_vWavelength.size() - 2))
							m_uiManual_Fit_Red_Idx = m_uiManual_Fit_Blue_Idx + 1;
						else
							m_uiManual_Fit_Red_Idx = m_uiManual_Fit_Blue_Idx;
					}	
				}
				break;
			case MS_CENTER:
				if (m_uiManual_Fit_Blue_Idx == -1 && m_uiManual_Fit_Red_Idx == -1)
					m_uiManual_Fit_Central_Idx = uiI;
				else if (m_uiManual_Fit_Blue_Idx != -1)
				{
					if (m_uiManual_Fit_Blue_Idx < uiI)
					{
						m_uiManual_Fit_Central_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Blue_Idx < (m_vWavelength.size() - 2))
							m_uiManual_Fit_Central_Idx = m_uiManual_Fit_Blue_Idx + 1;
						else
							m_uiManual_Fit_Central_Idx = m_uiManual_Fit_Blue_Idx;
					}	
				}
				else //if (m_uiManual_Fit_Red_Idx != -1)
				{
					if (m_uiManual_Fit_Red_Idx > uiI)
					{
						m_uiManual_Fit_Central_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Red_Idx > 0)
							m_uiManual_Fit_Central_Idx = m_uiManual_Fit_Red_Idx - 1;
						else
							m_uiManual_Fit_Central_Idx = 0;
					}	
				}
				break;
			case MS_CENTER_SECOND:
				if (m_uiManual_Fit_Blue_Idx == -1 && m_uiManual_Fit_Red_Idx == -1)
					m_uiManual_Fit_Central_Second_Idx = uiI;
				else if (m_uiManual_Fit_Blue_Idx != -1)
				{
					if (m_uiManual_Fit_Blue_Idx < uiI)
					{
						m_uiManual_Fit_Central_Second_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Blue_Idx < (m_vWavelength.size() - 2))
							m_uiManual_Fit_Central_Second_Idx = m_uiManual_Fit_Blue_Idx + 1;
						else
							m_uiManual_Fit_Central_Second_Idx = m_uiManual_Fit_Blue_Idx;
					}	
				}
				else //if (m_uiManual_Fit_Red_Idx != -1)
				{
					if (m_uiManual_Fit_Red_Idx > uiI)
					{
						m_uiManual_Fit_Central_Second_Idx = uiI;
					}
					else
					{
						if (m_uiManual_Fit_Red_Idx > 0)
							m_uiManual_Fit_Central_Second_Idx = m_uiManual_Fit_Red_Idx - 1;
						else
							m_uiManual_Fit_Central_Second_Idx = 0;
					}	
				}
				break;
			}
			if (m_uiManual_Fit_Blue_Idx != -1 && m_uiManual_Fit_Red_Idx != -1)
			{
				m_dMF_WL = m_vWavelength[m_uiManual_Fit_Blue_Idx];
				m_dMF_Flux = m_vFlux[m_uiManual_Fit_Blue_Idx];

				m_dMF_Slope = (m_vFlux[m_uiManual_Fit_Red_Idx] - m_vFlux[m_uiManual_Fit_Blue_Idx]) / (m_vWavelength[m_uiManual_Fit_Red_Idx] - m_vWavelength[m_uiManual_Fit_Blue_Idx]);
			}
			Request_Refresh();
		}
	}

}
void FIT_VIZ_MAIN::on_mouse_wheel(MOUSEBUTTON i_eWheel, int i_iWheel_Delta, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void FIT_VIZ_MAIN::on_timer(unsigned int i_uiTimer_ID, const double & i_dDelta_Time_s)
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
		case ERROR_ACK:
			m_vsError_Info.clear();
			Change_Pane_Visibility(m_idError_Pane,false);
			Request_Refresh();
			break;
		case MODEL_SELECT_UP:
			if (!m_vModel_List.empty())
			{
				if (m_uiSelected_Model < (m_vModel_List.size() - 1))
					m_uiSelected_Model++;
				Load_Display_Info();
				Request_Refresh();
			}
			break;
		case MODEL_SELECT_DOWN:
			if (!m_vModel_List.empty())
			{
				if (m_uiSelected_Model > 0)
					m_uiSelected_Model--;
				Load_Display_Info();
				Request_Refresh();
			}
			break;
		case DAY_SELECT_UP:
			if (!m_vDay_List.empty())
			{
				if (m_uiSelected_Day < (m_vDay_List.size() - 1))
					m_uiSelected_Day++;
				Load_Display_Info();
				Request_Refresh();
			}
			break;
		case DAY_SELECT_DOWN:
			if (!m_vDay_List.empty())
			{
				if (m_uiSelected_Day > 0)
					m_uiSelected_Day--;
				Load_Display_Info();
				Request_Refresh();
			}
			break;
		case METHOD_FLAT:
			m_eFit_Method = fm_flat;
			Load_Display_Info();
			Request_Refresh();
			break;
		case METHOD_JEFF:
			m_eFit_Method = fm_jeff;
			Load_Display_Info();
			Request_Refresh();
			break;
		case METHOD_MANUAL:
			if (m_eFit_Method != fm_manual)
			{
				m_eFit_Method = fm_manual;
				Load_Display_Info();
			}
			Request_Refresh();
			break;
		case METHOD_TOGGLE:
			if (m_eFit_Method == fm_flat)
				m_eFit_Method = fm_jeff;
			else
				m_eFit_Method = fm_flat;
			Load_Display_Info();
			Request_Refresh();
			break;
		case DISPLAY_SHELL_COMPONENT:
			m_bDisplay_Shell_Component = !m_bDisplay_Shell_Component;
			Request_Refresh();
			break;
		case DISPLAY_EJECTA_COMPONENT:
			m_bDisplay_Ejecta_Component = !m_bDisplay_Ejecta_Component;
			Request_Refresh();
			break;
		case QUIT_REQUEST:
			g_bQuit_Thread = true;
			m_bQuit_Request_Pending = true;
			break;
		case MAN_FIT_RED:
			m_eMan_Select_Mode = MS_RED;
			break;
		case MAN_FIT_BLUE:
			m_eMan_Select_Mode = MS_BLUE;
			break;
		case MAN_FIT_CENTER:
			m_eMan_Select_Mode = MS_CENTER;
			break;
		case MAN_FIT_CENTER_SECOND:
			m_eMan_Select_Mode = MS_CENTER_SECOND;
			break;
		case MAN_FIT_EXEC:
			if (m_uiManual_Fit_Blue_Idx != -1 && m_uiManual_Fit_Red_Idx != -1 && m_uiManual_Fit_Central_Idx != -1)
			{
				std::vector<double> vWL_Data;
				std::vector<double> vFlux_Data;
				std::vector<double> vError_Data;
				double dFlux_Center_WL = (m_vWavelength[m_uiManual_Fit_Central_Idx] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux;
				double	dMod_Flux_Center = m_vFlux[m_uiManual_Fit_Central_Idx] - dFlux_Center_WL;


				for (unsigned int uiI = m_uiManual_Fit_Blue_Idx; uiI < m_uiManual_Fit_Red_Idx; uiI++)
				{
					vWL_Data.push_back(m_vWavelength[uiI]);
					double dFlux_WL = (m_vWavelength[uiI] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux;
					vFlux_Data.push_back(m_vFlux[uiI] - dFlux_WL);
					vError_Data.push_back(0.001 * (m_vFlux[uiI] - dFlux_WL));
				}
				XVECTOR vX,vY,vW,vSigma; 
				double dpEW_PVF, dpEW_HVF, dV_PVF, dV_HVF,dSmin;
				g_vX = vWL_Data;
				g_vY = vFlux_Data;
				g_vW = vError_Data;
				g_dSuggested_Center_WL = m_vWavelength[m_uiManual_Fit_Central_Idx];
				g_dSuggested_Center_Flux = dMod_Flux_Center;
				if (m_uiManual_Fit_Central_Second_Idx != -1)
				{
					double dFlux_Center_Second_WL = (m_vWavelength[m_uiManual_Fit_Central_Second_Idx] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux;
					double	dMod_Flux_Center_Second = m_vFlux[m_uiManual_Fit_Central_Second_Idx] - dFlux_Center_Second_WL;
					g_dSuggested_Center_Second_WL = m_vWavelength[m_uiManual_Fit_Central_Second_Idx];
					g_dSuggested_Center_Second_Flux = dMod_Flux_Center_Second;
				}
				else
				{
					g_dSuggested_Center_Second_WL = -1.0;
					g_dSuggested_Center_Second_Flux = -1.0;
				}

				g_bPerform_Fit = true;
				Request_Refresh();
			}
			break;
		case MAN_FIT_CLEAR:
			m_uiManual_Fit_Blue_Idx = -1;
			m_uiManual_Fit_Red_Idx= -1;
			m_uiManual_Fit_Central_Idx = -1;
			m_uiManual_Fit_Central_Second_Idx = -1;
			m_vManual_Fit_Data.Set(0,0);
			m_vManual_Fit_Data.Set(1,0);
			m_vManual_Fit_Data.Set(2,0);
			if (m_vManual_Fit_Data.Get_Size() == 6)
			{
				m_vManual_Fit_Data.Set(3,0);
				m_vManual_Fit_Data.Set(4,0);
				m_vManual_Fit_Data.Set(5,0);
			}
			m_vFit_Residuals.clear();
			m_vFlux_Fit.clear();
			Request_Refresh();
			break;
		case MODEL_DISPLAY_AREA: // for manual gaussian fitting select
			if (m_eMan_Select_Mode != MS_OFF)
			{
				m_eMan_Select_Mode = MS_OFF;
				if (m_uiManual_Fit_Blue_Idx != -1 && m_uiManual_Fit_Red_Idx != -1)
				{
					m_dMF_WL = m_vWavelength[m_uiManual_Fit_Blue_Idx];
					m_dMF_Flux = m_vFlux[m_uiManual_Fit_Blue_Idx];

					m_dMF_Slope = (m_vFlux[m_uiManual_Fit_Red_Idx] - m_vFlux[m_uiManual_Fit_Blue_Idx]) / (m_vWavelength[m_uiManual_Fit_Red_Idx] - m_vWavelength[m_uiManual_Fit_Blue_Idx]);
				}
				Request_Refresh();
			}
			break;
		case EXPORT: // for manual gaussian fitting select
			Export_Graphic();
			break;
		}
		if (eEvent != MAN_FIT_RED && eEvent != MAN_FIT_BLUE && eEvent != MAN_FIT_CENTER_SECOND && eEvent != MAN_FIT_CENTER && m_eMan_Select_Mode != MS_OFF)
			m_eMan_Select_Mode = MS_OFF;
	}
	m_csEvent_Queue.Unset();
	if (g_bFit_Results_Ready)
	{
		g_bFit_Results_Ready = false;
		m_vManual_Fit_Data = g_vFit_Results;
		double dpEW_PVF, dpEW_HVF;
		double 		dWL_Ref = (8498.02 * pow(10.0,-0.39) + 8542.09 * pow(10,-0.36) + 8662.14 * pow(10.0,-0.622)) / (pow(10.0,-0.39) + pow(10.0,-0.36) + pow(10.0,-0.622));
		Compute_Gaussian_Fit_pEW(g_vX, m_vManual_Fit_Data, m_vWavelength[1] - m_vWavelength[0], g_lpgfpParamters, dpEW_PVF, dpEW_HVF);
		m_dFit_Velocity_PVF = -Compute_Velocity(m_vManual_Fit_Data.Get(2),dWL_Ref);
		m_dFit_Velocity_HVF = nan("");
		if (m_vManual_Fit_Data.Get_Size() == 6)
		{
			m_dFit_Velocity_HVF = -Compute_Velocity(m_vManual_Fit_Data.Get(5),dWL_Ref);
			if (m_dFit_Velocity_HVF < m_dFit_Velocity_PVF)
			{
				double dTemp = m_dFit_Velocity_HVF;
				m_dFit_Velocity_HVF = m_dFit_Velocity_PVF;
				m_dFit_Velocity_PVF = dTemp;
			}
		}
		m_dFit_pEW = dpEW_PVF + dpEW_HVF;

		m_vFit_Residuals.clear();
		m_vFlux_Fit.clear();
		GAUSS_FIT_PARAMETERS * lpgfpParamters = &g_cgfpCaNIR;
		for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
		{
			double dY = 1.0 + Multi_Gaussian(m_vWavelength[uiI],m_vManual_Fit_Data,lpgfpParamters).Get(0);
			m_vFlux_Fit.push_back(dY);
			m_vFit_Residuals.push_back(m_vFlux[uiI] - dY);
		}
		Request_Refresh();
	}
	if (m_bQuit_Request_Pending && !g_bFit_Thread_Running)
	{
		Request_Quit();
	}
	m_dTimer += i_dDelta_Time_s;
	bool bFlasher = fmod(m_dTimer,1.0) < 0.5;
	if (g_bPerform_Fit)
	{
		if (bFlasher != m_bFlasher_1s_50p)
			Request_Refresh();
	}
	m_bFlasher_1s_50p = bFlasher;
}

