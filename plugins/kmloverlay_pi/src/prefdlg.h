/***************************************************************************
 * $Id: prefdlg.h, v0.1 2012-01-20 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  KML overlay plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Jean-Eudes Onfray                               *
 *   je@onfray.fr                                                          *
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

#ifndef _KMLOverlayPrefDlg_H_
#define _KMLOverlayPrefDlg_H_

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#include <wx/spinctrl.h>

class KMLOverlayPreferencesDialog : public wxDialog
{
public:
      KMLOverlayPreferencesDialog( wxWindow *pparent, wxWindowID id, int interval );
      ~KMLOverlayPreferencesDialog() {}

      void OnCloseDialog(wxCloseEvent& event);
      void SaveKMLOverlayConfig();

      wxSpinCtrl       *m_pInterval;
      int m_interval;

private:
};

#endif
