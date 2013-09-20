/***************************************************************************
 * $Id: prefdlg.cpp, v0.1 2012-01-20 SethDart Exp $
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

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#include "prefdlg.h"

/* KMLOverlay preferences dialog implementation
 *
 *************************************************************************/

KMLOverlayPreferencesDialog::KMLOverlayPreferencesDialog( wxWindow *parent, wxWindowID id, int interval )
      :wxDialog( parent, id, _("KML overlay preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
      Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( KMLOverlayPreferencesDialog::OnCloseDialog ), NULL, this );

      wxBoxSizer* itemBoxSizerMainPanel = new wxBoxSizer(wxVERTICAL);
      SetSizer(itemBoxSizerMainPanel);

      wxStaticBox* itemStaticBox01 = new wxStaticBox( this, wxID_ANY, _("Preferences") );
      wxStaticBoxSizer* itemStaticBoxSizer01 = new wxStaticBoxSizer( itemStaticBox01, wxHORIZONTAL );
      itemBoxSizerMainPanel->Add( itemStaticBoxSizer01, 0, wxEXPAND|wxALL, 2 );

      wxFlexGridSizer *itemFlexGridSizer01 = new wxFlexGridSizer(2);
      itemFlexGridSizer01->AddGrowableCol(1);
      itemStaticBoxSizer01->Add( itemFlexGridSizer01, 0, wxGROW|wxALL, 2 );

      wxStaticText* itemStaticText01 = new wxStaticText( this, wxID_ANY, _("Interval (minutes):"), wxDefaultPosition, wxDefaultSize, 0 );
      itemFlexGridSizer01->Add( itemStaticText01, 0, wxEXPAND|wxALL, 2 );
      m_interval = interval;
      m_pInterval = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 60, interval );
      itemFlexGridSizer01->Add( m_pInterval, 1, wxALIGN_LEFT|wxALL, 2 );

      wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer(wxOK|wxCANCEL);
      itemBoxSizerMainPanel->Add(DialogButtonSizer, 0, wxALIGN_RIGHT|wxALL, 5);

      //SetMinSize(wxSize(454, -1));
      Fit();
}

void KMLOverlayPreferencesDialog::OnCloseDialog(wxCloseEvent& event)
{
      SaveKMLOverlayConfig();
      event.Skip();
}

void KMLOverlayPreferencesDialog::SaveKMLOverlayConfig()
{
//      m_filename = m_pFilename->GetPath();
      m_interval = m_pInterval->GetValue();
}

