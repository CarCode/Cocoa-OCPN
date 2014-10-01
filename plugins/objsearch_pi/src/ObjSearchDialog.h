///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OBJSEARCHDIALOG_H__
#define __OBJSEARCHDIALOG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/checklst.h>
#include <wx/combo.h>
#include "clcpopup.h"
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ObjSearchDialog
///////////////////////////////////////////////////////////////////////////////
class ObjSearchDialog : public wxDialog
{
	private:
	
	protected:
		wxGenericComboCtrl *m_choiceFeature;
		wxTextCtrl* m_textCtrlSearchTerm;
		wxButton* m_buttonSearch;
		wxListCtrl* m_listCtrlResults;
		wxButton* m_btnShowOnChart;
		wxButton* m_btnClose;
		wxCheckBox* m_cAutoClose;
		wxStaticText* m_stRange;
		wxSpinCtrl* m_scRange;
		wxStaticText* m_stUnit;
        wxButton* m_btnSettings;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSearch( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnShowOnChart( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnSettings( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ObjSearchDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Chart Object Search"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 660,480 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ObjSearchDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDialog
///////////////////////////////////////////////////////////////////////////////
class SettingsDialog : public wxDialog
{
	private:
	
	protected:
        wxNotebook* m_notebookSettings;
        wxPanel* m_panelPopulate;
        wxStaticText* m_stScanCharts;
        wxStaticText* m_staticTextFromLat;
		wxSpinCtrl* m_spFromLat;
		wxStaticText* m_staticTextFromLon;
		wxSpinCtrl* m_spFromLon;
		wxStaticText* m_staticTextToLat;
		wxSpinCtrl* m_spToLat;
		wxStaticText* m_staticTextToLon;
		wxSpinCtrl* m_spToLon;
		wxCheckBox* m_cb5000000;
		wxCheckBox* m_cb1000000;
		wxCheckBox* m_cb200000;
		wxCheckBox* m_cb20000;
        wxStaticText* m_stOr;
        wxStaticText* m_stFile;
        wxTextCtrl* m_tPath;
        wxButton* m_button4;
        wxPanel* m_panelManage;
		wxStdDialogButtonSizer* m_sdbSizerBtns;
		wxButton* m_sdbSizerBtnsOK;
		wxButton* m_sdbSizerBtnsCancel;

        // Virtual event handlers, overide them in your derived class
        virtual void OnScale( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnPathChange( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnBrowse( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnOk( wxCommandEvent& event ) { event.Skip(); }

	public:
		
    SettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Object Search Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,440 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~SettingsDialog();
	
};

#endif //__OBJSEARCHDIALOG_H__
