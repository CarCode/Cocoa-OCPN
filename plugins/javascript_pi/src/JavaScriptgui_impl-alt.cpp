/* ***********************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss
*
* Copyright Ⓒ 2020 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include "icons.h"
#include <wx/progdlg.h>
#include <wx/wx.h>
#include "wx/dir.h"
#include <wx/arrimpl.cpp>
#include <string>
#include "stdio.h"
#include <ostream>
#include "ocpn_plugin.h"
#include "wx/textfile.h"
#include "duktape.h"
#include "ocpn_duk.h"

#define FAIL(X) do { error = X; goto failed; } while(0)

bool compileJS(wxString script, Console* console);
WX_DEFINE_OBJARRAY(MessagesArray);
WX_DEFINE_OBJARRAY(TimesArray);
extern JS_control_class JS_control;

wxString stopLabel="Stop";
wxString runLabel="Run";

Console::Console(wxWindow *parent, JavaScript_pi *ppi)
: m_Console(parent)
{
    this->Fit();
    dbg = false; //for debug output set to true

    pPlugIn = ppi;
    pParent = parent;

    wxString blank_name = *GetpSharedDataLocation()
    + _T("plugins/JavaScript_pi/data/blank.ico");

#ifndef IN_HARNESS
    wxIcon icon(blank_name, wxBITMAP_TYPE_ICO);
    SetIcon(icon);
#endif
}

Console::~Console()
{

}

void Console::OnLoad( wxCommandEvent& event )
{
    int response = wxID_CANCEL;
    wxArrayString file_array;
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
        openConsole.GetPaths( file_array );
        //    Record the currently selected directory for later use
        if( file_array.GetCount() ) {
            wxFileName fn( file_array[0] );
            filePath = file_array[0];
            folderPath = fn.GetPath();
            wxFileName currentFile(filePath);
            currentFile.AssignDir(folderPath);
        }
        ourFile.Open(filePath);
        m_Script->ClearAll();   // clear old content
        for (lineOfData = ourFile.GetFirstLine(); !ourFile.Eof(); lineOfData = ourFile.GetNextLine()){
            script += lineOfData + "\n";
            }
        script = JScleanString(script);
        m_Script->AppendText(script);
        m_fileString->SetValue(wxString(filePath));
        return;
    }
    else if(response == wxID_CANCEL){
        cout  << "Load cancelled\n";
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

    wxFileDialog SaveAsConsole( this, _( "Saving your script - end file name with .js" ), wxEmptyString, wxEmptyString,
                               _T("js files (*.js)|*.js|All files (*.*)|*.*"),
                               wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxDD_NEW_DIR_BUTTON);
    response = SaveAsConsole.ShowModal();
    if( response == wxID_OK ) {
        filePath = SaveAsConsole.GetPath();
        if (!filePath.EndsWith(_(".js"))){
            wxMessageBox("File name must end with .js - not saved");
            return;
        }
        m_Script->SaveFile(filePath, wxTEXT_TYPE_ANY);
        m_fileString->SetValue(wxString(filePath));
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

    filePath = m_fileString->GetValue();
    if ((filePath != "") & wxFileExists(filePath))
    {  // Have a 'current' file, so can just save to it
        // cout << "Saving to existing file\n";
        m_Script->SaveFile(filePath);
        return;
    }
    else OnSaveAs(event);  // No previous save, so do Save As
    return;
}

void Console::OnCopyAll(wxCommandEvent& event) {
    int currentPosition = this->m_Script->GetCurrentPos();
//    this->m_Script->SetSelection((long) 0, (long) 100000000);   // select all
    this->m_Script->SelectAll();
    this->m_Script->Copy();
    this->m_Script->GotoPos(currentPosition);
    cout << "Have done Test B\n";
}

void Console::OnClearScript( wxCommandEvent& event ){
    this->m_Script->ClearAll();
    this->m_Script->SetFocus();
    this->m_fileString->Clear();    // clear the saved file string too
}

void Console::OnClearOutput( wxCommandEvent& event ){
    this->m_Output->ClearAll();
}


void Console::OnRun( wxCommandEvent& event )
{
    wxString    script;
    bool        more;    // true if more work by call-backs to functions
    void fatal_error_handler(void *udata, const char *msg);

    JS_control.m_pJSconsole = this;  // make a note of our console - used elsewhere
//    wxStreamToTextRedirector redirect(m_Output);  // redirect sdout to our window pane

    if (this->run_button->GetLabel() == runLabel){
        // OK to run JavaScript
        script = this->m_Script->GetValue();    // get the script to run
        if (script == wxEmptyString) return;  // ignore
        JS_control.m_pctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_error_handler);  // create the context
        if (!JS_control.m_pctx) {m_Output->AppendText("Duktape failed to create heap\n"); exit(1);}
        this->run_button->SetLabel(stopLabel);
        more = compileJS( script, this);         // compile and run the script
        if (!JS_control.m_JSactive){
            // not waiting for anything, so wrap up
            JS_control.m_runCompleted = true;
            JS_control.clear();
        }
    }
    else { // Stop button clicked - we have a script running - kill it off
        JS_control.m_runCompleted = true;
        JS_control.clear();
    }
}

void Console::OnClose(wxCloseEvent& event)
{
    cout << "Closing console\n";
    pPlugIn->OnJavaScriptConsoleClose();
}

void Console::OnTestA( wxCommandEvent& event){
    wxString    script;
    bool        more;
    wxString::const_iterator i;
    void fatal_error_handler(void *udata, const char *msg);
    wxString JScleanString(wxString given);

// #ifndef IN_HARNESS
    script = this->m_Script->GetValue();    // get the script
    if (script == wxEmptyString) {
        this->m_Output->AppendText(_("Empty script\n"));
        return;  // ignore
        }
    this->m_Output->AppendText(script + _("\n"));
    this->m_Output->AppendText(("Raw\n"));
    for (i = script.begin(); i != script.end(); ++i)
        {
        wxUniChar uni_ch = *i;
        this->m_Output->AppendText(wxString::Format(wxT("%d "), (int)uni_ch));
        }
    this->m_Output->AppendText(_("\n"));
    script = JScleanString(script);
    this->m_Output->AppendText(("Converted\n"));
    for (i = script.begin(); i != script.end(); ++i)
        {
        wxUniChar uni_ch = *i;
        this->m_Output->AppendText(wxString::Format(wxT("%d "), (int)uni_ch));
        }
    this->m_Output->AppendText(_("\n"));
    JS_control.m_pctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_error_handler);  // create the context
    if (!JS_control.m_pctx) {m_Output->AppendText("Duktape failed to create heap\n"); exit(1);}
    this->run_button->SetLabel(stopLabel);
    more = compileJS( script, this);         // compile and run the script
    if (!JS_control.m_JSactive){
        // not waiting for anything, so wrap up
        JS_control.m_runCompleted = true;
        JS_control.clear();
        }
// #else IN_HARNESS
    script = _("Messages ID");
    wxString message_body {_("message text")};
    this->pPlugIn->SetPluginMessage(script, message_body);
//#endif IN_HARNESS
    }
// testing functions modified as needed

/*
 const unsigned int GPXbufferLengthA {1000};
char GPXbufferA[GPXbufferLengthA];
void Console::OnTestA( wxCommandEvent& event)
{
    wxStreamToTextRedirector redirect(m_Output);  // redirect sdout to our window pane
    cout << "Test A started\n";
    wxString sentence = "$OCRMB,A,0.001,R,,001,5037.604,N,00149.556,W,10.584,109.060,0.000,V,A*73";  // correct
    JS_control.m_pJSconsole->pPlugIn->SetNMEASentence(sentence);
    //  wxString sentence = "$GPGSV,3,1,12,38,31,177,37,15,19,167,44,24,75,103,53,1,07,319,29*7A";  // incorrect
    //  JS_control.m_pJSconsole->pPlugIn->SetNMEASentence(sentence);
}


void Console::OnTestB( wxCommandEvent& event )  // for debugging fix API
{
    cout << "Test B started\n";
    JS_control.m_positionFix.Lat = 51.234;
    JS_control.m_positionFix.Lon = -1.234;
    JS_control.m_positionFix.Cog = 123.45;
    JS_control.m_positionFix.Sog = 4.5;
    JS_control.m_positionFix.Var = -0.123;
    JS_control.m_positionFix.Hdm = 179.87;
    JS_control.m_positionFix.Hdt = 178.97;
    JS_control.m_positionFix.FixTime = 99999;
    JS_control.m_positionFix.nSats = 11;
    return;
}
*/
