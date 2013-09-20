/***************************************************************************
 * $Id: ui.h, v0.1 2012-01-20 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  KML overlay plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Jean-Eudes Onfray                               *
 *   je@onfray.fr                                                          *
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

#ifndef _KMLOverlayUI_H_
#define _KMLOverlayUI_H_

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#include <wx/checklst.h>
#include "../../../include/ocpn_plugin.h"
#include "factory.h"

class KMLOverlayUI : public wxPanel // must be a wxPanel, not wxWindow so AutoLayout works
{
public:
      KMLOverlayUI( wxWindow *pparent, wxWindowID id, wxString filename );
      ~KMLOverlayUI();

      void SetColorScheme( PI_ColorScheme cs );
      bool RenderOverlay( wxDC &dc, PlugIn_ViewPort *vp );
      bool RenderGLOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp );

      void AddFile( wxString filename, bool visible );
      wxString GetFilename( int idx );
      bool GetVisibility( int idx );
      int GetCount();

private:
      void OnListItemSelected( wxCommandEvent& event );
      void OnCheckToggle( wxCommandEvent& event );
      void UpdateButtonsState();
      void OnItemAdd( wxCommandEvent& event );
      void OnItemDelete( wxCommandEvent& event );

      wxCheckListBox       *m_pCheckListBox;
      wxBitmapButton       *m_pButtonAdd;
      wxBitmapButton       *m_pButtonDelete;

      KMLOverlayFactory    *m_pFactory;
};

#endif
