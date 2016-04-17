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

#include <list>

enum HistoryEnum {TWS, TWD, TWA, AWS, AWA, SOG, COG, HDG, XTE, LAT, LON,
                  PDS10, PDS60, PDC10, PDC60, HISTORY_COUNT};

// three buffers, one for current data, and one
// with entrees averaged to each minute, and hour
#define HISTORY_BUCKETS 3

struct HistoryAtom
{
    HistoryAtom() {}
    HistoryAtom(double v, time_t t) : value(v), ticks(t) {}
    double value;
    time_t ticks;
};

struct HistoryData
{
    std::list<HistoryAtom> data;
    bool newdata;
};

struct History
{
    HistoryData data[HISTORY_BUCKETS];
    bool resolve;

    bool LastValue(double &value) { int dummy = 0; return LastValue(value, dummy); }
    bool LastValue(double &value, int &tick_diff);
    time_t LastTicks();
    void AddData(int i, HistoryAtom state);
    void AddData(double value, time_t ticks, bool resolve);

    static void Read(wxString filename);
    static void Write(wxString filename);
    static int Divisor(int i);
    static int Depth(int i);
};

extern History g_history[];
