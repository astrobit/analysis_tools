#include <core.hpp>
#include <thread>
#include <model_spectra_db.h>
#include <utility>

enum button_id
{
	QUIT_REQUEST,
	NEXT_SPECTRUM,
	PREV_SPECTRUM,
	DELETE_SPECTRUM,
	TXT_CURR_SPEC_ID,
	TXT_MODEL_ID,
	TXT_ION,
	TXT_PS_VEL,
	TXT_PS_TEMP,
	TXT_EJECTA_SCALAR,
	TXT_SHELL_SCALAR,
	TXT_WL_BLUE,
	TXT_WL_RED,
	TXT_WL_DELTA,
	DISPLAY_SPECTRUM,
	CONFIRM_DELETE
};


class MSDB_MGR_MAIN : public MAIN
{
private:
	pane_id	m_idPane;

	std::map<button_id, BUTTON_INFO> m_mMain_Pane_Buttons;

	double		m_dTimer;
	bool		m_bFlasher_1s_50p;
	criticalsection			m_csEvent_List;
	std::deque<button_id> m_qEvent_List;

	msdb::DATABASE m_cDB;
	std::vector<std::pair<msdb::dbid,msdb::PARAMETERS> >	m_vdbidSpectra_List;
	unsigned int m_uiSelected_Spectrum;

	ES::Spectrum	m_specCurr_Spectrum;

	void Refresh_Spectra_List(void);
	void Delete_Spectra(void);
	void Regenerate_Spectra(void);
	void Select_Next_Spectra(void);
	void Select_Prev_Spectra(void);

public:
	MSDB_MGR_MAIN(void) : m_cDB(false) {;}
private:
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
