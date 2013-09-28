/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  watch man Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Sean D'Epagnier                                 *
 *   sean at depagnier dot com                                             *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/process.h"

#include "../../../include/ocpndc.h"

#include "watchman_pi.h"
#include "WatchmanDialog.h"
#include "WatchmanPrefsDialog.h"
#include "icons.h"


double heading_resolve(double degrees)
{
    while(degrees < -180)
        degrees += 360;
    while(degrees >= 180)
        degrees -= 360;
    return degrees;
}

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new watchman_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    Watchman PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

watchman_pi::watchman_pi(void *ppimgr)
    : opencpn_plugin_18(ppimgr),
      m_iAlarm(0), m_iLastAlarm(0)
{
    // Create the PlugIn icons
    initialize_images();
    m_lastfix.Lat = NAN;
    m_lasttimerfix.Lat = NAN;
    sog = cog = NAN;
}

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

int watchman_pi::Init(void)
{
    AddLocaleCatalog( _T("opencpn-watchman_pi") );

    // Set some default private member parameters
    m_watchman_dialog_x = 0;
    m_watchman_dialog_y = 0;

    ::wxDisplaySize(&m_display_width, &m_display_height);
    
    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();
    m_pconfig = GetOCPNConfigObject(); //    Get opencpn configuration object
    
    LoadConfig(); //    And load the configuration items
    
    m_leftclick_tool_id  = InsertPlugInTool
        (_T(""), _img_watchman, _img_watchman, wxITEM_NORMAL,
         _("Watchman"), _T(""), NULL, WATCHMAN_TOOL_POSITION, 0, this);
    
    wxMenu dummy_menu;
    m_anchor_menu_id = AddCanvasContextMenuItem
        (new wxMenuItem(&dummy_menu, -1, _("Watchman Anchor Watch")), this );
    
    m_pWatchmanDialog = NULL;
    
    m_Timer.Connect(wxEVT_TIMER, wxTimerEventHandler
                    ( watchman_pi::OnTimer ), NULL, this);
    m_Timer.Start(3000);

    wxDateTime now = wxDateTime::Now();
    m_LastAlarmTime = now;
    m_DeadmanUpdateTime = now;
    m_LastLandFallCheck = now;

    return (WANTS_OVERLAY_CALLBACK |
            WANTS_OPENGL_OVERLAY_CALLBACK |
            WANTS_TOOLBAR_CALLBACK    |
            WANTS_CURSOR_LATLON       |
            WANTS_PREFERENCES         |
            WANTS_NMEA_EVENTS         |
            WANTS_AIS_SENTENCES       |
            WANTS_CONFIG);
}

bool watchman_pi::DeInit(void)
{
    //    Record the dialog position
    if (m_pWatchmanDialog)
    {
        wxPoint p = m_pWatchmanDialog->GetPosition();
        SetWatchmanDialogX(p.x);
        SetWatchmanDialogY(p.y);
        
        m_pWatchmanDialog->Close();
        delete m_pWatchmanDialog;
        m_pWatchmanDialog = NULL;
    }
    SaveConfig();

    RemovePlugInTool(m_leftclick_tool_id);

    return true;
}

int watchman_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int watchman_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int watchman_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int watchman_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *watchman_pi::GetPlugInBitmap()
{
    return _img_watchman;
}

wxString watchman_pi::GetCommonName()
{
    return _("Watchman");
}


wxString watchman_pi::GetShortDescription()
{
    return _("Watchman PlugIn for OpenCPN");
}

wxString watchman_pi::GetLongDescription()
{
    return _("Watchman PlugIn for OpenCPN\n\
Alarm user of possible dangerous situations. \n\
\n\
The Watchman plugin was written by Sean D'Epagnier\n\
");
}

int watchman_pi::GetToolbarToolCount(void)
{
    return 1;
}

void watchman_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_pWatchmanDialog)
        return;
#ifndef __WXOSX__
    DimeWindow(m_pWatchmanDialog);
#endif
}

void watchman_pi::RearrangeWindow()
{
    if (NULL == m_pWatchmanDialog)
        return;

    SetColorScheme(PI_ColorScheme());
    
    m_pWatchmanDialog->Fit();
}

void watchman_pi::OnToolbarToolCallback(int id)
{
    if(!m_pWatchmanDialog)
    {
        m_pWatchmanDialog = new WatchmanDialog(*this, m_parent_window);
        m_pWatchmanDialog->Move(wxPoint(m_watchman_dialog_x, m_watchman_dialog_y));
    }

    m_pWatchmanDialog->Show(!m_pWatchmanDialog->IsShown());
    m_pWatchmanDialog->UpdateAlarms();

    wxPoint p = m_pWatchmanDialog->GetPosition();
    m_pWatchmanDialog->Move(0, 0);        // workaround for gtk autocentre dialog behavior
    m_pWatchmanDialog->Move(p);
}

void watchman_pi::OnContextMenuItemCallback(int id)
{
    if(id == m_anchor_menu_id) {
        m_bAnchor = true;
        m_dAnchorLatitude = m_cursor_lat;
        m_dAnchorLongitude = m_cursor_lon;
    }
}

wxColour watchman_pi::Color(enum Alarm alarm_mask)
{
    if(m_iAlarm & alarm_mask)
        return *wxRED;
    else if(m_iLastAlarm & alarm_mask)
        return wxColor(200, 200, 0); // darker yellow

    return *wxGREEN;
}

bool watchman_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    ocpnDC odc(dc);
    Render(odc, *vp);
    return true;
}

bool watchman_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT ); //Save state
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    ocpnDC odc;
    Render(odc, *vp);

    glPopAttrib();
    return true;
}

void watchman_pi::Render(ocpnDC &dc, PlugIn_ViewPort &vp)
{
    if(m_bAnchor) {
        wxPoint r1, r2;
        GetCanvasPixLL(&vp, &r1, m_dAnchorLatitude, m_dAnchorLongitude);
        GetCanvasPixLL(&vp, &r2, m_dAnchorLatitude+m_iAnchorRadius/1853.0/60.0,
                       m_dAnchorLongitude);

        dc.SetPen(wxPen(Color(ANCHOR), 2));
        dc.DrawCircle( r1.x, r1.y, r2.y - r1.y );
    }

    if(!m_pWatchmanDialog || !m_pWatchmanDialog->IsShown())
        return;
}

void watchman_pi::OnTimer( wxTimerEvent & )
{
    wxDateTime now = wxDateTime::Now();

    m_iAlarm = 0;
    if(m_bLandFall) {
        /* kind of slow, don't perform as often */
        if((now - m_LastLandFallCheck).GetSeconds() > 10) {
            m_LastLandFallCheck = now;
            for(double t = 0; t<360; t+=18) {
                double dlat, dlon;
                PositionBearingDistanceMercator_Plugin(m_lastfix.Lat, m_lastfix.Lon, t,
                                                       m_dLandFallDistance, &dlat, &dlon);
            
                if(PlugIn_GSHHS_CrossesLand(m_lastfix.Lat, m_lastfix.Lon, dlat, dlon)) {
                    m_iAlarm |= LANDFALL;
                    break;
                }
            }
        }
    }

    double anchordist;
    DistanceBearingMercator_Plugin(m_lastfix.Lat, m_lastfix.Lon, m_dAnchorLatitude,
                                   m_dAnchorLongitude, 0, &anchordist);
    anchordist *= 1853.248; /* in meters */
    if(m_bAnchor && anchordist > m_iAnchorRadius)
        m_iAlarm |= ANCHOR;

    if(m_bDeadman && (now - m_DeadmanUpdateTime) > m_DeadmanSpan)
        m_iAlarm |= DEADMAN;

    double gpsseconds = m_LastPositionFix.IsValid() ?
        (now - m_LastPositionFix).GetSeconds().ToLong() : NAN;
    if(m_bGPSAlarm && gpsseconds > m_dGPSSeconds)
        m_iAlarm |= GPS;

    double aisseconds = m_LastAISSentence.IsValid() ? 
        (now - m_LastAISSentence).GetSeconds().ToLong() : NAN;
    if(m_bAISAlarm && aisseconds > m_dAISSeconds)
        m_iAlarm |= AIS;

    double courseerror = NAN;
    if(m_LastPositionFix.IsValid()) {
        if(!m_LastTimerFix.IsValid())
            goto setfix;

        if((m_LastPositionFix - m_LastTimerFix).GetSeconds() > 10) {
            if(!isnan(m_lastfix.Lat) && !isnan(m_lasttimerfix.Lat)) {
                DistanceBearingMercator_Plugin(m_lastfix.Lat, m_lastfix.Lon,
                                               m_lasttimerfix.Lat, m_lasttimerfix.Lon, &cog, &sog);
                /* this way helps avoid surge speed from gps from surfing waves etc... */
                sog *= 3600.0 / (m_lastfix.FixTime - m_lasttimerfix.FixTime);
            } else
                sog = cog = NAN;

        setfix:
            m_lasttimerfix = m_lastfix;
            m_LastTimerFix = m_LastPositionFix;
        }
    }

    if(m_bUnderSpeedAlarm && sog < m_dUnderSpeed)
        m_iAlarm |= UNDERSPEED;

    if(m_bOverSpeedAlarm && sog > m_dOverSpeed)
        m_iAlarm |= OVERSPEED;
#ifdef __WXOSX__
      courseerror = heading_resolve(cog - m_dCourseDegrees);
#else
    courseerror = fabs(heading_resolve(cog - m_dCourseDegrees));
#endif
    if(m_bOffCourseAlarm && courseerror > m_dOffCourseDegrees)
        m_iAlarm |= OFFCOURSE;

    if(m_iAlarm)
        Alarm();

    if(m_pWatchmanDialog) {
        m_pWatchmanDialog->UpdateLandFallTime(m_lastfix);
        m_pWatchmanDialog->UpdateAnchorDistance(anchordist);
        m_pWatchmanDialog->UpdateGPSTime(gpsseconds);
        m_pWatchmanDialog->UpdateAISTime(aisseconds);
        m_pWatchmanDialog->UpdateUnderSpeed(sog);
        m_pWatchmanDialog->UpdateOverSpeed(sog);
        m_pWatchmanDialog->UpdateCourseError(courseerror);
    }
}

void watchman_pi::ResetDeadman()
{
    m_DeadmanUpdateTime = wxDateTime::Now();
}

bool watchman_pi::LoadConfig(void)
{
    wxFileConfig *pConf = m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/Settings/Watchman" ) );
    
    m_watchman_dialog_x =  pConf->Read ( _T ( "DialogPosX" ), 20L );
    m_watchman_dialog_y =  pConf->Read ( _T ( "DialogPosY" ), 20L );

    pConf->Read ( _T ( "LandFallAlarm" ), &m_bLandFall, 0 );
    pConf->Read ( _T ( "LandFallDistance" ), &m_dLandFallDistance, 3 );
    
    pConf->Read ( _T ( "DeadmanAlarm" ), &m_bDeadman, 0 );
    int deadmanminutes;
    pConf->Read ( _T ( "DeadmanMinutes" ), &deadmanminutes, 20 );
    m_DeadmanSpan = wxTimeSpan::Minutes(deadmanminutes);
    
    pConf->Read ( _T ( "AnchorAlarm" ), &m_bAnchor, 0 );
    pConf->Read ( _T ( "AnchorLatitude" ), &m_dAnchorLatitude, NAN );
    pConf->Read ( _T ( "AnchorLongitude" ), &m_dAnchorLongitude, NAN );
    pConf->Read ( _T ( "AnchorRadius" ), &m_iAnchorRadius, 50 );

    pConf->Read ( _T ( "GPSAlarm" ), &m_bGPSAlarm, 0 );
    pConf->Read ( _T ( "GPSSeconds" ), &m_dGPSSeconds, 10 );
    pConf->Read ( _T ( "AISAlarm" ), &m_bAISAlarm, 0 );
    pConf->Read ( _T ( "AISSeconds" ), &m_dAISSeconds, 100 );

    pConf->Read ( _T ( "UnderSpeedAlarm" ), &m_bUnderSpeedAlarm, 0 );
    pConf->Read ( _T ( "UnderSpeed" ), &m_dUnderSpeed, 1 );
    pConf->Read ( _T ( "OverSpeedAlarm" ), &m_bOverSpeedAlarm, 0 );
    pConf->Read ( _T ( "OverSpeed" ), &m_dOverSpeed, 10 );

    pConf->Read ( _T ( "OffCourseAlarm" ), &m_bOffCourseAlarm, 0 );
    pConf->Read ( _T ( "OffCourseDegrees" ), &m_dOffCourseDegrees, 20 );
    pConf->Read ( _T ( "CourseDegrees" ), &m_dCourseDegrees, 0 );
    
    pConf->Read ( _T ( "SoundEnabled" ), &m_bSound, 0 );
    pConf->Read ( _T ( "SoundFilepath" ), &m_sSound, _T("") );
    pConf->Read ( _T ( "CommandEnabled" ), &m_bCommand, 0 );
    pConf->Read ( _T ( "CommandFilepath" ), &m_sCommand, _T("") );
    pConf->Read ( _T ( "MessageBox" ), &m_bMessageBox, 0);
    pConf->Read ( _T ( "RepeatSeconds" ), &m_iRepeatSeconds, 60);
    pConf->Read ( _T ( "AutoReset" ), &m_bAutoReset, 0);

    return true;
}

bool watchman_pi::SaveConfig(void)
{
    wxFileConfig *pConf = m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T ( "/Settings/Watchman" ) );

    pConf->Write ( _T ( "DialogPosX" ),   m_watchman_dialog_x );
    pConf->Write ( _T ( "DialogPosY" ),   m_watchman_dialog_y );

    pConf->Write ( _T ( "LandFallAlarm" ), m_bLandFall );
    pConf->Write ( _T ( "LandFallDistance" ), m_dLandFallDistance);
    
    pConf->Write ( _T ( "DeadmanAlarm" ), m_bDeadman );
    pConf->Write ( _T ( "DeadmanMinutes" ), m_DeadmanSpan.GetMinutes() );
    
    pConf->Write ( _T ( "AnchorAlarm" ), m_bAnchor );
    pConf->Write ( _T ( "AnchorLatitude" ), m_dAnchorLatitude );
    pConf->Write ( _T ( "AnchorLongitude" ), m_dAnchorLongitude );
    pConf->Write ( _T ( "AnchorRadius" ), m_iAnchorRadius );
    
    pConf->Write ( _T ( "GPSAlarm" ), m_bGPSAlarm );
    pConf->Write ( _T ( "GPSSeconds" ), m_dGPSSeconds );
    pConf->Write ( _T ( "AISAlarm" ), m_bAISAlarm );
    pConf->Write ( _T ( "AISSeconds" ), m_dAISSeconds );

    pConf->Write ( _T ( "UnderSpeedAlarm" ), m_bUnderSpeedAlarm );
    pConf->Write ( _T ( "UnderSpeed" ), m_dUnderSpeed );
    pConf->Write ( _T ( "OverSpeedAlarm" ), m_bOverSpeedAlarm );
    pConf->Write ( _T ( "OverSpeed" ), m_dOverSpeed );

    pConf->Write ( _T ( "OffCourseAlarm" ), m_bOffCourseAlarm );
    pConf->Write ( _T ( "OffCourseDegrees" ), m_dOffCourseDegrees );
    pConf->Write ( _T ( "CourseDegrees" ), m_dCourseDegrees );

    pConf->Write ( _T ( "SoundEnabled" ), m_bSound);
    pConf->Write ( _T ( "SoundFilepath" ), m_sSound);
    pConf->Write ( _T ( "CommandEnabled" ), m_bCommand);
    pConf->Write ( _T ( "CommandFilepath" ), m_sCommand);
    pConf->Write ( _T ( "MessageBox" ), m_bMessageBox);
    pConf->Write ( _T ( "RepeatSeconds" ), m_iRepeatSeconds);
    pConf->Write ( _T ( "AutoReset" ), m_bAutoReset);
    
    return true;
}

void watchman_pi::RunAlarm(wxString sound, wxString command, bool mb)
{
    if(sound.size())
        PlugInPlaySound(sound);

    if(command.size())
        wxProcess::Open(command);

    if(mb) {
        wxMessageDialog mdlg(m_parent_window, _("ALARM!!!!"),
                             _("Watchman"), wxOK | wxICON_ERROR);
        mdlg.ShowModal();
    }
}

void watchman_pi::ResetLastAlarm()
{
    m_iLastAlarm = 0;
}

void watchman_pi::Alarm()
{
    wxDateTime now = wxDateTime::Now();

    if(m_iLastAlarm == m_iAlarm) {
        if((now - m_LastAlarmTime).GetSeconds() < m_iRepeatSeconds || !m_iRepeatSeconds)
            return;
    } else
        if(!m_bAutoReset)
            m_iLastAlarm |= m_iAlarm;

    m_LastAlarmTime = now;

    ResetDeadman();

    RunAlarm(m_bSound ? m_sSound : _T(""),
             m_bCommand ? m_sCommand : _T(""),
             m_bMessageBox);
}

void watchman_pi::SetCursorLatLon(double lat, double lon)
{
    ResetDeadman();

    m_cursor_lat = lat;
    m_cursor_lon = lon;
}

void watchman_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    if(pfix.FixTime && pfix.nSats)
        m_LastPositionFix = wxDateTime::Now();

    m_lastfix = pfix;
}

void watchman_pi::SetAISSentence(wxString &)
{
    m_LastAISSentence = wxDateTime::Now();
}

void watchman_pi::ShowPreferencesDialog( wxWindow* parent )
{
    WatchmanPrefsDialog dialog(*this, parent);

    dialog.m_cbLandFall->SetValue(m_bLandFall);
    dialog.m_tcLandFallDistance->SetValue(wxString::Format(_T("%.2f"),
                                                            m_dLandFallDistance));
    dialog.m_cbDeadman->SetValue(m_bDeadman);
    dialog.m_sDeadmanMinutes->SetValue(m_DeadmanSpan.GetMinutes());

    dialog.m_cbAnchor->SetValue(m_bAnchor);
    dialog.m_tAnchorLatitude->SetValue(wxString::Format(_T("%f"), m_dAnchorLatitude));
    dialog.m_tAnchorLongitude->SetValue(wxString::Format(_T("%f"), m_dAnchorLongitude));
    dialog.m_sAnchorRadius->SetValue(m_iAnchorRadius);

    dialog.m_cbGPSAlarm->SetValue(m_bGPSAlarm);
    dialog.m_sGPSSeconds->SetValue(m_dGPSSeconds);
    dialog.m_cbAISAlarm->SetValue(m_bAISAlarm);
    dialog.m_sAISSeconds->SetValue(m_dAISSeconds);

    dialog.m_cbUnderSpeed->SetValue(m_bUnderSpeedAlarm);
    dialog.m_tUnderSpeed->SetValue(wxString::Format(_T("%.1f"), m_dUnderSpeed));
    dialog.m_cbOverSpeed->SetValue(m_bOverSpeedAlarm);
    dialog.m_tOverSpeed->SetValue(wxString::Format(_T("%.1f"), m_dOverSpeed));

    dialog.m_cbOffCourseAlarm->SetValue(m_bOffCourseAlarm);
    dialog.m_sOffCourseDegrees->SetValue(m_dOffCourseDegrees);
    dialog.m_sCourseDegrees->SetValue(m_dCourseDegrees);

    dialog.m_cbSound->SetValue(m_bSound);
    dialog.m_fpSound->SetPath(m_sSound);
    dialog.m_cbCommand->SetValue(m_bCommand);
    dialog.m_tCommand->SetValue(m_sCommand);
    dialog.m_cbMessageBox->SetValue(m_bMessageBox);
    dialog.m_cbRepeat->SetValue(m_iRepeatSeconds);
    dialog.m_sRepeatSeconds->SetValue(m_iRepeatSeconds);
    
    dialog.Fit();
    wxColour cl;
    GetGlobalColor(_T("DILG1"), &cl);
    dialog.SetBackgroundColour(cl);
    
    if(dialog.ShowModal() == wxID_OK)
    {
        m_bLandFall = dialog.m_cbLandFall->GetValue();
        dialog.m_tcLandFallDistance->GetValue().ToDouble(&m_dLandFallDistance);

        m_bDeadman = dialog.m_cbDeadman->GetValue();
        m_DeadmanSpan = wxTimeSpan::Minutes(dialog.m_sDeadmanMinutes->GetValue());

        m_bAnchor = dialog.m_cbAnchor->GetValue();
        dialog.m_tAnchorLatitude->GetValue().ToDouble(&m_dAnchorLatitude);
        dialog.m_tAnchorLongitude->GetValue().ToDouble(&m_dAnchorLongitude);
        m_iAnchorRadius = dialog.m_sAnchorRadius->GetValue();

        m_bGPSAlarm = dialog.m_cbGPSAlarm->GetValue();
        m_dGPSSeconds = dialog.m_sGPSSeconds->GetValue();
        m_bAISAlarm = dialog.m_cbAISAlarm->GetValue();
        m_dAISSeconds = dialog.m_sAISSeconds->GetValue();

        m_bUnderSpeedAlarm = dialog.m_cbUnderSpeed->GetValue();
        dialog.m_tUnderSpeed->GetValue().ToDouble(&m_dUnderSpeed);
        m_bOverSpeedAlarm = dialog.m_cbOverSpeed->GetValue();
        dialog.m_tOverSpeed->GetValue().ToDouble(&m_dOverSpeed);

        m_bOffCourseAlarm = dialog.m_cbOffCourseAlarm->GetValue();
        m_dOffCourseDegrees = dialog.m_sOffCourseDegrees->GetValue();
        m_dCourseDegrees = dialog.m_sCourseDegrees->GetValue();

        m_bSound = dialog.m_cbSound->GetValue();
        m_sSound = dialog.m_fpSound->GetPath();
        m_bCommand = dialog.m_cbCommand->GetValue();
        m_sCommand = dialog.m_tCommand->GetValue();
        m_bMessageBox = dialog.m_cbMessageBox->GetValue();
        m_iRepeatSeconds = dialog.m_cbRepeat->GetValue() ? dialog.m_sRepeatSeconds->GetValue() : 0;

        if(m_pWatchmanDialog)
            m_pWatchmanDialog->UpdateAlarms();

        SaveConfig();
    }

    RequestRefresh(m_parent_window);
}
