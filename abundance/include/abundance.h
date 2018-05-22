#pragma once
#include <map>
#include <vector>

namespace snatk_abundances
{
	class element
	{
	public:
		size_t m_tZ;
		size_t m_tA;

		element(void)
		{
			m_tZ = 0;
			m_tA = 1;
		}
		element(size_t i_tZ, size_t i_tA)
		{
			m_tZ = i_tZ;
			m_tA = i_tA;
		}


		bool operator <(const element & i_cRHO) const;
		bool operator <=(const element & i_cRHO) const;
		bool operator >(const element & i_cRHO) const;
		bool operator >=(const element & i_cRHO) const;
		bool operator ==(const element & i_cRHO) const;
		bool operator !=(const element & i_cRHO) const;

	};

	class abundance_list
	{
	private:
		std::map<element,double>	m_dAbundances;
		std::map<element,double>	m_dUncertainties;

	public:
		abundance_list(void);

		double	get_Abundance(size_t i_tZ, size_t i_tA = -1) const;
		double	get_Uncertainty(size_t i_tZ, size_t i_tA = -1) const;


		void	Read_Table(const char * i_lpszFilename);
		void	Normalize_Groups(void); // normalizes Mg, Si, and Fe groups individually (Mg: Z = 9--13; Si: Z = 14 -- 20; Fe: Z = 21--118)
	};

	class abundances
	{
	private:
		std::map<std::string,abundance_list> m_mapAbundance;
	public:
		abundance_list Get(const std::string & i_szAbundance) const;
		std::vector<std::string> Get_Type_List(void) const;
		bool Check_List(const std::string & i_szName) const;

		void Add_Abundance(const std::string & i_szName, const std::string & i_szPath_To_File);
		void Add_Abundance(const std::string & i_szName, const abundance_list & i_cAbundance_List);

		abundances(void);

	};
};

