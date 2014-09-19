/******************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2013 by David S. Register                               *
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
 ***************************************************************************
 */

#include "wx/wxprec.h"

#include "S57QueryDialog.h"
#include "chcanv.h"

extern ColorScheme global_color_scheme;
extern S57QueryDialog *g_pObjectQueryDialog;
extern int g_S57_dialog_sx;
extern int g_S57_dialog_sy;

IMPLEMENT_CLASS ( S57QueryDialog, wxDialog )
// S57QueryDialog event table definition
BEGIN_EVENT_TABLE ( S57QueryDialog, wxDialog )  //ws wxDialog
    EVT_SIZE ( S57QueryDialog::OnSize )
    EVT_CLOSE( S57QueryDialog::OnClose)
END_EVENT_TABLE()

S57QueryDialog::S57QueryDialog()
{
    Init();
}

S57QueryDialog::S57QueryDialog( wxWindow* parent, wxWindowID id, const wxString& caption,
                                const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );

}

S57QueryDialog::~S57QueryDialog()
{
    g_S57_dialog_sx = GetSize().x;
    g_S57_dialog_sy = GetSize().y;

}

void S57QueryDialog::Init()
{
}

bool S57QueryDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style )
{
    //    As a display optimization....
    //    if current color scheme is other than DAY,
    //    Then create the dialog ..WITHOUT.. borders and title bar.
    //    This way, any window decorations set by external themes, etc
    //    will not detract from night-vision

    long wstyle = wxDEFAULT_FRAME_STYLE;
#ifdef __WXOSX__
    wstyle |= wxSTAY_ON_TOP;
#endif
    
    if( ( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY )
            && ( global_color_scheme != GLOBAL_COLOR_SCHEME_RGB ) ) wstyle |= ( wxNO_BORDER );

    if( !wxDialog::Create( parent, id, caption, pos, size, wstyle ) ) return false;

    wxFont *dFont = GetOCPNScaledFont(_("ObjectQuery"), 10);

    SetFont( *dFont );
    CreateControls();

// This ensures that the dialog cannot be sized smaller
// than the minimum size
    GetSizer()->SetSizeHints( this );

// Explicitely set the size
    SetSize( size );

// Centre the dialog on the parent or (if none) screen
    Centre();

    DimeControl( this );
    return true;

}

void S57QueryDialog::CreateControls()
{
    wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( topSizer );

    m_phtml = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxHW_SCROLLBAR_AUTO );
    
    m_phtml->SetBorders( 5 );

    m_phtml->SetMinSize( wxSize( 100, 100 ) );            // this will constrain the dialog, too
    topSizer->Add( m_phtml, 1, wxBOTTOM | wxEXPAND, 10 );

    topSizer->FitInside( this );

    wxSizer* ok = CreateButtonSizer( wxOK );
    topSizer->Add( ok, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5 );
}

void S57QueryDialog::SetColorScheme( void )
{
    DimeControl( this );
    wxColor bg = GetBackgroundColour();
    m_phtml->SetBackgroundColour( bg );

}

void S57QueryDialog::SetHTMLPage( wxString& page )
{
    m_phtml->SetPage( page );
    SetColorScheme();
}

void S57QueryDialog::OnSize( wxSizeEvent& event )
{
    g_S57_dialog_sx = GetSize().x;
    g_S57_dialog_sy = GetSize().y;
    wxDialog::OnSize( event );
}

void S57QueryDialog::OnClose( wxCloseEvent& event )
{
    g_S57_dialog_sx = GetSize().x;
    g_S57_dialog_sy = GetSize().y;
    Destroy();
    g_pObjectQueryDialog = NULL;
}

