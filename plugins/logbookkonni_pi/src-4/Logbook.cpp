#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#define PI 3.14159265

#include "LogbookDialog.h"
#include "Logbook.h"
#include "LogbookHTML.h"
#include "logbook_pi.h"
#include "Options.h"
#include "LogbookOptions.h"
#include "MessageBoxOSX.h"

#include "nmea0183/nmea0183.h"

#include <wx/object.h>
#include "wx/stdpaths.h"
#include "wx/fileconf.h"
#include <wx/image.h>
#include <wx/fs_inet.h>
#include "wx/grid.h"
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include "wx/generic/gridctrl.h"
#include <wx/dir.h> 
#include <wx/filefn.h>
#include <wx/msgdlg.h> 
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 

#include <math.h>

Logbook::Logbook(LogbookDialog* parent, wxString data, wxString layout, wxString layoutODT)
: LogbookHTML(this,parent,data,layout)
{
#ifdef PBVE_DEBUG
	pbvecount = 0;
#endif
	oldLogbook = false;
	OCPN_Message = false;
	noSentence = true;
	modified = false;
	wxString logLay;
	lastWayPoint = _T("");
    logbookDescription = wxEmptyString;
	sLinesReminder = _("Your Logbook has %i lines\n\nYou should create a new logbook to minimize loadingtime.");

	dialog = parent;
	opt = dialog->logbookPlugIn->opt;

	wxString logData = data; 
	logData.Append(_T("logbook.txt"));
	dialog->backupFile = _T("logbook");

	wxFileName wxHomeFiledir = logData ;
	if(!wxHomeFiledir.FileExists())
	{
		logbookFile = new wxTextFile(logData);	
		logbookFile->Create();
	}
	else
		logbookFile = new wxTextFile(logData);

	data_locn = logData;
	logbookData_actuell = logData;

	if(dialog->m_radioBtnHTML->GetValue())
		logLay = layout;
	else
		logLay = layoutODT;
	setLayoutLocation(logLay);

	//weatherCol = dialog->m_gridGlobal->GetNumberCols();
	//sailsCol   = dialog->m_gridGlobal->GetNumberCols()+weatherCol-1;

	noAppend = false;
	gpsStatus = false;
	waypointArrived = false;
	WP_skipped = false;
	bCOW = false;
	dCOW = -1;
	dCOG = -1;
	courseChange = false;
	everySM = false;
	guardChange = false;
	dLastMinute = -1;
	oldPosition.latitude = 500;
	activeRoute = wxEmptyString;
	activeRouteGUID = wxEmptyString;
	activeMOB = wxEmptyString;
    MOBIsActive = false;
    routeIsActive = false;
    trackIsActive = false;
    wimdaSentence = false;
    bSOW = false;
    bTemperatureWater = false;
    bTemperatureAir = false;
    bWind = false;
    bDepth = false;
    dtEngine1Off = -1;
    bRPM1 = false;
    dtEngine2Off = -1;
    bRPM2 = false;
    sRPM1Shaft = wxEmptyString;
    sRPM1Source = wxEmptyString;
    sRPM2Shaft = wxEmptyString;
    sRPM2Source = wxEmptyString;
    rpmSentence = false;
}

Logbook::~Logbook(void)
{
	update();
}

void Logbook::setTrackToNewID(wxString target)
{
    if(mergeList.Count() == 0) return;

    wxDir dir;
    wxArrayString files;
    dir.GetAllFiles(parent->data,&files,_T("until*.txt"),wxDIR_FILES);

        for(unsigned int i = 0; i < files.Count(); i++)
        {
            wxFileInputStream file(files[i]);
            wxTextInputStream txt(file);

            wxString data = wxEmptyString;
            while(!file.Eof())
                data += txt.ReadLine() + _T("\n");

                for(unsigned int n = 0; n < mergeList.GetCount(); n++)
                    data.Replace(mergeList.Item(n),target);

                    wxFileOutputStream fileo(files[i]);
                    wxTextOutputStream txto(fileo);
                    txto << data;
                    fileo.Close();
        }
}

void Logbook::setLayoutLocation(wxString loc)
{
	bool radio = dialog->m_radioBtnHTML->GetValue();
	loc.Append(_T("logbook"));
	dialog->appendOSDirSlash(&loc);
	layout_locn = loc;
	setFileName(data_locn, layout_locn);
	dialog->loadLayoutChoice(LogbookDialog::LOGBOOK,layout_locn,dialog->logbookChoice,opt->layoutPrefix[LogbookDialog::LOGBOOK]);
	if(radio)
		dialog->logbookChoice->SetSelection(dialog->logbookPlugIn->opt->navGridLayoutChoice);
	else
		dialog->logbookChoice->SetSelection(dialog->logbookPlugIn->opt->navGridLayoutChoiceODT);
}

void Logbook::SetPosition(PlugIn_Position_Fix &pfix)
{
	if(opt->traditional)
		sLat = this->toSDMM(1,pfix.Lat, true);
	else
		sLat = this->toSDMMOpenCPN(1,pfix.Lat, true);

	if(opt->traditional)
		sLon = this->toSDMM(2, pfix.Lon, true);
	else
		sLon = this->toSDMMOpenCPN(2, pfix.Lon, true);

	if(pfix.nSats != 0)
    {
		sSOG = wxString::Format(_T("%5.2f %s"), pfix.Sog, opt->speed.c_str());
        sCOG = wxString::Format(_T("%5.2f %s"), pfix.Cog, opt->Deg.c_str());
        SetGPSStatus(true);
    }
    else
        SetGPSStatus(false);
    
	mUTCDateTime.Set(pfix.FixTime);
//	dialog->GPSTimer->Start(5000);
}
void Logbook::clearNMEAData()
{
	noSentence = true;
}

void Logbook::SetSentence(wxString &sentence)
{
	wxDateTime dt;
	m_NMEA0183 << sentence;

#ifdef PBVE_DEBUG
	if(sentence.Contains(_T("$PBVE")))
	{
		if(pvbe != NULL && pbvecount < 15)
		{
			pvbe->m_textCtrlPVBE->AppendText(sentence);
			pvbe->SetFocus();
			pbvecount++;
		}
	}
#endif

	if(m_NMEA0183.PreParse())
    {
		noSentence = false;
		if(m_NMEA0183.LastSentenceIDReceived == _T("GGA"))
        {
            if(m_NMEA0183.Parse())
            {
                if(m_NMEA0183.Gga.GPSQuality > 0)
                {
                    SetGPSStatus(true);
                    setPositionString(m_NMEA0183.Gga.Position.Latitude.Latitude,
                            m_NMEA0183.Gga.Position.Latitude.Northing,
                            m_NMEA0183.Gga.Position.Longitude.Longitude,
                            m_NMEA0183.Gga.Position.Longitude.Easting);

                }
            }
        }
		else if(m_NMEA0183.LastSentenceIDReceived == _T("GLL"))
        {
            if(m_NMEA0183.Parse())
            {
                SetGPSStatus(true);
                setPositionString(m_NMEA0183.Gll.Position.Latitude.Latitude,
                        m_NMEA0183.Gll.Position.Latitude.Northing,
                        m_NMEA0183.Gll.Position.Longitude.Longitude,
                        m_NMEA0183.Gll.Position.Longitude.Easting);
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("ZDA"))
        {
            if(m_NMEA0183.Parse())
            {
                dt = dt.Set(m_NMEA0183.Zda.Day,(wxDateTime::Month)(m_NMEA0183.Zda.Month-1),m_NMEA0183.Zda.Year);
               // dt.ParseTime((const char)dt.ParseFormat(m_NMEA0183.Zda.UTCTime,_T("%H%M%S")));
				dt.ParseFormat(m_NMEA0183.Zda.UTCTime,_T("%H%M%S"));
                setDateTimeString(dt);
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("HDT"))
        {
            if(m_NMEA0183.Parse())
            {
                if(opt->showHeading == 0)
                    sCOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Hdt.DegreesTrue,opt->Deg.c_str());
                    dCOW = m_NMEA0183.Hdt.DegreesTrue;
                    bCOW = true;
                    dtCOW = wxDateTime::Now();
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("HDM"))
        {
            if(m_NMEA0183.Parse())
            {
                if(opt->showHeading == 1)
                    sCOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Hdm.DegreesMagnetic,opt->Deg.c_str());
                    dCOW = m_NMEA0183.Hdm.DegreesMagnetic;
                    bCOW = true;
                    dtCOW = wxDateTime::Now();
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("RMB"))
        {
            if(opt->waypointArrived)
            {
                if(m_NMEA0183.Parse())
                {
                    if(m_NMEA0183.Rmb.IsArrivalCircleEntered == NTrue)
                    {
                        if(m_NMEA0183.Rmb.From != lastWayPoint)
                        {
                            checkWayPoint(m_NMEA0183.Rmb);
                        }
                    }
                }
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
        {
            if(m_NMEA0183.Parse())
            {
                SetGPSStatus(true);
                setPositionString(m_NMEA0183.Rmc.Position.Latitude.Latitude,
                        m_NMEA0183.Rmc.Position.Latitude.Northing,
                        m_NMEA0183.Rmc.Position.Longitude.Longitude,
                        m_NMEA0183.Rmc.Position.Longitude.Easting);


                if(m_NMEA0183.Rmc.SpeedOverGroundKnots != 999.0)
                    sSOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.SpeedOverGroundKnots,opt->speed.c_str());
                if(m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
                    sCOG = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue, opt->Deg.c_str());
                if(m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
                    dCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                long day,month,year;
                m_NMEA0183.Rmc.Date.SubString(0,1).ToLong(&day);
                m_NMEA0183.Rmc.Date.SubString(2,3).ToLong(&month);
                m_NMEA0183.Rmc.Date.SubString(4,5).ToLong(&year);
                dt.Set(((int)day),(wxDateTime::Month)(month-1),((int)year+2000));
                //dt.ParseTime((const char)dt.ParseFormat(m_NMEA0183.Rmc.UTCTime,_T("%H%M%S")));
				dt.ParseFormat(m_NMEA0183.Rmc.UTCTime,_T("%H%M%S"));

                setDateTimeString(dt);

                if(!dialog->logbookPlugIn->eventsEnabled && opt->courseChange)
                    checkCourseChanged();
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("VHW"))
        {
            if(m_NMEA0183.Parse())
            {
                sSOW = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Vhw.Knots,opt->speed.c_str());
                dtSOW = wxDateTime::Now();
                bSOW = true;
            }
        }
        else if(m_NMEA0183.LastSentenceIDReceived == _T("MWV"))
        {
            if(m_NMEA0183.Parse())
            {
                double dWind = 0;
                if(opt->showWindHeading && bCOW)
                {
                    dWind = m_NMEA0183.Mwv.WindAngle + dCOW;
                    if(dWind > 360) { dWind -= 360; }
                }
                else
                    dWind = m_NMEA0183.Mwv.WindAngle;

					sWind = wxString::Format(_T("%3.0f %s %s"), dWind,opt->Deg.c_str(),m_NMEA0183.Mwv.Reference.c_str());

					wxString temp = _T("");
					if(m_NMEA0183.Mwv.WindSpeedUnits == 'N')
						temp = opt->windkts;
					else if(m_NMEA0183.Mwv.WindSpeedUnits == 'M')
						temp = opt->windmeter;
					else if(m_NMEA0183.Mwv.WindSpeedUnits == 'K')
						temp = opt->windkmh;

					sWindSpeed = wxString::Format(_T("%5.2f %s"), m_NMEA0183.Mwv.WindSpeed,temp.c_str());
                      dtWind = wxDateTime::Now();
                      bWind = true;
				  }
			}
            else if(m_NMEA0183.LastSentenceIDReceived == _T("MTW"))
            {	
                if(m_NMEA0183.Parse())
                {
                    double t;
                    if(opt->temperature == _T("F"))
                        t = (( m_NMEA0183.Mtw.Temperature * 9 ) / 5 ) + 32;
                    else
                        t = m_NMEA0183.Mtw.Temperature;
						sTemperatureWater = wxString::Format(_T("%4.1f %s %s"),t,opt->Deg.c_str(),opt->temperature.c_str());
                    dtTemperatureWater = wxDateTime::Now();
                    bTemperatureWater = true;
                }
			}
            else if(m_NMEA0183.LastSentenceIDReceived == _T("DBT"))
            {			
					  m_NMEA0183.Parse();
                      dtDepth = wxDateTime::Now();
                      bDepth = true;
					  if(m_NMEA0183.Dbt.ErrorMessage.Contains(_T("Invalid")) ||
						  (m_NMEA0183.Dbt.DepthMeters == m_NMEA0183.Dbt.DepthFathoms))
					  {
						sDepth = _T("-----");
					  }
					  else
					  {
						switch(opt->showDepth)
                        {
							case 0:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthMeters, opt->meter.c_str());
								break;
							case 1:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthFeet, opt->feet.c_str());
								break;
							case 2:
								sDepth = wxString::Format(_T("%5.1f %s"), 
									m_NMEA0183.Dbt.DepthFathoms, opt->fathom.c_str());
							break;
						}
					  }
			}
	}

/*  Propietary NMEA sentences														*/
/*																					*/
/*  use function appendRow to add the values to the grid							*/
/*  For motorhours and/or fuel recalculate the grid with							*/
/*	changeCellValue(lastRow, 0,0)													*/
/*	In function checkGPS(bool appendClick) set the strings to emtpy string when GPS */
/*  is off.																			*/

    wxStringTokenizer tkz(sentence,_T(","));
    wxString sentenceInd = tkz.GetNextToken();

    if(sentenceInd == _T("$WIMDA") && opt->NMEAUseWIMDA)
    {
        wimdaSentence = true;
        dtWimda = wxDateTime::Now();

        double t;
        long p;
    
        tkz.GetNextToken();
        tkz.GetNextToken();
        tkz.GetNextToken().ToLong(&p);

        sPressure = wxString::Format(_T("%4d %s"),p,opt->baro.c_str());
        tkz.GetNextToken();

        tkz.GetNextToken().ToDouble(&t);
        if(opt->temperature == _T("F"))
            t = (( t * 9 ) / 5 ) + 32;
        sTemperatureAir = wxString::Format(_T("%2.2f %s %s"),t,opt->Deg.c_str(),opt->temperature.c_str());

        tkz.GetNextToken();
        tkz.GetNextToken();
        tkz.GetNextToken();
        sHumidity = tkz.GetNextToken();
    }
    else if(opt->bRPMIsChecked && sentenceInd == _T("$ERRPM"))
    {
        rpmSentence = true;
        if(opt->bRPMCheck)
            parent->logbookPlugIn->optionsDialog->setRPMSentence(sentence);
        long Umin1 = 0, Umin2 = 0;

        dtRPM = wxDateTime::Now();

        wxString source = tkz.GetNextToken();
        wxString engineNr = tkz.GetNextToken();
        wxString speed = tkz.GetNextToken();
        wxString pitch = tkz.GetNextToken();

        if(engineNr == opt->engine1)
        {
            speed.ToLong(&Umin1);
            if(source == _T("E"))
                sRPM1 = speed;
            sRPM1Source = source;

            if(Umin1 != 0L)
            {
                bRPM1 = true;
                if(source == _T("E"))
                {
                    if(!opt->engine1Running)
                    {
                        opt->dtEngine1On = wxDateTime::Now();
                        if(opt->engineMessageSails && opt->engineAllwaysSailsDown)
                            dialog->resetSails();
                        appendRow(false);
                    }
                    opt->engine1Running = true;
                }
                if(source == _T("S"))
                    sRPM1Shaft = speed;
            }
            else
            {
                bRPM1 = false;
                if(opt->engine1Running)
                {
                    dtEngine1Off = wxDateTime::Now().Subtract(opt->dtEngine1On);
                    opt->dtEngine1On = -1;
                    if(opt->engineMessageSails)
                        dialog->stateSails();
                    appendRow(false);
                    opt->engine1Running = false;
                }
            }
        }
        else
        {
            speed.ToLong(&Umin2);
            if(source == _T("E"))
                sRPM2 = speed;
                    
            if(Umin2 != 0L)
            {
                bRPM2 = true;
                if(source == _T("E"))
                {
                    if(!opt->engine2Running)
                    {
                        opt->dtEngine2On = wxDateTime::Now();
                        if(opt->engineMessageSails && opt->engineAllwaysSailsDown)
                            dialog->resetSails();
                        appendRow(false);
                    }
                    opt->engine2Running = true;
                }
                if(source == _T("S"))
                    sRPM2Shaft = speed;
            }
            else
            {
                bRPM2 = false;
                if(opt->engine2Running)
                {
                    dtEngine2Off = wxDateTime::Now().Subtract(opt->dtEngine2On);
                    opt->dtEngine2On = -1;
                    if(opt->engineMessageSails)
                        dialog->resetSails();
                    appendRow(false);
                    opt->engine2Running = false;
                }
            }
        }

    }

}

void Logbook::setDateTimeString(wxDateTime dt)
{
	mUTCDateTime = dt;

	if(opt->gpsAuto)
	{
		if(newPosition.NSflag == 'E')
			opt->tzIndicator = 0;
		else
			opt->tzIndicator = 1;

		opt->tzHour = (int) newPosition.longitude / 15;
	}

	wxTimeSpan span(opt->tzHour, 0, 0, 0);
	if(opt->tzIndicator == 0)
		mCorrectedDateTime = mUTCDateTime + span;
	else
		mCorrectedDateTime = mUTCDateTime - span;

	if(opt->UTC)
		mCorrectedDateTime = mUTCDateTime;

	sDate = mCorrectedDateTime.Format(opt->sdateformat);
	sTime = mCorrectedDateTime.Format(opt->stimeformat);
}

void Logbook::setPositionString(double dLat, int iNorth, double dLon, int iEast)
{
    double lat, lon;
	float llt = dLat;
    int lat_deg_int = (int)(llt / 100);
    float lat_deg = lat_deg_int;
    float lat_min = llt - (lat_deg * 100);
    lat = lat_deg + (lat_min/60.);
    if(iNorth == South)
           lat = -lat;
	if(opt->traditional)
		sLat = this->toSDMM(1,lat, true);
	else
		sLat = this->toSDMMOpenCPN(1,lat, true);

	float lln = dLon;
    int lon_deg_int = (int)(lln / 100);
    float lon_deg = lon_deg_int;
    float lon_min = lln - (lon_deg * 100);
    lon = lon_deg + (lon_min/60.);
    if(iEast == West)
          lon = -lon;
	if(opt->traditional)
		sLon = this->toSDMM(2,lon, false);
	else
		sLon = this->toSDMMOpenCPN(2,lon, true);

	SetGPSStatus(true);
//	dialog->GPSTimer->Start(5000);

	if(opt->everySM)
		checkDistance();
}

void Logbook::newLogbook()
{
	bool zero = false;

	if(data_locn != this->logbookData_actuell)
		this->switchToActuellLogbook();

#ifdef __WXOSX__
    int i = MessageBoxOSX(this->dialog,_("Are you sure ?"),_("New Logbook"),wxID_NO|wxID_OK);
    if(i == wxID_NO)
	{ 	dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus(); return; }
    
    int ii = MessageBoxOSX(this->dialog,_("Reset all Values to zero ?"),_T("New Logbook"),wxID_OK|wxID_NO);
    if(ii == wxID_OK)
        zero = true;
    else if(ii == wxID_NO)
	{ 	dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus(); return; }
#else
	int i = wxMessageBox(_("Are you sure ?"),_("New Logbook"),wxYES_NO );
	if(i == wxNO)
	{ 	dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus(); return;}

	i = wxMessageBox(_("Reset all Values to zero ?"),_T(""),wxYES_NO );
	if(i == wxYES)
		zero = true;
#endif

	if(dialog->m_gridGlobal->GetNumberRows() <= 0)
	{
#ifdef __WXOSX__
        MessageBoxOSX(this->dialog, _("Your Logbook has no lines ?"),_("New Logbook"),wxID_OK);
        return;
#else
		wxMessageBox(_("Your Logbook has no lines ?"),_("New Logbook"),wxOK );
		return;
#endif
	}

	update();

	wxFileName fn = data_locn;
	wxString temp = fn.GetPath();
	dialog->appendOSDirSlash(&temp);
	wxString sn;
	wxString ss = wxDateTime::Now().FormatISOTime();
	ss.Replace(_T(":"),_T("_"));
	ss = wxString::Format(_T("until_%s_%s_logbook.txt"),wxDateTime::Now().FormatISODate().c_str(),ss.c_str());
	sn = temp+ss;
	
	wxCopyFile(data_locn,sn);

	wxArrayString s;
	for(int i = 0; i < dialog->numPages; i++)
	{
		for(int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++)
		{
			s.Add(dialog->logGrids[i]->GetCellValue(dialog->logGrids[i]->GetNumberRows()-1,n));
		}
	}

	logbookFile->Open();
	logbookFile->Clear();
	logbookFile->Write();
	logbookFile->Close();

	clearAllGrids();

	int offset = 0;
	dialog->selGridRow = 0;
	for(int i = 0; i < dialog->numPages; i++)
	{
		if(zero) break;
		dialog->logGrids[i]->AppendRows();
		if(i > 0) offset += dialog->logGrids[i-1]->GetNumberCols();

		for(int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++)
		{
			dialog->logGrids[i]->SetCellValue(0,n,s[n+offset]);
		}
	}
	if(!zero)
	{
		dialog->logGrids[0]->SetCellValue(0,13,_("Last line from Logbook\n")+ss);
		dialog->logGrids[0]->SetCellValue(0,6,dialog->logGrids[0]->GetCellValue(0,6));
		wxString t = _T("0.00 ")+opt->distance;
		dialog->logGrids[0]->SetCellValue(0,5,t);
	}
	else
	{
		appendRow(true);
		dialog->logGrids[0]->SetCellValue(0,13,_("Last Logbook is\n")+ss);
	}

	update();

	dialog->setEqualRowHeight(0);
	setCellAlign(0);
	dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
}

void Logbook::selectLogbook()
{
	wxString path(*dialog->pHome_Locn);
	path = path + _T("data");

    update();
	SelectLogbook selLogbook(dialog,path);

	if(selLogbook.ShowModal() == wxID_CANCEL)
	{
		dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
		return;
	}

	if(selLogbook.selRow == -1) { 	dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus(); return; }
	
	wxString s = selLogbook.files[selLogbook.selRow];

	for(int i = 0; i < LOGGRIDS; i++)
		if(dialog->logGrids[i]->GetNumberRows() != 0)
			dialog->logGrids[i]->DeleteRows(0,dialog->logGrids[i]->GetNumberRows());

	loadSelectedData(s);
}

void Logbook::loadSelectedData(wxString path)
{
	data_locn = path;
	logbookFile = new wxTextFile(path);
	setFileName(path,layout_locn);
	wxFileName fn(path);
	path = fn.GetName();
	dialog->backupFile = path;
	if(path == _T("logbook"))
	{
		path = _("Active Logbook");
		oldLogbook = false;
	}
	else
	{
		wxDateTime dt = dialog->getDateTo(path);
		path = wxString::Format(_("Old Logbook until %s"),dt.FormatDate().c_str()); 
		oldLogbook = true;
	}
	title = path;
	dialog->SetTitle(title);

	loadData();
}
void Logbook::clearAllGrids()
{
	if(dialog->m_gridGlobal->GetNumberRows() > 0)
	{
		dialog->m_gridGlobal->DeleteRows(0,dialog->m_gridGlobal->GetNumberRows(),false);
		dialog->m_gridWeather->DeleteRows(0,dialog->m_gridWeather->GetNumberRows(),false);
		dialog->m_gridMotorSails->DeleteRows(0,dialog->m_gridMotorSails->GetNumberRows(),false);
	}
}

void Logbook::loadData()
{
	wxString s = _T(""),t;
	wxString nullhstr = _T("00:00");
	double nullval = 0.0;
	wxString dateFormat;

	dialog->selGridCol = dialog->selGridRow = 0;
	if(title.IsEmpty())
        title = _("Active Logbook");

	clearAllGrids();

	int row = 0;

	/** make a backup of 0.910 */
	wxString sep = wxFileName::GetPathSeparator();
	wxString source = *dialog->pHome_Locn +_T("data") + sep;
	wxString dest   = *dialog->pHome_Locn +_T("data") + sep + _T("910_Backup");

	wxFileInputStream input1( data_locn );
	wxTextInputStream* stream1 = new wxTextInputStream (input1);
	
	t = stream1->ReadLine();
	if(t.IsEmpty()) return; // first install only
	if(t.Contains(_T("#1.2#")))
	{
		dateFormat = t;
		t = stream1->ReadLine();
	}
	else
	{
		wxArrayString files;

		wxDir dir;
		wxString path = dialog->data;
		wxString dest = path+_T("Backup_1_1");
		wxDir destDir(dest);

		if(!wxDir::Exists(dest))
			::wxMkdir(dest);

#ifdef __WXOSX__
		MessageBoxOSX(NULL,wxString::Format(_("Start converting to new Date/Time-Format\nand backup all datafiles from version 1.1 to\n\n%s"),dest.c_str()),_("Information"),wxID_OK);
#else
		wxMessageBox(wxString::Format(_("Start converting to new Date/Time-Format\nand backup all datafiles from version 1.1 to\n\n%s"),dest.c_str()));
#endif
		dir.GetAllFiles(path,&files,_T("*.txt"),wxDIR_FILES);
		dest += wxFileName::GetPathSeparator();

		for(unsigned int i = 0; i < files.Count(); i++)
		{   
			wxFileName fn(files[i]);
			::wxCopyFile(path+fn.GetFullName(),dest+fn.GetFullName());
		}

		convertTo_1_2();
	}
	wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );

	if(tkz.CountTokens() == 33 && !wxDir::Exists(dest))
	{
		::wxMkdir(dest);
		wxArrayString files;
		wxDir dir;
		dir.GetAllFiles(source.RemoveLast(),&files,_T("*.txt"),wxDIR_FILES);
		for(unsigned int i = 0; i < files.Count(); i++)
		{
			wxFileName fn(files[i]);
			::wxCopyFile(files[i],dest+sep+fn.GetFullName(),true);
		}
	}
	
	/***************************/

	wxFileInputStream input( data_locn );
	wxTextInputStream* stream = new wxTextInputStream (input, _T("\n"),wxConvUTF8);
	
	wxString firstrow = stream->ReadLine(); // for #1.2#
    wxStringTokenizer first(firstrow, _T("\t"),wxTOKEN_RET_EMPTY );
    first.GetNextToken();
    logbookDescription = first.GetNextToken();

	wxDateTime dt;
	int month = 0,day = 0,year = 0,hour = 0,min = 0,sec = 0;
	dialog->m_gridGlobal->BeginBatch();
	dialog->m_gridWeather->BeginBatch();
	dialog->m_gridMotorSails->BeginBatch();
	int lines = 0;
	while( !(t = stream->ReadLine()).IsEmpty())
	{
		if(input.Eof()) break;
		lines++;
		dialog->m_gridGlobal->AppendRows();
		dialog->m_gridWeather->AppendRows();
		dialog->m_gridMotorSails->AppendRows();

		setCellAlign(row);

		wxStringTokenizer tkz(t, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;
		int fields =  tkz.CountTokens();

		while ( tkz.HasMoreTokens() )
		{
			s = dialog->restoreDangerChar(tkz.GetNextToken());
			s.RemoveLast();

			switch(c)
			{
			case 0:		dialog->m_gridGlobal->SetCellValue(row,ROUTE,s);
                break;
			case 1:		month = wxAtoi(s);
                break;
			case 2:     day = wxAtoi(s);
                break;
			case 3:		year = wxAtoi(s);
						dt.Set(day,(wxDateTime::Month)month,year);
						dialog->m_gridGlobal->SetCellValue(row,RDATE,dt.Format(opt->sdateformat));
                break;
			case 4:		hour = wxAtoi(s);
                break;
			case 5:		min = wxAtoi(s);
                break;
			case 6:		sec = wxAtoi(s);
						dt.Set(hour,min,sec);
						dialog->m_gridGlobal->SetCellValue(row,RTIME,dt.Format(opt->stimeformat));
                break;
			case 7:		dialog->m_gridGlobal->SetCellValue(row,SIGN,s);
                break;
			case 8:		dialog->m_gridGlobal->SetCellValue(row,WAKE,s);
                break;
			case 9:		dialog->m_gridGlobal->SetCellValue(row,DISTANCE,s);
                break;
			case 10:	dialog->m_gridGlobal->SetCellValue(row,DTOTAL,s);
						dialog->m_gridGlobal->SetReadOnly(row,DTOTAL);
                break;
            case 11:
                        dialog->m_gridGlobal->SetCellValue(row,POSITION,s);
                break;
			case 12:
                        dialog->m_gridGlobal->SetCellValue(row,COG,s);
                        break;
			case 13:	dialog->m_gridGlobal->SetCellValue(row,COW,s);
                break;
			case 14:	dialog->m_gridGlobal->SetCellValue(row,SOG,s);
                break;
			case 15:	dialog->m_gridGlobal->SetCellValue(row,SOW,s);
                break;
			case 16:	dialog->m_gridGlobal->SetCellValue(row,DEPTH,s);
                break;
			case 17:
                    dialog->m_gridGlobal->SetCellValue(row,REMARKS,s);
                break;
			case 18:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::BARO,s);
                break;
			case 19:
                        dialog->m_gridWeather->SetCellValue(row,LogbookHTML::WIND,s);
				break;
			case 20:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::WSPD,s);
				break;
			case 21:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::CURRENT,s);
				break;
			case 22:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::CSPD,s);
				break;
			case 23:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::WAVE,s);
				break;
			case 24:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::SWELL,s);
				break;
			case 25:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::WEATHER,s);
				break;
			case 26:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::CLOUDS,s);
				break;
			case 27:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::VISIBILITY,s);
				break;
			case 28:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTOR,s);
				break;
			case 29:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTORT,s);
				break;
			case 30:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::FUEL,s);
				break;
			case 31:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::FUELT,s);
				break;
			case 32:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::SAILS,s);
				break;
			case 33:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::REEF,s);
				break;
			case 34:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATER,s);
				break;
			case 35:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERT,s);
				break;
 
            case 36:
                    dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MREMARKS,s);
				break;
			case 37:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::HYDRO,s);
				break;
			case 38:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::AIRTE,s);
				break;
			case 39:	dialog->m_gridWeather->SetCellValue(row,LogbookHTML::WATERTE,s);
				break;
			case 40:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTOR1,s);
				break;
			case 41:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTOR1T,s);
				break;
			case 42:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::GENE,s);
				break;
			case 43:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::GENET,s);
				break;
			case 44:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK1,s);
				break;
			case 45:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK1T,s);
				break;
			case 46:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK2,s);
				break;
			case 47:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK2T,s);
				break;
			case 48:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERM,s);
				break;
			case 49:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERMT,s);
				break;
			case 50:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERMO,s);
				break;
            case 51:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::ROUTEID,s);
                break;
            case 52:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::TRACKID,s);
                break;
            case 53:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::RPM1,s);
                break;
            case 54:	dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::RPM2,s);
            //    int in =  0;
                break;
			}			
			c++;
		}
		wxString temp = dialog->m_gridGlobal->GetCellValue(row,DISTANCE);
		temp.Replace(_T(","),_T("."));
		double dist = wxAtof(temp);
		if((dialog->m_gridGlobal->GetCellValue(row,SIGN) == wxEmptyString || 
			dialog->m_gridGlobal->GetCellValue(row,SIGN).GetChar(0) == ' ') && dist > 0)
				dialog->m_gridGlobal->SetCellValue(row,SIGN,_T("S"));

		if(fields < dialog->totalColumns) // data from 0.910 ? need zero-values to calculate the columns 
			{
				dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTOR1, wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::MOTOR1T,wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::GENE,   wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::GENET,  wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK1,  wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK1T, wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK2,  wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::BANK2T, wxString::Format(_T("%2.2f %s"),nullval,opt->ampereh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERM, wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERMT,wxString::Format(_T("%s %s"),nullhstr.c_str(),opt->motorh.c_str()));
                dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERMO,wxString::Format(_T("%2.2f %s"),nullval,opt->vol.c_str()));
            }

		dialog->setEqualRowHeight(row);
		row++;
	}

	dialog->selGridRow = 0; dialog->selGridCol = 0;

	for(int i = 0; i < LOGGRIDS; i++)
	{
		dialog->logGrids[i]->Refresh();
		row = dialog->logGrids[i]->GetNumberRows()-1;
		dialog->logGrids[i]->MakeCellVisible(row,0);
		dialog->logGrids[i]->SetGridCursor(0,0);
	}

	dialog->m_gridGlobal->EndBatch();
	dialog->m_gridWeather->EndBatch();
	dialog->m_gridMotorSails->EndBatch();

	if(!oldLogbook && lines >= 500)
	{
		wxString str = wxString::Format(sLinesReminder,lines);
		LinesReminderDlg *dlg = new LinesReminderDlg(str,dialog);
		dlg->Show();
	}
}

wxString Logbook::makeDateFromFile(wxString date, wxString dateformat)
{
	wxStringTokenizer tkzd(date,_T("/"));
	wxDateTime dt;
	int month = wxAtoi(tkzd.GetNextToken());
	int day   = wxAtoi(tkzd.GetNextToken());
	int year  = wxAtoi(tkzd.GetNextToken());
	dt.Set(day,(wxDateTime::Month) month, year);

	return dt.Format(dateformat);
}

wxString Logbook::makeWatchtimeFromFile(wxString time, wxString timeformat)
{
	wxStringTokenizer tkzt(time,_T(","));
	wxDateTime dts,dte;
	int hours = wxAtoi(tkzt.GetNextToken());
	int mins  = wxAtoi(tkzt.GetNextToken());
	int houre = wxAtoi(tkzt.GetNextToken());
	int mine  = wxAtoi(tkzt.GetNextToken());
	dts.Set(hours,mins);
	dte.Set(houre,mine);

	return dts.Format(timeformat)+_T("-")+dte.Format(timeformat);
}

void Logbook::convertTo_1_2()
{
	wxString path = dialog->data;
	wxArrayString files;
	wxDir dir;
	int dtformat = opt->dateformat;
	int timeFormat = opt->timeformat;
	wxDateTime dt = wxDateTime::Now();
	bool b = true;

	wxString datePattern = LogbookDialog::datePattern;
	opt->dateformat = 0;
	opt->timeformat = 1;

	opt->setDateFormat();
	opt->setTimeFormat(1);

	update();

	dir.GetAllFiles(path,&files,_T("*logbook*.txt"),wxDIR_FILES);

	for(unsigned int i = 0; i < files.Count(); i++)
	{
		wxFileName fn(files[i]);
		wxFileInputStream stream( path + fn.GetFullName() );
		wxTextInputStream* in = new wxTextInputStream (stream,_T("\n"),wxConvUTF8);
		wxFileOutputStream stream1( path + fn.GetFullName() + _T("_") );
		wxTextOutputStream* out = new wxTextOutputStream (stream1,wxEOL_NATIVE,wxConvUTF8);
		
		int l = 0;
		
		while(true)
		{
			wxString s = in->ReadLine();
			if(stream.Eof() || s.IsEmpty()) break;

			wxStringTokenizer tkz(s,_T("\t"));
			tkz.GetNextToken();
			wxString d = tkz.GetNextToken();
			b  = LogbookDialog::myParseDate(d,dt);
			s.Replace(d,wxString::Format(_T("%i \t%i \t%i "),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
			wxString t = tkz.GetNextToken();
			LogbookDialog::myParseTime(t,dt);
			s.Replace(t,wxString::Format(_T("%i \t%i \t%i "),dt.GetHour(),dt.GetMinute(),dt.GetSecond()));

			if(l == 0) *out << _T("#1.2#\n");
			l++;
			*out << s+_T("\n");
		}
		stream1.Close();
		if(b)
		{
			::wxCopyFile(path + fn.GetFullName() + _T("_"), path + fn.GetFullName() );
			::wxRemoveFile(path + fn.GetFullName() + _T("_"));
		}
	}

	wxString m = _T("service.txt");
	wxFileInputStream streams( path + m );
	wxTextInputStream* in = new wxTextInputStream (streams,_T("\n"),wxConvUTF8);
	wxFileOutputStream stream2( path + m + _T("_") );
	wxTextOutputStream* out = new wxTextOutputStream (stream2,wxEOL_NATIVE,wxConvUTF8);
	
	int l = 0;
	while(true)
	{
		int i = 0;
		wxString tmp;
		wxString s = in->ReadLine();
		if(streams.Eof() || s.IsEmpty()) break;

		wxStringTokenizer tkz(s,_T("\t"));
		tkz.GetNextToken(); tkz.GetNextToken();
		wxString d = tkz.GetNextToken();
		d.RemoveLast();
		for(i = 0; i < dialog->maintenance->m_choicesCount; i++)
		{
			if(d == dialog->maintenance->m_choices[i])
				break;
		}

		if(i == 8)
		{
			tmp = tkz.GetNextToken();
			tmp.RemoveLast();
			LogbookDialog::myParseDate(tmp,dt);
			s.Replace(tmp,wxString::Format(_T("%i/%i/%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
			tmp = tkz.GetNextToken();
			LogbookDialog::myParseDate(tmp,dt);
			s.Replace(tmp,wxString::Format(_T("%i/%i/%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
		}
		else if( i > 8)
		{
			tmp = tkz.GetNextToken();
			tmp = tkz.GetNextToken();
			tmp = tkz.GetNextToken();
			tmp.RemoveLast();
			if(!tmp.IsEmpty())
			{
				LogbookDialog::myParseDate(tmp,dt);
				s.Replace(tmp,wxString::Format(_T("%i\t%i\t%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
			}
		}
		if(l == 0) *out << _T("#1.2#\n");
		l++;
		*out << s+_T("\n");
	}
	stream2.Close();
	if(b)
	{
		::wxCopyFile(path + m + _T("_"), path + m );
		::wxRemoveFile(path + m + _T("_"));
	}

	m = _T("crewlist.txt");
	wxFileInputStream streamc(path + m );
	in = new wxTextInputStream (streamc,_T("\n"),wxConvUTF8);
	wxFileOutputStream stream3( path + m + _T("_") );
	out = new wxTextOutputStream (stream3,wxEOL_NATIVE,wxConvUTF8);

	l = 0;
	while(true)
	{
		wxString tmp;
		wxString s = in->ReadLine();
		if(streamc.Eof() || s.IsEmpty()) break;

		wxStringTokenizer tkz(s,_T("\t"));
		tkz.GetNextToken(); tkz.GetNextToken(); tkz.GetNextToken(); tkz.GetNextToken(); tkz.GetNextToken();
		wxString d = tkz.GetNextToken();
		if(!d.IsEmpty() && d.GetChar(0) != ' ')
		{
			d.RemoveLast();
			tmp = d;
			LogbookDialog::myParseDate(d,dt);
			s.Replace(tmp,wxString::Format(_T("%i/%i/%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
		}
		tkz.GetNextToken();tkz.GetNextToken();tkz.GetNextToken();tkz.GetNextToken();
		d = tkz.GetNextToken();
		if(!d.IsEmpty() && d.GetChar(0) != ' ')
		{
			d.RemoveLast();
			tmp = d;
			LogbookDialog::myParseDate(d,dt);
			s.Replace(tmp,wxString::Format(_T("%i/%i/%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
		}
		if(l == 0) *out << _T("#1.2#\n");
		l++;
		*out << s+_T("\n");
	}
	stream3.Close();

	::wxCopyFile(path + m + _T("_"), path + m );
	::wxRemoveFile(path + m + _T("_"));

	m = _T("boat.txt");
	wxFileInputStream streamb( path + m );
	in = new wxTextInputStream (streamb,_T("\n"),wxConvUTF8);
	wxFileOutputStream stream4( path + m + _T("_") );
	out = new wxTextOutputStream (stream4,wxEOL_NATIVE,wxConvUTF8);

	l = 0;
	while(true)
	{
		wxString tmp;
		wxString s = in->ReadLine();
		if(streamb.Eof() || s.IsEmpty()) break;

		wxStringTokenizer tkz(s,_T("\t"));
		for(int x = 0; x < 18; x++)
			tkz.GetNextToken();
		wxString d = tkz.GetNextToken();
		if(!d.IsEmpty() && d.GetChar(0) != ' ')
		{
			d.RemoveLast();
			tmp = d;
			LogbookDialog::myParseDate(d,dt);
			s.Replace(tmp,wxString::Format(_T("%i/%i/%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear()));
		}

		if(l == 0) *out << _T("#1.2#\n");
		l++;
		*out << s+_T("\n");
	}
	stream4.Close();

	::wxCopyFile(path + m + _T("_"), path + m );
	::wxRemoveFile(path + m + _T("_"));

	opt->dateformat = dtformat;
	opt->timeformat = timeFormat;
	opt->setDateFormat();
	opt->setTimeFormat(opt->timeformat);
}

void Logbook::setCellAlign(int i)
{
		dialog->m_gridGlobal->SetCellAlignment    (i,ROUTE,                wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,RDATE,                wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,RTIME,                wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,SIGN,                 wxALIGN_CENTRE, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,WAKE,                 wxALIGN_LEFT, wxALIGN_TOP);
		dialog->m_gridGlobal->SetCellAlignment    (i,REMARKS,              wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridWeather->SetCellAlignment   (i,WEATHER,   wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridWeather->SetCellAlignment   (i,CLOUDS,    wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridWeather->SetCellAlignment   (i,VISIBILITY,wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridMotorSails->SetCellAlignment(i,LogbookHTML::SAILS,       wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridMotorSails->SetCellAlignment(i,LogbookHTML::REEF,        wxALIGN_LEFT, wxALIGN_TOP);
        dialog->m_gridMotorSails->SetCellAlignment(i,LogbookHTML::MREMARKS,    wxALIGN_LEFT, wxALIGN_TOP);

		dialog->m_gridGlobal->SetReadOnly(i,POSITION,true);
}

void Logbook::switchToActuellLogbook()
{
	dialog->selGridRow = 0; dialog->selGridCol = 0; 
	logbookFile = new wxTextFile(logbookData_actuell);
	data_locn = logbookData_actuell;
	setFileName(logbookData_actuell,layout_locn);
	dialog->SetTitle(_("Active Logbook"));	
	loadData();
}


void Logbook::appendRow(bool mode)
{
	wxString s;

	if(dialog->m_gridGlobal->IsSelection())
		dialog->deselectAllLogbookGrids();

	checkGPS(mode);

	if(noAppend) return;
	modified = true;

	wxFileName fn(logbookFile->GetName());
	if(fn.GetName() != (_T("logbook")))
	{

		switchToActuellLogbook();
		noAppend = true;
		NoAppendDialog *x = new NoAppendDialog(dialog);
		x->Show();

		noAppend = false;
		oldLogbook = false;
	}

	int lastRow = dialog->logGrids[0]->GetNumberRows();
	if(lastRow >= 499)
	{
		static int repeat=lastRow;
		//dialog->timer->Stop();
		if(lastRow == repeat)
		{
			repeat += 50;
			wxString str = wxString::Format(sLinesReminder,lastRow+1);
			LinesReminderDlg* dlg = new LinesReminderDlg(str,dialog);
			dlg->Show();
/*#ifdef __WXOSX__
        MessageBoxOSX(this->dialog, wxString::Format(_("Your Logbook has %i lines\n\nYou should create a new logbook to minimize loadingtime."),lastRow),_("Information"),wxID_OK);
#else
			wxMessageBox(wxString::Format(_("Your Logbook has %i lines\n\n\
You should create a new logbook to minimize loadingtime."),lastRow),_("Information"));
#endif
*/
		}
	//	dialog->logbookPlugIn->opt->timer = false;

	/*	wxFileConfig *pConf = (wxFileConfig *)dialog->logbookPlugIn->m_pconfig;

		if(pConf)
		{
            pConf->SetPath ( _T ( "/PlugIns/Logbook" ) );
			pConf->Write ( _T( "Timer" ), dialog->logbookPlugIn->opt->timer );
		}		
		*/
	}

	for(int i = 0; i < dialog->numPages; i++ )
		dialog->logGrids[i]->AppendRows();

	if(lastRow > 0)
		{
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,dialog->logGrids[0]->GetCellValue(lastRow-1,ROUTE));
			//if(gpsStatus)
				dialog->logGrids[0]->SetCellValue(lastRow,POSITION,sLat+sLon);
			//else
			//	dialog->logGrids[0]->SetCellValue(lastRow,POSITION,dialog->logGrids[0]->GetCellValue(lastRow-1,POSITION));
			changeCellValue(lastRow, 0,0);
			dialog->logGrids[0]->SetCellValue(lastRow,DTOTAL,dialog->logGrids[0]->GetCellValue(lastRow-1,DTOTAL));
			dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTORT,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::MOTORT));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR1T,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::MOTOR1T));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::GENET,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::GENET));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::FUELT,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::FUELT));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::WATERT,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::WATERT));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::WATERMT,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::WATERMT));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::BANK1T,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::BANK1T));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::BANK2T,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::BANK2T));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::TRACKID,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::TRACKID));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::ROUTEID,dialog->logGrids[2]->GetCellValue(lastRow-1,LogbookHTML::ROUTEID));
		}
	else
	{
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,_("unnamed Route"));
            if(gpsStatus)
				dialog->logGrids[0]->SetCellValue(lastRow,POSITION,sLat+sLon);
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::FUELT,opt->fuelTank.c_str());
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::WATERT,opt->waterTank.c_str());
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::BANK1T,opt->bank1.c_str());
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::BANK2T,opt->bank2.c_str());
	}

	if(sDate != _T(""))
		{
			dialog->logGrids[0]->SetCellValue(lastRow,RDATE,sDate);
			dialog->logGrids[0]->SetCellValue(lastRow,RTIME,sTime);
		}
		else
		{
            if(!opt->UTC)
                mCorrectedDateTime = wxDateTime::Now();
            else
                mCorrectedDateTime = wxDateTime::Now().ToUTC();
			dialog->logGrids[0]->SetCellValue(lastRow,RDATE,mCorrectedDateTime.Format(opt->sdateformat));
			dialog->logGrids[0]->SetCellValue(lastRow,RTIME,mCorrectedDateTime.Format(opt->stimeformat));
		}

	if(MOBIsActive)
        dialog->logGrids[0]->SetCellValue(lastRow,REMARKS,_("*** MAN OVERBOARD ***\n"));
    else
        dialog->logGrids[0]->SetCellValue(lastRow,REMARKS,sLogText);

    if(routeIsActive)
	{
		if(activeRoute != wxEmptyString)
			dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,activeRoute);
        
		dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::ROUTEID,activeRouteGUID);
	}
    else
        dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::ROUTEID,wxEmptyString);

    if(trackIsActive)
        {
        if(!routeIsActive)
            dialog->logGrids[0]->SetCellValue(lastRow,ROUTE,_("Track ")+activeTrack);

        dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::TRACKID,activeTrackGUID);
        }
    else
        dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::TRACKID,wxEmptyString);

	dialog->logGrids[0]->SetCellValue(lastRow,COG,sCOG);
	dialog->logGrids[0]->SetCellValue(lastRow,COW,sCOW);
	dialog->logGrids[0]->SetCellValue(lastRow,SOG,sSOG);
	dialog->logGrids[0]->SetCellValue(lastRow,SOW,sSOW);
	dialog->logGrids[0]->SetCellValue(lastRow,DEPTH,sDepth);
	dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::WATERTE,sTemperatureWater);
    dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::WIND,sWind);
    dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::WSPD,sWindSpeed);
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR,_T("00.00"));
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR1,_T("00.00"));
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::GENE,_T("00.00"));
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::WATERM,_T("00.00"));
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::WATERMO,_T("0"));
    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_T(" "));
    
    if(wimdaSentence)
    {
        dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::AIRTE,sTemperatureAir);
        dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::BARO,sPressure);
        dialog->logGrids[1]->SetCellValue(lastRow,LogbookHTML::HYDRO,sHumidity);
    }

	if(bRPM1)
    {
        if(	!opt->engine1Running)
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #1 started"));
        else
        {
            if(opt->engineMessageRunning)
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #1 running"));
            if(opt->NMEAUseERRPM || opt->toggleEngine1)
            {
                dtEngine1Off = wxDateTime::Now().Subtract(opt->dtEngine1On);
                opt->dtEngine1On = wxDateTime::Now();
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR,dtEngine1Off.Format(_T("%H:%M")));
                //			wxMessageBox(dtEngine1Off.Format(_T("%H:%M:%S")));
            }
        }
        dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::RPM1,sRPM1);
    }
    if(!bRPM1 && opt->engine1Running)
    {
        if(opt->NMEAUseERRPM || !opt->toggleEngine1)
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR,dtEngine1Off.Format(_T("%H:%M")));
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #1 stopped"));
    }
    if(bRPM2)
    {
        if(	!opt->engine2Running)
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #2 started"));
        else
        {
            if(opt->engineMessageRunning)
            {
                if(dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS).IsEmpty())
                    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #2 running"));
                else
                    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS)+_("\nEngine #2 running"));
            }
                if(opt->NMEAUseERRPM || engine2Manual)
                {
                    dtEngine2Off = wxDateTime::Now().Subtract(opt->dtEngine2On);
                    opt->dtEngine2On = wxDateTime::Now();
                    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR1,dtEngine2Off.Format(_T("%H:%M")));
    //				wxMessageBox(dtEngine2Off.Format(_T("%H:%M:%S")));
                }
            }
        }
        if(!bRPM2 && opt->engine2Running)
        {
            if(opt->NMEAUseERRPM || !opt->toggleEngine2)
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MOTOR1,dtEngine2Off.Format(_T("%H:%M")));
            if(dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS).IsEmpty() || dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS).GetChar(0) == ' ')
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,_("Engine #2 stopped"));
            else
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS)+_("\nEngine #2 stopped"));
        }
    
        wxString sEngine = _T(" ")+opt->rpm+_T(" (")+opt->engine+_T(")");
        wxString sShaft =  _T(" ")+opt->rpm+_T(" (")+opt->shaft+_T(")");

    	if(!sRPM1.IsEmpty())
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::RPM1,sRPM1+sEngine+
                    ((sRPM1Shaft.IsEmpty())? _T("") : _T("\n")+sRPM1Shaft+sShaft));
    	if(!sRPM2.IsEmpty())
            dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::RPM2,sRPM2+sEngine+
                    ((sRPM2Shaft.IsEmpty())? _T("") : _T("\n")+sRPM2Shaft+sShaft));

        if(sailsMessage && opt->engineMessageSails)
        {
            wxString temp = dialog->logGrids[2]->GetCellValue(lastRow,LogbookHTML::MREMARKS);
            if(temp.Len() == 1 && temp.GetChar(0) == ' ') temp.Remove(0,1);

            if((oldSailsState == 0 || oldSailsState == -1) && sailsState == 1)
            {
                dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,temp + ((temp.IsEmpty())?_T(""):_T("\n"))+_("Sails hoisted"));
                oldSailsState = 1;
            }
            else if((oldSailsState == 1 || oldSailsState == -1) && sailsState == 1)
                    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,temp + ((temp.IsEmpty())?_T(""):_T("\n"))+_("Sails changed"));
            else if((oldSailsState == 1 || oldSailsState == -1) && sailsState == 0)
            {
                    dialog->logGrids[2]->SetCellValue(lastRow,LogbookHTML::MREMARKS,temp + ((temp.IsEmpty())?_T(""):_T("\n"))+_("Sails down"));
                    oldSailsState = 0;
            }

            sailsMessage = false;
        }

        if(ActuellWatch::active == true)
            dialog->logGrids[0]->SetCellValue(lastRow,WAKE,ActuellWatch::member);
            
        wxString sails = wxEmptyString;  unsigned int n = 0;
    	for(unsigned i = 0; i < 14; i++)
        {
            if(dialog->checkboxSails[i]->IsChecked())
            {
                sails += opt->sailsName.Item(i);
                if(n == 1)
                {
                    sails += _T("\n");
                    n = 0;
                }
                else
                {
                    sails += _T(", ");
                    n++;
                }
            }
        }

        if(!sails.IsEmpty() && n == 1)
            sails.RemoveLast(2);
    	else if(!sails.IsEmpty())
            sails.RemoveLast(1);

    	dialog->m_gridMotorSails->SetCellValue(lastRow,LogbookHTML::SAILS,sails);

        changeCellValue(lastRow, 0,1);
        setCellAlign(lastRow);
        dialog->setEqualRowHeight(lastRow);

        dialog->m_gridGlobal->SetReadOnly(lastRow,6);
	
        if(mode)
        {
            dialog->m_gridGlobal->MakeCellVisible(lastRow,0);
            dialog->m_gridWeather->MakeCellVisible(lastRow,0);
            dialog->m_gridMotorSails->MakeCellVisible(lastRow,0);
        }
}

void Logbook::resetEngineManuallMode()
{
    bool t = opt->bRPMCheck;
    parent->m_toggleBtnEngine1->SetValue(false);
    opt->toggleEngine1 = false;
    bRPM1 = false;
    dtEngine1Off = wxDateTime::Now().Subtract(opt->dtEngine1On);

    parent->m_toggleBtnEngine2->SetValue(false);
    opt->toggleEngine2 = false;
    bRPM2 = false;
    dtEngine2Off = wxDateTime::Now().Subtract(opt->dtEngine2On);

    appendRow(false);

    opt->dtEngine1On = -1;
    opt->dtEngine2On = -1;

    engine1Manual = false;
    opt->engine1Running = false;
    engine2Manual = false;
    opt->engine2Running = false;
    opt->bRPMCheck = t;

}

void Logbook::checkNMEADeviceIsOn()
{
    wxDateTime dtn = wxDateTime::Now();
    if(bDepth && dtn.Subtract(dtDepth).GetSeconds() > DEVICE_TIMEOUT)						// Sounder
    {
        sDepth = wxEmptyString;
        bDepth = false;
    }
    if(bSOW && dtn.Subtract(dtSOW).GetSeconds() > DEVICE_TIMEOUT)							// Speedo
    {
        sSOW = wxEmptyString;
        bSOW = false;
    }
    if(bWind && dtn.Subtract(dtWind).GetSeconds() > DEVICE_TIMEOUT)							// Wind
    {
        sWind = wxEmptyString;
        sWindSpeed = wxEmptyString;
        bWind = false;
    }
    if(bCOW && dtn.Subtract(dtCOW).GetSeconds() > DEVICE_TIMEOUT)							// Heading
    {
        sCOW = wxEmptyString;
        bCOW = false;
    }
    if(bTemperatureWater && dtn.Subtract(dtTemperatureWater).GetSeconds() > DEVICE_TIMEOUT)  // Watertemperature
    {
        sTemperatureWater = wxEmptyString;
        bTemperatureWater = false;
    }

    if(wimdaSentence && dtn.Subtract(dtWimda).GetSeconds() > DEVICE_TIMEOUT)				// WeatherStation
    {
        sPressure		= wxEmptyString;
        sTemperatureAir = wxEmptyString;
        sHumidity		= wxEmptyString;
        wimdaSentence	= false;
    }
    if(rpmSentence && dtn.Subtract(dtRPM).GetSeconds() > DEVICE_TIMEOUT)					// Engine RPM and Engine elapsed time
    {
        rpmSentence = false;
        wxDateTime now = wxDateTime::Now();

        bRPM1 = false;
        dtEngine1Off = now.Subtract(opt->dtEngine1On);
        opt->dtEngine1On = -1;
        sRPM1 = wxEmptyString;
        sRPM1Shaft = wxEmptyString;

        bRPM2 = false;
        dtEngine2Off = now.Subtract(opt->dtEngine2On);
        opt->dtEngine2On = -1;
        sRPM2 = wxEmptyString;
        sRPM2Shaft = wxEmptyString;

        appendRow(false);
        opt->engine1Running = false;
        opt->engine2Running = false;
    }
}

void Logbook::recalculateLogbook(int row)
{
	int cells[] = { LogbookHTML::POSITION,LogbookHTML::MOTOR,LogbookHTML::MOTOR1,LogbookHTML::FUEL,
                    LogbookHTML::GENE,LogbookHTML::BANK1,LogbookHTML::BANK2,LogbookHTML::WATERM,LogbookHTML::WATER };
	int grid;

	if(row < 0) return;
	if(row == 0  && dialog->m_gridGlobal->GetNumberRows() > 1) row = 1; 

	int len = sizeof(cells)/sizeof(cells[0]);
	for(int i = 0; i < len ; i++)
	{
		if(i == 0)
			grid = 0;
		else
			grid = 2;

		if(dialog->m_gridGlobal->GetNumberRows() >= 2)
			getModifiedCellValue(grid,row,0,cells[i]);
	}

}

void Logbook::checkCourseChanged()
{
	static wxDateTime dt;
	static bool timer = true;

	wxDouble cog;
	wxGrid* grid = dialog->m_gridGlobal;

	wxString temp = grid->GetCellValue(grid->GetNumberRows()-1,8);
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&cog);

	if((cog == dCOG) || (oldLogbook || temp.IsEmpty())) return;


#ifdef __WXOSX__
	wxDouble result = labs(cog-dCOG); 
#else
	wxDouble result = abs(cog-dCOG); 
#endif
	if(result > 180) result -= 360;

#ifdef __WXOSX__
	if(labs(result) >= opt->dCourseChangeDegrees && !dialog->logbookPlugIn->eventsEnabled)
#else
	if(abs(result) >= opt->dCourseChangeDegrees && !dialog->logbookPlugIn->eventsEnabled)
#endif
		{
			if(timer)
			{
				timer = false;
				dt = mCorrectedDateTime;
				long min;
				opt->courseTextAfterMinutes.ToLong(&min);
				wxTimeSpan t(0,(int)min);
				dt.Add(t);
			}
			

			if(mCorrectedDateTime >= dt)
			{
				dialog->logbookTimerWindow->popUp();
				timer = true;
				courseChange = true;
				appendRow(false);
				courseChange = false;
			}
		}
}

void Logbook::checkWayPoint(RMB rmb)
{
	if(lastWayPoint == rmb.From) return;

	dialog->logbookTimerWindow->popUp();
	tempRMB = rmb;
	waypointArrived = true;
	appendRow(false);
	waypointArrived = false;
	lastWayPoint = rmb.From;
}

void Logbook::checkGuardChanged()
{
	if(dLastMinute == -1)
	{ dLastMinute = (long) mCorrectedDateTime.GetMinute()+1; return; }

	long hour,minute;
	long m_minute = (long) mCorrectedDateTime.GetMinute();
	long m_hour = (long) mCorrectedDateTime.GetHour();
	bool append = false;

	if(m_minute >= dLastMinute)
	{
		for(int row = 0; row < dialog->m_gridCrewWake->GetNumberRows(); row++)
		{
			for(int col = 2; col < dialog->m_gridCrewWake->GetNumberCols(); col += 2)
			{
				wxString s = dialog->m_gridCrewWake->GetCellValue(row,col);
				if(s.IsEmpty()) continue;
				wxStringTokenizer tkz(s,_T(":"));
				tkz.GetNextToken().ToLong(&hour);
				tkz.GetNextToken().ToLong(&minute);
				if(hour != m_hour) continue;
				if(minute == m_minute)
					append = true;
			}
		}
		if(append) 
		{ 
			guardChange = true;
			appendRow(false);
			guardChange = false;
		}
		dLastMinute = m_minute + 1;
	}
}

void Logbook::checkDistance()
{
	if(oldPosition.latitude == 500)
		oldPosition = newPosition;

	double fromlat = oldPosition.posLat * PI/180;
	double fromlon = oldPosition.posLon * PI/180;
	double tolat = newPosition.posLat * PI/180;
	double tolon = newPosition.posLon * PI/180;
	if(oldPosition.NSflag == 'S') fromlat = -fromlat;
	if(oldPosition.WEflag == 'W') fromlon = -fromlon;
	if(newPosition.NSflag == 'S') tolat = -fromlat;
	if(newPosition.WEflag == 'W') tolon = -fromlon;

	double sm = 
		acos(cos(fromlat)*cos(fromlon)*cos(tolat)*cos(tolon) + 
		  cos(fromlat)*sin(fromlon)*cos(tolat)*sin(tolon) + sin(fromlat)*sin(tolat)) * 3443.9;

	if(sm >= opt->dEverySM && !dialog->logbookPlugIn->eventsEnabled)
	{
		dialog->logbookTimerWindow->popUp();
		everySM = true;	
		appendRow(false);
		everySM = false;
		oldPosition = newPosition;
	}
}

wxString Logbook::calculateDistance(wxString fromstr, wxString tostr)
{
	if((fromstr.IsEmpty() || tostr.IsEmpty()) || fromstr == tostr) 
		return wxString(_T("0.00 ")+opt->distance);

	wxString sLat, sLon, sLatto, sLonto;
	wxDouble fromlat,fromlon,tolat,tolon, sm;

	wxStringTokenizer tkz(fromstr, _T("\n"));
	sLat = tkz.GetNextToken();
	sLon = tkz.GetNextToken();
	wxStringTokenizer tkzto(tostr, _T("\n"));
	sLatto = tkzto.GetNextToken();
	sLonto = tkzto.GetNextToken();

	if(opt->traditional)
	{
		fromlat = positionStringToDezimal(sLat)* (PI/180);
		fromlon = positionStringToDezimal(sLon)* (PI/180);
		tolat = positionStringToDezimal(sLatto)* (PI/180);
		tolon = positionStringToDezimal(sLonto)* (PI/180);
	}
	else
	{
		fromlat = positionStringToDezimalModern(sLat)* (PI/180);
		fromlon = positionStringToDezimalModern(sLon)* (PI/180);
		tolat = positionStringToDezimalModern(sLatto)* (PI/180);
		tolon = positionStringToDezimalModern(sLonto)* (PI/180);
	}
	if(oldPosition.NSflag == 'S') fromlat = -fromlat;
	if(oldPosition.WEflag == 'W') fromlon = -fromlon;
	if(newPosition.NSflag == 'S') tolat = -fromlat;
	if(newPosition.WEflag == 'W') tolon = -fromlon;

///////
sm = acos(cos(fromlat)*cos(fromlon)*cos(tolat)*cos(tolon) + 
		  cos(fromlat)*sin(fromlon)*cos(tolat)*sin(tolon) + sin(fromlat)*sin(tolat)) * 3443.9;
////// code snippet from http://www2.nau.edu/~cvm/latlongdist.html#formats

	wxString ret = wxString::Format(_T("%.2f %s"),sm,opt->distance.c_str());
	ret.Replace(_T("."),dialog->decimalPoint);
	return ret;
}

wxDouble Logbook::positionStringToDezimal(wxString pos)
{
	wxDouble resdeg, resmin, ressec = 0;
	wxString temp = pos;

	wxStringTokenizer tkz(pos, _T(" "));
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resmin);
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&ressec);
	if(pos.Contains(_T("S"))) ressec = -ressec;
	if(pos.Contains(_T("W"))) ressec = -ressec;
	resmin = (resmin/60 + ressec/3600);

	return resdeg + resmin;
}

wxDouble Logbook::positionStringToDezimalModern(wxString pos)
{
	wxDouble resdeg, resmin;
	wxString temp = pos;

	wxStringTokenizer tkz(pos, _T(" "));
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resdeg);
	if(pos.Contains(_T("S"))) resdeg = -resdeg;
	if(pos.Contains(_T("W"))) resdeg = -resdeg;
	temp = tkz.GetNextToken();
	temp.Replace(_T(","),_T("."));
	temp.ToDouble(&resmin);
	if(pos.Contains(_T("S"))) resmin = -resmin;
	if(pos.Contains(_T("W"))) resmin = -resmin;

	return resdeg + (resmin/60);
}

void Logbook::deleteRow(int row)
{
	dialog->logGrids[dialog->m_notebook8->GetSelection()]->SelectRow(row,true);
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL, dialog);
	if (answer == wxYES)
		deleteRows();
	modified = true;
}

void Logbook::changeCellValue(int row, int col, int mode)
{
	if(mode)
		for(int g =0; g < LOGGRIDS; g++)
			for(int i = 0; i < dialog->logGrids[g]->GetNumberCols(); i++)
				getModifiedCellValue(g,row,i,i);
	else
		getModifiedCellValue(dialog->m_notebook8->GetSelection(),row,col,col);
}

void Logbook::update()
{
	if(!modified) return;
	modified = false;

	dialog->logGrids[0]->Refresh();
	dialog->logGrids[1]->Refresh();
	dialog->logGrids[2]->Refresh();
	
	int count;
	if((count  = dialog->logGrids[0]->GetNumberRows() )== 0) { wxFile f; f.Create(data_locn,true); return; }
	
	wxString s = _T(""), temp;

	wxString newLocn = data_locn;
	newLocn.Replace(_T("txt"),_T("Bak"));
	wxRename(data_locn,newLocn);

	wxFileOutputStream output( data_locn );
	wxTextOutputStream* stream = new wxTextOutputStream (output,wxEOL_NATIVE,wxConvUTF8);

	stream->WriteString(_T("#1.2#\t")+logbookDescription+_T("\n"));
	for(int r = 0; r < count; r++)
	{
		for(int g = 0; g < LOGGRIDS; g++)
		{
			for(int c = 0; c < dialog->logGrids[g]->GetNumberCols(); c++)
			{
				if(g == 1 && (c == LogbookHTML::HYDRO || c == LogbookHTML::WATERTE || c == LogbookHTML::AIRTE))
					continue;
				if(g == 2 && (c == LogbookHTML::MOTOR1  || c == LogbookHTML::MOTOR1T ||
                              c == LogbookHTML::RPM1	|| c == LogbookHTML::RPM2	 ||
                              c == LogbookHTML::GENE    || c == LogbookHTML::GENET   ||
                              c == LogbookHTML::WATERM  || c == LogbookHTML::WATERMT ||
                              c == LogbookHTML::WATERMO || c == LogbookHTML::BANK1   ||
                              c == LogbookHTML::BANK1T  || c == LogbookHTML::BANK2   ||
                              c == LogbookHTML::BANK2T  ||
                              c == LogbookHTML::TRACKID || c == LogbookHTML::ROUTEID))
					continue;
				if(g == 0 && c == RDATE)
				{
					wxString t = dialog->logGrids[g]->GetCellValue(r,c);
					wxDateTime dt;
					dialog->myParseDate(t,dt);
					temp = wxString::Format(_T("%i \t%i \t%i"),dt.GetMonth(),dt.GetDay(),dt.GetYear());
				}
				else if(g == 0 && c == RTIME)
				{
					wxString t = dialog->logGrids[g]->GetCellValue(r,c);
					wxDateTime dt;
					dialog->myParseTime(t,dt);
					temp = wxString::Format(_T("%i \t%i \t%i"),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
				}
				else
					temp = dialog->logGrids[g]->GetCellValue(r,c);

				s += dialog->replaceDangerChar(temp);
				s += _T(" \t");
			}
		}

		for(int ext = LogbookHTML::HYDRO; ext != LogbookHTML::WIND; ext ++) // extended 3 columns in weathergrid
		{
			temp = dialog->logGrids[1]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = LogbookHTML::MOTOR1; ext <= LogbookHTML::MOTOR1T; ext ++) // extend MOTOR #1
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = LogbookHTML::GENE; ext <= LogbookHTML::BANK2T; ext ++) // extend for GENERATOR and Battery-Banks
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

		for(int ext = LogbookHTML::WATERM; ext <= LogbookHTML::WATERMO; ext ++) // extend WATERMAKER
		{
			temp = dialog->logGrids[2]->GetCellValue(r,ext);
			s += dialog->replaceDangerChar(temp);
			s += _T(" \t");
		}

        for(int ext = LogbookHTML::ROUTEID; ext < parent->m_gridMotorSails->GetNumberCols(); ext ++) // extend GUID's
        {
            temp = dialog->logGrids[2]->GetCellValue(r,ext);
            s += dialog->replaceDangerChar(temp);
            s += _T(" \t");
        }

        for(int ext = LogbookHTML::RPM1; ext < LogbookHTML::MOTOR1; ext ++) // extend RPM #1
        {
            temp = dialog->logGrids[2]->GetCellValue(r,ext);
            s += dialog->replaceDangerChar(temp);
            s += _T(" \t");
        }

        for(int ext = LogbookHTML::RPM2; ext < LogbookHTML::FUEL; ext ++) // extend RPM #2
        {
            temp = dialog->logGrids[2]->GetCellValue(r,ext);
            s += dialog->replaceDangerChar(temp);
            s += _T(" \t");
        }

		s.RemoveLast();
		s += _T("\n");
		stream->WriteString(s);
		s = _T("");
	}
	output.Close();
}

void  Logbook::getModifiedCellValue(int grid, int row, int selCol, int col)
{
	wxString s, wind, depth;

	modified = true;

	s = dialog->logGrids[grid]->GetCellValue(row,col);

	if((grid == 0 && (col == WAKE || col == REMARKS)) ||
       (grid == 1 && (col == LogbookHTML::WEATHER || col == LogbookHTML::CLOUDS || col == LogbookHTML::VISIBILITY)) ||
       (grid == 2 && (col == LogbookHTML::SAILS || col == LogbookHTML::REEF || col == LogbookHTML::MREMARKS)))
	{
		return;
	}

	if(grid == 0 && col == ROUTE )
	{
        if(s.Last() == '\n')
        {
            s.RemoveLast();
            dialog->logGrids[grid]->SetCellValue(row,col,s);
        }
    }
    else if(grid == 0 && col == RDATE )
					{
						wxDateTime dt;

						if(!dialog->myParseDate(s,dt))
						{
							dt = dt.Now();
#ifdef __WXOSX__
                            wxString s = dt.FormatDate().c_str();
                            MessageBoxOSX(NULL,wxString::Format(_("Please enter the Date in the format:\n      %s"),dt.Format(opt->sdateformat).c_str()), _("Information"),wxID_OK);                     
#else
							wxMessageBox(wxString::Format(_("Please enter the Date in the format:\n      %s"),dt.Format(opt->sdateformat).c_str()),_("Information"));
#endif
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.Format(opt->sdateformat));

							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == RTIME)
					{
						wxDateTime dt;
						const wxChar* c;
						s.Replace(_T(","),_T(":"));
						s.Replace(_T("."),_T(":"));
						c = dialog->myParseTime(s,dt);

						if(!c)
						{
#ifdef __WXOSX__
                            MessageBoxOSX(NULL,wxString::Format(_("Please enter the Time in the format:\n   %s"),dt.Format(opt->stimeformat).c_str()),_("Information"),wxID_OK);
#else
							wxMessageBox(wxString::Format(_("Please enter the Time in the format:\n   %s"),dt.Format(opt->stimeformat).c_str()));
#endif
							dialog->logGrids[grid]->SetCellValue(row,col,_T(""));
						}
						else
						{
							dialog->logGrids[grid]->SetCellValue(row,col,dt.Format(opt->stimeformat));
							if(row == dialog->m_gridGlobal->GetNumberRows()-1)
								dialog->maintenance->checkService(row);
						}
					}
	else if(grid == 0 && col == DISTANCE)
					{	
						s.Replace(_T(","),_T("."));

						s = wxString::Format(_T("%.2f %s"),wxAtof(s),opt->distance.c_str());
										
						s.Replace(_T("."),dialog->decimalPoint);
						dialog->logGrids[grid]->SetCellValue(row,col,s);

						computeCell(grid, row, col, s, true);
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
						
						s.Replace(_T(","),_T("."));
						if(wxAtof(s) >= 0.01) 
							dialog->m_gridGlobal->SetCellValue(row,SIGN,_T("S"));
						else
							dialog->m_gridGlobal->SetCellValue(row,SIGN,_T(""));
					}
	
	else if(grid == 0 && col== SIGN)
					{
						dialog->logGrids[grid]->SetCellValue(row,col,s.Upper());
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
							dialog->maintenance->checkService(row);
					}
	else if(grid == 0 && col== POSITION)
					{ 
						if(s != _T("") && !s.Contains(opt->Deg)
							&& !s.Contains(opt->Min)
							&& !s.Contains(opt->Sec))
						{
							if(opt->traditional && s.length() != 22)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min 12.15Sec N 030Deg 12Min 05.15Sec E"),_("Information"),wxID_OK);
#else
								wxMessageBox(_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min 12.15Sec N 030Deg 12Min 05.15Sec E"),_("Information"),wxOK);
#endif
								s = _T("");
							}
							else if(!opt->traditional && s.length() != 22)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Please enter 05445.1234n03012.0504e for\n054Deg 45.1234Min N 030Deg 12.0504Min E"),_("Information"),wxID_OK);
#else
								wxMessageBox(_("Please enter 05445.1234n03012.0504e for\n054Deg 45.1234Min N 030Deg 12.0504Min E"),_("Information"),wxOK);
#endif
								s = _T("");
							}
							if(s == _T("")) return;
							s.Replace(_T(","),_T("."));

							if(opt->traditional)
							{
								wxString temp = s.SubString(0,2)+opt->Deg+_T(" ");
								temp += s.SubString(3,4) + opt->Min+_T(" ");
								temp += s.SubString(5,9) + opt->Sec+_T(" ");
								temp += s.SubString(10,10).Upper() + _T("\n");
								temp += s.SubString(11,13) + opt->Deg+_T(" ");
								temp += s.SubString(14,15) + opt->Min+_T(" ");
								temp += s.SubString(16,20) + opt->Sec+_T(" ");
								temp += s.SubString(21,21).Upper();
								s = temp;
							}
							else
							{
								wxString temp = s.SubString(0,2)+opt->Deg+_T(" ");
								temp += s.SubString(3,9) + opt->Min+_T(" ");
								temp += s.SubString(10,10).Upper() + _T("\n");
								temp += s.SubString(11,13) + opt->Deg+_T(" ");
								temp += s.SubString(14,20) + opt->Min+_T(" ");
								temp += s.SubString(21,22).Upper();
								s = temp;
							}
						}
						s.Replace(_T("."),dialog->decimalPoint);
						dialog->logGrids[grid]->SetCellValue(row,col,s);
						if(row != 0)
						{
							for(int i = row; i < dialog->logGrids[grid]->GetNumberRows(); i++)
							{
								double distTotal,dist;
								dialog->logGrids[grid]->SetCellValue(i,5,
									calculateDistance(dialog->logGrids[grid]->GetCellValue(i-1,col),s));
								wxString temp = dialog->logGrids[grid]->GetCellValue(i-1,6);
								temp.Replace(_T(","),_T("."));
								temp.ToDouble(&distTotal);
								temp = dialog->logGrids[grid]->GetCellValue(i,5);
								temp.Replace(_T(","),_T("."));
								temp.ToDouble(&dist);
								s= wxString::Format(_T("%9.2f %s"),distTotal+dist,opt->distance.c_str());
								s.Replace(_T("."),dialog->decimalPoint);
								dialog->logGrids[grid]->SetCellValue(i,6,s);

								if(dist >= 0.1)
									dialog->m_gridGlobal->SetCellValue(i,3,_T("S"));
								else
									dialog->m_gridGlobal->SetCellValue(i,3,dialog->m_gridGlobal->GetCellValue(i-1,3));

								if(i < dialog->m_gridGlobal->GetNumberRows()-1)
								{
									s = dialog->logGrids[grid]->GetCellValue(i+1,col);
									if(s.IsEmpty() || s == _T(" "))
										break;
								}
							}							
						}
					}
	else if(grid == 0 && col == COG)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s"),wxAtof(s),opt->Deg.c_str());
						    s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == COW)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f%s %s"),wxAtof(s),opt->Deg.c_str(),(opt->showHeading)?_T("M"):_T("T"));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && (col == SOG || col == SOW))
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
#ifdef __WXOSX__
                            s = wxString::Format(_T("%2.2f %s"),wxAtof(s),(const wchar_t*)opt->speed.c_str());
#else
							s = wxString::Format(_T("%2.2f %s"),wxAtof(s),opt->speed.c_str());
#endif
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 0 && col == DEPTH)
					{
						if(s != _T(""))
						{
							switch(opt->showDepth)
							{
								case 0:	depth = opt->meter; break;
								case 1: depth = opt->feet; break;
								case 2: depth = opt->fathom; break;
							}
							if(s.Contains(opt->meter) || 
								s.Contains(opt->feet) || 
								s.Contains(opt->fathom.c_str()) ||
								s.Contains(_T("--")))
							{
								s.Replace(_T("."),dialog->decimalPoint);
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
							else
							{
								s.Replace(_T(","),_T("."));
								s = wxString::Format(_T("%3.1f %s"),wxAtof(s),depth.c_str());
								s.Replace(_T("."),dialog->decimalPoint);
								dialog->logGrids[grid]->SetCellValue(row,col,s);
							}
					
						}
					}
	else if(grid == 1 && col == LogbookHTML::BARO)
					{
						if(s != _T(""))
						{
							s = wxString::Format(_T("%u %s"),wxAtoi(s),opt->baro.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::HYDRO)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%4.1f%%"),wxAtof(s));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::AIRTE)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f %s %s"),wxAtof(s), opt->Deg.c_str(),opt->temperature.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::WATERTE)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f %s %s"),wxAtof(s), opt->Deg.c_str(),opt->temperature.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::WIND)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s = wxString::Format(_T("%3.0f%s %s"),wxAtof(s), opt->Deg.c_str(),opt->showWindDir?_T("R"):_T("T"));
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::WSPD )
					{
						if(s != _T(""))
						{
							if(!s.Contains(opt->windkts) && !s.Contains(opt->windmeter) && !s.Contains(opt->windkmh))
 							{
                                switch(opt->showWindSpeed)
                                {
                                case 0:	wind = opt->windkts; break;
								case 1: wind = opt->windmeter; break;
								case 2: wind = opt->windkmh; break;
                                }
                                s.Replace(_T(","),_T("."));
                                s = wxString::Format(_T("%3.2f %s"),wxAtof(s),wind.c_str());
                                s.Replace(_T("."),dialog->decimalPoint);
                            }
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && col == LogbookHTML::CURRENT)
					{
						if(s != _T("") && !s.Contains(opt->Deg))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.0f%s"),wxAtof(s), opt->Deg.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);						
						}
					}				
	else if(grid == 1 && col == LogbookHTML::CSPD)
					{
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f %s"),wxAtof(s),opt->speed.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 1 && (col == LogbookHTML::WAVE || col == LogbookHTML::SWELL))
					{
						wxString d;
						switch(opt->showWaveSwell)
							{
								case 0:	d = opt->meter; break;
								case 1: d = opt->feet; break;
								case 2: d = opt->fathom; break;
							}
						if(s != _T(""))
						{
							s.Replace(_T(","),_T("."));
							s = wxString::Format(_T("%3.2f %s"),wxAtof(s),d.c_str());
							s.Replace(_T("."),dialog->decimalPoint);
							dialog->logGrids[grid]->SetCellValue(row,col,s);
						}
					}
	else if(grid == 2 && ((col == LogbookHTML::MOTORT || col == LogbookHTML::MOTOR1T ||
                           col == LogbookHTML::GENET  || col == LogbookHTML::WATERMT )  && !s.IsEmpty()))
					{
						wxString pre, cur;
						double hp,hc,mp,mc;
						double res,hp_,hc_;

						if(!s.Contains(_T(":")) && !s.Contains(_T(",")) && !s.Contains(_T("."))) 
							s += _T(":");

						if(s.Contains(_T(",")) || s.Contains(_T(".")))
						{
							double d;
							s.Replace(_T(","),_T("."));
							s.ToDouble(&d);
							int h = (int) d;
							int m = (60*(d - h));
							s = wxString::Format(_T("%i:%i"),h,m);

						}

						if(row > 0)
						{
							pre = dialog->m_gridMotorSails->GetCellValue(row-1,col);
							wxStringTokenizer tkz(pre,_T(":"));
							tkz.GetNextToken().ToDouble(&hp);
							tkz.GetNextToken().ToDouble(&mp);
 
						}
						else
						{ hp = 0; mp = 0; }

						cur = s;
						wxStringTokenizer tkz1(cur,_T(":"));
						tkz1.GetNextToken().ToDouble(&hc);
						tkz1.GetNextToken().ToDouble(&mc);

						hc_ = hc + ((mc*(100.0/60.0))/100);
						hp_ = hp + ((mp*(100.0/60.0))/100);

						res = hc_ - hp_;

						if(row == 0 || res <= 0.0)
							dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("00:00 %s"),opt->motorh.c_str()));
						else
							dialog->m_gridMotorSails->SetCellValue(row,col-1,decimalToHours(res,false));

						dialog->m_gridMotorSails->SetCellValue(row,col,  decimalToHours(hc_,true));

						if(row < dialog->m_gridMotorSails->GetNumberRows()-1)
							computeCell(grid,row+1,col-1,dialog->m_gridMotorSails->GetCellValue(row+1,col-1),true);
						if(row == dialog->m_gridGlobal->GetNumberRows()-1)
									dialog->maintenance->checkService(row);
					}
	else if(grid == 2 && ((col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
                           col == LogbookHTML::GENE  || col == LogbookHTML::WATERM)  && !s.IsEmpty()))
					{
						double watermaker;
						opt->watermaker.ToDouble(&watermaker);

						bool t = false;
						wxString sep;

						if(s.Contains(_T(".")))
						{
							t = true;
							sep = _T(".");
						}
						if(s.Contains(_T(",")))
						{
							t = true;
							sep = _T(",");
						}
						if(s.Contains(_T(":")))
						{
							t = true;
							sep = _T(":");
						}

						if(true != t)
							s.Append(_T(":0"));

						wxStringTokenizer tkz(s,sep);
						wxString h = tkz.GetNextToken();
						wxString m = tkz.GetNextToken();
						if(wxAtoi(m) > 59)
							{
#ifdef __WXOSX__
                                MessageBoxOSX(NULL,_("Minutes greater than 59"),_T(""),wxID_OK);
#else
								wxMessageBox(_("Minutes greater than 59"),_T(""));
#endif
								dialog->logGrids[grid]->SetCellValue(row,col,_T("00:00"));
								return;
							}
						else
							{
								s = wxString::Format(_T("%s:%s"),h.c_str(),m.c_str());
								s = wxString::Format(_T("%s %s"),s.c_str(),opt->motorh.c_str());
								dialog->logGrids[grid]->SetCellValue(row,col,s);
								computeCell(grid, row, col,s, true);
								if(row == dialog->m_gridGlobal->GetNumberRows()-1)
									dialog->maintenance->checkService(row);
								if(col == LogbookHTML::WATERM)
								{
									wxString t = dialog->m_gridMotorSails->GetCellValue(row,LogbookHTML::WATERM);
									wxStringTokenizer tkz(t,_T(":"));
									double h,m;
									tkz.GetNextToken().ToDouble(&h);
									tkz.GetNextToken().ToDouble(&m);
									h = h + (m*(100/60)/100);
									double output = watermaker * h;
									dialog->m_gridMotorSails->SetCellValue(row,LogbookHTML::WATERMO,wxString::Format(_T("+%2.2f %s"),output,opt->vol.c_str()));
                                    computeCell(grid,row,LogbookHTML::WATERMO, dialog->m_gridMotorSails->GetCellValue(row,LogbookHTML::WATERMO),false);
								}
							}						
					}

	else if(grid == 2 && ( col == LogbookHTML::FUELT || col == LogbookHTML::WATERT ||
                            col == LogbookHTML::BANK1T || col == LogbookHTML::BANK2T) && !s.IsEmpty())
					{
						double div = 1.0;
						long capacity;
						wxString ap;
						double t,c;
						wxString ind;

						if(col == LogbookHTML::BANK1T || col == LogbookHTML::BANK2T)
							ap = opt->ampereh;
						else
							ap = opt->vol;

						if(row > 0)
							dialog->m_gridMotorSails->GetCellValue(row-1,col).ToDouble(&t);						
						else
							{ t = 0; c = 0; }
						dialog->m_gridMotorSails->GetCellValue(row,col).ToDouble(&c);

						if(s.Contains(_T("/")))
						{
							double a,b;
							wxStringTokenizer tkz(s,_T("/"));
							tkz.GetNextToken().ToDouble(&a);
							tkz.GetNextToken().ToDouble(&b);
							div = a/b;
							if(col == LogbookHTML::FUELT) 
								opt->fuelTank.ToLong(&capacity);
							else if( col == LogbookHTML::BANK1T)
								opt->bank1.ToLong(&capacity);
							else if(col == LogbookHTML::BANK2T)
								opt->bank2.ToLong(&capacity);
							else
								opt->waterTank.ToLong(&capacity);

							c = capacity*div;						
						}

						s.Replace(_T(","),_T("."));
						ind = (c < t)?_T("-"):_T("+");

						if(row != 0)
								dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("%s%.2f %s"),ind.c_str(),fabs(t-c),ap.c_str()));
						else
								dialog->m_gridMotorSails->SetCellValue(row,col-1,wxString::Format(_T("%s0.00 %s"),ind.c_str(),ap.c_str()));

							dialog->m_gridMotorSails->SetCellValue(row,col,wxString::Format(_T("%.2f %s"),c,ap.c_str()));
						
						int x;
						if(col == LogbookHTML::WATERT)
							x = 2;
						else
							x =1;
						if(row < dialog->m_gridMotorSails->GetNumberRows()-1)
							computeCell(grid,row+1,col-x,dialog->m_gridMotorSails->GetCellValue(row+1,col-x),false);

						dialog->maintenance->checkService(row);
					}
	else if(grid == 2 && ( col == LogbookHTML::FUEL    || col == LogbookHTML::WATER ||
                            col == LogbookHTML::WATERMO || col == LogbookHTML::BANK1 || col == LogbookHTML::BANK2) && !s.IsEmpty())
					{
						wxChar ch;
						wxString ap;

						if(col == LogbookHTML::BANK1 || col == LogbookHTML::BANK2)
							ap = opt->ampereh;
						else
							ap = opt->vol;

						s.Replace(_T(","),_T("."));
						if(col != LogbookHTML::WATERMO)
							ch = s.GetChar(0);
						else 
							ch = '+';

						s = wxString::Format(_T("%.2f %s"),wxAtof(s),ap.c_str());
						s.Replace(_T("."),dialog->decimalPoint);
						
						if(ch != '-' && ch != '+')
							dialog->logGrids[grid]->SetCellValue(row,col,_T("-")+s);
						else
						{
							if(ch == '+')
								dialog->logGrids[grid]->SetCellValue(row,col,wxString(ch)+s);
							else
								dialog->logGrids[grid]->SetCellValue(row,col,s);
						}

						computeCell(grid, row, col,s, false);
						dialog->maintenance->checkService(row);
					}
	return;
}

void Logbook::deleteRows()
{
	wxArrayInt rows;
	unsigned int rowsCount;
	int tab = dialog->m_notebook8->GetSelection();

	rows = dialog->logGrids[tab]->GetSelectedRows();
	rowsCount = rows.GetCount();
	if(rowsCount == 0) // complete grid
	{
		wxGridCellCoordsArray art = dialog->logGrids[tab]->GetSelectionBlockTopLeft();
		wxGridCellCoordsArray arb = dialog->logGrids[tab]->GetSelectionBlockBottomRight();
		int start = art[0].GetRow();
		int end = arb[0].GetRow();
		for(int grid = 0; grid < LOGGRIDS; grid++)
		{
			dialog->logGrids[grid]->DeleteRows(start,(end-start)+1);
			dialog->logGrids[grid]->ForceRefresh();
		}

		if(start == dialog->m_gridGlobal->GetNumberRows()-1)
			start--;

		if(dialog->logGrids[tab]->GetNumberRows() != 0)
		{
			dialog->selGridRow = start;
			dialog->logGrids[tab]->SetGridCursor(start,0);
			recalculateLogbook(start);
		}
		else
		{
			dialog->selGridRow = 0;
		}
		modified = true;
		return;
	}

	bool sort = true;
	if(rowsCount > 1)
	{
		while(sort)
		{
			sort = false;
			for(unsigned int i = 0; i < rowsCount-1; i++)
			{
				if(rows[i+1] > rows[i])
				{
					sort = true;
					int temp = rows[i];
					rows[i] = rows[i+1];
					rows[i+1] = temp;
				}
			}
		}
	}


	for(int grid = 0; grid < LOGGRIDS; grid++)
	{
		for(unsigned int i = 0; i < rowsCount; i++)
			dialog->logGrids[grid]->DeleteRows(rows[i]);
	}
	dialog->selGridRow = rows[rowsCount-1]-1;
    if(dialog->logGrids[tab]->GetNumberRows() > 0)
        dialog->logGrids[tab]->SetGridCursor(rows[rowsCount-1]-1,0);

	modified = true;
	if(dialog->logGrids[0]->GetNumberRows() > 0)
		recalculateLogbook(rows[rows.GetCount()-1]-1);
}

wxString  Logbook::decimalToHours(double res,bool b)
{
	int h = (int) res;
	double m = res - h;
	m = m * (60.0/100.0)*100;

	wxString fmt = (b)?_T("%05i:%02.0f %s"):_T("%02i:%02.0f %s");
	wxString str =  wxString::Format(fmt,h,m,opt->motorh.c_str());
	return str;
}

wxString Logbook::computeCell(int grid, int row, int col, wxString s, bool mode)
{
	double current = 0, last = 0.0;
	long hourCur=0, minCur=0 ,hourLast=0, minLast = 0;
	int count;
	wxString cur;
	wxString abrev;

	s.Replace(_T(","),_T("."));

	if(col == DISTANCE)
	   abrev = opt->distance; 
	else if(col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
            col == LogbookHTML::GENE || col == LogbookHTML::WATERM)
	   abrev = opt->motorh; 
	else if(col == LogbookHTML::FUEL || col == LogbookHTML::WATER ||
            col == LogbookHTML::WATERMO)
	   abrev = opt->vol; 
	else if(col == LogbookHTML::BANK1 || col == LogbookHTML::BANK2)
		abrev = opt->ampereh;


	count = dialog->m_gridGlobal->GetNumberRows();

	for(int i = row; i < count; i++)
	{
		if(col != LogbookHTML::WATERMO && col != LogbookHTML::WATER && col != LogbookHTML::FUEL &&
            col != LogbookHTML::BANK1   && col != LogbookHTML::BANK2)
		{
			s = dialog->logGrids[grid]->GetCellValue(i,col);
			s.Replace(_T(","),_T("."));
			if(s == _T("0000")) s = _T("00:00");
			if(grid == 2 && (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
                             col == LogbookHTML::GENE  || col == LogbookHTML::WATERM))
			{
				wxArrayString time = wxStringTokenize(s,_T(":"));
				time[0].ToLong(&hourCur);
				time[1].ToLong(&minCur);
			}
			else
			{
				s.ToDouble(&current);
			}
		}
		else
		{
			double t,t1 = 0.0 ,t2 = 0.0;

			s = dialog->logGrids[grid]->GetCellValue(i,col);
			s.Replace(_T(","),_T("."));
			s.ToDouble(&t);

			if(col == LogbookHTML::WATERMO)
			{
				s = dialog->logGrids[grid]->GetCellValue(i,LogbookHTML::WATER);
				s.Replace(_T(","),_T("."));
				s.ToDouble(&t1);

				if(i == 0)
				{
					s = dialog->logGrids[grid]->GetCellValue(i,LogbookHTML::WATERT);
					s.Replace(_T(","),_T("."));
					s.ToDouble(&t2);
				}
				current = t + t1 + t2;
			}
			else if(col == LogbookHTML::WATER)
			{
				s = dialog->logGrids[grid]->GetCellValue(i,LogbookHTML::WATERMO);
				s.Replace(_T(","),_T("."));
				s.ToDouble(&t1);

				if(i == 0)
				{
					s = dialog->logGrids[grid]->GetCellValue(i,LogbookHTML::WATERT);
					s.Replace(_T(","),_T("."));
					s.ToDouble(&t2);

					current = t + t2;
				}
				else
				{ current = t + t1 + t2; }

			}
			else
			{
				current = t + t1;
			}
		}

		if(i > 0)
		{
			wxString temp;
			if(col != LogbookHTML::WATERMO)
				 temp = dialog->logGrids[grid]->GetCellValue(i-1,col+1);
			else
				 temp = dialog->logGrids[grid]->GetCellValue(i-1,col+2);

			temp.Replace(_T(","),_T("."));
			if(grid == 2 && (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
                             col == LogbookHTML::GENE  || col == LogbookHTML::WATERM))
			{
				if(temp.Contains(_T(":")))
				{
					wxArrayString time = wxStringTokenize(temp,_T(":"));
					time[0].ToLong(&hourLast);
					time[1].ToLong(&minLast);
				}
				else { hourLast = 0; minLast = 0; }
			}
			else
				temp.ToDouble(&last);
		}
		else { 
				last = 0.0f; hourLast = 0; minLast = 0; 			
			 }
		
		if(grid == 2 && (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
                         col == LogbookHTML::GENE  || col == LogbookHTML::WATERM))
			{
				hourLast += hourCur;
				minLast  += minCur;
				if(minLast >= 60) { hourLast++; minLast -= 60; }
#ifdef __WXOSX__
                s = wxString::Format(_T("%05u:%02u %s"),(wchar_t)hourLast,(wchar_t)minLast,abrev.c_str());
#else
				s = wxString::Format(_T("%05u:%02u %s"),hourLast,minLast,abrev.c_str());
#endif
				dialog->logGrids[grid]->SetCellValue(i,col+1,s);
#ifdef __WXOSX__
                cur = wxString::Format(_T("%02u:%02u %s"),(wchar_t)hourCur,(wchar_t)minCur,abrev.c_str());
#else
				cur = wxString::Format(_T("%02u:%02u %s"),hourCur,minCur,abrev.c_str());
#endif
				dialog->logGrids[grid]->SetCellValue(i,col,cur);
			}
		else
			{
#ifdef __WXOSX__
                s = wxString::Format(_T("%10.2f %s"),last+current,abrev.c_str());
#else
				s = wxString::Format(_T("%10.2f %s"),last+current,abrev.c_str());
#endif
				s.Replace(_T("."),dialog->decimalPoint);
				if(col != LogbookHTML::WATERMO)
					dialog->logGrids[grid]->SetCellValue(i,col+1,s);
				else
					dialog->logGrids[grid]->SetCellValue(i,col+2,s);
			}	
	}
	return cur;
}

wxString Logbook::toSDMM ( int NEflag, double a, bool mode )
{
      short neg = 0;
      int d;
      long m;
	  wxDouble sec;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      m = ( long ) ( ( a - ( double ) d ) * 60000.0 );
	  double z = (m % 1000);
	  sec = 60 * (z/1000) ;

      if ( neg )
            d = -d;

      wxString s;

      if ( !NEflag )
            s.Printf ( _T ( "%d%02ld%02ld" ), d, m / 1000, m % 1000 );
      else
      {
            if ( NEflag == 1 )
            {
                  char c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }
					newPosition.posLat = a;
					newPosition.latitude = d;
					newPosition.latmin   = m / 1000.0;
					newPosition.WEflag = c;

				  s.Printf ( _T("%03d%02ld%05.2f%c"), d, m / 1000, sec , c );
            }
            else if ( NEflag == 2 )
            {
                  char c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
					newPosition.posLon = a;
					newPosition.longitude = d;
					newPosition.lonmin   = m / 1000.0;
					newPosition.NSflag = c;
					s.Printf ( _T("%03d%02ld%05.2f%c"), d, m / 1000, sec, c );
            }
      }
      return s;
}

wxString Logbook::toSDMMOpenCPN ( int NEflag, double a, bool hi_precision )
{
      wxString s;
      double mpy;
      short neg = 0;
      int d;
      long m;
      double ang = a;
      char c = 'N';
	  int g_iSDMMFormat = 0;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      if ( neg )
            d = -d;
      if(NEflag)
      {
            if ( NEflag == 1 )
            {
                  c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }
            }
            else if ( NEflag == 2 )
            {
                  c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
            }
      }

	  switch (g_iSDMMFormat)
      {
      case 0:
            mpy = 600.0;
            if(hi_precision)
                  mpy = mpy * 1000;

            m = ( long ) wxRound( ( a - ( double ) d ) * mpy );

            if ( !NEflag || NEflag < 1 || NEflag > 2) //Does it EVER happen?
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%d %02ld.%04ld'" ), d, m / 10000, m % 10000 );
                  else
					  s.Printf ( _T ( "%d %02ld.%01ld'" ), d, m / 10,   m % 10 );
            }
            else
            {
				if(NEflag == 1)
				{
					newPosition.posLat = a;
					newPosition.latitude = d;
					newPosition.latmin   = m / 1000.0;
					newPosition.WEflag = c;
				}
				else
				{
					newPosition.posLon = a;
					newPosition.longitude = d;
					newPosition.lonmin   = m / 1000.0;
					newPosition.NSflag = c;
				}
                  if(hi_precision)
					  s.Printf ( _T ( "%03d%02ld.%04ld%c" ), d, m / 10000,( m % 10000 ), c );
                   else
					   s.Printf ( _T ( "%03d%02ld.%01ld%c" ), d, m / 10,   ( m % 10 ), c );
            }
            break;
      case 1:
            if (hi_precision)
                  s.Printf (_T ( "%03.6f" ), ang); //cca 11 cm - the GPX precision is higher, but as we use hi_precision almost everywhere it would be a little too much....
            else
                  s.Printf (_T ( "%03.4f" ), ang); //cca 11m
            break;
      case 2:
            m = ( long ) ( ( a - ( double ) d ) * 60 );
            mpy = 10.0;
            if (hi_precision)
                  mpy = mpy * 100;
            long sec = (long) (( a - ( double ) d - ( ( ( double ) m ) / 60 ) ) * 3600 * mpy);

            if ( !NEflag || NEflag < 1 || NEflag > 2) //Does it EVER happen?
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 1000, sec % 1000 );
                  else
					  s.Printf ( _T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 10, sec % 10 );
            }
            else
            {
                  if(hi_precision)
					  s.Printf ( _T ( "%03d %02ld %02ld.%03ld %c" ), d, m, sec / 1000, sec % 1000, c );
                   else
					   s.Printf ( _T ( "%03d %02ld %02ld.%ld %c" ), d, m,sec / 10, sec % 10, c );
            }
            break;
      }
      return s;
}

bool Logbook::checkGPS(bool appendClick)
{
	sLogText = _T("");

	if(gpsStatus)
	{
		if(opt->showWindHeading == 1 && !bCOW)
		{
			sLogText = _("Wind is set to Heading,\nbut GPS sends no Heading Data.\nWind is set now to Realative to boat\n\n"); 
			opt->showWindHeading = 0;
		}
		if(courseChange && !appendClick)
			sLogText += opt->courseChangeText+opt->courseChangeDegrees+opt->Deg;
		else if(guardChange)
			sLogText += opt->guardChangeText;
		else if(waypointArrived)
		{
			wxString s, ext;

			if(!OCPN_Message)
			{	/* s = wxString::Format(_("\nName of Waypoint: %s\nTrue bearing to destination: %4.1f%s\nRange to destination: %4.2f%s"),
																	tempRMB.From.c_str(),
																	tempRMB.BearingToDestinationDegreesTrue,opt->Deg.c_str(),
																	tempRMB.RangeToDestinationNauticalMiles,opt->distance.c_str());
				s.Replace(_T("."),dialog->decimalPoint);*/
			}
			else
			{
				setWayPointArrivedText();
			}
			
		}
		else if(everySM && !appendClick)
			sLogText += opt->everySMText+opt->everySMAmount+opt->distance;
		else if((dialog->timer->IsRunning() || opt->timerType != 0) && !appendClick)
			sLogText += opt->ttext;
			
		return true;
	}
	else
	{
	//	sLat = sLon = sDate = sTime = _T("");
//		sCOG = sCOW = sSOG = sSOW = sDepth = sWind = sWindSpeed = sTemperatureWater = sTemperatureAir = sPressure = sHumidity = _T("");
        sCOG = sSOG = _T("");
		bCOW = false;

		if(opt->noGPS)
			sLogText = _("No GPS-Signal !");
		else
			sLogText = _T("");
		if(waypointArrived)
		{
			setWayPointArrivedText();
		}
		return false;
	}
}

void Logbook::setWayPointArrivedText()
{
			wxString ext;
			wxString msg;

			if(tempRMB.To != _T("-1"))
				{ msg = _("Next WP Name: "); }
			else
				{ msg = _("Last waypoint of the Route"); tempRMB.To = wxEmptyString; }

			wxString s = wxString::Format(_("\nName of Waypoint: %s\n%s %s"),tempRMB.From.c_str(), msg.c_str(), tempRMB.To.c_str());

			if(WP_skipped)
				ext = _("Waypoint skipped");
			else
				ext = _("WayPoint arrived");

			if(sLogText != _T(""))
				sLogText += wxString::Format(_T("\n%s\n%s%s"),opt->waypointText.c_str(),ext.c_str(),s.c_str());
			else
				sLogText += wxString::Format(_T("%s\n%s%s"),opt->waypointText.c_str(),ext.c_str(),s.c_str());
}

class ActuellWatch;
void Logbook::SetGPSStatus(bool status)
{
	if(!status)
		sDate = _T("");

	if(status != gpsStatus)
		dialog->crewList->dayNow(false);

	gpsStatus = status;

}

void Logbook::showSearchDlg(int row, int col)
{
	LogbookSearch* dlg = new LogbookSearch(dialog, row, col);
	dlg->Show(true);
}

////////////////////////////////////////////////////
NoAppendDialog::NoAppendDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText73 = new wxStaticText( this, wxID_ANY, _("It's not allowed to append Data to a old Logbook\n\n\
OpenCPN switchs to actuell logbook"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText73->Wrap( -1 );
	bSizer20->Add( m_staticText73, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer5 = new wxStdDialogButtonSizer();
	m_sdbSizer5OK = new wxButton( this, wxID_OK );
	m_sdbSizer5->AddButton( m_sdbSizer5OK );
	m_sdbSizer5->Realize();
	bSizer20->Add( m_sdbSizer5, 0, wxALIGN_CENTER, 5 );
	
	this->SetSizer( bSizer20 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

NoAppendDialog::~NoAppendDialog()
{
}

////////////////////////////
// PVBE-DIALOG
////////////////////////////
PBVEDialog::PBVEDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	dialog = (LogbookDialog*)parent;
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlPVBE = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer21->Add( m_textCtrlPVBE, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
	
	this->Centre( wxBOTH );
	m_textCtrlPVBE->AppendText(_T("this is a test if you have received PBVE-Sentences\nthey are manufacturer-specific\nit's use is for engine-hours and fuel-consumption\n"));
}

PBVEDialog::~PBVEDialog()
{
	dialog->logbook->pvbe = NULL;
}

void PBVEDialog::PBVEDialogOnClose(wxCloseEvent &event)
{
	dialog->logbook->pvbe = NULL;
}

void PBVEDialog::OnCloseWindow(wxCloseEvent& ev)
{
	dialog->logbook->pvbe = NULL;
}

/////////////////////// LogbookSearchDlg ////////////////////////////////////////////////////

LogbookSearch::LogbookSearch( wxWindow* parent, int row, int col, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->parent = (LogbookDialog*)parent;
	this->row = row;
	this->col = col;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText96 = new wxStaticText( this, wxID_ANY, _("Search in"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText96->Wrap( -1 );
	fgSizer41->Add( m_staticText96, 0, wxALL, 5 );
	
	m_radioBtnActuell = new wxRadioButton( this, wxID_ANY, _("Actuell Logbook"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_radioBtnActuell, 0, wxALL, 5 );
	
	m_radioBtnAll = new wxRadioButton( this, wxID_ANY, _("All Logbooks"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_radioBtnAll, 0, wxALL, 5 );
	
	bSizer23->Add( fgSizer41, 0, wxALIGN_CENTER, 5 );
	
	m_staticline32 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline32, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer411;
	fgSizer411 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer411->SetFlexibleDirection( wxBOTH );
	fgSizer411->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText108 = new wxStaticText( this, wxID_ANY, _("Searchstring"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText108->Wrap( -1 );
	fgSizer411->Add( m_staticText108, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl72 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), wxTE_LEFT|wxTE_MULTILINE );
	fgSizer411->Add( m_textCtrl72, 0, wxALL, 5 );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, _("In Column"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	fgSizer411->Add( m_staticText110, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choice23Choices;
	m_choice23 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 250,-1 ), m_choice23Choices, 0 );
	m_choice23->SetSelection( 0 );
	fgSizer411->Add( m_choice23, 0, wxALL, 5 );
	
	m_staticText97 = new wxStaticText( this, wxID_ANY, _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText97->Wrap( -1 );
	fgSizer411->Add( m_staticText97, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer42;
	fgSizer42 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer42->SetFlexibleDirection( wxBOTH );
	fgSizer42->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_choiceGreaterEqualChoices[] = { wxT(">="), wxT("<=") };
	int m_choiceGreaterEqualNChoices = sizeof( m_choiceGreaterEqualChoices ) / sizeof( wxString );
	m_choiceGreaterEqual = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceGreaterEqualNChoices, m_choiceGreaterEqualChoices, 0 );
	m_choiceGreaterEqual->SetSelection( 0 );
	fgSizer42->Add( m_choiceGreaterEqual, 0, wxALL, 5 );
	
	m_datePicker = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
	fgSizer42->Add( m_datePicker, 0, wxALL, 5 );
	
	m_buttonSelectDate = new wxButton( this, wxID_ANY, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer42->Add( m_buttonSelectDate, 0, wxALL, 5 );
	
	fgSizer411->Add( fgSizer42, 1, wxEXPAND, 5 );
	
	bSizer23->Add( fgSizer411, 0, wxEXPAND, 5 );
	
	m_staticline39 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline39, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer43;
	fgSizer43 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer43->SetFlexibleDirection( wxBOTH );
	fgSizer43->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_buttonBack = new wxButton( this, wxID_ANY, wxT("<<"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer43->Add( m_buttonBack, 0, wxALL, 5 );
	
	m_buttonForward = new wxButton( this, wxID_ANY, wxT(">>"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer43->Add( m_buttonForward, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer23->Add( fgSizer43, 0, wxALIGN_CENTER, 0 );
	
	this->SetSizer( bSizer23 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( LogbookSearch::OnInitDialog ) );
	m_buttonBack->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickBack ), NULL, this );
	m_buttonForward->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickForward ), NULL, this );
	m_buttonSelectDate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickSelectDate ), NULL, this );
}

LogbookSearch::~LogbookSearch()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( LogbookSearch::OnInitDialog ) );
	m_buttonBack->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickBack ), NULL, this );
	m_buttonForward->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickForward ), NULL, this );
	m_buttonSelectDate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookSearch::OnButtonClickSelectDate ), NULL, this );
	
}

void LogbookSearch::OnInitDialog( wxInitDialogEvent& event )
{
	wxDateTime dt;
	searchrow = 0;

	parent->myParseDate(parent->m_gridGlobal->GetCellValue(0,1),dt);
	m_datePicker->SetValue(dt);

	int gridNo = parent->m_notebook8->GetSelection();
	for(int i = 0; i < parent->logGrids[gridNo]->GetNumberCols(); i++)
		this->m_choice23->Append(parent->logGrids[gridNo]->GetColLabelValue(i));

	m_choice23->SetSelection(col);
	m_textCtrl72->SetFocus();

	m_radioBtnAll->Hide();
	Fit();
}

void LogbookSearch::OnButtonClickSelectDate( wxCommandEvent& event )
{
	DateDialog dateDlg(this);
	if(dateDlg.ShowModal() == wxID_OK)
		this->m_datePicker->SetValue(dateDlg.m_calendar2->GetDate());
}

void LogbookSearch::OnButtonClickForward( wxCommandEvent& event )
{
	int gridNo = parent->m_notebook8->GetSelection();
	int col = this->m_choice23->GetSelection();
	wxString ss = this->m_textCtrl72->GetValue().Lower();
	wxDateTime dt,dlgDt;

	dlgDt = m_datePicker->GetValue();
	if(searchrow < 0 ) searchrow = 0 ;
	if(!direction) searchrow++;
	direction = true;

	for(; searchrow < parent->logGrids[gridNo]->GetNumberRows(); searchrow++)
	{
		parent->myParseDate(parent->logGrids[0]->GetCellValue(searchrow,LogbookHTML::RDATE),dt);

		if(m_choiceGreaterEqual->GetSelection() == 0)
		{
			if(dt < dlgDt) 
				continue;
		}
		else
		{
			if(dt > dlgDt) 
				continue;
		}

		if(parent->logGrids[gridNo]->GetCellValue(searchrow,col).Lower().Contains(ss))
		{
			parent->logGrids[gridNo]->SetFocus();
			parent->logGrids[gridNo]->SetGridCursor(searchrow,col);
			searchrow++;
			break;
		}
	}
}

void LogbookSearch::OnButtonClickBack( wxCommandEvent& event )
{
	int gridNo = parent->m_notebook8->GetSelection();
	int col = this->m_choice23->GetSelection();
	wxString ss = this->m_textCtrl72->GetValue().Lower();
	wxDateTime dt,dlgDt;

	if(direction) searchrow--;
	direction = false;

	dlgDt = m_datePicker->GetValue();
	if(searchrow > parent->logGrids[gridNo]->GetNumberRows()-1) searchrow--;

	for(; searchrow >= 0; searchrow--)
	{
		parent->myParseDate(parent->logGrids[0]->GetCellValue(searchrow,LogbookHTML::RDATE),dt);
		if(m_choiceGreaterEqual->GetSelection() == 0)
		{
			if(m_choiceGreaterEqual->GetSelection() == 0)
			{
				if(dt < dlgDt) 
				continue;
			}
			else
			{
				if(dt > dlgDt) 
				continue;
			}
		}

		if(parent->logGrids[gridNo]->GetCellValue(searchrow,col).Lower().Contains(ss))
		{
			parent->logGrids[gridNo]->SetFocus();
			parent->logGrids[gridNo]->SetGridCursor(searchrow,col);
			searchrow--;
			break;
		}
	}
}

////////////////////  Reminder Dlg //////////////////////
LinesReminderDlg::LinesReminderDlg(wxString str, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextreminder = new wxStaticText( this, wxID_ANY, str, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticTextreminder->Wrap( -1 );
	bSizer38->Add( m_staticTextreminder, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_sdbSizer9 = new wxStdDialogButtonSizer();
	m_sdbSizer9OK = new wxButton( this, wxID_OK );
	m_sdbSizer9->AddButton( m_sdbSizer9OK );
	m_sdbSizer9->Realize();
	bSizer38->Add( m_sdbSizer9, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer38 );
	this->Layout();
	
	this->Fit();
	this->Centre( wxBOTH );
}

LinesReminderDlg::~LinesReminderDlg()
{
}
