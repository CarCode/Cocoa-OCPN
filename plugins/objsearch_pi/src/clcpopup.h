/* **************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Vector Chart Object Search Plugin
 * Author:   Pavel Kalian
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Pavel Kalian                                    *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#ifndef _CLCPOPUP_H_
#define _CLCPOPUP_H_

#include <wx/wxprec.h>

#include <wx/checklst.h>
#include <wx/combo.h>


class CheckListComboPopup : public wxCheckListBox, public wxComboPopup
{
public:

    virtual void Init() {}
    virtual bool Create( wxWindow* parent )
    {
        return wxCheckListBox::Create( parent, wxID_ANY, wxPoint(0,0));
    }
    
    virtual void OnShow() {}

    virtual wxSize GetAdjustedSize( int minWidth,
                                    int WXUNUSED(prefHeight),
                                    int maxHeight )
    {
        return wxSize(wxMax(300,minWidth),wxMin(250,maxHeight));
    }

    virtual wxWindow *GetControl() { return this; }
    
    virtual wxString GetStringValue() const;
    
    int Append(const wxString& item, const wxString& value);
    
    void Clear();
    
    //
    // Popup event handlers
    //
    
    void OnListBox(wxCommandEvent& event);
    
    void CheckAll(bool check = true);
    
private:
    wxArrayString m_values;
    DECLARE_EVENT_TABLE()
};
#endif
