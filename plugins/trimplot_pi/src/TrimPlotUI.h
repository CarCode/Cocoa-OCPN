///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TRIMPLOTUI_H__
#define __TRIMPLOTUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TrimPlotDialogBase
///////////////////////////////////////////////////////////////////////////////
class TrimPlotDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxScrolledWindow* m_swSpeed;
		wxScrolledWindow* m_swCourse;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText18;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnDoubleClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		
	
	public:
		wxStaticText* m_stSpeed;
		wxStaticText* m_stCourse;
		wxStaticText* m_stPositionSpeed;
		wxStaticText* m_stSpeedPercentage;
		
		TrimPlotDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Trim Plot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL ); 
		~TrimPlotDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class PreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText23;
		wxStaticText* m_staticText141;
		wxStaticText* m_staticText151;
		wxStaticText* m_staticText72;
		wxStaticText* m_staticText231;
		wxStaticText* m_staticText1411;
		wxStaticText* m_staticText1511;
		wxStaticText* m_staticText721;
		wxStaticText* m_staticText2311;
		wxButton* m_bAbout;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPlotChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxCheckBox* m_cbSpeed;
		wxTextCtrl* m_tSpeedScale;
		wxSpinCtrl* m_sSpeedSeconds;
		wxCheckBox* m_cbCourse;
		wxTextCtrl* m_tCourseScale;
		wxSpinCtrl* m_sCourseSeconds;
		wxCheckBox* m_cbCoursePrediction;
		wxSpinCtrl* m_sCoursePredictionLength;
		wxSpinCtrl* m_sCoursePredictionSeconds;
		
		PreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("TrimPlot Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE ); 
		~PreferencesDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog
///////////////////////////////////////////////////////////////////////////////
class AboutDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText110;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
	
	public:
		
		AboutDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About TrimPlot"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~AboutDialog();
	
};

#endif //__TRIMPLOTUI_H__
