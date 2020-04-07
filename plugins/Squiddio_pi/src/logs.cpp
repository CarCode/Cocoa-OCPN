/* *************************************************************************
 * Project:  OpenCPN
 * Purpose:  Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2020 by Mauro Calvi                                     *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#include "wx/hashmap.h"
#endif //precompiled headers

#include "logs.h"

#define SECOND 1
#define MINUTE 60
#define HOUR 3600
#define DAY 86400

BEGIN_EVENT_TABLE(logsWindow, wxWindow)
    EVT_TIMER(TIMER_ID0,  logsWindow::OnRecTimerTimeout)
    EVT_TIMER(TIMER_ID1,  logsWindow::OnSenTimerTimeout)
    EVT_TIMER(TIMER_ID2,  logsWindow::OnRefreshTimeout)
    EVT_PAINT (logsWindow::OnPaint)
END_EVENT_TABLE();

logsWindow::logsWindow(squiddio_pi * plugin, wxWindow *pparent, wxWindowID id) :
        wxWindow(pparent, id, wxPoint(10, 200), wxSize(1000, 25),
                wxSIMPLE_BORDER, _T("OpenCPN PlugIn")) {

    this->SetTransparent(30);
    p_plugin = plugin;
    m_parent_window = pparent;
    m_pRecTimer = new wxTimer(this, TIMER_ID0);      // logs receive timer
    m_pSenTimer = new wxTimer(this, TIMER_ID1);      // logs send timer
    m_pRefreshTimer = new wxTimer(this, TIMER_ID2);  // provide heartbeat for window refresh and internet connection check
    m_pRefreshTimer->Start(5000);

    m_LogsLayer = NULL;
    m_ErrorCondition = wxEmptyString;
    m_Notice = wxEmptyString;
    g_RetrieveSecs = period_secs(p_plugin->g_RetrievePeriod);
//     g_SendSecs     = 10;  // test
    g_SendSecs     = period_secs(p_plugin->g_PostPeriod);
    m_nmea_ready   = false;
    m_LogsFilePath = p_plugin->layerdir;
    p_plugin->appendOSDirSlash(&m_LogsFilePath);
    m_LogsFilePath.Append(_T("logs.gpx"));

    m_NmeaFileName = p_plugin->layerdir + wxFileName::GetPathSeparator() + _("nmea.txt");
#ifdef __WXOSX__
    m_NmeaFile.Open(m_NmeaFileName, wxFile::write_append);
#else
    bool ok = m_NmeaFile.Open(m_NmeaFileName, wxFile::write_append);
#endif
    m_pauimgr = GetFrameAuiManager();
    m_pauimgr->Connect( wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler( logsWindow::OnClose ),
                    NULL, this );

    if (g_RetrieveSecs > 0)  // display friends' logs
    {
        // At launch, schedule the next update based on elapsed time since last update.
        // If update is overdue, delay by a few seconds to prevent get request from interfering with opencpn launch,
        DisplayLogsLayer();
        int nextRecEvent = g_RetrieveSecs - (wxDateTime::Now().GetTicks() - p_plugin->g_LastLogsRcvd);
            SetRecTimer(wxMax(wxMin(nextRecEvent, g_RetrieveSecs), 7));
        }

        if (g_SendSecs > 0 )
        {
            // see note for g_RetrieveSecs
            int nextSenEvent = g_SendSecs - (wxDateTime::Now().GetTicks() - p_plugin->g_LastLogSent);
            SetSenTimer(wxMax(wxMin(nextSenEvent, g_SendSecs), 5));
    }
}

logsWindow::~logsWindow(){
    delete m_pRecTimer;
    delete m_pSenTimer;
    delete m_pRefreshTimer;
    m_NmeaFile.Close();
}

wxString logsWindow::timeAgo(wxDateTime currTime) {
    int delta = wxDateTime::Now().GetTicks() - currTime.GetTicks();
    wxString timeString;

    if (delta == 0) return _("Just now");
    else if (delta == 1) return _("One second ago");
    else if (delta < MINUTE)
    {
        timeString.Printf(_("%i seconds ago"), delta);
        return timeString;
    }
    else if (delta < 2 * MINUTE) return _("About a minute ago");
    else if (delta < 45 * MINUTE)
    {
        timeString.Printf(_("%i minutes ago"), delta/MINUTE);
        return timeString;
    }
    else if (delta < 90 * MINUTE) return _("About an hour ago");
    else if (delta < DAY)
    {
        timeString.Printf(_("%i hours ago"), delta/HOUR);
        return timeString;
    }
    else if (delta < 48 * HOUR) return _("Yesterday");
    else if (delta < 365 * DAY)
    {
        timeString.Printf(_("%i days ago"), delta/DAY);
        return timeString;
    } else return wxEmptyString;
}

void logsWindow::SetRecTimer(int RetrieveSecs) {
    m_pRecTimer->Stop();
    if (RetrieveSecs > 0)
        m_pRecTimer->Start(RetrieveSecs * 1000);
    Refresh(false);
}

void logsWindow::SetSenTimer(int SendSecs) {
    m_pSenTimer->Stop();
    if (SendSecs > 0)
        m_pSenTimer->Start(SendSecs * 1000);
    Refresh(false);
}

void logsWindow::OnRecTimerTimeout(wxTimerEvent& event) {
    if (p_plugin->CheckIsOnline()) {
        RequestRefresh(m_parent_window);
        ShowFriendsLogs();
        if (m_pRecTimer->GetInterval() / 1000 < g_RetrieveSecs)
            SetRecTimer(g_RetrieveSecs);
    }
    Refresh(false);
}

void logsWindow::OnSenTimerTimeout(wxTimerEvent& event) {
    if (p_plugin->CheckIsOnline()) {
        RequestRefresh(m_parent_window);
        if (m_nmea_ready) {

            wxString PostResponse;
            PostResponse = PostPosition(mLat, mLon, mSog, mCog);

            wxJSONReader r;
            wxJSONValue v;
            r.Parse(PostResponse, &v);

            if (v[_T("error")].AsString() != _T("null") )
            {
                m_ErrorCondition = v[_T("error")].AsString();
                Refresh(false);
            } else {
                if (v[_T("notice")].AsString() != _T("null") )
                    { m_Notice = v[_T("notice")].AsString();}
                else
                    { m_Notice = wxEmptyString;}
                p_plugin->g_LastLogSent = wxDateTime::Now().GetTicks(); //will be saved in config file
                m_nmea_ready = false;
                m_ErrorCondition = wxEmptyString;
                Refresh(false);
            }

            if (m_pSenTimer->GetInterval() / 1000 < g_SendSecs)
                // if timer was set initially by consutrctor for overdue posts, reset at regular interval
                SetSenTimer(g_SendSecs);
        }

    }
    Refresh(false);
}

void logsWindow::OnRefreshTimeout(wxTimerEvent& event) {
    // if the last CheckIsOnline() call returned negative (connection lost), check again every n seconds,
    // but only if there has been any mouse activity (to minimize data usage)
    if (!p_plugin->CheckIsOnline() &&
            (m_last_lat != p_plugin->m_cursor_lat || m_last_lon != p_plugin->m_cursor_lon)){
        p_plugin->CheckIsOnline();
        wxBell();
        m_last_lat =  p_plugin->m_cursor_lat;
        m_last_lon =  p_plugin->m_cursor_lon;
    }
    Refresh(false);
}

void logsWindow::OnPaint(wxPaintEvent& event) {

    wxPaintDC dc(this);
    wxColour cs;
    GetGlobalColor(_T("GREEN2"), &cs);
    wxColour cr;
    GetGlobalColor(_T("BLUE2"), &cr);
    wxColour ca;
    GetGlobalColor(_T("URED"), &ca);
    wxColour ci;
    GetGlobalColor(_T("DASHL"), &ci);
    wxColour cb;
    GetGlobalColor(_T("DASHB"), &cb);
    dc.SetBackground(cb);
    dc.SetTextBackground(cb);

    wxString lastRcvdStr, lastSentStr = wxEmptyString;
    lastRcvd = p_plugin->g_LastLogsRcvd;
    lastSent = p_plugin->g_LastLogSent;

    lastRcvdStr = lastRcvd.Format(_T(" %a-%d-%b-%Y %H:%M:%S  "), wxDateTime::Local);
    if (m_nmea_ready){
        lastSentStr = lastSent.Format(_T(" %a-%d-%b-%Y %H:%M:%S  "), wxDateTime::Local);
    }
    else if (p_plugin->g_PostPeriod > 0) {
        lastSentStr = _("Buffering NMEA events..");
    }

    wxFont *g_pFontSmall;
    g_pFontSmall = new wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(*g_pFontSmall);

    dc.Clear();

    if (p_plugin->CheckIsOnline()) {
        if (p_plugin->g_PostPeriod > 0)
            dc.SetTextForeground(cs);
        if (m_ErrorCondition == _("Offline"))
            m_ErrorCondition = wxEmptyString;
    } else {
        dc.SetTextForeground(ci);
        m_ErrorCondition = _("Offline");
    }

    // own log postings
    dc.DrawText(_("sQuiddio logs sent:"), 10, 5);
    dc.DrawText(timeAgo(lastSent),120,5);
    dc.DrawText(_T("(")+lastSentStr+_T(")"),240,5);

    dc.DrawText(_T("|"),425,5);

    // friends logs
    if (g_RetrieveSecs > 0 && p_plugin->CheckIsOnline())
        dc.SetTextForeground(cr);
    else
        dc.SetTextForeground(ci);

    wxString demo_msg = _T("");
    if (p_plugin->g_ApiKey == _T("squiddio_demo_api"))
        demo_msg = _(" (demo)");
    dc.DrawText(_("Received")+demo_msg+_T(":"), 440, 5);
    dc.DrawText(timeAgo(lastRcvd),500,5);
    dc.DrawText(_T("(") + lastRcvdStr +_T(")"),640,5);

    dc.DrawText(_T("|"),820,5);
    dc.SetTextForeground(ca);
    dc.DrawText(m_ErrorCondition ,840, 5);

    dc.SetTextForeground(cr);
    dc.DrawText(m_Notice ,840, 5);
}

void logsWindow::SetSentence(wxString &sentence) {

        int curr_time = wxDateTime::Now().GetTicks();
        int down_sample;

    m_NMEA0183 << sentence;

    if (m_NMEA0183.PreParse()) {
        wxString last_id = m_NMEA0183.LastSentenceIDReceived;

        if (last_id == _T("RMC")) {
            if (m_NMEA0183.Parse()) {
                if (m_NMEA0183.Rmc.IsDataValid == NTrue) {
                    float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                    int lat_deg_int = (int) (llt / 100);
                    float lat_deg = lat_deg_int;
                    float lat_min = llt - (lat_deg * 100);
                    mLat = lat_deg + (lat_min / 60.);
                    if (m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                        mLat = -mLat;

                    float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                    int lon_deg_int = (int) (lln / 100);
                    float lon_deg = lon_deg_int;
                    float lon_min = lln - (lon_deg * 100);
                    mLon = lon_deg + (lon_min / 60.);
                    if (m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                        mLon = -mLon;

                    mSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                    mCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                    m_nmea_ready = true;
                    down_sample = 20;
                }
            }
            } else
                down_sample = 40;

            if (m_NmeaLog[last_id] == 0 || (curr_time - m_NmeaLog[last_id] > down_sample) ) {
                m_NmeaFile.Write(sentence);
                wxTextFile::GetEOL();
                m_NmeaLog[last_id] = curr_time;
        }
    }
}

wxString logsWindow::PostPosition(double lat, double lon, double sog, double cog) {

    ssize_t length = m_NmeaFile.Length();

    m_NmeaFile.Close();
    wxString nmea_seq = wxEmptyString;

    if (p_plugin->g_SendXml) {
        if (length < 100000) {
            // file is not too big to be read in memory.
            wxFile f( m_NmeaFileName );
            f.ReadAll( &nmea_seq );
            f.Close();
            wxStringTokenizer str_arr(nmea_seq, _T("\n"));

            int num_lines = str_arr.CountTokens();
            int max_lines = 1000;
            if (num_lines > max_lines) {
                // file too big to post. Take only last max_lines.
                nmea_seq = wxEmptyString;
                int offset = num_lines - max_lines;
                int i = 0;
                wxString line;
                while (str_arr.HasMoreTokens()) {
                    line = str_arr.GetNextToken();
                    i+= 1;
                    if (i > offset)
                        nmea_seq += line + _("\n");
                }
                wxLogMessage(_T("squiddio_pi: Nmea file too large. Truncated before post"));
            }
        }
    }

    wxString reply = wxEmptyString;
    wxString parameters;
    wxString url = p_plugin->g_DomainName+_("/positions.json");
    parameters.Printf(_T("api_key=%s&email=%s&lat=%f&lon=%f&sog=%f&cog=%f&nmea=%s&source=ocpn&version=%s"),
                        p_plugin->g_ApiKey.c_str(), p_plugin->g_Email.c_str(),lat, lon, sog, cog, nmea_seq, p_plugin->g_UrlVersion);

    _OCPN_DLStatus res = OCPN_postDataHttp(url , parameters, reply, 5);

    if( res == OCPN_DL_NO_ERROR ) {
        wxLogMessage(_("squiddio_pi: Created sQuiddio log update:") + reply);
            ::wxRemoveFile(m_NmeaFileName);
#ifdef __WXOSX__
        m_NmeaFile.Open(m_NmeaFileName, wxFile::write);
#else
            bool ok = m_NmeaFile.Open(m_NmeaFileName, wxFile::write);
#endif
        }

    return reply;
}

void logsWindow::ShowFriendsLogs() {
    wxString layerContents;
    wxString request_url;
    bool isLayerUpdate;

    request_url.Printf(_T("/connections.xml?api_key=%s&email=%s"),
            p_plugin->g_ApiKey.c_str(), p_plugin->g_Email.c_str());

    layerContents = p_plugin->DownloadLayer(request_url);

    if (layerContents.length() > 200) {
        isLayerUpdate = p_plugin->SaveLayer(layerContents, m_LogsFilePath);
        if (isLayerUpdate) {
            if (m_LogsLayer) {
                // hide and delete the current logs layer
                m_LogsLayer->SetVisibleOnChart(false);
                p_plugin->RenderLayerContentsOnChart(m_LogsLayer);

                wxPoiListNode *node = pPoiMan->GetWaypointList()->GetFirst();
                while (node) {
                    Poi *rp = node->GetData();
                    if (rp->m_LayerID == m_LogsLayer->m_LayerID) // poi belongs to logs layer
                        p_plugin->HidePOI(rp);
                    node = node->GetNext();
                }
                RequestRefresh(m_parent_window);
                p_plugin->pLayerList->DeleteObject(m_LogsLayer);
            }

            DisplayLogsLayer();

            p_plugin->g_LastLogsRcvd = wxDateTime::Now().GetTicks();
            m_ErrorCondition = wxEmptyString;
        }
    } else {
        m_ErrorCondition = _("Unable to retrieve friends logs: check your credentials and Follow List");
        Refresh(false);
        wxLogMessage(_T("squiddio_pi: ")+m_ErrorCondition);
    }
}

void logsWindow::DisplayLogsLayer() {
    if (::wxFileExists(m_LogsFilePath) && g_RetrieveSecs > 0) {
        wxString null_region;
        m_LogsLayer = p_plugin->LoadLayer(m_LogsFilePath, null_region);
        m_LogsLayer->SetVisibleNames(false);
        p_plugin->RenderLayerContentsOnChart(m_LogsLayer);
    }
}

void logsWindow::OnClose(wxAuiManagerEvent &event) {
    wxMessageBox(_("This will deactivate logs sharing.\n\n To reactivate, go to the sQuiddio plugin settings -> Logs Sharing tab."));
    p_plugin->g_PostPeriod = 0;
    p_plugin->g_RetrievePeriod = 0;
    g_SendSecs = 0;
    g_RetrieveSecs = 0;
    m_pRecTimer->Stop();
    m_pSenTimer->Stop();
    m_pSenTimer->Stop();
    m_pRefreshTimer->Stop();
    if (m_LogsLayer)
    {
        m_LogsLayer->m_bIsVisibleOnChart = false;
        p_plugin->RenderLayerContentsOnChart(m_LogsLayer, false, true);
    }
    p_plugin->SetLogsWindow();
}
