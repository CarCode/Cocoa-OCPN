/* ************************************************************************
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

#include "stdio.h"
#include "duktape.h"
#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include <stdarg.h>
#include "ocpn_duk.h"
#include <string>

/*  On hold for now
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

void JS_dk_error(duk_context *ctx, wxString message){
    extern JS_control_class JS_control;
    JS_control.m_pJSconsole->Show(); // make sure console is visible
    duk_error(ctx, DUK_ERR_NONE, message); // we never return from this
}

wxString js_formOutput(duk_context *ctx){
    wxString JScleanOutput(wxString given);
    duk_idx_t nargs;  // number of args in call
    wxString output = "";

    nargs = duk_get_top(ctx);
    for(int i = 0; i<nargs; i++){
        switch (duk_int_t type = duk_get_type(ctx, i)){
            case DUK_TYPE_STRING:
            case DUK_TYPE_NUMBER:
                output = output + duk_to_string(ctx,i);  // mangles º chatcater on Windows!
                break;
            case DUK_TYPE_BOOLEAN:
                output = output + ((duk_to_boolean(ctx,i) == 1) ? "true":"false");
                break;
            case DUK_TYPE_OBJECT:
                JS_dk_error(ctx, "print - arg " + to_string(i) + " is object");
            default:
                JS_dk_error(ctx, "print - arg " + to_string(i) + " of unexpected type " + to_string(type));
        }
    }
#ifdef __WXMSW__
    output = JScleanOutput(output); // clean for Windows only
#endif
    return(output);
}

static duk_ret_t duk_alert(duk_context *ctx) {   // alert
    duk_ret_t result = 0;

    wxMessageBox(js_formOutput(ctx), _("Alert"));
    return (result);
}

static duk_ret_t duk_print(duk_context *ctx) {   // print
    duk_ret_t result = 0;
    extern JS_control_class JS_control;
    JS_control.m_pJSconsole->Show(); // make sure console is visible
    JS_control.m_pJSconsole->m_Output->AppendText(js_formOutput(ctx));
    return (result);
}

static duk_ret_t duk_read_text_file(duk_context *ctx) {  // read in a text file
    // read in a text file
    wxString fileNameGiven, fileString, lineOfFile, text;
    wxFileName filePath;
    wxTextFile inputFile;
    wxFileName homeDirectory;
    bool ok;
    wxString JScleanString(wxString line);

    fileNameGiven = duk_to_string(ctx,0);
    duk_pop(ctx);  // finished with that
    filePath = wxFileName(fileNameGiven);
    if (filePath.IsOk()){
        if (!filePath.FileExists()) {
            if (filePath.IsRelative()){  // Let's try relative to home directory
                homeDirectory.AssignHomeDir();
                fileString = homeDirectory.GetFullPath();
                filePath = wxFileName(homeDirectory.GetFullPath() + fileNameGiven);
                fileString = filePath.GetFullPath();
                }
            }
        if (!filePath.FileExists()) {  // try again
            JS_dk_error(ctx, "readTextFile " + filePath.GetFullPath() + " not found");
            }
        if (!filePath.IsFileReadable()) JS_dk_error(ctx, "readTextFile " + filePath.GetFullPath() + " not readable");
        }
    else JS_dk_error(ctx, "readTextFile " + filePath.GetFullPath() + " does not make sense");
    fileString = filePath.GetFullPath();
    ok = inputFile.Open(fileString);
    for (lineOfFile = inputFile.GetFirstLine(); !inputFile.Eof(); lineOfFile = inputFile.GetNextLine()){
        text += lineOfFile + "\n";
        }
    text = JScleanString(text);
    duk_push_string(ctx, text);
    return 1;
    };



duk_ret_t duk_require(duk_context *ctx){ // the module search function
    wxString fileNameGiven, fileString, lineOfData, script;
    wxFileName filePath;
    wxTextFile inputFile;
    wxFileName homeDirectory;
    wxString JScleanString(wxString line);
    bool ok;

    fileNameGiven = duk_to_string(ctx,0);
    duk_pop(ctx);  // finished with that
    filePath = wxFileName(fileNameGiven);
    if ((filePath.GetDirCount() == 0) && !filePath.HasExt()){
        // simple file name - we will look in the plugin library
        filePath = *GetpSharedDataLocation() +
            _T("plugins") + wxFileName::GetPathSeparator() +
            _T("JavaScript_pi") + wxFileName::GetPathSeparator() +
            _T("data") + wxFileName::GetPathSeparator() +
            _T("scripts") + wxFileName::GetPathSeparator() +
            fileNameGiven;
        if (!filePath.FileExists()){
                JS_dk_error(ctx, "require - " + filePath.GetFullName() + " not in scripts library");
            }
        if (!filePath.IsFileReadable()) JS_dk_error(ctx, "readTextFile " + filePath.GetFullPath() + " not readable");
        // ready to go
        }

    else{   // not a library script}
        if (filePath.IsOk()){
            if (!filePath.FileExists()) {
                if (filePath.IsRelative()){  // Let's try relative to home directory
                    homeDirectory.AssignHomeDir();
                    fileString = homeDirectory.GetFullPath();
                    filePath = wxFileName(homeDirectory.GetFullPath() + fileNameGiven);
                    fileString = filePath.GetFullPath();
                    }
                }
            if (!filePath.FileExists()) {  // try again
                JS_dk_error(ctx, "require - " + filePath.GetFullPath() + " not found");
                }
            if (!filePath.IsFileReadable()) JS_dk_error(ctx, "require - " + filePath.GetFullPath() + " not readable");
            }
        else JS_dk_error(ctx, "require - " + filePath.GetFullPath() + " does not make sense");
        }
    fileString = filePath.GetFullPath();
    ok = inputFile.Open(fileString);
    for (lineOfData = inputFile.GetFirstLine(); !inputFile.Eof(); lineOfData = inputFile.GetNextLine()){
        script += lineOfData + "\n";
        }
    script = JScleanString(script);
    duk_push_string(ctx, script);
    duk_push_string(ctx, "position");
    if (duk_pcompile(ctx, DUK_COMPILE_FUNCTION)){
        JS_dk_error(ctx, duk_safe_to_string(ctx, -1));
        }
    return(1);
    };


void duk_extensions_init(duk_context *ctx) {
    duk_idx_t duk_push_c_function(duk_context *ctx, duk_c_function func, duk_idx_t nargs);
    duk_push_global_object(ctx);

    duk_push_string(ctx, "print");
    duk_push_c_function(ctx, duk_print, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "alert");
    duk_push_c_function(ctx, duk_alert, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "readTextFile");
    duk_push_c_function(ctx, duk_read_text_file, 1 /* number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "require");
    duk_push_c_function(ctx, duk_require, 1 /* number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

//    duk_get_global_string(ctx, "Duktape");  // NB this one is quite different from the above!
//    duk_push_c_function(ctx, duk_mod_search, 4 /* number of arguments*/);
//    duk_put_prop_string(ctx, -2, "modSearch");


    duk_pop(ctx);
}
