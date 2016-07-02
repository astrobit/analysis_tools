#include<cstdio>
#include<cstdlib>
#include <ES_Synow.hh>
#include <line_routines.h>
#include <opacity_profile_data.h>
#include <sstream>
#include <fstream>

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	OPACITY_PROFILE_DATA::GROUP	eOpacity_Group = OPACITY_PROFILE_DATA::SILICON;
	double dTime = 1.0;
	double dPS_Vel = nan("");
	double dPS_Temp = 10.0;
	double dExc_T_Ejecta = 10.0;
	double dlog_S_Ejecta = 4.5;
	double dExc_T_Shell= 10.0;
	double dlog_S_Shell = 5.5;
	double dTime_Power_Law_Ejecta = -2.0;
	double dTime_Power_Law_Shell = -2.0;
	double dMixing = 1.0;
	unsigned int uiModel_ID = 57;
	unsigned int uiIon = 2001;


	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
	if (lpszLA_Data_Path != nullptr)
	{
		ES::Spectrum	cSpecOut;
		ES::Spectrum	cTarget = ES::Spectrum::create_from_range_and_step(2500.0,10000.0,0.5);
		ES::Spectrum	cOutput(cTarget);

		std::ostringstream ossFile_Path;
		std::ostringstream ossFile_Path_Ejecta;
		std::ostringstream ossFile_Path_Shell;
		std::ostringstream ossFile_Path_Photosphere;
		std::ostringstream ossFile_Path_Photosphere_EO;

		ossFile_Path << lpszLA_Data_Path;
		ossFile_Path << "/models/";
		ossFile_Path << uiModel_ID;
		ossFile_Path << "/";

		XDATASET xdOpacity_Ejecta;
		XDATASET xdOpacity_Shell;
		XDATASET xdPhotosphere;
		XDATASET xdPhotosphere_EO;

		switch (eOpacity_Group)
		{
		case OPACITY_PROFILE_DATA::CARBON:
			ossFile_Path_Ejecta << ossFile_Path.str() << "opacity_map_ejecta.C.xdataset";
			break;
		case OPACITY_PROFILE_DATA::OXYGEN:
			ossFile_Path_Ejecta << ossFile_Path.str() << "opacity_map_ejecta.O.xdataset";
			break;
		case OPACITY_PROFILE_DATA::MAGNESIUM:
			ossFile_Path_Ejecta << ossFile_Path.str() << "opacity_map_ejecta.Mg.xdataset";
			break;
		case OPACITY_PROFILE_DATA::SILICON:
			ossFile_Path_Ejecta << ossFile_Path.str() << "opacity_map_ejecta.Si.xdataset";
			break;
		case OPACITY_PROFILE_DATA::IRON:
			ossFile_Path_Ejecta << ossFile_Path.str() << "opacity_map_ejecta.Fe.xdataset";
			break;
		}
		ossFile_Path_Shell << ossFile_Path.str() << "opacity_map_shell.xdataset";
		ossFile_Path_Photosphere << ossFile_Path.str() << "photosphere.csv";
		ossFile_Path_Photosphere_EO << ossFile_Path.str() << "photosphere_eo.csv";

		xdOpacity_Ejecta.ReadDataFileBin(ossFile_Path_Ejecta.str().c_str(),true);
		xdOpacity_Shell.ReadDataFileBin(ossFile_Path_Shell.str().c_str(),true);
		xdPhotosphere.ReadDataFile(ossFile_Path_Photosphere.str().c_str(),false,false,',',1);
		xdPhotosphere_EO.ReadDataFile(ossFile_Path_Photosphere_EO.str().c_str(),false,false,',',1);

// xvector parameters for Generate_Synow_Spectra:
// 0: time after explosion in days
// 1: photosphere velocity in 1000 km/s
// 2: photosphere temperature in 1000 K
// 3: excitation temperature for PVF  in 1000 K
// 4: log_{10} S for PVF
// 5: excitation temperature for HVF  in 1000 K
// 6: log_{10} S for HVF
		if (std::isnan(dPS_Vel))
		{
			unsigned int uiI = 0;
			while (uiI < xdPhotosphere.GetNumRows() && xdPhotosphere.GetElement(0,uiI) < dTime)
				uiI++;
			if (uiI != 0)
			{
				dPS_Vel = ((xdPhotosphere.GetElement(4,uiI) - xdPhotosphere.GetElement(4,uiI - 1)) / (xdPhotosphere.GetElement(0,uiI) - xdPhotosphere.GetElement(0,uiI - 1)) * (dTime - xdPhotosphere.GetElement(0,uiI - 1)) + xdPhotosphere.GetElement(4,uiI - 1)) * 1.0e-8;
			}
			else
				dPS_Vel = xdPhotosphere.GetElement(4,uiI) * 1.0e-8;
		}
		XVECTOR	xvParams(7);
		xvParams.Set(0, dTime);
		xvParams.Set(1, dPS_Vel);
		xvParams.Set(2, dPS_Temp);
		xvParams.Set(3, dExc_T_Ejecta);
		xvParams.Set(4, dlog_S_Ejecta);
		xvParams.Set(5, dExc_T_Shell);
		xvParams.Set(6, dlog_S_Shell);
		
		
		Generate_Synow_Spectra(cTarget, xdOpacity_Ejecta, xdOpacity_Shell, uiIon, xvParams, cOutput, dTime_Power_Law_Ejecta, dTime_Power_Law_Shell);

		std::ofstream fsOut;
		fsOut.open("out.csv");
		for (unsigned int uiI = 0; uiI < cOutput.size(); uiI++)
		{
			fsOut << cOutput.wl(uiI) << ", " << cOutput.flux(uiI) << std::endl;
		}
		fsOut.close();
	}


	

	return 0;
}
