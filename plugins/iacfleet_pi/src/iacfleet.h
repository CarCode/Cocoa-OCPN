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
#ifndef __WXOSX__
#include "folder.xpm"
#endif
#include "../../../include/ocpn_plugin.h"
#include "iacfile.h"


enum{
      ID_OK = 11001,
      ID_CHOOSEIACFLEETDIR,
      ID_FILESELECTED,
      ID_NOTEBOOK,
      ID_RAWTEXT,
      ID_TIP_TIMER
    };

class iacfleet_pi;
class IACFleetUIDialog: public wxDialog
{
      DECLARE_CLASS( IACFleetUIDialog )
                  DECLARE_EVENT_TABLE()
      public:
            IACFleetUIDialog(void);
            ~IACFleetUIDialog(void);
            bool Create(  wxWindow *parent, iacfleet_pi *ppi, wxWindowID id = wxID_ANY,
                         const wxString& caption = _("IACFleet Display Control"), const wxString initial_dir = wxT(""),
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );
            void CreateControls();
            void OnClose(wxCloseEvent& event);
            void OnIdOKClick( wxCommandEvent& event );
            void OnMove( wxMoveEvent& event );
            void OnSize( wxSizeEvent& event );
            void OnChooseDirClick( wxCommandEvent& event );
            void OnFileSelect( wxCommandEvent& event );
            void OnRawTextChanged( wxCommandEvent& event );
            void OnTipTimer( wxTimerEvent& event );
            void SetCursorLatLon(double lat, double lon);
#ifdef __WXOSX__
            bool RenderOverlay(wxDC &pmdc, PlugIn_ViewPort *vp);
#else
            bool RenderOverlay(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
#endif
      private:
            void Invalidate(void);
            void updateFileList(void);
            void updateIACFleet(void);
            void updateRawPanel(wxString &awData);
            void updateTextPanel(void);

      private:
            wxTimer           m_TooltipTimer;
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

            // the Contols that will get updated       
            wxTextCtrl        *m_pitemCurrentDirectoryCtrl;
            wxListBox         *m_pFileListCtrl;
            wxTextCtrl        *m_pTextCtrl;
            wxTextCtrl        *m_pRawCtrl;
            wxStaticText      *m_pFileTime;
            wxTipWindow       *m_pTipWindow;
};

#endif