#include <line_routines.h>
#include <xlinalg.h>

void msdb_load_generate(msdb::USER_PARAMETERS	&i_cParam, msdb::SPECTRUM_TYPE i_eSpectrum_Type, const ES::Spectrum &i_cTarget, const xdataset * i_cOp_Map_A, const xdataset * i_cOp_Map_B, ES::Spectrum & o_cOutput)
{
	msdb::DATABASE	cMSDB;
	if (i_cOp_Map_A != nullptr)
	{
		bool bShell = i_cOp_Map_B != nullptr && i_cOp_Map_B[0].GetNumElements() > 0;
		if (cMSDB.Get_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput) == 0)
		{
			XVECTOR cParameters;
			cParameters.Set_Size(bShell? 7 : 5);
			cParameters.Set(0,1.0); // 1d after explosion
			cParameters.Set(1,i_cParam.m_dPhotosphere_Velocity_kkms); // ps velocity
			cParameters.Set(2,i_cParam.m_dPhotosphere_Temp_kK); // ps temp

			cParameters.Set(3,i_cParam.m_dEjecta_Effective_Temperature_kK); // fix excitation temp
			if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::EJECTA_ONLY)
				cParameters.Set(4,i_cParam.m_dEjecta_Log_Scalar); // PVF scalar
			else
				cParameters.Set(4,-20.0); // PVF scalar

	//				printf("%.5e %.5e\n",dEjecta_Scalar,dEjecta_Scalar + log10(dEjecta_Scalar_Prof / dEjecta_Scalar_Ref));
			if (bShell)
			{
				cParameters.Set(5,i_cParam.m_dShell_Effective_Temperature_kK); // fix excitation temp
				if (i_eSpectrum_Type == msdb::COMBINED || i_eSpectrum_Type == msdb::SHELL_ONLY)
					cParameters.Set(6,i_cParam.m_dShell_Log_Scalar); // HVF scalar
				else
					cParameters.Set(6,-20.0); // HVF scalar
			}
			//cParameters.Print();
			Generate_Synow_Spectra(i_cTarget,i_cOp_Map_A[0],i_cOp_Map_B[0],i_cParam.m_uiIon,cParameters,o_cOutput,-2.0,-2.0);
			//printf("gen done\n");
			msdb::dbid dbidID = cMSDB.Add_Spectrum(i_cParam, i_eSpectrum_Type, o_cOutput);
		}
	}
}
