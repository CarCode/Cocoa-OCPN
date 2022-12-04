// /////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
// /////////////////////////////////////////////////////////////////////////

#include "JavaScriptgui.h"

// /////////////////////////////////////////////////////////////////////////

m_Console::m_Console( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer9111;
	bSizer9111 = new wxBoxSizer( wxVERTICAL );

	bSizer9111->SetMinSize( wxSize( 600,200 ) );
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( m_Console::m_splitter1OnIdle ), NULL, this );

	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	bSizer3->SetMinSize( wxSize( 600,20 ) );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_clearScript = new wxButton( m_panel1, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_clearScript, 0, wxALL, 5 );

	copy_button = new wxButton( m_panel1, wxID_ANY, wxT("Copy all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( copy_button, 0, wxALL, 5 );

	load_button = new wxButton( m_panel1, wxID_ANY, wxT("Load"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( load_button, 0, wxALL, 5 );

	save_button = new wxButton( m_panel1, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( save_button, 0, wxALL, 5 );

	save_as_button = new wxButton( m_panel1, wxID_ANY, wxT("Save as..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( save_as_button, 0, wxALL, 5 );

	run_button = new wxButton( m_panel1, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	run_button->SetLabelMarkup( wxT("Run") );
	bSizer10->Add( run_button, 0, wxALL, 5 );

	testA_button = new wxButton( m_panel1, wxID_ANY, wxT(" Test A"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( testA_button, 0, wxALL, 5 );


	bSizer3->Add( bSizer10, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_fileString = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 600,18 ), wxTE_READONLY );
	m_fileString->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	m_fileString->SetHelpText( wxT("This is the current accessed file") );

	bSizer11->Add( m_fileString, 0, wxALL|wxEXPAND, 5 );


	bSizer3->Add( bSizer11, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_Script = new wxStyledTextCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	m_Script->SetUseTabs( true );
	m_Script->SetTabWidth( 5 );
	m_Script->SetIndent( 5 );
	m_Script->SetTabIndents( true );
	m_Script->SetBackSpaceUnIndents( true );
	m_Script->SetViewEOL( false );
	m_Script->SetViewWhiteSpace( false );
	m_Script->SetMarginWidth( 2, 0 );
	m_Script->SetIndentationGuides( true );
	m_Script->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	m_Script->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	m_Script->SetMarginWidth( 1, 16);
	m_Script->SetMarginSensitive( 1, true );
	m_Script->SetProperty( wxT("fold"), wxT("1") );
	m_Script->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	m_Script->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	m_Script->SetMarginWidth( 0, m_Script->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	{
		wxFont font = wxFont( 13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier") );
		m_Script->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
	}
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	m_Script->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	m_Script->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	m_Script->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	m_Script->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	m_Script->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	m_Script->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	m_Script->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	m_Script->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	m_Script->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	m_Script->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	m_Script->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	m_Script->SetFont( wxFont( 13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier") ) );
	m_Script->SetMinSize( wxSize( 600,500 ) );

	bSizer8->Add( m_Script, 1, wxEXPAND | wxALL, 5 );

	m_staticline1 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );


	bSizer3->Add( bSizer8, 1, wxEXPAND, 5 );


	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	bSizer14->SetMinSize( wxSize( 600,20 ) );
	m_staticline2 = new wxStaticLine( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer14->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_clearOutput = new wxButton( m_panel2, wxID_ANY, wxT("Clear output"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	bSizer14->Add( m_clearOutput, 0, wxALL, 5 );

	m_Output = new wxStyledTextCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	m_Output->SetUseTabs( true );
	m_Output->SetTabWidth( 5 );
	m_Output->SetIndent( 5 );
	m_Output->SetTabIndents( true );
	m_Output->SetBackSpaceUnIndents( true );
	m_Output->SetViewEOL( false );
	m_Output->SetViewWhiteSpace( false );
	m_Output->SetMarginWidth( 2, 0 );
	m_Output->SetIndentationGuides( true );
	m_Output->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	m_Output->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	m_Output->SetMarginWidth( 1, 16);
	m_Output->SetMarginSensitive( 1, true );
	m_Output->SetProperty( wxT("fold"), wxT("1") );
	m_Output->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	m_Output->SetMarginWidth( 0, 0 );
	{
		wxFont font = wxFont( 13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier") );
		m_Output->StyleSetFont( wxSTC_STYLE_DEFAULT, font );
	}
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	m_Output->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	m_Output->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	m_Output->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	m_Output->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	m_Output->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	m_Output->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	m_Output->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	m_Output->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	m_Output->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	m_Output->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	m_Output->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	m_Output->SetFont( wxFont( 13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier") ) );
	m_Output->SetMinSize( wxSize( 600,300 ) );

	bSizer14->Add( m_Output, 1, wxEXPAND | wxALL, 5 );


	m_panel2->SetSizer( bSizer14 );
	m_panel2->Layout();
	bSizer14->Fit( m_panel2 );
	m_splitter1->SplitHorizontally( m_panel1, m_panel2, 600 );
	bSizer9111->Add( m_splitter1, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer9111 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( m_Console::OnClose ) );
	m_clearScript->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnClearScript ), NULL, this );
	copy_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnCopyAll ), NULL, this );
	load_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnLoad ), NULL, this );
	save_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnSave ), NULL, this );
	save_as_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnSaveAs ), NULL, this );
	run_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnRun ), NULL, this );
	testA_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnTestA ), NULL, this );
	m_clearOutput->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnClearOutput ), NULL, this );
}

m_Console::~m_Console()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( m_Console::OnClose ) );
	m_clearScript->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnClearScript ), NULL, this );
	copy_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnCopyAll ), NULL, this );
	load_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnLoad ), NULL, this );
	save_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnSave ), NULL, this );
	save_as_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnSaveAs ), NULL, this );
	run_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnRun ), NULL, this );
	testA_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnTestA ), NULL, this );
	m_clearOutput->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_Console::OnClearOutput ), NULL, this );

}
