/* ************************************************************************
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

#include "duktape.h"
#include "JavaScript_pi.h"
#include "JavaScriptgui_impl.h"
#include "ocpn_duk.h"
#include <string>

/*  On hold for now - cannot find a way of handling the variable arguments
 // sprintf function
 static duk_ret_t duk_sprintf(duk_context *ctx) {
 const char *duk_push_string(duk_context *ctx, const char *str);
 const char *duk_to_string(duk_context *ctx, duk_idx_t idx);
 const char *duk_push_sprintf(duk_context *ctx, const char *fmt, ...);
 duk_int_t duk_get_type(duk_context *ctx, duk_idx_t idx);
 duk_idx_t nargs;  // number of args in call
 
 nargs = duk_get_top(ctx);
 cout << "Inside sprintf - nargs=" << nargs <<"\n";
 cout << "arg0 is type:" << duk_get_type(ctx, 0) << " and value:" << duk_to_string(ctx,0) << "\n";
 cout << "arg1 is type:" << duk_get_type(ctx, 1) << " and value:" << duk_to_string(ctx,1) << "\n";
 cout << "sprintf gives:" << duk_push_sprintf(ctx, duk_to_string(ctx,0), duk_to_string(ctx,1)) << "\n";
 
 return (1);
 //    return (*duk_push_sprintf(ctx, "meaning of life: %d, name", 42));
 }
 */

Console *findConsoleByCtx(duk_context *ctx);
    
wxString js_formOutput(duk_context *ctx){
    duk_idx_t nargs;  // number of args in call
    wxString output = wxEmptyString;
    
    nargs = duk_get_top(ctx);
//   MAYBE_DUK_DUMP
    for(int i = 0; i<nargs; i++){
        switch (duk_int_t type = duk_get_type(ctx, i)){
            case DUK_TYPE_STRING:
            case DUK_TYPE_NUMBER:
                output = output + duk_to_string(ctx,i);  // mangles º character on Windows!
                break;
            case DUK_TYPE_BOOLEAN:
                output = output + ((duk_to_boolean(ctx,i) == 1) ? "true":"false");
                break;
            case DUK_TYPE_OBJECT:
                duk_json_encode(ctx, i);
                output = output + duk_to_string(ctx,i);
                break;
            case DUK_TYPE_UNDEFINED:
                findConsoleByCtx(ctx)->throw_error(ctx, "print - arg " + to_string(i) + " is undefined");
            default:
                findConsoleByCtx(ctx)->throw_error(ctx, "print - arg " + to_string(i) + " of unexpected type " + to_string(type));
        }
 //       duk_pop_n(ctx, nargs);
    }
#ifdef __WXMSW__
    wxString JScleanOutput(wxString given);
    output = JScleanOutput(output); // clean for Windows only
#endif
    return(output);
}

duk_ret_t print_coloured(duk_context *ctx, int colour) {   // print arguments on stack in colour
    wxStyledTextCtrl* output_window;
    int long beforeLength, afterLength;
    Console *pConsole = findConsoleByCtx(ctx);
    
    pConsole->Show(); // make sure console is visible
    output_window = pConsole->m_Output;
    beforeLength = output_window->GetTextLength(); // where we are before adding text
    output_window->AppendText(js_formOutput(ctx));
    afterLength = output_window->GetTextLength(); // where we are after adding text
    output_window->StartStyling((int)beforeLength,0);   // 2nd parameter included Linux still using wxWidgets v3.0.2
    output_window->SetStyling((int)(afterLength-beforeLength), colour);
    return (0);
}

void onDismissAlert(wxCommandEvent & event){
    // here when alert to be dismissed
    jsButton *button = wxDynamicCast(event.GetEventObject(), jsButton);
    Console *pConsole = button->pConsole;
//    TRACE(3,pConsole->mConsoleName + "alert dismissed ");
    pConsole->clearAlert();
    if (!pConsole->isWaiting()) {
        pConsole->wrapUp(DONE);
        }
    }

static duk_ret_t duk_alert(duk_context *ctx) {   // create or add to an alert  box with dismiss button
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console *pConsole = findConsoleByCtx(ctx);
    wxDialog *alert = pConsole->mAlert.palert;    // any existing alert
    extern JavaScript_pi* pJavaScript_pi;
    
    if (nargs == 0){    // just return present state
        duk_push_boolean(ctx, (alert != nullptr) ? true : false);
        return 1;
        }
    
    // if called with single argument of false, is attempt to cancel any open alert
    // cancel any existing alert and return true if there was one
    if (nargs == 1 && duk_is_boolean(ctx, -1) && !duk_get_boolean(ctx, -1)){
        duk_pop(ctx);   // the argument
        if (alert != nullptr){
            // there is an alert displayed
            pConsole->clearAlert();
            duk_push_boolean(ctx, true);
            }
        else duk_push_boolean(ctx, false);
        return 1;
        }
    
    // we have alert text to display
    pConsole->mAlert.alertText += js_formOutput(ctx); // add text to any existing
    if (alert != nullptr){  // clear any existing alert being displayed
        pConsole->mAlert.palert->Close();
        pConsole->mAlert.palert->Destroy();
        }
    alert = new wxDialog(pJavaScript_pi->m_parent_window,  wxID_ANY, _("JavaScript alert"), pConsole->mAlert.position, wxDefaultSize,
                         wxCAPTION | wxSTAY_ON_TOP | wxBORDER_RAISED);
    alert->SetBackgroundColour(*wxYELLOW);
    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);  // A top-level sizer
    alert->SetSizer(boxSizer);
    wxStaticText* staticText = new wxStaticText( alert, wxID_STATIC, pConsole->mAlert.alertText, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    boxSizer->Add(staticText, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* buttonBox = new wxBoxSizer(wxHORIZONTAL);
    boxSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    jsButton* button = new jsButton ( pConsole, alert, wxNewId(), _("Dismiss"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonBox->Add(button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    alert->Bind(wxEVT_BUTTON, &onDismissAlert, wxID_ANY);
    alert->Fit();
    alert->Show(true);
    pConsole->mAlert.palert = alert;  // remember
    pConsole->mWaitingCached = pConsole->mWaiting = true;
    duk_pop_n(ctx, nargs);  // pop all the arguments
    duk_push_boolean(ctx, true);
    return 1;
    }

static duk_ret_t duk_result(duk_context *ctx) {   // result
    Console *pConsole = findConsoleByCtx(ctx);
    pConsole->m_explicitResult = true;
    if (duk_get_top(ctx) > 0) {// called with args
        pConsole->m_result = js_formOutput(ctx);
        pConsole->m_explicitResult = true;
        duk_pop(ctx);
        }
    duk_push_string(ctx, pConsole->m_result);
    return (1); // return present result
    }

static duk_ret_t duk_print(duk_context *ctx) {   // print
    duk_ret_t result = 0;
    Console *pConsole = findConsoleByCtx(ctx);
    pConsole->Show(); // make sure console is visible
    pConsole->m_Output->AppendText(js_formOutput(ctx));
    return (result);
    }

static duk_ret_t duk_print_red(duk_context *ctx) {   // print in red
    return (print_coloured(ctx, STYLE_RED));
    }

static duk_ret_t duk_print_orange(duk_context *ctx) {   // print in orange
    return (print_coloured(ctx, STYLE_ORANGE));
    }

static duk_ret_t duk_print_green(duk_context *ctx) {   // print in green
    return (print_coloured(ctx, STYLE_GREEN));
    }

static duk_ret_t duk_print_blue(duk_context *ctx) {   // print in blue
    return (print_coloured(ctx, STYLE_BLUE));
    }

/*
    static duk_ret_t duk_print_bold(duk_context *ctx) {   // print bold
    return (print_coloured(ctx, STYLE_BOLD));
    }
*/

static duk_ret_t duk_print_ul(duk_context *ctx) {   // print underlined
   return (print_coloured(ctx, STYLE_UNDERLINE));
  }

static duk_ret_t duk_log(duk_context *ctx) {   // log message
    duk_ret_t result = 0;
    wxLogMessage(js_formOutput(ctx));
    return (result);
    }

#if 0
// On hold for now - variable arguments of variable types defeated me
static duk_ret_t duk_format(duk_context *ctx) {   // format text
    MAYBE_DUK_DUMP
    duk_push_sprintf(<#duk_context *ctx#>, <#const char *fmt, ...#>)
    void *ptr;
    int argCount;
    const char *fmt;
    const char *arg1s, *arg2s, *arg3s;
    int arg1i, arg2i, arg3i;
    double arg1n, arge2n, arg3n;
    bool arge1b, arge2b, arg3b;
    
    cout<< "entered duk_format\n";
    MAYBE_DUK_DUMP
    argCount = duk_get_top(ctx);
    if (argCount > 1){
        
    }
    fmt = duk_get_string(ctx, -3);
    arg1 = duk_get_string(ctx, -2);
    arg2 = duk_get_string(ctx, -1);
    duk_pop_3(ctx);
    MAYBE_DUK_DUMP
    duk_push_sprintf(ctx, fmt, arg1);
    MAYBE_DUK_DUMP
    return (1);
}
#endif

static duk_ret_t duk_read_text_file(duk_context *ctx) {  // read in a text file
    // read in a text file
    extern JavaScript_pi* pJavaScript_pi;
    wxFileName resolveFileName(Console* pConsole, wxFileName filePath, int options);
    wxString fileNameGiven, fileString, lineOfFile, text;
    wxFileName filePath;
    wxTextFile inputFile;
    bool ok;
    wxString JScleanString(wxString line);
    Console *pConsole = findConsoleByCtx(ctx);
    
    fileNameGiven = duk_to_string(ctx,0);
    duk_pop(ctx);  // finished with that
    filePath = resolveFileName(pConsole, wxFileName(fileNameGiven), MUST_EXIST);
    fileString = filePath.GetFullPath();
    ok = inputFile.Open(fileString);
    for (lineOfFile = inputFile.GetFirstLine(); !inputFile.Eof(); lineOfFile = inputFile.GetNextLine()){
        text += lineOfFile + "\n";
        }
    text = JScleanString(text);
    duk_push_string(ctx, text);
    return 1;
    };

static duk_ret_t duk_write_text_file(duk_context *ctx) {  // write a text file
    // write a text file
    extern JavaScript_pi* pJavaScript_pi;
    wxString fileNameGiven, text;
    wxFileName filePath;
    wxTextFile outputFile;
    int access;
    Console *pConsole = findConsoleByCtx(ctx);
    
    text = duk_to_string(ctx, 0);
    fileNameGiven = duk_to_string(ctx,1);
    access = duk_to_int(ctx, 2);

    if ((access < 0) || (access > 2)) pConsole->throw_error(ctx, "writeTextFile " + filePath.GetFullPath() + "access type invalid");
    filePath = wxFileName(fileNameGiven);
    if (!filePath.IsAbsolute()){
        // relative  - so will prepend current working directory and do again
        fileNameGiven = pJavaScript_pi->mCurrentDirectory + wxFILE_SEP_PATH + fileNameGiven;
        filePath = wxFileName(fileNameGiven);
        }
    if (!filePath.IsOk()) pConsole->throw_error(ctx, "writeTextFile " + filePath.GetFullPath() + " does not make sense");
    if (!filePath.FileExists())
        outputFile.Create(fileNameGiven);
    else if(access == 0)
            pConsole->throw_error(ctx, "writeTextFile " + filePath.GetFullPath() + " already exists");
    else outputFile.Open(fileNameGiven);
    if (access == 1) outputFile.Clear();
    outputFile.AddLine(text);
    outputFile.Write();
    duk_pop_3(ctx);  // finished with that
    return 0;
    };

duk_ret_t duk_require(duk_context *ctx){ // the module search function
    wxString fileNameGiven, fileString, lineOfData, script;
    wxFileName filePath;
    wxTextFile inputFile;
//    wxFileName homeDirectory;
    wxString JScleanString(wxString line);
    bool loadComponent(duk_context *ctx, wxString name);
    wxFileName resolveFileName(Console* pConsole, wxFileName filePath, int options);
    Console *pConsole = findConsoleByCtx(ctx);
    
    fileNameGiven = duk_to_string(ctx,0);
    duk_pop(ctx);  // finished with that
    filePath = wxFileName(fileNameGiven);
    if ((filePath.GetDirCount() == 0) && !filePath.HasExt()){
        // simple file name

        if (loadComponent(ctx, fileNameGiven)) {
            // above we will have loaded a C++ component if it matches
            return 1;
            }
        else {  // we will look for it in the build-in scripts
            // this updated for ocpn v5.5
#ifdef __WXOSX__
            fileString = *GetpPrivateApplicationDataLocation() + wxFileName::GetPathSeparator() + _T("plugins") + wxFileName::GetPathSeparator() + _T("JavaScript_pi");
            fileString = GetPluginDataDir("JavaScript_pi");
            filePath.SetPath(fileString);
            filePath.AppendDir("data");
            filePath.AppendDir("scripts");
#else
            fileString = GetPluginDataDir("JavaScript_pi");
            filePath.SetPath(fileString);
            filePath.AppendDir("data");
            filePath.AppendDir("scripts");
#endif
            filePath.SetFullName(fileNameGiven);
            if (!filePath.FileExists()){
                pConsole->throw_error(ctx, "require - " + filePath.GetFullName() + " not in scripts library");
                }
            if (!filePath.IsFileReadable()) pConsole->throw_error(ctx, "readTextFile " + filePath.GetFullPath() + " not readable");
            // ready to go
            }
        }
    else{   // not a built-in or library script - we will hunt for it
        filePath = resolveFileName(pConsole, filePath, MUST_EXIST);
        }
    fileString = filePath.GetFullPath();
    inputFile.Open(fileString);
    for (lineOfData = inputFile.GetFirstLine(); !inputFile.Eof(); lineOfData = inputFile.GetNextLine()){
        script += lineOfData + "\n";
        }
    script = JScleanString(script);
    duk_push_string(ctx, script);
    duk_push_string(ctx, "anything"); // the name here is irrelevant
    if (duk_pcompile(ctx, DUK_COMPILE_FUNCTION)){
        pConsole->throw_error(ctx, duk_safe_to_string(ctx, -1));
        }
    return(1);
    };

static duk_ret_t onSeconds(duk_context *ctx) {  // call function after milliseconds elapsed
    wxString extractFunctionName(duk_context *ctx, duk_idx_t idx);
    duk_ret_t result = 0;
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    TimeActions ourTimeAction;
    wxString argument = wxEmptyString;
    Console *pConsole = findConsoleByCtx(ctx);
    
    if (nargs == 0) { // empty call - cancel all timers
        pConsole->mTimes.Clear();
        pConsole->mTimerActionBusy = false;
        pConsole->mWaitingCached = false;   // force full isWaiting check
        return(result);
        }
    
    if (!duk_is_function(ctx, 0)) pConsole->throw_error(ctx, "onSeconds first argument must be function");
    if ((int)pConsole->mTimes.GetCount() > MAX_TIMERS){
        pConsole->throw_error(ctx, "onSeconds already have maximum timers outstanding");
        }
    if ((nargs < 2) || (nargs > 3)){
        pConsole->throw_error(ctx, "onSeconds requires two or three arguments");

        }
    wxTimeSpan delay(0, 0, duk_to_number(ctx,1), 0);
    ourTimeAction.timeToCall = wxDateTime::Now() + delay;
    ourTimeAction.functionName = extractFunctionName(ctx, 0);
    if (nargs > 2) argument = wxString(duk_to_string(ctx,2));  //if user included 3rd argument, use it
    ourTimeAction.argument = argument;
    pConsole->mTimes.Add(ourTimeAction);  // add this action to array
    pConsole->mWaitingCached = pConsole->mWaiting = true;
    return(result);
    }

static duk_ret_t duk_timeAlloc(duk_context *ctx) {   // time allocation
    Console *pConsole = findConsoleByCtx(ctx);
    wxLongLong timeLeft =  pConsole->m_pcall_endtime - wxGetUTCTimeMillis();
    if (timeLeft < 0)  timeLeft = 0;    // have captured time left
        if (duk_get_top(ctx) > 0 ){
        // was called with argument - us to set new allocatins
            pConsole->m_time_to_allocate = duk_get_int(ctx, -1);
        duk_pop(ctx);
        }
    pConsole->m_pcall_endtime = wxGetUTCTimeMillis() + pConsole->m_time_to_allocate;  // grant more time
    duk_push_number(ctx, timeLeft.GetValue());   // will return time that was left
    return (1);
}

static duk_ret_t duk_stopScript(duk_context *ctx) {
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console *pConsole = findConsoleByCtx(ctx);
    
    if (nargs > 0){
        pConsole->m_result = duk_safe_to_string(ctx, -1);
        pConsole->m_explicitResult = true;
        duk_pop(ctx);
        }
    pConsole->mStatus.set(STOPPED);
    duk_throw(ctx);
    return 0;  //never executed but keeps compiler happy
    }


Console* getConsoleWithOptionalNameOnStack(duk_context *ctx){
    Console* findConsoleByName(wxString name);
    void throwErrorByCtx(duk_context *ctx, wxString message);
    extern JavaScript_pi *pJavaScript_pi;
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console* pConsole;
    wxString consoleName;
    
    if (nargs > 0){
        consoleName = duk_get_string(ctx, 0);
        duk_pop(ctx);
        pConsole = findConsoleByName(consoleName);
        if (!pConsole) throwErrorByCtx(ctx, "No console with name " + consoleName);
        }
    else pConsole = findConsoleByCtx(ctx);
    return pConsole;
    }

static duk_ret_t duk_consoleHide(duk_context *ctx) {
    // consoleHide() to hide this console
    // consoleHide(name) to hide that console
    if (duk_is_boolean(ctx, 0)){
        // old version 0.3 call
        bool which = duk_get_boolean(ctx, 0);
        duk_pop(ctx);
        Console* pConsole = findConsoleByCtx(ctx);
        if (which) pConsole->Hide();
        else pConsole->Show();
        pConsole->message(STYLE_ORANGE , "Warning - calling consoleHide with true or false deprecated");
        }
    else getConsoleWithOptionalNameOnStack(ctx)->Hide();
    return 0;
    }

static duk_ret_t duk_consoleShow(duk_context *ctx) {
    // consoleShow() to show this console
    // consoleShow(name) to show that console
    getConsoleWithOptionalNameOnStack(ctx)->Show();
    return 0;
    }

duk_ret_t chain_script(duk_context* ctx){
    // consoleChain(fileString,  brief)
    Console *findConsoleByCtx(duk_context *ctx);
    wxString getTextFile(Console* pConsole, wxString fileString);
    wxFileName resolveFileName(Console* pConsole, wxFileName filePath, int options);
    wxString fileString, script, brief;
    bool haveBrief {false};
    Console* pConsole = findConsoleByCtx(ctx);
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    
    duk_require_string(ctx, 0);
    fileString = wxString(duk_get_string(ctx, 0));
    // the file string is relative - maybe. Ensure is absolute for record
    wxFileName filePath = resolveFileName(pConsole, wxString(fileString), MUST_EXIST);
    fileString = filePath.GetFullPath();
    if (nargs>1){
        duk_require_string(ctx, 1);
        brief = wxString(duk_get_string(ctx, 1));
        haveBrief = true;
        duk_pop(ctx);
        }
    duk_pop(ctx);
    
    script = getTextFile(pConsole, fileString);
    pConsole->m_Script->SetValue(script);
    pConsole->m_fileStringBox->SetValue(fileString);
    pConsole->auto_run->Show();
    pConsole->auto_run->SetValue(false);
    if (haveBrief){
        pConsole->mBrief.fresh = true;
        pConsole->mBrief.theBrief = brief;
        pConsole->mBrief.hasBrief = true;
        pConsole->mBrief.fresh = true;
        }
    else pConsole->mBrief.hasBrief = false;
    // ready to go
    pConsole->m_result = wxEmptyString;
    pConsole->m_explicitResult = true;
    pConsole->mStatus.set(TOCHAIN);
    duk_throw(ctx); // terminates this script.  clearAndDestroyCtx will run the loaded script.
    return 0;   // this to keep compiler happy
    }

static duk_ret_t duk_getBrief(duk_context *ctx) {
    // get any brief left for this script
    extern JavaScript_pi *pJavaScript_pi;
    Console *pConsole = findConsoleByCtx(ctx);
    wxString brief;
    if ( !pConsole->mBrief.hasBrief)pConsole->throw_error(ctx, _T("getBrief found no brief"));
    duk_push_string(ctx, pConsole->mBrief.theBrief);
    return 1;
    }

static duk_ret_t duk_onExit(duk_context *ctx){
    // set an onExit function
    wxString extractFunctionName(duk_context *ctx, duk_idx_t idx);
    extern JavaScript_pi *pJavaScript_pi;
    Console *pConsole = findConsoleByCtx(ctx);
    pConsole->m_exitFunction = extractFunctionName(ctx, -1);
    duk_pop(ctx);
    return 0;
    }

#ifdef DUK_DUMP

static duk_ret_t duk_mainThread(duk_context *ctx){ // check if in main thread
    duk_push_boolean(ctx, wxThread::IsMain());
    return 1;
    
    }
static duk_ret_t jstest(duk_context *ctx){  // for testing when in harness
    int a = duk_get_int(ctx, 0);
    int b = duk_get_int(ctx, 1);
    if (a == b){
        duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, _("the arguments match"));
        duk_throw(ctx);
        }
    duk_push_int(ctx, a + b);
    return 1;
    }

static duk_ret_t dukDump(duk_context *ctx){  // for testing when in harness
    Console *pConsole = findConsoleByCtx(ctx);
    duk_push_string(ctx, pConsole->dukDump());
    return 1;
    }

static duk_ret_t consoleDump(duk_context *ctx){  // dumps useful stuff from the console structure
    Console *pConsole = findConsoleByCtx(ctx);
    duk_push_string(ctx, pConsole->consoleDump());
    return 1;
    }
#endif
    
void duk_extensions_init(duk_context *ctx) {
    extern duk_ret_t duk_dialog(duk_context *ctx);
    duk_idx_t duk_push_c_function(duk_context *ctx, duk_c_function func, duk_idx_t nargs);

    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "print");
    duk_push_c_function(ctx, duk_print, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "printRed");
    duk_push_c_function(ctx, duk_print_red, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "printOrange");
    duk_push_c_function(ctx, duk_print_orange, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "printGreen");
    duk_push_c_function(ctx, duk_print_green, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "printBlue");
    duk_push_c_function(ctx, duk_print_blue, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

#if 0
    duk_push_string(ctx, "printBold");
    duk_push_c_function(ctx, duk_print_bold, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
#endif
    
    duk_push_string(ctx, "printUnderlined");
    duk_push_c_function(ctx, duk_print_ul, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "printLog");
    duk_push_c_function(ctx, duk_log, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "alert");
    duk_push_c_function(ctx, duk_alert, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
     duk_push_string(ctx, "scriptResult");
    duk_push_c_function(ctx, duk_result, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "onSeconds");
    duk_push_c_function(ctx, onSeconds, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "onDialogue");
    duk_push_c_function(ctx, duk_dialog, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

#if 0
    duk_push_string(ctx, "format");
    duk_push_c_function(ctx, duk_format, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
#endif
    
    duk_push_string(ctx, "readTextFile");
    duk_push_c_function(ctx, duk_read_text_file, 1 /* number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "writeTextFile");
    duk_push_c_function(ctx, duk_write_text_file, 3 /* number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "require");
    duk_push_c_function(ctx, duk_require, 1 /* number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "timeAlloc");
    duk_push_c_function(ctx, duk_timeAlloc, DUK_VARARGS /* variable arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleHide");
    duk_push_c_function(ctx, duk_consoleHide, DUK_VARARGS /* variable arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleShow");
    duk_push_c_function(ctx, duk_consoleShow, DUK_VARARGS /* variable arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "stopScript");
    duk_push_c_function(ctx, duk_stopScript , DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "chainScript");
    duk_push_c_function(ctx, chain_script, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "getBrief");
    duk_push_c_function(ctx, duk_getBrief , 0 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "onExit");
    duk_push_c_function(ctx, duk_onExit , 1 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);


#ifdef DUK_DUMP
    duk_push_string(ctx, "JS_throw_test");
    duk_push_c_function(ctx, jstest, 2 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "duktapeDump");
    duk_push_c_function(ctx, dukDump, 0 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleDump");
    duk_push_c_function(ctx, consoleDump , 0 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "JS_mainThread");
    duk_push_c_function(ctx, duk_mainThread, 0 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

#endif

    duk_pop(ctx);
}
