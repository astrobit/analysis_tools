#include <Plot_Utilities.h>
#include <xio.h>
#include <xstdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unordered_map>

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
			m_lpszFilename = new char[strlen(i_cRHO.m_lpszFilename) + 1];
			strcpy(m_lpszFilename,i_cRHO.m_lpszFilename);
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
		bRet = strcmp((char *)i_lpAttr->children->content,"true");
	return bRet;
}
char Attr_Get_Char(const xmlAttr * i_lpAttr, unsigned int i_uiDefault)
{
	char chRet = i_uiDefault;
	if (Test_Attr_Content(i_lpAttr))
		chRet = i_lpAttr->children->content[0];
	return chRet;
}
void PrintColor(epsplot::COLOR i_eColor)
{
	switch (i_eColor)
	{
	case epsplot::BLACK:
		printf("black");
		break;
	case epsplot::RED:
		printf("red");
		break;
	case epsplot::GREEN:
		printf("green");
		break;
	case epsplot::BLUE:
		printf("blue");
		break;
	case epsplot::CYAN:
		printf("cyan");
		break;
	case epsplot::MAGENTA:
		printf("magenta");
		break;
	case epsplot::YELLOW:
		printf("yellow");
		break;
	case epsplot::WHITE:
		printf("white");
		break;
	case epsplot::GRAY_25:
	case epsplot::GREY_25:
		printf("grey (25%%)");
		break;
	case epsplot::GRAY_50:
	case epsplot::GREY_50:
		printf("grey (50%%)");
		break;
	case epsplot::GRAY_75:
	case epsplot::GREY_75:
		printf("grey (75%%)");
		break;
	case epsplot::CLR_CUSTOM_1:
	case epsplot::CLR_CUSTOM_2:
	case epsplot::CLR_CUSTOM_3:
	case epsplot::CLR_CUSTOM_4:
	case epsplot::CLR_CUSTOM_5:
	case epsplot::CLR_CUSTOM_6:
	case epsplot::CLR_CUSTOM_7:
	case epsplot::CLR_CUSTOM_8:
	case epsplot::CLR_CUSTOM_9:
	case epsplot::CLR_CUSTOM_10:
	case epsplot::CLR_CUSTOM_11:
	case epsplot::CLR_CUSTOM_12:
	case epsplot::CLR_CUSTOM_13:
	case epsplot::CLR_CUSTOM_14:
	case epsplot::CLR_CUSTOM_15:
	case epsplot::CLR_CUSTOM_16:
		printf("user");
		break;
	}
}

void PrintStyle(epsplot::STIPPLE i_eStipple)
{
	switch (i_eStipple)
	{
	case epsplot::SOLID:
		printf(", -----");
		break;
	case epsplot::SHORT_DASH:
		printf(", - - ");
		break;
	case epsplot::LONG_DASH:
		printf(", -- -- ");
		break;
	case epsplot::LONG_SHORT_DASH:
		printf(", -- - -- ");
		break;
	case epsplot::DOTTED:
		printf(", . . .");
		break;
	case epsplot::SHORT_DASH_DOTTED:
		printf(", - . - ");
		break;
	case epsplot::LONG_DASH_DOTTED:
		printf(", -- . --");
		break;
	}
}
int strcmp(const xmlChar * i_lpszLHO, const char * i_lpszRHO)
{
	return strcmp((char *) i_lpszLHO,i_lpszRHO);
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
	const char * lpszGraph_Title = NULL;
	const char * lpszOutput_Path = NULL;
	const char * lpszLayout = NULL;
	const char * lpszSize = NULL;
	const char * lpszWidth = NULL;
	const char * lpszHeight = NULL;
	epsplot::DATA cPlot;
	epsplot::PAGE_PARAMETERS	cPlot_Parameters;
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
				if (strcmp(lpCurr_Node->name,"SOURCEFILE") == 0)
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
				else if (strcmp(lpCurr_Node->name,"COLOR") == 0)
				{
					if (lpCurr_Node->properties)
					{
						xmlAttr * lpCurr_Attr = lpCurr_Node->properties;
						epsplot::COLOR_TRIPLET	cColor;
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
						epsplot::AXIS_PARAMETERS	cAxis;
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
			if (lpCurr_Node->type == XML_ELEMENT_NODE && strcmp(lpCurr_Node->name,"PLOT") == 0)
			{
				const char * lpszColor = NULL;
				const char * lpszStyle = NULL;
				const char * lpszSymbol = NULL;
				double dWidth;
				double dX_Offset;
				double dY_Offset;
				const char * lpszX_Axis_ID = NULL;
				const char * lpszY_Axis_ID = NULL;
				double dSymbol_Size;
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
						else if (strcmp(lpCurr_Attr->name,"xoffset") == 0)
						{
							dX_Offset = Attr_Get_Double(lpCurr_Attr,0.0);
						}
						else if (strcmp(lpCurr_Attr->name,"yoffset") == 0)
						{
							dY_Offset = Attr_Get_Double(lpCurr_Attr,0.0);
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
				bool bNo_Symbol = (!lpszSymbol || strcmp(lpszSymbol,"none") == 0);
				bool bNo_Line = (lpszStyle && strcmp(lpszStyle,"none") == 0);
				if (!bFault)
				{

					epsplot::TEXT_PARAMETERS	cText_Paramters;
					epsplot::LINE_PARAMETERS 	cLine_Parameters;
					epsplot::SYMBOL_PARAMETERS 	cSymbol_Parameters;
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

					unsigned int uiX_Axis = lpszX_Axis_ID == NULL ? -1 : cX_Axes[std::string(lpszX_Axis_ID)];
					unsigned int uiY_Axis = lpszY_Axis_ID == NULL ? -1 : cY_Axes[std::string(lpszY_Axis_ID)];
					cSymbol_Parameters.m_dSize = dSymbol_Size;
					cSymbol_Parameters.m_eColor = cLine_Parameters.m_eColor;
					std::vector < epsplot::eps_pair> cData;
					xmlNode * lpData_Node = lpCurr_Node->children;
					while (lpData_Node && lpData_Node->type != XML_ELEMENT_NODE)
						lpData_Node = lpData_Node->next;
					if (lpData_Node && lpData_Node->type == XML_ELEMENT_NODE)
					{
						if (strcmp(lpData_Node->name,"PLOTFILE") == 0)
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
								XDATASET cSF_Data = cSource_Files[lpszFile_ID].m_xdDataset;
								if (cSF_Data.GetNumColumns() > uiY_Column && cSF_Data.GetNumColumns() > uiX_Column)
								{
									for (unsigned int uiI = 0; uiI < cSF_Data.GetNumRows(); uiI++)
									{
										if (!cSF_Data.IsElementEmpty(uiX_Column,uiI) && !cSF_Data.IsElementEmpty(uiY_Column,uiI))
										{
											epsplot::eps_pair cPair;
											cPair.m_dX = cSF_Data.GetElement(uiX_Column,uiI) + dX_Offset;
											cPair.m_dY = cSF_Data.GetElement(uiY_Column,uiI) + dY_Offset;
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
							xmlNode * lpPair_Node = lpData_Node->children;
							while (lpPair_Node && lpPair_Node->type != XML_ELEMENT_NODE)
								lpPair_Node = lpPair_Node->next;

							if (lpPair_Node && lpPair_Node->type == XML_ELEMENT_NODE && strcmp(lpPair_Node->name,"TUPLE") == 0)
							{
								xmlNode * lpCurr_Tuple = lpPair_Node;
								epsplot::eps_pair	cPair;
								while (lpCurr_Tuple)
								{
									xmlAttr * lpCurr_Attr = lpCurr_Tuple->properties;
									while (lpCurr_Attr)
									{
										if (strcmp(lpCurr_Attr->name,"x") == 0)
										{
											cPair.m_dX = Attr_Get_Double(lpCurr_Attr,0.0);
										}
										else if (strcmp(lpCurr_Attr->name,"y") == 0)
										{
											cPair.m_dY = Attr_Get_Double(lpCurr_Attr,0.0);
										}
										lpCurr_Attr = lpCurr_Attr->next;
									}
									cData.push_back(cPair);
									lpCurr_Tuple = lpCurr_Tuple->next;
								}
							}
						}
					}
					if (cData.size() > 0)
					{
						if (!bNo_Symbol)
							; // @@TODO
						if (!bNo_Line)
						{
							printf("Plot : ");
							PrintColor(cLine_Parameters.m_eColor);
							printf(" ");
							PrintStyle(cLine_Parameters.m_eStipple);
							printf("\n");
							cPlot.Set_Plot_Data(cData,cLine_Parameters,uiX_Axis,uiY_Axis);
						}
					}
				}
			}
			lpCurr_Node = lpCurr_Node->next;
		}




		cPlot.Plot(cPlot_Parameters);
		printf("Plot output to %s\n",lpszOutput_Path);

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
		xmlDocSetRootElement(doc, root_node);
		if (lpszOutput_File && lpszOutput_File[0] != 0)
			xmlNewProp(root_node,BAD_CAST "outputfile", BAD_CAST lpszOutput_File);
		else
		{
			char lpszOutfile[256];
			sprintf(lpszOutfile,"%s.eps",lpszDatafile);
			xmlNewProp(root_node,BAD_CAST "outputfile", BAD_CAST lpszOutfile);
		}
		if (bX_Axis_Log || !isnan(dX_min) || !isnan(dX_max) || lpszX_Axis_Title)
		{
			bUse_X_Axis = true;
			xmlNodePtr axisnode = xmlNewChild(root_node, NULL, BAD_CAST "AXIS",NULL);
			xmlNewProp(axisnode,BAD_CAST "axisid", BAD_CAST "xaxis1");
			xmlNewProp(axisnode,BAD_CAST "direction", BAD_CAST "x");
			if (bX_Axis_Log)
				xmlNewProp(axisnode,BAD_CAST "log", BAD_CAST "true");
			if (!isnan(dX_min))
			{
				sprintf(lpszValue,"%.17e",dX_min);
				xmlNewProp(axisnode,BAD_CAST "min", BAD_CAST lpszValue);
			}
			if (!isnan(dX_max))
			{
				sprintf(lpszValue,"%.17e",dX_max);
				xmlNewProp(axisnode,BAD_CAST "max", BAD_CAST lpszValue);
			}
			if (lpszX_Axis_Title)
				xmlNewProp(axisnode,BAD_CAST "title", BAD_CAST lpszX_Axis_Title);
		}
		if (bY_Axis_Log || !isnan(dY_min) || !isnan(dY_max) || lpszY_Axis_Title)
		{
			bUse_Y_Axis = true;
			xmlNodePtr axisnode = xmlNewChild(root_node, NULL, BAD_CAST "AXIS",NULL);
			xmlNewProp(axisnode,BAD_CAST "axisid", BAD_CAST "yaxis1");
			xmlNewProp(axisnode,BAD_CAST "direction", BAD_CAST "y");
			if (bY_Axis_Log)
				xmlNewProp(axisnode,BAD_CAST "log", BAD_CAST "true");
			if (!isnan(dY_min))
			{
				sprintf(lpszValue,"%.17e",dY_min);
				xmlNewProp(axisnode,BAD_CAST "min", BAD_CAST lpszValue);
			}
			if (!isnan(dY_max))
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
			for (unsigned int uiI = 0; uiI < uiX_Count; uiI++)
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
