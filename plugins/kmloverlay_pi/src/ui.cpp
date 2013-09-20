/***************************************************************************
 * $Id: ui.cpp, v0.1 2012-01-20 SethDart Exp $
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

#include <wx/filename.h>
#include "icons.h"
#include "ui.h"

/*    KMLOverlay user interface implementation
 *
 *************************************************************************/

KMLOverlayUI::KMLOverlayUI( wxWindow *pparent, wxWindowID id, wxString filename )
      :wxPanel( pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE, _("KML overlay") )
{
      wxColour cl;
      GetGlobalColor(_T("DILG1"), &cl);
      SetBackgroundColour(cl);

      wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
      SetSizer( topsizer );

      m_pCheckListBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize(150, 180), 0,
                                         NULL, wxLB_SINGLE );
      m_pCheckListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED,
            wxCommandEventHandler( KMLOverlayUI::OnListItemSelected ), NULL, this );
      m_pCheckListBox->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
            wxCommandEventHandler( KMLOverlayUI::OnCheckToggle ), NULL, this );
      topsizer->Add( m_pCheckListBox, 0, wxEXPAND|wxALL );

      wxBoxSizer *itemBoxSizer01 = new wxBoxSizer( wxHORIZONTAL );
      topsizer->Add( itemBoxSizer01, 0, wxALL );

      m_pButtonAdd = new wxBitmapButton( this, wxID_ANY, *_img_plus, wxDefaultPosition, wxDefaultSize );
      itemBoxSizer01->Add( m_pButtonAdd, 0, wxALIGN_CENTER, 2 );
      m_pButtonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler( KMLOverlayUI::OnItemAdd ), NULL, this );
      m_pButtonDelete = new wxBitmapButton( this, wxID_ANY, *_img_minus, wxDefaultPosition, wxDefaultSize );
      itemBoxSizer01->Add( m_pButtonDelete, 0, wxALIGN_CENTER, 2 );
      m_pButtonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler( KMLOverlayUI::OnItemDelete ), NULL, this );

      Fit();
      // GetSize() doesn't seems to count aui borders so we add it now.
      wxSize sz = GetSize(); sz.IncBy(10,24);
      SetMinSize(sz);

      UpdateButtonsState();
      m_pFactory = new KMLOverlayFactory();
}

KMLOverlayUI::~KMLOverlayUI()
{
      delete m_pFactory;
}

void KMLOverlayUI::SetColorScheme( PI_ColorScheme cs )
{
      wxColour cl;
      GetGlobalColor( _T("DILG1"), &cl );
      SetBackgroundColour( cl );

      Refresh(false);
}

bool KMLOverlayUI::RenderOverlay( wxDC &dc, PlugIn_ViewPort *vp )
{
      return m_pFactory->RenderOverlay( dc, vp );
}

bool KMLOverlayUI::RenderGLOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
      return m_pFactory->RenderGLOverlay( pcontext, vp );
}

void KMLOverlayUI::AddFile( wxString filename, bool visible )
{
      if (! m_pFactory->Add( filename, visible )) {
            return;
      }
      m_pCheckListBox->Append( wxFileName(filename).GetFullName() );
      m_pCheckListBox->Check( m_pCheckListBox->GetCount()-1, visible );
}

wxString KMLOverlayUI::GetFilename( int idx )
{
      return m_pFactory->GetFilename( idx );
}

bool KMLOverlayUI::GetVisibility( int idx )
{
      return m_pFactory->GetVisibility( idx ); // m_pCheckListBox->IsChecked( idx );
}

int KMLOverlayUI::GetCount()
{
      return m_pFactory->GetCount(); // m_pCheckListBox->GetCount();
}

void KMLOverlayUI::OnListItemSelected( wxCommandEvent& event )
{
      UpdateButtonsState();
}

void KMLOverlayUI::OnCheckToggle( wxCommandEvent& event )
{
      m_pFactory->SetVisibility( event.GetInt(), m_pCheckListBox->IsChecked( event.GetInt() ) );
}

void KMLOverlayUI::UpdateButtonsState()
{
      m_pButtonDelete->Enable( m_pCheckListBox->GetSelection() != wxNOT_FOUND );
}

void KMLOverlayUI::OnItemAdd( wxCommandEvent &event )
{
      wxFileDialog fdlg( this, _("Select a file"), wxT(""), wxT(""), _("KML files (*.kml;*.kmz)|*.kml;*.kmz"), wxFD_OPEN|wxFD_FILE_MUST_EXIST );
      if ( fdlg.ShowModal() == wxID_OK)
      {
            AddFile( fdlg.GetPath(), true );
      }
}

void KMLOverlayUI::OnItemDelete( wxCommandEvent &event )
{
      long itemID = m_pCheckListBox->GetSelection();

      if (itemID != wxNOT_FOUND) {
            m_pCheckListBox->Delete( itemID );

            m_pFactory->Delete( itemID );
      }
      UpdateButtonsState();
}

