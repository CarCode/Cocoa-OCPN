#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define PI 3.14159265

#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/math.h>
#include <wx/stdpaths.h>
#include <wx/progdlg.h>
#include <wx/gdicmn.h>

#include "nmea0183/nmea0183.h"
#include "polar_pi.h"
#include "Polar.h"

#include <map>

Polar::Polar(PolarDialog* parent)
{
	sails.Clear();

	dlg = parent;
	opt = dlg->plugin->opt;

	windColor[0] = wxTheColourDatabase->Find(_T("YELLOW"));
	windColor[1] = wxTheColourDatabase->Find(_T("CORAL"));
	windColor[2] = wxTheColourDatabase->Find(_T("CYAN"));
	windColor[3] = wxTheColourDatabase->Find(_T("LIGHT BLUE"));
	windColor[4] = wxTheColourDatabase->Find(_T("CORNFLOWER BLUE"));
	windColor[5] = wxTheColourDatabase->Find(_T("GREEN"));
	windColor[6] = wxTheColourDatabase->Find(_T("BROWN"));
	windColor[7] = wxTheColourDatabase->Find(_T("RED"));
	windColor[8] = wxTheColourDatabase->Find(_T("VIOLET RED"));
	windColor[9] = wxTheColourDatabase->Find(_T("VIOLET"));

	nmea = false;
	engineRunning = false;
	dtRPM = wxDateTime::Now();

	mode			= 0;
	windSpeed		= -1;
	windAngle		= -1;
	windReference	= wxEmptyString;

	timeout = 5;
	filterDlg = new FilterDlg(dlg,this);

	dlg->m_choiceRingsPolar->SetSelection(1);
	dlg->m_choiceDegreesPolar->SetSelection(0);

	knots = wxAtoi(dlg->m_choiceRingsPolar->GetString(dlg->m_choiceRingsPolar->GetSelection()));
	degrees = wxAtoi(dlg->m_choiceDegreesPolar->GetString(dlg->m_choiceDegreesPolar->GetSelection()));
	for(int i = 4; i < 41; i += 4)
		dlg->m_choiceWindPolar->Append(wxString::Format(_T("%i knots"),i));


	dlg->m_choiceWindPolar->Append(_("Max. only"));
	dlg->m_choiceWindPolar->SetSelection(0);

	dlg->m_panelPolar->Layout();
	dlg->m_buttonSavePolar->Enable(false);

	filterSails = false;

	wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXMSW__
	wxString stdPath  = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
	wxString stdPath  = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
	wxString stdPath  = std_path.GetUserConfigDir();   // should be ~/Library/Preferences	
#endif

	wxString basePath = stdPath   + wxFileName::GetPathSeparator();
	basePath += _T("plugins") + wxString(wxFileName::GetPathSeparator());
	basePath += _T("logbook") + wxString(wxFileName::GetPathSeparator());
	basePath += _T("data") + wxString(wxFileName::GetPathSeparator());
	logbookDataPath  = basePath;

	reset();
}

Polar::~Polar(void)
{
	nmea = false;
	delete filterDlg;
}

void Polar::setValue(wxString s, int dir, int spd)
{
	s.Replace(_T(","),_T("."));
	double speed = wxAtof(s);
	if(speed > 0.0)
	{
		windsp[spd].wdirMax[dir] = speed;
		windsp[spd].count[dir] = 1;
	}
	else
	{ // entry deleted
		windsp[spd].wdirMax[dir] = -1;
		windsp[spd].count[dir] = 0;
	}

		//if(mode != 2)
			dlg->m_panelPolar->Refresh();
}

void Polar::setMode(int sel)
{
	int sash = dlg->m_splitter1->GetSize().GetWidth()-dlg->m_gridEdit->GetSize().GetWidth();

	mode = sel;
	switch(sel)
	{
	case 0:	
		if(dlg->m_splitter1->IsSplit())
			dlg->m_splitter1->Unsplit(dlg->m_panel6);
		dlg->m_button61->Enable();
		if(dlg->timer->IsRunning())
			dlg->timer->Stop();
		nmea = false;
		dlg->m_buttonFilterPolar->Enable();
		dlg->m_choiceSourcePolar->Enable();
		source(sel);
		break;
	case 1:	
	case 2:
		if(!dlg->m_splitter1->IsSplit())
		{
			dlg->m_splitter1->SplitVertically(dlg->m_panelPolar,dlg->m_panel6,sash);
			dlg->m_splitter1->SetSashPosition(400);
		}
		dlg->m_buttonSavePolar->Enable();
		dlg->m_button61->Disable();
		dlg->m_buttonFilterPolar->Disable();
		dlg->m_choiceSourcePolar->Enable();
		source(sel);

		if(sel == 2)
			loadVDR();
		else
		{
			dlg->timer->Start(1000);
			dlg->timerRPM->Start(5000);
			dlg->m_toggleBtnRecord->Show();
		}

		break;
	case 3:	
		if(!dlg->m_splitter1->IsSplit())
		{
			dlg->m_splitter1->SplitVertically(dlg->m_panelPolar,dlg->m_panel6,sash);
			dlg->m_splitter1->SetSashPosition(400);
		}
		if(dlg->timer->IsRunning())
			dlg->timer->Stop();
		dlg->m_buttonSavePolar->Enable();
		dlg->m_button61->Disable();
		dlg->m_buttonFilterPolar->Disable();
		dlg->m_choiceSourcePolar->Disable();
		dlg->m_toggleBtnRecord->Hide();
		source(sel);
		nmea = false;
		break;
	}

	dlg->m_panelPolar->Refresh();
}

void Polar::setEngineStatus(wxString str)
{
	if(mode == 1 && str == _T("ON"))
	{
		nmea = false;
		dlg->m_toggleBtnRecord->Hide();
		dlg->m_staticTextEngine->SetLabel(_("Engine started / Recording is stopped"));
		dlg->m_panel6->Layout();
	}
	else if(mode == 1 && str == _T("OFF"))
	{
		dlg->m_toggleBtnRecord->Show();
		dlg->m_staticTextEngine->SetLabel(_T(""));
		dlg->m_panel6->Layout();
		nmea = true;
	}
}

void Polar::loadVDR()
{
	dlg->timer->Stop();
	dlg->m_choiceSourcePolar->Enable();
	dlg->m_toggleBtnRecord->Hide();
	wxFileDialog fdlg(dlg,_("Select a VDR-File"),_T(""));
	if(fdlg.ShowModal() == wxID_CANCEL) return;

	reset();
	dlg->m_panelPolar->Refresh();
	dlg->m_gridEdit->ClearGrid();

	nmea = false;
	dlg->m_toggleBtnRecord->Hide();

	wxFileInputStream input(fdlg.GetPath());
	wxTextInputStream in(input);

	wxProgressDialog progressRead( _("Please wait."), _T(""),
		100, dlg,  wxFRAME_NO_TASKBAR | wxPD_AUTO_HIDE | wxPD_CAN_ABORT );
	progressRead.Fit(); 

	wxString s = wxEmptyString;
	do{
		long long p = input.TellI();
		long long ii = input.GetLength();
		int o = ((double)p/(double)ii)*100;

		in >> s;
		if(input.Eof()) break;
		if(!progressRead.Update(o))
			break;
		setSentence(s += _T("\x0D\x0A"));
		insert();
		s = wxEmptyString;
	//	::wxYield();
	}while(true);

	dlg->m_panelPolar->Refresh();
	dlg->m_toggleBtnRecord->Show();
}

bool Polar::insert()
{
	double speed;

	if(dlg->m_choiceSourcePolar->GetSelection() == 0)
		speed = speedoSpeed;
	else
		speed = gpsSpeed;

	if(windAngle >=0 && windSpeed >= 0 && (speedoSpeed > 0 || gpsSpeed > 0))
	{
		int tdir = windAngle;
		if(windReference == _T("R"))
			windAngle = wxRound(atan(windSpeed*sin(windAngle*PI/180)/(windSpeed*cos(windAngle*PI/180)-speed))*180/PI); // make true wind-angle
		if(windAngle < 0 && tdir <= 180)
			windAngle += 180;
		else if(windAngle < 0 && tdir > 180)
			windAngle += 360;
		else if(windAngle > 0 && tdir > 180)
			windAngle += 180;

		int row = wxRound((windAngle-24)/8);
		int col = windSpeed/4;
		if(row > 19)
			row = 39 - row;

		if((row >= 0 && row < 20) && (col >= 0 && col < 10))
		{ 
			if(!dlg->m_gridEdit->GetCellValue(row,col).IsEmpty())
			{
				if(wxAtof(dlg->m_gridEdit->GetCellValue(row,col)) < speed)
				{
					setValue(wxString::Format(_T("%.2f"),speedoSpeed),row,col);
					dlg->m_gridEdit->SetCellValue(row,col,wxString::Format(_T("%.2f"),speed));
				}

			}
			else if(speed > 0.0)
			{
				dlg->m_gridEdit->SetCellValue(row,col,wxString::Format(_T("%.2f"),speed));
				setValue(wxString::Format(_T("%.2f"),speedoSpeed),row,col);
			}

			speedoSpeed = 0;
			gpsSpeed = 0;
			windSpeed = -1;
			windAngle = -1;
			windReference = wxEmptyString;
			timeout = 5;

			return true;
		}
	}
	return false;
}

void Polar::createDiagram(wxDC& dc)
{
	center = dlg->m_panelPolar->GetSize();
	center.x = center.x / 2 - 150;
	center.y = (center.y / 2) + (dlg->bSizerPolar->GetSize().GetHeight() / 2);
	rSpeed[0] = center.y - dlg->bSizerPolar->GetSize().GetHeight()-40;
	dist = rSpeed[0] / knots;
	for(int i = 0; i < knots; i++)
		rSpeed[i] = wxRound(dist*(i+1));
}

void Polar::Render()
{
	if(mode == 0)
	{
//		dc->DrawText(_("Testing, work in progress..."), 40, 60); 

		dc->SetPen( wxPen( wxColor(0,0,0), 1 ) ); 
		dc->SetBrush( wxBrush(wxColour(255,0,0)) );	
		dc->DrawCircle( wxPoint(40,90), 5);
		dc->DrawText(_("less than 3 entries found"), 60, 80); 
		dc->SetBrush( wxBrush(wxColour(255,255,0)) );
		dc->DrawCircle( wxPoint(40,110), 5);
		dc->DrawText(_("less than 5 entries found"), 60, 100); 
		dc->SetBrush( wxBrush(wxColour(0,255,0)) );
		dc->DrawCircle( wxPoint(40,130), 5);
		dc->DrawText(_("more than 10 entries found"), 60, 120); 
		dc->SetBrush( wxBrush(wxColour(255,255,255)) );
	}
	int xknt, yknt;
	for(int i = knots-1; i >= 0; i--)
	{
		xknt = wxRound(cos((PI/180.0)*(0-90))*rSpeed[i]+center.x);
		yknt = wxRound(sin((PI/180.0)*(0-90))*rSpeed[i]+center.y);

		if(knots == 18 || knots == 24)
		{
			if((knots == 18 && ((i+1) % 2) == 0) || (knots == 24 && ((i+1) % 3) == 0))
			{
				//dc->DrawCircle( wxPoint(center.x,center.y), rSpeed[i]);
				dc->DrawArc(center.x,center.y+rSpeed[i],center.x,center.y-rSpeed[i],center.x,center.y);
				dc->DrawText(wxString::Format(_T("%i"),i+1),wxPoint(xknt,yknt-10));
			}
		}
		else
		{
			dc->DrawArc(center.x,center.y+rSpeed[i],center.x,center.y-rSpeed[i],center.x,center.y);
			///dc->DrawCircle( wxPoint(center.x,center.y), rSpeed[i]);
			dc->DrawText(wxString::Format(_T("%i"),i+1),wxPoint(xknt,yknt-10));
		}
	}

	int angle;
	if(degrees == 8) { angle = 24; } else { angle = 20; }
	for(; angle <= 180; angle += degrees)
	{
		int xt = wxRound(cos((PI/180.0)*(angle-90))*(rSpeed[(int)knots-1]+20)+center.x);
		int yt = wxRound(sin((PI/180.0)*(angle-90))*(rSpeed[(int)knots-1]+20)+center.y);

		dc->DrawLine(wxPoint(center.x,center.y),wxPoint(xt,yt));
	//	if(angle <= 180)
			dc->DrawText(wxString::Format(_T("%i\xB0"),angle), wxPoint(xt,yt)); 
	//	else
		//	dc->DrawText(wxString::Format(_T("%i°"),angle), wxPoint(xt-25,yt)); 

	}

	if(mode == 0 && opt->sailsName.Count() != 0)
	{
		dc->DrawText(_("Filtered by:"), center.x+rSpeed[(int)knots-1]+100, 60);

		wxString sail = wxEmptyString;  
		unsigned int y = 80;

		for(unsigned i = 0; i < 14; i++)
		{
			if(filterDlg->checkboxSails[i]->IsChecked())
			{
				dc->DrawText(opt->sailsName.Item(i), center.x+rSpeed[(int)knots-1]+100, y);
				y += 20;
			}
		}
	}

	createSpeedBullets();
}

void Polar::createPolar()
{
	::SendPluginMessage(wxString(_T("POLAR_SAVE_LOGBOOK")),wxString(_T("")));
	wxArrayString files;												
	wxString path = logbookDataPath;
	wxDir dir;
	dir.GetAllFiles(path,&files,_T("*logbook*.txt"),wxDIR_FILES);				// collect all logbook-files
	bool nextline = false;

	reset();

	wxProgressDialog progressRead( _("Please wait."), _T(""),
		100 , dlg,  wxFRAME_NO_TASKBAR | wxPD_AUTO_HIDE );
	progressRead.Fit();  

	for(unsigned int i = 0; i < files.Count(); i++)
	{
		wxFileName fn(files[i]);
		progressRead.Update(((i+1)/files.GetCount())*100,_("Collecting: ")+fn.GetFullName());
		progressRead.Fit();
		::wxSafeYield();


		wxFileInputStream stream( files[i] );									// get trough the files
		wxTextInputStream in(stream);	
		wxString wdirstr,wsp;
		double speed = -1;

		while(!stream.Eof())
		{
			wxString str = in.ReadLine();										// read line by line
			if(stream.Eof()) break;
			wxStringTokenizer tkz(str,_T("\t"),wxTOKEN_RET_EMPTY);				// split into fields
			int o = 0;
			int winddir = -1; int wsp = -1;
			bool rel = false;
			nextline = false;

			while(tkz.HasMoreTokens())
			{
				wxString s = tkz.GetNextToken();
				switch(o)
				{
				case 7: // Sign "S"
					if(s.Trim() != _T("S")) { nextline = true; break; }		// only at sea , no harbour,mooring etc., else read next line
					break;
				case 14: // SOG												// get speed over ground from GPS, if speed=0 get next line
					if(dlg->m_choiceSourcePolar->GetSelection() == 0) break;
					s.Replace(_T(","),_T("."));
					speed = wxAtof(s);
					if(speed == 0.0) { nextline = true; break; }	
					break;
				case 15: // STW												// get speed trough water from Speedo, if speed=0 get next line
					if(dlg->m_choiceSourcePolar->GetSelection() == 1) break;
					s.Replace(_T(","),_T("."));
					speed = wxAtof(s);
					if(speed == 0.0) { nextline = true; break; }
					break;
				case 19: // WindDirection
					if(s.Trim().IsEmpty()) 
					{ winddir = -1; nextline = true; break; }			// if winddirection is empty, get next line

					s.Replace(_T(","),_T("."));
					winddir = wxAtoi(s);
					if(s.Contains(_T("R")))
						rel = true;
					break;
				case 20: // WindSpeed										// add Windspeed in struct p[winddirection]
					{
						s.Replace(_T(","),_T("."));
						double wspd = wxAtof(s);
						if(wspd == 0.0) { nextline = true; break; }

						if(s.Contains(_T("m/s")))
							wsp = wxRound((wspd*3600)/1852);
						else if(s.Contains(_T("km/h")))
							wsp = wxRound(wspd/1.852);
						else
							wsp = wspd;

					}
					//filter here
					break;
				case 28: // Engine#1 Hours									// Don't use when Engine is on
					if(!s.Trim().Contains(_T("00:00")))
					{ speed = 0; nextline = true; }	
					break;
				case 32: // Sails
					if(filterSails)
					{
						if(s.Trim(false).IsEmpty())
						{ nextline = true; break; }
						s.Replace(_T("\n"),_T(","));
						wxStringTokenizer tkz(s,_T(","));
						if(tkz.CountTokens() != sails.GetCount())
						  { nextline = true; break; }
						else
						{
							unsigned int ok = 0;
							while(tkz.HasMoreTokens())
							{
								wxString t = tkz.GetNextToken().Trim(false);
								t = t.Trim();
								for(unsigned int i = 0; i < sails.GetCount(); i++)
								{
									if(t == sails.Item(i).Trim())
									{
										ok++;
										break;
									}
								}
							}				
							if(ok == 0 || ok != sails.Count())
							{ nextline = true; break; }
						}
					}
					else
						nextline = false;
					break;
				case 40: // Engine#2 Hours
					if(!s.Trim().Contains(_T("00:00")))						// Don't use when Engine in on
					{ speed = 0; nextline = true; }
					break;
				}
				if(nextline) { break; }										// true = read next line
				o++;
			}
			if(wsp >= 0 && !nextline)
			{
				if(rel)
				{
					int tdir = winddir;
					winddir = wxRound(atan(wsp*sin(winddir*PI/180)/(wsp*cos(winddir*PI/180)-speed))*180/PI);
					if(winddir < 0 && tdir <= 180)
						winddir += 180;
					else if(winddir < 0 && tdir > 180)
						winddir += 360;
					else if(winddir > 0 && tdir > 180)
						winddir += 180;

					rel = false;
				}
				wsp /= 4;
				if(winddir < 180)
					winddir = wxRound((winddir-24)/8);
				else
					winddir = 39-wxRound((winddir-24)/8);
				//	wxMessageBox(wxString::Format(_T("windspd:%i winddir:%i speed:%.2f size:%i"),wsp,winddir,speed,windsp[wsp].winddir.size()));
				if(winddir >= 0)
				{
					windsp[wsp].winddir.insert(std::pair<int,double>(winddir,speed));
					if(speed > windsp[wsp].wdirMax[winddir])
						windsp[wsp].wdirMax[winddir] = speed;
				}
			}
		}
	}

	if(filterDlg->m_checkBoxRangePercent->GetValue())
	{
		for(int i = 0; i < 10; i++)
		{
			for(int n = 0; n < WINDDIR; n++)
			{
				for (it = windsp[i].winddir.begin(); it!=windsp[i].winddir.end(); ++it)
				{
					int wdir = (*it).first;
					double spd = (*it).second;

					if((wdir == n) )
					{
						double percent = ((spd-windsp[i].wdirMax[n])/windsp[i].wdirMax[n])*100;
						//	wxMessageBox(wxString::Format(_T("%.2f %.2f %.2f proz = %.2f"),spd,windsp[i].wdirMax[n],spd-windsp[i].wdirMax[n],percent));
						if(percent >= -wxAtof(filterDlg->m_choice6->GetString(filterDlg->m_choice6->GetSelection())))
						{
							windsp[i].count[n]++;
							windsp[i].wdirTotal[n] += spd;
							/*wxMessageBox(wxString::Format(_T("%.2f %.2f %.2f proz = %.2f\n%i %.2f"),spd,windsp[i].wdirMax[n],windsp[i].wdirMax[n]-spd,((windsp[i].wdirMax[n]-spd)/spd)*100,
							windsp[i].count[n],windsp[i].wdirTotal[n]));*/

						}
					}
				}
			}
		}
	}

	if(filterDlg->m_checkBoxAverage->GetValue())
	{
		for(int i = 0; i < 10; i++)
		{
			for(int n = 0; n < WINDDIR; n++)
			{
				for (it = windsp[i].winddir.begin(); it!=windsp[i].winddir.end(); ++it)
				{
					int wdir = (*it).first;
					double spd = (*it).second;

					if((wdir == n) )
					{
						windsp[i].count[n]++;
						windsp[i].wdirTotal[n] += spd;
						/*wxMessageBox(wxString::Format(_T("%.2f %.2f %.2f proz = %.2f\n%i %.2f"),spd,windsp[i].wdirMax[n],windsp[i].wdirMax[n]-spd,((windsp[i].wdirMax[n]-spd)/spd)*100,
						windsp[i].count[n],windsp[i].wdirTotal[n]));*/

					}
				}
			}
		}
	}


	for(int i = 0; i < 10; i++)
	{
		for(int n = 0; n < WINDDIR; n++)
		{
			if(windsp[i].count[n] > 0)
			{
				windsp[i].wdirTotal[n] /= windsp[i].count[n];
				dlg->m_gridEdit->SetCellValue(n,i,wxString::Format(_T("%.2f"),windsp[i].wdirTotal[n]));
			}
		}

	}

	dlg->m_panelPolar->Refresh();
	dlg->m_buttonSavePolar->Enable();
}

void Polar::createSpeedBullets()
{
	int sel = dlg->m_choiceWindPolar->GetSelection();
	if(sel == 11) { createSpeedBulletsMax(); return; }

	int radius = 5;
	double length = dist;												
	dc->SetPen( wxPen( wxColor(0,0,0), 1 ) ); 
	int end;;
	int xt, yt, pc;
	wxPoint ptArr[360];

	if(sel != 0) 
		{ sel -= 1; end = sel+1; }
	else
		{ sel = 0; end = 10; }

	int linelength;
	wxColour colour,brush;
	wxPen p = dc->GetPen();													// get actual Pen for restoring later
	for(int wsp = sel; wsp < end; wsp++)									// go thru all winddirection-structures depending on choiocebox degrees
	{
		pc = 0;
		colour = windColor[wsp]; brush = windColor[wsp];
		for(int dir = 0; dir < WINDDIR; dir++)
		{
			if(windsp[wsp].count[dir] <= 0) continue;							// no data ? continue

			if(mode == 0)
			{
				if(windsp[wsp].count[dir] < 3)
					//dc->SetBrush( wxBrush(wxColour(255,0,0)) );							// set bulletcolor for logbook-mode
					brush = wxColour(255,0,0);
				else if( windsp[wsp].count[dir]  >= 3 && windsp[wsp].count[dir] < 5)
					//dc->SetBrush( wxBrush(wxColour(255,255,0)) );	
					brush = wxColour(255,255,0);
				else
					brush = wxColour(0,255,0);
					//dc->SetBrush( wxBrush(wxColour(0,255,0)) );
			}
			else
				dc->SetBrush( brush );

			switch(mode)
			{
			case 0:
				if(filterDlg->m_checkBoxMax->GetValue())
					linelength = windsp[wsp].wdirMax[dir]*length;
				else
					linelength = windsp[wsp].wdirTotal[dir]*length;			// calculate distance from centerpoint for speed
				break;
			case 1:
			case 2:
			case 3:
				linelength = (windsp[wsp].wdirMax[dir])*length;
				break;
			}
			xt = wxRound(cos(toRad(dir*8+24))*linelength+center.x);					// calculate the point for the bullet
			yt = wxRound(sin(toRad(dir*8+24))*linelength+center.y);
			wxPoint pt(xt,yt);
			if(pt != wxPoint(center.x,center.y))
				ptArr[pc++] = pt;
		}

		if(pc > 2)														//Draw splines, needs min. 3 points
		{
			dc->SetPen(wxPen(colour,3));
			dc->DrawSpline(pc,ptArr);
		}

		dc->SetBrush( brush );
		for(int i = 0; i < 360; i++)
		{
			if(ptArr[i].x != 0 && ptArr[i].y != 0)
			{
			dc->SetPen(wxPen(wxColour(0,0,0),2));
			dc->DrawCircle(ptArr[i],radius);												// draw the bullet
			ptArr[i].x = ptArr[i].y = 0;
			}
			else
				break;
		}
	}

	dc->SetPen(p);
}

void Polar::createSpeedBulletsMax()
{
	int radius = 5;
	double length = dist;												
	dc->SetPen( wxPen( wxColor(0,0,0), 1 ) ); 
	int end = 10;
	int xt, yt, pc;
	wxPoint ptArr[360];
	pol temp[1];

		for(int n = 0; n < WINDDIR; n++)
		{
			temp[0].wdir[n] = 0;
			temp[0].count[n] = 0;
			temp[0].scount[n] = 0;
			temp[0].wdirMax[n] = 0;
			temp[0].wdirTotal[n] = 0;
			temp[0].winddir.clear();
		}

	int linelength;
	wxColour colour,brush;
	wxPen p = dc->GetPen();													// get actual Pen for restoring later
	for(int wsp = 0; wsp < end; wsp++)									// go thru all winddirection-structures depending on choiocebox max. all
	{
		for(int dir = 0; dir < WINDDIR; dir++)
		{
			if(windsp[wsp].count[dir] <= 0) continue;							// no data ? continue
			if(wsp == 0)
			{
				temp[0].wdirMax[dir] = windsp[wsp].wdirMax[dir];
			}
			else
			{
				if(temp[0].wdirMax[dir] < windsp[wsp].wdirMax[dir])
					temp[0].wdirMax[dir] = windsp[wsp].wdirMax[dir];
			}

		}
	}




	for(int wsp = 0; wsp < end; wsp++)									// go thru all winddirection-structures depending on choiocebox degrees
	{
		pc = 0;
		colour = windColor[wsp]; brush = windColor[wsp];
		for(int dir = 0; dir < WINDDIR; dir++)
		{
			switch(mode)
			{
			case 0:
				linelength = temp[0].wdirTotal[dir]*length;			// calculate distance from centerpoint for speed
				break;
			case 1:
			case 2:
			case 3:
				linelength = (temp[0].wdirMax[dir])*length;
				break;
			}
			xt = wxRound(cos(toRad(dir*8+24))*linelength+center.x);					// calculate the point for the bullet
			yt = wxRound(sin(toRad(dir*8+24))*linelength+center.y);
			wxPoint pt(xt,yt);
			if(pt != wxPoint(center.x,center.y))
				ptArr[pc++] = pt;
		}
	}

		colour = windColor[GREEN]; brush = windColor[GREEN];
		if(pc > 2)														//Draw splines, needs min. 3 points
		{
			dc->SetPen(wxPen(colour,3));
			dc->DrawSpline(pc,ptArr);
		}


		dc->SetBrush( brush );
		for(int i = 0; i < 360; i++)
		{
			if(ptArr[i].x != 0 && ptArr[i].y != 0)
			{
			dc->SetPen(wxPen(wxColour(0,0,0),2));
			dc->DrawCircle(ptArr[i],radius);												// draw the bullet
			ptArr[i].x = ptArr[i].y = 0;
			}
			else
				break;
		}
}

double Polar::toRad(int angle)
{
	return (PI/180.0)*(angle-90);
}

void Polar::reset()
{
	for(int i = 0; i < 10; i++)
	{
		for(int n = 0; n < WINDDIR; n++)
		{
			windsp[i].wdir[n] = 0;
			windsp[i].count[n] = 0;
			windsp[i].scount[n] = 0;
			windsp[i].wdirMax[n] = 0;
			windsp[i].wdirTotal[n] = 0;
			windsp[i].winddir.clear();
		}
	}
}

void Polar::save()
{
	double data;

	wxFileDialog saveFileDialog(dlg, _("Save Polar File"), _T(""), _T("Polar"),
		_T("OCPN-Format(*.pol)|*.pol|QTVlm(*.pol)|*.pol|Maxsea(*.pol)|*.pol|CVS-Format(*csv)|*.csv"), 
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	int sel = saveFileDialog.GetFilterIndex();
	wxString saveFile = saveFileDialog.GetPath();
	
	if(sel == 3 && !saveFile.EndsWith(_T(".csv")))
	  saveFile += _T(".csv");
	else if(sel != 3 && !saveFile.EndsWith(_T(".pol")))
	  saveFile += _T(".pol");
	
	struct pol save[10];
	for(int i = 0; i < 10; i++)	
		save[i] = windsp[i];

	int end = WINDDIR/2;
	for(int sp = 0; sp < 10; sp++)
	{
		for(int dir = 0; dir < end; dir++)
		{
			save[sp].wdirTotal[dir] += save[sp].wdirTotal[39-dir];
			save[sp].count[dir]     += save[sp].count    [39-dir];
			save[sp].wdirMax[dir]   += save[sp].wdirMax  [39-dir];

			if(save[sp].count[dir] >= 2 && save[sp].count[39-dir] > 0)
				save[sp].wdirTotal[dir] /= 2;

			if(save[sp].wdirMax[dir] > 0 && save[sp].wdirMax[39-dir] > 0)
				save[sp].wdirMax[dir]	/= 2;

			save[sp].wdirTotal [39-dir] = 0;
			save[sp].count[39-dir] = 0;

			//if(save[sp].count[dir] > 0)
			//save[sp].count[dir] = 1;
		}
	}

	wxFileOutputStream output( saveFile);
	wxTextOutputStream polarFile(output);

	if(sel == 0)
	{
		for(int sp = 0; sp < 10; sp++)
		{
			polarFile << (sp+1)*4 << _T(" ");
			for(int dir = 0; dir < end; dir++)
			{
				data = -1;
				switch(mode)
				{
				case 0:
					data = save[sp].wdirTotal[dir];
					break;
				case 1:
				case 2:
				case 3:
					data = save[sp].wdirMax[dir];
					break;
				}
				  polarFile << (dir+3)*8 << _T(" ");
				  if(save[sp].count[dir] > 0 && data >= 0.0)
					  polarFile << wxString::Format(_T("%.2f "),data);
				  else
					polarFile << _T(" ");
				
			}
			polarFile << _T("\n");
		}
	}
	else if(sel == 1)
	{
		polarFile << _T("TWA/TWS\t");
		for(int i = 0 ; i < 11; i++)
			polarFile << wxString::Format(_T("%i\t"),i*4);
		polarFile << _T("60\n");

		for(int dir = -1; dir < end; dir++)
		{
			if(dir == -1)
			{
				for(int z = 0; z < 13; z++)
					polarFile << _T("0\t");
				polarFile << _T("\n");
				continue;
			}
			polarFile << 24+(dir*8) << _T("\t");
			for(int sp = 0; sp < 10; sp++)
			{
				data = -1;
				switch(mode)
				{
				case 0:
					data = save[sp].wdirTotal[dir];
					break;
				case 1:
				case 2:
				case 3:
					data = save[sp].wdirMax[dir];
					break;
				}
				if(sp == 0)
					polarFile << _T("0\t");
				if(save[sp].count[dir] > 0 && data >= 0.0)
					polarFile << wxString::Format(_T("%.2f\t"),data);
				else
					polarFile << _T("\t");

			}
			polarFile << _T("0\n");
		}
	}
	else if(sel == 2)
	{
		polarFile << _T("TWA\t");
		for(int i = 0 ; i < 11; i++)
			polarFile << wxString::Format(_T("%i\t"),i*4);
		polarFile << _T("\n");

		for(int dir = 0; dir < end; dir++)
		{
			polarFile << 24+(dir*8) << _T("\t");
			for(int sp = 0; sp < 10; sp++)
			{
				data = -1;
				switch(mode)
				{
				case 0:
					data = save[sp].wdirTotal[dir];
					break;
				case 1:
				case 2:
				case 3:
					data = save[sp].wdirMax[dir];
					break;
				}
				if(sp == 0)
					polarFile << _T("0\t");
				if(save[sp].count[dir] > 0 && data >= 0.0)
					polarFile << wxString::Format(_T("%.2f\t"),data);
				else
					polarFile << _T("\t");

			}
			polarFile << _T("\n");
		}
	}
	else
	{
	  wxString s = _T(",");	  
	  for(int col = 0; col < dlg->m_gridEdit->GetCols(); col++)
	      s <<  dlg->m_gridEdit->GetColLabelValue(col)+_T(",");
	  s.RemoveLast();
	  polarFile << s << _T("\n");
	  
	  for(int row = 0; row < dlg->m_gridEdit->GetRows(); row++)
	  { 
	    s = wxEmptyString;
	    s <<  dlg->m_gridEdit->GetRowLabelValue(row) << _T(",");
	    for(int col = 0; col < dlg->m_gridEdit->GetCols(); col++)
	      s << dlg->m_gridEdit->GetCellValue(row,col) << _T(",");
	    
	    s.RemoveLast();
	    polarFile << s << _T("\n");
	  }
	}
	output.Close();

	for(int sp = 0; sp < 10; sp++)	
		windsp[sp] = save[sp];
	dlg->m_panelPolar->Refresh();
}

void Polar::showDlg()
{
	filterDlg->ShowModal();
}

void Polar::source(int sel)
{
	if(mode == 0 && sel == 1)
	{
		dlg->m_staticText141->Show();
		dlg->m_choiceCurrentPolar->Show();
	}
	else
	{
		dlg->m_staticText141->Hide();
		dlg->m_choiceCurrentPolar->Hide();
	}
	dlg->Layout();
	dlg->Refresh();
}

void Polar::loadPolar()
{
	wxFileDialog fdlg(dlg,_("Select a Polar-File"),_T(""));
	if(fdlg.ShowModal() == wxID_CANCEL) return;

	reset();
	dlg->m_panelPolar->Refresh();
	dlg->m_gridEdit->ClearGrid();

	setMode(3);
	dlg->m_choiceMode->SetSelection(3);

	wxFileInputStream stream( fdlg.GetPath() );							
	wxTextInputStream in(stream);	
	wxString wdirstr,wsp;

	bool first = true;
	int mode = -1, row = -1;
	int ocpnCol = 0;
	while(!stream.Eof())
	{
		wxString str = in.ReadLine();										// read line by line
		if(stream.Eof()) break;
		if(first)
		{
			if(str.Contains(_T("TWA/TWS")))
				mode = 1;
			else if (str.Contains(_T("TWA")))
				mode = 2;
			else if(str.GetChar(0) == '4')
				mode = 0;
			else
			{
				wxMessageBox(_T("Cannot load this file"));
				return;
			}

			first = false;
			if( mode != 0)
				continue;
		}

		wxStringTokenizer tkz;
		switch(mode)
		{
			case 0:
				{
				wxStringTokenizer tk(str,_T(" "),wxTOKEN_RET_EMPTY);
				tkz = tk;
				}
				break;
			case 1:
			case 2:
				{
				wxStringTokenizer tk(str,_T("\t"),wxTOKEN_RET_EMPTY);	
				tkz = tk;
				}
				break;
		}

		wxString u = tkz.GetNextToken();
		if(u == _T("0") && mode == 1) 
			{ row++; continue; }
		else if(row == -1)
			row++;
		

		if(mode == 1 || mode == 2)
		{
			int col = 0, i = 0; 
			wxString s;
			while(tkz.HasMoreTokens())
			{
				i++;
				if(i > 11) break;
				s = tkz.GetNextToken();
				if(s == _T("0") && (mode == 1 || mode == 2)) continue;
				dlg->m_gridEdit->SetCellValue(row,col,s);
				setValue(s,row,col++);
			}
		}
		else
		{
			wxString s; int ocpnRow = 0;
			while(tkz.HasMoreTokens())
			{
				//if(ocpnCol > 10) break;
				tkz.GetNextToken();
				s = tkz.GetNextToken();
				if(s.GetChar(0) != ' ' && !s.IsEmpty())
				{
					dlg->m_gridEdit->SetCellValue(ocpnRow,ocpnCol,s);
					setValue(s,ocpnRow,ocpnCol);
				}
				ocpnRow++;
			}
			ocpnCol++;
		}
		
		row++;
	}
	dlg->m_panelPolar->Refresh();
}

void Polar::setSentence(wxString sentence)
{
	wxDateTime dt;
	m_NMEA0183 << sentence;

	if(m_NMEA0183.PreParse())
	{
		if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
		{
			if(m_NMEA0183.Parse())
				if(m_NMEA0183.Rmc.SpeedOverGroundKnots != 999.0)
					gpsSpeed = m_NMEA0183.Rmc.SpeedOverGroundKnots;

		}
		else if(m_NMEA0183.LastSentenceIDReceived == _T("VHW"))
		{
			if(m_NMEA0183.Parse())
				speedoSpeed = m_NMEA0183.Vhw.Knots;
		}
		else if(m_NMEA0183.LastSentenceIDReceived == _T("MWV"))
		{
			if(m_NMEA0183.Parse())
			{
				windAngle = m_NMEA0183.Mwv.WindAngle;
				windReference = m_NMEA0183.Mwv.Reference;

				if(m_NMEA0183.Mwv.WindSpeedUnits == 'N')
					windSpeed = wxRound(m_NMEA0183.Mwv.WindSpeed/1.852);
				else if(m_NMEA0183.Mwv.WindSpeedUnits == 'M')
					windSpeed = wxRound((m_NMEA0183.Mwv.WindSpeed*3600)/1852);
				else if(m_NMEA0183.Mwv.WindSpeedUnits == 'K')
					windSpeed = m_NMEA0183.Mwv.WindSpeed;
			}

		}
		else if(m_NMEA0183.LastSentenceIDReceived == _T("VWR"))
		{
			if(m_NMEA0183.Parse())
			{
				windAngle = m_NMEA0183.Vwr.WindDirectionMagnitude;
				windReference = _T("R");

				//if(m_NMEA0183.Vwr..WindSpeedUnits == 'N')
					windSpeed = m_NMEA0183.Vwr.WindSpeedKnots;
				//else if(m_NMEA0183.Mwv.WindSpeedUnits == 'M')
					windSpeed = m_NMEA0183.Vwr.WindSpeedms;
				//else if(m_NMEA0183.Mwv.WindSpeedUnits == 'K')
					windSpeed = m_NMEA0183.Vwr.WindSpeedKnots;
			}

		}
	}
		wxStringTokenizer tkz(sentence,_T(","));
		wxString sentenceInd = tkz.GetNextToken();

		if(sentenceInd == _T("$ERRPM"))
		{

			long Umin = 0;

			dtRPM = wxDateTime::Now();

			wxString source = tkz.GetNextToken();
			wxString engineNr = tkz.GetNextToken();
			wxString speed = tkz.GetNextToken();
			wxString pitch = tkz.GetNextToken();

			speed.ToLong(&Umin);

			if(Umin != 0L)
			{
				if(source == _T("E"))
				{
					if(!engineRunning)
					{
						engineRunning = true;
						setEngineStatus(_T("ON"));
					}
				}
			}
			else
			{
				if(engineRunning)
				{
					engineRunning = false;
					setEngineStatus(_T("OFF"));
				}
			}

		}
}

////////////// Filter  Dialog ////////////////////////

FilterDlg::FilterDlg( PolarDialog* parent, Polar* polar, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	dlg = parent;
	this->polar = polar;
	opt = polar->opt;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook6 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel4 = new wxPanel( m_notebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer8->Add( 0, 20, 0, 0, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxMax = new wxCheckBox( m_panel4, wxID_ANY, _("Use Max-Speed only"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMax->SetValue(true); 
	fgSizer2->Add( m_checkBoxMax, 0, wxALL, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxAverage = new wxCheckBox( m_panel4, wxID_ANY, _("Use Average from all speed-data"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxAverage, 0, wxALL, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxRangePercent = new wxCheckBox( m_panel4, wxID_ANY, _("Range from maximum speed to minus"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxRangePercent->SetToolTip( _("Example:\nMax-Speed = 2,5 kts\nSelected 10 % = 0,25 kts\nRange from 2,25 to 2,5 kts") );
	
	fgSizer2->Add( m_checkBoxRangePercent, 0, wxALL, 5 );
	
	wxString m_choice6Choices[] = { _T("5 %"), _T("10 %"), _T("15 %"), _T("20 %"), _T("25 %"), _T("50 %"), _T("75 %") };
	int m_choice6NChoices = sizeof( m_choice6Choices ) / sizeof( wxString );
	m_choice6 = new wxChoice( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice6NChoices, m_choice6Choices, 0 );
	m_choice6->SetSelection( 1 );
	fgSizer2->Add( m_choice6, 0, wxRIGHT|wxLEFT, 5 );
	
	
	bSizer8->Add( fgSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	m_panel4->SetSizer( bSizer8 );
	m_panel4->Layout();
	bSizer8->Fit( m_panel4 );
	m_notebook6->AddPage( m_panel4, _("Range"), true );
	m_panel33 = new wxPanel( m_notebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer54 = new wxBoxSizer( wxVERTICAL );
	
//	m_staticText152 = new wxStaticText( m_panel33, wxID_ANY, _("Work in irogress"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText152->Wrap( -1 );
//	bSizer54->Add( m_staticText152, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer54->Add( 0, 40, 0, 0, 5 );
	
	fgSizer50 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer50->SetFlexibleDirection( wxBOTH );
	fgSizer50->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	bSizer54->Add( fgSizer50, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	m_panel33->SetSizer( bSizer54 );
	m_panel33->Layout();
	bSizer54->Fit( m_panel33 );
	m_notebook6->AddPage( m_panel33, _("Sails"), false );
	m_panel34 = new wxPanel( m_notebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer52 = new wxBoxSizer( wxVERTICAL );
	
//	m_staticText153 = new wxStaticText( m_panel34, wxID_ANY, _("Work in progess"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText153->Wrap( -1 );
//	bSizer52->Add( m_staticText153, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer541;
	bSizer541 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText154 = new wxStaticText( m_panel34, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText154->Wrap( -1 );
	bSizer541->Add( m_staticText154, 0, wxALL, 5 );
	
	
	bSizer52->Add( bSizer541, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer52->Add( 0, 0, 0, 0, 5 );
	
	
	m_panel34->SetSizer( bSizer52 );
	m_panel34->Layout();
	bSizer52->Fit( m_panel34 );
	m_notebook6->AddPage( m_panel34, _("Wavehight"), false );
	
	bSizer51->Add( m_notebook6, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer11 = new wxStdDialogButtonSizer();
	m_sdbSizer11OK = new wxButton( this, wxID_OK );
	m_sdbSizer11->AddButton( m_sdbSizer11OK );
	m_sdbSizer11Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer11->AddButton( m_sdbSizer11Cancel );
	m_sdbSizer11->Realize();
	
	bSizer51->Add( m_sdbSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	this->SetSizer( bSizer51 );
	this->Layout();
	
	this->Centre( wxBOTH );
	this->Centre( wxBOTH );
	
	// Connect Events
//	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( FilterDlg::init ) );
	m_sdbSizer11OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterDlg::OnOKButtonClick ), NULL, this );
	init();
}

FilterDlg::~FilterDlg()
{
	// Disconnect Events
//	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( FilterDlg::PolarDlgOnInitDialog ) );
	m_sdbSizer11OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterDlg::OnOKButtonClick ), NULL, this );
	
}
void FilterDlg::init()
{
	if(opt->abrSails.Count() != 0)
	{
	fgSizer50->Clear(true);
	m_panel33->Layout();

	for(unsigned int i = 0; i < 14; i++)
	{
		checkboxSails[i] = new wxCheckBox( m_panel33, wxID_ANY,opt->abrSails.Item(i) , wxDefaultPosition, wxDefaultSize, 0 );
		checkboxSails[i]->SetValue( false );
		checkboxSails[i]->SetToolTip( opt->sailsName.Item(i) );

		fgSizer50->Add( checkboxSails[i] , 0, 0, 5 );
	}
	/*	m_buttonSailsReset = new wxButton( m_panel33, wxID_ANY, _("none"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSailsReset->SetToolTip(_("Reset"));
	m_buttonSailsReset->SetMinSize( wxSize( 40,15 ) );
	//	m_buttonSailsReset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookDialog::OnButtonClickResetSails ), NULL, this );
	fgSizer50->Add( m_buttonSailsReset, 0, 0, 5 );
	*/
	fgSizer50->SetVGap(opt->rowGap);
	fgSizer50->SetHGap(opt->colGap);
	}

	wxString d;
	switch(0)
	{
	case 0:	d = opt->meter; break;
	case 1: d = opt->feet; break;
	}

	for(int i = 0; i < 10; i++)
	{
		wxBoxSizer* bSizerWave = new wxBoxSizer( wxHORIZONTAL );

		wxStaticText* m_staticText = new wxStaticText( m_panel34, wxID_ANY, _("Wind:"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText->Wrap( -1 );
		bSizerWave->Add( m_staticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

		m_staticTextKnot = new wxStaticText( m_panel34, wxID_ANY, wxString::Format(_T("%i"),(i+1)*4), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticTextKnot->Wrap( -1 );
		bSizerWave->Add( m_staticTextKnot, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		m_staticTextUnitKnots = new wxStaticText( m_panel34, wxID_ANY, _("kts"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticTextUnitKnots->Wrap( -1 );
		bSizerWave->Add( m_staticTextUnitKnots, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		wxStaticLine* m_staticlinew = new wxStaticLine( m_panel34, wxID_ANY, wxDefaultPosition, wxSize( 1,-1 ), wxLI_HORIZONTAL );
		bSizerWave->Add( m_staticlinew, 0, wxEXPAND | wxALL, 5 );

		m_staticText = new wxStaticText( m_panel34, wxID_ANY, _("Wavehight from"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText->Wrap( -1 );
		bSizerWave->Add( m_staticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );


		m_choiceChoicesFrom[i].Add(_("any"));
		for(int n = 0; n < 10; n++)
		{ m_choiceChoicesFrom[i].Add(wxString::Format(_T("%.2f"),n*0.5)); }
		//int m_choice40NChoices = sizeof( m_choice40Choices ) / sizeof( wxString );
		checkboxWaveFrom[i] = new wxChoice( m_panel34, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceChoicesFrom[i], 0 );
		checkboxWaveFrom[i]->SetSelection( 0 );
		bSizerWave->Add( checkboxWaveFrom[i], 0, wxALL, 5 );

		m_staticText = new wxStaticText( m_panel34, wxID_ANY, _("to"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText->Wrap( -1 );
		bSizerWave->Add( m_staticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		wxArrayString m_choiceChoicesTo[10];
		m_choiceChoicesTo[i].Add(_("any"));
		for(int n = 0; n < 10; n++)
		{ m_choiceChoicesTo[i].Add(wxString::Format(_T("%.2f"),(n+1)*0.5)); }
		checkboxWaveTo[i] = new wxChoice( m_panel34, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceChoicesTo[i], 0 );
		checkboxWaveTo[i]->SetSelection( 0 );
		bSizerWave->Add( checkboxWaveTo[i], 0, wxALL, 5 );

		m_staticTextUnitHeight = new wxStaticText( m_panel34, wxID_ANY, d, wxDefaultPosition, wxDefaultSize, 0 );
		m_staticTextUnitHeight->Wrap( -1 );
		bSizerWave->Add( m_staticTextUnitHeight, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		bSizer52->Add( bSizerWave, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	}
	m_panel34->SetSizer( bSizer52 );
	m_panel34->Layout();
	bSizer52->Fit( m_panel34 );

	m_panel33->Layout();
	this->Layout();

	this->m_notebook6->RemovePage(2);
//	if(opt->abrSails.Count() == 0)
//		this->m_notebook6->RemovePage(1);
}

void FilterDlg::OnOKButtonClick( wxCommandEvent& event )
{
	if(polar->sails.Count() > 0)
		polar->sails.Clear();


	polar->filterSails = false;

	for(int i =  0; i < 14; i++)
		if(checkboxSails[i]->IsChecked())
		{
			polar->sails.Add(opt->sailsName.Item(i));
			polar->filterSails = true;
		}

		this->Hide();
}

///////////////////////////// Collect Dialog ///////////////////////////////////
CollectDlg::CollectDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText9 = new wxStaticText( this, wxID_ANY, _("Collecting Data from: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer11->Add( m_staticText9, 0, wxALL, 5 );

	m_staticTextFile = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFile->Wrap( -1 );
	bSizer11->Add( m_staticTextFile, 0, wxALL, 5 );


	bSizer10->Add( bSizer11, 0, 0, 5 );

	m_gauge1 = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 300,-1 ), wxGA_HORIZONTAL );
	m_gauge1->SetValue( 0 ); 
	bSizer10->Add( m_gauge1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer10 );
	this->Layout();

	this->Centre( wxBOTH );
	this->Centre( wxBOTH );
}

CollectDlg::~CollectDlg()
{
}


