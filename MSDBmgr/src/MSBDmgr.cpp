#include <MSDBmgr.hpp>
#include <string>
#include <vector>

MSDB_MGR_MAIN	g_cTemplate_Main;

void MSDB_MGR_MAIN::init(void) // initialization routine; rendering context not created
{
	m_idPane = Register_Pane(true);

	m_dTimer = 0.0;
	m_bFlasher_1s_50p = true;

	if (m_cDB.Database_Exists())
	{
		Refresh_Spectra_List();
	}

	

}
void MSDB_MGR_MAIN::close(void) // program exiting; rendering context destroyed
{
}

