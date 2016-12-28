#include <state_pops.h>

void Print_Config_Vector(const std::vector<config> & i_vcConfig, std::ostream & io_out)
{
	for (std::vector<config>::const_iterator iterI = i_vcConfig.begin(); iterI != i_vcConfig.end(); iterI++)
	{
		io_out << "( ";
		if (iterI->m_uin != -1)
		{
			io_out << iterI->m_uin;
		}
		io_out << "-";
		if (iterI->m_uil != -1)
		{
			io_out << iterI->m_uil;
		}
		io_out << "-";
		if (iterI->m_uiS != -1)
		{
			io_out << iterI->m_uiS;
		}
		io_out << "-";
		if (iterI->m_dL != -1)
		{
			io_out << iterI->m_dL;
		}
		io_out << "-";
		if (iterI->m_uiP != -1)
		{
			io_out << iterI->m_uiP;
		}
		io_out << ")";
	}
	io_out << std::endl;
}
