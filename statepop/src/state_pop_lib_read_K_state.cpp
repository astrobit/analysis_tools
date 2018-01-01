#include <statepop.h>
#include <cstring>


class cc
{
public:
	enum type {number,letter,paren,brace,plus,space,splat,tick,other};
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

statepop::vecconfig statepop::Read_Kurucz_State(unsigned int i_uiNe, std::string   i_szLabel_Kurucz, const statepop::floattype & i_dJ)
{
	vecconfig vRet;
	std::deque<cc> vccString;
	config cConfig;
	config cConfig_m1;
	config cConfig_m2;
	//std::cout << "Get default config " << i_uiNe << std::endl;
	vRet = Get_Default_Configuration(i_uiNe);
	//std::cout << "Get state config " << i_szLabel_Kurucz << std::endl;


	char lpszKurucz[32];
	strcpy(lpszKurucz,i_szLabel_Kurucz.c_str());
	const char * lpszEnd = lpszKurucz + strlen(lpszKurucz);
	const char * lpszCursor = lpszKurucz;
	lpszCursor = Pass_Whitespace(lpszCursor);
	bool bFlag_other = false;
	// parse the string and identify what information is at each position
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
		else if (lpszCursor[0] == '\'') // the tick seems unimportant -- ignore it
		{
			lpszCursor++;
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
	while (vccString.back().m_eType == cc::space)
		vccString.pop_back();

	// form "... PX" - the X just indicates that the level is experimentally unverified
	if (vccString.back().m_eType == cc::letter && vccString.back().m_chValue == 'X')
		vccString.pop_back();

	size_t tL = -1;
	size_t tS = -1;
	size_t tP = -1;
	size_t tLm1 = -1;
	double dJ = -1;
	double dK = -1;
	double dJm1 = -1;

	switch (vccString.back().m_eType)
	{
	case cc::brace: // JK or LK coupling term
		// J = 2n+1/2
		vccString.pop_back(); // brace
		if (vccString.back().m_eType == cc::plus)
			vccString.pop_back(); // plus
		else
			std::cerr << "odd JK/LK coupling term in "  << i_szLabel_Kurucz << " (" << i_uiNe << ")" << std::endl;
		if (vccString.back().m_eType == cc::number)
		{
			dK = (vccString.back().m_chValue + 1) * 0.5;
			vccString.pop_back(); // number
		}
		else
			dK = 0.5;
		vccString.pop_back(); // brace
		if (vccString.back().m_eType == cc::number)
		{
			tS = vccString.back().m_chValue;
			vccString.pop_back(); // number
		}
		// handle the leading data
		dJm1 = (2 * vccString.front().m_chValue + 1) * 0.5;
		vccString.pop_front(); // number
		vccString.pop_front(); // paren
		break;
	case cc::letter: //  "normal" 	LS coupling
		tL = Ang_Mom_Term_To_L(vccString.back().m_chValue);
		vccString.pop_back(); // letter
		tS = vccString.back().m_chValue;
		vccString.pop_back(); // number
		break;
	case cc::paren:
		//@@TODO
		// note-- jj coupling terms haven't been handled yet - I haven't yet found specific examples within the Kurucz data set to work with
		std::cerr << "JJ coupling term probably identified, but code not currently set up to handle it.  Make appropriate changes in the Read_Kurucz_State function" << std::endl;
		break;
	}

	if (vccString.back().m_eType == cc::splat)
	{
		tP = 1;
		vccString.pop_back(); // splat
	}

	while (vccString.back().m_eType == cc::space)
		vccString.pop_back();

	vRet[0].m_uiP = tP;
	vRet[0].m_uiS = tS;
	vRet[0].m_uiL = tL;
	vRet[0].m_dJ = i_dJ;
	vRet[0].m_dK = dK;
	if (vRet.size() > 1) // in case we are dealing with a Hydrogen like ion, don't set the J for the lower state
		vRet[1].m_dJ = dJm1; // only for JK/LK coupling

	//@@TODO: There may be other LK/JK or JJ coupling terms for lower level electrons. Here we handle only LS coupled terms
	if (vccString.size() > 0 && vccString.front().m_eType == cc::paren) // term for next lower set of electrons
	{
		vccString.pop_front(); // paren
		if (vccString.size() > 0 && vccString.front().m_eType == cc::number)
		{
			vRet[1].m_uiS = vccString.front().m_chValue;
			vccString.pop_front(); // number (S term)
			vRet[1].m_uiL = Ang_Mom_Term_To_L(vccString.front().m_chValue);
			vccString.pop_front(); // letter
			if (vccString.size() > 0 && vccString.front().m_eType != cc::paren)
				std::cerr << "Unexpected front coupling term for state" << i_szLabel_Kurucz << " in electron number " << i_uiNe << "." << std::endl;
			while (vccString.size() > 0 && vccString.front().m_eType != cc::paren)
				vccString.pop_front(); // unknown stuff
			if (vccString.size() > 0)
				vccString.pop_front(); // paren
		}
		else
		{
			std::cerr << "Unidentifiable Kurucz state " << i_szLabel_Kurucz << " in electron number " << i_uiNe << "." << std::endl;
		}
	}
	if (vccString.size() > 0 && (vccString.front().m_eType != cc::number && vccString.front().m_eType != cc::letter)) // this is probably a LK/JK or JJ coupling term
	{
		std::cerr << "Unexpected front coupling term for state" << i_szLabel_Kurucz << " in electron number " << i_uiNe << "." << std::endl;
	}

	size_t tIdx = 0;
	while (vccString.size() > 0)
	{
		if (vccString.back().m_eType == cc::letter)
		{
			vRet[tIdx].m_uil = Ang_Mom_Term_To_L(vccString.back().m_chValue);
			vccString.pop_back();
			if (vccString.size() > 0 && vccString.back().m_eType == cc::number)
			{ // e.g. 3s
				vRet[tIdx].m_uin = vccString.back().m_chValue;
				vccString.pop_back();
				tIdx++;
			}
			else if (vccString.size() > 0 && vccString.back().m_eType == cc::letter)
			{ // e.g. 3sp
				vRet[tIdx + 1].m_uil = Ang_Mom_Term_To_L(vccString.back().m_chValue);
				vccString.pop_back();
				if (vccString.size() > 0)
				{
					vRet[tIdx + 1].m_uin = vccString.back().m_chValue;
					vRet[tIdx].m_uin = vccString.back().m_chValue;
					vccString.pop_back();
				}
				tIdx += 2;
			}
			else if (vccString.size() > 0 && vccString.back().m_eType != cc::space)
			{
				std::cerr << "Unidentifiable Kurucz state " << i_szLabel_Kurucz << " in electron number " << i_uiNe << "." << std::endl;
			}
		}
		else if (vccString.back().m_eType == cc::number)
		{
			size_t tMult = vccString.back().m_chValue;
			vccString.pop_back();
			size_t tL = Ang_Mom_Term_To_L(vccString.back().m_chValue);
			vccString.pop_back();
			size_t tN = vRet[tIdx].m_uin;
			if (vccString.size() > 0 && vccString.back().m_eType == cc::number)
			{
				cc ccBack = vccString.back();
				vccString.pop_back();
				if (vccString.size() > 0 && vccString.back().m_eType == cc::letter)
				{ // this is a weird case where Kurucz lists a state as something like s2p5, potentially meaning ns2 np5, 
				// thus, don't process it as s 2p5
					vccString.push_back(ccBack);
				}
				else
					tN = ccBack.m_chValue;
			}

			for (size_t tX = 0; tX < tMult; tX++)
			{
				vRet[tIdx].m_uin = tN;
				vRet[tIdx].m_uil = tL;
				tIdx++;
			}

		}
		else if (vccString.back().m_eType == cc::space)
			vccString.pop_back();
		else
		{
			std::cerr << "Unidentifiable data in Kurucz state " << i_szLabel_Kurucz << " in electron number " << i_uiNe << "(" << vccString.back().m_chValue << ")." << std::endl;
			vccString.pop_back();
		}
	}

	
//	std::cout << lpszKurucz << " --> ";
//	Print_Config_Vector(vRet);

	return vRet;
}
