/* *************************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 25/02/2021
*
* Copyright Ⓒ 2021 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#ifndef toolsDialog_h
#define toolsDialog_h

#include "wx/wx.h"
#include "wx/window.h"
#include "toolsDialogGui.h"
#include "trace.h"

class ToolsClass: public ToolsClassBase
{
public:

    void onPageChanged( wxNotebookEvent& event );
    void onAddConsole( wxCommandEvent& event );
    void onRecieveNMEAmessage( wxCommandEvent& event );
    void onRecieveMessage( wxCommandEvent& event );
    void onChangeDirectory( wxCommandEvent& event );
    void onDump( wxCommandEvent& event );
    void onClean( wxCommandEvent& event );
    void onClose( wxCloseEvent& event );
    
//    ToolsClass **pPointerToThisInJavaScript_pi;   // pointer to pointer to this
    
    ToolsClass( wxWindow *parent,  wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ) :ToolsClassBase(parent, id, title, pos, size, style)
    {
        this->resizeDialogue(0); // we open on first page
    }

    void resizeDialogue(int pageNumber)
    {
        wxWindow *page;
        int page_x, page_y;
        page = this->m_notebook->GetPage(pageNumber);
        page->Fit();
        page->GetSize(&page_x, &page_y);
//        TRACE(6, wxString::Format("Dialogue GetSize gave %d x %d", page_x, page_y));
        wxString messg = wxString::Format("Dialogue GetSize gave %d x %d", page_x, page_y);
        wxMessageBox(messg);
        this->SetSize(600, page_y);
    }
};
#endif /* ToolsDialog_h */
