#include<eps_plot.h>

epsplot::symbol_type::symbol_type(void) : m_vType_Cycle(std::initializer_list<epsplot::SYMBOL_TYPE>({epsplot::SQUARE, epsplot::CIRCLE, epsplot::TRIANGLE_UP, epsplot::TRIANGLE_DOWN, epsplot::TRIANGLE_LEFT, epsplot::TRIANGLE_RIGHT, epsplot::DIAMOND, epsplot::TIMES_SYMB, epsplot::PLUS_SYMB, epsplot::DASH_SYMB, epsplot::ASTERISK_SYMB, epsplot::STAR4, epsplot::STAR5, epsplot::STAR6}))
{
	m_tCurr_Idx = 0;
}

void epsplot::symbol_type::reset(void)
{
	m_tCurr_Idx = 0;
}
std::vector<epsplot::SYMBOL_TYPE> epsplot::symbol_type::getTypeCycle(void) const{return m_vType_Cycle;}
void epsplot::symbol_type::setTypeCycle(const std::vector<epsplot::SYMBOL_TYPE> &i_vCycle)
{
	m_vType_Cycle = i_vCycle;
	m_tCurr_Idx = 0;
}
epsplot::SYMBOL_TYPE epsplot::symbol_type::get(void) const {if (m_tCurr_Idx < m_vType_Cycle.size()) return m_vType_Cycle[m_tCurr_Idx]; else return epsplot::SQUARE;}
epsplot::symbol_type::operator epsplot::SYMBOL_TYPE() const {return get();}
void epsplot::symbol_type::set(epsplot::SYMBOL_TYPE i_eType)
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
epsplot::symbol_type epsplot::symbol_type::operator =(epsplot::SYMBOL_TYPE i_eType)
{
	set(i_eType);
	return *this;
}

bool epsplot::symbol_type::operator ==(const epsplot::symbol_type & i_cRHO) const
{
	return get() == i_cRHO.get();
}
bool epsplot::symbol_type::operator ==(const epsplot::SYMBOL_TYPE & i_eRHO) const
{
	return get() == i_eRHO;
}

epsplot::symbol_type & epsplot::symbol_type::operator++(int)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type & epsplot::symbol_type::operator--(int)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type & epsplot::symbol_type::operator++(void)
{
	m_tCurr_Idx++;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type & epsplot::symbol_type::operator--(void)
{
	m_tCurr_Idx--;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type & epsplot::symbol_type::operator+=(size_t i_tDelta)
{
	m_tCurr_Idx += i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size())
		m_tCurr_Idx -= m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type & epsplot::symbol_type::operator-=(size_t i_tDelta)
{
	m_tCurr_Idx -= i_tDelta;
	if (m_tCurr_Idx > m_vType_Cycle.size()) // > is intended, because size_t is an unsigned type
		m_tCurr_Idx += m_vType_Cycle.size();
	return *this;
}
epsplot::symbol_type epsplot::symbol_type::operator+(size_t i_tDelta) const
{
	symbol_type cRet(*this);
	return (cRet += i_tDelta);
}
epsplot::symbol_type epsplot::symbol_type::operator-(size_t i_tDelta) const
{
	symbol_type cRet(*this);
	return (cRet -= i_tDelta);
}

