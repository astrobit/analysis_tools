#pragma once
#include <cstring>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>

namespace	epsplot
{
	enum PS_FONT {TIMES,HELVETICA,COURIER,SYMBOL};
	enum PS_HORIZONTAL_JUSTIFICATION {LEFT,CENTER,RIGHT};
	enum PS_VERTICAL_JUSTIFICATION {TOP,MIDDLE,BOTTOM};
	enum PS_STANDARD_COLORS { STD_BLACK,STD_WHITE,STD_RED,STD_GREEN,STD_BLUE,STD_CYAN,STD_MAGENTA,STD_YELLOW};
	enum COLOR {BLACK, RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA, GREY_25, GREY_50, GREY_75, GRAY_25, GRAY_50, GRAY_75, WHITE, CLR_CUSTOM_1, CLR_CUSTOM_2, CLR_CUSTOM_3, CLR_CUSTOM_4, CLR_CUSTOM_5, CLR_CUSTOM_6, CLR_CUSTOM_7, CLR_CUSTOM_8, CLR_CUSTOM_9, CLR_CUSTOM_10, CLR_CUSTOM_11, CLR_CUSTOM_12, CLR_CUSTOM_13, CLR_CUSTOM_14, CLR_CUSTOM_15, CLR_CUSTOM_16};
	enum STIPPLE {SOLID, SHORT_DASH, LONG_DASH, LONG_SHORT_DASH, DOTTED, SHORT_DASH_DOTTED, LONG_DASH_DOTTED, LONG_SHORT_DASH_DOTTED, LONG_LONG_DASH, STPL_CUSTOM_1, STPL_CUSTOM_2, STPL_CUSTOM_3, STPL_CUSTOM_4, STPL_CUSTOM_5, STPL_CUSTOM_6, STPL_CUSTOM_7, STPL_CUSTOM_8, STPL_CUSTOM_9, STPL_CUSTOM_10, STPL_CUSTOM_11, STPL_CUSTOM_12, STPL_CUSTOM_13, STPL_CUSTOM_14, STPL_CUSTOM_15, STPL_CUSTOM_16};
	enum SYMBOL_TYPE {SQUARE, CIRCLE, TRIANGLE_UP, TRIANGLE_DOWN, TRIANGLE_LEFT, TRIANGLE_RIGHT, DIAMOND, TIMES_SYMB, PLUS_SYMB, DASH_SYMB, ASTERISK_SYMB, STAR4, STAR5, STAR6, SYMB_CUSTOM_1, SYMB_CUSTOM_2, SYMB_CUSTOM_3, SYMB_CUSTOM_4, SYMB_CUSTOM_5, SYMB_CUSTOM_6, SYMB_CUSTOM_7, SYMB_CUSTOM_8, SYMB_CUSTOM_9, SYMB_CUSTOM_10, SYMB_CUSTOM_11, SYMB_CUSTOM_12, SYMB_CUSTOM_13, SYMB_CUSTOM_14, SYMB_CUSTOM_15, SYMB_CUSTOM_16};
	enum ERRORBAR_DIRECTION {ERRORBAR_X_LEFT, ERRORBAR_X_RIGHT, ERRORBAR_Y_UPPER, ERRORBAR_Y_LOWER};
	enum ERRORBAR_TIP_TYPE {ERRORBAR_TIP_LINE, ERRORBAR_TIP_ARROW, ERRORBAR_TIP_LINE_AND_ARROW};
	enum	AXIS {X_AXIS,Y_AXIS,Z_AXIS};
	enum z_axis_scheme {rainbow,inverse_rainbow,dark_to_light,light_to_dark,two_color_transition};

	class eps_pair
	{
	public:
		double	m_dX;
		double	m_dY;

		eps_pair(void) {m_dX = m_dY = 0.0;}
		eps_pair(const double & i_dX, const double & i_dY) {m_dX = i_dX; m_dY = i_dY;}
		eps_pair operator +(const eps_pair & i_cRHO) const
		{
			return eps_pair(m_dX + i_cRHO.m_dX,m_dY + i_cRHO.m_dY);
		}
		eps_pair operator -(const eps_pair & i_cRHO) const
		{
			return eps_pair(m_dX + i_cRHO.m_dX,m_dY + i_cRHO.m_dY);
		}
		eps_pair & operator +=(const eps_pair & i_cRHO)
		{
			m_dX += i_cRHO.m_dX;
			m_dY += i_cRHO.m_dY;
			return *this;
		}
		eps_pair & operator -=(const eps_pair & i_cRHO)
		{
			m_dX -= i_cRHO.m_dX;
			m_dY -= i_cRHO.m_dY;
			return *this;
		}
	};
	class eps_triplet
	{
	public:
		double	m_dX;
		double	m_dY;
		double	m_dZ;

		eps_triplet(void) {m_dX = m_dY = m_dZ = 0.0;}
		eps_triplet(const double & i_dX, const double & i_dY, const double & i_dZ) {m_dX = i_dX; m_dY = i_dY; m_dZ = i_dZ;}
		eps_triplet operator +(const eps_triplet & i_cRHO) const
		{
			return eps_triplet(m_dX + i_cRHO.m_dX,m_dY + i_cRHO.m_dY,m_dZ + i_cRHO.m_dZ);
		}
		eps_triplet operator -(const eps_triplet & i_cRHO) const
		{
			return eps_triplet(m_dX - i_cRHO.m_dX,m_dY - i_cRHO.m_dY,m_dZ - i_cRHO.m_dZ);
		}
		eps_triplet & operator +=(const eps_triplet & i_cRHO)
		{
			m_dX += i_cRHO.m_dX;
			m_dY += i_cRHO.m_dY;
			m_dZ += i_cRHO.m_dZ;
			return *this;
		}
		eps_triplet & operator -=(const eps_triplet & i_cRHO)
		{
			m_dX -= i_cRHO.m_dX;
			m_dY -= i_cRHO.m_dY;
			m_dZ -= i_cRHO.m_dZ;
			return *this;
		}
	};

	class color_triplet
	{
	public:
		double		m_dRed; // 0.0 - 1.0
		double		m_dGreen; // 0.0 - 1.0
		double		m_dBlue; // 0.0 - 1.0

		color_triplet(void) {m_dRed = m_dGreen = m_dBlue = 0.0;}
		color_triplet(const double & i_dRed, const double & i_dGreen, const double & i_dBlue)
		{
			m_dRed = i_dRed;
			m_dGreen = i_dGreen;
			m_dBlue = i_dBlue;
		}
	};
	typedef color_triplet COLOR_TRIPLET;
	
	class page_parameters
	{
	public:
		unsigned int 	m_uiNum_Columns;
		unsigned int 	m_uiNum_Rows;
		double			m_dTitle_Margin;
		double			m_dLeft_Axis_Margin;
		double			m_dRight_Axis_Margin;
		double			m_dBottom_Axis_Margin;
		double			m_dTop_Axis_Margin;
		double			m_dWidth_Inches;
		double			m_dHeight_Inches;
		double			m_dSide_Unprintable_Margins_Inches;
		double			m_dTop_Bottom_Unprintable_Margins_Inches;


		page_parameters(void)
		{
			m_dWidth_Inches = 8.5;
			m_dHeight_Inches = 11.0;
			m_uiNum_Columns = 1;
			m_uiNum_Rows = 1;
			m_dTitle_Margin = 1.0 / m_dHeight_Inches;
			m_dTop_Axis_Margin = 0.0;
			m_dBottom_Axis_Margin = 1.2 / m_dHeight_Inches;
			m_dLeft_Axis_Margin = 1.0 / m_dWidth_Inches;
			m_dRight_Axis_Margin = 0.0;

			m_dSide_Unprintable_Margins_Inches = 0.25; // Inches
			m_dTop_Bottom_Unprintable_Margins_Inches = 0.25; // Inches

		}
	};
	typedef page_parameters PAGE_PARAMETERS;


	class axis_parameters
	{
	protected:
		std::string	m_sTitle;
		void	Copy(const axis_parameters & i_cRHO)
		{
			m_sTitle = i_cRHO.m_sTitle;
			m_bLog = i_cRHO.m_bLog;
			m_bInvert = i_cRHO.m_bInvert;
			m_dLower_Limit = i_cRHO.m_dLower_Limit;
			m_dUpper_Limit = i_cRHO.m_dUpper_Limit;
			m_dLine_Width = i_cRHO.m_dLine_Width;
			m_dMajor_Tick_Width = i_cRHO.m_dMajor_Tick_Width;
			m_dMinor_Tick_Width = i_cRHO.m_dMinor_Tick_Width;
			m_eColor = i_cRHO.m_eColor;
			m_eTitle_Color = i_cRHO.m_eTitle_Color;
			m_eMajor_Label_Color = i_cRHO.m_eMajor_Label_Color;
			m_eMinor_Label_Color = i_cRHO.m_eMinor_Label_Color;
			m_eMajor_Tick_Color = i_cRHO.m_eMajor_Tick_Color;
			m_eMinor_Tick_Color = i_cRHO.m_eMinor_Tick_Color;
			m_dMajor_Label_Size = i_cRHO.m_dMajor_Label_Size;
			m_dMinor_Label_Size = i_cRHO.m_dMinor_Label_Size;
			m_dTitle_Size = i_cRHO.m_dTitle_Size;
			m_dMajor_Tick_Length = i_cRHO.m_dMajor_Tick_Length;
			m_dMinor_Tick_Length = i_cRHO.m_dMinor_Tick_Length;
			m_bLabel_Major_Indices = i_cRHO.m_bLabel_Major_Indices;
			m_bLabel_Minor_Indices = i_cRHO.m_bLabel_Minor_Indices;
			m_sMajor_Index_Format = i_cRHO.m_sMajor_Index_Format;
			m_sMinor_Index_Format = i_cRHO.m_sMajor_Index_Format;
		}
		void	Set_Defaults(void)
		{
			m_sTitle = "";
			m_bLog = m_bInvert = false;
			m_dLower_Limit = nan("");
			m_dUpper_Limit = nan("");
			m_dLine_Width = 1.5;
			m_dMajor_Tick_Width = 2.0;
			m_dMinor_Tick_Width = 1.0;
			m_eColor = BLACK;
			m_eTitle_Color = BLACK;
			m_eMajor_Label_Color = BLACK;
			m_eMinor_Label_Color = BLACK;
			m_eMajor_Tick_Color = BLACK;
			m_eMinor_Tick_Color = BLACK;
			m_dMajor_Label_Size = 18.0; // Points
			m_dMinor_Label_Size = 18.0; // Points
			m_dTitle_Size = 36.0; // Points
			m_dMajor_Tick_Length = 20.0;
			m_dMinor_Tick_Length = 10.0;
			m_bLabel_Major_Indices = true;
			m_bLabel_Minor_Indices = false;
			m_sMajor_Index_Format = "m";
			m_sMinor_Index_Format = "";
		}
	public:
		double	m_dLine_Width; // Points
		double	m_dMajor_Tick_Width; // Points
		double	m_dMajor_Tick_Length; // Points
		double	m_dMinor_Tick_Width; // Points
		double	m_dMinor_Tick_Length; // Points
		double	m_dMajor_Label_Size; // Points
		double	m_dMinor_Label_Size; // Points
		double	m_dTitle_Size; // Points
		COLOR	m_eColor; // color of line at edge of graph
		COLOR	m_eMajor_Label_Color;
		COLOR	m_eMinor_Label_Color;
		COLOR	m_eMajor_Tick_Color;
		COLOR	m_eMinor_Tick_Color;
		COLOR	m_eTitle_Color;
		std::string	m_sMajor_Index_Format;
		std::string	m_sMinor_Index_Format;
		bool	m_bLog;
		bool	m_bInvert;
		double	m_dLower_Limit; // note: use nan to indicate no limit
		double	m_dUpper_Limit; // note: use nan to indicate no limit
		bool	m_bLabel_Major_Indices;
		bool	m_bLabel_Minor_Indices; //@@TODO not implemented
		void	Set_Title(const char * i_lpszTitle)
		{
			m_sTitle = i_lpszTitle;
		}
		const char * Get_Title(void) const {return m_sTitle.c_str();}
		axis_parameters(const axis_parameters & i_cRHO)
		{
			Copy(i_cRHO);
		}
		axis_parameters & operator =(const axis_parameters & i_cRHO)
		{
			Copy(i_cRHO);
			return *this;
		}
		axis_parameters(void)
		{
			Set_Defaults();
		}
		axis_parameters(const char * i_lpszAxis_Description)
		{ // simplify code by allowing a constructor with the only a title
			Set_Defaults();
			Set_Title(i_lpszAxis_Description);
		}
		axis_parameters(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax)
		{ // simplify code by allowing a constructor with the most common parameters
			Set_Defaults();
			Set_Title(i_lpszAxis_Description);
			m_bLog = i_bLog_Axis;
			m_bInvert = i_bInvert_Axis;
			if (i_bSet_Min)
				m_dLower_Limit = i_dMin;
			if (i_bSet_Max)
				m_dUpper_Limit = i_dMax;
		}
		~axis_parameters(void)
		{
		}
	};
	typedef axis_parameters AXIS_PARAMETERS;

	class axis_parameters_Z : public axis_parameters
	{
	public:
		color_triplet	m_ctColor_Upper;
		color_triplet	m_ctColor_Lower;
		z_axis_scheme	m_eScheme;

		axis_parameters_Z(const axis_parameters_Z & i_cRHO)
		{
			Copy(i_cRHO);
			m_ctColor_Upper = i_cRHO.m_ctColor_Upper;
			m_ctColor_Lower = i_cRHO.m_ctColor_Lower;
			m_eScheme = i_cRHO.m_eScheme;
		}
		axis_parameters_Z & operator =(const axis_parameters_Z & i_cRHO)
		{
			Copy(i_cRHO);
			m_ctColor_Upper = i_cRHO.m_ctColor_Upper;
			m_ctColor_Lower = i_cRHO.m_ctColor_Lower;
			m_eScheme = i_cRHO.m_eScheme;
			return *this;
		}
		axis_parameters_Z(void)
		{
			Set_Defaults();
			m_eScheme = rainbow;
		}
		axis_parameters_Z(const char * i_lpszAxis_Description)
		{ // simplify code by allowing a constructor with the only a title
			Set_Defaults();
			m_eScheme = rainbow;
			Set_Title(i_lpszAxis_Description);
		}
		axis_parameters_Z(const char * i_lpszAxis_Description, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax, z_axis_scheme i_eScheme = rainbow, const color_triplet	&i_ctColor_Upper = color_triplet(1.,1.,1.), const color_triplet	&i_ctColor_Lower = color_triplet(0.,0.,0.))
		{ // simplify code by allowing a constructor with the most common parameters
			Set_Defaults();
			m_eScheme = i_eScheme;
			m_ctColor_Upper = i_ctColor_Upper;
			m_ctColor_Lower = i_ctColor_Lower;
			Set_Title(i_lpszAxis_Description);
			m_bLog = i_bLog_Axis;
			m_bInvert = i_bInvert_Axis;
			if (i_bSet_Min)
				m_dLower_Limit = i_dMin;
			if (i_bSet_Max)
				m_dUpper_Limit = i_dMax;
		}
		~axis_parameters_Z(void)
		{
		}
	};


	class text_entity
	{
	public:
		enum entity_type {text,superscript,subscript,leftbrace,rightbrace,symbol};
		entity_type	m_eType;
		std::string	m_szData;
	};

	class epsfile
	{
	protected:
		char	m_lpszFormat[32];
		char	m_lpszSetDash[4];
		char	m_lpszMoveto[32];
		char	m_lpszLineto[32];
		char	m_lpszTranslate[32];
		char	m_lpszSetLineWidth[32];
		char	m_lpszSetRGBColor[32];
		char	m_lpszGsave[4];
		char	m_lpszGrestore[4];
		char	m_lpszStroke[4];
		char	m_lpszFill[4];
		char	m_lpszClosepath[4];
		char	m_lpszTxtCentered[16];
		char	m_lpszScalefontSetFont[8];
		char	m_lpszRectClip[32];

		char	* m_lpszFilename;
		FILE	* m_lpFileOut;
		class glyphsymbol : public std::string
		{
		protected:
			bool m_bRequires_Symbol_Font;
		public:
			glyphsymbol(void){m_bRequires_Symbol_Font = false;}
			glyphsymbol(const glyphsymbol & i_cRHO) : std::string(i_cRHO) {m_bRequires_Symbol_Font = i_cRHO.m_bRequires_Symbol_Font;}
			glyphsymbol(const std::string & i_szName, bool i_bRequires_Symbol_Font = false)  : std::string(i_szName) {m_bRequires_Symbol_Font = i_bRequires_Symbol_Font;}
			glyphsymbol(const char * i_szName, bool i_bRequires_Symbol_Font = false)  : std::string(i_szName)  {m_bRequires_Symbol_Font = i_bRequires_Symbol_Font;}

			bool requires_symbol(void)const{return m_bRequires_Symbol_Font;}
		};
//		std::map<std::string, std::string> m_mSymbol_Map;
		std::map<std::string, glyphsymbol> m_mSymbol_Map;
//		std::map<std::string, std::string> m_mSymbol_Equiv_Map;
		std::vector< text_entity > Parse_String(const std::string & i_szString) const;

	public:


		epsfile(const char * i_lpszFormat);
		~epsfile(void);

		void	Open_File(const char * i_lpszFilename, const char * i_lpszDocument_Title, const double & i_dWidth_Inches, const double & i_dHeight_Inches);
		void	Close_File(void);


		void Text(PS_FONT i_eFont, bool i_bItalic, bool i_bBold, int i_iFont_Size, PS_HORIZONTAL_JUSTIFICATION i_eHoirzontal_Justification, PS_VERTICAL_JUSTIFICATION i_eVertical_Justification, const color_triplet & i_cColor,const double & i_dX, const double & i_dY, const char * i_lpszText, const double & i_dRotation = 0.0, const double & i_dLine_Width = 1.0) const;
		void Rect_Clip(const double & i_dX, const double & i_dY, const double & i_dWidth, const double & i_dHeight) const;
		void Rect_Fill(const double & i_dX, const double & i_dY, const double & i_dWidth, const double & i_dHeight) const;
		void Move_To(const double & i_dX, const double & i_dY) const;
		void Line_To(const double & i_dX, const double & i_dY) const;
		void Translate(const double & i_dX, const double & i_dY) const;
		void State_Push(void) const;
		void State_Pop(void) const;
		void Set_Line_Width(const double &i_dLine_Width) const;
		void Set_RGB_Color(const color_triplet & i_cColor) const;
		void Set_RGB_Color(PS_STANDARD_COLORS i_eColor) const;
		void Stroke(void) const;
		void Fill(void) const;
		void Close_Path(void) const;
		void Comment(const char * i_lpszComment) const;
		void Set_Dash(const double * i_lpdPattern, unsigned int i_uiNum_Pattern_Elements, const double & i_dSpace) const;
		void Text_Bounding_Box(PS_FONT i_eFont, bool i_bItalic, bool i_bBold, int i_iFont_Size, PS_HORIZONTAL_JUSTIFICATION i_eHoirzontal_Justification, PS_VERTICAL_JUSTIFICATION i_eVertical_Justification, const color_triplet & i_cColor,const double & i_dX, const double & i_dY, const char * i_lpszText, const double & i_dRotation = 0.0, const double & i_dLine_Width = 1.0) const;
	};
	typedef epsfile EPSFILE;


	class rectangle
	{
	public:
		double	m_dX_min;
		double	m_dX_max;
		double	m_dY_min;
		double	m_dY_max;

		rectangle(void)
		{
			m_dX_min = m_dX_max = m_dY_min = m_dY_max = 0.0;
		}
	};
	typedef rectangle RECTANGLE;
	class line_parameters
	{
	public:
		COLOR		m_eColor;
		double		m_dWidth;
		STIPPLE		m_eStipple;

		line_parameters(void)
		{
			m_eColor = BLACK;
			m_dWidth = -1.0;
			m_eStipple = SOLID;
		}
	};
	typedef line_parameters LINE_PARAMETERS;
	
	class symbol_parameters
	{
	public:
		SYMBOL_TYPE	m_eType;
		double		m_dSize;
		COLOR		m_eColor;
		bool		m_bFilled;
		symbol_parameters(void)
		{
			m_eColor = BLACK;
			m_dSize = 12.0;
			m_eType = SQUARE;
			m_bFilled = true;
		}
	};
	typedef symbol_parameters SYMBOL_PARAMETERS;

	class text_parameters
	{
	public:
		PS_FONT m_eFont;
		bool m_bItalic;
		bool m_bBold;
		int m_iFont_Size;
		double m_dRotation;
		PS_HORIZONTAL_JUSTIFICATION m_eHorizontal_Justification;
		PS_VERTICAL_JUSTIFICATION m_eVertical_Justification;

		text_parameters(void)
		{
			m_eFont = TIMES;
			m_bItalic = false;
			m_bBold = false;
			m_iFont_Size = 18;
			m_dRotation = 0.0;
			m_eHorizontal_Justification = LEFT;
			m_eVertical_Justification = TOP;
		}
	};
	typedef text_parameters TEXT_PARAMETERS;

	class errorbar_parameters
	{
	public:
		ERRORBAR_DIRECTION	m_eDirection;
		double				m_dTip_Width;
		ERRORBAR_TIP_TYPE	m_eTip_Type;
		unsigned int 		m_uiAssociated_Plot;

		errorbar_parameters(void)
		{
			m_eTip_Type = ERRORBAR_TIP_LINE;
			m_dTip_Width = 4.0;
			m_eDirection = ERRORBAR_X_LEFT;
			m_uiAssociated_Plot = -1;
		}
	};
	typedef errorbar_parameters ERRORBAR_PARAMETERS;

	enum ITEM_TYPE {TYPE_LINE,TYPE_SYMBOL,TYPE_rectangle,TYPE_TEXT,TYPE_ERRORBAR,TYPE_3D};
	class	plot_item
	{
	public:
		ITEM_TYPE m_eType;

//		plot_item	*	m_lpPrev_Item;
//		plot_item	*	m_lpNext_Item;
		unsigned int 	m_uiPlot_Axes_To_Use[3];

		plot_item(ITEM_TYPE i_eType)
		{
			m_eType = i_eType;
//			m_lpPrev_Item = NULL;
//			m_lpNext_Item = NULL;
			m_uiPlot_Axes_To_Use[0] = 0;
			m_uiPlot_Axes_To_Use[1] = 0;
			m_uiPlot_Axes_To_Use[2] = -1;
		}
	};
	typedef plot_item PLOT_ITEM;

	class plot_item_3d : public plot_item
	{
	public:

		eps_triplet * m_lppData;
		unsigned int m_uiNum_Points;

		plot_item_3d(void) : plot_item(TYPE_3D)
		{
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
		~plot_item_3d(void)
		{
			if (m_lppData)
				delete [] m_lppData;
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
	};
	class line_item : public plot_item
	{
	public:

		eps_pair * m_lppData;
		unsigned int m_uiNum_Points;
		line_parameters	m_cPlot_Line_Info;

		line_item(void) : plot_item(TYPE_LINE), m_cPlot_Line_Info()
		{
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
		~line_item(void)
		{
			if (m_lppData)
				delete [] m_lppData;
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
	};
	typedef line_item LINE_ITEM;

	class symbol_item : public plot_item
	{
	public:
		eps_pair * m_lppData;
		unsigned int m_uiNum_Points;
		symbol_parameters	m_cPlot_Symbol_Info;

		symbol_item(void) : plot_item(TYPE_SYMBOL), m_cPlot_Symbol_Info()
		{
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
		~symbol_item(void)
		{
			if (m_lppData)
				delete [] m_lppData;
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
	};
	typedef symbol_item SYMBOL_ITEM;

	class rectangle_item : public plot_item
	{
	public:
		rectangle	m_cPlot_Rectangle_Info;
		bool		m_bArea_Fill;
		COLOR		m_ePlot_Area_Fill_Color;
		bool		m_bDraw_Border;
		line_parameters	m_cPlot_Border_Info;

		rectangle_item(void) : plot_item(TYPE_rectangle), m_cPlot_Rectangle_Info(), m_cPlot_Border_Info()
		{
			m_bArea_Fill = false;
			m_bDraw_Border = false;
			m_ePlot_Area_Fill_Color = BLACK;
		}
	};
	typedef rectangle_item RECTANGLE_ITEM;

	class text_item : public plot_item
	{
	protected:
		char * m_lpszText;
		size_t m_uiText_Alloc_Len;
	public:
		text_parameters	m_cText_Parameters;
		line_parameters	m_cLine_Parameters; // stipple is ignored
		double m_dX;
		double m_dY;

		text_item(void) : plot_item(TYPE_TEXT), m_cText_Parameters(), m_cLine_Parameters()
		{
			m_dX = 0.0;
			m_dY = 0.0;
			m_lpszText = NULL;
			m_uiText_Alloc_Len = 0;
		}
		~text_item(void)
		{
			if (m_lpszText)
				delete [] m_lpszText;
			m_lpszText = NULL;
			m_uiText_Alloc_Len = 0;
		}
		void Set_Text(const char * i_lpszText)
		{
			size_t uiLen = strlen(i_lpszText) + 1;
			if (uiLen > m_uiText_Alloc_Len)
			{
				if (m_lpszText)
					delete [] m_lpszText;
				m_uiText_Alloc_Len = uiLen;
				m_lpszText = new char[m_uiText_Alloc_Len];
			}
			if (m_lpszText)
				strcpy(m_lpszText,i_lpszText);
		}
		const char * Get_Text(void) const {return m_lpszText;}
	};
	typedef text_item TEXT_ITEM;

	class errorbar_item : public plot_item
	{
	public:
		errorbar_parameters	m_cErrorbar_Info;
		double * m_lppData;
		unsigned int m_uiNum_Points;
		line_parameters	m_cPlot_Line_Info;

		errorbar_item(void) : plot_item(TYPE_ERRORBAR), m_cPlot_Line_Info(), m_cErrorbar_Info()
		{
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
		~errorbar_item(void)
		{
			if (m_lppData)
				delete [] m_lppData;
			m_lppData = NULL;
			m_uiNum_Points = 0;
		}
	};
	typedef errorbar_item ERRORBAR_ITEM;


	class axis_metadata
	{
	public:
		unsigned int	m_uiIdentifier;
		bool			m_bEnabled;
		axis_parameters	m_cParameters;
		double			m_dRange; // generated and used at plot time
		double			m_dScale; // generated and used at plot time
		double			m_dLower_Limit; // generated and used at plot time
		double			m_dUpper_Limit; // generated and used at plot time
		double			m_dStart; // generated and used at plot time
		double			m_dEnd; // generated and used at plot time

		void Reset_Limits(void)
		{
			m_dScale = m_dRange = m_dUpper_Limit = m_dLower_Limit = nan("");
		}
		axis_metadata(void) : m_cParameters()
		{
			m_bEnabled = true;
			m_uiIdentifier = -1;
			Reset_Limits();
		}
		axis_metadata(const axis_parameters & i_cAxis_Parameters) : m_cParameters(i_cAxis_Parameters)
		{
			m_bEnabled = true;
			m_uiIdentifier = -1;
			Reset_Limits();
		}

		void Adjust_Limits(const double & i_dValue)
		{
			if (std::isnan(m_cParameters.m_dLower_Limit) && (std::isnan(m_dLower_Limit) || m_dLower_Limit > i_dValue))
				m_dLower_Limit = i_dValue;
			if (std::isnan(m_cParameters.m_dUpper_Limit) && (std::isnan(m_dUpper_Limit) || m_dUpper_Limit < i_dValue))
				m_dUpper_Limit = i_dValue;
		}
		void Finalize_Limit(const double & i_dGraph_Space)
		{
			if (!std::isnan(m_cParameters.m_dLower_Limit))
				m_dLower_Limit = m_cParameters.m_dLower_Limit;
			if (!std::isnan(m_cParameters.m_dUpper_Limit))
				m_dUpper_Limit = m_cParameters.m_dUpper_Limit;
			m_dRange = m_dUpper_Limit - m_dLower_Limit;
			if (m_dRange > 0.0)
				m_dScale = i_dGraph_Space / m_dRange;
			else
				m_dScale = 1.0;

			if (m_cParameters.m_bLog && m_cParameters.m_bInvert)
			{
				if (m_dUpper_Limit > 0.0 && m_dLower_Limit > 0.0)
				{
					m_dStart = log10(m_dUpper_Limit);
					m_dEnd = log10(m_dLower_Limit);
				}
			}
			else if (m_cParameters.m_bLog)
			{
				if (m_dUpper_Limit > 0.0 && m_dLower_Limit > 0.0)
				{
					m_dStart = log10(m_dLower_Limit);
					m_dEnd = log10(m_dUpper_Limit);
				}
			}
			else if (m_cParameters.m_bInvert)
			{
				m_dStart = m_dUpper_Limit;
				m_dEnd = m_dLower_Limit;
			}
			else
			{
				m_dStart = m_dLower_Limit;
				m_dEnd = m_dUpper_Limit;
			}
			if (m_cParameters.m_bLog)
				m_dScale = i_dGraph_Space / fabs(m_dEnd - m_dStart);


			if (m_cParameters.m_bInvert)
			{
				m_dScale *= -1.0;
			}
		}
		double Scale(const double &i_dX) const
		{
			double dX = nan("");
			if (m_cParameters.m_bLog && i_dX > 0.0)
				dX = log10(i_dX);
			else if (!m_cParameters.m_bLog)
				dX = i_dX;
			dX -= m_dStart;
			dX *= m_dScale;
			return dX;
		}

	};
	typedef axis_metadata AXIS_METADATA;

	class axis_metadata_Z : public axis_metadata
	{
	public:
		color_triplet	m_ctColor_Upper;
		color_triplet	m_ctColor_Lower;
		z_axis_scheme	m_eScheme;

		axis_metadata_Z(void)
		{
			m_bEnabled = true;
			m_uiIdentifier = -1;
			Reset_Limits();
		}
		axis_metadata_Z(const axis_parameters_Z & i_cAxis_Parameters) : axis_metadata(i_cAxis_Parameters)
		{
			//m_cParameters = i_cAxis_Parameters;
			m_bEnabled = true;
			m_uiIdentifier = -1;
			Reset_Limits();
		}

		color_triplet	Get_Color(const double & i_dX)
		{
			color_triplet	cRet;
			double dVal = i_dX;
			if (m_cParameters.m_bLog && i_dX > 0.0)
				dVal = log10(i_dX);
			dVal -= m_dStart;
			dVal /= (m_dEnd - m_dStart);
			if (dVal < 0.0)
				dVal = 0.0;
			else if (dVal > 1.0)
				dVal = 1.0;
			switch (m_eScheme)
			{
			case inverse_rainbow:
				dVal = 1.0 - dVal;
			case rainbow:
				if (dVal > 0.833333333333333333)
				{
					m_ctColor_Lower = color_triplet(0.0,1.0,1.0);
					m_ctColor_Upper = color_triplet(0.2,0.0,1.0);
					dVal -= 0.833333333333333333;
				}
				else if (dVal > 0.6666666666666666666)
				{
					m_ctColor_Lower = color_triplet(0.0,0.0,1.0);
					m_ctColor_Upper = color_triplet(0.0,1.0,1.0);
					dVal -= 0.66666666666666666;
				}
				else if (dVal > 0.5)
				{
					m_ctColor_Lower = color_triplet(0.0,1.0,0.0);
					m_ctColor_Upper = color_triplet(0.0,0.0,1.0);
					dVal -= 0.5;
				}
				else if (dVal > 0.3333333333333333333)
				{
					m_ctColor_Lower = color_triplet(1.0,1.0,0.0);
					m_ctColor_Upper = color_triplet(0.0,1.0,0.0);
					dVal -= 0.3333333333333333333;
				}
				else if (dVal > 0.1666666666666666666)
				{
					m_ctColor_Lower = color_triplet(1.0,0.5,0.0);
					m_ctColor_Upper = color_triplet(1.0,1.0,0.0);
					dVal -= 0.1666666666666666666;
				}
				else
				{
					m_ctColor_Lower = color_triplet(1.0,0.0,0.0);
					m_ctColor_Upper = color_triplet(1.0,0.5,0.0);
				}
				dVal /= 0.166666666666666667;
			case dark_to_light:
			case light_to_dark:
			case two_color_transition:
				cRet.m_dRed = m_ctColor_Lower.m_dRed + (m_ctColor_Upper.m_dRed * dVal - m_ctColor_Lower.m_dRed * dVal);
				cRet.m_dGreen = m_ctColor_Lower.m_dGreen + (m_ctColor_Upper.m_dGreen * dVal - m_ctColor_Lower.m_dGreen * dVal);
				cRet.m_dBlue = m_ctColor_Lower.m_dBlue + (m_ctColor_Upper.m_dBlue * dVal - m_ctColor_Lower.m_dBlue * dVal);
				break;
			}
		}
	};
		

	class data
	{
	protected:
		char	* m_lpszTitle;
		double	m_dTitle_Size;
		COLOR	m_eTitle_Color;
		char 	* m_lpszFilename;

		std::vector<plot_item *> m_vcPlot_Item_List;

		color_triplet	m_cCustom_Colors[16];
		double	* m_lpdCustom_Stipple[16];
		unsigned int m_uiStipple_Length[16];

		std::vector<axis_metadata>	m_cX_Axis_Parameters;
		std::vector<axis_metadata>	m_cY_Axis_Parameters;
		std::vector<axis_metadata_Z> m_cZ_Axis_Parameters;

		void Draw_Symbol(EPSFILE & io_cEPS, const double & i_dX, const double & i_dY, const symbol_parameters & i_cSymbol_Param);
		void	Deallocate_Plot_Data(void)
		{
			for (std::vector<plot_item *>::iterator	cIterator = m_vcPlot_Item_List.begin(); cIterator != m_vcPlot_Item_List.end(); cIterator++)
			{
				line_item * lpcLine = NULL;
				symbol_item * lpcSymbol = NULL;
				rectangle_item * lpcRectangle = NULL;
				text_item * lpcText = NULL;
				plot_item * lpCurr = *cIterator;

				switch (lpCurr->m_eType)
				{
				case TYPE_LINE:
					lpcLine = (line_item *) lpCurr;
					break;
				case TYPE_SYMBOL:
					lpcSymbol = (symbol_item *) lpCurr;
					break;
				case TYPE_rectangle:
					lpcRectangle = (rectangle_item *) lpCurr;
					break;
				case TYPE_TEXT:
					lpcText = (text_item *) lpCurr;
					break;
				}
				if (lpcLine)
					delete lpcLine;
				else if (lpcSymbol)
					delete lpcSymbol;
				else if (lpcRectangle)
					delete lpcRectangle;
				else if (lpcText)
					delete lpcText;
			}
			m_vcPlot_Item_List.clear();
		}
		void	Deallocate_X_Axis_Data(void)
		{
			m_cX_Axis_Parameters.clear();
		}
		void	Deallocate_Y_Axis_Data(void)
		{
			m_cY_Axis_Parameters.clear();
		}
		void	Deallocate_Z_Axis_Data(void)
		{
			m_cZ_Axis_Parameters.clear();
		}
		void	Deallocate_Axis_Data(void)
		{
			Deallocate_X_Axis_Data();
			Deallocate_Y_Axis_Data();
			Deallocate_Z_Axis_Data();
		}
		void	Null_Pointers(void)
		{
			m_vcPlot_Item_List.clear();
			m_lpszTitle = NULL;
			m_lpszFilename = NULL;

			for (unsigned int uiI = 0; uiI < 16; uiI++)
			{
				m_lpdCustom_Stipple[uiI] = NULL;
				m_uiStipple_Length[uiI] = 0;
			}
		}
		std::vector<axis_metadata> * Get_Axis_Metedata_Vector_Ptr(AXIS i_eAxis);

	public:
	// Methods for the title
		void	Set_Plot_Title(const char * i_lpszTitle, const double & i_dSize = 36.0, COLOR i_eColor = BLACK) { if(m_lpszTitle) delete [] m_lpszTitle; if (i_lpszTitle && i_lpszTitle[0] != 0) {m_lpszTitle = new char[strlen(i_lpszTitle) + 1]; strcpy(m_lpszTitle,i_lpszTitle);} else m_lpszTitle = NULL;m_dTitle_Size = i_dSize;m_eTitle_Color = i_eColor;}
		const char *	Get_Plot_Title(void) const { return m_lpszTitle;}

	// Methods for the destination file
		void	Set_Plot_Filename(const char * i_lpszFilename) { if(m_lpszFilename) delete [] m_lpszFilename; if (i_lpszFilename && i_lpszFilename[0] != 0) {m_lpszFilename = new char[strlen(i_lpszFilename) + 1]; strcpy(m_lpszFilename,i_lpszFilename);} else m_lpszFilename = NULL;}
		const char *	Get_Plot_Filename(void) const { return m_lpszFilename;}


	// Methods for setting data to be plotted
		unsigned int	Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, COLOR i_eColor, STIPPLE i_eStipple, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID, const double & i_dLine_Width);
		unsigned int	Set_Plot_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Set_Plot_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Set_Plot_Data(const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Plot_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);

		unsigned int	Set_Errorbar_Data(const errorbar_parameters & i_cErrorbar_Parameters, const std::vector<double> &i_vdValues, const line_parameters & i_cLine_Parameters);

		unsigned int	Set_Symbol_Data(const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Set_Symbol_Data(const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Set_Symbol_Data(const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const double * i_lpdX_Values, const double * i_lpdY_Values, unsigned int i_uiNum_Points, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<double> &i_vdX_Values, const std::vector<double> &i_vdY_Values, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);
		unsigned int	Modify_Symbol_Data(unsigned int i_uiPlot_Data_ID, const std::vector<eps_pair> &i_vpValues, const symbol_parameters & i_cSymbol_Parameters, unsigned int i_uiX_Axis_ID, unsigned int i_uiY_Axis_ID);

	// Methods for controlling axes
		unsigned int	Set_X_Axis_Parameters(const char * i_lpszAxis_Title, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax);
		unsigned int	Set_Y_Axis_Parameters(const char * i_lpszAxis_Title, bool i_bLog_Axis, bool i_bInvert_Axis, bool i_bSet_Min, const double & i_dMin, bool i_bSet_Max, const double & i_dMax);
		unsigned int	Set_Axis_Parameters(AXIS i_eAxis, const axis_parameters & i_cAxis_Parameters);
		unsigned int	Set_Z_Axis_Parameters(const axis_parameters_Z & i_cAxis_Parameters);
		unsigned int	Set_X_Axis_Parameters(const axis_parameters & i_cAxis_Parameters);
		unsigned int	Set_Y_Axis_Parameters(const axis_parameters & i_cAxis_Parameters);
		unsigned int	Modify_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters);
		unsigned int	Modify_X_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters);
		unsigned int	Modify_Y_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters & i_cAxis_Parameters);
		unsigned int	Modify_Z_Axis_Parameters(unsigned int i_uiWhich, const axis_parameters_Z & i_cAxis_Parameters);
		axis_parameters	Get_Axis_Parameters(AXIS i_eAxis, unsigned int i_uiWhich);
		axis_parameters	Get_X_Axis_Parameters(unsigned int i_uiWhich);
		axis_parameters	Get_Y_Axis_Parameters(unsigned int i_uiWhich);
		axis_parameters_Z	Get_Z_Axis_Parameters(unsigned int i_uiWhich);
		unsigned int	Get_Num_Axes(AXIS i_eAxis) const
		{
			unsigned int uiRet = 0;
			switch (i_eAxis)
			{
			case X_AXIS:
				uiRet = m_cX_Axis_Parameters.size();
				break;
			case Y_AXIS:
				uiRet = m_cY_Axis_Parameters.size();
				break;
			case Z_AXIS:
				uiRet = m_cZ_Axis_Parameters.size();
				break;
			}
		}
		unsigned int	Get_Num_X_Axes(void) const {return Get_Num_Axes(X_AXIS);};
		unsigned int	Get_Num_Y_Axes(void) const {return Get_Num_Axes(Y_AXIS);};
		unsigned int	Get_Num_Z_Axes(void) const {return Get_Num_Axes(Z_AXIS);};

	// Methods for colors
		void	Define_Custom_Color(COLOR i_eColor, const color_triplet & i_cColor);
		color_triplet Get_Color(COLOR i_eColor) const;

	// Methods for stipples
		void	Define_Custom_Stipple(STIPPLE i_eStipple, const double * i_lpdStipple, unsigned int i_uiStipple_Length);
		const double *	Get_Stipple(STIPPLE i_eStipple, unsigned int & o_uiStipple_Length) const;

	// Methods for rectangles
		unsigned int	Set_Rectangle_Data(const rectangle & i_cArea, bool i_bFill, COLOR i_eFill_Color, bool i_bBorder, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type);
		unsigned int	Modify_Rectangle_Data(unsigned int i_uiPlot_Data_ID, const rectangle & i_cArea, bool i_bFill, COLOR i_eFill_Color, bool i_bBorder, const line_parameters & i_cLine_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type);

	// Methods for text
		unsigned int	Set_Text_Data(const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type);
		unsigned int	Modify_Text_Data(unsigned int i_uiText_Data_ID, const double & i_dX, const double & i_dY, const char * i_lpszText, const line_parameters & i_cLine_Parameters, const text_parameters & i_cText_Parameters, unsigned int i_uiX_Axis_Type, unsigned int i_uiY_Axis_Type);

	// Methods for plotting
		void	Plot(const page_parameters & i_cGrid);
		void	Plot_3d(const page_parameters & i_cGrid); // uses z-axis data to produce 2d map with color data depicting the 3rd axis


	// Methods for emptying the plot
		void	Clear_Plots(void)	{Deallocate_Plot_Data();}

		data(const data & i_cRHO)
		{
			throw(1); // copy constructor not allowed for now
		}
		data & operator = (const data & i_cRHO)
		{
			throw(1);
		}
		data(void)
		{
			Null_Pointers();
		}
		~data(void)
		{
			if (m_lpszTitle)
				delete [] m_lpszTitle;
			if (m_lpszFilename)
				delete [] m_lpszFilename;

			Deallocate_Plot_Data();

			for (unsigned int uiI = 0; uiI < 16; uiI++)
			{
				if (m_lpdCustom_Stipple[uiI])
					delete [] m_lpdCustom_Stipple[uiI];
				m_uiStipple_Length[uiI] = 0;
			}

			Deallocate_Axis_Data();

			Null_Pointers();
		}
		typedef data DATA;
	};
};


