/* ************************************************************************
 *
 * Project:  OpenCPN - plugin navdata_pi
 * Purpose:  navdata_pi dialog
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#ifndef __DATADIALOGBASE_H__
#define __DATADIALOGBASE_H__
/*
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/scrolwin.h>
#include <wx/radiobut.h>
#include <wx/statbmp.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/statline.h>
*/
//#include <wx/grid.h>
#include "customgrid.h"

// /////////////////////////////////////////////////////////////////////////////
// / Class DataTableBase
// /////////////////////////////////////////////////////////////////////////////
class DataTableBase : public wxDialog
{
	private:

	protected:
        wxStaticText* m_pStartDText;
        wxTextCtrl*   m_pStartDate;
        wxStaticText* m_pStartDTextat;
        wxTextCtrl*   m_pStartTime;
        wxStaticText* m_pDistText;
        wxTextCtrl*   m_pDistValue;
        wxStaticText* m_pTimetText;
        wxTextCtrl*   m_pTimeValue;
        wxStaticText* m_pSpeedText;
        wxTextCtrl*   m_pSpeedValue;
        wxStaticText* m_pEndDText;
        wxTextCtrl*   m_pEndDate;
        wxFlexGridSizer* m_pTripSizer01;
        wxFlexGridSizer* m_pTripSizer00;

		// Virtual event handlers, overide them in your derived class
        //virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }

	public:
        CustomGrid* m_pDataTable;
        DataTableBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Navigation Data"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxRESIZE_BORDER );
        ~DataTableBase();

};

#endif //__DATADIALOGBASE_H__

// /////////////////////////////////////////////////////////////////////////////
// / Class SettingsBase
// /////////////////////////////////////////////////////////////////////////////
class SettingsBase : public wxDialog
{
    private:

    protected:
        wxCheckBox*     m_pShowTripData;
        wxRadioBox*     m_pShowspeed;
        wxButton*       m_pSettingsOK;

        SettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 )
                , long style = wxCAPTION );

        ~SettingsBase(){}

};



