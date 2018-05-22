#include <cstring>
#include <cstdio>

#include <xio.h>
#include <xstdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <abundance.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <sstream>


bool snatk_abundances::element::operator <(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ < i_cRHO.m_tZ || (m_tZ == i_cRHO.m_tZ && m_tA < i_cRHO.m_tA));
}
bool snatk_abundances::element::operator <=(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ < i_cRHO.m_tZ || (m_tZ == i_cRHO.m_tZ && m_tA <= i_cRHO.m_tA));
}
bool snatk_abundances::element::operator >(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ > i_cRHO.m_tZ || (m_tZ == i_cRHO.m_tZ && m_tA > i_cRHO.m_tA));
}
bool snatk_abundances::element::operator >=(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ > i_cRHO.m_tZ || (m_tZ == i_cRHO.m_tZ && m_tA >= i_cRHO.m_tA));
}
bool snatk_abundances::element::operator ==(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ == i_cRHO.m_tZ && m_tA == i_cRHO.m_tA);
}
bool snatk_abundances::element::operator !=(const snatk_abundances::element & i_cRHO) const
{
	return (m_tZ != i_cRHO.m_tZ || m_tA != i_cRHO.m_tA);
}


////////////////////////////////////////////////////////////////////////////////
//
// abundances
//
////////////////////////////////////////////////////////////////////////////////

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
			if ((tExtPos = sFilename.find(".csv")) != std::string::npos && sFilename.find(".csv#") == std::string::npos)
			{
				std::string sName = sFilename.substr(0,tExtPos);
				std::ostringstream ossPath;

				snatk_abundances::abundance_list cAbd;
				ossPath << lpszData_Path;
				ossPath << "/";
				ossPath << dirEnt->d_name;
				cAbd.Read_Table(ossPath.str().c_str());
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
				if ((tExtPos = sFilename.find(".csv")) != std::string::npos && sFilename.find(".csv#") == std::string::npos)
				{
					std::string sName = sFilename.substr(0,tExtPos);
					std::ostringstream ossPath;

					snatk_abundances::abundance_list cAbd;
					ossPath << lpszUser_Abd_Dir;
					ossPath << "/";
					ossPath << dirEnt->d_name;
					cAbd.Read_Table(ossPath.str().c_str());
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
	std::cout << "m_mapAbundance.size() " << m_mapAbundance.size() << " " << i_szName << std::endl; 
	bool bFound = false;
	for (auto iterI = m_mapAbundance.cbegin(); iterI != m_mapAbundance.cend() && !bFound; iterI++)
	{
		std::cout << iterI->first << std::endl;
		bFound = iterI->first == i_szName;
	}
		
	return bFound; //(m_mapAbundance.count(i_szName) == 1);
}
snatk_abundances::abundance_list snatk_abundances::abundances::Get(const std::string & i_szAbundance) const
{
	snatk_abundances::abundance_list cRet;
//	if (Check_List(i_szAbundance))
	{
		std::cout << "snatk-abd get start" << std::endl; fflush(stdout);
		bool bFound = false;
		for (auto iterI = m_mapAbundance.cbegin(); iterI != m_mapAbundance.cend() && !bFound; iterI++)
		{
			std::cout << iterI->first << std::endl;
			if (iterI->first == i_szAbundance)
			{
				bFound = true;
				cRet = iterI->second;
			}
		}
		//cRet = m_mapAbundance.at(i_szAbundance);
		std::cout << "snatk-abd get end" << std::endl; fflush(stdout);
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
	m_dAbundances.clear();
	m_dUncertainties.clear();
}
double	snatk_abundances::abundance_list::get_Abundance(size_t i_tZ, size_t i_tA) const
{
	double dRet = 0.0;
	if (i_tA != -1)
	{
		if (m_dAbundances.count(element(i_tZ,i_tA)) == 1)
			dRet = m_dAbundances.at(element(i_tZ,i_tA));
	}
	else
	{
		for (auto iterI = m_dAbundances.cbegin(); iterI != m_dAbundances.cend(); iterI++)
		{
			if (iterI->first.m_tZ == i_tZ)
				dRet += iterI->second;
		}
	}
	return dRet;
}
double	snatk_abundances::abundance_list::get_Uncertainty(size_t i_tZ, size_t i_tA) const
{
	double dRet = 0.0;
	if (i_tA != -1)
	{
		if (m_dAbundances.count(element(i_tZ,i_tA)) == 1)
			dRet = m_dAbundances.at(element(i_tZ,i_tA)) * m_dAbundances.at(element(i_tZ,i_tA));
	}
	else
	{
		for (auto iterI = m_dAbundances.cbegin(); iterI != m_dAbundances.cend(); iterI++)
		{
			if (iterI->first.m_tZ == i_tZ)
				dRet += iterI->second * iterI->second;
		}
	}
	dRet = std::sqrt(dRet);
	return dRet;
}
void	snatk_abundances::abundance_list::Read_Table(const char * i_lpszFilename)
{
	xdataset_improved	cAbundance_File;
	if (i_lpszFilename != nullptr)
	{
		cAbundance_File.Read_Data_File(i_lpszFilename,false,',',1);
		m_dAbundances.clear();
		m_dUncertainties.clear();
		if (cAbundance_File.Get_Num_Rows() > 0)
		{
			double	dAbd_Sum = 0.0;
			for (unsigned int uiI = 0; uiI < cAbundance_File.Get_Num_Rows(); uiI++)
			{
				std::string szElem_ID = cAbundance_File.Get_Element(uiI,0);
				while (szElem_ID.back() == ' ')
					szElem_ID.pop_back();
				size_t tA = cAbundance_File.Get_Element_Int(uiI,1);

				double dZ = xGet_Element_Number(szElem_ID.c_str());
				unsigned int uiZ = (unsigned int)(dZ + 0.01);
	//			printf("Load %i %f\n",uiZ,cAbundance_File.GetElementDbl(2,uiI));
				if (uiZ <= 118)
				{
					double	dAbd_Curr = pow(10.0,cAbundance_File.Get_Element_Double(uiI,2));
					m_dAbundances[element(uiZ,tA)] = dAbd_Curr;
					m_dUncertainties[element(uiZ,tA)] = pow(10.0,cAbundance_File.Get_Element_Double(uiI,3));
					dAbd_Sum += dAbd_Curr;
				}
				else
					fprintf(stderr,"Could not find atomic number for element %s in file %s.\n",cAbundance_File.Get_Element(uiI,0).c_str(), i_lpszFilename);
			}
			double	dInv_Abd_Total = 1.0 / dAbd_Sum;
			for (auto iterI = m_dAbundances.begin(); iterI != m_dAbundances.end(); iterI++)
			{
				iterI->second *= dInv_Abd_Total;
			}

			for (auto iterI = m_dUncertainties.begin(); iterI != m_dUncertainties.end(); iterI++)
			{
				iterI->second *= dInv_Abd_Total;
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
	double dC_Sum = 0.0, dO_Sum = 0.0, dMg_Sum = 0.0, dSi_Sum = 0.0, dFe_Sum = 0.0;
	for (auto iterI = m_dAbundances.begin(); iterI != m_dAbundances.end(); iterI++)
	{
		if (iterI->first.m_tZ == 6)
			dC_Sum += iterI->second;
		else if (iterI->first.m_tZ == 8)
			dO_Sum += iterI->second;
		else if (iterI->first.m_tZ >= 9 && iterI->first.m_tZ < 14)
			dMg_Sum += iterI->second;
		else if (iterI->first.m_tZ >= 14 && iterI->first.m_tZ < 21)
			dSi_Sum += iterI->second;
		else if (iterI->first.m_tZ >= 21)
			dFe_Sum += iterI->second;
	}
	double dInv_C_Sum = 1.0 / dC_Sum,
			dInv_O_Sum = 1.0 / dO_Sum,
			dInv_Mg_Sum = 1.0 / dMg_Sum,
			dInv_Si_Sum = 1.0 / dSi_Sum,
			dInv_Fe_Sum = 1.0 / dFe_Sum;
	for (auto iterI = m_dAbundances.begin(); iterI != m_dAbundances.end(); iterI++)
	{
		if (iterI->first.m_tZ == 6)
			iterI->second *= dInv_C_Sum;
		else if (iterI->first.m_tZ == 8)
			iterI->second *= dInv_O_Sum;
		else if (iterI->first.m_tZ >= 9 && iterI->first.m_tZ < 14)
			iterI->second *= dInv_Mg_Sum;
		else if (iterI->first.m_tZ >= 14 && iterI->first.m_tZ < 21)
			iterI->second *= dInv_Si_Sum;
		else if (iterI->first.m_tZ >= 21)
			iterI->second *= dInv_Fe_Sum;
	}
}
