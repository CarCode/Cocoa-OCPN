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

#include <wx/wx.h>
#include "JavaScript_pi.h"
#include "JavaScriptgui_impl.h"
#include "wx/dirdlg.h"

extern JavaScript_pi *pJavaScript_pi;

void ToolsClass::onClose( wxCloseEvent& event ){
/*
    *pPointerToThisInJavaScript_pi = nullptr;
    TRACE(4, "In Tools close");
    this->Destroy();
 */
    this->Hide();
    }

void ToolsClass::onPageChanged( wxNotebookEvent& event ) {
    // The different pages need to be different sizes - this does it
    int pageNumber;

    pageNumber = event.GetSelection();
    resizeDialogue(pageNumber);
    wxString currentDirectory = pJavaScript_pi->mCurrentDirectory;
    mCurrentDirectoryString->SetLabel((currentDirectory == wxEmptyString)?"(Not yet set)":currentDirectory);
    }

void ToolsClass::onAddConsole( wxCommandEvent& event ){
    wxString newConsoleName;
    Console *pConsole;
    int x, y;
    
    this->m_ConsolesMessage->Clear();
    newConsoleName = this->m_newConsoleName->GetValue();
    if (newConsoleName == wxEmptyString){
        m_ConsolesMessage->AppendText("Must specify a name for the new console");
        return;
        }
    //check for existing console with this name
    for (pConsole = pJavaScript_pi->mpFirstConsole; pConsole != nullptr; pConsole = pConsole->mpNextConsole){
        if (newConsoleName == pConsole->mConsoleName){
            m_ConsolesMessage->AppendText("This name is already taken");
            return;
            }
        }
    pConsole = new Console(pJavaScript_pi->m_parent_window, newConsoleName);
   // pConsole->CenterOnScreen();
    // to prevent multiple new consoles hiding eachother completely, we will shift each randomly
    pConsole->GetPosition(&x, &y);
    x += - 25 + rand()%50; y += - 25 + rand()%50;
    pConsole->SetPosition(wxPoint(x, y));
    pConsole->Show();
    m_ConsolesMessage->AppendText(_("Console " + newConsoleName + " created"));
    }

wxString NMEAsentence;  // to hold NMEA sentence as enduring string
void ToolsClass::onRecieveNMEAmessage(wxCommandEvent& event ){
    void SetNMEASentence(wxString &sentence);
    
    NMEAsentence = this->m_NMEAmessage->GetValue();
    pJavaScript_pi->SetNMEASentence(NMEAsentence);
     return;
    }

wxString MessageID;  // to hold message ID as enduring string
wxString MessageBody;  // to hold message body as enduring string
void ToolsClass::onRecieveMessage(wxCommandEvent& event ){
    void SetNMEASentence(wxString &sentence);
    
    MessageID = m_MessageID->GetValue();
    MessageBody = m_MessageBody->GetValue();
    pJavaScript_pi->SetPluginMessage(MessageID, MessageBody);
    }

void ToolsClass::onChangeDirectory( wxCommandEvent& event ){
    wxDirDialog dirDialog(this, "Select a current directory", pJavaScript_pi->mCurrentDirectory, wxDD_NEW_DIR_BUTTON);
    if (dirDialog.ShowModal() == wxID_OK){
        pJavaScript_pi->mCurrentDirectory = dirDialog.GetPath();
        mCurrentDirectoryString->SetLabel(pJavaScript_pi->mCurrentDirectory);
        }
    }

void ToolsClass::onDump( wxCommandEvent& event ){
    cout << "Dumping\n";
    wxString ptrToString(Console* address);
    Console *pConsole;
    wxDialog* dumpWindow;
    wxTextCtrl *dumpTextCtrl;
    extern JavaScript_pi *pJavaScript_pi;
    wxString dump {wxEmptyString};
    
    dumpWindow = new wxDialog(this /*pJavaScript_pi->m_parent_window */, wxID_ANY, "JavaScript plugin dump", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE );
    dumpTextCtrl = new wxTextCtrl(dumpWindow, wxID_NEW,
                          wxEmptyString, wxDefaultPosition, wxSize(240, 100),
                          wxTE_MULTILINE);

    dump += (wxString::Format("wxWidgets version %d.%d.%d.%d\n", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, wxSUBRELEASE_NUMBER));
    dump += (wxString::Format("JavaScript plugin version %d.%d\n", PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR));
    dump += (wxString::Format("JavaScript patch %d\n", PLUGIN_VERSION_PATCH));
    wxString svg {"Not using svg"};
#ifdef JavaScript_USE_SVG   // WXOSX!! was: JAVASRIPT
    svg = "Using svg";
#endif
    dump += (svg + "\n");
    dump += "pJavaScript_pi->m_pconfig\t\t\t" + ptrToString((Console *)pJavaScript_pi->m_pconfig) + "\n";
    dump += "pJavaScript_pi->mpFirstConsole\t" + ptrToString(pJavaScript_pi->mpFirstConsole) + "\n";
    for (pConsole = pJavaScript_pi->mpFirstConsole; pConsole != nullptr; pConsole = pConsole->mpNextConsole){
        dump += ("\n————————————Console " + pConsole->mConsoleName + "————————————\n");
        dump += (pConsole->consoleDump());
        }
    dump += "\npJavaScript_pi->mpBin\t\t" + ptrToString(pJavaScript_pi->mpBin) + "\n";
    for (pConsole = pJavaScript_pi->mpBin; pConsole != nullptr; pConsole = pConsole->mpNextConsole){
        dump += ("\n————————————Console in bin " + pConsole->mConsoleName + "————————————\n");
        dump += (pConsole->consoleDump());
        }
    dump += ("\nEnd of dump\n");
    dumpTextCtrl->AppendText(dump);
    dumpWindow->SetSize(600, 900);
    dumpWindow->Show();
 }

wxString JS_dumpString(wxString identifier, wxString string){
    // dumps string to output window
    wxString::const_iterator i;
    int j;
    wxString dumpTextCtrl = "\n" + identifier + "\n" + string + "\n";
    for (j = 0, i = string.begin(); i != string.end(); ++i, j++)
        {
        wxUniChar uni_ch = *i;
        dumpTextCtrl += wxString::Format("[%02d]%02X ", j, uni_ch);
        if ((j > 0) && ((j+1)%10 == 0)) dumpTextCtrl += "\n";
        }
    return dumpTextCtrl;
}
void ToolsClass::onClean( wxCommandEvent& event ){
    wxWindow *stringWindow;
    wxTextCtrl *dumpTextCtrl;
    int j;
    wxString::const_iterator i;
    wxString JScleanString(wxString given);
    wxString text = this->m_charsToClean->GetValue();
    if (text == wxEmptyString) return;
    stringWindow = new wxDialog(this /* pJavaScript_pi->m_parent_window */, wxID_ANY, "JavaScript text cleaning", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE );

    dumpTextCtrl = new wxTextCtrl(stringWindow, wxID_NEW,
                          wxEmptyString, wxDefaultPosition, wxSize(240, 100),
                          wxTE_MULTILINE);
    for (j = 0, i = text.begin(); i != text.end(); ++i, j++)
        {
        wxUniChar uni_ch = *i;
            dumpTextCtrl->AppendText(wxString::Format("[%02d]%c ", j, uni_ch));
        if ((j > 0) && ((j+1)%10 == 0)) dumpTextCtrl->AppendText("\n");
        }
    dumpTextCtrl->AppendText(JS_dumpString("\nRaw", text));
    text = JScleanString(text);
    dumpTextCtrl->AppendText(JS_dumpString("\nCleaned", text));
    stringWindow->SetSize(500, 500);
    stringWindow->Show();

    }

void displayTools(wxWindow* parent){ // this used for testing in harness only
    ToolsClass *pToolsDialog = new ToolsClass(parent);

    pToolsDialog->Fit();
    pToolsDialog->Show();
    
}
