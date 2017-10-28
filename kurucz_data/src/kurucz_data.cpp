#include <kurucz_data.h>
#include <sstream>

void kurucz_data::Load_Data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion)
{
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
	// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
	if (lpszLA_Data_Path == nullptr)
	{
		const char lpszData_Dir[] = {DATADIR};
		lpszLA_Data_Path == lpszData_Dir;
	}
#endif
	if (lpszLA_Data_Path == nullptr)
		std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined." << std::endl;
	else
	{
		m_uiN = i_uiN;
		m_uiMax_Ion = i_uiMax_Ion;
		m_uiMin_Ion = i_uiMin_Ion;
		if (m_uiMax_Ion > m_uiN)
			m_uiMax_Ion = m_uiN;
		for (unsigned int uiI = i_uiMin_Ion; uiI <= m_uiMax_Ion; uiI++)
		{
			std::ostringstream ossFilename;
			ossFilename << lpszLA_Data_Path << "/Kurucz/gf";
	//		ossFilename.fill('0');
	//		ossFilename.width(2);
			ossFilename << std::setfill('0') << std::setw(2) << m_uiN;
			ossFilename << std::setfill('0') << std::setw(2) << uiI;
			ossFilename << ".all";
	//		std::cout << ossFilename.str() << std::endl;
			vkld vkldIon_Line_Data;

			FILE * fileElem = fopen(ossFilename.str().c_str(),"rt");
			if (fileElem)
			{
				while (feof(fileElem) == 0)
				{
					char lpszLine[256];
					fgets(lpszLine,sizeof(lpszLine),fileElem);
					Kurucz_Line_Data kldCurr_Line;
					std::string szLine = lpszLine;
					kldCurr_Line.Process_Datafile_Line(szLine);
//						kldCurr_Line.Compute_Z(dRadiation_Temperature_K,dRedshift);
					vkldIon_Line_Data.push_back(kldCurr_Line);
				}
				m_vvkldLine_Data.push_back(vkldIon_Line_Data);
			}
		}
	}
}

