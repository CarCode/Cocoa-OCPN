/* **************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  About Dialog
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2019 by David S. Register                               *
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
 **************************************************************************/

#include "AboutFrameImpl.h"
#include "config.h"
#include "OCPNPlatform.h"
#include "chart1.h"

#ifdef __WXMSW__
#define EXTEND_WIDTH 70
#define EXTEND_HEIGHT 70
#else
#define EXTEND_WIDTH 50
#define EXTEND_HEIGHT 50
#endif

extern OCPNPlatform  *g_Platform;

AboutFrameImpl::AboutFrameImpl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : AboutFrame( parent, id, title, pos, size, style )
{
    m_staticTextVersion->SetLabel(VERSION_FULL);
    m_staticTextCopyYears->SetLabel("\u00A9 2000-2023");
    m_hyperlinkIniFile->SetLabel(g_Platform->GetConfigFileName());
    m_hyperlinkIniFile->SetURL(g_Platform->GetConfigFileName());
    m_hyperlinkLogFile->SetLabel(g_Platform->GetLogFileName());
    m_hyperlinkLogFile->SetURL(g_Platform->GetLogFileName());
    m_htmlWinAuthors->Hide();
    m_htmlWinLicense->Hide();
    m_htmlWinHelp->Hide();
    m_btnBack->Hide();
    m_htmlWinLicense->LoadFile(wxString::Format("%s/license.html", g_Platform->GetSharedDataDir().c_str()));
    m_htmlWinAuthors->LoadFile(wxString::Format("%s/authors.html", g_Platform->GetSharedDataDir().c_str()));
#ifdef __WXOSX__
    m_htmlWinHelp->LoadFile(wxString::Format("%s/hilfe.html", g_Platform->GetSharedDataDir().c_str()));
#endif
    wxBitmap logo(wxString::Format("%s/opencpn.png", g_Platform->GetSharedDataDir().c_str()), wxBITMAP_TYPE_ANY);

#ifndef __WXOSX__
    m_hyperlinkHelp->SetURL(wxString::Format("file://%sdoc/help_en_US.html", g_Platform->GetSharedDataDir().c_str()));
#ifdef OCPN_USE_WEBVIEW
    m_htmlWinHelp->LoadURL(wxString::Format("file://%sdoc/help_en_US.html", g_Platform->GetSharedDataDir().c_str()));
#else
    m_htmlWinHelp->LoadFile(wxString::Format("%s/doc/help_en_US.html", g_Platform->GetSharedDataDir().c_str()));
#endif
#endif
    m_bitmapLogo->SetBitmap(logo);
    
    int width = m_scrolledWindowAbout->GetSizer()->GetSize().GetWidth() + m_bitmapLogo->GetSize().GetWidth() + EXTEND_WIDTH;
    int height = m_scrolledWindowAbout->GetSizer()->GetSize().GetHeight() + m_panelMainLinks->GetSizer()->GetSize().GetHeight() + EXTEND_HEIGHT;

    SetMinSize(wxSize(width, height));
    Layout();
    Fit();
}


void AboutFrameImpl::OnLinkHelp( wxHyperlinkEvent& event )
{
#ifdef __WXGTK__   
    wxString testFile = wxString::Format("/%s/doc/help_en_US.html", g_Platform->GetSharedDataDir().c_str());
    if( !::wxFileExists(testFile)){
        wxString msg = _("OpenCPN Help documentation is not available locally.");  msg += _T("\n");
        msg += _("Would you like to visit the opencpn.org website for more information?");
        
        if( wxID_YES == OCPNMessageBox(NULL, msg, _("OpenCPN Info"), wxYES_NO | wxCENTER, 60 ) )
        {
            wxLaunchDefaultBrowser(_T("https://opencpn.org"));
        }
    }
    else
#endif        
    {
        m_htmlWinAuthors->Hide();
        m_htmlWinLicense->Hide();
        m_htmlWinHelp->Show();
        m_scrolledWindowAbout->Hide();
#ifdef __WXOSX__
        m_btnBack->Hide();
        m_scrolledWindowAbout->Hide();
        Layout();
#else
        m_btnBack->Show();
#ifdef OCPN_USE_WEBVIEW
        m_btnBack->Enable(m_htmlWinHelp->CanGoBack());
#else
        m_btnBack->Enable(m_htmlWinHelp->HistoryCanBack());
#endif
        SetSize(m_parent->GetSize());
        Centre();
#endif
    }
}

void AboutFrameImpl::OnLinkLicense( wxHyperlinkEvent& event )
{
    m_htmlWinAuthors->Hide();
    m_htmlWinLicense->Show();
    m_htmlWinHelp->Hide();
    m_btnBack->Hide();
    m_scrolledWindowAbout->Hide();
    Layout();
}

void AboutFrameImpl::OnLinkAuthors( wxHyperlinkEvent& event )
{
    m_htmlWinAuthors->Show();
    m_htmlWinLicense->Hide();
    m_htmlWinHelp->Hide();
    m_btnBack->Hide();
    m_scrolledWindowAbout->Hide();
    Layout();
}

void AboutFrameImpl::AboutFrameOnActivate( wxActivateEvent& event )
{
    m_htmlWinAuthors->Hide();
    m_htmlWinLicense->Hide();
    m_htmlWinHelp->Hide();
    m_btnBack->Hide();
    m_scrolledWindowAbout->Show();
    Layout();
    m_scrolledWindowAbout->Refresh();
    m_panelMainLinks->Refresh();
}
