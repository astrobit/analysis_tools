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
			if (m_iterSelected_ID != m_OSCfile.m_mSpectra_List.end())
				m_iterSelected_ID++;
			else
				m_iterSelected_ID = m_OSCfile.m_mSpectra_List.begin();
			Process_Selected_Spectrum();
			Request_Refresh();
			break;
		case spectrum_select_down:
			if (m_iterSelected_ID != m_OSCfile.m_mSpectra_List.begin())
				m_iterSelected_ID--;
			else
			{
				m_iterSelected_ID = m_OSCfile.m_mSpectra_List.end();
				if (m_iterSelected_ID != m_OSCfile.m_mSpectra_List.begin())
					m_iterSelected_ID--;
			}
			Process_Selected_Spectrum();
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
				m_uiModel = 61;
				break;
			}
			Request_Refresh();
			break;
		case model_select_down:
			switch (m_uiModel)
			{
			case 61:
				m_uiModel = 45;
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
		case elem_select_up:
			if (m_uiElement < 28)
				m_uiElement++;
			Request_Refresh();
			break;
		case elem_select_down:
			if (m_uiElement > 6)
				m_uiElement--;
			if ((m_uiElement - 1) > m_uiIon)
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
		case fit_request:
			if (!g_bGen_In_Progress)
			{
				g_sdGen_Spectrum_Result.m_dPS_Temp = m_dPS_Temp;
				g_sdGen_Spectrum_Result.m_dPS_Velocity = m_dPS_Velocity;
				g_sdGen_Spectrum_Result.m_dShell_Scalar = m_dShell_Scalar;
				g_sdGen_Spectrum_Result.m_dEjecta_Scalar = m_dEjecta_Scalar;
				g_sdGen_Spectrum_Result.m_uiIon = m_uiElement * 100 + m_uiIon;//m_uiModel;
				g_lpmGen_Model = &m_mapModels[m_uiModel];
				g_sdGen_Spectrum_Result.m_specResult = Get_ES_Spectrum(m_specSelected_Spectrum);
				g_sdGen_Spectrum_Result.m_bValid = true;
				g_bGen_Process_Request = true;
			}
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
		case save_request:
			{
				FILE * fileSaves = fopen("results.csv","rt");
				if (fileSaves == nullptr)
				{
					fileSaves = fopen("results.csv","wt");
					fprintf(fileSaves,"dd-mm-yyyy, hh:mm:ss, SN, MJD, Instrument, Sources, Model #, Element, Ion, PS Temp, PS Velocity, Ss, Se\n");
					fclose(fileSaves);
				}
				else
					fclose(fileSaves);
				fileSaves = fopen("results.csv","at");
				if (fileSaves != nullptr)
				{
					time_t timer;
					tm * lpTime;
					time(&timer);  /* get current time; same as: timer = time(NULL)  */
					lpTime = gmtime(&timer);

					fprintf(fileSaves,"%02i-%02i-%04i, %02i:%02i:%02i, %s, %.1f, %s, \"%s\", %i, %i, %i, %.2f, %.2f, %.2f, %.2f\n",
									lpTime->tm_mday, 
									lpTime->tm_mon+1, 
									lpTime->tm_year + 1900, 
									lpTime->tm_hour, 
									lpTime->tm_min, 
									lpTime->tm_sec,
									m_OSCfile.m_szSupernova_ID.c_str(),
									m_idSelected_ID.m_dDate_MJD,
									m_idSelected_ID.m_szInstrument.c_str(),
									m_idSelected_ID.m_szSources.c_str(),
									m_uiModel,
									m_uiElement,
									m_uiIon,
									m_dPS_Temp,
									m_dPS_Velocity,
									m_dShell_Scalar,
									m_dEjecta_Scalar);
					fclose(fileSaves);
				}
			}
			break;
		case quit_request:
			std::cout << "quit" << std::endl;
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
		g_bGen_Done = false;
		Request_Refresh();
	}
	if (g_bGen_In_Progress)
		Request_Refresh();

	m_dTimer += i_dDelta_Time_s;
	bool bFlasher = fmod(m_dTimer,1.0) < 0.5;
	if (bFlasher != m_bFlasher_1s_50p)
		Request_Refresh();
	m_bFlasher_1s_50p = bFlasher;
}

