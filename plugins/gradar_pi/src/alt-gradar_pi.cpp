/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Garmin Radar Plugin
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *   bdbcat@yahoo.com   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/socket.h>
#include "wx/apptrait.h"
#include <wx/glcanvas.h>
#include "wx/sckaddr.h"
#include "wx/datetime.h"
#include <wx/fileconf.h>

#ifdef __WXGTK__
#include <netinet/in.h>
#include <sys/ioctl.h>
#endif

#ifdef __WXMSW__
#include "GL/glu.h"
#endif



#include "gradar_pi.h"

bool              g_thread_active;

unsigned char buf[10000];           // The data buffer shared between the RX thread and the main thread

int               g_max_scan_length_bytes;
int               g_current_scan_length_bytes;
unsigned char     *g_scan_buf;
int               g_range_meters;
int               g_scale_meters;
double            g_selected_range;


unsigned char     *g_static_buf;
int               g_static_max_scan_length_bytes;
int               g_static_scan_length_bytes;
int               g_static_scale_meters;

wxDateTime        g_static_timestamp;
wxDateTime        g_texture_timestamp;

GLuint            g_static_texture_name;
int               g_tex_width;
int               g_tex_height;
int               g_tex_sub_width;
int               g_tex_sub_height;
int               g_texture_scale_meters;

double            g_overlay_transparency;
bool              g_bmaster;

double available_ranges[] = {
                  0.25,
                  0.5,
                  1.0,
                  3.0,
                  6.0,
                  12.0,
                  18.0,
                  24.0,
                  36.0
                  };
int   n_ranges = 9;

bool g_bpos_set;
double g_ownship_lat;
double g_ownship_lon;
double g_hdm;
double g_hdt;
double hdt_last_message;

int   g_radar_state;
int   g_warmup_timer;
int   g_last_warmup_timer;
int   g_scanner_state;
int   g_pseudo_tick;
int   g_updatemode;           //0:realtime,  1:full screen
int   g_sweep_count;
wxDateTime  g_dt_last_tick;
int   g_scan_packets_per_tick;

bool  g_bshown_dc_message;
wxTextCtrl        *plogtc;
wxDialog          *plogcontainer;


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new gradar_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}


int nseq;
bool b_enable_log;

//	Private logging functions
void grLogMessage(wxString s)
{
    if(b_enable_log && plogtc && plogcontainer) {
        wxString seq;
        seq.Printf(_T("%6d: "), nseq++);

        plogtc->AppendText(seq);

        plogtc->AppendText(s);
        plogcontainer->Show();
    }
}


//---------------------------------------------------------------------------------------------------------
//
//    GRadar PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"
//#include "default_pi.xpm"
#include "icons.cpp"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

gradar_pi::gradar_pi(void *ppimgr)
      :opencpn_plugin_18(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
      m_pdeficon = new wxBitmap(*_img_radar_blank);

}

int gradar_pi::Init(void)
{
      m_pControlDialog = NULL;

      g_updatemode = 0;
      g_radar_state = RADAR_OFF;
      g_warmup_timer = 1000;
      g_scanner_state = 0;
      g_scan_buf = 0;
      g_static_buf = 0;
      g_static_max_scan_length_bytes = 0;
      g_max_scan_length_bytes = 0;
      g_texture_timestamp = wxDateTime::Now();  // it will be stale by the time we need it...
      g_overlay_transparency = .50;
      g_sweep_count = 0;
      g_bmaster = true;
      g_dt_last_tick = wxDateTime::Now();
      m_ptemp_icon = NULL;
      m_sent_bm_id_normal = -1;
      m_sent_bm_id_rollover =  -1;

//      m_plogwin = new wxLogWindow(NULL, _T("gradar_pi Log"));


      plogcontainer = new wxDialog(NULL, -1, _T("gradar_pi Log"), wxPoint(0,0), wxSize(600,400),
				     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );

      plogtc = new wxTextCtrl(plogcontainer, -1, _T(""), wxPoint(0,0), wxSize(600,400), wxTE_MULTILINE );
//      plogcontainer->Show();

//      plog = new wxLogTextCtrl(m_plogtc);


      wxLogMessage(_T("gradar_pi log opened"));
      grLogMessage(_T("gradar_pi log opened\n"));

      AddLocaleCatalog( _T("opencpn-gradar_pi") );

      // Set some default private member parameters
      m_GRadar_dialog_x = 0;
      m_GRadar_dialog_y= 0;
      m_GRadar_dialog_sx = 200;
      m_GRadar_dialog_sy = 200;
//      m_pDialog = NULL;

      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

      //    And load the configuration items
      LoadConfig();

      // Get a pointer to the opencpn display canvas, to use as a parent for the UI dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    This PlugIn needs a toolbar icon
      {
            m_tool_id  = InsertPlugInTool(_T(""), _img_radar_red, _img_radar_red, wxITEM_NORMAL,
                  _T("GRadar"), _T(""), NULL,
                   GRADAR_TOOL_POSITION, 0, this);

            CacheSetToolbarToolBitmaps( BM_ID_RED, BM_ID_BLANK);
      }

      wxIPV4address dummy;                // Do this to initialize the wxSocketBase tables

      //    Create the control socket for the Radar
      wxIPV4address addr101;;
      addr101.AnyAddress();
      addr101.Service(_T("50101"));     // does this matter?
      m_out_sock101 = new wxDatagramSocket(addr101, wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT);

      //    Create the thread for Multicast radar data reception
      m_pmcrxt = new MulticastRXThread(&m_mutex, _T("239.254.2.0"),_T("50100"));
      m_pmcrxt->Run();

      g_radar_state = RADAR_ACTIVATE;


      m_pmenu = new wxMenu();            // this is a dummy menu
                                        // required by Windows as parent to item created
      wxMenuItem *pmi = new wxMenuItem(m_pmenu, -1, _("Radar Control"));
      int miid = AddCanvasContextMenuItem(pmi, this );
      SetCanvasContextMenuItemViz(miid, true);

      return (WANTS_DYNAMIC_OPENGL_OVERLAY_CALLBACK |
           WANTS_OPENGL_OVERLAY_CALLBACK |
           WANTS_OVERLAY_CALLBACK     |
           WANTS_CURSOR_LATLON        |
           WANTS_TOOLBAR_CALLBACK     |
           INSTALLS_TOOLBAR_TOOL      |
           INSTALLS_CONTEXTMENU_ITEMS |
           WANTS_CONFIG               |
           WANTS_NMEA_EVENTS          |
           WANTS_PREFERENCES
            );


}

bool gradar_pi::DeInit(void)
{
//      printf("gradar_pi DeInit()\n");
//      delete _img_radar_pi;

      SaveConfig();

      RadarTxOff();
      if(m_pmcrxt)
      {
            m_pmcrxt->Delete();
            int max_timeout = 5;
            int timeout = max_timeout;                   // deadman
            while(g_thread_active && timeout > 0)
            {
                  wxSleep(1);
                  timeout--;
            }
            printf("Thread stopped in %d seconds\n", max_timeout - timeout);
      }

//  Cannot delete until PlugIn dtor (wheich never happens)
//	delete m_pdeficon;

      plogcontainer->Hide();
      plogcontainer->Close();
      delete plogcontainer;

      return true;
}

int gradar_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int gradar_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int gradar_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int gradar_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *gradar_pi::GetPlugInBitmap()
{
      return m_pdeficon;
}

wxString gradar_pi::GetCommonName()
{
      return _T("GRadar");
}


wxString gradar_pi::GetShortDescription()
{
      return _("Garmin Radar PlugIn for OpenCPN");
}


wxString gradar_pi::GetLongDescription()
{
      return _("Garmin Radar PlugIn for OpenCPN\n");

}


void gradar_pi::SetDefaults(void)
{
      // If the config somehow says NOT to show the icon, override it so the user gets good feedback
      if(!m_bShowIcon)
      {
            m_bShowIcon = true;

            m_tool_id  = InsertPlugInTool(_T(""), _img_radar_red, _img_radar_red, wxITEM_CHECK,
                  _T("GRadar"), _T(""), NULL,
                   GRADAR_TOOL_POSITION, 0, this);
      }

}


int gradar_pi::GetToolbarToolCount(void)
{
      return 1;
}

void gradar_pi::OnContextMenuItemCallback(int id)
{
      if(NULL == m_pControlDialog)
      {
           m_pControlDialog = new GRadarDialog;
           m_pControlDialog->Create(m_parent_window, this);
//           m_pControlDialog->SetSize(m_GRadar_dialog_x, m_GRadar_dialog_y,
//                   m_GRadar_dialog_sx, m_GRadar_dialog_sy);
           m_pControlDialog->Hide();
      }

      if(m_pControlDialog->IsShown())
            m_pControlDialog->Hide();
      else {
            m_pControlDialog->Show();
            m_pControlDialog->SetSize(m_GRadar_dialog_x, m_GRadar_dialog_y,
                   m_GRadar_dialog_sx, m_GRadar_dialog_sy);
      }

//      printf("Context menu callback\n");
}

void gradar_pi::OnGRadarDialogClose()
{

      if(m_pControlDialog)
            m_pControlDialog->Hide();
      SaveConfig();
}

void gradar_pi::ShowPreferencesDialog( wxWindow* parent )
{
      wxDialog *dialog = new wxDialog( parent, wxID_ANY, _("GRadar Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );


    wxBoxSizer* itemBoxSizerPanel = new wxBoxSizer(wxVERTICAL);
    dialog->SetSizer(itemBoxSizerPanel);

/*
    //  Grib toolbox icon checkbox
    wxStaticBox* itemStaticBoxSizerGRIBStatic = new wxStaticBox(dialog, wxID_ANY, _("GRIB"));
    wxStaticBoxSizer* itemStaticBoxSizerGRIB = new wxStaticBoxSizer(itemStaticBoxSizerGRIBStatic, wxVERTICAL);
    itemBoxSizerGRIBPanel->Add(itemStaticBoxSizerGRIB, 0, wxGROW|wxALL, border_size);

    m_pGRIBShowIcon = new wxCheckBox( dialog, -1, _("Show GRIB icon"), wxDefaultPosition, wxSize(-1, -1), 0 );
    itemStaticBoxSizerGRIB->Add(m_pGRIBShowIcon, 1, wxALIGN_LEFT|wxALL, border_size);

    m_pGRIBUseHiDef = new wxCheckBox( dialog, -1, _("Use High Definition Graphics"));
    itemStaticBoxSizerGRIB->Add(m_pGRIBUseHiDef, 1, wxALIGN_LEFT|wxALL, border_size);

    m_pGRIBShowIcon->SetValue(m_bGRIBShowIcon);
    m_pGRIBUseHiDef->SetValue(m_bGRIBUseHiDef);

      wxStdDialogButtonSizer* DialogButtonSizer = dialog->CreateStdDialogButtonSizer(wxOK|wxCANCEL);
      itemBoxSizerGRIBPanel->Add(DialogButtonSizer, 0, wxALIGN_RIGHT|wxALL, 5);
*/
      dialog->Fit();

      if(dialog->ShowModal() == wxID_OK)
      {
            //    Show Icon changed value?
            if(m_bShowIcon != m_pShowIcon->GetValue())
            {
                  m_bShowIcon= m_pShowIcon->GetValue();

                  if(m_bShowIcon)
                        m_tool_id  = InsertPlugInTool(_T(""),_img_radar_red, _img_radar_red, wxITEM_CHECK,
                              _T("GRadar"), _T(""), NULL, GRADAR_TOOL_POSITION,
                              0, this);
                  else
                        RemovePlugInTool(m_tool_id);
            }


            SaveConfig();
      }

}

void gradar_pi::OnToolbarToolCallback(int id)
{

      switch(g_radar_state)
      {
            case RADAR_TX_ACTIVE:
                  RadarTxOff();
                  if(!g_bmaster)          // slave mode turning display off
                        g_radar_state = RADAR_STANDBY;
                  break;

            case RADAR_STANDBY:
                  RadarTxOn();
                  if(!g_bmaster)          // slave mode turning display on
                        g_radar_state = RADAR_TX_ACTIVE;
                  break;

            case RADAR_ACTIVATE:
                  if(!g_bmaster)          // slave mode initial state
                        g_radar_state = RADAR_TX_ACTIVE;
                  break;

            default:
                  break;
      }

      UpdateState();


}





void gradar_pi::OnDialogClose()
{
      SaveConfig();
}

void gradar_pi::DoTick(void)
{
   wxDateTime now = wxDateTime::Now();
   if(g_dt_last_tick.IsValid()) {
      wxTimeSpan ts = now.Subtract(g_dt_last_tick);
      double delta_t = ts.GetMilliseconds().ToDouble();
      if(delta_t > 1000.) {
            g_dt_last_tick = now;

            //    If in slave mode, and no data appears to be coming in,
            //    switch to ACTIVATE mode
            if((g_scan_packets_per_tick == 0) && !g_bmaster) {

                  free (g_scan_buf);        // empty the buffer
                  g_scan_buf = 0;

                  if(g_static_texture_name) {
                    glDeleteTextures(1, &g_static_texture_name);
                    g_static_texture_name = 0;
                  }

                  g_radar_state = RADAR_ACTIVATE;
            }

            g_scan_packets_per_tick = 0;
      }
   }
}

bool gradar_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    if(0 == g_bshown_dc_message) {
        wxString message(_("The Radar Overlay PlugIn requires OpenGL mode to be activated in Toolbox->Settings"));
//        ::wxMessageBox(message, _T("gradar_pi message"), wxICON_INFORMATION | wxOK, GetOCPNCanvasWindow());

        g_bshown_dc_message = 1;

	wxMessageDialog dlg(GetOCPNCanvasWindow(),  message, _T("gradar_pi message"), wxOK);
	dlg.ShowModal();

    }

    return false;

}

bool gradar_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{

   g_bshown_dc_message = 0;             // show message box if RenderOverlay() is called again

   // We can use this method as a sort of time tick,
   // since it is expected to be called at least once per second
   DoTick();

   UpdateState();

   bool b_time_render = true;
/*
   wxDateTime now = wxDateTime::Now();
   if(m_dt_last_render.IsValid())
   {
      wxTimeSpan ts = now.Subtract(m_dt_last_render);
      double delta_t = ts.GetMilliseconds().ToDouble();
      printf("delta t: %g\n", delta_t);
      if(delta_t < 100.)
            b_time_render = false;
   }
*/
   m_dt_last_render = wxDateTime::Now();

   if((g_radar_state == RADAR_TX_ACTIVE) && b_time_render)
   {
       double max_distance = 0;

       //   If ownship position is not valid, use the ViewPort center
       wxPoint radar_center(vp->pix_width/2, vp->pix_height/2);
       if(g_bpos_set)
       {
      printf("g_bpos_set\n");
         GetCanvasPixLL(vp, &radar_center, g_ownship_lat, g_ownship_lon);

            // Calculate the "optimum" radar range setting

            // first calculate the max distance from ownship to 4 corners of the viewport
          double lat, lon;
          GetCanvasLLPix( vp, wxPoint(0, 0), &lat, &lon);
          max_distance = wxMax(max_distance, piDistGreatCircle(lat, lon, g_ownship_lat, g_ownship_lon));

          GetCanvasLLPix( vp, wxPoint(0, vp->pix_height), &lat, &lon);
          max_distance = wxMax(max_distance, piDistGreatCircle(lat, lon, g_ownship_lat, g_ownship_lon));

          GetCanvasLLPix( vp, wxPoint(vp->pix_width, vp->pix_height), &lat, &lon);
          max_distance = wxMax(max_distance, piDistGreatCircle(lat, lon, g_ownship_lat, g_ownship_lon));

          GetCanvasLLPix( vp, wxPoint(vp->pix_width, 0), &lat, &lon);
          max_distance = wxMax(max_distance, piDistGreatCircle(lat, lon, g_ownship_lat, g_ownship_lon));
       }
       else
       {
           int max_pix = sqrt((double)(vp->pix_width * vp->pix_width) + (vp->pix_height*vp->pix_height)) / 2;
           max_distance = (max_pix / (double)vp->view_scale_ppm) / 1852.0;
       }

      // now look in the list of available ranges to find the smallest range
      // that is larger than the required range

      double req_range = available_ranges[n_ranges - 1];
      for(int i=0 ; i < n_ranges ; i++)
      {
            if(available_ranges[i] >= max_distance)
            {
                  req_range = available_ranges[i];
                  break;
            }
      }

      // do we need a switch ?
      if(g_selected_range != req_range)
      {
            g_selected_range = req_range;
            Select_Range(g_selected_range);
      }

      //    Calculate drawing scale factor
      double ulat, llat, ulon, llon;
      GetCanvasLLPix( vp, wxPoint(0, vp->pix_height), &ulat, &ulon);
      GetCanvasLLPix( vp, wxPoint(0, 0), &llat, &llon);
      double dist_y = piDistGreatCircle(ulat, ulon, llat, llon);

      double v_scale_ppm = 1.0;
      if(dist_y > 0.)
            v_scale_ppm = vp->pix_height/(dist_y * 1852.);

      wxString msg;
      msg.Printf(_T("v_scale_ppm:  %g  Center: %d %d\n"), v_scale_ppm, radar_center.x, radar_center.y);
      grLogMessage(msg);


      if(g_updatemode == 0) {                   // direct realtime sweep render mode
            RenderRadarOverlaySwept(radar_center, v_scale_ppm, vp);
      }

      else {                                    // cached full screen render mode
            RenderRadarOverlayFull(radar_center, v_scale_ppm, vp);
      }




      return true;
  }
  else {
      return false;
  }

}

void gradar_pi::RenderRadarOverlaySwept(wxPoint radar_center, double v_scale_ppm, PlugIn_ViewPort *vp)
{

      glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_HINT_BIT);      //Save state
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glPushMatrix();

      glTranslated( radar_center.x, radar_center.y, 0);

      glRotatef(-90., 0, 0, 1);        // first correction
      glRotatef(g_hdt, 0, 0, 1);        // second correction

      // scaling...
      double pixels_per_meter = g_current_scan_length_bytes / (double)g_scale_meters;
      double scale_factor =  v_scale_ppm / pixels_per_meter;
      scale_factor *= 4.0;          // each scan radial is 4x pixels
      glScaled( scale_factor, scale_factor, 1. );

      RenderRadarBufferDirect(vp);
      glPopMatrix();

      glPopAttrib();            // restore state

}

void gradar_pi::RenderRadarOverlayFull(wxPoint radar_center, double v_scale_ppm, PlugIn_ViewPort *vp)
{
      if(g_static_timestamp.IsValid() && g_texture_timestamp.IsValid()) {
            if(g_static_timestamp.IsLaterThan(g_texture_timestamp)) {

            printf("Create Texture\n");
            // First approach...
            // If no FBO available....
            // Render the image to a bitmap, convert to image, then convert to texture

            // Do the render at 4x size, to get decent pixelation behavior
            // Note g_static_scan_length_bytes is already 4x the pixels captured
            wxBitmap bmp(g_static_scan_length_bytes *2, g_static_scan_length_bytes * 2);
            wxMemoryDC mdc(bmp);
            mdc.SetBackground(*wxBLACK_BRUSH);
            mdc.Clear();

            RenderRadarBuffer(g_static_buf, g_static_scan_length_bytes, &mdc,
                              bmp.GetWidth(), bmp.GetHeight());

            mdc.SelectObject(wxNullBitmap);

            // Convert bitmap to image
            wxImage image = bmp.ConvertToImage();
//            image.SaveFile(_T("/home/dsr/rad1.jpg"), wxBITMAP_TYPE_JPEG);
            wxImage image1 = image; //image.Blur(2);

#if 0

            // Convert to RGBA buffer
            int w = image.GetWidth(), h = image.GetHeight();

            //  Case:
            //  The data is in the red channel, and we want a transparent overlay
            //  with alpha scaled by data intensity and overall transparency value
            unsigned char *d = image1.GetData();
            unsigned char *e = new unsigned char[4*w*h];
            for(int y=0; y<h; y++) {
                  for(int x=0; x<w; x++) {
                        unsigned char r, g, b;
                        int off = (y*w)+x;
                        r = d[off*3 + 0];
                        g = d[off*3 + 1];
                        b = d[off*3 + 2];
                        if(r) {
                           e[off*4 + 0] = 255;
                           e[off*4 + 1] = 0;
                           e[off*4 + 2] = 0;
                           double ri = (double)r * g_overlay_transparency;
                           e[off*4 + 3] = (unsigned char)ri;
                        }
                        else
                           e[off*4 + 3] = 0;
                  }
             }


             glDeleteTextures(1, &g_static_texture_name);

             glGenTextures(1, &g_static_texture_name);
             glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_static_texture_name);
             glTexParameterf( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
             glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
             glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, w, h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, e );

             delete[] e;

             g_tex_width = w;
             g_tex_height = h;
             g_texture_timestamp = g_static_timestamp;
             g_texture_scale_meters = g_static_scale_meters;
            }

            // Render the texture

            printf("Render Texture\n");
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_static_texture_name);

            float wt = g_tex_width;
            float ht = g_tex_height;

            glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_HINT_BIT);      //Save state
            glEnable(GL_TEXTURE_RECTANGLE_ARB);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glPushMatrix();

            glTranslated( radar_center.x, radar_center.y, 0);
            glScaled( 1., 1., 1. );
            glRotatef(-90., 0, 0, 1);        // first correction
            glRotatef(g_hdt, 0, 0, 1);        // second correction

            float half_pixels = (double)g_texture_scale_meters * v_scale_ppm;

            glBegin( GL_QUADS );

            glTexCoord2f( 0., 0. );
            glVertex2f(-half_pixels, -half_pixels);

            glTexCoord2f( wt, 0. );
            glVertex2f(half_pixels, -half_pixels);

            glTexCoord2f( wt, ht );
            glVertex2f(half_pixels, half_pixels);

            glTexCoord2f( 0., ht );
            glVertex2f(-half_pixels, half_pixels);

            glEnd();
            glDisable(GL_TEXTURE_RECTANGLE_ARB);
            glPopMatrix();
            glPopAttrib();            // restore state

#endif

#if 1
            // Convert to POT RGBA buffer
            int w = image.GetWidth(), h = image.GetHeight();
            // Get next larger POT width/height
            int wpot = w;
            int hpot = h;
            wpot--;
            for (int i=1; i<16; i<<=1)
                wpot = wpot | wpot >> i;
            wpot++;

            hpot--;
            for (int i=1; i<16; i<<=1)
                hpot = hpot | hpot >> i;
            hpot++;

            printf("wpot hpot %d %d\n", wpot, hpot);

            //  Case:
            //  The data is in the red channel, and we want a transparent overlay
            //  with alpha scaled by data intensity and overall transparency value
            unsigned char *d = image1.GetData();
            unsigned char *e = new unsigned char[4*wpot*hpot];
            for(int y=0; y<h; y++) {
                  for(int x=0; x<wpot; x++) {
                        unsigned char r; //, g, b;

                        if(x < w) {
                            int soff = (y*w)+x;
                            r = d[soff*3 + 0];
//                        g = d[soff*3 + 1];
//                        b = d[soff*3 + 2];
                        }
                        else
                            r = 0;

                        int doff = (y*wpot)+x;
                        if(r) {
                           e[doff*4 + 0] = 255;
                           e[doff*4 + 1] = 0;
                           e[doff*4 + 2] = 0;
                           double ri = (double)r * g_overlay_transparency;
                           e[doff*4 + 3] = (unsigned char)ri;
                        }
                        else
                           e[doff*4 + 3] = 0;
                  }
             }


             glDeleteTextures(1, &g_static_texture_name);
             glGenTextures(1, &g_static_texture_name);
             glBindTexture(GL_TEXTURE_2D, g_static_texture_name);
             glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
             glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
             glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wpot, hpot, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, e );

             delete[] e;

             g_tex_width = wpot;
             g_tex_height = hpot;
             g_tex_sub_width = w;
             g_tex_sub_height = h;

             g_texture_timestamp = g_static_timestamp;
             g_texture_scale_meters = g_static_scale_meters;
            }

            // Render the texture

            if(g_static_texture_name) {
//                printf("Render Texture\n");
                glBindTexture(GL_TEXTURE_2D, g_static_texture_name);

//                printf("Tex w/h %d %d %d %d \n", g_tex_width, g_tex_height, g_tex_sub_width, g_tex_sub_height);

                glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_HINT_BIT);      //Save state
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glPushMatrix();

                glTranslated( radar_center.x, radar_center.y, 0);
                glScaled( 1., 1., 1. );
                glRotatef(-90., 0, 0, 1);        // first correction
                glRotatef(g_hdt, 0, 0, 1);        // second correction

                float half_pixels = (double)g_texture_scale_meters * v_scale_ppm;
                float wt = (double)g_tex_sub_width/g_tex_width;
                float ht = (double)g_tex_sub_height/g_tex_height;

 //               printf("%g\n", half_pixels);
                glBegin( GL_QUADS );

                glTexCoord2f( 0., 0. );
                glVertex2f(-half_pixels, -half_pixels);

                glTexCoord2f( wt, 0. );
                glVertex2f(half_pixels, -half_pixels);

                glTexCoord2f( wt, ht );
                glVertex2f(half_pixels, half_pixels);

                glTexCoord2f( 0., ht );
                glVertex2f(-half_pixels, half_pixels);

                glEnd();

                glDisable(GL_TEXTURE_2D);

                glPopMatrix();
                glPopAttrib();            // restore state
            }
#endif

      }
}


void gradar_pi::draw_blob_dc(wxDC &dc, double angle, double radius, double blob_r, double arc_length, double scale, int xoff, int yoff)
{
      double ca = cos(angle * PI / 180.);
      double sa = sin(angle * PI / 180.);


      //    Calculate the blob size...
      double xm1 = (radius - blob_r/2) * ca;
      double ym1 = (radius - blob_r/2) * sa;
      double xm2 = (radius + blob_r/2) * ca;
      double ym2 = (radius + blob_r/2) * sa;

      double blob_width2 =  (radius *(2.0 * PI)/(360. / arc_length))/2.0;

      double xa = (xm1 + blob_width2 * sa) * scale;
      double ya = (ym1 - blob_width2 * ca) * scale;

      double xb = (xm2 + blob_width2 * sa) * scale;
      double yb = (ym2 - blob_width2 * ca) * scale;

      double xc = (xm1 - blob_width2 * sa) * scale;
      double yc = (ym1 + blob_width2 * ca) * scale;

      double xd = (xm2 - blob_width2 * sa) * scale;
      double yd = (ym2 + blob_width2 * ca) * scale;

/*
      wxPoint p1[4];
      p1[0].x = wxRound(xa);  p1[0].y = wxRound(ya);
      p1[1].x = wxRound(xb);  p1[1].y = wxRound(yb);
      p1[2].x = wxRound(xc);  p1[2].y = wxRound(yc);
      p1[3].x = wxRound(xd);  p1[3].y = wxRound(yd);
      dc.DrawPolygon(4, p1, xoff, yoff);
*/

      wxPoint p1[3];
      p1[0].x = xa;  p1[0].y = ya;
      p1[1].x = xb;  p1[1].y = yb;
      p1[2].x = xc;  p1[2].y = yc;
      dc.DrawPolygon(3, p1, xoff, yoff);

      p1[0].x = xb;  p1[0].y = yb;
      p1[1].x = xc;  p1[1].y = yc;
      p1[2].x = xd;  p1[2].y = yd;
      dc.DrawPolygon(3, p1, xoff, yoff);
}





void gradar_pi::SetCursorLatLon(double lat, double lon)
{
/*
      if(m_pGribDialog)
      {
            m_pGribDialog->SetCursorLatLon(lat, lon);
      }
*/
}


bool gradar_pi::LoadConfig(void)
{

      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/Settings" ) );
            pConf->Read ( _T( "GRadarOpMode" ),  &g_bmaster, 0 );
            pConf->Read ( _T( "GRadarDisplayMode" ),  &g_updatemode, 0 );
            pConf->Read ( _T( "GRadarTransparency" ),  &g_overlay_transparency, .50 );
            pConf->Read ( _T( "GRadarLog" ),  &b_enable_log, true );


            m_GRadar_dialog_sx = pConf->Read ( _T ( "GRadarDialogSizeX" ), 300L );
            m_GRadar_dialog_sy = pConf->Read ( _T ( "GRadarDialogSizeY" ), 540L );
            m_GRadar_dialog_x =  pConf->Read ( _T ( "GRadarDialogPosX" ), 20L );
            m_GRadar_dialog_y =  pConf->Read ( _T ( "GRadarDialogPosY" ), 170L );


            return true;
      }
      else
            return false;

}

bool gradar_pi::SaveConfig(void)
{

      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Settings" ) );
            pConf->Write ( _T ( "GRadarOpMode" ), g_bmaster );
            pConf->Write ( _T ( "GRadarDisplayMode" ), g_updatemode );
            pConf->Write ( _T ( "GRadarTransparency" ), g_overlay_transparency );
            pConf->Write ( _T ( "GRadarLog" ), b_enable_log );

            pConf->Write ( _T ( "GRadarDialogSizeX" ),  m_GRadar_dialog_sx );
            pConf->Write ( _T ( "GRadarDialogSizeY" ),  m_GRadar_dialog_sy );
            pConf->Write ( _T ( "GRadarDialogPosX" ),   m_GRadar_dialog_x );
            pConf->Write ( _T ( "GRadarDialogPosY" ),   m_GRadar_dialog_y );

            return true;
      }
      else
            return false;

      return true;
}
void gradar_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
      g_ownship_lat = pfix.Lat;
      g_ownship_lon = pfix.Lon;

      g_bpos_set = true;

}
void gradar_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
      g_ownship_lat = pfix.Lat;
      g_ownship_lon = pfix.Lon;

      g_hdm = pfix.Hdm;
      g_hdt = pfix.Hdt;

      if(wxIsNaN(g_hdt)) g_hdt = pfix.Cog;

      if(wxIsNaN(g_hdt)) g_hdt = 0.;

      if(g_hdt != hdt_last_message) {
        wxString msg;
        msg.Printf(_T("Heading:  %g\n"), g_hdt);
        grLogMessage(msg);

        hdt_last_message = g_hdt;
      }

      g_bpos_set = true;

}
void gradar_pi::RadarTxOff(void)
{
      if(!g_bmaster)
            return;
      rad_ctl_pkt pck;
      pck.packet_type = 0x2b2;
      pck.len1 = 2;
      pck.parm1 = 1;            // 1 (one) for "off"

      wxIPV4address destaddr;
      destaddr.Service(_T("50101"));
      destaddr.Hostname(_T("172.16.2.0"));
      m_out_sock101->SendTo(destaddr, &pck, sizeof(pck));

      printf("TX Off\n");
      grLogMessage(_T("TX Off\n"));

      switch(g_radar_state)
      {
            case RADAR_TX_ACTIVATE:
            case RADAR_IN_TIMED_SPINUP:
            case RADAR_TX_ACTIVE:
                  g_radar_state = RADAR_STANDBY;
                  break;
            case RADAR_OFF:
            case RADAR_ACTIVATE:
            case RADAR_IN_TIMED_WARMUP:
            case RADAR_STANDBY:
                  g_radar_state = RADAR_ACTIVATE;
                  break;
      }


//      wxString msg;
//      msg.Printf(_T("Sent Radar Ctl Packet %0X"), parma);
//      wxLogMessage(msg);
}

void gradar_pi::RadarTxOn(void)
{
      if(!g_bmaster)
            return;
      switch(g_radar_state)
      {
            case RADAR_STANDBY:
                  g_radar_state = RADAR_TX_ACTIVATE;
                  break;

            default:
                  break;
      }

      rad_ctl_pkt pck;
      pck.packet_type = 0x2b2;
      pck.len1 = 2;
      pck.parm1 = 2;            // 2(two) for "on"

      wxIPV4address destaddr;
      destaddr.Service(_T("50101"));
      destaddr.Hostname(_T("172.16.2.0"));
      m_out_sock101->SendTo(destaddr, &pck, sizeof(pck));

      printf("TX On\n");
      grLogMessage(_T("TX On\n"));
}


void gradar_pi::CacheSetToolbarToolBitmaps( int bm_id_normal, int bm_id_rollover)
{
    if((bm_id_normal == m_sent_bm_id_normal) && (bm_id_rollover == m_sent_bm_id_rollover))
        return;                                                 // no change needed

    if((bm_id_normal == -1) || (bm_id_rollover == -1)) {          // don't do anything, caller's responsibility
        m_sent_bm_id_normal = bm_id_normal;
        m_sent_bm_id_rollover = bm_id_rollover;
        return;
    }

    m_sent_bm_id_normal = bm_id_normal;
    m_sent_bm_id_rollover = bm_id_rollover;

    wxBitmap *pnormal = NULL;
    wxBitmap *prollover = NULL;

    switch(bm_id_normal)
    {
        case BM_ID_RED:
            pnormal = _img_radar_red;
            break;
        case BM_ID_RED_SLAVE:
            pnormal = _img_radar_red_slave;
            break;
        case BM_ID_GREEN:
            pnormal = _img_radar_green;
            break;
        case BM_ID_GREEN_SLAVE:
            pnormal = _img_radar_green_slave;
            break;
        case BM_ID_AMBER:
             pnormal = _img_radar_amber;
           break;
        case BM_ID_AMBER_SLAVE:
             pnormal = _img_radar_amber_slave;
           break;
        case BM_ID_BLANK:
             pnormal = _img_radar_blank;
           break;
        case BM_ID_BLANK_SLAVE:
             pnormal = _img_radar_blank_slave;
           break;
        default:
            break;
    }

    switch(bm_id_rollover)
    {
        case BM_ID_RED:
            prollover = _img_radar_red;
            break;
        case BM_ID_RED_SLAVE:
            prollover = _img_radar_red_slave;
            break;
        case BM_ID_GREEN:
            prollover = _img_radar_green;
            break;
        case BM_ID_GREEN_SLAVE:
            prollover = _img_radar_green_slave;
            break;
        case BM_ID_AMBER:
             prollover = _img_radar_amber;
           break;
        case BM_ID_AMBER_SLAVE:
             prollover = _img_radar_amber_slave;
           break;
        case BM_ID_BLANK:
             prollover = _img_radar_blank;
           break;
       case BM_ID_BLANK_SLAVE:
             prollover = _img_radar_blank_slave;
           break;
       default:
            break;
    }

    if( (pnormal) && (prollover) )
        SetToolbarToolBitmaps(m_tool_id, pnormal, prollover);
}



void gradar_pi::UpdateState(void)
{
      g_pseudo_tick++;


      printf("UpdateState:  Plugin state: %d   Scanner state:  %d \n", g_radar_state, g_scanner_state);

      wxString scan_state;
      wxString plug_state;

      switch(g_scanner_state)
      {
                 case 1:
                        scan_state = _T("Warmup");
                        break;
                  case 3:
                        scan_state = _T("Standby");
                        break;
                  case 4:
                        scan_state = _T("TX Active");
                        break;
                  case 5:
                        scan_state = _T("Spinup");
                        break;
                  default:
                        scan_state = _T("Unknown");
                        break;
      }

      switch(g_radar_state)
      {
                  case 0:
                        plug_state = _T("Off");
                        break;
                  case 1:
                        plug_state = _T("Activate");
                        break;
                  case 2:
                        plug_state = _T("Warmup");
                        break;
                  case 3:
                        plug_state = _T("Standby");
                        break;
                  case 4:
                        plug_state = _T("Tx Activate");
                        break;
                  case 5:
                        plug_state = _T("Spinup");
                        break;
                  case 6:
                        plug_state = _T("TX Active");
                        break;
                  default:
                        plug_state = _T("Unknown");
                        break;
      }

      wxString msg(_T("UpdateState:  PluginState  "));

      msg += plug_state;
      wxString msg2;
      msg2.Printf(_T("  (%d)"), g_radar_state);
      msg += msg2;

      msg += _T("   Scanner state:  ");

      msg += scan_state;
      msg2.Printf(_T("  (%d)"), g_scanner_state);
      msg += msg2;

      msg += _T("\n");

      grLogMessage(msg);



      //    Auto state switching is only needed in master mode
      if(g_bmaster) {
            int current_state = g_radar_state;

            switch (g_scanner_state)
            {
                  case 1:
                        g_radar_state = RADAR_IN_TIMED_WARMUP;
                        break;
                  case 3:
                        g_radar_state = RADAR_STANDBY;
                        break;
                  case 4:
                        g_radar_state = RADAR_TX_ACTIVE;
                        break;
                  case 5:
                        g_radar_state = RADAR_IN_TIMED_SPINUP;
                        break;

                  default:
                        break;
            }

            if(current_state != g_radar_state)
                  printf("  >>>Switch to state: %d\n", g_radar_state);
      }


      //    Manage icons
      switch(g_radar_state)
      {
            case RADAR_OFF:
            case RADAR_ACTIVATE:
                  if(g_bmaster)
                        CacheSetToolbarToolBitmaps( BM_ID_RED, BM_ID_RED);
                  else
                        CacheSetToolbarToolBitmaps( BM_ID_RED_SLAVE, BM_ID_RED_SLAVE);

                  break;

            case RADAR_IN_TIMED_WARMUP:
            {
                  if(g_warmup_timer != g_last_warmup_timer)
                  {
                        g_last_warmup_timer = g_warmup_timer;

                        wxBitmap *pbmp;
                        if(g_warmup_timer & 1)
                              pbmp = _img_radar_blank;
                        else
                              pbmp = _img_radar_amber;

                        wxString str;
                        str.Printf(_T("%2d"), g_warmup_timer);

                        delete m_ptemp_icon;
                        m_ptemp_icon = new wxBitmap(pbmp->GetWidth(), pbmp->GetHeight());
                        wxMemoryDC dc(*m_ptemp_icon);
                        dc.Clear();
                        dc.DrawBitmap(*pbmp, 0, 0, true);
                        wxColour cf;
                        GetGlobalColor(_T("CHBLK"), &cf);
                        dc.SetTextForeground(cf);
                        wxFont *pFont = wxTheFontList->FindOrCreateFont(10, wxDEFAULT,
                              wxNORMAL, wxNORMAL, FALSE, wxString(_T("Eurostile Extended")));

                        dc.SetFont(*pFont);
                        wxSize s = dc.GetTextExtent(str);
                        dc.DrawText(str, (pbmp->GetWidth() - s.GetWidth()) / 2, (pbmp->GetHeight() - s.GetHeight()) / 2);
                        CacheSetToolbarToolBitmaps( -1, -1 );           // mark the cached values
                        SetToolbarToolBitmaps( m_tool_id, m_ptemp_icon, m_ptemp_icon);
                  }
                  break;
            }

            case RADAR_STANDBY:
                  if(g_bmaster)
                        CacheSetToolbarToolBitmaps( BM_ID_AMBER, BM_ID_AMBER);
                  else
                        CacheSetToolbarToolBitmaps( BM_ID_AMBER_SLAVE, BM_ID_AMBER_SLAVE);
                  break;

            case RADAR_TX_ACTIVATE:
            case RADAR_IN_TIMED_SPINUP:
            {
                  if(g_pseudo_tick & 1) {
                        if(g_bmaster)
                              CacheSetToolbarToolBitmaps( BM_ID_BLANK, BM_ID_BLANK);
                        else
                              CacheSetToolbarToolBitmaps( BM_ID_BLANK_SLAVE, BM_ID_BLANK_SLAVE);
                  }
                  else {
                        if(g_bmaster)
                              CacheSetToolbarToolBitmaps( BM_ID_GREEN, BM_ID_GREEN);
                        else
                              CacheSetToolbarToolBitmaps( BM_ID_GREEN_SLAVE, BM_ID_GREEN_SLAVE);
                  }
                  break;
            }

            case RADAR_TX_ACTIVE:
                        if(g_bmaster)
                              CacheSetToolbarToolBitmaps( BM_ID_GREEN, BM_ID_GREEN);
                        else
                              CacheSetToolbarToolBitmaps( BM_ID_GREEN_SLAVE, BM_ID_GREEN_SLAVE);
                  break;

            default:
                  break;
      }
}

void gradar_pi::Select_Range(double range_nm)
{
      if(!g_bmaster)
            return;

      int range_parm_meters = (range_nm * 1852)-1;
      rad_range_pkt pck;
      pck.packet_type = 0x2b3;
      pck.len1 = 4;
      pck.parm1 = range_parm_meters;

      wxIPV4address destaddr;
      destaddr.Service(_T("50101"));
      destaddr.Hostname(_T("172.16.2.0"));
      m_out_sock101->SendTo(destaddr, &pck, sizeof(pck));

      printf("SelectRange(): %g\n", range_nm);

      wxString msg;
      msg.Printf(_T("SelectRange: %g nm\n"), range_nm);
      grLogMessage(msg);

//      wxString msg;
//      msg.Printf(_T("Sent Radar Range Packet %0X"), parma);
//      wxLogMessage(msg);

}


void gradar_pi::draw_blob_gl(double angle, double radius, double blob_r, double arc_length)
{
      double ca = cos(angle * PI / 180.);
      double sa = sin(angle * PI / 180.);


      //    Calculate the blob size...
      double xm1 = (radius - blob_r/2) * ca;
      double ym1 = (radius - blob_r/2) * sa;
      double xm2 = (radius + blob_r/2) * ca;
      double ym2 = (radius + blob_r/2) * sa;

      double blob_width2 =  (radius *(2.0 * PI)/(360. / arc_length))/2.0;

      double xa = xm1 + blob_width2 * sa;
      double ya = ym1 - blob_width2 * ca;

      double xb = xm2 + blob_width2 * sa;
      double yb = ym2 - blob_width2 * ca;
      double xc = xm1 - blob_width2 * sa;
      double yc = ym1 + blob_width2 * ca;

      double xd = xm2 - blob_width2 * sa;
      double yd = ym2 + blob_width2 * ca;


      glBegin(GL_TRIANGLES);
        glVertex2d(xa, ya);
        glVertex2d(xb, yb);
        glVertex2d(xc, yc);

        glVertex2f(xb, yb);
        glVertex2f(xc, yc);
        glVertex2f(xd, yd);
      glEnd();
}

void gradar_pi::RenderRadarBufferDirect(PlugIn_ViewPort *vp)
{
      return RenderRadarBuffer(g_scan_buf, g_current_scan_length_bytes, NULL, 0, 0);
}



void gradar_pi::RenderRadarBuffer(unsigned char *buffer, int buffer_line_length, wxDC *pdc, int width, int height)
{
      if(buffer == 0) return;

      for(int i=0 ; i < 360 ; i+= 2)
      {
            unsigned char *packet_data = &buffer[i * buffer_line_length];

            double angle = i;
            for(int k=0 ; k < 2 ; k++)
            {
            for(int rad=0 ; rad < buffer_line_length/4 ; rad++)
            {
                  unsigned char data1 = *packet_data;
                  unsigned char data2 = *(packet_data + buffer_line_length/4 );

                  if(data1 || data2)
                  {
                        if(!pdc) {
                              glColor4ub(255, 0, 0, data1);
                              draw_blob_gl(angle, rad, 1.0, 0.5);
                              glColor4ub(255, 0, 0, data2);
                              draw_blob_gl(angle+0.5, rad, 1.0, 0.5);

                        }
                        else {
                              wxColour c1(data1, 0, 0);
                              wxPen p1(c1);
                              wxBrush b1(c1);
                              pdc->SetPen(p1);
                              pdc->SetBrush(b1);
                              draw_blob_dc(*pdc, angle, rad, 1.0, 0.5, 4., width/2, height/2);
                              wxColour c2(data2, 0, 0);
                              wxPen p2(c2);
                              wxBrush b2(c2);
                              pdc->SetPen(p2);
                              pdc->SetBrush(b2);
                              draw_blob_dc(*pdc, angle+0.5, rad, 1.0, 0.5, 4., width/2, height/2);
                        }
                  }

                  packet_data++;
            }

            packet_data += buffer_line_length/4;
            angle = i+1;
            }
      }

}

void gradar_pi::SetUpdateMode(int mode)
{
      g_updatemode = mode;
}

void gradar_pi::SetOperatingMode(int mode)
{
      g_bmaster = (mode == 0);
}

void gradar_pi::UpdateDisplayParameters(void)
{
      wxTimeSpan hr(1,0,0,0);
      g_texture_timestamp = wxDateTime::Now() - hr;
      RequestRefresh(GetOCPNCanvasWindow());
}

MulticastRXThread::MulticastRXThread( wxMutex *pMutex, const wxString &IP_addr, const wxString &service_port)
{
//      m_pTarget = MessageTarget;

      m_pShareMutex = pMutex;

      m_ip = IP_addr;
      m_service_port = service_port;
      m_sock = 0;

      Create();
}

MulticastRXThread::~MulticastRXThread()
{
      delete m_sock;
}

void MulticastRXThread::OnExit()
{
}

void *MulticastRXThread::Entry(void)
{
      g_thread_active = true;
      //    Create a datagram socket for input
      m_myaddr.AnyAddress();             // equivalent to localhost
      m_myaddr.Service(m_service_port);     // the port must align with the expected multicast address


      m_sock = new wxDatagramSocket(m_myaddr, wxSOCKET_REUSEADDR);
      m_sock->SetFlags(wxSOCKET_BLOCK);

      //    Subscribe to a multicast group
      unsigned int a;
#ifdef __WXGTK__
        GAddress gaddress;
       _GAddress_Init_INET(&gaddress);
       GAddress_INET_SetHostName(&gaddress, m_ip.mb_str());
//       struct in_addr *iaddr;
//       iaddr = &(((struct sockaddr_in *)gaddress.m_addr)->sin_addr);
//       iaddr->s_addr = inet_addr(m_ip.mb_str());

      struct in_addr *addr;
      addr = &(((struct sockaddr_in *)gaddress.m_addr)->sin_addr);
      a = addr->s_addr;
#endif

#ifdef __WXMSW__
      a = inet_addr(m_ip.mb_str());
#endif
#ifndef __WXOSX__
      struct ip_mreq mreq;
      mreq.imr_multiaddr.s_addr = a;
      mreq.imr_interface.s_addr= INADDR_ANY;    // this should be the RX interface
                                                // gotten like inet_addr("192.168.37.99");

      bool bam = m_sock->SetOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&mreq, sizeof(mreq));
#endif
//http://ho.runcode.us/q/how-to-set-up-a-socket-for-udp-multicast-with-2-network-cards-present
    
      wxString msg;
#ifdef __WXOSX__
    bool bam = false;
#endif
      if(bam) {
            printf("Successfully added to multicast group \n");

            msg = _T("->gradar_pi: Successfully added to multicast group ");
      }
      else {
            printf("   Failed to add to multicast group \n");
            msg = _T("   ->gradar_pi: Failed to add to multicast group ");
      }

      msg.Append(m_ip);
      wxLogMessage(msg);

//      tmp = s->buffer_size;
//      if (setsockopt(udp_fd, SOL_SOCKET, SO_RCVBUF, &tmp, sizeof(tmp)) < 0) {

      wxIPV4address rx_addr;
      rx_addr.Hostname(_T("0.0.0.0"));          // any address will do here,
                                                // just so long as it looks like an IP


      //    The big while....
      bool not_done = true;
      int n_rx_once = 0;
      while((not_done))
      {
            if(TestDestroy())
            {
                  not_done = false;                               // smooth exit
                  goto thread_exit;
            }

            m_sock->RecvFrom(rx_addr, buf, sizeof(buf));
//       printf(" bytes read %d\n", m_sock->LastCount());

            if(m_sock->LastCount()) {
                  if(0 == n_rx_once) {
                        wxLogMessage(_T("->gradar_pi: First Packet Rx"));
                        n_rx_once++;
                  }
                  process_buffer();
            }

      }


thread_exit:
      g_thread_active = false;
      return 0;
}

void MulticastRXThread::process_buffer(void)
{
//  printf("process_buffer\n");
//      unsigned short packet_type = *(unsigned short *)(&buf[0]);

      packet_type_pkt packet;
      memcpy(&packet, buf, sizeof(packet_type_pkt));
      unsigned short packet_type = packet.packet_type;

      switch(packet_type)
      {
            case 0x2a3:
            {
                  g_scan_packets_per_tick++;

                  g_scanner_state = 4;  //scanner must be alive

                  if(g_radar_state == RADAR_TX_ACTIVE) {
                      radar_scanline_pkt packet;
                      memcpy(&packet, buf, sizeof(radar_scanline_pkt));

                      if((packet.scan_length_bytes > g_max_scan_length_bytes) || (g_scan_buf == 0)) {
                        free (g_scan_buf);
                        g_scan_buf = (unsigned char *)calloc(360 * packet.scan_length_bytes, 1);

                        g_max_scan_length_bytes = packet.scan_length_bytes;
                      }

                  //    Range
                      double range_nm = (packet.range_meters+1) / 1852.;

                  // Range change?
                      if(g_range_meters != packet.range_meters + 1)
                      {
                        printf("Scan Packet...\n Range: %g NM\n range_meters: %d\n bytes: %d\n bytes_1:%d\n",
                         range_nm, packet.range_meters, packet.scan_length_bytes, packet.scan_length_bytes_1);

                        if(g_scan_buf)
                              memset(g_scan_buf, 0, g_max_scan_length_bytes * 360);
                        g_sweep_count = 0;
                      }

                      g_range_meters = packet.range_meters + 1;
                      g_scale_meters = packet.scan_length_bytes_1 + 1;

                      if(packet.angle == 358) g_sweep_count++;


                      g_current_scan_length_bytes = packet.scan_length_bytes;

                      unsigned char *packet_data = buf + sizeof(radar_scanline_pkt) - 1;
                      unsigned char *dest_data = &g_scan_buf[packet.scan_length_bytes * packet.angle];
                      memcpy(dest_data, packet_data, packet.scan_length_bytes);

                  // if in full screen mode, take a snapshot of the scan data on last radial (358 deg)
                      if((g_updatemode == 1) && (packet.angle == 358) && (g_sweep_count >= 2)) {
                        if(packet.scan_length_bytes > g_static_max_scan_length_bytes) {
                              free(g_static_buf);
                              g_static_buf = (unsigned char *)calloc(360 * packet.scan_length_bytes, 1);
                              g_static_max_scan_length_bytes = packet.scan_length_bytes;
                        }

                        memcpy(g_static_buf, g_scan_buf, packet.scan_length_bytes * 360);
                        printf("Static copy\n");
                        g_static_timestamp = wxDateTime::Now();
                        g_static_scan_length_bytes = g_current_scan_length_bytes;
                        g_static_scale_meters = g_scale_meters;
                      }
                  }
                  break;


            }
            case 0x2a5:
            {
                  rad_status_pkt packet;
                  memcpy(&packet, buf, sizeof(rad_status_pkt));
                  g_scanner_state = packet.parm1;
                  g_warmup_timer = packet.parm2;
                  printf("  0x2a5:  state: %d\n", packet.parm1);
                  break;
            }

            default:
                  break;

      }

}



/* --------------------------------------------------------------------------------- */
/*
      Geodesic Forward and Reverse calculation functions
      Abstracted and adapted from PROJ-4.5.0 by David S.Register (bdbcat@yahoo.com)

      Original source code contains the following license:

      Copyright (c) 2000, Frank Warmerdam

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
*/
/* --------------------------------------------------------------------------------- */

static const double WGS84_semimajor_axis_meters       = 6378137.0;           // WGS84 semimajor axis
static const double mercator_k0                       = 0.9996;
static const double WGSinvf                           = 298.257223563;       /* WGS84 1/f */
#define DEGREE    (PI/180.0)



#define DTOL                 1e-12

#define HALFPI  1.5707963267948966
#define SPI     3.14159265359
#define TWOPI   6.2831853071795864769
#define ONEPI   3.14159265358979323846
#define MERI_TOL 1e-9

static double th1,costh1,sinth1,sina12,cosa12,M,N,c1,c2,D,P,s1;
static int merid, signS;

/*   Input/Output from geodesic functions   */
static double al12;           /* Forward azimuth */
static double al21;           /* Back azimuth    */
static double geod_S;         /* Distance        */
static double phi1, lam1, phi2, lam2;

static int ellipse;
static double geod_f;
static double geod_a;
static double es, onef, f, f64, f2, f4;

double adjlon (double lon) {
      if (fabs(lon) <= SPI) return( lon );
      lon += ONEPI;  /* adjust to 0..2pi rad */
      lon -= TWOPI * floor(lon / TWOPI); /* remove integral # of 'revolutions'*/
      lon -= ONEPI;  /* adjust back to -pi..pi rad */
      return( lon );
}



void geod_inv() {
      double      th1,th2,thm,dthm,dlamm,dlam,sindlamm,costhm,sinthm,cosdthm,
      sindthm,L,E,cosd,d,X,Y,T,sind,tandlammp,u,v,D,A,B;


            /*   Stuff the WGS84 projection parameters as necessary
      To avoid having to include <geodesic,h>
            */

      ellipse = 1;
      f = 1.0 / WGSinvf;       /* WGS84 ellipsoid flattening parameter */
      geod_a = WGS84_semimajor_axis_meters;

      es = 2 * f - f * f;
      onef = sqrt(1. - es);
      geod_f = 1 - onef;
      f2 = geod_f/2;
      f4 = geod_f/4;
      f64 = geod_f*geod_f/64;


      if (ellipse) {
            th1 = atan(onef * tan(phi1));
            th2 = atan(onef * tan(phi2));
      } else {
            th1 = phi1;
            th2 = phi2;
      }
      thm = .5 * (th1 + th2);
      dthm = .5 * (th2 - th1);
      dlamm = .5 * ( dlam = adjlon(lam2 - lam1) );
      if (fabs(dlam) < DTOL && fabs(dthm) < DTOL) {
            al12 =  al21 = geod_S = 0.;
            return;
      }
      sindlamm = sin(dlamm);
      costhm = cos(thm);      sinthm = sin(thm);
      cosdthm = cos(dthm);    sindthm = sin(dthm);
      L = sindthm * sindthm + (cosdthm * cosdthm - sinthm * sinthm)
                  * sindlamm * sindlamm;
      d = acos(cosd = 1 - L - L);
      if (ellipse) {
            E = cosd + cosd;
            sind = sin( d );
            Y = sinthm * cosdthm;
            Y *= (Y + Y) / (1. - L);
            T = sindthm * costhm;
            T *= (T + T) / L;
            X = Y + T;
            Y -= T;
            T = d / sind;
            D = 4. * T * T;
            A = D * E;
            B = D + D;
            geod_S = geod_a * sind * (T - f4 * (T * X - Y) +
                        f64 * (X * (A + (T - .5 * (A - E)) * X) -
                        Y * (B + E * Y) + D * X * Y));
            tandlammp = tan(.5 * (dlam - .25 * (Y + Y - E * (4. - X)) *
                        (f2 * T + f64 * (32. * T - (20. * T - A)
                        * X - (B + 4.) * Y)) * tan(dlam)));
      } else {
            geod_S = geod_a * d;
            tandlammp = tan(dlamm);
      }
      u = atan2(sindthm , (tandlammp * costhm));
      v = atan2(cosdthm , (tandlammp * sinthm));
      al12 = adjlon(TWOPI + v - u);
      al21 = adjlon(TWOPI - v - u);
}




void geod_pre(void) {

      /*   Stuff the WGS84 projection parameters as necessary
      To avoid having to include <geodesic,h>
      */
      ellipse = 1;
      f = 1.0 / WGSinvf;       /* WGS84 ellipsoid flattening parameter */
      geod_a = WGS84_semimajor_axis_meters;

      es = 2 * f - f * f;
      onef = sqrt(1. - es);
      geod_f = 1 - onef;
      f2 = geod_f/2;
      f4 = geod_f/4;
      f64 = geod_f*geod_f/64;

      al12 = adjlon(al12); /* reduce to  +- 0-PI */
      signS = fabs(al12) > HALFPI ? 1 : 0;
      th1 = ellipse ? atan(onef * tan(phi1)) : phi1;
      costh1 = cos(th1);
      sinth1 = sin(th1);
      if ((merid = fabs(sina12 = sin(al12)) < MERI_TOL)) {
            sina12 = 0.;
            cosa12 = fabs(al12) < HALFPI ? 1. : -1.;
            M = 0.;
      } else {
            cosa12 = cos(al12);
            M = costh1 * sina12;
      }
      N = costh1 * cosa12;
      if (ellipse) {
            if (merid) {
                  c1 = 0.;
                  c2 = f4;
                  D = 1. - c2;
                  D *= D;
                  P = c2 / D;
            } else {
                  c1 = geod_f * M;
                  c2 = f4 * (1. - M * M);
                  D = (1. - c2)*(1. - c2 - c1 * M);
                  P = (1. + .5 * c1 * M) * c2 / D;
            }
      }
      if (merid) s1 = HALFPI - th1;
      else {
            s1 = (fabs(M) >= 1.) ? 0. : acos(M);
            s1 =  sinth1 / sin(s1);
            s1 = (fabs(s1) >= 1.) ? 0. : acos(s1);
      }
}


void  geod_for(void) {
      double d,sind,u,V,X,ds,cosds,sinds,ss,de;

      ss = 0.;

      if (ellipse) {
            d = geod_S / (D * geod_a);
            if (signS) d = -d;
            u = 2. * (s1 - d);
            V = cos(u + d);
            X = c2 * c2 * (sind = sin(d)) * cos(d) * (2. * V * V - 1.);
            ds = d + X - 2. * P * V * (1. - 2. * P * cos(u)) * sind;
            ss = s1 + s1 - ds;
      } else {
            ds = geod_S / geod_a;
            if (signS) ds = - ds;
      }
      cosds = cos(ds);
      sinds = sin(ds);
      if (signS) sinds = - sinds;
      al21 = N * cosds - sinth1 * sinds;
      if (merid) {
            phi2 = atan( tan(HALFPI + s1 - ds) / onef);
            if (al21 > 0.) {
                  al21 = PI;
                  if (signS)
                        de = PI;
                  else {
                        phi2 = - phi2;
                        de = 0.;
                  }
            } else {
                  al21 = 0.;
                  if (signS) {
                        phi2 = - phi2;
                        de = 0;
                  } else
                        de = PI;
            }
      } else {
            al21 = atan(M / al21);
            if (al21 > 0)
                  al21 += PI;
            if (al12 < 0.)
                  al21 -= PI;
            al21 = adjlon(al21);
            phi2 = atan(-(sinth1 * cosds + N * sinds) * sin(al21) /
                        (ellipse ? onef * M : M));
            de = atan2(sinds * sina12 ,
                       (costh1 * cosds - sinth1 * sinds * cosa12));
            if (ellipse){
                  if (signS)
                        de += c1 * ((1. - c2) * ds +
                                    c2 * sinds * cos(ss));
                  else
                        de -= c1 * ((1. - c2) * ds -
                                    c2 * sinds * cos(ss));
            }
      }
      lam2 = adjlon( lam1 + de );
}







/* --------------------------------------------------------------------------------- */
/*
// Given the lat/long of starting point, and traveling a specified distance,
// at an initial bearing, calculates the lat/long of the resulting location.
// using elliptic earth model.
*/
/* --------------------------------------------------------------------------------- */

void ll_gc_ll(double lat, double lon, double brg, double dist, double *dlat, double *dlon)
{
    /*      Setup the static parameters  */
    phi1 = lat * DEGREE;            /* Initial Position  */
    lam1 = lon * DEGREE;
    al12 = brg * DEGREE;            /* Forward azimuth */
    geod_S = dist * 1852.0;         /* Distance        */

    geod_pre();
    geod_for();

    *dlat = phi2 / DEGREE;
    *dlon = lam2 / DEGREE;
}

void ll_gc_ll_reverse(double lat1, double lon1, double lat2, double lon2,
                     double *bearing, double *dist)
{
    /*      Setup the static parameters  */
    phi1 = lat1 * DEGREE;            /* Initial Position  */
    lam1 = lon1 * DEGREE;
    phi2 = lat2 * DEGREE;
    lam2 = lon2 * DEGREE;

    geod_inv();
    if(al12 < 0)
       al12 += 2*PI;

    if(bearing)
       *bearing = al12 / DEGREE;
    if(dist)
       *dist = geod_S / 1852.0;
}



/* --------------------------------------------------------------------------------- */
/*
// Given the lat/long of starting point and ending point,
// calculates the distance along a geodesic curve, using elliptic earth model.
*/
/* --------------------------------------------------------------------------------- */


double piDistGreatCircle(double slat, double slon, double dlat, double dlon)
{

      double d5;
      phi1 = slat * DEGREE;
      lam1 = slon * DEGREE;
      phi2 = dlat * DEGREE;
      lam2 = dlon * DEGREE;

      geod_inv();
      d5 = geod_S / 1852.0;

      return d5;
}

//---------------------------------------------------------------------------------------
//          Radar Control Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( GRadarDialog, wxDialog )

BEGIN_EVENT_TABLE ( GRadarDialog, wxDialog )

            EVT_CLOSE ( GRadarDialog::OnClose )
            EVT_BUTTON ( ID_OK, GRadarDialog::OnIdOKClick )
            EVT_MOVE ( GRadarDialog::OnMove )
            EVT_SIZE ( GRadarDialog::OnSize )

END_EVENT_TABLE()


GRadarDialog::GRadarDialog( )
{
//      printf("GRIBUIDialog ctor\n");
      Init();
}

GRadarDialog::~GRadarDialog( )
{
}


void GRadarDialog::Init( )
{
}


bool GRadarDialog::Create ( wxWindow *parent, gradar_pi *ppi, wxWindowID id,
                              const wxString& caption,
                              const wxPoint& pos, const wxSize& size, long style )
{

      pParent = parent;
      pPlugIn = ppi;


      //    As a display optimization....
      //    if current color scheme is other than DAY,
      //    Then create the dialog ..WITHOUT.. borders and title bar.
      //    This way, any window decorations set by external themes, etc
      //    will not detract from night-vision

      long wstyle = wxDEFAULT_FRAME_STYLE;
//      if ( ( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY ) && ( global_color_scheme != GLOBAL_COLOR_SCHEME_RGB ) )
//            wstyle |= ( wxNO_BORDER );

      wxSize size_min = size;
//      size_min.IncTo ( wxSize ( 500,600 ) );
      if ( !wxDialog::Create ( parent, id, caption, pos, size_min, wstyle ) )
            return false;


      CreateControls();

//      DimeWindow(this);

      Fit();
      SetMinSize(GetBestSize());

      return true;
}

void GRadarDialog::CreateControls()
{
      int border_size = 4;


// A top-level sizer
      wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );
      SetSizer ( topSizer );

// A second box sizer to give more space around the controls
      wxBoxSizer* boxSizer = new wxBoxSizer ( wxVERTICAL );
      topSizer->Add ( boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 2 );

//  Operating options
    wxStaticBox* BoxOpOptStatic = new wxStaticBox(this, wxID_ANY, _("Operational Control"));
    wxStaticBoxSizer* BoxSizerOpOpt = new wxStaticBoxSizer(BoxOpOptStatic, wxVERTICAL);
    boxSizer->Add(BoxSizerOpOpt, 0, wxEXPAND|wxALL, border_size);

    wxString OperatingModeStrings[] = {
        _("Master Mode"),
        _("Slave Mode"),
    };

    pOperatingMode = new wxRadioBox( this, ID_OMRADIOBOX, _("Operating Mode"),
                               wxDefaultPosition, wxDefaultSize,
                               2, OperatingModeStrings, 1, wxRA_SPECIFY_COLS );

    BoxSizerOpOpt->Add(pOperatingMode, 0, wxALL|wxEXPAND, 2);

    pOperatingMode->Connect(wxEVT_COMMAND_RADIOBOX_SELECTED,
                             wxCommandEventHandler(GRadarDialog::OnOperatingModeClick), NULL, this);
    if(g_bmaster)
      pOperatingMode->SetSelection(0);
    else
      pOperatingMode->SetSelection(1);


//  Display Options
    wxStaticBox* itemStaticBoxSizerDisOptStatic = new wxStaticBox(this, wxID_ANY, _("Display Options"));
    wxStaticBoxSizer* itemStaticBoxSizerDisOpt = new wxStaticBoxSizer(itemStaticBoxSizerDisOptStatic, wxVERTICAL);
    boxSizer->Add(itemStaticBoxSizerDisOpt, 0, wxEXPAND|wxALL, border_size);

    wxString DisplayUpdateStrings[] = {
        _("Swept Scan Update"),
        _("Full Scan Update"),
    };

    pUpdateMode = new wxRadioBox( this, ID_DORADIOBOX, _("Scan Update Method"),
                               wxDefaultPosition, wxDefaultSize,
                               2, DisplayUpdateStrings, 1, wxRA_SPECIFY_COLS );

    itemStaticBoxSizerDisOpt->Add(pUpdateMode, 0, wxALL|wxEXPAND, 2);

    pUpdateMode->Connect(wxEVT_COMMAND_RADIOBOX_SELECTED,
                             wxCommandEventHandler(GRadarDialog::OnUpdateModeClick), NULL, this);

    if(g_updatemode)
      pUpdateMode->SetSelection(1);
    else
      pUpdateMode->SetSelection(0);

      //Transparency slider
    wxStaticBox* transliderbox = new wxStaticBox(this, wxID_ANY, _("Transparency"));
    wxStaticBoxSizer* transliderboxsizer= new wxStaticBoxSizer(transliderbox, wxVERTICAL);
    itemStaticBoxSizerDisOpt->Add(transliderboxsizer, 0, wxALL|wxEXPAND, 2);

    pTranSlider = new wxSlider(this, ID_TRANSLIDER, 90 , 10, 100, wxDefaultPosition,  wxDefaultSize,
             wxSL_HORIZONTAL,  wxDefaultValidator, _("slider"));

    transliderboxsizer->Add(pTranSlider, 0, wxALL|wxEXPAND, 2);

    pTranSlider->Connect(wxEVT_SCROLL_THUMBRELEASE,
                              wxCommandEventHandler(GRadarDialog::OnUpdateTransSlider), NULL, this);

    pTranSlider->SetValue(g_overlay_transparency * 100);

    //  Log enable
    pCB_log = new wxCheckBox( this, ID_LOGENABLE, _("Enable gradar log"));
    boxSizer->Add(pCB_log, 1, wxALIGN_LEFT|wxALL, 2);

    pCB_log->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                             wxCommandEventHandler(GRadarDialog::OnLogModeClick), NULL, this);

    pCB_log->SetValue(b_enable_log);


#if 0
//      int check_spacing = 4;
      int group_item_spacing = 1;           // use for items within one group, with Add(...wxALL)


//    The GRIB directory
      wxStaticBox* itemStaticBoxSizer11Static = new wxStaticBox ( this, wxID_ANY,_ ( "GRIB File Directory" ) );
      wxStaticBoxSizer *itemStaticBoxSizer11 = new wxStaticBoxSizer ( itemStaticBoxSizer11Static, wxHORIZONTAL );
      boxSizer->Add ( itemStaticBoxSizer11, 0, wxEXPAND );

      m_pitemCurrentGribDirectoryCtrl = new wxTextCtrl (this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      itemStaticBoxSizer11->Add ( m_pitemCurrentGribDirectoryCtrl, 1, wxALIGN_LEFT|wxALL, 5 );

      m_pitemCurrentGribDirectoryCtrl->AppendText( m_currentGribDir );

      wxButton* bChooseDir = new wxBitmapButton ( this, ID_CHOOSEGRIBDIR, *m_pfolder_bitmap );
      itemStaticBoxSizer11->Add ( bChooseDir, 0, wxALIGN_RIGHT|wxALL, 5 );




//  The Tree control
      m_pRecordTree = new GribRecordTree ( this, ID_GRIBRECORDREECTRL, wxDefaultPosition, wxSize ( -1, 200 ), wxTR_HAS_BUTTONS );
      boxSizer->Add ( m_pRecordTree, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 2 );


      m_RecordTree_root_id = m_pRecordTree->AddRoot ( _T ( "GRIBs" ) );
      PopulateTreeControl();
      m_pRecordTree->Expand ( m_RecordTree_root_id );
      m_pRecordTree->SelectItem(m_RecordTree_root_id);

//      Data Box
      wxStaticBox* itemStaticBoxData = new wxStaticBox(this, wxID_ANY, _("GRIB Data"));
      wxStaticBoxSizer* itemStaticBoxSizerData= new wxStaticBoxSizer(itemStaticBoxData, wxVERTICAL);
      boxSizer->Add(itemStaticBoxSizerData, 0, wxALL|wxEXPAND, border_size);

      wxFlexGridSizer *pDataGrid = new wxFlexGridSizer(3);
      pDataGrid->AddGrowableCol(1);
      itemStaticBoxSizerData->Add(pDataGrid, 0, wxALL|wxEXPAND, border_size);


      m_cbWindSpeed.Create(this, ID_CB_WINDSPEED, _T(""));
      m_cbWindSpeed.SetValue(true);
      pDataGrid->Add(&m_cbWindSpeed, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxString wind_speed_str = _("Wind Speed, Kts.");
      if(pPlugIn->GetUseMS())
           wind_speed_str = _("Wind Speed, m/sec.");

      wxStaticText *ps1 = new wxStaticText(this, wxID_ANY, wind_speed_str);
      pDataGrid->Add(ps1, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pWindSpeedTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pWindSpeedTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);

      m_cbWindDir.Create(this, ID_CB_WINDDIR, _T(""));
      m_cbWindDir.SetValue(true);
      pDataGrid->Add(&m_cbWindDir, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxStaticText *ps2 = new wxStaticText(this, wxID_ANY, _("Wind Direction"));
      pDataGrid->Add(ps2, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pWindDirTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pWindDirTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);

      m_cbPress.Create(this, ID_CB_PRESS, _T(""));
      m_cbPress.SetValue(true);
      pDataGrid->Add(&m_cbPress, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxStaticText *ps3 = new wxStaticText(this, wxID_ANY, _("Pressure, mBar"));
      pDataGrid->Add(ps3, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pPressureTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pPressureTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);

      m_cbSigHw.Create(this, ID_CB_SIGHW, _T(""));
      m_cbSigHw.SetValue(true);
      pDataGrid->Add(&m_cbSigHw, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxStaticText *ps4 = new wxStaticText(this, wxID_ANY, _("Significant Wave Height, m"));
      pDataGrid->Add(ps4, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pSigWHTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pSigWHTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);


      m_cbSeaTmp.Create(this, ID_CB_SEATMP, _T(""));
      m_cbSeaTmp.SetValue(true);
      pDataGrid->Add(&m_cbSeaTmp, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxStaticText *ps5 = new wxStaticText(this, wxID_ANY, _("Sea Surface Temp, C"));
      pDataGrid->Add(ps5, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pSeaTmpTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pSeaTmpTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);

      m_cbSeaCurrent.Create(this, ID_CB_SEACURRENT, _T(""));
      m_cbSeaCurrent.SetValue(true);
      pDataGrid->Add(&m_cbSeaCurrent, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      wxStaticText *ps6 = new wxStaticText(this, wxID_ANY, _("Current Velocity, Kts."));
      pDataGrid->Add(ps6, 0, wxALIGN_LEFT|wxALL, group_item_spacing);

      m_pSeaCurrentTextCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
      pDataGrid->Add(m_pSeaCurrentTextCtrl, 0, wxALIGN_RIGHT, group_item_spacing);


#endif
// A horizontal box sizer to contain OK
      wxBoxSizer* AckBox = new wxBoxSizer ( wxHORIZONTAL );
      boxSizer->Add ( AckBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

//    Button color
//      wxColour button_color = GetGlobalColor ( _T ( "UIBCK" ) );;

// The OK button
      wxButton* bOK = new wxButton ( this, ID_OK, _( "&Close" ),
                                     wxDefaultPosition, wxDefaultSize, 0 );
      AckBox->Add ( bOK, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
}

void GRadarDialog::OnUpdateModeClick(wxCommandEvent &event)
{
      pPlugIn->SetUpdateMode(pUpdateMode->GetSelection());
}

void GRadarDialog::OnOperatingModeClick(wxCommandEvent &event)
{
      pPlugIn->SetOperatingMode(pOperatingMode->GetSelection());
}

void GRadarDialog::OnLogModeClick(wxCommandEvent &event)
{
      b_enable_log = pCB_log->GetValue();
}

void GRadarDialog::OnUpdateTransSlider(wxCommandEvent &event)
{
      g_overlay_transparency = ((double)pTranSlider->GetValue()) / 100.;
      pPlugIn->UpdateDisplayParameters();
}


void GRadarDialog::OnClose ( wxCloseEvent& event )
{
     pPlugIn->OnGRadarDialogClose();
}


void GRadarDialog::OnIdOKClick ( wxCommandEvent& event )
{
     pPlugIn->OnGRadarDialogClose();
      printf("IDOK\n");
     event.Skip();
}


void GRadarDialog::OnMove ( wxMoveEvent& event )
{
      //    Record the dialog position
      wxPoint p =  GetPosition();
      pPlugIn->SetGRadarDialogX(p.x);
      pPlugIn->SetGRadarDialogY(p.y);

      event.Skip();
}

void GRadarDialog::OnSize ( wxSizeEvent& event )
{
      //    Record the dialog size
      wxSize p = event.GetSize();
      pPlugIn->SetGRadarDialogSizeX(p.x);
      pPlugIn->SetGRadarDialogSizeY(p.y);

      event.Skip();
}







