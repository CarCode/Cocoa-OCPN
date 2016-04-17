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

struct PlotColor
{
    wxColor TraceColor[4];
    wxColor GridColor;
    wxColor TextColor;
    wxColor BackgroundColor;
} extern PlotColorSchemes[];

enum PlotStyle { CONTINUOUS, SWEEP };

struct PlotSettings
{
    PlotSettings(PlotColor &c, int totalseconds, PlotStyle s)
    : colors(c), TotalSeconds(totalseconds), style(s) {}

    wxRect rect;
    PlotColor &colors;
    int TotalSeconds;
    enum PlotStyle style;
};

struct TraceSettings
{
    double scale, offset;
    bool resolve;
};

struct Trace
{

    Trace(wxString n, wxCheckBox *cb=NULL) : name(n), CheckBox(cb) {}
    virtual ~Trace() {}

    virtual bool NewData(int TotalSeconds) = 0;
    virtual void Bounds(double &min, double &max, PlotSettings &plotsettings, bool resolve) = 0;
    virtual void Paint(wxDC &dc, PlotSettings &plotsettings, TraceSettings &tracesettings) = 0;
    virtual bool LastValue(double &value) { return false; }

    virtual bool Visible() { return CheckBox->GetValue(); }

    wxString name;
    wxCheckBox *CheckBox;
};

struct HistoryTrace : Trace
{
    HistoryTrace(wxString n, wxCheckBox *cb, enum HistoryEnum i)
    : Trace(n, cb), datai(i) {}

    int HistoryIndex(int TotalSeconds);
    int HistoryIndex(PlotSettings &plotsettings);

    virtual bool NewData(int TotalSeconds);

    virtual void Bounds(double &min, double &max, PlotSettings &plotsettings, bool resolve);
    virtual void Paint(wxDC &dc, PlotSettings &plotsettings, TraceSettings &tracesettings);
    virtual bool LastValue(double &value);
    
    enum HistoryEnum datai;
};

struct HistoryFFTWTrace : HistoryTrace
{
    HistoryFFTWTrace(wxString n, wxCheckBox *cb, enum HistoryEnum i)
        : HistoryTrace(n, cb, i) {}

    virtual void Bounds(double &min, double &max, PlotSettings &plotsettings, bool resolve);
    virtual void Paint(wxDC &dc, PlotSettings &plotsettings, TraceSettings &tracesettings);
};

struct Plot
{
    Plot(wxString n, bool r = false)
        : name(n), resolve(r) {}

    ~Plot();
    bool NewData(int TotalSeconds);
    void Paint(wxDC &dc, PlotSettings &settings);
    bool Visible();

    wxString name;
    bool resolve;
    std::list<Trace*> traces;
};
