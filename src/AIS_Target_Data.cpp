/***************************************************************************
 *
 * Project:  OpenCPN
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

#include "AIS_Target_Data.h"

extern bool bGPSValid;
extern ChartCanvas *cc1;
extern bool g_bAISRolloverShowClass;
extern bool g_bAISRolloverShowCOG;
extern bool g_bAISRolloverShowCPA;
extern bool g_bShowMag;
extern MyFrame *gFrame;

//    Define and declare a hasmap for ERI Ship type strings, keyed by their UN Codes.
WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, ERIShipTypeHash);

static ERIShipTypeHash s_ERI_hash;

void make_hash_ERI(int key, const wxString & description)
{
	s_ERI_hash[key] = description;
}

static wxString FormatTimeAdaptive( int seconds )
{
    int s = seconds % 60;
    int m = seconds / 60;
    if( seconds < 100 )
        return wxString::Format( _T("%3ds"), seconds );
    else if( seconds < 3600 ) {
        int m = seconds / 60;
        int s = seconds % 60;
        return wxString::Format( _T("%2dmin %02ds"), m, s );
    }
    int h = seconds / 3600;
    m -= h* 60;
    return wxString::Format( _T("%2dh %02dmin"), h, m );
}


AIS_Target_Data::AIS_Target_Data()
{
    strncpy(ShipName, "Unknown             ", 21);
    strncpy(CallSign, "       ", 8);
    strncpy(Destination, "                    ", 21);
    ShipNameExtension[0] = 0;
    b_show_AIS_CPA = false;

    SOG = 555.;
    COG = 666.;
    HDG = 511.;
    ROTAIS = -128;
    Lat = 0.;
    Lon = 0.;

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT();
    PositionReportTicks = now.GetTicks();       // Default is my idea of NOW
    StaticReportTicks = now.GetTicks();
    b_lost = false;

    IMO = 0;
    MID = 555;
    MMSI = 666;
    NavStatus = UNDEFINED;
    SyncState = 888;
    SlotTO = 999;
    ShipType = 19;    // "Unknown"

    CPA = 100;     // Large values avoid false alarms
    TCPA = 100;

    Range_NM = -1.;
    Brg = -1.;

    DimA = DimB = DimC = DimD = 0;;

    ETA_Mo = 0;
    ETA_Day = 0;
    ETA_Hr = 24;
    ETA_Min = 60;

    Draft = 0.;

    RecentPeriod = 0;

    m_utc_hour = 0;
    m_utc_min = 0;
    m_utc_sec = 0;

    Class = AIS_CLASS_A;      // default
    n_alarm_state = AIS_NO_ALARM;
    b_suppress_audio = false;
    b_positionDoubtful = false;
    b_positionOnceValid = false;
    b_nameValid = false;

    Euro_Length = 0;            // Extensions for European Inland AIS
    Euro_Beam = 0;
    Euro_Draft = 0;
    strncpy(Euro_VIN, "       ", 8);
    UN_shiptype = 0;

    b_isEuroInland = false;
    b_blue_paddle = false;

    b_OwnShip = false;
    b_in_ack_timeout = false;

    m_ptrack = new AISTargetTrackList;
    b_active = false;
    blue_paddle = 0;
    bCPA_Valid = false;
    ROTIND = 0;
    b_show_track = true;
    b_specialPosnReport = false;
    altitude = 0;
}

AIS_Target_Data::~AIS_Target_Data()
{
      delete m_ptrack;
}

wxString AIS_Target_Data::GetFullName( void )
{
    wxString retName;
    if( b_nameValid ) {
        wxString shipName = trimAISField( ShipName );
        if( shipName == _T("Unknown") )
            retName = wxGetTranslation( shipName );
        else
            retName = shipName;

        if( strlen( ShipNameExtension ) ) {
            wxString shipNameExt = trimAISField( ShipNameExtension );
            retName += shipNameExt;
        }
    }

    return retName;
}


wxString AIS_Target_Data::BuildQueryResult( void )
{
    wxString html;
    wxDateTime now = wxDateTime::Now();

    wxString tableStart = _T("\n<table border=0 cellpadding=1 cellspacing=0>\n");
    wxString tableEnd = _T("</table>\n\n");
    wxString rowStart = _T("<tr><td><font size=-2>");
    wxString rowStartH = _T("<tr><td nowrap>");
    wxString rowSeparator = _T("</font></td><td></td><td><b>");
    wxString rowSeparatorH = _T("</td><td></td><td>");
    wxString colSeparator = _T("<td></td>");
    wxString rowEnd = _T("</b></td></tr>\n");
    wxString vertSpacer = _T("<tr><td></td></tr><tr><td></td></tr><tr><td></td></tr>\n\n");

    wxString IMOstr, MMSIstr, ClassStr, CountryStr;

    html << tableStart << _T("<tr><td nowrap colspan=2>");
    if( ( Class != AIS_BASE ) && ( Class != AIS_SART ) ) {
        if( b_nameValid ) {
//            wxString shipName = trimAISField( ShipName );
//            wxString intlName;
//            if( shipName == _T("Unknown") ) intlName = wxGetTranslation( shipName );
//            else
//                intlName = shipName;
            html << _T("<font size=+2><i><b>") << GetFullName() ;
//            if( strlen( ShipNameExtension ) ) {
//                wxString shipNameExt = trimAISField( ShipNameExtension );
//                html << shipNameExt;
//            }
            html << _T("</b></i></font>&nbsp;&nbsp;<b>");
        }
    }

    if( ( Class != AIS_ATON ) && ( Class != AIS_BASE ) && ( Class != AIS_GPSG_BUDDY )
            && ( Class != AIS_SART ) ) {
        html << trimAISField( CallSign ) << _T("</b>") << rowEnd;

        if( Class != AIS_CLASS_B ) {
            if( IMO > 0 ) IMOstr = wxString::Format( _T("%08d"), abs( IMO ) );
        }
    }
    else html << _T("</b>") << rowEnd;

    html << vertSpacer;

    if( Class != AIS_GPSG_BUDDY ) {
        MMSIstr = wxString::Format( _T("%09d"), abs( MMSI ) );
    }
    ClassStr = wxGetTranslation( Get_class_string( false ) );

    if( Class == AIS_ATON ) {
        wxString cls(_T("AtoN: ") );
        cls += Get_vessel_type_string(false);
        ClassStr = wxGetTranslation( cls );
    }

    CountryStr = GetCountry(false);  //false = Short country ID. true = Full country name

    if( IMOstr.Length() )
        html << _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")
            << rowStart <<_("MMSI") << _T("</font></td><td>&nbsp;</td><td><font size=-2>")
            << _("Class") << _T("</font></td><td>&nbsp;</td><td align=right><font size=-2>")
            << _("IMO") << _T("</font></td></tr>")
            << rowStartH << _T("<b>") << MMSIstr << _T("</b></td><td>&nbsp;</td><td><b>")
            << ClassStr << _T("</b></td><td>&nbsp;</td><td align=right><b>")
            << IMOstr << rowEnd << _T("</table></td></tr>")
            << vertSpacer;
    else
        html << _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")
            << rowStart <<_("MMSI") << _T("</font></td><td>&nbsp;</td><td align=right><font size=-2>")
            << _("Class") << _T("</font></td></tr>")
            << rowStartH << _T("<b>") << MMSIstr << _T("</b></td><td>&nbsp;</td><td align=right><b>")
            << ClassStr << rowEnd << _T("</table></td></tr>");
            //<< vertSpacer;

        if((Class != AIS_SART ) && ( Class != AIS_BASE ) && ( Class != AIS_DSC ) ) {
            CountryStr = GetCountry(true);  //false = Short country ID. true = Full country name
            html << rowStart << _("Flag") << _T(": ") << CountryStr << rowEnd
            << vertSpacer;
            }
    wxString navStatStr;
    if( ( Class != AIS_BASE ) && ( Class != AIS_CLASS_B ) && ( Class != AIS_SART ) ) {
        if( ( NavStatus <= 21  ) && ( NavStatus >= 0 ) )
            navStatStr = wxGetTranslation(ais_get_status(NavStatus));
    } else if( Class == AIS_SART ) {
        if( NavStatus == RESERVED_14 ) navStatStr = _("Active");
        else if( NavStatus == UNDEFINED ) navStatStr = _("Testing");
    }

    wxString sart_sub_type;
    if( Class == AIS_SART ) {
        int mmsi_start = MMSI / 1000000;
        switch( mmsi_start ){
            case 970:
//                sart_sub_type = _T("SART");
                break;
            case 972:
                sart_sub_type = _T("MOB");
                break;
            case 974:
                sart_sub_type = _T("EPIRB");
                break;
            default:
                sart_sub_type = _("Unknown");
                break;
        }
    }

    wxString AISTypeStr, UNTypeStr, sizeString;
    if( ( Class != AIS_BASE ) && ( Class != AIS_SART ) && ( Class != AIS_DSC ) ) {

        //      Ship type
        AISTypeStr = wxGetTranslation( Get_vessel_type_string() );

        if( b_isEuroInland && UN_shiptype ) {
            ERIShipTypeHash::iterator it = s_ERI_hash.find( UN_shiptype );
            wxString type;
            if( it == s_ERI_hash.end() ) type = _("Undefined");
            else
                type = it->second;

            UNTypeStr = wxGetTranslation( type );
        }

        if( b_specialPosnReport ){
            AISTypeStr = wxGetTranslation( _("Special Position Report") );
            UNTypeStr.Clear();
            navStatStr.Clear();
        }


        if( Class == AIS_SART ) {
            if( MSG_14_text.Len() ) {
                html << rowStart << _("Safety Broadcast Message") << rowEnd
                    << rowStartH << _T("<b>") << MSG_14_text << rowEnd;
            }
        }

       //  Dimensions

        if( NavStatus != ATON_VIRTUAL && Class != AIS_ARPA && Class != AIS_APRS ) {
            if( ( Class == AIS_CLASS_B ) || ( Class == AIS_ATON ) ) {
                sizeString = wxString::Format( _T("%dm x %dm"), ( DimA + DimB ), ( DimC + DimD ) );
            } else if(!b_specialPosnReport) {
                if( ( DimA + DimB + DimC + DimD ) == 0 ) {
                    if( b_isEuroInland ) {
                        if( Euro_Length == 0.0 ) {
                            if( Euro_Draft > 0.01 ) {
                                sizeString << wxString::Format( _T("---m x ---m x %4.1fm"), Euro_Draft );
                            } else {
                                sizeString << _T("---m x ---m x ---m");
                            }
                        } else {
                            if( Euro_Draft > 0.01 ) {
                                sizeString
                                        << wxString::Format( _T("%5.1fm x %4.1fm x %4.1fm"), Euro_Length,
                                                Euro_Beam, Euro_Draft );
                            } else {
                                sizeString
                                        << wxString::Format( _T("%5.1fm x %4.1fm x ---m\n\n"), Euro_Length,
                                                Euro_Beam );
                            }
                        }
                    } else {
                        if( Draft > 0.01 ) {
                            sizeString << wxString::Format( _T("---m x ---m x %4.1fm"), Draft );
                        } else {
                            sizeString << _T("---m x ---m x ---m");
                        }
                    }
                } else if( Draft < 0.01 ) {
                    sizeString
                            << wxString::Format( _T("%dm x %dm x ---m"), ( DimA + DimB ), ( DimC + DimD ) );
                } else {
                    sizeString
                            << wxString::Format( _T("%dm x %dm x %4.1fm"), ( DimA + DimB ), ( DimC + DimD ),
                                    Draft );
                }
            }
        }
    }

    if( Class == AIS_SART ) {
        html << _T("<tr><td colspan=2>") << _T("<b>") << AISTypeStr;
        if( sart_sub_type.Length() )
            html << _T(" (") << sart_sub_type << _T("), ");
        html << navStatStr;
        html << rowEnd << _T("<tr><td colspan=2>") << _T("<b>") << sizeString << rowEnd;
    }

    else if( Class == AIS_ATON )  {
        html << _T("<tr><td colspan=2>") << _T("<b>") << navStatStr;
        html << rowEnd << _T("<tr><td colspan=2>") << _T("<b>") << sizeString << rowEnd;
    }

    else if( ( Class != AIS_BASE ) && ( Class != AIS_DSC ) ) {
        html << _T("<tr><td colspan=2>") << _T("<b>") << AISTypeStr;
        if( navStatStr.Length() )
            html << _T(", ") << navStatStr;
        if( UNTypeStr.Length() )
            html << _T(" (UN Type ") << UNTypeStr << _T(")");
        html << rowEnd << _T("<tr><td colspan=2>") << _T("<b>") << sizeString << rowEnd;
    }

    if( b_positionOnceValid ) {
        wxString posTypeStr;
        if( b_positionDoubtful ) posTypeStr << _(" (Last Known)");

        now.MakeGMT();
        int target_age = now.GetTicks() - PositionReportTicks;

        html << vertSpacer
             << rowStart << _("Position") << posTypeStr << _T("</font></td><td align=right><font size=-2>")
             << _("Report Age") << _T("</font></td></tr>")

             << rowStartH << _T("<b>") << toSDMM( 1, Lat ) << _T("</b></td><td align=right><b>")
             << FormatTimeAdaptive( target_age ) << rowEnd
             << rowStartH << _T("<b>") << toSDMM( 2, Lon ) << rowEnd;
    }

    wxString courseStr, sogStr, hdgStr, rotStr, rngStr, brgStr, destStr, etaStr;

    if( Class == AIS_GPSG_BUDDY ) {
        html << vertSpacer << rowStart << _("Report as of") << rowEnd
             << rowStartH << wxString::Format( _T("<b>%d:%d UTC "), m_utc_hour, m_utc_min )
             << rowEnd;
    } else {
        if( Class == AIS_CLASS_A && !b_specialPosnReport ) {
            html << vertSpacer << rowStart << _("Destination")
                 << _T("</font></td><td align=right><font size=-2>")
                 << _("ETA") << _T("</font></td></tr>\n")
                 << rowStartH << _T("<b>");
                 wxString dest =  trimAISField( Destination );
                 if(dest.Length() )
                     html << dest;
                 else
                     html << _("---");
                 html << _T("</b></td><td nowrap align=right><b>");

            if( ( ETA_Mo ) && ( ETA_Hr < 24 ) ) {
                int yearOffset = 0;
                if( now.GetMonth() > ( ETA_Mo - 1 ) ) yearOffset = 1;
                wxDateTime eta( ETA_Day, wxDateTime::Month( ETA_Mo - 1 ),
                        now.GetYear() + yearOffset, ETA_Hr, ETA_Min );
                html << eta.Format( _T("%b %d %H:%M") );
            }
            else html << _("---");
            html << rowEnd;
        }

        if( Class == AIS_CLASS_A || Class == AIS_CLASS_B || Class == AIS_ARPA || Class == AIS_APRS ) {
            int crs = wxRound( COG );
            if( crs < 360 ) {
                if( g_bShowMag )
                    courseStr << wxString::Format( wxString("%03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( crs ) );
                else
                    courseStr << wxString::Format( wxString("%03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( crs ) );
            }
            else if( COG == 360.0 )
                courseStr = _T("---");
            else if( crs == 360 )
                courseStr = _T("0&deg;");

            double speed_show = toUsrSpeed( SOG );

            if( ( SOG <= 102.2 ) || b_specialPosnReport ){
                if( speed_show < 10.0 )
                    sogStr = wxString::Format( _T("%.2f "), speed_show ) + getUsrSpeedUnit();
                else if( speed_show < 100.0 )
                    sogStr = wxString::Format( _T("%.1f "), speed_show ) + getUsrSpeedUnit();
                else
                    sogStr = wxString::Format( _T("%.0f "), speed_show ) + getUsrSpeedUnit();
            }
//                sogStr = wxString::Format( _T("%5.2f ") + getUsrSpeedUnit(), toUsrSpeed( SOG ) );
            else
                sogStr = _("---");

            if( (int) HDG != 511 )
                hdgStr = wxString::Format( _T("%03d&deg;"), (int) HDG );
            else
                hdgStr = _T("---");


            if( ROTAIS != -128 ) {
                if( ROTAIS == 127 ) rotStr << _T("> 5&deg;/30s ") << _("Right");
                else if( ROTAIS == -127 ) rotStr << _T("> 5&deg;/30s ") << _("Left");
                else {
                    if( ROTIND > 0 ) rotStr << wxString::Format( _T("%3d&deg;/Min "), ROTIND ) << _("Right");
                    else if( ROTIND < 0 ) rotStr << wxString::Format( _T("%3d&deg;/Min "), -ROTIND ) << _("Left");
                    else rotStr = _T("0");
                }
            }
            else if( !b_specialPosnReport )
                rotStr = _("---");
        }
    }

    if( b_positionOnceValid && bGPSValid && ( Range_NM >= 0. ) )
        rngStr = cc1->FormatDistanceAdaptive( Range_NM );
    else
        rngStr = _("---");

    int brg = (int) wxRound( Brg );
    if( Brg > 359.5 )
        brg = 0;
    if( b_positionOnceValid && bGPSValid && ( Brg >= 0. ) && ( Range_NM > 0. ) && ( fabs( Lat ) < 85. ) ){
        if( g_bShowMag )
            brgStr << wxString::Format( wxString("%03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( Brg ) );
        else
            brgStr << wxString::Format( wxString("%03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( Brg ) );
    }
    else
        brgStr = _("---");

    wxString turnRateHdr; // Blank if ATON or BASE or Special Position Report (9)
    if( ( Class != AIS_ATON ) && ( Class != AIS_BASE ) && ( Class != AIS_DSC ) ) {
        html << vertSpacer << _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")
            << rowStart <<_("Speed") << _T("</font></td><td>&nbsp;</td><td><font size=-2>")
            << _("Course") << _T("</font></td><td>&nbsp;</td><td align=right><font size=-2>");
            if( !b_specialPosnReport )
                html << _("Heading") ;

            html << _T("</font></td></tr>")
            << rowStartH << _T("<b>") << sogStr << _T("</b></td><td>&nbsp;</td><td><b>")
            << courseStr << _T("</b></td><td>&nbsp;</td><td align=right><b>");
            if(!b_specialPosnReport)
                html << hdgStr;
            html  << rowEnd << _T("</table></td></tr>")
            << vertSpacer;
        if( !b_specialPosnReport )
            turnRateHdr = _("Turn Rate");
    }
#ifdef __WXOSX__
    html << _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")
    << rowStart <<_("Entfernung") << _T("</font></td><td>&nbsp;</td><td><font size=-2>")
    << _("Bearing") << _T("</font></td><td>&nbsp;</td><td align=right><font size=-2>")
    << turnRateHdr << _T("</font></td></tr>")
    << rowStartH << _T("<b>") << rngStr << _T("</b></td><td>&nbsp;</td><td><b>")
    << brgStr << _T("</b></td><td>&nbsp;</td><td align=right><b>");
    if(!b_specialPosnReport)
        html << rotStr;
    html << rowEnd << _T("</table></td></tr>")
    << vertSpacer;
#else
    html << _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")
        << rowStart <<_("Range") << _T("</font></td><td>&nbsp;</td><td><font size=-2>")
        << _("Bearing") << _T("</font></td><td>&nbsp;</td><td align=right><font size=-2>")
        << turnRateHdr << _T("</font></td></tr>")
        << rowStartH << _T("<b>") << rngStr << _T("</b></td><td>&nbsp;</td><td><b>")
        << brgStr << _T("</b></td><td>&nbsp;</td><td align=right><b>");
        if(!b_specialPosnReport)
            html << rotStr;
        html << rowEnd << _T("</table></td></tr>")
        << vertSpacer;
#endif
    wxString tcpaStr;
    if( bCPA_Valid ) tcpaStr << _T("/<b> ") << _("in") << _T("<b>") << FormatTimeAdaptive( (int)(TCPA*60.) );

    if( bCPA_Valid ) {
        html<< vertSpacer << rowStart << _("CPA") << rowEnd
            << rowStartH << _T("<b>") << cc1->FormatDistanceAdaptive( CPA )
            << tcpaStr << rowEnd;
    }

    if( Class != AIS_BASE ) {
        if( blue_paddle == 1 ) {
            html << rowStart << _("Inland Blue Flag") << rowEnd
                 << rowStartH << _T("<b>") << _("Clear") << rowEnd;
        } else if( blue_paddle == 2 ) {
            html << rowStart << _("Inland Blue Flag") << rowEnd
                 << rowStartH << _T("<b>") << _("Set") << rowEnd;
        }
    }

    if(b_specialPosnReport) {
        if(altitude != 4095) {
            wxString altStr;
            altStr.Printf(_T("%4d M"), altitude );
            html /*<< _T("<tr><td colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0>")*/
            << rowStart <<_("Altitude") << _T("</font></td><td>&nbsp;</td><td><font size=-2>")
            << rowStartH << _T("<b>") << altStr << _T("</b></td><td>&nbsp;</td><td><b>")
            << rowEnd << _T("</table></td></tr>")
            << vertSpacer;
        }
    }

    html << _T("</table>");
    return html;
}

wxString AIS_Target_Data::GetRolloverString( void )
{
    wxString result;
    wxString t;
    if( b_nameValid ) {
        result.Append( _T("\"") );
        result.Append( GetFullName() );
        result.Append( _T("\" ") );
    }
    if( Class != AIS_GPSG_BUDDY ) {
        t.Printf( _T("%09d"), abs( MMSI ) );
        result.Append( t );
        result.Append( _T(" ") );
        result.Append( GetCountry(false) );  //Get country Short country ID = false
    }
    t = trimAISField( CallSign );
    if( t.Len() ) {
        result.Append( _T(" (") );
        result.Append( t );
        result.Append( _T(")") );
    }
    if( g_bAISRolloverShowClass || ( Class == AIS_SART ) ) {
        if( result.Len() ) result.Append( _T("\n") );
        result.Append( _T("[") );
        if( Class == AIS_ATON ) {
            result.Append( wxGetTranslation( Get_class_string( true ) ) );
            result.Append(_T(": "));
            result.Append( wxGetTranslation( Get_vessel_type_string( false ) ) );
        }
        else
            result.Append( wxGetTranslation( Get_class_string( false ) ) );

        result.Append( _T("] ") );
        if( ( Class != AIS_ATON ) && ( Class != AIS_BASE ) ) {
            if( Class == AIS_SART ) {
                int mmsi_start = MMSI / 1000000;
                switch( mmsi_start ){
                    case 970:
                        break;
                    case 972:
                        result += _T("MOB");
                        break;
                    case 974:
                        result += _T("EPIRB");
                        break;
                    default:
                        result += _("Unknown");
                        break;
                }
            }

            if( Class != AIS_SART ) {
                if( !b_specialPosnReport )
                    result.Append( wxGetTranslation( Get_vessel_type_string( false ) ) );
                else
                    result.Append( _("Special Position Report") );
                }

            if( ( Class != AIS_CLASS_B ) && ( Class != AIS_SART ) && !b_specialPosnReport) {
                if( ( NavStatus <= 15 ) && ( NavStatus >= 0 ) ) {
                    result.Append( _T(" (") );
                    result.Append(wxGetTranslation(ais_get_status(NavStatus)));
                    result.Append( _T(")") );
                }
            } else if( Class == AIS_SART ) {
                result.Append( _T(" (") );
                if( NavStatus == RESERVED_14 ) result.Append( _("Active") );
                else if( NavStatus == UNDEFINED ) result.Append( _("Testing") );

                result.Append( _T(")") );
            }

        }
    }

    if( g_bAISRolloverShowCOG && (( SOG <= 102.2 ) || b_specialPosnReport)
            && ( ( Class != AIS_ATON ) && ( Class != AIS_BASE ) ) ) {
        if( result.Len() ) result << _T("\n");
        
        double speed_show = toUsrSpeed( SOG );
        if( speed_show < 10.0 )
            result << wxString::Format( _T("SOG %.2f "), speed_show ) << getUsrSpeedUnit() << _T(" ");
        else if( speed_show < 100.0 )
            result << wxString::Format( _T("SOG %.1f "), speed_show ) << getUsrSpeedUnit() << _T(" ");
        else
            result << wxString::Format( _T("SOG %.0f "), speed_show ) << getUsrSpeedUnit() << _T(" ");

        int crs = wxRound( COG );
        if( b_positionOnceValid ) {
            if( crs < 360 ) {
                if( g_bShowMag )
                    result << wxString::Format( wxString("COG %03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( crs ) );
                else
                    result << wxString::Format( wxString("COG %03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( crs ) );
                }

            else if( COG == 360.0 )
                result << _(" COG Unavailable");
            else if( crs == 360 )
                result << wxString( " COG 000°", wxConvUTF8 );
        } else
            result << _(" COG Unavailable");
    }

    if( g_bAISRolloverShowCPA && bCPA_Valid ) {
        if( result.Len() ) result << _T("\n");
        result << _("CPA") << _T(" ") << cc1->FormatDistanceAdaptive( CPA )
        << _T(" ") << _("in") << _T(" ")
        << wxString::Format( _T("%.0f"), TCPA ) << _T(" ") << _("min");
    }
    return result;
}

wxString AIS_Target_Data::Get_vessel_type_string( bool b_short )
{
    int i = 19;
    if( Class == AIS_ATON ) {
        i = ShipType + 20;
    } else
        switch( ShipType ){
            case 30:
                i = 0;
                break;
            case 31:
                i = 1;
                break;
            case 32:
                i = 2;
                break;
            case 33:
                i = 3;
                break;
            case 34:
                i = 4;
                break;
            case 35:
                i = 5;
                break;
            case 36:
                i = 6;
                break;
            case 37:
                i = 7;
                break;
            case 50:
                i = 9;
                break;
            case 51:
                i = 10;
                break;
            case 52:
                i = 11;
                break;
            case 53:
                i = 12;
                break;
            case 54:
                i = 13;
                break;
            case 55:
                i = 14;
                break;
            case 58:
                i = 15;
                break;
            default:
                i = 19;
                break;
        }

    if( ( Class == AIS_CLASS_B ) || ( Class == AIS_CLASS_A ) ) {
        if( ( ShipType >= 40 ) && ( ShipType < 50 ) ) i = 8;

        if( ( ShipType >= 60 ) && ( ShipType < 70 ) ) i = 16;

        if( ( ShipType >= 70 ) && ( ShipType < 80 ) ) i = 17;

        if( ( ShipType >= 80 ) && ( ShipType < 90 ) ) i = 18;
    } else if( Class == AIS_GPSG_BUDDY )
        i = 52;
    else if( Class == AIS_ARPA )
        i = 55;
    else if( Class == AIS_APRS )
        i = 56;
    else if( Class == AIS_DSC )
        i = ( ShipType == 12 ) ? 54 : 53;  // 12 is distress

    if( !b_short )
        return ais_get_type(i);
    else
        return ais_get_short_type(i);
}

wxString AIS_Target_Data::Get_class_string( bool b_short )
{
    switch( Class ){
        case AIS_CLASS_A:
            return _("A");
        case AIS_CLASS_B:
            return _("B");
        case AIS_ATON:
            return b_short ? _("AtoN") : _("Aid to Navigation");
        case AIS_BASE:
            return b_short ? _("Base") : _("Base Station");
        case AIS_GPSG_BUDDY:
            return b_short ? _("Buddy") : _("GPSGate Buddy");
        case AIS_DSC:
            if( ShipType == 12 )
                return b_short ? _("DSC") : _("DSC Distress");
            else
                return b_short ? _("DSC") : _("DSC Position Report");
        case AIS_SART:
            return b_short ? _("SART") : _("SART");
        case AIS_ARPA:
            return b_short ? _("ARPA") : _("ARPA");
        case AIS_APRS:
            return b_short ? _("APRS") : _("APRS Position Report");

        default:
            return b_short ? _("Unk") : _("Unknown");
    }
}

void AIS_Target_Data::Toggle_AIS_CPA(void)
{
    b_show_AIS_CPA = !b_show_AIS_CPA ? true : false;
}

void AIS_Target_Data::ToggleShowTrack(void)
{
    b_show_track = !b_show_track ? true : false;
}

//Get country designation short and long according to ISO 3166-2 2014-01 (www.itu.int/online/mms/glad/cga_mids.sh?lng=E)
wxString AIS_Target_Data::GetCountry( bool b_CntryLongStr )
{
    int mmsi_start = MMSI / 1000000;
    wxString S_L_Cntry = _T("");

    switch(mmsi_start) {
    case 201:
            S_L_Cntry = b_CntryLongStr ? _("Albania") : _T("AL") ;
            break;
    case 202:
            S_L_Cntry = b_CntryLongStr ? _("Andorra") : _T("AD") ;
            break;
    case 203:
            S_L_Cntry = b_CntryLongStr ? _("Austria") : _T("AT") ;
            break;
    case 204:
            S_L_Cntry = b_CntryLongStr ? _("Azores") : _T("AZ") ;
            break;
    case 205:
            S_L_Cntry = b_CntryLongStr ? _("Belgium") : _T("BE") ;
            break;
    case 206:
            S_L_Cntry = b_CntryLongStr ? _("Belarus") : _T("BY") ;
            break;
    case 207:
            S_L_Cntry = b_CntryLongStr ? _("Bulgaria") : _T("BG") ;
            break;
    case 208:
            S_L_Cntry = b_CntryLongStr ? _("Vatican City State") : _T("VA") ;
            break;
    case 209:
    case 210:
    case 212:
            S_L_Cntry = b_CntryLongStr ? _("Cyprus") : _T("CY") ;
            break;
    case 211:
    case 218:
            S_L_Cntry = b_CntryLongStr ? _("Germany") : _T("DE") ;
            break;
    case 213:
            S_L_Cntry = b_CntryLongStr ? _("Georgia") : _T("GE") ;
            break;
    case 214:
            S_L_Cntry = b_CntryLongStr ? _("Moldova") : _T("MD") ;
            break;
    case 216:
            S_L_Cntry = b_CntryLongStr ? _("Armenia") : _T("AM") ;
            break;
    case 219:
    case 220:
            S_L_Cntry = b_CntryLongStr ? _("Denmark") : _T("DK") ;
            break;
    case 224:
    case 225:
            S_L_Cntry = b_CntryLongStr ? _("Spain") : _T("ES") ;
            break;
    case 226:
    case 227:
    case 228:
            S_L_Cntry = b_CntryLongStr ? _("France") : _T("FR") ;
            break;
    case 215:
    case 229:
    case 248:
    case 249:
    case 256:
            S_L_Cntry = b_CntryLongStr ? _("Malta") : _T("MT") ;
            break;
    case 230:
            S_L_Cntry = b_CntryLongStr ? _("Finland") : _T("FI") ;
            break;
    case 231:
            S_L_Cntry = b_CntryLongStr ? _("Faroe Islands") : _T("FO") ;
            break;
    case 232:
    case 233:
    case 234:
    case 235:
            S_L_Cntry = b_CntryLongStr ? _("Great Britain") : _T("GB") ;
            break;
    case 236:
            S_L_Cntry = b_CntryLongStr ? _("Gibraltar") : _T("GI") ;
            break;
    case 238:
            S_L_Cntry = b_CntryLongStr ? _("Croatia") : _T("HR") ;
            break;
    case 237:
    case 239:
    case 240:
    case 241:
            S_L_Cntry = b_CntryLongStr ? _("Greece") : _T("GR") ;
            break;
    case 242:
            S_L_Cntry = b_CntryLongStr ? _("Morocco") : _T("MA") ;
            break;
    case 243:
            S_L_Cntry = b_CntryLongStr ? _("Hungary") : _T("HU") ;
            break;
    case 244:
    case 245:
    case 246:
            S_L_Cntry = b_CntryLongStr ? _("Netherlands") : _T("NL") ;
            break;
    case 247:
            S_L_Cntry = b_CntryLongStr ? _("Italy") : _T("IT") ;
            break;
    case 250:
            S_L_Cntry = b_CntryLongStr ? _("Ireland") : _T("IE") ;
            break;
    case 251:
            S_L_Cntry = b_CntryLongStr ? _("Iceland") : _T("IS") ;
            break;
    case 252:
            S_L_Cntry = b_CntryLongStr ? _("Liechtenstein") : _T("LI") ;
            break;
    case 253:
            S_L_Cntry = b_CntryLongStr ? _("Luxembourg") : _T("LU") ;
            break;
    case 254:
            S_L_Cntry = b_CntryLongStr ? _("Monaco") : _T("MC") ;
            break;
    case 255:
            S_L_Cntry = b_CntryLongStr ? _("Madeira") : _T("PT") ;
            break;
    case 257:
    case 258:
    case 259:
            S_L_Cntry = b_CntryLongStr ? _("Norway") : _T("NO") ;
            break;
    case 261:
            S_L_Cntry = b_CntryLongStr ? _("Poland") : _T("PL") ;
            break;
    case 262:
            S_L_Cntry = b_CntryLongStr ? _("Montenegro") : _T("ME") ;
            break;
    case 263:
            S_L_Cntry = b_CntryLongStr ? _("Portugal") : _T("PT") ;
            break;
    case 264:
            S_L_Cntry = b_CntryLongStr ? _("Romania") : _T("RO") ;
            break;
    case 265:
    case 266:
            S_L_Cntry = b_CntryLongStr ? _("Sweden") : _T("SE") ;
            break;
    case 267:
            S_L_Cntry = b_CntryLongStr ? _("Slovak Republic") : _T("SK") ;
            break;
    case 268:
            S_L_Cntry = b_CntryLongStr ? _("San Marino") : _T("SM") ;
            break;
    case 269:
            S_L_Cntry = b_CntryLongStr ? _("Switzerland") : _T("CH") ;
            break;
    case 270:
            S_L_Cntry = b_CntryLongStr ? _("Czech Republic") : _T("CZ") ;
            break;
    case 271:
            S_L_Cntry = b_CntryLongStr ? _("Turkey") : _T("TR") ;
            break;
    case 272:
            S_L_Cntry = b_CntryLongStr ? _("Ukraine") : _T("UA") ;
            break;
    case 273:
            S_L_Cntry = b_CntryLongStr ? _("Russia") : _T("RU") ;
            break;
    case 274:
            S_L_Cntry = b_CntryLongStr ? _("Macedonia") : _T("MK") ;
            break;
    case 275:
            S_L_Cntry = b_CntryLongStr ? _("Latvia") : _T("LV") ;
            break;
    case 276:
            S_L_Cntry = b_CntryLongStr ? _("Estonia") : _T("EE") ;
            break;
    case 277:
            S_L_Cntry = b_CntryLongStr ? _("Lithuania") : _T("LT") ;
            break;
    case 278:
            S_L_Cntry = b_CntryLongStr ? _("Slovenia") : _T("SI") ;
            break;
    case 279:
            S_L_Cntry = b_CntryLongStr ? _("Serbia") : _T("RS") ;
            break;
    case 301:
            S_L_Cntry = b_CntryLongStr ? _("Anguilla") : _T("AI") ;
            break;
    case 303:
            S_L_Cntry = b_CntryLongStr ? _("Alaska") : _T("AK") ;
            break;
    case 304:
    case 305:
            S_L_Cntry = b_CntryLongStr ? _("Antigua and Barbuda") : _T("AG") ;
            break;
    case 306:
            S_L_Cntry = b_CntryLongStr ? _("Netherlands Antilles") : _T("AN") ;
            break;
    case 307:
            S_L_Cntry = b_CntryLongStr ? _("Aruba") : _T("AW") ;
            break;
    case 308:
    case 309:
    case 311:
            S_L_Cntry = b_CntryLongStr ? _("Bahamas") : _T("BS") ;
            break;
    case 310:
            S_L_Cntry = b_CntryLongStr ? _("Bermuda") : _T("BM") ;
            break;
    case 312:
            S_L_Cntry = b_CntryLongStr ? _("Belize") : _T("BZ") ;
            break;
    case 314:
            S_L_Cntry = b_CntryLongStr ? _("Barbados") : _T("BB") ;
            break;
    case 316:
            S_L_Cntry = b_CntryLongStr ? _("Canada") : _T("CA") ;
            break;
    case 319:
            S_L_Cntry = b_CntryLongStr ? _("Cayman Islands") : _T("KY") ;
            break;
    case 321:
            S_L_Cntry = b_CntryLongStr ? _("Costa Rica") : _T("CR") ;
            break;
    case 323:
            S_L_Cntry = b_CntryLongStr ? _("Cuba") : _T("CU") ;
            break;
    case 325:
            S_L_Cntry = b_CntryLongStr ? _("Dominica") : _T("DM") ;
            break;
    case 327:
            S_L_Cntry = b_CntryLongStr ? _("Dominican Republic") : _T("DM") ;
            break;
    case 329:
            S_L_Cntry = b_CntryLongStr ? _("Guadeloupe") : _T("GP") ;
            break;
    case 330:
            S_L_Cntry = b_CntryLongStr ? _("Grenada") : _T("GD") ;
            break;
    case 331:
            S_L_Cntry = b_CntryLongStr ? _("Greenland") : _T("GL") ;
            break;
    case 332:
            S_L_Cntry = b_CntryLongStr ? _("Guatemala") : _T("GT") ;
            break;
    case 334:
            S_L_Cntry = b_CntryLongStr ? _("Honduras") : _T("HN") ;
            break;
    case 336:
            S_L_Cntry = b_CntryLongStr ? _("Haiti") : _T("HT") ;
            break;
    case 339:
            S_L_Cntry = b_CntryLongStr ? _("Jamaica") : _T("JM") ;
            break;
    case 341:
            S_L_Cntry = b_CntryLongStr ? _("Saint Kitts and Nevis") : _T("KN") ;
            break;
    case 343:
            S_L_Cntry = b_CntryLongStr ? _("Saint Lucia") : _T("LC") ;
            break;
    case 345:
            S_L_Cntry = b_CntryLongStr ? _("Mexico") : _T("MX") ;
            break;
    case 347:
            S_L_Cntry = b_CntryLongStr ? _("Martinique") : _T("MQ") ;
            break;
    case 348:
            S_L_Cntry = b_CntryLongStr ? _("Montserrat") : _T("MS") ;
            break;
    case 350:
            S_L_Cntry = b_CntryLongStr ? _("Nicaragua") : _T("NI") ;
            break;
    case 351:
    case 352:
    case 353:
    case 354:
    case 355:
    case 356:
    case 357:
    case 370:
    case 371:
    case 372:
    case 373:
            S_L_Cntry = b_CntryLongStr ? _("Panama") : _T("PA") ;
            break;
    case 358:
            S_L_Cntry = b_CntryLongStr ? _("Puerto Rico") : _T("PR") ;
            break;
    case 359:
            S_L_Cntry = b_CntryLongStr ? _("El Salvador") : _T("SV") ;
            break;
    case 361:
            S_L_Cntry = b_CntryLongStr ? _("Saint Pierre and Miquelon") : _T("PM") ;
            break;
    case 362:
            S_L_Cntry = b_CntryLongStr ? _("Trinidad and Tobago") : _T("TT") ;
            break;
    case 364:
            S_L_Cntry = b_CntryLongStr ? _("Turks and Caicos Islands") : _T("TC") ;
            break;
    case 338:
    case 366:
    case 367:
    case 368:
    case 369:
            S_L_Cntry = b_CntryLongStr ? _("United States of America") : _T("USA") ;
            break;
    case 375:
    case 376:
    case 377:
            S_L_Cntry = b_CntryLongStr ? _("Saint Vincent and Grenadines") : _T("VC") ;
            break;
    case 378:
            S_L_Cntry = b_CntryLongStr ? _("British Virgin Islands") : _T("VG") ;
            break;
    case 379:
            S_L_Cntry = b_CntryLongStr ? _("United States Virgin Islands") : _T("AE") ;
            break;
    case 401:
            S_L_Cntry = b_CntryLongStr ? _("Afghanistan") : _T("AF") ;
            break;
    case 403:
            S_L_Cntry = b_CntryLongStr ? _("Saudi Arabia") : _T("SA") ;
            break;
    case 405:
            S_L_Cntry = b_CntryLongStr ? _("Bangladesh") : _T("BD") ;
            break;
    case 408:
            S_L_Cntry = b_CntryLongStr ? _("Bahrain") : _T("BH") ;
            break;
    case 410:
            S_L_Cntry = b_CntryLongStr ? _("Bhutan") : _T("BT") ;
            break;
    case 412:
    case 413:
    case 414:
            S_L_Cntry = b_CntryLongStr ? _("China") : _T("CN") ;
            break;
    case 416:
            S_L_Cntry = b_CntryLongStr ? _("Taiwan") : _T("TW") ;
            break;
    case 417:
            S_L_Cntry = b_CntryLongStr ? _("Sri Lanka") : _T("LK") ;
            break;
    case 419:
            S_L_Cntry = b_CntryLongStr ? _("India") : _T("IN") ;
            break;
    case 422:
            S_L_Cntry = b_CntryLongStr ? _("Iran") : _T("IR") ;
            break;
    case 423:
            S_L_Cntry = b_CntryLongStr ? _("Azerbaijani Republic") : _T("AZ") ;
            break;
    case 425:
            S_L_Cntry = b_CntryLongStr ? _("Iraq") : _T("IQ") ;
            break;
    case 428:
            S_L_Cntry = b_CntryLongStr ? _("Israel") : _T("IL") ;
            break;
    case 431:
    case 432:
            S_L_Cntry = b_CntryLongStr ? _("Japan") : _T("JP") ;
            break;
    case 434:
            S_L_Cntry = b_CntryLongStr ? _("Turkmenistan") : _T("TM") ;
            break;
    case 436:
            S_L_Cntry = b_CntryLongStr ? _("Kazakhstan") : _T("KZ") ;
            break;
    case 437:
            S_L_Cntry = b_CntryLongStr ? _("Uzbekistan") : _T("UZ") ;
            break;
    case 438:
            S_L_Cntry = b_CntryLongStr ? _("Jordan") : _T("JO") ;
            break;
    case 440:
    case 441:
            S_L_Cntry = b_CntryLongStr ? _("Korea") : _T("KR") ;
            break;
    case 443:
            S_L_Cntry = b_CntryLongStr ? _("Palestine") : _T("PS") ;
            break;
    case 445:
            S_L_Cntry = b_CntryLongStr ? _("People's Rep. of Korea") : _T("KP") ;
            break;
    case 447:
            S_L_Cntry = b_CntryLongStr ? _("Kuwait") : _T("KW") ;
            break;
    case 450:
            S_L_Cntry = b_CntryLongStr ? _("Lebanon") : _T("LB") ;
            break;
    case 451:
            S_L_Cntry = b_CntryLongStr ? _("Kyrgyz Republic") : _T("KG") ;
            break;
    case 453:
            S_L_Cntry = b_CntryLongStr ? _("Macao") : _T("MO") ;
            break;
    case 455:
            S_L_Cntry = b_CntryLongStr ? _("Maldives") : _T("MV") ;
            break;
    case 457:
            S_L_Cntry = b_CntryLongStr ? _("Mongolia") : _T("MN") ;
            break;
    case 459:
            S_L_Cntry = b_CntryLongStr ? _("Nepal") : _T("NP") ;
            break;
    case 461:
            S_L_Cntry = b_CntryLongStr ? _("Oman") : _T("OM") ;
            break;
    case 463:
            S_L_Cntry = b_CntryLongStr ? _("Pakistan") : _T("PK") ;
            break;
    case 466:
            S_L_Cntry = b_CntryLongStr ? _("Qatar") : _T("QA") ;
            break;
    case 468:
            S_L_Cntry = b_CntryLongStr ? _("Syrian Arab Republic") : _T("SY") ;
            break;
    case 470:
            S_L_Cntry = b_CntryLongStr ? _("United Arab Emirates") : _T("AE") ;
            break;
    case 472:
            S_L_Cntry = b_CntryLongStr ? _("Tajikistan") : _T("TK") ;
            break;
    case 473:
    case 475:
            S_L_Cntry = b_CntryLongStr ? _("Yemen") : _T("YE") ;
            break;
    case 477:
            S_L_Cntry = b_CntryLongStr ? _("Hong Kong") : _T("HK") ;
            break;
    case 478:
            S_L_Cntry = b_CntryLongStr ? _("Bosnia and Herzegovina") : _T("BA") ;
            break;
    case 501:
            S_L_Cntry = b_CntryLongStr ? _("Adelie Land") : _T("TF") ;
            break;
    case 503:
            S_L_Cntry = b_CntryLongStr ? _("Australia") : _T("AU") ;
            break;
    case 506:
            S_L_Cntry = b_CntryLongStr ? _("Myanmar") : _T("MM") ;
            break;
    case 508:
            S_L_Cntry = b_CntryLongStr ? _("Brunei Darussalam") : _T("BN") ;
            break;
    case 510:
            S_L_Cntry = b_CntryLongStr ? _("Micronesia") : _T("FM") ;
            break;
    case 511:
            S_L_Cntry = b_CntryLongStr ? _("Palau") : _T("PW") ;
            break;
    case 512:
            S_L_Cntry = b_CntryLongStr ? _("New Zealand") : _T("NZ") ;
            break;
    case 514:
    case 515:
            S_L_Cntry = b_CntryLongStr ? _("Cambodia") : _T("KH") ;
            break;
    case 516:
            S_L_Cntry = b_CntryLongStr ? _("Christmas Island") : _T("CX") ;
            break;
    case 518:
            S_L_Cntry = b_CntryLongStr ? _("Cook Islands") : _T("CK") ;
            break;
    case 520:
            S_L_Cntry = b_CntryLongStr ? _("Fiji") : _T("FJ") ;
            break;
    case 523:
            S_L_Cntry = b_CntryLongStr ? _("Cocos") : _T("CC") ;
            break;
    case 525:
            S_L_Cntry = b_CntryLongStr ? _("Indonesia") : _T("ID") ;
            break;
    case 529:
            S_L_Cntry = b_CntryLongStr ? _("Kiribati") : _T("KI") ;
            break;
    case 531:
            S_L_Cntry = b_CntryLongStr ? _("Lao People's Dem. Rep.") : _T("LA") ;
            break;
    case 533:
            S_L_Cntry = b_CntryLongStr ? _("Malaysia") : _T("MY") ;
            break;
    case 536:
            S_L_Cntry = b_CntryLongStr ? _("Northern Mariana Islands") : _T("MP") ;
            break;
    case 538:
            S_L_Cntry = b_CntryLongStr ? _("Marshall Islands") : _T("MH") ;
            break;
    case 540:
            S_L_Cntry = b_CntryLongStr ? _("New Caledonia") : _T("NC") ;
            break;
    case 542:
            S_L_Cntry = b_CntryLongStr ? _("Niue") : _T("NU") ;
            break;
    case 544:
            S_L_Cntry = b_CntryLongStr ? _("Nauru") : _T("NR") ;
            break;
    case 546:
            S_L_Cntry = b_CntryLongStr ? _("French Polynesia") : _T("PF") ;
            break;
    case 548:
            S_L_Cntry = b_CntryLongStr ? _("Philippines") : _T("PH") ;
            break;
    case 553:
            S_L_Cntry = b_CntryLongStr ? _("Papua New Guinea") : _T("PG") ;
            break;
    case 555:
            S_L_Cntry = b_CntryLongStr ? _("Pitcairn Island") : _T("PN") ;
            break;
    case 557:
            S_L_Cntry = b_CntryLongStr ? _("Solomon Islands") : _T("SB") ;
            break;
    case 559:
            S_L_Cntry = b_CntryLongStr ? _("American Samoa") : _T("AS") ;
            break;
    case 561:
            S_L_Cntry = b_CntryLongStr ? _("Samoa") : _T("WS") ;
            break;
    case 563:
    case 564:
    case 565:
    case 566:
            S_L_Cntry = b_CntryLongStr ? _("Singapore") : _T("SG") ;
            break;
    case 567:
            S_L_Cntry = b_CntryLongStr ? _("Thailand") : _T("TH") ;
            break;
    case 570:
            S_L_Cntry = b_CntryLongStr ? _("Tonga") : _T("TO") ;
            break;
    case 572:
            S_L_Cntry = b_CntryLongStr ? _("Tuvalu") : _T("TV") ;
            break;
    case 574:
            S_L_Cntry = b_CntryLongStr ? _("Viet Nam") : _T("VN") ;
            break;
    case 576:
    case 577:
            S_L_Cntry = b_CntryLongStr ? _("Vanuatu") : _T("VU") ;
            break;
    case 578:
            S_L_Cntry = b_CntryLongStr ? _("Wallis and Futuna Islands") : _T("WF") ;
            break;
    case 601:
            S_L_Cntry = b_CntryLongStr ? _("South Africa") : _T("ZA") ;
            break;
    case 603:
            S_L_Cntry = b_CntryLongStr ? _("Angola") : _T("AO") ;
            break;
    case 605:
            S_L_Cntry = b_CntryLongStr ? _("Algeria") : _T("DZ") ;
            break;
    case 607:
            S_L_Cntry = b_CntryLongStr ? _("Saint Paul") : _T("TF") ;
            break;
    case 608:
            S_L_Cntry = b_CntryLongStr ? _("Ascension Island") : _T("SH") ;
            break;
    case 609:
            S_L_Cntry = b_CntryLongStr ? _("Burundi") : _T("BI") ;
            break;
    case 610:
            S_L_Cntry = b_CntryLongStr ? _("Benin") : _T("BJ") ;
            break;
    case 611:
            S_L_Cntry = b_CntryLongStr ? _("Botswana") : _T("BW") ;
            break;
    case 612:
            S_L_Cntry = b_CntryLongStr ? _("Central African Republic") : _T("CF") ;
            break;
    case 613:
            S_L_Cntry = b_CntryLongStr ? _("Cameroon") : _T("CM") ;
            break;
    case 615:
            S_L_Cntry = b_CntryLongStr ? _("Congo") : _T("CD") ;
            break;
    case 616:
    case 620:
            S_L_Cntry = b_CntryLongStr ? _("Comoros") : _T("KM") ;
            break;
    case 617:
            S_L_Cntry = b_CntryLongStr ? _("Cape Verde") : _T("CV") ;
            break;
    case 618:
            S_L_Cntry = b_CntryLongStr ? _("Crozet Archipelago") : _T("TF") ;
            break;
    case 619:
            S_L_Cntry = b_CntryLongStr ? _("Côte d'Ivoire") : _T("CI") ;
            break;
    case 621:
            S_L_Cntry = b_CntryLongStr ? _("Djibouti") : _T("DJ") ;
            break;
    case 622:
            S_L_Cntry = b_CntryLongStr ? _("Egypt") : _T("EG") ;
            break;
    case 624:
            S_L_Cntry = b_CntryLongStr ? _("Ethiopia") : _T("ET") ;
            break;
    case 625:
            S_L_Cntry = b_CntryLongStr ? _("Eritrea") : _T("ER") ;
            break;
    case 626:
            S_L_Cntry = b_CntryLongStr ? _("Gabonese Republic") : _T("GA") ;
            break;
    case 627:
            S_L_Cntry = b_CntryLongStr ? _("Ghana") : _T("GH") ;
            break;
    case 629:
            S_L_Cntry = b_CntryLongStr ? _("Gambia") : _T("GM") ;
            break;
    case 630:
            S_L_Cntry = b_CntryLongStr ? _("Guinea-Bissau") : _T("GW") ;
            break;
    case 631:
            S_L_Cntry = b_CntryLongStr ? _("Equatorial Guinea") : _T("GQ") ;
            break;
    case 632:
            S_L_Cntry = b_CntryLongStr ? _("Guinea") : _T("GN") ;
            break;
    case 633:
            S_L_Cntry = b_CntryLongStr ? _("Burkina Faso") : _T("BF") ;
            break;
    case 634:
            S_L_Cntry = b_CntryLongStr ? _("Kenya") : _T("KE") ;
            break;
    case 635:
            S_L_Cntry = b_CntryLongStr ? _T("Kerguelen Islands") : _T("TF") ;
            break;
    case 636:
    case 637:
            S_L_Cntry = b_CntryLongStr ? _("Liberia") : _T("LR") ;
            break;
    case 638:
            S_L_Cntry = b_CntryLongStr ? _("South Sudan (Republic of)") : _T("SS") ;
            break;
    case 642:
            S_L_Cntry = b_CntryLongStr ? _("Libya") : _T("LY") ;
            break;
    case 644:
            S_L_Cntry = b_CntryLongStr ? _("Lesotho") : _T("LS") ;
            break;
    case 645:
            S_L_Cntry = b_CntryLongStr ? _("Mauritius") : _T("MU") ;
            break;
    case 647:
            S_L_Cntry = b_CntryLongStr ? _("Madagascar") : _T("MG") ;
            break;
    case 649:
            S_L_Cntry = b_CntryLongStr ? _("Mali") : _T("ML") ;
            break;
    case 650:
            S_L_Cntry = b_CntryLongStr ? _("Mozambique") : _T("MZ") ;
            break;
    case 654:
            S_L_Cntry = b_CntryLongStr ? _("Mauritania") : _T("MR") ;
            break;
    case 655:
            S_L_Cntry = b_CntryLongStr ? _("Malawi") : _T("MW") ;
            break;
    case 656:
            S_L_Cntry = b_CntryLongStr ? _("Niger") : _T("NE") ;
            break;
    case 657:
            S_L_Cntry = b_CntryLongStr ? _("Nigeria") : _T("NG") ;
            break;
    case 659:
            S_L_Cntry = b_CntryLongStr ? _("Namibia") : _T("NA") ;
            break;
    case 660:
            S_L_Cntry = b_CntryLongStr ? _("Reunion") : _T("RE") ;
            break;
    case 661:
            S_L_Cntry = b_CntryLongStr ? _("Rwanda") : _T("RW") ;
            break;
    case 662:
            S_L_Cntry = b_CntryLongStr ? _("Sudan") : _T("SD") ;
            break;
    case 663:
            S_L_Cntry = b_CntryLongStr ? _("Senegal") : _T("SN") ;
            break;
    case 664:
            S_L_Cntry = b_CntryLongStr ? _("Seychelles") : _T("SC") ;
            break;
    case 665:
            S_L_Cntry = b_CntryLongStr ? _("Saint Helena") : _T("SH") ;
            break;
    case 666:
            S_L_Cntry = b_CntryLongStr ? _("Somali Democratic Republic") : _T("SO") ;
            break;
    case 667:
            S_L_Cntry = b_CntryLongStr ? _("Sierra Leone") : _T("SL") ;
            break;
    case 668:
            S_L_Cntry = b_CntryLongStr ? _("Sao Tome and Principe") : _T("ST") ;
            break;
    case 669:
            S_L_Cntry = b_CntryLongStr ? _("Swaziland") : _T("SZ") ;
            break;
    case 670:
            S_L_Cntry = b_CntryLongStr ? _("Chad") : _T("TD") ;
            break;
    case 671:
            S_L_Cntry = b_CntryLongStr ? _("Togolese Republic") : _T("TG") ;
            break;
    case 672:
            S_L_Cntry = b_CntryLongStr ? _("Tunisia") : _T("TN") ;
            break;
    case 674:
            S_L_Cntry = b_CntryLongStr ? _("Tanzania") : _T("TZ") ;
            break;
    case 675:
            S_L_Cntry = b_CntryLongStr ? _("Uganda") : _T("UG") ;
            break;
    case 676:
            S_L_Cntry = b_CntryLongStr ? _("Dem Rep.of the Congo") : _T("CD") ;
            break;
    case 677:
            S_L_Cntry = b_CntryLongStr ? _("Tanzania") : _T("TZ") ;
            break;
    case 678:
            S_L_Cntry = b_CntryLongStr ? _("Zambia") : _T("ZM") ;
            break;
    case 679:
            S_L_Cntry = b_CntryLongStr ? _("Zimbabwe") : _T("ZW") ;
            break;
    case 701:
            S_L_Cntry = b_CntryLongStr ? _("Argentine Republic") : _T("AR") ;
            break;
    case 710:
            S_L_Cntry = b_CntryLongStr ? _("Brazil") : _T("BR") ;
            break;
    case 720:
            S_L_Cntry = b_CntryLongStr ? _("Bolivia") : _T("BO") ;
            break;
    case 725:
            S_L_Cntry = b_CntryLongStr ? _("Chile") : _T("CL") ;
            break;
    case 730:
            S_L_Cntry = b_CntryLongStr ? _("Colombia") : _T("CO") ;
            break;
    case 735:
            S_L_Cntry = b_CntryLongStr ? _("Ecuador") : _T("EC") ;
            break;
    case 740:
            S_L_Cntry = b_CntryLongStr ? _("Falkland Islands") : _T("FK") ;
            break;
    case 745:
            S_L_Cntry = b_CntryLongStr ? _("French Guina") : _T("FG") ;
            break;
    case 750:
            S_L_Cntry = b_CntryLongStr ? _("Gyana") : _T("GU") ;
            break;
    case 755:
            S_L_Cntry = b_CntryLongStr ? _("Paraguay") : _T("PG") ;
            break;
    case 760:
            S_L_Cntry = b_CntryLongStr ? _("Peru") : _T("PU") ;
            break;
    case 765:
            S_L_Cntry = b_CntryLongStr ? _("Suriname") : _T("SU") ;
            break;
    case 770:
            S_L_Cntry = b_CntryLongStr ? _("Uruguay") : _T("UG") ;
            break;
    case 775:
            S_L_Cntry = b_CntryLongStr ? _("Venezuela") : _T("VZ") ;
            break;
    default:
            S_L_Cntry = b_CntryLongStr ? _T("") : _T("") ;
            break;
        }

    return S_L_Cntry;
}
