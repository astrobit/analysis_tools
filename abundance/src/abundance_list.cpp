#include <cstring>
#include <cstdio>

#include <xio.h>
#include <xstdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <abundance.h>
#include <unistd.h>
#include <pwd.h>


snatk_abundances::abundances::abundances(void)
{
	const char * lpszData_Path = {DATADIR};
    DIR * dir;
    dirent * dirEnt;
    if((dir = opendir(DATADIR)) != nullptr)
	{
		while ((dirEnt = readdir(dir)) != nullptr)
		{
			std::string sFilename = std::string(dirEnt->d_name);
			size_t tExtPos;
			if ((tExtPos = sFilename.find(".csv")) != std::string::npos)
			{
				std::string sName = sFilename.substr(0,tExtPos);
				char lpszPath[512];
				snatk_abundances::abundance_list cAbd;
				sprintf(lpszPath,"%s/%s",lpszData_Path,dirEnt->d_name);
				cAbd.Read_Table(lpszPath);
				m_mapAbundance[sName] = cAbd;
			}
		}
	    closedir(dir);
    }


	const char *homedir = getenv("HOME");

	if (homedir == nullptr)
	{
		passwd * updata = getpwuid(getuid());
		if (updata != nullptr)
			homedir = updata->pw_dir;
	}
	if (homedir != nullptr)
	{
		char lpszUser_Abd_Dir[512];
		sprintf(lpszUser_Abd_Dir,"%s/snatk-abundances",homedir);
	
		if((dir = opendir(lpszUser_Abd_Dir)) != nullptr)
		{
			while ((dirEnt = readdir(dir)) != nullptr)
			{
				std::string sFilename = std::string(dirEnt->d_name);
				size_t tExtPos;
				if ((tExtPos = sFilename.find(".csv")) != std::string::npos)
				{
					std::string sName = sFilename.substr(0,tExtPos);
					char lpszPath[512];
					snatk_abundances::abundance_list cAbd;
					sprintf(lpszPath,"%s/%s",lpszData_Path,dirEnt->d_name);
					cAbd.Read_Table(lpszPath);
					m_mapAbundance[sName] = cAbd;
				}
			}
			closedir(dir);
		}
	}
}

std::vector<std::string> snatk_abundances::abundances::Get_Type_List(void) const
{
	std::vector<std::string> vsRet;
	for (auto iterI = m_mapAbundance.begin(); iterI != m_mapAbundance.end(); iterI++)
	{
		vsRet.push_back(iterI->first);
	}
	return vsRet;
}

void snatk_abundances::abundances::Add_Abundance(const std::string & i_szName, const std::string & i_szPath_To_File)
{
	snatk_abundances::abundance_list cAbd;
	cAbd.Read_Table(i_szPath_To_File.c_str());
	m_mapAbundance[i_szName] = cAbd;

}
void snatk_abundances::abundances::Add_Abundance(const std::string & i_szName, const snatk_abundances::abundance_list & i_cAbundance_List)
{
	m_mapAbundance[i_szName] = i_cAbundance_List;
}
bool snatk_abundances::abundances::Check_List(const std::string & i_szName) const
{
	return (m_mapAbundance.count(i_szName) == 1);
}
snatk_abundances::abundance_list snatk_abundances::abundances::Get(const std::string & i_szAbundance) const
{
	snatk_abundances::abundance_list cRet;
	if (Check_List(i_szAbundance))
	{
		cRet = m_mapAbundance.at(i_szAbundance);
	}
	return cRet;
}


////////////////////////////////////////////////////////////////////////////////
//
// abundance_list
//
////////////////////////////////////////////////////////////////////////////////

snatk_abundances::abundance_list::abundance_list(void)
{
	memset(m_dAbundances,0,sizeof(m_dAbundances));
	memset(m_dUncertainties,0,sizeof(m_dUncertainties));
	m_dAbundances[0] = 1.0;
	m_dUncertainties[0] = 1.0;
}
double	snatk_abundances::abundance_list::get_Abundance(size_t i_tZ) const
{
	double dRet = 0.0;
	if (i_tZ < 128)
		dRet = m_dAbundances[i_tZ];
	return dRet;
}
double	snatk_abundances::abundance_list::get_Uncertainty(size_t i_tZ) const
{
	double dRet = 0.0;
	if (i_tZ < 128)
		dRet = m_dUncertainties[i_tZ];
	return dRet;
}
void	snatk_abundances::abundance_list::Read_Table(const char * i_lpszFilename)
{
	xdataset_improved	cAbundance_File;
	if (i_lpszFilename != nullptr)
	{
		cAbundance_File.Read_Data_File(i_lpszFilename,false,',',1);
		memset(m_dAbundances,0,sizeof(m_dAbundances));
		memset(m_dUncertainties,0,sizeof(m_dUncertainties));
		if (cAbundance_File.Get_Num_Rows() > 0)
		{
			double	dAbd_Sum = 0.0;
			for (unsigned int uiI = 0; uiI < cAbundance_File.Get_Num_Rows(); uiI++)
			{
				double dZ = xGet_Element_Number(cAbundance_File.Get_Element(uiI,0).c_str());
				unsigned int uiZ = (unsigned int)(dZ + 0.01);
	//			printf("Load %i %f\n",uiZ,cAbundance_File.GetElementDbl(2,uiI));
				if (uiZ <= 118)
				{
					double	dAbd_Curr = pow(10.0,cAbundance_File.Get_Element_Double(uiI,2));
					m_dAbundances[uiZ] += dAbd_Curr;
					m_dUncertainties[uiZ] += pow(10.0,cAbundance_File.Get_Element_Double(uiI,3));
					dAbd_Sum += dAbd_Curr;
				}
				else
					fprintf(stderr,"Could not find atomic number for element %s in file %s.\n",cAbundance_File.Get_Element(uiI,0).c_str(), i_lpszFilename);
			}
			double	dInv_Abd_Total = 1.0 / dAbd_Sum;
			for (unsigned int uiZ = 0; uiZ < 128; uiZ++)
			{
				m_dAbundances[uiZ] *= dInv_Abd_Total;
				m_dUncertainties[uiZ] *= dInv_Abd_Total;
			}
		}
		else
			fprintf(stderr,"Could not open abundance file %s.\n",i_lpszFilename);
	}
	else
		fprintf(stderr,"Abundance file filename is empty\n");
}

// Gamezo et al abundance information is group abundance.  Normalize the abundances for the individual groups instead of whoel abundances
void	snatk_abundances::abundance_list::Normalize_Groups(void)
{
	// Mg group: F to Al
	double	dSum = 0.0;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
		dSum += m_dAbundances[uiZ];
	double	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 9; uiZ < 14; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Si group: Si to Mn
	dSum = 0.0;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 14; uiZ < 21; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}

	// Fe group: Fe to Uuo (really more like Fe to Zn or Ge)
	dSum = 0.0;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
		dSum += m_dAbundances[uiZ];
	dInv_Abd_Total = 1.0 / dSum;
	for (unsigned int uiZ = 21; uiZ < 119; uiZ++)
	{
		m_dAbundances[uiZ] *= dInv_Abd_Total;
		m_dUncertainties[uiZ] *= dInv_Abd_Total;
	}
}
