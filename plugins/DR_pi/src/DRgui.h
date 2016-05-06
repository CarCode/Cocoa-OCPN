///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DRGUI_H__
#define __DRGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/event.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DlgDef
///////////////////////////////////////////////////////////////////////////////
class DlgDef : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_Speed_PS;
		wxStaticText* m_staticText33;
		wxStaticText* m_staticText31;
		wxChoice* m_Nship;
		wxStaticText* m_staticText30;
		wxButton* m_button3;
        wxStaticText* m_staticText10;
		wxStaticText* m_staticText7;
        wxBitmapButton* m_button71;
		wxTextCtrl* m_Route;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPSGPX( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
	
	public:
		wxBoxSizer* bSframe;
        virtual void onButtonClickFile( wxCommandEvent& event );

		DlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("DR"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 438,210 ), long style = wxCAPTION|wxCLOSE_BOX|wxDIALOG_NO_PARENT|wxRESIZE_BORDER|wxSYSTEM_MENU );
		~DlgDef();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CfgDlgDef
///////////////////////////////////////////////////////////////////////////////
class CfgDlgDef : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText17;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
	
	public:
		
		CfgDlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("DR preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~CfgDlgDef();
	
};

#endif //__DRGUI_H__
