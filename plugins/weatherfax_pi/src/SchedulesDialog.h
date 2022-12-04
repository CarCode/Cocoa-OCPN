/**************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  weather fax Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2015 by Sean D'Epagnier                                 *
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

static const int seconds_in_a_day = 24*60*60;

class Schedule
{
public:
    bool Filtered, Capture;

    wxString Station;
    double Frequency;
    int Time;
    wxString Contents;
    int ValidTime;
    wxString area_name;

    int Duration;
    FaxArea Area;

    long StartSeconds() {
        wxDateTime t(Time/100, Time%100);
        long seconds = (t - wxDateTime::Now().ToUTC()).GetSeconds().ToLong()%seconds_in_a_day;
        if(seconds < 0) /* already happened today */
            seconds += seconds_in_a_day; /* try next day */
        return seconds;
    }

    long Seconds() {
        wxDateTime t(Time/100, Time%100);
        long seconds = (wxDateTime::Now().ToUTC() - t).GetSeconds().ToLong()%seconds_in_a_day;
        if(seconds < 0) /* already happened today */
            seconds += seconds_in_a_day; /* try next day */
        return seconds;
    }
};

class WeatherFaxWizard;
class SchedulesDialog: public SchedulesDialogBase
{
public:
    enum {CAPTURE, STATION, FREQUENCY, TIME, CONTENTS, VALID_TIME, DURATION, MAP_AREA};

    SchedulesDialog( weatherfax_pi &_weatherfax_pi, wxWindow* parent);
    ~SchedulesDialog();
#ifdef __WXOSX__
    void Load();
#else
    void Load(bool force = false);
#endif
    void ClearSchedules();
    bool OpenXML(wxString filename);

    void OnSchedulesLeftDown( wxMouseEvent& event );
    void OnSchedulesSort( wxListEvent& event );
    void OnFilter( wxCommandEvent& event ) { Filter(); }
    void OnFilterSpin( wxSpinEvent& event ) { Filter(); }
    void OnBoatPosition( wxCommandEvent& event );
    void OnReset( wxCommandEvent& event );
    void OnAllStations( wxCommandEvent& event );
    void OnNoStations( wxCommandEvent& event );
    void OnAllFrequencies( wxCommandEvent& event );
    void OnClearCaptures( wxCommandEvent& event );
    void OnClose( wxCommandEvent& event );

    bool HasStation(wxString station);
    void Filter();
    void RebuildList();
    void UpdateItem(long index);
    void AddScheduleToCapture(Schedule *s);
    void RemoveScheduleToCapture(Schedule *s);
    void UpdateTimer();
    void UpdateProgress();

    WeatherFaxWizard *m_CaptureWizard;

private:
    void OnAlarmTimer( wxTimerEvent & );
    void OnCaptureTimer( wxTimerEvent & );
    void OnEndCaptureTimer( wxTimerEvent & );
    void OnProgressTimer( wxTimerEvent & );

    void OnTerminate(wxProcessEvent& event);

    void StopExternalProcess();

    weatherfax_pi &m_weatherfax_pi;

    std::list<Schedule*> m_Schedules;
    std::list<Schedule*> m_CaptureSchedules;

    wxTimer m_AlarmTimer, m_CaptureTimer, m_EndCaptureTimer, m_ProgressTimer;
    wxString m_ExternalCaptureFilename;
    wxProcess *m_ExternalCaptureProcess;

    Schedule *m_CurrentSchedule;

    bool m_bLoaded, m_bDisableFilter;
    bool m_bKilled;

    bool m_bRebuilding;
};
