//#include <cstdlib>
//#include <cstdio>
//#include <cstring>
//#include <cmath>
//#include <cfloat>
//#include <ctime>
//#include <unistd.h>
#include <eps_plot.h>
//#include <sstream>

epsplot::color::color(void) : m_vType_Cycle(std::initializer_list<epsplot::COLOR>({epsplot::BLACK, epsplot::BLUE, epsplot::RED, epsplot::MAGENTA, epsplot::GREEN, epsplot::CYAN, epsplot::YELLOW, epsplot::GREY_75, epsplot::GREY_50, epsplot::GREY_25}))
{
	m_tCurr_Idx = 0;
}

void epsplot::color::reset(void)
{
	m_tCurr_Idx = 0;
}

std::vector<epsplot::COLOR> epsplot::color::getTypeCycle(void) const{return m_vType_Cycle;}
void epsplot::color::setTypeCycle(const std::vector<epsplot::COLOR> &i_vCycle)
{
	m_vType_Cycle = i_vCycle;
	m_tCurr_Idx = 0;
}
epsplot::COLOR epsplot::color::get(void) const {if (m_tCurr_Idx < m_vType_Cycle.size()) return m_vType_Cycle[m_tCurr_Idx]; else return BLACK;}
epsplot::color::operator epsplot::COLOR() const {return get();}
void epsplot::color::set(epsplot::COLOR i_eType)
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
epsplot::color epsplot::color::operator =(epsplot::COLOR i_eType)
{
	set(i_eType);
	return *this;
}

bool epsplot::color::operator ==(const epsplot::color & i_cRHO) const
{
	return get() == i_cRHO.get();
}
bool epsplot::color::operator ==(const epsplot::COLOR & i_eRHO) const
{
	return get() == i_eRHO;
}

epsplot::color & epsplot::color::operator++(int)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::color & epsplot::color::operator--(int)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::color & epsplot::color::operator++(void)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::color & epsplot::color::operator--(void)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::color & epsplot::color::operator+=(size_t i_tDelta)
{
	m_tCurr_Idx += i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::color & epsplot::color::operator-=(size_t i_tDelta)
{
	m_tCurr_Idx -= i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::color epsplot::color::operator+(size_t i_tDelta) const
{
	color cRet(*this);
	return (cRet += i_tDelta);
}
epsplot::color epsplot::color::operator-(size_t i_tDelta) const
{
	color cRet(*this);
	return (cRet -= i_tDelta);
}

