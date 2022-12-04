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

#ifndef OCPN_DUK_H
#define OCPN_DUK_H
#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include "wx/tokenzr.h"
#include "../../include/ocpn_plugin.h"

#define DUK_DUMP true
#if DUK_DUMP
#define MAYBE_DUK_DUMP duk_push_context_dump(ctx);cout << "Duk context dump:" << duk_to_string(ctx, -1) <<"\n";duk_pop(ctx);
#else
#define MAYBE_DUK_DUMP
#endif

// declare wxStyledText styles
enum {
    STYLE_BLACK,
    STYLE_RED,
    STYLE_BLUE
    };

void jsMessage(duk_context *ctx, int style, wxString messageAttribute, wxString message);

typedef wxString jsFunctionNameString_t;
typedef wxString messageNameString_t;

void jsMessage(duk_context *ctx, wxTextAttr p_textColour, wxString messageAttribute, wxString message);

#define NUMBER_OF_MESSAGE_TYPES 1

enum CallBackTypes
{
    // The call back types supported in the API
    NMEA_SENTENCES,
    PLUGIN_MESSAGING
};

class MessagePair  // holds OPCN messages seen, together with JS callback function name, if to be called back
{
public:
    messageNameString_t messageName;
    jsFunctionNameString_t functionName;
};
WX_DECLARE_OBJARRAY(MessagePair, MessagesArray);
#define MAX_SENTENCE_TYPES 50    // safety limit in case of coding error

class TimeActions  // holds times at which function is to be called
{
public:
    wxDateTime timeToCall;
    jsFunctionNameString_t functionName;
    wxString argument;  // optional argument to pass to function
};
#define MAX_TIMERS 20   // safety limit of timers

WX_DECLARE_OBJARRAY(TimeActions, TimesArray);

class JS_control_class
{
public:
    bool                    m_JSactive;    //true if call-backs waiting to run
    bool                    m_runCompleted; // true when run script completed
    bool                    m_timerBusy;  // true while handling timer event to stop them piling up
    duk_context             *m_pctx;      // Pointer to the Duktape context
    Console                 *m_pJSconsole;  // our own pointer to the console
    MessagesArray           m_messages;   // The messages call back table
    TimesArray              m_times;       // Timers call-back table
    jsFunctionNameString_t  m_NMEAmessageFunction;  // function to invoke on receipt of NMEA message, else ""
    PlugIn_Position_Fix_Ex  m_positionFix;  // latest position fix - if none yet, time is NULL


private:
    static int messageComp(MessagePair** arg1, MessagePair** arg2) {
        return (strcmp((*arg1)->messageName, (*arg2)->messageName));}

public:
    int messageIndex(messageNameString_t thisMessage)
    {   // Returns index number for given sentence
        // If unknown sentence, adds it to array, sots it into place and returns its index
        MessagePair newPair;

        size_t messageCount = m_messages.GetCount();
        int index = 0;
        if (messageCount == 0){
            // it's empty
            newPair.messageName = thisMessage;
            newPair.functionName = wxEmptyString;
            m_messages.Add(newPair);
            return(0);  // this is fisrt and only entry, so index is 0
            }
        else { // have existing entries - search for match in sorted list
            for (index = 0; index < messageCount; index++)
                {
                    int cmp = strcmp(thisMessage, m_messages[index].messageName);
                    if (cmp > 0) continue;  // look further
                    else if (cmp == 0) return(index);
                    else break;             // no need to look further as list is sorted
                };
            }
        if (index == messageCount){  // no match
            if (index >= MAX_SENTENCE_TYPES){
                // should not get here unless something wrong
                wxMessageBox(_("Number of message types exceeding safety limit"), _("JavaScript plugin - lookupfunction"));
                return(-1);
                }
        }
        // did not find a match, so add a new entry
        newPair.messageName = thisMessage;
        newPair.functionName = wxEmptyString;
        m_messages.Add(newPair);
        m_messages.Sort(messageComp);  // This likely to move new entry so look up index again
        messageCount = m_messages.GetCount();  // update
        for (index = 0; index < messageCount; index++) // NB condition == here to allow for new entry
            {
            if ((m_messages[index].messageName == thisMessage)) return(index);
            }
        // should never get here
        wxMessageBox(_("Failed to add new sentence - programming error"), _("JavaScript plugin - messageIndex fuction"));
        return(-1);
        }

    bool waiting()  // returns true if waiting for anything
    {
        if (m_times.GetCount() > 0) return true;    // timer active
        if (m_NMEAmessageFunction != wxEmptyString) return true;  // waiting on NMEA

        size_t messageCount = m_messages.GetCount();    // look at messages
        if (messageCount == 0) return false;
        for(unsigned int index = 0; index < messageCount; index++){
            if (m_messages[index].functionName != wxEmptyString) return true;
        }
        return false;
    }

    void clear()  // clear out all waiting callbacks
    {
        wxString runLabel="Run";
        JS_control_class JS_control;

        if (m_runCompleted){ // if original run script complete, we need to destroy context
            duk_destroy_heap(m_pctx);
            m_pctx = nullptr;
            }
        m_JSactive = false;
        m_pJSconsole->setRunButton(runLabel);

        m_NMEAmessageFunction = wxEmptyString;
        size_t messageCount = m_messages.GetCount();
        if (messageCount > 0){
            for(unsigned int index = 0; index < messageCount; index++){
                m_messages[index].functionName = wxEmptyString;
            }
        m_times.Clear();  // clear out the timers - does not seem to work
        m_timerBusy = false;
         }
        return;
    }


    wxString getMessagesString()        // returns string containing list of all messages
    {
        wxString result = wxEmptyString;
        wxString functionPart;

        size_t count = this->m_messages.GetCount();
        if (count > 0){
            for (unsigned int i = 0; i < count; i++){
                result += m_messages[i].messageName;
                functionPart = m_messages[i].functionName;
                if (functionPart != wxEmptyString)
                    result += _(":") + functionPart;
                result += _("\n");
            }
        }
        else result = _("<nil>\n");
        return (result);
    }

    wxString getFunctionName() // extracts function name from call string
    {
        wxStringTokenizer tokens( wxString(duk_to_string(m_pctx, 0)), _(" ("));
        if (tokens.GetNextToken() != "function")
            {cout << "OCPNon.. must supply function\n";
            return("Invalid");
            }
        else return (tokens.GetNextToken());
        }

    void setTimedCallback(jsFunctionNameString_t functionName, wxString argument, wxDateTime timeToCall){
    // create a callback entry to call fuction with argument at timeToCall
    size_t timersCount = this->m_times.GetCount();
    if (timersCount >= MAX_TIMERS){
        // should not get here unless something wrong
        wxMessageBox(_("Number of timers exceeding safety limit"), _("JavaScript plugin - setTimedCallback"));
        return;
    }
    TimeActions newAction;
    newAction.functionName = functionName;
    newAction.argument = argument;
    newAction.timeToCall = timeToCall;
    m_times.Add(newAction);

    }
};


#endif /* OCPN_DUK_H */


    
