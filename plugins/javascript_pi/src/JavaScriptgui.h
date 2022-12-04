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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stc/stc.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/dialog.h>

// /////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////////////////////////////////
// / Class m_Console
// /////////////////////////////////////////////////////////////////////////////
class m_Console : public wxDialog
{
	private:

	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxButton* m_clearScript;
		wxButton* copy_button;
		wxButton* load_button;
		wxButton* save_button;
		wxButton* save_as_button;
		wxButton* tools_button;
		wxStaticLine* m_staticline1;
		wxPanel* m_panel2;
		wxStaticLine* m_staticline2;
		wxButton* m_clearOutput;

		// Virtual event handlers, overide them in your derived class
		virtual void OnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onMouse( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnClearScript( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCopyAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAutoRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTools( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearOutput( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxButton* run_button;
		wxCheckBox* auto_run;
		wxTextCtrl* m_fileStringBox;
		wxStyledTextCtrl* m_Script;
		wxStyledTextCtrl* m_Output;

		m_Console( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("JavaScript"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 738,741 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~m_Console();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 600 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( m_Console::m_splitter1OnIdle ), NULL, this );
		}

};

