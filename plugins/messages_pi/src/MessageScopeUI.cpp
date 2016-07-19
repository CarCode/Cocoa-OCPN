///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 15 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MessageScopeUI.h"

///////////////////////////////////////////////////////////////////////////

MessageScopeDialogBase::MessageScopeDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableRow( 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( MessageScopeDialogBase::m_splitter1OnIdle ), NULL, this );
	m_splitter1->SetMinimumPaneSize( 120 );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer17->AddGrowableCol( 0 );
	fgSizer17->AddGrowableRow( 0 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_scrolledWindow1 = new wxScrolledWindow( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableRow( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_stMessage = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_stMessage->Wrap( -1 );
	fgSizer4->Add( m_stMessage, 0, wxALL|wxEXPAND, 5 );
	
	
	m_scrolledWindow1->SetSizer( fgSizer4 );
	m_scrolledWindow1->Layout();
	fgSizer4->Fit( m_scrolledWindow1 );
	fgSizer17->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel1->SetSizer( fgSizer17 );
	m_panel1->Layout();
	fgSizer17->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer18->AddGrowableCol( 0 );
	fgSizer18->AddGrowableRow( 0 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	fgSizer18->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lMessages = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	fgSizer18->Add( m_lMessages, 0, wxALL|wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( fgSizer18 );
	m_panel2->Layout();
	fgSizer18->Fit( m_panel2 );
	m_splitter1->SplitHorizontally( m_panel1, m_panel2, 200 );
	fgSizer8->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bClear = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_bClear, 0, wxALL, 5 );
	
	m_bCloase = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_bCloase, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	fgSizer8->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_lMessages->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( MessageScopeDialogBase::OnMessageSelected ), NULL, this );
	m_lMessages->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MessageScopeDialogBase::OnMessageSelected ), NULL, this );
	m_bClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageScopeDialogBase::OnClear ), NULL, this );
	m_bCloase->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageScopeDialogBase::OnClose ), NULL, this );
}

MessageScopeDialogBase::~MessageScopeDialogBase()
{
	// Disconnect Events
	m_lMessages->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( MessageScopeDialogBase::OnMessageSelected ), NULL, this );
	m_lMessages->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( MessageScopeDialogBase::OnMessageSelected ), NULL, this );
	m_bClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageScopeDialogBase::OnClear ), NULL, this );
	m_bCloase->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageScopeDialogBase::OnClose ), NULL, this );
	
}
