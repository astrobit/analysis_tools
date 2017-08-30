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
void epsfile::ColorImage(const double & i_dX, const double & i_dY, const double & i_dWidth, const double & i_dHeight, int i_iColor_Depth, const std::vector<color_triplet> & i_vctImage_Data, size_t i_nRow_Length) const
{
	if (std::isfinite(i_dX) && std::isfinite(i_dY) && std::isfinite(i_dWidth) && std::isfinite(i_dHeight) &&
		(i_iColor_Depth == 1 || i_iColor_Depth == 2 || i_iColor_Depth == 4 || i_iColor_Depth == 8 || i_iColor_Depth == 12) &&
		i_vctImage_Data.size() != 0 && (i_vctImage_Data.size() % i_nRow_Length) == 0)
	{
		size_t tCols = i_vctImage_Data.size() / i_nRow_Length;
		fprintf(m_lpFileOut,m_lpszGsave);
		fprintf(m_lpFileOut,"0 0 moveto\n");
		fprintf(m_lpFileOut,m_lpszFormat, i_dWidth);
		fprintf(m_lpFileOut," ");
		fprintf(m_lpFileOut,m_lpszFormat, i_dHeight);
		fprintf(m_lpFileOut," scale\n");
		fprintf(m_lpFileOut,"0 -1 translate\n");
		fprintf(m_lpFileOut,"%i %i %i [%i 0 0 %i 0 -%i]\n", i_nRow_Length,tCols,i_iColor_Depth,i_nRow_Length,tCols,tCols);
		size_t tColor_Ref = 1;
		tColor_Ref <<= i_iColor_Depth;
		tColor_Ref -= 1; // 0... (2^n -1)
		char szFormat[12];
		size_t tFmt_Length = 3;
		size_t tLine_Size = 0;
		if (i_iColor_Depth == 12)
		{
			strcpy(szFormat,"%03x%03x%03x");
			tFmt_Length = 9;
		}
		else if (i_iColor_Depth == 8)
		{
			strcpy(szFormat,"%02x%02x%02x");
			tFmt_Length = 6;
		}
		else
			strcpy(szFormat,"%01x%01x%01x");
		fprintf(m_lpFileOut,"{<\n");
		size_t tCol = 0;
		for (auto iterI = i_vctImage_Data.cbegin(); iterI != i_vctImage_Data.cend(); iterI++)
		{
			tLine_Size += tFmt_Length;
			if (tLine_Size >= 255)
			{
				fprintf(m_lpFileOut,"\n");
				tLine_Size = 0;
			}
			unsigned short sRed = iterI->m_dRed * tColor_Ref;
			unsigned short sGreen = iterI->m_dGreen * tColor_Ref;
			unsigned short sBlue = iterI->m_dBlue * tColor_Ref;
			fprintf(m_lpFileOut,szFormat,sRed,sGreen,sBlue);
			tCol++;
			if (tCol == tCols)
			{
				fprintf(m_lpFileOut,"\n");
				tCol = 0;
				tLine_Size = 0;
			}
		}
		fprintf(m_lpFileOut,">}");
		fprintf(m_lpFileOut," false 3 colorimage\n"); // 3 indicates color triplet (RGB)
		fprintf(m_lpFileOut,m_lpszGrestore);
	}
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

