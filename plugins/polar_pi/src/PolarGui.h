///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __POLARGUI_H__
#define __POLARGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/tglbtn.h>
#include <wx/splitter.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PolarDlg
///////////////////////////////////////////////////////////////////////////////
class PolarDlg : public wxDialog
{
	private:
	
	public:
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText144;
		wxChoice* m_choiceRingsPolar;
		wxStaticText* m_staticText145;
		wxChoice* m_choiceDegreesPolar;
		wxStaticText* m_staticText148;
		wxChoice* m_choiceWindPolar;
		wxStaticLine* m_staticline441;
		wxStaticText* m_staticText146;
		wxChoice* m_choiceSourcePolar;
		wxStaticText* m_staticText141;
		wxChoice* m_choiceCurrentPolar;
		wxButton* m_buttonFilterPolar;
		wxStaticLine* m_staticline421;
		wxButton* m_button61;
		wxButton* m_buttonSavePolar;
		wxButton* m_buttonLoad;
		wxStaticLine* m_staticline45;
		wxButton* m_buttonClearData;
		wxToggleButton* m_toggleBtnRecord;
		
		// Virtual event handlers, overide them in your derived class
		virtual void PolarDlgOnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnSizePolarDlg( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnChoiceMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChoicePolarKnots( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChoiceDegreesPolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChoiceWindPolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChoiceSourcePolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClickFilterPolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCreatePolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClickSavePolar( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClickLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSizesplitter1( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnSplitterSashPosChanged( wxSplitterEvent& event ) { event.Skip(); }
		virtual void OnPaintPolar( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnGridCellChange( wxGridEvent& event ) { event.Skip(); }
		virtual void OnButtonClickClearData( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggleButtonRecord( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxBoxSizer* bSizerPolar;
		wxChoice* m_choiceMode;
		wxStaticLine* m_staticline431;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panelPolar;
		wxPanel* m_panel6;
		wxGrid* m_gridEdit;
		wxStaticText* m_staticTextEngine;
		
		PolarDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Polar Diagram"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 968,602 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
		~PolarDlg();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 400 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PolarDlg::m_splitter1OnIdle ), NULL, this );
		}
	
};
/*
///////////////////////////////////////////////////////////////////////////////
/// Class CollectDlg
///////////////////////////////////////////////////////////////////////////////
class CollectDlg : public wxDialog public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText9;
		wxStaticText* m_staticTextFile;
		wxGauge* m_gauge1;
	
	public:
		
		CollectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 338,74 ), long style = 0 ); CollectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 338,74 ), long style = 0 ); 
		~CollectDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FilterDlg
///////////////////////////////////////////////////////////////////////////////
class FilterDlg : public wxDialog public wxDialog 
{
	private:
		wxPanel* m_panel33;
		wxFlexGridSizer* fgSizer50;
	
	protected:
		wxNotebook* m_notebook6;
		wxPanel* m_panel4;
		wxCheckBox* m_checkBox3;
		wxCheckBox* m_checkBoxAverage;
		wxCheckBox* m_checkBoxRangePercent;
		wxChoice* m_choice6;
		wxStaticText* m_staticText152;
		wxPanel* m_panel34;
		wxStaticText* m_staticText153;
		wxStaticText* m_staticText154;
		wxStdDialogButtonSizer* m_sdbSizer11;
		wxButton* m_sdbSizer11OK;
		wxButton* m_sdbSizer11Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void PolarDlgOnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnOKButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxBoxSizer* bSizer54;
		wxBoxSizer* bSizer52;
		
		FilterDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Polar Filter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 451,470 ), long style = wxDEFAULT_DIALOG_STYLE ); FilterDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Polar Filter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 451,470 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~FilterDlg();
	
};
*/
#endif //__POLARGUI_H__
