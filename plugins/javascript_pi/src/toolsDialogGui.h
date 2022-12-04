// /////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
// /////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

// /////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////////////////////////////////
// / Class ToolsClassBase
// /////////////////////////////////////////////////////////////////////////////
class ToolsClassBase : public wxDialog
{
	private:

	protected:
		wxPanel* Consoles;
		wxStaticText* m_prompt;
		wxTextCtrl* m_newConsoleName;
		wxTextCtrl* m_ConsolesMessage;
		wxPanel* Directory;
		wxStaticText* mCurrentDirectory;
		wxStaticText* mCurrentDirectoryString;
		wxPanel* NMEA;
		wxStaticText* NMEAtopText;
		wxStaticText* m_NMEAMessageText;
		wxButton* m_NMEAReceiveMessageButton;
		wxPanel* Message;
		wxStaticText* MessageTopText;
		wxStaticText* m_MessageIDText1;
		wxButton* m_receiveMessageButton;
		wxStaticText* m_MessageText11;
		wxPanel* Diagnostics;
		wxStaticText* m_diagnosticAdvice;
		wxStaticText* m_dumpPrompt;
		wxButton* m_dumpButton;
		wxTextCtrl* m_charsToClean;
		wxStaticText* m_cleanPrompt;
		wxButton* m_cleanButton;

		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void onAddConsole( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChangeDirectory( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRecieveNMEAmessage( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRecieveMessage( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDump( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClean( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxNotebook* m_notebook;
		wxButton* m_addButton;
		wxButton* mDirectoryChangeButton;
		wxTextCtrl* m_NMEAmessage;
		wxTextCtrl* m_MessageID;
		wxTextCtrl* m_MessageBody;
		wxString mConsoleNameInput;

		ToolsClassBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("JavaScript tools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~ToolsClassBase();

};

