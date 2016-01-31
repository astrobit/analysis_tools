#include <FitViz.hpp>

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
		m_qEvent_List.push_back(METHOD_TOGGLE);
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
}
void FIT_VIZ_MAIN::on_mouse_button_up(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void FIT_VIZ_MAIN::on_mousemove(const PAIR<unsigned int> &i_tMouse_Position)
{
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
		case METHOD_TOGGLE:
			if (m_eFit_Method == fm_flat)
				m_eFit_Method = fm_jeff;
			else
				m_eFit_Method = fm_flat;
			Load_Display_Info();
			Request_Refresh();
			break;
		case QUIT_REQUEST:
			Request_Quit();
			break;
		}
	}
	m_csEvent_Queue.Unset();
}

