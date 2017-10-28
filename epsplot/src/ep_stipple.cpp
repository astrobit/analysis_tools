#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <unistd.h>
#include <eps_plot.h>
#include <sstream>

using namespace epsplot;


#define LDASHLEN 16
const double		g_dStipple_Std_Short_Dash[] = {LDASHLEN >> 1,LDASHLEN >> 1};
const double		g_dStipple_Std_Long_Dash[] = {LDASHLEN,LDASHLEN};
const double		g_dStipple_Std_Long_Short_Dash[] = {LDASHLEN,LDASHLEN,LDASHLEN >> 1,LDASHLEN};
const double		g_dStipple_Std_Dotted[] = {LDASHLEN >> 3,LDASHLEN >> 3}; // maybe 1,1?
const double		g_dStipple_Std_Short_Dash_Dotted[] = {LDASHLEN >> 1,LDASHLEN >> 1,LDASHLEN >> 3,LDASHLEN >> 1};
const double		g_dStipple_Std_Long_Dash_Dotted[] = {LDASHLEN,LDASHLEN,LDASHLEN >> 3,LDASHLEN};
const double		g_dStipple_Std_Long_Short_Dash_Dotted[] = {LDASHLEN,LDASHLEN,LDASHLEN >> 1,LDASHLEN,LDASHLEN >> 3,LDASHLEN};
const double		g_dStipple_Std_Long_Short_Dot_Short_Dash[] = {LDASHLEN,LDASHLEN,LDASHLEN >> 1,LDASHLEN,LDASHLEN >> 3,LDASHLEN,LDASHLEN >> 1,LDASHLEN};
const double		g_dStipple_Std_Long_Long_Dash[] = {LDASHLEN << 1,LDASHLEN << 1};
#undef LDASHLEN


void	data::Define_Custom_Stipple(STIPPLE i_eStipple, const double * i_lpdStipple, unsigned int i_uiStipple_Length)
{
	if (i_eStipple >= STPL_CUSTOM_1 && i_eStipple <= STPL_CUSTOM_16)
	{
		unsigned int uiIdx = (i_eStipple - STPL_CUSTOM_1);
		if (m_lpdCustom_Stipple[uiIdx])
			delete [] m_lpdCustom_Stipple[uiIdx];
		m_lpdCustom_Stipple[uiIdx] = NULL;
		if (i_uiStipple_Length > 0)
		{
			m_lpdCustom_Stipple[uiIdx] = new double [i_uiStipple_Length];
			memcpy(m_lpdCustom_Stipple[uiIdx],i_lpdStipple,i_uiStipple_Length * sizeof(double));
		}
		m_uiStipple_Length[uiIdx] = i_uiStipple_Length;
	}
}

const double *	data::Get_Stipple(STIPPLE i_eStipple, unsigned int & o_uiStipple_Length) const
{
	const double * lpdRet = NULL;
	o_uiStipple_Length = 0;

	switch (i_eStipple)
	{
	case SOLID:
	default: 
		break;
	case SHORT_DASH:
		lpdRet = g_dStipple_Std_Short_Dash;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Short_Dash) / sizeof(double);
		break;
	case LONG_DASH:
		lpdRet = g_dStipple_Std_Long_Dash;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Long_Dash) / sizeof(double);
		break;
	case LONG_LONG_DASH:
		lpdRet = g_dStipple_Std_Long_Long_Dash;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Long_Long_Dash) / sizeof(double);
		break;
	case LONG_SHORT_DASH:
		lpdRet = g_dStipple_Std_Long_Short_Dash;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Long_Short_Dash) / sizeof(double);
		break;
	case DOTTED:
		lpdRet = g_dStipple_Std_Dotted;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Dotted) / sizeof(double);
		break;
	case SHORT_DASH_DOTTED:
		lpdRet = g_dStipple_Std_Short_Dash_Dotted;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Short_Dash_Dotted) / sizeof(double);
		break;
	case LONG_DASH_DOTTED:
		lpdRet = g_dStipple_Std_Long_Dash_Dotted;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Long_Dash_Dotted) / sizeof(double);
		break;
	case LONG_SHORT_DASH_DOTTED:
		lpdRet = g_dStipple_Std_Long_Short_Dash_Dotted;
		o_uiStipple_Length = sizeof(g_dStipple_Std_Long_Short_Dash_Dotted) / sizeof(double);
		break;
	case STPL_CUSTOM_1:
	case STPL_CUSTOM_2:
	case STPL_CUSTOM_3:
	case STPL_CUSTOM_4:
	case STPL_CUSTOM_5:
	case STPL_CUSTOM_6:
	case STPL_CUSTOM_7:
	case STPL_CUSTOM_8:
	case STPL_CUSTOM_9:
	case STPL_CUSTOM_10:
	case STPL_CUSTOM_11:
	case STPL_CUSTOM_12:
	case STPL_CUSTOM_13:
	case STPL_CUSTOM_14:
	case STPL_CUSTOM_15:
	case STPL_CUSTOM_16:
		{
			unsigned int uiIdx = (i_eStipple - STPL_CUSTOM_1);
			lpdRet = m_lpdCustom_Stipple[uiIdx];
			o_uiStipple_Length = m_uiStipple_Length[uiIdx];
		}
		break;
	}
	return lpdRet;
}

