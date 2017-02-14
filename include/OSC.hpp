#pragma once
#include <string>
#include <json.h>
#include <cmath>

class OSCspectra_id
{
public:
	double	m_dDate_MJD;
	std::string	m_szInstrument;
	std::string m_szSources;

	inline bool operator == (const OSCspectra_id & i_cRHO) const
	{
		return m_dDate_MJD == i_cRHO.m_dDate_MJD && m_szInstrument == i_cRHO.m_szInstrument && m_szSources == i_cRHO.m_szSources;
	}
	inline bool operator != (const OSCspectra_id & i_cRHO) const
	{
		return m_dDate_MJD != i_cRHO.m_dDate_MJD || m_szInstrument != i_cRHO.m_szInstrument || m_szSources != i_cRHO.m_szSources;
	}

	inline bool operator <  (const OSCspectra_id & i_cRHO) const
	{
		return (m_dDate_MJD < i_cRHO.m_dDate_MJD) ||
				(m_dDate_MJD == i_cRHO.m_dDate_MJD && m_szInstrument < i_cRHO.m_szInstrument) ||
				(m_dDate_MJD == i_cRHO.m_dDate_MJD && m_szInstrument == i_cRHO.m_szInstrument && m_szSources < i_cRHO.m_szSources);
	}
	inline bool operator <=  (const OSCspectra_id & i_cRHO) const
	{
		return *this < i_cRHO ||
				*this == i_cRHO;
	}
	inline bool operator >  (const OSCspectra_id & i_cRHO) const
	{
		return (m_dDate_MJD > i_cRHO.m_dDate_MJD) ||
				(m_dDate_MJD == i_cRHO.m_dDate_MJD && m_szInstrument > i_cRHO.m_szInstrument) ||
				(m_dDate_MJD == i_cRHO.m_dDate_MJD && m_szInstrument == i_cRHO.m_szInstrument && m_szSources > i_cRHO.m_szSources);
	}
	inline bool operator >=  (const OSCspectra_id & i_cRHO) const
	{
		return *this > i_cRHO ||
				*this == i_cRHO;
	}
};

class OSCspectrum_dp
{
public:
	double 	m_dWavelength;
	double	m_dFlux;
	double	m_dFlux_Error;

	OSCspectrum_dp(void)
	{
		m_dWavelength = m_dFlux = m_dFlux_Error = nan("");
	}
	OSCspectrum_dp(const double & i_dWavelength, const double & i_dFlux, const double & i_dFlux_Error = 0.0)
	{
		m_dWavelength = i_dWavelength;
		m_dFlux = i_dFlux;
		m_dFlux_Error = i_dFlux_Error;
	}
};
class OSCspectrum
{
public:
	std::vector <OSCspectrum_dp> m_vSpectrum;

	double wl(unsigned int i_uiIdx) const { double dRet = nan(""); if (m_vSpectrum.size() > i_uiIdx) dRet = m_vSpectrum[i_uiIdx].m_dWavelength; return dRet;}
	double flux(unsigned int i_uiIdx) const { double dRet = nan(""); if (m_vSpectrum.size() > i_uiIdx) dRet = m_vSpectrum[i_uiIdx].m_dFlux; return dRet;}
	double flux_error(unsigned int i_uiIdx) const { double dRet = nan(""); if (m_vSpectrum.size() > i_uiIdx) dRet = m_vSpectrum[i_uiIdx].m_dFlux_Error; return dRet;}

	OSCspectrum_dp operator [] (unsigned int i_uiIdx) const {OSCspectrum_dp cRet; if (m_vSpectrum.size() > i_uiIdx) cRet = m_vSpectrum[i_uiIdx]; return cRet;}

	void push_back(const OSCspectrum_dp & i_cNew) { m_vSpectrum.push_back(i_cNew);}
	void clear(void) {m_vSpectrum.clear();}

	std::vector <OSCspectrum_dp>::iterator begin(void) { return m_vSpectrum.begin();}
	std::vector <OSCspectrum_dp>::iterator end(void) { return m_vSpectrum.end();}
	std::vector <OSCspectrum_dp>::const_iterator cbegin(void) const { return m_vSpectrum.cbegin();}
	std::vector <OSCspectrum_dp>::const_iterator cend(void) const { return m_vSpectrum.cend();}
	std::vector <OSCspectrum_dp>::reverse_iterator rbegin(void) { return m_vSpectrum.rbegin();}
	std::vector <OSCspectrum_dp>::reverse_iterator rend(void) { return m_vSpectrum.rend();}

	std::vector <std::tuple<double, double, double> > Get_Tuple_Vector(void) const;

	size_t size(void) const {return m_vSpectrum.size();}

	void Deredden(const double & i_dE_BmV);

	typedef std::vector <OSCspectrum_dp>::iterator iterator;
	typedef std::vector <OSCspectrum_dp>::const_iterator const_iterator;
	typedef std::vector <OSCspectrum_dp>::reverse_iterator reverse_iterator;

};
class OSCreference
{
public:
	std::string	m_szAlias;
	std::string m_szReference;
	std::string m_szName;

	OSCreference(void) {;}
	OSCreference(std::string	i_szAlias, std::string i_szReference, std::string i_szName)
	{
		m_szAlias = i_szAlias;
		m_szReference = i_szReference;
		m_szName = i_szName;
	}
};

	

class OSCfile
{
public:
	double 			m_dEBV_Mean;
	Json::Value 	m_jvDatafile;

	std::string			m_szSupernova_ID;

	std::string 		m_szFilename;
	std::map<OSCspectra_id, OSCspectrum > m_mSpectra_List;
	std::vector<OSCreference> m_vsReference_List;

	void Load(const std::string &i_sFilename);
	void Deredden(void);
};

