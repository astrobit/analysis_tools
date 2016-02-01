#include <FitViz.hpp>
#include <string>
#include <vector>

FIT_VIZ_MAIN	g_cFit_Viz_Main;

void FIT_VIZ_MAIN::init(void) // initialization routine; rendering context not created
{
	m_idPane = Register_Pane(true);
	m_idError_Pane = Register_Pane(false);
	Raise_Pane(m_idError_Pane);

	m_szDirectory = ".";
	m_szFile_Prefix.clear();
	m_vModel_List.clear();
	m_vDay_List.clear();
	m_eFit_Method = fm_flat;
	m_uiSelected_Model = 0;
	m_uiSelected_Day = 0;
	m_uiDay_Data_Loaded = -1;
	m_bDisplay_Shell_Component = true;
	m_bDisplay_Ejecta_Component = true;

	m_vsError_Info.clear();
	Load_Model_Day_Lists();

}
void FIT_VIZ_MAIN::close(void) // program exiting; rendering context destroyed
{
}

