#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

int MessageBoxOSX(wxWindow* parent, wxString str,  wxString title = _(""), int buttons = wxID_NO|wxID_CANCEL|wxID_OK);
class MessageBoxOSX_ : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerNo;
		wxButton* m_sdbSizerCancel;
		int returnint;
		
		// Virtual event handlers, overide them in your derived class
		void OnCloseDialog( wxCloseEvent& event );
		void OnCancelClick( wxCommandEvent& event );
		void OnOKClick( wxCommandEvent& event );
		void OnNoClick( wxCommandEvent& event );
		
	
	public:
		MessageBoxOSX_( wxWindow* parent, wxString str,  const wxString& title = _(""), int buttons = wxID_OK, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 286,108 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE);
		~MessageBoxOSX_();
		int ShowModal_();
	
};