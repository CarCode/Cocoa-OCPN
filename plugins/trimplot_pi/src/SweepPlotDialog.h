/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  sweepplot Plugin
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

#include "SweepPlotUI.h"

class sweepplot_pi;
#ifdef __WXOSX__
struct Plot;
#else
class Plot;
#endif

class SweepPlotDialog: public SweepPlotDialogBase
{
public:
    SweepPlotDialog(wxWindow* parent, sweepplot_pi &_sweepplot_pi, PreferencesDialog &preferences);
    ~SweepPlotDialog();

    void Relay( wxKeyEvent& event );
    void OnSize(wxSizeEvent& event) { Refresh(); event.Skip(); }
    void OnDoubleClick( wxMouseEvent& event );
    void OnPaint(wxPaintEvent& event);

    void SetupPlot();

private:
    void OnTimeChoice( wxCommandEvent& event ) { Refresh(); }
    void OnConfiguration( wxCommandEvent& event );

    void OnRefreshTimer( wxTimerEvent & );

    int PlotCount();
    int TotalSeconds();

    sweepplot_pi &m_sweepplot_pi;
    PreferencesDialog &m_preferences;

    wxTimer m_tRefreshTimer;
    int m_lastTimerTotalSeconds;

    std::list<Plot*> m_plots;
};
