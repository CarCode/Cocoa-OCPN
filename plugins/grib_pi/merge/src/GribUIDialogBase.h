///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GRIBUIDIALOGBASE_H__
#define __GRIBUIDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/radiobut.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PLAYSTOP 1000
#define ID_TIMELINE 1001
#define ID_CB_WIND_SPEED 1002
#define ID_CB_SIG_WAVE_HEIGHT 1003
#define ID_CB_CURRENT_VELOCITY 1004
#define ID_CB_PRESSURE 1005
#define ID_CB_SEA_TEMPERATURE 1006

///////////////////////////////////////////////////////////////////////////////
/// Class GRIBUIDialogBase
///////////////////////////////////////////////////////////////////////////////
class GRIBUIDialogBase : public wxDialog
{
private:

protected:
wxBitmapButton* m_bpPrev;
wxComboBox* m_cRecordForecast;
wxBitmapButton* m_bpNext;
wxBitmapButton* m_bpNow;
wxBitmapButton* m_bpOpenFile;
wxBitmapButton* m_bpSettings;
wxBitmapButton* m_bpRequest;
wxFlexGridSizer* m_fgTrackingControls;
wxTextCtrl* m_tcWindSpeed;
wxTextCtrl* m_tcWindDirection;
wxCheckBox* m_cbWindScat;
wxTextCtrl* m_tcWindScatSpeed;
wxTextCtrl* m_tcWindScatDirection;
wxTextCtrl* m_tcWaveHeight;
wxTextCtrl* m_tcWaveDirection;
wxTextCtrl* m_tcCurrentVelocity;
wxTextCtrl* m_tcCurrentDirection;
wxTextCtrl* m_tcWindGust;
wxTextCtrl* m_tcPressure;
wxTextCtrl* m_tcPrecipitation;
wxTextCtrl* m_tcCloud;
wxTextCtrl* m_tcAirTemperature;
wxTextCtrl* m_tcSeaTemperature;

// Virtual event handlers, overide them in your derived class
virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
virtual void OnRecordForecast( wxCommandEvent& event ) { event.Skip(); }
virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
virtual void OnNow( wxCommandEvent& event ) { event.Skip(); }
virtual void OnOpenFile( wxCommandEvent& event ) { event.Skip(); }
virtual void OnSettings( wxCommandEvent& event ) { event.Skip(); }
virtual void OnRequest( wxCommandEvent& event ) { event.Skip(); }
virtual void OnPlayStop( wxCommandEvent& event ) { event.Skip(); }
virtual void OnTimeline( wxScrollEvent& event ) { event.Skip(); }
virtual void OnCBAny( wxCommandEvent& event ) { event.Skip(); }


public:
wxToggleButton* m_tbPlayStop;
wxSlider* m_sTimeline;
wxCheckBox* m_cbWind;
wxCheckBox* m_cbWave;
wxCheckBox* m_cbCurrent;
wxCheckBox* m_cbWindGust;
wxCheckBox* m_cbPressure;
wxCheckBox* m_cbPrecipitation;
wxCheckBox* m_cbCloud;
wxCheckBox* m_cbAirTemperature;
wxCheckBox* m_cbSeaTemperature;

GRIBUIDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("GRIB Display Control"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDIALOG_NO_PARENT|wxSYSTEM_MENU );
~GRIBUIDialogBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class GribSettingsDialogBase
///////////////////////////////////////////////////////////////////////////////
class GribSettingsDialogBase : public wxDialog
{
private:

protected:
wxStaticText* m_staticText5;
wxSpinCtrl* m_sSlicesPerUpdate;
wxStaticText* m_staticText9;
wxSpinCtrl* m_sUpdatesPerSecond;
wxStaticText* m_staticText4;
wxChoice* m_cDataType;
wxStaticText* m_staticText12;
wxChoice* m_cDataUnits;
wxCheckBox* m_cbBarbedArrows;
wxStaticText* m_staticText10;
wxSpinCtrl* m_sBarbedRange;
wxCheckBox* m_cbIsoBars;
wxStaticText* m_staticText6;
wxSpinCtrl* m_sIsoBarSpacing;
wxCheckBox* m_cbDirectionArrows;
wxStaticText* m_staticText51;
wxSpinCtrl* m_sDirectionArrowSize;
wxCheckBox* m_cbOverlayMap;
wxStaticText* m_staticText7;
wxChoice* m_cOverlayColors;
wxCheckBox* m_cbNumbers;
wxStaticText* m_staticText8;
wxSpinCtrl* m_sNumbersSpacing;
wxStdDialogButtonSizer* m_sdbSizer1;
wxButton* m_sdbSizer1OK;
wxButton* m_sdbSizer1Apply;
wxButton* m_sdbSizer1Cancel;

// Virtual event handlers, overide them in your derived class
virtual void OnDataTypeChoice( wxCommandEvent& event ) { event.Skip(); }
virtual void OnApply( wxCommandEvent& event ) { event.Skip(); }


public:
wxCheckBox* m_cInterpolate;
wxCheckBox* m_cLoopMode;
wxSpinCtrl* m_sHourDivider;

GribSettingsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Grib Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
~GribSettingsDialogBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class GribPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class GribPreferencesDialogBase : public wxDialog
{
private:

protected:
wxStdDialogButtonSizer* m_sdbSizer2;
wxButton* m_sdbSizer2OK;
wxButton* m_sdbSizer2Cancel;

public:
wxCheckBox* m_cbUseHiDef;
wxCheckBox* m_cbUseGradualColors;
wxCheckBox* m_cbCopyFirstCumulativeRecord;
wxCheckBox* m_cbCopyMissingWaveRecord;
wxRadioButton* m_rbLocalTime;
wxRadioButton* m_rbUTC;

GribPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Grib Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
~GribPreferencesDialogBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class GribPofileDisplayBase
///////////////////////////////////////////////////////////////////////////////
class GribPofileDisplayBase : public wxDialog
{
private:

protected:
wxStaticText* m_staticText10;
wxStaticText* m_stProfile;
wxButton* m_bSend;
wxStdDialogButtonSizer* m_sdbSizer3;
wxButton* m_sdbSizer3Cancel;

// Virtual event handlers, overide them in your derived class
virtual void OnSend( wxCommandEvent& event ) { event.Skip(); }


public:

GribPofileDisplayBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Request Pofile"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
~GribPofileDisplayBase();

};

#endif //__GRIBUIDIALOGBASE_H__
