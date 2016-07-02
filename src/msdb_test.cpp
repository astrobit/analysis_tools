#include <iostream>
#include <ostream>
#include <string>
#include <xio.h>
#include <xlinalg.h>
#include "ES_Synow.hh"
//#include "ES_Generic_Error.hh"
#include <best_fit_data.h>
#include <line_routines.h>
#include <model_spectra_db.h>
#include <opacity_profile_data.h>

bool Test(const std::string i_strDescription, bool i_bPass)
{
	std::string strFail = "...\033[0;31mFAIL\033[0m";
	std::string strPass = "...\033[0;32mPASS\033[0m";
	if (!i_bPass)
		std::cerr << i_strDescription << strFail << std::endl;
	else
		std::cout << i_strDescription << strPass << std::endl;
	return i_bPass;
		
}
bool Compare_Spectra(const ES::Spectrum &i_cA, const ES::Spectrum &i_cB)
{
	bool bRet = i_cA.size() == i_cB.size();
	for (unsigned int uiI = 0; uiI < i_cA.size() && bRet; uiI++)
	{
		bRet &= (i_cA.wl(uiI) == i_cB.wl(uiI) && i_cA.flux(uiI) == i_cB.flux(uiI));
	}
	return bRet;
}
int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	XDATASET cOpacity_Map_Shell;
	XDATASET cOpacity_Map_Ejecta;
	XVECTOR cParameters(7);
	msdb::USER_PARAMETERS	cParam;
	msdb::DATABASE cMSDB(true);

	ES::Spectrum cSpec_Gen;
	ES::Spectrum cSpec_DB;
	bool	bSuccess = true;

	cOpacity_Map_Ejecta.ReadDataFileBin("run57/opacity_map_ejecta.Si.xdataset");
	cOpacity_Map_Shell.ReadDataFileBin("run57/opacity_map_shell.xdataset");

	cParam.m_uiIon = 2001;
	cParam.m_dTime_After_Explosion = 1.0;
	cParam.m_dPhotosphere_Velocity_kkms = 10.0;
	cParam.m_dPhotosphere_Temp_kK = 10.0;
	cParam.m_dEjecta_Effective_Temperature_kK = 10.0;
	cParam.m_dShell_Effective_Temperature_kK = 10.0;
	cParam.m_eFeature = msdb::CaNIR;
	cParam.m_dWavelength_Range_Lower_Ang = 2000.0;;
	cParam.m_dWavelength_Range_Upper_Ang = 10000.0;
	cParam.m_dWavelength_Delta_Ang = 1.00;
	cParam.m_uiModel_ID = 57;
	cParam.m_dEjecta_Log_Scalar = 0.0;
	cParam.m_dShell_Log_Scalar = 0.0;

	cSpec_Gen = ES::Spectrum::create_from_range_and_step(cParam.m_dWavelength_Range_Lower_Ang,cParam.m_dWavelength_Range_Upper_Ang, cParam.m_dWavelength_Delta_Ang);
	cSpec_DB = ES::Spectrum::create_from_range_and_step(cParam.m_dWavelength_Range_Lower_Ang,cParam.m_dWavelength_Range_Upper_Ang, cParam.m_dWavelength_Delta_Ang);



	cParameters.Set(0,cParam.m_dTime_After_Explosion);
	cParameters.Set(1,cParam.m_dPhotosphere_Velocity_kkms);
	cParameters.Set(2,cParam.m_dPhotosphere_Temp_kK);
	cParameters.Set(3,cParam.m_dEjecta_Effective_Temperature_kK); // fix excitation temp
	cParameters.Set(4,cParam.m_dEjecta_Log_Scalar);
	cParameters.Set(5,cParam.m_dShell_Effective_Temperature_kK); // fix excitation temp
	cParameters.Set(6,cParam.m_dShell_Log_Scalar);

	// generate combined spectra
	Generate_Synow_Spectra(cSpec_Gen, cOpacity_Map_Ejecta, cOpacity_Map_Shell, cParam.m_uiIon, cParameters, cSpec_Gen);
	/// save to db
	msdb::dbid dbidDBID = cMSDB.Add_Spectrum(cParam, msdb::COMBINED, cSpec_Gen);
	bSuccess &= Test("Save combined spectrum to db", dbidDBID != 0);
	bSuccess &= Test("Find dbid from paramters", dbidDBID == cMSDB.Get_DB_ID(cParam)); // warning: this test only works for combined.  Don't attempt for any others
	// test reload
	bSuccess &= Test("Load combined spectrum by parameters", cMSDB.Get_Spectrum(cParam, msdb::COMBINED, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db combined spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	bSuccess &= Test("Load combined spectrum by id", cMSDB.Get_Spectrum(dbidDBID, msdb::COMBINED, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db combined spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	cParameters.Set(4,-20.0);
	Generate_Synow_Spectra(cSpec_Gen, cOpacity_Map_Ejecta, cOpacity_Map_Shell, cParam.m_uiIon, cParameters, cSpec_Gen);
	dbidDBID = cMSDB.Add_Spectrum(cParam, msdb::SHELL_ONLY, cSpec_Gen);
	bSuccess &= Test("Save shell only spectrum to db", dbidDBID != 0);
	bSuccess &= Test("Load shell only spectrum by parameters", cMSDB.Get_Spectrum(cParam, msdb::SHELL_ONLY, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db shell only spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	bSuccess &= Test("Load shell only spectrum by id", cMSDB.Get_Spectrum(dbidDBID, msdb::SHELL_ONLY, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db shell only spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	cParameters.Set(4,cParam.m_dEjecta_Log_Scalar);
	cParameters.Set(6,-20.0);
	Generate_Synow_Spectra(cSpec_Gen, cOpacity_Map_Ejecta, cOpacity_Map_Shell, cParam.m_uiIon, cParameters, cSpec_Gen);
	dbidDBID = cMSDB.Add_Spectrum(cParam, msdb::EJECTA_ONLY, cSpec_Gen);
	bSuccess &= Test("Save ejecta only spectrum to db", dbidDBID != 0);
	bSuccess &= Test("Load ejecta only spectrum by parameters", cMSDB.Get_Spectrum(cParam, msdb::EJECTA_ONLY, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db ejecta only spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	bSuccess &= Test("Load ejecta only spectrum by id", cMSDB.Get_Spectrum(dbidDBID, msdb::EJECTA_ONLY, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db ejecta only spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	cParameters.Set(4,-20.0);
	cParameters.Set(6,-20.0);
	Generate_Synow_Spectra(cSpec_Gen, cOpacity_Map_Ejecta, cOpacity_Map_Shell, cParam.m_uiIon, cParameters, cSpec_Gen);
	dbidDBID = cMSDB.Add_Spectrum(cParam, msdb::CONTINUUM, cSpec_Gen);
	bSuccess &= Test("Save continuum spectrum to db", dbidDBID != 0);
	bSuccess &= Test("Load continuum spectrum by parameters", cMSDB.Get_Spectrum(cParam, msdb::CONTINUUM, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db continuum spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));
	bSuccess &= Test("Load continuum spectrum by id", cMSDB.Get_Spectrum(dbidDBID, msdb::CONTINUUM, cSpec_DB) == dbidDBID);
	bSuccess &= Test("Compare db continuum spectrum to generated spectrum", Compare_Spectra(cSpec_Gen, cSpec_DB));


	
	return 0;
}
