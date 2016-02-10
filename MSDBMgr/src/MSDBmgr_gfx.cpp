#include <MSDBmgr.hpp>
#include <sstream>

void MSDB_MGR_MAIN::gfx_init(void) // initialization routine; rendering context already created
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
void MSDB_MGR_MAIN::gfx_reshape(const PAIR<unsigned int> & i_tNew_Size) // window size change
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

	double dAR = Get_Pane_Aspect_Ratio(m_idPane);
	PAIR<double> pIB_Size = PAIR<double>(0.10,0.04);
	PAIR<double> pDisp_Size = PAIR<double>(dAR,0.70);
	m_mMain_Pane_Buttons.clear();
	m_mMain_Pane_Buttons[TXT_CURR_SPEC_ID] 	= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.05,0.30),pIB_Size,TXT_CURR_SPEC_ID);
	m_mMain_Pane_Buttons[TXT_MODEL_ID] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.05,0.25),pIB_Size,TXT_MODEL_ID);
	m_mMain_Pane_Buttons[TXT_ION] 			= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.05,0.20),pIB_Size,TXT_ION);
	m_mMain_Pane_Buttons[TXT_PS_VEL] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.15,0.30),pIB_Size,TXT_PS_VEL);
	m_mMain_Pane_Buttons[TXT_PS_TEMP] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.15,0.25),pIB_Size,TXT_PS_TEMP);
	m_mMain_Pane_Buttons[TXT_EJECTA_SCALAR] = BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.15,0.20),pIB_Size,TXT_EJECTA_SCALAR);
	m_mMain_Pane_Buttons[TXT_SHELL_SCALAR] 	= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.15,0.15),pIB_Size,TXT_SHELL_SCALAR);
	m_mMain_Pane_Buttons[TXT_WL_BLUE] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.25,0.30),pIB_Size,TXT_WL_BLUE);
	m_mMain_Pane_Buttons[TXT_WL_RED] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.25,0.25),pIB_Size,TXT_WL_RED);
	m_mMain_Pane_Buttons[TXT_WL_DELTA] 		= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(dAR * 0.25,0.10),pIB_Size,TXT_WL_DELTA);
	m_mMain_Pane_Buttons[DISPLAY_SPECTRUM] 	= BUTTON_INFO(BUTTON_INFO::RECTANGLE,PAIR<double>(      0.00,1.00),pDisp_Size,DISPLAY_SPECTRUM);

}
void MSDB_MGR_MAIN::gfx_close(void) // graphics exiting; rendering context still active
{
}

void MSDB_MGR_MAIN::gfx_display(pane_id i_idPane) // primary display routine
{
	if (g_vEllipse.empty())
	{
		GenerateEllipse(1.0,1.0,g_vEllipse);
	}
	if (i_idPane == m_idPane)
	{
		double dSize = Get_Pane_Aspect_Ratio(m_idPane);
		for (std::map<button_id,BUTTON_INFO>::const_iterator cI = m_mMain_Pane_Buttons.begin(); cI != m_mMain_Pane_Buttons.end(); cI++)
		{
			PAIR<double> pdPosition = cI->second.GetPosition();
			PAIR<double> pdSize = cI->second.GetSize();
			std::ostringstream ssText;
			glPushMatrix();
				glTranslated(pdPosition.m_tX,pdPosition.m_tY,0.0);
				glScaled(pdSize.m_tX,pdSize.m_tY,1.0);
				int iStatus = cI->second.GetStatus();
				switch (cI->second.GetID())
				{
				case TXT_CURR_SPEC_ID:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].first;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_MODEL_ID:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_uiModel_ID;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_ION:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_uiIon;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_PS_VEL:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dPhotosphere_Velocity_kkms;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_PS_TEMP:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dPhotosphere_Temp_kK;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_EJECTA_SCALAR:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dEjecta_Effective_Log_Scalar;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_SHELL_SCALAR:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dShell_Effective_Log_Scalar;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_WL_BLUE:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dWavelength_Range_Lower_Ang;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_WL_RED:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dWavelength_Range_Upper_Ang;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case TXT_WL_DELTA:
					if (!m_vdbidSpectra_List.empty())
						ssText << m_vdbidSpectra_List[m_uiSelected_Spectrum].second.m_dWavelength_Delta_Ang;
					else
						ssText << "---";
					glScaled(1.0/pdSize.m_tX,1.0/pdSize.m_tY,1.0);
					glScaled(1.0/dSize,1.0,1.0);
					glPrintJustified(pdSize.m_tY * 0.8,pdSize.m_tX*0.5,-pdSize.m_tY*0.5,0.0,ssText.str().c_str(),HJ_CENTER,VJ_MIDDLE);
					break;
				case DISPLAY_SPECTRUM:
					{
						glTranslated(0.0,-1.0,0.0);
						glColor4d(0.0,0.0,0.0,1.0);
						glPushMatrix();
							glTranslated(0.05,0.1,0.0);
							std::ostringstream sWL;
							sWL << m_specCurr_Spectrum.wl(0);
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.05,0.0,-0.01,0.0,sWL.str().c_str(),HJ_CENTER,VJ_TOP);
						glPopMatrix();
						glPushMatrix();
							glTranslated(0.95,0.1,0.0);
							std::ostringstream sWL2;
							sWL2 << m_specCurr_Spectrum.wl(m_specCurr_Spectrum.size() - 1);
							glScaled(1.0/dSize,1.0,1.0);
							glPrintJustified(0.05,0.0,-0.01,0.0,sWL2.str().c_str(),HJ_CENTER,VJ_TOP);
						glPopMatrix();
						double dWL_scale = 0.90 / (m_specCurr_Spectrum.wl(m_specCurr_Spectrum.size() - 1) - m_specCurr_Spectrum.wl(0));
						double dFlux_Scale = 0.90 / 2.0;//(m_dMax_Flux * 1.25);
						glTranslated(0.05,0.1,0.0);
						glBegin(GL_LINE_STRIP);
						for (unsigned int uiI = 0; uiI < m_specCurr_Spectrum.size(); uiI++)
						{
							double dX = (m_specCurr_Spectrum.wl(uiI) - m_specCurr_Spectrum.wl(0)) * dWL_scale;
							double dY = m_specCurr_Spectrum.flux(uiI) * dFlux_Scale;
							glVertex2d(dX,dY);
						}
						glEnd();
					}
					break;
				default:
					break;
				}
			glPushMatrix();
		}
	}
}

