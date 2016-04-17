///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 12 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SWEEPPLOTUI_H__
#define __SWEEPPLOTUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/fontpicker.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/statbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SweepPlotDialogBase
///////////////////////////////////////////////////////////////////////////////
class SweepPlotDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxScrolledWindow* m_swPlots;
		wxMenu* m_menu1;
		wxMenuItem* m_mt1;
		wxMenuItem* m_mt2;
		wxMenuItem* m_mt3;
		wxMenuItem* m_mt4;
		wxMenuItem* m_mt5;
		wxMenuItem* m_mt6;
		wxMenuItem* m_mt7;
		wxMenuItem* m_mt8;
		wxMenuItem* m_mt9;
		wxMenuItem* m_mt10;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void Relay( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnDoubleClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnConfiguration( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxScrolledWindow* m_scrollWindow;
		
		SweepPlotDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Sweep Plot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER|wxSUNKEN_BORDER|wxWANTS_CHARS );
		~SweepPlotDialogBase();
		
		void m_swPlotsOnContextMenu( wxMouseEvent &event )
		{
			m_swPlots->PopupMenu( m_menu1, event.GetPosition() );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class PreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxListbook* m_listbook1;
		wxButton* m_button5;
		wxPanel* m_panel2;
		wxPanel* m_panel3;
		wxStaticText* m_staticText15;
		wxPanel* m_panel4;
		wxStaticText* m_staticText16;
		wxPanel* m_panel5;
		wxStaticText* m_staticText161;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText12;
		wxSpinCtrl* m_sPlotMinHeight;
		wxStaticText* m_staticText121;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText122;
		wxStaticText* m_staticText1411;
		wxStaticText* m_staticText1511;
		wxStaticText* m_staticText721;
		wxStaticText* m_staticText2311;
		wxButton* m_bAbout;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPDS( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPlotChange( wxFontPickerEvent& event ) { event.Skip(); }
		virtual void OnPlotChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnPlotChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxPanel* m_panel1;
		wxCheckBox* m_cbSOG;
		wxCheckBox* m_cbPDS10;
		wxCheckBox* m_cbPDS60;
		wxCheckBox* m_cbCOG;
		wxCheckBox* m_cbPDC10;
		wxCheckBox* m_cbPDC60;
		wxCheckBox* m_cbHDG;
		wxCheckBox* m_cbCourseFFTWPlot;
		wxFontPickerCtrl* m_fpPlotFont;
		wxChoice* m_cColors;
		wxSpinCtrl* m_sPlotTransparency;
		wxChoice* m_cPlotStyle;
		wxCheckBox* m_cbShowTitleBar;
		wxCheckBox* m_cbCoursePrediction;
		wxCheckBox* m_cbCoursePredictionBlended;
		wxSpinCtrl* m_sCoursePredictionLength;
		wxSpinCtrl* m_sCoursePredictionSeconds;
		
		PreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SweepPlot Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE ); 
		~PreferencesDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialogBase
///////////////////////////////////////////////////////////////////////////////
class AboutDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText110;
		wxButton* m_button2;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAboutAuthor( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AboutDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About SweepPlot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~AboutDialogBase();
	
};

#endif //__SWEEPPLOTUI_H__
