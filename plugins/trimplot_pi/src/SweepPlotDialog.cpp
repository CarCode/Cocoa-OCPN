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

#include "sweepplot_pi.h"
#include "SweepPlotDialog.h"
#include "PreferencesDialog.h"

#include "Plot.h"

SweepPlotDialog::SweepPlotDialog(wxWindow* parent, sweepplot_pi &_sweepplot_pi, PreferencesDialog &preferences)
    : SweepPlotDialogBase( parent, wxID_ANY, _("Sweep Plot"), wxDefaultPosition, wxDefaultSize,
                    (preferences.m_cbShowTitleBar->GetValue() ? wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP : 0)
                           |wxRESIZE_BORDER|wxSUNKEN_BORDER|wxWANTS_CHARS),
      m_sweepplot_pi(_sweepplot_pi), m_preferences(preferences),
      m_lastTimerTotalSeconds(0)
{
    m_tRefreshTimer.Connect(wxEVT_TIMER, wxTimerEventHandler
                            ( SweepPlotDialog::OnRefreshTimer ), NULL, this);
    m_tRefreshTimer.Start(1000);

#define PUSH_HISTORY_TRACE(NAME) \
    traces.push_back(new HistoryTrace(_T(#NAME), m_preferences.m_cb##NAME, NAME));

    Plot *speedPlot = new Plot(_("Speed"), false);
    speedPlot->PUSH_HISTORY_TRACE(SOG);
    speedPlot->PUSH_HISTORY_TRACE(PDS10);
    speedPlot->PUSH_HISTORY_TRACE(PDS60);
    m_plots.push_back(speedPlot);

    Plot *coursePlot = new Plot(_("Course"), true);
    coursePlot->PUSH_HISTORY_TRACE(COG);
    coursePlot->PUSH_HISTORY_TRACE(PDC10);
    coursePlot->PUSH_HISTORY_TRACE(PDC60);
    coursePlot->PUSH_HISTORY_TRACE(HDG);
    m_plots.push_back(coursePlot);

    Plot *courseFFTWPlot = new Plot(_("Course FFTW"), false);
    courseFFTWPlot->traces.push_back
        (new HistoryFFTWTrace(_("Course FFTW"), m_preferences.m_cbCourseFFTWPlot, COG));
    m_plots.push_back(courseFFTWPlot);
}

SweepPlotDialog::~SweepPlotDialog()
{
    for(std::list<Plot*>::iterator it=m_plots.begin(); it != m_plots.end(); it++)
        delete *it;
}

void SweepPlotDialog::Relay( wxKeyEvent& event )
{
    GetOCPNCanvasWindow()->GetEventHandler()->AddPendingEvent( event );
}

void SweepPlotDialog::OnDoubleClick( wxMouseEvent& event )
{
    m_sweepplot_pi.m_Preferences->Show();
}

void SweepPlotDialog::OnPaint( wxPaintEvent& event )
{
    wxWindow *window = dynamic_cast<wxWindow*>(event.GetEventObject());
    if(!window)
        return;

    wxPaintDC dc( window );
    dc.SetFont(m_preferences.m_fpPlotFont->GetSelectedFont());
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    PlotSettings settings(PlotColorSchemes[m_preferences.m_cColors->GetSelection()],
                          TotalSeconds(), (PlotStyle)m_preferences.m_cPlotStyle->GetSelection());

    window->SetBackgroundColour(settings.colors.BackgroundColor);

    if(PlotCount() == 0) {
        dc.DrawText(_("No Plots Enabled"), 0, 0);
        return;
    }

    int PlotHeight = wxMax(m_preferences.PlotMinHeight(),
                           window->GetSize().y / PlotCount());

    int plotcount = 0;
    for(std::list<Plot*>::iterator it=m_plots.begin(); it != m_plots.end(); it++) {
        if(!(*it)->Visible())
            continue;

        settings.rect = wxRect(0, plotcount++ * PlotHeight,
                               window->GetSize().x, PlotHeight);
        (*it)->Paint(dc, settings);
    }

    // Print period in lower right
    dc.SetTextForeground(settings.colors.TextColor);
    wxString period;
    int t = TotalSeconds();
    if(t < 60)
        period = _T("s");
    else {
        t /= 60;
        if(t < 60)
            period = _T("m");
        else {
            t /= 60;
            if(t < 24)
                period = _T("h");
            else {
                t /= 24;
                period = _T("d");
            }
        }
    }

    period = wxString::Format(_T("%d "), t) + period;
    int w, h;
    dc.GetTextExtent(period, &w, &h);
    dc.DrawText(period, window->GetSize().x - w, window->GetSize().y - h);
}

void SweepPlotDialog::SetupPlot()
{
    int count = wxMax(PlotCount(), 1); // even with no plots, make size of one plot
    int minh = m_preferences.PlotMinHeight() * count;

    int w, h;
    m_swPlots->GetSize(&w, &h);
    m_swPlots->SetMinSize(wxSize(10, minh));

    if(h < minh) {
        // hack needed to make scrollbar appear
        wxSize s = m_sweepplot_pi.m_SweepPlotDialog->GetSize();
        SetSize(s.x+1, s.y);
        SetSize(s.x, s.y);
    }

    SetTransparent(255 - 255*m_preferences.m_sPlotTransparency->GetValue()/100);
}

void SweepPlotDialog::OnConfiguration( wxCommandEvent& event )
{
    m_preferences.Show();
}

void SweepPlotDialog::OnRefreshTimer( wxTimerEvent & )
{
    if(m_lastTimerTotalSeconds != TotalSeconds())
        Refresh();
    else
    for(std::list<Plot*>::iterator it=m_plots.begin(); it != m_plots.end(); it++)
        if((*it)->Visible() && (*it)->NewData(TotalSeconds())) {
            Refresh();
            break;
        }

    m_lastTimerTotalSeconds = TotalSeconds();
}

int SweepPlotDialog::PlotCount()
{
    int count = 0;

    for(std::list<Plot*>::iterator it=m_plots.begin(); it != m_plots.end(); it++)
        count += (*it)->Visible();

    return count;
}

int SweepPlotDialog::TotalSeconds()
{
    const int h = 60, d = 24*h;
    const int cts[] = {5, 20, h, 4*h, 8*h, d, 3*d, 10*d, 30*d, 60*d};
    wxMenuItem *items[] = {m_mt1, m_mt2, m_mt3, m_mt4, m_mt5, m_mt6, m_mt7, m_mt8, m_mt9, m_mt10};

    for(unsigned int i=0; i<sizeof cts / sizeof *cts; i++)
        if(items[i]->IsChecked())
            return 60*cts[i];

    return 60;
}
