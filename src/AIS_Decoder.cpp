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
 ***************************************************************************
 */

#include "AIS_Decoder.h"
#include "AIS_Target_Data.h"
#include "AISTargetAlertDialog.h"
#include "Select.h"
#include "georef.h"
#include "OCPN_DataStreamEvent.h"

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

extern AISTargetAlertDialog *g_pais_alert_dialog_active;
extern Select *pSelectAIS;
extern MyFrame *gFrame;
extern int g_ais_alert_dialog_x;
extern int g_ais_alert_dialog_y;
extern int g_ais_alert_dialog_sx;
extern int g_ais_alert_dialog_sy;
extern bool bGPSValid;
extern bool     g_bShowAIS;
extern bool     g_bCPAMax;
extern double   g_CPAMax_NM;
extern bool     g_bCPAWarn;
extern double   g_CPAWarn_NM;
extern bool     g_bTCPA_Max;
extern double   g_TCPA_Max;
extern bool     g_bMarkLost;
extern double   g_MarkLost_Mins;
extern bool     g_bRemoveLost;
extern double   g_RemoveLost_Mins;
extern bool     g_bShowCOG;
extern double   g_ShowCOG_Mins;
extern bool     g_bAISShowTracks;
extern double   g_AISShowTracks_Mins;
extern bool     g_bShowMoored;
extern double   g_ShowMoored_Kts;
extern wxString g_sAIS_Alert_Sound_File;
extern bool     g_bAIS_CPA_Alert_Suppress_Moored;
extern bool     g_bAIS_ACK_Timeout;
extern double   g_AckTimeout_Mins;
extern bool     g_bShowAreaNotices;
extern bool     g_bDrawAISSize;
extern bool     g_bShowAISName;
extern int      g_Show_Target_Name_Scale;
extern bool     g_bWplIsAprsPosition;
extern double gLat;
extern double gLon;
extern double gCog;
extern double gSog;
extern double gHdt;
extern double gHdm;
extern double gVar;
extern bool g_bAIS_CPA_Alert;
extern bool g_bAIS_CPA_Alert_Audio;

BEGIN_EVENT_TABLE(AIS_Decoder, wxEvtHandler)
    EVT_TIMER(TIMER_AIS1, AIS_Decoder::OnTimerAIS)
    EVT_TIMER(TIMER_AISAUDIO, AIS_Decoder::OnTimerAISAudio)
    EVT_TIMER(TIMER_DSC, AIS_Decoder::OnTimerDSC)
END_EVENT_TABLE()

static int n_msgs;
static int n_msg1;
static int n_msg5;
static int n_msg24;
static int n_newname;
static bool b_firstrx;
static int first_rx_ticks;
static int rx_ticks;
static double arpa_ref_hdg = NAN;

AIS_Decoder::AIS_Decoder( wxFrame *parent )
{
    AISTargetList = new AIS_Target_Hash;
    AIS_AreaNotice_Sources = new AIS_Target_Hash;
    BuildERIShipTypeHash();

    g_pais_alert_dialog_active = NULL;
    m_bAIS_Audio_Alert_On = false;

    m_n_targets = 0;

    m_parent_frame = parent;

    TimerAIS.SetOwner(this, TIMER_AIS1);
    TimerAIS.Start(TIMER_AIS_MSEC,wxTIMER_CONTINUOUS);
    
    m_dsc_timer.SetOwner( this, TIMER_DSC );
    

    //  Create/connect a dynamic event handler slot for wxEVT_OCPN_DATASTREAM(s)
    Connect(wxEVT_OCPN_DATASTREAM, (wxObjectEventFunction)(wxEventFunction)&AIS_Decoder::OnEvtAIS);
}

AIS_Decoder::~AIS_Decoder( void )
{
    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
        AIS_Target_Data *td = it->second;

        delete td;
    }

    delete current_targets;
    delete AIS_AreaNotice_Sources;
    
    m_dsc_timer.Stop();
    m_AIS_Audio_Alert_Timer.Stop();
    TimerAIS.Stop();

#ifdef AIS_DEBUG
    printf("First message[1, 2] ticks: %d  Last Message [1,2]ticks %d  Difference:  %d\n", first_rx_ticks, rx_ticks, rx_ticks - first_rx_ticks);
#endif
}

void AIS_Decoder::BuildERIShipTypeHash(void)
{
      make_hash_ERI(8000, _("Vessel, type unknown"));
      make_hash_ERI(8150, _("Freightbarge"));
      make_hash_ERI(8160, _("Tankbarge"));
      make_hash_ERI(8163, _("Tankbarge, dry cargo as if liquid (e.g. cement)"));
      make_hash_ERI(8450, _("Service vessel, police patrol, port service"));
      make_hash_ERI(8430, _("Pushboat, single"));
      make_hash_ERI(8510, _("Object, not otherwise specified"));
      make_hash_ERI(8470, _("Object, towed, not otherwise specified"));
      make_hash_ERI(8490, _("Bunkership"));
      make_hash_ERI(8010, _("Motor freighter"));
      make_hash_ERI(8020, _("Motor tanker"));
      make_hash_ERI(8021, _("Motor tanker, liquid cargo, type N"));
      make_hash_ERI(8022, _("Motor tanker, liquid cargo, type C"));
      make_hash_ERI(8023, _("Motor tanker, dry cargo as if liquid (e.g. cement)"));
      make_hash_ERI(8030, _("Container vessel"));
      make_hash_ERI(8040, _("Gas tanker"));
      make_hash_ERI(8050, _("Motor freighter, tug"));
      make_hash_ERI(8060, _("Motor tanker, tug"));
      make_hash_ERI(8070, _("Motor freighter with one or more ships alongside"));
      make_hash_ERI(8080, _("Motor freighter with tanker"));
      make_hash_ERI(8090, _("Motor freighter pushing one or more freighters"));
      make_hash_ERI(8100, _("Motor freighter pushing at least one tank-ship"));
      make_hash_ERI(8110, _("Tug, freighter"));
      make_hash_ERI(8120, _("Tug, tanker"));
      make_hash_ERI(8130, _("Tug freighter, coupled"));
      make_hash_ERI(8140, _("Tug, freighter/tanker, coupled"));
      make_hash_ERI(8161, _("Tankbarge, liquid cargo, type N"));
      make_hash_ERI(8162, _("Tankbarge, liquid cargo, type C"));
      make_hash_ERI(8170, _("Freightbarge with containers"));
      make_hash_ERI(8180, _("Tankbarge, gas"));
      make_hash_ERI(8210, _("Pushtow, one cargo barge"));
      make_hash_ERI(8220, _("Pushtow, two cargo barges"));
      make_hash_ERI(8230, _("Pushtow, three cargo barges"));
      make_hash_ERI(8240, _("Pushtow, four cargo barges"));
      make_hash_ERI(8250, _("Pushtow, five cargo barges"));
      make_hash_ERI(8260, _("Pushtow, six cargo barges"));
      make_hash_ERI(8270, _("Pushtow, seven cargo barges"));
      make_hash_ERI(8280, _("Pushtow, eight cargo barges"));
      make_hash_ERI(8290, _("Pushtow, nine or more barges"));
      make_hash_ERI(8310, _("Pushtow, one tank/gas barge"));
      make_hash_ERI(8320, _("Pushtow, two barges at least one tanker or gas barge"));
      make_hash_ERI(8330, _("Pushtow, three barges at least one tanker or gas barge"));
      make_hash_ERI(8340, _("Pushtow, four barges at least one tanker or gas barge"));
      make_hash_ERI(8350, _("Pushtow, five barges at least one tanker or gas barge"));
      make_hash_ERI(8360, _("Pushtow, six barges at least one tanker or gas barge"));
      make_hash_ERI(8370, _("Pushtow, seven barges at least one tanker or gas barge"));
      make_hash_ERI(8380, _("Pushtow, eight barges at least one tanker or gas barge"));
      make_hash_ERI(8390, _("Pushtow, nine or more barges at least one tanker or gas barge"));
      make_hash_ERI(8400, _("Tug, single"));
      make_hash_ERI(8410, _("Tug, one or more tows"));
      make_hash_ERI(8420, _("Tug, assisting a vessel or linked combination"));
      make_hash_ERI(8430, _("Pushboat, single"));
      make_hash_ERI(8440, _("Passenger ship, ferry, cruise ship, red cross ship"));
      make_hash_ERI(8441, _("Ferry"));
      make_hash_ERI(8442, _("Red cross ship"));
      make_hash_ERI(8443, _("Cruise ship"));
      make_hash_ERI(8444, _("Passenger ship without accomodation"));
      make_hash_ERI(8460, _("Vessel, work maintainance craft, floating derrick, cable-ship, buoy-ship, dredge"));
      make_hash_ERI(8480, _("Fishing boat"));
      make_hash_ERI(8500, _("Barge, tanker, chemical"));
      make_hash_ERI(1500, _("General cargo Vessel maritime"));
      make_hash_ERI(1510, _("Unit carrier maritime"));
      make_hash_ERI(1520, _("Bulk carrier maritime"));
      make_hash_ERI(1530, _("Tanker"));
      make_hash_ERI(1540, _("Liquified gas tanker"));
      make_hash_ERI(1850, _("Pleasure craft, longer than 20 metres"));
      make_hash_ERI(1900, _("Fast ship"));
      make_hash_ERI(1910, _("Hydrofoil"));
}

//----------------------------------------------------------------------------------
//     Handle events from AIS DataStream
//----------------------------------------------------------------------------------
void AIS_Decoder::OnEvtAIS( OCPN_DataStreamEvent& event )
{
    wxString message = wxString(event.GetNMEAString().c_str(), wxConvUTF8);

    int nr = 0;
    if( !message.IsEmpty() )
    {
        if( message.Mid( 3, 3 ).IsSameAs( _T("VDM") ) ||
            message.Mid( 3, 3 ).IsSameAs( _T("VDO") ) ||
            message.Mid( 1, 5 ).IsSameAs( _T("FRPOS") ) ||
            message.Mid( 1, 2 ).IsSameAs( _T("CD") ) ||
            message.Mid( 3, 3 ).IsSameAs( _T("TLL") ) ||
            message.Mid( 3, 3 ).IsSameAs( _T("TTM") ) ||
            message.Mid( 3, 3 ).IsSameAs( _T("OSD") ) ||
            ( g_bWplIsAprsPosition && message.Mid( 3, 3 ).IsSameAs( _T("WPL") ) ) )
        {
                nr = Decode( message );
                gFrame->TouchAISActive();
        }
    }
}

//----------------------------------------------------------------------------------
//      Decode a single AIVDO sentence to a Generic Position Report
//----------------------------------------------------------------------------------
AIS_Error AIS_Decoder::DecodeSingleVDO( const wxString& str, GenericPosDatEx *pos, wxString *accumulator )
{
    //  Make some simple tests for validity
    if( str.Len() > 100 )
        return AIS_NMEAVDX_TOO_LONG;

    if( !NMEACheckSumOK( str ) )
        return AIS_NMEAVDX_CHECKSUM_BAD;

    if( !pos )
        return AIS_GENERIC_ERROR;

    if( !accumulator )
        return AIS_GENERIC_ERROR;

    //  We only process AIVDO messages
    if( !str.Mid( 1, 5 ).IsSameAs( _T("AIVDO") ) )
        return AIS_GENERIC_ERROR;

    //  Use a tokenizer to pull out the first 4 fields
    wxStringTokenizer tkz( str, _T(",") );

    wxString token;
    token = tkz.GetNextToken();         // !xxVDx

    token = tkz.GetNextToken();
    int nsentences = atoi( token.mb_str() );

    token = tkz.GetNextToken();
    int isentence = atoi( token.mb_str() );

    token = tkz.GetNextToken();         // skip 2 fields
    token = tkz.GetNextToken();

    wxString string_to_parse;
    string_to_parse.Clear();

    //  Fill the output structure with all NANs
    pos->kLat = NAN;
    pos->kLon = NAN;
    pos->kCog = NAN;
    pos->kSog = NAN;
    pos->kHdt = NAN;
    pos->kVar = NAN;
    pos->kHdm = NAN;

    //  Simple case first
    //  First and only part of a one-part sentence
    if( ( 1 == nsentences ) && ( 1 == isentence ) ) {
        string_to_parse = tkz.GetNextToken();         // the encapsulated data
    }

    else if( nsentences > 1 ) {
        if( 1 == isentence ) {
            *accumulator = tkz.GetNextToken();         // the encapsulated data
        }

        else {
            accumulator->Append(tkz.GetNextToken() );
        }

        if( isentence == nsentences ) {         // ready to parse
            string_to_parse = *accumulator;
        }
    }

    if( string_to_parse.IsEmpty() && (nsentences > 1) ) {      // not ready, so return with NAN
        return AIS_INCOMPLETE_MULTIPART;                       // and non-zero return
    }


    //  Create the bit accessible string
    AIS_Bitstring strbit( string_to_parse.mb_str() );

    AIS_Target_Data *pTargetData = new AIS_Target_Data;

    bool bdecode_result = Parse_VDXBitstring( &strbit, pTargetData );

    if(bdecode_result) {
        switch(pTargetData->MID)
        {
            case 1:
            case 2:
            case 3:
            case 18:
            {
                if( !pTargetData->b_positionDoubtful ) {
                    pos->kLat = pTargetData->Lat;
                    pos->kLon = pTargetData->Lon;
                }
                else {
                    pos->kLat = NAN;
                    pos->kLon = NAN;
                }

                if(pTargetData->COG == 360.0)
                    pos->kCog = NAN;
                else
                    pos->kCog = pTargetData->COG;


                if(pTargetData->SOG > 102.2)
                    pos->kSog = NAN;
                else
                    pos->kSog = pTargetData->SOG;

                if((int)pTargetData->HDG == 511)
                    pos->kHdt = NAN;
                else
                    pos->kHdt = pTargetData->HDG;

                //  VDO messages do not contain variation or magnetic heading
                pos->kVar = NAN;
                pos->kHdm = NAN;
                break;
            }
            default:
                return AIS_GENERIC_ERROR;       // unrecognised sentence
        }

        return AIS_NoError;
    }
    else
        return AIS_GENERIC_ERROR;
}


//----------------------------------------------------------------------------------------
//      Decode NMEA VDM/VDO/FRPOS/DSCDSE/TTM/TLL/OSD/RSD/TLB/WPL sentence to AIS Target(s)
//----------------------------------------------------------------------------------------
AIS_Error AIS_Decoder::Decode( const wxString& str )
{
    AIS_Error ret;
    wxString string_to_parse;

    double gpsg_lat, gpsg_lon, gpsg_mins, gpsg_degs;
    double gpsg_cog, gpsg_sog, gpsg_utc_time;
    int gpsg_utc_hour = 0;
    int gpsg_utc_min = 0;
    int gpsg_utc_sec = 0;
    char gpsg_name_str[21];
    wxString gpsg_date;
    
    bool bdecode_result = false; 
    
    bool  b_dsx = false;
    
    int gpsg_mmsi = 0;
    int arpa_mmsi = 0;
    int aprs_mmsi = 0;
    int mmsi = 0;

    long arpa_tgt_num = 0;
    double arpa_sog = 0.;
    double arpa_cog = 0.;
    double arpa_lat = 0.;
    double arpa_lon = 0.;
    double arpa_dist = 0.;
    double arpa_brg = 0.;
    wxString arpa_brgunit;
    wxString arpa_status;
    wxString arpa_distunit;
    wxString arpa_cogunit;
    wxString arpa_reftarget;
    double arpa_mins, arpa_degs;
    double arpa_utc_time;
    int arpa_utc_hour = 0;
    int arpa_utc_min = 0;
    int arpa_utc_sec = 0;
    char arpa_name_str[21];
    bool arpa_lost = true;
    bool arpa_nottracked = false;

    double aprs_sog = 0.;
    double aprs_cog = 0.;
    double aprs_lat = 0.;
    double aprs_lon = 0.;
    char aprs_name_str[21];
    double aprs_mins, aprs_degs;

    AIS_Target_Data *pTargetData;
    AIS_Target_Data *pStaleTarget = NULL;
    bool bnewtarget = false;
    int last_report_ticks;
    
    //  Make some simple tests for validity

    if( str.Len() > 100 ) return AIS_NMEAVDX_TOO_LONG;

    if( !NMEACheckSumOK( str ) ) {
            return AIS_NMEAVDX_CHECKSUM_BAD;
    }
    if( str.Mid( 1, 2 ).IsSameAs( _T("CD") ) ) {
        ProcessDSx( str );
        return AIS_NoError;
    }
    else if( str.Mid( 3, 3 ).IsSameAs( _T("TTM") ) ) {
    //$--TTM,xx,x.x,x.x,a,x.x,x.x,a,x.x,x.x,a,c--c,a,a*hh <CR><LF>
    //or
    //$--TTM,xx,x.x,x.x,a,x.x,x.x,a,x.x,x.x,a,c--c,a,a,hhmmss.ss,a*hh<CR><LF>
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",*") );

        wxString token;
        token = tkz.GetNextToken(); //Sentence (xxTTM)
        token = tkz.GetNextToken(); //1) Target Number
        token.ToLong( &arpa_tgt_num );
        token = tkz.GetNextToken(); // 2)Target Distance
        token.ToDouble( &arpa_dist );
        token = tkz.GetNextToken(); //3) Bearing from own ship
        token.ToDouble( &arpa_brg );
        arpa_brgunit = tkz.GetNextToken(); //4) Bearing Units
        if ( arpa_brgunit == _T("R") )
        {
            if ( wxIsNaN(arpa_ref_hdg) )
            {
                if ( !wxIsNaN(gHdt) )
                    arpa_brg += gHdt;
                else
                    arpa_brg += gCog;
            }
            else
                arpa_brg += arpa_ref_hdg;
            if ( arpa_brg >= 360. )
                arpa_brg -= 360.;
        }
        token = tkz.GetNextToken(); //5) Target speed
        token.ToDouble( &arpa_sog );
        token = tkz.GetNextToken(); //6) Target Course
        token.ToDouble( &arpa_cog );
        arpa_cogunit = tkz.GetNextToken(); //7) Course Units
        if ( arpa_cogunit == _T("R") )
        {
            if ( wxIsNaN(arpa_ref_hdg) )
            {
                if ( !wxIsNaN(gHdt) )
                    arpa_cog += gHdt;
                else
                    arpa_cog += gCog;
            }
            else
                arpa_cog += arpa_ref_hdg;
            if ( arpa_cog >= 360. )
                arpa_cog -= 360.;
        }
        token = tkz.GetNextToken(); //8) Distance of closest-point-of-approach
        token = tkz.GetNextToken(); //9) Time until closest-point-of-approach "-" means increasing
        arpa_distunit = tkz.GetNextToken(); //10)Speed/ dist unit
        token = tkz.GetNextToken(); //11) Target name
        if ( token == wxEmptyString )
            token = wxString::Format( _T("ARPA %d"), arpa_tgt_num );
        int len = token.Length();
        strncpy( arpa_name_str, token.mb_str(), len );
        arpa_name_str[len] = 0;
        arpa_status = tkz.GetNextToken(); //12) Target Status
        if (arpa_status != _T("L"))
            arpa_lost = false;
        arpa_reftarget = tkz.GetNextToken(); //13) Reference Target
        if ( tkz.HasMoreTokens() )
        {
            token = tkz.GetNextToken();
            token.ToDouble( &arpa_utc_time );
            arpa_utc_hour = (int) ( arpa_utc_time / 10000.0 );
            arpa_utc_min = (int) ( arpa_utc_time / 100.0 ) - arpa_utc_hour * 100;
            arpa_utc_sec = (int) arpa_utc_time - arpa_utc_hour * 10000 - arpa_utc_min * 100;
        } else {
            arpa_utc_hour = wxDateTime::Now().ToUTC().GetHour();
            arpa_utc_min = wxDateTime::Now().ToUTC().GetMinute();
            arpa_utc_sec = wxDateTime::Now().ToUTC().GetSecond();
        }

        if( arpa_distunit == _T("K") )
        {
            arpa_dist = fromUsrDistance( arpa_dist, DISTANCE_KM );
            arpa_sog = fromUsrSpeed(arpa_sog, SPEED_KMH);
        } else if( arpa_distunit == _T("S") ) {
            arpa_dist = fromUsrDistance( arpa_dist, DISTANCE_MI );
            arpa_sog = fromUsrSpeed(arpa_sog, SPEED_MPH);
        }

        mmsi = arpa_mmsi = 199200000 + arpa_tgt_num; // 199 is INMARSAT-A MID, should not occur ever in AIS stream + we make sure we are out of the hashes for GPSGate buddies by being above 1992*
    } else if( str.Mid( 3, 3 ).IsSameAs( _T("TLL") ) ) {
    //$--TLL,xx,llll.lll,a,yyyyy.yyy,a,c--c,hhmmss.ss,a,a*hh<CR><LF>
    //"$RATLL,01,5603.370,N,01859.976,E,ALPHA,015200.36,T,*75\r\n"
        wxString aprs_tll_str;
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",*") );

        wxString token;
        aprs_tll_str = tkz.GetNextToken(); //Sentence (xxTLL)
        token = tkz.GetNextToken(); //1) Target number 00 - 99
        token.ToLong( &arpa_tgt_num );
        token = tkz.GetNextToken(); //2) Latitude, N/S
        token.ToDouble( &arpa_lat );
        arpa_degs = (int) ( arpa_lat / 100.0 );
        arpa_mins = arpa_lat - arpa_degs * 100.0;
        arpa_lat = arpa_degs + arpa_mins / 60.0;
        token = tkz.GetNextToken(); // hemisphere N or S
        if( token.Mid( 0, 1 ).Contains( _T("S") ) == true || token.Mid( 0, 1 ).Contains(  _T("s") ) == true )
            arpa_lat = 0. - arpa_lat;
        token = tkz.GetNextToken(); //3) Longitude, E/W
        token.ToDouble( &arpa_lon );
        arpa_degs = (int) ( arpa_lon / 100.0 );
        arpa_mins = arpa_lon - arpa_degs * 100.0;
        arpa_lon = arpa_degs + arpa_mins / 60.0;
        token = tkz.GetNextToken(); // hemisphere E or W
        if( token.Mid( 0, 1 ).Contains(  _T("W") ) == true || token.Mid( 0, 1 ).Contains(  _T("w") ) == true )
            arpa_lon = 0. - arpa_lon;
        token = tkz.GetNextToken(); //4) Target name
        if ( token == wxEmptyString )
            token = wxString::Format( _T("ARPA %d"), arpa_tgt_num );
        int len = token.Length();
        strncpy( arpa_name_str, token.mb_str(), len );
        arpa_name_str[len] = 0;
        token = tkz.GetNextToken(); //5) UTC of data
        token.ToDouble( &arpa_utc_time );
        arpa_utc_hour = (int) ( arpa_utc_time / 10000.0 );
        arpa_utc_min = (int) ( arpa_utc_time / 100.0 ) - arpa_utc_hour * 100;
        arpa_utc_sec = (int) arpa_utc_time - arpa_utc_hour * 10000 - arpa_utc_min * 100;
        arpa_status = tkz.GetNextToken(); //6) Target status: L = lost,tracked target has beenlost Q = query,target in the process of acquisition T = tracking
        if( arpa_status != _T("L") )
            arpa_lost = false;
        else if ( arpa_status != wxEmptyString )
            arpa_nottracked = true;
        arpa_reftarget = tkz.GetNextToken(); //7) Reference target=R,null otherwise
        mmsi = arpa_mmsi = 199200000 + arpa_tgt_num; // 199 is INMARSAT-A MID, should not occur ever in AIS stream + we make sure we are out of the hashes for GPSGate buddies by being above 1992*
    } else if( str.Mid( 3, 3 ).IsSameAs( _T("OSD") ) ) {
    //$--OSD,x.x,A,x.x,a,x.x,a,x.x,x.x,a*hh <CR><LF>
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",*") );

        wxString token;
        token = tkz.GetNextToken(); //Sentence (xxOSD)
        token = tkz.GetNextToken(); //1) Heading (true)
        token.ToDouble( &arpa_ref_hdg );
        //2) speed
        //3) Vessel Course, degrees True
        //4) Course Reference, B/M/W/R/P (see note)
        //5) Vessel Speed
        //6) Speed Reference, B/M/W/R/P (see note)
        //7) Vessel Set, degrees True - Manually entered
        //8) Vessel drift (speed) - Manually entered
        //9) Speed Units K = km/h; N = Knots; S = statute miles/h
    } else if( str.Mid( 3, 3 ).IsSameAs( _T("WPL") ) ) {
    //** $--WPL,llll.ll,a,yyyyy.yy,a,c--c*hh<CR><LF>
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",*") );

        wxString token;
        token = tkz.GetNextToken(); //Sentence (xxWPL)
        token = tkz.GetNextToken(); //1) Latitude, N/S
        token.ToDouble( &aprs_lat );
        aprs_degs = (int) ( aprs_lat / 100.0 );
        aprs_mins = aprs_lat - aprs_degs * 100.0;
        aprs_lat = aprs_degs + aprs_mins / 60.0;
        token = tkz.GetNextToken();            //2) hemisphere N or S
        if( token.Mid( 1, 1 ).Contains( _T("Ss") ) )
            aprs_lat = 0. - aprs_lat;
        token = tkz.GetNextToken(); //3) Longitude, E/W
        token.ToDouble( &aprs_lon );
        aprs_degs = (int) ( aprs_lon / 100.0 );
        aprs_mins = aprs_lon - aprs_degs * 100.0;
        aprs_lon = aprs_degs + aprs_mins / 60.0;
        token = tkz.GetNextToken();            //4) hemisphere E or W
        if( token.Mid( 1, 1 ).Contains( _T("Ww") ) )
            aprs_lon = 0. - aprs_lon;
        token = tkz.GetNextToken(); //5) Target name
        int len = token.Length();
        int i, hash = 0;
        strncpy( aprs_name_str, token.mb_str(), len );
        aprs_name_str[len] = 0;
        hash = 0;
        for( i = 0; i < len; i++ ) {
            hash = hash * 10;
            hash += (int) ( aprs_name_str[i] );
            while( hash >= 100000 )
                hash = hash / 100000;
        }
        mmsi = aprs_mmsi = 199300000 + hash; // 199 is INMARSAT-A MID, should not occur ever in AIS stream + we make sure we are out of the hashes for GPSGate buddies and ARPA by being above 1993*
    } else if( str.Mid( 1, 5 ).IsSameAs( _T("FRPOS") ) ) {
        // parse a GpsGate Position message            $FRPOS,.....

        //  Use a tokenizer to pull out the first 9 fields
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",*") );

        wxString token;
        token = tkz.GetNextToken();         // !$FRPOS

        token = tkz.GetNextToken();            //    latitude DDMM.MMMM
        token.ToDouble( &gpsg_lat );
        gpsg_degs = (int) ( gpsg_lat / 100.0 );
        gpsg_mins = gpsg_lat - gpsg_degs * 100.0;
        gpsg_lat = gpsg_degs + gpsg_mins / 60.0;

        token = tkz.GetNextToken();            //  hemisphere N or S
        if( token.Mid( 1, 1 ).Contains( _T("S") ) ) gpsg_lat = 0. - gpsg_lat;

        token = tkz.GetNextToken();            // longitude DDDMM.MMMM
        token.ToDouble( &gpsg_lon );
        gpsg_degs = (int) ( gpsg_lon / 100.0 );
        gpsg_mins = gpsg_lon - gpsg_degs * 100.0;
        gpsg_lon = gpsg_degs + gpsg_mins / 60.0;

        token = tkz.GetNextToken();            // hemisphere E or W
        if( token.Mid( 1, 1 ).Contains( _T("W") ) ) gpsg_lon = 0. - gpsg_lon;

        token = tkz.GetNextToken();            //    altitude AA.a
        //    token.toDouble(&gpsg_alt);

        token = tkz.GetNextToken();            //  speed over ground SSS.SS knots
        token.ToDouble( &gpsg_sog );

        token = tkz.GetNextToken();            //  heading over ground HHH.hh degrees
        token.ToDouble( &gpsg_cog );

        token = tkz.GetNextToken();            // date DDMMYY
        gpsg_date = token;
        
        token = tkz.GetNextToken();            // time UTC hhmmss.dd
        token.ToDouble( &gpsg_utc_time );
        gpsg_utc_hour = (int) ( gpsg_utc_time / 10000.0 );
        gpsg_utc_min = (int) ( gpsg_utc_time / 100.0 ) - gpsg_utc_hour * 100;
        gpsg_utc_sec = (int) gpsg_utc_time - gpsg_utc_hour * 10000 - gpsg_utc_min * 100;

        // now comes the name, followed by in * and NMEA checksum

        token = tkz.GetNextToken();
        int i, len, hash = 0;
        len = wxMin(wxStrlen(token),20);
        strncpy( gpsg_name_str, token.mb_str(), len );
        gpsg_name_str[len] = 0;
        for( i = 0; i < len; i++ ) {
            hash = hash * 10;
            hash += (int) ( token[i] );
            while( hash >= 100000 )
                hash = hash / 100000;
        }
        gpsg_mmsi = 199000000 + hash;  // 199 is INMARSAT-A MID, should not occur ever in AIS stream
        mmsi = gpsg_mmsi;
    } else if( !str.Mid( 3, 2 ).IsSameAs( _T("VD") ) ) {
        return AIS_NMEAVDX_BAD;
    }

    //  OK, looks like the sentence is OK

        //  Use a tokenizer to pull out the first 4 fields
        wxString string( str );
        wxStringTokenizer tkz( string, _T(",") );

        wxString token;
        token = tkz.GetNextToken();         // !xxVDx

        token = tkz.GetNextToken();
        nsentences = atoi( token.mb_str() );

        token = tkz.GetNextToken();
        isentence = atoi( token.mb_str() );

        token = tkz.GetNextToken();
        long lsequence_id = 0;
        token.ToLong( &lsequence_id );

        token = tkz.GetNextToken();
        long lchannel;
        token.ToLong( &lchannel );

        //  Now, some decisions

        string_to_parse.Clear();

        //  Simple case first
        //  First and only part of a one-part sentence
        if( ( 1 == nsentences ) && ( 1 == isentence ) ) {
            string_to_parse = tkz.GetNextToken();         // the encapsulated data
        }

        else if( nsentences > 1 ) {
            if( 1 == isentence ) {
                sentence_accumulator = tkz.GetNextToken();         // the encapsulated data
            }

            else {
                sentence_accumulator += tkz.GetNextToken();
            }

            if( isentence == nsentences ) {
                string_to_parse = sentence_accumulator;
            }
        }

        if( mmsi || ( !string_to_parse.IsEmpty() && ( string_to_parse.Len() < AIS_MAX_MESSAGE_LEN ) ) ) {

            //  Create the bit accessible string
            wxCharBuffer abuf = string_to_parse.ToUTF8();
            if( !abuf.data() )                            // badly formed sentence?
                return AIS_GENERIC_ERROR;
            
            AIS_Bitstring strbit( abuf.data() );

            //  Extract the MMSI
            if( !mmsi ) mmsi = strbit.GetInt( 9, 30 );
            long mmsi_long = mmsi;

             //  Search the current AISTargetList for an MMSI match
            AIS_Target_Hash::iterator it = AISTargetList->find( mmsi );
            if( it == AISTargetList->end() )                  // not found
                    {
                pTargetData = new AIS_Target_Data;
                bnewtarget = true;
                m_n_targets++;
            } else {
                pTargetData = ( *AISTargetList )[mmsi];          // find current entry
                pStaleTarget = pTargetData;                   // save a pointer to stale data
            }

            //  Grab the stale targets's last report time
             wxDateTime now = wxDateTime::Now();
            now.MakeGMT();

            if( pStaleTarget )
                last_report_ticks = pStaleTarget->PositionReportTicks;
            else
                last_report_ticks = now.GetTicks();

            // Delete the stale AIS Target selectable point
            if( pStaleTarget )
                pSelectAIS->DeleteSelectablePoint( (void *) mmsi_long, SELTYPE_AISTARGET );

            bool bhad_name = false;
            if( pStaleTarget ) bhad_name = pStaleTarget->b_nameValid;

            if( gpsg_mmsi && pTargetData ) {
                pTargetData->PositionReportTicks = now.GetTicks();
                pTargetData->StaticReportTicks = now.GetTicks();
                pTargetData->m_utc_hour = gpsg_utc_hour;
                pTargetData->m_utc_min = gpsg_utc_min;
                pTargetData->m_utc_sec = gpsg_utc_sec;
                pTargetData->m_date_string = gpsg_date;
                pTargetData->MMSI = gpsg_mmsi;
                pTargetData->NavStatus = 0; // underway
                pTargetData->Lat = gpsg_lat;
                pTargetData->Lon = gpsg_lon;
                pTargetData->b_positionOnceValid = true;
                pTargetData->COG = gpsg_cog;
                pTargetData->SOG = gpsg_sog;
                pTargetData->ShipType = 52; // buddy
                pTargetData->Class = AIS_GPSG_BUDDY;
                strncpy( pTargetData->ShipName, gpsg_name_str, strlen( gpsg_name_str ) + 1 );
                pTargetData->b_nameValid = true;
                pTargetData->b_active = true;
                pTargetData->b_lost = false;

                bdecode_result = true;
            } else if( arpa_mmsi && pTargetData ) {
                pTargetData->m_utc_hour = arpa_utc_hour;
                pTargetData->m_utc_min = arpa_utc_min;
                pTargetData->m_utc_sec = arpa_utc_sec;
                pTargetData->MMSI = arpa_mmsi;
                pTargetData->NavStatus = 15; // undefined
                if( str.Mid( 3, 3 ).IsSameAs( _T("TLL") ) ) {
                    if( !bnewtarget ) {
                        int age_of_last = ( now.GetTicks() - pTargetData->PositionReportTicks );
                        if ( age_of_last > 0 ) {
                            ll_gc_ll_reverse( pTargetData->Lat, pTargetData->Lon, arpa_lat, arpa_lon, &pTargetData->COG, &pTargetData->SOG );
                            pTargetData->SOG = pTargetData->SOG * 3600 / age_of_last;
                        }
                    }
                    pTargetData->Lat = arpa_lat;
                    pTargetData->Lon = arpa_lon;
                } else if( str.Mid( 3, 3 ).IsSameAs( _T("TTM") ) ) {
                    if( arpa_dist != 0. ) //Not a new or turned off target
                        ll_gc_ll( gLat, gLon, arpa_brg, arpa_dist, &pTargetData->Lat, &pTargetData->Lon );
                    else
                        arpa_lost = true;
                    pTargetData->COG = arpa_cog;
                    pTargetData->SOG = arpa_sog;
                }
                pTargetData->PositionReportTicks = now.GetTicks();
                pTargetData->StaticReportTicks = now.GetTicks();
                pTargetData->b_positionOnceValid = true;
                pTargetData->ShipType = 55; // arpa
                pTargetData->Class = AIS_ARPA;

                strncpy( pTargetData->ShipName, arpa_name_str, strlen( arpa_name_str ) + 1 );
                if( arpa_status != _T("Q") )
                    pTargetData->b_nameValid = true;
                else
                    pTargetData->b_nameValid = false;
                pTargetData->b_active = !arpa_lost;
                pTargetData->b_lost = arpa_nottracked;

                bdecode_result = true;
            } else if( aprs_mmsi && pTargetData ) {
                pTargetData->m_utc_hour = now.GetHour();
                pTargetData->m_utc_min = now.GetMinute();
                pTargetData->m_utc_sec = now.GetSecond();
                pTargetData->MMSI = aprs_mmsi;
                pTargetData->NavStatus = 15; // undefined
                if( !bnewtarget ) {
                    int age_of_last = (now.GetTicks() - pTargetData->PositionReportTicks);
                    if ( age_of_last > 0 ) {
                        ll_gc_ll_reverse( pTargetData->Lat, pTargetData->Lon, aprs_lat, aprs_lon, &pTargetData->COG, &pTargetData->SOG );
                        pTargetData->SOG = pTargetData->SOG * 3600 / age_of_last;
                    }
                }
                pTargetData->PositionReportTicks = now.GetTicks();
                pTargetData->StaticReportTicks = now.GetTicks();
                pTargetData->Lat = aprs_lat;
                pTargetData->Lon = aprs_lon;
                pTargetData->b_positionOnceValid = true;
                pTargetData->ShipType = 56; // aprs
                pTargetData->Class = AIS_APRS;
                strncpy( pTargetData->ShipName, aprs_name_str, strlen( aprs_name_str ) + 1 );
                pTargetData->b_nameValid = true;
                pTargetData->b_active = true;
                pTargetData->b_lost = false;

                bdecode_result = true;
            } else
                bdecode_result = Parse_VDXBitstring( &strbit, pTargetData );       // Parse the new data

                //     Update the most recent report period
            if( pTargetData )    
                pTargetData->RecentPeriod = pTargetData->PositionReportTicks - last_report_ticks;
            ret = AIS_NoError;
            
        } else{
            ret = AIS_Partial;                // accumulating parts of a multi-sentence message
            pTargetData = 0;
        }
        

        if(pTargetData){
            //  pTargetData is valid, either new or existing. Continue processing

            m_pLatestTargetData = pTargetData;

            if( str.Mid( 3, 3 ).IsSameAs( _T("VDO") ) ) pTargetData->b_OwnShip = true;

    //        if( ( bdecode_result ) && ( pTargetData->b_nameValid ) && ( pStaleTarget ) ) if( !bhad_name ) n_newname++;

            //  If the message was decoded correctly
            //  Update the AIS Target information
            if( bdecode_result ) {
                ( *AISTargetList )[pTargetData->MMSI] = pTargetData;            // update the hash table entry

                if( !pTargetData->area_notices.empty() ) {
                    AIS_Target_Hash::iterator it = AIS_AreaNotice_Sources->find( pTargetData->MMSI );
                    if( it == AIS_AreaNotice_Sources->end() )
                        ( *AIS_AreaNotice_Sources ) [pTargetData->MMSI] = pTargetData;
                }


                //  If this is not an ownship message, update the AIS Target in the Selectable list, and update the CPA info
                if( !pTargetData->b_OwnShip ) {
                    if( pTargetData->b_positionOnceValid ) {
                        long mmsi_long = pTargetData->MMSI;
                        SelectItem *pSel = pSelectAIS->AddSelectablePoint( pTargetData->Lat,
                                pTargetData->Lon, (void *) mmsi_long, SELTYPE_AISTARGET );
                        pSel->SetUserData( pTargetData->MMSI );
                    }

                    //    Calculate CPA info for this target immediately
                    UpdateOneCPA( pTargetData );

                    //    Update this target's track
                    if( pTargetData->b_show_track )
                        UpdateOneTrack( pTargetData );
                }
            } else {
    //             printf("Unrecognised AIS message ID: %d\n", pTargetData->MID);
                if( bnewtarget ) {
                    delete pTargetData;                           // this target is not going to be used
                    m_n_targets--;
                } else {
                    //  If this is not an ownship message, update the AIS Target in the Selectable list
                    //  even if the message type was not recognized
                    if( !pTargetData->b_OwnShip ) {
                        if( pTargetData->b_positionOnceValid ) {
                            long mmsi_long = pTargetData->MMSI;
                            SelectItem *pSel = pSelectAIS->AddSelectablePoint( pTargetData->Lat,
                                                pTargetData->Lon, (void *) mmsi_long, SELTYPE_AISTARGET );
                            pSel->SetUserData( pTargetData->MMSI );
                        }
                    }
                }
            }
        }

    n_msgs++;
#ifdef AIS_DEBUG
    if((n_msgs % 10000) == 0)
    printf("n_msgs %10d m_n_targets: %6d  n_msg1: %10d  n_msg5+24: %10d  n_new5: %10d \n", n_msgs, m_n_targets, n_msg1, n_msg5 + n_msg24, n_newname);
#endif

    return ret;
}

AIS_Target_Data *AIS_Decoder::ProcessDSx( const wxString& str, bool b_take_dsc )
{
    double dsc_lat = 0.;
    double dsc_lon = 0.;
    double dsc_mins, dsc_degs, dsc_tmp, dsc_addr;
    double dse_tmp, dse_addr;
    double dse_lat = 0.;
    double dse_lon = 0.;
    long dsc_fmt, dsc_quadrant;
    
    int dsc_mmsi = 0;
    int dse_mmsi = 0;
    int mmsi = 0;
    
    AIS_Target_Data *pTargetData = NULL;
    AIS_Target_Data *pStaleTarget = NULL;
    
    // parse a DSC Position message            $CDDSx,.....
    //  Use a tokenizer to pull out the first 9 fields
    wxString string( str );
    wxStringTokenizer tkz( string, _T(",*") );
    
    wxString token;
    token = tkz.GetNextToken();         // !$CDDS
    
    if( str.Mid( 3, 3 ).IsSameAs( _T("DSC") ) ) {
        m_dsc_last_string = str;
        
        token = tkz.GetNextToken(); // format specifier (02-area,12-distress,16-allships,20-individual,...)
        token.ToLong( &dsc_fmt );
        
        token = tkz.GetNextToken();       // address i.e. mmsi*10 for received msg, or area spec
        token.ToDouble( &dsc_addr );
        dsc_mmsi = 0 - (int) ( dsc_addr / 10 ); // as per NMEA 0183 3.01
        
        token = tkz.GetNextToken();         // category
        token = tkz.GetNextToken();         // nature of distress or telecommand1
        token = tkz.GetNextToken();         // comm type or telecommand2
        
        token = tkz.GetNextToken();         // position or channel/freq
        token.ToDouble( &dsc_tmp );
        
        token = tkz.GetNextToken();         // time or tel. no.
        token = tkz.GetNextToken();         // mmsi of ship in distress
        token = tkz.GetNextToken();         // nature of distress
        token = tkz.GetNextToken();         // acknowledgement
        token = tkz.GetNextToken();         // expansion indicator
        
        dsc_quadrant = (int) (dsc_tmp / 1000000000.0);
        
        dsc_lat = (int) ( dsc_tmp / 100000.0 );
        dsc_lon = dsc_tmp - dsc_lat * 100000.0;
        dsc_lat = dsc_lat - dsc_quadrant * 10000;
        dsc_degs = (int) ( dsc_lat / 100.0 );
        dsc_mins = dsc_lat - dsc_degs * 100.0;
        dsc_lat = dsc_degs + dsc_mins / 60.0;
        
        dsc_degs = (int) ( dsc_lon / 100.0 );
        dsc_mins = dsc_lon - dsc_degs * 100.0;
        dsc_lon = dsc_degs + dsc_mins / 60.0;
        switch( dsc_quadrant ) {
            case 0: break;                                             // NE
            case 1: dsc_lon = -dsc_lon; break;                         // NW
            case 2: dsc_lat = -dsc_lat; break;                         // SE
            case 3: dsc_lon = -dsc_lon; dsc_lat = -dsc_lat; break;     // SW
            default: break;
        }
        if( dsc_fmt != 02 )
            mmsi = (int) dsc_mmsi;
    } else if( str.Mid( 3, 3 ).IsSameAs( _T("DSE") ) ) {
        
        token = tkz.GetNextToken();         // total number of sentences
        token = tkz.GetNextToken();         // sentence number
        token = tkz.GetNextToken();         // query/rely flag
        token = tkz.GetNextToken();         // vessel MMSI
        token.ToDouble( &dse_addr );
        dse_mmsi = 0 - (int) ( dse_addr / 10 ); // as per NMEA 0183 3.01
        
        token = tkz.GetNextToken();         // code field
        token = tkz.GetNextToken();         // data field - position - 2*4 digits latlon .mins
        token.ToDouble( &dse_tmp );
        dse_lat = (int) ( dse_tmp / 10000.0 );
        dse_lon = (int) ( dse_tmp - dse_lat * 10000.0 );
        dse_lat = dse_lat / 600000.0;
        dse_lon = dse_lon / 600000.0;
        
        mmsi = (int) dse_mmsi;
    }
    
    long mmsi_long = mmsi;

    //  Get the last report time for this target, if it exists
    wxDateTime now = wxDateTime::Now();
    now.MakeGMT();
    int last_report_ticks = now.GetTicks();
    
    //  Search the current AISTargetList for an MMSI match
    AIS_Target_Hash::iterator it = AISTargetList->find( mmsi );
    if( it == AISTargetList->end() ) {                 // not found
    } else {
        pStaleTarget = ( *AISTargetList )[mmsi];          // find current entry
        last_report_ticks = pStaleTarget->PositionReportTicks;
    }
    
    if( dsc_mmsi ){
        //      Create a tentative target, but do not post it pending receipt of extended data
        m_ptentative_dsctarget = new AIS_Target_Data;
        
        m_ptentative_dsctarget->PositionReportTicks = now.GetTicks();
        m_ptentative_dsctarget->StaticReportTicks = now.GetTicks();
        
        m_ptentative_dsctarget->MMSI = mmsi;
        m_ptentative_dsctarget->NavStatus = 0; // underway
        m_ptentative_dsctarget->Lat = dsc_lat;
        m_ptentative_dsctarget->Lon = dsc_lon;
        m_ptentative_dsctarget->b_positionOnceValid = true; 
        m_ptentative_dsctarget->COG = 0;
        m_ptentative_dsctarget->SOG = 0;
        m_ptentative_dsctarget->ShipType = dsc_fmt; // DSC report
        m_ptentative_dsctarget->Class = AIS_DSC;
        m_ptentative_dsctarget->b_nameValid = true;
        if( dsc_fmt == 12 ) {
            strncpy( m_ptentative_dsctarget->ShipName, "DISTRESS            ", 21 );
        }
        else
            strncpy( m_ptentative_dsctarget->ShipName, "POSITION REPORT     ", 21 );
        
        m_ptentative_dsctarget->b_active = true;
        m_ptentative_dsctarget->b_lost = false;
        m_ptentative_dsctarget->RecentPeriod = m_ptentative_dsctarget->PositionReportTicks - last_report_ticks;

        //      Start a timer, looking for an expected DSE extension message
        if( !b_take_dsc )
            m_dsc_timer.Start( 500, wxTIMER_ONE_SHOT);
    }
    
    //    Got an extension message, or the timer expired and no extension is expected
    if( dse_mmsi || b_take_dsc){
        if( m_ptentative_dsctarget ){
            
            //  stop the timer for sure
            m_dsc_timer.Stop();
            
            //  Update the extended information
            if( dse_mmsi ) {
                m_ptentative_dsctarget->Lat = m_ptentative_dsctarget->Lat + ( ( m_ptentative_dsctarget->Lat ) >= 0 ? dse_lat : -dse_lat );
                m_ptentative_dsctarget->Lon = m_ptentative_dsctarget->Lon + ( ( m_ptentative_dsctarget->Lon ) >= 0 ? dse_lon : -dse_lon );
            }
            
            //     Update the most recent report period
            m_ptentative_dsctarget->RecentPeriod = m_ptentative_dsctarget->PositionReportTicks - last_report_ticks;

            //  And post the target

            //  Search the current AISTargetList for an MMSI match
            AIS_Target_Hash::iterator it = AISTargetList->find( mmsi );
            if( it == AISTargetList->end() ) {                 // not found
                pTargetData = m_ptentative_dsctarget;
            } else {
                pTargetData = ( *AISTargetList )[mmsi];          // find current entry
                AISTargetTrackList *ptrack = pTargetData->m_ptrack;
                pTargetData->CloneFrom( m_ptentative_dsctarget);  // this will make an empty track list
                
                delete pTargetData->m_ptrack;           // get rid of the new empty one
                pTargetData->m_ptrack = ptrack;         // and substitute the old track list
                
                delete m_ptentative_dsctarget;
            }

            //  Reset for next time
            m_ptentative_dsctarget = NULL;
            
            m_pLatestTargetData = pTargetData;
            
            ( *AISTargetList )[pTargetData->MMSI] = pTargetData;            // update the hash table entry
                
            long mmsi_long = pTargetData->MMSI;

            // Delete any stale Target selectable point
            if( pStaleTarget )
                pSelectAIS->DeleteSelectablePoint( (void *) mmsi_long, SELTYPE_AISTARGET );
            //  And add the updated target
            SelectItem *pSel = pSelectAIS->AddSelectablePoint( pTargetData->Lat,
                                                               pTargetData->Lon, (void *) mmsi_long, SELTYPE_AISTARGET );
            pSel->SetUserData( pTargetData->MMSI );
                    
           //    Calculate CPA info for this target immediately
            UpdateOneCPA( pTargetData );
                    
           //    Update this target's track
            if( pTargetData->b_show_track )
                UpdateOneTrack( pTargetData );
                    
        }
        
    }
        
    return pTargetData;
}


//----------------------------------------------------------------------------
//      Parse a NMEA VDM/VDO Bitstring
//----------------------------------------------------------------------------
bool AIS_Decoder::Parse_VDXBitstring( AIS_Bitstring *bstr, AIS_Target_Data *ptd )
{
    bool parse_result = false;
    bool b_posn_report = false;

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT( true );                    // no DST
    if( now.IsDST() ) now.Subtract( wxTimeSpan( 1, 0, 0, 0 ) );
    int message_ID = bstr->GetInt( 1, 6 );        // Parse on message ID
    ptd->MID = message_ID;
    ptd->MMSI = bstr->GetInt( 9, 30 );           // MMSI is always in the same spot in the bitstream

    switch( message_ID ){
        case 1:                                 // Position Report
        case 2:
        case 3: {
            n_msg1++;

            ptd->NavStatus = bstr->GetInt( 39, 4 );
            ptd->SOG = 0.1 * ( bstr->GetInt( 51, 10 ) );

            int lon = bstr->GetInt( 62, 28 );
            if( lon & 0x08000000 )                    // negative?
            lon |= 0xf0000000;
            double lon_tentative = lon / 600000.;

            int lat = bstr->GetInt( 90, 27 );
            if( lat & 0x04000000 )                    // negative?
            lat |= 0xf8000000;
            double lat_tentative = lat / 600000.;

            if( ( lon_tentative <= 180. ) && ( lat_tentative <= 90. ) ) // Ship does not report Lat or Lon "unavailable"
                    {
                ptd->Lon = lon_tentative;
                ptd->Lat = lat_tentative;
                ptd->b_positionDoubtful = false;
                ptd->b_positionOnceValid = true;          // Got the position at least once
                ptd->PositionReportTicks = now.GetTicks();
            } else
                ptd->b_positionDoubtful = true;

            //    decode balance of message....
            ptd->COG = 0.1 * ( bstr->GetInt( 117, 12 ) );
            ptd->HDG = 1.0 * ( bstr->GetInt( 129, 9 ) );

            ptd->ROTAIS = bstr->GetInt( 43, 8 );
            double rot_dir = 1.0;

            if( ptd->ROTAIS == 128 ) ptd->ROTAIS = -128;              // not available codes as -128
            else if( ( ptd->ROTAIS & 0x80 ) == 0x80 ) {
                ptd->ROTAIS = ptd->ROTAIS - 256;       // convert to twos complement
                rot_dir = -1.0;
            }

            ptd->ROTIND = wxRound( rot_dir * pow( ( ( (double) ptd->ROTAIS ) / 4.733 ), 2 ) ); // Convert to indicated ROT

            ptd->m_utc_sec = bstr->GetInt( 138, 6 );

            if( ( 1 == message_ID ) || ( 2 == message_ID ) )      // decode SOTDMA per 7.6.7.2.2
                    {
                ptd->SyncState = bstr->GetInt( 151, 2 );
                ptd->SlotTO = bstr->GetInt( 153, 2 );
                if( ( ptd->SlotTO == 1 ) && ( ptd->SyncState == 0 ) ) // UTCDirect follows
                        {
                    ptd->m_utc_hour = bstr->GetInt( 155, 5 );

                    ptd->m_utc_min = bstr->GetInt( 160, 7 );

                    if( ( ptd->m_utc_hour < 24 ) && ( ptd->m_utc_min < 60 )
                            && ( ptd->m_utc_sec < 60 ) ) {
                        wxDateTime rx_time( ptd->m_utc_hour, ptd->m_utc_min, ptd->m_utc_sec );
                        rx_ticks = rx_time.GetTicks();
                        if( !b_firstrx ) {
                            first_rx_ticks = rx_ticks;
                            b_firstrx = true;
                        }
                    }
                }
            }

            //    Capture Euro Inland special passing arrangement signal ("stbd-stbd")
            ptd->blue_paddle = bstr->GetInt( 144, 2 );
            ptd->b_blue_paddle = ( ptd->blue_paddle == 2 );             // paddle is set

            ptd->Class = AIS_CLASS_A;

            //    Check for SART and friends by looking at first two digits of MMSI
            int mmsi_start = ptd->MMSI / 10000000;

            if( mmsi_start == 97 ) {
                ptd->Class = AIS_SART;
                ptd->StaticReportTicks = now.GetTicks(); // won't get a static report, so fake it here

                //    On receipt of Msg 3, force any existing SART target out of acknowledge mode
                //    by adjusting its ack_time to yesterday
                //    This will cause any previously "Acknowledged" SART to re-alert.
                ptd->m_ack_time = wxDateTime::Now() - wxTimeSpan::Day();
            }

            parse_result = true;                // so far so good
            b_posn_report = true;

            break;
        }

        case 18: {
            ptd->NavStatus = UNDEFINED;         // Class B targets have no status.  Enforce this...

            ptd->SOG = 0.1 * ( bstr->GetInt( 47, 10 ) );

            int lon = bstr->GetInt( 58, 28 );
            if( lon & 0x08000000 )                    // negative?
            lon |= 0xf0000000;
            double lon_tentative = lon / 600000.;

            int lat = bstr->GetInt( 86, 27 );
            if( lat & 0x04000000 )                    // negative?
            lat |= 0xf8000000;
            double lat_tentative = lat / 600000.;

            if( ( lon_tentative <= 180. ) && ( lat_tentative <= 90. ) ) // Ship does not report Lat or Lon "unavailable"
                    {
                ptd->Lon = lon_tentative;
                ptd->Lat = lat_tentative;
                ptd->b_positionDoubtful = false;
                ptd->b_positionOnceValid = true;          // Got the position at least once
                ptd->PositionReportTicks = now.GetTicks();
            } else
                ptd->b_positionDoubtful = true;

            ptd->COG = 0.1 * ( bstr->GetInt( 113, 12 ) );
            ptd->HDG = 1.0 * ( bstr->GetInt( 125, 9 ) );

            ptd->m_utc_sec = bstr->GetInt( 134, 6 );

            ptd->Class = AIS_CLASS_B;

            parse_result = true;                // so far so good
            b_posn_report = true;

            break;
        }

        case 5: {
            n_msg5++;
            ptd->Class = AIS_CLASS_A;

//          Get the AIS Version indicator
//          0 = station compliant with Recommendation ITU-R M.1371-1
//          1 = station compliant with Recommendation ITU-R M.1371-3
//          2-3 = station compliant with future editions
            int AIS_version_indicator = bstr->GetInt( 39, 2 );
            if( AIS_version_indicator < 2 ) {
                ptd->IMO = bstr->GetInt( 41, 30 );

                bstr->GetStr( 71, 42, &ptd->CallSign[0], 7 );
                bstr->GetStr( 113, 120, &ptd->ShipName[0], 20 );
                ptd->b_nameValid = true;

                ptd->ShipType = (unsigned char) bstr->GetInt( 233, 8 );

                ptd->DimA = bstr->GetInt( 241, 9 );
                ptd->DimB = bstr->GetInt( 250, 9 );
                ptd->DimC = bstr->GetInt( 259, 6 );
                ptd->DimD = bstr->GetInt( 265, 6 );

                ptd->ETA_Mo = bstr->GetInt( 275, 4 );
                ptd->ETA_Day = bstr->GetInt( 279, 5 );
                ptd->ETA_Hr = bstr->GetInt( 284, 5 );
                ptd->ETA_Min = bstr->GetInt( 289, 6 );

                ptd->Draft = (double) ( bstr->GetInt( 295, 8 ) ) / 10.0;

                bstr->GetStr( 303, 120, &ptd->Destination[0], 20 );

                ptd->StaticReportTicks = now.GetTicks();

                parse_result = true;
            }

            break;
        }

        case 24: {
            int part_number = bstr->GetInt( 39, 2 );
            if( 0 == part_number ) {
                bstr->GetStr( 41, 120, &ptd->ShipName[0], 20 );
                ptd->b_nameValid = true;
                parse_result = true;
                n_msg24++;
            } else if( 1 == part_number ) {
                ptd->ShipType = (unsigned char) bstr->GetInt( 41, 8 );
                bstr->GetStr( 91, 42, &ptd->CallSign[0], 7 );

                ptd->DimA = bstr->GetInt( 133, 9 );
                ptd->DimB = bstr->GetInt( 142, 9 );
                ptd->DimC = bstr->GetInt( 151, 6 );
                ptd->DimD = bstr->GetInt( 157, 6 );
                parse_result = true;
            }
            break;
        }
        case 4:                                    // base station
        {
            ptd->Class = AIS_BASE;

            ptd->m_utc_hour = bstr->GetInt( 62, 5 );
            ptd->m_utc_min = bstr->GetInt( 67, 6 );
            ptd->m_utc_sec = bstr->GetInt( 73, 6 );
            //                              (79,  1);
            int lon = bstr->GetInt( 80, 28 );
            if( lon & 0x08000000 )                    // negative?
            lon |= 0xf0000000;
            double lon_tentative = lon / 600000.;

            int lat = bstr->GetInt( 108, 27 );
            if( lat & 0x04000000 )                    // negative?
            lat |= 0xf8000000;
            double lat_tentative = lat / 600000.;

            if( ( lon_tentative <= 180. ) && ( lat_tentative <= 90. ) ) // Ship does not report Lat or Lon "unavailable"
                    {
                ptd->Lon = lon_tentative;
                ptd->Lat = lat_tentative;
                ptd->b_positionDoubtful = false;
                ptd->b_positionOnceValid = true;          // Got the position at least once
                ptd->PositionReportTicks = now.GetTicks();
            } else
                ptd->b_positionDoubtful = true;

            ptd->COG = -1.;
            ptd->HDG = 511;
            ptd->SOG = -1.;

            parse_result = true;
            b_posn_report = true;

            break;
        }
        case 9:                   // Special Position Report (Standard SAR Aircraft Position Report)
        {
            ptd->SOG =  bstr->GetInt( 51, 10 ) ;
            
            int lon = bstr->GetInt( 62, 28 );
            if( lon & 0x08000000 )                    // negative?
            lon |= 0xf0000000;
            double lon_tentative = lon / 600000.;
            
            int lat = bstr->GetInt( 90, 27 );
            if( lat & 0x04000000 )                    // negative?
            lat |= 0xf8000000;
            double lat_tentative = lat / 600000.;
            
            if( ( lon_tentative <= 180. ) && ( lat_tentative <= 90. ) ) // Ship does not report Lat or Lon "unavailable"
                    {
                        ptd->Lon = lon_tentative;
                        ptd->Lat = lat_tentative;
                        ptd->b_positionDoubtful = false;
                        ptd->b_positionOnceValid = true;          // Got the position at least once
                        ptd->PositionReportTicks = now.GetTicks();
                    } else
                        ptd->b_positionDoubtful = true;
                    
            //    decode balance of message....
            ptd->COG = 0.1 * ( bstr->GetInt( 117, 12 ) );

            int alt_tent = bstr->GetInt( 39, 12 );
            ptd->altitude = alt_tent;
            
            ptd->b_SarAircraftPosnReport = true;
            
            parse_result = true;
            b_posn_report = true;
            
            break;
        }
        case 21:                                    // Test Message (Aid to Navigation)
        {
            ptd->ShipType = (unsigned char) bstr->GetInt( 39, 5 );
            ptd->IMO = 0;
            ptd->SOG = 0;
            ptd->HDG = 0;
            ptd->COG = 0;
            ptd->ROTAIS = -128;                 // i.e. not available
            ptd->DimA = bstr->GetInt( 220, 9 );
            ptd->DimB = bstr->GetInt( 229, 9 );
            ptd->DimC = bstr->GetInt( 238, 6 );
            ptd->DimD = bstr->GetInt( 244, 6 );
            ptd->Draft = 0;

            ptd->m_utc_sec = bstr->GetInt( 254, 6 );

            int offpos = bstr->GetInt( 260, 1 ); // off position flag
            int virt = bstr->GetInt( 270, 1 ); // virtual flag

            if( virt ) ptd->NavStatus = ATON_VIRTUAL;
            else
                ptd->NavStatus = ATON_REAL;
            if( ptd->m_utc_sec <= 59 /*&& !virt*/) {
                ptd->NavStatus += 1;
                if( offpos ) ptd->NavStatus += 1;
            }

            bstr->GetStr( 44, 120, &ptd->ShipName[0], 20 ); // short name only, extension wont fit in Ship structure

            if( bstr->GetBitCount() > 276 ) {
                int nx = ( ( bstr->GetBitCount() - 272 ) / 6 ) * 6;
                bstr->GetStr( 273, nx, &ptd->ShipNameExtension[0], 14 );
                ptd->ShipNameExtension[14] = 0;
            } else {
                ptd->ShipNameExtension[0] = 0;
            }

            ptd->b_nameValid = true;

            parse_result = true;                // so far so good

            ptd->Class = AIS_ATON;

            int lon = bstr->GetInt( 165, 28 );

            if( lon & 0x08000000 )                    // negative?
            lon |= 0xf0000000;
            double lon_tentative = lon / 600000.;

            int lat = bstr->GetInt( 193, 27 );

            if( lat & 0x04000000 )                    // negative?
            lat |= 0xf8000000;
            double lat_tentative = lat / 600000.;

            if( ( lon_tentative <= 180. ) && ( lat_tentative <= 90. ) ) // Ship does not report Lat or Lon "unavailable"
                    {
                ptd->Lon = lon_tentative;
                ptd->Lat = lat_tentative;
                ptd->b_positionDoubtful = false;
                ptd->b_positionOnceValid = true;          // Got the position at least once
                ptd->PositionReportTicks = now.GetTicks();
            } else
                ptd->b_positionDoubtful = true;

            b_posn_report = true;
            break;
        }
        case 8:                                    // Binary Broadcast
        {
            int dac = bstr->GetInt( 41, 10 );
            int fi = bstr->GetInt( 51, 6 );
            if( dac == 200 )                  // European inland
                    {
                if( fi == 10 )              // "Inland ship static and voyage related data"
                        {
                    ptd->b_isEuroInland = true;

                    bstr->GetStr( 57, 48, &ptd->Euro_VIN[0], 8 );
                    ptd->Euro_Length = ( (double) bstr->GetInt( 105, 13 ) ) / 10.0;
                    ptd->Euro_Beam = ( (double) bstr->GetInt( 118, 10 ) ) / 10.0;
                    ptd->UN_shiptype = bstr->GetInt( 128, 14 );
                    ptd->Euro_Draft = ( (double) bstr->GetInt( 145, 11 ) ) / 100.0;
                    parse_result = true;
                }
            }
            if( dac == 1 )                     // IMO
                    {
                if( fi == 22 )                 // Area Notice
                        {
                    if( bstr->GetBitCount() >= 111 ) {
                        Ais8_001_22 an;
                        an.link_id = bstr->GetInt( 57, 10 );
                        an.notice_type = bstr->GetInt( 67, 7 );
                        an.month = bstr->GetInt( 74, 4 );
                        an.day = bstr->GetInt( 78, 5 );
                        an.hour = bstr->GetInt( 83, 5 );
                        an.minute = bstr->GetInt( 88, 6 );
                        an.duration_minutes = bstr->GetInt( 94, 18 );

                        wxDateTime now = wxDateTime::Now();
                        now.MakeGMT();

                        an.start_time.Set( an.day, wxDateTime::Month( an.month - 1 ), now.GetYear(),
                                an.hour, an.minute );

                        // msg is not supposed to be transmitted more than a day before it comes into effect,
                        // so a start_time less than a day or two away might indicate a month rollover
                        if( an.start_time > now + wxTimeSpan::Hours( 48 ) ) an.start_time.Set(
                                an.day, wxDateTime::Month( an.month - 1 ), now.GetYear() - 1,
                                an.hour, an.minute );

                        an.expiry_time = an.start_time + wxTimeSpan::Minutes( an.duration_minutes );

                        // msg is not supposed to be transmitted beyond expiration, so taking into account a
                        // fudge factor for clock issues, assume an expiry date in the past indicates incorrect year
                        if( an.expiry_time < now - wxTimeSpan::Hours( 24 ) ) {
                            an.start_time.Set( an.day, wxDateTime::Month( an.month - 1 ),
                                    now.GetYear() + 1, an.hour, an.minute );
                            an.expiry_time = an.start_time
                                    + wxTimeSpan::Minutes( an.duration_minutes );
                        }

                        int subarea_count = ( bstr->GetBitCount() - 111 ) / 87;
                        for( int i = 0; i < subarea_count; ++i ) {
                            int base = 111 + i * 87;
                            Ais8_001_22_SubArea sa;
                            sa.shape = bstr->GetInt( base + 1, 3 );
                            int scale_factor = 1;
                            if( sa.shape == AIS8_001_22_SHAPE_TEXT ) {
                                char t[15];
                                t[14] = 0;
                                bstr->GetStr( base + 4, 84, t, 14 );
                                sa.text = wxString( t, wxConvUTF8 );
                            } else {
                                int scale_multipliers[4] = { 1, 10, 100, 1000 };
                                scale_factor = scale_multipliers[bstr->GetInt( base + 4, 2 )];
                                switch( sa.shape ){
                                    case AIS8_001_22_SHAPE_CIRCLE:
                                    case AIS8_001_22_SHAPE_SECTOR:
                                        sa.radius_m = bstr->GetInt( base + 58, 12 ) * scale_factor;
                                    case AIS8_001_22_SHAPE_RECT:
                                        sa.longitude = bstr->GetInt( base + 6, 25, true ) / 60000.0;
                                        sa.latitude = bstr->GetInt( base + 31, 24, true ) / 60000.0;
                                        break;
                                    case AIS8_001_22_SHAPE_POLYLINE:
                                    case AIS8_001_22_SHAPE_POLYGON:
                                        for( int i = 0; i < 4; ++i ) {
                                            sa.angles[i] = bstr->GetInt( base + 6 + i * 20, 10 )
                                                    * 0.5;
                                            sa.dists_m[i] = bstr->GetInt( base + 16 + i * 20, 10 )
                                                    * scale_factor;
                                        }
                                }
                                if( sa.shape == AIS8_001_22_SHAPE_RECT ) {
                                    sa.e_dim_m = bstr->GetInt( base + 58, 8 ) * scale_factor;
                                    sa.n_dim_m = bstr->GetInt( base + 66, 8 ) * scale_factor;
                                    sa.orient_deg = bstr->GetInt( base + 74, 9 );
                                }
                                if( sa.shape == AIS8_001_22_SHAPE_SECTOR ) {
                                    sa.left_bound_deg = bstr->GetInt( 70, 9 );
                                    sa.right_bound_deg = bstr->GetInt( 79, 9 );
                                }
                            }
                            an.sub_areas.push_back( sa );
                        }
                        ptd->area_notices[an.link_id] = an;
                        parse_result = true;
                    }
                }
            }
            break;
        }
        case 14:                                    // Safety Related Broadcast
        {
            //  Always capture the MSG_14 text
            char msg_14_text[968];
            if( bstr->GetBitCount() > 40 ) {
                int nx = ( ( bstr->GetBitCount() - 40 ) / 6 ) * 6;
                int nd = bstr->GetStr( 41, nx, msg_14_text, 968 );
                nd = wxMax(0, nd);
                nd = wxMin(nd, 967);
                msg_14_text[nd] = 0;
                ptd->MSG_14_text = wxString( msg_14_text, wxConvUTF8 );
            }
            parse_result = true;                // so far so good

            break;
        }

        case 6:                                    // Addressed Binary Message
        {
            break;
        }
        case 7:                                    // Binary Ack
        {
            break;
        }
        default: {
            break;
        }

    }

    if( b_posn_report ) ptd->b_lost = false;

    if( true == parse_result ) {
        //      Revalidate the target under some conditions
        if( !ptd->b_active && !ptd->b_positionDoubtful && b_posn_report ) ptd->b_active = true;
    }

    return parse_result;
}

bool AIS_Decoder::NMEACheckSumOK( const wxString& str_in )
{

    unsigned char checksum_value = 0;
    int sentence_hex_sum;

    wxCharBuffer buf = str_in.ToUTF8();
    if( !buf.data()) 
        return false;                           // cannot decode string
        
    char str_ascii[AIS_MAX_MESSAGE_LEN + 1];
    strncpy( str_ascii, buf.data(), AIS_MAX_MESSAGE_LEN );
    str_ascii[AIS_MAX_MESSAGE_LEN] = '\0';

    int string_length = strlen( str_ascii );

    int payload_length = 0;
    while( ( payload_length < string_length ) && ( str_ascii[payload_length] != '*' ) ) // look for '*'
        payload_length++;

    if( payload_length == string_length ) return false; // '*' not found at all, no checksum

    int index = 1; // Skip over the $ at the begining of the sentence

    while( index < payload_length ) {
        checksum_value ^= str_ascii[index];
        index++;
    }

    if( string_length > 4 ) {
        char scanstr[3];
        scanstr[0] = str_ascii[payload_length + 1];
        scanstr[1] = str_ascii[payload_length + 2];
        scanstr[2] = 0;
        sscanf( scanstr, "%2x", &sentence_hex_sum );

        if( sentence_hex_sum == checksum_value ) return true;
    }

    return false;
}

void AIS_Decoder::UpdateAllCPA( void )
{
    //    Iterate thru all the targets
    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
        AIS_Target_Data *td = it->second;

        if( NULL != td ) UpdateOneCPA( td );
    }
}

void AIS_Decoder::UpdateAllTracks( void )
{
    //    Iterate thru all the targets
    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
        AIS_Target_Data *td = it->second;

        if( NULL != td ) UpdateOneTrack( td );
    }
}

void AIS_Decoder::UpdateOneTrack( AIS_Target_Data *ptarget )
{
    if( !ptarget->b_positionOnceValid ) return;

    //    Add the newest point
    AISTargetTrackPoint *ptrackpoint = new AISTargetTrackPoint;
    ptrackpoint->m_lat = ptarget->Lat;
    ptrackpoint->m_lon = ptarget->Lon;
    ptrackpoint->m_time = wxDateTime::Now().GetTicks();

    ptarget->m_ptrack->Append( ptrackpoint );

    //    Walk the list, removing any track points that are older than the stipulated time

    time_t test_time = wxDateTime::Now().GetTicks() - (time_t) ( g_AISShowTracks_Mins * 60 );

    wxAISTargetTrackListNode *node = ptarget->m_ptrack->GetFirst();
    while( node ) {
        AISTargetTrackPoint *ptrack_point = node->GetData();

        if( ptrack_point->m_time < test_time ) {
            if( ptarget->m_ptrack->DeleteObject( ptrack_point ) ) {
                node = ptarget->m_ptrack->GetFirst();                // restart the list
            }
        } else
            node = node->GetNext();
    }
}

void AIS_Decoder::UpdateAllAlarms( void )
{
    m_bGeneralAlert = false;                // no alerts yet

    //    Iterate thru all the targets
    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
        AIS_Target_Data *td = it->second;

        if( NULL != td ) {
            //  Maintain General Alert
            if( !m_bGeneralAlert ) {
                //    Quick check on basic condition
                if( ( td->CPA < g_CPAWarn_NM ) && ( td->TCPA > 0 ) && ( td->Class != AIS_ATON ) )
                    m_bGeneralAlert = true;

                //    Some options can suppress general alerts
                if( g_bAIS_CPA_Alert_Suppress_Moored && ( td->SOG <= g_ShowMoored_Kts ) )
                    m_bGeneralAlert = false;

                //    Skip distant targets if requested
                if( ( g_bCPAMax ) && ( td->Range_NM > g_CPAMax_NM ) )
                    m_bGeneralAlert = false;

                //    Skip if TCPA is too long
                if( ( g_bTCPA_Max ) && ( td->TCPA > g_TCPA_Max ) )
                    m_bGeneralAlert = false;

                //  SART targets always alert if "Active"
                if( td->Class == AIS_SART && td->NavStatus == 14)
                    m_bGeneralAlert = true;

                //  DSC Distress targets always alert
                if( ( td->Class == AIS_DSC ) && ( td->ShipType == 12 ) )
                    m_bGeneralAlert = true;
            }

            ais_alert_type this_alarm = AIS_NO_ALERT;

            //  SART targets always alert if "Active"
            if( td->Class == AIS_SART && td->NavStatus == 14)
                this_alarm = AIS_ALERT_SET;
            
            //  DSC Distress targets always alert
            if( ( td->Class == AIS_DSC ) && ( td->ShipType == 12 ) )
                    this_alarm = AIS_ALERT_SET;
            
            if( g_bCPAWarn && td->b_active && td->b_positionOnceValid &&
                ( td->Class != AIS_SART ) && ( td->Class != AIS_DSC ) ) {
                //      Skip anchored/moored(interpreted as low speed) targets if requested
                if( ( !g_bShowMoored ) && ( td->SOG <= g_ShowMoored_Kts ) ) {       // dsr
                    td->n_alert_state = AIS_NO_ALERT;
                    continue;
                }

                //    No Alert on moored(interpreted as low speed) targets if so requested
                if( g_bAIS_CPA_Alert_Suppress_Moored && ( td->SOG <= g_ShowMoored_Kts ) ) {    // dsr
                    td->n_alert_state = AIS_NO_ALERT;
                    continue;
                }

                //    Skip distant targets if requested
                if( g_bCPAMax ) {
                    if( td->Range_NM > g_CPAMax_NM ) {
                        td->n_alert_state = AIS_NO_ALERT;
                        continue;
                    }
                }

                if( ( td->CPA < g_CPAWarn_NM ) && ( td->TCPA > 0 ) && ( td->Class != AIS_ATON ) ) {
                    if( g_bTCPA_Max ) {
                        if( td->TCPA < g_TCPA_Max ) this_alarm = AIS_ALERT_SET;
                    } else
                        this_alarm = AIS_ALERT_SET;
                }
            }

            
            //    Maintain the timer for in_ack flag
            //  SART and DSC targets always maintain ack timeout

            if( g_bAIS_ACK_Timeout || (td->Class == AIS_SART) || ((td->Class == AIS_DSC) && (td->ShipType == 12))) {
                if( td->b_in_ack_timeout ) {
                    wxTimeSpan delta = wxDateTime::Now() - td->m_ack_time;
                    if( delta.GetMinutes() > g_AckTimeout_Mins ) td->b_in_ack_timeout = false;
                }
            } else {
                //  Not using ack timeouts.
                //  If a target has been acknowledged, leave it ack'ed until it goes out of AIS_ALARM_SET state
                if( td->b_in_ack_timeout ){
                    if( this_alarm == AIS_NO_ALERT )
                        td->b_in_ack_timeout = false;
                }
            }

            td->n_alert_state = this_alarm;

        }
    }
}

void AIS_Decoder::UpdateOneCPA( AIS_Target_Data *ptarget )
{
    ptarget->Range_NM = -1.;            // Defaults
    ptarget->Brg = -1.;

    if( !ptarget->b_positionOnceValid || !bGPSValid ) {
        ptarget->bCPA_Valid = false;
        return;
    }

    //    Compute the current Range/Brg to the target
    double brg, dist;
    DistanceBearingMercator( ptarget->Lat, ptarget->Lon, gLat, gLon, &brg, &dist );
    ptarget->Range_NM = dist;
    ptarget->Brg = brg;

    if( dist <= 1e-5 ) ptarget->Brg = -1.0;             // Brg is undefined if Range == 0.

    //    There can be no collision between ownship and itself....
    //    This can happen if AIVDO messages are received, and there is another source of ownship position, like NMEA GLL
    //    The two positions are always temporally out of sync, and one will always be exactly in front of the other one.
    if( ptarget->b_OwnShip ) {
        ptarget->CPA = 100;
        ptarget->TCPA = -100;
        ptarget->bCPA_Valid = false;
        return;
    }

    double cpa_calc_ownship_cog = gCog;
    double cpa_calc_target_cog = ptarget->COG;

//    Ownship is not reporting valid SOG, so no way to calculate CPA
    if( wxIsNaN(gSog) || ( gSog > 102.2 ) ) {
        ptarget->bCPA_Valid = false;
        return;
    }

//    Ownship is maybe anchored and not reporting COG
    if( wxIsNaN(gCog) || gCog == 360.0 ) {
        if( gSog < .01 ) cpa_calc_ownship_cog = 0.;          // substitute value
                                                             // for the case where SOG ~= 0, and COG is unknown.
        else {
            ptarget->bCPA_Valid = false;
            return;
        }
    }

//    Target is maybe anchored and not reporting COG
    if( ptarget->COG == 360.0 ) {
        if( ptarget->SOG > 102.2 ) {
            ptarget->bCPA_Valid = false;
            return;
        } else if( ptarget->SOG < .01 ) cpa_calc_target_cog = 0.;           // substitute value
                                                                            // for the case where SOG ~= 0, and COG is unknown.
        else {
            ptarget->bCPA_Valid = false;
            return;
        }
    }

    //    Express the SOGs as meters per hour
    double v0 = gSog * 1852.;
    double v1 = ptarget->SOG * 1852.;

    if( ( v0 < 1e-6 ) && ( v1 < 1e-6 ) ) {
        ptarget->TCPA = 0.;
        ptarget->CPA = 0.;

        ptarget->bCPA_Valid = false;
    } else {
        //    Calculate the TCPA first

        //    Working on a Reduced Lat/Lon orthogonal plotting sheet....
        //    Get easting/northing to target,  in meters

        double east1 = ( ptarget->Lon - gLon ) * 60 * 1852;
        double north1 = ( ptarget->Lat - gLat ) * 60 * 1852;

        double east = east1 * ( cos( gLat * PI / 180. ) );
        
        double north = north1;

        //    Convert COGs trigonometry to standard unit circle
        double cosa = cos( ( 90. - cpa_calc_ownship_cog ) * PI / 180. );
        double sina = sin( ( 90. - cpa_calc_ownship_cog ) * PI / 180. );
        double cosb = cos( ( 90. - cpa_calc_target_cog ) * PI / 180. );
        double sinb = sin( ( 90. - cpa_calc_target_cog ) * PI / 180. );

        //    These will be useful
        double fc = ( v0 * cosa ) - ( v1 * cosb );
        double fs = ( v0 * sina ) - ( v1 * sinb );

        double d = ( fc * fc ) + ( fs * fs );
        double tcpa;

        // the tracks are almost parallel
        if( fabs( d ) < 1e-6 ) tcpa = 0.;
        else
            //    Here is the equation for t, which will be in hours
            tcpa = ( ( fc * east ) + ( fs * north ) ) / d;

        //    Convert to minutes
        ptarget->TCPA = tcpa * 60.;

        //    Calculate CPA
        //    Using TCPA, predict ownship and target positions

        double OwnshipLatCPA, OwnshipLonCPA, TargetLatCPA, TargetLonCPA;

        ll_gc_ll( gLat, gLon, cpa_calc_ownship_cog, gSog * tcpa, &OwnshipLatCPA, &OwnshipLonCPA );
        ll_gc_ll( ptarget->Lat, ptarget->Lon, cpa_calc_target_cog, ptarget->SOG * tcpa,
                &TargetLatCPA, &TargetLonCPA );

        //   And compute the distance
        ptarget->CPA = DistGreatCircle( OwnshipLatCPA, OwnshipLonCPA, TargetLatCPA, TargetLonCPA );

        ptarget->bCPA_Valid = true;

        if( ptarget->TCPA < 0 ) ptarget->bCPA_Valid = false;
    }
}

void AIS_Decoder::OnTimerAISAudio( wxTimerEvent& event )
{
    if( g_bAIS_CPA_Alert_Audio && m_bAIS_Audio_Alert_On ) {
        if(!m_AIS_Sound.IsOk() )
             m_AIS_Sound.Create( g_sAIS_Alert_Sound_File );
             
#ifndef __WXMSW__
       if( m_AIS_Sound.IsOk() && !m_AIS_Sound.IsPlaying())
            m_AIS_Sound.Play();
#else
       if( m_AIS_Sound.IsOk() )
                m_AIS_Sound.Play();
#endif
    }
    
    m_AIS_Audio_Alert_Timer.Start( TIMER_AIS_AUDIO_MSEC, wxTIMER_CONTINUOUS );
}

void AIS_Decoder::OnTimerDSC( wxTimerEvent& event )
{
    //  Timer expired, no CDDSE message was received, so accept the latest CDDSC message
    if(m_ptentative_dsctarget){
        ProcessDSx( m_dsc_last_string, true );
    }
}


void AIS_Decoder::OnTimerAIS( wxTimerEvent& event )
{
    TimerAIS.Stop();

    //    Scrub the target hash list
    //    removing any targets older than stipulated age

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT();

    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    it = ( *current_targets ).begin();
    while( it != ( *current_targets ).end() ) {
        bool b_new_it = false;

        AIS_Target_Data *td = it->second;

        if( NULL == td )                        // This should never happen, but I saw it once....
                {
            current_targets->erase( it );
            break;                          // leave the loop
        }

        int target_posn_age = now.GetTicks() - td->PositionReportTicks;
        int target_static_age = now.GetTicks() - td->StaticReportTicks;

        //      Mark lost targets if specified
        if( g_bMarkLost ) {
            if( ( target_posn_age > g_MarkLost_Mins * 60 ) && ( td->Class != AIS_GPSG_BUDDY ) ) td->b_active =
                    false;
        }

        //      Remove lost targets if specified
        double removelost_Mins = fmax(g_RemoveLost_Mins,g_MarkLost_Mins);

        if( td->Class == AIS_SART ) removelost_Mins = 18.0;

        if( g_bRemoveLost ) {
            if( ( target_posn_age > removelost_Mins * 60 ) && ( td->Class != AIS_GPSG_BUDDY ) ) {
                //      So mark the target as lost, with unknown position, and make it not selectable
                td->b_lost = true;
                td->b_positionOnceValid = false;
                td->COG = 360.0;
                td->SOG = 103.0;
                td->HDG = 511.0;
                td->ROTAIS = -128;

                long mmsi_long = td->MMSI;
                pSelectAIS->DeleteSelectablePoint( (void *) mmsi_long, SELTYPE_AISTARGET );

                //      If we have not seen a static report in 3 times the removal spec,
                //      then remove the target from all lists.
                if( target_static_age > removelost_Mins * 60 * 3 ) {
                    current_targets->erase( it );
                    delete td;

                    //      Reset the iterator on item erase.
                    it = ( *current_targets ).begin();
                    b_new_it = true;
                }
            }
        }

        if( !b_new_it ) ++it;
    }

    UpdateAllCPA();
    UpdateAllAlarms();

    //    Update the general suppression flag
    m_bSuppressed = false;
    if( g_bAIS_CPA_Alert_Suppress_Moored || !g_bShowMoored ) m_bSuppressed = true;

    m_bAIS_Audio_Alert_On = false;            // default, may be set on

    //    Process any Alarms

    //    If the AIS Alert Dialog is not currently shown....

    //    Scan all targets, looking for SART, DSC Distress, and CPA incursions
    //    In the case of multiple targets of the same type, select the shortest range or shortest TCPA
    
    if( NULL == g_pais_alert_dialog_active ) {
        double tcpa_min = 1e6;             // really long
        double sart_range = 1e6;
        double dsc_range = 1e6;
        AIS_Target_Data *palert_target_cpa = NULL;
        AIS_Target_Data *palert_target_sart = NULL;
        AIS_Target_Data *palert_target_dsc = NULL;
        
        for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
            AIS_Target_Data *td = it->second;
            if( td ) {
                if( (td->Class != AIS_SART) &&  (td->Class != AIS_DSC) ) {

                    if( g_bAIS_CPA_Alert && td->b_active ) {
                        if( ( AIS_ALERT_SET == td->n_alert_state ) && !td->b_in_ack_timeout ) {
                            if( td->TCPA < tcpa_min ) {
                                tcpa_min = td->TCPA;
                                palert_target_cpa = td;
                            }
                        }
                    }
                }
                else if( (td->Class == AIS_DSC ) && ( td->ShipType == 12) ){
                    if( td->b_active ) {
                        if( ( AIS_ALERT_SET == td->n_alert_state ) && !td->b_in_ack_timeout ) {
                            palert_target_dsc = td;
                        }
                    }
                }
                                
                else if( td->Class == AIS_SART ){
                    if( td->b_active ) {
                        if( ( AIS_ALERT_SET == td->n_alert_state ) && !td->b_in_ack_timeout ) {
                            if( td->Range_NM < sart_range ) {
                                tcpa_min = sart_range;
                                palert_target_sart = td;
                            }
                        }
                    }
                }
            }
        }

        //    Which of multiple targets?
        //    Give priority to SART targets, then DSC Distress, then CPA incursion
        
        AIS_Target_Data *palert_target = palert_target_cpa;

        if( palert_target_sart )
            palert_target = palert_target_sart;
        
        if( palert_target_dsc )
            palert_target = palert_target_dsc;
        

        //    Show the alert
        if( palert_target ) {

            bool b_jumpto = (palert_target->Class == AIS_SART) || (palert_target->Class == AIS_DSC);
            bool b_createWP = palert_target->Class == AIS_DSC;
            bool b_ack = palert_target->Class != AIS_DSC;
            
        //    Show the Alert dialog
            
//      See FS# 968/998
//      If alert occurs while OCPN is iconized to taskbar, then clicking the taskbar icon
//      only brings up the Alert dialog, and not the entire application.
//      This is an OS specific behavior, not seen on linux or Mac.
//      This patch will allow the audio alert to occur, and the visual alert will pop up soon
//      after the user selects the OCPN icon from the taskbar. (on the next timer tick, probably)
            
//#ifdef __WXMSW__            
         if( gFrame->IsIconized() || !gFrame->IsActive() )
                gFrame->RequestUserAttention();
//#endif
            
//#ifdef __WXMSW__            
            if( !gFrame->IsIconized() )
//#endif                
            {
                AISTargetAlertDialog *pAISAlertDialog = new AISTargetAlertDialog();
                pAISAlertDialog->Create( palert_target->MMSI, m_parent_frame, this,
                                         b_jumpto, b_createWP, b_ack,
                                         -1, _("AIS Alert"), wxPoint( g_ais_alert_dialog_x, g_ais_alert_dialog_y ),
                    wxSize( g_ais_alert_dialog_sx, g_ais_alert_dialog_sy ) );

                g_pais_alert_dialog_active = pAISAlertDialog;
                pAISAlertDialog->Show();                     // Show modeless, so it stays on the screen
            }

            //    Audio alert if requested
            m_bAIS_Audio_Alert_On = true;             // always on when alert is first shown
        }
    }

    //    The AIS Alert dialog is already shown.  If the  dialog MMSI number is still alerted, update the dialog
    //    otherwise, destroy the dialog
    else {
        AIS_Target_Data *palert_target = Get_Target_Data_From_MMSI(
                g_pais_alert_dialog_active->Get_Dialog_MMSI() );

        if( palert_target ) {
            if( ( ( AIS_ALERT_SET == palert_target->n_alert_state )
                    && !palert_target->b_in_ack_timeout )
                    || ( palert_target->Class == AIS_SART ) ) {
                g_pais_alert_dialog_active->UpdateText();
            } else {
                g_pais_alert_dialog_active->Close();
                m_bAIS_Audio_Alert_On = false;
            }

            if( true == palert_target->b_suppress_audio ) m_bAIS_Audio_Alert_On = false;
            else
                m_bAIS_Audio_Alert_On = true;
        } else {                                               // this should not happen, however...
            g_pais_alert_dialog_active->Close();
            m_bAIS_Audio_Alert_On = false;
        }

    }

    //    At this point, the audio flag is set
    //    Honor the global flag
    if( !g_bAIS_CPA_Alert_Audio )
        m_bAIS_Audio_Alert_On = false;

    if( m_bAIS_Audio_Alert_On ) {
        if( !m_AIS_Audio_Alert_Timer.IsRunning() ) {
            m_AIS_Audio_Alert_Timer.SetOwner( this, TIMER_AISAUDIO );
            m_AIS_Audio_Alert_Timer.Start( TIMER_AIS_AUDIO_MSEC );

            if( !m_AIS_Sound.IsOk() )
                m_AIS_Sound.Create( g_sAIS_Alert_Sound_File );
            
#ifndef __WXMSW__
            if( m_AIS_Sound.IsOk() && !m_AIS_Sound.IsPlaying())
                m_AIS_Sound.Play();
#else
            if( m_AIS_Sound.IsOk() )
                m_AIS_Sound.Play();
#endif
        }
    } else
        m_AIS_Audio_Alert_Timer.Stop();

    TimerAIS.Start( TIMER_AIS_MSEC, wxTIMER_CONTINUOUS );
}

AIS_Target_Data *AIS_Decoder::Get_Target_Data_From_MMSI( int mmsi )
{
    if( AISTargetList->find( mmsi ) == AISTargetList->end() )     // if entry does not exist....
    return NULL;
    else
        return ( *AISTargetList )[mmsi];          // find current entry
}

