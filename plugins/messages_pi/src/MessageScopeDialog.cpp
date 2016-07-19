/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Message Scope Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2016 by Sean D'Epagnier                                 *
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

#include "messagescope_pi.h"
#include "MessageScopeDialog.h"

enum { MESSAGE_ID, COUNT, TIME, RATE };

class Message
{
public:
    Message(wxString body) : time(wxDateTime::Now()), message_body(body)
        {
        }

    wxDateTime time;
    wxString message_body;
};

MessageScopeDialog::MessageScopeDialog( messagescope_pi &_messagescope_pi, wxWindow* parent)
    : MessageScopeDialogBase( parent ), m_messagescope_pi(_messagescope_pi)
{

    m_lMessages->InsertColumn(MESSAGE_ID, _("Message ID"));
    m_lMessages->InsertColumn(COUNT, _("Count"));
    m_lMessages->InsertColumn(TIME, _("Time"));
    m_lMessages->InsertColumn(RATE, _("Rate"));
}

void MessageScopeDialog::OnMessageSelected( wxListEvent& event )
{
    long index = m_lMessages->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(index == -1)
        m_stMessage->SetLabel(_("No Message Selected"));
    else {
        wxString message_body =
            reinterpret_cast<Message*>(wxUIntToPtr(m_lMessages->GetItemData(index)))->message_body;
        m_stMessage->SetLabel(message_body);
        m_stMessage->Fit();
    }
}

static wxString StringCurrentTime()
{
    return wxDateTime::Now().FormatTime();
}

void MessageScopeDialog::SetPluginMessage(wxString &message_id, wxString &message_body)
{
//    wxJSONReader r;
//    wxJSONValue v;
//    r.Parse(message_body, &v);
    long index = m_lMessages->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    for(int i=0; i<m_lMessages->GetItemCount(); i++)
        if(message_id == m_lMessages->GetItemText(i, MESSAGE_ID)) {
            // increment count
            long count = 0;
            m_lMessages->GetItemText(i, COUNT).ToLong(&count);
            count++;
            m_lMessages->SetItem(i, COUNT, wxString::Format(_T("%ld"), count));

            Message *msg =
                reinterpret_cast<Message*>(wxUIntToPtr(m_lMessages->GetItemData(i)));
            wxTimeSpan period = wxDateTime::Now() - msg->time;

            m_lMessages->SetItem(i, RATE, wxString::Format(_T("%.1f Hz"), count*1000.0/period.GetMilliseconds().ToLong()));

            msg->message_body = message_body; // update message

            if(i == index)
                m_stMessage->SetLabel(message_body);
            return;
        }

    // not in list, add it
    wxListItem item;
    index = m_lMessages->InsertItem(m_lMessages->GetItemCount(), item);
    m_lMessages->SetItem(index, MESSAGE_ID, message_id);
    m_lMessages->SetColumnWidth(MESSAGE_ID, wxLIST_AUTOSIZE);
    m_lMessages->SetItem(index, COUNT, _T("1"));
    m_lMessages->SetItem(index, TIME, StringCurrentTime());
    m_lMessages->SetItem(index, RATE, wxEmptyString);

    m_lMessages->SetItemPtrData(index, wxPtrToUInt(new Message(message_body)));
}
