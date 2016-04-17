/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  sweepplot Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2015 by Sean D'Epagnier                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include "sweepplot_pi.h"
#include "SweepPlotDialog.h"
#include "PreferencesDialog.h"
#include "AboutDialog.h"

PreferencesDialog::PreferencesDialog(wxWindow* parent, sweepplot_pi &_sweepplot_pi)
    : PreferencesDialogBase(parent), m_sweepplot_pi(_sweepplot_pi)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return;

#define ADD_CB(NAME)                                    \
    m_cbStates.push_back(cbState(m_cb##NAME, _T(#NAME)));

    ADD_CB(SOG);
    ADD_CB(PDS10);
    ADD_CB(PDS60);
    ADD_CB(COG);
    ADD_CB(PDC10);
    ADD_CB(PDC60);
    ADD_CB(HDG);
    ADD_CB(CourseFFTWPlot);

    pConf->SetPath ( _T ( "/Settings/SweepPlot" ) );

    for(std::list<cbState>::iterator it = m_cbStates.begin(); it != m_cbStates.end(); it++)
        it->cb->SetValue(pConf->Read(_T("Plot ") + it->name, it->cb->GetValue()));

#if wxCHECK_VERSION(3,0,0)
    m_fpPlotFont->SetSelectedFont(pConf->Read(_T("PlotFont"), wxToString(m_fpPlotFont->GetSelectedFont())));
#else
    wxLogMessage(_T("sweepplot_pi: cannot save and load fonts using wxwidgets version < 3"));
#endif    
    m_sPlotMinHeight->SetValue(pConf->Read(_T("PlotMinHeight"), m_sPlotMinHeight->GetValue()));
    m_cColors->SetSelection(pConf->Read(_T("PlotColors"), m_cColors->GetSelection()));
    m_sPlotTransparency->SetValue(pConf->Read(_T("PlotTransparency"), m_sPlotTransparency->GetValue()));
    m_cPlotStyle->SetSelection(pConf->Read(_T("PlotStyle"), m_cPlotStyle->GetSelection()));
    m_cbShowTitleBar->SetValue(pConf->Read(_T("PlotShowTitleBar"), m_cbShowTitleBar->GetValue()));
    
    bool bvalue;
    int ivalue;
    pConf->Read(_T("CoursePrediction"), &bvalue, false);
    m_cbCoursePrediction->SetValue(bvalue);

    pConf->Read(_T("CoursePredictionBlended"), &bvalue, false);
    m_cbCoursePredictionBlended->SetValue(bvalue);

    pConf->Read(_T("CoursePredictionLength"), &ivalue, 10);
    m_sCoursePredictionLength->SetValue(ivalue);

    pConf->Read(_T("CoursePredictionSeconds"), &ivalue, 10);
    m_sCoursePredictionSeconds->SetValue(ivalue);
}

PreferencesDialog::~PreferencesDialog()
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return;

    pConf->SetPath ( _T ( "/Settings/SweepPlot" ) );

    for(std::list<cbState>::iterator it = m_cbStates.begin(); it != m_cbStates.end(); it++)
        pConf->Write(_T("Plot ") + it->name, it->cb->GetValue());

#if wxCHECK_VERSION(3,0,0)
    pConf->Write(_T("PlotFont"), m_fpPlotFont->GetSelectedFont());
#endif
    pConf->Write(_T("PlotMinHeight"), m_sPlotMinHeight->GetValue());
    pConf->Write(_T("PlotColors"), m_cColors->GetSelection());
    pConf->Write(_T("PlotTransparency"), m_sPlotTransparency->GetValue());
    pConf->Write(_T("PlotStyle"), m_cPlotStyle->GetSelection());
    pConf->Write(_T("PlotShowTitleBar"), m_cbShowTitleBar->GetValue());

    pConf->Write(_T("CoursePrediction"), m_cbCoursePrediction->GetValue());
    pConf->Write(_T("CoursePredictionBlended"), m_cbCoursePredictionBlended->GetValue());
    pConf->Write(_T("CoursePredictionLength"), m_sCoursePredictionLength->GetValue());
    pConf->Write(_T("CoursePredictionSeconds"), m_sCoursePredictionSeconds->GetValue());
}

void PreferencesDialog::OnPDS( wxCommandEvent& event )
{
        wxMessageDialog mdlg(this, _("\
Position Determined Speed (PDS) finds the speed of the vessel by comparing the current position to the position \
from the past.  For example PDS10 (10 seconds) takes the position from 10 seconds before, and determines the \
vessel speed by taking the distance from where it was then.\n\n\
This method filters the data giving a much steadier reading, but also shows a comparison of \
useful speed traveled rather than the immediate gps speed.  Consider autosteering that is \
under or over dampened, or the case of large waves, even with an ideal autopilot. \
This will cause the boat to not travel in a straight line, but instead in an S curve.\
The gps speed will read higher than the position determined speed."),
                             _("Positon Determined Speed"), wxOK | wxICON_INFORMATION);
        mdlg.ShowModal();
}

void PreferencesDialog::OnAbout( wxCommandEvent& event )
{
    AboutDialog dlg(this);
    dlg.ShowModal();
}

void PreferencesDialog::PlotChange()
{
    m_sweepplot_pi.m_SweepPlotDialog->Refresh();
    m_sweepplot_pi.m_SweepPlotDialog->SetupPlot();
}
