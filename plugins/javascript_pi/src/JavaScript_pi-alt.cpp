/* *************************************************************************
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
 ***************************************************************************
 * This module draws on  DR_pi by Mike Rossiter
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "JavaScript_pi.h"
#include "JavaScriptgui_impl.h"
#include "JavaScriptgui.h"
#include "duktape.h"
#include "ocpn_duk.h"
#include <string>
#include "wx/string.h"

#define PLUGIN_VERSION_COMMENT ""


int messageIndex(MessagesArray&, messageNameString_t);

/* **** Warning *****
 MessagesArray is static, so multiple instances of Console will share it
 This is because when OCPN APIs are called from JS, I have found no way of determining the Console address
 */
// MessagesArray      Console::m_messages;

JS_control_class JS_control;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new JavaScript_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    JavaScript PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

void JSlexit(wxStyledTextCtrl* pane){  // lex the script window
        // markers
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
        pane->MarkerSetBackground (wxSTC_MARKNUM_FOLDER, wxColour (wxT("BLACK")));
        pane->MarkerSetForeground (wxSTC_MARKNUM_FOLDER, wxColour (wxT("WHITE")));
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
        pane->MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPEN, wxColour (wxT("BLACK")));
        pane->MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPEN, wxColour (wxT("WHITE")));
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
        pane->MarkerSetBackground (wxSTC_MARKNUM_FOLDEREND, wxColour (wxT("BLACK")));
        pane->MarkerSetForeground (wxSTC_MARKNUM_FOLDEREND, wxColour (wxT("WHITE")));
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
        pane->MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPENMID, wxColour (wxT("BLACK")));
        pane->MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPENMID, wxColour (wxT("WHITE")));
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
        pane->MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
        pane->SetMarginType (1, wxSTC_MARGIN_SYMBOL);
        pane->SetMarginMask (1, wxSTC_MASK_FOLDERS);
        pane->SetMarginWidth (1, 16);
        pane->SetMarginSensitive (1, true);
        pane->SetProperty( wxT("fold"), wxT("1") );
        pane->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
        pane->SetMarginWidth( 1, 0 );
        pane->SetMarginWidth( 2, 0 );
        pane->SetLexer(wxSTC_LEX_CPP);
        // supported keywords
        wxString keywordsSupported = "arguments\
            boolean break\
            case catch const continue\
            default delete do\
            else eval\
            false finally for function\
            if in\
            new null\
            package private protected public\
            return\
            switch\
            this throw true try typeof\
            var void\
            while";
        wxString keywordsUnsupported = "abstract\
            byte\
            char class\
            debugger double\
            enum\
            export extends\
            final float\
            goto\
            implements import instanceof int interface\
            of\
            let long\
            native\
            short static super synchronized\
            throws transient\
            volatile \
            with\
            yield";
    pane->SetKeyWords(0, keywordsSupported + keywordsUnsupported);

 //     wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        pane->StyleClearAll();
        pane->StyleSetBold(wxSTC_C_WORD, true);
        pane->StyleSetForeground(wxSTC_C_WORD, *wxBLUE);
        pane->StyleSetForeground(wxSTC_C_STRING, *wxRED);
        pane->StyleSetForeground(wxSTC_C_STRINGEOL, *wxRED);
 //     pane->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(49, 106, 197));
        pane->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
        pane->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
 //     pane->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 0));
        pane->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(0, 128, 0));
        pane->StyleSetForeground(wxSTC_C_NUMBER, *wxBLUE );
        pane->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        pane->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        pane->SetCaretWidth(2);
    }

JavaScript_pi::JavaScript_pi(void *ppimgr)
:opencpn_plugin_116 (ppimgr)  // was 18
{
#ifndef IN_HARNESS
    // Create the PlugIn icons
    initialize_images();

    wxString shareLocn = *GetpSharedDataLocation() +
    _T("plugins") + wxFileName::GetPathSeparator() +
    _T("JavaScript_pi") + wxFileName::GetPathSeparator()
    + _T("data") + wxFileName::GetPathSeparator();
    wxImage panelIcon(shareLocn + _T("JavaScript_pi_panel_icon.png"));
    if (panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon);
    else
        wxLogMessage(_T("    JavaScript_pi panel icon NOT loaded"));
#endif
    m_bShowJavaScript = false;
}

JavaScript_pi::~JavaScript_pi(void)
{
    delete _img_JavaScript_pi;
    delete _img_JavaScript;

}

int JavaScript_pi::Init(void)
{
    cout << "starting plugin init\n";
    AddLocaleCatalog( _T("opencpn-JavaScript_pi") );

    // Set some default private member parameters
    m_route_console_x = 0;
    m_route_console_y = 0;
    ::wxDisplaySize(&m_display_width, &m_display_height);

    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager console
    m_parent_window = GetOCPNCanvasWindow();

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();

    //    And load the configuration items
    LoadConfig();

    //    This PlugIn needs a toolbar icon, so request its insertion
    if(m_bJavaScriptShowIcon){

#ifndef IN_HARNESS
#ifdef JavaScript_USE_SVG
        m_leftclick_tool_id = InsertPlugInToolSVG(_T("JavaScript"), _svg_JavaScript, _svg_JavaScript, _svg_JavaScript_toggled,
                                                  wxITEM_CHECK, _("JavaScript"), _T(""), NULL, CALCULATOR_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_JavaScript, _img_JavaScript, wxITEM_CHECK,
                                           _("JavaScript"), _T(""), NULL,
                                           CALCULATOR_TOOL_POSITION, 0, this);
#endif // JavaScript_USE_SVG
#endif // IN_HARNESS
    }
    m_pConsole = NULL;


#ifdef IN_HARNESS
    cout << "finished plugin init\n";
#endif

    return (WANTS_TOOLBAR_CALLBACK |
            WANTS_CONFIG             |
            WANTS_PLUGIN_MESSAGING |
            WANTS_NMEA_SENTENCES |
            WANTS_NMEA_EVENTS
            );

}

bool JavaScript_pi::DeInit(void)
{
    //    Record the console position
    if (NULL != m_pConsole)
    {
        //Capture console position
        wxPoint p = m_pConsole->GetPosition();
        SetCalculatorConsoleX(p.x);
        SetCalculatorConsoleY(p.y);
        m_pConsole->Close();
        delete m_pConsole;
        m_pConsole = NULL;

        m_bShowJavaScript = false;
        SetToolbarItemState( m_leftclick_tool_id, m_bShowJavaScript );

    }

    SaveConfig();

    RequestRefresh(m_parent_window); // refresh mainn window

    return true;
}

int JavaScript_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int JavaScript_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int JavaScript_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int JavaScript_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *JavaScript_pi::GetPlugInBitmap()
{
    return &m_panelBitmap;
}

wxString JavaScript_pi::GetCommonName()
{
    return _("JavaScript");
}


wxString JavaScript_pi::GetShortDescription()
{
    return _("Providing JavaScript support");
}

wxString JavaScript_pi::GetLongDescription()
{
    return _("Allows running of JavaScript\n\
             provided in a file");
}

int JavaScript_pi::GetToolbarToolCount(void)
{
    return 1;
}

void JavaScript_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_pConsole)
        return;

    DimeWindow(m_pConsole);
}

void JavaScript_pi::OnToolbarToolCallback(int id)
{
    if(NULL == m_pConsole)
    {
        m_pConsole = new Console(m_parent_window, this);
        m_pConsole->Move(wxPoint(m_route_console_x, m_route_console_y));
        // script pane set up
        wxString welcome = wxString(_("print(\"Hello from the JavaScript plugin v")) << PLUGIN_VERSION_MAJOR << "." << PLUGIN_VERSION_MINOR <<  PLUGIN_VERSION_COMMENT << _(" !\\n\");\n\"All OK\";");
        m_pConsole->m_Script->AddText(welcome); // some initial script
        JSlexit(m_pConsole->m_Script);
        // output pane set up
        m_pConsole->m_Output->StyleSetSpec(STYLE_RED, _("fore:red"));
        m_pConsole->m_Output->StyleSetSpec(STYLE_BLUE, _("fore:blue"));
    }

    m_pConsole->Fit();
    m_pConsole->filePath = "";   // note we do not have a current file at this stage
    //Toggle
    m_bShowJavaScript = !m_bShowJavaScript;
    //   m_pConsole.save_button
    JS_control.m_timerBusy = false;

    //    Toggle console?
    if(m_bShowJavaScript) {
        m_pConsole->Show();
    } else
        m_pConsole->Hide();

    // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
    // to actual status to ensure correct status upon toolbar rebuild
    SetToolbarItemState( m_leftclick_tool_id, m_bShowJavaScript );

    RequestRefresh(m_parent_window); // refresh main window
}

bool JavaScript_pi::LoadConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(pConf)
    {
        pConf->SetPath ( _T( "/Settings/JavaScript_pi" ) );
        pConf->Read ( _T( "ShowJavaScriptIcon" ), &m_bJavaScriptShowIcon, 1 );

        m_route_console_x =  pConf->Read ( _T ( "ConsolePosX" ), 20L );
        m_route_console_y =  pConf->Read ( _T ( "ConsolePosY" ), 20L );

        if((m_route_console_x < 0) || (m_route_console_x > m_display_width))
            m_route_console_x = 5;
        if((m_route_console_y < 0) || (m_route_console_y > m_display_height))
            m_route_console_y = 5;
        return true;
    }
    else
        return false;
}

bool JavaScript_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(pConf)
    {
        pConf->SetPath ( _T ( "/Settings/JavaScript_pi" ) );
        pConf->Write ( _T ( "ShowJavaScriptIcon" ), m_bJavaScriptShowIcon );

        pConf->Write ( _T ( "ConsolePosX" ),   m_route_console_x );
        pConf->Write ( _T ( "ConsolePosY" ),   m_route_console_y );

        return true;
    }
    else
        return false;
}

void JavaScript_pi::OnJavaScriptConsoleClose()
{
    m_bShowJavaScript = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowJavaScript );
    m_pConsole->Hide();
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

}

/*  Experiment - works but clogs up screen with multiple message boxes
 void JavaScript_pi::SetCursorLatLon(double lat, double lon)
 {
 wxString text = "";
 text << "Lat:" << lat << " Lon:" << lon;
 wxMessageBox(text, "Cursor position");
 }
 */

void JavaScript_pi::SetNMEASentence(wxString &sentence)
{    // NMEA sentence received
    wxString thisFunction, checksum, correctChecksum;
    size_t starPos;
    bool OK;
    duk_context *ctx;
    wxStyledTextCtrl* output;
    auto ComputeChecksum{       // Using Lambda function here to keep it private to this function
        [](wxString sentence) {
            unsigned char calculated_checksum = 0;
            for(wxString::const_iterator i = sentence.begin()+1; i != sentence.end() && *i != '*'; ++i)
                calculated_checksum ^= static_cast<unsigned char> (*i);
            return( wxString::Format(_("%02X"), calculated_checksum) );
        }
    };
    OK = false;
    if (!JS_control.m_JSactive) return;     // Don't hang around if not looking for action
    thisFunction = JS_control.m_NMEAmessageFunction;  // function to be called
    if (thisFunction == wxEmptyString) return;  // not waiting for NMEA
    JS_control.m_NMEAmessageFunction = wxEmptyString; // call only once
    sentence.Trim();
    starPos = sentence.find("*"); // position of *
    checksum = sentence.substr(starPos + 1, starPos + 3);
    if (starPos != wxNOT_FOUND){ // yes there is one
        sentence = sentence.SubString(0, starPos-1); // truncate at * onwards
    }
    correctChecksum = ComputeChecksum(sentence);
    if (checksum == correctChecksum) OK = true;
    sentence = sentence.BeforeFirst('*');   // drop * onwards
    output = JS_control.m_pJSconsole->m_Output;  // the output window
 //   wxStreamToTextRedirector redirectJS_control(JS_control.m_pJSconsole->m_Output);  // redirect sdout to window pane
    ctx = JS_control.m_pctx;
    if (ctx != nullptr) {  // Only try this if we have an active context
        // cout << "NMEA function to call %" << thisFunction.c_str() << "%\n";
        if (!duk_get_global_string(ctx, thisFunction.c_str())){
            jsMessage(ctx, STYLE_RED, _("JavaScript NMEA function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
        }
        else {
            duk_push_object(ctx);
            duk_push_string(ctx, sentence.c_str());
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_boolean(ctx, OK);
            duk_put_prop_literal(ctx, -2, "OK");
            if (duk_pcall(ctx, 1)){  // NB returns zero on success
                jsMessage(ctx, STYLE_RED, _("JavaScript error on call to function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
            }
        }
        duk_pop(ctx);
    }
    else   wxMessageBox("Had a function to run but no context!", "JavaScript plugin logic error");
    if (!JS_control.waiting()) {
        JS_control.clear();
    }
    return;
}

void JavaScript_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{   // note the pfix for others use
    JS_control.m_positionFix = pfix;
}


void JavaScript_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    int index;
    int count;
    jsFunctionNameString_t thisFunction;
    wxString argument;
    duk_context *ctx;
    wxStyledTextCtrl* output;

    // message received - first we will check the timers
    if (!JS_control.m_timerBusy){
        if (!JS_control.m_times.IsEmpty()){
            JS_control.m_timerBusy = true;  // There is at least one timer running - stop being called again until we are done
            output = JS_control.m_pJSconsole->m_Output;  // the output window
            ctx = JS_control.m_pctx;
            count = (int)JS_control.m_times.GetCount();
            for (int i = count-1; i >= 0; i--){  // we work backwards down array so we can remove item and continue down
                if (JS_control.m_times[i].timeToCall <= wxDateTime::Now()){
                    // this one due
                    thisFunction = JS_control.m_times[i].functionName;
                    argument = JS_control.m_times[i].argument;
                    if (thisFunction != wxEmptyString){  //only if have valid function
                        ctx = JS_control.m_pctx;
                        if (ctx != nullptr) {  // Only try this if we have an active context
                            duk_bool_t ret = duk_get_global_string(ctx, thisFunction.c_str());
                            if (!ret) jsMessage(ctx, STYLE_RED, _T("function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
                            else {
                                duk_push_string(ctx, argument.c_str());
                                if (duk_pcall(ctx, 1)){  // NB returns zero on success
                                    jsMessage(ctx, STYLE_RED, _T("error on call to function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
                                    }
                                }
                            };
                        duk_pop(ctx);
                        }
                    else   wxMessageBox("Had a function to run on timer but no context!", "JavaScript plugin logic error");
                    JS_control.m_times.RemoveAt(i);
                    }
                }
            }
            if ((int)JS_control.m_times.GetCount()) JS_control.m_timerBusy = false;  // no more timers
            }

        //now to deal with the message
        index = JS_control.messageIndex(message_id);  // look up and remember if new
        if (!JS_control.m_JSactive) return;     // Don't hang around if not looking for action
        output = JS_control.m_pJSconsole->m_Output;  // the output window
        thisFunction = JS_control.m_messages[index].functionName;
        JS_control.m_messages[index].functionName = wxEmptyString;  // only one time
        if (thisFunction != wxEmptyString){
            // have function to be invoked
 //           wxStreamToTextRedirector redirectJS_control(JS_control.m_pJSconsole->m_Output);  // redirect sdout to our window pane
            ctx = JS_control.m_pctx;
            if (ctx != nullptr) {  // Only try this if we have an active context
                if (!duk_get_global_string(ctx, thisFunction.c_str())){
                    jsMessage(ctx, STYLE_RED, _("function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
                }
                else {
                    // fix problematic characters
                    // message_body.Replace(wxString("\n"),wxString("\\n"), true);  // escape newlines
                    // message_body.Replace(wxString("”"),wxString('"'), true);  // closing double quote
                    // message_body.Replace(wxString("’"),wxString("'"), true);  // fix prime
                    duk_push_string(ctx, message_body.c_str());
                    if (duk_pcall(ctx, 1)){  // NB returns zero on success
                        jsMessage(ctx, STYLE_RED, _("error on call to function ") + thisFunction + " ", duk_safe_to_string(ctx, -1));
                    }
                };
                duk_pop(ctx);
            }
            else   wxMessageBox("Had a function to run but no context!", "JavaScript plugin logic error");
        }
        if (!JS_control.waiting()) {
            JS_control.clear();
        }
        return;
    }
