/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  weather fax Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Sean D'Epagnier                                 *
 *   sean at depagnier dot com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#include <vector>

#include "WeatherFaxUI.h"


class FaxArea
{
public:
    wxString name, description;
    double lat1, lat2, lon1, lon2;

    FaxArea() : lat1(NAN), lat2(NAN), lon1(NAN), lon2(NAN) {}

    bool ContainsLat(double lat) { return isnan(lat) || (lat >= lat1 && lat <= lat2); }
    bool ContainsLon(double lon) { return isnan(lon) ||
            (lon2 - lon1 < 180 && lon >= lon1 && lon <= lon2) ||
            (lon2 - lon1 >= 180 && (lon <= lon1 || lon >= lon2)); }

    wxString LatArea(double lat) { return wxString::Format(_T("%.0f"), fabs(lat))
            + ((lat >= 0) ? _T("N") : _T("S")); }
    wxString LonArea(double lon) { return wxString::Format(_T("%.0f"), fabs(lon))
            + ((lon >= 0) ? _T("E") : _T("W")); }
    wxString AreaDescription() {
        return description +
            ((!isnan(lat1) && !isnan(lat2)) ?
            _T(" ") + LatArea(lat1) + _T("-") + LatArea(lat2) : _T("")) +
            ((!isnan(lon1) && !isnan(lon2)) ?
            _T(" ") + LonArea(lon1) + _T("-") + LonArea(lon2) : _T(""));
    }
};

#include "SchedulesDialog.h"
#include "InternetRetrievalDialog.h"

class weatherfax_pi;

class WeatherFax: public WeatherFaxBase
{
public:
    WeatherFax( weatherfax_pi &_weatherfax_pi, wxWindow* parent);

    ~WeatherFax();

    void EnableDisplayControls(bool enable);
    void OnClose( wxCloseEvent& event ) { Hide(); }
    void OnFaxes( wxCommandEvent& event );
    void OnFaxesToggled( wxCommandEvent& event );
    void TransparencyChanged( wxScrollEvent& event );
    void WhiteTransparencyChanged( wxScrollEvent& event );
    void OnInvert( wxCommandEvent& event );
    void OnOpen( wxCommandEvent& event );
    void OnEdit( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnExport( wxCommandEvent& event );
    void OnPreferences( wxCommandEvent& event ) { m_weatherfax_pi.ShowPreferencesDialog( this ); }
    void OnClose( wxCommandEvent& event ) { Show(false); }
    void OnCapture( wxCommandEvent& event );
    void OnSchedules( wxCommandEvent& event );
    void OnInternet( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );

    void OpenWav(wxString filename, wxString station=_T(""), wxString area=_T(""));
    void OpenImage(wxString filename);
    void Export(wxString filename);

    void UpdateMenuStates();

    WeatherFaxImageCoordinateList m_Coords;

    std::vector<WeatherFaxImage*>m_Faxes;

protected:
    SchedulesDialog m_SchedulesDialog;
    InternetRetrievalDialog m_InternetRetrievalDialog;

    weatherfax_pi &m_weatherfax_pi;
};

class FaxDecoder;
class DecoderThread : public wxThread
{
public:
    DecoderThread(FaxDecoder &decoder)
        : wxThread(wxTHREAD_JOINABLE), m_decoder(decoder) { Create(); }
    void *Entry();
private:
    FaxDecoder &m_decoder;
};
