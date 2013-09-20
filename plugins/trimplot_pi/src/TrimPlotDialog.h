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

#include "TrimPlotUI.h"

class trimplot_pi;

class TrimPlotDialog: public TrimPlotDialogBase
{
public:
    TrimPlotDialog(wxWindow* parent, trimplot_pi &_trimplot_pi);
    ~TrimPlotDialog();

    void OnSize(wxSizeEvent& event) { Refresh(); event.Skip(); }
    void OnDoubleClick( wxMouseEvent& event );
    void OnPaint(wxPaintEvent& event);

    void RepopulatePlots(bool speed, bool course, bool prediction);

private:
    trimplot_pi &m_trimplot_pi;
};
