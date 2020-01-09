/* *************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  ROUTE Plugin
 * Author:   SaltyPaws
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Brazil BrokeTail                                *
 *   $EMAIL$                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "calculatorgui_impl.h"

CfgDlg::CfgDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : CfgDlgDef( parent, id, title, pos, size, style )
{
}

HlpDlg::HlpDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : HlpDlgDef( parent, id, title, pos, size, style )
{
}


FunDlg::FunDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : FunDlgDef( parent, id, title, pos, size, style )
{
    this->LoadCategories();
    this->LoadFunctions(wxT("All"),wxT("All"));
    this->dbg=true;
}

void FunDlg::LoadFunctions(wxString Category, wxString Unit)
{
    //Clear Pulldown
    this->m_Function_Dropdown->Clear();
    //Load functions into pulldown
    for ( unsigned int count = 0; count < testf.m_Formula.GetCount() ; count++)
	{
        if (Category.IsSameAs(testf.m_Category[count], false) || Category.IsSameAs(wxT("all"), false)   ) this->m_Function_Dropdown->Append(testf.m_ShortDesc[count]);
	}
    this->m_Function_Dropdown->SetSelection(0);
    //Load default function in the menu
    this->OnItemSelect();
}

void FunDlg::LoadCategories(void)
{
    wxArrayString Categories;
    //Clear Pulldown
    this->m_Function_Categories->Clear();
    this->m_Function_Categories->Append(_("All"));
    //Cycle through functions
    for ( unsigned int count = 0; count < testf.m_Formula.GetCount() ; count++)
	{
        if (Categories.Index(testf.m_Category[count], false, false)==wxNOT_FOUND)
        {
            //Add Category to Wxarraystring
            Categories.Add(testf.m_Category[count]);
            //Add category to pulldown
            this->m_Function_Categories->Append(testf.m_Category[count]);
            //printf("Just added: %s\n",(const char*) testf.m_Category[count].mb_str() );
        }
	}
	this->m_Function_Categories->SetSelection(0);
}

void FunDlg::OnCategorySelect( wxCommandEvent& event )
{
    this->LoadFunctions(event.GetString(),wxT("All"));
}

void FunDlg::OnClose( wxCommandEvent& event )
{
#ifdef __WXOSX__
    this->Close();
#else
    this->Destroy();
#endif
}

void FunDlg::OnItemSelect( wxCommandEvent& event )
{


    this->OnItemSelect();

}

void FunDlg::OnItemSelect(void)
{
    wxString Selected_Result = this->m_Function_Dropdown->GetString( this->m_Function_Dropdown->GetCurrentSelection());
    unsigned int n;
    for ( n = 0; n < testf.m_Formula.GetCount() ; n++)
    {
        if(Selected_Result==testf.m_ShortDesc[n]) break;
    }
    this->testf.Selected_Formula=n;
    this->m_Function->SetLabel(testf.m_Formula[testf.Selected_Formula]);
    this->m_Description->SetLabel(testf.m_LongDesc[testf.Selected_Formula]);
    this->m_Output_Parameter->SetLabel(testf.m_Formula[testf.Selected_Formula].BeforeFirst('='));
    PopulatePuldown(testf.m_Result_Unit[testf.Selected_Formula],this->m_Output_Parameter_UnitC, m_panel9);

    this->Parameter->SetLabel(testf.m_Input_parameter[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit[testf.Selected_Formula],this->Units, m_panel9);

    this->Parameter1->SetLabel(testf.m_Input_parameter1[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit1[testf.Selected_Formula],this->Units1, m_panel1);

    this->Parameter2->SetLabel(testf.m_Input_parameter2[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit2[testf.Selected_Formula],this->Units2, m_panel2);

    this->Parameter3->SetLabel(testf.m_Input_parameter3[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit3[testf.Selected_Formula],this->Units3, m_panel3);

    this->Parameter4->SetLabel(testf.m_Input_parameter4[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit4[testf.Selected_Formula],this->Units4, m_panel4);

    this->Parameter5->SetLabel(testf.m_Input_parameter5[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit5[testf.Selected_Formula],this->Units5, m_panel5);

    this->Parameter6->SetLabel(testf.m_Input_parameter6[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit6[testf.Selected_Formula],this->Units6, m_panel6);

    this->Parameter7->SetLabel(testf.m_Input_parameter7[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit7[testf.Selected_Formula],this->Units7, m_panel7);

    this->Parameter8->SetLabel(testf.m_Input_parameter8[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit8[testf.Selected_Formula],this->Units8, m_panel8);

    this->Parameter9->SetLabel(testf.m_Input_parameter9[testf.Selected_Formula]);
    PopulatePuldown(testf.m_Input_unit9[testf.Selected_Formula],this->Units9, m_panel9);

    this->m_Function->Wrap(400); ///Width of description can be put in settings
    this->m_Description->Wrap(400); ///Width of description can be put in settings
    this->Fit();
    this->m_Function_Result->SetValue(_T(""));
}

void FunDlg::PopulatePuldown(wxString& Input_Units, wxChoice* Pulldown, wxPanel *Panel)
{
    if (Input_Units.IsEmpty())
    {
        Panel->Show(false);
    }
    else
    {
        //Clear Pulldown
        Pulldown->Clear();

        //Show panel
        Panel->Show(true);
        //Determine output units
        int unit_index=this->Units_conv.m_Unit.Index(Input_Units, false, false);
        if (unit_index==wxNOT_FOUND)
        {
            //Unit not found
            //  printf("Cannot find unit: %s\n",(const char*) Input_Units.mb_str() );
            Pulldown->Append(Input_Units);
        }
        else
        {
            //unit found
            // printf("Found unit: %s",(const char*) Input_Units.mb_str() );
            // printf(" of category: %s\n",(const char*) this->Units_conv.m_Unit_category[unit_index].mb_str() );

            for ( unsigned int count = 0; count < this->Units_conv.m_Unit.GetCount() ; count++)
            {
                if (Units_conv.m_Unit_category[unit_index].IsSameAs(this->Units_conv.m_Unit_category[count], false) )
                {
                    // printf("Other units found in category: %s\n",(const char*) Units_conv.m_Unit[count].mb_str() );
                    if (Units_conv.m_Display[count].IsSameAs(wxT("TRUE"), false) )
                        Pulldown->Append(Units_conv.m_Unit[count]);
                }
            }
        }
        Pulldown->SetSelection(0);
    }
}

wxString FunDlg::Unit_Conversion( wxString Output_Unit,wxString Input_Unit, wxString Var)
{
if (dbg) printf("\n\n------------------------------------------------------\n");
if (dbg) printf("Input_Unit: %s\n",(const char*) Input_Unit.mb_str() );
if (dbg) printf("Output_Unit: %s\n",(const char*) Output_Unit.mb_str() );
if (dbg) printf("Var: %s\n",(const char*) Var.mb_str() );

//If Var is empty, return 0, nothing else to do
if (Var.IsEmpty()) return wxT("0");

if (Input_Unit.CmpNoCase(Output_Unit)!=0) //Convert if input unit is not output unit
    {
     if (dbg) printf("Do something, return factor\n");
     //find input unit factor
     int to_base = this->Units_conv.m_Unit.Index(Input_Unit, false);
     int to_output = this->Units_conv.m_Unit.Index(Output_Unit, false);
     if (to_base==wxNOT_FOUND || to_output==wxNOT_FOUND)
        {
         printf("not found unit, end conversion\n");
         return( Var );
        }
    else
        {
            if (dbg) printf("Conversion to base: %s\n",(const char*) this->Units_conv.m_Conversion[to_base].mb_str() );
            if (dbg) printf("Conversion to output: 1/%s\n",(const char*) this->Units_conv.m_Conversion[to_output].mb_str() );

            //Check if to_base contains VAR, if yes to_base= replace VAR with Var else to_base=Var*to_base
            if (Units_conv.m_Conversion[to_base].Contains(wxT("VAR")))
                {
                Input_Unit = wxT("(")+Units_conv.m_Conversion[to_base].BeforeFirst('=')+wxT(")");
                Input_Unit.Replace(wxT("VAR"), Var);
                if (dbg) printf("Replace mode.... Input_Unit: -->%s<--\n",(const char*) Input_Unit.mb_str() );

                }
            else
            {
                Input_Unit =wxT("(")+Var+wxT("*")+Units_conv.m_Conversion[to_base]+wxT(")");
                if (dbg) printf("Normal Mode: Input_Unit: -->%s<--\n",(const char*) Input_Unit.mb_str() );
            }


            //Check if to_output contains VAR, if yes, replace VAR with to_base string, if not devide by to_output
            if (Units_conv.m_Conversion[to_output].Contains(wxT("VAR")))
                {
                Output_Unit = Units_conv.m_Conversion[to_output].AfterLast('=');
                Output_Unit.Replace(wxT("VAR"), Input_Unit);
                if (dbg) printf("Replace mode.... Output_Unit: -->%s<--\n",(const char*) Input_Unit.mb_str() );
                if (dbg)
                    {
                    wxString returnf=wxT("(")+Output_Unit+wxT(")");
                    printf("Returning: -->%s<--\n",(const char*) returnf.mb_str() );
                    }
                return (wxT("(")+Output_Unit+wxT(")"));
                }
            else
            {
                Output_Unit =Input_Unit+wxT("/")+Units_conv.m_Conversion[to_output];
                if (dbg) printf("Normal Mode: Input_Unit: -->%s<--\n",(const char*) Output_Unit.mb_str() );
                if (dbg)
                    {
                    wxString returnf=wxT("(")+Output_Unit+wxT(")");
                    printf("Returning: -->%s<--\n",(const char*) returnf.mb_str() );
                }
                return (wxT("(")+Output_Unit+wxT(")"));
            }
        }
    }
else
    {

    if (dbg) printf("Nothing to do, return input variable\n");
    return (Var);
    }
}

void FunDlg::OnExtraCalculate( wxCommandEvent& event )
{
    wxString Result=testf.m_Formula[testf.Selected_Formula].BeforeFirst('=');
    wxString Formula=testf.m_Formula[testf.Selected_Formula].AfterFirst('=');
    /*
    if (!testf.m_Input_parameter[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter[testf.Selected_Formula],this->NotEmpty(this->Value->GetValue()),true);
    if (!testf.m_Input_parameter1[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter1[testf.Selected_Formula],this->NotEmpty(this->Value1->GetValue()),true);
    if (!testf.m_Input_parameter2[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter2[testf.Selected_Formula],this->NotEmpty(this->Value2->GetValue()),true);
    if (!testf.m_Input_parameter3[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter3[testf.Selected_Formula],this->NotEmpty(this->Value3->GetValue()),true);
    if (!testf.m_Input_parameter4[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter4[testf.Selected_Formula],this->NotEmpty(this->Value4->GetValue()),true);
    if (!testf.m_Input_parameter5[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter5[testf.Selected_Formula],this->NotEmpty(this->Value5->GetValue()),true);
    if (!testf.m_Input_parameter6[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter6[testf.Selected_Formula],this->NotEmpty(this->Value6->GetValue()),true);
    if (!testf.m_Input_parameter7[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter7[testf.Selected_Formula],this->NotEmpty(this->Value7->GetValue()),true);
    if (!testf.m_Input_parameter8[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter8[testf.Selected_Formula],this->NotEmpty(this->Value8->GetValue()),true);
    if (!testf.m_Input_parameter9[testf.Selected_Formula].IsEmpty()) Formula.Replace(testf.m_Input_parameter9[testf.Selected_Formula],this->NotEmpty(this->Value9->GetValue()),true);
    */
    //Add input parameters converted for unit
    if (!testf.m_Input_parameter [testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit[testf.Selected_Formula],this->Units->GetString( this->Units->GetCurrentSelection()),this->Value->GetValue()),true);
    if (!testf.m_Input_parameter1[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter1[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit1[testf.Selected_Formula],this->Units1->GetString( this->Units1->GetCurrentSelection()),this->Value1->GetValue()),true);
    if (!testf.m_Input_parameter2[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter2[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit2[testf.Selected_Formula],this->Units2->GetString( this->Units2->GetCurrentSelection()),this->Value2->GetValue()),true);
    if (!testf.m_Input_parameter3[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter3[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit3[testf.Selected_Formula],this->Units3->GetString( this->Units3->GetCurrentSelection()),this->Value3->GetValue()),true);
    if (!testf.m_Input_parameter4[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter4[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit4[testf.Selected_Formula],this->Units4->GetString( this->Units4->GetCurrentSelection()),this->Value4->GetValue()),true);
    if (!testf.m_Input_parameter5[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter5[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit5[testf.Selected_Formula],this->Units5->GetString( this->Units5->GetCurrentSelection()),this->Value5->GetValue()),true);
    if (!testf.m_Input_parameter6[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter6[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit6[testf.Selected_Formula],this->Units6->GetString( this->Units6->GetCurrentSelection()),this->Value6->GetValue()),true);
    if (!testf.m_Input_parameter7[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter7[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit7[testf.Selected_Formula],this->Units7->GetString( this->Units7->GetCurrentSelection()),this->Value7->GetValue()),true);
    if (!testf.m_Input_parameter8[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter8[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit8[testf.Selected_Formula],this->Units8->GetString( this->Units8->GetCurrentSelection()),this->Value8->GetValue()),true);
    if (!testf.m_Input_parameter9[testf.Selected_Formula].IsEmpty())  Formula.Replace(testf.m_Input_parameter9[testf.Selected_Formula],Unit_Conversion(testf.m_Input_unit9[testf.Selected_Formula],this->Units9->GetString( this->Units9->GetCurrentSelection()),this->Value9->GetValue()),true);

    //Convert formula to desired output unit
    Formula=Unit_Conversion(this->m_Output_Parameter_UnitC->GetString( this->m_Output_Parameter_UnitC->GetCurrentSelection()),testf.m_Result_Unit[testf.Selected_Formula],Formula);

    Plugin_Dialog->m_result->SetValue(Result.Append('=').Append(Formula));
    //wxMessageBox(Formula);
    //Plugin_Dialog->OnCalculate();
    this->m_Function_Result->SetValue(Plugin_Dialog->OnCalculate());
}

void FunDlg::OnToggle( wxCommandEvent& event ){
    //wxMessageBox(_("toggle"));
    this->m_Description->Show(this->m_checkBox8->GetValue());
    this->Fit();
}

Dlg::Dlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DlgDef( parent, id, title, pos, size, style )
{
    prs.parse("dtr=0.0174532925"); //define degree to radians conversion factor

    /*
    ("This is what the Dialog got\n");
    printf("m_bshowhelpB: %s\n",(m_bshowhelpB)?"true":"false");
    printf("m_bshowhistoryB: %s\n",(m_bshowhistoryB)?"true":"false");
    printf("m_bCalculateB: %s\n",(m_bCalculateB)?"true":"false");
    printf("m_bshowhistory: %s\n",(m_bshowhistory)?"true":"false");
    printf("m_bcapturehidden: %s\n",(m_bcapturehidden)?"true":"false");
    printf("m_blogresults: %s\n",(m_blogresults)?"true":"false");*/

    this->m_listCtrl->Show(false);
    this->m_Overview->Layout();
    this->Layout();
    this->Fit();
    this->m_Overview->Fit();
    this->m_Overview->Layout();
    this->SetSize(wh);
    i_buffer=0;
    i_counter=0;
    item_counter=0;
    MemoryFull=false;
    m_pHelpdialog=NULL;

}

void Dlg::OnFunctionCalc( void )
{

        // Plugin_Dialog->OnFunctionCalc() ;
}

void Dlg::OnHelp( wxCommandEvent& event ){
    OnHelp();}

void Dlg::OnHelp( void )
{
        HlpDlg *m_pHelpdialog = new HlpDlg(this);m_pHelpdialog->Show(true);
        m_pHelpdialog->HelpPanel->Fit();
        m_pHelpdialog->m_textCtrl3->Fit();
        m_pHelpdialog->m_textCtrl3->Layout();
}

void Dlg::OnFunction( wxCommandEvent& event )
{
    OnFunction();
}

void Dlg::OnFunction( void )
{
        FunDlg *m_pFunctiondialog = new FunDlg(this);m_pFunctiondialog->Show(true);
        m_pFunctiondialog->Plugin_Dialog = this;
}

void Dlg::set_History(void)
{
    if(this->m_Help->GetValue())
    {
        //Capture dialog position
        xy=this->GetPosition();
        //Capture dialog size
        wh=this->GetSize();
        wh.y=-1; //Set default dialog height
    	this->m_listCtrl->Show(true);
    	this->m_listCtrl->Show(true);
    	this->m_listCtrl->Fit();
        this->m_Overview->Layout();
        this->Fit();
        this->Layout();
    }
    else
    {
    	this->m_listCtrl->Show(false);
    	this->m_listCtrl->Show(false);
    	this->m_listCtrl->Fit();
        this->m_Overview->Layout();
        this->Fit();
        this->Layout();
        this->SetPosition(xy);
        //Capture dialog size
        this->SetSize(wh);
    }
    wxMilliSleep(50);
}

void Dlg::OnTest(wxCommandEvent& event){
   wxMessageBox(_("Test"));
}

void Dlg::OnTest(wxMouseEvent& event){
   wxMessageBox(_("Mouse"));
}

void Dlg::OnTest(wxListEvent& event){
   wxMessageBox(_("List"));
}

void Dlg::OnItem(wxListEvent& event){
    long item = -1;
    wxString ItemText;
    for ( ;; )
    {
        item = this->m_listCtrl->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;

        ItemText=this->m_listCtrl->GetItemText(item);
        ItemText=ItemText.AfterLast('=');
        ItemText=ItemText.AfterLast(' ');
        m_result->AppendText(ItemText);
    }
}

void Dlg::OnToggle( wxCommandEvent& event )
{
        this->set_History();
}

void Dlg::set_Buttons(void)
{
    this->m_HelpButton->Show(m_bshowhelpB);
    this->m_Help->Show(m_bshowhistoryB);
    this->Calculate->Show(m_bCalculateB);
    this->m_Function->Show(m_bshowFunction);
    this->m_Overview->Fit();
    this->m_Overview->Layout();
    this->m_Help->SetValue(m_bshowhistory);
    this->set_History();
}

void Dlg::OnCalculate( wxCommandEvent& event )
{
    OnCalculate();
}

wxString Dlg::OnCalculate( void )
{
    char* test;
    wxString Text = m_result->GetValue();


    buffer[i_buffer]=Text; //store input
    i_plus(i_buffer);
    i_counter=i_buffer;

    bool error_check=false;
    if ((Text.StartsWith(_T("Error"))) || (Text.StartsWith(_T("Ans")))){
        error_check=true;
    }

    if ((Text.StartsWith(_T("clear"))) || (Text.StartsWith(_T("Clear")))|| (Text.StartsWith(_T("CLEAR")))){ //clear old results
        m_listCtrl->ClearAll();
        error_check=true;
    }
    if ((Text.StartsWith(_T("HideHelp"))) || (Text.StartsWith(_T("hidehelp")))|| (Text.StartsWith(_T("HIDEHELP"))) ||(Text.StartsWith(_T("ShowHelp"))) || (Text.StartsWith(_T("showhelp")))|| (Text.StartsWith(_T("SHOWHELP"))) ){
        m_bshowhelpB=(!m_bshowhelpB);
        this->set_Buttons();
        error_check=true;
    }

    if ((Text.StartsWith(_T("HideHistory"))) || (Text.StartsWith(_T("hidehistory")))|| (Text.StartsWith(_T("HIDEHISTORY")))||(Text.StartsWith(_T("ShowHistory"))) || (Text.StartsWith(_T("showhistory")))|| (Text.StartsWith(_T("SHOWHISTORY")))){
        m_bshowhistoryB=(!m_bshowhistoryB);
        this->set_Buttons();
        error_check=true;
    }

    if ((Text.StartsWith(_T("HideCalculate"))) || (Text.StartsWith(_T("hidecalculate")))|| (Text.StartsWith(_T("HIDECALCULATE"))) || (Text.StartsWith(_T("ShowCalculate"))) || (Text.StartsWith(_T("showcalculate")))|| (Text.StartsWith(_T("SHOWCALCULATE")))){
        m_bCalculateB=(!m_bCalculateB);
        this->set_Buttons();
        error_check=true;
    }

    if ((Text.StartsWith(_T("HideFunction"))) || (Text.StartsWith(_T("hidefunction")))|| (Text.StartsWith(_T("HIDEFUNCTION"))) || (Text.StartsWith(_T("showfunction")))|| (Text.StartsWith(_T("ShowFunction")))|| (Text.StartsWith(_T("SHOWFUNCTION")))){
        m_bshowFunction=(!m_bshowFunction);
        this->set_Buttons();
        error_check=true;
    }

    if ((Text.StartsWith(_T("Help"))) || (Text.StartsWith(_T("HELP")))|| (Text.StartsWith(_T("help")))){
        this->OnHelp ();
        error_check=true;
    }

    if ((Text.StartsWith(_T("Function"))) || (Text.StartsWith(_T("function")))|| (Text.StartsWith(_T("FUNCTION")))){
        this->OnFunction ();
        error_check=true;
    }

    if ((Text.StartsWith(_T("history"))) || (Text.StartsWith(_("History")))|| (Text.StartsWith(_T("HISTORY")))){
        m_bshowhistory=(!m_bshowhistory);
        this->m_Help->SetValue(m_bshowhistory);
        this->set_History();
        error_check=true;
    }

    if (error_check)
        {
        m_result->SetValue(_T(""));
        return wxT("");
        }
    else
        {
        test = prs.parse((const char*)Text.mb_str());
        //printf("\t%s\n", test);
        wxString mystring = wxString::FromUTF8(test);
        if (m_blogresults)
#ifdef __WXOSX__
        {wxLogMessage(_("Calculator INPUT:") + Text + _(" Calculator output:") + mystring);}
#else
            wxLogMessage(_("Calculator INPUT:") + Text + _(" Calculator output:") + mystring);
#endif
        buffer[i_buffer]=mystring; //store input
        i_plus(i_buffer);
        i_counter=i_buffer;

        if((!this->m_Help->GetValue()) || (mystring.StartsWith(_("Error")) )) //print result in messagebox if not history box or error
            m_result->SetValue(mystring.c_str());
        else
            m_result->SetValue(_T(""));
        //m_listCtrl->SetItem(itemIndex, item_counter, "hallo"); //want this for col. 2
        Text.Right(Text.Length()-3);
        //wxLogMessage(mystring);
        //mhelp  capture hidden
        //false  false  --dont capture
        /*printf("m_bcapturehidden: %s\n",(m_bcapturehidden)?"true":"false");
        printf("this->m_Help->GetValue(): %s\n",(this->m_Help->GetValue())?"true":"false");*/

        if (!mystring.StartsWith(_("Error")) )
            {
            if ((this->m_Help->GetValue()) || (m_bcapturehidden))
                {
                //     m_listCtrl->DeleteItem(item_counter);

                itemIndex = m_listCtrl->InsertItem(item_counter, Text + wxT(" = ") + mystring); //want this for col. 1
                m_listCtrl->EnsureVisible(itemIndex);

                item_counter++;
                if (item_counter>Max_Results)
                    {
                    item_counter=0;
                    m_listCtrl->ClearAll();
                    }

                //printf("Item counter:%d, Max results: %d, ItemIndex: %ld\n",item_counter,Max_Results,itemIndex);
                }
            //event.Skip();
            }
            return mystring;
        }
        return wxT("");//Just in case, should not be required
}

void Dlg::OnKey (wxKeyEvent& event)
{
    wxChar uc = event.GetUnicodeKey();
    // It's a "normal" character. Notice that this includes
    // control characters in 1..31 range, e.g. WXK_RETURN or
    // WXK_BACK, so check for them explicitly.
    if ( ((uc >= 32) && (uc<=127))  )
        m_result->AppendText(uc);
    else if ( uc == 13 )
        OnCalculate();
    else if ( uc == 8 ){
        wxString Text = m_result->GetValue();
        m_result->SetValue(Text.Left(Text.Len()-1 ));
    }
}

void Dlg::key_shortcut(wxKeyEvent& event) {
    // of course, it doesn't have to be the control key. You can use others:
    // http://docs.wxwidgets.org/stable/wx_wxkeyevent.html
   // if(event.GetModifiers() == wxMOD_CONTROL) {
        switch(event.GetKeyCode()) {
            case WXK_DOWN: // can return the upper ASCII value of a key
                // do whatever you like for a Ctrl+G event here!
                //wxMessageBox(_("down") );
                down();
                //OnCursor();
                break;
            case WXK_UP: // we also have special keycodes for non-ascii values.
                // get a full list of special keycodes here:
                // http://docs.wxwidgets.org/stable/wx_keycodes.html
                //wxMessageBox(_("up") );
                up();

                break;
            default: // do nothing

                break;
      //  }
   }
   event.Skip();
}

void Dlg::up()
{
    i_plus(i_counter);
    if (buffer[i_counter].Len()>1) {
        m_result->SetValue(buffer[i_counter].c_str());
                        //wxMessageBox(_("Not Empty") );
    }
    else{
        i_min(i_counter);
        //wxMessageBox(_("Empty") );
    }
}

void Dlg::down()
{
    i_min(i_counter);
    if (buffer[i_counter].Len()>1) {
        m_result->SetValue(buffer[i_counter].c_str());
                        //wxMessageBox(_("Not Empty") );
    }
    else{
        i_plus(i_counter);
       // wxMessageBox(_("Empty") );
    }
}

void Dlg::i_plus(int &counter_test){
    counter_test++;
    if (counter_test>40) counter_test=0;
}

void Dlg::i_min(int &counter_test){
    counter_test--;
    if (counter_test<0) counter_test=40;
}
