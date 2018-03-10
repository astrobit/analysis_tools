#pragma once
#include <map>
#include <vector>

namespace snatk_abundances
{
	class abundance_list
	{
	private:
		double	m_dAbundances[128]; // ordered by z; neutrons not included; no differentiation between isotopes
		double	m_dUncertainties[128];

	public:
		abundance_list(void);

		double	get_Abundance(size_t i_tZ) const;
		double	get_Uncertainty(size_t i_tZ) const;


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

