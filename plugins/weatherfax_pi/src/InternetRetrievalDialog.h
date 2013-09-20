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

#include <list>
#include <vector>

#include "wx/process.h"

#include "WeatherFaxUI.h"

class weatherfax_pi;

class FaxUrl
{
public:
    bool Filtered;
    bool Selected;

    wxString Server;
    wxString Url;
    wxString Contents;
    wxString area_name;

    FaxArea Area;
};

class InternetRetrievalDialog: public InternetRetrievalDialogBase
{
public:
    enum {SELECTED, SERVER, CONTENTS, MAP_AREA};

    InternetRetrievalDialog( weatherfax_pi &_weatherfax_pi, wxWindow* parent);
    ~InternetRetrievalDialog();

    void Load();
    void ClearInternetRetrieval();
    bool OpenXML(wxString filename);

    void OnUrlsLeftDown( wxMouseEvent& event );
    void OnUrlsSort( wxListEvent& event );
    void OnFilter( wxCommandEvent& event ) { Filter(); }
    void OnBoatPosition( wxCommandEvent& event );
    void OnReset( wxCommandEvent& event );
    void OnAllServers( wxCommandEvent& event );
    void OnNoServers( wxCommandEvent& event );
    void OnRetrieve( wxCommandEvent& event );
    void OnClose( wxCommandEvent& event );

    bool HasServer(wxString server);
    void Filter();
    void RebuildList();
    void UpdateItem(long index);

private:

    void StopExternalProcess();

    weatherfax_pi &m_weatherfax_pi;

    std::list<FaxUrl*> m_InternetRetrieval;
    std::list<FaxUrl*> m_CaptureInternetRetrieval;

    bool m_bLoaded, m_bDisableFilter;
    bool m_bKilled;

    bool m_bRebuilding;
};
