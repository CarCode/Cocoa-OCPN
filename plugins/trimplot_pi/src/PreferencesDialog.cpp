/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  trimplot Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Sean D'Epagnier                                 *
 *   sean at depagnier dot com                                             *
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
 ***************************************************************************
 */

#include "trimplot_pi.h"
#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(wxWindow* parent, trimplot_pi &_trimplot_pi)
    : PreferencesDialogBase(parent), m_trimplot_pi(_trimplot_pi)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return;

    pConf->SetPath ( _T ( "/Settings/TrimPlot" ) );

    bool bvalue;
    double dvalue;
    int ivalue;

    pConf->Read(_T("SpeedPlot"), &bvalue, true);
    m_cbSpeed->SetValue(bvalue);

    pConf->Read(_T("SpeedScale"), &dvalue, 3);
    m_tSpeedScale->SetValue(wxString::Format(_T("%.2f"), dvalue));

    pConf->Read(_T("SpeedSeconds"), &ivalue, 10);
    m_sSpeedSeconds->SetValue(ivalue);


    pConf->Read(_T("CoursePlot"), &bvalue, true);
    m_cbCourse->SetValue(bvalue);

    pConf->Read(_T("CourseScale"), &dvalue, 20);
    m_tCourseScale->SetValue(wxString::Format(_T("%.2f"), dvalue));

    pConf->Read(_T("CourseSeconds"), &ivalue, 10);
    m_sCourseSeconds->SetValue(ivalue);


    pConf->Read(_T("CoursePrediction"), &bvalue, false);
    m_cbCoursePrediction->SetValue(bvalue);

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

    pConf->SetPath ( _T ( "/Settings/TrimPlot" ) );

    double dvalue;

    pConf->Write(_T("SpeedPlot"), m_cbSpeed->GetValue());
    m_tSpeedScale->GetValue().ToDouble(&dvalue);
    pConf->Write(_T("SpeedScale"), dvalue);
    pConf->Write(_T("SpeedSeconds"), m_tSpeedScale->GetValue());

    pConf->Write(_T("CoursePlot"), m_cbCourse->GetValue());
    m_tCourseScale->GetValue().ToDouble(&dvalue);
    pConf->Write(_T("CourseScale"), dvalue);
    pConf->Write(_T("CourseSeconds"), m_tCourseScale->GetValue());

    pConf->Write(_T("CoursePrediction"), m_cbCoursePrediction->GetValue());
    pConf->Write(_T("CoursePredictionLength"), m_sCoursePredictionLength->GetValue());
    pConf->Write(_T("CoursePredictionSeconds"), m_sCoursePredictionSeconds->GetValue());
}

void PreferencesDialog::OnPlotChange( wxCommandEvent& event )
{
    m_trimplot_pi.RepopulatePlots();
}

void PreferencesDialog::OnAbout( wxCommandEvent& event )
{
    AboutDialog dlg(this);
    dlg.ShowModal();
}
