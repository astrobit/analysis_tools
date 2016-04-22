#include <FitViz.hpp>
#include <sstream>
#include <line_routines.h>

void FIT_VIZ_MAIN::gfx_init(void) // initialization routine; rendering context already created
{
	glClearColor(1.0,1.0,1.0,0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialf(GL_FRONT,GL_SPECULAR,10.0);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	SelectFontFace(SANS,true,false);
}
void FIT_VIZ_MAIN::gfx_reshape(const PAIR<unsigned int> & i_tNew_Size) // window size change
{
	PAIR<unsigned int> m_tViewport_Size = i_tNew_Size;
	glViewport(0,0,(GLsizei) m_tViewport_Size.m_tX, (GLsizei) m_tViewport_Size.m_tY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,m_tViewport_Size.m_tX / m_tViewport_Size.m_tY,0.0,1.0,0.0,1.0);
	glMatrixMode(GL_MODELVIEW);

	PAIR<unsigned int> tWindow_BL(0,0);
	PAIR<unsigned int> tWindow_TR((unsigned int)m_tViewport_Size.m_tX,(unsigned int)(m_tViewport_Size.m_tY));
	PAIR<unsigned int> tError_BL = tWindow_TR * 0.25;
	PAIR<unsigned int> tError_TR = tWindow_TR * 0.75;

	Move_Pane(m_idPane,QUAD<unsigned int> (tWindow_BL,tWindow_TR));
	Move_Pane(m_idError_Pane,QUAD<unsigned int> (tError_BL,tError_TR));

	double dAR = Get_Pane_Aspect_Ratio(m_idError_Pane);
	m_mError_Pane_Buttons.clear();
	m_mError_Pane_Buttons[ERROR_ACK] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.5 - 0.05,0.20),PAIR<double>(0.14,0.07),ERROR_ACK);
	PAIR<double> pSB_Size = PAIR<double>(0.05,0.05);
	PAIR<double> pLB_Size = PAIR<double>(0.15,0.05);
	PAIR<double> pSmB_Size = PAIR<double>(0.05,0.05);
	PAIR<double> pTB_Size = PAIR<double>(0.10,0.10);
	PAIR<double> pIB_Size = PAIR<double>(0.10,0.04);
	PAIR<double> pIB_Size_Sml = PAIR<double>(0.10,0.03);

	dAR = Get_Pane_Aspect_Ratio(m_idPane);
	m_mMain_Pane_Buttons.clear();
	m_mMain_Pane_Buttons[MODEL_SELECT_UP] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.25,0.20),pSB_Size,MODEL_SELECT_UP);
	m_mMain_Pane_Buttons[MODEL_SELECT_DOWN] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.25,0.10),pSB_Size,MODEL_SELECT_DOWN);
	m_mMain_Pane_Buttons[MODEL_SEL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.18,0.10),pTB_Size,MODEL_SEL_TEXT);
	m_mMain_Pane_Buttons[DAY_SELECT_UP] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.37,0.20),pSB_Size,DAY_SELECT_UP);
	m_mMain_Pane_Buttons[DAY_SELECT_DOWN] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.37,0.10),pSB_Size,DAY_SELECT_DOWN);
	m_mMain_Pane_Buttons[DAY_SEL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.30,0.10),pTB_Size,DAY_SEL_TEXT);
	m_mMain_Pane_Buttons[METHOD_FLAT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.42,0.29),pLB_Size,METHOD_FLAT);
	m_mMain_Pane_Buttons[METHOD_JEFF] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.42,0.23),pLB_Size,METHOD_JEFF);
	m_mMain_Pane_Buttons[METHOD_MANUAL] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.42,0.17),pLB_Size,METHOD_MANUAL);
	m_mMain_Pane_Buttons[AUTO_FIT_TEST] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.50	,0.17),pLB_Size,AUTO_FIT_TEST);


//	m_mMain_Pane_Buttons[SELECT_DIRECTORY] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.5,0.20),pLB_Size,SELECT_DIRECTORY);
	m_mMain_Pane_Buttons[DISPLAY_SHELL_COMPONENT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.50,0.29),pLB_Size,DISPLAY_SHELL_COMPONENT);
	m_mMain_Pane_Buttons[DISPLAY_EJECTA_COMPONENT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.50,0.23),pLB_Size,DISPLAY_EJECTA_COMPONENT);
	m_mMain_Pane_Buttons[QUIT_REQUEST] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.90,0.10),pLB_Size,QUIT_REQUEST);
	m_mMain_Pane_Buttons[EXPORT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.90,0.25),pLB_Size,EXPORT);
	m_mMain_Pane_Buttons[MODEL_DISPLAY_AREA] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.05,1.00),PAIR<double>(dAR * 0.9,0.70),MODEL_DISPLAY_AREA);
	m_mMain_Pane_Buttons[PVF_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.25),pIB_Size,PVF_TEXT);
	m_mMain_Pane_Buttons[HVF_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.20),pIB_Size,HVF_TEXT);
	m_mMain_Pane_Buttons[MAN_FIT_RED_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.155),pIB_Size_Sml,MAN_FIT_RED_TEXT);
	m_mMain_Pane_Buttons[MAN_FIT_BLUE_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.110),pIB_Size_Sml,MAN_FIT_BLUE_TEXT);
	m_mMain_Pane_Buttons[MAN_FIT_CENTER_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.065),pIB_Size_Sml,MAN_FIT_CENTER_TEXT);
	m_mMain_Pane_Buttons[MAN_FIT_CENTER_SECOND_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.02),pIB_Size_Sml,MAN_FIT_CENTER_SECOND_TEXT);



	m_mMain_Pane_Buttons[pEW_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.73,0.25),pIB_Size,pEW_TEXT);
	m_mMain_Pane_Buttons[MAN_FIT_QUALITY_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.73,0.20),pIB_Size,MAN_FIT_QUALITY_TEXT);
	m_mMain_Pane_Buttons[SHELL_pEW_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.73,0.15),pIB_Size,SHELL_pEW_TEXT);
	m_mMain_Pane_Buttons[EJECTA_pEW_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.73,0.10),pIB_Size,EJECTA_pEW_TEXT);
	m_mMain_Pane_Buttons[TOTAL_pEW_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.73,0.05),pIB_Size,TOTAL_pEW_TEXT);

	m_mMain_Pane_Buttons[PS_TEMP_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01,0.26),pIB_Size,PS_TEMP_TEXT);
	m_mMain_Pane_Buttons[EJECTA_SCALAR_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01,0.22),pIB_Size,EJECTA_SCALAR_TEXT);
	m_mMain_Pane_Buttons[SHELL_SCALAR_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01,0.16),pIB_Size,SHELL_SCALAR_TEXT);
	m_mMain_Pane_Buttons[EJECTA_PL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01,0.11),pIB_Size,EJECTA_PL_TEXT);
	m_mMain_Pane_Buttons[SHELL_PL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01,0.06),pIB_Size,SHELL_PL_TEXT);
	m_mMain_Pane_Buttons[REF_MODEL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.01	,0.01),pIB_Size,REF_MODEL_TEXT);

	m_mMain_Pane_Buttons[MAN_FIT_RED] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.42	,0.11),pSmB_Size,MAN_FIT_RED);
	m_mMain_Pane_Buttons[MAN_FIT_BLUE] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.46	,0.11),pSmB_Size,MAN_FIT_BLUE);
	m_mMain_Pane_Buttons[MAN_FIT_CENTER] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.50	,0.11),pSmB_Size,MAN_FIT_CENTER);
	m_mMain_Pane_Buttons[MAN_FIT_CENTER_SECOND] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.54	,0.11),pSmB_Size,MAN_FIT_CENTER_SECOND);
	m_mMain_Pane_Buttons[MAN_FIT_EXEC] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.42	,0.05),pLB_Size,MAN_FIT_EXEC);
	m_mMain_Pane_Buttons[MAN_FIT_CLEAR] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.50	,0.05),pLB_Size,MAN_FIT_CLEAR);
}
void FIT_VIZ_MAIN::gfx_close(void) // graphics exiting; rendering context still active
{
}

void FIT_VIZ_MAIN::Display_Man_Select_Item(unsigned int i_uiIdx, man_select_mode i_eMode, const double & i_dSize, const PAIR<double> & i_pdSize)
{
	if (i_uiIdx != -1)
	{
		double dWL_scale = 0.90 / (m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]);
		double dFlux_Scale = 0.90 / 2.0;//(m_dMax_Flux * 1.25);

		switch (i_eMode)
		{
		case MS_BLUE:
			glColor4d(0.0,0.0,1.0,1.0);
			break;
		case MS_RED:
			glColor4d(1.0,0.0,0.0,1.0);
			break;
		case MS_CENTER:
			glColor4d(1.0,1.0,0.0,1.0);
			break;
		case MS_CENTER_SECOND:
			glColor4d(0.5,0.0,1.0,1.0);
			break;
		case MS_OFF:
			glColor4d(0.75,0.75,0.75,1.0);
			break;
		}
		glPushMatrix();
			glTranslated((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
			glScaled(1.0/i_dSize,1.0,1.0);
			glScaled(0.01,0.01,0.0);
			glBegin(GL_TRIANGLE_FAN);
				glVertex2d(0.0,0.0);
				glVertexList(g_vEllipse);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glBegin(GL_LINES);
				glVertex3d((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
				glVertex3d((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,0.0, 0.0);
			glEnd();
		glPopMatrix();

		// residuals
		if (m_vFit_Residuals.size() > 0)
		{
			glPushMatrix();
				glTranslated(0.0,0.75,0.0);
				glScaled(1.0,0.25,1.0);
				glBegin(GL_LINES);
					double dX = (m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale;
					double dY = m_vFit_Residuals[i_uiIdx] * dFlux_Scale;
					glVertex2d(dX,dY);
					glVertex2d(dX,-dFlux_Scale);
				glEnd();
				glTranslated(dX + dWL_scale * 5.0,-dFlux_Scale, 0.0);
				glScaled(1.0/i_pdSize.m_tX,1.0/i_pdSize.m_tY,1.0); // undo button scaling
				glScaled(1.0/i_dSize,1.0,1.0); // adjust for aspect ratio
				std::ostringstream sTextResid;
				sTextResid << m_vFit_Residuals[i_uiIdx];
				glPrintJustified(i_pdSize.m_tY * 0.05,0.0,0.0,0.0,sTextResid.str().c_str(),HJ_LEFT,VJ_MIDDLE);
			glPopMatrix();
		}

		glPushMatrix();
			glBegin(GL_LINES);
				glVertex3d((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
				glVertex3d((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,0.0, 0.0);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslated((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,0.0, 0.0);
			glScaled(1.0/i_pdSize.m_tX,1.0/i_pdSize.m_tY,1.0); // undo button scaling
			glScaled(1.0/i_dSize,1.0,1.0); // adjust for aspect ratio
			std::ostringstream sText;
			sText << m_vWavelength[i_uiIdx];
			glPrintJustified(i_pdSize.m_tY * 0.05,0.0,0.0,0.0,sText.str().c_str(),HJ_CENTER,VJ_TOP);
		glPopMatrix();
		if (m_eMan_Select_Mode == i_eMode)
		{
			glPushMatrix();
				glBegin(GL_LINES);
					glVertex3d((m_vWavelength[i_uiIdx] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
					glVertex3d((m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
				glEnd();
			glPopMatrix();
			glPushMatrix();
				glTranslated((m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]) * dWL_scale,m_vFlux[i_uiIdx] * dFlux_Scale, 0.0);
				glScaled(1.0/i_pdSize.m_tX,1.0/i_pdSize.m_tY,1.0); // undo button scaling
				glScaled(1.0/i_dSize,1.0,1.0); // adjust for aspect ratio
				std::ostringstream sText;
				sText << m_vFlux[i_uiIdx];
				glPrintJustified(i_pdSize.m_tY * 0.05,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_MIDDLE);
			glPopMatrix();
		}
	}
}
void FIT_VIZ_MAIN::gfx_display(pane_id i_idPane) // primary display routine
{
	if (g_vEllipse.empty())
	{
		GenerateEllipse(1.0,1.0,g_vEllipse);
	}
	if (i_idPane == m_idError_Pane)
	{
		double dSize = Get_Pane_Aspect_Ratio(m_idError_Pane);
		unsigned int uiNum_Lines = m_vsError_Info.size();
		double	dHeight = uiNum_Lines * 0.07 + 0.37;
		glColor4d(0.5,0.5,0.5,1.0);
		glBegin(GL_QUADS);
			glVertex2d(0.0,dHeight);
			glVertex2d(dSize,dHeight);
			glVertex2d(dSize,0.0);
			glVertex2d(0.0,0.0);
		glEnd();
		glColor4d(0.0,0.0,0.0,1.0);
		double dText_Y = 0.27 + uiNum_Lines * 0.07;
		for (std::vector<std::string>::const_iterator cI = m_vsError_Info.begin(); cI != m_vsError_Info.end(); cI++)
		{
			glPrintJustified(0.05,dSize * 0.5,dText_Y,0.0,(*cI).c_str(),HJ_CENTER,VJ_BOTTOM);
			dText_Y -= 0.07;
		}
		for (std::map<button_id,BUTTON_INFO>::const_iterator cI = m_mError_Pane_Buttons.begin(); cI != m_mError_Pane_Buttons.end(); cI++)
		{
			PAIR<double> pdPosition = cI->second.GetPosition();
			PAIR<double> pdSize = cI->second.GetSize();

			glPushMatrix();
				glTranslated(pdPosition.m_tX,pdPosition.m_tY,0.0);
				glScaled(pdSize.m_tX,pdSize.m_tY,1.0);
				int iStatus = cI->second.GetStatus();
				switch (cI->second.GetID())
				{
				case ERROR_ACK:
					glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.7,0.0,0.0,0.0,"OK",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				}
			glPopMatrix();
		}
	}
	else 
	{
		double dSize = Get_Pane_Aspect_Ratio(m_idPane);
		glColor4d(0.375,0.375,0.375,1.0);
		glBegin(GL_QUADS);
			glVertex2d(0.0,0.3);
			glVertex2d(dSize,0.3);
			glVertex2d(dSize,0.0);
			glVertex2d(0.0,0.0);
		glEnd();
		glColor4d(0.0,0.0,0.0,1.0);
		for (std::map<button_id,BUTTON_INFO>::const_iterator cI = m_mMain_Pane_Buttons.begin(); cI != m_mMain_Pane_Buttons.end(); cI++)
		{
			PAIR<double> pdPosition = cI->second.GetPosition();
			PAIR<double> pdSize = cI->second.GetSize();

			glPushMatrix();
				glTranslated(pdPosition.m_tX,pdPosition.m_tY,0.0);
				glScaled(pdSize.m_tX,pdSize.m_tY,1.0);
				int iStatus = cI->second.GetStatus();
				switch (cI->second.GetID())
				{
				case MODEL_SELECT_UP:
					Draw_Scroll_Button(SBD_UP, !m_vModel_List.empty() && m_uiSelected_Model < (m_vModel_List.size() - 1));
					break;
				case MODEL_SELECT_DOWN:
					Draw_Scroll_Button(SBD_DOWN, !m_vModel_List.empty() && m_uiSelected_Model > 0);
					break;
				case DAY_SELECT_UP:
					Draw_Scroll_Button(SBD_UP, !m_vDay_List.empty() && m_uiSelected_Day < (m_vDay_List.size() - 1));
					break;
				case DAY_SELECT_DOWN:
					Draw_Scroll_Button(SBD_DOWN, !m_vDay_List.empty() && m_uiSelected_Day > 0);
					break;
				case METHOD_FLAT:
					if (m_eFit_Method == fm_flat)
						glColor4d(0.0,0.75,0.0,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Flat",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case METHOD_JEFF:
					if (m_eFit_Method == fm_jeff)
						glColor4d(0.0,0.75,0.0,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Jeff",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case METHOD_MANUAL:
					if (m_eFit_Method == fm_manual)
						glColor4d(0.0,0.75,0.0,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Manual",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case AUTO_FIT_TEST:
					if (m_eFit_Method == fm_auto)
						glColor4d(0.0,0.75,0.0,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Auto",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case MAN_FIT_EXEC:
					if (m_eFit_Method == fm_manual || m_eFit_Method == fm_auto)
					{
						if (m_uiManual_Fit_Blue_Idx != -1 && m_uiManual_Fit_Red_Idx != -1 && m_uiManual_Fit_Central_Idx != -1)
							glColor4d(0.0,0.75,0.0,1.0);
						else
							glColor4d(0.0,0.25,0.0,1.0);
					}
					else
						glColor4d(0.25,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Fit",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case MAN_FIT_CLEAR:
					if (m_eFit_Method == fm_manual || m_eFit_Method == fm_auto)
					{
						if (m_uiManual_Fit_Blue_Idx != -1 || m_uiManual_Fit_Red_Idx != -1 || m_uiManual_Fit_Central_Idx != -1)
							glColor4d(0.0,0.75,0.0,1.0);
						else
							glColor4d(0.0,0.25,0.0,1.0);
					}
					else
						glColor4d(0.25,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Clear",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case MAN_FIT_RED:
					if (m_eFit_Method == fm_manual || m_eFit_Method == fm_auto)
						glColor4d(0.75,0.0,0.0,1.0);
					else
						glColor4d(0.06125,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					break;
				case MAN_FIT_CENTER:
					if (m_eFit_Method == fm_manual || m_eFit_Method == fm_auto)
						glColor4d(0.75,0.75,0.0,1.0);
					else
						glColor4d(0.06125,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					break;
				case MAN_FIT_CENTER_SECOND:
					if (m_eFit_Method == fm_manual)
						glColor4d(0.375,0.0,0.75,1.0);
					else
						glColor4d(0.06125,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					break;
				case MAN_FIT_BLUE:
					if (m_eFit_Method == fm_manual)
						glColor4d(0.0,0.0,0.75,1.0);
					else
						glColor4d(0.06125,0.25,0.25,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					break;
				case SELECT_DIRECTORY:
					glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Select",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case DISPLAY_SHELL_COMPONENT:
					if (m_bDisplay_Shell_Component)
						glColor4d(0.75,0.00,075,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Shell",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case DISPLAY_EJECTA_COMPONENT:
					if (m_bDisplay_Ejecta_Component)
						glColor4d(0.0,0.75,0.75,1.0);
					else
						glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Ejecta",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case QUIT_REQUEST:
					glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Quit",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case EXPORT:
					glColor4d(0.75,0.75,0.75,1.0);
					Draw_Rounded_Rectangle(true);
					glColor4d(0.0,0.0,0.0,1.0);
					glLineWidth(2.0);
					Draw_Rounded_Rectangle(false);
					glPushMatrix();
						glTranslated(0.5,-0.5,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.4,0.0,0.0,0.0,"Export",HJ_CENTER,VJ_MIDDLE);
					glPopMatrix();
					break;
				case MODEL_SEL_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						glTranslated(0.5,1.0,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.35,0.0,0.0,0.0,"Model",HJ_CENTER,VJ_BOTTOM);
					glPopMatrix();
					glPushMatrix();
						{
							std::ostringstream sModel_ID;
							if (m_vModel_List.empty())
								sModel_ID << "--";
							else
								sModel_ID << m_vModel_List[m_uiSelected_Model];
							glTranslated(0.5,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sModel_ID.str().c_str(),HJ_CENTER,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case DAY_SEL_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						glTranslated(0.5,1.0,0.0);
						glScaled(1.0/dSize,1.0,1.0);
						glPrintJustified(0.35,0.0,0.0,0.0,"Day",HJ_CENTER,VJ_BOTTOM);
					glPopMatrix();
					glPushMatrix();
						{
							std::ostringstream sDay;
							if (m_vDay_List.empty())
								sDay << "--";
							else
								sDay << m_vDay_List[m_uiSelected_Day];
							glTranslated(0.5,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sDay.str().c_str(),HJ_CENTER,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case PVF_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "PVF ";
							if (isnan(m_dFit_Velocity_PVF))
								sText << "--";
							else
								sText << (m_dFit_Velocity_PVF * 1.0e-3);
							sText << " kkm/s";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case HVF_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "HVF ";
							if (isnan(m_dFit_Velocity_HVF))
								sText << "--";
							else
								sText << (m_dFit_Velocity_HVF * 1.0e-3);
							sText << " kkm/s";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MAN_FIT_RED_TEXT:
					glColor4d(1.0,0.0,0.0,1.0);
					glPushMatrix();
						if (m_uiManual_Fit_Red_Idx != -1)
						{
							std::ostringstream sText;
							sText << m_vWavelength[m_uiManual_Fit_Red_Idx];
							sText << " Ang";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MAN_FIT_BLUE_TEXT:
					glColor4d(0.0,0.0,1.0,1.0);
					glPushMatrix();
						if (m_uiManual_Fit_Blue_Idx != -1)
						{
							std::ostringstream sText;
							sText << m_vWavelength[m_uiManual_Fit_Blue_Idx];
							sText << " Ang";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MAN_FIT_CENTER_TEXT:
					glColor4d(1.0,1.0,0.0,1.0);
					glPushMatrix();
						if (m_uiManual_Fit_Central_Idx != -1)
						{
							std::ostringstream sText;
							sText << m_vWavelength[m_uiManual_Fit_Central_Idx];
							sText << " Ang";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MAN_FIT_CENTER_SECOND_TEXT:
					glColor4d(0.5,0.0,1.0,1.0);
					glPushMatrix();
						if (m_uiManual_Fit_Central_Second_Idx != -1)
						{
							std::ostringstream sText;
							sText << m_vWavelength[m_uiManual_Fit_Central_Second_Idx];
							sText << " Ang";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case pEW_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Fit pEW ";
							sText << m_dFit_pEW;
							sText << " Ang.";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MAN_FIT_QUALITY_TEXT:
					if (m_eFit_Method == fm_manual && m_vFlux_Fit.size() > 0)
					{
						glColor4d(0.0,0.0,0.0,1.0);
						glPushMatrix();
							{
								std::ostringstream sText;
								sText << "Fit Qual ";
								sText.setf( std::ios_base::scientific, std::ios_base::floatfield );
								sText << g_dFit_Results_Smin;
								glTranslated(0.0,0.0,0.0);
								glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
								glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
								glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
							}
						glPopMatrix();
					}
					break;
				case EJECTA_pEW_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Ejecta pEW ";
							if (m_dEjecta_Flat_pEW == -1)
								sText << "--";
							else
								sText << m_dEjecta_Flat_pEW;
							sText << " Ang.";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case SHELL_pEW_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Shell pEW ";
							if (m_dShell_Flat_pEW == -1)
								sText << "--";
							else
								sText << m_dShell_Flat_pEW;
							sText << " Ang.";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case TOTAL_pEW_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Total pEW ";
							if (m_dTotal_Flat_pEW == -1)
								sText << "--";
							else
								sText << m_dTotal_Flat_pEW;
							sText << " Ang.";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case PS_TEMP_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "PS Temp: ";
							if (m_dPS_Temp == -1)
								sText << "--";
							else
								sText << m_dPS_Temp;
							sText << " K";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case EJECTA_SCALAR_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Ejecta Scalar: ";
							sText << m_dEjecta_Scalar;
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case SHELL_SCALAR_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Shell Scalar: ";
							sText << m_dShell_Scalar;
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case EJECTA_PL_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Ejecta Power Law: ";
							sText << m_dEjecta_Power_Law;
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case SHELL_PL_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Shell Power Law: ";
							sText << m_dShell_Power_Law;
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case REF_MODEL_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "Ref Model: ";
							sText << m_szRef_Model;
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
							glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
							glPrintJustified(pdSize.m_tY * 0.9,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MODEL_DISPLAY_AREA:
					if (g_bPerform_Fit && m_bFlasher_1s_50p)
					{
						glPushMatrix();
								glTranslated(0.0,0.0,0.0);
								glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0); // undo button scaling
								glScaled(1.0/dSize,1.0,1.0); // adjust for aspect ratio
								glPrintJustified(pdSize.m_tY * 0.04,0.0,0.0,0.0,"Fitting",HJ_LEFT,VJ_TOP);
						glPopMatrix();
					}
				
					if (!m_vWavelength.empty() && !m_vFlux.empty())
					{
						glTranslated(0.0,-1.0,0.0);
						glColor4d(0.0,0.0,0.0,1.0);
						glPushMatrix();
							glTranslated(0.05,0.1,0.0);
							std::ostringstream sWL;
							sWL << m_vWavelength[0];
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.05,0.0,-0.01,0.0,sWL.str().c_str(),HJ_CENTER,VJ_TOP);
						glPopMatrix();
						glPushMatrix();
							glTranslated(0.95,0.1,0.0);
							std::ostringstream sWL2;
							sWL2 << m_vWavelength[m_vWavelength.size() - 1];
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.05,0.0,-0.01,0.0,sWL2.str().c_str(),HJ_CENTER,VJ_TOP);
						glPopMatrix();
						double dWL_scale = 0.90 / (m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]);
						double dFlux_Scale = 0.90 / 2.0;//(m_dMax_Flux * 1.25);
						glTranslated(0.05,0.1,0.0);
						glBegin(GL_LINE_STRIP);
						for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
						{
							double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
							double dY = m_vFlux[uiI] * dFlux_Scale;
							glVertex2d(dX,dY);
						}
						glEnd();
						glColor4d(0.75,0.75,0.75,1.0);
						glBegin(GL_LINES);
							glVertex2d(0.0,0.0);
							glVertex2d(0.9,0.0);
							glVertex2d(0.0,dFlux_Scale);
							glVertex2d(0.9,dFlux_Scale);
							glVertex2d(0.0,0.0);
							glVertex2d(0.0,2.0*dFlux_Scale);
							glVertex2d(0.9,0.0);
							glVertex2d(0.9,2.0*dFlux_Scale);
						glEnd();
				
						if (m_bDisplay_Shell_Component && !m_bNo_Shell)
						{
							glColor4d(1.0,0.0,1.0,1.0);
							glBegin(GL_LINE_STRIP);
							for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
							{
								double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
								double dY = m_vFlux_Shell[uiI] * dFlux_Scale;
								glVertex2d(dX,dY);
							}
							glEnd();
						}
						if (m_bDisplay_Ejecta_Component)
						{
							glColor4d(0.0,1.0,1.0,1.0);
							glBegin(GL_LINE_STRIP);
							for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
							{
								double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
								double dY = m_vFlux_Ejecta[uiI] * dFlux_Scale;
								glVertex2d(dX,dY);
							}
							glEnd();
						}
						glColor4d(0.0,0.0,0.0,1.0);
						for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
						{
							double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
							double dY = m_vFlux[uiI] * dFlux_Scale;
							glVertex2d(dX,dY);
						}
						glEnd();
						if (m_vFlux_Fit.size() > 0)
						{
							glColor4d(0.0,0.0,1.0,1.0);
							glBegin(GL_LINE_STRIP);
//							GAUSS_FIT_PARAMETERS * lpgfpParamters = &g_cgfpCaNIR;
							for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
							{
								double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
								double dY = m_vFlux_Fit[uiI] * dFlux_Scale;
//								if (m_eFit_Method == fm_flat)
//									dY = (1.0 + Multi_Gaussian(m_vWavelength[uiI],m_vGaussian_Fit_Data,lpgfpParamters).Get(0)) * dFlux_Scale;
//								else if (m_eFit_Method == fm_jeff)
//								{
//									dY = Multi_Gaussian(m_vWavelength[uiI],m_vGaussian_Fit_Data,lpgfpParamters).Get(0);
//									dY += (m_vWavelength[uiI] - m_dJeff_WL) * m_dJeff_Slope + m_dJeff_Flux;
//									dY *= dFlux_Scale;
//								}
//								else if (m_eFit_Method == fm_manual)
//								{
//									dY = Multi_Gaussian(m_vWavelength[uiI],m_vManual_Fit_Data,lpgfpParamters).Get(0);
//									dY += (m_vWavelength[uiI] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux;
//									dY *= dFlux_Scale;
//								}
								glVertex2d(dX,dY);
							}
							glEnd();
							// draw residuals
							glPushMatrix();
							glTranslated(0.0,0.75,0.0);
							glScaled(1.0,0.25,1.0);
							glColor4d(0.0,0.0,0.0,1.0);
							glBegin(GL_LINES);
								glVertex2d(0.0,0.0);
								glVertex2d((m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]) * dWL_scale,0.0);
								glVertex2d(0.0,-dFlux_Scale);
								glVertex2d(0.0,dFlux_Scale);
								glVertex2d((m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]) * dWL_scale,-dFlux_Scale);
								glVertex2d((m_vWavelength[m_vWavelength.size() - 1] - m_vWavelength[0]) * dWL_scale,dFlux_Scale);
							glEnd();
							glColor4d(0.0,0.75,0.75,1.0);
							glBegin(GL_LINE_STRIP);
							for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
							{
								double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
								double dY = m_vFit_Residuals[uiI] * dFlux_Scale;
								glVertex2d(dX,dY);
							}
							glEnd();
							glPopMatrix();
						}
						glColor4d(1.0,1.0,0.0,1.0);
						glPushMatrix();
							glTranslated((m_pGlobal_Min.m_tX - m_vWavelength[0]) * dWL_scale,m_pGlobal_Min.m_tY * dFlux_Scale, 0.0);
							glScaled(1.0/dSize,1.0,1.0);
							glScaled(0.01,0.01,0.0);
							glBegin(GL_LINE_LOOP);
								glVertexList(g_vEllipse);
							glEnd();
						glPopMatrix();
						if (m_eFit_Method == fm_jeff)
						{
							glColor4d(1.0,0.0,0.0,1.0);
							glPushMatrix();
								glTranslated((m_pJeff_Blue.m_tX - m_vWavelength[0]) * dWL_scale,m_pJeff_Blue.m_tY * dFlux_Scale, 0.0);
								glScaled(1.0/dSize,1.0,1.0);
								glScaled(0.01,0.01,0.0);
								glBegin(GL_LINE_LOOP);
									glVertexList(g_vEllipse);
								glEnd();
							glPopMatrix();
							glPushMatrix();
								glTranslated((m_pJeff_Red.m_tX - m_vWavelength[0]) * dWL_scale,m_pJeff_Red.m_tY * dFlux_Scale, 0.0);
								glScaled(1.0/dSize,1.0,1.0);
								glScaled(0.01,0.01,0.0);
								glBegin(GL_LINE_LOOP);
									glVertexList(g_vEllipse);
								glEnd();
							glPopMatrix();
							glBegin(GL_LINES);
								glVertex3d(0.0,((m_vWavelength[0] - m_dJeff_WL) * m_dJeff_Slope + m_dJeff_Flux) * dFlux_Scale, 0.0);
								glVertex3d(0.9,((m_vWavelength[m_vWavelength.size() - 1] - m_dJeff_WL) * m_dJeff_Slope + m_dJeff_Flux) * dFlux_Scale, 0.0);
							glEnd();
						}
						else if (m_eFit_Method == fm_flat)
						{
							glColor4d(1.0,0.0,0.0,1.0);
							glPushMatrix();
								glTranslated((m_pP_Cygni_Emission_Edge.m_tX - m_vWavelength[0]) * dWL_scale,m_pP_Cygni_Emission_Edge.m_tY * dFlux_Scale, 0.0);
								glScaled(1.0/dSize,1.0,1.0);
								glScaled(0.01,0.01,0.0);
								glBegin(GL_LINE_LOOP);
									glVertexList(g_vEllipse);
								glEnd();
							glPopMatrix();
						}
						else if (m_eFit_Method == fm_manual)
						{
							if (m_uiManual_Fit_Blue_Idx != -1 && m_uiManual_Fit_Red_Idx != -1)
							{
								glColor4d(1.0,0.0,0.0,1.0);
								glBegin(GL_LINES);
									glVertex3d(0.0,((m_vWavelength[0] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux) * dFlux_Scale, 0.0);
									glVertex3d(0.9,((m_vWavelength[m_vWavelength.size() - 1] - m_dMF_WL) * m_dMF_Slope + m_dMF_Flux) * dFlux_Scale, 0.0);
								glEnd();
							}
							Display_Man_Select_Item(m_uiManual_Fit_Blue_Idx,MS_BLUE,dSize,pdSize);
							Display_Man_Select_Item(m_uiManual_Fit_Red_Idx,MS_RED,dSize,pdSize);
							Display_Man_Select_Item(m_uiManual_Fit_Central_Idx,MS_CENTER,dSize,pdSize);
							Display_Man_Select_Item(m_uiManual_Fit_Central_Second_Idx,MS_CENTER_SECOND,dSize,pdSize);

						}
						Display_Man_Select_Item(m_uiInfo_Idx,MS_OFF,dSize,pdSize);
					}
					break;
				}
			glPopMatrix();
		}
	}
}

