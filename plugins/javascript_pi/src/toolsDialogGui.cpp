// /////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
// /////////////////////////////////////////////////////////////////////////

#include "toolsDialogGui.h"

// /////////////////////////////////////////////////////////////////////////

ToolsClassBase::ToolsClassBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );

    wxBoxSizer* TopSizer;
    TopSizer = new wxBoxSizer( wxVERTICAL );

    TopSizer->SetMinSize( wxSize( 620,600 ) );
    m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( 600,190 ), 0 );
    m_notebook->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    Consoles = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( 600,190 ), 0 );
    Consoles->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande") ) );
    Consoles->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
    Consoles->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    wxBoxSizer* ConsolesSizer;
    ConsolesSizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* ConsolesChoiceSizer;
    ConsolesChoiceSizer = new wxStaticBoxSizer( new wxStaticBox( Consoles, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
#ifdef __WXOSX__
    m_prompt = new wxStaticText( ConsolesChoiceSizer->GetStaticBox(), wxID_ANY, _("Add a new console with name  "), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_NONE );
#else
    m_prompt = new wxStaticText( ConsolesChoiceSizer->GetStaticBox(), wxID_ANY, wxT("Add a new console with name  "), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_NONE );
#endif
    m_prompt->Wrap( -1 );
    m_prompt->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    ConsolesChoiceSizer->Add( m_prompt, 0, wxALL, 5 );

    m_newConsoleName = new wxTextCtrl( ConsolesChoiceSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), 0 );
#ifdef __WXGTK__
    if ( !m_newConsoleName->HasFlag( wxTE_MULTILINE ) )
    {
        m_newConsoleName->SetMaxLength( 14 );
    }
#else
    m_newConsoleName->SetMaxLength( 14 );
#endif
    m_newConsoleName->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande") ) );
    m_newConsoleName->SetMinSize( wxSize( 120,-1 ) );
    m_newConsoleName->SetMaxSize( wxSize( 110,-1 ) );

    m_newConsoleName->SetValidator( wxTextValidator( wxFILTER_ALPHANUMERIC, &mConsoleNameInput ) );

    ConsolesChoiceSizer->Add( m_newConsoleName, 0, wxALL, 2 );


    ConsolesChoiceSizer->Add( 130, 0, 1, 0, 5 );
#ifdef __WXOSX__
    m_addButton = new wxButton( ConsolesChoiceSizer->GetStaticBox(), wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_addButton = new wxButton( ConsolesChoiceSizer->GetStaticBox(), wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    ConsolesChoiceSizer->Add( m_addButton, 0, wxALL, 5 );


    ConsolesSizer->Add( ConsolesChoiceSizer, 0, 0, 0 );

    wxStaticBoxSizer* ConsolesMessageSizer;
    ConsolesMessageSizer = new wxStaticBoxSizer( new wxStaticBox( Consoles, wxID_ANY, wxEmptyString ), wxVERTICAL );

    m_ConsolesMessage = new wxTextCtrl( ConsolesMessageSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 550,-1 ), 0|wxBORDER_NONE );
    m_ConsolesMessage->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande") ) );
    m_ConsolesMessage->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
    m_ConsolesMessage->SetBackgroundColour( wxColour( 242, 242, 242 ) );

    ConsolesMessageSizer->Add( m_ConsolesMessage, 0, wxALL, 5 );


    ConsolesSizer->Add( ConsolesMessageSizer, 0, 0, 0 );


    Consoles->SetSizer( ConsolesSizer );
    Consoles->Layout();
    m_notebook->AddPage( Consoles, _("Consoles"), false );
    Directory = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( 600,160 ), 0 );
    Directory->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande") ) );
    Directory->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
    Directory->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    wxBoxSizer* DirectorySizer1;
    DirectorySizer1 = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* DirectorySizer;
    DirectorySizer = new wxStaticBoxSizer( new wxStaticBox( Directory, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
#ifdef __WXOSX__
    mCurrentDirectory = new wxStaticText( DirectorySizer->GetStaticBox(), wxID_ANY, _("Current directory"), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_NONE );
#else
    mCurrentDirectory = new wxStaticText( DirectorySizer->GetStaticBox(), wxID_ANY, wxT("Current directory"), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_NONE );
#endif
    mCurrentDirectory->Wrap( -1 );
    mCurrentDirectory->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    DirectorySizer->Add( mCurrentDirectory, 0, wxALL, 5 );


    DirectorySizer->Add( 130, 0, 1, wxEXPAND, 5 );
#ifdef __WXOSX__
    mDirectoryChangeButton = new wxButton( DirectorySizer->GetStaticBox(), wxID_ANY, _("Change..."), wxDefaultPosition, wxDefaultSize, 0 );
#else
    mDirectoryChangeButton = new wxButton( DirectorySizer->GetStaticBox(), wxID_ANY, wxT("Change..."), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    mDirectoryChangeButton->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );

    DirectorySizer->Add( mDirectoryChangeButton, 0, wxALL, 5 );


    DirectorySizer1->Add( DirectorySizer, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer11;
    bSizer11 = new wxBoxSizer( wxHORIZONTAL );


    bSizer11->Add( 12, 0, 0, wxEXPAND, 5 );
#ifdef __WXOSX__
    mCurrentDirectoryString = new wxStaticText( Directory, wxID_ANY, _("Will be directory string"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    mCurrentDirectoryString = new wxStaticText( Directory, wxID_ANY, wxT("Will be directory string"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    mCurrentDirectoryString->Wrap( -1 );
    mCurrentDirectoryString->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
    mCurrentDirectoryString->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    bSizer11->Add( mCurrentDirectoryString, 0, wxALL, 5 );


    DirectorySizer1->Add( bSizer11, 0, wxEXPAND, 5 );


    Directory->SetSizer( DirectorySizer1 );
    Directory->Layout();
    m_notebook->AddPage( Directory, wxT("Directory"), false );
    NMEA = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( 600,190 ), 0 );
    NMEA->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );

    wxBoxSizer* NMEASizer;
    NMEASizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    NMEAtopText = new wxStaticText( NMEA, wxID_ANY, _("Receive an NMEA message into plugin         (will not be seen by OpenCPN)"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    NMEAtopText = new wxStaticText( NMEA, wxID_ANY, wxT("Receive an NMEA message into plugin         (will not be seen by OpenCPN)"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    NMEAtopText->Wrap( -1 );
    NMEASizer->Add( NMEAtopText, 0, wxALL, 5 );

    wxStaticBoxSizer* NMEASizer1;
    NMEASizer1 = new wxStaticBoxSizer( new wxStaticBox( NMEA, wxID_ANY, wxEmptyString ), wxHORIZONTAL );

    m_NMEAMessageText = new wxStaticText( NMEASizer1->GetStaticBox(), wxID_ANY, wxT("OCPN message"), wxDefaultPosition, wxSize( 600,-1 ), 0 );
    m_NMEAMessageText->Wrap( -1 );
    m_NMEAMessageText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
    m_NMEAMessageText->Hide();

    NMEASizer1->Add( m_NMEAMessageText, 0, wxALL, 5 );

    m_NMEAmessage = new wxTextCtrl( NMEASizer1->GetStaticBox(), wxID_ANY, wxT("$GPRMC,152912,A,5041.0535,N,00205.3070,W,0.06,0,070600,,,A,V*3D"), wxDefaultPosition, wxSize( 540,-1 ), 0 );
    m_NMEAmessage->SetMaxSize( wxSize( 580,-1 ) );

    NMEASizer1->Add( m_NMEAmessage, 0, wxALL, 5 );


    NMEASizer->Add( NMEASizer1, 0, 0, 5 );

    wxBoxSizer* bNMEAbuttonBoxSizer;
    bNMEAbuttonBoxSizer = new wxBoxSizer( wxHORIZONTAL );


    bNMEAbuttonBoxSizer->Add( 0, 0, 1, 0, 5 );
#ifdef __WXOSX__
    m_NMEAReceiveMessageButton = new wxButton( NMEA, wxID_ANY, _("Receive"), wxDefaultPosition, wxDefaultSize, 0 );
    m_NMEAReceiveMessageButton->SetLabelMarkup( _("Receive") );
#else
    m_NMEAReceiveMessageButton = new wxButton( NMEA, wxID_ANY, wxT("Receive"), wxDefaultPosition, wxDefaultSize, 0 );
    m_NMEAReceiveMessageButton->SetLabelMarkup( wxT("Receive") );
#endif
    bNMEAbuttonBoxSizer->Add( m_NMEAReceiveMessageButton, 0, wxALL, 5 );


    NMEASizer->Add( bNMEAbuttonBoxSizer, 0, wxEXPAND, 5 );


    NMEA->SetSizer( NMEASizer );
    NMEA->Layout();
    m_notebook->AddPage( NMEA, wxT("NMEA"), false );
    Message = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( 600,250 ), 0 );
    Message->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );

    wxBoxSizer* MessageSizer;
    MessageSizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    MessageTopText = new wxStaticText( Message, wxID_ANY, _("Receive an OPCN message into plugin         (will not be seen by OpenCPN)"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    MessageTopText = new wxStaticText( Message, wxID_ANY, wxT("Receive an OPCN message into plugin         (will not be seen by OpenCPN)"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    MessageTopText->Wrap( -1 );
    MessageSizer->Add( MessageTopText, 0, wxALL, 5 );

    wxStaticBoxSizer* MessageIDSizer;
    MessageIDSizer = new wxStaticBoxSizer( new wxStaticBox( Message, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
#ifdef __WXOSX__
    m_MessageIDText1 = new wxStaticText( MessageIDSizer->GetStaticBox(), wxID_ANY, _("Message ID"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_MessageIDText1 = new wxStaticText( MessageIDSizer->GetStaticBox(), wxID_ANY, wxT("Message ID"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    m_MessageIDText1->Wrap( -1 );
    m_MessageIDText1->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    MessageIDSizer->Add( m_MessageIDText1, 0, wxALL, 5 );

    m_MessageID = new wxTextCtrl( MessageIDSizer->GetStaticBox(), wxID_ANY, wxT("EXAMPLE_MESSAGE_ID"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MessageID->SetMinSize( wxSize( 300,-1 ) );

    MessageIDSizer->Add( m_MessageID, 0, wxALL, 5 );


    MessageIDSizer->Add( 0, 0, 1, 0, 5 );
#ifdef __WXOSX__
    m_receiveMessageButton = new wxButton( MessageIDSizer->GetStaticBox(), wxID_ANY, _("Receive"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_receiveMessageButton = new wxButton( MessageIDSizer->GetStaticBox(), wxID_ANY, wxT("Receive"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    MessageIDSizer->Add( m_receiveMessageButton, 1, wxALL, 5 );


    MessageSizer->Add( MessageIDSizer, 0, 0, 5 );

    wxStaticBoxSizer* MessageBodySizer1;
    MessageBodySizer1 = new wxStaticBoxSizer( new wxStaticBox( Message, wxID_ANY, wxEmptyString ), wxVERTICAL );

    MessageBodySizer1->SetMinSize( wxSize( -1,250 ) );
#ifdef __WXOSX__
    m_MessageText11 = new wxStaticText( MessageBodySizer1->GetStaticBox(), wxID_ANY, _("Message"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_MessageText11 = new wxStaticText( MessageBodySizer1->GetStaticBox(), wxID_ANY, wxT("Message"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    m_MessageText11->Wrap( -1 );
    m_MessageText11->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    MessageBodySizer1->Add( m_MessageText11, 0, wxALL, 5 );

    m_MessageBody = new wxTextCtrl( MessageBodySizer1->GetStaticBox(), wxID_ANY, wxT("“Latitude” : 61.234,\n“Longitude\" : -1.234"), wxDefaultPosition, wxSize( 580,250 ), wxTE_MULTILINE|wxTE_WORDWRAP|wxVSCROLL );
    m_MessageBody->SetMinSize( wxSize( 580,200 ) );

    MessageBodySizer1->Add( m_MessageBody, 0, wxALL, 5 );


    MessageSizer->Add( MessageBodySizer1, 0, wxFIXED_MINSIZE, 5 );


    Message->SetSizer( MessageSizer );
    Message->Layout();
#ifdef __WXOSX__
    m_notebook->AddPage( Message, _("Message"), false );
#else
    m_notebook->AddPage( Message, wxT("Message"), false );
#endif
    Diagnostics = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxSize( 600,250 ), wxTAB_TRAVERSAL );
    Diagnostics->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
    Diagnostics->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );

    wxBoxSizer* DiagnosticsTpSizer;
    DiagnosticsTpSizer = new wxBoxSizer( wxVERTICAL );

    DiagnosticsTpSizer->SetMinSize( wxSize( -1,250 ) );
    wxStaticBoxSizer* diagnosticAdviceSizer;
    diagnosticAdviceSizer = new wxStaticBoxSizer( new wxStaticBox( Diagnostics, wxID_ANY, wxEmptyString ), wxVERTICAL );
#ifdef __WXOSX__
    m_diagnosticAdvice = new wxStaticText( diagnosticAdviceSizer->GetStaticBox(), wxID_ANY, _("For help using these functions, see the section 'Preferences panel diagnostics tab'  in the plugin technical guide."), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_diagnosticAdvice = new wxStaticText( diagnosticAdviceSizer->GetStaticBox(), wxID_ANY, wxT("For help using these functions, see the section ‘Preferences panel diagnostics tab’  in the plugin technical guide."), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    m_diagnosticAdvice->Wrap( -1 );
    m_diagnosticAdvice->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande") ) );
    m_diagnosticAdvice->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    diagnosticAdviceSizer->Add( m_diagnosticAdvice, 0, wxALL, 0 );


    DiagnosticsTpSizer->Add( diagnosticAdviceSizer, 0, 0, 5 );

    wxStaticBoxSizer* DumpTopSizer;
    DumpTopSizer = new wxStaticBoxSizer( new wxStaticBox( Diagnostics, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
#ifdef __WXOSX__
    m_dumpPrompt = new wxStaticText( DumpTopSizer->GetStaticBox(), wxID_ANY, _("Plugin and consoles diagnostic information"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_dumpPrompt = new wxStaticText( DumpTopSizer->GetStaticBox(), wxID_ANY, wxT("Plugin and consoles diagnostic information"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    m_dumpPrompt->Wrap( -1 );
    m_dumpPrompt->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    DumpTopSizer->Add( m_dumpPrompt, 0, wxALL, 5 );


    DumpTopSizer->Add( 28, 0, 1, wxEXPAND, 5 );

    m_dumpButton = new wxButton( DumpTopSizer->GetStaticBox(), wxID_ANY, wxT("Dump"), wxDefaultPosition, wxDefaultSize, 0 );
    DumpTopSizer->Add( m_dumpButton, 0, wxALL, 5 );


    DiagnosticsTpSizer->Add( DumpTopSizer, 0, wxEXPAND, 5 );

    wxBoxSizer* cleanSizer;
    cleanSizer = new wxBoxSizer( wxVERTICAL );

    cleanSizer->SetMinSize( wxSize( -1,150 ) );
    m_charsToClean = new wxTextCtrl( Diagnostics, wxID_ANY, wxT("‟Fancy quotes” ordinal º primes ‘’‛’′´`"), wxDefaultPosition, wxSize( 450,-1 ), 0 );
    m_charsToClean->SetMinSize( wxSize( 500,-1 ) );

    cleanSizer->Add( m_charsToClean, 0, wxALL, 5 );

    wxStaticBoxSizer* CharsTopSizer;
    CharsTopSizer = new wxStaticBoxSizer( new wxStaticBox( Diagnostics, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
#ifdef __WXOSX__
    m_cleanPrompt = new wxStaticText( CharsTopSizer->GetStaticBox(), wxID_ANY, _("Clean and dump string"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_cleanPrompt = new wxStaticText( CharsTopSizer->GetStaticBox(), wxID_ANY, wxT("Clean and dump string"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    m_cleanPrompt->Wrap( -1 );
    m_cleanPrompt->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );

    CharsTopSizer->Add( m_cleanPrompt, 0, wxALL, 5 );


    CharsTopSizer->Add( 40, 0, 1, wxEXPAND, 5 );
#ifdef __WXOSX__
    m_cleanButton = new wxButton( CharsTopSizer->GetStaticBox(), wxID_ANY, _("Clean"), wxDefaultPosition, wxDefaultSize, 0 );
#else
    m_cleanButton = new wxButton( CharsTopSizer->GetStaticBox(), wxID_ANY, wxT("Clean"), wxDefaultPosition, wxDefaultSize, 0 );
#endif
    CharsTopSizer->Add( m_cleanButton, 0, wxALL, 5 );


    cleanSizer->Add( CharsTopSizer, 0, wxEXPAND, 0 );


    DiagnosticsTpSizer->Add( cleanSizer, 1, wxEXPAND|wxFIXED_MINSIZE, 5 );


    Diagnostics->SetSizer( DiagnosticsTpSizer );
    Diagnostics->Layout();
#ifdef __WXOSX__
    m_notebook->AddPage( Diagnostics, _("Diagnostics"), false );
#else
    m_notebook->AddPage( Diagnostics, wxT("Diagnostics"), false );
#endif
    TopSizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );


    this->SetSizer( TopSizer );
    this->Layout();
    TopSizer->Fit( this );

    // Connect Events
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ToolsClassBase::onClose ) );
    m_notebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( ToolsClassBase::onPageChanged ), NULL, this );
    m_addButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onAddConsole ), NULL, this );
    mDirectoryChangeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onChangeDirectory ), NULL, this );
    m_NMEAReceiveMessageButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onRecieveNMEAmessage ), NULL, this );
    m_receiveMessageButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onRecieveMessage ), NULL, this );
    m_dumpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onDump ), NULL, this );
    m_cleanButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onClean ), NULL, this );
}

ToolsClassBase::~ToolsClassBase()
{
    // Disconnect Events
    this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ToolsClassBase::onClose ) );
    m_notebook->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( ToolsClassBase::onPageChanged ), NULL, this );
    m_addButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onAddConsole ), NULL, this );
    mDirectoryChangeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onChangeDirectory ), NULL, this );
    m_NMEAReceiveMessageButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onRecieveNMEAmessage ), NULL, this );
    m_receiveMessageButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onRecieveMessage ), NULL, this );
    m_dumpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onDump ), NULL, this );
    m_cleanButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ToolsClassBase::onClean ), NULL, this );

}
