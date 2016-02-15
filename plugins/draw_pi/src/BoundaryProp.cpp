/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Boundary Properties
 * Author:   Jon Gough
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
 **************************************************************************/

#include "BoundaryProp.h"
#include "Boundary.h"
#include "ocpn_draw_pi.h"

extern BoundaryList         *g_pBoundaryList;
extern unsigned int         g_uiFillTransparency;
extern int                  g_iInclusionBoundarySize;


BoundaryProp::BoundaryProp()
{
    //ctor
}

//BoundaryProp::BoundaryProp( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) 
//: PathProp( parent, id, caption, pos, size, style )
BoundaryProp::BoundaryProp( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) 
: ODPathPropertiesDialogImpl( parent, id, caption, pos, size, style )
{
    //ctor
    m_staticTextFillColour->Show();
    m_staticTextFillColour->Enable( true );
    m_colourPickerFillColour->Show();
    m_colourPickerFillColour->Enable( true );
    m_staticTextFillTransparency->Show();
    m_staticTextFillTransparency->Enable( true );
#ifdef __WXOSX__
    m_sliderFillTransparency->Show(true);
#else
    m_sliderFillTransparency->Show();
#endif
    m_sliderFillTransparency->Enable( true );
    m_staticTextIncluseionBoundarySize->Show();
    m_staticTextIncluseionBoundarySize->Enable( true );
#ifdef __WXOSX__
    m_sliderInclusionBoundarySize->Show(true);
#else
    m_sliderInclusionBoundarySize->Show();
#endif
    m_sliderInclusionBoundarySize->Enable( true );
    
    m_radioBoxBoundaryType->Show();
    m_radioBoxBoundaryType->Enable( true );
    
    this->GetSizer()->Fit( this );
    this->Layout();
    m_uiFillTransparency = g_uiFillTransparency;
}


BoundaryProp::~BoundaryProp()
{
    //dtor
}

bool BoundaryProp::UpdateProperties( Boundary *pBoundary )
{
    m_colourPickerFillColour->SetColour( m_pBoundary->m_wxcActiveFillColour );
    m_sliderFillTransparency->SetValue( m_pBoundary->m_uiFillTransparency );
    m_sliderInclusionBoundarySize->SetValue( m_pBoundary->m_iInclusionBoundarySize );
    if(m_pBoundary->m_bExclusionBoundary && !m_pBoundary->m_bInclusionBoundary) m_radioBoxBoundaryType->SetSelection( ID_BOUNDARY_EXCLUSION );
    else if(!m_pBoundary->m_bExclusionBoundary && m_pBoundary->m_bInclusionBoundary) m_radioBoxBoundaryType->SetSelection( ID_BOUNDARY_INCLUSION );
    else if(!m_pBoundary->m_bExclusionBoundary && !m_pBoundary->m_bInclusionBoundary) m_radioBoxBoundaryType->SetSelection( ID_BOUNDARY_NIETHER );
    else m_radioBoxBoundaryType->SetSelection( ID_BOUNDARY_EXCLUSION );
    
    ODPathPropertiesDialogImpl::UpdateProperties( pBoundary );
    
    return true;
}

bool BoundaryProp::SaveChanges( void )
{
    if( m_pPath && !m_pPath->m_bIsInLayer ) {
        m_pBoundary->m_wxcActiveFillColour = m_colourPickerFillColour->GetColour();    
        m_pBoundary->m_uiFillTransparency = m_sliderFillTransparency->GetValue();
        m_pBoundary->m_iInclusionBoundarySize = m_sliderInclusionBoundarySize->GetValue();
        switch (m_radioBoxBoundaryType->GetSelection()) {
            case ID_BOUNDARY_EXCLUSION:
                m_pBoundary->m_bExclusionBoundary = true;
                m_pBoundary->m_bInclusionBoundary = false;
                break;
            case ID_BOUNDARY_INCLUSION:
                m_pBoundary->m_bExclusionBoundary = false;
                m_pBoundary->m_bInclusionBoundary = true;
                break;
            case ID_BOUNDARY_NIETHER:
                m_pBoundary->m_bExclusionBoundary = false;
                m_pBoundary->m_bInclusionBoundary = false;
                break;
            default:
                m_pBoundary->m_bExclusionBoundary = true;
                m_pBoundary->m_bInclusionBoundary = false;
                break;
        }
    }
    ODPathPropertiesDialogImpl::SaveChanges();

    return true;
}

