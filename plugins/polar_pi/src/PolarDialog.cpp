#include "PolarDialog.h"
#include "polar_pi.h"
#include "Polar.h"

#include <math.h>  
#define PI 3.14159265

PolarDialog::PolarDialog( wxWindow* parent, polar_pi* pol )
:
PolarDlg( parent )
{
	plugin = pol;
	polar = new Polar(this);

	timer = new wxTimer (this,500);
	this->Connect( timer->GetId(),wxEVT_TIMER, wxTimerEventHandler( PolarDialog::OnTimer ),NULL,this);

	timerRPM = new wxTimer (this,501);
	this->Connect( timerRPM->GetId(),wxEVT_TIMER, wxTimerEventHandler( PolarDialog::OnTimerRPM ),NULL,this);
}

PolarDialog::~PolarDialog(void)
{
	timer->Stop();
	this->Disconnect( 500 ,wxEVT_TIMER, wxTimerEventHandler( PolarDialog::OnTimer ),NULL,this);
	this->Disconnect( 501 ,wxEVT_TIMER, wxTimerEventHandler( PolarDialog::OnTimerRPM ),NULL,this);
	delete timerRPM;
	delete timer;
}

void PolarDialog::OnToggleButtonRecord( wxCommandEvent& event )
{
	setToggleButtonRecord(event.IsChecked());
}

void PolarDialog::setToggleButtonRecord(bool status)
{
	const static wxString startstop[2] = {_("Start"),_("Stop")};
	polar->nmea = status;
	this->m_toggleBtnRecord->SetLabel(_("Record ")+startstop[status]);
	if(!status)
		timer->Stop();
	else
		timer->Start(1000);
}

void PolarDialog::OnButtonClickClearData(wxCommandEvent& event)
{
	m_gridEdit->ClearGrid();
	polar->reset();
	m_panelPolar->Refresh();
}

void PolarDialog::OnTimerRPM(wxTimerEvent& event)
{
	if(polar->mode == 1 && polar->engineRunning)
	{
		wxTimeSpan sp = wxDateTime::Now().Subtract(polar->dtRPM);
		if(sp.GetSeconds() > 5)
		{
			polar->engineRunning = false;
			polar->setEngineStatus(_T("OFF"));
		}
	}
}

void PolarDialog::OnTimer(wxTimerEvent& event)
{
	event.Skip();
	timer->Stop();
	
	if(!polar->insert())
		polar->timeout--;

	if(polar->timeout == 0)
	{
		polar->windAngle = -1;
		polar->windReference = wxEmptyString;
		polar->windSpeed = -1;
		polar->speedoSpeed = -1;
		polar->gpsSpeed = -1;
	}
	timer->Start(1000);
}

void PolarDialog::PolarDlgOnInitDialog( wxInitDialogEvent& event )
{
//	m_splitter1->Unsplit();
/*	m_gridEdit->GetGridCornerLabelWindow()->SetFocus();
	m_gridEdit->GetGridCornerLabelWindow()->SetForegroundColour(wxColour(0,0,0));
	m_gridEdit->GetGridCornerLabelWindow()->SetLabel(wxString((_("TWA/TWS"))));
	m_gridEdit->GetGridCornerLabelWindow()->Refresh();
*/
}

void PolarDialog::OnPaintPolar( wxPaintEvent& event )
{
	wxPaintDC dc(m_panelPolar);
	polar->dc = &dc;
	polar->createDiagram(dc);
    polar->Render();
}

void PolarDialog::OnSizePolar( wxSizeEvent& event )
{
	m_panelPolar->Fit();
	m_panelPolar->Refresh();
}

void PolarDialog::OnSizePolarDlg( wxSizeEvent& event )
{
	Layout();
	Refresh();
//	m_panelPolar->Fit();
//	m_panelPolar->Refresh();
}

void PolarDialog::OnButtonCreatePolar( wxCommandEvent& event )
{
	polar->createPolar();
}

void PolarDialog::OnChoicePolarKnots( wxCommandEvent& event )
{
	polar->knots = wxAtof(event.GetString());
	m_panelPolar->Refresh();
}

void PolarDialog::OnChoiceDegreesPolar( wxCommandEvent& event )
{
	polar->degrees = wxAtof(event.GetString());
	m_panelPolar->Refresh();
}

void PolarDialog::OnButtonClickSavePolar( wxCommandEvent& event )
{
	polar->save();
}

void PolarDialog::OnChoiceWindPolar( wxCommandEvent& event )
{
	m_panelPolar->Refresh();
}

void PolarDialog::OnButtonClickFilterPolar( wxCommandEvent& event )
{
	polar->showDlg();
}

void PolarDialog::OnChoiceSourcePolar( wxCommandEvent& event )
{
	polar->source(event.GetSelection());
}

void PolarDialog::OnSplitterSashPosChanged( wxSplitterEvent& event )
{
	m_panelPolar->Refresh();
	m_splitter1->GetWindow2()->Update();
}

void PolarDialog::OnChoiceMode( wxCommandEvent& event )
{
	static int sel = -1;

	if(event.GetSelection() == 0 && plugin->opt->abrSails.Count() == 0)
	{
		wxMessageBox(_("The plugin logbookkonni isn't installed\n\nIt's needed to collect wind/speed-data from all available logbooks\n\nThe create-button is hidden and the mode isn't useable"));
		m_choiceMode->SetSelection((sel==-1)?sel = 1:sel);
		return;
	}

	sel = event.GetSelection();
	polar->setMode(sel);
}

void PolarDialog::OnGridCellChange( wxGridEvent& event )
{
	wxString s = m_gridEdit->GetCellValue(event.GetRow(),event.GetCol());
	polar->setValue(s,event.GetRow(),event.GetCol());
	bool comma = false;

	if(!s.IsEmpty())
	{
		if(s.Contains(_T(",")))
		{
			s.Replace(_T(","),_T("."));
			comma = true;
		}
		s = wxString::Format(_T("%.2f"),wxAtof(s));
		if(comma)
			s.Replace(_T("."),_T(","));

		m_gridEdit->SetCellValue(s,event.GetRow(),event.GetCol());
	}
	event.Skip();
}

void PolarDialog::OnSizesplitter1( wxSizeEvent& event )
{
	int pos = this->m_splitter1->GetSashPosition();
	this->m_splitter1->SetSashPosition(pos+1);
	this->m_splitter1->SetSashPosition(pos);
}

void PolarDialog::OnButtonClickLoad( wxCommandEvent& event )
{
	polar->loadPolar();
}
