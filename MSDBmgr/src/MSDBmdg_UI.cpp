#include <MSDBmgr.hpp>

void MSDB_MGR_MAIN::on_key_down(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
	switch (eKey_ID)
	{
	case KEY_ESCAPE:
	case KEY_Q:
		m_csEvent_List.Set();
		m_qEvent_List.push_back(QUIT_REQUEST);
		m_csEvent_List.Unset();
		break;
	case KEY_UP:
		m_csEvent_List.Set();
		m_qEvent_List.push_back(PREV_SPECTRUM);
		m_csEvent_List.Unset();
		break;
	case KEY_DOWN:
		m_csEvent_List.Set();
		m_qEvent_List.push_back(NEXT_SPECTRUM);
		m_csEvent_List.Unset();
		break;
	}
}

void MSDB_MGR_MAIN::on_key_up(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State)
{
}
void MSDB_MGR_MAIN::on_mouse_button_double_click(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void MSDB_MGR_MAIN::on_mouse_button_down(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
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
		for (std::map<button_id,BUTTON_INFO>::iterator cI = m_mMain_Pane_Buttons.begin(); cI != m_mMain_Pane_Buttons.end() && !bProcessed; cI++)
		{
			if (cI->second.TestHit(tMouse_Scaled))
			{
				m_csEvent_List.Set();
				m_qEvent_List.push_back((button_id)(cI->second.GetID()));
				m_csEvent_List.Unset();
				bProcessed = true;
			}
		}
	}
}
void MSDB_MGR_MAIN::on_mouse_button_up(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void MSDB_MGR_MAIN::on_mousemove(const PAIR<unsigned int> &i_tMouse_Position)
{

}
void MSDB_MGR_MAIN::on_mouse_wheel(MOUSEBUTTON i_eWheel, int i_iWheel_Delta, const PAIR<unsigned int> &i_tMouse_Position)
{
}
void MSDB_MGR_MAIN::on_timer(unsigned int i_uiTimer_ID, const double & i_dDelta_Time_s)
{
	if (!m_qEvent_List.empty())
	{
		m_csEvent_List.Set();
		while (!m_qEvent_List.empty())
		{
			button_id eEvent = m_qEvent_List.front();
			m_qEvent_List.pop_front();
			switch(eEvent)
			{
			case QUIT_REQUEST:
				Request_Quit();
				break;
			case PREV_SPECTRUM:
				Select_Prev_Spectra();
				Request_Refresh();
				break;
			case NEXT_SPECTRUM:
				Select_Next_Spectra();
				Request_Refresh();
				break;
			}
		}
		m_csEvent_List.Unset();
	}
	m_dTimer += i_dDelta_Time_s;
	bool bFlasher = fmod(m_dTimer,1.0) < 0.5;
	m_bFlasher_1s_50p = bFlasher;
}

