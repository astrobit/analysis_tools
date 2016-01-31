#include <core.hpp>
#include <xio.h>
#include <xlinalg.h>


enum button_id
{
	MODEL_SELECT_UP,
	MODEL_SELECT_DOWN,
	DAY_SELECT_UP,
	DAY_SELECT_DOWN,
	METHOD_FLAT,
	METHOD_JEFF,
	METHOD_TOGGLE,
	SELECT_DIRECTORY,
	ERROR_ACK,
	QUIT_REQUEST,
	MODEL_SEL_TEXT,
	DAY_SEL_TEXT,
	MODEL_DISPLAY_AREA
};
enum fit_method
{
	fm_flat,
	fm_jeff
};

class FIT_VIZ_MAIN : public MAIN
{
	pane_id	m_idPane;
	pane_id m_idError_Pane;

	std::map<button_id, BUTTON_INFO> m_mMain_Pane_Buttons;
	std::map<button_id, BUTTON_INFO> m_mError_Pane_Buttons;
	std::string m_szDirectory;
	std::string m_szFile_Prefix;
	std::vector<unsigned int> m_vModel_List;
	std::vector<unsigned int> m_vDay_List;
	std::vector<std::string> m_vsError_Info;
	fit_method	m_eFit_Method;
	criticalsection	m_csEvent_Queue;

	unsigned int m_uiDay_Data_Loaded;
	XDATASET	m_dDay_Fit_Data;
	XDATASET	m_dDay_Spectrum_Data;
	XVECTOR		m_vGaussian_Fit_Data;
	double		m_dJeff_WL; // WL, flux
	double		m_dJeff_Flux;
	double		m_dJeff_Slope;
	PAIR<double> m_pJeff_Blue;
	PAIR<double> m_pJeff_Red;
	PAIR<double> m_pGlobal_Min;
	PAIR<double> m_pP_Cygni_Emission_Edge;

	std::vector<double>	m_vWavelength;
	std::vector<double> m_vFlux;
	double		m_dMax_Flux;

	std::deque<button_id> m_qEvent_List;

	unsigned int m_uiSelected_Model;
	unsigned int m_uiSelected_Day;
	
	void Load_Model_Day_Lists(void);
	void Load_Display_Info(void);


	void on_key_down(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State);
	void on_key_up(KEYID eKey_ID, unsigned char chScan_Code, unsigned int uiRepeat_Count, bool bExtended_Key, bool bPrevious_Key_State);
	void on_mouse_button_double_click(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position);
	void on_mouse_button_down(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position);
	void on_mouse_button_up(MOUSEBUTTON i_eButton, const PAIR<unsigned int> &i_tMouse_Position);
	void on_mousemove(const PAIR<unsigned int> &i_tMouse_Position);
	void on_mouse_wheel(MOUSEBUTTON i_eWheel, int i_iWheel_Delta, const PAIR<unsigned int> &i_tMouse_Position);
	void on_timer(unsigned int i_uiTimer_ID, const double & i_dDelta_Time_s);

	void init(void); // initialization routine; rendering context not created
	void gfx_display(pane_id i_idPane); // primary display routine
	void gfx_init(void); // initialization routine; rendering context already created
	void gfx_reshape(const PAIR<unsigned int> & i_tNew_Size); // window size change
	void gfx_close(void); // graphics exiting; rendering context still active
	void close(void); // program exiting; rendering context destroyed
};
