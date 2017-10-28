#include <eps_plot.h>
#include <xio.h>
#include <xstdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unordered_map>

struct EnumHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};


class SOURCE_FILE
{
private:
	char * 	m_lpszFilename;
public:
	XDATASET		m_xdDataset;
	void Set_File(const char * i_lpszFilename)
	{
		if(i_lpszFilename)
		{
			if (m_lpszFilename)
				delete m_lpszFilename;
			m_lpszFilename = new char[strlen(i_lpszFilename) + 1];
			strcpy(m_lpszFilename,i_lpszFilename);
		}
	}
	const char * Get_File(void)
	{
		return m_lpszFilename;
	}

	void Copy(const SOURCE_FILE & i_cRHO)
	{
		if (i_cRHO.m_lpszFilename)
		{
			Set_File(i_cRHO.m_lpszFilename);
			m_xdDataset = i_cRHO.m_xdDataset;
		}
	}
	SOURCE_FILE(void)
	{
		m_lpszFilename = NULL;
	}
	SOURCE_FILE(const SOURCE_FILE & i_cRHO)
	{
		m_lpszFilename = NULL;
		Copy(i_cRHO);
	}
	SOURCE_FILE & operator = (const SOURCE_FILE & i_cRHO)
	{
		Copy(i_cRHO);
	}
	~SOURCE_FILE(void)
	{
		if (m_lpszFilename)
			delete [] m_lpszFilename;
		m_lpszFilename = NULL;
	}
};

const char * Node_Get_PCDATA_Content(const xmlNode * i_lpNode)
{
	const char * lpszRet = NULL;
	xmlNode* lpText = i_lpNode->children;
	while (lpText && lpText->type != XML_TEXT_NODE)
		lpText = lpText->next;
	if (lpText && lpText->content)
		lpszRet = (char *)lpText->content;
	return lpszRet;
}
inline bool Test_Attr_Content(const xmlAttr * i_lpAttr)
{
	return (i_lpAttr && i_lpAttr->children && i_lpAttr->children->type == XML_TEXT_NODE);
}
const char * Attr_Get_String(const xmlAttr * i_lpAttr)
{
	const char * lpRet = NULL;
	if (Test_Attr_Content(i_lpAttr))
		lpRet = (const char *) i_lpAttr->children->content;
	return lpRet;
}
const char * Attr_Fill_String(const xmlAttr * i_lpAttr, const char * &io_lpszStr_Out)
{
	const char * lpRet = NULL;
	if (Test_Attr_Content(i_lpAttr))
		lpRet = (const char *)i_lpAttr->children->content;
	if (io_lpszStr_Out)
		delete [] io_lpszStr_Out;
	io_lpszStr_Out = new char[strlen(lpRet) + 1];
	strcpy((char *)io_lpszStr_Out,lpRet);
	return lpRet;
}
unsigned int Attr_Get_Uint(const xmlAttr * i_lpAttr, unsigned int i_uiDefault)
{
	unsigned int uiRet = i_uiDefault;
	if (Test_Attr_Content(i_lpAttr))
		uiRet = atoi((char *)i_lpAttr->children->content);
	return uiRet;
}
int Attr_Get_Int(const xmlAttr * i_lpAttr, int i_iDefault)
{
	int iRet = i_iDefault;
	if (Test_Attr_Content(i_lpAttr))
		iRet = atoi((char *)i_lpAttr->children->content);
	return iRet;
}
double Attr_Get_Double(const xmlAttr * i_lpAttr, double i_dDefault)
{
	double dRet = i_dDefault;
	if (Test_Attr_Content(i_lpAttr))
		dRet = atof((char *)i_lpAttr->children->content);
	return dRet;
}
bool Attr_Get_Bool(const xmlAttr * i_lpAttr, bool i_bDefault)
{
	bool bRet = i_bDefault;
	if (Test_Attr_Content(i_lpAttr))
		bRet = (strcmp((char *)i_lpAttr->children->content,"true") == 0);
	return bRet;
}
char Attr_Get_Char(const xmlAttr * i_lpAttr, unsigned int i_uiDefault)
{
	char chRet = i_uiDefault;
	if (Test_Attr_Content(i_lpAttr))
		chRet = i_lpAttr->children->content[0];
	return chRet;
}

std::unordered_map<epsplot::COLOR, std::string, EnumHash> g_cColor_Map;
std::string GetColorText(epsplot::COLOR i_eColor)
{
	std::string sRet;
	if (g_cColor_Map.size() == 0)
	{
		g_cColor_Map[epsplot::BLACK] = std::string("black");
		g_cColor_Map[epsplot::RED] = std::string("red");
		g_cColor_Map[epsplot::GREEN] = std::string("green");
		g_cColor_Map[epsplot::BLUE] = std::string("blue");
		g_cColor_Map[epsplot::CYAN] = std::string("cyan");
		g_cColor_Map[epsplot::MAGENTA] = std::string("magenta");
		g_cColor_Map[epsplot::YELLOW] = std::string("yellow");
		g_cColor_Map[epsplot::WHITE] = std::string("white");
		g_cColor_Map[epsplot::GRAY_25] = std::string("grey (25%)");
		g_cColor_Map[epsplot::GREY_25] = std::string("grey (25%)");
		g_cColor_Map[epsplot::GRAY_50] = std::string("grey (50%)");
		g_cColor_Map[epsplot::GREY_50] = std::string("grey (50%)");
		g_cColor_Map[epsplot::GRAY_75] = std::string("grey (75%)");
		g_cColor_Map[epsplot::GREY_75] = std::string("grey (75%)");
		for (epsplot::COLOR eColor = epsplot::CLR_CUSTOM_1; eColor < epsplot::CLR_CUSTOM_16; eColor = (epsplot::COLOR)(eColor + 1))
		{
			char lpszString[16];
			sprintf(lpszString,"user %i",eColor - epsplot::CLR_CUSTOM_1 + 1);
			g_cColor_Map[eColor] = std::string(lpszString);
		}
	}
	
	sRet = g_cColor_Map[i_eColor];
	return sRet;
}
void PrintColor(epsplot::COLOR i_eColor)
{
	printf("%s",GetColorText(i_eColor).c_str());
}
std::unordered_map<epsplot::STIPPLE, std::string, EnumHash> g_cStyle_Map;
std::string GetStyleText(epsplot::STIPPLE i_eStyle)
{
	std::string sRet;
	if (g_cStyle_Map.size() == 0)
	{
		g_cStyle_Map[epsplot::SOLID] = std::string("solid");
		g_cStyle_Map[epsplot::SHORT_DASH] = std::string("short dash");
		g_cStyle_Map[epsplot::LONG_DASH] = std::string("long dash");
		g_cStyle_Map[epsplot::LONG_SHORT_DASH] = std::string("long dash - short dash");
		g_cStyle_Map[epsplot::DOTTED] = std::string("dotted");
		g_cStyle_Map[epsplot::SHORT_DASH_DOTTED] = std::string("short dash - dotted");
		g_cStyle_Map[epsplot::LONG_DASH_DOTTED] = std::string("long dash - dotted");
		g_cStyle_Map[epsplot::LONG_SHORT_DASH_DOTTED] = std::string("long dash - short dash - dotted");
		g_cStyle_Map[epsplot::LONG_LONG_DASH] = std::string("long long dash");
		for (epsplot::STIPPLE eStyle = epsplot::STPL_CUSTOM_1; eStyle < epsplot::STPL_CUSTOM_16; eStyle = (epsplot::STIPPLE)(eStyle + 1))
		{
			char lpszString[16];
			sprintf(lpszString,"user %i",eStyle - epsplot::STPL_CUSTOM_1 + 1);
			g_cStyle_Map[eStyle] = std::string(lpszString);
		}
	}
	
	sRet = g_cStyle_Map[i_eStyle];
	return sRet;
}
std::unordered_map<epsplot::STIPPLE, std::string, EnumHash> g_cStyle_ASCII_Map;
std::string GetStyleASCII(epsplot::STIPPLE i_eStyle)
{
	std::string sRet;
	if (g_cStyle_ASCII_Map.size() == 0)
	{
		g_cStyle_ASCII_Map[epsplot::SOLID] = std::string("-----");
		g_cStyle_ASCII_Map[epsplot::SHORT_DASH] = std::string(" - - ");
		g_cStyle_ASCII_Map[epsplot::LONG_DASH] = std::string("-- --");
		g_cStyle_ASCII_Map[epsplot::LONG_SHORT_DASH] = std::string("-- - --");
		g_cStyle_ASCII_Map[epsplot::DOTTED] = std::string(". . .");
		g_cStyle_ASCII_Map[epsplot::SHORT_DASH_DOTTED] = std::string("- . -");
		g_cStyle_ASCII_Map[epsplot::LONG_DASH_DOTTED] = std::string("-- . --");
		g_cStyle_ASCII_Map[epsplot::LONG_SHORT_DASH_DOTTED] = std::string("-- - . -- -");
		g_cStyle_ASCII_Map[epsplot::LONG_LONG_DASH] = std::string("----   ----");
		for (epsplot::STIPPLE eStyle = epsplot::STPL_CUSTOM_1; eStyle < epsplot::STPL_CUSTOM_16; eStyle = (epsplot::STIPPLE)(eStyle + 1))
		{
			char lpszString[16];
			sprintf(lpszString,"user %i",eStyle - epsplot::STPL_CUSTOM_1 + 1);
			g_cStyle_ASCII_Map[eStyle] = std::string(lpszString);
		}
	}
	
	sRet = g_cStyle_ASCII_Map[i_eStyle];
	return sRet;
}
void PrintStyle(epsplot::STIPPLE i_eStipple)
{
	printf(", %s",GetStyleASCII(i_eStipple).c_str());
}
std::unordered_map<epsplot::SYMBOL_TYPE, std::string, EnumHash> g_cSymbol_Map;
std::string GetSymbolText(epsplot::SYMBOL_TYPE i_eStyle)
{
	std::string sRet;
	if (g_cSymbol_Map.size() == 0)
	{
		g_cSymbol_Map[epsplot::SQUARE] = std::string("square");
		g_cSymbol_Map[epsplot::CIRCLE] = std::string("circle");
		g_cSymbol_Map[epsplot::TRIANGLE_UP] = std::string("triangle (up)");
		g_cSymbol_Map[epsplot::TRIANGLE_DOWN] = std::string("triangle (down)");
		g_cSymbol_Map[epsplot::TRIANGLE_LEFT] = std::string("triangle (left)");
		g_cSymbol_Map[epsplot::TRIANGLE_RIGHT] = std::string("triangle (right)");
		g_cSymbol_Map[epsplot::DIAMOND] = std::string("diamond");
		g_cSymbol_Map[epsplot::TIMES_SYMB] = std::string("times");
		g_cSymbol_Map[epsplot::PLUS_SYMB] = std::string("plus");
		g_cSymbol_Map[epsplot::DASH_SYMB] = std::string("dash");
		g_cSymbol_Map[epsplot::ASTERISK_SYMB] = std::string("asterisk");
		g_cSymbol_Map[epsplot::STAR4] = std::string("4-point star");
		g_cSymbol_Map[epsplot::STAR5] = std::string("5-point star");
		g_cSymbol_Map[epsplot::STAR6] = std::string("6 point star");
		for (epsplot::SYMBOL_TYPE eStyle = epsplot::SYMB_CUSTOM_1; eStyle < epsplot::SYMB_CUSTOM_16; eStyle = (epsplot::SYMBOL_TYPE)(eStyle + 1))
		{
			char lpszString[16];
			sprintf(lpszString,"user %i",eStyle - epsplot::SYMB_CUSTOM_1 + 1);
			g_cSymbol_Map[eStyle] = std::string(lpszString);
		}
	}
	
	sRet = g_cSymbol_Map[i_eStyle];
	return sRet;
}
void PrintSymbolType(epsplot::SYMBOL_TYPE i_eType)
{
	printf(", %s",GetSymbolText(i_eType).c_str());
}
int strcmp(const xmlChar * i_lpszLHO, const char * i_lpszRHO)
{
	return strcmp((char *) i_lpszLHO,i_lpszRHO);
}

class CAPTION_INFO
{
public:
	char *				m_lpszCaption_Text;
	epsplot::line_parameters		m_cLine_Parameters;
	epsplot::symbol_parameters	m_cSymbol_Parameters;
	CAPTION_INFO(void)
	{
		m_lpszCaption_Text = NULL;
		m_cLine_Parameters.m_eColor = (epsplot::COLOR) -1;
		m_cSymbol_Parameters.m_eColor = (epsplot::COLOR) -1;
	}
};

void Output_Caption(FILE * o_fileOut, const std::vector<CAPTION_INFO> &i_vCaption_Info)
{
	bool bAll_Symbols_Same_Type = true;
	bool bAll_Symbols_Same_Fill_Type = true;
	bool bSymbol_Fill_Type = true;
	epsplot::SYMBOL_TYPE eSymbol_Type = (epsplot::SYMBOL_TYPE)-1;
	epsplot::STIPPLE eStipple = (epsplot::STIPPLE)-1;
	epsplot::COLOR eLine_Color = (epsplot::COLOR)-1;
	epsplot::COLOR eSymbol_Color = (epsplot::COLOR)-1;
	bool bAll_Lines_Same_Style = true;
	bool bAll_Lines_Same_Color = true;
	bool bAll_Symbols_Same_Color = true;
	for (std::vector<CAPTION_INFO>::const_iterator cIter = i_vCaption_Info.begin(); cIter != i_vCaption_Info.end(); cIter++)
	{
		if (eLine_Color == -1 && (*cIter).m_cLine_Parameters.m_eColor != -1)
		{
			eLine_Color = (*cIter).m_cLine_Parameters.m_eColor;
			eStipple = (*cIter).m_cLine_Parameters.m_eStipple;
		}
		if (eSymbol_Color == -1 && (*cIter).m_cSymbol_Parameters.m_eColor != -1)
		{
			eSymbol_Color = (*cIter).m_cSymbol_Parameters.m_eColor;
			eSymbol_Type = (*cIter).m_cSymbol_Parameters.m_eType;
			bSymbol_Fill_Type = (*cIter).m_cSymbol_Parameters.m_bFilled;
		}

		bAll_Symbols_Same_Fill_Type &= ((*cIter).m_cSymbol_Parameters.m_eColor == -1 || (*cIter).m_cSymbol_Parameters.m_bFilled == bSymbol_Fill_Type);
		bAll_Symbols_Same_Type &= ((*cIter).m_cSymbol_Parameters.m_eColor == -1 || (*cIter).m_cSymbol_Parameters.m_eType == eSymbol_Type);
		bAll_Symbols_Same_Color &= ((*cIter).m_cSymbol_Parameters.m_eColor == -1 || (*cIter).m_cSymbol_Parameters.m_eColor == eSymbol_Color);

		bAll_Lines_Same_Style &= ((*cIter).m_cLine_Parameters.m_eColor == -1 || (*cIter).m_cLine_Parameters.m_eStipple == eStipple);
		bAll_Lines_Same_Color &= ((*cIter).m_cLine_Parameters.m_eColor == -1 || (*cIter).m_cLine_Parameters.m_eColor == eLine_Color);
	}

	unsigned int uiCount = 0;
	for (std::vector<CAPTION_INFO>::const_iterator cIter = i_vCaption_Info.begin(); cIter != i_vCaption_Info.end(); cIter++)
	{
		if (uiCount != 0)
			fprintf(o_fileOut,", ");
		if ((*cIter).m_lpszCaption_Text != NULL)
			fprintf(o_fileOut,"%s (",(*cIter).m_lpszCaption_Text);
		else
			fprintf(o_fileOut,"Plot %i (",uiCount);

		if ((!bAll_Lines_Same_Color || !bAll_Lines_Same_Style) && ((*cIter).m_cLine_Parameters.m_eColor != -1))
		{
			if((*cIter).m_cSymbol_Parameters.m_eColor != -1)
				fprintf(o_fileOut,"Line: ");
			if (!bAll_Lines_Same_Color)
				fprintf(o_fileOut,"%s",GetColorText((*cIter).m_cLine_Parameters.m_eColor).c_str());
			if (!bAll_Lines_Same_Color && !bAll_Lines_Same_Style)
				fprintf(o_fileOut,", ");
			if (!bAll_Lines_Same_Style)
				fprintf(o_fileOut,"%s",GetStyleText((*cIter).m_cLine_Parameters.m_eStipple).c_str());
		}
		if ((*cIter).m_cSymbol_Parameters.m_eColor != -1 && (!bAll_Symbols_Same_Fill_Type || !bAll_Symbols_Same_Color || !bAll_Symbols_Same_Type))
		{
			if((*cIter).m_cLine_Parameters.m_eColor != -1)
				fprintf(o_fileOut,"; Symbol: ");

			if (!bAll_Symbols_Same_Type)
				fprintf(o_fileOut,"%s",GetSymbolText((*cIter).m_cSymbol_Parameters.m_eType).c_str());
			if (!bAll_Symbols_Same_Type && (!bAll_Symbols_Same_Color || !bAll_Symbols_Same_Fill_Type))
				fprintf(o_fileOut,", ");
			if (!bAll_Symbols_Same_Color)
				fprintf(o_fileOut,"%s",GetColorText((*cIter).m_cSymbol_Parameters.m_eColor).c_str());
			if (!bAll_Symbols_Same_Color && !bAll_Symbols_Same_Fill_Type)
				fprintf(o_fileOut,", ");
			if (!bAll_Symbols_Same_Fill_Type)
				fprintf(o_fileOut,", %s",(*cIter).m_cSymbol_Parameters.m_bFilled ? "filled" : "hollow");
		}
		fprintf(o_fileOut,")");
		uiCount++;
	}
}


void Parse_XML(xmlNode * i_lpRoot_Element)
{
	std::unordered_map<std::string, SOURCE_FILE> cSource_Files;
	std::unordered_map<std::string, unsigned int> cX_Axes;
	std::unordered_map<std::string, unsigned int> cY_Axes;
	epsplot::COLOR eUser_Color = epsplot::CLR_CUSTOM_1;
	std::unordered_map<std::string, epsplot::COLOR>	cColor_Map;
	std::unordered_map<std::string, epsplot::STIPPLE>	cStipple_Map;
	std::unordered_map<std::string, epsplot::SYMBOL_TYPE>	cSymbol_Map;
	std::vector <CAPTION_INFO> vCaption_Info;
	const char * lpszGraph_Title = NULL;
	const char * lpszOutput_Path = NULL;
	const char * lpszLayout = NULL;
	const char * lpszSize = NULL;
	const char * lpszWidth = NULL;
	const char * lpszHeight = NULL;
	const char * lpszCaptionfile_Path = NULL;
	const char * lpszLaTeX_Figure_File_Path = NULL;
	const char * lpszCaption_Prefix = NULL;
	const char * lpszCaption_Postfix = NULL;
	const char * lpszFigure_Label = NULL;
	epsplot::data cPlot;
	epsplot::page_parameters	cPlot_Parameters;
	epsplot::COLOR eDefault_Color = epsplot::BLACK;
	epsplot::STIPPLE eDefault_Stipple = epsplot::SOLID;
	epsplot::SYMBOL_TYPE eDefault_Symbol = epsplot::SQUARE;
	cColor_Map["black"] = epsplot::BLACK;
	cColor_Map["Black"] = epsplot::BLACK;
	cColor_Map["BLACK"] = epsplot::BLACK;
	cColor_Map["red"] = epsplot::RED;
	cColor_Map["Red"] = epsplot::RED;
	cColor_Map["RED"] = epsplot::RED;
	cColor_Map["green"] = epsplot::GREEN;
	cColor_Map["Green"] = epsplot::GREEN;
	cColor_Map["GREEN"] = epsplot::GREEN;
	cColor_Map["blue"] = epsplot::BLUE;
	cColor_Map["Blue"] = epsplot::BLUE;
	cColor_Map["BLUE"] = epsplot::BLUE;
	cColor_Map["cyan"] = epsplot::CYAN;
	cColor_Map["Cyan"] = epsplot::CYAN;
	cColor_Map["CYAN"] = epsplot::CYAN;
	cColor_Map["magenta"] = epsplot::MAGENTA;
	cColor_Map["Magenta"] = epsplot::MAGENTA;
	cColor_Map["MAGENTA"] = epsplot::MAGENTA;
	cColor_Map["yellow"] = epsplot::YELLOW;
	cColor_Map["Yellow"] = epsplot::YELLOW;
	cColor_Map["YELLOW"] = epsplot::YELLOW;
	cColor_Map["grey"] = epsplot::GREY_50;
	cColor_Map["Grey"] = epsplot::GREY_50;
	cColor_Map["GREY"] = epsplot::GREY_50;
	cColor_Map["gray"] = epsplot::GREY_50;
	cColor_Map["Gray"] = epsplot::GREY_50;
	cColor_Map["GRAY"] = epsplot::GREY_50;
	cColor_Map["grey25"] = epsplot::GREY_25;
	cColor_Map["Grey25"] = epsplot::GREY_25;
	cColor_Map["GREY25"] = epsplot::GREY_25;
	cColor_Map["gray25"] = epsplot::GREY_25;
	cColor_Map["Gray25"] = epsplot::GREY_25;
	cColor_Map["GRAY25"] = epsplot::GREY_25;
	cColor_Map["grey50"] = epsplot::GREY_50;
	cColor_Map["Grey50"] = epsplot::GREY_50;
	cColor_Map["GREY50"] = epsplot::GREY_50;
	cColor_Map["gray50"] = epsplot::GREY_50;
	cColor_Map["Gray50"] = epsplot::GREY_50;
	cColor_Map["GRAY50"] = epsplot::GREY_50;
	cColor_Map["grey75"] = epsplot::GREY_75;
	cColor_Map["Grey75"] = epsplot::GREY_75;
	cColor_Map["GREY75"] = epsplot::GREY_75;
	cColor_Map["gray75"] = epsplot::GREY_75;
	cColor_Map["Gray75"] = epsplot::GREY_75;
	cColor_Map["GRAY75"] = epsplot::GREY_75;
	cColor_Map["white"] = epsplot::WHITE;
	cColor_Map["White"] = epsplot::WHITE;
	cColor_Map["WHITE"] = epsplot::WHITE;

	cStipple_Map["solid"] = epsplot::SOLID;
	cStipple_Map["sdash"] = epsplot::SHORT_DASH;
	cStipple_Map["ldash"] = epsplot::LONG_DASH;
	cStipple_Map["lsdash"] = epsplot::LONG_SHORT_DASH;
	cStipple_Map["dot"] = epsplot::DOTTED;
	cStipple_Map["sdashdot"] = epsplot::SHORT_DASH_DOTTED;
	cStipple_Map["ldashdot"] = epsplot::LONG_DASH_DOTTED;
	cStipple_Map["lsdashdot"] = epsplot::LONG_SHORT_DASH_DOTTED;
	cStipple_Map["longlongdash"] = epsplot::LONG_LONG_DASH;
	

	cSymbol_Map["square"] = epsplot::SQUARE;
	cSymbol_Map["circle"] = epsplot::CIRCLE;
	cSymbol_Map["triup"] = epsplot::TRIANGLE_UP;
	cSymbol_Map["tridn"] = epsplot::TRIANGLE_DOWN;
	cSymbol_Map["trilt"] = epsplot::TRIANGLE_LEFT;
	cSymbol_Map["trirt"] = epsplot::TRIANGLE_RIGHT;
	cSymbol_Map["diamond"] = epsplot::DIAMOND;
	cSymbol_Map["times"] = epsplot::TIMES_SYMB;
	cSymbol_Map["plus"] = epsplot::PLUS_SYMB;
	cSymbol_Map["dash"] = epsplot::DASH_SYMB;
	cSymbol_Map["asterisk"] = epsplot::ASTERISK_SYMB;
	cSymbol_Map["star4"] = epsplot::STAR4;
	cSymbol_Map["star5"] = epsplot::STAR5;
	cSymbol_Map["star6"] = epsplot::STAR6;


	// this routine parses the XML tree describing the graph
	if (i_lpRoot_Element && i_lpRoot_Element->type == XML_ELEMENT_NODE && strcmp(i_lpRoot_Element->name,"GRAPH") == 0)
	{
		if (i_lpRoot_Element->properties)
		{
			xmlAttr * lpCurr_Attr = i_lpRoot_Element->properties;
			while (lpCurr_Attr)
			{
				if (strcmp(lpCurr_Attr->name,"outputfile") == 0)
				{
					lpszOutput_Path = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"captionfile") == 0)
				{
					lpszCaptionfile_Path = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"latexfigurefile") == 0)
				{
					lpszLaTeX_Figure_File_Path = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"title") == 0)
				{
					lpszGraph_Title = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"layout") == 0)
				{
					lpszLayout = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"size") == 0)
				{
					lpszSize = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"width") == 0)
				{
					lpszWidth = Attr_Get_String(lpCurr_Attr);
				}
				else if (strcmp(lpCurr_Attr->name,"height") == 0)
				{
					lpszHeight = Attr_Get_String(lpCurr_Attr);
				}
				lpCurr_Attr = lpCurr_Attr->next;
			}
			cPlot.Set_Plot_Filename(lpszOutput_Path);
			//@@TODO: title, layout, size, width, height
			cPlot_Parameters.m_uiNum_Columns = 1;
			cPlot_Parameters.m_uiNum_Rows = 1;
			cPlot_Parameters.m_dWidth_Inches = 11.0;
			cPlot_Parameters.m_dHeight_Inches = 8.5;
		}
		xmlNode * lpCurr_Node = i_lpRoot_Element->children;
		// first go through and identify all sourcefiles, colors, and axes
		while (lpCurr_Node)
		{
			switch (lpCurr_Node->type)
			{
			case XML_ELEMENT_NODE:
				if (strcmp(lpCurr_Node->name,"CAPTIONPREFIX") == 0)
					lpszCaption_Prefix = Node_Get_PCDATA_Content(lpCurr_Node);
				else if (strcmp(lpCurr_Node->name,"CAPTIONPOSTFIX") == 0)
					lpszCaption_Postfix = Node_Get_PCDATA_Content(lpCurr_Node);
				else if (strcmp(lpCurr_Node->name,"SOURCEFILE") == 0)
				{
					if (lpCurr_Node->properties)
					{
						xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
						SOURCE_FILE cFile;
						bool bWhitespace_Separated = false;
						unsigned int uiHeader_Lines = 0;
						bool	bHas_Strings = false;
						char chSeparator = ',';
						const char *  lpszID = NULL;
						while (lpCurr_Attr)
						{
							if (strcmp(lpCurr_Attr->name,"name") == 0)
							{
								cFile.Set_File(Attr_Get_String(lpCurr_Attr));
							}
							else if (strcmp(lpCurr_Attr->name,"fileid") == 0)
							{
								lpszID = Attr_Get_String(lpCurr_Attr);
							}
							else if (strcmp(lpCurr_Attr->name,"separator") == 0)
							{
								chSeparator = Attr_Get_Char(lpCurr_Attr,',');
							}
							else if (strcmp(lpCurr_Attr->name,"headerlines") == 0)
							{
								uiHeader_Lines = Attr_Get_Uint(lpCurr_Attr,1);
							}
							else if (strcmp(lpCurr_Attr->name,"wsseparated") == 0)
							{
								bWhitespace_Separated = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (strcmp(lpCurr_Attr->name,"hasstrings") == 0)
							{
								bHas_Strings = Attr_Get_Bool(lpCurr_Attr,false);
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
						if (cFile.Get_File() && lpszID != NULL)
						{
							cFile.m_xdDataset.ReadDataFile(cFile.Get_File(),bWhitespace_Separated,bHas_Strings,bWhitespace_Separated ? 0 : chSeparator, uiHeader_Lines);
							cSource_Files[std::string(lpszID)] = cFile;
						}
					}
				}
				else if (strcmp(lpCurr_Node->name,"XDATASET") == 0)
				{
					if (lpCurr_Node->properties)
					{
						xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
						SOURCE_FILE cFile;
						bool bWhitespace_Separated = false;
						const char *  lpszID = NULL;
						while (lpCurr_Attr)
						{
							if (strcmp(lpCurr_Attr->name,"name") == 0)
							{
								cFile.Set_File(Attr_Get_String(lpCurr_Attr));
							}
							else if (strcmp(lpCurr_Attr->name,"fileid") == 0)
							{
								lpszID = Attr_Get_String(lpCurr_Attr);
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
						if (cFile.Get_File() && lpszID != NULL)
						{
							cFile.m_xdDataset.ReadDataFileBin(cFile.Get_File());
							cSource_Files[std::string(lpszID)] = cFile;
						}
					}
				}
				else if (strcmp(lpCurr_Node->name,"COLOR") == 0)
				{
					if (lpCurr_Node->properties)
					{
						xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
						epsplot::color_triplet	cColor;
						const char * lpszID = NULL;
						while (lpCurr_Attr)
						{
							if (strcmp(lpCurr_Attr->name,"name") == 0)
							{
								lpszID = Attr_Get_String(lpCurr_Attr);
							}
							else if (strcmp(lpCurr_Attr->name,"red") == 0)
							{
								cColor.m_dRed = Attr_Get_Double(lpCurr_Attr,0.0);
								if (cColor.m_dRed > 1.0)
									cColor.m_dRed /= 255.0;
							}
							else if (strcmp(lpCurr_Attr->name,"green") == 0)
							{
								cColor.m_dGreen = Attr_Get_Double(lpCurr_Attr,0.0);
								if (cColor.m_dGreen > 1.0)
									cColor.m_dGreen /= 255.0;
							}
							else if (strcmp(lpCurr_Attr->name,"blue") == 0)
							{
								cColor.m_dBlue = Attr_Get_Double(lpCurr_Attr,0.0);
								if (cColor.m_dBlue > 1.0)
									cColor.m_dBlue /= 255.0;
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
						if (lpszID && eUser_Color <= epsplot::CLR_CUSTOM_16)
						{
							cColor_Map[std::string(lpszID)] = eUser_Color;
							cPlot.Define_Custom_Color(eUser_Color,cColor);
							eUser_Color = (epsplot::COLOR)(eUser_Color + 1);
						}
						else if (!lpszID)
						{
							fprintf(stderr,"genplot: Color ID not specified for color R%f G%f B%f.\n",cColor.m_dRed,cColor.m_dGreen,cColor.m_dBlue);
						}
						else
						{
							fprintf(stderr,"genplot: Too many user colors specified.  Dropping color %s.\n",lpszID);
						}
						
					}
				}
				else if (strcmp(lpCurr_Node->name,"AXIS") == 0)
				{
					if (lpCurr_Node->properties)
					{
						xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
						epsplot::axis_parameters	cAxis;
						const char * lpszID = NULL;
						char chDirection;
						while (lpCurr_Attr)
						{
							if (strcmp(lpCurr_Attr->name,"title") == 0)
							{
								cAxis.Set_Title(Attr_Get_String(lpCurr_Attr));
							}
							else if (strcmp(lpCurr_Attr->name,"axisid") == 0)
							{
								lpszID = Attr_Get_String(lpCurr_Attr);
							}
							else if (strcmp(lpCurr_Attr->name,"log") == 0)
							{
								cAxis.m_bLog = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (strcmp(lpCurr_Attr->name,"invert") == 0)
							{
								cAxis.m_bInvert = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (strcmp(lpCurr_Attr->name,"min") == 0)
							{
								cAxis.m_dLower_Limit = Attr_Get_Double(lpCurr_Attr,nan(""));
							}
							else if (strcmp(lpCurr_Attr->name,"max") == 0)
							{
								cAxis.m_dUpper_Limit = Attr_Get_Double(lpCurr_Attr,nan(""));
							}
							else if (strcmp(lpCurr_Attr->name,"direction") == 0)
							{
								chDirection = Attr_Get_Char(lpCurr_Attr,0);
							}
							else if (strcmp(lpCurr_Attr->name,"displaymajorindices") == 0)
							{
								cAxis.m_bLabel_Major_Indices = Attr_Get_Bool(lpCurr_Attr,false);
							}
							else if (strcmp(lpCurr_Attr->name,"displayminorindices") == 0)
							{
								cAxis.m_bLabel_Minor_Indices = Attr_Get_Bool(lpCurr_Attr,false);
							}
							lpCurr_Attr = lpCurr_Attr->next;
						}
						if(lpszID && chDirection != 0)
						{
							if (chDirection == 'x')
							{
								unsigned int uiX_Axis = cPlot.Set_X_Axis_Parameters( cAxis);
								cX_Axes[std::string(lpszID)] = uiX_Axis;
							}
							else // dir = y
							{
								unsigned int uiY_Axis = cPlot.Set_Y_Axis_Parameters( cAxis);
								cY_Axes[std::string(lpszID)] = uiY_Axis;
							}
						}
					}
				}
				break;
			}
			lpCurr_Node = lpCurr_Node->next;
		}

		// process plots
		lpCurr_Node = i_lpRoot_Element->children;
		while (lpCurr_Node)
		{
//			if (lpCurr_Node && lpCurr_Node->name)
//				printf("%s\n",lpCurr_Node->name);
			if (lpCurr_Node->type == XML_ELEMENT_NODE && strcmp(lpCurr_Node->name,"TEXT") == 0)
			{
				const char * lpszColor = NULL;
				const char * lpszFont = NULL;
				const char * lpszHoriz_Justification = NULL;
				const char * lpszVert_Justification = NULL;
				bool	bBold = false, bItalics = false;
				int	iSize = 18;
				double dWidth = 1.0;
				double	dAngle = 0.0;
				double dX;
				double dY;
				const char * lpszX_Axis_ID = NULL;
				const char * lpszY_Axis_ID = NULL;
				bool bFault = false;
				if (lpCurr_Node->properties)
				{
					xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
					while (lpCurr_Attr)
					{
						if (strcmp(lpCurr_Attr->name,"color") == 0)
						{
							lpszColor = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"width") == 0)
						{
							dWidth = Attr_Get_Double(lpCurr_Attr,2.0);
						}
						else if (strcmp(lpCurr_Attr->name,"angle") == 0)
						{
							dAngle = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"size") == 0)
						{
							iSize = Attr_Get_Int(lpCurr_Attr,18);
						}
						else if (strcmp(lpCurr_Attr->name,"x") == 0)
						{
							dX = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"y") == 0)
						{
							dY = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"xaxisid") == 0)
						{
							lpszX_Axis_ID = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"yaxisid") == 0)
						{
							lpszY_Axis_ID = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"bold") == 0)
						{
							bBold = Attr_Get_Bool(lpCurr_Attr,false);
						}
						else if (strcmp(lpCurr_Attr->name,"italics") == 0)
						{
							bItalics = Attr_Get_Bool(lpCurr_Attr,false);
						}
						else if (strcmp(lpCurr_Attr->name,"font") == 0)
						{
							lpszFont = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"horizontaljustification") == 0)
						{
							lpszHoriz_Justification = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"verticaljustification") == 0)
						{
							lpszVert_Justification = Attr_Get_String(lpCurr_Attr);
						}
						lpCurr_Attr = lpCurr_Attr->next;
					}
				}

				if (lpszX_Axis_ID && cX_Axes.count(std::string(lpszX_Axis_ID)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity x axis id %s.\n",lpszX_Axis_ID);
				}
				if (lpszY_Axis_ID && cY_Axes.count(std::string(lpszY_Axis_ID)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity y axis id %s.\n",lpszY_Axis_ID);
				}
				bool bDefault_Color = (!lpszColor || strcmp(lpszColor,"default") == 0);
				if (!bFault)
				{

					epsplot::text_parameters	cText_Paramters;
					epsplot::line_parameters 	cLine_Parameters;
					cLine_Parameters.m_dWidth = dWidth;
				
					if (bDefault_Color)
					{
						cLine_Parameters.m_eColor = eDefault_Color;
						eDefault_Color = (epsplot::COLOR)(eDefault_Color + 1);
						if (eDefault_Color > epsplot::MAGENTA)
							eDefault_Color = epsplot::BLACK;
					}
					else
						cLine_Parameters.m_eColor = cColor_Map[std::string(lpszColor)];
					if (!lpszFont || strcmp(lpszFont,"times") == 0)
						cText_Paramters.m_eFont = epsplot::TIMES;
					else if (strcmp(lpszFont,"helvetica") == 0)
						cText_Paramters.m_eFont = epsplot::HELVETICA;
					else if (strcmp(lpszFont,"courier") == 0)
						cText_Paramters.m_eFont = epsplot::COURIER;
					else if (strcmp(lpszFont,"symbol") == 0)
						cText_Paramters.m_eFont = epsplot::SYMBOL;

					cText_Paramters.m_bItalic = bItalics;
					cText_Paramters.m_bBold = bBold;
					cText_Paramters.m_iFont_Size = iSize;
					cText_Paramters.m_dRotation = dAngle;

					if (!lpszHoriz_Justification || strcmp(lpszHoriz_Justification,"left") == 0)
						cText_Paramters.m_eHorizontal_Justification = epsplot::LEFT;
					else if (strcmp(lpszHoriz_Justification,"center") == 0)
						cText_Paramters.m_eHorizontal_Justification = epsplot::CENTER;
					else if (strcmp(lpszHoriz_Justification,"right") == 0)
						cText_Paramters.m_eHorizontal_Justification = epsplot::RIGHT;

					if (!lpszVert_Justification || strcmp(lpszVert_Justification,"top") == 0)
						cText_Paramters.m_eVertical_Justification = epsplot::TOP;
					else if (strcmp(lpszVert_Justification,"middle") == 0)
						cText_Paramters.m_eVertical_Justification = epsplot::MIDDLE;
					else if (strcmp(lpszVert_Justification,"bottom") == 0)
						cText_Paramters.m_eVertical_Justification = epsplot::BOTTOM;

					unsigned int uiX_Axis = lpszX_Axis_ID == NULL ? (cX_Axes.size() == 1 ? (*cX_Axes.begin()).second : -1) : cX_Axes[std::string(lpszX_Axis_ID)];
					unsigned int uiY_Axis = lpszY_Axis_ID == NULL ? (cY_Axes.size() == 1 ? (*cY_Axes.begin()).second : -1) : cY_Axes[std::string(lpszY_Axis_ID)];
					const char * lpszText = Node_Get_PCDATA_Content(lpCurr_Node);
					if (lpszText)
					{
						printf("Text : ");
						PrintColor(cLine_Parameters.m_eColor);
						printf("\n");
						cPlot.Set_Text_Data( dX, dY, lpszText, cLine_Parameters, cText_Paramters, uiX_Axis, uiY_Axis);
					}
				}
			}
			else if (lpCurr_Node->type == XML_ELEMENT_NODE && strcmp(lpCurr_Node->name,"PLOT") == 0)
			{
				const char * lpszColor = NULL;
				const char * lpszStyle = NULL;
				const char * lpszSymbol = NULL;
				const char * lpszFilled = NULL;
				double dWidth = 2.0;
				double dX_Offset = 0.0;
				double dY_Offset = 0.0;
				double	dX_Multiplier = 1.0;
				double	dY_Multiplier = 1.0;
				const char * lpszX_Axis_ID = NULL;
				const char * lpszY_Axis_ID = NULL;
				const char * lpszCaption_Text = NULL;
				const char * lpszLegend_Text = NULL;
				double dSymbol_Size = 12.0;
				bool bFault = false;
				if (lpCurr_Node->properties)
				{
					xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
					while (lpCurr_Attr)
					{
						if (strcmp(lpCurr_Attr->name,"color") == 0)
						{
							lpszColor = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"style") == 0)
						{
							lpszStyle = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"width") == 0)
						{
							dWidth = Attr_Get_Double(lpCurr_Attr,2.0);
						}
						else if (strcmp(lpCurr_Attr->name,"symbol") == 0)
						{
							lpszSymbol = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"symbolsize") == 0)
						{
							dSymbol_Size = Attr_Get_Double(lpCurr_Attr,1.0);
						}
						else if (strcmp(lpCurr_Attr->name,"symbolfilled") == 0)
						{
							lpszFilled = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"xoffset") == 0)
						{
							dX_Offset = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"yoffset") == 0)
						{
							dY_Offset = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"xmult") == 0)
						{
							dX_Multiplier = Attr_Get_Double(lpCurr_Attr,1.0);
						}
						else if (strcmp(lpCurr_Attr->name,"ymult") == 0)
						{
							dY_Multiplier = Attr_Get_Double(lpCurr_Attr,1.0);
						}
						else if (strcmp(lpCurr_Attr->name,"xaxisid") == 0)
						{
							lpszX_Axis_ID = Attr_Get_String(lpCurr_Attr);
						}
						else if (strcmp(lpCurr_Attr->name,"yaxisid") == 0)
						{
							lpszY_Axis_ID = Attr_Get_String(lpCurr_Attr);
						}
						lpCurr_Attr = lpCurr_Attr->next;
					}
				}

				if (lpszX_Axis_ID && cX_Axes.count(std::string(lpszX_Axis_ID)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity x axis id %s.\n",lpszX_Axis_ID);
				}
				if (lpszY_Axis_ID && cY_Axes.count(std::string(lpszY_Axis_ID)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity y axis id %s.\n",lpszY_Axis_ID);
				}
				if (lpszStyle && strcmp(lpszStyle,"none") != 0 && strcmp(lpszStyle,"default") != 0 && cStipple_Map.count(std::string(lpszStyle)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity style %s.\n",lpszStyle);
				}
				if (lpszColor && strcmp(lpszColor,"default") != 0 && cColor_Map.count(std::string(lpszColor)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity color %s.\n",lpszColor);
				}
				if (lpszSymbol && strcmp(lpszSymbol,"none") != 0 && strcmp(lpszSymbol,"default") != 0 && cSymbol_Map.count(std::string(lpszSymbol)) == 0)
				{
					bFault = true;
					fprintf(stderr,"genplot: unable to idenfity symbol %s.\n",lpszSymbol);
				}
				bool bDefault_Symbol = (lpszSymbol && strcmp(lpszSymbol,"default") == 0);
				bool bDefault_Style = (!lpszStyle || strcmp(lpszStyle,"default") == 0);
				bool bDefault_Color = (!lpszColor || strcmp(lpszColor,"default") == 0);
				bool bFilled_Symbol = (!lpszFilled || strcmp(lpszFilled,"true") == 0);
				bool bNo_Symbol = (!lpszSymbol || strcmp(lpszSymbol,"none") == 0);
				bool bNo_Line = (lpszStyle && strcmp(lpszStyle,"none") == 0);
				if (!bFault)
				{

					epsplot::text_parameters	cText_Paramters;
					epsplot::line_parameters 	cLine_Parameters;
					epsplot::symbol_parameters 	cSymbol_Parameters;
					cLine_Parameters.m_dWidth = dWidth;
				
					if (bDefault_Color)
					{
						cLine_Parameters.m_eColor = eDefault_Color;
						eDefault_Color = (epsplot::COLOR)(eDefault_Color + 1);
						if (eDefault_Color > epsplot::MAGENTA)
							eDefault_Color = epsplot::BLACK;
					}
					else
						cLine_Parameters.m_eColor = cColor_Map[std::string(lpszColor)];

					if (bDefault_Style)
					{
						cLine_Parameters.m_eStipple = eDefault_Stipple;
						eDefault_Stipple = (epsplot::STIPPLE)(eDefault_Stipple + 1);
						if (eDefault_Stipple > epsplot::LONG_SHORT_DASH_DOTTED)
							eDefault_Stipple = epsplot::SOLID;
					}
					else
						cLine_Parameters.m_eStipple = cStipple_Map[std::string(lpszStyle)];

					if (bDefault_Symbol)
					{
						cSymbol_Parameters.m_eType = eDefault_Symbol;
						eDefault_Symbol = (epsplot::SYMBOL_TYPE)(eDefault_Symbol + 1);
						if (eDefault_Symbol > epsplot::STAR6)
							eDefault_Symbol = epsplot::SQUARE;
					}
					else if (!bNo_Symbol)
						cSymbol_Parameters.m_eType = cSymbol_Map[std::string(lpszSymbol)];
					cSymbol_Parameters.m_bFilled = bFilled_Symbol;

					unsigned int uiX_Axis = lpszX_Axis_ID == NULL ? (cX_Axes.size() == 1 ? (*cX_Axes.begin()).second : -1) : cX_Axes[std::string(lpszX_Axis_ID)];
					unsigned int uiY_Axis = lpszY_Axis_ID == NULL ? (cY_Axes.size() == 1 ? (*cY_Axes.begin()).second : -1) : cY_Axes[std::string(lpszY_Axis_ID)];
					cSymbol_Parameters.m_dSize = dSymbol_Size;
					cSymbol_Parameters.m_eColor = cLine_Parameters.m_eColor;
					std::vector < epsplot::eps_pair> cData;
					std::vector <double> cErrorbar_Data[4]; // 0 = x left, 1 = x right, 2 = y up, 3 = y down
					epsplot::errorbar_parameters cErrorbar_Parameters[4];
					epsplot::line_parameters 	cErrorbar_Line_Parameters[4];

					cData.clear();
					for (unsigned int uiI = 0; uiI < 4; uiI++)
					{
						cErrorbar_Data[uiI].clear();
						cErrorbar_Line_Parameters[uiI].m_eColor = (epsplot::COLOR) -1;
						cErrorbar_Line_Parameters[uiI].m_eStipple = epsplot::SOLID;
					}
					xmlNode * lpData_Node = lpCurr_Node->children;
					while (lpData_Node)
					{
						if (lpData_Node && lpData_Node->type == XML_ELEMENT_NODE)
						{
							if (strcmp(lpData_Node->name,"CAPTIONINFO") == 0)
							{
								lpszCaption_Text = Node_Get_PCDATA_Content(lpData_Node);
							}
							else if (strcmp(lpData_Node->name,"LEGENDINFO") == 0)
							{
								lpszLegend_Text = Node_Get_PCDATA_Content(lpData_Node);
							}
							else if (strcmp(lpData_Node->name,"PLOTFILE") == 0)
							{
								const char * lpszFile_ID = NULL;
								unsigned int uiX_Column=0;
								unsigned int uiY_Column=1;
								if (lpData_Node->properties)
								{
									xmlAttr * lpCurr_Attr = lpData_Node->properties;
									while (lpCurr_Attr)
									{
										if (strcmp(lpCurr_Attr->name,"fileid") == 0)
										{
											lpszFile_ID = Attr_Get_String(lpCurr_Attr);
										}
										else if (strcmp(lpCurr_Attr->name,"xcol") == 0)
										{
											uiX_Column = Attr_Get_Uint(lpCurr_Attr,0);
										}
										else if (strcmp(lpCurr_Attr->name,"ycol") == 0)
										{
											uiY_Column = Attr_Get_Uint(lpCurr_Attr,1);
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								if (lpszFile_ID && cSource_Files.count(std::string(lpszFile_ID)) != 0)
								{
									XDATASET &cSF_Data(cSource_Files[lpszFile_ID].m_xdDataset);
									printf("Parsing file %s for cols %i and %i.\n",cSource_Files[lpszFile_ID].Get_File(),uiX_Column,uiY_Column);
									if (cSF_Data.GetNumColumns() > uiY_Column && cSF_Data.GetNumColumns() > uiX_Column)
									{
										for (unsigned int uiI = 0; uiI < cSF_Data.GetNumRows(); uiI++)
										{
											if (!cSF_Data.IsElementEmpty(uiX_Column,uiI) && !cSF_Data.IsElementEmpty(uiY_Column,uiI))
											{
												epsplot::eps_pair cPair;
												cPair.m_dX = (cSF_Data.GetElement(uiX_Column,uiI) + dX_Offset) * dX_Multiplier;
												cPair.m_dY = (cSF_Data.GetElement(uiY_Column,uiI) + dY_Offset) * dY_Multiplier;
												cData.push_back(cPair);
											}
										}
									}
								}
								else if (lpszFile_ID)
								{
									fprintf(stderr,"genplot: unable to find file ID %s.\n",lpszFile_ID);
								}
								else
								{
									fprintf(stderr,"genplot: file ID not specified for plot.\n");
								}
							}
							else if (strcmp(lpData_Node->name,"PLOTDATA") == 0)
							{
								xmlNode * lpCurr_Tuple = lpData_Node->children;
								while (lpCurr_Tuple)
								{
									if (lpCurr_Tuple && lpCurr_Tuple->type == XML_ELEMENT_NODE && strcmp(lpCurr_Tuple->name,"TUPLE") == 0)
									{
										epsplot::eps_pair	cPair;
										xmlAttr * lpCurr_Attr = lpCurr_Tuple->properties;
										while (lpCurr_Attr)
										{
											if (strcmp(lpCurr_Attr->name,"x") == 0)
											{
												cPair.m_dX = (Attr_Get_Double(lpCurr_Attr,0.0) + dX_Offset) * dX_Multiplier;
											}
											else if (strcmp(lpCurr_Attr->name,"y") == 0)
											{
												cPair.m_dY = (Attr_Get_Double(lpCurr_Attr,0.0) + dY_Offset) * dY_Multiplier;
											}
											lpCurr_Attr = lpCurr_Attr->next;
										}
										cData.push_back(cPair);
									}
									lpCurr_Tuple = lpCurr_Tuple->next;
								}
							}
							else if (strcmp(lpData_Node->name,"ERRORBAR") == 0)
							{
								const char * lpszDirection = NULL, * lpszType = NULL, *lpszColor = NULL, *lpszStyle = NULL;
								double dSize = 4.0;
								double dWidth = 1.0;
								double dMultiplier;
								if (lpData_Node->properties)
								{
									xmlAttr * lpCurr_Attr = lpData_Node->properties;
									while (lpCurr_Attr)
									{
										if (strcmp(lpCurr_Attr->name,"direction") == 0)
										{
											lpszDirection = Attr_Get_String(lpCurr_Attr);
										}
										else if (strcmp(lpCurr_Attr->name,"type") == 0)
										{
											lpszType = Attr_Get_String(lpCurr_Attr);
										}
										else if (strcmp(lpCurr_Attr->name,"size") == 0)
										{
											dSize = Attr_Get_Double(lpCurr_Attr,4.0);
										}
										else if (strcmp(lpCurr_Attr->name,"width") == 0)
										{
											dWidth = Attr_Get_Double(lpCurr_Attr,1.0);
										}
										else if (strcmp(lpCurr_Attr->name,"color") == 0)
										{
											lpszColor = Attr_Get_String(lpCurr_Attr);
										}
										else if (strcmp(lpCurr_Attr->name,"style") == 0)
										{
											lpszStyle = Attr_Get_String(lpCurr_Attr);
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
								}
								unsigned int uiEB_Idx = 0;
								if (lpszDirection && strcmp(lpszDirection,"xleft") == 0)
									uiEB_Idx = 0;
								else if (lpszDirection && strcmp(lpszDirection,"xright") == 0)
									uiEB_Idx = 1;
								else if (lpszDirection && strcmp(lpszDirection,"yup") == 0)
									uiEB_Idx = 2;
								else if (lpszDirection && strcmp(lpszDirection,"ydown") == 0)
									uiEB_Idx = 3;
								if (lpszColor && strcmp(lpszColor,"default") != 0)
									cErrorbar_Line_Parameters[uiEB_Idx].m_eColor = cColor_Map[std::string(lpszColor)];
								cErrorbar_Line_Parameters[uiEB_Idx].m_dWidth = dWidth;
								if (lpszStyle)
								{
									printf("style %s\n",lpszStyle);
									cErrorbar_Line_Parameters[uiEB_Idx].m_eStipple = cStipple_Map[std::string(lpszStyle)];
								}
								if (uiEB_Idx == 0 || uiEB_Idx == 1)
									dMultiplier = dX_Multiplier;
								else
									dMultiplier = dY_Multiplier;


								cErrorbar_Parameters[uiEB_Idx].m_eDirection = (epsplot::ERRORBAR_DIRECTION)(uiEB_Idx + epsplot::ERRORBAR_X_LEFT);
								cErrorbar_Parameters[uiEB_Idx].m_dTip_Width = dSize;
								if (!lpszType || strcmp(lpszType,"line") == 0)
									cErrorbar_Parameters[uiEB_Idx].m_eTip_Type = epsplot::ERRORBAR_TIP_LINE;
								else if (strcmp(lpszType,"arrow") == 0)
									cErrorbar_Parameters[uiEB_Idx].m_eTip_Type = epsplot::ERRORBAR_TIP_ARROW;
								else if (strcmp(lpszType,"linearrow") == 0)
									cErrorbar_Parameters[uiEB_Idx].m_eTip_Type = epsplot::ERRORBAR_TIP_LINE_AND_ARROW;
								xmlNode * lpEB_Data_Node = lpData_Node->children;
								while (lpEB_Data_Node)
								{
									if (lpEB_Data_Node && lpEB_Data_Node->type == XML_ELEMENT_NODE)
									{
										const char * lpszFile_ID = NULL;
										unsigned int uiCol_ID;
										if (strcmp(lpEB_Data_Node->name,"ERRORBARFILE") == 0)
										{
											xmlAttr * lpCurr_Attr = lpEB_Data_Node->properties;
											while (lpCurr_Attr)
											{
												if (strcmp(lpCurr_Attr->name,"fileid") == 0)
												{
													lpszFile_ID = Attr_Get_String(lpCurr_Attr);
												}
												else if (strcmp(lpCurr_Attr->name,"col") == 0)
												{
													uiCol_ID = Attr_Get_Uint(lpCurr_Attr,0);
												}
												lpCurr_Attr = lpCurr_Attr->next;
											}
											if (lpszFile_ID && cSource_Files.count(std::string(lpszFile_ID)) != 0)
											{
												XDATASET &cSF_Data(cSource_Files[lpszFile_ID].m_xdDataset);
												printf("Parsing file %s for col %i.\n",cSource_Files[lpszFile_ID].Get_File(),uiCol_ID);
												if (cSF_Data.GetNumColumns() > uiCol_ID)
												{
													for (unsigned int uiI = 0; uiI < cSF_Data.GetNumRows(); uiI++)
													{
														if (!cSF_Data.IsElementEmpty(uiCol_ID,uiI))
														{
															cErrorbar_Data[uiEB_Idx].push_back(cSF_Data.GetElement(uiCol_ID,uiI)*dMultiplier);
														}
													}
												}
											}
											else if (lpszFile_ID)
											{
												fprintf(stderr,"genplot: unable to find file ID %s.\n",lpszFile_ID);
											}
											else
											{
												fprintf(stderr,"genplot: file ID not specified for plot.\n");
											}
										}
										else if (strcmp(lpEB_Data_Node->name,"ERRORBARDATA") == 0)
										{
											xmlNode * lpPair_Node = lpEB_Data_Node->children;
											while (lpPair_Node && lpPair_Node->type != XML_ELEMENT_NODE)
												lpPair_Node = lpPair_Node->next;

											if (lpPair_Node && lpPair_Node->type == XML_ELEMENT_NODE && strcmp(lpPair_Node->name,"VALUE") == 0)
											{
												xmlNode * lpCurr_Tuple = lpPair_Node;
												while (lpCurr_Tuple)
												{
													const char * lpszValue = Node_Get_PCDATA_Content(lpCurr_Tuple);
													if (lpszValue)
													{
														while (lpszValue[0] == 10 || lpszValue[0] == 13 || lpszValue[0] == ' ' || lpszValue[0] == '\t')
															lpszValue++;
														cErrorbar_Data[uiEB_Idx].push_back(atof(lpszValue)*dMultiplier);
													}
													lpCurr_Tuple = lpCurr_Tuple->next;
												}
											}
										}
									}
									lpEB_Data_Node = lpEB_Data_Node->next;
								}
								

							}
						}
						lpData_Node = lpData_Node->next;
					}
					if (cData.size() > 0)
					{
						CAPTION_INFO	cCaption_Data;
						if (!bNo_Symbol)
						{
							printf("Symbol : ");
							PrintColor(cSymbol_Parameters.m_eColor);
							printf(" ");
							PrintSymbolType(cSymbol_Parameters.m_eType);
							printf("\n");
							unsigned int uiPlot = cPlot.Set_Symbol_Data(cData,cSymbol_Parameters,uiX_Axis,uiY_Axis);
							for (unsigned int uiI = 0; uiI < 4; uiI++)
							{
								if (cErrorbar_Line_Parameters[uiI].m_eColor == (epsplot::COLOR) -1)
								{
									cErrorbar_Line_Parameters[uiI].m_eColor = cSymbol_Parameters.m_eColor;
								}
								cErrorbar_Parameters[uiI].m_uiAssociated_Plot = uiPlot;
								if (cErrorbar_Data[uiI].size() > 0)
									cPlot.Set_Errorbar_Data(cErrorbar_Parameters[uiI], cErrorbar_Data[uiI], cErrorbar_Line_Parameters[uiI]);
							}
							cCaption_Data.m_cSymbol_Parameters = cSymbol_Parameters;
						}
						if (!bNo_Line)
						{
							printf("Plot : ");
							PrintColor(cLine_Parameters.m_eColor);
							printf(" ");
							PrintStyle(cLine_Parameters.m_eStipple);
							printf("\n");
							unsigned int uiPlot = cPlot.Set_Plot_Data(cData,cLine_Parameters,uiX_Axis,uiY_Axis);
							if (bNo_Symbol)
							{
								for (unsigned int uiI = 0; uiI < 4; uiI++)
								{
									if (cErrorbar_Line_Parameters[uiI].m_eColor == (epsplot::COLOR) -1)
									{
										cErrorbar_Line_Parameters[uiI].m_eColor = cSymbol_Parameters.m_eColor;
									}
									cErrorbar_Parameters[uiI].m_uiAssociated_Plot = uiPlot;
									if (cErrorbar_Data[uiI].size() > 0)
									{
										cPlot.Set_Errorbar_Data(cErrorbar_Parameters[uiI], cErrorbar_Data[uiI], cErrorbar_Line_Parameters[uiI]);
									}
								}
							}
							cCaption_Data.m_cLine_Parameters = cLine_Parameters;
						}
						cCaption_Data.m_lpszCaption_Text = (char *)lpszCaption_Text;
						vCaption_Info.push_back(cCaption_Data);

					}
				}
			}
			lpCurr_Node = lpCurr_Node->next;
		}




		cPlot.Plot(cPlot_Parameters);
		printf("Plot output to %s\n",lpszOutput_Path);
		if (lpszLaTeX_Figure_File_Path)
		{
			FILE * fileFigure = fopen(lpszLaTeX_Figure_File_Path,"wt");
			if (fileFigure)
			{
				fprintf(fileFigure,"\\begin{figure}[H]\n\\centering\n\\includegraphics[height=\\textwidth,angle=270]{%s}\n\\caption{\\label{%s}",
lpszOutput_Path,lpszOutput_Path);
				if (lpszCaption_Prefix)
					fprintf(fileFigure,lpszCaption_Prefix);
				if (vCaption_Info.size() > 0)
					Output_Caption(fileFigure,vCaption_Info);
				if (lpszCaption_Postfix)
					fprintf(fileFigure,lpszCaption_Postfix);
				fprintf(fileFigure,"}\n\\end{figure*}\n");
				fclose(fileFigure);
				printf("LaTeX figure ouput to %s.\n",lpszLaTeX_Figure_File_Path);
			}
		}

		if (lpszCaptionfile_Path && vCaption_Info.size() > 0)
		{
			FILE * fileCaption = fopen(lpszCaptionfile_Path,"wt");
			if (fileCaption)
			{
				if (lpszCaption_Prefix)
					fprintf(fileCaption,lpszCaption_Prefix);
				Output_Caption(fileCaption,vCaption_Info);
				if (lpszCaption_Postfix)
					fprintf(fileCaption,lpszCaption_Postfix);
				fprintf(fileCaption,".\n");
				fclose(fileCaption);
				printf("Caption information ouput to %s.\n",lpszCaptionfile_Path);
			}
		}

	}
	else
	{
		fprintf(stderr,"genplot: attempt to parse graph data failed; this is not a valid graph.\n");
	}
}

int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{
	const char * lpszX_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-title");
	const char * lpszY_Axis_Title = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-title");
	const char * lpszDatafile = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--file");
	const char * lpszColumnList = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-columns");
	unsigned int uiX_Axis_Column = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-column",-1);
	const char * lpszX_Columns = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-columns");
	unsigned int uiHeader_Lines  = xParse_Command_Line_UInt(i_iArg_Count,(const char **)i_lpszArg_Values,"--header-lines",0);
	bool	bWhitespace_Separated = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--whitespace-separated");
	bool	bX_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-axis-log");
	bool	bY_Axis_Log = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-axis-log");
	double	dX_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-min",nan(""));
	double	dX_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--x-max",nan(""));
	double	dY_min = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-min",nan(""));
	double	dY_max = xParse_Command_Line_Dbl(i_iArg_Count,(const char **)i_lpszArg_Values,"--y-max",nan(""));
	bool	bCycle_Line_Style = xParse_Command_Line_Exists(i_iArg_Count,(const char **)i_lpszArg_Values,"--cycle-line-style");
	char lpszOutput_File[256];
	xParse_Command_Line_String(i_iArg_Count,(const char **)i_lpszArg_Values,"--output",lpszOutput_File,sizeof(lpszOutput_File),"");
	const char * lpszPlotFile = xParse_Command_Line_Data_Ptr(i_iArg_Count,(const char **)i_lpszArg_Values,"--plotfile");

	if (lpszPlotFile)
	{
		xmlDocPtr doc = xmlReadFile(lpszPlotFile, NULL, XML_PARSE_DTDVALID);

		if (doc == NULL)
		{
		    fprintf(stderr, "Failed to parse %s\n", lpszPlotFile);
			return 1;
		}

		/*Get the root element node */
		xmlNodePtr root_node = xmlDocGetRootElement(doc);
		Parse_XML(root_node);
	}
	else if (lpszDatafile)
	{
		char lpszValue[32];
		bool bUse_X_Axis = false, bUse_Y_Axis = false;

		xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
		xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "GRAPH");
		const char * lpszDTD_Path = getenv("LINE_ANALYSIS_DATA_PATH");
		if (lpszDTD_Path)
		{
			char *lpszDTD_Full_Path = new char[strlen(lpszDTD_Path) + 16];
			sprintf(lpszDTD_Full_Path,"%s/genplots.dtd",lpszDTD_Path);
			xmlDtdPtr dtd = xmlCreateIntSubset(doc, BAD_CAST "GRAPH", NULL, BAD_CAST lpszDTD_Full_Path);
			delete [] lpszDTD_Full_Path;
		}
		xmlDocSetRootElement(doc, root_node);
		if (lpszOutput_File && lpszOutput_File[0] != 0)
			xmlNewProp(root_node,BAD_CAST "outputfile", BAD_CAST lpszOutput_File);
		else
		{
			char lpszOutfile[256];
			sprintf(lpszOutfile,"%s.eps",lpszDatafile);
			xmlNewProp(root_node,BAD_CAST "outputfile", BAD_CAST lpszOutfile);
		}
		if (bX_Axis_Log || !std::isnan(dX_min) || !std::isnan(dX_max) || lpszX_Axis_Title)
		{
			bUse_X_Axis = true;
			xmlNodePtr axisnode = xmlNewChild(root_node, NULL, BAD_CAST "AXIS",NULL);
			xmlNewProp(axisnode,BAD_CAST "axisid", BAD_CAST "xaxis1");
			xmlNewProp(axisnode,BAD_CAST "direction", BAD_CAST "x");
			if (bX_Axis_Log)
				xmlNewProp(axisnode,BAD_CAST "log", BAD_CAST "true");
			if (!std::isnan(dX_min))
			{
				sprintf(lpszValue,"%.17e",dX_min);
				xmlNewProp(axisnode,BAD_CAST "min", BAD_CAST lpszValue);
			}
			if (!std::isnan(dX_max))
			{
				sprintf(lpszValue,"%.17e",dX_max);
				xmlNewProp(axisnode,BAD_CAST "max", BAD_CAST lpszValue);
			}
			if (lpszX_Axis_Title)
				xmlNewProp(axisnode,BAD_CAST "title", BAD_CAST lpszX_Axis_Title);
		}
		if (bY_Axis_Log || !std::isnan(dY_min) || !std::isnan(dY_max) || lpszY_Axis_Title)
		{
			bUse_Y_Axis = true;
			xmlNodePtr axisnode = xmlNewChild(root_node, NULL, BAD_CAST "AXIS",NULL);
			xmlNewProp(axisnode,BAD_CAST "axisid", BAD_CAST "yaxis1");
			xmlNewProp(axisnode,BAD_CAST "direction", BAD_CAST "y");
			if (bY_Axis_Log)
				xmlNewProp(axisnode,BAD_CAST "log", BAD_CAST "true");
			if (!std::isnan(dY_min))
			{
				sprintf(lpszValue,"%.17e",dY_min);
				xmlNewProp(axisnode,BAD_CAST "min", BAD_CAST lpszValue);
			}
			if (!std::isnan(dY_max))
			{
				sprintf(lpszValue,"%.17e",dY_max);
				xmlNewProp(axisnode,BAD_CAST "max", BAD_CAST lpszValue);
			}
			if (lpszY_Axis_Title)
				xmlNewProp(axisnode,BAD_CAST "title", BAD_CAST lpszY_Axis_Title);
		}
		xmlNodePtr sourcefilenode = xmlNewChild(root_node,NULL, BAD_CAST "SOURCEFILE",NULL);
		xmlNewProp(sourcefilenode,BAD_CAST "name", BAD_CAST lpszDatafile);
		xmlNewProp(sourcefilenode,BAD_CAST "fileid", BAD_CAST "file1");
		sprintf(lpszValue,"%i",uiHeader_Lines);
		xmlNewProp(sourcefilenode,BAD_CAST "headerlines", BAD_CAST lpszValue);
		if (bWhitespace_Separated)
			strcpy(lpszValue,"true");
		else
			strcpy(lpszValue,"false");
		xmlNewProp(sourcefilenode,BAD_CAST "wsseparated", BAD_CAST lpszValue);

		unsigned int uiY_Count = 0;
		unsigned int uiX_Count = 0;
		if (lpszColumnList)
		{
			uiY_Count = 0;
			uiX_Count = 0;
			const char * lpszCursor = lpszColumnList;
			while (lpszCursor[0] != 0)
			{
				// bypass initial junk
				while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
					lpszCursor++;
				if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					uiY_Count++;
				while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					lpszCursor++;
			}
			// parse the X column list
			if (uiX_Axis_Column == -1)
			{
				lpszCursor = lpszX_Columns;
				while (lpszCursor && lpszCursor[0] != 0)
				{
					// bypass initial junk
					while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
						lpszCursor++;
					if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						uiX_Count++;
					while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						lpszCursor++;
				}
			}
			if (uiX_Axis_Column == -1 && uiX_Count != uiY_Count)
			{
				fprintf(stderr,"X column has not been specified and number of Y columns specified does not equate the number of X columns specified.\n");
				exit(1);
			}
			class PLOTCOLS { public: unsigned int m_uiX_Column; unsigned int m_uiY_Column;};
			PLOTCOLS * lpcPlots = new PLOTCOLS[uiY_Count];
			uiY_Count = 0;
			lpszCursor = lpszColumnList;
			while (lpszCursor[0] != 0)
			{
				// bypass initial junk
				while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
					lpszCursor++;
				if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
				{
					lpcPlots[uiY_Count].m_uiY_Column = atoi(lpszCursor);
					if (uiX_Axis_Column != -1)
						lpcPlots[uiY_Count].m_uiX_Column = uiX_Axis_Column;
					uiY_Count++;
				}
				while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					lpszCursor++;
			}
			if (uiX_Axis_Column == -1)
			{
				uiX_Count = 0;
				lpszCursor = lpszX_Columns;
				while (lpszCursor[0] != 0)
				{
					// bypass initial junk
					while (lpszCursor[0] != 0 && (lpszCursor[0] < '0' || lpszCursor[0] > '9'))
						lpszCursor++;
					if (lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
					{
						lpcPlots[uiX_Count].m_uiX_Column = atoi(lpszCursor);
						uiX_Count++;
					}
					while (lpszCursor[0] != 0 && lpszCursor[0] >= '0' && lpszCursor[0] <= '9')
						lpszCursor++;
				}
			}
			for (unsigned int uiI = 0; uiI < uiY_Count; uiI++)
			{
				xmlNodePtr plotnode = xmlNewChild(root_node,NULL, BAD_CAST "PLOT",NULL);
				if (bUse_X_Axis)
					xmlNewProp(plotnode,BAD_CAST "xaxisid", BAD_CAST "xaxis1");
				if (bUse_Y_Axis)
					xmlNewProp(plotnode,BAD_CAST "yaxisid", BAD_CAST "yaxis1");
				if (bCycle_Line_Style)
					xmlNewProp(plotnode,BAD_CAST "style", BAD_CAST "default");
				else
					xmlNewProp(plotnode,BAD_CAST "style", BAD_CAST "solid");
				xmlNodePtr plotfilenode = xmlNewChild(plotnode,NULL, BAD_CAST "PLOTFILE",NULL);
				xmlNewProp(plotfilenode,BAD_CAST "fileid", BAD_CAST "file1");
				sprintf(lpszValue,"%i",lpcPlots[uiI].m_uiX_Column);
				xmlNewProp(plotfilenode,BAD_CAST "xcol", BAD_CAST lpszValue);
				sprintf(lpszValue,"%i",lpcPlots[uiI].m_uiY_Column);
				xmlNewProp(plotfilenode,BAD_CAST "ycol", BAD_CAST lpszValue);
			}
			delete [] lpcPlots;
			
		}
		else
		{
			XDATASET cData;
			cData.ReadDataFile(lpszDatafile,bWhitespace_Separated,false,bWhitespace_Separated ? 0 : ',', uiHeader_Lines);

			unsigned int uiNum_Col = cData.GetNumColumns();
			if (uiNum_Col > 1)
			{
				uiY_Count = 0;
				for (unsigned int uiI = 0; uiI < uiNum_Col; uiI++)
				{
					if (uiI != uiX_Axis_Column)
					{
						xmlNodePtr plotnode = xmlNewChild(root_node,NULL, BAD_CAST "PLOT",NULL);
						if (bUse_X_Axis)
							xmlNewProp(plotnode,BAD_CAST "xaxisid", BAD_CAST "xaxis1");
						if (bUse_Y_Axis)
							xmlNewProp(plotnode,BAD_CAST "yaxisid", BAD_CAST "yaxis1");
						if (bCycle_Line_Style)
							xmlNewProp(plotnode,BAD_CAST "style", BAD_CAST "default");
						else
							xmlNewProp(plotnode,BAD_CAST "style", BAD_CAST "solid");
						xmlNodePtr plotfilenode = xmlNewChild(plotnode,NULL, BAD_CAST "PLOTFILE",NULL);
						xmlNewProp(plotfilenode,BAD_CAST "fileid", BAD_CAST "file1");
						sprintf(lpszValue,"%i",uiX_Axis_Column);
						xmlNewProp(plotfilenode,BAD_CAST "xcol", BAD_CAST lpszValue);
						sprintf(lpszValue,"%i",uiI);
						xmlNewProp(plotfilenode,BAD_CAST "ycol", BAD_CAST lpszValue);
					}
				}
			}
		}
		Parse_XML(root_node);
	   xmlSaveFormatFileEnc("plot.xml", doc, "UTF-8", 1);

	}
	else
	{
		fprintf(stderr,"Usage: genplot --plotfile=<file>\n");
		fprintf(stderr,"     or\n"); 
		fprintf(stderr,"       genplot --file=<file> [OPTIONS]\n");
		fprintf(stderr,"Command line options:\n");
		fprintf(stderr,"\t--x-title=<string> : specify title of x-axis\n");
		fprintf(stderr,"\t--y-title=<string> : specify title of y-axis\n");
		fprintf(stderr,"\t--x-column=<col #> : specify which column to use as x axis\n");
		fprintf(stderr,"\t--y-columns=\"#,#,#,#,#,...,#\" : specify columns to use for plotting data.\n");
		fprintf(stderr,"\t--header-lines=# : specify how many rows of the data file to ignore\n\t\t(non numeric data will cause a fault)\n");
		fprintf(stderr,"\t--whitespace-separated : by default, the file is assumed to be comma\n\t\tseparated.  Use this if columns in the data files are separated by\n\t\tspaces or tabs.\n");
		fprintf(stderr,"\t--x-axis-log: use a log scale for the x axis\n\t\t[NOTE: this currently doesn't work well]\n");
		fprintf(stderr,"\t--y-axis-log: use a log scale for the y axis\n\t\t[NOTE: this currently doesn't work well]\n");
		fprintf(stderr,"\t--output=<outfile>: output plot to <outfile>.  Default file name for\n\t\toutput is <file>.eps\n");
		fprintf(stderr,"\t--x-min=#: Lower limit for x axis\n");
		fprintf(stderr,"\t--x-max=#: Upper limit for x axis\n");
		fprintf(stderr,"\t--y-min=#: Lower limit for y axis\n");
		fprintf(stderr,"\t--y-max=#: Upper limit for y axis\n");
	}
}
