// /////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
// /////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/radiobox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

// #include "wxWTranslateCatalog.h"

///////////////////////////////////////////////////////////////////////////

#define ID_CB_ALL 1000

// /////////////////////////////////////////////////////////////////////////////
// / Class SquiddioPrefsDialogBase
// /////////////////////////////////////////////////////////////////////////////
class SquiddioPrefsDialogBase : public wxDialog
{
	private:

	protected:
        wxNotebook* m_notebook1;
        wxPanel* m_panel1;
        wxPanel* rendering;
        wxStaticLine* m_staticline2;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticTextTextColour;
		wxStaticText* m_staticTextBackgroundColour;
		wxStaticText* m_staticTextBackgroundTransparency;
		wxStaticText* m_staticTextFont;
        wxPanel* m_panel3;
        wxStaticText* m_staticText11;
        wxStaticText* m_staticText12;
        wxStaticText* m_staticText14;
        wxPanel* logSharing;
        wxStaticText* m_staticText6;
        wxStaticText* m_staticText4;
        wxPanel* m_panel5;
        wxStaticText* m_staticText7;
        wxStaticText* m_staticText8;
		wxStdDialogButtonSizer* m_sdbButtonSizer;
		wxButton* m_sdbButtonSizerOK;
		wxButton* m_sdbButtonSizerCancel;
		wxButton* m_sdbButtonSizerHelp;

		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckBoxAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClickFonts( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sdbButtonSizerOnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void LaunchHelpPage( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sdbButtonSizerOnOKButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxFlexGridSizer* m_fgMainSizer;
		wxFlexGridSizer* m_fgSubSizer;
		wxCheckBox* m_checkBoxAll;
		wxCheckBox* m_checkBoxMarinas;
		wxCheckBox* m_checkBoxAnchorages;
		wxCheckBox* m_checkBoxDocks;
        wxCheckBox* m_checkBoxYachtClubs;
        wxCheckBox* m_checkBoxMoorings;
		wxCheckBox* m_checkBoxFuelStations;
        wxCheckBox* m_checkBoxBoatYards;
        wxCheckBox* m_checkBoxRamps;
		wxCheckBox* m_checkBoxOthers;
        wxCheckBox* m_checkBoxNDBC;
        wxCheckBox* m_checkBoxShipRep;
        wxCheckBox* m_checkBoxAIS;
		wxRadioBox* m_radioBoxOCPNorOD;
		wxFlexGridSizer* m_fgSizerODSettings;
		wxCheckBox* m_checkBoxShowName;
		wxChoice* m_choicePosition;
		wxColourPickerCtrl* m_colourPickerText;
		wxColourPickerCtrl* m_colourPickerBackgroundColour;
		wxSlider* m_sliderBackgroundTransparency;
		wxStaticText* m_staticTextFontFaceExample;
		wxButton* m_buttonTextFont;
		wxRadioBox* m_radioBoxShowDisplayText;
        wxTextCtrl* m_textZoomLevels;
        wxDirPickerCtrl* m_dirPickerDownload;
        wxCheckBox* m_checkBoxVPMap;
        wxCheckBox* m_checkBoxDelGpxs;
        wxChoice* m_choiceHowOften;
        wxChoice* m_choiceReceive;
        wxCheckBox* m_checkBoxSendXml;
        wxTextCtrl* m_textSquiddioID;
        wxTextCtrl* m_textApiKey;
        wxStaticText* m_version;

        SquiddioPrefsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("sQuiddio Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 476,487 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SquiddioPrefsDialogBase();

};

