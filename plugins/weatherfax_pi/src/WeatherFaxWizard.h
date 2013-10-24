/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  weather fax Plugin
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
 ***************************************************************************/

#include "WeatherFaxUI.h"

class WeatherFax;
class DecoderThread;
class FaxDecoder;

class WeatherFaxWizard : public WeatherFaxWizardBase
{
public:
    WeatherFaxWizard( WeatherFaxImage &img, FaxDecoder *decoder,
                    WeatherFax &parent,
                    WeatherFaxImageCoordinateList &coords,
                    wxString newcoordbasename);

    ~WeatherFaxWizard();

    wxTimer m_tDecoder;
    DecoderThread *m_thDecoder;
    FaxDecoder *m_decoder;

    void MakeNewCoordinates();
    void OnDecoderTimer( wxTimerEvent & );

    void OnSetSizes( wxInitDialogEvent& event );
    void UpdateMappingControls();
    void OnStopDecoding( wxCommandEvent& event );
    void OnPaintPhasing( wxPaintEvent& event );
    void OnWizardPageChanged( wxWizardEvent& event );
    void OnMappingChoice( wxCommandEvent& event );
    void GetMappingPolar(bool onlyequator);
    void GetMappingFixedFlat();
    void OnGetMapping( wxCommandEvent& event );
    void OnGetEquator( wxCommandEvent& event );
    void OnBitmapClickPage2( wxMouseEvent& event );
    void OnBitmapClickPage3( wxMouseEvent& event );
    void OnCoordSet( wxCommandEvent& event );
    void OnCoordText( wxCommandEvent& event );
    void OnRemoveCoords( wxCommandEvent& event );
    void StoreMappingParams();
    bool ApplyMapping();
    
    void SetUnMappedCoordRanges();
    void SetCoordRanges();
    void OnUpdateMapping( wxSpinEvent& event ) { Refresh(); }
    void OnUpdateMapping( wxCommandEvent& event ) { Refresh(); }
    void OnInformation( wxCommandEvent& event );

    void StoreCoords();

protected:

    void UpdatePage1();
    void UpdatePage1Rotation ( wxCommandEvent& event );
    void UpdatePage1( wxCommandEvent& event );
    void UpdatePage1( wxScrollEvent& event );

    wxString NewCoordName();
    void SetCoords(int index);
    void OnSpin( wxSpinEvent& event );
    void OnPaintImage( wxPaintEvent& event);

    WeatherFax &m_parent;
    WeatherFaxImage &m_wfimg;
    WeatherFaxImageCoordinates *&m_curCoords;

    wxString m_NewCoordBaseName;

    WeatherFaxImageCoordinates *m_newCoords;
    WeatherFaxImageCoordinateList &m_Coords;

    bool m_skippaint;

    /* page 2 */
    int m_SelectedIndex;
};
