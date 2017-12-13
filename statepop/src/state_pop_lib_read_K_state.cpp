#include <statepop.h>
#include <cstring>


class cc
{
public:
	enum type {number,letter,paren,brace,plus,space,splat,other};
	type m_eType;
	char m_chValue;
};

void Report_Bad_Format(const std::string & i_szLabel, const std::vector<cc> & i_vccString)
{
	std::cerr << "uh oh. Unidentified format. " << i_szLabel << "(" << i_vccString.size() << "): ";
	for (auto iterI = i_vccString.begin(); iterI != i_vccString.end(); iterI++)
	{
		switch (iterI->m_eType)
		{
		case cc::number:
			std::cerr << "n";
			break;
		case cc::letter:
			std::cerr << "l";
			break;
		case cc::space:
			std::cerr << " ";
			break;
		case cc::paren:
			std::cerr << iterI->m_chValue;
			break;
		case cc::brace:
			std::cerr << iterI->m_chValue;
			break;
		case cc::plus:
			std::cerr << "+";
			break;
		case cc::splat:
			std::cerr << "*";
			break;
		case cc::other:
			std::cerr << "?";
			break;
		default:
			std::cerr << "X";
			break;
		}
	}
	std::cerr << std::endl;

}

statepop::vecconfig statepop::Read_Kurucz_State(unsigned int i_uiNe, std::string   i_szLabel_Kurucz)
{
	vecconfig vRet;
	std::vector<cc> vccString;
	config cConfig;
	config cConfig_m1;
	config cConfig_m2;

	vRet = Get_Default_Configuration(i_uiNe);

	char lpszKurucz[32];
	strcpy(lpszKurucz,i_szLabel_Kurucz.c_str());
	const char * lpszEnd = lpszKurucz + strlen(lpszKurucz);
	const char * lpszCursor = lpszKurucz;
	lpszCursor = Pass_Whitespace(lpszCursor);
	bool bFlag_other = false;
	while (lpszCursor != nullptr && lpszCursor[0] != 0 && lpszCursor < lpszEnd)
	{
		if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::number;
			ccCurr.m_chValue = atoi(lpszCursor);
			vccString.push_back(ccCurr);
			lpszCursor = Pass_Integer(lpszCursor);
		}
		else if ((lpszCursor[0] >= 'a' && lpszCursor[0] <= 'z') || (lpszCursor[0] >= 'A' && lpszCursor[0] <= 'Z'))
		{
			cc ccCurr;
			ccCurr.m_eType = cc::letter;
			ccCurr.m_chValue = lpszCursor[0];
			vccString.push_back(ccCurr);
			lpszCursor++;
		}
		else if (lpszCursor[0] == ')' || lpszCursor[0] == '(')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::paren;
			ccCurr.m_chValue = lpszCursor[0];
			vccString.push_back(ccCurr);
			lpszCursor++;
		}
		else if (lpszCursor[0] == '*')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::splat;
			ccCurr.m_chValue = '*';
			vccString.push_back(ccCurr);
			lpszCursor++;
		}
		else if (lpszCursor[0] == '+')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::plus;
			ccCurr.m_chValue = '+';
			vccString.push_back(ccCurr);
			lpszCursor++;
		}
		else if (lpszCursor[0] == '[' || lpszCursor[0] == ']')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::brace;
			ccCurr.m_chValue = lpszCursor[0];
			vccString.push_back(ccCurr);
			lpszCursor++;
		}
		else if (lpszCursor[0] == ' ')
		{
			cc ccCurr;
			ccCurr.m_eType = cc::space;
			ccCurr.m_chValue = lpszCursor[0];
			vccString.push_back(ccCurr);
			lpszCursor = Pass_Whitespace(lpszCursor);
		}
		else if (lpszCursor[0] == '?')
		{
			lpszCursor++; // I think this is an uncertain identification - ignore it for now
		}
		else
		{
			cc ccCurr;
			ccCurr.m_eType = cc::other;
			ccCurr.m_chValue = lpszCursor[0];
			vccString.push_back(ccCurr);
			lpszCursor++;
			bFlag_other = true;
		}
	}
	if (bFlag_other)
	{
		std::cerr << "uh oh. Unidentified character in of configuration string. " << lpszKurucz << std::endl;
	}

	cc ccBack = (*vccString.rbegin()); vccString.pop_back();
	while (ccBack.m_eType == cc::space) // get rid of any trailiing spaces
	{
		ccBack = (*vccString.rbegin()); vccString.pop_back();
	}
	if (ccBack.m_eType == cc::letter && ccBack.m_chValue == 'X')
	{
//		std::cerr << "here" << std::endl;
//		Report_Bad_Format(lpszKurucz,vccString);
		cc ccBack2 = (*vccString.rbegin());
		if (ccBack2.m_eType == cc::letter)
		{
			vccString.pop_back();
			ccBack = ccBack2;
		}
	}



	switch (ccBack.m_eType)
	{
	case cc::brace:
		vccString.pop_back(); // plus
		ccBack = (*vccString.rbegin()); vccString.pop_back(); // number
		vRet[0].m_dL = (ccBack.m_chValue + 1) * 0.5;
		vccString.pop_back(); // brace
		ccBack = (*vccString.rbegin()); vccString.pop_back(); // number
		vRet[0].m_uiS = ccBack.m_chValue;
		break;
	case cc::letter:
		vRet[0].m_dL = Ang_Mom_Term_To_L(ccBack.m_chValue);
		ccBack = (*vccString.rbegin()); vccString.pop_back();; // number
		vRet[0].m_uiS = ccBack.m_chValue;
		break;
	}
	ccBack = (*vccString.rbegin()); vccString.pop_back();
	if (ccBack.m_eType == cc::splat)
		vRet[0].m_uiP = 1;
	while (ccBack.m_eType == cc::space) // bypass spaces
	{
		ccBack = (*vccString.rbegin()); vccString.pop_back();
	}
	vccString.push_back(ccBack); // put the last item back in place

	if (vccString[0].m_eType == cc::number && vccString[1].m_eType == cc::paren)
	{
		vRet[1].m_dL = (vccString[0].m_chValue + 1) * 0.5;
		
		vRet[0].m_uin = vccString[2].m_chValue;
		vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[3].m_chValue);
	}
	else
	{
		switch (vccString.size())
		{
		case 1:
			if (vccString[0].m_eType == cc::letter)
				vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[0].m_chValue);
			else
				Report_Bad_Format(lpszKurucz,vccString);
			break;
		case 2:
			if (vccString[0].m_eType == cc::number && vccString[1].m_eType == cc::letter)
			{
				vRet[0].m_uin = vccString[0].m_chValue;
				vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[1].m_chValue);
			}
			else
				Report_Bad_Format(lpszKurucz,vccString);
			break;
		case 3:
			if (vccString[0].m_eType == cc::number && vccString[1].m_eType == cc::letter && vccString[2].m_eType == cc::number)
			{
				vRet[0].m_uin = vccString[0].m_chValue;
				vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[1].m_chValue);
				for (unsigned int uiI = 1; uiI < vccString[2].m_chValue; uiI++)
				{
					vRet[uiI].m_uin = vRet[0].m_uin;
					vRet[uiI].m_uil = vRet[0].m_uil;
				}
			}
			else if (vccString[0].m_eType == cc::letter && vccString[1].m_eType == cc::letter && vccString[2].m_eType == cc::number)
			{
				unsigned int uiN0 = vRet[0].m_uin;
				unsigned int uiNs = uiN0;
				unsigned int uiN1,uiN2;
				vecconfig vTest;
				unsigned int uil1 = Ang_Mom_Term_To_L(vccString[0].m_chValue), uil2 = Ang_Mom_Term_To_L(vccString[1].m_chValue);
				bool bTests[7] = {false,false,false,false,false,false,false};

				if (uil2 < uil1)
				{
					unsigned int uiswap = uil2;
					uil2 = uil1;
					uil1 = uiswap;
				}
				if (uiNs != 1)
					uiNs--;
				
				unsigned int uiTest_ID = 0;
				for (unsigned int uiN1 = uiNs; uiN1 < uiN0 + 2; uiN1++)
				{
					for (unsigned int uiN2 = uiNs; uiN2 < uiN0 + 2; uiN2++)
					{
						if (uiN1 != (uiN2 + 2) && (uiN1 + 2) != uiN2) //  make sure not to do 1s3p or 3s1p, the latter is nonsensical, the former is pretty unlikely
						{
							vecconfig vTest;
							if (uiN1 == uiN2 || uiN1 < uiN2)
							{ // e.g. 3s3p, 3s4p
								for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
								vTest.push_back(config(uiN1,uil1));
							}
							else
							{ // e.g. 3p4s
								vTest.push_back(config(uiN1,uil1));
								for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
							}

							unsigned int uiI = 0;
							while (vTest[uiI] >= vRet[uiI] && uiI < vTest.size())
								uiI++;
							bTests[uiTest_ID] = (uiI == vTest.size()); // we successfully got through and all are higher energy or equal energy of the ground state
							uiTest_ID++;
						}
					}
				}
				unsigned int uiTest_Type = 1;
				if (uiN0 != 1)
				{
					std::cout << std::endl;

					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else if (bTests[5])
					{
						uiN1 = uiNs + 2;
						uiN2 = uiNs + 1;
						uiTest_Type = 2;
					}
					else if (bTests[4])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs + 2;
					}
					else if (bTests[6])
					{
						uiN1 = uiN2 = uiNs + 2;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}

				}
				else
				{
					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}
				}
				if (uiTest_Type == 1)
				{
					vRet[0].m_uin = uiN1;
					vRet[0].m_uil = uil1;
					for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
					{
						vRet[uiI + 1].m_uin = uiN2;
						vRet[uiI + 1].m_uil = uil2;
					}
				}
				else if (uiTest_Type == 2)
				{
					for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
					{
						vRet[uiI].m_uin = uiN2;
						vRet[uiI].m_uil = uil2;
					}
					vRet[vccString[2].m_chValue].m_uin = uiN1;
					vRet[vccString[2].m_chValue].m_uil = uil1;
				}
			}
			else
				Report_Bad_Format(lpszKurucz,vccString);
			break;
		case 4:
			if (vccString[0].m_eType == cc::letter && vccString[1].m_eType == cc::number && vccString[2].m_eType == cc::letter && vccString[3].m_eType == cc::number)
			{
				unsigned int uiN0 = vRet[0].m_uin;
				unsigned int uiNs = uiN0;
				unsigned int uiN1,uiN2;
				vecconfig vTest;
				unsigned int uil1 = Ang_Mom_Term_To_L(vccString[0].m_chValue), uil2 = Ang_Mom_Term_To_L(vccString[2].m_chValue);
				bool bTests[7] = {false,false,false,false,false,false,false};

				if (uil2 < uil1)
				{
					unsigned int uiswap = uil2;
					uil2 = uil1;
					uil1 = uiswap;
				}
				if (uiNs != 1)
					uiNs--;
				
				unsigned int uiTest_ID = 0;
				for (unsigned int uiN1 = uiNs; uiN1 < uiN0 + 2; uiN1++)
				{
					for (unsigned int uiN2 = uiNs; uiN2 < uiN0 + 2; uiN2++)
					{
						if (uiN1 != (uiN2 + 2) && (uiN1 + 2) != uiN2) //  make sure not to do 1s3p or 3s1p, the latter is nonsensical, the former is pretty unlikely
						{
							vecconfig vTest;
							if (uiN1 == uiN2 || uiN1 < uiN2)
							{ // e.g. 3s3p, 3s4p
								for (unsigned int uiI = 0; uiI < vccString[1].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
								for (unsigned int uiI = 0; uiI < vccString[3].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN1,uil1));
								}
							}
							else
							{ // e.g. 3p4s
								for (unsigned int uiI = 0; uiI < vccString[3].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN1,uil1));
								}
								for (unsigned int uiI = 0; uiI < vccString[1].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
							}

							unsigned int uiI = 0;
							while (vTest[uiI] >= vRet[uiI] && uiI < vTest.size())
								uiI++;
							bTests[uiTest_ID] = (uiI == vTest.size()); // we successfully got through and all are higher energy or equal energy of the ground state
							uiTest_ID++;
						}
					}
				}
				unsigned int uiTest_Type = 1;
				if (uiN0 != 1)
				{
					std::cout << std::endl;

					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else if (bTests[5])
					{
						uiN1 = uiNs + 2;
						uiN2 = uiNs + 1;
						uiTest_Type = 2;
					}
					else if (bTests[4])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs + 2;
					}
					else if (bTests[6])
					{
						uiN1 = uiN2 = uiNs + 2;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}

				}
				else
				{
					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}
				}
				if (uiTest_Type == 1)
				{
					for (unsigned int uiI = 0; uiI < vccString[1].m_chValue; uiI++)
					{
						vRet[uiI].m_uin = uiN2;
						vRet[uiI].m_uil = uil2;
					}
					for (unsigned int uiI = 0; uiI < vccString[3].m_chValue; uiI++)
					{
						vRet[uiI + vccString[1].m_chValue].m_uin = uiN1;
						vRet[uiI + vccString[1].m_chValue].m_uil = uil1;
					}
				}
				else if (uiTest_Type == 2)
				{
					for (unsigned int uiI = 0; uiI < vccString[3].m_chValue; uiI++)
					{
						vRet[uiI].m_uin = uiN1;
						vRet[uiI].m_uil = uil1;
					}
					for (unsigned int uiI = 0; uiI < vccString[1].m_chValue; uiI++)
					{
						vRet[uiI + vccString[3].m_chValue].m_uin = uiN2;
						vRet[uiI + vccString[3].m_chValue].m_uil = uil2;
					}
				}
			}
			else if (vccString[0].m_eType == cc::number && vccString[1].m_eType == cc::letter && vccString[2].m_eType == cc::number && vccString[3].m_eType == cc::letter)
			{
				vRet[0].m_uin = vccString[2].m_chValue;
				vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[3].m_chValue);
				vRet[1].m_uin = vccString[0].m_chValue;
				vRet[1].m_uil = Ang_Mom_Term_To_L(vccString[1].m_chValue);

				if (vRet[0] < vRet[1])
				{
					config cCfg = vRet[0];
					vRet[0].m_uin = vRet[1].m_uin;
					vRet[0].m_uil = vRet[1].m_uil;
					vRet[1].m_uin = cCfg.m_uin;
					vRet[1].m_uil = cCfg.m_uil;
				}
			}
			else
				Report_Bad_Format(lpszKurucz,vccString);
			break;
		case 6:
			if (vccString[0].m_eType == cc::number && vccString[1].m_eType == cc::letter && vccString[2].m_eType == cc::number && 
				vccString[3].m_eType == cc::space  && vccString[4].m_eType == cc::number && vccString[5].m_eType == cc::letter)
			{
				vRet[0].m_uin = vccString[4].m_chValue;
				vRet[0].m_uil = Ang_Mom_Term_To_L(vccString[5].m_chValue);
				for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
				{
					vRet[uiI + 1].m_uin = vccString[0].m_chValue;
					vRet[uiI + 1].m_uil = Ang_Mom_Term_To_L(vccString[1].m_chValue);
				}
			}
			else if (vccString[0].m_eType == cc::letter && vccString[1].m_eType == cc::letter && vccString[2].m_eType == cc::number && 
				vccString[3].m_eType == cc::space  && vccString[4].m_eType == cc::number && vccString[5].m_eType == cc::letter)
			{
				unsigned int uiN0 = vRet[0].m_uin;
				unsigned int uiNs = uiN0;
				unsigned int uiN1,uiN2;
				unsigned int uiN3 = vccString[4].m_chValue;
				vecconfig vTest;
				unsigned int uil1 = Ang_Mom_Term_To_L(vccString[0].m_chValue), uil2 = Ang_Mom_Term_To_L(vccString[1].m_chValue);
				unsigned int uil3 = Ang_Mom_Term_To_L(vccString[5].m_chValue);
				bool bTests[7] = {false,false,false,false,false,false,false};

				if (uil2 < uil1)
				{
					unsigned int uiswap = uil2;
					uil2 = uil1;
					uil1 = uiswap;
				}
				if (uiNs != 1)
					uiNs--;
				
				unsigned int uiTest_ID = 0;
				for (unsigned int uiN1 = uiNs; uiN1 < uiN0 + 2; uiN1++)
				{
					for (unsigned int uiN2 = uiNs; uiN2 < uiN0 + 2; uiN2++)
					{
						if (uiN1 != (uiN2 + 2) && (uiN1 + 2) != uiN2) //  make sure not to do 1s3p or 3s1p, the latter is nonsensical, the former is pretty unlikely
						{
							vecconfig vTest;
							if (uiN1 == uiN2 || uiN1 < uiN2)
							{ // e.g. 3s3p, 3s4p
								vTest.push_back(config(uiN3,uil3));
								for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
								vTest.push_back(config(uiN1,uil1));
							}
							else
							{ // e.g. 3p4s
								vTest.push_back(config(uiN3,uil3));
								vTest.push_back(config(uiN1,uil1));
								for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
								{
									vTest.push_back(config(uiN2,uil2));
								}
							}
							unsigned int uiI = 0;
							while (vTest[uiI] >= vRet[uiI] && uiI < vTest.size())
								uiI++;
							bTests[uiTest_ID] = (uiI == vTest.size()); // we successfully got through and all are higher energy or equal energy of the ground state
							uiTest_ID++;
						}
					}
				}
				unsigned int uiTest_Type = 1;
				if (uiN0 != 1)
				{
					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else if (bTests[5])
					{
						uiN1 = uiNs + 2;
						uiN2 = uiNs + 1;
						uiTest_Type = 2;
					}
					else if (bTests[4])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs + 2;
					}
					else if (bTests[6])
					{
						uiN1 = uiN2 = uiNs + 2;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}

				}
				else
				{
					if (bTests[0])
					{
						uiN1 = uiN2 = uiNs;

					}
					else if (bTests[2])
					{
						uiN1 = uiNs + 1;
						uiN2 = uiNs;
						uiTest_Type = 2;
					}
					else if (bTests[1])
					{
						uiN1 = uiNs;
						uiN2 = uiNs + 1;
					}
					else if (bTests[3])
					{
						uiN1 = uiN2 = uiNs + 1;
					}
					else
					{
						uiTest_Type = 0;
						std::cerr << "uh oh. Unable to definitively identify state " << lpszKurucz << std::endl;
					}
				}
				if (uiTest_Type == 1)
				{
					for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
					{
						vRet[uiI + 1].m_uin = uiN2;
						vRet[uiI + 1].m_uil = uil2;
					}
					vRet[vccString[2].m_chValue + 1].m_uin = uiN1;
					vRet[vccString[2].m_chValue + 1].m_uil = uil1;
				}
				else if (uiTest_Type == 2)
				{
					vRet[1].m_uin = uiN1;
					vRet[1].m_uil = uil1;
					for (unsigned int uiI = 0; uiI < vccString[2].m_chValue; uiI++)
					{
						vRet[uiI + 2].m_uin = uiN2;
						vRet[uiI + 2].m_uil = uil2;
					}
				}

				vRet[0].m_uin = uiN3;
				vRet[0].m_uil = uil3;
			}
			else if (vccString[0].m_eType == cc::paren && vccString[1].m_eType == cc::number &&
				vccString[2].m_eType == cc::letter && vccString[3].m_eType == cc::paren &&
				vccString[4].m_eType == cc::number && vccString[5].m_eType == cc::letter)
			{
				unsigned int uil1 = Ang_Mom_Term_To_L(vccString[2].m_chValue), uil2 = Ang_Mom_Term_To_L(vccString[5].m_chValue);
				unsigned int uil3 = Ang_Mom_Term_To_L(vccString[8].m_chValue);

				vRet[0].m_uin = vccString[4].m_chValue;
				vRet[0].m_uil = uil2;
				vRet[1].m_uiS = vccString[1].m_chValue;
				vRet[1].m_dL = uil1;
			}
			else
				Report_Bad_Format(lpszKurucz,vccString);
			break;
		case 5:
		default:
			Report_Bad_Format(lpszKurucz,vccString);
			break;
		}
	}

	
//	std::cout << lpszKurucz << " --> ";
//	Print_Config_Vector(vRet);

	return vRet;
}
