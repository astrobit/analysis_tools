#include <kurucz_data.h>
#include <sstream>

void kurucz_data::Load_Data(unsigned int i_uiN, unsigned int i_uiMin_Ion, unsigned int i_uiMax_Ion)
{
	const char * lpszLA_Data_Path = std::getenv("LINE_ANALYSIS_DATA_PATH");
#ifdef DATADIR
	// use the user specified path in LINE_ANALYSIS_DATA path. If it is undefined, use the DATADIR specified when the package was installed
	const char lpszData_Dir[] = {DATADIR};
	if (lpszLA_Data_Path == nullptr)
	{
		lpszLA_Data_Path = lpszData_Dir;
	}
#endif
	if (lpszLA_Data_Path == nullptr)
		std::cerr << "LINE_ANALYSIS_DATA_PATH is undefined. (kurucz)" << std::endl;
	else
	{
		m_uiN = i_uiN;
		m_uiMax_Ion = i_uiMax_Ion;
		m_uiMin_Ion = i_uiMin_Ion;
		if (m_uiMax_Ion > m_uiN)
			m_uiMax_Ion = m_uiN;
		for (unsigned int uiI = i_uiMin_Ion; uiI <= m_uiMax_Ion; uiI++)
		{
			bool bIsotope_Warning = false;
			std::ostringstream ossFilename;
			ossFilename << lpszLA_Data_Path << "/gf";
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
					if (kldCurr_Line.m_uiIsotope == 0)//@@TODO - handle isotopes intelligently
						vkldIon_Line_Data.push_back(kldCurr_Line);
					else
						bIsotope_Warning = true;
				}
				m_vvkldLine_Data.push_back(vkldIon_Line_Data);

				if (bIsotope_Warning)
					std::cerr << "Isotope information was contained in the line list of atomic number " << i_uiN << ", ion " << uiI << ". This data was not included when loading the dataset to avoid incorrect identification of levels." << std::endl;
					
			}
			else
				std::cerr << "Could not open " << ossFilename.str() << std::endl;
		}
	}
}

