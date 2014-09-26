/**************************************************************************
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
 
 #include "clcpopup.h"

BEGIN_EVENT_TABLE( CheckListComboPopup, wxCheckListBox )
    EVT_CHECKLISTBOX( wxID_ANY, CheckListComboPopup::OnListBox )
END_EVENT_TABLE()

wxString CheckListComboPopup::GetStringValue() const
{
    bool add_comma = false;
    wxString ret = wxEmptyString;
    for ( unsigned int i = 1; i < GetCount(); i++ )
        if ( IsChecked(i) )
        {
            if ( add_comma )
                ret += _T(",");
            else
                add_comma = true;
            ret += (m_values.Item(i - 1));
        }

    return ret;
}

void CheckListComboPopup::CheckAll(bool check)
{
    for ( unsigned int i = 1; i < GetCount(); i++ )
        Check(i, check);
}

void CheckListComboPopup::OnListBox(wxCommandEvent& event)
{
    int item_id = event.GetInt();
    if (item_id == 0)
    {
        if (IsChecked(item_id))
        {
            CheckAll();
        }
        else
        {
            CheckAll(false);
        }
    }
    else
    {
        Check(0, false);
    }
}

int CheckListComboPopup::Append(const wxString& item, const wxString& value)
{
    int r = wxCheckListBox::Append(item);
    m_values.Add(value);
    return r;
}

void CheckListComboPopup::Clear()
{
    wxCheckListBox::Clear();
    m_values.Clear();
}
