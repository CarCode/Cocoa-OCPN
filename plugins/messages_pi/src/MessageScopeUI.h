///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 15 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGESCOPEUI_H__
#define __MESSAGESCOPEUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MessageScopeDialogBase
///////////////////////////////////////////////////////////////////////////////
class MessageScopeDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxScrolledWindow* m_scrolledWindow1;
		wxStaticText* m_stMessage;
		wxPanel* m_panel2;
		wxListCtrl* m_lMessages;
		wxButton* m_bClear;
		wxButton* m_bCloase;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnMessageSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MessageScopeDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Messages"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,450 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL ); 
		~MessageScopeDialogBase();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 200 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MessageScopeDialogBase::m_splitter1OnIdle ), NULL, this );
		}
	
};

#endif //__MESSAGESCOPEUI_H__
