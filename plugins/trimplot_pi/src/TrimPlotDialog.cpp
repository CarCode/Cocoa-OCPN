/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  trimplot Plugin
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
 ***************************************************************************
 */

#include "trimplot_pi.h"
#include "TrimPlotDialog.h"
#include "PreferencesDialog.h"

TrimPlotDialog::TrimPlotDialog(wxWindow* parent, trimplot_pi &_trimplot_pi)
    : TrimPlotDialogBase( parent ), m_trimplot_pi(_trimplot_pi)
{
}

TrimPlotDialog::~TrimPlotDialog()
{
}

void TrimPlotDialog::OnDoubleClick( wxMouseEvent& event )
{
    m_trimplot_pi.m_Preferences->Show();
}

void TrimPlotDialog::OnPaint( wxPaintEvent& event )
{
    wxWindow *window = dynamic_cast<wxWindow*>(event.GetEventObject());
    if(!window)
        return;

    wxPaintDC dc( window );
    dc.SetBrush(wxBrush(*wxBLACK));
    dc.SetPen(wxPen( *wxBLACK, 1 ));

    int w, h;
    window->GetSize(&w, &h);

    double scale = 0;
    if(window == m_swSpeed)
        m_trimplot_pi.m_Preferences->m_tSpeedScale->GetValue().ToDouble(&scale);
    else if(window == m_swCourse)
        m_trimplot_pi.m_Preferences->m_tCourseScale->GetValue().ToDouble(&scale);

    int x = 0;
    double cur = NAN;
    double u = 0;
    for(std::list<PlugIn_Position_Fix_Ex>::iterator it = m_trimplot_pi.m_fixes.begin();
        it != m_trimplot_pi.m_fixes.end(); it++) {
        double val = NAN;
        if(window == m_swSpeed)
            val = (*it).Sog;
        else if(window == m_swCourse)
            val = (*it).Cog;

        if(!isnan(val)) {
            if(isnan(cur))
                cur = val;
            double v = val-cur;
            if(window == m_swCourse)
                v = heading_resolve(v);
            v = h*v/scale;

            if(x > 0)
                dc.DrawLine(w-x+1, h/2-u, w-x, h/2-v);
            u = v;
        }

        if(++x > w)
            break;
    }
}

void TrimPlotDialog::RepopulatePlots(bool speed, bool course, bool prediction)
{
    m_swSpeed->Show(speed);
    m_stSpeed->Show(speed);
    m_swCourse->Show(course);
    m_stCourse->Show(course);
    m_stPositionSpeed->Show(prediction);
    m_stSpeedPercentage->Show(prediction);

    wxSize size = GetSize();
    Fit();
    SetSize(size);
    Refresh();
}
