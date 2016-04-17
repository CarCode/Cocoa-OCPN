/***************************************************************************
 * $Id: logbookkonni_pi.h,v 1.8 2010/06/21 01:54:37 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  DEMO Plugin
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *   $EMAIL$                                                               *
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

#ifndef _LOGBOOKPIKONNI_H_
#define _LOGBOOKPIKONNI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    1
#define     PLUGIN_VERSION_MINOR    2

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    7


#include "../../../include/ocpn_plugin.h"
#include "Options.h"
#include "../../../src/nmea0183/nmea0183.h"

#include <wx/aui/aui.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dialog.h>
#include <wx/splitter.h>
#include <wx/fileconf.h>
#include <wx/dynarray.h>

#include "../../../include/wx/jsonwriter.h"
//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define LOGBOOK_TOOL_POSITION -1

class Logbook;
class LogbookDialog;
class LogbookTimer;
class LogbookOptions;
class Options;

class logbookkonni_pi : public opencpn_plugin_17
{
public:
    enum states {OFF,ONNOEVENTS,ONWITHEVENTS};

    logbookkonni_pi(void *ppimgr);
    ~logbookkonni_pi();

    bool            dlgShow;
    LogbookTimer    *timer;
    wxWindow        *m_parent_window;
    LogbookDialog   *m_plogbook_window;
    LogbookOptions  *optionsDialog;
    wxFileConfig    *m_pconfig;
    Options         *opt;
    wxTimer         *m_timer;
    wxColour        col,col1,gridline,uitext,udkrd,back_color,text_color;
    wxColour        mcol,mcol1,mgridline, muitext,mudkrd,mback_color,mtext_color;
    wxString        lastWaypointInRoute;

    wxLocale*       local;
    int             state;
    bool            eventsEnabled;

    //    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();
    void UpdateAuiStatus(void);
    void SetColorScheme(PI_ColorScheme cs);
    void GetOriginalColors();
    void SetOriginalColors();

    //    The optional method overrides

    void SetNMEASentence(wxString &sentence);
    void SetPositionFix(PlugIn_Position_Fix &pfix);
    void OnContextMenuItemCallback(int id);

    //    The required override PlugIn Methods
    //     bool RenderOverlay(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
    //      void SetCursorLatLon(double lat, double lon);


    void SetDefaults(void);
    int GetToolbarToolCount(void);
    void ShowPreferencesDialog( wxWindow* parent );
    void OnToolbarToolCallback(int id);
    void SetPluginMessage(wxString &message_id, wxString &message_body);
    void SendLogbookMessage(wxString message_id, wxString message_body);
    //	  void SendPluginMessage(wxString &message_id, wxString &message_body);

    void loadLanguages(wxWindow * parent);
    void loadLayouts(wxWindow * parent);
    void startLogbook();
    void shutdown(bool menu);

private:
    void                    OnTimer(wxTimerEvent& ev);
    void                    SaveConfig();
    void                    LoadConfig();
    ArrayOfGridColWidth     readCols(ArrayOfGridColWidth ar, wxString str);
    void                    writeCols(wxFileConfig *pConf, ArrayOfGridColWidth ar, wxString entry);
    ArrayOfGridColWidth     readColsOld(wxFileConfig *pConf, ArrayOfGridColWidth ar, wxString entry);
    void                    dialogDimmer(PI_ColorScheme cs,wxWindow* ctrl,wxColour col,wxColour col1, wxColour back_color,wxColour text_color, wxColour uitext, wxColour udkrd);
    wxAuiManager            *m_pauimgr;
// not used
//    int                     m_show_id;
//    int                     m_hide_id;
    bool                    show;
    int                     m_leftclick_tool_id;

    bool                    m_bLOGShowIcon;
};


class LogbookTimer : public wxWindow
{

public:
    LogbookTimer(logbookkonni_pi* l) : wxWindow() { plogbook_pi = l; }

    void		OnTimer(wxTimerEvent& ev);
    bool		popUp();
    void		timerEvent();

public:
    logbookkonni_pi* plogbook_pi;
};

#ifdef __WXGTK__
///////////////////////////////////////////////////////////////////////////////
/// Class PWDialog, only for Password
///////////////////////////////////////////////////////////////////////////////
class PWDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText126;
		wxStdDialogButtonSizer* m_sdbSizer10;
		wxButton* m_sdbSizer10OK;
		wxButton* m_sdbSizer10Cancel;
	
	public:
		wxTextCtrl* m_textCtrl89;

		PWDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 233,115 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~PWDialog();
	
};
#endif
#endif



