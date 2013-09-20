/******************************************************************************
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
 ***************************************************************************
 */

#ifndef _CALCULATORGUI_IMPL_H_
#define _CALCULATORGUI_IMPL_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "parser.h"

#if defined (_WIN32) || defined (_WIN64) || defined (__WIN32__) || defined (__TOS_WIN__) || defined (__WINDOWS__) || defined (_WIN64)
    #include "calculatorgui.h"
#else
    #include "calculatorgui.h"
#endif

#include "calculator_pi.h"
#include "functions.h"
#include "units_conversion.h"

using namespace std;

class calculator_pi;
class Dlg;

class CfgDlg : public CfgDlgDef
{
public:
      CfgDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Calculator preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
};

class HlpDlg : public HlpDlgDef
{
public:
      HlpDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Help!!!!"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
};

class FunDlg : public FunDlgDef
{
    public:
        FunDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Extra Functions"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
        CFormula testf;
        Units_Conversion Units_conv;
        void OnClose( wxCommandEvent& event );
        void OnToggle( wxCommandEvent& event );
        void OnExtraCalculate( wxCommandEvent& event );
        void OnItemSelect( wxCommandEvent& event );
        void OnItemSelect (void);
        void LoadFunctions(wxString Category, wxString );
        void LoadCategories(void);
        void PopulatePuldown(wxString& Input_Units, wxChoice* Pulldown, wxPanel *Panel);
        void OnCategorySelect( wxCommandEvent& event );
        wxString Unit_Conversion(wxString Input_Unit, wxString Output_Unit, wxString Var);
       // wxString NotEmpty (wxString);
        bool dbg;
        Dlg *Plugin_Dialog;
    private:

};

class Dlg : public DlgDef
{
public:
        Dlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Calculator by SaltyPaws"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
        Parser prs; //The calculator class. Here all the functions and variables are stored.
        void OnCalculate( wxCommandEvent& event );
        wxString OnCalculate( void );
        void OnHelp( wxCommandEvent& event );
        void OnHelp( void );
        void OnTest( wxMouseEvent& event );
        void OnTest(wxCommandEvent& event);
        void OnTest( wxListEvent& event );
        void OnFunction( wxCommandEvent& event );
        void OnFunction ( void );
        void OnFunctionCalc ( void );
        void OnToggle( wxCommandEvent& event );
        void key_shortcut(wxKeyEvent& event);
        void OnKey (wxKeyEvent& event);
        void OnItem( wxListEvent& event );
        void down(void);
        void up (void);
        void i_plus(int &counter_test);
        void i_min(int &counter_test);
        calculator_pi *plugin;

        void SetMaxResults          (int x){Max_Results = x;};
        void setm_bshowhelpB        (bool x){m_bshowhelpB = x; };
        void setm_bshowhistoryB     (bool x){m_bshowhistoryB = x;};
        void setm_bCalculateB       (bool x){m_bCalculateB = x;};
        void setm_bshowfunction     (bool x){m_bshowFunction = x;};
        void setm_bshowhistory      (bool x){m_bshowhistory = x;};
        void setm_bcapturehidden    (bool x){m_bcapturehidden = x;};
        void setm_blogresults       (bool x){m_blogresults = x;};
        void set_Buttons (void);
        void set_History(void);
        //void set_Parentwindow(wxwindow *x){*m_parent_windowref = *x;};

private:
        wxPoint xy;
        wxSize  wh;
        wxString buffer[41];
        int i_buffer;
        int i_counter;
        int item_counter;
        long itemIndex;
        bool MemoryFull;
        int Max_Results;
        bool              m_bshowhelpB;
        bool              m_bshowhistoryB;
        bool              m_bCalculateB;
        bool              m_bshowFunction;
        bool              m_bshowhistory;
        bool              m_bcapturehidden;
        bool              m_blogresults;
        HlpDlg            *m_pHelpdialog;
        FunDlg            *m_pFunctiondialog;

};

#endif
