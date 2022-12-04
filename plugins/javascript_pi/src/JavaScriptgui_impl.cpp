/* *************************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright Ⓒ 2021 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#include <iostream>
#include "JavaScript_pi.h"
#include "JavaScriptgui_impl.h"
#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include "ocpn_plugin.h"
#include "trace.h"
#include <wx/event.h>


#define FAIL(X) do { error = X; goto failed; } while(0)

extern JavaScript_pi *pJavaScript_pi;

int messageComp(MessagePair** arg1, MessagePair** arg2) {   // used when sorting messages
    return (strcmp((*arg1)->messageName, (*arg2)->messageName));}

WX_DEFINE_OBJARRAY(MessagesArray);
WX_DEFINE_OBJARRAY(TimesArray);
WX_DEFINE_OBJARRAY(ConsoleCallbackArray);
// WX_DEFINE_OBJARRAY(DialogActionsArray);

// wxString stopLabel="Stop";
// wxString runLabel="Run";

void Console::OnActivate(wxActivateEvent& event){
    wxDialog* pConsole = wxDynamicCast(event.GetEventObject(), wxDialog);
    long int style = pConsole->GetWindowStyle();
    if (event.GetActive()) pConsole->SetWindowStyle(style | wxSTAY_ON_TOP); // bring console on top
    pConsole->SetWindowStyle(style ^ wxSTAY_ON_TOP);    // but do not force to stay there
    };


void Console::OnLoad( wxCommandEvent& event )
{
    int response = wxID_CANCEL;
 //   wxArrayString file_array;
    wxString filename;
    wxTextFile ourFile;
    wxString lineOfData, script;
    wxDialog query;
    wxString JScleanString(wxString line);
    
    wxFileDialog openConsole( this, _( "File to load" ), wxT ( "" ), wxT ( "" ),
                             wxT ( "*.js" ),
                             wxFD_OPEN);
    response = openConsole.ShowModal();
    if( response == wxID_OK ) {
        mFileString = openConsole.GetPath();
        ourFile.Open(mFileString);
        m_Script->ClearAll();   // clear old content
        for (lineOfData = ourFile.GetFirstLine(); !ourFile.Eof(); lineOfData = ourFile.GetNextLine()){
            script += lineOfData + "\n";
            }
        script = JScleanString(script);
        m_Script->AppendText(script);
        m_fileStringBox->SetValue(wxString(mFileString));
        auto_run->Show();
        auto_run->SetValue(false);
        return;
    }
    else if(response == wxID_CANCEL){
//        TRACE(3, "Load cancelled");
        return;
    }
}

void Console::OnSaveAs( wxCommandEvent& event )
{
    int response = wxID_CANCEL;
    wxArrayString file_array;
    wxString filename, filePath;
    wxTextFile ourFile;
    wxString lineOfData;
    wxDialog query;
    
    wxFileDialog SaveAsConsole( this, _( "Saving your script" ), wxEmptyString, wxEmptyString,
                               _T("js files (*.js)|*.js|All files (*.*)|*.*"),
                               wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxDD_NEW_DIR_BUTTON);
    response = SaveAsConsole.ShowModal();
    if( response == wxID_OK ) {
        filePath = SaveAsConsole.GetPath();
/*
        if (!filePath.EndsWith(".js")){
            filePath += ".js";
            message(STYLE_ORANGE, "OnSaveAs - file name must end with .js - not saved");
            return;
        }
 */
        if (!filePath.EndsWith(".js")) filePath += ".js";
        m_Script->SaveFile(filePath, wxTEXT_TYPE_ANY);
        m_fileStringBox->SetValue(wxString(filePath));
        auto_run->Show();
        return;
    }
    else if(response == wxID_CANCEL){
        // cout  << "Save cancelled\n";
        return;
    }
}

void Console::OnSave( wxCommandEvent& event )
{
    wxArrayString file_array;
    wxString filename;
    wxTextFile ourFile;
    wxString lineOfData;
    wxDialog query;
    
    mFileString = m_fileStringBox->GetValue();
    if ((   mFileString != "") & wxFileExists(   mFileString))
    {  // Have a 'current' file, so can just save to it
        m_Script->SaveFile(mFileString);
//        TRACE(3, wxString::Format("Saved to  %s",mFileString));
        auto_run->Show();
        return;
    }
    else OnSaveAs(event);  // No previous save, so do Save As
    return;
}

void Console::OnCopyAll(wxCommandEvent& event) {
    int currentPosition = m_Script->GetCurrentPos();
    m_Script->SelectAll();
    m_Script->Copy();
    m_Script->GotoPos(currentPosition);
}

void Console::OnClearScript( wxCommandEvent& event ){
    m_Script->ClearAll();
    m_Script->SetFocus();
    m_fileStringBox->SetValue(wxEmptyString);
    mFileString = wxEmptyString;
    auto_run->SetValue(false);
    auto_run->Hide();
}

void Console::OnClearOutput( wxCommandEvent& event ){
    m_Output->ClearAll();
}

void Console::OnRun( wxCommandEvent& event )
{
#if TRACE_LEVEL > 0
    extern JavaScript_pi *pJavaScript_pi;
    if (!pJavaScript_pi->mTraceLevelStated)
        message(STYLE_ORANGE, wxString::Format("Trace level is %d",TRACE_LEVEL));
    pJavaScript_pi->mTraceLevelStated = true;
#endif
    clearBrief();
    mConsoleCallbacksAwaited = 0;
    doRunCommand(mBrief);
}

void Console::OnAutoRun(wxCommandEvent& event){   // Auto run tick box
    if (this->auto_run->GetValue()){
        // box has been ticked
        mFileString  = m_fileStringBox->GetValue();
        if ((   mFileString != "") & wxFileExists(   mFileString /*FilePath*/)){
            // Have a 'current' file we can auto-run
            this->m_autoRun = true;
            }
        else {
            this->message(STYLE_RED, wxString("No script loaded to auto-run"));
            this->auto_run->SetValue(false);
            this->m_autoRun = false;
            }
        }
    else this->m_autoRun = false;
    }

void Console::OnClose(wxCloseEvent& event)
{
    extern JavaScript_pi *pJavaScript_pi;
//    TRACE(1, "Closing console " + this->mConsoleName + " Can veto is " + (event.CanVeto()?"true":"false"));

    if (event.CanVeto()){
        if ((this == pJavaScript_pi->mpFirstConsole) && (this->mpNextConsole == nullptr)) {
            // This is only console - decline
            this->message(STYLE_RED, "Console close: You cannot close the only console");
            event.Veto(true);
            return;
            }
        if (!this->m_Script->IsEmpty()) {
            // We will not delete a console with a script
            this->message(STYLE_RED, "Console close: clear the script first");
            event.Veto(true);
            return;
            }
//        TRACE(3, "Binning console " + this->mConsoleName);
        this->bin();
//        this->clearAndUnhook();
//        this->Destroy();
        return;
        }
//    TRACE(3, "Destroying console " + this->mConsoleName);
    this->Destroy();
    //RequestRefresh(pJavaScript_pi->m_parent_window);
 }

static wxString dummyMessage, message_id;
wxDialog* dialog;

void Console::OnTools( wxCommandEvent& event){
//void createDialog();
    extern JavaScript_pi *pJavaScript_pi;
    
    pJavaScript_pi->ShowPreferencesDialog(pJavaScript_pi->m_parent_window);
    return;
    }
