///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CALCULATORGUI_H__
#define __CALCULATORGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/choice.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DlgDef
///////////////////////////////////////////////////////////////////////////////
class DlgDef : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_Overview;
		wxButton* Calculate;
		wxButton* m_Function;
		wxCheckBox* m_Help;
		wxButton* m_HelpButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void key_shortcut( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnCalculate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFunction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggle( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKey( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnItem( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		wxTextCtrl* m_result;
		wxListCtrl* m_listCtrl;
		
		DlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Calculator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 665,91 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER ); 
		~DlgDef();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CfgDlgDef
///////////////////////////////////////////////////////////////////////////////
class CfgDlgDef : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_Font_Size_txt;
		wxStaticText* m_staticText17;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
	
	public:
		wxCheckBox* m_showhelpB;
		wxCheckBox* m_showhistoryB;
		wxCheckBox* m_CalculateB;
		wxCheckBox* m_showfunction;
		wxSpinCtrl* m_MaxResults;
		wxSlider* m_sOpacity;
		wxCheckBox* m_showhistory;
		wxCheckBox* m_capturehidden;
		wxCheckBox* m_logresults;
		
		CfgDlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Calculator preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~CfgDlgDef();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class HlpDlgDef
///////////////////////////////////////////////////////////////////////////////
class HlpDlgDef : public wxDialog 
{
	private:
	
	protected:
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxPanel* HelpPanel;
		wxTextCtrl* m_textCtrl3;
		
		HlpDlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER ); 
		~HlpDlgDef();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyDialog5
///////////////////////////////////////////////////////////////////////////////
class MyDialog5 : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel14;
		wxNotebook* m_wxNotebook234;
		wxPanel* m_panel11;
		wxStaticText* m_staticText3231;
		wxTextCtrl* m_Lat1;
		wxStaticText* m_staticText3331;
		wxStaticText* m_staticText32311;
		wxTextCtrl* m_Lon1;
		wxStaticText* m_staticText33311;
		wxButton* m_button4;
		wxPanel* m_panel151;
		wxStaticText* m_staticText323131;
		wxTextCtrl* m_Lat1_d1;
		wxStaticText* m_staticText333131;
		wxTextCtrl* m_Lat1_m1;
		wxStaticText* m_staticText11431;
		wxChoice* m_Lat1_NS1;
		wxStaticText* m_staticText3231121;
		wxTextCtrl* m_Lon1_d1;
		wxStaticText* m_staticText3331121;
		wxTextCtrl* m_Lon1_m1;
		wxStaticText* m_staticText114121;
		wxChoice* m_Lon1_EW1;
		wxButton* m_button811;
		wxPanel* m_panel15;
		wxStaticText* m_staticText32313;
		wxTextCtrl* m_Lat1_d;
		wxStaticText* m_staticText33313;
		wxTextCtrl* m_Lat1_m;
		wxStaticText* m_staticText1143;
		wxTextCtrl* m_Lat1_s;
		wxStaticText* m_staticText1153;
		wxChoice* m_Lat1_NS;
		wxStaticText* m_staticText323112;
		wxTextCtrl* m_Lon1_d;
		wxStaticText* m_staticText333112;
		wxTextCtrl* m_Lon1_m;
		wxStaticText* m_staticText11412;
		wxTextCtrl* m_Lon1_s;
		wxStaticText* m_staticText11512;
		wxChoice* m_Lon1_EW;
		wxButton* m_button81;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNoteBookFit( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnConverttoDegree( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MyDialog5( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~MyDialog5();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FunDlgDef
///////////////////////////////////////////////////////////////////////////////
class FunDlgDef : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel152;
		wxChoice* m_Function_Categories;
		wxPanel* m_panel101;
		wxStaticText* m_Function;
		wxPanel* m_panel100;
		wxTextCtrl* m_Function_Result;
		wxChoice* m_Output_Parameter_UnitC;
		wxButton* m_button7;
		wxButton* m_button10;
		wxCheckBox* m_checkBox8;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCategorySelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExtraCalculate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggle( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxChoice* m_Function_Dropdown;
		wxStaticText* m_Description;
		wxStaticText* m_Output_Parameter;
		wxPanel* m_panel;
		wxBoxSizer* bSizer;
		wxTextCtrl* Value;
		wxStaticText* Parameter;
		wxChoice* Units;
		wxPanel* m_panel1;
		wxBoxSizer* bSizer1;
		wxTextCtrl* Value1;
		wxStaticText* Parameter1;
		wxChoice* Units1;
		wxPanel* m_panel2;
		wxBoxSizer* bSizer2;
		wxTextCtrl* Value2;
		wxStaticText* Parameter2;
		wxChoice* Units2;
		wxPanel* m_panel3;
		wxBoxSizer* bSizer3;
		wxTextCtrl* Value3;
		wxStaticText* Parameter3;
		wxChoice* Units3;
		wxPanel* m_panel4;
		wxBoxSizer* bSizer4;
		wxTextCtrl* Value4;
		wxStaticText* Parameter4;
		wxChoice* Units4;
		wxPanel* m_panel5;
		wxBoxSizer* bSizer5;
		wxTextCtrl* Value5;
		wxStaticText* Parameter5;
		wxChoice* Units5;
		wxPanel* m_panel6;
		wxBoxSizer* bSizer6;
		wxTextCtrl* Value6;
		wxStaticText* Parameter6;
		wxChoice* Units6;
		wxPanel* m_panel7;
		wxBoxSizer* bSizer8;
		wxTextCtrl* Value7;
		wxStaticText* Parameter7;
		wxChoice* Units7;
		wxPanel* m_panel8;
		wxBoxSizer* bSizer9;
		wxTextCtrl* Value8;
		wxStaticText* Parameter8;
		wxChoice* Units8;
		wxPanel* m_panel9;
		wxBoxSizer* bSizer10;
		wxTextCtrl* Value9;
		wxStaticText* Parameter9;
		wxChoice* Units9;
		
		FunDlgDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Calculator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER ); 
		~FunDlgDef();
	
};

#endif //__CALCULATORGUI_H__
