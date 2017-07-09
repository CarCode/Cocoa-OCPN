/***************************************************************************
 * $Id: IACFleet.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  IACFleet Plugin
 * Author:   Carsten Borchardt
 *
 ***************************************************************************
 *   Copyright (C) 2010 by Carsten Borchardt                               *
 *   Carsten@sy-fortytwo.de                                                *
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
 ***************************************************************************/

#ifndef _IACFLEET_H_
#define _IACFLEET_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/treectrl.h>
#include <wx/fileconf.h>
#include <wx/notebook.h>
#include <wx/textfile.h>
#include <wx/arrstr.h>
#include <wx/timer.h>
#include <wx/dirctrl.h>
#include <wx/tipwin.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/spinctrl.h>

//#include "../../../include/ocpn_plugin.h"
#include "ocpn_plugin.h"
#include "iacfile.h"

#define SORT_NAME           1
#define SORT_TIME           2

#define ANIMATION_FRAME_MS  1000

enum
{
    ID_OK = 11001,
    ID_CHOOSEIACFLEETDIR,
    ID_FILESELECTED,
    ID_NOTEBOOK,
    ID_RAWTEXT,
    ID_TIP_TIMER,
    ID_ANIMATE,
    ID_ANIMATION_TIMER
};

class iacfleet_pi;
class IACFleetUIDialog: public wxDialog
{
    DECLARE_CLASS( IACFleetUIDialog )
    DECLARE_EVENT_TABLE()
public:
    IACFleetUIDialog( void );
    ~IACFleetUIDialog( void );
    bool Create( wxWindow *parent, iacfleet_pi *ppi, wxWindowID id = wxID_ANY,
            const wxString& caption = _("IACFleet Display Control"),
            const wxString initial_dir = wxEmptyString, int sort_type = SORT_NAME,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
#ifdef __WXMAC__
            long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
#else
            long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#endif
    void CreateControls();
    void OnClose( wxCloseEvent& event );
    void OnIdOKClick( wxCommandEvent& event );
    void OnIdAnimateClick( wxCommandEvent& event );
    void OnTimerAnimation( wxTimerEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnChooseDirClick( wxCommandEvent& event );
    void OnFileSelect( wxCommandEvent& event );
    void OnRawTextChanged( wxCommandEvent& event );
    void OnTipTimer( wxTimerEvent& event );
    void SetCursorLatLon( double lat, double lon );
    bool RenderOverlay( wxDC *dc, PlugIn_ViewPort *vp );
    void OnBrDownload( wxCommandEvent& event );
    void OnSortChange( wxCommandEvent& event );
    void OnNoaaDownload( wxCommandEvent& event );

private:
    void Invalidate( void );
    void updateFileList( void );
    void updateIACFleet( void );
    void updateRawPanel( wxString &awData );
    void updateTextPanel( void );

private:
    wxTimer            m_TooltipTimer;
    wxWindow          *pParent;
    iacfleet_pi       *pPlugIn;
    wxString           m_currentDir;
    wxString           m_currentFileName;
    wxBitmap          *m_pfolder_bitmap;
    wxArrayString      m_FilenameArray;
    IACFile            m_iacfile;
    GeoPoint           m_cursorpos;
    PlugIn_ViewPort    m_lastViewPort;
    bool               m_lastViewPortValid;
    int                m_sortType;
    wxArrayString      m_filesToAnimate;
    size_t             m_animationCurrentFile;

    // the Contols that will get updated
    wxTextCtrl        *m_pitemCurrentDirectoryCtrl;
    wxListBox         *m_pFileListCtrl;
    wxTextCtrl        *m_pTextCtrl;
    wxTextCtrl        *m_pRawCtrl;
    wxStaticText      *m_pFileTime;
    wxTipWindow       *m_pTipWindow;
    wxRadioButton     *m_rbSortName;
    wxRadioButton     *m_rbSortTime;

    wxButton          *m_bAnimation;

    //Download panel
    wxStaticText      *m_stDate;
    wxDatePickerCtrl  *m_dpBrazil;
    wxStaticText      *m_stHour;
    wxChoice          *m_chHour;
    wxButton          *m_bBrDownload;
    wxStaticText      *m_stBrDesc;
    wxHyperlinkCtrl   *m_hlBr;
    wxStaticText      *m_stSort;
    wxStaticText      *m_stHist;
    wxSpinCtrl        *m_spHist;
    wxStaticText      *m_stForecasts;

    wxRadioButton     *m_rbAnalysis;
    wxRadioButton     *m_rbForecast;
    wxButton          *m_bNoaaDownload;

    wxStaticText      *m_stNadi;

    wxTimer           *m_timer;
};

#endif
