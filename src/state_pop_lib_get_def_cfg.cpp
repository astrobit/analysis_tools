#include <state_pops.h>
#include <cmath>

std::vector<config> Get_Default_Configuration(unsigned int i_uiNe)
{
	std::vector<config> vRet;
	switch (i_uiNe)
	{
	case 134:
		vRet.push_back(config(6,3));
	case 133:
		vRet.push_back(config(6,3));
	case 132:
		vRet.push_back(config(6,3));
	case 131:
		vRet.push_back(config(6,3));
	case 130:
		vRet.push_back(config(6,3));
	case 129:
		vRet.push_back(config(6,3));
	case 128:
		vRet.push_back(config(6,3));
	case 127:
		vRet.push_back(config(6,3));
	case 126:
		vRet.push_back(config(6,3));
	case 125:
		vRet.push_back(config(6,3));
	case 124:
		vRet.push_back(config(6,3));
	case 123:
		vRet.push_back(config(6,3));
	case 122:
		vRet.push_back(config(6,3));
	case 121:
		vRet.push_back(config(6,3));
	case 120:
		vRet.push_back(config(8,0));
	case 119:
		vRet.push_back(config(8,0));
	case 118:
		vRet.push_back(config(7,1));
	case 117:
		vRet.push_back(config(7,1));
	case 116:
		vRet.push_back(config(7,1));
	case 115:
		vRet.push_back(config(7,1));
	case 114:
		vRet.push_back(config(7,1));
	case 113:
		vRet.push_back(config(7,1));
	case 112:
		vRet.push_back(config(6,2));
	case 111:
		vRet.push_back(config(6,2));
	case 110:
		vRet.push_back(config(6,2));
	case 109:
		vRet.push_back(config(6,2));
	case 108:
		vRet.push_back(config(6,2));
	case 107:
		vRet.push_back(config(6,2));
	case 106:
		vRet.push_back(config(6,2));
	case 105:
		vRet.push_back(config(6,2));
	case 104:
		vRet.push_back(config(6,2));
	case 103:
		vRet.push_back(config(6,2));
	case 102:
		vRet.push_back(config(5,3));
	case 101:
		vRet.push_back(config(5,3));
	case 100:
		vRet.push_back(config(5,3));
	case 99:
		vRet.push_back(config(5,3));
	case 98:
		vRet.push_back(config(5,3));
	case 97:
		vRet.push_back(config(5,3));
	case 96:
		vRet.push_back(config(5,3));
	case 95:
		vRet.push_back(config(5,3));
	case 94:
		vRet.push_back(config(5,3));
	case 93:
		vRet.push_back(config(5,3));
	case 92:
		vRet.push_back(config(5,3));
	case 91:
		vRet.push_back(config(5,3));
	case 90:
		vRet.push_back(config(5,3));
	case 89:
		vRet.push_back(config(5,3));
	case 88:
		vRet.push_back(config(7,0));
	case 87:
		vRet.push_back(config(7,0));
	case 86:
		vRet.push_back(config(6,1));
	case 85:
		vRet.push_back(config(6,1));
	case 84:
		vRet.push_back(config(6,1));
	case 83:
		vRet.push_back(config(6,1));
	case 82:
		vRet.push_back(config(6,1));
	case 81:
		vRet.push_back(config(6,1));
	case 80:
		vRet.push_back(config(5,2));
	case 79:
		vRet.push_back(config(5,2));
	case 78:
		vRet.push_back(config(5,2));
	case 77:
		vRet.push_back(config(5,2));
	case 76:
		vRet.push_back(config(5,2));
	case 75:
		vRet.push_back(config(5,2));
	case 74:
		vRet.push_back(config(5,2));
	case 73:
		vRet.push_back(config(5,2));
	case 72:
		vRet.push_back(config(5,2));
	case 71:
		vRet.push_back(config(5,2));
	case 70:
		vRet.push_back(config(4,3));
	case 69:
		vRet.push_back(config(4,3));
	case 68:
		vRet.push_back(config(4,3));
	case 67:
		vRet.push_back(config(4,3));
	case 66:
		vRet.push_back(config(4,3));
	case 65:
		vRet.push_back(config(4,3));
	case 64:
		vRet.push_back(config(4,3));
	case 63:
		vRet.push_back(config(4,3));
	case 62:
		vRet.push_back(config(4,3));
	case 61:
		vRet.push_back(config(4,3));
	case 60:
		vRet.push_back(config(4,3));
	case 59:
		vRet.push_back(config(4,3));
	case 58:
		vRet.push_back(config(4,3));
	case 57:
		vRet.push_back(config(4,3));
	case 56:
		vRet.push_back(config(6,0));
	case 55:
		vRet.push_back(config(6,0));
	case 54:
		vRet.push_back(config(5,1));
	case 53:
		vRet.push_back(config(5,1));
	case 52:
		vRet.push_back(config(5,1));
	case 51:
		vRet.push_back(config(5,1));
	case 50:
		vRet.push_back(config(5,1));
	case 49:
		vRet.push_back(config(5,1));
	case 48:
		vRet.push_back(config(4,2));
	case 47:
		vRet.push_back(config(4,2));
	case 46:
		vRet.push_back(config(4,2));
	case 45:
		vRet.push_back(config(4,2));
	case 44:
		vRet.push_back(config(4,2));
	case 43:
		vRet.push_back(config(4,2));
	case 42:
		vRet.push_back(config(4,2));
	case 41:
		vRet.push_back(config(4,2));
	case 40:
		vRet.push_back(config(4,2));
	case 39:
		vRet.push_back(config(4,2));
	case 38:
		vRet.push_back(config(5,0));
	case 37:
		vRet.push_back(config(5,0));
	case 36:
		vRet.push_back(config(4,1));
	case 35:
		vRet.push_back(config(4,1));
	case 34:
		vRet.push_back(config(4,1));
	case 33:
		vRet.push_back(config(4,1));
	case 32:
		vRet.push_back(config(4,1));
	case 31:
		vRet.push_back(config(4,1));
	case 30:
		vRet.push_back(config(3,2));
	case 29:
		vRet.push_back(config(3,2));
	case 28:
		vRet.push_back(config(3,2));
	case 27:
		vRet.push_back(config(3,2));
	case 26:
		vRet.push_back(config(3,2));
	case 25:
		vRet.push_back(config(3,2));
	case 24:
		vRet.push_back(config(3,2));
	case 23:
		vRet.push_back(config(3,2));
	case 22:
		vRet.push_back(config(3,2));
	case 21:
		vRet.push_back(config(3,2));
	case 20:
		vRet.push_back(config(4,0));
	case 19:
		vRet.push_back(config(4,0));
	case 18:
		vRet.push_back(config(3,1));
	case 17:
		vRet.push_back(config(3,1));
	case 16:
		vRet.push_back(config(3,1));
	case 15:
		vRet.push_back(config(3,1));
	case 14:
		vRet.push_back(config(3,1));
	case 13:
		vRet.push_back(config(3,1));
	case 12:
		vRet.push_back(config(3,0));
	case 11:
		vRet.push_back(config(3,0));
	case 10:
		vRet.push_back(config(2,1));
	case  9:
		vRet.push_back(config(2,1));
	case  8:
		vRet.push_back(config(2,1));
	case  7:
		vRet.push_back(config(2,1));
	case  6:
		vRet.push_back(config(2,1));
	case  5:
		vRet.push_back(config(2,1));
	case  4:
		vRet.push_back(config(2,0));
	case  3:
		vRet.push_back(config(2,0));
	case  2:
		vRet.push_back(config(1,0));
	case  1:
		vRet.push_back(config(1,0));
	}
	return vRet;
}

bool operator ==(const std::vector<config> & i_vccLHO, const std::vector<config> i_vccRHO)
{
	bool bRet = true;
	bRet = (i_vccLHO.size() == i_vccRHO.size());
	for (unsigned int uiI = 0; uiI < i_vccLHO.size() && bRet; uiI++)
	{
		bRet &= (i_vccLHO[uiI].m_uin == i_vccRHO[uiI].m_uin);
		bRet &= (i_vccLHO[uiI].m_uil == i_vccRHO[uiI].m_uil);
		bRet &= (i_vccLHO[uiI].m_uiS == i_vccRHO[uiI].m_uiS || i_vccLHO[uiI].m_uiS == -1 || i_vccRHO[uiI].m_uiS == -1);
		bRet &= (fabs(i_vccLHO[uiI].m_dL - i_vccRHO[uiI].m_dL) < 0.01  || i_vccLHO[uiI].m_dL  == -1 || i_vccRHO[uiI].m_dL  == -1);
		bRet &= (i_vccLHO[uiI].m_uiP == i_vccRHO[uiI].m_uiP || i_vccLHO[uiI].m_uiP == -1 || i_vccRHO[uiI].m_uiP == -1);
	}
	return bRet;
}
