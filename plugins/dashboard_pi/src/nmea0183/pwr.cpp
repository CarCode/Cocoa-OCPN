/* *************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA0183 Support Classes
 * Author:   Samuel R. Blackburn, David S. Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by Samuel R. Blackburn, David S Register           *
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
 ***************************************************************************
 *
 *   S Blackburn's original source license:                                *
 *         "You can use it any way you like."                              *
 *   More recent (2010) license statement:                                 *
 *         "It is BSD license, do with it what you will"                   *
 */


#include "nmea0183.h"

/*
** Author: Samuel R. Blackburn
** CI$: 76300,326
** Internet: sammy@sed.csc.com
**
** You can use it any way you like.
*/

//IMPLEMENT_DYNAMIC( PWR, RESPONSE )

PWR::PWR()
{
   Mnemonic = _T("PWR");
   Empty();
}

PWR::~PWR()
{
   Mnemonic.Empty();
   Empty();
}

void PWR::Empty( void )
{
//   ASSERT_VALID( this );

    BattVolt    = 0.0;
    chargeXGPS  = Unknown0183;
}

bool PWR::Parse( const SENTENCE& sentence )
{
//   ASSERT_VALID( this );

   /*
   ** PWR - Dual XGPS160 Device, Power consumption report
   **
   ** Example:
   **         1   2 3 4 5  6 7 8  9  10   11  12
   **         |   | | | |  | | |  |   |    |  |
   ** $GPPWR,0510,1,0,1,1,00,0,S,2C, 2 0 ,S00*08
   ** $--PWR,
   ** Field Number:
   **  1) is the battery voltage. Battery voltage is not valid while the device is charging. 5,1 Volt ?
   **  2) value 1 = ???
   **  3) value 0 = ???
   **  4) value 1 = ???
   **  5) is the charging status: 1 = charging, 0 = not charging.
   **  6) value 00 = ???
   **  7) value 0 = ???
   **  8) value S = ???
   **  9) value 9 = ???
   ** 10) value " 2 0 " = ???
   ** 11) value S00 = ???
   ** 12) Checksum
   */

   /*
   ** First we check the checksum...
   */

   if ( sentence.IsChecksumBad( 12 ) == TRUE )
   {
      SetErrorMessage( _T("Invalid Checksum") );
      return( FALSE );
   } 

    BattVolt            = sentence.Double( 1 );
    chargeXGPS          = sentence.Boolean( 5 );

    return( TRUE );
}

bool PWR::Write( SENTENCE& sentence )
{
//   ASSERT_VALID( this );

   /*
   ** Let the parent do its thing
   */
   
   RESPONSE::Write( sentence );

    sentence += BattVolt;
    sentence += chargeXGPS;

    sentence.Finish();

   return( TRUE );
}

const PWR& PWR::operator = ( const PWR& source )
{
//   ASSERT_VALID( this );

   BattVolt           = source.BattVolt;
   chargeXGPS         = source.chargeXGPS;

   return( *this );
}

