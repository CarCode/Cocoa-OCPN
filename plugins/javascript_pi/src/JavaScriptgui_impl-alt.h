/* ***********************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright Ⓒ 2020 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#ifndef _CALCULATORGUI_IMPL_H_
#define _CALCULATORGUI_IMPL_H_

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include <wx/stc/stc.h>

using namespace std;

class JavaScript_pi;

class Console : public m_Console
{
public:
    Console(wxWindow *parent, JavaScript_pi *ppi);
    ~Console();

    wxWindow *pParent;
    JavaScript_pi *pPlugIn;
    wxString filePath;  // of active file
    wxString folderPath; //  of current folder
    wxFileName currentFile;
    	
    void OnClearScript( wxCommandEvent& event );
    void OnCopyAll( wxCommandEvent& event );
    void OnLoad( wxCommandEvent& event );
    void OnSave (wxCommandEvent& event);
    void OnSaveAs ( wxCommandEvent& event );
    void OnRun( wxCommandEvent& event );
    void OnClearOutput( wxCommandEvent& event );
    void OnTestA( wxCommandEvent& event );
    void OnTestB( wxCommandEvent& event );


private:
    void OnClose( wxCloseEvent& event );
    bool error_found;
    bool dbg;

public:
    void setRunButton(wxString label){
        run_button->SetLabel(label);
        return;
    }


};
#endif
