/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Navico br24 Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "br24ControlsDialog.h"
#include "RadarPanel.h"

PLUGIN_BEGIN_NAMESPACE

enum {  // process ID's
  ID_TEXTCTRL1 = 10000,
  ID_BACK,
  ID_PLUS_TEN,
  ID_PLUS,
  ID_VALUE,
  ID_MINUS,
  ID_MINUS_TEN,
  ID_AUTO,
  ID_MULTISWEEP,
  ID_TRAILS_MOTION,

  ID_TRANSPARENCY,
  ID_INTERFERENCE_REJECTION,
  ID_TARGET_BOOST,
  ID_TARGET_EXPANSION,
  ID_NOISE_REJECTION,
  ID_TARGET_SEPARATION,
  ID_REFRESHRATE,
  ID_SCAN_SPEED,
  ID_INSTALLATION,
  ID_PREFERENCES,
  ID_TIMED_IDLE,

  ID_BEARING_ALIGNMENT,
  ID_ANTENNA_HEIGHT,
  ID_LOCAL_INTERFERENCE_REJECTION,
  ID_SIDE_LOBE_SUPPRESSION,
  ID_MAIN_BANG_SIZE,

  ID_RANGE,
  ID_GAIN,
  ID_SEA,
  ID_RAIN,

  ID_CLEAR_CURSOR,

  ID_TARGET_TRAILS,
  ID_CLEAR_TRAILS,
  ID_ORIENTATION,

  ID_RADAR_STATE,
  ID_SHOW_RADAR,
  ID_RADAR_OVERLAY,
  ID_ADJUST,
  ID_ADVANCED,
  ID_VIEW,
  ID_BEARING,
  ID_ZONE1,
  ID_ZONE2,

  ID_CONFIRM_BOGEY,

  ID_MESSAGE,
  ID_BPOS,
  ID_HEADING,
  ID_RADAR,
  ID_DATA,

  ID_BEARING_SET,  // next one should be BEARING_LINES higher
  ID_NEXT = ID_BEARING_SET + BEARING_LINES,

};

//---------------------------------------------------------------------------------------
//          Radar Control Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS(br24ControlsDialog, wxDialog)

BEGIN_EVENT_TABLE(br24ControlsDialog, wxDialog)

EVT_CLOSE(br24ControlsDialog::OnClose)
EVT_BUTTON(ID_BACK, br24ControlsDialog::OnBackClick)
EVT_BUTTON(ID_PLUS_TEN, br24ControlsDialog::OnPlusTenClick)
EVT_BUTTON(ID_PLUS, br24ControlsDialog::OnPlusClick)
EVT_BUTTON(ID_MINUS, br24ControlsDialog::OnMinusClick)
EVT_BUTTON(ID_MINUS_TEN, br24ControlsDialog::OnMinusTenClick)
EVT_BUTTON(ID_AUTO, br24ControlsDialog::OnAutoClick)
EVT_BUTTON(ID_MULTISWEEP, br24ControlsDialog::OnMultiSweepClick)
EVT_BUTTON(ID_TRAILS_MOTION, br24ControlsDialog::OnTrailsMotionClick)

EVT_BUTTON(ID_TRANSPARENCY, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_INTERFERENCE_REJECTION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TARGET_BOOST, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TARGET_EXPANSION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_NOISE_REJECTION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TARGET_SEPARATION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_REFRESHRATE, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_SCAN_SPEED, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_INSTALLATION, br24ControlsDialog::OnInstallationButtonClick)
EVT_BUTTON(ID_PREFERENCES, br24ControlsDialog::OnPreferencesButtonClick)
EVT_BUTTON(ID_TIMED_IDLE, br24ControlsDialog::OnRadarControlButtonClick)

EVT_BUTTON(ID_BEARING_ALIGNMENT, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_ANTENNA_HEIGHT, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_LOCAL_INTERFERENCE_REJECTION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_SIDE_LOBE_SUPPRESSION, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_MAIN_BANG_SIZE, br24ControlsDialog::OnRadarControlButtonClick)

EVT_BUTTON(ID_RADAR_STATE, br24ControlsDialog::OnRadarStateButtonClick)
EVT_BUTTON(ID_SHOW_RADAR, br24ControlsDialog::OnRadarShowButtonClick)
EVT_BUTTON(ID_RADAR_OVERLAY, br24ControlsDialog::OnRadarOverlayButtonClick)
EVT_BUTTON(ID_RANGE, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_GAIN, br24ControlsDialog::OnRadarGainButtonClick)
EVT_BUTTON(ID_SEA, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_RAIN, br24ControlsDialog::OnRadarControlButtonClick)

EVT_BUTTON(ID_TARGET_TRAILS, br24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_CLEAR_TRAILS, br24ControlsDialog::OnClearTrailsButtonClick)
EVT_BUTTON(ID_ORIENTATION, br24ControlsDialog::OnOrientationButtonClick)

EVT_BUTTON(ID_ADJUST, br24ControlsDialog::OnAdjustButtonClick)
EVT_BUTTON(ID_ADVANCED, br24ControlsDialog::OnAdvancedButtonClick)
EVT_BUTTON(ID_VIEW, br24ControlsDialog::OnViewButtonClick)

EVT_BUTTON(ID_BEARING, br24ControlsDialog::OnBearingButtonClick)
EVT_BUTTON(ID_ZONE1, br24ControlsDialog::OnZone1ButtonClick)
EVT_BUTTON(ID_ZONE2, br24ControlsDialog::OnZone2ButtonClick)

EVT_BUTTON(ID_MESSAGE, br24ControlsDialog::OnMessageButtonClick)

EVT_BUTTON(ID_BEARING_SET, br24ControlsDialog::OnBearingSetButtonClick)
EVT_BUTTON(ID_CLEAR_CURSOR, br24ControlsDialog::OnClearCursorButtonClick)

EVT_MOVE(br24ControlsDialog::OnMove)
EVT_CLOSE(br24ControlsDialog::OnClose)

END_EVENT_TABLE()

static wxSize g_buttonSize;
static wxSize g_smallButtonSize;

class br24RadarControlButton : public wxButton {
 public:
  br24RadarControlButton(){

  };

  br24RadarControlButton(br24ControlsDialog* parent, wxWindowID id, const wxString& label, ControlType ct, bool newHasAuto,
                         int newValue) {
    Create(parent, id, label + wxT("\n"), wxDefaultPosition, g_buttonSize, 0, wxDefaultValidator, label);

    m_parent = parent;
    m_pi = m_parent->m_pi;
    minValue = 0;
    maxValue = 100;
    value = 0;
    if (ct == CT_GAIN) {
      value = 50;
    }
    hasAuto = newHasAuto;
    isAuto = false;
    firstLine = label;
    names = 0;
    controlType = ct;
    if (hasAuto) {
      SetLocalAuto();
    } else {
      SetLocalValue(newValue);
    }

    this->SetFont(m_parent->m_pi->m_font);
  }

  virtual void AdjustValue(int adjustment);
  virtual void SetAuto();
  virtual void SetLocalValue(int newValue);
  virtual void SetLocalAuto();
  const wxString* names;

  wxString firstLine;

  br24ControlsDialog* m_parent;
  br24radar_pi* m_pi;

  int value;

  int minValue;
  int maxValue;
  bool hasAuto;
  bool isAuto;
  ControlType controlType;
};

class br24RadarRangeControlButton : public br24RadarControlButton {
 public:
  br24RadarRangeControlButton(br24ControlsDialog* parent, RadarInfo* ri, wxWindowID id, const wxString& label) {
    Create(parent, id, label + wxT("\n"), wxDefaultPosition, g_buttonSize, 0, wxDefaultValidator, label);

    m_parent = parent;
    m_pi = m_parent->m_pi;
    m_ri = ri;
    minValue = 0;
    maxValue = 0;
    value = -1;  // means: never set
    hasAuto = true;
    isAuto = false;
    firstLine = label;
    names = 0;
    controlType = CT_RANGE;

    this->SetFont(m_parent->m_pi->m_font);
  }

  virtual void AdjustValue(int adjustment);
  virtual void SetAuto();
  void SetRangeLabel();

 private:
  RadarInfo* m_ri;
};

wxString interference_rejection_names[4];
wxString target_separation_names[4];
wxString noise_rejection_names[3];
wxString target_boost_names[3];
wxString target_expansion_names[2];
wxString scan_speed_names[2];
wxString timed_idle_times[8];
wxString guard_zone_names[3];
wxString target_trail_names[TRAIL_ARRAY_SIZE];

void br24RadarControlButton::AdjustValue(int adjustment) {
  int newValue = value + adjustment;

  if (newValue < minValue) {
    newValue = minValue;
  } else if (newValue > maxValue) {
    newValue = maxValue;
  }
  if (newValue != value) {
    LOG_VERBOSE(wxT("%s Adjusting %s by %d from %d to %d"), m_parent->m_log_name.c_str(), GetName(), adjustment, value, newValue);
    if (m_pi->SetControlValue(m_parent->m_ri->m_radar, controlType, newValue)) {
      SetLocalValue(newValue);
    }
  }
}

void br24RadarControlButton::SetLocalValue(int newValue) {  // sets value in the button without sending new value to the radar
  if (newValue < minValue) {
    value = minValue;
  } else if (newValue > maxValue) {
    value = maxValue;
  } else {
    value = newValue;
  }
  isAuto = false;

  wxString label;

  if (names) {
    label.Printf(wxT("%s\n%s"), firstLine.c_str(), names[value].c_str());
  } else {
    label.Printf(wxT("%s\n%d"), firstLine.c_str(), value);
  }

  this->SetLabel(label);
}

void br24RadarControlButton::SetAuto() {
  SetLocalAuto();
  m_parent->m_ri->SetControlValue(controlType, -1);
}

void br24RadarControlButton::SetLocalAuto() {  // sets auto in the button without sending new value
                                               // to the radar
  wxString label;

  isAuto = true;
  label << firstLine << wxT("\n") << _("Auto");
  this->SetLabel(label);
}

void br24RadarRangeControlButton::SetRangeLabel() {
  wxString text = m_ri->GetRangeText();
  this->SetLabel(firstLine + wxT("\n") + text);
}

void br24RadarRangeControlButton::AdjustValue(int adjustment) {
  LOG_VERBOSE(wxT("%s Adjusting %s by %d"), m_parent->m_log_name.c_str(), GetName(), adjustment);
  isAuto = false;
  m_parent->m_ri->AdjustRange(adjustment);  // send new value to the radar
}

void br24RadarRangeControlButton::SetAuto() {
  isAuto = true;
  m_parent->m_ri->m_auto_range_mode = true;
}

br24ControlsDialog::br24ControlsDialog() { Init(); }

br24ControlsDialog::~br24ControlsDialog() {
  wxPoint pos = GetPosition();

  LOG_DIALOG(wxT("%s saved position %d,%d"), m_log_name.c_str(), pos.x, pos.y);
  m_pi->m_settings.control_pos[m_ri->m_radar] = pos;
}

void br24ControlsDialog::Init() {
  // Initialize all members that need initialization
  m_hide = false;
  m_hide_temporarily = true;

  m_from_control = 0;

  m_panel_position = wxDefaultPosition;
  m_manually_positioned = false;
}

bool br24ControlsDialog::Create(wxWindow* parent, br24radar_pi* ppi, RadarInfo* ri, wxWindowID id, const wxString& caption,
                                const wxPoint& pos) {
  m_parent = parent;
  m_pi = ppi;
  m_ri = ri;

  m_log_name = wxString::Format(wxT("BR24radar_pi: Radar %c ControlDialog:"), ri->m_radar + 'A');

#ifdef __WXMSW__
  long wstyle = wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN;
#endif
#ifdef __WXMAC__
  long wstyle = wxCLOSE_BOX | wxCAPTION;
  // long wstyle = wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP;

  wstyle |= wxSTAY_ON_TOP;  // Radar AUI windows, when float, are already FLOAT_ON_PARENT and we don't seem to be on top of those.
  wstyle |= wxFRAME_FLOAT_ON_PARENT;  // Float on our parent
  wstyle |= wxFRAME_TOOL_WINDOW;      // This causes window to hide when OpenCPN is not activated, but this doesn't wo
#endif
#ifdef __WXGTK__
  long wstyle = wxCLOSE_BOX | wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR;
#endif

  if (!wxDialog::Create(parent, id, caption, pos, wxDefaultSize, wstyle)) {
    return false;
  }

  CreateControls();

  return true;
}

void br24ControlsDialog::CreateControls() {
  static int BORDER = 0;
  wxString backButtonStr;
  backButtonStr << wxT("<<\n") << _("Back");

  // A top-level sizer
  m_top_sizer = new wxBoxSizer(wxVERTICAL);
  SetSizer(m_top_sizer);

  /*
   * Here be dragons...
   * Since I haven't been able to create buttons that adapt up, and at the same
   * time calculate the biggest button, and I want all buttons to be the same width I use a trick.
   * I generate a multiline StaticText containing all the (long) button labels and find out what the
   * width of that is, and then generate the buttons using that width.
   * I know, this is a hack, but this way it works relatively nicely even with translations.
   */

  wxBoxSizer* testBox = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(testBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  wxString label;
  label << _("Standby / Transmit") << wxT("\n");
  label << _("Noise rejection") << wxT("\n");
  label << _("Target expansion") << wxT("\n");
  label << _("Interference rejection") << wxT("\n");
  label << _("Target separation") << wxT("\n");
  label << _("Scan speed") << wxT("\n");
  label << _("Target boost") << wxT("\n");
  label << _("Installation") << wxT("\n");
  label << _("Bearing alignment") << wxT("\n");
  label << _("Antenna height") << wxT("\n");
  label << _("Local interference rej.") << wxT("\n");
  label << _("Side lobe suppression") << wxT("\n");
  label << _("Main bang size") << wxT("\n");
  label << _("Guard zones") << wxT("\n");
  label << _("Zone type") << wxT("\n");
  label << _("Guard zones") << wxT("\n");
  label << _("Inner range") << wxT("\n");
  label << _("Outer range") << wxT("\n");
  label << _("Start bearing") << wxT("\n");
  label << _("End bearing") << wxT("\n");
  label << _("Multi sweep filter") << wxT("\n");
  label << _("Range") << wxT("\n");
  label << _("Gain") << wxT("\n");
  label << _("Sea clutter") << wxT("\n");
  label << _("Rain clutter") << wxT("\n");
  label << _("Clear cursor") << wxT("\n");
  label << _("Place EBL/VRM") << wxT("\n");
  label << _("Target trails") << wxT("\n");
  label << _("True/Relative trails") << wxT("\n");
  label << _("Clear trails") << wxT("\n");
  label << _("Orientation") << wxT("\n");
  label << _("Refresh rate") << wxT("\n");
  label << _("Transparency") << wxT("\n");
  label << _("Overlay") << wxT("\n");
  label << _("Adjust") << wxT("\n");
  label << _("Advanced") << wxT("\n");
  label << _("View") << wxT("\n");
  label << _("EBL/VRM") << wxT("\n");
  label << _("Timed Transmit") << wxT("\n");
  label << _("Info") << wxT("\n");

  wxStaticText* testMessage =
      new wxStaticText(this, ID_BPOS, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
  testMessage->SetFont(m_pi->m_font);
  testBox->Add(testMessage, 0, wxALL, 2);

  wxStaticText* testButtonText =
      new wxStaticText(this, ID_BPOS, wxT("Button"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
  testButtonText->SetFont(m_pi->m_font);
  testBox->Add(testButtonText, 0, wxALL, 2);

  wxStaticText* testButton2Text =
      new wxStaticText(this, ID_BPOS, wxT("Button\ndata"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
  testButton2Text->SetFont(m_pi->m_font);
  testBox->Add(testButton2Text, 0, wxALL, 2);

  m_top_sizer->Fit(this);
  m_top_sizer->Layout();
  int width = m_top_sizer->GetSize().GetWidth() + 20;

  wxSize bestSize = GetBestSize();
  if (width < bestSize.GetWidth()) {
    width = bestSize.GetWidth();
  }
  if (width < 100) {
    width = 100;
  }
  if (width > 500) {
    width = 500;
  }

#define BUTTON_BORDER 4
#ifdef __WXMAC__
#define BUTTON_HEIGTH_FUDGE 1 + BUTTON_BORDER
#else
#define BUTTON_HEIGTH_FUDGE 1 + 2 * BUTTON_BORDER
#endif

  g_smallButtonSize = wxSize(width, testButtonText->GetBestSize().y + BUTTON_BORDER);
  g_buttonSize = wxSize(width, testButton2Text->GetBestSize().y * BUTTON_HEIGTH_FUDGE);
  //  g_smallButtonSize = g_buttonSize;  nicer ??

  LOG_DIALOG(wxT("%s Dynamic button width = %d height = %d, %d"), m_log_name.c_str(), g_buttonSize.x, g_buttonSize.y,
             g_smallButtonSize.y);

  m_top_sizer->Hide(testBox);
  m_top_sizer->Remove(testBox);
  // delete testBox; -- this core dumps!
  // Determined desired button width

  //**************** EDIT BOX ******************//
  // A box sizer to contain RANGE button
  m_edit_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_edit_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // A box sizer to contain RANGE button
  m_edit_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_edit_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The <<Back button
  wxButton* back_button = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(back_button, 0, wxALL, BORDER);
  back_button->SetFont(m_pi->m_font);

  // The +10 button
  m_plus_ten_button = new wxButton(this, ID_PLUS_TEN, _("+10"), wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(m_plus_ten_button, 0, wxALL, BORDER);
  m_plus_ten_button->SetFont(m_pi->m_font);

  // The + button
  m_plus_button = new wxButton(this, ID_PLUS, _("+"), wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(m_plus_button, 0, wxALL, BORDER);
  m_plus_button->SetFont(m_pi->m_font);

  // The VALUE button
  wxSize valueSize = wxSize(g_buttonSize.x, g_buttonSize.y + 20);
  m_value_text = new wxStaticText(this, ID_VALUE, _("Value"), wxDefaultPosition, valueSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
  m_edit_sizer->Add(m_value_text, 0, wxALL, BORDER);
  m_value_text->SetFont(m_pi->m_fat_font);
  m_value_text->SetBackgroundColour(*wxLIGHT_GREY);

  // The - button
  m_minus_button = new wxButton(this, ID_MINUS, _("-"), wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(m_minus_button, 0, wxALL, BORDER);
  m_minus_button->SetFont(m_pi->m_font);

  // The -10 button
  m_minus_ten_button = new wxButton(this, ID_MINUS_TEN, _("-10"), wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(m_minus_ten_button, 0, wxALL, BORDER);
  m_minus_ten_button->SetFont(m_pi->m_font);

  // The Auto button
  m_auto_button = new wxButton(this, ID_AUTO, _("Auto"), wxDefaultPosition, g_buttonSize, 0);
  m_edit_sizer->Add(m_auto_button, 0, wxALL, BORDER);
  m_auto_button->SetFont(m_pi->m_font);

  m_top_sizer->Hide(m_edit_sizer);

  //**************** ADVANCED BOX ******************//
  // These are the controls that the users sees when the Advanced button is selected

  m_advanced_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_advanced_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Back button
  wxButton* bAdvancedBack = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_advanced_sizer->Add(bAdvancedBack, 0, wxALL, BORDER);
  bAdvancedBack->SetFont(m_pi->m_font);

  // The NOISE REJECTION button
  noise_rejection_names[0] = _("Off");
  noise_rejection_names[1] = _("Low");
  noise_rejection_names[2] = _("High");

  m_noise_rejection_button = new br24RadarControlButton(this, ID_NOISE_REJECTION, _("Noise rejection"), CT_NOISE_REJECTION, false,
                                                        m_ri->m_noise_rejection.button);
  m_advanced_sizer->Add(m_noise_rejection_button, 0, wxALL, BORDER);
  m_noise_rejection_button->minValue = 0;
  m_noise_rejection_button->maxValue = ARRAY_SIZE(noise_rejection_names) - 1;
  m_noise_rejection_button->names = noise_rejection_names;
  m_noise_rejection_button->SetLocalValue(m_ri->m_noise_rejection.button);  // redraw after adding names

  m_advanced_4G_sizer = new wxBoxSizer(wxVERTICAL);
  m_advanced_sizer->Add(m_advanced_4G_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  // The TARGET EXPANSION button
  target_expansion_names[0] = _("Off");
  target_expansion_names[1] = _("On");
  m_target_expansion_button = new br24RadarControlButton(this, ID_TARGET_EXPANSION, _("Target expansion"), CT_TARGET_EXPANSION,
                                                         false, m_ri->m_target_expansion.button);
  m_advanced_sizer->Add(m_target_expansion_button, 0, wxALL, BORDER);
  m_target_expansion_button->minValue = 0;
  m_target_expansion_button->maxValue = ARRAY_SIZE(target_expansion_names) - 1;
  m_target_expansion_button->names = target_expansion_names;
  m_target_expansion_button->SetLocalValue(m_ri->m_target_expansion.button);  // redraw after adding names

  // The REJECTION button

  interference_rejection_names[0] = _("Off");
  interference_rejection_names[1] = _("Low");
  interference_rejection_names[2] = _("Medium");
  interference_rejection_names[3] = _("High");

  m_interference_rejection_button =
      new br24RadarControlButton(this, ID_INTERFERENCE_REJECTION, _("Interference rejection"), CT_INTERFERENCE_REJECTION, false,
                                 m_ri->m_interference_rejection.button);
  m_advanced_sizer->Add(m_interference_rejection_button, 0, wxALL, BORDER);
  m_interference_rejection_button->minValue = 0;
  m_interference_rejection_button->maxValue = ARRAY_SIZE(interference_rejection_names) - 1;
  m_interference_rejection_button->names = interference_rejection_names;
  m_interference_rejection_button->SetLocalValue(m_ri->m_interference_rejection.button);  // redraw after adding names

  // The TARGET SEPARATION button

  target_separation_names[0] = _("Off");
  target_separation_names[1] = _("Low");
  target_separation_names[2] = _("Medium");
  target_separation_names[3] = _("High");

  m_target_separation_button = new br24RadarControlButton(this, ID_TARGET_SEPARATION, _("Target separation"), CT_TARGET_SEPARATION,
                                                          false, m_ri->m_target_separation.button);
  m_advanced_4G_sizer->Add(m_target_separation_button, 0, wxALL, BORDER);
  m_target_separation_button->minValue = 0;
  m_target_separation_button->maxValue = ARRAY_SIZE(target_separation_names) - 1;
  m_target_separation_button->names = target_separation_names;
  m_target_separation_button->SetLocalValue(m_ri->m_target_separation.button);  // redraw after adding names

  // The SCAN SPEED button
  scan_speed_names[0] = _("Normal");
  scan_speed_names[1] = _("Fast");
  m_scan_speed_button =
      new br24RadarControlButton(this, ID_SCAN_SPEED, _("Scan speed"), CT_SCAN_SPEED, false, m_ri->m_scan_speed.button);
  m_advanced_sizer->Add(m_scan_speed_button, 0, wxALL, BORDER);
  m_scan_speed_button->minValue = 0;
  m_scan_speed_button->maxValue = ARRAY_SIZE(scan_speed_names) - 1;
  m_scan_speed_button->names = scan_speed_names;
  m_scan_speed_button->SetLocalValue(m_ri->m_scan_speed.button);  // redraw after adding names

  // The TARGET BOOST button
  target_boost_names[0] = _("Off");
  target_boost_names[1] = _("Low");
  target_boost_names[2] = _("High");
  m_target_boost_button =
      new br24RadarControlButton(this, ID_TARGET_BOOST, _("Target boost"), CT_TARGET_BOOST, false, m_ri->m_target_boost.button);
  m_advanced_sizer->Add(m_target_boost_button, 0, wxALL, BORDER);
  m_target_boost_button->minValue = 0;
  m_target_boost_button->maxValue = ARRAY_SIZE(target_boost_names) - 1;
  m_target_boost_button->names = target_boost_names;
  m_target_boost_button->SetLocalValue(m_ri->m_target_boost.button);  // redraw after adding names

  // The INSTALLATION button
  wxButton* bInstallation = new wxButton(this, ID_INSTALLATION, _("Installation"), wxDefaultPosition, g_smallButtonSize, 0);
  m_advanced_sizer->Add(bInstallation, 0, wxALL, BORDER);
  bInstallation->SetFont(m_pi->m_font);

  // The PREFERENCES button
  wxButton* bPreferences = new wxButton(this, ID_PREFERENCES, _("Preferences"), wxDefaultPosition, g_smallButtonSize, 0);
  m_advanced_sizer->Add(bPreferences, 0, wxALL, BORDER);
  bPreferences->SetFont(m_pi->m_font);

  m_top_sizer->Hide(m_advanced_sizer);

  //**************** Installation BOX ******************//
  // These are the controls that the users sees when the Installation button is selected

  m_installation_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_installation_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Back button
  wxButton* bInstallationBack = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_installation_sizer->Add(bInstallationBack, 0, wxALL, BORDER);
  bInstallationBack->SetFont(m_pi->m_font);

  // The BEARING ALIGNMENT button
  m_bearing_alignment_button = new br24RadarControlButton(this, ID_BEARING_ALIGNMENT, _("Bearing alignment"), CT_BEARING_ALIGNMENT,
                                                          false, m_ri->m_bearing_alignment.button);
  m_installation_sizer->Add(m_bearing_alignment_button, 0, wxALL, BORDER);
  m_bearing_alignment_button->SetFont(m_pi->m_font);  // this bearing alignment work opposite to the one defined in the pi!
  m_bearing_alignment_button->minValue = -179;
  m_bearing_alignment_button->maxValue = 180;

  // The ANTENNA HEIGHT button
  m_antenna_height_button = new br24RadarControlButton(this, ID_ANTENNA_HEIGHT, _("Antenna height"), CT_ANTENNA_HEIGHT, false,
                                                       m_ri->m_antenna_height.button);
  m_installation_sizer->Add(m_antenna_height_button, 0, wxALL, BORDER);
  m_antenna_height_button->minValue = 0;
  m_antenna_height_button->maxValue = 30;

  // The LOCAL INTERFERENCE REJECTION button
  m_local_interference_rejection_button =
      new br24RadarControlButton(this, ID_LOCAL_INTERFERENCE_REJECTION, _("Local interference rej."),
                                 CT_LOCAL_INTERFERENCE_REJECTION, false, m_ri->m_local_interference_rejection.button);
  m_installation_sizer->Add(m_local_interference_rejection_button, 0, wxALL, BORDER);
  m_local_interference_rejection_button->minValue = 0;
  m_local_interference_rejection_button->maxValue =
      ARRAY_SIZE(target_separation_names) - 1;  // off, low, medium, high, same as target separation
  m_local_interference_rejection_button->names = target_separation_names;
  m_local_interference_rejection_button->SetLocalValue(m_ri->m_local_interference_rejection.button);

  // The SIDE LOBE SUPPRESSION button
  m_side_lobe_suppression_button = new br24RadarControlButton(this, ID_SIDE_LOBE_SUPPRESSION, _("Side lobe suppression"),
                                                              CT_SIDE_LOBE_SUPPRESSION, true, m_ri->m_side_lobe_suppression.button);
  m_installation_sizer->Add(m_side_lobe_suppression_button, 0, wxALL, BORDER);
  m_side_lobe_suppression_button->minValue = 0;
  m_side_lobe_suppression_button->maxValue = 100;
  m_side_lobe_suppression_button->SetLocalValue(m_ri->m_side_lobe_suppression.button);  // redraw after adding names

  // The MAIN BANG SIZE button
  m_main_bang_size_button = new br24RadarControlButton(this, ID_MAIN_BANG_SIZE, _("Main bang size"), CT_MAIN_BANG_SIZE, false,
                                                       m_pi->m_settings.main_bang_size);
  m_installation_sizer->Add(m_main_bang_size_button, 0, wxALL, BORDER);
  m_main_bang_size_button->minValue = 0;
  m_main_bang_size_button->maxValue = 10;

  m_top_sizer->Hide(m_installation_sizer);

  //***************** GUARD ZONE EDIT BOX *************//

  m_guard_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_guard_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The <<Back button
  wxButton* guard_back_button = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_guard_sizer->Add(guard_back_button, 0, wxALL, BORDER);
  guard_back_button->SetFont(m_pi->m_font);

  m_guard_zone_text = new wxStaticText(this, wxID_ANY, _("Guard zones"));
  m_guard_sizer->Add(m_guard_zone_text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

  wxStaticText* type_Text = new wxStaticText(this, wxID_ANY, _("Zone type"), wxDefaultPosition, wxDefaultSize, 0);
  m_guard_sizer->Add(type_Text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  guard_zone_names[0] = _("Off");
  guard_zone_names[1] = _("Arc");
  guard_zone_names[2] = _("Circle");
  m_guard_zone_type = new wxRadioBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, ARRAY_SIZE(guard_zone_names),
                                     guard_zone_names, 1, wxRA_SPECIFY_COLS);
  m_guard_sizer->Add(m_guard_zone_type, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  m_guard_zone_type->Connect(wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(br24ControlsDialog::OnGuardZoneModeClick), NULL,
                             this);

  // Inner and Outer Ranges
  wxStaticText* inner_range_Text = new wxStaticText(this, wxID_ANY, _("Inner range"), wxDefaultPosition, wxDefaultSize, 0);
  m_guard_sizer->Add(inner_range_Text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  m_inner_range = new wxTextCtrl(this, wxID_ANY);
  m_guard_sizer->Add(m_inner_range, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);
  m_inner_range->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(br24ControlsDialog::OnInner_Range_Value), NULL, this);
  ///   start of copy
  wxStaticText* outer_range_Text = new wxStaticText(this, wxID_ANY, _("Outer range"), wxDefaultPosition, wxDefaultSize, 0);
  m_guard_sizer->Add(outer_range_Text, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  m_outer_range = new wxTextCtrl(this, wxID_ANY);
  m_guard_sizer->Add(m_outer_range, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);
  m_outer_range->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(br24ControlsDialog::OnOuter_Range_Value), NULL, this);

  wxStaticText* pStart_Bearing = new wxStaticText(this, wxID_ANY, _("Start bearing"), wxDefaultPosition, wxDefaultSize, 0);
  m_guard_sizer->Add(pStart_Bearing, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  m_start_bearing = new wxTextCtrl(this, wxID_ANY);
  m_guard_sizer->Add(m_start_bearing, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);
  m_start_bearing->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(br24ControlsDialog::OnStart_Bearing_Value), NULL,
                           this);

  wxStaticText* pEnd_Bearing = new wxStaticText(this, wxID_ANY, _("End bearing"), wxDefaultPosition, wxDefaultSize, 0);
  m_guard_sizer->Add(pEnd_Bearing, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  m_end_bearing = new wxTextCtrl(this, wxID_ANY);
  m_guard_sizer->Add(m_end_bearing, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);
  m_end_bearing->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(br24ControlsDialog::OnEnd_Bearing_Value), NULL, this);

  // added check box to control multi swep filtering
  m_filter = new wxCheckBox(this, wxID_ANY, _("Multi Sweep Filter"), wxDefaultPosition, wxDefaultSize,
                            wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE);
  m_guard_sizer->Add(m_filter, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  m_filter->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(br24ControlsDialog::OnFilterClick), NULL, this);

  m_top_sizer->Hide(m_guard_sizer);

  //**************** ADJUST BOX ******************//

  m_adjust_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_adjust_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Back button
  wxButton* bAdjustBack = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_adjust_sizer->Add(bAdjustBack, 0, wxALL, BORDER);
  bAdjustBack->SetFont(m_pi->m_font);

  // The RANGE button
  m_range_button = new br24RadarRangeControlButton(this, m_ri, ID_RANGE, _("Range"));
  m_adjust_sizer->Add(m_range_button, 0, wxALL, BORDER);

  // The GAIN button
  m_gain_button = new br24RadarControlButton(this, ID_GAIN, _("Gain"), CT_GAIN, true, m_ri->m_gain.button);
  m_adjust_sizer->Add(m_gain_button, 0, wxALL, BORDER);

  // The SEA button
  m_sea_button = new br24RadarControlButton(this, ID_SEA, _("Sea clutter"), CT_SEA, true, m_ri->m_sea.button);
  m_adjust_sizer->Add(m_sea_button, 0, wxALL, BORDER);

  // The RAIN button
  m_rain_button = new br24RadarControlButton(this, ID_RAIN, _("Rain clutter"), CT_RAIN, false, m_ri->m_rain.button);
  m_adjust_sizer->Add(m_rain_button, 0, wxALL, BORDER);

  m_top_sizer->Hide(m_adjust_sizer);

  //**************** BEARING BOX ******************//

  m_bearing_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_bearing_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Back button
  wxButton* bBearingBack = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_bearing_sizer->Add(bBearingBack, 0, wxALL, BORDER);
  bBearingBack->SetFont(m_pi->m_font);

  // The CLEAR CURSOR button
  m_clear_cursor = new wxButton(this, ID_CLEAR_CURSOR, _("Clear cursor"), wxDefaultPosition, g_smallButtonSize, 0);
  m_bearing_sizer->Add(m_clear_cursor, 0, wxALL, BORDER);
  m_clear_cursor->SetFont(m_pi->m_font);

  for (int b = 0; b < BEARING_LINES; b++) {
    // The BEARING button
    wxString label = _("Place EBL/VRM");
    label << wxString::Format(wxT("%d"), b + 1);
    m_bearing_buttons[b] = new wxButton(this, ID_BEARING_SET + b, label, wxDefaultPosition, g_smallButtonSize, 0);
    m_bearing_buttons[b]->SetFont(m_pi->m_font);
    m_bearing_sizer->Add(m_bearing_buttons[b], 0, wxALL, BORDER);
    m_bearing_buttons[b]->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(br24ControlsDialog::OnBearingSetButtonClick),
                                  0, this);
  }

  m_top_sizer->Hide(m_bearing_sizer);

  //**************** VIEW BOX ******************//
  // These are the controls that the users sees when the View button is selected

  m_view_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_view_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Back button
  wxButton* bMenuBack = new wxButton(this, ID_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
  m_view_sizer->Add(bMenuBack, 0, wxALL, BORDER);
  bMenuBack->SetFont(m_pi->m_font);

  // The TARGET_TRAIL button
  target_trail_names[TRAIL_OFF] = _("Off");
  target_trail_names[TRAIL_15SEC] = _("15 sec");
  target_trail_names[TRAIL_30SEC] = _("30 sec");
  target_trail_names[TRAIL_1MIN] = _("1 min");
  target_trail_names[TRAIL_3MIN] = _("3 min");
  target_trail_names[TRAIL_10MIN] = _("10 min");
  target_trail_names[TRAIL_CONTINUOUS] = _("Continuous");

  m_target_trails_button =
      new br24RadarControlButton(this, ID_TARGET_TRAILS, _("Target trails"), CT_TARGET_TRAILS, false, m_ri->m_target_trails.button);
  m_view_sizer->Add(m_target_trails_button, 0, wxALL, BORDER);
  m_target_trails_button->minValue = 0;
  m_target_trails_button->maxValue = ARRAY_SIZE(target_trail_names) - 1;
  m_target_trails_button->names = target_trail_names;
  m_target_trails_button->SetLocalValue(m_ri->m_target_trails.button);  // redraw after adding names

  // The Trails Motion button
  m_trails_motion_button = new wxButton(this, ID_TRAILS_MOTION, _("True/Relative trails"), wxDefaultPosition, g_buttonSize, 0);
  m_view_sizer->Add(m_trails_motion_button, 0, wxALL, BORDER);
  m_trails_motion_button->SetFont(m_pi->m_font);

  // The Clear Trails button
  m_clear_trails_button = new wxButton(this, ID_CLEAR_TRAILS, _("Clear trails"), wxDefaultPosition, g_smallButtonSize, 0);
  m_view_sizer->Add(m_clear_trails_button, 0, wxALL, BORDER);
  m_clear_trails_button->SetFont(m_pi->m_font);

  // The Multi Sweep Filter button
  wxString labelSweep;
  labelSweep << _("Multi Sweep Filter") << wxT("\n") << (m_ri->m_multi_sweep_filter ? _("On") : _("Off"));
  m_multi_sweep_button = new wxButton(this, ID_MULTISWEEP, labelSweep, wxDefaultPosition, g_buttonSize, 0);
  m_view_sizer->Add(m_multi_sweep_button, 0, wxALL, BORDER);
  m_multi_sweep_button->SetFont(m_pi->m_font);

  // The Rotation button
  m_orientation_button = new wxButton(this, ID_ORIENTATION, _("Orientation"), wxDefaultPosition, g_buttonSize, 0);
  m_view_sizer->Add(m_orientation_button, 0, wxALL, BORDER);
  m_orientation_button->SetFont(m_pi->m_font);
  // Updated when we receive data

  // The REFRESHRATE button
  m_refresh_rate_button =
      new br24RadarControlButton(this, ID_REFRESHRATE, _("Refresh rate"), CT_REFRESHRATE, false, m_pi->m_settings.refreshrate);
  m_view_sizer->Add(m_refresh_rate_button, 0, wxALL, BORDER);
  m_refresh_rate_button->minValue = 1;
  m_refresh_rate_button->maxValue = 5;

  // The TRANSPARENCY button
  m_transparency_button = new br24RadarControlButton(this, ID_TRANSPARENCY, _("Transparency"), CT_TRANSPARENCY, false,
                                                     m_pi->m_settings.overlay_transparency);
  m_view_sizer->Add(m_transparency_button, 0, wxALL, BORDER);
  m_transparency_button->minValue = MIN_OVERLAY_TRANSPARENCY;
  m_transparency_button->maxValue = MAX_OVERLAY_TRANSPARENCY;

  m_top_sizer->Hide(m_view_sizer);

  //**************** CONTROL BOX ******************//
  // These are the controls that the users sees when the dialog is started

  // A box sizer to contain RANGE, GAIN etc button
  m_control_sizer = new wxBoxSizer(wxVERTICAL);
  m_top_sizer->Add(m_control_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The Transmit button
  m_radar_state = new wxButton(this, ID_RADAR_STATE, _("Unknown"), wxDefaultPosition, g_buttonSize, 0);
  m_control_sizer->Add(m_radar_state, 0, wxALL, BORDER);
  m_radar_state->SetFont(m_pi->m_font);
  // Updated when we receive data

  // The SHOW / HIDE RADAR button
  m_window_button = new wxButton(this, ID_SHOW_RADAR, wxT(""), wxDefaultPosition, g_smallButtonSize, 0);
  m_control_sizer->Add(m_window_button, 0, wxALL, BORDER);
  m_window_button->SetFont(m_pi->m_font);

  // The RADAR ONLY / OVERLAY button
  wxString overlay = _("Overlay");
  m_overlay_button = new wxButton(this, ID_RADAR_OVERLAY, overlay, wxDefaultPosition, g_buttonSize, 0);
  m_control_sizer->Add(m_overlay_button, 0, wxALL, BORDER);
  m_overlay_button->SetFont(m_pi->m_font);

  //***************** TRANSMIT SIZER, items hidden when not transmitting ****************//

  m_transmit_sizer = new wxBoxSizer(wxVERTICAL);
  m_control_sizer->Add(m_transmit_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

  // The ADJUST button
  m_adjust_button = new wxButton(this, ID_ADJUST, _("Adjust"), wxDefaultPosition, g_smallButtonSize, 0);
  m_transmit_sizer->Add(m_adjust_button, 0, wxALL, BORDER);
  m_adjust_button->SetFont(m_pi->m_font);

  // The ADVANCED button
  wxButton* bAdvanced = new wxButton(this, ID_ADVANCED, _("Advanced"), wxDefaultPosition, g_smallButtonSize, 0);
  m_transmit_sizer->Add(bAdvanced, 0, wxALL, BORDER);
  bAdvanced->SetFont(m_pi->m_font);

  // The VIEW menu
  wxButton* bView = new wxButton(this, ID_VIEW, _("View"), wxDefaultPosition, g_smallButtonSize, 0);
  m_transmit_sizer->Add(bView, 0, wxALL, BORDER);
  bView->SetFont(m_pi->m_font);

  // The BEARING button
  m_bearing_button = new wxButton(this, ID_BEARING, _("EBL/VRM"), wxDefaultPosition, g_smallButtonSize, 0);
  m_transmit_sizer->Add(m_bearing_button, 0, wxALL, BORDER);
  m_bearing_button->SetFont(m_pi->m_font);

  // The GUARD ZONE 1 button
  m_guard_1_button = new wxButton(this, ID_ZONE1, wxT(""), wxDefaultPosition, g_buttonSize, 0);
  m_transmit_sizer->Add(m_guard_1_button, 0, wxALL, BORDER);
  m_guard_1_button->SetFont(m_pi->m_font);

  // The GUARD ZONE 2 button
  m_guard_2_button = new wxButton(this, ID_ZONE2, wxT(""), wxDefaultPosition, g_buttonSize, 0);
  m_transmit_sizer->Add(m_guard_2_button, 0, wxALL, BORDER);
  m_guard_2_button->SetFont(m_pi->m_font);

  // The TIMED TRANSMIT button
  timed_idle_times[0] = _("Off");
  timed_idle_times[1] = _("5 min");
  timed_idle_times[2] = _("10 min");
  timed_idle_times[3] = _("15 min");
  timed_idle_times[4] = _("20 min");
  timed_idle_times[5] = _("25 min");
  timed_idle_times[6] = _("30 min");
  timed_idle_times[7] = _("35 min");

  m_timed_idle_button =
      new br24RadarControlButton(this, ID_TIMED_IDLE, _("Timed Transmit"), CT_TIMED_IDLE, false, m_pi->m_settings.timed_idle);
  m_control_sizer->Add(m_timed_idle_button, 0, wxALL, BORDER);
  m_timed_idle_button->minValue = 0;
  m_timed_idle_button->maxValue = ARRAY_SIZE(timed_idle_times) - 1;
  m_timed_idle_button->names = timed_idle_times;
  m_timed_idle_button->SetLocalValue(m_pi->m_settings.timed_idle);

  // The INFO button
  wxButton* bMessage = new wxButton(this, ID_MESSAGE, _("Info"), wxDefaultPosition, g_smallButtonSize, 0);
  m_control_sizer->Add(bMessage, 0, wxALL, BORDER);
  bMessage->SetFont(m_pi->m_font);

  m_from_sizer = m_control_sizer;
  m_control_sizer->Hide(m_transmit_sizer);
  m_top_sizer->Hide(m_control_sizer);

  UpdateGuardZoneState();

  DimeWindow(this);  // Call OpenCPN to change colours depending on day/night mode
  Layout();
  Fit();
  // wxSize size_min = GetBestSize();
  // SetMinSize(size_min);
  // SetSize(size_min);
}

void br24ControlsDialog::SwitchTo(wxBoxSizer* to, const wxChar* name) {
  m_top_sizer->Hide(m_from_sizer);
  m_top_sizer->Show(to);
  LOG_VERBOSE(wxT("%s switch to control view %s"), m_log_name.c_str(), name);

  UpdateAdvanced4GState();
  UpdateGuardZoneState();
  SetMenuAutoHideTimeout();

  if (to != m_edit_sizer) {
    m_from_sizer = to;
  }

  to->Layout();
  m_top_sizer->Layout();
  Fit();
}

void br24ControlsDialog::UpdateAdvanced4GState() {
  if (m_top_sizer->IsShown(m_advanced_sizer)) {
    if (m_ri->m_radar_type == RT_4G) {
      m_advanced_sizer->Show(m_advanced_4G_sizer);
    } else {
      m_advanced_sizer->Hide(m_advanced_4G_sizer);
    }
  }
}

void br24ControlsDialog::UpdateGuardZoneState() {
  wxString label1, label2;

  label1 << _("Guard zone") << wxT(" 1\n") << guard_zone_names[m_ri->m_guard_zone[0]->m_type];
  m_guard_1_button->SetLabel(label1);

  label2 << _("Guard zone") << wxT(" 2\n") << guard_zone_names[m_ri->m_guard_zone[1]->m_type];
  m_guard_2_button->SetLabel(label2);
}

#if 0
void br24ControlsDialog::SetTimedIdleIndex(int index) {
  m_timed_idle_button->SetValue(index);  // set and recompute the Timed Idle label
}
#endif

void br24ControlsDialog::OnZone1ButtonClick(wxCommandEvent& event) { ShowGuardZone(0); }

void br24ControlsDialog::OnZone2ButtonClick(wxCommandEvent& event) { ShowGuardZone(1); }

void br24ControlsDialog::OnClose(wxCloseEvent& event) { m_pi->OnControlDialogClose(m_ri); }

void br24ControlsDialog::OnIdOKClick(wxCommandEvent& event) { m_pi->OnControlDialogClose(m_ri); }

void br24ControlsDialog::OnPlusTenClick(wxCommandEvent& event) {
  LOG_DIALOG(wxT("%s OnPlustTenClick for %s value %d"), m_log_name.c_str(), m_from_control->GetLabel().c_str(),
             m_from_control->value + 10);
  m_from_control->AdjustValue(+10);
  m_auto_button->Enable();

  wxString label = m_from_control->GetLabel();
  m_value_text->SetLabel(label);
}

void br24ControlsDialog::OnPlusClick(wxCommandEvent& event) {
  m_from_control->AdjustValue(+1);
  m_auto_button->Enable();

  wxString label = m_from_control->GetLabel();
  m_value_text->SetLabel(label);
}

void br24ControlsDialog::OnBackClick(wxCommandEvent& event) {
  if (m_top_sizer->IsShown(m_edit_sizer)) {
    m_top_sizer->Hide(m_edit_sizer);
    SwitchTo(m_from_sizer, wxT("from (back click)"));
    m_from_control = 0;
  } else if (m_top_sizer->IsShown(m_installation_sizer)) {
    SwitchTo(m_advanced_sizer, wxT("advanced (back click)"));
  } else {
    SwitchTo(m_control_sizer, wxT("main (back click)"));
  }
}

void br24ControlsDialog::OnAutoClick(wxCommandEvent& event) {
  m_from_control->SetAuto();
  m_auto_button->Disable();

  OnBackClick(event);
}

void br24ControlsDialog::OnMultiSweepClick(wxCommandEvent& event) {
  wxString labelSweep;
  m_ri->m_multi_sweep_filter = !m_ri->m_multi_sweep_filter;
  labelSweep << _("Multi Sweep Filter") << wxT("\n") << (m_ri->m_multi_sweep_filter ? _("On") : _("Off"));
  m_multi_sweep_button->SetLabel(labelSweep);
}

void br24ControlsDialog::OnTrailsMotionClick(wxCommandEvent& event) {
  m_ri->ClearTrails();
  if (m_ri->m_trails_motion.value == TARGET_MOTION_RELATIVE) {
    m_ri->m_trails_motion.Update(TARGET_MOTION_TRUE);
  } else {
    m_ri->m_trails_motion.Update(TARGET_MOTION_RELATIVE);
  }
  UpdateControlValues(false);
}

void br24ControlsDialog::OnMinusClick(wxCommandEvent& event) {
  m_from_control->AdjustValue(-1);
  m_auto_button->Enable();

  wxString label = m_from_control->GetLabel();
  m_value_text->SetLabel(label);
}

void br24ControlsDialog::OnMinusTenClick(wxCommandEvent& event) {
  m_from_control->AdjustValue(-10);
  m_auto_button->Enable();

  wxString label = m_from_control->GetLabel();
  m_value_text->SetLabel(label);
}

void br24ControlsDialog::OnAdjustButtonClick(wxCommandEvent& event) { SwitchTo(m_adjust_sizer, wxT("adjust")); }

void br24ControlsDialog::OnAdvancedButtonClick(wxCommandEvent& event) { SwitchTo(m_advanced_sizer, wxT("advanced")); }

void br24ControlsDialog::OnViewButtonClick(wxCommandEvent& event) { SwitchTo(m_view_sizer, wxT("view")); }

void br24ControlsDialog::OnInstallationButtonClick(wxCommandEvent& event) { SwitchTo(m_installation_sizer, wxT("installation")); }

void br24ControlsDialog::OnPreferencesButtonClick(wxCommandEvent& event) { m_pi->ShowPreferencesDialog(m_pi->m_parent_window); }

void br24ControlsDialog::OnBearingButtonClick(wxCommandEvent& event) { SwitchTo(m_bearing_sizer, wxT("bearing")); }

void br24ControlsDialog::OnMessageButtonClick(wxCommandEvent& event) {
  SetMenuAutoHideTimeout();

  if (m_pi->m_pMessageBox) {
    m_pi->m_pMessageBox->UpdateMessage(true);
  }
}

void br24ControlsDialog::EnterEditMode(br24RadarControlButton* button) {
  m_from_control = button;  // Keep a record of which button was clicked
  m_value_text->SetLabel(button->GetLabel());

  SwitchTo(m_edit_sizer, wxT("edit"));

  if (m_from_control->hasAuto) {
    m_auto_button->Show();
    if (m_from_control->isAuto) {
      m_auto_button->Disable();
    } else {
      m_auto_button->Enable();
    }
  } else {
    m_auto_button->Hide();
  }
  if (m_from_control->maxValue > 20) {
    m_plus_ten_button->Show();
    m_minus_ten_button->Show();
  } else {
    m_plus_ten_button->Hide();
    m_minus_ten_button->Hide();
  }
  m_edit_sizer->Layout();
}

void br24ControlsDialog::OnRadarControlButtonClick(wxCommandEvent& event) {
  EnterEditMode((br24RadarControlButton*)event.GetEventObject());
}

void br24ControlsDialog::OnRadarShowButtonClick(wxCommandEvent& event) {
  SetMenuAutoHideTimeout();

  bool show = true;

  if (m_pi->m_settings.enable_dual_radar) {
    if (m_pi->m_settings.show_radar[m_ri->m_radar]) {
      int show_other_radar = m_pi->m_settings.show_radar[1 - m_ri->m_radar];
      if (show_other_radar) {
        // Hide both windows
        show = false;
      }
    }
    for (int r = 0; r < RADARS; r++) {
      m_pi->m_settings.show_radar[r] = show;
      if (!show && m_pi->m_settings.chart_overlay != r) {
        m_pi->m_settings.show_radar_control[r] = false;
      }
      LOG_DIALOG(wxT("%s OnRadarShowButton: show_radar[%d]=%d"), m_log_name.c_str(), r, show);
    }
  } else {
    if (m_ri->IsPaneShown()) {
      show = false;
    }
    m_pi->m_settings.show_radar[0] = show;
    LOG_DIALOG(wxT("%s OnRadarShowButton: show_radar[%d]=%d"), m_log_name.c_str(), 0, show);
  }

  m_pi->NotifyRadarWindowViz();
}

void br24ControlsDialog::OnRadarOverlayButtonClick(wxCommandEvent& event) {
  SetMenuAutoHideTimeout();

  int this_radar = m_ri->m_radar;
  int other_radar = 1 - this_radar;

  if (m_pi->m_settings.chart_overlay != this_radar) {
    m_pi->m_settings.chart_overlay = this_radar;
  } else if (m_pi->m_settings.enable_dual_radar && ALL_RADARS(m_pi->m_settings.show_radar, 0)) {
    // If no radar window shown, toggle overlay to different radar
    m_pi->m_settings.chart_overlay = other_radar;

    wxPoint pos = m_pi->m_radar[this_radar]->m_control_dialog->GetPosition();

    // Flip which control is visible.
    m_pi->ShowRadarControl(this_radar, false);

    if (!m_pi->m_radar[other_radar]->m_control_dialog || !m_pi->m_radar[other_radar]->m_control_dialog->IsShown()) {
      m_pi->ShowRadarControl(other_radar, true);
      m_pi->m_radar[other_radar]->m_control_dialog->SetPosition(pos);
    }
  } else {
    // If a radar window is shown, switch overlay off
    m_pi->m_settings.chart_overlay = -1;
  }
  m_ri->m_overlay.Update(m_pi->m_settings.chart_overlay == this_radar);
  UpdateControlValues(true);
}

void br24ControlsDialog::OnRadarGainButtonClick(wxCommandEvent& event) {
  EnterEditMode((br24RadarControlButton*)event.GetEventObject());
}

void br24ControlsDialog::OnRadarStateButtonClick(wxCommandEvent& event) {
  SetMenuAutoHideTimeout();
  m_pi->m_settings.timed_idle = 0;
  switch (m_ri->m_state.value) {
    case RADAR_OFF:
    case RADAR_WAKING_UP:
        break;
    case RADAR_STANDBY:
        m_ri->RequestRadarState(RADAR_TRANSMIT);
        break;
    case RADAR_TRANSMIT:
        m_ri->RequestRadarState(RADAR_STANDBY);
        break;
    }
}

void br24ControlsDialog::OnClearTrailsButtonClick(wxCommandEvent& event) { m_ri->ClearTrails(); }

void br24ControlsDialog::OnOrientationButtonClick(wxCommandEvent& event) {
  m_ri->m_orientation.Update(1 - m_ri->m_orientation.value);
  UpdateControlValues(false);
}

void br24ControlsDialog::OnBearingSetButtonClick(wxCommandEvent& event) {
  int bearing = event.GetId() - ID_BEARING_SET;
  LOG_DIALOG(wxT("%s OnBearingSetButtonClick for bearing #%d"), m_log_name.c_str(), bearing + 1);

  m_ri->SetBearing(bearing);
}

void br24ControlsDialog::OnClearCursorButtonClick(wxCommandEvent& event) {
  LOG_DIALOG(wxT("%s OnClearCursorButtonClick"), m_log_name.c_str());
  m_ri->SetMouseVrmEbl(0., 0.);
  SwitchTo(m_control_sizer, wxT("main (clear cursor)"));
}

void br24ControlsDialog::OnMove(wxMoveEvent& event) {
  m_manually_positioned = true;
  event.Skip();
}

void br24ControlsDialog::UpdateControlValues(bool refreshAll) {
  wxString o;

  if (m_ri->m_state.mod) {
    refreshAll = true;
  }

  RadarState state = (RadarState)m_ri->m_state.GetButton();

  o = _("Standby / Transmit");
  o << wxT("\n");
  if (m_pi->m_settings.timed_idle == 0) {
    switch (state) {
      case RADAR_OFF:
        o << _("Off");
        break;
      case RADAR_STANDBY:
        o << _("Standby");
        break;
      case RADAR_WAKING_UP:
        o << _("Waking up");
        break;
      case RADAR_TRANSMIT:
        o << _("Transmit");
        break;
    }
    m_timed_idle_button->SetLocalValue(0);
  } else {
    time_t now = time(0);
    int left = m_pi->m_idle_standby - now;
    if (left > 0) {
      o = _("Standby in");
      o << wxString::Format(wxT(" %d:%02d"), left / 60, left % 60);
    } else {
      left = m_pi->m_idle_transmit - now;
      if (left >= 0) {
        o = _("Transmit in");
        o << wxString::Format(wxT(" %d:%02d"), left / 60, left % 60);
      }
    }
  }
  m_radar_state->SetLabel(o);

  if (state == RADAR_TRANSMIT) {
    if (m_top_sizer->IsShown(m_control_sizer)) {
      m_control_sizer->Show(m_transmit_sizer);
      m_top_sizer->Show(m_timed_idle_button);
      m_control_sizer->Layout();
    }
  } else {
    m_control_sizer->Hide(m_transmit_sizer);
    if (m_pi->m_settings.timed_idle) {
      m_top_sizer->Show(m_timed_idle_button);
    } else {
      m_top_sizer->Hide(m_timed_idle_button);
    }
    m_control_sizer->Layout();
  }
  m_top_sizer->Layout();
  Layout();

  if (m_pi->m_settings.enable_dual_radar) {
    int show_other_radar = m_pi->m_settings.show_radar[1 - m_ri->m_radar];
    if (m_pi->m_settings.show_radar[m_ri->m_radar]) {
      if (show_other_radar) {
        o = _("Hide both windows");
      } else {
        o = _("Show other window");
      }
    } else {
      if (show_other_radar) {
        o = _("Show this window");  // can happen if this window hidden but control is for overlay
      } else {
        o = _("Show both windows");
      }
    }
  } else {
    o = (m_pi->m_settings.show_radar[m_ri->m_radar]) ? _("Hide window") : _("Show window");
  }
  m_window_button->SetLabel(o);

  for (int b = 0; b < BEARING_LINES; b++) {
    if (m_ri->m_vrm[b] != 0.0) {
      o = _("Clear EBL/VRM");
      o << wxString::Format(wxT("%d"), b + 1);
    } else {
      o = _("Place EBL/VRM");
      o << wxString::Format(wxT("%d"), b + 1);
    }
    m_bearing_buttons[b]->SetLabel(o);
  }

  if (m_ri->m_target_trails.mod || refreshAll) {
    m_target_trails_button->SetLocalValue(m_ri->m_target_trails.GetButton());
  }

  if (m_ri->m_trails_motion.mod || refreshAll) {
    o = _("True/Relative trails");
    o << wxT("\n");
    if (m_ri->m_trails_motion.value == TARGET_MOTION_TRUE) {
      o << _("True");
    } else {
      o << _("Relative");
    }
    m_trails_motion_button->SetLabel(o);
  }

  if (m_ri->m_orientation.mod || refreshAll) {
    o = _("Orientation");
    o << wxT("\n");
    switch (m_ri->m_orientation.GetButton()) {
      case ORIENTATION_NORTH_UP:
        o << _("North up");
        break;
      case ORIENTATION_HEAD_UP:
        o << _("Head up");
        break;
      default:
        o << _("???");
    }
    m_orientation_button->SetLabel(o);
  }

  if (m_ri->m_overlay.mod || ((m_pi->m_settings.chart_overlay == m_ri->m_radar) != (m_ri->m_overlay.button != 0)) || refreshAll) {
    o = _("Overlay");
    o << wxT("\n");
    if (m_pi->m_settings.enable_dual_radar && ALL_RADARS(m_pi->m_settings.show_radar, 0)) {
      o << ((m_ri->m_overlay.GetButton() > 0) ? m_ri->m_name : _("Off"));
    } else {
      o << ((m_ri->m_overlay.GetButton() > 0) ? _("On") : _("Off"));
    }
    m_overlay_button->SetLabel(o);
  }

  if (m_ri->m_range.mod || refreshAll) {
    m_ri->m_range.GetButton();
    m_range_button->SetRangeLabel();
  }

  // gain
  if (m_ri->m_gain.mod || refreshAll) {
    int button = m_ri->m_gain.GetButton();
    if (button == -1) {
      m_gain_button->SetLocalAuto();
    } else {
      m_gain_button->SetLocalValue(button);
    }
  }

  //  rain
  if (m_ri->m_rain.mod || refreshAll) {
    m_rain_button->SetLocalValue(m_ri->m_rain.GetButton());
  }

  //   sea
  if (m_ri->m_sea.mod || refreshAll) {
    int button = m_ri->m_sea.GetButton();
    if (button == -1) {
      m_sea_button->SetLocalAuto();
    } else {
      m_sea_button->SetLocalValue(button);
    }
  }

  //   target_boost
  if (m_ri->m_target_boost.mod || refreshAll) {
    m_target_boost_button->SetLocalValue(m_ri->m_target_boost.GetButton());
  }

  //   target_expansion
  if (m_ri->m_target_expansion.mod || refreshAll) {
    m_target_expansion_button->SetLocalValue(m_ri->m_target_expansion.GetButton());
  }

  //  noise_rejection
  if (m_ri->m_noise_rejection.mod || refreshAll) {
    m_noise_rejection_button->SetLocalValue(m_ri->m_noise_rejection.GetButton());
  }

  //  target_separation
  if (m_ri->m_target_separation.mod || refreshAll) {
    m_target_separation_button->SetLocalValue(m_ri->m_target_separation.GetButton());
  }

  //  interference_rejection
  if (m_ri->m_interference_rejection.mod || refreshAll) {
    m_interference_rejection_button->SetLocalValue(m_ri->m_interference_rejection.GetButton());
  }

  // scanspeed
  if (m_ri->m_scan_speed.mod || refreshAll) {
    m_scan_speed_button->SetLocalValue(m_ri->m_scan_speed.GetButton());
  }

  //   antenna height
  if (m_ri->m_antenna_height.mod || refreshAll) {
    m_antenna_height_button->SetLocalValue(m_ri->m_antenna_height.GetButton());
  }

  //  bearing alignment
  if (m_ri->m_bearing_alignment.mod || refreshAll) {
    m_bearing_alignment_button->SetLocalValue(m_ri->m_bearing_alignment.GetButton());
  }

  //  local interference rejection
  if (m_ri->m_local_interference_rejection.mod || refreshAll) {
    m_local_interference_rejection_button->SetLocalValue(m_ri->m_local_interference_rejection.GetButton());
  }

  // side lobe suppression
  if (m_ri->m_side_lobe_suppression.mod || refreshAll) {
    int button = m_ri->m_side_lobe_suppression.GetButton();
    if (button == -1) {
      m_side_lobe_suppression_button->SetLocalAuto();
    } else {
      m_side_lobe_suppression_button->SetLocalValue(button);
    }
  }

  // Update the text that is currently shown in the edit box, this is a copy of the button itself
  if (m_from_control) {
    wxString label = m_from_control->GetLabel();
    m_value_text->SetLabel(label);
  }
}

void br24ControlsDialog::UpdateDialogShown() {
  if (m_hide) {
    if (IsShown()) {
      LOG_DIALOG(wxT("%s UpdateDialogShown explicit closed: Hidden"), m_log_name.c_str());
      Hide();
    }
    return;
  }

  if (m_hide_temporarily) {
    if (IsShown()) {
      LOG_DIALOG(wxT("%s UpdateDialogShown temporarily hidden"), m_log_name.c_str());
      Hide();
    }
    return;
  }

  if (m_top_sizer->IsShown(m_control_sizer)) {
    if (m_auto_hide_timeout && TIMED_OUT(time(0), m_auto_hide_timeout)) {
      if (IsShown()) {
        LOG_DIALOG(wxT("%s UpdateDialogShown auto-hide"), m_log_name.c_str());
        Hide();
      }
      return;
    }
  } else {
    // If we're somewhere in the sub-window, don't close the dialog
    m_auto_hide_timeout = 0;
  }

  // Following helps on OSX where the control is SHOW_ON_TOP to not show when no part of OCPN is focused
  wxWindow* focused = FindFocus();
  if (!focused) {
    LOG_DIALOG(wxT("%s UpdateDialogShown app not focused"), m_log_name.c_str());
    return;
  }

  if (!IsShown()) {
    LOG_DIALOG(wxT("%s UpdateDialogShown manually opened"), m_log_name.c_str());
    if (!m_top_sizer->IsShown(m_control_sizer) && !m_top_sizer->IsShown(m_advanced_sizer) && !m_top_sizer->IsShown(m_view_sizer) &&
        !m_top_sizer->IsShown(m_edit_sizer) && !m_top_sizer->IsShown(m_installation_sizer) &&
        !m_top_sizer->IsShown(m_guard_sizer) && !m_top_sizer->IsShown(m_adjust_sizer) && !m_top_sizer->IsShown(m_bearing_sizer)) {
      SwitchTo(m_control_sizer, wxT("main (manual open)"));
    }
    m_control_sizer->Layout();
    m_top_sizer->Layout();
    Show();
    Raise();

    m_edit_sizer->Layout();
    m_top_sizer->Layout();

    // If the corresponding radar panel is now in a different position from what we remembered
    // then reset the dialog to the left or right of the radar panel.
    wxPoint panelPos = m_ri->m_radar_panel->GetPos();
    bool controlInitialShow = m_pi->m_settings.control_pos[m_ri->m_radar] == OFFSCREEN_CONTROL;
    bool panelShown = m_ri->m_radar_panel->IsShown();
    bool panelMoved = m_panel_position.IsFullySpecified() && panelPos != m_panel_position;

    if (panelShown                                  // if the radar pane is shown and
        && ((panelMoved && !m_manually_positioned)  // has moved this session and user did not touch pos, or
            || controlInitialShow)) {               // the position has never been set at all, ever
      wxSize panelSize = m_ri->m_radar_panel->GetSize();
      wxSize mySize = this->GetSize();

      wxPoint newPos;
      newPos.x = panelPos.x + panelSize.x - mySize.x;
      newPos.y = panelPos.y;
      SetPosition(newPos);
      LOG_DIALOG(wxT("%s show control menu over menu button"), m_log_name.c_str());
    } else if (controlInitialShow) {  // When all else fails set it to default position
      SetPosition(wxPoint(100 + m_ri->m_radar * 100, 100));
      LOG_DIALOG(wxT("%s show control menu at initial location"), m_log_name.c_str());
    }
    EnsureWindowNearOpenCPNWindow();  // If the position is really weird, move it
    m_pi->m_settings.control_pos[m_ri->m_radar] = GetPosition();
    m_pi->m_settings.show_radar_control[m_ri->m_radar] = true;
    m_panel_position = panelPos;
  }
  if (m_top_sizer->IsShown(m_control_sizer)) {
    Fit();
  }
}

#define PROXIMITY_MARGIN 32

void br24ControlsDialog::EnsureWindowNearOpenCPNWindow() {
  wxWindow* parent = m_pi->m_parent_window;
  while (parent->GetParent()) {
    parent = parent->GetParent();
  }
  wxPoint oPos = parent->GetScreenPosition();
  wxSize oSize = parent->GetSize();
  oSize.x += PROXIMITY_MARGIN;
  oSize.y += PROXIMITY_MARGIN;

  wxPoint mPos = GetPosition();
  wxSize mSize = GetSize();
  mSize.x += PROXIMITY_MARGIN;
  mSize.y += PROXIMITY_MARGIN;

  bool move = false;

  // LOG_DIALOG(wxT("%s control %d,%d is near OpenCPN at %d,%d to %d,%d?"), m_log_name.c_str(), mPos.x, mPos.y, oPos.x, oPos.y
  // , oPos.x + oSize.x, oPos.y + oSize.y);

  if (mPos.x + mSize.x < oPos.x) {
    mPos.x = oPos.x;
    move = true;
  }
  if (oPos.x + oSize.x < mPos.x) {
    mPos.x = oPos.x + oSize.x - mSize.x;
    move = true;
  }
  if (mPos.y + mSize.y < oPos.y) {
    mPos.y = oPos.y;
    move = true;
  }
  if (oPos.y + oSize.y < mPos.y) {
    mPos.y = oPos.y + oSize.y - mSize.y;
    move = true;
  }
  if (move) {
    LOG_DIALOG(wxT("%s Move control dialog to %d,%d to be near OpenCPN at %d,%d to %d,%d"), m_log_name.c_str(), mPos.x, mPos.y,
               oPos.x, oPos.y, oPos.x + oSize.x, oPos.y + oSize.y);
  }
  SetPosition(mPos);
}

void br24ControlsDialog::HideTemporarily() {
  m_hide_temporarily = true;
  UpdateDialogShown();
}

void br24ControlsDialog::UnHideTemporarily() {
  m_hide_temporarily = false;
  SetMenuAutoHideTimeout();
  UpdateDialogShown();
}

void br24ControlsDialog::ShowDialog() {
  m_hide = false;
  UnHideTemporarily();
  UpdateControlValues(true);
}

void br24ControlsDialog::HideDialog() {
  m_hide = true;
  m_auto_hide_timeout = 0;
  UpdateDialogShown();
}

void br24ControlsDialog::SetMenuAutoHideTimeout() {
  if (m_top_sizer->IsShown(m_control_sizer)) {
    switch (m_pi->m_settings.menu_auto_hide) {
      case 1:
        m_auto_hide_timeout = time(0) + 10;
        break;
      case 2:
        m_auto_hide_timeout = time(0) + 30;
        break;
      default:
        m_auto_hide_timeout = 0;
        break;
    }
  } else {
    m_auto_hide_timeout = 0;
  }
}

void br24ControlsDialog::ShowGuardZone(int zone) {
  double conversionFactor = RangeUnitsToMeters[m_pi->m_settings.range_units];

  m_guard_zone = m_ri->m_guard_zone[zone];

  wxString GuardZoneText;
  GuardZoneText << _("Guard Zone") << wxString::Format(wxT(" %d"), zone + 1);
  m_guard_zone_text->SetLabel(GuardZoneText);

  m_guard_zone_type->SetSelection(m_guard_zone->m_type);
  m_inner_range->SetValue(wxString::Format(wxT("%2.2f"), m_guard_zone->m_inner_range / conversionFactor));
  m_outer_range->SetValue(wxString::Format(wxT("%2.2f"), m_guard_zone->m_outer_range / conversionFactor));

  double bearing = SCALE_RAW_TO_DEGREES2048(m_guard_zone->m_start_bearing);
  if (bearing >= 180.0) {
    bearing -= 360.;
  }
  bearing = round(bearing);
  m_start_bearing->SetValue(wxString::Format(wxT("%3.0f"), bearing));

  bearing = SCALE_RAW_TO_DEGREES2048(m_guard_zone->m_end_bearing);
  if (bearing >= 180.0) {
    bearing -= 360.;
  }
  bearing = round(bearing);
  m_end_bearing->SetValue(wxString::Format(wxT("%3.0f"), bearing));
  m_filter->SetValue(m_guard_zone->m_multi_sweep_filter ? 1 : 0);

  m_top_sizer->Hide(m_control_sizer);
  SwitchTo(m_guard_sizer, wxT("guard"));
  SetGuardZoneVisibility();
  UpdateDialogShown();
}

void br24ControlsDialog::SetGuardZoneVisibility() {
  GuardZoneType zoneType = (GuardZoneType)m_guard_zone_type->GetSelection();

  m_guard_zone->SetType(zoneType);

  if (zoneType == GZ_OFF) {
    m_start_bearing->Disable();
    m_end_bearing->Disable();
    m_inner_range->Disable();
    m_outer_range->Disable();

  } else if (zoneType == GZ_CIRCLE) {
    m_start_bearing->Disable();
    m_end_bearing->Disable();
    m_inner_range->Enable();
    m_outer_range->Enable();

  } else {
    m_start_bearing->Enable();
    m_end_bearing->Enable();
    m_inner_range->Enable();
    m_outer_range->Enable();
  }
  m_guard_sizer->Layout();
}

void br24ControlsDialog::OnGuardZoneModeClick(wxCommandEvent& event) { SetGuardZoneVisibility(); }

void br24ControlsDialog::OnInner_Range_Value(wxCommandEvent& event) {
  wxString temp = m_inner_range->GetValue();
  double t;
  temp.ToDouble(&t);

  int conversionFactor = RangeUnitsToMeters[m_pi->m_settings.range_units];

  m_guard_zone->SetInnerRange((int)(t * conversionFactor));
}

void br24ControlsDialog::OnOuter_Range_Value(wxCommandEvent& event) {
  wxString temp = m_outer_range->GetValue();
  double t;
  temp.ToDouble(&t);

  int conversionFactor = RangeUnitsToMeters[m_pi->m_settings.range_units];

  m_guard_zone->SetOuterRange((int)(t * conversionFactor));
}

void br24ControlsDialog::OnStart_Bearing_Value(wxCommandEvent& event) {
  wxString temp = m_start_bearing->GetValue();
  double t;

  temp.ToDouble(&t);
  t = fmod(t, 360.);
  if (t < 0.) {
    t += 360.;
  }
  m_guard_zone->SetStartBearing(SCALE_DEGREES_TO_RAW2048(t));
}

void br24ControlsDialog::OnEnd_Bearing_Value(wxCommandEvent& event) {
  wxString temp = m_end_bearing->GetValue();
  double t;

  temp.ToDouble(&t);
  t = fmod(t, 360.);
  if (t < 0.) {
    t += 360.;
  }
  m_guard_zone->SetEndBearing(SCALE_DEGREES_TO_RAW2048(t));
}

void br24ControlsDialog::OnFilterClick(wxCommandEvent& event) {
  int filt = m_filter->GetValue();
  m_guard_zone->SetMultiSweepFilter(filt);
}

PLUGIN_END_NAMESPACE
