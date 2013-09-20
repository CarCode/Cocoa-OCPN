/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Polar Plugin
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/tokenzr.h>
#include <wx/fileconf.h>

#include "polar_pi.h"
#include "PolarDialog.h"
#include "Polar.h"
#include "Options.h"

#include "jsonreader.h"
#include "jsonwriter.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new polar_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    Polar PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"



//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

polar_pi::polar_pi(void *ppimgr)
      :opencpn_plugin_17(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
}

polar_pi::~polar_pi(void)
{
      delete _img_Polar_pi;
      delete _img_Polar;
}

int polar_pi::Init(void)
{
//      printf("polar_pi Init()\n");
      AddLocaleCatalog( _T("opencpn-polar_pi") );

      // Set some default private member parameters
      m_Polar_dialog_x = 0;
      m_Polar_dialog_y = 0;
      m_Polar_dialog_sx = 200;
      m_Polar_dialog_sy = 200;
      m_pPolarDialog = NULL;
      m_bShowPolar = false;

	  opt = new Options();
	  opt->meter = _T("m");
	  opt->feet  = _T("ft");

	  opt->abrSails.Clear();
	  opt->sailsName.Clear();

      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

      //    And load the configuration items
      LoadConfig();

      // Get a pointer to the opencpn display canvas, to use as a parent for the Polar dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    This PlugIn needs a toolbar icon, so request its insertion if enabled locally
      if(m_bPolarShowIcon)
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_Polar, _img_Polar, wxITEM_CHECK,
                  _("Polar"), _T(""), NULL,
                   POLAR_TOOL_POSITION, 0, this);

//      wxMenuItem *pmi = new wxMenuItem(NULL, -1, _("PlugIn Item"));
//      int miid = AddCanvasContextMenuItem(pmi, (PlugInCallBackFunction )&s_ContextMenuCallback );
//      SetCanvasContextMenuItemViz(miid, true);

      return (WANTS_TOOLBAR_CALLBACK |
           INSTALLS_TOOLBAR_TOOL     |
           WANTS_CONFIG              |
		   WANTS_NMEA_SENTENCES      |
		   WANTS_NMEA_EVENTS		 |
           WANTS_PREFERENCES		 |
		   WANTS_PLUGIN_MESSAGING
            );
}

bool polar_pi::DeInit(void)
{
//      printf("polar_pi DeInit()\n");
      if(m_pPolarDialog)
            m_pPolarDialog->Close();

      return true;
}

void polar_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{		
      if(message_id == _T("LOGBOOK_ENGINEBUTTON1"))
      {
		if(m_pPolarDialog)
			m_pPolarDialog->polar->setEngineStatus(message_body);
      }
}

int polar_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int polar_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int polar_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int polar_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *polar_pi::GetPlugInBitmap()
{
      return _img_Polar_pi;
}

wxString polar_pi::GetCommonName()
{
      return _("Polar");
}


wxString polar_pi::GetShortDescription()
{
      return _("Polar PlugIn for OpenCPN");
}


wxString polar_pi::GetLongDescription()
{
      return _("Polar PlugIn for OpenCPN\n\
");

}


void polar_pi::SetDefaults(void)
{
      // If the config somehow says NOT to show the icon, override it so the user gets good feedback
      if(!m_bPolarShowIcon)
      {
            m_bPolarShowIcon = true;

            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_Polar, _img_Polar, wxITEM_NORMAL,
                  _("Polar"), _T(""), NULL,
                   POLAR_TOOL_POSITION, 0, this);
      }
}


int polar_pi::GetToolbarToolCount(void)
{
      return 1;
}

void polar_pi::ShowPreferencesDialog( wxWindow* parent )
{
      wxDialog *dialog = new wxDialog( parent, wxID_ANY, _("Polar Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );

    //      Build Polar. Page for Toolbox
    int border_size = 4;

    wxBoxSizer* itemBoxSizerPolarPanel = new wxBoxSizer(wxVERTICAL);
    dialog->SetSizer(itemBoxSizerPolarPanel);

    //  Polar toolbox icon checkbox
    wxStaticBox* itemStaticBoxSizerPolarStatic = new wxStaticBox(dialog, wxID_ANY, _("Polar"));
    wxStaticBoxSizer* itemStaticBoxSizerPolar = new wxStaticBoxSizer(itemStaticBoxSizerPolarStatic, wxVERTICAL);
    itemBoxSizerPolarPanel->Add(itemStaticBoxSizerPolar, 0, wxGROW|wxALL, border_size);

    m_pPolarShowIcon = new wxCheckBox( dialog, -1, _("Show Polar icon"), wxDefaultPosition, wxSize(-1, -1), 0 );
    itemStaticBoxSizerPolar->Add(m_pPolarShowIcon, 1, wxALIGN_LEFT|wxALL, border_size);

    m_pPolarUseMS = new wxCheckBox( dialog, -1, _("Show metres/sec for Wind Speed"));
    itemStaticBoxSizerPolar->Add(m_pPolarUseMS, 1, wxALIGN_LEFT|wxALL, border_size);

    m_pPolarShowIcon->SetValue(m_bPolarShowIcon);
    m_pPolarUseMS->SetValue(m_bPolarUseMS);


      wxStdDialogButtonSizer* DialogButtonSizer = dialog->CreateStdDialogButtonSizer(wxOK|wxCANCEL);
      itemBoxSizerPolarPanel->Add(DialogButtonSizer, 0, wxALIGN_RIGHT|wxALL, 5);

      dialog->Fit();

      if(dialog->ShowModal() == wxID_OK)
      {

            //    Show Icon changed value?
            if(m_bPolarShowIcon != m_pPolarShowIcon->GetValue())
            {
                  m_bPolarShowIcon= m_pPolarShowIcon->GetValue();

                  if(m_bPolarShowIcon)
                        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_Polar, _img_Polar, wxITEM_CHECK,
                              _("Polar"), _T(""), NULL, POLAR_TOOL_POSITION,
                              0, this);
                  else
                        RemovePlugInTool(m_leftclick_tool_id);
            }


            if(m_bPolarUseMS != m_pPolarUseMS->GetValue())
            {
                  m_bPolarUseMS = m_pPolarUseMS->GetValue();

                  if(m_pPolarDialog)
                  {
                    m_pPolarDialog->Destroy();

                    m_pPolarDialog = new PolarDialog( m_parent_window, this);
                /*    m_pPolarDialog->Create ( m_parent_window, this, -1, _("Polar Display Control"), m_Polar_dir,
                               wxPoint( m_Polar_dialog_x, m_Polar_dialog_y), wxSize( m_Polar_dialog_sx,
                               m_Polar_dialog_sy));
							   */
                    m_pPolarDialog->Show();                        // Show modeless, so it stays on the screen
                    SetToolbarItemState( m_leftclick_tool_id, true );
                  }
                  else
                    SetToolbarItemState( m_leftclick_tool_id, false );

            }

            SaveConfig();
      }
}

void polar_pi::OnToolbarToolCallback(int id)
{
//      printf("polar_pi ToolCallBack()\n");
//     ::wxBell();

      // Qualify the Polar dialog position
            bool b_reset_pos = false;


#ifdef __WXMSW__
        //  Support MultiMonitor setups which an allow negative window positions.
        //  If the requested window does not intersect any installed monitor,
        //  then default to simple primary monitor positioning.
            RECT frame_title_rect;
            frame_title_rect.left =   m_Polar_dialog_x;
            frame_title_rect.top =    m_Polar_dialog_y;
            frame_title_rect.right =  m_Polar_dialog_x + m_Polar_dialog_sx;
            frame_title_rect.bottom = m_Polar_dialog_y + 30;


            if(NULL == MonitorFromRect(&frame_title_rect, MONITOR_DEFAULTTONULL))
                  b_reset_pos = true;
#else
       //    Make sure drag bar (title bar) of window on Client Area of screen, with a little slop...
            wxRect window_title_rect;                    // conservative estimate
            window_title_rect.x = m_Polar_dialog_x;
            window_title_rect.y = m_Polar_dialog_y;
            window_title_rect.width = m_Polar_dialog_sx;
            window_title_rect.height = 30;

            wxRect ClientRect = wxGetClientDisplayRect();
            ClientRect.Deflate(60, 60);      // Prevent the new window from being too close to the edge
            if(!ClientRect.Intersects(window_title_rect))
                  b_reset_pos = true;

#endif

            if(b_reset_pos)
            {
                  m_Polar_dialog_x = 20;
                  m_Polar_dialog_y = 170;
                  m_Polar_dialog_sx = 300;
                  m_Polar_dialog_sy = 540;
            }

      //Toggle Polar display
      m_bShowPolar = !m_bShowPolar;

      // show the Polar dialog
	  if(NULL == m_pPolarDialog)
	  {
		  m_pPolarDialog = new PolarDialog(m_parent_window, this);
		  if(opt->sailsName.Count() == 0)
		  {
			  m_pPolarDialog->m_buttonFilterPolar->Hide();
			  m_pPolarDialog->m_button61->Hide();
			  m_pPolarDialog->m_staticline421->Hide();
			  m_pPolarDialog->m_choiceMode->SetSelection(1); // to NMEA
			  m_pPolarDialog->polar->setMode(1); // to NMEA

		  }
		  else
			  m_pPolarDialog->m_splitter1->Unsplit();

            m_pPolarDialog->Hide();                        
      }

      //    Toggle dialog?
      if(m_bShowPolar)
            m_pPolarDialog->Show();
      else
            m_pPolarDialog->Hide();


      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowPolar );


}

void polar_pi::OnPolarDialogClose()
{
      SetToolbarItemState( m_leftclick_tool_id, false );

      if(m_pPolarDialog)
            m_pPolarDialog->Hide();

      m_bShowPolar = false;

      SaveConfig();
}

void polar_pi::SetNMEASentence(wxString &sentence)
{
	if(m_pPolarDialog && m_pPolarDialog->polar->nmea)
      {
		  m_pPolarDialog->polar->setSentence(sentence);
      }
}

void polar_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
}

bool polar_pi::LoadConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/PlugIns/Polar" ) );
            pConf->Read ( _T( "ShowPolarIcon" ),  &m_bPolarShowIcon, 1 );
            pConf->Read ( _T( "PolarUseMS" ),     &m_bPolarUseMS, 0 );


            m_Polar_dialog_sx = pConf->Read ( _T ( "PolarDialogSizeX" ), 300L );
            m_Polar_dialog_sy = pConf->Read ( _T ( "PolarDialogSizeY" ), 540L );
            m_Polar_dialog_x =  pConf->Read ( _T ( "PolarDialogPosX" ), 20L );
            m_Polar_dialog_y =  pConf->Read ( _T ( "PolarDialogPosY" ), 170L );

//            if((m_Polar_dialog_x < 0) || (m_Polar_dialog_x > m_display_width))
//                  m_Polar_dialog_x = 5;
//            if((m_Polar_dialog_y < 0) || (m_Polar_dialog_y > m_display_height))
//                  m_Polar_dialog_y = 5;

            //pConf->SetPath ( _T ( "/PlugIns/Polar/Directories" ) );
            pConf->Read ( _T ( "PolarDirectory" ), &m_Polar_dir );

			pConf->SetPath ( _T( "/PlugIns/Logbook" ) );
			wxString sails = wxEmptyString;
			pConf->Read ( _T ( "Sails" ), &sails);
			if(!sails.IsEmpty())
			{
				wxStringTokenizer tkz(sails,_T(","));
				if(wxString(sails.GetChar(0)).IsNumber())
				{
					opt->rowGap = wxAtoi(tkz.GetNextToken());
					opt->colGap = wxAtoi(tkz.GetNextToken());
				}
				
				for(int i = 0; i < 14; i++)
				{
					opt->abrSails.Add(tkz.GetNextToken());
					opt->sailsName.Add(tkz.GetNextToken());
					opt->bSailIsChecked[i] = (wxAtoi(tkz.GetNextToken())?true:false);
				}
			}
            return true;
      }
      else
            return false;
}

bool polar_pi::SaveConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Plugins/Polar" ) );
            pConf->Write ( _T ( "ShowPolarIcon" ), m_bPolarShowIcon );
            pConf->Write ( _T ( "PolarUseMS" ),    m_bPolarUseMS );

            pConf->Write ( _T ( "PolarDialogSizeX" ),  m_Polar_dialog_sx );
            pConf->Write ( _T ( "PolarDialogSizeY" ),  m_Polar_dialog_sy );
            pConf->Write ( _T ( "PolarDialogPosX" ),   m_Polar_dialog_x );
            pConf->Write ( _T ( "PolarDialogPosY" ),   m_Polar_dialog_y );

           // pConf->SetPath ( _T ( "/Directories" ) );
            pConf->Write ( _T ( "PolarDirectory" ), m_Polar_dir );

            return true;
      }
      else
            return false;
}

void polar_pi::SetColorScheme(PI_ColorScheme cs)
{

      DimeWindow(m_pPolarDialog);

}
