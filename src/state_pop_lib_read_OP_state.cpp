#include <state_pops.h>

std::vector<config> Read_OP_State(unsigned int i_uiNe, std::string   i_szLabel_OP)
{
	std::vector<config> vRet;
	std::vector<config> vReverse;
//	std::vector<cc> vccString;

//	std::cout << "OP: " << i_szLabel_OP << std::endl;

	for (std::string::iterator iterI = i_szLabel_OP.begin(); iterI != i_szLabel_OP.end(); iterI++)
	{
		if (*iterI != ' ' && *iterI != '\t')
			{
//			std::cout << "---- i = " << *iterI << std::endl;

			if (*iterI == '(')
			{
				iterI++;
				if (iterI == i_szLabel_OP.end())
					break;
				if (*iterI >= '0' && *iterI <= '9')
				{
					unsigned int uiS = (*iterI) - '0';
					iterI++;
					if (iterI == i_szLabel_OP.end())
						break;
					if ((*iterI) >= '0' && (*iterI) <= '9')
					{
						uiS *= 10;
						uiS += (*iterI) - '0';
						iterI++;
						if (iterI == i_szLabel_OP.end())
							break;
					}
					unsigned int uiL = Ang_Mom_Term_To_L(*iterI);
					iterI++;
					if (iterI == i_szLabel_OP.end())
						break;
					unsigned int uiP = -1;
					if (*iterI =='O')
					{
						uiP = 1;
						iterI++;
						if (iterI == i_szLabel_OP.end())
							break;
					}
					else if (*iterI =='E')
					{
						uiP = 0;
						iterI++;
						if (iterI == i_szLabel_OP.end())
							break;
					}
					unsigned int uiIdx = vReverse.size() - 1;
					vReverse[uiIdx].m_uiS = uiS;
					vReverse[uiIdx].m_dL = uiL;
					vReverse[uiIdx].m_uiP = uiP;
				}
				iterI++;
				if (iterI == i_szLabel_OP.end())
					break;
			}
			else
			{
				unsigned int uin = (*iterI) - '0';
				iterI++;
				if (iterI == i_szLabel_OP.end())
					break;
				if ((*iterI) >= '0' && (*iterI) <= '9')
				{
					uin *= 10;
					uin += (*iterI) - '0';
					iterI++;
					if (iterI == i_szLabel_OP.end())
						break;
				}
//				std::cout << "n = " << uin << std::endl;
		
				unsigned int uil = Ang_Mom_Term_To_L(*iterI);
//				std::cout << "l = " << uil << std::endl;
				if (*iterI >= 'A' && *iterI <= 'Z')
				{
					unsigned int uiIdx = vReverse.size() - 1;
					vReverse[uiIdx].m_uiS = uin;
					vReverse[uiIdx].m_dL = uil;
					iterI++;
					if (iterI != i_szLabel_OP.end() && *iterI == '*')
						vReverse[uiIdx].m_uiP = 1;
					else
						vReverse[uiIdx].m_uiP = 0;
//					std::cout << "S = " << uin << " L = " << uil << " P = " << vReverse[uiIdx].m_uiP << std::endl;
					if (iterI == i_szLabel_OP.end())
						break;
				}
				else
				{
					std::string::iterator iterJ = iterI;
					iterJ++;
					if (iterJ != i_szLabel_OP.end() && (*iterJ >= '0' && *iterJ <= '9'))
					{
						iterJ++;
						if (iterJ != i_szLabel_OP.end() && *iterJ >= 'a' && *iterJ <= 'z')
						{
	//						iterI--; // need to back up to make sure to process the next item correctly
							vReverse.push_back(config(uin,uil));
							continue;
						}
					}
					else if (iterJ != i_szLabel_OP.end() && *iterJ  == '(')
					{
						vReverse.push_back(config(uin,uil));
						continue;
					}
					iterI++;
					unsigned int uim = 1;
					if (iterI != i_szLabel_OP.end() && (*iterI >= '0' && *iterI <= '9'))
					{
						uim = *iterI - '0';
						std::string::iterator iterJ = iterI;
						iterJ++;
						if (iterJ != i_szLabel_OP.end() && (*iterJ >= '0' && *iterJ <= '9'))
						{
							uim *= 10;
							uim += *iterJ - '0';
							iterI++;
							if (iterI == i_szLabel_OP.end())
							{
								for (unsigned int uiI = 0; uiI < uim; uiI++)
									vReverse.push_back(config(uin,uil));
								break;
							}
						}
					}
//					std::cout << "m = " << uim << std::endl;
					for (unsigned int uiI = 0; uiI < uim; uiI++)
						vReverse.push_back(config(uin,uil));
				}
			}
		}
	}
	// the above process puts the configuration in order from lowest (index 0) to highest energy (index size -1)
	// since we want index 0 to be the highest energy state, we reverse the vector here
	vRet = Get_Default_Configuration(i_uiNe);

	for (unsigned int uiI = 0; uiI < vReverse.size(); uiI++)
	{
		vRet[uiI] = vReverse[vReverse.size() - uiI - 1];
	}


	std::cout << i_szLabel_OP << " --> ";
	Print_Config_Vector(vRet);
	return vRet;
}

