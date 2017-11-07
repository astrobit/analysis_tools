#include <OSCIn-SuShI.hpp>
#include <string>
#include <vector>

OSCIn_SuShI_main	g_cFit_Viz_Main;

void OSCIn_SuShI_main::init(void) // initialization routine; rendering context not created
{
	if (m_vszCommand_Line_Parameters.size() == 1)
	{
		Set_Screenshot_Default_Filename("OSCIn-SuShI-screenshot");

		m_idPane = Register_Pane(true);
		m_idError_Pane = Register_Pane(false);
		Raise_Pane(m_idError_Pane);

		m_OSCfile.Load(m_vszCommand_Line_Parameters[0]);
		m_OSCfile.Deredden();
		printf("Redshift: %.2e\n",m_OSCfile.m_dRedshift_Weighted_Mean);
		m_OSCfile.Unredshift();
		printf("Window name\n");
		Set_Window_Name(std::string("Open Supernova Catalog Interactive Viewer for Supernova - Shell Interaction (OSCIn-SuShI) --- ") + m_OSCfile.m_szSupernova_ID);

		printf("Selected spectrum\n");
		m_iterSelected_ID = m_OSCfile.m_mSpectra_List.begin();

		printf("Top/Bottom iterators\n");
		std::map<OSCspectra_id, OSCspectrum >::iterator iterFindLast = m_OSCfile.m_mSpectra_List.begin();
		while (iterFindLast != m_OSCfile.m_mSpectra_List.end())
		{
			m_iterLast_ID = iterFindLast;
			iterFindLast++;
		}

		m_dTimer = 0.0;
		m_bFlasher_1s_50p = true;
		m_bQuit_Request_Pending = false;
		m_eFeature_Select = fs_full;

		m_uiModel = 57;
		m_dPS_Temp = 8.0;
		m_dPS_Velocity = 18.0;
		m_dEjecta_Scalar = 2.4;
		m_dShell_Scalar = 2.8;
		m_dExc_Temp = 10.0;

		m_uiElement = 20;
		m_uiIon = 1;

		m_dNorm_Blue = 7900.0;
		m_dNorm_Red = 8600.0;

		m_dRefine_Blue = 7700.0;
		m_dRefine_Red = 8600.0;

		printf("Model loading\n");
		m_mapModels[17] = model(17);
		m_mapModels[41] = model(41);
		m_mapModels[45] = model(45);
		m_mapModels[49] = model(49);
		m_mapModels[57] = model(57);
		m_mapModels[61] = model(61);

		m_dGauss_Fit_Blue = 7700.0;
		m_dGauss_Fit_Red = 8600.0;

		m_dGauss_Fit_Gen_Blue = 7700.0;
		m_dGauss_Fit_Gen_Red = 8600.0;

		m_dFlux_Zoom = 1.0;

		m_bDisplay_Components = false;

		printf("Process selected spectrum\n");
		Process_Selected_Spectrum();

	}
	else
	{
		std::cerr << "Must specify path to .json file in command line." << std::endl;
		g_bQuit_Thread = true;
		m_bQuit_Request_Pending = true;
	}
}
void OSCIn_SuShI_main::close(void) // program exiting; rendering context destroyed
{
}

