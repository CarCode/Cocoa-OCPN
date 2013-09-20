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

#include "SARgui_impl.h"

CfgDlg::CfgDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : CfgDlgDef( parent, id, title, pos, size, style )
{
}

Dlg::Dlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DlgDef( parent, id, title, pos, size, style )
{
    this->Fit();
    initialize_bitmaps();
    dbg=false; //for debug output set to true

    //initialise images
    m_bitmap_oil_rig->SetBitmap(* _img_oil_rig);
    m_bitmap_trackln->SetBitmap(* _img_trackln);
    m_bitmap_sector->SetBitmap(* _img_sector);
    m_bitmap_exp_sq->SetBitmap(* _img_exp_sq);
}

void Dlg::OnConverttoDegree( wxCommandEvent& event )
{

   //set cell values to 0 if they are empty. This ensures conversion goes ok.
    double test_value;
    if(!this->m_Lat1_d->GetValue().ToDouble(&test_value)){m_Lat1_d->SetValue(wxString::Format(wxT("%i"),0 ));}
    if(!this->m_Lat1_m->GetValue().ToDouble(&test_value)){m_Lat1_m->SetValue(wxString::Format(wxT("%i"),0 ));}
    if(!this->m_Lat1_s->GetValue().ToDouble(&test_value)){m_Lat1_s->SetValue(wxString::Format(wxT("%i"),0 ));}

    if(!this->m_Lon1_d->GetValue().ToDouble(&test_value)){m_Lon1_d->SetValue(wxString::Format(wxT("%i"),0 ));}
    if(!this->m_Lon1_m->GetValue().ToDouble(&test_value)){m_Lon1_m->SetValue(wxString::Format(wxT("%i"),0 ));}
    if(!this->m_Lon1_s->GetValue().ToDouble(&test_value)){m_Lon1_s->SetValue(wxString::Format(wxT("%i"),0 ));}

    //Lat1
    wxString Lat1 = this->m_Lat1_d->GetValue() + _T(" ")  + this->m_Lat1_m->GetValue() + _T(" ")  + this->m_Lat1_s->GetValue() + _T(" ");// + this->m_Lon1_EW->GetCurrentSelection();
    if(this->m_Lat1_NS->GetCurrentSelection()) //S=1
        Lat1=Lat1 + _T("S");
    else
        Lat1=Lat1 + _T("N");
    m_Lat1->SetValue(wxString::Format(wxT("%g"), fromDMStodouble((char*)Lat1.mb_str().data())));

    //Lon1
    wxString Lon1 = this->m_Lon1_d->GetValue() + _T(" ") + this->m_Lon1_m->GetValue() + _T(" ")  + this->m_Lon1_s->GetValue() + _T(" ") ;// + this->m_Lon1_EW->GetString();
    if(this->m_Lon1_EW->GetCurrentSelection()) //W=1
        Lon1=Lon1 + _T("W");
    else
        Lon1=Lon1 + _T("E");
    m_Lon1->SetValue(wxString::Format(wxT("%g"), fromDMStodouble((char*)Lon1.mb_str().data())));

}

void Dlg::OnNoteBookFit( wxCommandEvent& event )
{

    this->m_panel11->Layout();
    this->m_wxNotebook234->InvalidateBestSize();
    this->Fit();
    if (dbg) printf("Resizing window \n");
}
void Dlg::OnFit( wxCommandEvent& event )
{
    this->m_wxNotebook234->InvalidateBestSize();
    this->m_panel11->Fit();
    this->m_panel11->Layout();
    this->Fit();
    this->Layout();
    if (dbg) printf("Resizing window \n");
}

void Dlg::Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype){
//add point
	TiXmlElement * RoutePoint = new TiXmlElement( "rtept" );
    RoutePoint->SetAttribute("lat", ptlat.mb_str());
    RoutePoint->SetAttribute("lon", ptlon.mb_str());


    TiXmlElement * Name = new TiXmlElement( "name" );
    TiXmlText * text = new TiXmlText( ptname.mb_str() );
    RoutePoint->LinkEndChild( Name );
    Name->LinkEndChild( text );

    TiXmlElement * Symbol = new TiXmlElement( "sym" );
    TiXmlText * text1 = new TiXmlText( ptsym.mb_str() );
    RoutePoint->LinkEndChild( Symbol );
    Symbol->LinkEndChild( text1 );

    TiXmlElement * Type = new TiXmlElement( "type" );
    TiXmlText * text2 = new TiXmlText( pttype.mb_str() );
    RoutePoint->LinkEndChild( Type );
    Type->LinkEndChild( text2 );
    Route->LinkEndChild( RoutePoint );
//done adding point
}

void Dlg::OnPSCalc( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, false, 1);
}

void Dlg::OnPSGPX( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, true, 1);
}
void Dlg::OnESCalc( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, false, 2);
}
void Dlg::OnESGPX( wxCommandEvent& event )
{
    //wxMessageBox(_("Function not yet implemented :p")) ;
    Calculate(event, true, 2);
}
void Dlg::OnSSCalc( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, false, 3);
}
void Dlg::OnSSGPX( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, true, 3);
}

void Dlg::OnORCalc( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, false, 4);
}
void Dlg::OnORGPX( wxCommandEvent& event )
{
   // wxMessageBox(_("Function not yet implemented :p")) ;
   Calculate(event, true, 4);
}

void Dlg::Calculate( wxCommandEvent& event, bool write_file, int Pattern  )
/*
1 Parrallel Search
2 Expanding Square
3 Sector search
4 Oil Rig
*/
{
    bool error_occured=false;
   // double dist, fwdAz, revAz;


    double lat1,lon1;
    if(!this->m_Lat1->GetValue().ToDouble(&lat1)){ lat1=0.0;}
    if(!this->m_Lon1->GetValue().ToDouble(&lon1)){ lon1=0.0;}
    //if (error_occured) wxMessageBox(_T("error in conversion of input coordinates"));

    //error_occured=false;
    wxString s;
    if (write_file){
        wxFileDialog dlg(this, _("Export SAR track GPX file as"), wxEmptyString, wxEmptyString, _T("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() == wxID_CANCEL)
            error_occured=true;     // the user changed idea...
        //dlg.ShowModal();
        s=dlg.GetPath();
        //  std::cout<<s<< std::endl;
        if (dlg.GetPath() == wxEmptyString){ error_occured=true; if (dbg) printf("Empty Path\n");}
    }

    //Validate input ranges
    if (!error_occured){
        if (std::abs(lat1)>90){ error_occured=true; }
        if (std::abs(lon1)>180){ error_occured=true; }
        if (error_occured) wxMessageBox(_("error in input range validation"));
    }

    //Start GPX
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
    doc.LinkEndChild( decl );
    TiXmlElement * root = new TiXmlElement( "gpx" );
    TiXmlElement * Route = new TiXmlElement( "rte" );
    TiXmlElement * RouteName = new TiXmlElement( "name" );
    TiXmlText * text4 = new TiXmlText( this->m_Route->GetValue().ToUTF8() );

    if (write_file){
        doc.LinkEndChild( root );
        root->SetAttribute("version", "1.1");
        root->SetAttribute("creator", "Route_pi by SaltyPaws");
        root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        root->SetAttribute("xmlns:gpxx","http://www.garmin.com/xmlschemas/GpxExtensions/v3" );
        root->SetAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
        root->SetAttribute("xmlns:opencpn","http://www.opencpn.org");
        Route->LinkEndChild( RouteName );
        RouteName->LinkEndChild( text4 );


        TiXmlElement * Extensions = new TiXmlElement( "extensions" );

        TiXmlElement * StartN = new TiXmlElement( "opencpn:start" );
        TiXmlText * text5 = new TiXmlText( "Datum" );
        Extensions->LinkEndChild( StartN );
        StartN->LinkEndChild( text5 );

        TiXmlElement * EndN = new TiXmlElement( "opencpn:end" );
        TiXmlText * text6 = new TiXmlText( "End of grid" );
        Extensions->LinkEndChild( EndN );
        EndN->LinkEndChild( text6 );

        Route->LinkEndChild( Extensions );
    }

    //Calculate GCL
   // double step_size=dist/100;

    if (error_occured){
        wxLogMessage(_("Error occured, aborting SAR calc!") );
        //wxMessageBox(_("Route interval > Distance, 0 or negative") );
        }

    //Call SAR routine here
    /*
    1 Parrallel Search
    2 Expanding Square
    3 Sector search
    4 Oil Rig
    */
    switch ( Pattern ) {
    case 1:
        {
        if (dbg) cout<<"Parrallel Search\n";
        //Expanding Parallel search Start
        double approach=0;
        double leg_distancex=0;
        double leg_distancey=0;
        double nlegs=0;
        double speed=0;
        double SAR_distance=0;
        bool First_Ship=false;
        if(!this->m_Approach_PS->GetValue().ToDouble(&approach)){ approach=0.0;} //approach course
        if(!this->m_dx_PS->GetValue().ToDouble(&leg_distancex)){ leg_distancex=1.0;} //leg distance
        if(!this->m_dy_PS->GetValue().ToDouble(&leg_distancey)){ leg_distancey=1.0;} //leg distance
        if(!this->m_n_PS->GetValue().ToDouble(&nlegs)){ nlegs=1.0;} //number of legs
        if(!this->m_Speed_PS->GetValue().ToDouble(&speed)){ speed=5.0;} // search velocity
        if(this->m_Nship->GetCurrentSelection()) First_Ship=true;//S=1


        if (leg_distancex<0.1) {leg_distancex=1.0;}//check for negative or small values
        if (nlegs<0.1) {nlegs=1.0;}//check for negative or small values
        /*pattern
        *datum
        *Start of search
            datum +approach+90-->-dx/2
        *Leg1
        start+approach-->distance
        pt1+approach+90-->width
        *leg2
        pt2+approach+180-->distance
        pt3+approach-90-->width
        leg3
        pt4+approach-->distance
        pt5+approach+90-->width
        */

        //add  datum
        if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lat1), wxString::Format(wxT("%f"),lon1), _T("Datum") ,_T("diamond"),_T("WPT"));}
        int n=0;
        //int multiplier=1;
        double lati, loni,ESheading=approach+180.0,ESdistance=leg_distancex/2;
        //calculate start position
        destRhumb(lat1, lon1, -ESheading,ESdistance, &lati, &loni);
        lat1=lati;
        lon1=loni;
        if (!First_Ship) ESheading=approach+90.0; else ESheading=approach-90.0;
        destRhumb(lat1, lon1, -ESheading,leg_distancey, &lati, &loni);
        lat1=lati;
        lon1=loni;

        if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lati), wxString::Format(wxT("%f"),loni), _T("Start") ,_T("diamond"),_T("WPT"));}

        wxString wpt_title;
        for ( int x = 1; x <= nlegs; x++ ) { //Loop over the number of legs
            for ( int y = 1; y <= 2; y++ ) { //Loop over the tracks
                if (y==1) {
                    if ( x % 2 != 0 ) ESheading=approach; else ESheading=approach+180.0 ;
                    ESdistance=leg_distancex;
                }
                else {
                    ESdistance=leg_distancey;
                    if (!First_Ship) ESheading=approach+90.0; else ESheading=approach-90.0;
                }

            n++;
            wpt_title=wxT("");
            wpt_title << wxT("Leg (") << x << wxT(") Pt(") << y <<wxT(")");
            destRhumb(lat1, lon1, -ESheading,ESdistance, &lati, &loni);
            SAR_distance+=ESdistance;
            if (write_file){Addpoint(Route,wxString::Format(wxT("%f"),lati),wxString::Format(wxT("%f"),loni), wpt_title ,_T("diamond"),_T("WPT"));}
            lat1=lati;
            lon1=loni;
            }
        }
        this->m_Distance->SetValue(wxString::Format(wxT("%g"), SAR_distance));
        this->m_Time->SetValue(wxString::Format(wxT("%g"), SAR_distance/speed));







            //Expanding Parallel search End
            break;
        }
        case 2:            // Note the colon, not a semicolon
        {
            //Expanding Square Start
            if (dbg) cout<<"Expanding Square\n";
            double approach=0.0;
            double leg_distancex=0.0;
            double squares=0.0;
            double speed=0.0;
            double SAR_distance=0;
            if(!this->m_Approach_ES->GetValue().ToDouble(&approach)){ approach=0.0;} //approach course
            if(!this->m_dx_ES->GetValue().ToDouble(&leg_distancex)){ leg_distancex=1.0;} //leg distance
            if(!this->m_n_ES->GetValue().ToDouble(&squares)){ squares=1.0;} //number of squares
            if(!this->m_Speed_ES->GetValue().ToDouble(&speed)){ speed=5.0;} // search velocity
            if (leg_distancex<0.1) {leg_distancex=1.0;}//check for negative or small values
            if (squares<1) {squares=1;}

            /* Pattern
            Datum
            square 1
            1 Datum+distance-->approach
            2 pt1+distance-->approach+90
            3 pt2+distancex2--approach+180
            4 pt3+distancex2-->approach+270
            5 pt4+distancex3-->approach
            6 pt5+distancex3-->approach+90
            7 pt6+distancex4-->approach+180
            */

            //add  datum
            if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lat1), wxString::Format(wxT("%f"),lon1), _T("Datum") ,_T("diamond"),_T("WPT"));}
            int n=0;
            int multiplier=0;
            double lati, loni;
            for ( int x = 1; x <= squares; x++ ) { //Loop over the number of search squares

                double ESdistance=leg_distancex;
                double ESheading=0;
                wxString wpt_title;

                for ( int y = 1; y <= 4; y++ ) {//Loop over the points in the square
                    switch ( y ) {
                        case 1:{ESheading=-approach;multiplier++;break;}
                        case 2:{ESheading=-approach-90.0;break;}
                        case 3:{ESheading=-approach-180.0;multiplier++;break;}
                        case 4:{ESheading=-approach-270.0;break;}
                    }
                    ESdistance=leg_distancex*multiplier;

                     n++;
                     wpt_title=wxT("");
                     wpt_title << wxT("Sq (") << x << wxT(") Pt(") << y <<wxT(")");
                     destRhumb(lat1, lon1, ESheading,ESdistance, &lati, &loni);
                     SAR_distance+=ESdistance;
                     if (write_file){Addpoint(Route,wxString::Format(wxT("%f"),lati),wxString::Format(wxT("%f"),loni), wpt_title ,_T("diamond"),_T("WPT"));}
                     lat1=lati;
                     lon1=loni;
                }

            }
            this->m_Distance->SetValue(wxString::Format(wxT("%g"), SAR_distance));
            this->m_Time->SetValue(wxString::Format(wxT("%g"), SAR_distance/speed));

        }
        //Expanding Square End
        break;
    case 3:
        {
        //Sector search start
        cout<<"Sector search\n";

        if (dbg) cout<<"Sector Square\n";
        double approach=0;
        double leg_distancex=0;
        double speed=0;
        double SAR_distance=0;
        bool two_cycles=false;
        if(!this->m_Approach_SS->GetValue().ToDouble(&approach)){ approach=0.0;} //approach course
        if(!this->m_dx_SS->GetValue().ToDouble(&leg_distancex)){ leg_distancex=1.0;} //leg distance
        if(!this->m_Speed_SS->GetValue().ToDouble(&speed)){ speed=5.0;} // search velocity
        if (leg_distancex<0.1) {leg_distancex=1.0;}//check for negative or small values
        if(this->m_Ncycles->GetCurrentSelection()) two_cycles=true;//S=1

        /* Pattern
        Datum
        Go Downwind for 1 mile,
        *** alter 120 degrees to starboard, this course for 1 mile
        then
        Alter to starboard 120 degrees and go on this course for 2 miles going through datum
        then
        Alter to starboard 120 degrees for 1 mile
        then
        Alter to starboard 120 degrees for 2 miles going through datum

        then
        Alter to starboard 120 degrees for 1 mile
        then
        Alter 120 degrees to starboard for 1 mile back to datum
        then go down the Blue track as follows
        Alter 30 degrees to starboard for 1 mile
        then go to the *** above and do the same again
        */
        //add  datum
        if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lat1), wxString::Format(wxT("%f"),lon1), _T("Datum") ,_T("diamond"),_T("WPT"));}
        int n=0,nleg=0;
        double lati, loni;
        double ESheading=-approach;
        for ( int x = 1; x<= ((two_cycles) ? 18 : 9); x++ ) { //Loop over the legs
            wxString wpt_title;
            n++;
            destRhumb(lat1, lon1, ESheading,leg_distancex, &lati, &loni);
            SAR_distance+=leg_distancex;
            if ( n % 3!= 0 ) {
                ESheading-=120.0;
                nleg++;
                wpt_title=wxT("");
                wpt_title << wxT("Leg (") << nleg << wxT(")");//" Pt(") << y <<wxT(")");
                if (write_file){Addpoint(Route,wxString::Format(wxT("%f"),lati),wxString::Format(wxT("%f"),loni), wpt_title ,_T("diamond"),_T("WPT"));}
                }

            if  ( n % 9 ==0 ) {
                nleg++;
                wpt_title=wxT("");
                wpt_title << wxT("Leg (") << nleg << wxT(")");//" Pt(") << y <<wxT(")")
                if (write_file){Addpoint(Route,wxString::Format(wxT("%f"),lati),wxString::Format(wxT("%f"),loni), wpt_title ,_T("diamond"),_T("WPT"));}
                if (two_cycles) ESheading-=30;
                }
             lat1=lati;
             lon1=loni;
         }
         this->m_Distance->SetValue(wxString::Format(wxT("%g"), SAR_distance));
         this->m_Time->SetValue(wxString::Format(wxT("%g"), SAR_distance/speed));
        //Sector search end
        break;
      }
    case 4:            // Note the colon, not a semicolon
        {
        //Oil Rig begin
        if (dbg) cout<<"Oil Rig\n";
        double approach=0;
        double leg_distancex=0;
        double nlegs=0;
        double speed=0;
        double SAR_distance=0;
        if(!this->m_Approach_OR->GetValue().ToDouble(&approach)){ approach=0.0;} //approach course
        if(!this->m_dx_OR->GetValue().ToDouble(&leg_distancex)){ leg_distancex=1.0;} //leg distance
        if(!this->m_NLegs_OR->GetValue().ToDouble(&nlegs)){ nlegs=1.0;} //number of legs
        if(!this->m_Speed_SS->GetValue().ToDouble(&speed)){ speed=5.0;} // search velocity
        if (leg_distancex<0.1) {leg_distancex=1.0;}//check for negative or small values
        if (nlegs<0.1) {nlegs=1.0;}//check for negative or small values
        /*pattern
        datum
        *Leg1
        datum+approach-->dx
        pt1+approach+90-->dx
        *leg2
        pt2+approach-->dx
        pt3+approach-90-->dx*3
        leg3
        pt4+approach-->dx
        pt5+approach+90-->dx*7
        */

        //add  datum
        if (write_file){Addpoint(Route, wxString::Format(wxT("%f"),lat1), wxString::Format(wxT("%f"),lon1), _T("Datum") ,_T("diamond"),_T("WPT"));}
        int n=0;
        int multiplier=1;
        double lati, loni,ESheading,ESdistance;
        bool sign_flipper=false;
        wxString wpt_title;
        for ( int x = 1; x <= nlegs; x++ ) { //Loop over the number of legs
            for ( int y = 1; y <= 2; y++ ) { //Loop over the tracks
                if (y==1) {
                    ESheading=approach;
                    ESdistance=leg_distancex;
                    }
                else {
                    ESdistance=leg_distancex*multiplier;
                    multiplier+=2;
                    if (sign_flipper) {
                           ESheading=approach-90;
                           sign_flipper=false;
                        }
                    else {
                        ESheading=approach+90;
                        sign_flipper=true;
                    }
                }
            n++;
            wpt_title=wxT("");
            wpt_title << wxT("Leg (") << x << wxT(") Pt(") << y <<wxT(")");
            destRhumb(lat1, lon1, -ESheading,ESdistance, &lati, &loni);
            SAR_distance+=ESdistance;
            if (write_file){Addpoint(Route,wxString::Format(wxT("%f"),lati),wxString::Format(wxT("%f"),loni), wpt_title ,_T("diamond"),_T("WPT"));}
            lat1=lati;
            lon1=loni;
            }
        }
        this->m_Distance->SetValue(wxString::Format(wxT("%g"), SAR_distance));
        this->m_Time->SetValue(wxString::Format(wxT("%g"), SAR_distance/speed));
        //Oil Rig end
        break;
       }
      default:
      {            // Note the colon, not a semicolon
        cout<<"Error, bad input, quitting\n";
        break;
      }
    }

       if (write_file){
            root->LinkEndChild( Route );
            // check if string ends with .gpx or .GPX
            if (!wxFileExists(s)){
                 s = s + _T(".gpx");
            }
            wxCharBuffer buffer=s.ToUTF8();
            if (dbg) std::cout<< buffer.data()<<std::endl;
            doc.SaveFile( buffer.data() );}
    //} //end of if no error occured

    if (error_occured==true)  {
        wxLogMessage(_("Error in calculation. Please check input!") );
        wxMessageBox(_("Error in calculation. Please check input!") );
    }

}

void Dlg::OnCursor(wxCommandEvent& event ){OnCursor();}
void Dlg::OnCursor(void){
    this->m_Lat1->SetValue(wxString::Format(wxT("%g"), plugin->GetCursorLat() ));
    this->m_Lon1->SetValue(wxString::Format(wxT("%g"), plugin->GetCursorLon() ));
}

void Dlg::OnShip(wxCommandEvent& event ){OnShip();}
void Dlg::OnShip(void){
    this->m_Lat1->SetValue(wxString::Format(wxT("%g"), plugin->GetShipLat() ));
    this->m_Lon1->SetValue(wxString::Format(wxT("%g"), plugin->GetShipLon() ));
}

void Dlg::key_shortcut(wxKeyEvent& event) {
    // of course, it doesn't have to be the control key. You can use others:
    // http://docs.wxwidgets.org/stable/wx_wxkeyevent.html
    if(event.GetModifiers() == wxMOD_CONTROL) {
        switch(event.GetKeyCode()) {
            case 'S': // can return the upper ASCII value of a key
                // do whatever you like for a Ctrl+G event here!
                //wxMessageBox(_("CTRL+G") );
                OnCursor();
                break;/*
            case WXK_LEFT: // we also have special keycodes for non-ascii values.
                // get a full list of special keycodes here:
                // http://docs.wxwidgets.org/stable/wx_keycodes.html
                wxMessageBox(_("CTRL+Left") );
                break;*/
            default: // do nothing
                break;
        }
    }
    event.Skip();
}
/*
void Dlg::mouse_shortcut(wxMouseEvent& event) {
    // of course, it doesn't have to be the control key. You can use others:
    // http://docs.wxwidgets.org/stable/wx_wxkeyevent.html
    if(event.LeftDown() == true) {
        wxMessageBox(_("CTRL+G") );
        OnCursor();
    }
    //else{
        event.Skip();
    //}

}*/


void Dlg::OnCursorSelect(wxCommandEvent& event){
    wxMessageBox(_("In order to copy the cursor location, select either the Lattitude or Longitude coordinate box, and press <CTRL>+S.") );
   // wxMessageBox(_("While this button is selected, or the cursor is in the lattitude or longitude box, you can copy the cursor location with <CTRL>+S") );
    event.Skip();
}


