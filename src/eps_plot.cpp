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

using namespace epsplot;

const color_triplet	g_cColor_Std_Black(0.0,0.0,0.0);
const color_triplet	g_cColor_Std_Red(1.0,0.0,0.0);
const color_triplet	g_cColor_Std_Green(0.0,1.0,0.0);
const color_triplet	g_cColor_Std_Blue(0.0,0.0,1.0);
const color_triplet	g_cColor_Std_Cyan(0.0,1.0,1.0);
const color_triplet	g_cColor_Std_Magenta(1.0,0.0,1.0);
const color_triplet	g_cColor_Std_Yellow(1.0,1.0,0.0);
const color_triplet	g_cColor_Std_Grey_25(0.25,0.25,0.25);
const color_triplet	g_cColor_Std_Grey_50(0.50,0.50,0.50);
const color_triplet	g_cColor_Std_Grey_75(0.75,0.75,0.75);
const color_triplet	g_cColor_Std_White(1.0,1.0,1.0);

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


class SYMBOL_CONTAINER
{
public:
	std::vector<eps_pair>	m_vSquare;
	std::vector<eps_pair>	m_vCircle;
	std::vector<eps_pair>	m_vTriangle_Up;
	std::vector<eps_pair>	m_vTriangle_Down;
	std::vector<eps_pair>	m_vTriangle_Left;
	std::vector<eps_pair>	m_vTriangle_Right;
	std::vector<eps_pair>	m_vDiamond;
	std::vector<eps_pair>	m_vTimes;
	std::vector<eps_pair>	m_vPlus;
	std::vector<eps_pair>	m_vDash;
	std::vector<eps_pair>	m_vAsterisk;
	std::vector<eps_pair>	m_vStar_4;
	std::vector<eps_pair>	m_vStar_5;
	std::vector<eps_pair>	m_vStar_6;
	std::vector<eps_pair>	m_vUser_Symbols[16];

	SYMBOL_CONTAINER(void)
	{
		m_vSquare.push_back(eps_pair(-0.5,-0.5));
		m_vSquare.push_back(eps_pair(-0.5, 0.5));
		m_vSquare.push_back(eps_pair( 0.5, 0.5));
		m_vSquare.push_back(eps_pair( 0.5,-0.5));
		double	dPi = acos(-1.0);
		double dAngle;
		for (dAngle = 0.0; dAngle < (2. * dPi); dAngle += ((2. * dPi) / 32.))
		{
			m_vCircle.push_back(eps_pair(0.5*sin(dAngle),0.5*cos(dAngle)));
		}

		m_vTriangle_Up.push_back(eps_pair(-0.5,-0.5));
		m_vTriangle_Up.push_back(eps_pair( 0.0, 0.5));
		m_vTriangle_Up.push_back(eps_pair( 0.5,-0.5));

		m_vTriangle_Down.push_back(eps_pair( 0.5, 0.5));
		m_vTriangle_Down.push_back(eps_pair( 0.0,-0.5));
		m_vTriangle_Down.push_back(eps_pair(-0.5, 0.5));

		m_vTriangle_Left.push_back(eps_pair( 0.5,-0.5));
		m_vTriangle_Left.push_back(eps_pair(-0.5, 0.0));
		m_vTriangle_Left.push_back(eps_pair( 0.5, 0.5));

		m_vTriangle_Right.push_back(eps_pair(-0.5, 0.5));
		m_vTriangle_Right.push_back(eps_pair( 0.5, 0.0));
		m_vTriangle_Right.push_back(eps_pair(-0.5,-0.5));

		m_vDiamond.push_back(eps_pair(-0.5, 0.0));
		m_vDiamond.push_back(eps_pair( 0.0, 0.5));
		m_vDiamond.push_back(eps_pair( 0.5, 0.0));
		m_vDiamond.push_back(eps_pair( 0.0,-0.5));

		m_vTimes.push_back(eps_pair(-0.5			, 0.5 - 1./12.	));
		m_vTimes.push_back(eps_pair(-0.5 + 1/12.	, 0.5			));
		m_vTimes.push_back(eps_pair(0.0			, 1/12.			));
		m_vTimes.push_back(eps_pair( 0.5 - 1/12.	, 0.5			));
		m_vTimes.push_back(eps_pair( 0.5			, 0.5 - 1./12.	));
		m_vTimes.push_back(eps_pair(1/12.		, 0.0			));
		m_vTimes.push_back(eps_pair( 0.5			,-0.5 + 1./12.	));
		m_vTimes.push_back(eps_pair( 0.5 - 1/12.	,-0.5			));
		m_vTimes.push_back(eps_pair(0.0			, -1/12.		));
		m_vTimes.push_back(eps_pair(-0.5 + 1/12.	,-0.5			));
		m_vTimes.push_back(eps_pair(-0.5			,-0.5 + 1./12.	));
		m_vTimes.push_back(eps_pair(-1/12.		, 0.0			));

		m_vDash.push_back(eps_pair(-0.5			, 1/12.			));
		m_vDash.push_back(eps_pair( 0.5			, 1/12.			));
		m_vDash.push_back(eps_pair( 0.5			,-1/12.			));
		m_vDash.push_back(eps_pair(-0.5			,-1/12.			));

		m_vPlus.push_back(eps_pair(-0.5			, 1/12.			));
		m_vPlus.push_back(eps_pair(-1/12.		, 1/12.			));
		m_vPlus.push_back(eps_pair(-1/12.		, 0.5			));
		m_vPlus.push_back(eps_pair( 1/12.		, 0.5			));
		m_vPlus.push_back(eps_pair( 1/12.		, 1/12.			));
		m_vPlus.push_back(eps_pair( 0.5			, 1/12.			));
		m_vPlus.push_back(eps_pair( 0.5			,-1/12.			));
		m_vPlus.push_back(eps_pair( 1/12.		,-1/12.			));
		m_vPlus.push_back(eps_pair( 1/12.		,-0.5			));
		m_vPlus.push_back(eps_pair(-1/12.		,-0.5			));
		m_vPlus.push_back(eps_pair(-1/12.		,-1/12.			));
		m_vPlus.push_back(eps_pair(-0.5			,-1/12.			));

		for (unsigned int uiArm = 0; uiArm < 8; uiArm++)
		{
			dAngle = uiArm * dPi * 0.25;
			m_vAsterisk.push_back(eps_pair(-0.5 * cos(dAngle) - 1/12. * sin(dAngle),-1/12.*cos(dAngle) + 0.5 * sin(dAngle)));
			m_vAsterisk.push_back(eps_pair(-0.5 * cos(dAngle) + 1/12. * sin(dAngle), 1/12.*cos(dAngle) + 0.5 * sin(dAngle)));
			m_vAsterisk.push_back(eps_pair( 0.0, 0.0));
		}

		for (unsigned int uiArm = 0; uiArm < 4; uiArm++)
		{
			dAngle = uiArm * dPi * 0.5;
			m_vStar_4.push_back(eps_pair(-0.5 * cos(dAngle), 0.5 * sin(dAngle)));
			m_vStar_4.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
		}

		for (unsigned int uiArm = 0; uiArm < 5; uiArm++)
		{
			dAngle = uiArm * dPi * 0.4;
			m_vStar_5.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
			m_vStar_5.push_back(eps_pair(0.5 * sin(dAngle), 0.5 * cos(dAngle)));
		}

		for (unsigned int uiArm = 0; uiArm < 6; uiArm++)
		{
			dAngle = uiArm * dPi / 3.;
			m_vStar_6.push_back(eps_pair(-2./12. * cos(dAngle) + 2./12.*sin(dAngle), 2./12. * cos(dAngle) + 2./12.*sin(dAngle)));
			m_vStar_6.push_back(eps_pair(0.5 * sin(dAngle), 0.5 * cos(dAngle)));
		}
	}
};

SYMBOL_CONTAINER	g_cSymbol_Containers;


epsfile::epsfile(const char * i_lpszFormat)
{
	strcpy(m_lpszFormat,i_lpszFormat);
	m_lpFileOut = NULL;
	m_lpszFilename = NULL;
	strcpy(m_lpszGsave,"gs\n");
	strcpy(m_lpszGrestore,"gr\n");
	strcpy(m_lpszStroke,"s\n");
	strcpy(m_lpszFill,"f\n");
	strcpy(m_lpszClosepath,"cp\n");
	strcpy(m_lpszTxtCentered,"(%s) txcnt\n");
	strcpy(m_lpszScalefontSetFont,"%i fs\n");
	strcpy(m_lpszSetDash,"sd\n");

	sprintf(m_lpszMoveto,"%s %s m\n",i_lpszFormat,i_lpszFormat);
	sprintf(m_lpszLineto,"%s %s l\n",i_lpszFormat,i_lpszFormat);
	sprintf(m_lpszTranslate,"%s %s t\n",i_lpszFormat,i_lpszFormat);
	sprintf(m_lpszSetLineWidth,"%s slw\n",i_lpszFormat);	
	sprintf(m_lpszRectClip,"%s %s %s %s rc\n",i_lpszFormat,i_lpszFormat,i_lpszFormat,i_lpszFormat);
	strcpy(m_lpszSetRGBColor,"%.3f %.3f %.3f srgb\n");

	// generate symbol mapping from symbols the user will use to those used by postscript
	// a useful and perhaps comprehensive list of the postscript commands can be found at http://www.jdawiseman.com/papers/trivia/character-entities.html
	m_mSymbol_Map["Ang"] = "Aring";
	m_mSymbol_Map["Aring"] = "Aring";
	m_mSymbol_Map["Alpha"] = "Alpha";
	m_mSymbol_Map["Beta"] = "Beta";
	m_mSymbol_Map["Gamma"] = "Gamma";
	m_mSymbol_Map["Delta"] = "Deltagreek";
	m_mSymbol_Map["Epsilon"] = "Epsilon";
	m_mSymbol_Map["Zeta"] = "Zeta";
	m_mSymbol_Map["Eta"] = "Eta";
	m_mSymbol_Map["Theta"] = "Theta";
	m_mSymbol_Map["Iota"] = "Iota";
	m_mSymbol_Map["Kappa"] = "Kappa";
	m_mSymbol_Map["Lambda"] = "Lambda";
	m_mSymbol_Map["Mu"] = "Mu";
	m_mSymbol_Map["Nu"] = "Nu";
	m_mSymbol_Map["Xi"] = "Xi";
	m_mSymbol_Map["Omicron"] = "Omicron";
	m_mSymbol_Map["Pi"] = "Pi";
	m_mSymbol_Map["Rho"] = "Rho";
	m_mSymbol_Map["Sigma"] = "Sigma";
	m_mSymbol_Map["Tau"] = "Tau";
	m_mSymbol_Map["Upsilon"] = "Upsilon";
	m_mSymbol_Map["Phi"] = "Phi";
	m_mSymbol_Map["Chi"] = "Chi";
	m_mSymbol_Map["Psi"] = "Psi";
	m_mSymbol_Map["Omega"] = "Omegagreek";
	m_mSymbol_Map["alpha"] = "alpha";
	m_mSymbol_Map["beta"] = "beta";
	m_mSymbol_Map["gamma"] = "gamma";
	m_mSymbol_Map["delta"] = "delta";
	m_mSymbol_Map["epsilon"] = "epsilon";
	m_mSymbol_Map["zeta"] = "zeta";
	m_mSymbol_Map["eta"] = "eta";
	m_mSymbol_Map["theta"] = "theta";
	m_mSymbol_Map["iota"] = "iota";
	m_mSymbol_Map["kappa"] = "kappa";
	m_mSymbol_Map["lambda"] = "lambda";
	m_mSymbol_Map["mu"] = "mugreek";
	m_mSymbol_Map["nu"] = "nu";
	m_mSymbol_Map["xi"] = "xi";
	m_mSymbol_Map["omicron"] = "omicron";
	m_mSymbol_Map["pi"] = "pi";
	m_mSymbol_Map["rho"] = "rho";
	m_mSymbol_Map["sigma"] = "sigma";
	m_mSymbol_Map["tau"] = "tau";
	m_mSymbol_Map["upsilon"] = "upsilon";
	m_mSymbol_Map["phi"] = "phi";
	m_mSymbol_Map["chi"] = "chi";
	m_mSymbol_Map["psi"] = "psi";
	m_mSymbol_Map["omega"] = "omega";
	m_mSymbol_Map["vartheta"] = "thetasymbolgreek";
	m_mSymbol_Map["varUpsilon"] = "Upsilonhooksymbol";
	m_mSymbol_Map["varpi"] = "pisymbolgreek";
	m_mSymbol_Map["prod"] = glyphsymbol("product",true);
	m_mSymbol_Map["sum"] = "Sigma";
	m_mSymbol_Map["oplus"] = glyphsymbol("circleplus",true);
	m_mSymbol_Map["earth"] = glyphsymbol("circleplus",true);

}
epsfile::~epsfile(void)
{
	Close_File();
	if (m_lpszFilename)
		delete [] m_lpszFilename;
	m_lpszFilename = NULL;
}
void	epsfile::Close_File(void)
{
	if (m_lpFileOut)
	{
		fprintf(m_lpFileOut,m_lpszGrestore);
		fprintf(m_lpFileOut,"showpage\n");
		fprintf(m_lpFileOut,"%%%%EOF\n");
		fclose(m_lpFileOut);
	}
	m_lpFileOut = NULL;
}
void epsfile::Open_File(const char * i_lpszFilename, const char * i_lpszDocument_Title, const double & i_dWidth_Inches, const double & i_dHeight_Inches)
{
	struct tm * timeinfo;
	time_t rawtime;
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	m_lpszFilename = new char [strlen(i_lpszFilename) + 1];
	strcpy(m_lpszFilename,i_lpszFilename);
	m_lpFileOut = fopen(m_lpszFilename,"wt");
	fprintf(m_lpFileOut,"%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(m_lpFileOut,"%%%%Creator: B. W. Mulligan; Plot_Utilities: Plot_EPS_Grid\n");
	fprintf(m_lpFileOut,"%%%%Title: %s\n",i_lpszDocument_Title && i_lpszDocument_Title[0] != 0 ? i_lpszDocument_Title : i_lpszFilename);
	fprintf(m_lpFileOut,"%%%%CreationDate: %s",asctime(timeinfo));
	fprintf(m_lpFileOut,"%%%%DocumentData: Clean7Bit\n");
	if (i_dHeight_Inches < i_dWidth_Inches)
		fprintf(m_lpFileOut,"%%%%BoundingBox: 0 0 %.2f %.2f\n",i_dHeight_Inches * 72.0, i_dWidth_Inches * 72.0);
	else
		fprintf(m_lpFileOut,"%%%%BoundingBox: 0 0 %.2f %.2f\n",i_dWidth_Inches * 72.0, i_dHeight_Inches * 72.0);
	fprintf(m_lpFileOut,"%%%%LanguageLevel: 2 [could be 1 2 or 3]\n");
	fprintf(m_lpFileOut,"%%%%Pages: 1\n");
	fprintf(m_lpFileOut,"%%%%Page: 1 1\n");
	fprintf(m_lpFileOut,"%%%%Page: 1 1\n");
	fprintf(m_lpFileOut,"/sd {setdash} bind def\n");
	fprintf(m_lpFileOut,"/gs {gsave} bind def\n");
	fprintf(m_lpFileOut,"/gr {grestore} bind def\n");
	fprintf(m_lpFileOut,"/m {newpath moveto} bind def\n");
	fprintf(m_lpFileOut,"/l {lineto} bind def\n");
	fprintf(m_lpFileOut,"/s {stroke} bind def\n");
	fprintf(m_lpFileOut,"/f {fill} bind def\n");
	fprintf(m_lpFileOut,"/t {translate} bind def\n");
	fprintf(m_lpFileOut,"/cp {closepath} bind def\n");
	fprintf(m_lpFileOut,"/rc {rectclip} bind def\n");
	fprintf(m_lpFileOut,"/slw {setlinewidth} bind def\n");
	fprintf(m_lpFileOut,"/srgb {setrgbcolor} bind def\n");
	fprintf(m_lpFileOut,"/txcnt {dup stringwidth pop 2 div neg 0 rmoveto} bind def\n");
	fprintf(m_lpFileOut,"/fs {scalefont setfont} bind def \n");
	fprintf(m_lpFileOut,"/tbb {dup true charpath pathbbox 3 -1 roll dup 3 1 roll sub dup 1.2 mul 3 1 roll 0.1 mul sub 4 -1 roll dup 5 -1 roll exch sub dup 1.2 mul 3 1 roll 0.1 mul sub 4 1 roll exch 3 1 roll exch} bind def \n");
	fprintf(m_lpFileOut,m_lpszGsave);
	if (i_dHeight_Inches < i_dWidth_Inches) // rotate to landscape mode
		fprintf(m_lpFileOut,"90 rotate 0 -%.2f translate\n",i_dHeight_Inches * 72.0);
}
void epsfile::Text(PS_FONT i_eFont, bool i_bItalic, bool i_bBold, int i_iFont_Size, PS_HORIZONTAL_JUSTIFICATION i_eHoirzontal_Justification, PS_VERTICAL_JUSTIFICATION i_eVertical_Justification, const color_triplet & i_cColor,const double & i_dX, const double & i_dY, const char * i_lpszText, const double & i_dRotation, const double & i_dLine_Width) const
{
	std::string szFont;
	fprintf(m_lpFileOut,m_lpszGsave);
	fprintf(m_lpFileOut,m_lpszSetLineWidth,i_dLine_Width);
	switch (i_eFont)
	{
	case TIMES:
		if (i_bItalic && i_bBold)
			szFont = "/Times-BoldItalic findfont\n";
		else if (i_bBold)
			szFont = "/Times-Bold findfont\n";
		else if (i_bItalic)
			szFont = "/Times-Italic findfont\n";
		else
			szFont = "/Times-Roman findfont\n";
		break;
	case HELVETICA:
		if (i_bItalic && i_bBold)
			szFont = "/Helvetica-BoldItalic findfont\n";
		else if (i_bBold)
			szFont = "/Helvetica-Bold findfont\n";
		else if (i_bItalic)
			szFont = "/Helvetica-Italic findfont\n";
		else
			szFont = "/Helvetica-Roman findfont\n";
		break;
	case COURIER:
		if (i_bItalic && i_bBold)
			szFont = "/Courier-BoldItalic findfont\n";
		else if (i_bBold)
			szFont = "/Courier-Bold findfont\n";
		else if (i_bItalic)
			szFont = "/Courier-Italic findfont\n";
		else
			szFont = "/Courier-Roman findfont\n";
		break;
	case SYMBOL:
		szFont = "/Symbol findfont\n";
		break;
	}
	fprintf(m_lpFileOut,szFont.c_str());

	fprintf(m_lpFileOut,m_lpszScalefontSetFont,i_iFont_Size);
	fprintf(m_lpFileOut,m_lpszMoveto, i_dX,i_dY);
	if (i_dRotation != 0.0)
	{
		fprintf(m_lpFileOut,m_lpszFormat,i_dRotation);
		fprintf(m_lpFileOut," rotate\n");
	}
	fprintf(m_lpFileOut,m_lpszSetRGBColor, i_cColor.m_dRed, i_cColor.m_dGreen, i_cColor.m_dBlue);
	fprintf(m_lpFileOut,"%% text %s\n",i_lpszText);
	std::vector< text_entity > vText_Stream = Parse_String(i_lpszText);
	if (i_eHoirzontal_Justification == CENTER || i_eHoirzontal_Justification == RIGHT ||
		i_eVertical_Justification == TOP || i_eVertical_Justification == MIDDLE)
	{
		// first compute position to start drawing
		fprintf(m_lpFileOut,"(");
		for (std::vector< text_entity >::iterator cI = vText_Stream.begin(); cI != vText_Stream.end(); cI++)
		{
			switch (cI->m_eType)
			{
			case text_entity::text:
				fprintf(m_lpFileOut,"%s",cI->m_szData.c_str());
				break;
			case text_entity::symbol:
				fprintf(m_lpFileOut," "); // for the moment, use a space to represent the glyph.  This will not really gnerate the correct size, but it should be close enough
				break;
			}
		}
		// get complete bounding box, then perform operations on it
		fprintf(m_lpFileOut, ") true charpath pathbbox 3 -1 roll sub ");
		if (i_eVertical_Justification == TOP)
			fprintf(m_lpFileOut, "neg ");
		else if (i_eVertical_Justification == MIDDLE)
			fprintf(m_lpFileOut,"-0.5 mul ");
		else
			fprintf(m_lpFileOut,"pop 0 ");

		fprintf(m_lpFileOut,"3 1 roll sub ");

		if (i_eHoirzontal_Justification == RIGHT)
			fprintf(m_lpFileOut,"2.0 mul ");
		else if (i_eHoirzontal_Justification == CENTER)
			fprintf(m_lpFileOut,"1.5 mul ");
		else
			fprintf(m_lpFileOut," ");
		fprintf(m_lpFileOut,"exch rmoveto\n");
	}

	for (std::vector< text_entity >::iterator cI = vText_Stream.begin(); cI != vText_Stream.end(); cI++)
	{
		switch (cI->m_eType)
		{
		case text_entity::text:
			fprintf(m_lpFileOut,"(%s) show\n",cI->m_szData.c_str());
			break;
		case text_entity::symbol:
			if (m_mSymbol_Map.count(cI->m_szData) == 1)
			{
				std::string szSymb = m_mSymbol_Map.at(cI->m_szData);
				if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
					fprintf(m_lpFileOut,"/Symbol findfont\n");

				fprintf(m_lpFileOut,"/%s glyphshow\n",szSymb.c_str());

				if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
					fprintf(m_lpFileOut,szFont.c_str());

			}
			break;
		case text_entity::superscript:
			fprintf(m_lpFileOut,m_lpszGsave);
			fprintf(m_lpFileOut,szFont.c_str());
			fprintf(m_lpFileOut,m_lpszScalefontSetFont,(int)(i_iFont_Size * 0.45));
			fprintf(m_lpFileOut, "%.1f %.1f rmoveto\n",i_iFont_Size * 0.175,i_iFont_Size * 0.50);
			cI++;
			if (cI != vText_Stream.end() && cI->m_eType == text_entity::leftbrace)
			{
				cI++;
				if (cI != vText_Stream.end() && cI->m_eType == text_entity::text)
				{
					fprintf(m_lpFileOut,"(%s) show\n",cI->m_szData.c_str());
					cI++;
				}
				else if (cI != vText_Stream.end() && cI->m_eType == text_entity::symbol && m_mSymbol_Map.count(cI->m_szData) == 1)
				{
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,"/Symbol findfont\n");

					fprintf(m_lpFileOut,"/%s glyphshow\n",m_mSymbol_Map.at(cI->m_szData).c_str());

					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,szFont.c_str());
					cI++;
				}
				if (cI != vText_Stream.end() && cI->m_eType != text_entity::rightbrace)
					fprintf(stderr,"Warning: epsplot encountered invalid superscript sequence\n");
			}
			else
			{ 
				if (cI != vText_Stream.end() && cI->m_eType == text_entity::text)
				{
					const char * lpszText = cI->m_szData.c_str();
					fprintf(m_lpFileOut,"(%c) show\n",lpszText[0]);
					lpszText++;
					cI->m_szData = lpszText;
					cI++;
				}
				else if (cI != vText_Stream.end() && cI->m_eType == text_entity::symbol && m_mSymbol_Map.count(cI->m_szData) == 1)
				{
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,"/Symbol findfont\n");
					fprintf(m_lpFileOut,"/%s glyphshow\n",m_mSymbol_Map.at(cI->m_szData).c_str());
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,szFont.c_str());
					cI++;
				}
			}
			fprintf(m_lpFileOut,m_lpszGrestore);
			break;
		case text_entity::subscript:
			fprintf(m_lpFileOut,m_lpszGsave);
			fprintf(m_lpFileOut,szFont.c_str());
			fprintf(m_lpFileOut,m_lpszScalefontSetFont,(int)(i_iFont_Size * 0.45));
			fprintf(m_lpFileOut, "%.1f %.1f rmoveto\n",i_iFont_Size * 0.065,i_iFont_Size * -0.25);
			cI++;
			if (cI != vText_Stream.end() && cI->m_eType == text_entity::leftbrace)
			{
				cI++;
				if (cI != vText_Stream.end() && cI->m_eType == text_entity::text)
				{
					fprintf(m_lpFileOut,"(%s) show\n",cI->m_szData.c_str());
					cI++;
				}
				else if (cI != vText_Stream.end() && cI->m_eType == text_entity::symbol && m_mSymbol_Map.count(cI->m_szData) == 1)
				{
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,"/Symbol findfont\n");
					fprintf(m_lpFileOut,"/%s glyphshow\n",m_mSymbol_Map.at(cI->m_szData).c_str());
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,szFont.c_str());
					cI++;
				}
				if (cI != vText_Stream.end() && cI->m_eType != text_entity::rightbrace)
					fprintf(stderr,"Warning: epsplot encountered invalid superscript sequence\n");
			}
			else
			{ 
				if (cI != vText_Stream.end() && cI->m_eType == text_entity::text)
				{
					const char * lpszText = cI->m_szData.c_str();
					fprintf(m_lpFileOut,"(%c) show\n",lpszText[0]);
					lpszText++;
					cI->m_szData = lpszText;
					cI++;
				}
				else if (cI != vText_Stream.end() && cI->m_eType == text_entity::symbol && m_mSymbol_Map.count(cI->m_szData) == 1)
				{
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,"/Symbol findfont\n");
					fprintf(m_lpFileOut,"/%s glyphshow\n",m_mSymbol_Map.at(cI->m_szData).c_str());
					if (m_mSymbol_Map.at(cI->m_szData).requires_symbol())
						fprintf(m_lpFileOut,szFont.c_str());
					cI++;
				}
			}
			fprintf(m_lpFileOut,m_lpszGrestore);
			break;
		}
	}
	fprintf(m_lpFileOut,m_lpszGrestore);
}

void epsfile::Rect_Clip(const double & i_dX, const double & i_dY, const double & i_dWidth, const double & i_dHeight) const
{
	fprintf(m_lpFileOut,m_lpszRectClip, i_dX,i_dY,i_dWidth,i_dHeight);
}
void epsfile::Move_To(const double & i_dX, const double & i_dY) const
{
	if (!std::isnan(i_dX) && !std::isnan(i_dY) && !std::isinf(i_dX) && !std::isinf(i_dY))
		fprintf(m_lpFileOut,m_lpszMoveto, i_dX,i_dY);
}
void epsfile::Line_To(const double & i_dX, const double & i_dY) const
{
	if (!std::isnan(i_dX) && !std::isnan(i_dY) && !std::isinf(i_dX) && !std::isinf(i_dY))
		fprintf(m_lpFileOut,m_lpszLineto, i_dX,i_dY);
}
void epsfile::Translate(const double & i_dX, const double & i_dY) const
{
	if (!std::isnan(i_dX) && !std::isnan(i_dY) && !std::isinf(i_dX) && !std::isinf(i_dY))
		fprintf(m_lpFileOut,m_lpszTranslate, i_dX,i_dY);
}
void epsfile::State_Push(void) const
{
	fprintf(m_lpFileOut,m_lpszGsave);
}
void epsfile::State_Pop(void) const
{
	fprintf(m_lpFileOut,m_lpszGrestore);
}
void epsfile::Set_Line_Width(const double &i_dLine_Width) const
{
	fprintf(m_lpFileOut,m_lpszSetLineWidth, i_dLine_Width);
}
void epsfile::Set_RGB_Color(const color_triplet & i_cColor) const
{
	fprintf(m_lpFileOut,m_lpszSetRGBColor, i_cColor.m_dRed, i_cColor.m_dGreen, i_cColor.m_dBlue);
}
void epsfile::Set_RGB_Color(PS_STANDARD_COLORS i_eColor) const
{
	color_triplet cColor;
	switch (i_eColor)
	{
	case STD_BLACK:
	default:
		cColor = g_cColor_Std_Black;
		break;
	case STD_WHITE:
		cColor = g_cColor_Std_White;
		break;
	case STD_RED:
		cColor = g_cColor_Std_Red;
		break;
	case STD_GREEN:
		cColor = g_cColor_Std_Green;
		break;
	case STD_BLUE:
		cColor = g_cColor_Std_Blue;
		break;
	case STD_CYAN:
		cColor = g_cColor_Std_Cyan;
		break;
	case STD_MAGENTA:
		cColor = g_cColor_Std_Magenta;
		break;
	case STD_YELLOW:
		cColor = g_cColor_Std_Yellow;
		break;
	}
	Set_RGB_Color(cColor);
}
void epsfile::Stroke(void) const
{
	fprintf(m_lpFileOut,m_lpszStroke);
}
void epsfile::Fill(void) const
{
	fprintf(m_lpFileOut,m_lpszFill);
}
void epsfile::Close_Path(void) const
{
	fprintf(m_lpFileOut,m_lpszClosepath);
}
void epsfile::Comment(const char * i_lpszComment) const
{
	fprintf(m_lpFileOut,"%% %s\n",i_lpszComment);
}
void epsfile::Set_Dash(const double * i_lpdPattern, unsigned int i_uiNum_Pattern_Elements, const double & i_dSpace) const
{
	fprintf(m_lpFileOut,"[");
	if (i_lpdPattern)
	{
		for (unsigned int uiI = 0; uiI < i_uiNum_Pattern_Elements; uiI++)
		{
			if (uiI != 0)
				fprintf(m_lpFileOut," ");
			fprintf(m_lpFileOut,m_lpszFormat,i_lpdPattern[uiI]);
		}
	}
	fprintf(m_lpFileOut,"] ");
	fprintf(m_lpFileOut,m_lpszFormat,i_dSpace);
	fprintf(m_lpFileOut," ");
	fprintf(m_lpFileOut,m_lpszSetDash);
}

void epsfile::Text_Bounding_Box(PS_FONT i_eFont, bool i_bItalic, bool i_bBold, int i_iFont_Size, PS_HORIZONTAL_JUSTIFICATION i_eHoirzontal_Justification, PS_VERTICAL_JUSTIFICATION i_eVertical_Justification, const color_triplet & i_cColor,const double & i_dX, const double & i_dY, const char * i_lpszText, const double & i_dRotation, const double & i_dLine_Width) const
{
	fprintf(m_lpFileOut,m_lpszGsave);
	fprintf(m_lpFileOut,m_lpszSetLineWidth,i_dLine_Width);
	switch (i_eFont)
	{
	case TIMES:
		if (i_bItalic && i_bBold)
			fprintf(m_lpFileOut,"/Times-BoldItalic findfont\n");
		else if (i_bBold)
			fprintf(m_lpFileOut,"/Times-Bold findfont\n");
		else if (i_bItalic)
			fprintf(m_lpFileOut,"/Times-Italic findfont\n");
		else
			fprintf(m_lpFileOut,"/Times-Roman findfont\n");
		break;
	case HELVETICA:
		if (i_bItalic && i_bBold)
			fprintf(m_lpFileOut,"/Helvetica-BoldItalic findfont\n");
		else if (i_bBold)
			fprintf(m_lpFileOut,"/Helvetica-Bold findfont\n");
		else if (i_bItalic)
			fprintf(m_lpFileOut,"/Helvetica-Italic findfont\n");
		else
			fprintf(m_lpFileOut,"/Helvetica-Roman findfont\n");
		break;
	case COURIER:
		if (i_bItalic && i_bBold)
			fprintf(m_lpFileOut,"/Courier-BoldItalic findfont\n");
		else if (i_bBold)
			fprintf(m_lpFileOut,"/Courier-Bold findfont\n");
		else if (i_bItalic)
			fprintf(m_lpFileOut,"/Courier-Italic findfont\n");
		else
			fprintf(m_lpFileOut,"/Courier-Roman findfont\n");
		break;
	case SYMBOL:
		fprintf(m_lpFileOut,"/Symbol findfont\n");
	}
	fprintf(m_lpFileOut,m_lpszScalefontSetFont,i_iFont_Size);
	fprintf(m_lpFileOut,m_lpszMoveto, i_dX,i_dY);
	if (i_dRotation != 0.0)
	{
		fprintf(m_lpFileOut,m_lpszFormat,i_dRotation);
		fprintf(m_lpFileOut," rotate\n");
	}
	fprintf(m_lpFileOut,m_lpszTxtCentered,i_lpszText);
	fprintf(m_lpFileOut,m_lpszSetRGBColor, i_cColor.m_dRed, i_cColor.m_dGreen, i_cColor.m_dBlue);
	fprintf(m_lpFileOut,"tbb rectfill\n");
	fprintf(m_lpFileOut,m_lpszGrestore);
}

std::vector< text_entity > epsfile::Parse_String(const std::string & i_szString) const
{
	std::vector <text_entity> veEntities;
	std::ostringstream ossCurr_Entity;

	unsigned int uiI = 0;
	while (uiI < i_szString.size())
	{
		while (uiI < i_szString.size() && i_szString[uiI] != '^' && i_szString[uiI] != '_' && i_szString[uiI] != '\\' && i_szString[uiI] != '{' && i_szString[uiI] != '}')
		{
			ossCurr_Entity << i_szString[uiI];
			uiI++;
		}
		if (i_szString[uiI] == '^')
		{
			if (ossCurr_Entity.tellp() > 0)
			{
				text_entity cEntity;

				cEntity.m_eType = text_entity::text;
				cEntity.m_szData = ossCurr_Entity.str();
				veEntities.push_back(cEntity);
				ossCurr_Entity.str("");
				ossCurr_Entity.clear();
			}
			text_entity cEntity;

			cEntity.m_eType = text_entity::superscript;
			veEntities.push_back(cEntity);
			uiI++;
		}
		else if (i_szString[uiI] == '_')
		{
			if (ossCurr_Entity.tellp() > 0)
			{
				text_entity cEntity;

				cEntity.m_eType = text_entity::text;
				cEntity.m_szData = ossCurr_Entity.str();
				veEntities.push_back(cEntity);
				ossCurr_Entity.str("");
				ossCurr_Entity.clear();
			}
			text_entity cEntity;

			cEntity.m_eType = text_entity::subscript;
			veEntities.push_back(cEntity);
			uiI++;
		}
		else if (i_szString[uiI] == '{')
		{
			if (ossCurr_Entity.tellp() > 0)
			{
				text_entity cEntity;

				cEntity.m_eType = text_entity::text;
				cEntity.m_szData = ossCurr_Entity.str();
				veEntities.push_back(cEntity);
				ossCurr_Entity.clear();
				ossCurr_Entity.seekp(0);
			}
			text_entity cEntity;

			cEntity.m_eType = text_entity::leftbrace;
			veEntities.push_back(cEntity);
			uiI++;
		}
		else if (i_szString[uiI] == '}')
		{
			if (ossCurr_Entity.tellp() > 0)
			{
				text_entity cEntity;

				cEntity.m_eType = text_entity::text;
				cEntity.m_szData = ossCurr_Entity.str();
				veEntities.push_back(cEntity);
				ossCurr_Entity.str("");
				ossCurr_Entity.clear();
			}
			text_entity cEntity;

			cEntity.m_eType = text_entity::rightbrace;
			veEntities.push_back(cEntity);
			uiI++;
		}
		else if (i_szString[uiI] == '\\')
		{
			uiI++;
			if (i_szString[uiI] == '^')
			{
				ossCurr_Entity << '^';
				uiI++;
			}
			else if (i_szString[uiI] == '_')
			{
				ossCurr_Entity << '_';
				uiI++;
			}
			else if (i_szString[uiI] == '{')
			{
				ossCurr_Entity << '{';
				uiI++;
			}
			else if (i_szString[uiI] == '}')
			{
				ossCurr_Entity << '}';
				uiI++;
			}
			else if (i_szString[uiI] == '\\')
			{
				ossCurr_Entity << '\\';
				uiI++;
			}
			else
			{
				if (ossCurr_Entity.tellp() > 0)
				{
					text_entity cEntity;

					cEntity.m_eType = text_entity::text;
					cEntity.m_szData = ossCurr_Entity.str();
					veEntities.push_back(cEntity);
					ossCurr_Entity.str("");
					ossCurr_Entity.clear();
				}
				while (uiI < i_szString.size() && ((i_szString[uiI] >= 'A' && i_szString[uiI] <= 'Z') || (i_szString[uiI] >= 'a' && i_szString[uiI] <= 'z')))
				{
					ossCurr_Entity << i_szString[uiI];
					uiI++;
				}
				text_entity cEntity;
				cEntity.m_eType = text_entity::symbol;
				cEntity.m_szData = ossCurr_Entity.str();
				veEntities.push_back(cEntity);
				ossCurr_Entity.str("");
				ossCurr_Entity.clear();
			}
		}
	}
	if (ossCurr_Entity.tellp() > 0)
	{
		text_entity cEntity;

		cEntity.m_eType = text_entity::text;
		cEntity.m_szData = ossCurr_Entity.str();
		veEntities.push_back(cEntity);
	}
	return veEntities;
}

//----------------------------------------------------------------------------
//
// data class members
//
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
// Line Plots
//
///////////////////////////////////////////////////////////////////////////////

unsigned int	data::Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, COLOR i_eColor, STIPPLE i_eStipple, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type, const double & i_dLine_Width)
{
	line_parameters	cLine_Param;

	cLine_Param.m_eColor = i_eColor;
	cLine_Param.m_dWidth = i_dLine_Width;
	cLine_Param.m_eStipple = i_eStipple;
	return Set_Plot_Data(i_lpdX_Values,i_lpdY_Values,i_uiNum_Points, cLine_Param, i_uiX_Axis_Type, i_uiY_Axis_Type);
}

unsigned int	data::Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)

{
	unsigned int uiRet = -1;
	line_item * lpLine_Data = new line_item;

	if (lpLine_Data)
	{
		lpLine_Data->m_uiNum_Points = i_uiNum_Points;
		lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;
		lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
		if (lpLine_Data->m_lppData)
			delete [] lpLine_Data->m_lppData;
		lpLine_Data->m_lppData = NULL;
		if (i_uiNum_Points > 0)
		{
			lpLine_Data->m_lppData = new eps_pair [i_uiNum_Points];
			for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
			{
				lpLine_Data->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
				lpLine_Data->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
			}
		}

		uiRet = m_vcPlot_Item_List.size();
		m_vcPlot_Item_List.push_back(lpLine_Data);
	}
	return uiRet;
}

unsigned int	data::Set_Plot_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)

{
	unsigned int uiRet = -1;
	line_item * lpLine_Data = new line_item;

	if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Set_Plot_Data called using std::vector, but X and Y vectors differ in size.\n");
	}
	if (lpLine_Data && i_vdX_Values.size() == i_vdY_Values.size() && i_vdX_Values.size() > 0)
	{
		unsigned int uiNum_Points = i_vdX_Values.size();
		lpLine_Data->m_uiNum_Points = uiNum_Points;
		lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;
		lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
		if (lpLine_Data->m_lppData)
			delete [] lpLine_Data->m_lppData;
		lpLine_Data->m_lppData = NULL;
		if (uiNum_Points > 0)
		{
			lpLine_Data->m_lppData = new eps_pair [uiNum_Points];
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				lpLine_Data->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
				lpLine_Data->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
			}
		}

		uiRet = m_vcPlot_Item_List.size();
		m_vcPlot_Item_List.push_back(lpLine_Data);
	}
	return uiRet;
}
unsigned int	data::Set_Plot_Data(const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)

{
	unsigned int uiRet = -1;
	line_item * lpLine_Data = new line_item;

	if (lpLine_Data && i_vpValues.size() > 0)
	{
		unsigned int uiNum_Points = i_vpValues.size();
		lpLine_Data->m_uiNum_Points = uiNum_Points;
		lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;
		lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
		if (lpLine_Data->m_lppData)
			delete [] lpLine_Data->m_lppData;
		lpLine_Data->m_lppData = NULL;
		if (uiNum_Points > 0)
		{
			lpLine_Data->m_lppData = new eps_pair [uiNum_Points];
			for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
			{
				lpLine_Data->m_lppData[uiI] = i_vpValues[uiI];
			}
		}

		uiRet = m_vcPlot_Item_List.size();
		m_vcPlot_Item_List.push_back(lpLine_Data);
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		line_item * lpLine_Data = (line_item *)m_vcPlot_Item_List[i_uiPlot_Data_ID];
		if (lpLine_Data)
		{
			uiRet = i_uiPlot_Data_ID;
			if (lpLine_Data->m_eType == TYPE_LINE)
			{
				uiRet = (uintptr_t) lpLine_Data;

				lpLine_Data->m_uiNum_Points = i_uiNum_Points;
				lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
				if (i_cLine_Parameters.m_dWidth >= 0.0)
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
				else
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
				if (lpLine_Data->m_lppData)
					delete [] lpLine_Data->m_lppData;
				lpLine_Data->m_lppData = NULL;
				if (i_uiNum_Points > 0)
				{
					lpLine_Data->m_lppData = new eps_pair [i_uiNum_Points];
					for (unsigned int uiI = 0; uiI < i_uiNum_Points; uiI++)
					{
						lpLine_Data->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
						lpLine_Data->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
					}
				}
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Modify_Plot_Data called using std::vector, but X and Y vectors differ in size.\n");
	}

	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		line_item * lpLine_Data = (line_item *)m_vcPlot_Item_List[i_uiPlot_Data_ID];
		if (lpLine_Data)
		{
			uiRet = i_uiPlot_Data_ID;
			if (lpLine_Data->m_eType == TYPE_LINE)
			{
				uiRet = (uintptr_t) lpLine_Data;

				unsigned int uiNum_Points = i_vdX_Values.size();

				lpLine_Data->m_uiNum_Points = uiNum_Points;
				lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
				if (i_cLine_Parameters.m_dWidth >= 0.0)
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
				else
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
				if (lpLine_Data->m_lppData)
					delete [] lpLine_Data->m_lppData;
				lpLine_Data->m_lppData = NULL;
				if (uiNum_Points > 0)
				{
					lpLine_Data->m_lppData = new eps_pair [uiNum_Points];
					for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
					{
						lpLine_Data->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
						lpLine_Data->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
					}
				}
			}
		}
	}
	return uiRet;
}
unsigned int	data::Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)

{
	unsigned int uiRet = -1;
	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		line_item * lpLine_Data = (line_item *)m_vcPlot_Item_List[i_uiPlot_Data_ID];
		if (lpLine_Data)
		{
			uiRet = i_uiPlot_Data_ID;
			if (lpLine_Data->m_eType == TYPE_LINE)
			{
				uiRet = (uintptr_t) lpLine_Data;

				unsigned int uiNum_Points = i_vpValues.size();

				lpLine_Data->m_uiNum_Points = uiNum_Points;
				lpLine_Data->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpLine_Data->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpLine_Data->m_cPlot_Line_Info = i_cLine_Parameters;
				if (i_cLine_Parameters.m_dWidth >= 0.0)
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
				else
					lpLine_Data->m_cPlot_Line_Info.m_dWidth = 1.0;
				if (lpLine_Data->m_lppData)
					delete [] lpLine_Data->m_lppData;
				lpLine_Data->m_lppData = NULL;
				if (uiNum_Points > 0)
				{
					lpLine_Data->m_lppData = new eps_pair [uiNum_Points];
					for (unsigned int uiI = 0; uiI < uiNum_Points; uiI++)
					{
						lpLine_Data->m_lppData[uiI] = i_vpValues[uiI];
					}
				}
			}
		}
	}
	return uiRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Errorbars
//
///////////////////////////////////////////////////////////////////////////////
unsigned int	data::Set_Errorbar_Data(const errorbar_parameters & i_cErrorbar_Parameters, const std::vector<double> &i_vdValues, const line_parameters & i_cLine_Parameters)
{
	unsigned int uiRet = -1;
	if (i_cErrorbar_Parameters.m_uiAssociated_Plot < m_vcPlot_Item_List.size() && i_vdValues.size() > 0)
	{
		plot_item * lpPlot = m_vcPlot_Item_List[i_cErrorbar_Parameters.m_uiAssociated_Plot];
		line_item * lpLine = (line_item *) lpPlot;
		symbol_item * lpSymbol = (symbol_item *) lpPlot;

		if (lpPlot && (lpPlot->m_eType == TYPE_LINE || lpPlot->m_eType == TYPE_SYMBOL))
		{
			errorbar_item * lpItem = new errorbar_item;
			if (lpItem)
			{
				lpItem->m_uiPlot_Axes_To_Use[0] = lpPlot->m_uiPlot_Axes_To_Use[0];
				lpItem->m_uiPlot_Axes_To_Use[1] = lpPlot->m_uiPlot_Axes_To_Use[1];
				lpItem->m_cErrorbar_Info = i_cErrorbar_Parameters;
				lpItem->m_uiNum_Points = i_vdValues.size();
				lpItem->m_cPlot_Line_Info = i_cLine_Parameters;
				lpItem->m_lppData = new double[lpItem->m_uiNum_Points];
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI] = i_vdValues[uiI];
				}
				uiRet = m_vcPlot_Item_List.size();
				m_vcPlot_Item_List.push_back(lpItem);
			}
		}
	}
	return uiRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Symbols
//
///////////////////////////////////////////////////////////////////////////////
unsigned int	data::Set_Symbol_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_lpdX_Values && i_lpdY_Values && i_uiNum_Points > 0)
	{
		symbol_item * lpItem = new symbol_item;

		if (lpItem)
		{
			lpItem->m_uiNum_Points = i_uiNum_Points;
			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
		
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			lpItem->m_lppData = new eps_pair [lpItem->m_uiNum_Points];
			for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
			{
				lpItem->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
				lpItem->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
			}

			uiRet = m_vcPlot_Item_List.size();
			m_vcPlot_Item_List.push_back(lpItem);
		}
	}
	return uiRet;
}
unsigned int	data::Set_Symbol_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vdX_Values.size() != i_vdY_Values.size())
	{
		fprintf(stderr,"epsplot::data Error! Set_Symbol_Data called using std::vector, but X and Y vectors differ in size.\n");
	}
	else if (i_vdX_Values.size() > 0)
	{
		symbol_item * lpItem = new symbol_item;
		if (lpItem)
		{
			lpItem->m_uiNum_Points = i_vdX_Values.size();
			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
		
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			lpItem->m_lppData = new eps_pair [lpItem->m_uiNum_Points];
			if (lpItem->m_lppData)
			{
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
					lpItem->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
				}

				uiRet = m_vcPlot_Item_List.size();
				m_vcPlot_Item_List.push_back(lpItem);
			}
		}
	}
	return uiRet;
}
unsigned int	data::Set_Symbol_Data(const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_vpValues.size() > 0)
	{
		symbol_item * lpItem = new symbol_item;
		if (lpItem)
		{
			lpItem->m_uiNum_Points = i_vpValues.size();
			lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
			lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
			lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
		
			if (i_cSymbol_Parameters.m_dSize >= 0.0)
				lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
			else
				lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
			lpItem->m_lppData = new eps_pair [lpItem->m_uiNum_Points];
			if (lpItem->m_lppData)
			{
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI].m_dX = i_vpValues[uiI].m_dX;
					lpItem->m_lppData[uiI].m_dY = i_vpValues[uiI].m_dY;
				}

				uiRet = m_vcPlot_Item_List.size();
				m_vcPlot_Item_List.push_back(lpItem);
				//printf("#pts %i\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_uiNum_Points);
				//printf("xaxis%i\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_uiPlot_Axes_To_Use[0]);
				//printf("yaxis %i\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_uiPlot_Axes_To_Use[1]);
				//printf("size %f\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_cPlot_Symbol_Info.m_dSize);
				//printf("fill %c\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_cPlot_Symbol_Info.m_bFilled ? 't' : 'f');
				//printf("color %i\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_cPlot_Symbol_Info.m_eColor);
				//printf("trype %i\n",((symbol_item *)m_vcPlot_Item_List[uiRet])->m_cPlot_Symbol_Info.m_eType);
			}
		}
	}
	return uiRet;
}

unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		uiRet = i_uiPlot_Data_ID;
		symbol_item * lpItem = (symbol_item *)m_vcPlot_Item_List[uiRet];
		if (lpItem)
		{
			if (i_lpdX_Values && i_lpdY_Values && i_uiNum_Points > 0)
			{

				lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
		
				if (i_cSymbol_Parameters.m_dSize >= 0.0)
					lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
				else
					lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
				if (lpItem->m_lppData && lpItem->m_uiNum_Points < i_uiNum_Points)
				{
					delete [] lpItem->m_lppData;
					lpItem->m_lppData = new eps_pair [i_uiNum_Points];
				}
				lpItem->m_uiNum_Points = i_uiNum_Points;
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI].m_dX = i_lpdX_Values[uiI];
					lpItem->m_lppData[uiI].m_dY = i_lpdY_Values[uiI];
				}
			}
			else
			{
				lpItem->m_uiNum_Points = 0;
				if (lpItem->m_lppData != nullptr)
					delete [] lpItem->m_lppData;
				lpItem->m_lppData = nullptr;
			}
		}
		else
			uiRet = -1;
	}
	return uiRet;
}
unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		uiRet = i_uiPlot_Data_ID;
		symbol_item * lpItem = (symbol_item *)m_vcPlot_Item_List[uiRet];
		if (lpItem)
		{
			if (i_vdX_Values.size() > 0 && i_vdY_Values.size() == i_vdX_Values.size())
			{
				lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
		
				if (i_cSymbol_Parameters.m_dSize >= 0.0)
					lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
				else
					lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
				if (lpItem->m_lppData && lpItem->m_uiNum_Points < i_vdX_Values.size())
				{
					delete [] lpItem->m_lppData;
					lpItem->m_lppData = new eps_pair [i_vdX_Values.size()];
				}
				lpItem->m_uiNum_Points = i_vdX_Values.size();
				for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
				{
					lpItem->m_lppData[uiI].m_dX = i_vdX_Values[uiI];
					lpItem->m_lppData[uiI].m_dY = i_vdY_Values[uiI];
				}
			}
			else
			{
				lpItem->m_uiNum_Points = 0;
				if (lpItem->m_lppData != nullptr)
					delete [] lpItem->m_lppData;
				lpItem->m_lppData = nullptr;
			}
		}
		else
			uiRet = -1;
	}
	return uiRet;
}
unsigned int	data::Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID)
{
	unsigned int uiRet = -1;
	if (i_uiPlot_Data_ID < m_vcPlot_Item_List.size())
	{
		uiRet = i_uiPlot_Data_ID;
		symbol_item * lpItem = (symbol_item *)m_vcPlot_Item_List[uiRet];
		if (lpItem)
		{
			if (i_vpValues.size() > 0)
			{
				lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_ID;
				lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_ID;
				lpItem->m_cPlot_Symbol_Info = i_cSymbol_Parameters;
	
				if (i_cSymbol_Parameters.m_dSize >= 0.0)
					lpItem->m_cPlot_Symbol_Info.m_dSize = i_cSymbol_Parameters.m_dSize;
				else
					lpItem->m_cPlot_Symbol_Info.m_dSize = 1.0;
				if (lpItem->m_lppData && lpItem->m_uiNum_Points < i_vpValues.size())
				{
					delete [] lpItem->m_lppData;
					lpItem->m_lppData = new eps_pair [i_vpValues.size()];
				}
				lpItem->m_uiNum_Points = i_vpValues.size();
				if (lpItem->m_lppData)
				{
					for (unsigned int uiI = 0; uiI < lpItem->m_uiNum_Points; uiI++)
					{
						lpItem->m_lppData[uiI].m_dX = i_vpValues[uiI].m_dX;
						lpItem->m_lppData[uiI].m_dY = i_vpValues[uiI].m_dY;
					}

					uiRet = m_vcPlot_Item_List.size();
					m_vcPlot_Item_List.push_back(lpItem);
				}
			}
			else
			{
				lpItem->m_uiNum_Points = 0;
				if (lpItem->m_lppData != nullptr)
					delete [] lpItem->m_lppData;
				lpItem->m_lppData = nullptr;
			}
		}
		else
			uiRet = -1;
	}
	return uiRet;
}
///////////////////////////////////////////////////////////////////////////////
//
// Rectangle
//
///////////////////////////////////////////////////////////////////////////////

unsigned int	data::Set_Rectangle_Data(const rectangle & i_cArea, bool i_bFill, COLOR i_eFill_Color, bool i_bBorder, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)
{
	unsigned int uiRet = -1;
	rectangle_item * lpItem = new rectangle_item;
	if (lpItem)
	{
		uiRet = m_vcPlot_Item_List.size();

		lpItem->m_cPlot_Rectangle_Info = i_cArea;
		lpItem->m_bArea_Fill = i_bFill;
		lpItem->m_ePlot_Area_Fill_Color = i_eFill_Color;
		lpItem->m_bDraw_Border = i_bBorder;
		lpItem->m_cPlot_Border_Info = i_cLine_Parameters;
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;

		if (i_cLine_Parameters.m_dWidth >= 0.0)
			lpItem->m_cPlot_Border_Info.m_dWidth = i_cLine_Parameters.m_dWidth;
		else
			lpItem->m_cPlot_Border_Info.m_dWidth = 1.0;

		m_vcPlot_Item_List.push_back(lpItem);
	}
	return uiRet;
}

unsigned int	data::Set_Text_Data(const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)
{
	unsigned int uiRet = -1;
	text_item * lpItem = new text_item;
	if (lpItem)
	{
		uiRet = m_vcPlot_Item_List.size();

		lpItem->Set_Text(i_lpszText);
		lpItem->m_cLine_Parameters = i_cLine_Parameters;
		lpItem->m_cText_Parameters = i_cText_Parameters;
		lpItem->m_dX = i_dX;
		lpItem->m_dY = i_dY;
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;

		m_vcPlot_Item_List.push_back(lpItem);
	}
	return uiRet;
}

unsigned int	data::Modify_Text_Data(unsigned int i_uiText_Data_ID, const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type)
{
	unsigned int uiRet = -1;
	plot_item * lpPlot_Item = NULL;
	text_item * lpItem = NULL;
	if (i_uiText_Data_ID < m_vcPlot_Item_List.size())
		lpPlot_Item = m_vcPlot_Item_List[i_uiText_Data_ID];
	if (lpPlot_Item && lpPlot_Item->m_eType == TYPE_TEXT)
		lpItem = (text_item *)lpPlot_Item;
	if (lpItem)
	{
		lpItem->Set_Text(i_lpszText);
		lpItem->m_cLine_Parameters = i_cLine_Parameters;
		lpItem->m_cText_Parameters = i_cText_Parameters;
		lpItem->m_dX = i_dX;
		lpItem->m_dY = i_dY;
		lpItem->m_uiPlot_Axes_To_Use[0] = i_uiX_Axis_Type;
		lpItem->m_uiPlot_Axes_To_Use[1] = i_uiY_Axis_Type;
	}
}

std::vector<axis_metadata> * data::Get_Axis_Metedata_Vector_Ptr(AXIS i_eAxis)
{
	std::vector<axis_metadata> * lpvcAxis_Data;
	switch (i_eAxis)
	{
	case X_AXIS:
		lpvcAxis_Data = &m_cX_Axis_Parameters;
		break;
	case Y_AXIS:
		lpvcAxis_Data = &m_cY_Axis_Parameters;
		break;
	default:
	case Z_AXIS:
		lpvcAxis_Data = NULL;
		break;
	}
	return lpvcAxis_Data;
}
unsigned int	data::Set_Axis_Parameters(AXIS i_eAxis, const axis_parameters & i_cAxis_Parameters)
{
	axis_metadata	cMeta_Data;
	std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr(i_eAxis);
	if(lpvcAxis_Data)
	{
		cMeta_Data.m_uiIdentifier = lpvcAxis_Data->size();
		cMeta_Data.m_cParameters = i_cAxis_Parameters;
		lpvcAxis_Data->push_back(cMeta_Data);
	}

	return cMeta_Data.m_uiIdentifier;
}

unsigned int	data::Set_X_Axis_Parameters(const axis_parameters & i_cAxis_Parameters)
{
	return Set_Axis_Parameters(X_AXIS,i_cAxis_Parameters);
}
unsigned int	data::Set_Y_Axis_Parameters(const axis_parameters & i_cAxis_Parameters)
{
	return Set_Axis_Parameters(Y_AXIS,i_cAxis_Parameters);
}
unsigned int	data::Set_X_Axis_Parameters(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax)
{
	axis_parameters	cAxis_Parameters(i_lpszAxis_Description, i_bLog_Axis, i_bInvert_Axis, i_bSet_Min, i_dMin, i_bSet_Max, i_dMax);
	return Set_Axis_Parameters(X_AXIS,cAxis_Parameters);
}
unsigned int	data::Set_Y_Axis_Parameters(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax)
{
	axis_parameters	cAxis_Parameters(i_lpszAxis_Description, i_bLog_Axis, i_bInvert_Axis, i_bSet_Min, i_dMin, i_bSet_Max, i_dMax);
	return Set_Axis_Parameters(Y_AXIS,cAxis_Parameters);
}
unsigned int	data::Modify_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	unsigned int uiRet = -1;
	std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr(i_eAxis);
	if (lpvcAxis_Data && i_uiWhich < lpvcAxis_Data->size())
	{
		lpvcAxis_Data[0][i_uiWhich].m_cParameters = i_cAxis_Parameters;
		uiRet = lpvcAxis_Data[0][i_uiWhich].m_uiIdentifier;
	}
	return uiRet;
}
unsigned int	data::Modify_X_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	Modify_Axis_Parameters(X_AXIS,i_uiWhich,i_cAxis_Parameters);
}
unsigned int	data::Modify_Y_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters)
{
	Modify_Axis_Parameters(Y_AXIS,i_uiWhich,i_cAxis_Parameters);
}
axis_parameters	data::Get_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich)
{
	axis_parameters	cRet;
	std::vector<axis_metadata> * lpvcAxis_Data = Get_Axis_Metedata_Vector_Ptr(i_eAxis);
	if (lpvcAxis_Data && i_uiWhich < lpvcAxis_Data->size())
	{
		cRet = lpvcAxis_Data[0][i_uiWhich].m_cParameters;
	}
	return cRet;
}

axis_parameters	data::Get_X_Axis_Parameters(unsigned int i_uiWhich)
{
	return Get_Axis_Parameters(X_AXIS, i_uiWhich);
}
axis_parameters	data::Get_Y_Axis_Parameters(unsigned int i_uiWhich)
{
	return Get_Axis_Parameters(Y_AXIS, i_uiWhich);
}


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


void data::Draw_Symbol(epsfile & io_cEPS, const double & i_dX, const double & i_dY, const symbol_parameters & i_cSymbol_Param)
{
	double dSize = i_cSymbol_Param.m_dSize;
	std::vector<eps_pair> *vSymbol;
	switch (i_cSymbol_Param.m_eType)
	{
	case SQUARE:
		vSymbol = &g_cSymbol_Containers.m_vSquare;
		break;
	case CIRCLE:
		vSymbol = &g_cSymbol_Containers.m_vCircle;
		break;
	case TRIANGLE_UP:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Up;
		break;
	case TRIANGLE_DOWN:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Down;
		break;
	case TRIANGLE_LEFT:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Left;
		break;
	case TRIANGLE_RIGHT:
		vSymbol = &g_cSymbol_Containers.m_vTriangle_Right;
		break;
	case DIAMOND:
		vSymbol = &g_cSymbol_Containers.m_vDiamond;
		break;
	case TIMES_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vTimes;
		break;
	case PLUS_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vPlus;
		break;
	case DASH_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vDash;
		break;
	case ASTERISK_SYMB:
		vSymbol = &g_cSymbol_Containers.m_vAsterisk;
		break;
	case STAR4:
		vSymbol = &g_cSymbol_Containers.m_vStar_4;
		break;
	case STAR5:
		vSymbol = &g_cSymbol_Containers.m_vStar_5;
		break;
	case STAR6:
		vSymbol = &g_cSymbol_Containers.m_vStar_6;
		break;
	case SYMB_CUSTOM_1:
	case SYMB_CUSTOM_2:
	case SYMB_CUSTOM_3:
	case SYMB_CUSTOM_4:
	case SYMB_CUSTOM_5:
	case SYMB_CUSTOM_6:
	case SYMB_CUSTOM_7:
	case SYMB_CUSTOM_8:
	case SYMB_CUSTOM_9:
	case SYMB_CUSTOM_10:
	case SYMB_CUSTOM_11:
	case SYMB_CUSTOM_12:
	case SYMB_CUSTOM_13:
	case SYMB_CUSTOM_14:
	case SYMB_CUSTOM_15:
	case SYMB_CUSTOM_16:
		{
			unsigned int uiIdx = i_cSymbol_Param.m_eType - SYMB_CUSTOM_1;
			vSymbol = &(g_cSymbol_Containers.m_vUser_Symbols[uiIdx]);
		}
		break;
	default:
		vSymbol = NULL;
		break;
	}
	if (vSymbol)
	{
			
		for (unsigned int uiI = 0; uiI < vSymbol->size(); uiI++)
		{
			if (uiI == 0)
				io_cEPS.Move_To(vSymbol[0][uiI].m_dX * dSize + i_dX,vSymbol[0][uiI].m_dY * dSize + i_dY);
			else
				io_cEPS.Line_To(vSymbol[0][uiI].m_dX * dSize + i_dX,vSymbol[0][uiI].m_dY * dSize + i_dY);
		}
		if (i_cSymbol_Param.m_bFilled)
			io_cEPS.Fill();
		else
		{
			io_cEPS.Line_To(vSymbol[0][0].m_dX * dSize + i_dX,vSymbol[0][0].m_dY * dSize + i_dY);
			io_cEPS.Stroke();
		}
	}
}

void	data::Plot(const page_parameters & i_cGrid)
{
	color_triplet	cBlue(0.0,0.0,1.0);
	char	lpszText[32];
	double ** lpdMinima_WL = NULL, ** lpdMinima_Flux = NULL;
	double ** lpdEW = NULL;
	double lpdDash_Style[2] = {2,2};
	double lpdLong_Short_Dash_Style[4] = {4,2,2,2};
	epsfile	cEPS("%.3f");// it seems this resolution is sufficient.  For higher resolution, use %.6f
	axis_metadata cX_Axis_Default;
	axis_metadata cY_Axis_Default;
	if (m_lpszFilename != NULL && m_lpszFilename[0] != 0)
	{
		cEPS.Open_File(m_lpszFilename, m_lpszTitle, i_cGrid.m_dWidth_Inches, i_cGrid.m_dHeight_Inches);

		double	dGraph_Space_X;
		double	dGraph_Space_Y;
		double	dRight_Margin = i_cGrid.m_dRight_Axis_Margin;
		double	dTop_Margin = i_cGrid.m_dTop_Axis_Margin;
		double	dTitle_Margin = i_cGrid.m_dTitle_Margin;
		if (m_cY_Axis_Parameters.size() > 1 && dRight_Margin <= 0.0)
			dRight_Margin = i_cGrid.m_dLeft_Axis_Margin;
		if (m_lpszTitle == NULL || m_lpszTitle[0] == 0)
			dTitle_Margin = 0.0;
		if (m_cX_Axis_Parameters.size() <= 1)
			dTop_Margin = 0.05; // give a little buffer
		if (m_cX_Axis_Parameters.size() > 1 && dTop_Margin <= 0.0)
			dTop_Margin = i_cGrid.m_dBottom_Axis_Margin;

		dGraph_Space_X = (i_cGrid.m_dWidth_Inches - 2.0 * i_cGrid.m_dSide_Unprintable_Margins_Inches)* 72.0 * (1.0 - i_cGrid.m_dLeft_Axis_Margin - dRight_Margin) - 2.0;
		dGraph_Space_Y = (i_cGrid.m_dHeight_Inches - 2.0 * i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0 * (1.0 - dTop_Margin - i_cGrid.m_dBottom_Axis_Margin - dTitle_Margin) - 2.0; // -2 to allow a little gap at the edge
		

		double	dGraph_Offset_X = (i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0;
		double	dGraph_Offset_Y = (i_cGrid.m_dHeight_Inches * i_cGrid.m_dBottom_Axis_Margin + i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0;
		// Don't do multi plot for now.. @@TODO
		//double	dSingle_Plot_Space_X = dGraph_Space_X / i_cGrid.m_uiNum_Columns;
		//double	dSingle_Plot_Space_Y = dGraph_Space_Y / i_cGrid.m_uiNum_Rows;

		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Reset_Limits();
		}
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Reset_Limits();
		}

		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			line_item * lpcLine = NULL;
			symbol_item * lpcSymbol = NULL;
			rectangle_item * lpcRectangle = NULL;
			text_item * lpcText = NULL;

//			double	dXminLcl = DBL_MAX, dXmaxLcl = -DBL_MAX;
			axis_metadata * lpX_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];

			switch (lpCurr->m_eType)
			{
			case TYPE_LINE:
				lpcLine = (line_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
				{
					lpX_Axis->Adjust_Limits(lpcLine->m_lppData[uiJ].m_dX);
				}
				break;
			case TYPE_SYMBOL:
				lpcSymbol = (symbol_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
				{
					lpX_Axis->Adjust_Limits(lpcSymbol->m_lppData[uiJ].m_dX);
				}
				break;
			case TYPE_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;
				lpX_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
				lpX_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
				break;
			case TYPE_TEXT:
//				lpcText = (text_item *) lpCurr;
				//Do not adjust axis limits to accomodate text.
				break;
			}
		}
		cX_Axis_Default.Finalize_Limit(dGraph_Space_X);
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Finalize_Limit(dGraph_Space_X);
		}

		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			line_item * lpcLine = NULL;
			symbol_item * lpcSymbol = NULL;
			rectangle_item * lpcRectangle = NULL;
			text_item * lpcText = NULL;

//			double	dYminLcl = DBL_MAX, dYmaxLcl = -DBL_MAX;
			axis_metadata * lpX_Axis;
			axis_metadata * lpY_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];
			if (m_cY_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cY_Axis_Parameters.size())
				lpY_Axis = &cY_Axis_Default;
			else
				lpY_Axis = &m_cY_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[1]];

			switch (lpCurr->m_eType)
			{
			case TYPE_LINE:
				lpcLine = (line_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
				{
					if (lpcLine->m_lppData[uiJ].m_dX >= lpX_Axis->m_dLower_Limit && lpcLine->m_lppData[uiJ].m_dX <= lpX_Axis->m_dUpper_Limit)
						lpY_Axis->Adjust_Limits(lpcLine->m_lppData[uiJ].m_dY);
				}
				break;
			case TYPE_SYMBOL:
				lpcSymbol = (symbol_item *) lpCurr;
				for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
				{
					if (lpcSymbol->m_lppData[uiJ].m_dX >= lpX_Axis->m_dLower_Limit && lpcSymbol->m_lppData[uiJ].m_dX <= lpX_Axis->m_dUpper_Limit)
						lpY_Axis->Adjust_Limits(lpcSymbol->m_lppData[uiJ].m_dY);
				}
				break;
			case TYPE_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;
					if ((lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min >= lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min <= lpX_Axis->m_dUpper_Limit) ||
(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max >= lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max <= lpX_Axis->m_dUpper_Limit) ||
(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max < lpX_Axis->m_dLower_Limit && lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max > lpX_Axis->m_dUpper_Limit))
				{
					lpY_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					lpY_Axis->Adjust_Limits(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
				}
				break;
			case TYPE_TEXT:
//				lpcText = (text_item *) lpCurr;
				//Do not adjust axis limits to accomodate text.
				break;
			}
		}
		cY_Axis_Default.Finalize_Limit(dGraph_Space_Y);

		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			(*cAxis_Iter).Finalize_Limit(dGraph_Space_Y);
		}

		COLOR eCurr_Color = BLACK;
		STIPPLE eCurr_Stipple = SOLID;
		double	dCurr_Line_Width = 1.0;
		// all scales and ranges worked out.. time to plot the data
		cEPS.State_Push();
		cEPS.Translate(dGraph_Offset_X,dGraph_Offset_Y);
		cEPS.Set_Line_Width(2.0);
		dCurr_Line_Width = 2.0;

		if (m_cX_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(0.0,0.0);
		cEPS.Line_To(dGraph_Space_X,0.0);
		cEPS.Stroke();

		if (m_cY_Axis_Parameters.size() > 1)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[1].m_cParameters.m_eColor));
		else if (m_cY_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(dGraph_Space_X,0.0);
		cEPS.Line_To(dGraph_Space_X,dGraph_Space_Y);
		cEPS.Stroke();

		if (m_cX_Axis_Parameters.size() > 1)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[1].m_cParameters.m_eColor));
		else if (m_cX_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cX_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(dGraph_Space_X,dGraph_Space_Y);
		cEPS.Line_To(0.0,dGraph_Space_Y);
		cEPS.Stroke();

		if (m_cY_Axis_Parameters.size() > 0)
			cEPS.Set_RGB_Color(Get_Color(m_cY_Axis_Parameters[0].m_cParameters.m_eColor));
		else
			cEPS.Set_RGB_Color(Get_Color(BLACK));
		cEPS.Move_To(0.0,dGraph_Space_Y);
		cEPS.Line_To(0.0,0.0);
		cEPS.Stroke();

		cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
		cEPS.Rect_Clip(0.0,0.0,dGraph_Space_X - 2.0,dGraph_Space_Y - 2.0);

		unsigned int uiI = 0;

		if (m_vcPlot_Item_List.size() == 0)
		{
			fprintf(stderr,"No data to graph\n");
			fflush(stderr);
		}
		for (std::vector<plot_item *>::iterator cPlot_Item_Iter = m_vcPlot_Item_List.begin(); cPlot_Item_Iter != m_vcPlot_Item_List.end(); cPlot_Item_Iter++)
		{
			plot_item * lpCurr = *cPlot_Item_Iter;
			uiI++;
			line_item * lpcLine = NULL;
			symbol_item * lpcSymbol = NULL;
			rectangle_item * lpcRectangle = NULL;
			text_item * lpcText = NULL;
			errorbar_item * lpcErrorbar = NULL;
			axis_metadata * lpX_Axis;
			axis_metadata * lpY_Axis;

			if (m_cX_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cX_Axis_Parameters.size())
				lpX_Axis = &cX_Axis_Default;
			else
				lpX_Axis = &m_cX_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[0]];
			if (m_cY_Axis_Parameters.size() == 0 || lpCurr->m_uiPlot_Axes_To_Use[0] >= m_cY_Axis_Parameters.size())
				lpY_Axis = &cY_Axis_Default;
			else
				lpY_Axis = &m_cY_Axis_Parameters[lpCurr->m_uiPlot_Axes_To_Use[1]];

			switch (lpCurr->m_eType)
			{
			case TYPE_LINE:
				lpcLine = (line_item *) lpCurr;
				if (lpcLine->m_uiNum_Points >= 2)
				{
					if (lpcLine->m_cPlot_Line_Info.m_eColor != eCurr_Color)
					{
						eCurr_Color = lpcLine->m_cPlot_Line_Info.m_eColor;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					if (lpcLine->m_cPlot_Line_Info.m_eStipple != eCurr_Stipple)
					{
						eCurr_Stipple = lpcLine->m_cPlot_Line_Info.m_eStipple;
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
					if (lpcLine->m_cPlot_Line_Info.m_dWidth != dCurr_Line_Width)
					{
						dCurr_Line_Width = lpcLine->m_cPlot_Line_Info.m_dWidth;
						cEPS.Set_Line_Width(dCurr_Line_Width);
					}

					{
						char lpszText[32];
						sprintf(lpszText,"Line %i\n",uiI);
						cEPS.Comment(lpszText);
						bool bFirst = true;
						for (unsigned int uiJ = 0; uiJ < lpcLine->m_uiNum_Points; uiJ++)
						{
							double dX = lpX_Axis->Scale(lpcLine->m_lppData[uiJ].m_dX);
							double dY = lpY_Axis->Scale(lpcLine->m_lppData[uiJ].m_dY);

							if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
							{
								if (bFirst)
									cEPS.Move_To(dX,dY);
								else
									cEPS.Line_To(dX,dY);
								bFirst = false;
							}
						}
						if (!bFirst) // don't draw if at least one valid point didn't show up
							cEPS.Stroke();
					}
				}
				break;
			case TYPE_rectangle:
				lpcRectangle = (rectangle_item *) lpCurr;

				if (lpcRectangle->m_bArea_Fill)
				{
					char lpszText[32];
					sprintf(lpszText,"Rect Fill %i\n",uiI);
					cEPS.Comment(lpszText);

					if (lpcRectangle->m_ePlot_Area_Fill_Color != eCurr_Color)
					{
						eCurr_Color = lpcRectangle->m_ePlot_Area_Fill_Color;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					double	dX, dY;

					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Move_To(dX,dY);
					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dX = lpX_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = lpY_Axis->Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					cEPS.Fill();
				}
				if (lpcRectangle->m_bDraw_Border)
				{
					sprintf(lpszText,"Rect Border %i\n",uiI);
					cEPS.Comment(lpszText);

					if (lpcRectangle->m_cPlot_Border_Info.m_eColor != eCurr_Color)
					{
						eCurr_Color = lpcRectangle->m_cPlot_Border_Info.m_eColor;
						cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
					}
					if (lpcRectangle->m_cPlot_Border_Info.m_eStipple != eCurr_Stipple)
					{
						eCurr_Stipple = lpcRectangle->m_cPlot_Border_Info.m_eStipple;
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
					if (lpcRectangle->m_cPlot_Border_Info.m_dWidth != dCurr_Line_Width)
					{
						dCurr_Line_Width = lpcRectangle->m_cPlot_Border_Info.m_dWidth;
						cEPS.Set_Line_Width(dCurr_Line_Width);
					}


					double dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					double dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Move_To(dX,dY);
					dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_max);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dX = m_cX_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[0]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dX_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					dY = m_cY_Axis_Parameters[lpcRectangle->m_uiPlot_Axes_To_Use[1]].Scale(lpcRectangle->m_cPlot_Rectangle_Info.m_dY_min);
					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						cEPS.Line_To(dX,dY);
					cEPS.Stroke();
				}
				break;
			case TYPE_TEXT:
				lpcText = (text_item *) lpCurr;
				if (lpcText->m_cLine_Parameters.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcText->m_cLine_Parameters.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				if (lpcText->m_cLine_Parameters.m_dWidth != dCurr_Line_Width)
				{
					dCurr_Line_Width = lpcText->m_cLine_Parameters.m_dWidth;
					cEPS.Set_Line_Width(dCurr_Line_Width);
				}

				{
					char lpszText[32];
					sprintf(lpszText,"Text %i (%s)\n",uiI,lpcText->Get_Text());
					cEPS.Comment(lpszText);
					double dX = lpX_Axis->Scale(lpcText->m_dX);
					double dY = lpY_Axis->Scale(lpcText->m_dY);

					if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
					{
						cEPS.Text(lpcText->m_cText_Parameters.m_eFont, lpcText->m_cText_Parameters.m_bItalic, lpcText->m_cText_Parameters.m_bBold, lpcText->m_cText_Parameters.m_iFont_Size, lpcText->m_cText_Parameters.m_eHorizontal_Justification, lpcText->m_cText_Parameters.m_eVertical_Justification, Get_Color(eCurr_Color),dX, dY, lpcText->Get_Text(), lpcText->m_cText_Parameters.m_dRotation, lpcText->m_cLine_Parameters.m_dWidth);
					}
				}
				break;
			case TYPE_SYMBOL:
				lpcSymbol = (symbol_item *) lpCurr;
				if (lpcSymbol->m_cPlot_Symbol_Info.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcSymbol->m_cPlot_Symbol_Info.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				{
					char lpszText[32];
					sprintf(lpszText,"Symbol %i\n",uiI);
					cEPS.Comment(lpszText);
					if (eCurr_Stipple != SOLID)
					{
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(SOLID,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}

					for (unsigned int uiJ = 0; uiJ < lpcSymbol->m_uiNum_Points; uiJ++)
					{
						double dX = lpX_Axis->Scale(lpcSymbol->m_lppData[uiJ].m_dX);
						double dY = lpY_Axis->Scale(lpcSymbol->m_lppData[uiJ].m_dY);

						if (!std::isnan(dX) && !std::isinf(dX) && !std::isnan(dY) && !std::isinf(dY))
						{
							Draw_Symbol(cEPS,dX,dY,lpcSymbol->m_cPlot_Symbol_Info);
						}
					}
					if (eCurr_Stipple != SOLID)
					{
						unsigned int uiStipple_Size;
						const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
						cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
					}
				}

				break;
			case TYPE_ERRORBAR:
				lpcErrorbar = (errorbar_item *) lpCurr;
				if (lpcErrorbar->m_cPlot_Line_Info.m_eColor != eCurr_Color)
				{
					eCurr_Color = lpcErrorbar->m_cPlot_Line_Info.m_eColor;
					cEPS.Set_RGB_Color(Get_Color(eCurr_Color));
				}
				if (lpcErrorbar->m_cPlot_Line_Info.m_dWidth != dCurr_Line_Width)
				{
					dCurr_Line_Width = lpcErrorbar->m_cPlot_Line_Info.m_dWidth;
					cEPS.Set_Line_Width(dCurr_Line_Width);
				}
				{
					
					char lpszText[32];
					sprintf(lpszText,"Errorbar %i (%i)\n",uiI,lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot);
					cEPS.Comment(lpszText);
					eps_pair * lpPair_List = NULL;
					unsigned int uiNum_Points = 0;
					if (m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot]->m_eType ==  TYPE_SYMBOL)
					{
						lpcSymbol = (symbol_item *)m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot];
						uiNum_Points = lpcSymbol->m_uiNum_Points;
						lpPair_List = lpcSymbol->m_lppData;
					}
					else if (m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot]->m_eType ==  TYPE_LINE)
					{
						lpcLine = (line_item *)m_vcPlot_Item_List[lpcErrorbar->m_cErrorbar_Info.m_uiAssociated_Plot];
						uiNum_Points = lpcLine->m_uiNum_Points;
						lpPair_List = lpcLine->m_lppData;
					}
					if (lpPair_List && uiNum_Points > 0)
					{
						for (unsigned int uiJ = 0; uiJ < uiNum_Points; uiJ++)
						{
							eps_pair p1(lpX_Axis->Scale(lpPair_List[uiJ].m_dX),lpY_Axis->Scale(lpPair_List[uiJ].m_dY));
							eps_pair p2,p3,p4;
							switch (lpcErrorbar->m_cErrorbar_Info.m_eDirection)
							{
							case ERRORBAR_X_LEFT:
								p2.m_dX = lpX_Axis->Scale(lpPair_List[uiJ].m_dX - lpcErrorbar->m_lppData[uiJ]);
								p2.m_dY = p1.m_dY;

								break;
							case ERRORBAR_X_RIGHT:
								p2.m_dX = lpX_Axis->Scale(lpPair_List[uiJ].m_dX + lpcErrorbar->m_lppData[uiJ]);
								p2.m_dY = p1.m_dY;

								break;
							case ERRORBAR_Y_UPPER:
								p2.m_dX = p1.m_dX;
								p2.m_dY = lpY_Axis->Scale(lpPair_List[uiJ].m_dY + lpcErrorbar->m_lppData[uiJ]);

								break;
							case ERRORBAR_Y_LOWER:
								p2.m_dX = p1.m_dX;
								p2.m_dY = lpY_Axis->Scale(lpPair_List[uiJ].m_dY - lpcErrorbar->m_lppData[uiJ]);

								break;
							}


							if (!std::isnan(p1.m_dX) && !std::isinf(p1.m_dX) && !std::isnan(p1.m_dY) && !std::isinf(p1.m_dY) &&
								!std::isnan(p2.m_dX) && !std::isinf(p2.m_dX) && !std::isnan(p2.m_dY) && !std::isinf(p2.m_dY) &&
								!std::isnan(p3.m_dX) && !std::isinf(p3.m_dX) && !std::isnan(p3.m_dY) && !std::isinf(p3.m_dY) &&
								!std::isnan(p4.m_dX) && !std::isinf(p4.m_dX) && !std::isnan(p4.m_dY) && !std::isinf(p4.m_dY))
							{
								if (lpcErrorbar->m_cPlot_Line_Info.m_eStipple != eCurr_Stipple)
								{
									eCurr_Stipple = lpcErrorbar->m_cPlot_Line_Info.m_eStipple;
									unsigned int uiStipple_Size;
									const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
									cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
								}
								cEPS.Move_To(p1.m_dX,p1.m_dY);
								cEPS.Line_To(p2.m_dX,p2.m_dY);
								cEPS.Stroke();
								// make the end-lines solid
								if (eCurr_Stipple != epsplot::SOLID)
								{
									eCurr_Stipple = epsplot::SOLID;
									unsigned int uiStipple_Size;
									const double * i_lpdStipple = Get_Stipple(eCurr_Stipple,uiStipple_Size);
									cEPS.Set_Dash(i_lpdStipple,uiStipple_Size,0.0);
								}
								switch (lpcErrorbar->m_cErrorbar_Info.m_eDirection)
								{
								case ERRORBAR_X_LEFT:
								case ERRORBAR_X_RIGHT:
									p3.m_dX = p2.m_dX;
									p3.m_dY = p2.m_dY - lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;

									p4.m_dX = p2.m_dX;
									p4.m_dY = p2.m_dY + lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									break;
								case ERRORBAR_Y_UPPER:
								case ERRORBAR_Y_LOWER:
									p3.m_dX = p2.m_dX - lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									p3.m_dY = p2.m_dY;

									p4.m_dX = p2.m_dX + lpcErrorbar->m_cErrorbar_Info.m_dTip_Width;
									p4.m_dY = p2.m_dY;
									break;
								}

								if (lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE ||
									lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE_AND_ARROW)
								{
									cEPS.Move_To(p3.m_dX,p3.m_dY);
									cEPS.Line_To(p4.m_dX,p4.m_dY);
									cEPS.Stroke();
								}
								if (lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_ARROW ||
									lpcErrorbar->m_cErrorbar_Info.m_eTip_Type == epsplot::ERRORBAR_TIP_LINE_AND_ARROW)
								{
									cEPS.Move_To(p2.m_dX,p2.m_dY);
									cEPS.Line_To(p4.m_dX,p4.m_dY);
									cEPS.Stroke();

									cEPS.Move_To(p2.m_dX,p2.m_dY);
									cEPS.Line_To(p3.m_dX,p3.m_dY);
									cEPS.Stroke();
								}
							}
						}
					}
				}
				break;				
				
			default:
				break;
			}
		}

		cEPS.State_Pop();

		if (m_lpszTitle)
		{
			cEPS.State_Push();
			cEPS.Comment("Title");
			double dY = dGraph_Space_Y + dGraph_Offset_Y + 40;
			double dX = i_cGrid.m_dWidth_Inches * 72.0  *0.5;
			cEPS.Text(TIMES,false,false,m_dTitle_Size,CENTER,BOTTOM,Get_Color(m_eTitle_Color),dX,dY, m_lpszTitle,0.0);
			cEPS.State_Pop();
		}

		cEPS.Comment("Axis titles");
		// write axis data
		cEPS.State_Push();
		uiI = 0;
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
		{
			double dX,dY;
			dX = (i_cGrid.m_dWidth_Inches * i_cGrid.m_dLeft_Axis_Margin + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0 + dGraph_Space_X * 0.5;
			if (uiI & 1)
				dY = ((dTop_Margin * 0.4 + i_cGrid.m_dBottom_Axis_Margin) * i_cGrid.m_dHeight_Inches + i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0 + dGraph_Space_Y;
			else			
				dY = ((0.10 * i_cGrid.m_dBottom_Axis_Margin) * i_cGrid.m_dHeight_Inches + i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0;

			cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dTitle_Size,CENTER,BOTTOM,Get_Color((*cAxis_Iter).m_cParameters.m_eTitle_Color),dX,dY, (*cAxis_Iter).m_cParameters.Get_Title());
			uiI++;
		}
		uiI = 0;
		for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
		{
			double	dX,dY;
			dY = (i_cGrid.m_dBottom_Axis_Margin * i_cGrid.m_dHeight_Inches + i_cGrid.m_dTop_Bottom_Unprintable_Margins_Inches) * 72.0 + dGraph_Space_Y * 0.5;
			if (uiI & 1)
				dX = ((i_cGrid.m_dLeft_Axis_Margin + dRight_Margin * 0.8) * i_cGrid.m_dWidth_Inches + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0 + dGraph_Space_X;
			else
				dX = (i_cGrid.m_dLeft_Axis_Margin * 0.4 * i_cGrid.m_dWidth_Inches + i_cGrid.m_dSide_Unprintable_Margins_Inches) * 72.0;
			cEPS.Text(TIMES,false,false,(*cAxis_Iter).m_cParameters.m_dTitle_Size,CENTER,BOTTOM,Get_Color((*cAxis_Iter).m_cParameters.m_eTitle_Color),dX,dY, (*cAxis_Iter).m_cParameters.Get_Title(),90.0);
			uiI++;
			
		}
		cEPS.State_Pop();
		cEPS.Comment("Axis labels and ticks");
		cEPS.State_Push();
		cEPS.Translate(dGraph_Offset_X,dGraph_Offset_Y);
		cEPS.Set_Line_Width(2.0);
		uiI = 0;
		if (m_cX_Axis_Parameters.size() == 0 && cX_Axis_Default.m_dRange != 0.0)
		{
			double dLogRange = log10(fabs(cX_Axis_Default.m_dRange));
			double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
			double	dMinor_Ticks = dMajor_Ticks * 0.1;
			unsigned int uiNum_Ticks = cX_Axis_Default.m_dRange / dMajor_Ticks;
			char lpszFormat[8];
			if (uiNum_Ticks > 10)
			{
				dMajor_Ticks *= 5.0; // e.g. units of 500
				dMinor_Ticks *= 10.0; // e.g. units of 100
			}
			else if (uiNum_Ticks > 5)
			{
				dMajor_Ticks *= 2.5; // e.g. units of 500
				dMinor_Ticks *= 5.0; // e.g. units of 100
			}
			else
			{
				dMajor_Ticks *= 1.0; // e.g. units of 500
				dMinor_Ticks *= 2.5; // e.g. units of 100
			}
			double	dLog_Major = floor(log10(dMajor_Ticks));
			bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
			if (dLog_Major < 0.0)
				sprintf(lpszFormat,"%%.%.0ff",-dLog_Major);
			else
				strcpy(lpszFormat,"%.0f");
			double	dStart = XRoundNearest(cX_Axis_Default.m_dStart,dMinor_Ticks);
			double	dDeltaTickMinor = ((uiI % 2) == 0) ? cX_Axis_Default.m_cParameters.m_dMinor_Tick_Length : -(cX_Axis_Default.m_cParameters.m_dMinor_Tick_Length);
			double	dDeltaTickMajor = ((uiI % 2) == 0) ? cX_Axis_Default.m_cParameters.m_dMajor_Tick_Length : -(cX_Axis_Default.m_cParameters.m_dMajor_Tick_Length);
			double	dDeltaText = ((uiI % 2) == 0) ? -(cX_Axis_Default.m_cParameters.m_dMinor_Tick_Length) : (cX_Axis_Default.m_cParameters.m_dMajor_Label_Size * 0.5);//8.0;
			for (double dVal = dStart; dVal <= cX_Axis_Default.m_dEnd; dVal += dMinor_Ticks)
			{
				if (dVal >= cX_Axis_Default.m_dLower_Limit && dVal <= cX_Axis_Default.m_dUpper_Limit) // just to make sure
				{
					double dX = cX_Axis_Default.Scale(dVal);
					double dY = (uiI % 2) * dGraph_Space_Y;
					bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
					cEPS.Set_Line_Width(bMajor_Test ? cX_Axis_Default.m_cParameters.m_dMajor_Tick_Width : cX_Axis_Default.m_cParameters.m_dMinor_Tick_Width);
					cEPS.Move_To(dX,dY);
					if (bMajor_Test)
						cEPS.Line_To(dX,dY + dDeltaTickMajor);
					else
						cEPS.Line_To(dX,dY + dDeltaTickMinor);
					cEPS.Stroke();
					double dSize;
					epsplot::COLOR eColor;
					if (bMajor_Test && cX_Axis_Default.m_cParameters.m_bLabel_Major_Indices)
					{
						dSize = cX_Axis_Default.m_cParameters.m_dMajor_Label_Size;
						eColor = cX_Axis_Default.m_cParameters.m_eMajor_Label_Color;
					}
					else if (!bMajor_Test && cX_Axis_Default.m_cParameters.m_bLabel_Minor_Indices)
					{
						dSize = cX_Axis_Default.m_cParameters.m_dMinor_Label_Size;
						eColor = cX_Axis_Default.m_cParameters.m_eMinor_Label_Color;
					}
					else
						dSize = 0.0;

					if (dSize > 0.0)
					{
						char lpszValue[16];
						if (bScientific_Notation)
						{
							double dLog10 = log10(dVal);
							double dPower = floor(dLog10);
							double	dMantissa = dVal * pow(10.0,-dPower);\
							if (dVal == 0.0)
							{
								sprintf(lpszValue,"%.1f",dVal);
							}
							else if (dMantissa != 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
							}
							else if (dMantissa == 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"10^{%.0f}",dPower);
							}
							else
								sprintf(lpszValue,"%.1f",dMantissa);
						}
						else
						{
							sprintf(lpszValue,lpszFormat,dVal);
						}
						cEPS.Text(TIMES,false,false,dSize,CENTER,MIDDLE,Get_Color(eColor),dX,dY + dDeltaText, lpszValue);
					}
				}
			}
		}
		else
		{
			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cX_Axis_Parameters.begin(); cAxis_Iter != m_cX_Axis_Parameters.end(); cAxis_Iter++)
			{
				// round minimum axis value to nearest major tick
				if (!(*cAxis_Iter).m_cParameters.m_bLog)
				{
					double dLogRange = log10(fabs((*cAxis_Iter).m_dRange));
					double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
					double	dMinor_Ticks = dMajor_Ticks * 0.1;
					unsigned int uiNum_Ticks = (*cAxis_Iter).m_dRange / dMajor_Ticks;
					char lpszFormat[8];
					if (uiNum_Ticks > 10)
					{
						dMajor_Ticks *= 5.0; // e.g. units of 500
						dMinor_Ticks *= 10.0; // e.g. units of 100
					}
					else if (uiNum_Ticks > 5)
					{
						dMajor_Ticks *= 2.5; // e.g. units of 500
						dMinor_Ticks *= 5.0; // e.g. units of 100
					}
					else
					{
						dMajor_Ticks *= 1.0; // e.g. units of 500
						dMinor_Ticks *= 2.5; // e.g. units of 100
					}
					double	dLog_Major = floor(log10(dMajor_Ticks));
					bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
					if (dLog_Major < 0.0)
						sprintf(lpszFormat,"%%.%.0ff",-dLog_Major);
					else if (dLog_Major > 3)
						strcpy(lpszFormat,"%.2e");
					else
						strcpy(lpszFormat,"%.0f");
					double	dStart = XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					double	dEnd = XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);//8.0;
					if ((*cAxis_Iter).m_cParameters.m_bInvert)
					{
						dStart =  XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
						dEnd =  XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					}
					for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
					{
						if (dVal >= (*cAxis_Iter).m_dLower_Limit && dVal <= (*cAxis_Iter).m_dUpper_Limit) // just to make sure
						{
							double dX = (*cAxis_Iter).Scale(dVal);
							double dY = (uiI % 2) * dGraph_Space_Y;
							bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
							cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
							cEPS.Move_To(dX,dY);
							if (bMajor_Test)
								cEPS.Line_To(dX,dY + dDeltaTickMajor);
							else
								cEPS.Line_To(dX,dY + dDeltaTickMinor);
							cEPS.Stroke();
							double dSize;
							epsplot::COLOR eColor;
							if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
							}
							else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
							}
							else
								dSize = 0.0;

							if (dSize > 0.0)
							{
								char lpszValue[16];
								if (bScientific_Notation)
								{
									double dLog10 = log10(dVal);
									double dPower = floor(dLog10);
									double	dMantissa = dVal * pow(10.0,-dPower);
									if (dVal == 0.0)
									{
										sprintf(lpszValue,"%.1f",dVal);
									}
									else if (dMantissa != 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
									}
									else if (dMantissa == 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"10^{%.0f}",dPower);
									}
									else
										sprintf(lpszValue,"%.1f",dMantissa);
			
								}
								else
								{
									sprintf(lpszValue,lpszFormat,dVal);
								}
								cEPS.Text(TIMES,false,false,dSize,CENTER,MIDDLE,Get_Color(eColor),dX,dY + dDeltaText, lpszValue);
							}
				
						}
					}

				}
				else
				{
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
					double	dRange_Lower = (*cAxis_Iter).m_dStart;
					double	dRange_Upper = (*cAxis_Iter).m_dEnd;
					if (!std::isnan(dRange_Lower) && !std::isnan(dRange_Upper))
					{
						double dLower = floor(dRange_Lower) - 1.0;
						double dUpper = floor(dRange_Upper) + 1.0;
						for (double dPower = dLower; dPower <= dUpper; dPower += 1.0)
						{
							double dMult = pow(10.0,dPower);
							for (unsigned int uiScalar = 1; uiScalar < 10; uiScalar ++)
							{
								double dVal = dMult * uiScalar;
								double dX = (*cAxis_Iter).Scale(dVal);
								double dY = (uiI % 2) * dGraph_Space_Y;
								bool bMajor_Test = (uiScalar == 1);
								if ((*cAxis_Iter).m_dLower_Limit <= dVal  && dVal <= (*cAxis_Iter).m_dUpper_Limit)
								{
									cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
									cEPS.Move_To(dX,dY);
									if (bMajor_Test)
										cEPS.Line_To(dX,dY + dDeltaTickMajor);
									else
										cEPS.Line_To(dX,dY + dDeltaTickMinor);
									cEPS.Stroke();
									double dSize;
									epsplot::COLOR eColor;
									if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
									}
									else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
									}
									else
										dSize = 0.0;

									if (dSize > 0.0)
									{
										char lpszValue[16];
										double dLog10 = log10(dVal);
										double dPower = floor(dLog10);
										double	dMantissa = dVal * pow(10.0,-dPower);
										if (dVal == 0.0)
										{
											sprintf(lpszValue,"%.1f",dVal);
										}
										else if (dPower != 0.0 && dMantissa != 1.0)
										{
											sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
										}
										else if (dPower == 0.0 || dMantissa != 1.0)
										{
											sprintf(lpszValue,"%.1f",dMantissa);
										}
										else
										{
											sprintf(lpszValue,"10^{%.0f}",dPower);
										}
										cEPS.Text(TIMES,false,false,dSize,RIGHT,MIDDLE,Get_Color(eColor),dX,dY + dDeltaText, lpszValue, 90.0);
									}
								}
							}
						}
					}
					else
						fprintf(stderr,"epsplot: x-axis is requested with log values, but range includes a negative number or zero (%f, %f).\n",dRange_Lower,dRange_Upper);
				}
				uiI++; // indicates axis at top or bottom of graph
			}
		}
		uiI = 0;
		if (m_cY_Axis_Parameters.size() == 0 && cY_Axis_Default.m_dRange != 0.0)
		{
			double dLogRange = log10(fabs(cY_Axis_Default.m_dRange));
			double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
			double	dMinor_Ticks = dMajor_Ticks * 0.1;
			unsigned int uiNum_Ticks = cY_Axis_Default.m_dRange / dMajor_Ticks;
			char lpszFormat[8];
			if (uiNum_Ticks > 10)
			{
				dMajor_Ticks *= 5.0; // e.g. units of 500
				dMinor_Ticks *= 10.0; // e.g. units of 100
			}
			else if (uiNum_Ticks > 5)
			{
				dMajor_Ticks *= 2.5; // e.g. units of 500
				dMinor_Ticks *= 5.0; // e.g. units of 100
			}
			else
			{
				dMajor_Ticks *= 1.0; // e.g. units of 500
				dMinor_Ticks *= 2.5; // e.g. units of 100
			}
			double	dLog_Major = floor(log10(dMajor_Ticks));
			bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
			if (dLog_Major < 0.0)
				sprintf(lpszFormat,"%%.%.0ff",-dLog_Major);
			else if (dLog_Major > 3)
				strcpy(lpszFormat,"%.2e");
			else
				strcpy(lpszFormat,"%.0f");
			double	dStart = XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			double	dEnd = XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
			double	dDeltaTickMinor = ((uiI % 2) == 0) ? cY_Axis_Default.m_cParameters.m_dMinor_Tick_Length : -(cY_Axis_Default.m_cParameters.m_dMinor_Tick_Length);
			double	dDeltaTickMajor = ((uiI % 2) == 0) ? cY_Axis_Default.m_cParameters.m_dMajor_Tick_Length : -(cY_Axis_Default.m_cParameters.m_dMajor_Tick_Length);
			double	dDeltaText = ((uiI % 2) == 0) ? -(cY_Axis_Default.m_cParameters.m_dMinor_Tick_Length) : 2.5;
			if (cY_Axis_Default.m_cParameters.m_bInvert)
			{
				dStart =  XRoundNearest(cY_Axis_Default.m_dEnd,dMinor_Ticks);
				dEnd =  XRoundNearest(cY_Axis_Default.m_dStart,dMinor_Ticks);
			}
			for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
			{
				if (dVal >= cY_Axis_Default.m_dLower_Limit && dVal <= cY_Axis_Default.m_dUpper_Limit) // just to make sure
				{
					double dY = cY_Axis_Default.Scale(dVal);
					double dX = (uiI % 2) * dGraph_Space_X;
					bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
					cEPS.Set_Line_Width(bMajor_Test ? cY_Axis_Default.m_cParameters.m_dMajor_Tick_Width : cY_Axis_Default.m_cParameters.m_dMinor_Tick_Width);
					cEPS.Move_To(dX,dY);
					if (bMajor_Test)
						cEPS.Line_To(dX + dDeltaTickMajor,dY);
					else
						cEPS.Line_To(dX + dDeltaTickMinor,dY);
					cEPS.Stroke();
					double dSize;
					epsplot::COLOR eColor;
					if (bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Major_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMajor_Label_Size;
						eColor = cY_Axis_Default.m_cParameters.m_eMajor_Label_Color;
					}
					else if (!bMajor_Test && cY_Axis_Default.m_cParameters.m_bLabel_Minor_Indices)
					{
						dSize = cY_Axis_Default.m_cParameters.m_dMinor_Label_Size;
						eColor = cY_Axis_Default.m_cParameters.m_eMinor_Label_Color;
					}
					else
						dSize = 0.0;

					if (dSize > 0.0)
					{
						char lpszValue[16];
						if (bScientific_Notation)
						{
							double dLog10 = log10(dVal);
							double dPower = floor(dLog10);
							double	dMantissa = dVal * pow(10.0,-dPower);
							if (dVal == 0.0)
							{
								sprintf(lpszValue,"%.1f",dVal);
							}
							else if (dMantissa != 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
							}
							else if (dMantissa == 1.0 && dPower != 0.0)
							{
								sprintf(lpszValue,"10^{%.0f}",dPower);
							}
							else
								sprintf(lpszValue,"%.1f",dMantissa);
						}
						else
							sprintf(lpszValue,lpszFormat,dVal);
						
						cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dX + dDeltaText, dY, lpszValue, 0.0);
					}
				}
			}
		}
		else
		{
			for (std::vector<axis_metadata>::iterator cAxis_Iter = m_cY_Axis_Parameters.begin(); cAxis_Iter != m_cY_Axis_Parameters.end(); cAxis_Iter++)
			{
				// round minimum axis value to nearest major tick
				if (!(*cAxis_Iter).m_cParameters.m_bLog)
				{
					double dLogRange = log10(fabs((*cAxis_Iter).m_dRange));
					double	dMajor_Ticks = pow(10.0,floor(dLogRange - 0.5)); // i.e. 1.0
					double	dMinor_Ticks = dMajor_Ticks * 0.1;
					unsigned int uiNum_Ticks = (*cAxis_Iter).m_dRange / dMajor_Ticks;
					char lpszFormat[8];
					if (uiNum_Ticks > 10)
					{
						dMajor_Ticks *= 5.0; // e.g. units of 500
						dMinor_Ticks *= 10.0; // e.g. units of 100
					}
					else if (uiNum_Ticks > 5)
					{
						dMajor_Ticks *= 2.5; // e.g. units of 500
						dMinor_Ticks *= 5.0; // e.g. units of 100
					}
					else
					{
						dMajor_Ticks *= 1.0; // e.g. units of 500
						dMinor_Ticks *= 2.5; // e.g. units of 100
					}
					double	dLog_Major = floor(log10(dMajor_Ticks));
					bool bScientific_Notation = (dLog_Major > 3 || dLog_Major < -3); 
					if (dLog_Major < 0.0)
						sprintf(lpszFormat,"%%.%.0ff",-dLog_Major);
					else if (dLog_Major > 3)
						strcpy(lpszFormat,"%.2e");
					else
						strcpy(lpszFormat,"%.0f");
					double	dStart = XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					double	dEnd = XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
//					printf("Y %f %f %f (%f %f)\n",dStart,dEnd,dMinor_Ticks,(*cAxis_Iter).m_dLower_Limit,(*cAxis_Iter).m_dUpper_Limit);
					if ((*cAxis_Iter).m_cParameters.m_bInvert)
					{
						dStart =  XRoundNearest((*cAxis_Iter).m_dEnd,dMinor_Ticks);
						dEnd =  XRoundNearest((*cAxis_Iter).m_dStart,dMinor_Ticks);
					}

					for (double dVal = dStart; dVal <= dEnd; dVal += dMinor_Ticks)
					{
						if (dVal >= (*cAxis_Iter).m_dLower_Limit && dVal <= (*cAxis_Iter).m_dUpper_Limit) // just to make sure
						{
							double dY = (*cAxis_Iter).Scale(dVal);
							double dX = (uiI % 2) * dGraph_Space_X;
							bool bMajor_Test = ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) < 0.05) || ((fmod(fabs(dVal),dMajor_Ticks) / dMajor_Ticks) > 0.95);
							cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
							cEPS.Move_To(dX,dY);
							if (bMajor_Test)
								cEPS.Line_To(dX + dDeltaTickMajor,dY);
							else
								cEPS.Line_To(dX + dDeltaTickMinor,dY);
							cEPS.Stroke();
//							printf("%f (%f %f) (%f %f)\n",dVal,dX,dY,dX + dDeltaTickMajor,dY);
							double dSize;
							epsplot::COLOR eColor;
							if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
							}
							else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
							{
								dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
								eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
							}
							else
								dSize = 0.0;

							if (dSize > 0.0)
							{
								char lpszValue[16];
								if (bScientific_Notation)
								{
									double dLog10 = log10(dVal);
									double dPower = floor(dLog10);
									double	dMantissa = dVal * pow(10.0,-dPower);
									if (dVal == 0.0)
									{
										sprintf(lpszValue,"%.1f",dVal);
									}
									else if (dMantissa != 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
									}
									else if (dMantissa == 1.0 && dPower != 0.0)
									{
										sprintf(lpszValue,"10^{%.0f}",dPower);
									}
									else
										sprintf(lpszValue,"%.1f",dMantissa);
								}
								else
									sprintf(lpszValue,lpszFormat,dVal);
					
								cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dX + dDeltaText, dY, lpszValue, 0.0);
							}
						}
					}

				}
				else
				{
					double	dDeltaTickMinor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length);
					double	dDeltaTickMajor = ((uiI % 2) == 0) ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length : -((*cAxis_Iter).m_cParameters.m_dMajor_Tick_Length);
					double	dDeltaText = ((uiI % 2) == 0) ? -((*cAxis_Iter).m_cParameters.m_dMinor_Tick_Length) : ((*cAxis_Iter).m_cParameters.m_dMajor_Label_Size * 0.5);
					double	dRange_Lower = (*cAxis_Iter).m_dStart;
					double	dRange_Upper = (*cAxis_Iter).m_dEnd;
					if (!std::isnan(dRange_Lower) && !std::isnan(dRange_Upper))
					{
						double dLower = floor(dRange_Lower) - 1.0;
						double dUpper = floor(dRange_Upper) + 1.0;
						for (double dPower = dLower; dPower <= dUpper; dPower += 1.0)
						{
							double dMult = pow(10.0,dPower);
							for (unsigned int uiScalar = 1; uiScalar < 10; uiScalar ++)
							{
								double dVal = dMult * uiScalar;
								double dY = (*cAxis_Iter).Scale(dVal);
								double dX = (uiI % 2) * dGraph_Space_X;
								bool bMajor_Test = (uiScalar == 1);
								if ((*cAxis_Iter).m_dLower_Limit <= dVal  && dVal <= (*cAxis_Iter).m_dUpper_Limit)
								{
									cEPS.Set_Line_Width(bMajor_Test ? (*cAxis_Iter).m_cParameters.m_dMajor_Tick_Width : (*cAxis_Iter).m_cParameters.m_dMinor_Tick_Width);
									cEPS.Move_To(dX,dY);
									if (bMajor_Test)
										cEPS.Line_To(dX + dDeltaTickMajor,dY);
									else
										cEPS.Line_To(dX + dDeltaTickMinor,dY);
									cEPS.Stroke();
		//							printf("%f (%f %f) (%f %f)\n",dVal,dX,dY,dX + dDeltaTickMajor,dY);
									double dSize;
									epsplot::COLOR eColor;
									if (bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Major_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMajor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMajor_Label_Color;
									}
									else if (!bMajor_Test && (*cAxis_Iter).m_cParameters.m_bLabel_Minor_Indices)
									{
										dSize = (*cAxis_Iter).m_cParameters.m_dMinor_Label_Size;
										eColor = (*cAxis_Iter).m_cParameters.m_eMinor_Label_Color;
									}
									else
										dSize = 0.0;
									if (dSize > 0.0)
									{
										char lpszValue[16];
										double dLog10 = log10(dVal);
										double dPower = floor(dLog10);
										double	dMantissa = dVal * pow(10.0,-dPower);
										if (dVal == 0.0)
										{
											sprintf(lpszValue,"%.1f",dVal);
										}
										else if (dMantissa != 1.0 && dPower != 0.0)
										{
											sprintf(lpszValue,"%.1fx10^{%.0f}",dMantissa,dPower);
										}
										else if (dMantissa == 1.0 && dPower != 0.0)
										{
											sprintf(lpszValue,"10^{%.0f}",dPower);
										}
										else
											sprintf(lpszValue,"%.1f",dMantissa);
					
										cEPS.Text(TIMES,false,false,dSize,(uiI % 2) == 0 ? RIGHT: LEFT,MIDDLE,Get_Color(eColor),dX + dDeltaText, dY, lpszValue, 0.0);
									}
								}
							}
						}
					}
					else
						fprintf(stderr,"epsplot: y-axis is requested with log values, but range includes a negative number or zero (%f, %f).\n",dRange_Lower,dRange_Upper);
				}
				uiI++; // indicates axis at top or bottom of graph
			}
		}
		cEPS.State_Pop();

		cEPS.Close_File();
	}
}

