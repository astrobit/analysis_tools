#include <statepop.h>

void statepop::Print_Config_Vector(const vecconfig & i_vcConfig, std::ostream & io_out)
{
	for (auto iterI = i_vcConfig.cbegin(); iterI != i_vcConfig.cend(); iterI++)
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
		io_out << " ";
		if (iterI->m_dK != -1)
		{
			// JK or LK coupling
			if (iterI->m_uiS != -1)
			{
				io_out << iterI->m_uiS;
			}
			io_out << "[";
			int iK = iterI->m_dK * 2;
			io_out << iK << "/2]";
			if (iterI->m_uiP == 1)
			io_out << iK << "o";
		}
		else if (iterI->m_uiS != -1)
		{
			io_out << "(" << iterI->m_uiS;
			switch(iterI->m_uiL)
			{
			case 0:
				io_out << "S";
				break;
			case 1:
				io_out << "P";
				break;
			case 2:
				io_out << "D";
				break;
			default:
				{
					char chTerm = 'F';
					chTerm += (iterI->m_uiL - 3);
					if (chTerm >= 'H')
						chTerm++; // 'H' is not an allowed term
					if (chTerm >= 'P')
						chTerm++; // 'P' occurrs earlier, so it is skipped here
					if (chTerm >= 'S')
						chTerm++; // 'S' occurs earlier, so it is skipped here
					io_out << chTerm;
				}
			}
			io_out << ")";
		}
		io_out << " ";
		if (iterI->m_dJ != -1)
		{
			io_out << iterI->m_dJ;
		}
		else
			io_out << "?";
		io_out << " )";
	}
	io_out << std::endl;
}
