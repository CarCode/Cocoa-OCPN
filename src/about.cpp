/**************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  About Dialog
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/textfile.h>
#include <wx/ffile.h>
#include <wx/clipbrd.h>
#include <wx/html/htmlwin.h>

#include <version.h>

#include "about.h"
#include "chart1.h"
#include "chcanv.h"
#include "styles.h"
#include "version.h"
#include "OCPNPlatform.h"
#include "FontMgr.h"

extern OCPNPlatform *g_Platform;
extern MyFrame *gFrame;
//extern wxString         mlog_file;
extern ocpnStyle::StyleManager* g_StyleManager;
extern about *g_pAboutDlg;
extern bool g_bresponsive;

wxString OpenCPNVersion =
wxString::Format( wxT("\n      Version %i.%i.%i Build %s"),
                 VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_DATE );

#ifdef __WXOSX__
const wxString AboutText = wxT(
    "<br>OpenCPN<br>"
    "(c) 2000-2015 Die OpenCPN Autoren<br><br>");
#else
const wxString AboutText = wxT(
                               "<br>OpenCPN<br>"
                               "(c) 2000-2015 The OpenCPN Authors<br><br>");
#endif
#ifdef __WXOSX__
const wxString OpenCPNInfo = wxT(
    "<br><br>"
    "OpenCPN ist ein freies Software Projekt, erstellt von Seglern.<br>"
    "Es ist frei verfügbar zum Download und zur Verbreitung ohne Kosten "
    "von Opencpn.org.<br>"
    "Verwenden Sie OpenCPN, beteiligen Sie sich bitte daran "
    "oder machen Sie eine Spende.<br><br>"
    "Dokumentation:<br>"
    "Über OS X Menü Hilfe (auch Begriffe-Suchfunktion)<br><br>");
#else
const wxString OpenCPNInfo = wxT(
    "<br><br>"
    "OpenCPN is a Free Software project, built by sailors. "
    "It is freely available to download and distribute "
    "without charge at opencpn.org.<br><br>"
    "If you use OpenCPN, please consider contributing "
    "or donating funds to the project.<br><br>"
    "For more information, visit http://opencpn.org<br><br>");
#endif
#ifdef __WXOSX__
const wxString AuthorText = wxT(
"   David S Register\n"
"    OpenCPN Lead Developer\n\n"
"    Pavel Kalian\n"
"    S52 Rasterization Improvements\n\n"
"    Sean D'Epagnier\n"
"    OpenGL Architecture\n\n"
"    Thomas Höckne\n"
"    Documentation and Wiki support\n\n"
"    Caesar Schinas\n"
"    User Interface and OS X improvements\n\n"
"    Jesper Weissglas\n"
"    Vector Chart Rendering\n\n"
"    Jean-Eudes Onfray\n"
"    Dashboard and Dialog enhancements\n\n"
"    Kathleen Boswell\n"
"    Icon design\n\n"
"    Flavius Bindea\n"
"    CM93 Offset and AIS enhancements\n\n"
"    Gunther Pilz\n"
"    Windows Installer enhancements\n\n"
"    Alan Bleasby\n"
"    Garmin jeeps module\n\n"
"    Piotr Carlson\n"
"    General usability enhancements\n\n"
"    Anders Lund\n"
"    RouteManagerDialog\n\n"
"    Gordon Mau\n"
"    OpenCPN Documentation\n\n"
"    Tim Francis\n"
"    OpenCPN Documentation\n\n"
"    Mark A Sikes\n"
"    OpenCPN CoDeveloper\n\n"
"    Thomas Haller\n"
"    GPX Import/Export Implementation\n\n"
"    Will Kamp\n"
"    Toolbar Icon design\n\n"
"    Richard Smith\n"
"    OpenCPN CoDeveloper, MacOSX\n\n"
"    David Herring\n"
"    OpenCPN CoDeveloper, MacOSX\n\n"
"    Philip Lange\n"
"    OpenCPN Documentation\n\n"
"    Ron Kuris\n"
"    wxWidgets Support\n\n"
"    Julian Smart, Robert Roebling et al\n"
"    wxWidgets Authors\n\n"
"    Sylvain Duclos\n"
"    S52 Presentation Library code\n\n"
"    Manish P. Pagey\n"
"    Serial Port Library\n\n"
"    David Flater\n"
"    XTIDE tide and current code\n\n"
"    Frank Warmerdam\n"
"    GDAL Class Library\n\n"
"    Mike Higgins\n"
"    BSB Chart Format Detail\n\n"
"    Samuel R. Blackburn\n"
"    NMEA0183 Class Library\n\n"
"    Atul Narkhede\n"
"    Polygon Graphics utilities\n\n"
"    Jan C. Depner\n"
"    WVS Chart Library\n\n"
"    Stuart Cunningham, et al\n"
"    BSB Chart Georeferencing Algorithms\n\n"
"    John F. Waers\n"
"    UTM Conversion Algorithms\n\n"
"    Carsten Tschach\n"
"    UTM Conversion Algorithms\n\n"
"    Ed Williams\n"
"    Great Circle Formulary\n\n"
"    Philippe Bekaert\n"
"    CIE->RGB Color Conversion Matrix\n\n"
"    Robert Lipe\n"
"    Garmin USB GPS Interface\n\n"
"    Gerhard Mueller (CarCode)\n"
"    Mac OS X 64-bit support\n");
#else
const wxString AuthorText = wxT(
"   David S Register\n\"
"      OpenCPN Lead Developer\n\n\"
"    Pavel Kalian\n\"
"      S52 Rasterization Improvements\n\n\"
"    Sean D'Epagnier\n\"
"      OpenGL Architecture\n\n\"
"    J.P. Joubert\n\"
"      GRIB PlugIn enhancements\n\n\"
"    Thomas Höckne\n\"
"      Documentation and Wiki support\n\n\"
"    Caesar Schinas\n\"
"      User Interface and OS X improvements\n\n\"
"    Jesper Weissglas\n\"
"      Vector Chart Rendering\n\n\"
"    Jean-Eudes Onfray\n\"
"      Dashboard and Dialog enhancements\n\n\"
"    Kathleen Boswell\n\"
"      Icon design\n\n\"
"    Flavius Bindea\n\"
"      CM93 Offset and AIS enhancements\n\n\"
"    Gunther Pilz\n\"
"      Windows Installer enhancements\n\n\"
"    Alan Bleasby\n\"
"      Garmin jeeps module\n\n\"
"    Piotr Carlson\n\"
"      General usability enhancements\n\n\"
"    Anders Lund\n\"
"      RouteManagerDialog\n\n\"
"    Gordon Mau\n\"
"      OpenCPN Documentation\n\n\"
"    Tim Francis\n\"
"      OpenCPN Documentation\n\n\"
"    Mark A Sikes\n\"
"      OpenCPN CoDeveloper\n\n\"
"    Thomas Haller\n\"
"      GPX Import/Export Implementation\n\n\"
"    Will Kamp\n\"
"      Toolbar Icon design\n\n\"
"    Richard Smith\n\"
"      OpenCPN CoDeveloper, MacOSX\n\n\"
"    David Herring\n\"
"      OpenCPN CoDeveloper, MacOSX\n\n\"
"    Philip Lange\n\"
"      OpenCPN Documentation\n\n\"
"    Ron Kuris\n\"
"      wxWidgets Support\n\n\"
"    Julian Smart, Robert Roebling et al\n\"
"      wxWidgets Authors\n\n\"
"    Sylvain Duclos\n\"
"      S52 Presentation Library code\n\n\"
"    Manish P. Pagey\n\"
"      Serial Port Library\n\n\"
"    David Flater\n\"
"      XTIDE tide and current code\n\n\"
"    Frank Warmerdam\n\"
"      GDAL Class Library\n\n\"
"    Mike Higgins\n\"
"      BSB Chart Format Detail\n\n\"
"    Samuel R. Blackburn\n\"
"      NMEA0183 Class Library\n\n\"
"    Atul Narkhede\n\"
"      Polygon Graphics utilities\n\n\"
"    Jan C. Depner\n\"
"      WVS Chart Library\n\n\"
"    Stuart Cunningham, et al\n\"
"      BSB Chart Georeferencing Algorithms\n\n\"
"    John F. Waers\n\"
"      UTM Conversion Algorithms\n\n\"
"    Carsten Tschach\n\"
"      UTM Conversion Algorithms\n\n\"
"    Ed Williams\n\"
"      Great Circle Formulary\n\n\"
"    Philippe Bekaert\n\"
"      CIE->RGB Color Conversion Matrix\n\n\"
"    Robert Lipe\n\"
"      Garmin USB GPS Interface\n");
#endif
IMPLEMENT_DYNAMIC_CLASS( about, wxDialog )

BEGIN_EVENT_TABLE( about, wxDialog )
    EVT_BUTTON( xID_OK, about::OnXidOkClick )
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK_HELP, about::OnPageChange)
    EVT_BUTTON( ID_DONATE, about::OnDonateClick)
    EVT_BUTTON( ID_COPYINI, about::OnCopyClick)
    EVT_BUTTON( ID_COPYLOG, about::OnCopyClick)
    EVT_CLOSE( about::OnClose )
END_EVENT_TABLE()

about::about( void ) :
m_DataLocn( wxEmptyString ),
m_parent( NULL ),
m_btips_loaded ( FALSE ) { }

about::about( wxWindow* parent,wxString Data_Locn, wxWindowID id, const wxString& caption,
             const wxPoint& pos, const wxSize& size, long style) :
    m_DataLocn( Data_Locn ),
    m_parent( parent ),
    m_btips_loaded ( FALSE )
{
    Create(parent, id, caption, pos, size, style);
}


bool about::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos,
        const wxSize& size, long style )
{
    m_parent = parent;
#ifdef __WXOSX__
    style |= wxSTAY_ON_TOP;
#endif

    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    SetFont( *qFont );

    m_displaySize = g_Platform->getDisplaySize();
    CreateControls();
    Populate();
    
    // Set the maximum size of the entire about dialog
    wxSize displaySize = wxSize( m_displaySize.x - 100, m_displaySize.y - 100 );
    SetSizeHints( wxSize( -1, -1 ), displaySize );
    
    if ( g_bresponsive )
        SetSize( displaySize );
    else {
        GetSizer()->Fit( this );
        SetSize( wxSize( GetCharHeight() * 40, GetCharHeight() * 30 ) );
    }

    Centre();

    return TRUE;
}

void about::SetColorScheme( void )
{
    DimeControl( this );
    wxColor bg = GetBackgroundColour();
    pAboutHTMLCtl->SetBackgroundColour( bg );

    // This looks like non-sense, but is needed for __WXGTK__
    // to get colours to propagate down the control's family tree.
    SetBackgroundColour( bg );
    
#ifdef __WXQT__
    // wxQT has some trouble clearing the background of HTML window...
    wxBitmap tbm( GetSize().x, GetSize().y, -1 );
    wxMemoryDC tdc( tbm );
    tdc.SetBackground( bg );
    tdc.Clear();
    pAboutHTMLCtl->SetBackgroundImage(tbm);
#endif

}

void about::Populate( void )
{
    wxColor bg = GetBackgroundColour();
    wxColor fg = wxColour( 0, 0, 0 );
    
    // The HTML Header
    wxString aboutText =
    wxString::Format(
                     _T( "<html><body bgcolor=#%02x%02x%02x><font color=#%02x%02x%02x>" ),
                     bg.Red(), bg.Blue(), bg.Green(), fg.Red(), fg.Blue(), fg.Green() );
    
    wxFont *dFont = FontMgr::Get().GetFont( _("Dialog") );
    
    int points = dFont->GetPointSize();
#ifndef __WXOSX__
    ++points;
#endif
    int sizes[7];
    for ( int i = -2; i < 5; i++ ) {
        sizes[i+2] = points + i + ( i > 0 ? i : 0 );
    }
    wxString face = dFont->GetFaceName();
    pAboutHTMLCtl->SetFonts( face, face, sizes );

    if( wxFONTSTYLE_ITALIC == dFont->GetStyle() )
        aboutText.Append( _T("<i>") );
    aboutText.Append( AboutText + OpenCPNVersion + OpenCPNInfo );

    // Show where the log file is going to be placed
#ifdef __WXOSX__
    aboutText.Append( _T("Ort der Logdatei: ") + g_Platform->GetLogFileName() );
#else
    aboutText.Append( _T("Logfile location: ") + g_Platform->GetLogFileName() );
#endif

    // Show where the config file is going to be placed
#ifdef __WXOSX__
    aboutText.Append( _T("<br><br>Ort der Konfig-Datei: ") + g_Platform->GetConfigFileName() );
#else
    aboutText.Append( _T("<br><br>Config file location: ") + g_Platform->GetConfigFileName() );
#endif

    if(wxFONTSTYLE_ITALIC == dFont->GetStyle())
        aboutText.Append( _T("</i>") );
    
    // The HTML Footer
    aboutText.Append( _T("</font></body></html>") );
    
    pAboutHTMLCtl->SetPage( aboutText );
    pAuthorTextCtl->Clear();
    pAuthorTextCtl->WriteText( AuthorText );
    pAuthorTextCtl->SetInsertionPoint( 0 );
    
    wxTextFile license_file( m_DataLocn + _T("license.txt") );
    if ( license_file.Open() ) {
        for ( wxString str = license_file.GetFirstLine(); !license_file.Eof() ; str = license_file.GetNextLine() )
            pLicenseTextCtl->AppendText( str + '\n' );
        license_file.Close();
    } else {
        wxLogMessage( _T("Could not open License file: ") + m_DataLocn );
    }
    pLicenseTextCtl->SetInsertionPoint( 0 );

#ifdef __WXOSX__
    const wxString HilfeText = wxT(
    "Für Hilfe gehen Sie bitte zum OS X Menü Hilfe oben in der Menüzeile. Geben Sie dort Suchbegriffe ein wie z.B. Plugins.");
    pHelpTextCtl->Clear();
    pHelpTextCtl->WriteText( HilfeText );
    pHelpTextCtl->SetInsertionPoint ( 0 );
#endif

    SetColorScheme();
}

void about::CreateControls( void )
{
    //  Set the nominal vertical size of the embedded controls
    int v_size = g_bresponsive ? -1 : 300;
    
    wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( mainSizer );
    wxStaticText *pST1 = new wxStaticText( this, -1,
                                          _("The Open Source Chart Plotter/Navigator"), wxDefaultPosition,
                                          wxSize( -1, 30 /* 500, 30 */ ), wxALIGN_CENTRE /* | wxALIGN_CENTER_VERTICAL */ );

    
    wxFont *headerFont = wxTheFontList->FindOrCreateFont( 14, wxFONTFAMILY_SWISS,
            wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
    pST1->SetFont( *headerFont );
    mainSizer->Add( pST1, 0, wxALL | wxEXPAND, 8 );

    wxSizer *buttonSizer = new wxBoxSizer( m_displaySize.x < m_displaySize.y ?
                                          wxVERTICAL : wxHORIZONTAL );
    wxButton* donateButton = new wxBitmapButton( this, ID_DONATE,
            g_StyleManager->GetCurrentStyle()->GetIcon( _T("donate") ),
            wxDefaultPosition, wxDefaultSize, 0 );

    buttonSizer->Add( new wxButton( this, ID_COPYLOG, _("Copy Log File to Clipboard") ), 1, wxALL | wxEXPAND, 3 );
    buttonSizer->Add( new wxButton( this, ID_COPYINI, _("Copy Settings File to Clipboard") ), 1, wxALL | wxEXPAND, 3 );
#ifdef __WXOSX__
    buttonSizer->Add( donateButton, 1, wxALL | wxEXPAND, 3 );
#else
    buttonSizer->Add( donateButton, 1, wxALL | wxEXPAND | wxALIGN_RIGHT, 3 );
#endif

    //  Main Notebook
    pNotebook = new wxNotebook( this, ID_NOTEBOOK_HELP, wxDefaultPosition,
            wxSize( -1, -1 ), wxNB_TOP );
    pNotebook->InheritAttributes();
#ifdef __WXOSX__
    mainSizer->Add( pNotebook, 1, wxEXPAND | wxALL, 5 );
#else
    mainSizer->Add( pNotebook, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5 );
#endif
    mainSizer->Add( buttonSizer, 0, wxALL, 0 );

    //  About Panel
    itemPanelAbout = new wxPanel( pNotebook, -1, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    itemPanelAbout->InheritAttributes();
    pNotebook->AddPage( itemPanelAbout, _("About"), TRUE /* Default page */ );

    pAboutHTMLCtl = new wxHtmlWindow( itemPanelAbout, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                     wxHW_SCROLLBAR_AUTO | wxHW_NO_SELECTION);

    pAboutHTMLCtl->SetBorders( 5 );
    wxBoxSizer* aboutSizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    aboutSizer->Add( pAboutHTMLCtl, 1, wxEXPAND | wxALL, 5 );
#else
    aboutSizer->Add( pAboutHTMLCtl, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, 5 );
#endif
    itemPanelAbout->SetSizer( aboutSizer );

    //  Authors Panel

    itemPanelAuthors = new wxPanel( pNotebook, -1, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    itemPanelAuthors->InheritAttributes();
    pNotebook->AddPage( itemPanelAuthors, _("Authors") );

    pAuthorTextCtl = new wxTextCtrl( itemPanelAuthors, -1, _T(""), wxDefaultPosition,
                                    wxSize( -1, v_size ), wxTE_MULTILINE | wxTE_READONLY );
    pAuthorTextCtl->InheritAttributes();
    wxBoxSizer* authorSizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    authorSizer->Add( pAuthorTextCtl, 1, wxEXPAND | wxALL, 5 );
#else
    authorSizer->Add( pAuthorTextCtl, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, 5 );
#endif
    itemPanelAuthors->SetSizer( authorSizer );

    //  License Panel
    itemPanelLicense = new wxPanel( pNotebook, -1, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    itemPanelLicense->InheritAttributes();
    pNotebook->AddPage( itemPanelLicense, _("License") );

    int tcflags = wxTE_MULTILINE | wxTE_READONLY;
    //  wxX11 TextCtrl is broken in many ways.
    //  Here, the wxTE_DONTWRAP flag creates a horizontal scroll bar
    //  which fails in wxX11 2.8.2....
#ifndef __WXX11__
    tcflags |= wxTE_DONTWRAP;
#endif
    pLicenseTextCtl = new wxTextCtrl( itemPanelLicense, -1, _T(""), wxDefaultPosition,
                                     wxSize( -1, v_size ), tcflags );

    pLicenseTextCtl->InheritAttributes();
    wxBoxSizer* licenseSizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    licenseSizer->Add( pLicenseTextCtl, 1, wxEXPAND | wxALL, 5 );
#else
    licenseSizer->Add( pLicenseTextCtl, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, 5 );
#endif
    itemPanelLicense->SetSizer( licenseSizer );

    //  Help Panel
    itemPanelTips = new wxPanel( pNotebook, -1, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    itemPanelTips->InheritAttributes();
    pNotebook->AddPage( itemPanelTips, _("Help") );

    wxBoxSizer* helpSizer = new wxBoxSizer( wxVERTICAL );
    itemPanelTips->SetSizer( helpSizer );
#ifdef __WXOSX__
    pHelpTextCtl = new wxTextCtrl (itemPanelTips, -1, _T(""), wxDefaultPosition,
                                   wxSize(-1, v_size), tcflags );
    pHelpTextCtl->InheritAttributes();
    helpSizer->Add(pHelpTextCtl,0, wxEXPAND | wxALL, 5 );
#endif
    
    //  Close Button
    wxButton* closeButton = new wxButton( this, xID_OK, _("Close"), wxDefaultPosition,
            wxDefaultSize, 0 );
    closeButton->SetDefault();
    closeButton->InheritAttributes();
    mainSizer->Add( closeButton, 0, wxALIGN_RIGHT | wxALL, 5 );
}


void about::OnXidOkClick( wxCommandEvent& event )
{
  Close();
}

void about::OnClose( wxCloseEvent& event )
{
    Destroy();
    g_pAboutDlg = NULL;
}

void about::OnDonateClick( wxCommandEvent& event )
{
      wxLaunchDefaultBrowser(_T("https://sourceforge.net/donate/index.php?group_id=180842"));
}

void about::OnCopyClick( wxCommandEvent& event )
{
    wxString filename = event.GetId() == ID_COPYLOG ?
    g_Platform->GetLogFileName() : g_Platform->GetConfigFileName();

    wxFFile file( filename );

    if( !file.IsOpened() ) {
        wxLogMessage( _T("Failed to open file for Copy to Clipboard.") );
        return;
    }

    wxString fileContent;
    char buf[1024];
    while( ! file.Eof() ) {
        int c = file.Read( &buf, 1024 );
        if( c ) fileContent += wxString( buf, wxConvUTF8, c );
    }

    file.Close();
//    int length = fileContent.Length();  // Not used

    if( event.GetId() == ID_COPYLOG ) {
        wxString lastLogs = fileContent;
        int pos = lastLogs.Find( _T("________") );
        while( pos != wxNOT_FOUND && lastLogs.Length() > 65000 ) {
            lastLogs = lastLogs.Right(lastLogs.Length() - pos - 8);
            pos = lastLogs.Find( _T("________") );
        }
        fileContent = lastLogs;
    }

    ::wxBeginBusyCursor();
    if( wxTheClipboard->Open() ) {
        if( !wxTheClipboard->SetData( new wxTextDataObject( fileContent ) ) )
            wxLogMessage( _T("wxTheClipboard->Open() failed.") );
        wxTheClipboard->Close();
    } else {
        wxLogMessage( _T("wxTheClipboard->Open() failed.") );
    }
    ::wxEndBusyCursor();
}

void about::OnPageChange( wxNotebookEvent& event )
{
    if( event.GetSelection() != 3 ) return; // 3 is the index of "Help" page
#ifndef __WXOSX__
        gFrame->LaunchLocalHelp();
#endif
//    pNotebook->ChangeSelection(0);
}
