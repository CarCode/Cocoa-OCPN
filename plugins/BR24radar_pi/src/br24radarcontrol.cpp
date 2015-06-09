/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Navico BR24 Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2013 by Kees Verruijt         canboat@verruijt.net *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif                          //precompiled headers

#include <wx/socket.h>
#include "wx/apptrait.h"
//#include <wx/glcanvas.h>
#include "wx/datetime.h"
//#include <wx/fileconf.h>
//#include <fstream>
//#include "chart1.h"
using namespace std;

#ifdef __WXGTK__
#include <netinet/in.h>
#include <sys/ioctl.h>
#endif

#ifdef __WXMSW__
#include "GL/glu.h"
#endif

#include "br24radar_pi.h"


enum {                                      // process ID's
    ID_TEXTCTRL1 = 10000,
    ID_BACK,
    ID_PLUS_TEN,
    ID_PLUS,
    ID_VALUE,
    ID_MINUS,
    ID_MINUS_TEN,
    ID_AUTO,

    ID_MSG_BACK,

    ID_ADVANCED_BACK,
    ID_TRANSPARENCY,
    ID_INTERFERENCE_REJECTION,
    ID_TARGET_BOOST,
    ID_NOISE_REJECTION,
    ID_TARGET_SEPARATION,
    ID_DOWNSAMPLE,
    ID_REFRESHRATE,
    ID_SCAN_SPEED,
    ID_SCAN_AGE,
    ID_TIMED_IDLE,

    ID_RANGE,
    ID_GAIN,
    ID_SEA,
    ID_RAIN,
    ID_ADVANCED,
    ID_ZONE1,
    ID_ZONE2,

    ID_MESSAGE,
    ID_BPOS,
    ID_HEADING,
    ID_RADAR,
    ID_DATA

};

//---------------------------------------------------------------------------------------
//          Radar Control Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS(BR24ControlsDialog, wxDialog)

BEGIN_EVENT_TABLE(BR24ControlsDialog, wxDialog)

EVT_CLOSE(BR24ControlsDialog::OnClose)
EVT_BUTTON(ID_BACK, BR24ControlsDialog::OnBackClick)
EVT_BUTTON(ID_PLUS_TEN,  BR24ControlsDialog::OnPlusTenClick)
EVT_BUTTON(ID_PLUS,  BR24ControlsDialog::OnPlusClick)
EVT_BUTTON(ID_MINUS, BR24ControlsDialog::OnMinusClick)
EVT_BUTTON(ID_MINUS_TEN, BR24ControlsDialog::OnMinusTenClick)
EVT_BUTTON(ID_AUTO,  BR24ControlsDialog::OnAutoClick)

EVT_BUTTON(ID_MSG_BACK, BR24ControlsDialog::OnMessageBackButtonClick)

EVT_BUTTON(ID_ADVANCED_BACK,  BR24ControlsDialog::OnAdvancedBackButtonClick)
EVT_BUTTON(ID_TRANSPARENCY, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_INTERFERENCE_REJECTION, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TARGET_BOOST, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_NOISE_REJECTION, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TARGET_SEPARATION, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_DOWNSAMPLE, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_REFRESHRATE, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_SCAN_SPEED, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_SCAN_AGE, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_TIMED_IDLE, BR24ControlsDialog::OnRadarControlButtonClick)

EVT_BUTTON(ID_RANGE, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_GAIN, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_SEA, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_RAIN, BR24ControlsDialog::OnRadarControlButtonClick)
EVT_BUTTON(ID_ADVANCED, BR24ControlsDialog::OnAdvancedButtonClick)
EVT_BUTTON(ID_ZONE1, BR24ControlsDialog::OnZone1ButtonClick)
EVT_BUTTON(ID_ZONE2, BR24ControlsDialog::OnZone2ButtonClick)
EVT_BUTTON(ID_MESSAGE, BR24ControlsDialog::OnMessageButtonClick)

EVT_MOVE(BR24ControlsDialog::OnMove)
EVT_SIZE(BR24ControlsDialog::OnSize)

END_EVENT_TABLE()

//Ranges are metric for BR24 - the hex codes are little endian = 10 X range value

static const wxString g_range_names[2][20] = {
    {
        wxT("50 m"),
        wxT("75 m"),
        wxT("100 m"),
        wxT("150 m"),
        wxT("1/10 NM"),
        wxT("1/8 NM"),
        wxT("1/4 NM"),
        wxT("1/2 NM"),
        wxT("3/4 NM"),
        wxT("1 NM"),
        wxT("1.5 NM"),
        wxT("2 NM"),
        wxT("3 NM"),
        wxT("4 NM"),
        wxT("6 NM"),
        wxT("8 NM"),
        wxT("12 NM"),
        wxT("16 NM"),
        wxT("24 NM"),
        wxT("36 NM")
    },
    {
        wxT("50 m"),
        wxT("75 m"),
        wxT("100 m"),
        wxT("150 m"),
        wxT("200 m"),
        wxT("300 m"),
        wxT("500 m"),
        wxT("750 m"),
        wxT("1 km"),
        wxT("1.5 km"),
        wxT("2 km"),
        wxT("3 km"),
        wxT("4 km"),
        wxT("6 km"),
        wxT("8 km"),
        wxT("12 km"),
        wxT("16 km"),
        wxT("24 km"),
        wxT("36 km"),
        wxT("48 km")
    }
};

static const int g_range_distances[2][20] = {
    {
        50,
        75,
        110,
        160,
        220,
        330,
        500,
        800,
        1389,
        1852*1,
        2778,
        1852*2,
        1852*3,
        1852*4,
        1852*6,
        1852*8,
        1852*12,
        1852*16,
        1852*24,
        1852*36
    },
    {
        50,
        75,
        100,
        150,
        200,
        300,
        500,
        750,
        1000,
        1500,
        2000,
        3000,
        4000,
        6000,
        8000,
        12000,
        16000,
        24000,
        36000,
        48000
    }
};

static const int MILE_RANGE_COUNT = 20;
static const int METRIC_RANGE_COUNT = 20;

static const int g_range_maxValue[2] = { MILE_RANGE_COUNT, METRIC_RANGE_COUNT };

wxString interference_rejection_names[4];
wxString target_separation_names[4];
wxString noise_rejection_names[3];
wxString target_boost_names[3];
wxString scan_speed_names[2];
wxString timed_idle_times[8];

extern size_t convertMetersToRadarAllowedValue(int * range_meters, int units, RadarType radarType)
{
    const int * ranges;
    int myrange = int (*range_meters);
    myrange = int (myrange * 0.9);   // be shure to be inside the right interval
    // to prevent you get 1.5 mile with a value of 1855 meters
    size_t      n;

    if (units < 1) {                    /* NMi or Mi */
        n = MILE_RANGE_COUNT - 1;
        ranges = g_range_distances[0];
    }
    else {
        n = METRIC_RANGE_COUNT - 1;
        ranges = g_range_distances[1];
    }
    if (radarType != RT_4G) {
        n--;
    }
    unsigned int max = n;
    for (; n > 0; n--) {
        if (ranges[n] > 0 && ranges[n] < myrange) {  // step down until past the right range value
            break;
        }
    }
    if (n < max) n++;    //   and increase with 1 to get the correct index
    // n now points at the smallest value that is larger then *range_meters
    *range_meters = ranges[n];

    return n;
}

void RadarControlButton::SetValue(int newValue)
{
    if (newValue < minValue) {
        value = minValue;
    } else if (newValue > maxValue) {
        value = maxValue;
    } else {
        value = newValue;
    }
    if (controlType == CT_GAIN && isAuto) value = 40;  // start with gain = 40 when coming from auto
    wxString label;

    if (names) {
        label.Printf(wxT("%s\n%s"), firstLine.c_str(), names[value].c_str());
    } else {
        label.Printf(wxT("%s\n%d"), firstLine.c_str(), value);
    }

    this->SetLabel(label);

    isAuto = false;
    pPlugIn->SetControlValue(controlType, value);
}

void RadarControlButton::SetAuto()
{
    wxString label;

    label << firstLine << wxT("\n") << _("Auto");
    this->SetLabel(label);

    isAuto = true;

    pPlugIn->SetControlValue(controlType, -1);
}

int RadarRangeControlButton::SetValueInt(int newValue)
{                    // only called from the receive thread
    // newValue is the new range index number
    // sets the new range label in the button and returns the new range in meters
    int units = pPlugIn->settings.range_units;

    maxValue = g_range_maxValue[units] - 1;
    int oldValue = value;  // for debugging only
    if (newValue >= minValue && newValue <= maxValue) {
        value = newValue;
    } else if (pPlugIn->settings.auto_range_mode) {
        value = auto_range_index;
    } else if (value > maxValue) {
        value = maxValue;
    }
    int meters = g_range_distances[units][value];
    wxString label;
    wxString rangeText = value < 0 ? wxT("?") : g_range_names[units][value];

    if (pPlugIn->settings.auto_range_mode) {
        label << firstLine << wxT("\n") << _("Auto") << wxT(" (") << rangeText << wxT(")");
    }
    else {
        label << firstLine << wxT("\n") << rangeText;
    }
    this->SetLabel(label);
    if (pPlugIn->settings.verbose > 0) {
        wxLogMessage(wxT("BR24radar_pi: Range label '%s' auto=%d unit=%d max=%d new=%d old=%d"), rangeText.c_str(), pPlugIn->settings.auto_range_mode, units, maxValue, newValue, oldValue);
    }

    return meters;
}



void RadarRangeControlButton::SetValue(int newValue)
{                                        // newValue is the index of the new range
    // sends the command for the new range to the radar
    isAuto = false;
    pPlugIn->settings.auto_range_mode = false;

    int meters = SetValueInt(newValue);   // do not display the new value now, will be done by receive thread when frame with new range is received
    pPlugIn->SetRangeMeters(meters);        // send new value to the radar
}

void RadarRangeControlButton::SetAuto()
{
    isAuto = true;
    pPlugIn->settings.auto_range_mode = true;
    //   SetValueInt(auto_range_index);    // do not display the new value now, will be done by receive thread
}

BR24ControlsDialog::BR24ControlsDialog()
{
    //      printf("BR24BUIDialog ctor\n");
    Init();
}

BR24ControlsDialog::~BR24ControlsDialog()
{
}


void BR24ControlsDialog::Init()
{
    // Initialize all members that need initialization
}

bool BR24ControlsDialog::Create(wxWindow *parent, br24radar_pi *ppi, wxWindowID id,
                                const wxString& caption,
                                const wxPoint& pos, const wxSize& size, long style)
{

    pParent = parent;
    pPlugIn = ppi;

#ifdef wxMSW
    long wstyle = wxSYSTEM_MENU | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN;
#else
    long wstyle =                 wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER;
#endif

    if (!wxDialog::Create(parent, id, caption, pos, wxDefaultSize, wstyle)) {
        return false;
    }
    g_font = *OCPNGetFont(_("Dialog"), 12);

    CreateControls();
    return true;
}


void BR24ControlsDialog::CreateControls()
{
    static int BORDER = 0;
    wxFont fatFont = g_font;
    fatFont.SetWeight(wxFONTWEIGHT_BOLD);
    fatFont.SetPointSize(g_font.GetPointSize() + 1);

    // A top-level sizer
    topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);

    /*
     * Here be dragons... Since I haven't been able to create buttons that adapt up, and at the same time
     * calculate the biggest button, and I want all buttons to be the same width I use a trick.
     * I generate a multiline StaticText containing all the (long) button labels and find out what the width
     * of that is, and then generate the buttons using that width.
     * I know, this is a hack, but this way it works relatively nicely even with translations.
     */
    wxBoxSizer * testBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(testBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

    wxString label;
    label << _("Transparency") << wxT("\n");
    label << _("Interference rejection") << wxT("\n");
    label << _("Target separation") << wxT("\n");
    label << _("Noise rejection") << wxT("\n");
    label << _("Target boost") << wxT("\n");
    label << _("Downsample") << wxT("\n");
    label << _("Scan speed") << wxT("\n");
    label << _("Scan age") << wxT("\n");
    label << _("Timed Transmit") << wxT("\n");
    label << _("Gain") << wxT("\n");
    label << _("Sea clutter") << wxT("\n");
    label << _("Rain clutter") << wxT("\n");
    label << _("Auto") << wxT(" (1/20 Nm)\n");

    wxStaticText * testMessage = new wxStaticText(this, ID_BPOS, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    testBox->Add(testMessage, 0, wxALL, 2);
#else
    testBox->Add(testMessage, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
#endif
    testMessage->SetFont(g_font);

    topSizer->Fit(this);
    topSizer->Layout();
    int width = topSizer->GetSize().GetWidth() + 10;
    wxSize bestSize = GetBestSize();
    if (width < bestSize.GetWidth()) {
        width = bestSize.GetWidth();
    }
    if (width < 100) {
        width = 100;
    }
    if (width > 300) {
        width = 300;
    }
    g_buttonSize = wxSize(width, 50);
    if (pPlugIn->settings.verbose) {
        wxLogMessage(wxT("BR24radar_pi: Dynamic button width = %d"), g_buttonSize.GetWidth());
    }
    topSizer->Hide(testBox);
    topSizer->Remove(testBox);
    // Determined desired button width


    //**************** MESSAGE BOX ******************//
    // A box sizer to contain warnings

    messageBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(messageBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

    tMessage = new wxStaticText(this, ID_BPOS, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    messageBox->Add(tMessage, 0, wxALL, 2);
#else
    messageBox->Add(tMessage, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
#endif
    tMessage->SetLabel(_("Radar overlay requires the following data"));
    tMessage->SetFont(g_font);

    wxStaticBox* optionsBox = new wxStaticBox(this, wxID_ANY, _("OpenCPN options"));
    optionsBox->SetFont(g_font);
    wxStaticBoxSizer* optionsSizer = new wxStaticBoxSizer(optionsBox, wxVERTICAL);
    messageBox->Add(optionsSizer, 0, wxEXPAND | wxALL, BORDER * 2);

    cbOpenGL = new wxCheckBox(this, ID_BPOS, _("Accelerated Graphics (OpenGL)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    optionsSizer->Add(cbOpenGL, 0, wxALL, BORDER);
#else
    optionsSizer->Add(cbOpenGL, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbOpenGL->SetFont(g_font);
    cbOpenGL->Disable();

    wxStaticBox* nmeaBox = new wxStaticBox(this, wxID_ANY, _("Data sources"));
    nmeaBox->SetFont(g_font);
    wxStaticBoxSizer* nmeaSizer = new wxStaticBoxSizer(nmeaBox, wxVERTICAL);
    messageBox->Add(nmeaSizer, 0, wxEXPAND | wxALL, BORDER * 2);

    cbBoatPos = new wxCheckBox(this, ID_BPOS, _("Boat position"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    nmeaSizer->Add(cbBoatPos, 0, wxALL, BORDER);
#else
    nmeaSizer->Add(cbBoatPos, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbBoatPos->SetFont(g_font);
    cbBoatPos->Disable();

    cbHeading = new wxCheckBox(this, ID_HEADING, _("Heading"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    nmeaSizer->Add(cbHeading, 0, wxALL, BORDER);
#else
    nmeaSizer->Add(cbHeading, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbHeading->SetFont(g_font);
    cbHeading->Disable();

    cbVariation = new wxCheckBox(this, ID_HEADING, _("Variation"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    nmeaSizer->Add(cbVariation, 0, wxALL, BORDER);
#else
    nmeaSizer->Add(cbVariation, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbVariation->SetFont(g_font);
    cbVariation->Disable();

    ipBox = new wxStaticBox(this, wxID_ANY, _("ZeroConf via (wired) Ethernet"));
    ipBox->SetFont(g_font);
    wxStaticBoxSizer* ipSizer = new wxStaticBoxSizer(ipBox, wxVERTICAL);
    messageBox->Add(ipSizer, 0, wxEXPAND | wxALL, BORDER * 2);

    cbRadar = new wxCheckBox(this, ID_RADAR, _("Radar present"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    ipSizer->Add(cbRadar, 0, wxALL, BORDER);
#else
    ipSizer->Add(cbRadar, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbRadar->SetFont(g_font);
    cbRadar->Disable();

    cbData = new wxCheckBox(this, ID_DATA, _("Radar sending data"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    ipSizer->Add(cbData, 0, wxALL, BORDER);
#else
    ipSizer->Add(cbData, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    cbData->SetFont(g_font);
    cbData->Disable();

    tStatistics = new wxStaticText(this, ID_VALUE, _("Statistics"), wxDefaultPosition, g_buttonSize, 0);
    tStatistics->SetFont(*OCPNGetFont(_("Dialog"), 8));
    messageBox->Add(tStatistics, 0, wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE, BORDER);

    // The <Close> button
    bMsgBack = new wxButton(this, ID_MSG_BACK, _("&Close"), wxDefaultPosition, wxDefaultSize, 0);
#ifdef __WXOSX__
    messageBox->Add(bMsgBack, 0, wxALL, BORDER);
#else
    messageBox->Add(bMsgBack, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bMsgBack->SetFont(g_font);
    messageBox->Hide(bMsgBack);

    // topSizer->Hide(messageBox);

    //**************** EDIT BOX ******************//
    // A box sizer to contain RANGE button
    editBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(editBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

    // The <<Back button
    bBack = new wxButton(this, ID_BACK, _("<<\nBack"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bBack, 0, wxALL, BORDER);
#else
    editBox->Add(bBack, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bBack->SetFont(g_font);

    // The +10 button
    bPlusTen = new wxButton(this, ID_PLUS_TEN, _("+10"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bPlusTen, 0, wxALL, BORDER);
#else
    editBox->Add(bPlusTen, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bPlusTen->SetFont(g_font);

    // The + button
    bPlus = new wxButton(this, ID_PLUS, _("+"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bPlus, 0, wxALL, BORDER);
#else
    editBox->Add(bPlus, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bPlus->SetFont(g_font);

    // The VALUE button
    tValue = new wxStaticText(this, ID_VALUE, _("Value"), wxDefaultPosition, g_buttonSize, wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
#ifdef __WXOSX__
    editBox->Add(tValue, 0, wxALL, BORDER);
#else
    editBox->Add(tValue, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    tValue->SetFont(fatFont);
    tValue->SetBackgroundColour(*wxLIGHT_GREY);

    // The - button
    bMinus = new wxButton(this, ID_MINUS, _("-"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bMinus, 0, wxALL, BORDER);
#else
    editBox->Add(bMinus, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bMinus->SetFont(g_font);

    // The -10 button
    bMinusTen = new wxButton(this, ID_MINUS_TEN, _("-10"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bMinusTen, 0, wxALL, BORDER);
#else
    editBox->Add(bMinusTen, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bMinusTen->SetFont(g_font);

    // The Auto button
    bAuto = new wxButton(this, ID_AUTO, _("Auto"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    editBox->Add(bAuto, 0, wxALL, BORDER);
#else
    editBox->Add(bAuto, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bAuto->SetFont(g_font);

    topSizer->Hide(editBox);

    //**************** ADVANCED BOX ******************//
    // These are the controls that the users sees when the Advanced button is selected

    advancedBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(advancedBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

    // The Back button
    wxString backButtonStr;
    backButtonStr << wxT("<<\n") << _("Back");
    bAdvancedBack = new wxButton(this, ID_ADVANCED_BACK, backButtonStr, wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    advancedBox->Add(bAdvancedBack, 0, wxALL, BORDER);
#else
    advancedBox->Add(bAdvancedBack, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bAdvancedBack->SetFont(g_font);

    // The TRANSPARENCY button
    bTransparency = new RadarControlButton(this, ID_TRANSPARENCY, _("Transparency"), pPlugIn, CT_TRANSPARENCY, false, pPlugIn->settings.overlay_transparency);
#ifdef __WXOSX__
    advancedBox->Add(bTransparency, 0, wxALL, BORDER);
#else
    advancedBox->Add(bTransparency, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bTransparency->minValue = MIN_OVERLAY_TRANSPARENCY;
    bTransparency->maxValue = MAX_OVERLAY_TRANSPARENCY;

    // The REJECTION button

    interference_rejection_names[0] = _("Off");
    interference_rejection_names[1] = _("Low");
    interference_rejection_names[2] = _("Medium");
    interference_rejection_names[3] = _("High");

    bInterferenceRejection = new RadarControlButton(this, ID_INTERFERENCE_REJECTION, _("Interference rejection"), pPlugIn, CT_INTERFERENCE_REJECTION, false, pPlugIn->settings.interference_rejection);
#ifdef __WXOSX__
    advancedBox->Add(bInterferenceRejection, 0, wxALL, BORDER);
#else
    advancedBox->Add(bInterferenceRejection, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bInterferenceRejection->minValue = 0;
    bInterferenceRejection->maxValue = ARRAY_SIZE(interference_rejection_names) - 1;
    bInterferenceRejection->names = interference_rejection_names;
    bInterferenceRejection->SetValue(pPlugIn->settings.interference_rejection); // redraw after adding names

    // The TARGET BOOST button
    target_boost_names[0] = _("Off");
    target_boost_names[1] = _("Low");
    target_boost_names[2] = _("High");
    bTargetBoost = new RadarControlButton(this, ID_TARGET_BOOST, _("Target boost"), pPlugIn, CT_TARGET_BOOST, false, pPlugIn->settings.target_boost);
#ifdef __WXOSX__
    advancedBox->Add(bTargetBoost, 0, wxALL, BORDER);
#else
    advancedBox->Add(bTargetBoost, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bTargetBoost->minValue = 0;
    bTargetBoost->maxValue = ARRAY_SIZE(target_boost_names) - 1;
    bTargetBoost->names = target_boost_names;
    bTargetBoost->SetValue(pPlugIn->settings.target_boost); // redraw after adding names


    // The NOISE REJECTION button
    noise_rejection_names[0] = _("Off");
    noise_rejection_names[1] = _("Low");
    noise_rejection_names[2] = _("High");

    bNoiseRejection = new RadarControlButton(this, ID_NOISE_REJECTION, _("Noise rejection"), pPlugIn, CT_NOISE_REJECTION, false, pPlugIn->settings.noise_rejection);
#ifdef __WXOSX__
    advancedBox->Add(bNoiseRejection, 0, wxALL, BORDER);
#else
    advancedBox->Add(bNoiseRejection, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bNoiseRejection->minValue = 0;
    bNoiseRejection->maxValue = ARRAY_SIZE(noise_rejection_names) - 1;
    bNoiseRejection->names = noise_rejection_names;
    bNoiseRejection->SetValue(pPlugIn->settings.noise_rejection); // redraw after adding names

    advanced4gBox = new wxBoxSizer(wxVERTICAL);
    advancedBox->Add(advanced4gBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

    // The TARGET SEPARATION button

    target_separation_names[0] = _("Off");
    target_separation_names[1] = _("Low");
    target_separation_names[2] = _("Medium");
    target_separation_names[3] = _("High");

    bTargetSeparation = new RadarControlButton(this, ID_TARGET_SEPARATION, _("Target separation"), pPlugIn, CT_TARGET_SEPARATION, false, pPlugIn->settings.target_separation);
#ifdef __WXOSX__
    advanced4gBox->Add(bTargetSeparation, 0, wxALL, BORDER);
#else
    advanced4gBox->Add(bTargetSeparation, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bTargetSeparation->minValue = 0;
    bTargetSeparation->maxValue = ARRAY_SIZE(target_separation_names) - 1;
    bTargetSeparation->names = target_separation_names;
    bTargetSeparation->SetValue(pPlugIn->settings.target_separation); // redraw after adding names

    // The SCAN SPEED button
    scan_speed_names[0] = _("Normal");
    scan_speed_names[1] = _("Fast");
    bScanSpeed = new RadarControlButton(this, ID_SCAN_SPEED, _("Scan speed"), pPlugIn, CT_SCAN_SPEED, false, pPlugIn->settings.scan_speed);
#ifdef __WXOSX__
    advanced4gBox->Add(bScanSpeed, 0, wxALL, BORDER);
#else
    advanced4gBox->Add(bScanSpeed, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bScanSpeed->minValue = 0;
    bScanSpeed->maxValue = ARRAY_SIZE(scan_speed_names) - 1;
    bScanSpeed->names = scan_speed_names;
    bScanSpeed->SetValue(pPlugIn->settings.scan_speed); // redraw after adding names

    // The DOWNSAMPLE button
    bDownsample = new RadarControlButton(this, ID_DOWNSAMPLE, _("Downsample"), pPlugIn, CT_DOWNSAMPLE, false, pPlugIn->settings.downsampleUser);
#ifdef __WXOSX__
    advancedBox->Add(bDownsample, 0, wxALL, BORDER);
#else
    advancedBox->Add(bDownsample, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bDownsample->minValue = 1;
    bDownsample->maxValue = 8;

    // The REFRESHRATE button
    bRefreshrate = new RadarControlButton(this, ID_REFRESHRATE, _("Refresh rate"), pPlugIn, CT_REFRESHRATE, false, pPlugIn->settings.refreshrate);
#ifdef __WXOSX__
    advancedBox->Add(bRefreshrate, 0, wxALL, BORDER);
#else
    advancedBox->Add(bRefreshrate, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bRefreshrate->minValue = 1;
    bRefreshrate->maxValue = 5;

    // The SCAN AGE button
    bScanAge = new RadarControlButton(this, ID_SCAN_AGE, _("Scan age"), pPlugIn, CT_SCAN_AGE, false, pPlugIn->settings.max_age);
#ifdef __WXOSX__
    advancedBox->Add(bScanAge, 0, wxALL, BORDER);
#else
    advancedBox->Add(bScanAge, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bScanAge->minValue = MIN_AGE;
    bScanAge->maxValue = MAX_AGE;
    // The TIMED TRANSMIT button
    timed_idle_times[0] = _("Off");
    timed_idle_times[1] = _("5 min");
    timed_idle_times[2] = _("10 min");
    timed_idle_times[3] = _("15 min");
    timed_idle_times[4] = _("20 min");
    timed_idle_times[5] = _("25 min");
    timed_idle_times[6] = _("30 min");
    timed_idle_times[7] = _("35 min");

    bTimedIdle = new RadarControlButton(this, ID_TIMED_IDLE, _("Timed Transmit"), pPlugIn, CT_TIMED_IDLE, false, pPlugIn->settings.timed_idle); //HakanToDo new setting
#ifdef __WXOSX__
    advancedBox->Add(bTimedIdle, 0, wxALL, BORDER);
#else
    advancedBox->Add(bTimedIdle, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bTimedIdle->minValue = 0;
    bTimedIdle->maxValue = ARRAY_SIZE(timed_idle_times) - 1;
    bTimedIdle->names = timed_idle_times;
    bTimedIdle->SetValue(pPlugIn->settings.timed_idle); // redraw after adding names

    topSizer->Hide(advancedBox);

    //**************** CONTROL BOX ******************//
    // These are the controls that the users sees when the dialog is started

    // A box sizer to contain RANGE, GAIN etc button
    controlBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(controlBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, BORDER);

    // The RANGE button
    bRange = new RadarRangeControlButton(this, ID_RANGE, _("Range"), pPlugIn);
#ifdef __WXOSX__
    controlBox->Add(bRange, 0, wxALL, BORDER);
#else
    controlBox->Add(bRange, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    // The GAIN button
    bGain = new RadarControlButton(this, ID_GAIN, _("Gain"), pPlugIn, CT_GAIN, true, pPlugIn->settings.gain);
#ifdef __WXOSX__
    controlBox->Add(bGain, 0, wxALL, BORDER);
#else
    controlBox->Add(bGain, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    // The SEA button
    bSea = new RadarControlButton(this, ID_SEA, _("Sea clutter"), pPlugIn, CT_SEA, true, pPlugIn->settings.sea_clutter_gain);
#ifdef __WXOSX__
    controlBox->Add(bSea, 0, wxALL, BORDER);
#else
    controlBox->Add(bSea, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    // The RAIN button
    bRain = new RadarControlButton(this, ID_RAIN, _("Rain clutter"), pPlugIn, CT_RAIN, false, pPlugIn->settings.rain_clutter_gain);
#ifdef __WXOSX__
    controlBox->Add(bRain, 0, wxALL, BORDER);
#else
    controlBox->Add(bRain, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    // The ADVANCED button
    bAdvanced = new wxButton(this, ID_ADVANCED, _("Advanced\ncontrols"), wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    controlBox->Add(bAdvanced, 0, wxALL, BORDER);
#else
    controlBox->Add(bAdvanced, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bAdvanced->SetFont(g_font);

    // The GUARD ZONE 1 button
    wxString label1;
    label1 << _("Guard zone") << wxT(" 1\n");
    bGuard1 = new wxButton(this, ID_ZONE1, label1, wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    controlBox->Add(bGuard1, 0, wxALL, BORDER);
#else
    controlBox->Add(bGuard1, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bGuard1->SetFont(g_font);

    // The GUARD ZONE 2 button
    wxString label2;
    label2 << _("Guard zone") << wxT(" 2\n");
    bGuard2 = new wxButton(this, ID_ZONE2, label2, wxDefaultPosition, g_buttonSize, 0);
#ifdef __WXOSX__
    controlBox->Add(bGuard2, 0, wxALL, BORDER);
#else
    controlBox->Add(bGuard2, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bGuard2->SetFont(g_font);

    // The INFO button
    bMessage = new wxButton(this, ID_MESSAGE, _("Info"), wxDefaultPosition, wxSize(width, 25), 0);
#ifdef __WXOSX__
    controlBox->Add(bMessage, 0, wxALL, BORDER);
#else
    controlBox->Add(bMessage, 0, wxALIGN_CENTER_VERTICAL | wxALL, BORDER);
#endif
    bMessage->SetFont(g_font);
    wantShowMessage = false;

    fromBox = controlBox;
    topSizer->Hide(controlBox);

    UpdateGuardZoneState();

    pPlugIn->UpdateDisplayParameters();
#ifndef __WXOSX__
    DimeWindow(this); // Call OpenCPN to change colours depending on day/night mode
#endif
    Fit();
    //wxSize size_min = GetBestSize();
    //SetMinSize(size_min);
    //SetSize(size_min);
}

void BR24ControlsDialog::UpdateGuardZoneState()
{
    wxString label1, label2;

    wxString GuardZoneNames[] = {
        _("Off"),
        _("Arc"),
        _("Circle")
    };

    label1 << _("Guard zone") << wxT(" 1\n") << GuardZoneNames[pPlugIn->guardZones[0].type];
    bGuard1->SetLabel(label1);

    label2 << _("Guard zone") << wxT(" 2\n") << GuardZoneNames[pPlugIn->guardZones[1].type];
    bGuard2->SetLabel(label2);
}

void BR24ControlsDialog::SetRangeIndex(size_t index)
{
    bRange->SetValueInt(index); // set and recompute the range label
}

void BR24ControlsDialog::SetAutoRangeIndex(size_t index)
{
    bRange->auto_range_index = index;
    bRange->SetValueInt(-1); // recompute the range label
}

void BR24ControlsDialog::SetTimedIdleIndex(int index)
{
    bTimedIdle->SetValue(index) ; // set and recompute the Timed Idle label
    if (pPlugIn->m_pIdleDialog && index == 0) pPlugIn->m_pIdleDialog->Close();
}

void BR24ControlsDialog::OnZone1ButtonClick(wxCommandEvent &event)
{
    pPlugIn->Select_Guard_Zones(0);
}

void BR24ControlsDialog::OnZone2ButtonClick(wxCommandEvent &event)
{
    pPlugIn->Select_Guard_Zones(1);
}

void BR24ControlsDialog::OnClose(wxCloseEvent& event)
{
    pPlugIn->OnBR24ControlDialogClose();
}


void BR24ControlsDialog::OnIdOKClick(wxCommandEvent& event)
{
    pPlugIn->OnBR24ControlDialogClose();
}

void BR24ControlsDialog::OnPlusTenClick(wxCommandEvent& event)
{
    fromControl->SetValue(fromControl->value + 10);
    wxString label = fromControl->GetLabel();

    tValue->SetLabel(label);
}

void BR24ControlsDialog::OnPlusClick(wxCommandEvent& event)
{
    fromControl->SetValue(fromControl->value + 1);

    wxString label = fromControl->GetLabel();

    tValue->SetLabel(label);
}

void BR24ControlsDialog::OnBackClick(wxCommandEvent &event)
{
    extern RadarType br_radar_type;

    topSizer->Hide(editBox);
    topSizer->Show(fromBox);
    if (fromBox == advancedBox) {
        if (br_radar_type == RT_4G) {
            advancedBox->Show(advanced4gBox);
        } else {
            advancedBox->Hide(advanced4gBox);
        }
    }

    topSizer->Layout();
}

void BR24ControlsDialog::OnAutoClick(wxCommandEvent &event)
{
    fromControl->SetAuto();

    OnBackClick(event);
}

void BR24ControlsDialog::OnMinusClick(wxCommandEvent& event)
{
    fromControl->SetValue(fromControl->value - 1);

    wxString label = fromControl->GetLabel();
    tValue->SetLabel(label);
}

void BR24ControlsDialog::OnMinusTenClick(wxCommandEvent& event)
{
    fromControl->SetValue(fromControl->value - 10);

    wxString label = fromControl->GetLabel();
    tValue->SetLabel(label);
}

void BR24ControlsDialog::OnAdvancedBackButtonClick(wxCommandEvent& event)
{
    fromBox = controlBox;
    topSizer->Hide(advancedBox);
    topSizer->Show(controlBox);
    advancedBox->Layout();
    Fit();
    topSizer->Layout();
}

void BR24ControlsDialog::OnAdvancedButtonClick(wxCommandEvent& event)
{
    extern RadarType br_radar_type;

    fromBox = advancedBox;
    topSizer->Show(advancedBox);
    if (br_radar_type == RT_4G) {
        advancedBox->Show(advanced4gBox);
    } else {
        advancedBox->Hide(advanced4gBox);
    }
    advancedBox->Layout();
    topSizer->Hide(controlBox);
    controlBox->Layout();
    Fit();
    topSizer->Layout();
}

void BR24ControlsDialog::OnMessageBackButtonClick(wxCommandEvent& event)
{
    wantShowMessage = false;
    fromBox = messageBox;
    topSizer->Hide(messageBox);
    topSizer->Show(controlBox);
    Fit();
    topSizer->Layout();
}

void BR24ControlsDialog::OnMessageButtonClick(wxCommandEvent& event)
{
    wantShowMessage = true;
    topSizer->Hide(controlBox);
    messageBox->Show(bMsgBack);
    topSizer->Show(messageBox);
    Fit();
    topSizer->Layout();
}

void BR24ControlsDialog::EnterEditMode(RadarControlButton * button)
{
    fromControl = button; // Keep a record of which button was clicked

    tValue->SetLabel(button->GetLabel());
    topSizer->Hide(controlBox);
    topSizer->Hide(advancedBox);
    topSizer->Show(editBox);
    if (fromControl->hasAuto) {
        bAuto->Show();
    }
    else {
        bAuto->Hide();
    }
    if (fromControl->maxValue > 20) {
        bPlusTen->Show();
        bMinusTen->Show();
    }
    else {
        bPlusTen->Hide();
        bMinusTen->Hide();
    }
    editBox->Layout();
    topSizer->Layout();
}


void BR24ControlsDialog::OnRadarControlButtonClick(wxCommandEvent& event)
{
    EnterEditMode((RadarControlButton *) event.GetEventObject());
}



void BR24ControlsDialog::OnMove(wxMoveEvent& event)
{
    //    Record the dialog position
    wxPoint p =  GetPosition();
    pPlugIn->SetBR24ControlsDialogX(p.x);
    pPlugIn->SetBR24ControlsDialogY(p.y);

    event.Skip();
}

void BR24ControlsDialog::OnSize(wxSizeEvent& event)
{
    //    Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetBR24ControlsDialogSizeX(p.GetWidth());
    pPlugIn->SetBR24ControlsDialogSizeY(p.GetHeight());

    event.Skip();
}


void BR24ControlsDialog::UpdateMessage(bool haveOpenGL, bool haveGPS, bool haveHeading, bool haveVariation, bool haveRadar, bool haveData)
{
    cbOpenGL->SetValue(haveOpenGL);
    cbBoatPos->SetValue(haveGPS);
    cbHeading->SetValue(haveHeading);
    cbVariation->SetValue(haveVariation);
    cbRadar->SetValue(haveRadar);
    cbData->SetValue(haveData);

    if (haveOpenGL && haveGPS && haveHeading && haveRadar && haveData) {
        if (topSizer->IsShown(messageBox) && !wantShowMessage)
        {
            topSizer->Hide(messageBox);
            topSizer->Show(controlBox);
            topSizer->Hide(advancedBox);
            topSizer->Hide(editBox);
            Fit();
            topSizer->Layout();
        }
    } else {
        if (!topSizer->IsShown(messageBox)) {
            topSizer->Show(messageBox);
            topSizer->Hide(controlBox);
            topSizer->Hide(advancedBox);
            topSizer->Hide(editBox);
            messageBox->Layout();
            Fit();
            topSizer->Layout();
        }
    }

    editBox->Layout();
    topSizer->Layout();
}

void BR24ControlsDialog::SetErrorMessage(wxString &msg)
{
    tMessage->SetLabel(msg);
    topSizer->Show(messageBox);
    topSizer->Hide(controlBox);
    topSizer->Hide(advancedBox);
    topSizer->Hide(editBox);
    messageBox->Layout();
    Fit();
    topSizer->Layout();
}

void BR24ControlsDialog::SetRadarIPAddress(wxString &msg)
{
    if (cbRadar) {
        wxString label;

        label << _("Radar IP") << wxT(" ") << msg;
        cbRadar->SetLabel(label);
    }
}

void BR24ControlsDialog::SetMcastIPAddress(wxString &msg)
{
    if (ipBox) {
        wxString label;

        label << _("ZeroConf via (wired) Ethernet") << wxT(" ") << msg;
        ipBox->SetLabel(label);
    }
}

void BR24ControlsDialog::SetHeadingInfo(wxString &msg)
{
    if (cbHeading && topSizer->IsShown(messageBox)) {
        wxString label;

        label << _("Heading") << wxT(" ") << msg;
        cbHeading->SetLabel(label);
    }
}

void BR24ControlsDialog::SetVariationInfo(wxString &msg)
{
    if (cbVariation && topSizer->IsShown(messageBox)) {
        wxString label;

        label << _("Variation") << wxT(" ") << msg;
        cbVariation->SetLabel(label);
    }
}

void BR24ControlsDialog::SetRadarInfo(wxString &msg)
{
    if (tStatistics && topSizer->IsShown(messageBox)) {
        tStatistics->SetLabel(msg);
    }
}
