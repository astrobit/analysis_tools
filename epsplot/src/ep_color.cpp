#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <line_routines.h>
#include <eps_plot.h>
#include <sstream>

const epsplot::color_triplet	epsplot::g_cColor_Std_Black(0.0,0.0,0.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Red(1.0,0.0,0.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Green(0.0,1.0,0.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Blue(0.0,0.0,1.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Cyan(0.0,1.0,1.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Magenta(1.0,0.0,1.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Yellow(1.0,1.0,0.0);
const epsplot::color_triplet	epsplot::g_cColor_Std_Grey_25(0.25,0.25,0.25);
const epsplot::color_triplet	epsplot::g_cColor_Std_Grey_50(0.50,0.50,0.50);
const epsplot::color_triplet	epsplot::g_cColor_Std_Grey_75(0.75,0.75,0.75);
const epsplot::color_triplet	epsplot::g_cColor_Std_White(1.0,1.0,1.0);

using namespace epsplot;

void	data::Define_Custom_Color(COLOR i_eColor, const color_triplet & i_cColor)
{
	if (i_eColor >= CLR_CUSTOM_1 && i_eColor <= CLR_CUSTOM_16)
	{
		m_cCustom_Colors[i_eColor - CLR_CUSTOM_1] = i_cColor;
	}
}
color_triplet data::Get_Color(COLOR i_eColor) const
{
	color_triplet cRet(-1.0,-1.0,-1.0); // invalid color
	switch (i_eColor)
	{
	case CLR_CUSTOM_1:
	case CLR_CUSTOM_2:
	case CLR_CUSTOM_3:
	case CLR_CUSTOM_4:
	case CLR_CUSTOM_5:
	case CLR_CUSTOM_6:
	case CLR_CUSTOM_7:
	case CLR_CUSTOM_8:
	case CLR_CUSTOM_9:
	case CLR_CUSTOM_10:
	case CLR_CUSTOM_11:
	case CLR_CUSTOM_12:
	case CLR_CUSTOM_13:
	case CLR_CUSTOM_14:
	case CLR_CUSTOM_15:
	case CLR_CUSTOM_16:
		cRet = m_cCustom_Colors[i_eColor - CLR_CUSTOM_1];
		break;
	case BLACK:
		cRet = g_cColor_Std_Black;
		break;
	case RED:
		cRet = g_cColor_Std_Red;
		break;
	case GREEN:
		cRet = g_cColor_Std_Green;
		break;
	case BLUE:
		cRet = g_cColor_Std_Blue;
		break;
	case CYAN:
		cRet = g_cColor_Std_Cyan;
		break;
	case MAGENTA:
		cRet = g_cColor_Std_Magenta;
		break;
	case YELLOW:
		cRet = g_cColor_Std_Yellow;
		break;
	case WHITE:
		cRet = g_cColor_Std_White;
		break;
	case GRAY_25:
	case GREY_25:
		cRet = g_cColor_Std_Grey_25;
		break;
	case GRAY_50:
	case GREY_50:
		cRet = g_cColor_Std_Grey_50;
		break;
	case GRAY_75:
	case GREY_75:
		cRet = g_cColor_Std_Grey_75;
		break;
	default: // sometimes the compiler wants this case?
		break; 
	}
	return cRet;
}
