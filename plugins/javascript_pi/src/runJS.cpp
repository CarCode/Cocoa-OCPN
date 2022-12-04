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

// Runs some JavaScript and returns any result

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "duktape.h"
#include "stdio.h"
#include <wx/arrimpl.cpp>
#include <string>
#include "wx/strconv.h"
#include <stdarg.h>
#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include "ocpn_duk.h"
#include "wx/stc/stc.h"

extern JS_control_class JS_control;

// Provide for handling fatal Javascript error
    void fatal_error_handler(void *udata, const char *msg) {
    (void) udata;  /* ignored in this case, silence warning */

    /* Note that 'msg' may be NULL. */
    wxMessageBox((msg ? msg : "(no message)"), "Fatal JavaScript error");
    fprintf(stderr, "Causing intentional segfault...\n");
    fflush(stderr);
    *((volatile unsigned int *) 0) = (unsigned int) 0xdeadbeefUL;
    abort();
}
wxString JScleanString(wxString given){ // cleans script string of unacceptable characters
    const wxString leftQuote    { _("\u201c")};
    const wxString rightQuote   {_("\u201d")};
//  const wxString quote        {_("\u0022")};
    const wxString quote        {_("\"")};
    const wxString accute       {_("\u00b4")};
    const wxString rightSquote  {_("\u2019")};    // right single quote
//  const wxString apostrophe   {_("\u0027")};
    const wxString apostrophe   {_("\'")};
    const wxString ordinal      {_("\u00ba")};  // masculine ordinal indicator - like degree
    const wxString degree       {_("\u00b0")};
#ifndef __WXMSW__   // Don't try this one on Windows
    const wxString prime        {_("\u2032")};
    given.Replace(prime, apostrophe, true);
#endif  // __WXMSW__
    given.Replace(leftQuote, quote, true);
    given.Replace(rightQuote, quote, true);
    given.Replace(accute, apostrophe, true);
    given.Replace(rightSquote, apostrophe, true);
    given.Replace(ordinal, degree, true);
    return (given);
    }

wxString JScleanOutput(wxString given){ // clean unacceptable characters in output
    // As far as we know this only occurs with º symbol on Windows
    const wxString A_stringDeg{ _("\u00C2\u00b0")};    // Âº
    const wxString A_stringOrd{ _("\u00C2\u00ba")};    // Â ordinal
    given.Replace(A_stringDeg, _("\u00b0"), true);
    return (given);
    }

void jsMessage(duk_context *ctx, int style, wxString messageAttribute, wxString message){
    JS_control.m_pJSconsole->Show(); // make sure console is visible
    wxStyledTextCtrl* output_window = JS_control.m_pJSconsole->m_Output;
    int long beforeLength = output_window->GetTextLength(); // where we are before adding text
    output_window->AppendText("JavaScript ");
    output_window->AppendText(messageAttribute);
    output_window->AppendText(message);
    output_window->AppendText("\n");
    int long afterLength = output_window->GetTextLength(); // where we are after adding text
    output_window->StartStyling(beforeLength);
    output_window->SetStyling(afterLength-beforeLength-1, style);
    }

bool compileJS(wxString script, Console* console){
    // compiles and runs supplied JavaScript script and returns true if succesful
    wxString result;
    jsFunctionNameString_t function;
    bool more, errors;   // set to true if follow-up functions to be run
    void duk_extensions_init(duk_context *ctx);
    void ocpn_apis_init(duk_context *ctx);
    
    JS_control.m_pJSconsole = console;  // remember our console
//    wxStyledTextCtrl* output_window = console->m_Output;    // where we want stdout to go // Unused variable

    // clean up fatefull characters in script
    script = JScleanString(script);

    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_error_handler);  // create the context
    JS_control.m_pctx = ctx;  // remember it
    duk_extensions_init(ctx);  // register our own extensions
    ocpn_apis_init(ctx);       // register our API extensions
    more = false;      // start with no call-backs - will be set true in 'on' callback APIs
    errors = false;
    JS_control.m_runCompleted = false;

    duk_push_string(ctx, script.mb_str());   // load the script
    if (duk_peval(ctx) != 0) { // compile and run - how did it go?
        jsMessage(ctx, STYLE_RED, _(" "), duk_safe_to_string(ctx, -1));
        return false;
        }
    result = duk_safe_to_string(ctx, -1);
#ifdef __WXMSW__
    result = JScleanOutput(result); // clean up if Windows
#endif
    jsMessage(ctx, STYLE_BLUE, _("result: "), result);
     duk_pop(ctx);  /* pop result/error */

    // check if script has set up call backs with nominated functions
    size_t count = JS_control.m_messages.GetCount();
    if (count > 0){
        more = true;
        for (unsigned int i = 0; i < count; i++){
            function = JS_control.m_messages[i].functionName;
            if (function != wxEmptyString){
                // script set call back to this function - check it exists
                duk_push_global_object(ctx);  // ready with our compiled script
                if (!duk_get_prop_string(ctx, -1, function.c_str())){  // Does function exist in there?
                    jsMessage(ctx, STYLE_RED, _("function ") + function + " ", duk_safe_to_string(ctx, -1));
                    errors = true;   // will not proceed with call-backs
                    }
                duk_pop(ctx);
                }
        }
    }
    // check if timers set
    count = JS_control.m_times.GetCount();
    if ((count > 0)){
        more = true;
        for (unsigned int i = 0; i < count; i++){
            function = JS_control.m_times[i].functionName;
            if (function != wxEmptyString){
                // script set call back to this function - check it exists
                duk_push_global_object(ctx);  // ready with our compiled script
                if (!duk_get_prop_string(ctx, -1, function.c_str())){  // Does function exist?
                    jsMessage(ctx, STYLE_RED, _("Timed function ") + function + " ", duk_safe_to_string(ctx, -1));
                    errors = true;   // will not proceed with call-backs
                    }
                duk_pop(ctx);
                }
            }
        }
    
    // check if NMEA call back set
    function = JS_control.m_NMEAmessageFunction;
    if (function != wxEmptyString){
        more = true;
        // script set call back to this function - check it exists
        duk_push_global_object(ctx);  // ready with our compiled script
        if (!duk_get_prop_string(ctx, -1, function.c_str())){  // Does function exist?
            jsMessage(ctx, STYLE_RED, _("function ") + function + " ", duk_safe_to_string(ctx, -1));
            errors = true;   // will not proceed with call-backs
            }
        duk_pop(ctx);
        }


    if (errors) more = false;
    JS_control.m_JSactive = more;  // remember for outside here
    return(more);
}

