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

#include "LogbookDialog.h"
#include "Logbook.h"
#include "LogbookHTML.h"
#include "logbook_pi.h"
#include "Options.h"
#include "LogbookOptions.h"

#include <map>

Polar::Polar(LogbookDialog* parent)
{
	sails.Clear();

	dlg = parent;
	opt = dlg->logbookPlugIn->opt;
	polarDlg = new PolarDlg(dlg,this);

	dlg->m_choiceRingsPolar->SetSelection(1);
	dlg->m_choiceDegreesPolar->SetSelection(0);
	knots = wxAtoi(dlg->m_choiceRingsPolar->GetString(dlg->m_choiceRingsPolar->GetSelection()));
	degrees = wxAtoi(dlg->m_choiceDegreesPolar->GetString(dlg->m_choiceDegreesPolar->GetSelection()));
	for(int i = 4; i < 41; i += 4)
		dlg->m_choiceWindPolar->Append(wxString::Format(_T("%i knots"),i));
	dlg->m_panelPolar->Layout();
	dlg->m_buttonSavePolar->Enable(false);

	filterSails = false;
}

Polar::~Polar(void)
{
	delete polarDlg;
}

void Polar::createDiagram(wxDC& dc)
{
	center = dlg->m_panelPolar->GetSize();
	center.x /= 2;
	int f = dlg->bSizerPolar->GetSize().y;
	center.y = (center.y / 2) + (dlg->bSizerPolar->GetSize().GetHeight() / 2);
	rSpeed[0] = center.y - dlg->bSizerPolar->GetSize().GetHeight()-40;
	dist = rSpeed[0] / knots;
	for(int i = 0; i < knots; i++)
		rSpeed[i] = wxRound(dist*(i+1));
}

void Polar::Render()
{
	dc->DrawText(wxT("Testing, work in progress..."), 40, 60); 

	dc->SetPen( wxPen( wxColor(0,0,0), 1 ) ); 
	dc->SetBrush( wxBrush(wxColour(255,0,0)) );	
	dc->DrawCircle( wxPoint(40,90), 5);
	dc->DrawText(wxT("less than 3 entries found"), 60, 80); 
	dc->SetBrush( wxBrush(wxColour(255,255,0)) );
	dc->DrawCircle( wxPoint(40,110), 5);
	dc->DrawText(wxT("less than 5 entries found"), 60, 100); 
	dc->SetBrush( wxBrush(wxColour(0,255,0)) );
	dc->DrawCircle( wxPoint(40,130), 5);
	dc->DrawText(wxT("more than 10 entries found"), 60, 120); 
	dc->SetBrush( wxBrush(wxColour(255,255,255)) );

	int xknt, yknt;
	for(int i = knots-1; i >= 0; i--)
	{
		xknt = wxRound(cos((PI/180.0)*(0-90))*rSpeed[i]+center.x);
		yknt = wxRound(sin((PI/180.0)*(0-90))*rSpeed[i]+center.y);

		if(knots == 18 || knots == 24)
		{
			if((knots == 18 && ((i+1) % 2) == 0) || (knots == 24 && ((i+1) % 3) == 0))
			{
				dc->DrawCircle( wxPoint(center.x,center.y), rSpeed[i]);
				dc->DrawText(wxString::Format(_T("%i"),i+1),wxPoint(xknt,yknt-10));
			}
		}
		else
		{
			dc->DrawCircle( wxPoint(center.x,center.y), rSpeed[i]);
			dc->DrawText(wxString::Format(_T("%i"),i+1),wxPoint(xknt,yknt-10));
		}
	}

	int angle, d;
	if(degrees == 8) { angle = 24; d = 24; } else { angle = 20; d = 20; }
	for(; angle <= 360-d; angle += degrees)
	{
		int xt = wxRound(cos((PI/180.0)*(angle-90))*(rSpeed[(int)knots-1]+20)+center.x);
		int yt = wxRound(sin((PI/180.0)*(angle-90))*(rSpeed[(int)knots-1]+20)+center.y);

		dc->DrawLine(wxPoint(center.x,center.y),wxPoint(xt,yt));
		if(angle <= 180)
			dc->DrawText(wxString::Format(_T("%i°"),angle), wxPoint(xt,yt)); 
		else
			dc->DrawText(wxString::Format(_T("%i°"),angle), wxPoint(xt-25,yt)); 

	}

	dc->DrawText(_("Filtered by:"), center.x+rSpeed[(int)knots-1]+60, 60);

	wxString sail = wxEmptyString;  
	unsigned int n = 0, y = 80;

	for(unsigned i = 0; i < 14; i++)
	{
		if(polarDlg->checkboxSails[i]->IsChecked())
		{
			dc->DrawText(opt->sailsName.Item(i), center.x+rSpeed[(int)knots-1]+60, y);
			y += 20;
		}
	}

	createSpeedBullets();
}

void Polar::createPolar()
{
	wxArrayString files;												
	wxString path = dlg->data;
	wxDir dir;
	dir.GetAllFiles(path,&files,_T("*logbook*.txt"),wxDIR_FILES);				// collect all logbook-files
	bool nextline = false;

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

	for(unsigned int i = 0; i < files.Count(); i++)
	{
		wxFileName fn(files[i]);
		wxFileInputStream stream( files[i] );									// get trough the files
		wxTextInputStream in(stream);	
		wxString wdirstr,wsp;
		int winddir = -1;
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

						if(s.Contains(opt->windmeter))
							wsp = wxRound((wspd*3600)/1852);
						else if(s.Contains(opt->windkmh))
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
							int ok = 0;
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
				}
				wsp /= 4;
				winddir = wxRound((winddir-24)/8);
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
					if(percent >= -10.0)
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

	dlg->m_panelPolar->Refresh();
	dlg->m_buttonSavePolar->Enable();
}

void Polar::createSpeedBullets()
{
	int radius = 5;
	double length = dist;												
	dc->SetPen( wxPen( wxColor(0,0,0), 1 ) ); 
	int c= 0;
	int end;;
	int xt, yt, pc;
	wxPoint ptArr[360];

	int sel = dlg->m_choiceWindPolar->GetSelection();
	if(sel != 0) 
	{ sel -= 1; end = sel+1; }
	else
	{ sel = 0; end = 10; }

	for(int wsp = sel; wsp < end; wsp++)									// go thru all winddirection-structures depending on choiocebox degrees
	{
		pc = 0;
		for(int dir = 0; dir < WINDDIR; dir++)
		{
			if(windsp[wsp].count[dir] <= 0) continue;							// no data ? continue

			if(windsp[wsp].count[dir] < 3)
				dc->SetBrush( wxBrush(wxColour(255,0,0)) );							// set bulletcolor
			else if( windsp[wsp].count[dir]  >= 3 && windsp[wsp].count[dir] < 5)
				dc->SetBrush( wxBrush(wxColour(255,255,0)) );	
			else
				dc->SetBrush( wxBrush(wxColour(0,255,0)) );

			int linelength = wxRound((windsp[wsp].wdirTotal[dir]/windsp[wsp].count[dir])*length);			// calculate distance from centerpoint for speed
			xt = wxRound(cos(toRad(dir*8+24))*linelength+center.x);					// calculate the point for the bullet
			yt = wxRound(sin(toRad(dir*8+24))*linelength+center.y);
			wxPoint pt(xt,yt);
			if(pt != wxPoint(center.x,center.y))
				ptArr[pc++] = pt;
			dc->DrawCircle(pt,radius);												// draw the bullet
		}
		if(pc > 2)																	//Draw splines, needs min. 3 points
			dc->DrawSpline(pc,ptArr);
	}
}

double Polar::toRad(int angle)
{
	return (PI/180.0)*(angle-90);
}

void Polar::save()
{
	wxFileDialog saveFileDialog(dlg, _("Save Polar File"), _T(""), _T("Polar"),
		_T("OCPN-Format(*.pol)|*.pol"), 
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	struct pol save[10];
	for(int i = 0; i < 10; i++)	
		save[i] = windsp[i];

	int end = WINDDIR/2;
	for(int sp = 0; sp < 10; sp++)
	{
		for(int dir = 0; dir < end; dir++)
		{
			save[sp].wdirTotal[dir] += save[sp].wdirTotal[39-dir];
			save[sp].count[dir]     += save[sp].count[39-dir];

			save[sp].wdirTotal [39-dir] = 0;
			save[sp].count[39-dir] = 0;
		}
	}


	for(int i = 0; i < 10; i++)	
		windsp[i] = save[i];
	dlg->m_panelPolar->Refresh();

	wxFileOutputStream output( saveFileDialog.GetPath() );
	wxTextOutputStream polarFile(output);

	for(int i = 0; i < 10; i++)
	{
		polarFile << (i+1)*4 << _T(" ");
		for(int n = 0; n < end; n++)
		{
			polarFile << (n+3)*8 << _T(" ");
			if(save[i].count[n] != 0)
				polarFile << wxString::Format(_T("%.2f "),save[i].wdirTotal[n]/save[i].count[n]);
			else
				polarFile << _T(" ");
		}
		polarFile << _T("\n");
	}
	output.Close();
}

void Polar::showDlg()
{
	polarDlg->ShowModal();
}

void Polar::source(int sel)
{
	dlg->m_choiceCurrentPolar->Show((sel==0)?false:true);
	dlg->m_staticText141->Show((sel==0)?false:true);
	dlg->m_panelPolar->Layout();
}

////////////// Filter  Dialog ////////////////////////
#include "logbook.h"

PolarDlg::PolarDlg( LogbookDialog* parent, Polar* polar, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	dlg = parent;
	opt = dlg->logbookPlugIn->opt;
	this->polar = polar;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxVERTICAL );

	m_notebook6 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel33 = new wxPanel( m_notebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer54 = new wxBoxSizer( wxVERTICAL );

	m_staticText152 = new wxStaticText( m_panel33, wxID_ANY, _("Work in irogress"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText152->Wrap( -1 );
	bSizer54->Add( m_staticText152, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


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

	m_staticText153 = new wxStaticText( m_panel34, wxID_ANY, _("Work in progess"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText153->Wrap( -1 );
	bSizer52->Add( m_staticText153, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer52->Add( 0, 0, 0, 0, 5 );

	m_panel34->SetSizer( bSizer52 );
	m_panel34->Layout();
	bSizer52->Fit( m_panel34 );
	m_notebook6->AddPage( m_panel34, _("Wavehight"), true );

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

	// Connect Events
	m_sdbSizer11OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolarDlg::OnOKButtonClick ), NULL, this );

	init();

}

PolarDlg::~PolarDlg()
{
	// Disconnect Events
	m_sdbSizer11OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolarDlg::OnOKButtonClick ), NULL, this );
	//	m_buttonSailsReset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LogbookDialog::OnButtonClickResetSails ), NULL, this );	
}

void PolarDlg::init()
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

	wxString d;
	switch(opt->showWaveSwell)
	{
	case 0:	d = opt->meter; break;
	case 1: d = opt->feet; break;
	case 2: d = opt->fathom; break;
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

}

void PolarDlg::OnOKButtonClick( wxCommandEvent& event )
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


