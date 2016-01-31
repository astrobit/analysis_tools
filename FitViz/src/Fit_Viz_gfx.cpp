#include <FitViz.hpp>
#include <sstream>
#include <line_routines.h>

std::vector<PAIR<double> > g_vRounded_Rectangle_TL;
std::vector<PAIR<double> > g_vRounded_Rectangle_TR;
std::vector<PAIR<double> > g_vRounded_Rectangle_BR;
std::vector<PAIR<double> > g_vRounded_Rectangle_BL;
std::vector<PAIR<double> > g_vEllipse;
GLuint i_uiFilled_RR_List = 0;
GLuint i_uiOutline_RR_List = 0;

void Draw_Rounded_Rectangle(bool i_bFilled)
{
	double	dEdge_Radius = 0.20;
	double	dEdge_Center = 0.50 - dEdge_Radius;
	if (g_vRounded_Rectangle_TL.empty())
	{
		PAIR<double> pOffset(-dEdge_Center,dEdge_Center);
		GenerateArc(dEdge_Radius,dEdge_Radius,90.0,180.0,g_vRounded_Rectangle_TL);
		for (std::vector<PAIR<double> >::iterator cI = g_vRounded_Rectangle_TL.begin(); cI != g_vRounded_Rectangle_TL.end(); cI++)
		{
			cI[0] += pOffset;
		}
	}
	if (g_vRounded_Rectangle_TR.empty())
	{
		PAIR<double> pOffset(dEdge_Center,dEdge_Center);
		GenerateArc(dEdge_Radius,dEdge_Radius,0.0,90.0,g_vRounded_Rectangle_TR);
		for (std::vector<PAIR<double> >::iterator cI = g_vRounded_Rectangle_TR.begin(); cI != g_vRounded_Rectangle_TR.end(); cI++)
		{
			cI[0] += pOffset;
		}
	}
	if (g_vRounded_Rectangle_BR.empty())
	{
		PAIR<double> pOffset(dEdge_Center,-dEdge_Center);
		GenerateArc(dEdge_Radius,dEdge_Radius,270.0,0.0,g_vRounded_Rectangle_BR);
		for (std::vector<PAIR<double> >::iterator cI = g_vRounded_Rectangle_BR.begin(); cI != g_vRounded_Rectangle_BR.end(); cI++)
		{
			cI[0] += pOffset;
		}
	}
	if (g_vRounded_Rectangle_BL.empty())
	{
		PAIR<double> pOffset(-dEdge_Center,-dEdge_Center);
		GenerateArc(dEdge_Radius,dEdge_Radius,180.0,270.0,g_vRounded_Rectangle_BL);
		for (std::vector<PAIR<double> >::iterator cI = g_vRounded_Rectangle_BL.begin(); cI != g_vRounded_Rectangle_BL.end(); cI++)
		{
			cI[0] += pOffset;
		}
	}
	if (i_uiFilled_RR_List == 0)
	{
		i_uiFilled_RR_List = glGenLists(1);
		glNewList(i_uiFilled_RR_List,GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2d(0.0,0.0);
			glVertexList(g_vRounded_Rectangle_TL);
			glVertexList(g_vRounded_Rectangle_TR);
			glVertexList(g_vRounded_Rectangle_BR);
			glVertexList(g_vRounded_Rectangle_BL);
			g_vRounded_Rectangle_TL[0].glVertex();
		glEnd();
		glEndList();
	}
	if (i_uiOutline_RR_List == 0)
	{
		i_uiOutline_RR_List = glGenLists(1);
		glNewList(i_uiOutline_RR_List,GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glVertexList(g_vRounded_Rectangle_TL);
			glVertexList(g_vRounded_Rectangle_TR);
			glVertexList(g_vRounded_Rectangle_BR);
			glVertexList(g_vRounded_Rectangle_BL);
		glEnd();
		glEndList();
	}

	if (i_bFilled && i_uiFilled_RR_List != 0)
	{
		glPushMatrix();
            glTranslated(0.5,-0.5,0.0);
		glCallList(i_uiFilled_RR_List);
        glPopMatrix();
	}
	else if (!i_bFilled && i_uiOutline_RR_List != 0)
	{
		glPushMatrix();
            glTranslated(0.5,-0.5,0.0);
		glCallList(i_uiOutline_RR_List);
        glPopMatrix();
	}
	else
	{
		glPushMatrix();
            glTranslated(1.0,-0.5,0.0);
			if (!i_bFilled)
				glLineWidth(2.0);
			glBegin(i_bFilled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
				if (i_bFilled)
					glVertex2d(0.0,0.0);
	//			glTranslated(-0.95, 0.95,0.0);
				glVertexList(g_vRounded_Rectangle_TL);
	//			glTranslated( 1.90, 0.00,0.0);
				glVertexList(g_vRounded_Rectangle_TR);
	//			glTranslated( 0.00, 1.90,0.0);
				glVertexList(g_vRounded_Rectangle_BR);
	//			glTranslated(-1.90, 0.00,0.0);
				glVertexList(g_vRounded_Rectangle_BL);
	//			glTranslated( 0.00,-1.90,0.0);
				if (i_bFilled)
					g_vRounded_Rectangle_TL[0].glVertex();
			glEnd();
		glPopMatrix();
	}
}
void Draw_Rounded_Rectangle_Button(bool i_bPressed, bool i_bActive)
{
	glPushMatrix();
		glColor4d(0.0,0.0,0.0,0.25);
		if (i_bPressed)
			glTranslated(0.02,-0.04,0.0);
		else
			glTranslated(0.10,-0.20,0.0);
		Draw_Rounded_Rectangle(true);
	glPopMatrix();

	if (i_bActive)
		glColor4d(0.0,1.0,0.0,1.0); // green
	else
		glColor4d(0.0,0.1,0.0,1.0); // green

	Draw_Rounded_Rectangle(true);
	glColor4d(0.0,0.0,0.0,1.0);
	glLineWidth(2.0);
	Draw_Rounded_Rectangle(false);
}
enum SB_DIRECTION {SBD_UP,SBD_DOWN,SBD_LEFT,SBD_RIGHT};

void Draw_Scroll_Button(SB_DIRECTION i_eDirection, bool i_bActive)
{
	glColor4d(0.75,0.75,0.75,1.0);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
		glVertex2d(0.00,0.00);
		glVertex2d(1.0,0.00);
		glVertex2d(1.0,-1.0);
		glVertex2d(0.00,-1.0);
	glEnd();
	if (i_bActive)
		glBegin(GL_TRIANGLES);
	else
		glBegin(GL_LINE_STRIP);

		switch (i_eDirection)
		{
		case SBD_UP:
			glVertex2d(0.0,-1.0);
			glVertex2d(0.5,0.00);
			glVertex2d(1.0,-1.0);
			break;
		case SBD_DOWN:
			glVertex2d(1.0,0.0);
			glVertex2d(0.5,-1.00);
			glVertex2d(0.0,0.0);
			break;
		case SBD_LEFT:
			glVertex2d(1.0,-1.0);
			glVertex2d(0.0,-0.5);
			glVertex2d(1.0, 0.0);
			break;
		case SBD_RIGHT:
			glVertex2d(0.0, 0.0);
			glVertex2d(1.0,-0.5);
			glVertex2d(0.0,-1.0);
			break;
		}
	glEnd();
}
void Draw_Simple_Button(bool bStatus)
{
    glColor4d(0.0,0.25,0.0,1.0);
    glBegin(GL_QUADS);
        glVertex2d(0.0,0.0);
        glVertex2d(1.0,0.0);
        glVertex2d(1.0,-1.0);
        glVertex2d(0.0,-1.0);
    glEnd();
    if (bStatus)
    {
        glColor4d(0.0,1.0,0.0,1.0);
        glLineWidth(2.5);
    }
    else
    {
        glColor4d(0.0,0.0,0.0,1.0);
        glLineWidth(1.0);
    }
    glBegin(GL_LINE_LOOP);
        glVertex2d(0.0,0.0);
        glVertex2d(1.0,0.0);
        glVertex2d(1.0,-1.0);
        glVertex2d(0.0,-1.0);
    glEnd();

}
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
	PAIR<double> pTB_Size = PAIR<double>(0.10,0.10);
	PAIR<double> pIB_Size = PAIR<double>(0.10,0.05);

	dAR = Get_Pane_Aspect_Ratio(m_idPane);
	m_mMain_Pane_Buttons.clear();
	m_mMain_Pane_Buttons[MODEL_SELECT_UP] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.1,0.20),pSB_Size,MODEL_SELECT_UP);
	m_mMain_Pane_Buttons[MODEL_SELECT_DOWN] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.1,0.10),pSB_Size,MODEL_SELECT_DOWN);
	m_mMain_Pane_Buttons[MODEL_SEL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.03,0.10),pTB_Size,MODEL_SEL_TEXT);
	m_mMain_Pane_Buttons[DAY_SELECT_UP] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.3,0.20),pSB_Size,DAY_SELECT_UP);
	m_mMain_Pane_Buttons[DAY_SELECT_DOWN] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.3,0.10),pSB_Size,DAY_SELECT_DOWN);
	m_mMain_Pane_Buttons[DAY_SEL_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.21,0.10),pTB_Size,DAY_SEL_TEXT);
	m_mMain_Pane_Buttons[METHOD_FLAT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.4,0.20),pLB_Size,METHOD_FLAT);
	m_mMain_Pane_Buttons[METHOD_JEFF] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.4,0.10),pLB_Size,METHOD_JEFF);
	m_mMain_Pane_Buttons[SELECT_DIRECTORY] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.5,0.20),pLB_Size,SELECT_DIRECTORY);
	m_mMain_Pane_Buttons[QUIT_REQUEST] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.90,0.20),pLB_Size,QUIT_REQUEST);
	m_mMain_Pane_Buttons[MODEL_DISPLAY_AREA] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.05,0.30),PAIR<double>(dAR * 0.9,0.70),MODEL_DISPLAY_AREA);
	m_mMain_Pane_Buttons[PVF_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.20),pIB_Size,PVF_TEXT);
	m_mMain_Pane_Buttons[HVF_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.15),pIB_Size,HVF_TEXT);
	m_mMain_Pane_Buttons[pEW_TEXT] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.6,0.10),pIB_Size,pEW_TEXT);

}
void FIT_VIZ_MAIN::gfx_close(void) // graphics exiting; rendering context still active
{
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
		glColor4d(0.5,0.5,0.5,1.0);
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
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.7,0.0,0.0,0.0,sModel_ID.str().c_str(),HJ_CENTER,VJ_BOTTOM);
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
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.7,0.0,0.0,0.0,sDay.str().c_str(),HJ_CENTER,VJ_BOTTOM);
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
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.7,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
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
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.7,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case pEW_TEXT:
					glColor4d(0.0,0.0,0.0,1.0);
					glPushMatrix();
						{
							std::ostringstream sText;
							sText << "pEW ";
							sText << m_dFit_pEW;
							sText << " Ang.";
							glTranslated(0.0,0.0,0.0);
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.7,0.0,0.0,0.0,sText.str().c_str(),HJ_LEFT,VJ_BOTTOM);
						}
					glPopMatrix();
					break;
				case MODEL_DISPLAY_AREA:
					if (!m_vWavelength.empty() && !m_vFlux.empty())
					{
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
						glEnd();
				
						glColor4d(0.0,0.0,0.0,1.0);
						for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
						{
							double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
							double dY = m_vFlux[uiI] * dFlux_Scale;
							glVertex2d(dX,dY);
						}
						glEnd();
						glColor4d(0.0,0.0,1.0,1.0);
						glBegin(GL_LINE_STRIP);
						GAUSS_FIT_PARAMETERS * lpgfpParamters = &g_cgfpCaNIR;
						for (unsigned int uiI = 0; uiI < m_vWavelength.size(); uiI++)
						{
							double dX = (m_vWavelength[uiI] - m_vWavelength[0]) * dWL_scale;
							double dY;
							if (m_eFit_Method == fm_flat)
								dY = (1.0 + Multi_Gaussian(m_vWavelength[uiI],m_vGaussian_Fit_Data,lpgfpParamters).Get(0)) * dFlux_Scale;
							else
							{
								dY = Multi_Gaussian(m_vWavelength[uiI],m_vGaussian_Fit_Data,lpgfpParamters).Get(0);
								dY += (m_vWavelength[uiI] - m_dJeff_WL) * m_dJeff_Slope + m_dJeff_Flux;
								dY *= dFlux_Scale;
							}
							glVertex2d(dX,dY);
						}
						glEnd();
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
						else
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
					}
					break;
				}
			glPopMatrix();
		}
	}
}

