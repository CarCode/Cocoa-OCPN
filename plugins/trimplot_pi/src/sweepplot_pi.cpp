/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  sweepplot Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2015 by Sean D'Epagnier                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#include <wx/wx.h>
#include <wx/stdpaths.h>

#include "sweepplot_pi.h"
#include "SweepPlotDialog.h"
#include "PreferencesDialog.h"
#include "icons.h"

History g_history[HISTORY_COUNT];

double heading_resolve(double degrees, double ref)
{
    while(degrees < ref-180)
        degrees += 360;
    while(degrees >= ref+180)
        degrees -= 360;
    return degrees;
}

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new sweepplot_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    SweepPlot PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

sweepplot_pi::sweepplot_pi(void *ppimgr)
    : opencpn_plugin_113(ppimgr)
{
    // Create the PlugIn icons
    initialize_images();
}

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

int sweepplot_pi::Init(void)
{
    AddLocaleCatalog( _T("opencpn-trimplot_pi") );

    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();

    m_Preferences = new PreferencesDialog(m_parent_window, *this);
    
    LoadConfig(); //    And load the configuration items

    // read history
    wxString data = StandardPath() + _T("data");
    History::Read(data);

    m_HistoryWriteTimer.Connect(wxEVT_TIMER, wxTimerEventHandler
                               ( sweepplot_pi::OnHistoryWriteTimer ), NULL, this);
    m_HistoryWriteTimer.Start(1000*60*20); // every 20 minutes

#ifdef SWEEPPLOT_USE_SVG
    m_leftclick_tool_id = InsertPlugInToolSVG( _T( "SweepPlot" ), _svg_sweepplot, _svg_sweepplot_rollover, _svg_sweepplot_toggled, wxITEM_CHECK, _( "SweepPlot" ), _T( "" ), NULL, SWEEPPLOT_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id  = InsertPlugInTool
        (_T(""), _img_sweepplot, _img_sweepplot, wxITEM_NORMAL,
         _("SweepPlot"), _T(""), NULL, SWEEPPLOT_TOOL_POSITION, 0, this);
#endif

    m_SweepPlotDialog = NULL;
    
    return (WANTS_OVERLAY_CALLBACK |
            WANTS_OPENGL_OVERLAY_CALLBACK |
            WANTS_TOOLBAR_CALLBACK    |
            WANTS_NMEA_SENTENCES   |
            WANTS_NMEA_EVENTS         |
//            WANTS_PREFERENCES         |
            WANTS_CONFIG);
}

bool sweepplot_pi::DeInit(void)
{
    SaveConfig();

    // write history
    WriteHistory();

    if (m_SweepPlotDialog)
    {
        m_SweepPlotDialog->Close();
        delete m_SweepPlotDialog;
        m_SweepPlotDialog = NULL;
    }

    delete m_Preferences;

    RemovePlugInTool(m_leftclick_tool_id);

    return true;
}

int sweepplot_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int sweepplot_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int sweepplot_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int sweepplot_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *sweepplot_pi::GetPlugInBitmap()
{
    return new wxBitmap(_img_sweepplot->ConvertToImage().Copy());
}

wxString sweepplot_pi::GetCommonName()
{
    return _("SweepPlot");
}


wxString sweepplot_pi::GetShortDescription()
{
    return _("SweepPlot PlugIn for OpenCPN");
}

wxString sweepplot_pi::GetLongDescription()
{
    return _("SweepPlot PlugIn for OpenCPN\n\
Plot speed and course over ground to make the result of \
small adjustments evident.");
}

int sweepplot_pi::GetToolbarToolCount(void)
{
    return 1;
}

void sweepplot_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_SweepPlotDialog)
        return;

    DimeWindow(m_SweepPlotDialog);
}

void sweepplot_pi::RearrangeWindow()
{
    if (NULL == m_SweepPlotDialog)
        return;

    SetColorScheme(PI_ColorScheme());
}

void sweepplot_pi::OnToolbarToolCallback(int id)
{
    if(!m_SweepPlotDialog)
    {
        m_SweepPlotDialog = new SweepPlotDialog(m_parent_window, *this, *m_Preferences);

        wxFileConfig *pConf = GetOCPNConfigObject();
        pConf->SetPath ( _T ( "/Settings/SweepPlot" ) );

        m_SweepPlotDialog->Move(pConf->Read ( _T ( "DialogPosX" ), 20L ),
                               pConf->Read ( _T ( "DialogPosY" ), 20L ));
        m_SweepPlotDialog->SetSize(pConf->Read ( _T ( "DialogW" ), 400L ),
                                  pConf->Read ( _T ( "DialogH" ), 300L ));

        wxIcon icon;
        icon.CopyFromBitmap(*_img_sweepplot);
        m_SweepPlotDialog->SetIcon(icon);
        m_Preferences->SetIcon(icon);
    }

    RearrangeWindow();
    m_SweepPlotDialog->Show(!m_SweepPlotDialog->IsShown());

    if(m_SweepPlotDialog->IsShown())
        m_SweepPlotDialog->SetupPlot();

    wxPoint p = m_SweepPlotDialog->GetPosition();
    m_SweepPlotDialog->Move(0, 0);        // workaround for gtk autocentre dialog behavior
    m_SweepPlotDialog->Move(p);
}

bool sweepplot_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    Render(&dc, *vp);
    return true;
}

bool sweepplot_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );

    Render(NULL, *vp);

    glDisable( GL_BLEND );
    glDisable( GL_LINE_SMOOTH );

    return true;
}

void sweepplot_pi::Render(wxDC *dc, PlugIn_ViewPort &vp)
{
    if(!m_Preferences->m_cbCoursePrediction->GetValue())
        return;

    int ticks = m_Preferences->m_sCoursePredictionSeconds->GetValue();
    int length = m_Preferences->m_sCoursePredictionLength->GetValue();

    double lat0, lon0, lat1, lon1;
    double brg, dist, dlat, dlon;
    wxPoint r0, r1, r2;

    if(m_Preferences->m_cbCoursePredictionBlended->GetValue()) {
        if(dc)
            return;

        if(!g_history[LAT].LastValue(lat0) ||
           !g_history[LON].LastValue(lon0))
            return;

        GetCanvasPixLL(&vp, &r0, lat0, lon0);

        glColor4f(1, 0, 0, 1.0/ticks);

        glBegin(GL_TRIANGLES);

        GetCanvasPixLL(&vp, &r0, lat0, lon0);

        bool first = true;

        for(int i=1; i<ticks; i++) {
            int t = i;
            if(!g_history[LAT].LastValue(lat1, t) ||
               !g_history[LON].LastValue(lon1, t))
                continue;

            DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, &brg, &dist);
            PositionBearingDistanceMercator_Plugin(lat0, lon0, brg, dist * length * 60.0 / t, &dlat, &dlon);
            GetCanvasPixLL(&vp, &r1, dlat, dlon);

            if(first)
                first = false;
            else {
                if(dc) {
//                dc->SetPen(wxPen(*wxRED, 3));
//                dc->DrawLine( r0.x, r0.y, r1.x, r1.y);
                } else {
                    glVertex2i(r0.x, r0.y);
                    glVertex2i(r1.x, r1.y);
                    glVertex2i(r2.x, r2.y);
                }
            }
            r2 = r1;
        }

        glEnd();
    } else {
        if(!g_history[LAT].LastValue(lat0) ||
           !g_history[LON].LastValue(lon0) ||
           !g_history[LAT].LastValue(lat1, ticks) ||
           !g_history[LON].LastValue(lon1, ticks))
            return;

        DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, &brg, &dist);
        PositionBearingDistanceMercator_Plugin(lat0, lon0, brg, dist * length * 60.0 / ticks, &dlat, &dlon);
        GetCanvasPixLL(&vp, &r0, lat0, lon0);
        GetCanvasPixLL(&vp, &r1, dlat, dlon);

        if(dc) {
            dc->SetPen(wxPen(*wxRED, 3));
            dc->DrawLine( r0.x, r0.y, r1.x, r1.y);
        } else {
            glColor3ub(255, 0, 0);
            glLineWidth(3.0);
            glBegin(GL_LINES);
            glVertex2i(r0.x, r0.y);
            glVertex2i(r1.x, r1.y);
            glEnd();
        }
    }
}

bool sweepplot_pi::LoadConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/Settings/SweepPlot" ) );

    return true;
}

bool sweepplot_pi::SaveConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();
    pConf->SetPath ( _T ( "/Settings/SweepPlot" ) );

    if(m_SweepPlotDialog) {
        wxPoint p = m_SweepPlotDialog->GetPosition();
        wxSize s = m_SweepPlotDialog->GetSize();

        pConf->Write ( _T ( "DialogPosX" ), p.x);
        pConf->Write ( _T ( "DialogPosY" ), p.y);
        pConf->Write ( _T ( "DialogW" ), s.x);
        pConf->Write ( _T ( "DialogH" ), s.y);
    }
    
    return true;
}

void sweepplot_pi::WriteHistory()
{
    wxString data = StandardPath() + _T("data");
    wxFileName fn = data;
    wxFileName fn2 = fn.GetPath();
    if(!fn.DirExists()) {
        fn2.Mkdir();
        fn.Mkdir();
    }

    History::Write(data);
}

wxString sweepplot_pi::StandardPath()
{
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXMSW__
    wxString stdPath  = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
    wxString stdPath  = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
    wxString stdPath  = std_path.GetUserConfigDir();   // should be ~/Library/Preferences	
#endif

    return stdPath + wxFileName::GetPathSeparator() +
        _T("opencpn/plugins") + wxFileName::GetPathSeparator() +
        _T("trimplot") +  wxFileName::GetPathSeparator();
}

void sweepplot_pi::SetNMEASentence( wxString &sentence )
{
    m_NMEA0183 << sentence;

    if( !m_NMEA0183.PreParse() )
        return;

    if( m_NMEA0183.LastSentenceIDReceived == _T("HDT") ) {
        if( m_NMEA0183.Parse() ) {
            if( !wxIsNaN(m_NMEA0183.Hdt.DegreesTrue) )
                AddData(HDG, m_NMEA0183.Hdt.DegreesTrue);
        }
    }
    // NMEA 0183 standard Wind Direction and Speed, with respect to north.
    else if( m_NMEA0183.LastSentenceIDReceived == _T("MWD") ) {
        if( m_NMEA0183.Parse() ) {
            // Option for True vs Magnetic
//            wxString windunit;
            if( m_NMEA0183.Mwd.WindAngleTrue < 999. ) { //if WindAngleTrue is available, use it ...
                AddData(TWD, m_NMEA0183.Mwd.WindAngleTrue);
                AddData(TWS, m_NMEA0183.Mwd.WindSpeedKnots);
            } else if( m_NMEA0183.Mwd.WindAngleMagnetic < 999. ) { //otherwise try WindAngleMagnetic ...
                // TODO: use wmm plugin to compensate to true wind
//                truewind = m_NMEA0183.Mwd.WindAngleMagnetic;
            }
        }
    }
}

void sweepplot_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    if(pfix.FixTime && pfix.nSats) {

        AddData(SOG, pfix.Sog);
        if(pfix.Sog > .35) // cog is very unreliable below this speed
            AddData(COG, pfix.Cog);

        AddData(LAT, pfix.Lat);
        AddData(LON, pfix.Lon);

        UpdatePositionDetermined(PDS10, PDC10, 10);
        UpdatePositionDetermined(PDS60, PDC60, 60);
    }
}

void sweepplot_pi::UpdatePositionDetermined(enum HistoryEnum speed, enum HistoryEnum course, int tick_diff)
{
    if(g_history[speed].LastTicks() + tick_diff > g_history[LAT].LastTicks())
        return;

    double lat0, lon0, lat1, lon1;
    int actual_ticks = tick_diff;
    if(!g_history[LAT].LastValue(lat0) ||
       !g_history[LON].LastValue(lon0) ||
       !g_history[LAT].LastValue(lat1, actual_ticks) ||
       !g_history[LON].LastValue(lon1, actual_ticks))
        return;

    if(abs(actual_ticks - tick_diff) > 2) // invalidate if we aren't getting continuous enough data
        return;

    double brg, dist;
    DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, &brg, &dist);

    time_t ticks = wxDateTime::Now().GetTicks() - actual_ticks/2;
    AddData(speed, dist * 3600.0 / actual_ticks, ticks);
    AddData(course, brg, ticks);
}

void sweepplot_pi::AddData(enum HistoryEnum e, double value, time_t ticks)
{
    const int resolve[] = {TWD, TWA, AWA, COG, HDG, PDC10, PDC60};
    bool bresolve = false;
    for(unsigned int i=0; i < (sizeof resolve) / (sizeof *resolve); i++)
        if(resolve[i])
            bresolve = true;
    g_history[e].AddData(value, ticks, bresolve);
}

void sweepplot_pi::OnHistoryWriteTimer( wxTimerEvent & )
{
    WriteHistory();
}
