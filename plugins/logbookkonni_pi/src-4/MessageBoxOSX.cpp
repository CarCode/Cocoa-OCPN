//#pragma once
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif
#include "MessageBoxOSX.h"

MessageBoxOSX_::MessageBoxOSX_( wxWindow* parent, wxString str,  const wxString& title, int buttons, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_sdbSizerOK = NULL;
	m_sdbSizerNo = NULL;
	m_sdbSizerCancel = NULL;

	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _T("\n")+str+_T("\n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	if((buttons & wxID_OK) == wxID_OK)
	{
		m_sdbSizerOK = new wxButton( this, wxID_OK );
		m_sdbSizer->AddButton( m_sdbSizerOK );
		m_sdbSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnOKClick ), NULL, this );
	}
	if((buttons & wxID_NO) == wxID_NO)
	{
		m_sdbSizerNo = new wxButton( this, wxID_NO );
		m_sdbSizer->AddButton( m_sdbSizerNo );
		m_sdbSizerNo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnNoClick ), NULL, this );
	}
	if((buttons & wxID_CANCEL) == wxID_CANCEL)
	{
		m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
		m_sdbSizer->AddButton( m_sdbSizerCancel );
		m_sdbSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnCancelClick ), NULL, this );
	}

	m_sdbSizer->Realize();
	mainSizer->Add( m_sdbSizer, 0, wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MessageBoxOSX_::OnCloseDialog ) );
	Fit();
}

MessageBoxOSX_::~MessageBoxOSX_()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MessageBoxOSX_::OnCloseDialog ) );
	if(m_sdbSizerCancel)
		m_sdbSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnCancelClick ), NULL, this );
	if(m_sdbSizerOK)
		m_sdbSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnOKClick ), NULL, this );
	if(m_sdbSizerNo)
		m_sdbSizerNo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessageBoxOSX_::OnNoClick ), NULL, this );
		
}
void MessageBoxOSX_::OnCloseDialog(wxCloseEvent& event)
{
	event.Skip();
}

void MessageBoxOSX_::OnOKClick(wxCommandEvent& event)
{
	returnint = wxID_OK;
	Close();
	event.Skip();
}

void MessageBoxOSX_::OnCancelClick(wxCommandEvent& event)
{
	returnint = wxID_CANCEL;
	Close();
	event.Skip();
}

void MessageBoxOSX_::OnNoClick(wxCommandEvent& event)
{
	returnint = wxID_NO;
	Close();
	event.Skip();
}

int MessageBoxOSX_::ShowModal_()
{
	this->ShowModal();
	return returnint;
}

////////////// global Function ////////////
int MessageBoxOSX(wxWindow* parent, wxString str, wxString title, int buttons)
{
	MessageBoxOSX_ x(parent,str,title,buttons);
	int i = x.ShowModal_();
	return i;
}