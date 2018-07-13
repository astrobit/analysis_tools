#include <eps_plot.h>

epsplot::stipple::stipple(void) : m_vType_Cycle(std::initializer_list<epsplot::STIPPLE>({epsplot::SOLID, epsplot::SHORT_DASH, epsplot::LONG_DASH, epsplot::LONG_SHORT_DASH, epsplot::DOTTED, epsplot::SHORT_DASH_DOTTED, epsplot::LONG_DASH_DOTTED, epsplot::LONG_SHORT_DASH_DOTTED, epsplot::LONG_LONG_DASH}))

{
	m_tCurr_Idx = 0;
}

void epsplot::stipple::reset(void)
{
	m_tCurr_Idx = 0;
}

std::vector<epsplot::STIPPLE> epsplot::stipple::getTypeCycle(void) const{return m_vType_Cycle;}
void epsplot::stipple::setTypeCycle(const std::vector<epsplot::STIPPLE> &i_vCycle)
{
	m_vType_Cycle = i_vCycle;
	m_tCurr_Idx = 0;
}
epsplot::STIPPLE epsplot::stipple::get(void) const {if (m_tCurr_Idx < m_vType_Cycle.size()) return m_vType_Cycle[m_tCurr_Idx]; else return SOLID;}
epsplot::stipple::operator epsplot::STIPPLE() const {return get();}
void epsplot::stipple::set(epsplot::STIPPLE i_eType)
{
	m_tCurr_Idx = -1;
	for (size_t tIdx = 0; tIdx < m_vType_Cycle.size() && m_tCurr_Idx == -1; tIdx++)
	{
		if (i_eType == m_vType_Cycle[tIdx])
			m_tCurr_Idx = tIdx;
	}
	if (m_tCurr_Idx == -1) // not found
	{
		m_tCurr_Idx = m_vType_Cycle.size();
		m_vType_Cycle.push_back(i_eType);
	}
}
epsplot::stipple epsplot::stipple::operator =(epsplot::STIPPLE i_eType)
{
	set(i_eType);
	return *this;
}

bool epsplot::stipple::operator ==(const epsplot::stipple & i_cRHO) const
{
	return get() == i_cRHO.get();
}
bool epsplot::stipple::operator ==(const epsplot::STIPPLE & i_eRHO) const
{
	return get() == i_eRHO;
}

epsplot::stipple & epsplot::stipple::operator++(int)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::stipple & epsplot::stipple::operator--(int)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::stipple & epsplot::stipple::operator++(void)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::stipple & epsplot::stipple::operator--(void)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::stipple & epsplot::stipple::operator+=(size_t i_tDelta)
{
	m_tCurr_Idx += i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::stipple & epsplot::stipple::operator-=(size_t i_tDelta)
{
	m_tCurr_Idx -= i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::stipple epsplot::stipple::operator+(size_t i_tDelta) const
{
	stipple cRet(*this);
	return (cRet += i_tDelta);
}
epsplot::stipple epsplot::stipple::operator-(size_t i_tDelta) const
{
	stipple cRet(*this);
	return (cRet -= i_tDelta);
}

