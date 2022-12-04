/* ************************************************************************
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

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/button.h>
#include "wx/spinctrl.h"
#include "wx/valgen.h"
#include "wx/statline.h"
#include "wx/textctrl.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include "wx/radiobox.h"
#include "wx/listbox.h"
#include "wx/choice.h"

void onButton(wxCommandEvent & event){  // here when any dialogue button clicked ****************************
    duk_context *ctx;
    wxWindow *window;
    jsButton *button;
    Console *pConsole;
    wxString elementType, theData, functionName;
    std::vector<dialogElement>::const_iterator it;
    int i;
    Completions result;
    
    button = wxDynamicCast(event.GetEventObject(), jsButton);
    pConsole = button->pConsole;
//    TRACE(3,pConsole->mConsoleName + " Dialogue button processing");
    window = button->GetParent();
    ctx = pConsole->mpCtx;
    if (ctx == nullptr){
        pConsole->message(STYLE_RED, "Plugin logic error: onButton invoked with no ctx context\n");
        return;
        }
    functionName = pConsole->mDialog.functionName;
    pConsole->mDialog.functionName = wxEmptyString;   // clear out so we do not use again
    pConsole->mDialog.position = window->GetPosition();   // remember where it is
    if ( window->Validate() && window->TransferDataFromWindow() ){
        window->Show(false);
        // now to build the returned object
        duk_push_array(ctx);
        for (i = 0, it = pConsole->mDialog.dialogElementsArray.cbegin(); it !=  pConsole->mDialog.dialogElementsArray.cend(); i++, it++){
            elementType = it->type;
            duk_push_object(ctx);
            duk_push_string(ctx, elementType);
            duk_put_prop_string(ctx, -2, "type");
            if ((elementType == "caption") || (elementType == "text") || (elementType == "hLine")){
                duk_push_string(ctx, it->stringValue);
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "field"){
                duk_push_string(ctx, it->label);
                duk_put_prop_string(ctx, -2, "label");
                duk_push_string(ctx, it->textValue);
                duk_put_prop_string(ctx, -2, "value");
                if (it->suffix != wxEmptyString){
                    duk_push_string(ctx, it->suffix);
                    duk_put_prop_string(ctx, -2, "suffix");
                    }
                }
            else if (elementType == "tick"){
                wxCheckBox *tickedBox;
                tickedBox = wxDynamicCast(window->FindWindowById(it->itemID), wxCheckBox);
                duk_push_boolean(ctx, tickedBox->GetValue());
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "tickList"){
                wxCheckListBox *tickListBox = wxDynamicCast(window->FindWindowById(it->itemID), wxCheckListBox);
                duk_push_array(ctx);
                for (int j = 0, k = 0; j < tickListBox->GetCount(); j++){
                    if (tickListBox->IsChecked(j)){
                        duk_push_string(ctx, tickListBox->GetString(j));
                        duk_put_prop_index(ctx, -2, k++);
                        }
                    }
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "radio"){
                wxRadioBox* radioBox;
                duk_push_string(ctx, it->label);
                duk_put_prop_string(ctx, -2, "label");
                radioBox = wxDynamicCast(window->FindWindowById(it->itemID), wxRadioBox);
                theData = radioBox->GetString(radioBox->GetSelection());
                duk_push_string(ctx, theData);
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "choice"){
                wxChoice* choice;
                choice = wxDynamicCast(window->FindWindowById(it->itemID), wxChoice);
                theData = choice->GetString(choice->GetSelection());
                duk_push_string(ctx, theData);
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "slider"){
                wxSlider* slider;
                duk_push_string(ctx, it->label);
                duk_put_prop_string(ctx, -2, "label");
                slider = wxDynamicCast(window->FindWindowById(it->itemID), wxSlider);
                duk_push_number(ctx, slider->GetValue());
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "spinner"){
                wxSpinCtrl* spinner;
                duk_push_string(ctx, it->label);
                duk_put_prop_string(ctx, -2, "label");
                spinner = wxDynamicCast(window->FindWindowById(it->itemID), wxSpinCtrl);
                duk_push_number(ctx, spinner->GetValue());
                duk_put_prop_string(ctx, -2, "value");
                }
            else if (elementType == "button"){
                duk_push_string(ctx, button->GetLabel());
                duk_put_prop_string(ctx, -2, "label");
                }
            else pConsole->throw_error(ctx, "onDialog error: onButton found unexpected type " + elementType);
            duk_put_prop_index(ctx, -2, i);
            }
        // now to add extra element for clicked-on button
        duk_push_object(ctx);
        duk_push_string(ctx, button->GetLabel());
        duk_put_prop_string(ctx, -2, "label");
        duk_put_prop_index(ctx, -2, i); // i will have been left one greater than length of array so this extends it
        pConsole->clearDialog();
 //       window->Destroy();
 //       delete window;
 //       window = nullptr;
//        TRACE(4,pConsole->mConsoleName + " Done on button processing - to call function " + functionName);
        result = pConsole->executeFunction(functionName);
//        TRACE(4,pConsole->mConsoleName + " Button processing - back from function");
        if (result != MORE) pConsole->wrapUp(result);
        }
    else pConsole->throw_error(ctx, "JavaScript onDialogue data validation failed");
    }

// create the dialog  *********************************
duk_ret_t duk_dialog(duk_context *ctx) {  // provides wxWidgets dialogue
    extern JavaScript_pi* pJavaScript_pi;
    int i;
    wxString elementType, value;
    dialogElement anElement;
    bool hadButton {false};
    wxArrayString strings;
    wxString getStringFromDuk(duk_context *ctx);
    Console *findConsoleByCtx(duk_context *ctx);
    wxString extractFunctionName(duk_context *ctx, duk_idx_t idx);
    
    Console *pConsole = findConsoleByCtx(ctx);
    std::vector<dialogElement> dialogElementArray;  // will be array of the elements for this call
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    wxDialog *dialog = pConsole->mDialog.pdialog; // any existing dialogue
    if (nargs == 0){    // just return present state
        duk_push_boolean(ctx, (dialog != nullptr) ? true : false);
        return 1;
        }
    // if called with single argument of false, is attempt to cancel any open dialogue
    // cancel any existing dialogue and return true if there was one
    if (duk_get_top(ctx) == 1 && duk_is_boolean(ctx, -1) && !duk_get_boolean(ctx, -1)){
        duk_pop(ctx);   // the argument
        if (dialog != nullptr){
            // there is a dialogue displayed
            pConsole->clearDialog();
            duk_push_boolean(ctx, true);
            }
        else duk_push_boolean(ctx, false);
        return 1;
        }
    
    if ( dialog != nullptr) pConsole->throw_error(ctx, "onDialog error: called with another dialogue active");
    if (nargs != 2) pConsole->throw_error(ctx, "onDialog error: creating dialogue requires two arguments");
    duk_require_function(ctx, -2);  // first arugment must be function
    
    // ready to create new dialogue
    dialog = new wxDialog(pJavaScript_pi->m_parent_window,  wxID_ANY, "JavaScript dialogue", pConsole->mDialog.position, wxDefaultSize,
            wxRESIZE_BORDER | wxCAPTION | wxSTAY_ON_TOP);
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);  // A top-level sizer
    dialog->SetSizer(topSizer);
    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL); // A second box sizer to give more space around the controls
    topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    // work through the supplied structure
    int elements = (int) duk_get_length(ctx, -1);   // number of elements supplied for this dialog
    pConsole->mDialog.dialogElementsArray.clear();    // clear out any previous incomplete stuff
    pConsole->mDialog.dialogElementsArray.reserve(elements);    // reserve our space
    for (i = 0; i < elements; i++){
        anElement.label = wxEmptyString;
        anElement.stringValue = wxEmptyString;
        anElement.textValue = wxEmptyString;
        anElement.numberValue = 0;
        anElement.width = 100;  // default size
        anElement.height = 22;  // default height for field
        anElement.multiLine = false;
        anElement.suffix = wxEmptyString;
        anElement.itemID = 0;
        int fontSize = 12;    // default font size
        wxFont font = wxFontInfo(fontSize);
        duk_get_prop_index(ctx, -1, i);
        if (duk_get_prop_literal(ctx, -1, "type") == 0)
            pConsole->throw_error(ctx, wxString::Format("onDialog error: array index %i does not have type property", i));

        elementType = duk_get_string(ctx, -1);
        anElement.type = elementType;
        duk_pop(ctx);
    
        // look for styling
        if (duk_get_prop_literal(ctx, -1, "style")){
            wxString fontName;
            if (duk_get_prop_literal(ctx, -1, "size")) {
                fontSize = duk_get_number(ctx, -1);
                font.SetPointSize(fontSize);
                } duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "font")) {
                wxString fontName;
                fontName = duk_get_string(ctx, -1);
                if (fontName == "monospace") font.SetFamily(wxFONTFAMILY_TELETYPE);
                else font.SetFaceName(fontName);
                }
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "italic"))
                if (duk_get_boolean(ctx, -1)) font.MakeItalic();
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "underline"))
                if (duk_get_boolean(ctx, -1)) font.MakeUnderlined();
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "bold"))
                if (duk_get_boolean(ctx, -1)) font.MakeBold();
            duk_pop(ctx);
            } duk_pop(ctx);   // pop off the style
        if (elementType == "caption"){
            duk_get_prop_literal(ctx, -1, "value");
            value = getStringFromDuk(ctx);
            duk_pop_2(ctx);     // pop off the value string and the element
            dialog->SetTitle(value);
            anElement.stringValue = value;
            }
        else if (elementType == "text"){
            duk_get_prop_literal(ctx, -1, "value");
            value = getStringFromDuk(ctx);
            duk_pop_2(ctx);     // pop off the value string and the element
            anElement.stringValue = value;
            wxStaticText* staticText = new wxStaticText( dialog, wxID_STATIC, value, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
            staticText->SetFont(font);
            boxSizer->Add(staticText, 0, wxALIGN_LEFT|wxALL, 5);
            }
        else if (elementType == "field"){
            wxString textValue, label, suffix;
            wxTextCtrl* textCtrl;
            if (duk_get_prop_literal(ctx, -1, "label")){
                label = getStringFromDuk(ctx) + " ";
                }
            else label = wxEmptyString;
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "value")){
                textValue = getStringFromDuk(ctx);
                }
            else textValue = wxEmptyString;
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "width")){
                anElement.width = duk_get_number(ctx, -1);
                }
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "height")){
                anElement.height = duk_get_number(ctx, -1);
                }
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "multiLine")){
                anElement.multiLine = duk_get_boolean(ctx, -1)?wxTE_MULTILINE:0;
                }
            duk_pop(ctx);
            if (duk_get_prop_literal(ctx, -1, "suffix")){
                suffix = " " + getStringFromDuk(ctx);
                }
            else suffix = wxEmptyString;
            duk_pop(ctx);
            anElement.label = label;
            anElement.textValue = textValue;
            anElement.suffix = suffix;
            anElement.itemID = wxNewId();
            wxBoxSizer* fieldBox = new wxBoxSizer(wxHORIZONTAL);
            boxSizer->Add(fieldBox, 0, wxGROW|wxALL, 5);
            wxStaticText* staticText = new wxStaticText( dialog, wxID_STATIC, label, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
            staticText->SetFont(font);
#ifdef __WXOSX__
            fieldBox->Add(staticText, 0, wxALIGN_LEFT,0);
#else
            fieldBox->Add(staticText, 0, wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL, 0);
#endif
            textCtrl = new wxTextCtrl ( dialog, anElement.itemID, "", wxDefaultPosition, wxSize(anElement.width,  anElement.height /* 6+fontSize */), anElement.multiLine);
            fieldBox->Add(textCtrl, 0, wxGROW|wxALL, 0);
            textCtrl->SetValidator(wxTextValidator(wxFILTER_NONE, &pConsole->mDialog.dialogElementsArray[i].textValue));
            staticText = new wxStaticText( dialog, wxID_STATIC, suffix, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
            staticText->SetFont(font);
#ifdef __WXOSX__
            fieldBox->Add(staticText, 0, wxALIGN_LEFT, 0);
#else
            fieldBox->Add(staticText, 0, wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL, 0);
#endif
            
            // look for field styling which overrides the main styling
            if (duk_get_prop_literal(ctx, -1, "fieldStyle")){
                if (duk_get_prop_literal(ctx, -1, "italic")){
                    if (duk_get_boolean(ctx, -1)) font.SetStyle(wxFONTSTYLE_ITALIC);
                    else font.SetStyle(wxFONTSTYLE_NORMAL);
                    } duk_pop(ctx);
                if (duk_get_prop_literal(ctx, -1, "underline"))
                    font.SetUnderlined(duk_get_boolean(ctx, -1));
                    duk_pop(ctx);
                if (duk_get_prop_literal(ctx, -1, "bold")){
                    if (duk_get_boolean(ctx, -1)) font.SetWeight(wxFONTWEIGHT_BOLD);
                    else font.SetWeight(wxFONTWEIGHT_NORMAL);
                    } duk_pop(ctx);
                } duk_pop(ctx);   // pop off the style
            textCtrl->SetFont(font);    // overrid for the field
            duk_pop(ctx);
            }
        else if (elementType == "tick"){
            if (!duk_get_prop_literal(ctx, -1, "value")){
                pConsole->throw_error(ctx, "onDialog error: tick requires value");
                }
            else {
                bool ticked = false;
                value = getStringFromDuk(ctx);
                if (value.GetChar(0) == '*'){   // if first char is *, remove and pre-tick this
                    value.Remove(0, 1);
                    ticked = true;
                    }
                anElement.stringValue = value;
                anElement.itemID = wxNewId();
                wxCheckBox *checkBox = new wxCheckBox(dialog, anElement.itemID, value, wxDefaultPosition, wxDefaultSize, 0 );
                checkBox->SetValue(ticked);
                checkBox->SetFont(font);
                boxSizer->Add(checkBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
                }
            duk_pop_2(ctx);    // pop off the value string and the element
            }
        else if (elementType == "tickList"){
            if (!duk_get_prop_literal(ctx, -1, "value")){
                pConsole->throw_error(ctx, "onDialog error: tickList requires value");
                }
            else {
                if (!duk_is_array(ctx, -1)) pConsole->throw_error(ctx, "onDialog error: tickLit requires value array");
                else {
                    int maxChars = 0;
                    strings.Clear();
                    int listLength = (int) duk_get_length(ctx, -1);
                    for (int j = 0; j < listLength; j++) {
                        duk_get_prop_index(ctx, -1, j);
                        value = getStringFromDuk(ctx);
                        duk_pop(ctx);
                        strings.Add(value);
                        if (value.Length() > maxChars) maxChars = (int) value.Length();
                        }
                    duk_pop(ctx);
                    anElement.itemID = wxNewId();
                    wxBoxSizer* checkListBoxBox = new wxBoxSizer(wxVERTICAL);
                    boxSizer->Add(checkListBoxBox);

                    // add label if we have one
                    if (duk_get_prop_literal(ctx, -1, "label")){
                        anElement.label = getStringFromDuk(ctx);
                        wxStaticText* label = new wxStaticText( dialog, wxID_STATIC, anElement.label, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
                        checkListBoxBox->Add(label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
                        }
                    duk_pop(ctx);
                    
                    wxCheckListBox *checkListBox =  new wxCheckListBox(dialog, anElement.itemID, wxDefaultPosition, wxSize(maxChars*9+45, listLength*22), strings, wxLB_EXTENDED);
                    checkListBoxBox->Add(checkListBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
                    checkListBox->SetFont(font);
                    checkListBox->Fit();
                    
                    duk_pop(ctx);
                    }
                }
            }
        else if (elementType == "slider"){
            int start, end;
            
            // range attribute
            if (!duk_get_prop_literal(ctx, -1, "range"))
                pConsole->throw_error(ctx, "onDialog error: slider requires range");
            if (!duk_is_array(ctx, -1) || (duk_get_length(ctx, -1) != 2)) pConsole->throw_error(ctx, "onDialog error: slider requires range with 2 values");
            duk_get_prop_index(ctx, -1, 0);
            start = duk_get_number(ctx, -1);
            duk_pop(ctx);
            duk_get_prop_index(ctx, -1, 1);
            end = duk_get_number(ctx, -1);
            duk_pop_2(ctx); // pop off end and range
            
            // value attribute
            if (duk_get_prop_literal(ctx, -1, "value")){
                anElement.numberValue = duk_get_number(ctx, -1);
                }
            else anElement.numberValue = (start+end)/2;    // default to mid-range
            duk_pop(ctx);
            
            // width attribute
            if (duk_get_prop_literal(ctx, -1, "width")){
                anElement.width = duk_get_number(ctx, -1);
                }
            else anElement.width = 300;
            duk_pop(ctx);
            
            anElement.itemID = wxNewId();
            wxBoxSizer* sliderBox = new wxBoxSizer(wxVERTICAL);
            wxSlider *slider = new wxSlider(dialog, anElement.itemID, anElement.numberValue, start, end, wxDefaultPosition, wxSize(anElement.width,-1), wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS);

            // add label if we have one
            if (duk_get_prop_literal(ctx, -1, "label")){
                anElement.label = getStringFromDuk(ctx);
                wxStaticText* label = new wxStaticText( dialog, wxID_STATIC, anElement.label, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
                label->SetFont(font);
                sliderBox->Add(label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
                }
            duk_pop(ctx);

            sliderBox->Add(slider, 0, wxGROW|wxALL, 0);
            boxSizer->Add(sliderBox,  0, wxGROW|wxALL, 5);
            duk_pop(ctx);
            }
        else if (elementType == "spinner"){
            int start, end;
            
            // range attribute
            if (!duk_get_prop_literal(ctx, -1, "range"))
                pConsole->throw_error(ctx, "onDialog error: spinner requires range");
            if (!duk_is_array(ctx, -1) || (duk_get_length(ctx, -1) != 2)) pConsole->throw_error(ctx, "onDialog error: spinner requires range with 2 values");
            duk_get_prop_index(ctx, -1, 0);
            start = duk_get_number(ctx, -1);
            duk_pop(ctx);
            duk_get_prop_index(ctx, -1, 1);
            end = duk_get_number(ctx, -1);
            duk_pop_2(ctx); // pop off end and range
            
            // value attribute
            if (duk_get_prop_literal(ctx, -1, "value")){
                anElement.numberValue = duk_get_number(ctx, -1);
                }
            else anElement.numberValue = 0;
            duk_pop(ctx);
            
            // width attribute  // seems to have no effect - always expands to fill box
            if (duk_get_prop_literal(ctx, -1, "width")){
                anElement.width = duk_get_number(ctx, -1);
                }
            else anElement.width = 300;
            duk_pop(ctx);

            anElement.itemID = wxNewId();
            wxBoxSizer* spinnerBox = new wxBoxSizer(wxVERTICAL);
            wxSpinCtrl *spinner = new wxSpinCtrl(dialog, anElement.itemID, wxEmptyString, wxDefaultPosition, wxSize(anElement.width, 22), wxSP_ARROW_KEYS, start, end, anElement.numberValue);

            // add label if we have one
            if (duk_get_prop_literal(ctx, -1, "label")){
                anElement.label = getStringFromDuk(ctx);
                wxStaticText* label = new wxStaticText( dialog, wxID_STATIC, anElement.label, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
                label->SetFont(font);
                spinnerBox->Add(label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
                }
            duk_pop(ctx);

            spinnerBox->Add(spinner, 0, wxGROW|wxALL, 0);
            boxSizer->Add(spinnerBox,  0, wxGROW|wxALL, 5);
            duk_pop(ctx);
            }
        else if (elementType == "choice"){
            if (!duk_get_prop_literal(ctx, -1, "value")){
                pConsole->throw_error(ctx, "onDialog error: choice requires value");
                }
            else {
                if (!duk_is_array(ctx, -1)) pConsole->throw_error(ctx, "onDialog error: choice requires value array");
                else {
                    int maxChars = 0;
                    strings.Clear();
                    int listLength = (int) duk_get_length(ctx, -1);
                    for (int j = 0; j < listLength; j++) {
                        duk_get_prop_index(ctx, -1, j);
                        value = getStringFromDuk(ctx);
                        duk_pop(ctx);
                        strings.Add(value);
                        if (value.Length() > maxChars) maxChars = (int) value.Length();
                        }
                    duk_pop(ctx);
                    anElement.itemID = wxNewId();
                    wxBoxSizer* choiceBox = new wxBoxSizer(wxVERTICAL);
                    boxSizer->Add(choiceBox);
                    wxChoice *choice =  new wxChoice(dialog, anElement.itemID, wxDefaultPosition, wxSize(maxChars*9+45, listLength*22), strings, wxCB_DROPDOWN);
                    choiceBox->Add(choice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
                    duk_pop(ctx);
                    }
                }
            }
        else if (elementType == "radio"){
            wxString label;
            wxRadioBox *radioBox;
            int numberOfButtons = (int) duk_get_length(ctx, -1);
            strings.Clear();
            if (duk_get_prop_literal(ctx, -1, "label")){
                label = getStringFromDuk(ctx);
                }
            else label = wxEmptyString;
            duk_pop(ctx);
            if (!duk_get_prop_literal(ctx, -1, "value")) pConsole->throw_error(ctx, "onDialog error: radio requires value");
            else {
                if (!duk_is_array(ctx, -1)) pConsole->throw_error(ctx, "onDialog error: radio requires value array");
                else {
                    numberOfButtons = (int) duk_get_length(ctx, -1);
                    numberOfButtons = numberOfButtons>50 ? 50: numberOfButtons; // place an upper limit
                    int maxChars = 0;
                    for (int j = 0; j < numberOfButtons; j++) {
                        duk_get_prop_index(ctx, -1, j);
                        value = getStringFromDuk(ctx);
                        duk_pop(ctx);
                        strings.Add(value);
                        if (value.Length() > maxChars) maxChars = (int) value.Length();
                        }
                    duk_pop_2(ctx);
                    anElement.itemID = wxNewId();
                    radioBox = new wxRadioBox(dialog, anElement.itemID,label, wxDefaultPosition, wxSize(maxChars*10+45, numberOfButtons*23+14), strings, 1, wxRA_SPECIFY_COLS);
                    boxSizer->Add(radioBox, 0, wxHORIZONTAL|wxALL, 2);
                    anElement.label = label;
                    }
                }
            }
        else if (elementType == "hLine"){
            anElement.type = elementType;
            wxStaticLine* line = new wxStaticLine ( dialog, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
            boxSizer->Add(line, 0, wxGROW|wxALL, 5);
            duk_pop(ctx);
            }
        else if (elementType == "button"){
            bool defaultButton;
            wxString label;
            jsButton *button;
            wxBoxSizer* buttonBox;
            hadButton = true;
            buttonBox = new wxBoxSizer(wxHORIZONTAL);
            boxSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);
            duk_get_prop_literal(ctx, -1, "label");
            if (duk_is_array(ctx, -1)){
                // have array of buttons
                int numberOfButtons = (int) duk_get_length(ctx, -1);
                for (int j = 0; j < numberOfButtons; j++) {
                    defaultButton = false;
                    duk_get_prop_index(ctx, -1, j);
                    label = getStringFromDuk(ctx);
                    duk_pop(ctx);
                    if (label.GetChar(0) == '*'){   // if first char is *, remove and make this default button
                        label.Remove(0, 1);
                        defaultButton = true;
                        }
                    anElement.itemID = wxNewId();
                    button = new jsButton ( pConsole, dialog, anElement.itemID, label, wxDefaultPosition, wxDefaultSize, 0 );
                    button->SetFont(font);
                    if (defaultButton) button->SetDefault();
                    buttonBox->Add(button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
                    }
                duk_pop_2(ctx);
                }
            else {
                label = getStringFromDuk(ctx);
                if (label.GetChar(0) == '*'){   // if first char is *, remove and make this default button
                    label.Remove(0, 1);
                    defaultButton = true;
                    }
                else defaultButton = false;
                anElement.itemID = wxNewId();
                button = new jsButton ( pConsole, dialog, anElement.itemID, label, wxDefaultPosition, wxDefaultSize, 0 );
                buttonBox->Add(button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
                if (defaultButton) button->SetDefault();
                duk_pop_2(ctx);     // pop off the text string and the element
                 }
            anElement.label = label;
            }
        else {
            pConsole->throw_error(ctx, "onDialogue - unsupported element type: " + elementType);
            }
        pConsole->mDialog.dialogElementsArray.push_back(anElement);
        }
    if (!hadButton) {
        // caller has not upplied a button - create a default one
        wxBoxSizer* buttonBox = new wxBoxSizer(wxHORIZONTAL);
        boxSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        jsButton* button = new jsButton ( pConsole, dialog, wxNewId(), "OK", wxDefaultPosition, wxDefaultSize, 0 );
        buttonBox->Add(button, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        }
    dialog->Bind(wxEVT_BUTTON, &onButton, wxID_ANY);
    pConsole->mDialog.functionName = extractFunctionName(ctx, 0);
    pConsole->mDialog.pdialog = dialog;
    pConsole->mWaitingCached = pConsole->mWaiting = true;
    duk_pop_2(ctx); // pop off both call arguments
    duk_push_boolean(ctx, true);
    dialog->Fit();
    dialog->Show(true);

/*
// for debug - dumps element array
 std::vector<dialogElement>::const_iterator it;
    int k;
    k = pConsole->mDialog.dialogElementsArray.size();
    for (k = 0, it = pConsole->mDialog.dialogElementsArray.begin(); it != pConsole->mDialog.dialogElementsArray.end(); k++, it++ ){
        cout << k << " " << pConsole->mDialog.dialogElementsArray.at(k).type << " " << pConsole->mDialog.dialogElementsArray.at(k).itemID << "\n";
        }
 */

    return 1;
    }
