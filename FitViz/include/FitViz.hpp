#include <core.hpp>
#include <xio.h>
#include <xlinalg.h>
#include <line_routines.h>
#include <thread>

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
	MODEL_DISPLAY_AREA,
	PVF_TEXT,
	HVF_TEXT,
	pEW_TEXT,
	SHELL_pEW_TEXT,
	EJECTA_pEW_TEXT,
	TOTAL_pEW_TEXT,
	DISPLAY_SHELL_COMPONENT,
	DISPLAY_EJECTA_COMPONENT,
	PS_TEMP_TEXT,
	EJECTA_SCALAR_TEXT,
	SHELL_SCALAR_TEXT,
	EJECTA_PL_TEXT,
	SHELL_PL_TEXT,
	REF_MODEL_TEXT,
	METHOD_MANUAL,
	MAN_FIT_RED,
	MAN_FIT_BLUE,
	MAN_FIT_CENTER,
	MAN_FIT_CENTER_SECOND,
	MAN_FIT_EXEC,
	MAN_FIT_CLEAR,
	AUTO_FIT_TEST,
	MAN_FIT_RED_TEXT,
	MAN_FIT_BLUE_TEXT,
	MAN_FIT_CENTER_TEXT,
	MAN_FIT_CENTER_SECOND_TEXT,
	MAN_FIT_QUALITY_TEXT,
	EXPORT

};
enum fit_method
{
	fm_flat,
	fm_jeff,
	fm_manual,
	fm_auto
};


extern bool	g_bFit_Thread_Running;
extern bool	g_bQuit_Thread;
extern bool	g_bPerform_Fit;
extern bool	g_bFit_Results_Ready;
extern XVECTOR	g_vFit_Results;
extern XVECTOR g_vX;
extern XVECTOR	g_vY;
extern XVECTOR g_vW;
extern GAUSS_FIT_PARAMETERS * g_lpgfpParamters;
extern double g_dSuggested_Center_WL;
extern double g_dSuggested_Center_Flux;
extern double g_dSuggested_Center_Second_WL;
extern double g_dSuggested_Center_Second_Flux;
extern double g_dFit_Results_Smin;
extern XSQUARE_MATRIX	g_mCovariance_Matrix;

void Perform_Fit(void);

class FIT_VIZ_MAIN : public MAIN
{
private:
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

	std::thread 	m_thrFit;

	bool			m_bQuit_Request_Pending;

	unsigned int m_uiDay_Data_Loaded;
	XDATASET	m_dDay_Fit_Data;
	XDATASET	m_dDay_Spectrum_Data;
	XVECTOR		m_vGaussian_Fit_Data;
	double		m_dGaussian_Fit_Quality;
	double		m_dJeff_WL; // WL, flux
	double		m_dJeff_Flux;
	double		m_dJeff_Slope;
	PAIR<double> m_pJeff_Blue;
	PAIR<double> m_pJeff_Red;
	PAIR<double> m_pGlobal_Min;
	PAIR<double> m_pP_Cygni_Emission_Edge;
	double		m_dFit_Velocity_PVF;
	double		m_dFit_Velocity_HVF;
	double		m_dFit_pEW;
	bool		m_bDisplay_Shell_Component;
	bool		m_bDisplay_Ejecta_Component;

	unsigned int m_uiInfo_Idx;

	enum man_select_mode	{MS_OFF,MS_BLUE,MS_RED,MS_CENTER,MS_CENTER_SECOND};
	man_select_mode	m_eMan_Select_Mode;
	XVECTOR			m_vManual_Fit_Data;
	unsigned int	m_uiManual_Fit_Blue_Idx;
	unsigned int	m_uiManual_Fit_Red_Idx;
	unsigned int	m_uiManual_Fit_Central_Idx;
	unsigned int	m_uiManual_Fit_Central_Second_Idx;
	double			m_dMF_WL;
	double			m_dMF_Flux;
	double			m_dMF_Slope;
	double			m_dMF_Quality;

	std::vector<double>	m_vWavelength;
	std::vector<double> m_vFlux_Unflat;
	std::vector<double> m_vFlux_Flat;
	std::vector<double> m_vFlux;
	std::vector<double> m_vFlux_Shell;
	std::vector<double> m_vFlux_Ejecta;
	std::vector<double> m_vFlux_Fit;
	std::vector<double> m_vFlux_Deriv;
	double	m_dFlux_Deriv_Max;
	std::vector<double> m_vFlux_Second_Deriv;
	double	m_dFlux_Second_Deriv_Max;

	std::vector<double>	m_vFit_Residuals;
	double		m_dMax_Flux;
	double		m_dShell_Flat_pEW;
	double		m_dEjecta_Flat_pEW;
	double		m_dTotal_Flat_pEW;
	double		m_dPS_Temp;
	double		m_dEjecta_Scalar;
	double		m_dShell_Scalar;
	double		m_dShell_Power_Law;
	double		m_dEjecta_Power_Law;
	bool		m_bNo_Shell;
	std::string	m_szRef_Model;

	double		m_dTimer;
	bool		m_bFlasher_1s_50p;
	std::deque<button_id> m_qEvent_List;

	unsigned int m_uiSelected_Model;
	unsigned int m_uiSelected_Day;
	
	void Load_Model_Day_Lists(void);
	void Load_Display_Info(void);
	void Display_Man_Select_Item(unsigned int i_uiIdx, man_select_mode i_eMode, const double & i_dSize, const PAIR<double> & i_pdSize);

	void Export_Graphic(void);

public:
	FIT_VIZ_MAIN(void) : m_thrFit(Perform_Fit)  {m_thrFit.detach();};
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
