/* ************************************************************************
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

#ifndef JavaScript_impl_h
#define JavaScript_impl_h

#include "wx/wx.h"
#include "wx/window.h"
#include "wx/string.h"
#include "wx/datetime.h"
#include "wx/arrimpl.cpp"
#include <wx/stc/stc.h>
#include "duktape.h"
#include "ocpn_plugin.h"
#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "trace.h"
#include <vector>
#include <bitset>

#define DUK_DUMP true
#if DUK_DUMP
#define MAYBE_DUK_DUMP duk_push_context_dump(ctx);cout<<"Duktape context dump:\n"<<duk_to_string(ctx, -1)+_T("\n");duk_pop(ctx);
//#define MAYBE_DUK_DUMP duk_push_context_dump(ctx);message(STYLE_ORANGE, _T(""), _T("Duktape context dump:\n")+duk_to_string(ctx, -1)+_T("\n"));duk_pop(ctx);
#define ERROR_DUMP {message(STYLE_ORANGE, _T(""), _T("error dump\n")+(duk_is_error(ctx, -1)?(_T("Error object with " )+duk_safe_to_string(ctx, -1)+_T("\n")):_T("No error object\n")));}\
MAYBE_DUK_DUMP
#endif  //  DUK_DUMP

using namespace std;
typedef wxString jsFunctionNameString_t;
typedef wxString messageNameString_t;

class MessagePair  // holds OPCN messages seen, together with JS callback function name, if to be called back
{
public:
    messageNameString_t messageName;
    jsFunctionNameString_t functionName;
};
WX_DECLARE_OBJARRAY(MessagePair, MessagesArray);

#if 0   // not used?
#define NUMBER_OF_MESSAGE_TYPES 1    no longer needed?

enum CallBackTypes
{
    // The call back types supported in the API
    NMEA_SENTENCES,
    PLUGIN_MESSAGING
};
#endif

// declare wxStyledText styles
enum {
    STYLE_BLACK,
    STYLE_RED,
    STYLE_BLUE,
    STYLE_ORANGE,
    STYLE_GREEN,
    STYLE_BOLD,
    STYLE_UNDERLINE
    };

// declare completion states
enum Completions {
    HAD_ERROR,  // avoid 'ERROR' - conflicts with "ERROR" defined in "wingdi.h", a Windows file
    DONE,
    STOPPED,
    MORE,
    TOCHAIN,
    CLOSE
    };

#define MAX_SENTENCE_TYPES 50    // safety limit in case of coding error
#include "wx/datetime.h"
class TimeActions  // holds times at which function is to be called
{
public:
    wxDateTime timeToCall;
    jsFunctionNameString_t functionName;
    wxString argument;  // optional argument to pass to function
};
#define MAX_TIMERS 10   // safety limit of timers

WX_DECLARE_OBJARRAY(TimeActions, TimesArray);

class ConsoleCallbackAwaited
{
public:
    Console*    pWaitedfrom;   // The console the callback is awaited from
    wxString    consoleName;    // name of console
    wxString    functionToCall; // the function to call when callback arrives
};

WX_DECLARE_OBJARRAY(ConsoleCallbackAwaited, ConsoleCallbackArray);

class ConsoleCallbackResult{
public:
    wxString        result;
    Completions     resultType;
    wxString        functionName;   // function to be called
};

// dialogues storage
class dialogElement{
public:
    wxString    type;
    wxString    stringValue;
    wxString    label;
    wxString    textValue;
    int         numberValue;
    int         width;
    int         height;
    int         multiLine;
    wxString    suffix;
    int         itemID; // The ID of the window element if anyitemID
    };

class DialogAction // hold details of open dialogue
{
public:
    jsFunctionNameString_t functionName;    // function to be called when dialogue acted on
    wxDialog    *pdialog;    // points to open dialog else nullptr
    wxPoint     position = wxPoint(wxDefaultPosition);
    std::vector<dialogElement> dialogElementsArray;// will be an array of dialogue elements
};

class AlertDetails // holds details of open dialogue
{
public:
    wxDialog    *palert;    // points to open alert else nullptr
    wxPoint     position = wxPoint(wxDefaultPosition);
    wxString    alertText;  // the currently displayed text
};

class Brief   // holds details of briefs and call-backs for this console
    {
public:
    bool        fresh; // this brief just posted - keep for collection
    bool        hasBrief;  // there is a brief in theBrief
    wxString    theBrief;
    bool        callback;   // true if callback to be made
    wxString    briefingConsoleName;
    wxString    function;   // function to be called back
    };

class Console : public m_Console
{
public:
    Console     *mpNextConsole {nullptr}; // -> next console in chained list of consoles
    wxString    mConsoleName;
    duk_context *mpCtx {nullptr};      // Pointer to the Duktape context
    bool        mWaitingCached = true; // true if mWaiting valid cached state
    bool        mWaiting {false};       // true if waiting for something
    bool        mRunningMain {false}; // true while main is running
    bool        mChainedScriptToRun {false};   // true if script to be run because it was chained
    status_t    mStatus;     // the status of this process
    

    // callback management
    bool        mTimerActionBusy;  // true while handling timer event to stop them piling up
    MessagesArray   mMessages;   // The messages call-back table
    TimesArray      mTimes;       // Timers call-back table
    DialogAction    mDialog;      // Dialogue call-back table
    AlertDetails    mAlert;        // details of alert dialog
    jsFunctionNameString_t  m_NMEAmessageFunction;  // function to invoke on receipt of NMEA message, else ""
    jsFunctionNameString_t  m_activeLegFunction;  // function to invoke on receipt of active leg, else ""
    jsFunctionNameString_t m_exitFunction;  // function to call on exit, else ""
    int         mConsoleCallbacksAwaited {0};   // number of console callbacks awaited

    // duktape timeout handling
    wxLongLong m_pcall_endtime;         // time we must end execution (msecs since 1970)
    long       m_time_to_allocate;      // the time we allocate (msec)
    long       m_timeout_check_counter; // counter of number of timeout checks
    bool       m_backingOut;            // true while backing out following timeout
    
    // result handling - can be made explicit by scriptResult or stopScript
    bool        m_explicitResult;    // true if using explicit result
    wxString    m_result {wxEmptyString};   // the result if explicit
    bool        m_hadError;     // true if unwinding from dukpate after throing error
    Brief       mBrief;         // brief for this console
    
    // file handling
    wxString    mFileString;   // file loaded/saved, if any
    wxString    mFolderPath;    //  of current folder
    wxFileName  mCurrentFile;
    bool        m_autoRun;      // true if script to be auto run after activation of plugin
    bool        mWaitingToRun {false};  // true if an auto-run script has been loaded but not yet run

    void OnClearScript( wxCommandEvent& event );
    void OnCopyAll( wxCommandEvent& event );
    void OnLoad( wxCommandEvent& event );
    void OnSave (wxCommandEvent& event);
    void OnSaveAs ( wxCommandEvent& event );
    void OnRun( wxCommandEvent& event );
    void OnAutoRun(wxCommandEvent& event);
    void OnClearOutput( wxCommandEvent& event );
    void OnTools( wxCommandEvent& event );
    void onActivate( wxActivateEvent& event );
    void OnMouse(wxMouseEvent& event);
    void OnActivate(wxActivateEvent& event);
    
private:
    void OnClose( wxCloseEvent& event );
    
public:
    Console(wxWindow *parent, wxString consoleName, wxPoint consolePosition = wxPoint(300,20), wxPoint dialogPosition = wxPoint(150, 100), wxPoint alertPosition = wxPoint(90, 20), wxString fileString = wxEmptyString, bool autoRun = false): m_Console(parent, wxID_ANY, consoleName, consolePosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxCLOSE_BOX|wxMINIMIZE|wxSYSTEM_MENU)
        {
        void JSlexit(wxStyledTextCtrl* pane);
        extern JavaScript_pi *pJavaScript_pi;
        Console *pConsole;
        wxPoint checkPointOnScreen(wxPoint point);
//        TRACE(3, "Constructing console " + consoleName);
   
        // hook this new console onto end of chain of consoles
        pConsole = pJavaScript_pi->mpFirstConsole;
        if (pConsole == nullptr) pJavaScript_pi->mpFirstConsole = this;   // is first and only
        else {
            while (pConsole->mpNextConsole) pConsole = pConsole->mpNextConsole; // point to last
            pConsole->mpNextConsole = this; // add us
            }

#ifndef IN_HARNESS
        //wxString iconLocation = *GetpSharedDataLocation()
       //+ _T("plugins/JavaScript_pi/data/blank.ico");
       // wxIcon icon(iconLocation, wxBITMAP_TYPE_ICO);
       // SetIcon(icon);
#endif

        mConsoleName = consoleName;
        mpCtx = nullptr;
        Move(wxPoint(checkPointOnScreen(consolePosition)));
        mDialog.position = checkPointOnScreen(dialogPosition);
        mDialog.pdialog = nullptr;
        mAlert.position = checkPointOnScreen(alertPosition);
        mAlert.palert = nullptr;
        m_fileStringBox->SetValue(fileString);
        auto_run->SetValue(autoRun);
        mWaitingToRun = false;
        mBrief.hasBrief = false;
        mBrief.theBrief = wxEmptyString;
        mBrief.callback = false;

        mBrief.briefingConsoleName = wxEmptyString;
        // output pane set up
        m_Output->StyleSetSpec(STYLE_RED, _T("fore:#FF0000"));
        m_Output->StyleSetSpec(STYLE_BLUE, _T("fore:#2020FF"));
        m_Output->StyleSetSpec(STYLE_ORANGE, _T("fore:#FF7F00"));
        m_Output->StyleSetSpec(STYLE_GREEN, _T("fore:#00CE00"));
        m_Output->StyleSetSpec(STYLE_BOLD, _T("bold"));
        m_Output->StyleSetSpec(STYLE_UNDERLINE, _T("underline"));
        // wrap text in output pane
        m_Output->SetWrapMode(wxSTC_WRAP_WORD);
        m_Output->SetWrapIndentMode(wxSTC_WRAPINDENT_INDENT);
        m_Output->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);


        // script pane set up
        Fit(); // fit now to keep space for Autorun button, even if initially hidden
        JSlexit(this->m_Script);  // set up lexing
        // wrap text in script pane
        m_Script->SetWrapMode(wxSTC_WRAP_WORD);
        m_Script->SetWrapIndentMode(wxSTC_WRAPINDENT_INDENT);
        m_Script->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);

        auto_run->Hide();
        if (m_fileStringBox->GetValue() != wxEmptyString) {    // we have a script to load
            wxString    script;
            wxString    fileString = this->m_fileStringBox->GetValue();
            if (!wxFileExists(fileString)){
                message(STYLE_RED, _("Load file " + fileString + ": ") +_("File does not exist"));
                auto_run->SetValue(false);
                }
            else {
                m_Script->LoadFile(fileString);
                auto_run->Show();
                mWaitingToRun = this->auto_run->GetValue();
                }
            }
        else {
            wxString welcome = wxString(_("print(\"Hello from the JavaScript plugin v")) << PLUGIN_VERSION_MAJOR << "." << PLUGIN_VERSION_MINOR <<  " patch " << PLUGIN_VERSION_PATCH  << " " << PLUGIN_VERSION_DATE << " " << PLUGIN_VERSION_COMMENT << _("\\n\");\n\"All OK\";");
            m_Script->AddText(welcome); // some initial script
            }
        Hide();   // we hide console now but this may be changed later
        }
    
    Console *clearAndUnhook(){  //  Clear down and unhook console prior to deleting
        extern JavaScript_pi *pJavaScript_pi;
        Console *pConsole, *pPrevConsole;
//        TRACE(3,"Unhooking console " + this->mConsoleName);
        
        mStatus = 0; // clear out all other reasons
        mMessages.Clear();    // also delete the messages array
        mConsoleCallbacksAwaited = 0;
        wrapUp(CLOSE);
        // unhook ourself from the chained list of consoles
        if (!pJavaScript_pi->mpFirstConsole)
            message(STYLE_RED, "Logic error ClearAndUnhook called with no first console");   // should never be
        if (this == pJavaScript_pi->mpFirstConsole){
            // we are the first on the chain
            pJavaScript_pi->mpFirstConsole = this->mpNextConsole;   // unhook ourself
            }
        else {
            // we will start at the second on the chain
            pPrevConsole = pJavaScript_pi->mpFirstConsole;
            for (pConsole = pPrevConsole->mpNextConsole; pConsole != nullptr;
                        pPrevConsole = pConsole, pConsole = pConsole->mpNextConsole){
                if (this == pConsole){
                    // this is us
                    pPrevConsole->mpNextConsole = this->mpNextConsole;
                    break;
                }
            }
        }
        this->mpNextConsole = nullptr;
//        TRACE(3,"Done unhooking console " + this->mConsoleName);
        return this;
    }
    
    void clearBrief(){  // initialise brief by clearing it down
        mBrief.hasBrief = mBrief.fresh = mBrief.callback = false;
        mConsoleCallbacksAwaited = 0;
        }
    
  Completions run(wxString script) { // compile and run the script
        Completions outcome;       // result of JS run
        bool more;    // true if more work by call-backs to functions
        extern bool runLable, stopLabel;
        wxString result;
        void fatal_error_handler(void *udata, const char *msg);
        void duk_extensions_init(duk_context *ctx);
        void ocpn_apis_init(duk_context *ctx);
        void ConsoleHandle_init(duk_context *ctx);
        wxString getStringFromDuk(duk_context *ctx);
        
//        TRACE(3, this->mConsoleName + "->run() entered");
        if (script == wxEmptyString) return HAD_ERROR;  // ignore
        
        // initialise
        mStatus = 0;
        mWaitingCached = false;
        m_explicitResult = false;
        m_result = wxEmptyString;
        m_hadError = false;
        mRunningMain = true;
        m_time_to_allocate = 1000;   //default time allocation (msecs)
        mTimerActionBusy = false;
        m_backingOut = false;
        mDialog.pdialog = nullptr;
        mDialog.functionName = wxEmptyString;
        mAlert.palert = nullptr;
        mAlert.alertText = wxEmptyString;
        mConsoleCallbacksAwaited = 0;
        m_exitFunction = wxEmptyString;
        if (mAlert.position.y == -1){ // shift initial position of alert  away from default used by dialogue
            mAlert.position.y = 150;
            }
        // ready to go
        mpCtx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_error_handler);  // create the Duktape context
        if (!mpCtx) {
        	message(STYLE_RED, _T("Plugin logic error: Duktape failed to create heap"));
            return HAD_ERROR;
            }
        duk_extensions_init(mpCtx);  // register our own extensions
        ocpn_apis_init(mpCtx);       // register our API extensions
        run_button->SetLabel(_T("Stop"));
        more = false;      // start with no call-backs - will be set true in 'on' callback APIs
        duk_push_string(mpCtx, script.mb_str());   // load the script
        startTimeout();
        duk_int_t dukOutcome = duk_peval(mpCtx);    // run code **** this is where it all happens ****
        clearTimeout();
        mRunningMain = false;
       // if certain flags set, it will have thown an error
       // we check for this before other types of error
      // For now we will mark not busy in case the following are true
        mWaiting = false;
        mWaitingCached = true;
       if (mStatus.test(STOPPED)) return STOPPED;
       if (mStatus.test(TOCHAIN)) return TOCHAIN;
       if (dukOutcome != DUK_EXEC_SUCCESS){
            wxString formErrorMessage(duk_context *ctx);
            m_result = formErrorMessage(mpCtx);
            duk_pop(mpCtx);
            return HAD_ERROR;
            }
       mWaitingCached = false;   // now we are past those cases, get a re-check
        result = getStringFromDuk(mpCtx);
        if (!m_explicitResult) m_result = result; // for when not explicit result
        duk_pop(mpCtx);  // pop result
        outcome = DONE; // starting assumption
        
        if (isWaiting()){ // if script set up call-backs with nominated functions, check they exist
            outcome = MORE;
//            TRACE(4, mConsoleName + "->run()  completed with something to wait for");
            duk_push_global_object(mpCtx);  // get our compiled script
            if (
                functionMissing(m_NMEAmessageFunction) ||
                functionMissing(mDialog.functionName) ||
                functionMissing(m_activeLegFunction) )
                outcome = HAD_ERROR;;
            if (!mTimes.IsEmpty()){
                // has set up one or more timers - check them out
                int count = (int)mTimes.GetCount();
                for (int i = 0; i < count; i++)
                    if (functionMissing(mTimes[i].functionName)) outcome = HAD_ERROR;
                }
            duk_pop(mpCtx);   // the global object
            }
//       TRACE(4, wxString::Format("%s->run() completed with outcome %d",mConsoleName, outcome ));
        return outcome;
        }
    
    void doRunCommand(Brief brief){
        // this is implemented as a method so we can lazy call with CallAfter
        if (run_button->GetLabel() == _("Run")){
            Completions outcome;
            mBrief = brief;
            outcome = run(m_Script->GetText());
            if (isBusy()) return;   // callbacks etc to do
            wrapUp(outcome);
            }
        else { // Stop button clicked - we have a script running - kill it off
//            TRACE(4, mConsoleName + " script stopped");
            m_explicitResult = true;
            m_result = _("script was stopped");
            mStatus = 0;
            wrapUp(STOPPED);
            }
    }
    
    Completions executeFunction(wxString function){
        // execute function wih single argument already on duktape stack
        // returns result with nothing on stack
        duk_int_t outcome;
        duk_context *ctx = mpCtx;
        outcome = duk_get_global_string(ctx, function.c_str());
        if (!outcome){ // failed to find the function
//            TRACE(14, "executeFunction failed to find function");
            m_result = _T("Logic error: ") + mConsoleName + _T("->executeFunction - no function ") + function;
            duk_pop(ctx);
            return HAD_ERROR;
            }
        duk_swap_top(ctx, -2); // the argument needs to be above the function
        Console *findConsoleByCtx(duk_context *ctx); // ******
//        TRACE(8, "Before call " + findConsoleByCtx(ctx)->dukDump());
        startTimeout();
        outcome = duk_pcall(ctx, 1);   // single argument
        clearTimeout();
//        TRACE(8, "After call " + findConsoleByCtx(ctx)->dukDump() + _("Result is ") + ((outcome == DUK_EXEC_SUCCESS)?"success":"error"));
        if (mStatus.test(STOPPED)){
            duk_pop(ctx); // result
            mStatus.reset(MORE);
            return STOPPED;
            }
        if (mStatus.test(TOCHAIN)){
            duk_pop(ctx); // result
            mStatus.reset(MORE);
            return TOCHAIN;
            }
        if (outcome != DUK_EXEC_SUCCESS){
             wxString formErrorMessage(duk_context *ctx);
             m_result = formErrorMessage(mpCtx);
             duk_pop(mpCtx);
             return HAD_ERROR;
            }
        duk_pop(ctx);  // result
        mWaitingCached = false; // function may have changed something
        return isWaiting()?MORE:DONE;
        }
    
    void doExecuteFunction(Console* pFromConsole, ConsoleCallbackResult result){
        // this is implemented as a method so we can lazy-call a fuction with CallAfter
        wxString functionName;
        ConsoleCallbackAwaited thisConsoleCallback;
//        TRACE(4, mConsoleName + "->doExecuteFunction " + result.functionName + " with " + result.result + " reason " + ((result.resultType == HAD_ERROR)?"HAD_ERROR":((result.resultType == MORE)?"MORE":((result.resultType == STOPPED)?"STOPPED":((result.resultType == DONE)?"DONE":"unknown")))));
#if 0   // Not implementing automatic throw of calling console
        if (result.resultType == HAD_ERROR){
            // the other script threw an error - so we will simulate this for the invoking script
            m_result = _T("onConsoleResult -") +_T(" other script threw error ") + result.result;
            wrapUp(HAD_ERROR);
            return;
            }
#endif
        mConsoleCallbacksAwaited--;
#if 0   // Passing error through to function, so not here
        if (result.resultType == HAD_ERROR){
            message(STYLE_RED, _T("onConsoleResult - Console ") + thisConsoleCallback.consoleName + _T("threw error ") + result.result);
            wrapUp(HAD_ERROR);
            }
#endif
        // ready to invoke function and give it an object we will construct here
        functionName = result.functionName;
        duk_push_object(mpCtx);
        duk_push_int(mpCtx, result.resultType);
        duk_put_prop_literal(mpCtx, -2, "type");
        duk_push_string(mpCtx , result.result.c_str());
        duk_put_prop_literal(mpCtx, -2, "value");
        Completions outcome = executeFunction(functionName);
        if (isBusy()) return;   // callbacks etc to do
        // This script has finished
        wrapUp(outcome);
        if (mStatus.test(TOCHAIN)){
            // we are to run the script that has been loaded into our script window
            // we use CallAfter so present action can complete and unwind
            CallAfter(&Console::doRunCommand, mBrief);
            }
        }
    
    void startTimeout(){
        extern Console* pConsoleBeingTimed;
        m_timeout_check_counter = 0;
        m_pcall_endtime =  wxGetUTCTimeMillis() + m_time_to_allocate;
        pConsoleBeingTimed = this;
        }
    
    void clearTimeout() {   // to cancel the timeout
        extern Console* pConsoleBeingTimed;
        m_pcall_endtime = 0;
        pConsoleBeingTimed = nullptr;
        }
    
    void setWaiting(){
        // mark that we have set something that waits on a response
//        TRACE(5,mConsoleName + "->setWaiting()");
        mWaitingCached = mWaiting = true;
        }
    
    bool isWaiting()  // returns true if waiting for anything
    {
        // for efficiencey, we may have a cached result in mWaiting
        if (mWaitingCached) {
//            TRACE(15,mConsoleName + "->isWaiting() returning cached result " + (mWaiting?"true":"false"));
            return mWaiting;
            }
//        TRACE(15,mConsoleName + "->isWaiting() doing full check");
        bool result = false;
        int count;
        if (
            (mTimes.GetCount() > 0) ||
            (m_NMEAmessageFunction != wxEmptyString) ||
            (m_activeLegFunction != wxEmptyString) ||
            (mDialog.pdialog != nullptr) ||
            (mAlert.palert != nullptr) )
                result = true;
        else if ((count = (int)mMessages.GetCount()) > 0){ // look at messages
             for(unsigned int index = 0; index < count; index++){
                if (mMessages[index].functionName != wxEmptyString) {
                    result = true;
                    break;
                    }
                }
            }
        else if (mConsoleCallbacksAwaited > 0) result = true;
        
        mWaiting = result;
        mWaitingCached = true;
//        TRACE(5,mConsoleName + "->isWaiting() returning new result " + (mWaiting?"true":"false"));
        return mWaiting;
    }
    
    bool isBusy(){  // tests if wating or running main script
        return(mRunningMain || isWaiting());
        }
    
    void wrapUp(Completions reason) {    // clears down and destroys context etc.
        // reason is or'd into existing status and actions taken accordingly
        wxString statusesToString(status_t mStatus);
        mStatus.set(reason);
//        TRACE(4, wxString::Format("%s->wrapUp() status %d and now %s",mConsoleName, reason, statusesToString(mStatus)));
#if TRACE_LEVEL>0
        {wxString dump {""};
            if (mBrief.hasBrief || mBrief.callback){
                dump += "mBrief\n";
                dump += "\tfresh:\t\t\t" + (mBrief.fresh?_T("true"):_T("false")) + "\n";
                dump += "\thasBrief:\t\t\t" + (mBrief.hasBrief?_T("true"):_T("false")) + "\n";
                dump += "\ttheBrief:\t\t\t" + mBrief.theBrief + "\n";
                dump += "\tcallback:\t\t\t" + (mBrief.callback?_T("true"):_T("false")) + "\n";
                dump += "\tconsole:\t\t\t" + mBrief.briefingConsoleName + "\n";
                dump += "\tfunction:\t\t\t" + mBrief.function + "\n";
            }
            else dump += "No brief\n";
            TRACE(4, dump);
        }
#endif
        wxString runLabel="Run";
        this->Show();
        if (m_backingOut) message(STYLE_RED, "Script timed out");
        else if (mStatus.test(HAD_ERROR)) this->message(STYLE_RED,  this->m_result);
        else {
            if (!this->m_explicitResult && this->m_result == wxEmptyString)this->m_result = "undefined";
            if (!this->m_explicitResult || this->m_result != wxEmptyString)
            this->message(STYLE_BLUE, _T("result: ") + this->m_result);
            }
        run_button->SetLabel(runLabel);
        
        // clear any non-console callbacks set up
        m_NMEAmessageFunction = wxEmptyString;
        m_activeLegFunction = wxEmptyString;
        size_t messageCount = mMessages.GetCount();
        if (messageCount > 0){
            for(unsigned int index = 0; index < messageCount; index++){
                mMessages[index].functionName = wxEmptyString;
                }
            }
        // clear out all timer stuff
        // while (mTimes.GetCount() > 0) mTimes.RemoveAt(0); not needed?
        mTimes.Clear();
        mConsoleCallbacksAwaited = 0;
        mTimerActionBusy = false;
        mRunningMain = false;
        clearDialog();
        clearAlert();
        mWaitingCached = mWaiting = false;
        if (!mBrief.fresh) mBrief.hasBrief = false; // was only available once

        if (reason == TOCHAIN){  // a new script will have been loaded to chain to and any brief set
//            TRACE(4, wxString::Format("%s->wrapUp() chaining",mConsoleName));
            mStatus.reset(TOCHAIN);
            CallAfter(&Console::doRunCommand, mBrief);
            }
        // if not chaining, maybe we have to do a callback to another console?
        else if (mBrief.callback){
//            TRACE(4, wxString::Format("%s->wrapUp() calling back",mConsoleName));
            Console* findConsoleByName(wxString name);
            Console* pConsole = findConsoleByName(mBrief.briefingConsoleName);
            ConsoleCallbackResult resultStruct;
            
            mBrief.callback = false;    // we only do this once
            if (!pConsole) message(STYLE_RED, "Console to call back not found");
            else if (!pConsole->isWaiting()) message(STYLE_RED, "Console to call back not waiting for us");
            else {
                resultStruct.result = m_result;
                resultStruct.resultType = reason;
                resultStruct.functionName = mBrief.function;
//                TRACE(4, wxString::Format("Calling back to console %s function %s with result %s & reason %d", mBrief.briefingConsoleName, resultStruct.functionName, resultStruct.result, resultStruct.resultType));
                pConsole->CallAfter(&Console::doExecuteFunction , this, resultStruct);
                }
            }
        else {
//            TRACE(4, wxString::Format("%s->wrapUp() Neither chaining nor calling back",mConsoleName));
            if (m_exitFunction != wxEmptyString){
                duk_int_t outcome;
                // we have an onExit function to run
                duk_push_string(mpCtx, ""); // next bit expects an argument - put anything there
                outcome = executeFunction(m_exitFunction);
                m_exitFunction = wxEmptyString; // only once
                if (outcome == HAD_ERROR){
                    // error during exit function
                    this->message(STYLE_RED,  this->m_result);
                    }
                // Just in case the script tried to set up call-backs, clear them down again
                mTimes.Clear();
                clearDialog();
                clearAlert();
                }
            }
        if (mpCtx != nullptr) { // for safety - nasty consequences if no context
            duk_destroy_heap(mpCtx);
            mpCtx = nullptr;
            }
        }
    
    void clearAlert(){  // clears away any alert dialogue
//        TRACE(4,this->mConsoleName + "->clearAlert()  " + ((this->mAlert.palert == nullptr)?"(None)":"Alert exists"));
        if (mAlert.palert != nullptr){ // only if have an alert displayed
            mAlert.position = mAlert.palert->GetPosition(); // remember alert position
            mAlert.palert->Close();
            mAlert.palert->Destroy();
            delete mAlert.palert;
            mAlert.palert = nullptr;
            mAlert.alertText = wxEmptyString;
            mWaitingCached = false;
            }
        }
    
    void clearDialog(){ // clear away any open dialogue
//        TRACE(4,this->mConsoleName + "->clearDialog()  " + ((this->mDialog.pdialog == nullptr)?"(None)":"Dialog exists"));
        if (mDialog.pdialog != nullptr){
            mDialog.position = mDialog.pdialog->GetPosition();   // remember where it is
            mDialog.dialogElementsArray.clear();
            mDialog.pdialog->Close();
            mDialog.pdialog->Destroy();
            delete mDialog.pdialog;
            mDialog.pdialog = nullptr;
            mDialog.functionName = wxEmptyString;
            mWaitingCached = false;
            }
        }
    
    void throw_error(duk_context *ctx, wxString message){
        // throw an error from within c++ code called from running script
        // either there is an error object on the stack or a message
        // ! do not call otherwise
//        TRACE(4, mConsoleName + "->throw_error() " + message);        m_result = wxEmptyString;    // supress result
        m_explicitResult = true;    // supress result
        if (!duk_is_error(ctx, -1)){
            // we do not have an error object on the stack
            duk_push_error_object(ctx, DUK_ERR_ERROR, _T("Console ") + mConsoleName + _T(" - ") + message);  // convert message to error object
            }
        m_hadError = true;
        duk_throw(ctx);
        mRunningMain = false;
        }
    
    void message(int style, wxString message){
//        TRACE(5,mConsoleName + "->message() " + message);
        Show(); // make sure console is visible
        wxStyledTextCtrl* output_window = m_Output;
        int long beforeLength = output_window->GetTextLength(); // where we are before adding text
        output_window->AppendText(message);
        output_window->AppendText("\n");
        int long afterLength = output_window->GetTextLength(); // where we are after adding text
        output_window->StartStyling((int)beforeLength,0);   // 2nd parameter included Linux still using wxWidgets v3.0.2
        output_window->SetStyling((int)(afterLength-beforeLength-1), style);
        }
    
    void display_error(duk_context *ctx, wxString message){
        // display an error NOT from running script
        // either there is an error object on the stack or a message
        m_explicitResult = true;    // supress result
        m_result = duk_safe_to_string(ctx, -1);
        m_hadError = true;
        duk_pop(ctx);
        this->message(STYLE_RED, _T("From display_error ") +  m_result);
        }

    int OCPNmessageIndex(messageNameString_t thisMessage){
        // Returns index number for given sentence
        // If unknown sentence, adds it to array, sorts it into place and returns its index
        MessagePair newPair;
        int messageComp(MessagePair** arg1, MessagePair** arg2);
        
        size_t messageCount = mMessages.GetCount();
        unsigned int index = 0;
        if (messageCount == 0){
            // it's empty
            newPair.messageName = thisMessage;
            newPair.functionName = wxEmptyString;
            mMessages.Add(newPair);
            return(0);  // this is first and only entry, so index is 0
            }
        else { // have existing entries - search for match in sorted list
            for (index = 0; index < messageCount; index++)
                {
                    int cmp = strcmp(thisMessage, mMessages[index].messageName);
                    if (cmp > 0) continue;  // look further
                    else if (cmp == 0) return(index);
                    else break;             // no need to look further as list is sorted
                };
            }
        if (index == messageCount){  // no match
            if (index >= MAX_SENTENCE_TYPES){
                // should not get here unless something wrong
                wxMessageBox(_T("Number of message types exceeding safety limit"), _T("JavaScript plugin - lookupfunction"));
                return(-1);
                }
        }
        // did not find a match, so add a new entry
        newPair.messageName = thisMessage;
        newPair.functionName = wxEmptyString;
        mMessages.Add(newPair);
        mMessages.Sort(messageComp);  // This likely to move new entry so look up index again
        messageCount = mMessages.GetCount();  // update
        for (index = 0; index < messageCount; index++) // NB condition == here to allow for new entry
            {
            if ((mMessages[index].messageName == thisMessage)) return(index);
            }
        // should never get here
        message(STYLE_RED, _("Failed to add new sentence - programming error") + ("JavaScript plugin - messageIndex function"));
        return(-1);
        }
            
    wxString getMessagesString()        // returns string containing list of all messages
    {
        wxString result = wxEmptyString;
        wxString functionPart;
        
        size_t count = this->mMessages.GetCount();
        if (count > 0){
            for (unsigned int i = 0; i < count; i++){
                result += mMessages[i].messageName;
                functionPart = mMessages[i].functionName;
                if (functionPart != wxEmptyString)
                    result += _T(":") + functionPart;
                result += _T("\n");
            }
        }
        else result = _T("<nil>\n");
        return (result);
    }

    void setTimedCallback(jsFunctionNameString_t functionName, wxString argument, wxDateTime timeToCall){
        // create a callback entry to call fuction with argument at timeToCall
        size_t timersCount = this->mTimes.GetCount();
        if (timersCount >= MAX_TIMERS){
            // should not get here unless something wrong
            wxMessageBox(_T("Number of timers exceeding safety limit"), _T("JavaScript plugin - setTimedCallback"));
            return;
            }
        TimeActions newAction;
        newAction.functionName = functionName;
        newAction.argument = argument;
        newAction.timeToCall = timeToCall;
        mTimes.Add(newAction);
        }
    
    void destroyConsole(){
        Destroy();
    };
    
    void bin(){
        // move console to bin
        extern JavaScript_pi *pJavaScript_pi;
        Console *pConsole, *pFreedConsole;
        pFreedConsole = this->clearAndUnhook();
        pConsole = pJavaScript_pi->mpBin;
        if (pConsole == nullptr) pJavaScript_pi->mpBin = pFreedConsole;
        else {  // there is one already in the bin - push freed one on the front
            pFreedConsole->mpNextConsole = pJavaScript_pi->mpBin;
            pJavaScript_pi->mpBin = pFreedConsole;
            }
        pFreedConsole->Hide();
    }
    
    wxString consoleDump(){    // returns string being dump of selected information from console structure
        wxString ptrToString(Console* address);
        wxString statusesToString(status_t mStatus);
        int i, count;
        wxString dump {""};
        dump += "Console name:\t\t" + mConsoleName + "\n";
        dump += "this:\t\t\t\t\t" +  ptrToString((Console *)this) + "\n";
        dump += "mpNextConsole:\t\t" + ptrToString((Console *)mpNextConsole) + "\n";
        dump += "mStatus:\t\t\t\t" + statusesToString(mStatus) + "\n";
        dump += "mRunningMain:\t\t" + (this->mRunningMain?_("true"):_("false")) + "\n";
        dump += "m_backingOut:\t\t" + (this->m_backingOut?_("true"):_("false")) + "\n";
        dump += "mWaitingCached:\t\t" + (this->mWaitingCached?_("true"):_("false")) + "\n";
        dump += "mWaiting:\t\t\t" + (this->mWaiting?_("true"):_("false")) + "\n";
        dump += "isBusy():\t\t\t\t" + (this->isBusy()?_("true"):_("false")) + "\n";
        dump += "isWaiting():\t\t\t" + (this->isWaiting()?_("true"):_("false")) + "\n";
        dump += "auto_run:\t\t\t\t" + (this->auto_run ->GetValue()? _("true"):_("false")) + "\n";
        dump += _T("Messages callback table\n");
        count =(int)this->mMessages.GetCount();
        if (count == 0) dump += _T("\t(empty)\n");
        else {
            for (i = 0; i < count; i++) dump += _T("\t") + this->mMessages[i].messageName +_T("\t\t") +
            this->mMessages[i].functionName + _T("\n");
            }
        dump += "m_timerActionBusy:\t" + (this->mTimerActionBusy?_("true"):_("false")) + "\n";
        dump += _T("Timers callback table\n");
        count = (int)this->mTimes.GetCount();
        if (count == 0) dump += _T("\t(empty)\n");
        else {
            for (i = 0; i < count; i++){
                dump += _T("\t") + mTimes[i].timeToCall.FormatTime() + "\t" + this->mTimes[i].functionName +_T("\t") + this->mTimes[i].argument + _T("\n");
                }
            }
        dump += "m_dialog:\t\t\t" + ((this->mDialog.pdialog == nullptr)?_T("None"):wxString::Format("Active with %d elements",  this->mDialog.dialogElementsArray.size()) ) + "\n";
        dump += "m_alert:\t\t\t\t" + ((this->mAlert.palert == nullptr)?_("None"):_("Active")) + "\n";
        dump += "m_NMEAmessageFunction:\t\t" + m_NMEAmessageFunction + "\n";
        dump += "m_activeLegFunction:\t\t" + m_activeLegFunction + "\n";
        dump += "m_exitFunction:\t" + m_exitFunction + "\n";
        dump += "m_explicitResult:\t\t" + (m_explicitResult?_T("true"):_("false")) + "\n";
        dump += "m_result:\t\t\t\t" + m_result + "\n";
        dump += "mChainedScriptToRun:\t" + (this->mChainedScriptToRun?_("true"):_("false")) + "\n";
        if (mBrief.hasBrief || mBrief.callback){
            dump += "mBrief\n";
            dump += "\tfresh:\t\t\t" + (mBrief.fresh?_("true"):_("false")) + "\n";
            dump += "\thasBrief:\t\t\t" + (mBrief.hasBrief?_("true"):_("false")) + "\n";
            dump += "\ttheBrief:\t\t\t" + mBrief.theBrief + "\n";
            dump += "\tcallback:\t\t\t" + (mBrief.callback?_("true"):_("false")) + "\n";
            dump += "\tconsole:\t\t\t" + mBrief.briefingConsoleName + "\n";
            dump += "\tfunction:\t\t\t" + mBrief.function + "\n";
            }
        else dump += "No brief\n";
        dump += wxString::Format("mConsoleCallbacksAwaited\t%d\n", mConsoleCallbacksAwaited);
        dump+= dukDump();
        return dump;
        }
    
    wxString dukDump(){ // returns string being dump of duktape context
        if (!mpCtx) return "No Duktape context\n";
        else {
            wxString result;
            duk_push_context_dump(mpCtx);
            result = wxString(_("Duktape context dump:\n") + duk_to_string(mpCtx, -1) + _("\n"));
            duk_pop(mpCtx);
            return result;
        }
    }
    
private:
    bool functionMissing(wxString function){ // checks if function missing in global object
        // global object must be on stack when this is called
        if (function == wxEmptyString) return false;
        if (!duk_get_prop_string(mpCtx, -1, function.c_str())){  // Does function exist in there?
            duk_pop(mpCtx);   // pop off function or undefined
            m_result = "call-back function " + function + " not provided";
//                display_error(mpCtx, "call-back function " + function + " not provided");
            return true;
            }
        duk_pop(mpCtx);   // pop off function or undefined
        return false;
        }

void consoleInit(){
    // initialise various attributes of a console
    mBrief.hasBrief = false;
    mBrief.theBrief = wxEmptyString;
    mBrief.briefingConsoleName = wxEmptyString;
    }
};

class jsButton : public wxButton {
    // adds pointer to console to button so we can find it from button event
public:
    Console *pConsole;   // pointer to the console associated with this button
    jsButton (Console *buttonConsole, wxWindow *parent, wxWindowID id, const wxString &label = wxEmptyString, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0, const wxValidator &validator = wxDefaultValidator,const wxString &name = wxButtonNameStr) :
            wxButton(parent, id, label, pos, size, style, validator, name ),
            pConsole(buttonConsole)
    {};
};

#endif  // JavaScript_impl_h
