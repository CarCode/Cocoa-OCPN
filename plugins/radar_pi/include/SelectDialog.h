/* *************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
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

#ifndef _SELECTDIALOG_H_
#define _SELECTDIALOG_H_
// ////////////////////////77


// #include <algorithm>
// #include <vector>
// 

//  #include "RadarControlItem.h"
//  #include "RadarLocationInfo.h"
//  #include "drawutil.h"
//  #include "wx/jsonreader.h"
//  #include "nmea0183.h"
#include "pi_common.h"
#include "raymarine/RaymarineLocate.h"
#include "socketutil.h"
#ifdef __WXOSX__
// #include "config.h"
// #include "version.h"
#endif
#include "nmea0183.hpp"


// ////////////////////////////

#include <wx/checkbox.h>
#include <wx/dialog.h>

class radar_pi;


namespace RadarPlugin {

class SelectDialog : public wxDialog {
 public:
  SelectDialog(wxWindow* parent, radar_pi* pi);

  radar_pi* m_pi;
  wxCheckBox* m_selected[RT_MAX];
};

}  // namespace

#endif /* _SELECTDIALOG_H_ */
