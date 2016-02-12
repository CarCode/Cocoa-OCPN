/***************************************************************************
 * Project:  OpenCPN
 * Purpose:  Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Mauro Calvi                                     *
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
 
#include "squiddio_pi_utils.h"
 
/** \brief wxCurlHTTP for some odd reason does not honor the options, override the offending method and set them here
 *
 * \param relativeURL const wxString&
 * \return void
 *
 */    
void myCurlHTTP::SetCurlHandleToDefaults(const wxString& relativeURL)
{
    wxCurlHTTP::SetCurlHandleToDefaults(relativeURL);

    SetStringOpt(CURLOPT_ENCODING, "gzip,deflate");
}
