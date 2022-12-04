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

#include <iostream>
#include "wx/wx.h"
#include "wx/fileconf.h"
#include "duktape.h"
// #include <dukglue/dukglue.h>
#include <iostream>
#include "JavaScriptgui_impl.h"
//#include "wx/arrimpl.cpp"

extern JavaScript_pi* pJavaScript_pi;
extern Console* pConsoleBeingTimed;
void throwErrorByCtx(duk_context *ctx, wxString message);
Console* findConsoleByName(wxString name);

duk_ret_t console_exists(duk_context *ctx){
    //test if console exists
    duk_require_string(ctx, 0);
    Console* pConsole;
    wxString name = duk_get_string(ctx, 0);
    duk_pop(ctx);
    pConsole = findConsoleByName(name);
    duk_push_boolean(ctx, (pConsole != nullptr)?true:false);
    return 1;
    };

duk_ret_t console_busy(duk_context *ctx){
    //test if console busy
    duk_require_string(ctx, 0);
    Console* pConsole;
    wxString name = duk_get_string(ctx, 0);
    duk_pop(ctx);
    pConsole = findConsoleByName(name);
    if (!pConsole) throwErrorByCtx(ctx, "Console " + name + " does not exist");
    duk_push_boolean(ctx, pConsole->isBusy());
    return 1;
    };

duk_ret_t console_clearOutput(duk_context *ctx){
    // clear output pane
    Console* pConsole;
    Console *findConsoleByCtx(duk_context *ctx);
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    if (nargs>1) throwErrorByCtx(ctx, "consoleClearOutput bad call (too many arguments)");
    if (nargs == 0){    // clearing our own output
        pConsole = findConsoleByCtx(ctx);
        }
    else {
        duk_require_string(ctx, 0);
        wxString name = duk_get_string(ctx, 0);
        duk_pop(ctx);
        pConsole = findConsoleByName(name);
        if (!pConsole) throwErrorByCtx(ctx, "Console " + name + " does not exist");
        }
    pConsole->m_Output->ClearAll();
    return 0;
    };

duk_ret_t console_getOutput(duk_context *ctx){
    // clear output pane
    duk_require_string(ctx, 0);
    Console* pConsole;
    Console *findConsoleByCtx(duk_context *ctx);
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    if (nargs>1) throwErrorByCtx(ctx, "consoleClearOutput bad call (too many arguments)");
    if (nargs == 0){    // clearing our own output
        pConsole = findConsoleByCtx(ctx);
        }
    else {
        duk_require_string(ctx, 0);
        wxString name = duk_get_string(ctx, 0);
        duk_pop(ctx);
        pConsole = findConsoleByName(name);
        if (!pConsole) throwErrorByCtx(ctx, "Console " + name + " does not exist");
        }
    duk_push_string(ctx, pConsole->m_Output->GetText());
    return 1;
    };

duk_ret_t console_add(duk_context *ctx){
    // add new console
    duk_require_string(ctx, 0);
    Console* pConsole;
    int x, y;
    wxString name = duk_get_string(ctx, 0);
    duk_pop(ctx);
    pConsole = findConsoleByName(name);
    if (pConsole) throwErrorByCtx(ctx, "Console " + name + " already exists");
    pConsole = new Console(pJavaScript_pi->m_parent_window, name);
    pConsole->CenterOnScreen();
    // to prevent multiple new consoles hiding eachother completely, we will shift each randomly
    pConsole->GetPosition(&x, &y);
    x += - 25 + rand()%50; y += - 25 + rand()%50;
    pConsole->SetPosition(wxPoint(x, y));
    pConsole->Show();
    return 0;
    };

duk_ret_t console_close(duk_context* ctx){
    Console *findConsoleByCtx(duk_context *ctx);
    duk_require_string(ctx, 0);
    wxString name = duk_get_string(ctx, 0);
    duk_pop(ctx);
    Console* pConsole = findConsoleByName(name);
    if (!pConsole) throwErrorByCtx(ctx, "Console " + name + " does not exist");
    if (pConsole == findConsoleByCtx(ctx)) throwErrorByCtx(ctx, "Console " + name + " cannot close yourself");
    pConsole->bin();
    return 0;
    }

duk_ret_t console_load(duk_context* ctx){
    // consoleLoad(consoleName,  script) into console
    wxString fileString, lineOfFile, consoleName;
    wxFileName resolveFileName(Console* pConsole, wxFileName filePath, int options);
    wxTextFile inputFile;
    Console* pConsole;
    
    duk_require_string(ctx, 0);
    duk_require_string(ctx, 1);
    consoleName = wxString(duk_get_string(ctx, 0));
    fileString = wxString(duk_get_string(ctx, 1));
    duk_pop_2(ctx);
    pConsole = findConsoleByName(consoleName);
    if (!pConsole) throwErrorByCtx(ctx, "Console " + consoleName + " does not exist");
    if (pConsole == pConsoleBeingTimed)
        pConsole->throw_error(pConsoleBeingTimed->mpCtx, "Load console " + pConsole->mConsoleName + " cannot load into own console");
    if (pConsole->mpCtx) pConsoleBeingTimed->throw_error(pConsoleBeingTimed->mpCtx, "Load console " + pConsole->mConsoleName + " is busy");
    wxFileName filePath = resolveFileName(pConsoleBeingTimed, wxString(fileString), MUST_EXIST);
    fileString = filePath.GetFullPath();
    inputFile.Open(fileString);
    pConsole->m_Script->ClearAll();
    for (lineOfFile = inputFile.GetFirstLine(); !inputFile.Eof(); lineOfFile = inputFile.GetNextLine()){
        pConsole->m_Script->AppendText(lineOfFile + "\n");
        }
    pConsole->m_fileStringBox->SetValue(wxString(fileString));
    pConsole->auto_run->Show();
    pConsole->auto_run->SetValue(false);
    return 0;
    }

duk_ret_t console_run(duk_context* ctx){
    // consoleRun(consoleName,  brief)
    wxString consoleName, brief;
    Console* pConsole;
    bool haveBrief {false};
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    
    duk_require_string(ctx, 0);
    consoleName = wxString(duk_get_string(ctx, 0));
    if (nargs>1){
        duk_require_string(ctx, 1);
        brief = wxString(duk_get_string(ctx, 1));
        haveBrief = true;
        duk_pop(ctx);
        }
    duk_pop(ctx);
    pConsole = findConsoleByName(consoleName);
    if (pConsole == pConsoleBeingTimed)
        pConsole->throw_error(pConsoleBeingTimed->mpCtx, "Run console " + pConsole->mConsoleName + " cannot run own console");
    if (pConsole->mpCtx) pConsoleBeingTimed->throw_error(pConsoleBeingTimed->mpCtx, "Run console " + pConsole->mConsoleName + " is busy");
    pConsole->mBrief.callback = false;
    if (haveBrief){
        pConsole->mBrief.theBrief = brief;
        pConsole->mBrief.hasBrief = true;
        }
    pConsole->mChainedScriptToRun = true;
    pConsole->CallAfter(&Console::doRunCommand, pConsole->mBrief);
    return 0;
    }
    
duk_ret_t onConsoleResult(duk_context* ctx){
    // onConsoleResult(consoleName,  [brief,] function)
    // or onConsoleResult(consoleName, function, brief)
    Console* findConsoleByCtx(duk_context* ctx);
    wxString extractFunctionName(duk_context *ctx, duk_idx_t idx);
    wxString consoleName, functionName, brief;
    Console* pConsole, *pCallingConsole;
    bool haveBrief {false};
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    pCallingConsole = pConsoleBeingTimed;
 
    if (nargs < 2) pCallingConsole->throw_error(ctx, "onConsoleResult called with insufficient args");
    duk_require_string(ctx, 0);
    consoleName = wxString(duk_get_string(ctx, 0));
    if (nargs>2){
        if (duk_is_string(ctx, 2)){
            // arg 2 must be brief and arg 1 the function
            duk_require_object(ctx, 1);
            brief = duk_get_string(ctx, 2);
            haveBrief = true;
            functionName = extractFunctionName(ctx, 1);
            }
        else {
            // arg 1 must be brief and arg 2 the function
            duk_require_object(ctx, 2);
            functionName = extractFunctionName(ctx, 2);
            brief = duk_get_string(ctx, 1);
            haveBrief = true;
            }
        duk_pop(ctx);   // the third arg
        }
    else {
        // 2 args - 2nd must be function
        duk_require_object(ctx, 1);
        functionName = extractFunctionName(ctx, 1);
        }
    duk_pop_2(ctx); // first and second args
    pConsole = findConsoleByName(consoleName);
    if (pConsole->mRunningMain || pConsole->isWaiting())
        pCallingConsole->throw_error(ctx, "onConsoleResult target console " + pConsole->mConsoleName + " is busy");
    if (pCallingConsole->mConsoleCallbacksAwaited > MAX_TIMERS){
        pCallingConsole->throw_error(ctx, "onConsoleResult error: already have maximum callbacks outstanding");
        }
    // OK - ready to go
    pConsole->mBrief.briefingConsoleName = pCallingConsole->mConsoleName;
    pConsole->mBrief.callback = true;
    pConsole->mBrief.function = functionName;
    if (haveBrief){
        pConsole->mBrief.theBrief = brief;
        pConsole->mBrief.hasBrief = true;
        }
    else pConsole->mBrief.hasBrief = false;
    pConsole->mChainedScriptToRun = true;
    pCallingConsole->mConsoleCallbacksAwaited++;
    pCallingConsole->mWaitingCached = false;
    pConsole->mWaitingCached = pConsole->mWaiting = true;
    pConsole->CallAfter(&Console::doRunCommand, pConsole->mBrief);
    return 0;
    }

void register_console(duk_context *ctx){
    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "consoleExists");
    duk_push_c_function(ctx, console_exists, 1 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleBusy");
    duk_push_c_function(ctx, console_busy, 1 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "consoleClearOutput");
    duk_push_c_function(ctx, console_clearOutput, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);;
    
    duk_push_string(ctx, "consoleGetOutput");
    duk_push_c_function(ctx, console_getOutput, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "consoleAdd");
    duk_push_c_function(ctx, console_add, 1 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleClose");
    duk_push_c_function(ctx, console_close, 1 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleLoad");
    duk_push_c_function(ctx, console_load, 2 /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "consoleRun");
    duk_push_c_function(ctx, console_run, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "onConsoleResult");
    duk_push_c_function(ctx, onConsoleResult, DUK_VARARGS /* arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    };

bool loadComponent(duk_context *ctx, wxString name) {
    bool result {false};
    if (name == "Consoles") {register_console(ctx); result = true;}
    return result;
    };
