/* **************************************************************************
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
 *                                                                         *
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


GSV::GSV()
{
   Mnemonic = _T("GSV");
   Empty();
}

GSV::~GSV()
{
   Mnemonic.Empty();
   Empty();
}

void GSV::Empty( void )
{
   NumberOfMessages = 0;
   MessageNumber = 0;
   SatsInView = 0;
   for (int idx = 0; idx < 4; idx++)
   {
         SatInfo[idx].SatNumber = 0;
         SatInfo[idx].ElevationDegrees = 0;
         SatInfo[idx].AzimuthDegreesTrue = 0;
         SatInfo[idx].SignalToNoiseRatio = 0;
   }
}

bool GSV::Parse( const SENTENCE& sentence )
{
/*
  $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75

Where:
      GSV          Satellites in view
      2            Number of sentences for full data
      1            sentence 1 of 2
      08           Number of satellites in view

      01           Satellite PRN number
      40           Elevation, degrees
      083          Azimuth, degrees
      46           SNR - higher is better
           for up to 4 satellites per sentence
      *75          the checksum data, always begins with *
*/
/*
Muster Kalamata: $GP - GPS; $GL - Glonas
 $GPGSV,4,1,14,10,79,100,31,27,67,327,31,21,62,314,34,23,52,051,14*74
 $GPGSV,4,2,14,16,51,230,37,20,47,050,26,49,43,207,34,08,32,314,30*78
 $GPGSV,4,3,14,26,25,192,23,18,21,067,30,32,15,151,34,11,10,289,23*7F
 $GPGSV,4,4,14,15,05,038,21,14,03,164,*70
 GP: 14 Satelliten(10,27,21,23,16,20,49,08,26,18,32,11,15,14)
 $GLGSV,3,1,09,68,57,086,28,83,49,050,26,84,36,321,25,69,36,165,26*63
 $GLGSV,3,2,09,74,23,284,25,67,21,032,27,73,19,245,33,82,15,092,28*60
 $GLGSV,3,3,09,75,05,337,21*5F
 GL: 9 Satelliten(68,83,84,69,74,67,73,82,75)
Muster Hannover: $GP - GPS; $GL - Glonas
 $GPGSV,3,1,10,09,65,075,35,07,60,162,39,02,40,272,39,30,34,192,40*7B
 $GPGSV,3,2,10,06,32,212,43,05,27,302,46,04,26,076,24,16,19,052,23*75
 $GPGSV,3,3,10,29,09,331,17,26,06,021,03*7F
 GP: 10 Satelliten(09,07,02,30,06,05,04,16,29,17)
 $GLGSV,3,1,09,68,70,332,17,77,50,072,,67,44,088,,78,35,150,*6F
 $GLGSV,3,2,09,84,33,301,23,69,22,292,30,83,17,238,,76,15,023,*67
 $GLGSV,3,3,09,85,09,355,*5B
 GL: 9 Satelliten(68,77,67,78,84,69,83,76,85)
*/
   /*
   ** GSV - GPS satellite Status
   **
   **        1 2 3 4 5 6 7         hn
   **        | | | | | | |         ||
   ** $--GSV,x,x,x,x,x,x,x.........h*hh<CR><LF>
   **
   ** Field Number:
   **  1) Number of sentences for full data (1 - 9)
   **  2) sentence number (1 - 9)
   **  3) Number of satellites in view
   **  4) Satellite PRN number (#1 - #4)
   **  5) Elevation, degrees (90° max)
   **  6) Azimuth, degrees (True, 000 zo 359)
   **  7) SNR - higher is better (NULL when not tracking?)
    
   **  Fields 4-7 may repeat up to 4 times per sentence
   **  h) Signal ID (NMEA 0183 4.11 from 2019.05.07)
   **  n) Checksum
   */

   /*
   ** First we check the checksum...
   */

    int nNumberOfDataFields = sentence.GetNumberOfDataFields();
    int satInfoCnt = (nNumberOfDataFields-3) / 4;
    if (satInfoCnt < 1)
    {
        SetErrorMessage(_T("Invalid Field count"));
        return(FALSE);
    }
    
    if (sentence.IsChecksumBad(nNumberOfDataFields + 1) == NTrue)
    {
        SetErrorMessage( _T("Invalid Checksum" ));
        return( FALSE );
    }
    
    NumberOfMessages = sentence.Integer( 1 );
    MessageNumber = sentence.Integer( 2 );
    SatsInView = sentence.Integer( 3 );
    
    for (int idx = 0; idx < satInfoCnt; idx++)
    {
        SatInfo[idx].SatNumber = sentence.Integer( idx*4+4 );
        SatInfo[idx].ElevationDegrees = sentence.Integer( idx*4+5 );
        SatInfo[idx].AzimuthDegreesTrue = sentence.Integer( idx*4+6 );
        SatInfo[idx].SignalToNoiseRatio = sentence.Integer( idx*4+7 );
    }
    
    return( TRUE );
}

bool GSV::Write( SENTENCE& sentence )
{
   /*
   ** Let the parent do its thing
   */

   RESPONSE::Write( sentence );

   sentence += NumberOfMessages;
   sentence += MessageNumber;
   sentence += SatsInView;

   for (int idx = 0; idx < 4; idx++)
   {
         sentence += SatInfo[idx].SatNumber;
         sentence += SatInfo[idx].ElevationDegrees;
         sentence += SatInfo[idx].AzimuthDegreesTrue;
         sentence += SatInfo[idx].SignalToNoiseRatio;
   }

   sentence.Finish();

   return( TRUE );
}

const GSV& GSV::operator = ( const GSV& source )
{
   NumberOfMessages = source.NumberOfMessages;
   MessageNumber = source.MessageNumber;
   SatsInView = source.SatsInView;

   for (int idx = 0; idx < 4; idx++)
   {
         SatInfo[idx].SatNumber = source.SatInfo[idx].SatNumber;
         SatInfo[idx].ElevationDegrees = source.SatInfo[idx].ElevationDegrees;
         SatInfo[idx].AzimuthDegreesTrue = source.SatInfo[idx].AzimuthDegreesTrue;
         SatInfo[idx].SignalToNoiseRatio = source.SatInfo[idx].SignalToNoiseRatio;
   }

   return( *this );
}
