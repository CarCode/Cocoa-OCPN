/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Options Dialog
 * Author:   David Register
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/progdlg.h>
#include <wx/sound.h>
#include <wx/radiobox.h>
#include <wx/listbox.h>
#include <wx/imaglist.h>
#include <wx/display.h>
#include <wx/choice.h>
#include <wx/dirdlg.h>
#if wxCHECK_VERSION(2,9,4) /* does this work in 2.8 too.. do we need a test? */
#include <wx/renderer.h>
#endif
#ifdef __WXGTK__
#include <wx/colordlg.h>
#endif

#include "dychart.h"
#include "chart1.h"

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
extern GLuint g_raster_format;
#endif

#include "chartdbs.h"
#include "options.h"
#include "styles.h"
#include "datastream.h"
#include "multiplexer.h"
#include "FontMgr.h"
#include "OCPN_Sound.h"
#include "NMEALogWindow.h"

#include "ais.h"
#include "AIS_Decoder.h"
#include "AIS_Target_Data.h"

#include "navutil.h"

#ifdef USE_S57
#include "s52plib.h"
#include "s52utils.h"
#include "cm93.h"
#endif

wxString GetOCPNKnownLanguage(wxString lang_canonical, wxString *lang_dir);

extern MyFrame          *gFrame;
extern ChartCanvas      *cc1;
extern wxString         g_PrivateDataDir;

extern bool             g_bShowFPS;

extern bool             g_bShowOutlines;
extern bool             g_bShowChartBar;
extern bool             g_bShowDepthUnits;
extern bool             g_bskew_comp;
extern bool             g_bopengl;
extern bool             g_bsmoothpanzoom;
extern bool             g_bShowMag;
extern double           g_UserVar;
extern int              g_chart_zoom_modifier;
extern int              g_NMEAAPBPrecision;

extern wxString         *pInit_Chart_Dir;
extern wxArrayOfConnPrm *g_pConnectionParams;
extern Multiplexer      *g_pMUX;
extern bool             g_bfilter_cogsog;
extern int              g_COGFilterSec;
extern int              g_SOGFilterSec;

extern PlugInManager    *g_pi_manager;
extern ocpnStyle::StyleManager*   g_StyleManager;

extern wxString         g_SData_Locn;

extern bool             g_bDisplayGrid;

//    AIS Global configuration
extern bool             g_bCPAMax;
extern double           g_CPAMax_NM;
extern bool             g_bCPAWarn;
extern double           g_CPAWarn_NM;
extern bool             g_bTCPA_Max;
extern double           g_TCPA_Max;
extern bool             g_bMarkLost;
extern double           g_MarkLost_Mins;
extern bool             g_bRemoveLost;
extern double           g_RemoveLost_Mins;
extern bool             g_bShowCOG;
extern double           g_ShowCOG_Mins;
extern bool             g_bAISShowTracks;
extern double           g_AISShowTracks_Mins;
extern bool             g_bShowMoored;
extern double           g_ShowMoored_Kts;
extern bool             g_bAIS_CPA_Alert;
extern bool             g_bAIS_CPA_Alert_Audio;
extern wxString         g_sAIS_Alert_Sound_File;
extern bool             g_bAIS_CPA_Alert_Suppress_Moored;
extern bool             g_bShowAreaNotices;
extern bool             g_bDrawAISSize;
extern bool             g_bShowAISName;
extern int              g_Show_Target_Name_Scale;
extern bool             g_bWplIsAprsPosition;

extern int              g_iNavAidRadarRingsNumberVisible;
extern float            g_fNavAidRadarRingsStep;
extern int              g_pNavAidRadarRingsStepUnits;
extern bool             g_bWayPointPreventDragging;

extern bool             g_bPreserveScaleOnX;
extern bool             g_bPlayShipsBells;
extern bool             g_bFullscreenToolbar;
extern bool             g_bTransparentToolbar;

extern int              g_OwnShipIconType;
extern double           g_n_ownship_length_meters;
extern double           g_n_ownship_beam_meters;
extern double           g_n_gps_antenna_offset_y;
extern double           g_n_gps_antenna_offset_x;
extern int              g_n_ownship_min_mm;
extern double           g_n_arrival_circle_radius;

extern bool             g_bEnableZoomToCursor;
extern bool             g_bTrackDaily;
extern bool             g_bHighliteTracks;
extern double           g_TrackIntervalSeconds;
extern double           g_TrackDeltaDistance;
extern double           g_TrackDeltaDistance;
extern int              g_nTrackPrecision;

extern int              g_iSDMMFormat;
extern int              g_iDistanceFormat;
extern int              g_iSpeedFormat;

extern bool             g_bSailing;

extern int              g_cm93_zoom_factor;

extern int              g_COGAvgSec;

extern bool             g_bCourseUp;
extern bool             g_bLookAhead;

extern double           g_ownship_predictor_minutes;
extern double           g_ownship_HDTpredictor_miles;

extern bool             g_bAISRolloverShowClass;
extern bool             g_bAISRolloverShowCOG;
extern bool             g_bAISRolloverShowCPA;

extern bool             g_bAIS_ACK_Timeout;
extern double           g_AckTimeout_Mins;

extern bool             g_bQuiltEnable;
extern bool             g_bFullScreenQuilt;
extern bool             g_bConfirmObjectDelete;
extern wxString         g_GPS_Ident;
extern bool             g_bGarminHostUpload;

extern wxLocale         *plocale_def_lang;
extern OCPN_Sound        g_anchorwatch_sound;
extern bool             g_bMagneticAPB;

extern bool             g_fog_overzoom;
extern double           g_overzoom_emphasis_base;
extern bool             g_oz_vector_scale;



#ifdef USE_S57
extern s52plib          *ps52plib;
#endif

extern wxString         g_locale;
extern bool             g_bportable;
extern bool             g_bdisable_opengl;
extern wxString         *pHome_Locn;
extern wxString         g_Plugin_Dir;

extern ChartGroupArray  *g_pGroupArray;
extern ocpnStyle::StyleManager* g_StyleManager;

#ifdef ocpnUSE_GL
extern ocpnGLOptions g_GLOptions;
#endif

extern bool             g_bexpert;
//    Some constants
#define ID_CHOICE_NMEA  wxID_HIGHEST + 1

extern wxArrayString *EnumerateSerialPorts(void);           // in chart1.cpp

extern wxArrayString    TideCurrentDataSet;
extern wxString         g_TCData_Dir;

extern AIS_Decoder      *g_pAIS;
extern bool             g_bserial_access_checked;

options                *g_pOptions;
bool                    g_bLoadedDisabledPlugins;

extern bool             g_btouch;
extern bool             g_bresponsive;

extern double           g_config_display_size_mm;

extern "C" bool CheckSerialAccess( void );

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfDirCtrls);

IMPLEMENT_DYNAMIC_CLASS( options, wxDialog )

// sort callback for Connections list  Sort by priority.
#if wxCHECK_VERSION(2, 9, 0)
int wxCALLBACK SortConnectionOnPriority(long item1, long item2, wxIntPtr list)
#else
int wxCALLBACK SortConnectionOnPriority(long item1, long item2, long list)
#endif

{
    wxListCtrl *lc = (wxListCtrl*)list;

    wxListItem it1, it2;
    it1.SetId(lc->FindItem(-1, item1));
    it1.SetColumn(3);
    it1.SetMask(it1.GetMask() | wxLIST_MASK_TEXT);

    it2.SetId(lc->FindItem(-1, item2));
    it2.SetColumn(3);
    it2.SetMask(it2.GetMask() | wxLIST_MASK_TEXT);

    lc->GetItem(it1);
    lc->GetItem(it2);

#ifdef __WXOSX__
    return it1.GetText().CmpNoCase(it2.GetText());
#else
    return it2.GetText().CmpNoCase(it1.GetText());
#endif
}




extern ArrayOfMMSIProperties   g_MMSI_Props_Array;






///////////////////////////////////////////////////////////////////////////////
/// Class MMSIEditDialog
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS( MMSIEditDialog, wxDialog )

BEGIN_EVENT_TABLE( MMSIEditDialog, wxDialog )
EVT_BUTTON( ID_MMSIEDIT_CANCEL, MMSIEditDialog::OnMMSIEditCancelClick )
EVT_BUTTON( ID_MMSIEDIT_OK, MMSIEditDialog::OnMMSIEditOKClick )
END_EVENT_TABLE()

MMSIEditDialog::MMSIEditDialog()
{
}

MMSIEditDialog::MMSIEditDialog( MMSIProperties *props, wxWindow* parent, wxWindowID id, const wxString& caption,
                               const wxPoint& pos, const wxSize& size, long style )
{
    m_props = props;
    
    long wstyle = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER;
    wxDialog::Create( parent, id, caption, pos, size, wstyle );
    
    CreateControls();
    GetSizer()->SetSizeHints( this );
    Centre();
    
}

MMSIEditDialog::~MMSIEditDialog()
{
    delete m_MMSICtl;
}


bool MMSIEditDialog::Create( MMSIProperties *props, wxWindow* parent, wxWindowID id, const wxString& caption,
                            const wxPoint& pos, const wxSize& size, long style )
{
    m_props = props;
    
    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );
    
    CreateControls();
    GetSizer()->SetSizeHints( this );
    Centre();
    
    return TRUE;
}

void MMSIEditDialog::CreateControls()
{
    MMSIEditDialog* itemDialog1 = this;
    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer( wxVERTICAL );
    itemDialog1->SetSizer( itemBoxSizer2 );
    
    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox( itemDialog1, wxID_ANY,
                                                             _("MMSI Extended Properties") );
    
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer( itemStaticBoxSizer4Static,
                                                                 wxVERTICAL );
    itemBoxSizer2->Add( itemStaticBoxSizer4, 0, wxEXPAND | wxALL, 5 );
    
    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("MMSI") );
#ifdef __WXOSX__
    itemStaticBoxSizer4->Add( itemStaticText5, 0,
                             wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 5 );
#else
    itemStaticBoxSizer4->Add( itemStaticText5, 0,
                             wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );
#endif
    m_MMSICtl = new wxTextCtrl( itemDialog1, ID_MMSI_CTL, _T(""), wxDefaultPosition, wxSize( 180, -1 ), 0 );
    itemStaticBoxSizer4->Add( m_MMSICtl, 0,
                             wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 5 );
    
    
    wxStaticBoxSizer *sbSizerPropsTrack = new wxStaticBoxSizer( new wxStaticBox( itemDialog1, wxID_ANY, _("Tracking") ), wxVERTICAL );
    
    wxGridSizer* bSizer15;
    bSizer15 = new wxGridSizer( 0, 3, 0, 0 );
    
    m_rbTypeTrackDefault = new wxRadioButton( itemDialog1, wxID_ANY, _("Default tracking"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rbTypeTrackDefault->SetValue( true );
    bSizer15->Add( m_rbTypeTrackDefault, 0, wxALL, 5 );
    
    m_rbTypeTrackAlways = new wxRadioButton( itemDialog1, wxID_ANY, _("Always track"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer15->Add( m_rbTypeTrackAlways, 0, wxALL, 5 );
    
    m_rbTypeTrackNever = new wxRadioButton( itemDialog1, wxID_ANY, _("Never track"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer15->Add( m_rbTypeTrackNever, 0, wxALL, 5 );

    m_cbTrackPersist = new wxCheckBox( itemDialog1, wxID_ANY, _("Persistent") );
    bSizer15->Add( m_cbTrackPersist, 0, wxALL, 5 );

    sbSizerPropsTrack->Add( bSizer15, 0, wxEXPAND, 0 );
    itemStaticBoxSizer4->Add(sbSizerPropsTrack, 0, wxEXPAND, 0);
    
    
    m_IgnoreButton = new wxCheckBox( itemDialog1, wxID_ANY, _("Ignore this MMSI") );
    itemStaticBoxSizer4->Add( m_IgnoreButton, 0, wxEXPAND, 5 );
    
    m_MOBButton = new wxCheckBox( itemDialog1, wxID_ANY, _("Handle this MMSI as SART/PLB(AIS) MOB") );
    itemStaticBoxSizer4->Add( m_MOBButton, 0, wxEXPAND, 5 );
    
    m_VDMButton = new wxCheckBox( itemDialog1, wxID_ANY, _("Convert AIVDM to AIVDO for this MMSI") );
    itemStaticBoxSizer4->Add( m_VDMButton, 0, wxEXPAND, 5 );
    
    
    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer2->Add( itemBoxSizer16, 0, wxALIGN_RIGHT | wxALL, 5 );
    
    m_CancelButton = new wxButton( itemDialog1, ID_MMSIEDIT_CANCEL, _("Cancel"), wxDefaultPosition,
                                  wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_CancelButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    
    m_OKButton = new wxButton( itemDialog1, ID_MMSIEDIT_OK, _("OK"), wxDefaultPosition,
                              wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_OKButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_OKButton->SetDefault();
    
    //  Set initial values...
    wxString sMMSI;
    if(m_props->MMSI > 0)
        sMMSI.Printf(_T("%d"), m_props->MMSI);
    else
        sMMSI = _T("");
    m_MMSICtl->AppendText(sMMSI);
    
    switch(m_props->TrackType){
        case TRACKTYPE_DEFAULT:
            break;
        case TRACKTYPE_ALWAYS:
            m_rbTypeTrackAlways->SetValue(true);
            break;
        case TRACKTYPE_NEVER:
            m_rbTypeTrackNever->SetValue(true);
            break;
        default:
            break;
    }

    m_cbTrackPersist->SetValue(m_props->m_bPersistentTrack);

    m_IgnoreButton->SetValue(m_props->m_bignore);
    m_MOBButton->SetValue(m_props->m_bMOB);
    m_VDMButton->SetValue(m_props->m_bVDM);
    
    
    SetColorScheme( (ColorScheme) 0 );
}

void MMSIEditDialog::SetColorScheme( ColorScheme cs )
{
    DimeControl( this );
}


void MMSIEditDialog::OnMMSIEditCancelClick( wxCommandEvent& event )
{
    EndModal(wxID_CANCEL);
}

void MMSIEditDialog::OnMMSIEditOKClick( wxCommandEvent& event )
{
    
    // Update the MMSIProperties by the passed pointer
    if(m_props){
        long nmmsi;
        m_MMSICtl->GetValue().ToLong(&nmmsi);
        m_props->MMSI = nmmsi;
        
        if( m_rbTypeTrackDefault->GetValue() )
            m_props->TrackType = TRACKTYPE_DEFAULT;
        else if ( m_rbTypeTrackAlways->GetValue() )
            m_props->TrackType = TRACKTYPE_ALWAYS;
        else
            m_props->TrackType = TRACKTYPE_NEVER;
        
        
        m_props->m_bignore = m_IgnoreButton->GetValue();
        m_props->m_bMOB = m_MOBButton->GetValue();
        m_props->m_bVDM = m_VDMButton->GetValue();
        m_props->m_bPersistentTrack = m_cbTrackPersist->GetValue();
    }
    
    EndModal(wxID_OK);
}


void MMSIEditDialog::OnCtlUpdated( wxCommandEvent& event )
{
}





///////////////////////////////////////////////////////////////////////////////
/// Class MMSIListCtrl
///////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE( MMSIListCtrl, wxListCtrl )
EVT_LIST_ITEM_SELECTED( ID_MMSI_PROPS_LIST, MMSIListCtrl::OnListItemClick )
EVT_LIST_ITEM_ACTIVATED( ID_MMSI_PROPS_LIST, MMSIListCtrl::OnListItemActivated )
EVT_LIST_ITEM_RIGHT_CLICK( ID_MMSI_PROPS_LIST, MMSIListCtrl::OnListItemRightClick )
EVT_MENU ( ID_DEF_MENU_MMSI_EDIT,             MMSIListCtrl::PopupMenuHandler )
EVT_MENU ( ID_DEF_MENU_MMSI_DELETE,           MMSIListCtrl::PopupMenuHandler )

END_EVENT_TABLE()


MMSIListCtrl::MMSIListCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos,
                           const wxSize& size, long style ) :
wxListCtrl( parent, id, pos, size, style )
{
    m_parent = parent;
}

MMSIListCtrl::~MMSIListCtrl()
{
}

wxString MMSIListCtrl::OnGetItemText( long item, long column ) const
{
    wxString ret = _T("");
    
    MMSIProperties *props = g_MMSI_Props_Array.Item( item );
    
    if(props){
        switch( column ){
            case mlMMSI:
                if(props->MMSI > 0)
                    ret.Printf(_T("%d"), props->MMSI );
                
                break;
                
            case mlTrackMode:
                if(TRACKTYPE_DEFAULT == props->TrackType)
                    ret = _("Default");
                else if(TRACKTYPE_ALWAYS == props->TrackType)
                    ret = _("Always");
                else if(TRACKTYPE_NEVER == props->TrackType)
                    ret = _("Never");
                else
                    ret = _T("???");
                if( props->m_bPersistentTrack )
                    ret.Append(_T(", ")).Append(_("Persistent"));
                break;
                
            case mlIgnore:
                if( props->m_bignore)
                    ret = _T("X");
                break;
                
            case mlMOB:
                if( props->m_bMOB)
                    ret = _T("X");
                break;
                
            case mlVDM:
                if( props->m_bVDM)
                    ret = _T("X");
                break;
                
            default:
                ret = _T("??");
                break;
        }
    }
    
    
    return ret;
}

void MMSIListCtrl::OnListItemClick( wxListEvent &event)
{
}

void MMSIListCtrl::OnListItemActivated( wxListEvent &event)
{
    MMSIProperties *props = g_MMSI_Props_Array.Item( event.GetIndex() );
    MMSIProperties *props_new = new MMSIProperties( *props);
    
    MMSIEditDialog *pd = new MMSIEditDialog( props_new, m_parent, -1, _("Edit MMSI Properties"),
                                            wxDefaultPosition, wxSize(200,200) );
    
    if ( pd->ShowModal() == wxID_OK ){
        g_MMSI_Props_Array.RemoveAt( event.GetIndex() );
        g_MMSI_Props_Array.Insert(props_new, event.GetIndex());
    }
    
    pd->Destroy();
}

void MMSIListCtrl::OnListItemRightClick( wxListEvent &event)
{
    m_context_item = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    
    if(-1 == m_context_item)
        return;
    
    wxMenu* menu = new wxMenu( _("MMSI Properties") );
    
    wxMenuItem *item_edit = new wxMenuItem(menu, ID_DEF_MENU_MMSI_EDIT, _("Edit..."));
    menu->Append(item_edit);
    
    wxMenuItem *item_delete = new wxMenuItem(menu, ID_DEF_MENU_MMSI_DELETE, _("Delete"));
    menu->Append(item_delete);

#ifdef __WXMSW__
    wxFont *qFont = GetOCPNScaledFont(_("Menu"));
    item_edit->SetFont(*qFont);
    item_delete->SetFont(*qFont);
#endif
    
    wxPoint p = ScreenToClient(wxGetMousePosition());
    PopupMenu( menu, p.x, p.y );
    
    SetItemCount( g_MMSI_Props_Array.GetCount() );
    Refresh(true);
    
}


void MMSIListCtrl::PopupMenuHandler( wxCommandEvent& event )
{
    MMSIProperties *props = g_MMSI_Props_Array.Item( m_context_item );
    MMSIProperties *props_new = new MMSIProperties( *props);
    
    MMSIEditDialog *pd;
    
    switch( event.GetId() ) {
        case ID_DEF_MENU_MMSI_EDIT:
            if(props){
                pd = new MMSIEditDialog( props_new, m_parent, -1, _("Edit MMSI Properties"), wxDefaultPosition, wxSize(200,200) );
                
                if ( pd->ShowModal() == wxID_OK ){
                    g_MMSI_Props_Array.RemoveAt( m_context_item );
                    g_MMSI_Props_Array.Insert(props_new, m_context_item);
                }
                
                pd->Destroy();
            }
            break;
            
        case ID_DEF_MENU_MMSI_DELETE:
            if(props){
                g_MMSI_Props_Array.RemoveAt( m_context_item );
            }
            break;
    }
    
}


///////////////////////////////////////////////////////////////////////////////
/// Class MMSI_Props_Panel
///////////////////////////////////////////////////////////////////////////////


MMSI_Props_Panel::MMSI_Props_Panel( wxWindow *parent ):
wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE )
{
    m_pparent = parent;
    
    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    SetFont( *qFont );
    
    wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( topSizer );
    
    wxString MMSI_props_column_spec = _T("120;120;100;100;100;100;100");
    //  Parse the global column width string as read from config file
    wxStringTokenizer tkz( MMSI_props_column_spec, _T(";") );
    wxString s_width = tkz.GetNextToken();
    int width;
    long lwidth;
    
    m_pListCtrlMMSI = new MMSIListCtrl( this, ID_MMSI_PROPS_LIST, wxDefaultPosition,  wxSize(-1, 400),
                                       wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES | wxBORDER_SUNKEN  | wxLC_VIRTUAL );
    wxImageList *imglist = new wxImageList( 16, 16, true, 2 );
    
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
    imglist->Add( style->GetIcon( _T("sort_asc") ) );
    imglist->Add( style->GetIcon( _T("sort_desc") ) );
    
    //   m_pListCtrlMMSI->AssignImageList( imglist, wxIMAGE_LIST_SMALL );
    
    int dx = GetCharWidth();
    
    
    width = dx * 12;
    if( s_width.ToLong( &lwidth ) ) {
        width = wxMax(dx * 2, lwidth);
        width = wxMin(width, dx * 30);
    }
    m_pListCtrlMMSI->InsertColumn( tlMMSI, _("MMSI"), wxLIST_FORMAT_LEFT, width );
    s_width = tkz.GetNextToken();
    
    width = dx * 12;
    if( s_width.ToLong( &lwidth ) ) {
        width = wxMax(dx * 2, lwidth);
        width = wxMin(width, dx * 30);
    }
    m_pListCtrlMMSI->InsertColumn( tlCLASS, _("Track Mode"), wxLIST_FORMAT_CENTER, width );
    s_width = tkz.GetNextToken();
    
    width = dx * 8;
    if( s_width.ToLong( &lwidth ) ) {
        width = wxMax(dx * 2, lwidth);
        width = wxMin(width, dx * 30);
    }
    m_pListCtrlMMSI->InsertColumn( tlTYPE, _("Ignore"), wxLIST_FORMAT_CENTER, width );
    s_width = tkz.GetNextToken();
    
    width = dx * 8;
    if( s_width.ToLong( &lwidth ) ) {
        width = wxMax(dx * 2, lwidth);
        width = wxMin(width, dx * 30);
    }
    m_pListCtrlMMSI->InsertColumn( tlTYPE, _("MOB"), wxLIST_FORMAT_CENTER, width );
    s_width = tkz.GetNextToken();
    
    width = dx * 8;
    if( s_width.ToLong( &lwidth ) ) {
        width = wxMax(dx * 2, lwidth);
        width = wxMin(width, dx * 30);
    }
    m_pListCtrlMMSI->InsertColumn( tlTYPE, _("VDM->VDO"), wxLIST_FORMAT_CENTER, width );
    s_width = tkz.GetNextToken();
    
    
    topSizer->Add( m_pListCtrlMMSI, 1, wxEXPAND | wxALL, 0 );
    
    m_pButtonNew = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxSize( 200, -1));
    m_pButtonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler( MMSI_Props_Panel::OnNewButton ), NULL, this );
    topSizer->Add( m_pButtonNew, 0, wxALIGN_RIGHT | wxALL, 0 );
    
    
    topSizer->Layout();
    
    //    This is silly, but seems to be required for __WXMSW__ build
    //    If not done, the SECOND invocation of the panel fails to expand the list to the full wxSizer size....
    SetSize( GetSize().x, GetSize().y - 1 );
    
    SetColorScheme( (ColorScheme) 0 );
}

MMSI_Props_Panel::~MMSI_Props_Panel()
{
}

void MMSI_Props_Panel::OnNewButton( wxCommandEvent &event )
{
    MMSIProperties *props = new MMSIProperties(-1);
    
    MMSIEditDialog *pd = new MMSIEditDialog( props, m_parent, -1, _("Add MMSI Properties"), wxDefaultPosition, wxSize(200,200) );
    
    if ( pd->ShowModal() == wxID_OK )
        g_MMSI_Props_Array.Add( props );
    
    pd->Destroy();
    
    UpdateMMSIList();
}


void MMSI_Props_Panel::UpdateMMSIList( void )
{
    //    Capture the MMSI of the curently selected list item
    long selItemID = -1;
    selItemID = m_pListCtrlMMSI->GetNextItem( selItemID, wxLIST_NEXT_ALL,
                                             wxLIST_STATE_SELECTED );
    
    int selMMSI = -1;
    if( selItemID != -1 )
        selMMSI = g_MMSI_Props_Array.Item( selItemID )->MMSI;
    
    
    m_pListCtrlMMSI->SetItemCount( g_MMSI_Props_Array.GetCount() );
    
    
    if( g_MMSI_Props_Array.GetCount() ){
        //    Restore selected item
        long item_sel = 0;
        if( ( selItemID != -1 ) && ( selMMSI != -1 ) ) {
            for( unsigned int i = 0; i < g_MMSI_Props_Array.GetCount(); i++ ) {
                if( g_MMSI_Props_Array.Item( i )->MMSI == selMMSI ) {
                    item_sel = i;
                    break;
                }
            }
        }

        m_pListCtrlMMSI->SetItemState( item_sel,
                                      wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
                                      wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED );

    }
    
#ifdef __WXMSW__
    m_pListCtrlMMSI->Refresh( false );
#endif
}

void MMSI_Props_Panel::SetColorScheme( ColorScheme cs )
{
    DimeControl( this );
}


BEGIN_EVENT_TABLE( options, wxDialog )
    EVT_CHECKBOX( ID_DEBUGCHECKBOX1, options::OnDebugcheckbox1Click )
    EVT_BUTTON( ID_BUTTONADD, options::OnButtonaddClick )
    EVT_BUTTON( ID_BUTTONDELETE, options::OnButtondeleteClick )
    EVT_BUTTON( ID_TCDATAADD, options::OnInsertTideDataLocation )
    EVT_BUTTON( ID_TCDATADEL, options::OnRemoveTideDataLocation )
    EVT_BUTTON( ID_APPLY, options::OnApplyClick )
    EVT_BUTTON( xID_OK, options::OnXidOkClick )
    EVT_BUTTON( wxID_CANCEL, options::OnCancelClick )
    EVT_BUTTON( ID_BUTTONFONTCHOOSE, options::OnChooseFont )
    EVT_CLOSE( options::OnClose)
    
#ifdef __WXGTK__
    EVT_BUTTON( ID_BUTTONFONTCOLOR, options::OnChooseFontColor )
#endif
    EVT_BUTTON( ID_OPENGLOPTIONS, options::OnOpenGLOptions )
    EVT_CHOICE( ID_RADARDISTUNIT, options::OnDisplayCategoryRadioButton )
    EVT_CHOICE( ID_DEPTHUNITSCHOICE, options::OnUnitsChoice )
    EVT_BUTTON( ID_CLEARLIST, options::OnButtonClearClick )
    EVT_BUTTON( ID_SELECTLIST, options::OnButtonSelectClick )
    EVT_BUTTON( ID_AISALERTSELECTSOUND, options::OnButtonSelectSound )
    EVT_BUTTON( ID_AISALERTTESTSOUND, options::OnButtonTestSound )
    EVT_CHECKBOX( ID_SHOWGPSWINDOW, options::OnShowGpsWindowCheckboxClick )
    EVT_CHECKBOX( ID_ZTCCHECKBOX, options::OnZTCCheckboxClick )
    EVT_CHOICE( ID_SHIPICONTYPE, options::OnShipTypeSelect )
    EVT_CHOICE( ID_RADARRINGS, options::OnRadarringSelect )
    EVT_CHAR_HOOK( options::OnCharHook )
END_EVENT_TABLE()

options::options()
{
    Init();
}

options::options( MyFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos,
        const wxSize& size, long style )
{
    Init();

    pParent = parent;

    long wstyle = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER;
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

    wxDialog::Create( parent, id, caption, pos, size, wstyle );

    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    SetFont( *qFont );
    
    CreateControls();
    Fit();
    Center();
}

options::~options()
{
    // Disconnect Connection page Events
    if(m_pNMEAForm){
    m_lcSources->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( options::OnSelectDatasource ), NULL, this );
    m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnAddDatasourceClick ), NULL, this );
    m_buttonRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnRemoveDatasourceClick ), NULL, this );
    m_tFilterSec->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_rbTypeSerial->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnTypeSerialSelected ), NULL, this );
    m_rbTypeNet->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnTypeNetSelected ), NULL, this );
    m_rbNetProtoTCP->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_rbNetProtoUDP->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_rbNetProtoGPSD->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_tNetAddress->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_tNetPort->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_comboPort->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_comboPort->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_choiceBaudRate->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnBaudrateChoice ), NULL, this );
    m_choiceSerialProtocol->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnProtocolChoice ), NULL, this );
    m_choicePriority->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_cbCheckCRC->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCrcCheck ), NULL, this );
    m_cbGarminHost->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_cbGarminUploadHost->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_cbFurunoGP3X->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_rbIAccept->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbAcceptInput ), NULL, this );
    m_rbIIgnore->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbIgnoreInput ), NULL, this );
    m_tcInputStc->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_btnInputStcList->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnBtnIStcs ), NULL, this );
    m_cbInput->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCbInput ), NULL, this );
    m_cbOutput->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCbOutput ), NULL, this );
    m_rbOAccept->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbOutput ), NULL, this );
    m_rbOIgnore->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbOutput ), NULL, this );
    m_tcOutputStc->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_btnOutputStcList->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnBtnOStcs ), NULL, this );
    m_cbNMEADebug->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnShowGpsWindowCheckboxClick ), NULL, this );
    }

    delete m_pSerialArray;
    groupsPanel->EmptyChartGroupArray( m_pGroupArray );
    delete m_pGroupArray;
    m_pGroupArray = NULL;
    m_groupsPage = NULL;
    g_pOptions = NULL;
    if( m_topImgList ) delete m_topImgList;
    delete smallFont;
}

void options::Init()
{
    m_pWorkDirList = NULL;

    pShowStatusBar = NULL;
    pShowMenuBar = NULL;
    pShowCompassWin = NULL;
    pSelCtl = NULL;
    pActiveChartsList = NULL;
    ps57CtlListBox = NULL;
    pDispCat = NULL;
    m_pSerialArray = NULL;
    pUpdateCheckBox = NULL;
    k_charts = 0;
    k_vectorcharts = 0;
    k_plugins = 0;
    k_tides = 0;
    smallFont = 0;

    activeSizer = NULL;
    itemActiveChartStaticBox = NULL;

    m_bVisitLang = false;
    m_itemFontElementListBox = NULL;
    m_topImgList = NULL;
    m_pSerialArray = EnumerateSerialPorts();

    m_pListbook = NULL;
    m_pGroupArray = NULL;
    m_groups_changed = 0;

    m_pageDisplay = -1;
    m_pageConnections = -1;
    m_pageCharts = -1;
    m_pageShips = -1;
    m_pageUI = -1;
    m_pagePlugins = -1;
    m_pageConnections = -1;

    lastPage = -1;

    m_pPlugInCtrl = NULL;       // for deferred loading
    m_pNMEAForm = NULL;

    // This variable is used by plugin callback function AddOptionsPage
    g_pOptions = this;
}

bool options::Create( MyFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos,
        const wxSize& size, long style )
{
    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Fit();
    if( lastWindowPos == wxPoint(0,0) ) {
        Centre();
    } else {
        Move( lastWindowPos );
    }
    lastWindowPos = GetPosition();
    return TRUE;
}

wxWindow* options::GetContentWindow() const
{
    return NULL;
}

size_t options::CreatePanel(const wxString & title)
{
    size_t id = m_pListbook->GetPageCount();
    /* This is the default empty content for any top tab.
       It'll be replaced when we call AddPage */
    wxPanel *panel = new wxPanel( m_pListbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, title );
    m_pListbook->AddPage( panel, title, false, id );
    return id;
}

wxScrolledWindow *options::AddPage( size_t parent, const wxString & title)
{
    if( parent > m_pListbook->GetPageCount() - 1 ) {
        wxLogMessage(
                wxString::Format( _T("Warning: invalid parent in options::AddPage( %d, "),
                        parent ) + title + _T(" )") );
        return NULL;
    }
    wxNotebookPage* page = m_pListbook->GetPage( parent );

    wxScrolledWindow *window;
    int style = wxVSCROLL | wxTAB_TRAVERSAL;
    if( page->IsKindOf( CLASSINFO(wxNotebook))) {
        window = new wxScrolledWindow( page, wxID_ANY, wxDefaultPosition, wxDefaultSize, style );
        window->SetScrollRate(5,5);
        ((wxNotebook *)page)->AddPage( window, title );
    } else if (page->IsKindOf(CLASSINFO(wxScrolledWindow))) {
        wxString toptitle = m_pListbook->GetPageText( parent );
        wxNotebook *nb = new wxNotebook( m_pListbook, wxID_ANY, wxDefaultPosition, wxDefaultSize,wxNB_TOP );
        /* Only remove the tab from listbook, we still have original content in {page} */
        m_pListbook->RemovePage( parent );
        m_pListbook->InsertPage( parent, nb, toptitle, false, parent );
        wxString previoustitle = page->GetName();
        page->Reparent( nb );
        nb->AddPage( page, previoustitle );
        /* wxNotebookPage is hidden under wxGTK after RemovePage/Reparent
         * we must explicitely Show() it */
        page->Show();
        window = new wxScrolledWindow( nb, wxID_ANY, wxDefaultPosition, wxDefaultSize, style );
        window->SetScrollRate(5, 5);
        nb->AddPage( window, title );
        nb->ChangeSelection( 0 );
    } else { // This is the default content, we can replace it now
        window = new wxScrolledWindow( m_pListbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, title );
        window->SetScrollRate(5, 5);
        wxString toptitle = m_pListbook->GetPageText( parent );
        m_pListbook->DeletePage( parent );
        m_pListbook->InsertPage( parent, window, toptitle, false, parent );
    }

    return window;
}

bool options::DeletePage( wxScrolledWindow *page  )
{
    for (size_t i = 0; i < m_pListbook->GetPageCount(); i++)
    {
        wxNotebookPage* pg = m_pListbook->GetPage( i );

        if( pg->IsKindOf( CLASSINFO(wxNotebook))) {
            wxNotebook *nb = ((wxNotebook *)pg);
            for (size_t j = 0; j < nb->GetPageCount(); j++)
            {
                wxNotebookPage* spg = nb->GetPage( j );
                if ( spg == page )
                {
                    nb->DeletePage( j );
                    if (nb->GetPageCount()==1)
                    {
                        /* There's only one page, remove inner notebook */
                        spg = nb->GetPage( 0 );
                        spg->Reparent( m_pListbook );
                        nb->RemovePage( 0 );
                        wxString toptitle = m_pListbook->GetPageText( i );
                        m_pListbook->DeletePage( i );
                        m_pListbook->InsertPage( i, spg, toptitle, false, i );
                    }
                    return true;
                }
            }
        } else if (pg->IsKindOf(CLASSINFO(wxScrolledWindow)) && pg == page) {
            /* There's only one page, replace it with empty panel */
            m_pListbook->DeletePage( i );
            wxPanel *panel = new wxPanel( m_pListbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("") );
            wxString toptitle = m_pListbook->GetPageText( i );
            m_pListbook->InsertPage( i, panel, toptitle, false, i );
            return true;
        }
    }
    return false;
}

void options::CreatePanel_NMEA( size_t parent, int border_size, int group_item_spacing, wxSize small_button_size )
{
    m_pNMEAForm = AddPage( parent, _("NMEA") );

    wxBoxSizer* bSizer4 = new wxBoxSizer( wxVERTICAL );
    m_pNMEAForm->SetSizer( bSizer4 );
    m_pNMEAForm->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizerOuterContainer = new wxBoxSizer( wxVERTICAL );


    wxStaticBoxSizer* sbSizerGeneral;
    sbSizerGeneral = new wxStaticBoxSizer( new wxStaticBox( m_pNMEAForm, wxID_ANY, _("General") ), wxVERTICAL );

    wxBoxSizer* bSizer151;
    bSizer151 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer161;
    bSizer161 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer171;
    bSizer171 = new wxBoxSizer( wxHORIZONTAL );

    m_cbFilterSogCog = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Filter NMEA Course and Speed data"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbFilterSogCog->SetValue( g_bfilter_cogsog );
    bSizer171->Add( m_cbFilterSogCog, 0, wxALL, 5 );

    m_stFilterSec = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Filter period (sec)"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stFilterSec->Wrap( -1 );

    int nspace = 5;
#ifdef __WXGTK__
    nspace = 9;
#endif
    bSizer171->Add( m_stFilterSec, 0, wxALL, nspace );

    m_tFilterSec = new wxTextCtrl( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    wxString sfilt;
    sfilt.Printf( _T("%d"), g_COGFilterSec );
    m_tFilterSec->SetValue( sfilt );
    bSizer171->Add( m_tFilterSec, 0, wxALL, 4 );

    bSizer161->Add( bSizer171, 1, wxEXPAND, 5 );

    int cb_space = 2;
    m_cbNMEADebug = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Show NMEA Debug Window"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbNMEADebug->SetValue(NMEALogWindow::Get().Active());
    bSizer161->Add( m_cbNMEADebug, 0, wxALL, cb_space );

    m_cbFurunoGP3X = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Format uploads for Furuno GP3X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbFurunoGP3X->SetValue(g_GPS_Ident == _T("FurunoGP3X"));
    bSizer161->Add( m_cbFurunoGP3X, 0, wxALL, cb_space );

    m_cbGarminUploadHost = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Use Garmin GRMN (Host) mode for uploads"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbGarminUploadHost->SetValue(g_bGarminHostUpload);
    bSizer161->Add( m_cbGarminUploadHost, 0, wxALL, cb_space );

    m_cbAPBMagnetic = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Use magnetic bearings in output sentence ECAPB"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbAPBMagnetic->SetValue(g_bMagneticAPB);
    bSizer161->Add( m_cbAPBMagnetic, 0, wxALL, cb_space );
    
    bSizer151->Add( bSizer161, 1, wxEXPAND, 5 );
    sbSizerGeneral->Add( bSizer151, 1, wxEXPAND, 5 );
    bSizerOuterContainer->Add( sbSizerGeneral, 0, wxALL|wxEXPAND, 5 );


    //  Connections listbox, etc
    wxStaticBoxSizer* sbSizerLB= new wxStaticBoxSizer( new wxStaticBox( m_pNMEAForm, wxID_ANY, _("Data Connections") ), wxVERTICAL );


    wxBoxSizer* bSizer17;
    bSizer17 = new wxBoxSizer( wxVERTICAL );

    m_lcSources = new wxListCtrl( m_pNMEAForm, wxID_ANY, wxDefaultPosition, wxSize(-1, 150), wxLC_REPORT|wxLC_SINGLE_SEL);
    bSizer17->Add( m_lcSources, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer18;
    bSizer18 = new wxBoxSizer( wxHORIZONTAL );

    m_buttonAdd = new wxButton( m_pNMEAForm, wxID_ANY, _("Add Connection"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer18->Add( m_buttonAdd, 0, wxALL, 5 );

    m_buttonRemove = new wxButton( m_pNMEAForm, wxID_ANY, _("Remove Connection"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonRemove->Enable( false );
    bSizer18->Add( m_buttonRemove, 0, wxALL, 5 );

    bSizer17->Add( bSizer18, 0, wxEXPAND, 5 );
    sbSizerLB->Add( bSizer17, 1, wxEXPAND, 5 );
    bSizerOuterContainer->Add( sbSizerLB, 0, wxEXPAND, 5 );


    //  Connections Properties

    sbSizerConnectionProps = new wxStaticBoxSizer( new wxStaticBox( m_pNMEAForm, wxID_ANY, _("Properties") ), wxVERTICAL );

    wxBoxSizer* bSizer15;
    bSizer15 = new wxBoxSizer( wxHORIZONTAL );

    m_rbTypeSerial = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Serial"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rbTypeSerial->SetValue( true );
    bSizer15->Add( m_rbTypeSerial, 0, wxALL, 5 );

    m_rbTypeNet = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Network"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer15->Add( m_rbTypeNet, 0, wxALL, 5 );


    sbSizerConnectionProps->Add( bSizer15, 0, wxEXPAND, 0 );

    gSizerNetProps = new wxGridSizer( 0, 2, 0, 0 );

    m_stNetProto = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Protocol"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stNetProto->Wrap( -1 );
    gSizerNetProps->Add( m_stNetProto, 0, wxALL, 5 );

    wxBoxSizer* bSizer16;
    bSizer16 = new wxBoxSizer( wxHORIZONTAL );

    m_rbNetProtoTCP = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("TCP"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_rbNetProtoTCP->Enable( true );

    bSizer16->Add( m_rbNetProtoTCP, 0, wxALL, 5 );

    m_rbNetProtoUDP = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("UDP"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rbNetProtoUDP->Enable( true );

    bSizer16->Add( m_rbNetProtoUDP, 0, wxALL, 5 );

    m_rbNetProtoGPSD = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("GPSD"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rbNetProtoGPSD->SetValue( true );
    bSizer16->Add( m_rbNetProtoGPSD, 0, wxALL, 5 );


    gSizerNetProps->Add( bSizer16, 1, wxEXPAND, 5 );

    m_stNetAddr = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stNetAddr->Wrap( -1 );
    gSizerNetProps->Add( m_stNetAddr, 0, wxALL, 5 );

    m_tNetAddress = new wxTextCtrl( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    gSizerNetProps->Add( m_tNetAddress, 0, wxEXPAND|wxTOP, 5 );

    m_stNetPort = new wxStaticText( m_pNMEAForm, wxID_ANY, _("DataPort"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stNetPort->Wrap( -1 );
    gSizerNetProps->Add( m_stNetPort, 0, wxALL, 5 );

    m_tNetPort = new wxTextCtrl( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    gSizerNetProps->Add( m_tNetPort, 1, wxEXPAND|wxTOP, 5 );


    sbSizerConnectionProps->Add( gSizerNetProps, 0, wxEXPAND, 5 );

    gSizerSerProps = new wxGridSizer( 0, 1, 0, 0 );

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer( 0, 4, 0, 0 );
    fgSizer1->SetFlexibleDirection( wxBOTH );
    fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_stSerPort = new wxStaticText( m_pNMEAForm, wxID_ANY, _("DataPort"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stSerPort->Wrap( -1 );
    fgSizer1->Add( m_stSerPort, 0, wxALL, 5 );

    m_comboPort = new wxComboBox( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
    fgSizer1->Add( m_comboPort, 0, wxEXPAND|wxTOP, 5 );

    m_stSerBaudrate = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Baudrate"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stSerBaudrate->Wrap( -1 );
    fgSizer1->Add( m_stSerBaudrate, 0, wxALL, 5 );

    wxString m_choiceBaudRateChoices[] = { _("150"), _("300"), _("600"), _("1200"), _("2400"), _("4800"), _("9600"), _("19200"), _("38400"), _("57600"), _("115200"), _("230400"), _("460800"), _("921600") };
    int m_choiceBaudRateNChoices = sizeof( m_choiceBaudRateChoices ) / sizeof( wxString );
    m_choiceBaudRate = new wxChoice( m_pNMEAForm, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceBaudRateNChoices, m_choiceBaudRateChoices, 0 );
    m_choiceBaudRate->SetSelection( 0 );
    fgSizer1->Add( m_choiceBaudRate, 1, wxEXPAND|wxTOP, 5 );

    m_stSerProtocol = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Protocol"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stSerProtocol->Wrap( -1 );
    fgSizer1->Add( m_stSerProtocol, 0, wxALL, 5 );

    wxString m_choiceSerialProtocolChoices[] = { _("NMEA 0183"), _("NMEA 2000"), _("Seatalk") };
    int m_choiceSerialProtocolNChoices = sizeof( m_choiceSerialProtocolChoices ) / sizeof( wxString );
    m_choiceSerialProtocol = new wxChoice( m_pNMEAForm, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceSerialProtocolNChoices, m_choiceSerialProtocolChoices, 0 );
    m_choiceSerialProtocol->SetSelection( 0 );
    m_choiceSerialProtocol->Enable( false );

    fgSizer1->Add( m_choiceSerialProtocol, 1, wxEXPAND|wxTOP, 5 );

    m_stPriority = new wxStaticText( m_pNMEAForm, wxID_ANY, _("Priority"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stPriority->Wrap( -1 );
    fgSizer1->Add( m_stPriority, 0, wxALL, 5 );

    wxString m_choicePriorityChoices[] = { _("0"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9") };
    int m_choicePriorityNChoices = sizeof( m_choicePriorityChoices ) / sizeof( wxString );
    m_choicePriority = new wxChoice( m_pNMEAForm, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePriorityNChoices, m_choicePriorityChoices, 0 );
    m_choicePriority->SetSelection( 9 );
    fgSizer1->Add( m_choicePriority, 0, wxEXPAND|wxTOP, 5 );


    gSizerSerProps->Add( fgSizer1, 0, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer5;
    fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer5->SetFlexibleDirection( wxBOTH );
    fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_cbCheckCRC = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Control checksum"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbCheckCRC->SetValue(true);
    m_cbCheckCRC->SetToolTip( _("If checked, only the sentences with a valid checksum are passed through") );
    fgSizer5->Add( m_cbCheckCRC, 0, wxALL, 5 );

    m_cbGarminHost = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Use Garmin (GRMN) mode for input"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbGarminHost->SetValue(false);
    fgSizer5->Add( m_cbGarminHost, 0, wxALL, 5 );

    m_cbInput = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Receive Input on this Port"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer5->Add( m_cbInput, 0, wxALL, 5 );

    m_cbOutput = new wxCheckBox( m_pNMEAForm, wxID_ANY, _("Output on this port ( as Autopilot or NMEA Repeater)"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer5->Add( m_cbOutput, 0, wxALL, 5 );

    m_stPrecision = new wxStaticText( m_pNMEAForm, wxID_ANY, _("APB bearing precision"), wxDefaultPosition, wxDefaultSize, 0 );
    m_stPrecision->Wrap( -1 );
    fgSizer5->Add( m_stPrecision, 0, wxALL, 5 );

    wxString m_choicePrecisionChoices[] = { _("x"), _("x.x"), _("x.xx"), _("x.xxx"), _("x.xxxx") };
    int m_choicePrecisionNChoices = sizeof( m_choicePrecisionChoices ) / sizeof( wxString );
    m_choicePrecision = new wxChoice( m_pNMEAForm, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePrecisionNChoices, m_choicePrecisionChoices, 0 );
    m_choicePrecision->SetSelection( g_NMEAAPBPrecision );
    fgSizer5->Add( m_choicePrecision, 0, wxALL, 5 );

    sbSizerConnectionProps->Add( gSizerSerProps, 0, wxEXPAND, 5 );
    sbSizerConnectionProps->Add( fgSizer5, 0, wxEXPAND, 5 );

    sbSizerInFilter = new wxStaticBoxSizer( new wxStaticBox( m_pNMEAForm, wxID_ANY, _("Input filtering") ), wxVERTICAL );

    wxBoxSizer* bSizer9;
    bSizer9 = new wxBoxSizer( wxHORIZONTAL );

    m_rbIAccept = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Accept only sentences"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    bSizer9->Add( m_rbIAccept, 0, wxALL, 5 );

    m_rbIIgnore = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Ignore sentences"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer9->Add( m_rbIIgnore, 0, wxALL, 5 );


    sbSizerInFilter->Add( bSizer9, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer11;
    bSizer11 = new wxBoxSizer( wxHORIZONTAL );

    m_tcInputStc = new wxTextCtrl( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    bSizer11->Add( m_tcInputStc, 1, wxALL|wxEXPAND, 5 );

    m_btnInputStcList = new wxButton( m_pNMEAForm, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    bSizer11->Add( m_btnInputStcList, 0, wxALL, 5 );

    sbSizerInFilter->Add( bSizer11, 0, wxEXPAND, 5 );

    sbSizerConnectionProps->Add( sbSizerInFilter, 0, wxEXPAND, 5 );

    sbSizerOutFilter = new wxStaticBoxSizer( new wxStaticBox( m_pNMEAForm, wxID_ANY, _("Output filtering") ), wxVERTICAL );

    wxBoxSizer* bSizer10;
    bSizer10 = new wxBoxSizer( wxHORIZONTAL );

    m_rbOAccept = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Transmit sentences"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    bSizer10->Add( m_rbOAccept, 0, wxALL, 5 );

    m_rbOIgnore = new wxRadioButton( m_pNMEAForm, wxID_ANY, _("Drop sentences"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer10->Add( m_rbOIgnore, 0, wxALL, 5 );


    sbSizerOutFilter->Add( bSizer10, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer12;
    bSizer12 = new wxBoxSizer( wxHORIZONTAL );

    m_tcOutputStc = new wxTextCtrl( m_pNMEAForm, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    bSizer12->Add( m_tcOutputStc, 1, wxALL|wxEXPAND, 5 );

    m_btnOutputStcList = new wxButton( m_pNMEAForm, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    bSizer12->Add( m_btnOutputStcList, 0, wxALL, 5 );


    sbSizerOutFilter->Add( bSizer12, 0, wxEXPAND, 5 );
    sbSizerConnectionProps->Add( sbSizerOutFilter, 0, wxEXPAND, 5 );


    bSizerOuterContainer->Add( sbSizerConnectionProps, 1, wxALL|wxEXPAND, 5 );

    bSizer4->Add( bSizerOuterContainer, 1, wxEXPAND, 5 );

    // Connect Events
    m_lcSources->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( options::OnSelectDatasource ), NULL, this );
    m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnAddDatasourceClick ), NULL, this );
    m_buttonRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnRemoveDatasourceClick ), NULL, this );
    m_rbTypeSerial->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnTypeSerialSelected ), NULL, this );
    m_rbTypeNet->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnTypeNetSelected ), NULL, this );
    m_rbNetProtoTCP->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_rbNetProtoUDP->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_rbNetProtoGPSD->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnNetProtocolSelected ), NULL, this );
    m_tNetAddress->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_tNetPort->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_comboPort->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_comboPort->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_choiceBaudRate->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnBaudrateChoice ), NULL, this );
    m_choiceSerialProtocol->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnProtocolChoice ), NULL, this );
    m_choicePriority->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_cbCheckCRC->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCrcCheck ), NULL, this );
    m_cbGarminHost->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_cbGarminUploadHost->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_cbFurunoGP3X->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnUploadFormatChange ), NULL, this );
    m_rbIAccept->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbAcceptInput ), NULL, this );
    m_rbIIgnore->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbIgnoreInput ), NULL, this );
    m_tcInputStc->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_btnInputStcList->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnBtnIStcs ), NULL, this );
    m_cbInput->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCbInput ), NULL, this );
    m_cbOutput->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnCbOutput ), NULL, this );
    m_rbOAccept->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbOutput ), NULL, this );
    m_rbOIgnore->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( options::OnRbOutput ), NULL, this );
    m_tcOutputStc->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnConnValChange ), NULL, this );
    m_btnOutputStcList->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( options::OnBtnOStcs ), NULL, this );

    m_cbNMEADebug->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnShowGpsWindowCheckboxClick ), NULL, this );
    m_cbFilterSogCog->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_tFilterSec->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_cbAPBMagnetic->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( options::OnValChange ), NULL, this );
    m_lcSources->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler(options::OnConnectionToggleEnable), NULL, this );

    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("Enable") );
    m_lcSources->InsertColumn(0, col0);

    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("Type") );
    m_lcSources->InsertColumn(1, col1);

    wxListItem col2;
    col2.SetId(2);
    col2.SetText( _("DataPort") );
    m_lcSources->InsertColumn(2, col2);

    wxListItem col3;
    col3.SetId(3);
    col3.SetText( _("Priority") );
    m_lcSources->InsertColumn(3, col3);

    wxListItem col4;
    col4.SetId(4);
    col4.SetText( _("Parameters") );
    m_lcSources->InsertColumn(4, col4);

    wxListItem col5;
    col5.SetId(5);
    col5.SetText( _("Connection") );
    m_lcSources->InsertColumn(5, col5);

    wxListItem col6;
    col6.SetId(6);
    col6.SetText( _("Filters") );
    m_lcSources->InsertColumn(6, col6);

    //  Build the image list
    wxBitmap unchecked_bmp(16, 16), checked_bmp(16, 16);

    {
        wxMemoryDC renderer_dc;

        // Unchecked
        renderer_dc.SelectObject(unchecked_bmp);
        renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID));
        renderer_dc.Clear();
        wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), 0);

        // Checked
        renderer_dc.SelectObject(checked_bmp);
        renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID));
        renderer_dc.Clear();
        wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), wxCONTROL_CHECKED);
    }

    wxImageList *imglist = new wxImageList( 16, 16, true, 1 );
    imglist->Add( unchecked_bmp );
    imglist->Add( checked_bmp );
    m_lcSources->AssignImageList( imglist, wxIMAGE_LIST_SMALL );

    m_lcSources->Refresh();

    m_stcdialog_in = new SentenceListDlg(FILTER_INPUT, this);
    m_stcdialog_out = new SentenceListDlg(FILTER_OUTPUT, this);

    FillSourceList();

    if(m_pSerialArray){
        for(size_t i = 0; i < m_pSerialArray->Count() ; i++){
            m_comboPort->Append(m_pSerialArray->Item(i));
        }
    }

    ShowNMEACommon( false );
    ShowNMEASerial( false );
    ShowNMEANet( false );
    connectionsaved = true;
}

void options::OnConnectionToggleEnable( wxMouseEvent &event )
{
    wxPoint pos = event.GetPosition();
    int flags = 0;
    long clicked_index = m_lcSources->HitTest( pos, flags );

    //    Clicking Enable Checkbox (full column)?
    if( clicked_index > -1 && event.GetX() < m_lcSources->GetColumnWidth( 0 ) ) {
        // Process the clicked item
        ConnectionParams *conn = g_pConnectionParams->Item( m_lcSources->GetItemData( clicked_index ) );
        if(conn){
            conn->bEnabled = !conn->bEnabled;
            m_connection_enabled = conn->bEnabled;
            conn->b_IsSetup = false;            // Mark as changed
            
            m_lcSources->SetItemImage( clicked_index, conn->bEnabled ? 1 : 0 );
        }

        cc1->Refresh();
    }
    else if( clicked_index == -1 ) {
        ClearNMEAForm();
        m_buttonRemove->Enable( false );
    }

    // Allow wx to process...
    event.Skip();
}

void options::CreatePanel_Ownship( size_t parent, int border_size, int group_item_spacing, wxSize small_button_size )
{
    itemPanelShip = AddPage( parent, _("Own Ship") );

    ownShip = new wxBoxSizer( wxVERTICAL );
    itemPanelShip->SetSizer( ownShip );

    //      OwnShip Display options
    wxStaticBox* osdBox = new wxStaticBox( itemPanelShip, wxID_ANY, _("Display Options") );
    dispOptions = new wxStaticBoxSizer( osdBox, wxVERTICAL );
    ownShip->Add( dispOptions, 0, wxTOP | wxALL | wxEXPAND, border_size );
#ifdef __WXOSX__
    wxFlexGridSizer* dispOptionsGrid = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    wxFlexGridSizer* dispOptionsGrid = new wxFlexGridSizer( 2, 2, group_item_spacing, group_item_spacing );
#endif
    dispOptionsGrid->AddGrowableCol( 1 );
    dispOptions->Add( dispOptionsGrid, 0, wxALL | wxEXPAND, border_size );

    wxStaticText *pStatic_OSCOG_Predictor = new wxStaticText( itemPanelShip, wxID_ANY, _("COG Predictor Length (min)") );
    dispOptionsGrid->Add( pStatic_OSCOG_Predictor, 0 );

    m_pText_OSCOG_Predictor = new wxTextCtrl( itemPanelShip, wxID_ANY );
    dispOptionsGrid->Add( m_pText_OSCOG_Predictor, 0, wxALIGN_RIGHT );

    wxStaticText *pStatic_OSHDT_Predictor = new wxStaticText( itemPanelShip, wxID_ANY, _("Heading Predictor Length (NMi)") );
    dispOptionsGrid->Add( pStatic_OSHDT_Predictor, 0 );
    
    m_pText_OSHDT_Predictor = new wxTextCtrl( itemPanelShip, wxID_ANY );
    dispOptionsGrid->Add( m_pText_OSHDT_Predictor, 0, wxALIGN_RIGHT );
    
    wxStaticText *iconTypeTxt = new wxStaticText( itemPanelShip, wxID_ANY, _("Ship Icon Type") );
    dispOptionsGrid->Add( iconTypeTxt, 0 );

    wxString iconTypes[] = { _("Default"), _("Real Scale Bitmap"), _("Real Scale Vector") };
    m_pShipIconType = new wxChoice( itemPanelShip, ID_SHIPICONTYPE, wxDefaultPosition, wxDefaultSize, 3, iconTypes );
    dispOptionsGrid->Add( m_pShipIconType, 0, wxALIGN_RIGHT | wxLEFT|wxRIGHT|wxTOP, group_item_spacing );
#ifdef __WXOSX__
    realSizes = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    realSizes = new wxFlexGridSizer( 5, 2, group_item_spacing, group_item_spacing );
#endif
    realSizes->AddGrowableCol(1);

    dispOptions->Add( realSizes, 0, wxEXPAND | wxLEFT, 30 );

    realSizes->Add( new wxStaticText( itemPanelShip, wxID_ANY, _("Length Over All (m)") ), 1, wxALIGN_LEFT );
    m_pOSLength = new wxTextCtrl( itemPanelShip, 1 );
    realSizes->Add( m_pOSLength, 1, wxALIGN_RIGHT | wxALL, group_item_spacing );

    realSizes->Add( new wxStaticText( itemPanelShip, wxID_ANY, _("Width Over All (m)") ), 1, wxALIGN_LEFT );
    m_pOSWidth = new wxTextCtrl( itemPanelShip, wxID_ANY );
    realSizes->Add( m_pOSWidth, 1, wxALIGN_RIGHT | wxALL, group_item_spacing );

    realSizes->Add( new wxStaticText( itemPanelShip, wxID_ANY, _("GPS Offset from Bow (m)") ), 1, wxALIGN_LEFT );
    m_pOSGPSOffsetY = new wxTextCtrl( itemPanelShip, wxID_ANY );
    realSizes->Add( m_pOSGPSOffsetY, 1, wxALIGN_RIGHT | wxALL, group_item_spacing );

    realSizes->Add( new wxStaticText( itemPanelShip, wxID_ANY, _("GPS Offset from Midship (m)") ), 1, wxALIGN_LEFT );
    m_pOSGPSOffsetX = new wxTextCtrl( itemPanelShip, wxID_ANY );
    realSizes->Add( m_pOSGPSOffsetX, 1, wxALIGN_RIGHT | wxALL, group_item_spacing );

    realSizes->Add( new wxStaticText( itemPanelShip, wxID_ANY, _("Minimum Screen Size (mm)") ), 1, wxALIGN_LEFT );
    m_pOSMinSize = new wxTextCtrl( itemPanelShip, wxID_ANY );
    realSizes->Add( m_pOSMinSize, 1, wxALIGN_RIGHT | wxALL, group_item_spacing );

    // Radar rings
#ifdef __WXOSX__
    wxFlexGridSizer* rrSelect = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    wxFlexGridSizer* rrSelect = new wxFlexGridSizer( 1, 2, group_item_spacing, group_item_spacing );
#endif
    rrSelect->AddGrowableCol( 1 );
    dispOptions->Add( rrSelect, 0, wxLEFT|wxRIGHT | wxEXPAND, border_size );

    wxStaticText *rrTxt = new wxStaticText( itemPanelShip, wxID_ANY, _("Show range rings") );
    rrSelect->Add( rrTxt, 1, wxEXPAND | wxALL, group_item_spacing );

    wxString rrAlt[] = { _("None"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("10") };
    pNavAidRadarRingsNumberVisible = new wxChoice( itemPanelShip, ID_RADARRINGS, wxDefaultPosition, m_pShipIconType->GetSize(), 11, rrAlt );
    rrSelect->Add( pNavAidRadarRingsNumberVisible, 0, wxALIGN_RIGHT | wxALL, group_item_spacing );
#ifdef __WXOSX__
    radarGrid = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    radarGrid = new wxFlexGridSizer( 2, 2, group_item_spacing, group_item_spacing );
#endif
    radarGrid->AddGrowableCol( 1 );
    dispOptions->Add( radarGrid, 0, wxLEFT | wxEXPAND, 30 );

    wxStaticText* distanceText = new wxStaticText( itemPanelShip, wxID_STATIC, _("Distance Between Rings") );
    radarGrid->Add( distanceText, 1, wxEXPAND | wxALL, group_item_spacing );

    pNavAidRadarRingsStep = new wxTextCtrl( itemPanelShip, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 100, -1 ), 0 );
    radarGrid->Add( pNavAidRadarRingsStep, 0, wxALIGN_RIGHT | wxALL, group_item_spacing );

    wxStaticText* unitText = new wxStaticText( itemPanelShip, wxID_STATIC, _("Distance Unit") );
    radarGrid->Add( unitText, 1, wxEXPAND | wxALL, group_item_spacing );

    wxString pDistUnitsStrings[] = { _("Nautical Miles"), _("Kilometers") };
    m_itemRadarRingsUnits = new wxChoice( itemPanelShip, ID_RADARDISTUNIT, wxDefaultPosition, m_pShipIconType->GetSize(), 2, pDistUnitsStrings );
    radarGrid->Add( m_itemRadarRingsUnits, 0, wxALIGN_RIGHT | wxALL, border_size );

    //  Tracks
    wxStaticBox* trackText = new wxStaticBox( itemPanelShip, wxID_ANY, _("Tracks") );
    wxStaticBoxSizer* trackSizer = new wxStaticBoxSizer( trackText, wxVERTICAL );
    ownShip->Add( trackSizer, 0, wxGROW | wxALL, border_size );

    pTrackDaily = new wxCheckBox( itemPanelShip, ID_DAILYCHECKBOX, _("Automatic Daily Tracks") );
    trackSizer->Add( pTrackDaily, 1, wxALL, border_size );

    pTrackHighlite = new wxCheckBox( itemPanelShip, ID_TRACKHILITE, _("Highlight Tracks") );
    trackSizer->Add( pTrackHighlite, 1, wxALL, border_size );
#ifdef __WXOSX__
    wxFlexGridSizer *pTrackGrid = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    wxFlexGridSizer *pTrackGrid = new wxFlexGridSizer( 1, 2, group_item_spacing, group_item_spacing );
#endif
    pTrackGrid->AddGrowableCol( 1 );
    trackSizer->Add( pTrackGrid, 0, wxALL | wxEXPAND, border_size );

    wxStaticText* tpText = new wxStaticText( itemPanelShip, wxID_STATIC, _("Tracking Precision") );
    pTrackGrid->Add( tpText, 1, wxEXPAND | wxALL, group_item_spacing );

    wxString trackAlt[] = { _("Low"), _("Medium"), _("High") };
    pTrackPrecision = new wxChoice( itemPanelShip, wxID_ANY, wxDefaultPosition, m_pShipIconType->GetSize(), 3, trackAlt );
    pTrackGrid->Add( pTrackPrecision, 0, wxALIGN_RIGHT | wxALL, group_item_spacing );

    //  Routes
    wxStaticBox* routeText = new wxStaticBox( itemPanelShip, wxID_ANY, _("Routes") );
    wxStaticBoxSizer* routeSizer = new wxStaticBoxSizer( routeText, wxVERTICAL );
    ownShip->Add( routeSizer, 0, wxGROW | wxALL, border_size );

    pSailing = new wxCheckBox( itemPanelShip, ID_DAILYCHECKBOX, _("Advance route waypoint on arrival only") );
    routeSizer->Add( pSailing, 1, wxALL, border_size );

#ifdef __WXOSX__
    wxFlexGridSizer *pRouteGrid = new wxFlexGridSizer( 2, group_item_spacing, group_item_spacing );
#else
    wxFlexGridSizer *pRouteGrid = new wxFlexGridSizer( 1, 2, group_item_spacing, group_item_spacing );
#endif
    pRouteGrid->AddGrowableCol( 1 );
    routeSizer->Add( pRouteGrid, 0, wxALL | wxEXPAND, border_size );

    wxStaticText* raText = new wxStaticText( itemPanelShip, wxID_STATIC, _("Waypoint Arrival Circle Radius (NMi)") );
    pRouteGrid->Add( raText, 1, wxEXPAND | wxALL, group_item_spacing );

    m_pText_ACRadius = new wxTextCtrl( itemPanelShip, -1 );
    pRouteGrid->Add( m_pText_ACRadius, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );
    
    DimeControl( itemPanelShip );
}

void options::CreatePanel_ChartsLoad( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    wxScrolledWindow *chartPanelWin = AddPage( m_pageCharts, _("Chart Files") );

    chartPanel = new wxBoxSizer( wxVERTICAL );
    chartPanelWin->SetSizer( chartPanel );

    wxStaticBox* loadedBox = new wxStaticBox( chartPanelWin, wxID_ANY, _("Directories") );
    activeSizer = new wxStaticBoxSizer( loadedBox, wxHORIZONTAL );
    chartPanel->Add( activeSizer, 1, wxALL | wxEXPAND, border_size );

    wxString* pListBoxStrings = NULL;
    pActiveChartsList = new wxListBox( chartPanelWin, ID_LISTBOX, wxDefaultPosition, wxDefaultSize,
             0, pListBoxStrings, wxLB_MULTIPLE );

    activeSizer->Add( pActiveChartsList, 1, wxALL | wxEXPAND, border_size );

    pActiveChartsList->Connect(  wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( options::OnChartDirListSelect ), NULL, this );

    wxBoxSizer* cmdButtonSizer = new wxBoxSizer( wxVERTICAL );
    activeSizer->Add( cmdButtonSizer, 0, wxALL, border_size );

    // Currently loaded chart dirs
    wxString dirname;
    if( pActiveChartsList ) {
        pActiveChartsList->Clear();
        int nDir = m_CurrentDirList.GetCount();
        for( int i = 0; i < nDir; i++ ) {
            dirname = m_CurrentDirList.Item( i ).fullpath;
            if( !dirname.IsEmpty() ) pActiveChartsList->Append( dirname );
        }
    }

    wxButton* addBtn = new wxButton( chartPanelWin, ID_BUTTONADD, _("Add Directory...") );
    cmdButtonSizer->Add( addBtn, 1, wxALL | wxEXPAND, group_item_spacing );

    m_removeBtn = new wxButton( chartPanelWin, ID_BUTTONDELETE, _("Remove Selected") );
    cmdButtonSizer->Add( m_removeBtn, 1, wxALL | wxEXPAND, group_item_spacing );
    m_removeBtn->Disable();


    wxStaticBox* itemStaticBoxUpdateStatic = new wxStaticBox( chartPanelWin, wxID_ANY,
            _("Update Control") );
    wxStaticBoxSizer* itemStaticBoxSizerUpdate = new wxStaticBoxSizer( itemStaticBoxUpdateStatic,
            wxVERTICAL );
    chartPanel->Add( itemStaticBoxSizerUpdate, 0, wxGROW | wxALL, 5 );

    pScanCheckBox = new wxCheckBox( chartPanelWin, ID_SCANCHECKBOX,
            _("Scan Charts and Update Database") );
    itemStaticBoxSizerUpdate->Add( pScanCheckBox, 1, wxALL, 5 );

    pUpdateCheckBox = new wxCheckBox( chartPanelWin, ID_UPDCHECKBOX,
            _("Force Full Database Rebuild") );
    itemStaticBoxSizerUpdate->Add( pUpdateCheckBox, 1, wxALL, 5 );

    chartPanel->Layout();

    chartPanelWin->SetScrollRate(5, 5);

}

void options::CreatePanel_Advanced( size_t parent, int border_size, int group_item_spacing,
                                       wxSize small_button_size )
{
    m_ChartDisplayPage = AddPage( parent, _("Advanced") );
    
    wxFlexGridSizer* itemBoxSizerUI = new wxFlexGridSizer( 2 );
    itemBoxSizerUI->SetHGap(border_size);
//    itemBoxSizerUI->AddGrowableCol( 0, 1 );
//    itemBoxSizerUI->AddGrowableCol( 1, 1 );
//    m_ChartDisplayPage->SetSizer( itemBoxSizerUI );
    
    // wxFlexGridSizer grows wrongly in wx2.8, so we need to centre it in another sizer instead of letting it grow.
    wxBoxSizer* wrapperSizer = new wxBoxSizer( wxVERTICAL );
    m_ChartDisplayPage->SetSizer( wrapperSizer );
    wrapperSizer->Add( itemBoxSizerUI, 1, wxALL | wxALIGN_CENTER, border_size );
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );
    
    // Chart Display Options
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Chart Display") ), groupLabelFlags );
    wxBoxSizer* boxCharts = new wxBoxSizer( wxVERTICAL );
    itemBoxSizerUI->Add( boxCharts, groupInputFlags );
    
    pSkewComp = new wxCheckBox( m_ChartDisplayPage, ID_SKEWCOMPBOX, _("Show Skewed Raster Charts as North-Up") );
    boxCharts->Add( pSkewComp, inputFlags );
    
    pFullScreenQuilt = new wxCheckBox( m_ChartDisplayPage, ID_FULLSCREENQUILT, _("Disable Full Screen Quilting") );
    boxCharts->Add( pFullScreenQuilt, inputFlags );

    pOverzoomEmphasis = new wxCheckBox( m_ChartDisplayPage, ID_FULLSCREENQUILT, _("Suppress blur/fog effects on overzoom") );
    boxCharts->Add( pOverzoomEmphasis, inputFlags );
    
    pOZScaleVector = new wxCheckBox( m_ChartDisplayPage, ID_FULLSCREENQUILT, _("Suppress scaled vector charts on overzoom") );
    boxCharts->Add( pOZScaleVector, inputFlags );

    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );
    
    //  Course Up display update period
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Course-Up Update Period") ), labelFlags );
    wxBoxSizer *pCOGUPFilterRow = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizerUI->Add( pCOGUPFilterRow, 0, wxALL | wxEXPAND, group_item_spacing );
    
    pCOGUPUpdateSecs = new wxTextCtrl( m_ChartDisplayPage, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 50, -1 ), wxTE_RIGHT  );
    pCOGUPFilterRow->Add( pCOGUPUpdateSecs, 0, wxALIGN_RIGHT | wxALL, group_item_spacing );
    
    pCOGUPFilterRow->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("seconds") ), inputFlags );
    
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );
    
    
    // Chart Zoom Scale Weighting
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Chart Zoom/Scale Weighting") ), labelFlags );
    m_pSlider_Zoom = new wxSlider( m_ChartDisplayPage, ID_CM93ZOOM, 0, -5,
                                  5, wxDefaultPosition, wxSize( 300, 50),
                                  wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS );
    itemBoxSizerUI->Add( m_pSlider_Zoom, inputFlags );
    
    itemBoxSizerUI->Add( 0, border_size*3 );
    wxStaticText* zoomText = new wxStaticText( m_ChartDisplayPage, wxID_ANY,
                                              _("With a lower value, the same zoom level shows a less detailed chart.\nWith a higher value, the same zoom level shows a more detailed chart.") );

    wxFont *dialogFont = GetOCPNScaledFont(_("Dialog"));
    smallFont = new wxFont( * dialogFont ); // we can't use Smaller() because wx2.8 doesn't support it
    smallFont->SetPointSize( (smallFont->GetPointSize() / 1.2) + 0.5 ); // + 0.5 to round instead of truncate
    zoomText->SetFont( * smallFont );
    itemBoxSizerUI->Add( zoomText, 0, wxALL | wxEXPAND, group_item_spacing );
    
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );
    
    
    // Control Options
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Controls") ), groupLabelFlags );
    wxBoxSizer* boxCtrls = new wxBoxSizer( wxVERTICAL );
    itemBoxSizerUI->Add( boxCtrls, groupInputFlags );
    
    pWayPointPreventDragging = new wxCheckBox( m_ChartDisplayPage, ID_DRAGGINGCHECKBOX, _("Lock Waypoints (Unless waypoint property dialog visible)") );
    pWayPointPreventDragging->SetValue( FALSE );
    boxCtrls->Add( pWayPointPreventDragging, inputFlags );
    
    pConfirmObjectDeletion = new wxCheckBox( m_ChartDisplayPage, ID_DELETECHECKBOX, _("Confirm deletion of tracks and routes") );
    pConfirmObjectDeletion->SetValue( FALSE );
    boxCtrls->Add( pConfirmObjectDeletion, inputFlags );
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );
    
    
    itemBoxSizerUI->Add( 0, border_size*3 );
    pPlayShipsBells = new wxCheckBox( m_ChartDisplayPage, ID_BELLSCHECKBOX, _("Play Ships Bells"));
    itemBoxSizerUI->Add( pPlayShipsBells, inputFlags );
    
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );

    //  Display size/DPI
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Physical Screen Width") ), labelFlags );
    wxBoxSizer *pDPIRow = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizerUI->Add( pDPIRow, 0, wxEXPAND );
    
    pRBSizeAuto = new wxRadioButton( m_ChartDisplayPage, wxID_ANY, _("Auto") );
    pDPIRow->Add( pRBSizeAuto, inputFlags );
    
    pDPIRow->AddSpacer( 10 );
    pRBSizeManual = new wxRadioButton( m_ChartDisplayPage, ID_SIZEMANUALRADIOBUTTON, _("Manual:") );
    pDPIRow->Add( pRBSizeManual, inputFlags );
    
    pScreenMM = new wxTextCtrl( m_ChartDisplayPage, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 50, -1 ), wxTE_RIGHT  );
    pDPIRow->Add( pScreenMM, 0, wxALIGN_RIGHT | wxALL, group_item_spacing );

    pDPIRow->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("mm") ), inputFlags );

    pRBSizeAuto->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                         wxCommandEventHandler( options::OnSizeAutoButton ), NULL, this );
    pRBSizeManual->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                           wxCommandEventHandler( options::OnSizeManualButton ), NULL, this );
    
    // spacer
    itemBoxSizerUI->Add( 0, border_size*3 );
    itemBoxSizerUI->Add( 0, border_size*3 );

    // OpenGL Options
    itemBoxSizerUI->Add( new wxStaticText( m_ChartDisplayPage, wxID_ANY, _("Graphics") ), labelFlags );
    wxBoxSizer* OpenGLSizer = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizerUI->Add( OpenGLSizer, 0, 0, 0 );
    
    pOpenGL = new wxCheckBox( m_ChartDisplayPage, ID_OPENGLBOX, _("Use Accelerated Graphics (OpenGL)") );
    OpenGLSizer->Add( pOpenGL, inputFlags );
    pOpenGL->Enable(!g_bdisable_opengl);
    
    wxButton *bOpenGL = new wxButton( m_ChartDisplayPage, ID_OPENGLOPTIONS, _("Options...") );
    OpenGLSizer->Add( bOpenGL, inputFlags );
    bOpenGL->Enable(!g_bdisable_opengl);

}

void options::CreatePanel_VectorCharts( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    ps57Ctl = AddPage( parent, _("Vector Chart Display") );

    vectorPanel = new wxBoxSizer( wxHORIZONTAL );
    ps57Ctl->SetSizer( vectorPanel );

    // 1st column, all options except Mariner's Standard
    wxFlexGridSizer* optionsColumn = new wxFlexGridSizer(2);
    optionsColumn->SetHGap(border_size);
    optionsColumn->AddGrowableCol( 0, 2 );
    optionsColumn->AddGrowableCol( 1, 3 );
    vectorPanel->Add( optionsColumn, 3, wxALL | wxEXPAND, border_size );
    
    // spacer
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _T("")) );
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _T("")) );

    // dislay category
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Display Category")), labelFlags );
    wxString pDispCatStrings[] = { _("Base"), _("Standard"), _("All"), _("Mariner's Standard") };
    pDispCat = new wxChoice( ps57Ctl, ID_RADARDISTUNIT, wxDefaultPosition,
                            wxDefaultSize, 4, pDispCatStrings );
    optionsColumn->Add( pDispCat, 0, wxALL, 2 );
    
    
    // spacer
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _T("")) );
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _T("")) );
    
    
    // display options
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Display")), groupLabelFlags );
    
    wxBoxSizer* miscSizer = new wxBoxSizer( wxVERTICAL );
    optionsColumn->Add( miscSizer, groupInputFlags );

    pCheck_SOUNDG = new wxCheckBox( ps57Ctl, ID_SOUNDGCHECKBOX, _("Depth Soundings") );
    pCheck_SOUNDG->SetValue( FALSE );
    miscSizer->Add( pCheck_SOUNDG, inputFlags );

    pCheck_META = new wxCheckBox( ps57Ctl, ID_METACHECKBOX, _("Chart Information Objects") );
    pCheck_META->SetValue( FALSE );
    miscSizer->Add( pCheck_META, inputFlags );

    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Buoys/Lights")), groupLabelFlags );
    
    wxBoxSizer* lightSizer = new wxBoxSizer( wxVERTICAL );
    optionsColumn->Add( lightSizer, groupInputFlags );

    pCheck_ATONTEXT = new wxCheckBox( ps57Ctl, ID_ATONTEXTCHECKBOX, _("Buoy/Light Labels") );
    pCheck_ATONTEXT->SetValue( FALSE );
    lightSizer->Add( pCheck_ATONTEXT, inputFlags );

    pCheck_LDISTEXT = new wxCheckBox( ps57Ctl, ID_LDISTEXTCHECKBOX, _("Light Descriptions") );
    pCheck_LDISTEXT->SetValue( FALSE );
    lightSizer->Add( pCheck_LDISTEXT, inputFlags );

    pCheck_XLSECTTEXT = new wxCheckBox( ps57Ctl, ID_LDISTEXTCHECKBOX, _("Extended Light Sectors") );
    pCheck_XLSECTTEXT->SetValue( FALSE );
    lightSizer->Add( pCheck_XLSECTTEXT, inputFlags );

    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Chart Texts")), groupLabelFlags );
    
    wxBoxSizer* textSizer = new wxBoxSizer( wxVERTICAL );
    optionsColumn->Add( textSizer, groupInputFlags );

    pCheck_NATIONALTEXT = new wxCheckBox( ps57Ctl, ID_NATIONALTEXTCHECKBOX, _("National text on chart") );
    pCheck_NATIONALTEXT->SetValue( FALSE );
    textSizer->Add( pCheck_NATIONALTEXT, inputFlags );
    
    pCheck_SHOWIMPTEXT = new wxCheckBox( ps57Ctl, ID_IMPTEXTCHECKBOX, _("Important Text Only") );
    pCheck_SHOWIMPTEXT->SetValue( FALSE );
    textSizer->Add( pCheck_SHOWIMPTEXT, inputFlags );
    
    pCheck_DECLTEXT = new wxCheckBox( ps57Ctl, ID_DECLTEXTCHECKBOX, _("De-Cluttered Text") );
    pCheck_DECLTEXT->SetValue( FALSE );
    textSizer->Add( pCheck_DECLTEXT, inputFlags );
    
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Chart Detail")), labelFlags );
    pCheck_SCAMIN = new wxCheckBox( ps57Ctl, ID_SCAMINCHECKBOX, _("Reduced Detail at Small Scale") );
    pCheck_SCAMIN->SetValue( FALSE );
    optionsColumn->Add( pCheck_SCAMIN, inputFlags );

    // spacer
    optionsColumn->Add( 0, border_size*4 );
    optionsColumn->Add( 0, border_size*4 );
    
    
    // graphics options
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Graphics Style")), labelFlags );
    wxString pPointStyleStrings[] = { _("Paper Chart"), _("Simplified"), };
    pPointStyle = new wxChoice( ps57Ctl, ID_RADARDISTUNIT, wxDefaultPosition,
                               wxDefaultSize, 2, pPointStyleStrings );
    optionsColumn->Add( pPointStyle, inputFlags );
    
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Boundaries")), labelFlags );
    wxString pBoundStyleStrings[] = { _("Plain"), _("Symbolized"), };
    pBoundStyle = new wxChoice( ps57Ctl, ID_RADARDISTUNIT, wxDefaultPosition,
                               wxDefaultSize, 2, pBoundStyleStrings );
    optionsColumn->Add( pBoundStyle, inputFlags );
    
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("Colors")), labelFlags );
    wxString pColorNumStrings[] = { _("2 Color"), _("4 Color"), };
    p24Color = new wxChoice( ps57Ctl, ID_RADARDISTUNIT, wxDefaultPosition,
                            wxDefaultSize, 2, pColorNumStrings );
    optionsColumn->Add( p24Color, inputFlags );

    // spacer
    optionsColumn->Add( 0, border_size*4 );
    optionsColumn->Add( 0, border_size*4 );

    // depth options
    optionsColumn->Add( new wxStaticText( ps57Ctl, wxID_ANY, _("Shallow Depth") ), labelFlags );
    wxBoxSizer* depShalRow = new wxBoxSizer( wxHORIZONTAL );
    optionsColumn->Add( depShalRow );
    m_ShallowCtl = new wxTextCtrl( ps57Ctl, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 60, -1 ), wxTE_RIGHT );
    depShalRow->Add( m_ShallowCtl, inputFlags );
    m_depthUnitsShal = new wxStaticText( ps57Ctl, wxID_ANY, _("metres") );
    depShalRow->Add( m_depthUnitsShal, inputFlags );

    optionsColumn->Add( new wxStaticText( ps57Ctl, wxID_ANY, _("Safety Depth") ), labelFlags );
    wxBoxSizer* depSafeRow = new wxBoxSizer( wxHORIZONTAL );
    optionsColumn->Add( depSafeRow );
    m_SafetyCtl = new wxTextCtrl( ps57Ctl, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 60, -1 ), wxTE_RIGHT );
    depSafeRow->Add( m_SafetyCtl, inputFlags );
    m_depthUnitsSafe = new wxStaticText( ps57Ctl, wxID_ANY, _("metres") );
    depSafeRow->Add( m_depthUnitsSafe, inputFlags );

    optionsColumn->Add( new wxStaticText( ps57Ctl, wxID_ANY, _("Deep Depth") ), labelFlags );
    wxBoxSizer* depDeepRow = new wxBoxSizer( wxHORIZONTAL );
    optionsColumn->Add( depDeepRow );
    m_DeepCtl = new wxTextCtrl( ps57Ctl, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize( 60, -1 ), wxTE_RIGHT );
    depDeepRow->Add( m_DeepCtl, inputFlags );
    m_depthUnitsDeep = new wxStaticText( ps57Ctl, wxID_ANY, _("metres") );
    depDeepRow->Add( m_depthUnitsDeep, inputFlags );

    // spacer
    optionsColumn->Add( 0, border_size*4 );
    optionsColumn->Add( 0, border_size*4 );

#ifdef USE_S57
    optionsColumn->Add( new wxStaticText(ps57Ctl, wxID_ANY, _("CM93 Detail Level")), labelFlags );
    m_pSlider_CM93_Zoom = new wxSlider( ps57Ctl, ID_CM93ZOOM, 0, -CM93_ZOOM_FACTOR_MAX_RANGE,
                                       CM93_ZOOM_FACTOR_MAX_RANGE, wxDefaultPosition, wxSize( 140, 50),
                                       wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS );
    optionsColumn->Add( m_pSlider_CM93_Zoom, 0, wxALL | wxEXPAND, border_size );
    //    cm93Sizer->SetSizeHints(cm93DetailBox);
#endif

    // 2nd column, Display Category / Mariner's Standard options
    wxBoxSizer* dispSizer = new wxBoxSizer( wxVERTICAL );
    vectorPanel->Add( dispSizer, 2, wxALL | wxEXPAND, border_size );
    
    wxStaticBox* marinersBox = new wxStaticBox( ps57Ctl, wxID_ANY, _("Mariner's Standard") );
    wxStaticBoxSizer* marinersSizer = new wxStaticBoxSizer( marinersBox, wxVERTICAL );
    dispSizer->Add( marinersSizer, 1, wxALL | wxEXPAND, border_size );
    
    wxString* ps57CtlListBoxStrings = NULL;
    ps57CtlListBox = new wxCheckListBox( ps57Ctl, ID_CHECKLISTBOX, wxDefaultPosition,
                                        wxSize( 250, 350 ), 0, ps57CtlListBoxStrings, wxLB_SINGLE | wxLB_HSCROLL | wxLB_SORT );
    marinersSizer->Add( ps57CtlListBox, 1, wxALL | wxEXPAND, group_item_spacing );
    
    wxBoxSizer* btnRow = new wxBoxSizer( wxHORIZONTAL );
    itemButtonSelectList = new wxButton( ps57Ctl, ID_SELECTLIST, _("Select All") );
    btnRow->Add( itemButtonSelectList, 1, wxALL | wxEXPAND, group_item_spacing );
    itemButtonClearList = new wxButton( ps57Ctl, ID_CLEARLIST, _("Clear All") );
    btnRow->Add( itemButtonClearList, 1, wxALL | wxEXPAND, group_item_spacing );
    marinersSizer->Add( btnRow );
    
//    m_choicePrecision->SetSelection( g_NMEAAPBPrecision );
}

void options::CreatePanel_TidesCurrents( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    wxScrolledWindow *tcPanel = AddPage( parent, _("Tides && Currents") );

    wxBoxSizer* mainHBoxSizer = new wxBoxSizer( wxVERTICAL );
    tcPanel->SetSizer( mainHBoxSizer );

    wxStaticBox *tcBox = new wxStaticBox( tcPanel, wxID_ANY, _("Active Datasets" ));
    wxStaticBoxSizer* tcSizer = new wxStaticBoxSizer( tcBox, wxHORIZONTAL );
    mainHBoxSizer->Add( tcSizer, 1, wxALL | wxEXPAND, border_size );

    tcDataSelected = new wxListBox( tcPanel, ID_TIDESELECTED, wxDefaultPosition, wxDefaultSize );

    tcSizer->Add( tcDataSelected, 1, wxALL | wxEXPAND, border_size );

    //  Populate Selection List Control with the contents
    //  of the Global static array
    for( unsigned int id = 0 ; id < TideCurrentDataSet.Count() ; id++ ) {
        tcDataSelected->Append( TideCurrentDataSet.Item(id) );
    }

    //    Add the "Insert/Remove" buttons
    wxButton *insertButton = new wxButton( tcPanel, ID_TCDATAADD, _("Add Dataset...") );
    wxButton *removeButton = new wxButton( tcPanel, ID_TCDATADEL, _("Remove Selected") );

    wxBoxSizer* btnSizer = new wxBoxSizer( wxVERTICAL );
    tcSizer->Add( btnSizer );

    btnSizer->Add( insertButton, 1, wxALL | wxEXPAND, group_item_spacing );
    btnSizer->Add( removeButton, 1, wxALL | wxEXPAND, group_item_spacing );
}

void options::CreatePanel_ChartGroups( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    // Special case for adding the tab here. We know this page has multiple tabs,
    // and we have the actual widgets in a separate class (because of its complexity)

    m_groupsPage = m_pListbook->GetPage( parent );
    groupsPanel = new ChartGroupsUI( m_groupsPage );

    groupsPanel->CreatePanel( parent, border_size, group_item_spacing, small_button_size );
    ((wxNotebook *)m_groupsPage)->AddPage( groupsPanel, _("Chart Groups") );

    m_groupsPage->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxListbookEventHandler( options::OnChartsPageChange ), NULL, this );

}

void ChartGroupsUI::CreatePanel( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    modified = false;
    m_border_size = border_size;
    m_group_item_spacing = group_item_spacing;

    groupsSizer = new wxFlexGridSizer( 4, 2, border_size, border_size );
    groupsSizer->AddGrowableCol( 0 );
    groupsSizer->AddGrowableRow( 1, 1 );
    groupsSizer->AddGrowableRow( 3, 1 );

    SetSizer( groupsSizer );

    m_UIcomplete = false;
}

void ChartGroupsUI::CompletePanel( void )
{
    //    The chart file/dir tree
    wxStaticText *allChartsLabel = new wxStaticText( this, wxID_ANY, _("All Available Charts") );
    groupsSizer->Add( allChartsLabel, 0, wxTOP | wxRIGHT | wxLEFT, m_border_size );

    wxStaticText *dummy1 = new wxStaticText( this, -1, _T("") );
    groupsSizer->Add( dummy1 );

    wxBoxSizer* activeListSizer = new wxBoxSizer( wxVERTICAL );
    groupsSizer->Add( activeListSizer, 1, wxALL | wxEXPAND, 5 );

    allAvailableCtl = new wxGenericDirCtrl( this, ID_GROUPAVAILABLE, _T(""), wxDefaultPosition, wxDefaultSize, wxVSCROLL );
    activeListSizer->Add( allAvailableCtl, 1, wxEXPAND );

    m_pAddButton = new wxButton( this, ID_GROUPINSERTDIR, _("Add") );
    m_pAddButton->Disable();
    m_pRemoveButton = new wxButton( this, ID_GROUPREMOVEDIR, _("Remove Chart") );
    m_pRemoveButton->Disable();

    wxBoxSizer* addRemove = new wxBoxSizer( wxVERTICAL );
    addRemove->Add( m_pAddButton, 0, wxALL | wxEXPAND, m_group_item_spacing );
    groupsSizer->Add( addRemove, 0, wxALL | wxEXPAND, m_border_size );

    //    Add the Groups notebook control
    wxStaticText *groupsLabel = new wxStaticText( this, wxID_ANY, _("Chart Groups") );
    groupsSizer->Add( groupsLabel, 0, wxTOP | wxRIGHT | wxLEFT, m_border_size );

    wxStaticText *dummy2 = new wxStaticText( this, -1, _T("") );
    groupsSizer->Add( dummy2 );

    wxBoxSizer* nbSizer = new wxBoxSizer( wxVERTICAL );
    m_GroupNB = new wxNotebook( this, ID_GROUPNOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
    nbSizer->Add( m_GroupNB, 1, wxEXPAND );
    groupsSizer->Add( nbSizer, 1, wxALL | wxEXPAND, m_border_size );

    //    Add default (always present) Default Chart Group
    wxPanel *allActiveGroup = new wxPanel( m_GroupNB, -1, wxDefaultPosition, wxDefaultSize );
    m_GroupNB->AddPage( allActiveGroup, _("All Charts") );

    wxBoxSizer* page0BoxSizer = new wxBoxSizer( wxHORIZONTAL );
    allActiveGroup->SetSizer( page0BoxSizer );

    defaultAllCtl = new wxGenericDirCtrl( allActiveGroup, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxVSCROLL );

    //    Set the Font for the All Active Chart Group tree to be italic, dimmed
    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    iFont = new wxFont(*qFont);
    iFont->SetStyle(wxFONTSTYLE_ITALIC);
    iFont->SetWeight(wxFONTWEIGHT_LIGHT);

    page0BoxSizer->Add( defaultAllCtl, 1, wxALIGN_TOP | wxALL | wxEXPAND );

    m_DirCtrlArray.Add( defaultAllCtl );

    //    Add the Chart Group (page) "New" and "Delete" buttons
    m_pNewGroupButton = new wxButton( this, ID_GROUPNEWGROUP, _("New Group...") );
    m_pDeleteGroupButton = new wxButton( this, ID_GROUPDELETEGROUP, _("Delete Group") );

    wxBoxSizer* newDeleteGrp = new wxBoxSizer( wxVERTICAL );
    groupsSizer->Add( newDeleteGrp, 0, wxALL, m_border_size );

    newDeleteGrp->AddSpacer( 25 );
    newDeleteGrp->Add( m_pNewGroupButton, 0, wxALL | wxEXPAND, m_group_item_spacing );
    newDeleteGrp->Add( m_pDeleteGroupButton, 0, wxALL | wxEXPAND, m_group_item_spacing );
    newDeleteGrp->AddSpacer( 25 );
    newDeleteGrp->Add( m_pRemoveButton, 0, wxALL | wxEXPAND, m_group_item_spacing );

    // Connect this last, otherwise handler is called before all objects are initialized.
    this->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED,
    wxTreeEventHandler(ChartGroupsUI::OnAvailableSelection), NULL, this );

    m_UIcomplete = true;

}

void options::CreatePanel_Display( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    pDisplayPanel = AddPage( parent, _("General") );

    wxFlexGridSizer *generalSizer = new wxFlexGridSizer( 2 );
    generalSizer->SetHGap(border_size);
    //    generalSizer->AddGrowableCol( 0, 1 );
    //    generalSizer->AddGrowableCol( 1, 1 );
    //    pDisplayPanel->SetSizer( generalSizer );

    // wxFlexGridSizer grows wrongly in wx2.8, so we need to centre it in another sizer instead of letting it grow.
    wxBoxSizer* wrapperSizer = new wxBoxSizer( wxVERTICAL );
    pDisplayPanel->SetSizer( wrapperSizer );
    wrapperSizer->Add( generalSizer, 1, wxALL | wxALIGN_CENTER, border_size );
    
    // spacer
    generalSizer->Add( 0, border_size*4 );
    generalSizer->Add( 0, border_size*4 );

    // Nav Mode
    generalSizer->Add( new wxStaticText( pDisplayPanel, wxID_ANY, _("Navigation Mode") ), groupLabelFlags );
    wxBoxSizer* boxNavMode = new wxBoxSizer( wxVERTICAL );
    generalSizer->Add( boxNavMode, groupInputFlags );

    wxBoxSizer* rowOrientation = new wxBoxSizer( wxHORIZONTAL );
    boxNavMode->Add( rowOrientation );
    
    pCBNorthUp = new wxRadioButton( pDisplayPanel, wxID_ANY, _("North Up") );
    rowOrientation->Add( pCBNorthUp, inputFlags );
    pCBCourseUp = new wxRadioButton( pDisplayPanel, ID_COURSEUPCHECKBOX, _("Course Up") );
    rowOrientation->Add( pCBCourseUp, wxSizerFlags(0).Align(wxALIGN_CENTRE_VERTICAL).Border(wxLEFT, group_item_spacing*2) );
    
    pCBLookAhead = new wxCheckBox( pDisplayPanel, ID_CHECK_LOOKAHEAD, _("Look Ahead Mode") );
    boxNavMode->Add( pCBLookAhead, inputFlags );

    // spacer
    generalSizer->Add( 0, border_size*4 );
    generalSizer->Add( 0, border_size*4 );

    // Control Options
    generalSizer->Add( new wxStaticText( pDisplayPanel, wxID_ANY, _("Chart Display") ), groupLabelFlags );
    wxBoxSizer* boxCharts = new wxBoxSizer( wxVERTICAL );
    generalSizer->Add( boxCharts, groupInputFlags );

    pCDOQuilting = new wxCheckBox( pDisplayPanel, ID_QUILTCHECKBOX1, _("Enable Chart Quilting") );
    boxCharts->Add( pCDOQuilting, inputFlags );
    
    pPreserveScale = new wxCheckBox( pDisplayPanel, ID_PRESERVECHECKBOX, _("Preserve Scale when Switching Charts") );
    boxCharts->Add( pPreserveScale, inputFlags );
    
    
    // spacer
    generalSizer->Add( 0, border_size*4 );
    generalSizer->Add( 0, border_size*4 );

    // Control Options
    generalSizer->Add( new wxStaticText( pDisplayPanel, wxID_ANY, _("Controls") ), groupLabelFlags );
    wxBoxSizer* boxCtrls = new wxBoxSizer( wxVERTICAL );
    generalSizer->Add( boxCtrls, groupInputFlags );
    
    pSmoothPanZoom = new wxCheckBox( pDisplayPanel, ID_SMOOTHPANZOOMBOX, _("Smooth Panning / Zooming") );
    boxCtrls->Add( pSmoothPanZoom, inputFlags );
    
    pEnableZoomToCursor = new wxCheckBox( pDisplayPanel, ID_ZTCCHECKBOX, _("Zoom to Cursor") );
    pEnableZoomToCursor->SetValue( FALSE );
    boxCtrls->Add( pEnableZoomToCursor, inputFlags );

    // spacer
    generalSizer->Add( 0, border_size*4 );
    generalSizer->Add( 0, border_size*4 );
    
    // Display Options
    generalSizer->Add( new wxStaticText( pDisplayPanel, wxID_ANY, _("Display Features") ), groupLabelFlags );
    wxBoxSizer* boxDisp = new wxBoxSizer( wxVERTICAL );
    generalSizer->Add( boxDisp, groupInputFlags );
    
    pSDisplayGrid = new wxCheckBox( pDisplayPanel, ID_CHECK_DISPLAYGRID, _("Show Grid") );
    boxDisp->Add( pSDisplayGrid, inputFlags );
    
    pCDOOutlines = new wxCheckBox( pDisplayPanel, ID_OUTLINECHECKBOX1, _("Show Chart Outlines") );
    boxDisp->Add( pCDOOutlines, inputFlags );
    
    pSDepthUnits = new wxCheckBox( pDisplayPanel, ID_SHOWDEPTHUNITSBOX1, _("Show Depth Units") );
    boxDisp->Add( pSDepthUnits, inputFlags );

}

void options::CreatePanel_Units( size_t parent, int border_size, int group_item_spacing,
                                wxSize small_button_size )
{
    wxScrolledWindow *panelUnits = AddPage( parent, _("Units") );
    
    wxFlexGridSizer *unitsSizer = new wxFlexGridSizer( 2 );
    unitsSizer->SetHGap(border_size);
    //    unitsSizer->AddGrowableCol( 0, 1 );
    //    unitsSizer->AddGrowableCol( 1, 1 );
    //    panelUnits->SetSizer( unitsSizer );
    
    // wxFlexGridSizer grows wrongly in wx2.8, so we need to centre it in another sizer instead of letting it grow.
    wxBoxSizer* wrapperSizer = new wxBoxSizer( wxVERTICAL );
    panelUnits->SetSizer( wrapperSizer );
    wrapperSizer->Add( unitsSizer, 1, wxALL | wxALIGN_CENTER, border_size );
    
    
    // spacer
    unitsSizer->Add( 0, border_size*4 );
    unitsSizer->Add( 0, border_size*4 );
    
    
    // distance units
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("Distance")), labelFlags );
    wxString pDistanceFormats[] = { _("Nautical miles"), _("Statute miles"), _("Kilometers"), _("Meters") };
    int m_DistanceFormatsNChoices = sizeof(pDistanceFormats) / sizeof(wxString);
    pDistanceFormat = new wxChoice( panelUnits, ID_DISTANCEUNITSCHOICE, wxDefaultPosition,
                                   wxDefaultSize, m_DistanceFormatsNChoices, pDistanceFormats );
    unitsSizer->Add( pDistanceFormat, inputFlags );
    
    
    // speed units
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("Speed")), labelFlags );
    wxString pSpeedFormats[] = { _("Knots"), _("Mph"), _("km/h"), _("m/s") };
    int m_SpeedFormatsNChoices = sizeof( pSpeedFormats ) / sizeof(wxString);
    pSpeedFormat = new wxChoice( panelUnits, ID_SPEEDUNITSCHOICE, wxDefaultPosition,
                                wxDefaultSize, m_SpeedFormatsNChoices, pSpeedFormats );
    unitsSizer->Add( pSpeedFormat, inputFlags );
    
    
    // depth units
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("Depth")), labelFlags );
    wxString pDepthUnitStrings[] = { _("Feet"), _("Meters"), _("Fathoms"), };
    pDepthUnitSelect = new wxChoice( panelUnits, ID_DEPTHUNITSCHOICE, wxDefaultPosition,
                                    wxDefaultSize, 3, pDepthUnitStrings );
    unitsSizer->Add( pDepthUnitSelect, inputFlags );
    
    
    // spacer
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _T("")) );
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _T("")) );
    
    
    // lat/long units
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("Lat/Long")), labelFlags );
    wxString pSDMMFormats[] = { _("Degrees, Decimal Minutes"), _("Decimal Degrees"), _("Degrees, Minutes, Seconds") };
    int m_SDMMFormatsNChoices = sizeof( pSDMMFormats ) / sizeof(wxString);
    pSDMMFormat = new wxChoice( panelUnits, ID_SDMMFORMATCHOICE, wxDefaultPosition,
                               wxDefaultSize, m_SDMMFormatsNChoices, pSDMMFormats );
    unitsSizer->Add( pSDMMFormat, inputFlags );
    
    
    // spacer
    unitsSizer->Add( 0, border_size*4 );
    unitsSizer->Add( 0, border_size*4 );
    
    
    // bearings (magnetic/true, variation)
    unitsSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("Bearings")), groupLabelFlags );
    
    wxBoxSizer* bearingsSizer = new wxBoxSizer( wxVERTICAL );
    unitsSizer->Add( bearingsSizer, 0, 0, 0 );
    
    //  "Mag Heading" checkbox
    pCBMagShow = new wxCheckBox( panelUnits, ID_MAGSHOWCHECKBOX, _("Show magnetic bearings and headings") );
    bearingsSizer->Add( pCBMagShow, 0, wxALL, group_item_spacing );
    
    //  Mag Heading user variation
    wxBoxSizer* magVarSizer = new wxBoxSizer( wxHORIZONTAL );
    bearingsSizer->Add( magVarSizer, 0, wxALL, group_item_spacing );
    
    wxStaticText* itemStaticTextUserVar = new wxStaticText( panelUnits, wxID_ANY, _("Assumed magnetic variation") );
    magVarSizer->Add( itemStaticTextUserVar, 0, wxALL | wxALIGN_CENTRE_VERTICAL, group_item_spacing );
    
    pMagVar = new wxTextCtrl( panelUnits, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    magVarSizer->Add( pMagVar, 0, wxALIGN_CENTRE_VERTICAL, group_item_spacing );
    
    magVarSizer->Add( new wxStaticText(panelUnits, wxID_ANY, _("deg (+W, -E)")),
                     0, wxALL | wxALIGN_CENTRE_VERTICAL, group_item_spacing );

}

void options::CreatePanel_MMSI( size_t parent, int border_size, int group_item_spacing, wxSize small_button_size )
{
    wxScrolledWindow *panelMMSI = AddPage( parent, _("MMSI Properties") );
    
    wxBoxSizer* MMSISizer = new wxBoxSizer( wxVERTICAL );
    panelMMSI->SetSizer( MMSISizer );
    
    // MMSI list control (panel)
    wxStaticBox* itemStaticBoxMMSI = new wxStaticBox( panelMMSI, wxID_ANY, _("MMSI Properties") );
    wxStaticBoxSizer* itemStaticBoxSizerMMSI = new wxStaticBoxSizer( itemStaticBoxMMSI, wxVERTICAL );
    MMSISizer->Add( itemStaticBoxSizerMMSI, 0, wxALL | wxEXPAND, border_size );
    
    MMSI_Props_Panel *pPropsPanel = new MMSI_Props_Panel( panelMMSI );
    
    pPropsPanel->UpdateMMSIList( );
    
    itemStaticBoxSizerMMSI->Add( pPropsPanel, 0, wxALL | wxEXPAND, border_size );
    
    panelMMSI->Layout();
}

void options::CreatePanel_AIS( size_t parent, int border_size, int group_item_spacing, wxSize small_button_size )
{
    wxScrolledWindow *panelAIS = AddPage( parent, _("AIS Targets") );

    wxBoxSizer* aisSizer = new wxBoxSizer( wxVERTICAL );
    panelAIS->SetSizer( aisSizer );

    //      CPA Box
    wxStaticBox* itemStaticBoxCPA = new wxStaticBox( panelAIS, wxID_ANY, _("CPA Calculation") );
    wxStaticBoxSizer* itemStaticBoxSizerCPA = new wxStaticBoxSizer( itemStaticBoxCPA, wxVERTICAL );
    aisSizer->Add( itemStaticBoxSizerCPA, 0, wxALL | wxEXPAND, border_size );

    wxFlexGridSizer *pCPAGrid = new wxFlexGridSizer( 2 );
    pCPAGrid->AddGrowableCol( 1 );
    itemStaticBoxSizerCPA->Add( pCPAGrid, 0, wxALL | wxEXPAND, border_size );

    m_pCheck_CPA_Max = new wxCheckBox( panelAIS, -1,
            _("No CPA Calculation if target range is greater than (NMi)") );
    pCPAGrid->Add( m_pCheck_CPA_Max, 0, wxALL, group_item_spacing );

    m_pText_CPA_Max = new wxTextCtrl( panelAIS, -1 );
    pCPAGrid->Add( m_pText_CPA_Max, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_CPA_Warn = new wxCheckBox( panelAIS, -1, _("Warn if CPA less than (NMi)") );
    pCPAGrid->Add( m_pCheck_CPA_Warn, 0, wxALL, group_item_spacing );

    m_pText_CPA_Warn = new wxTextCtrl( panelAIS, -1, _T(""), wxDefaultPosition,
            wxSize( -1, -1 ) );
    pCPAGrid->Add( m_pText_CPA_Warn, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_CPA_Warn->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED,
                                wxCommandEventHandler( options::OnCPAWarnClick ), NULL, this );
    
    
    m_pCheck_CPA_WarnT = new wxCheckBox( panelAIS, -1,
            _("...and TCPA is less than (min)") );
    pCPAGrid->Add( m_pCheck_CPA_WarnT, 0, wxALL, group_item_spacing );

    m_pText_CPA_WarnT = new wxTextCtrl( panelAIS, -1 );
    pCPAGrid->Add( m_pText_CPA_WarnT, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );

    //      Lost Targets
    wxStaticBox* lostBox = new wxStaticBox( panelAIS, wxID_ANY, _("Lost Targets") );
    wxStaticBoxSizer* lostSizer = new wxStaticBoxSizer( lostBox, wxVERTICAL );
    aisSizer->Add( lostSizer, 0, wxALL | wxEXPAND, 3 );

    wxFlexGridSizer *pLostGrid = new wxFlexGridSizer( 2 );
    pLostGrid->AddGrowableCol( 1 );
    lostSizer->Add( pLostGrid, 0, wxALL | wxEXPAND, border_size );

    m_pCheck_Mark_Lost = new wxCheckBox( panelAIS, -1, _("Mark targets as lost after (min)") );
    pLostGrid->Add( m_pCheck_Mark_Lost, 1, wxALL, group_item_spacing );

    m_pText_Mark_Lost = new wxTextCtrl( panelAIS, -1 );
    pLostGrid->Add( m_pText_Mark_Lost, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Remove_Lost = new wxCheckBox( panelAIS, -1,
            _("Remove lost targets after (min)") );
    pLostGrid->Add( m_pCheck_Remove_Lost, 1, wxALL, group_item_spacing );

    m_pText_Remove_Lost = new wxTextCtrl( panelAIS, -1 );
    pLostGrid->Add( m_pText_Remove_Lost, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    //      Display
    wxStaticBox* displBox = new wxStaticBox( panelAIS, wxID_ANY, _("Display") );
    wxStaticBoxSizer* displSizer = new wxStaticBoxSizer( displBox, wxHORIZONTAL );
    aisSizer->Add( displSizer, 0, wxALL | wxEXPAND, border_size );

    wxFlexGridSizer *pDisplayGrid = new wxFlexGridSizer( 2 );
    pDisplayGrid->AddGrowableCol( 1 );
    displSizer->Add( pDisplayGrid, 1, wxALL | wxEXPAND, border_size );

    m_pCheck_Show_COG = new wxCheckBox( panelAIS, -1, _("Show target COG predictor arrow, length (min)") );
    pDisplayGrid->Add( m_pCheck_Show_COG, 1, wxALL | wxEXPAND, group_item_spacing );

    m_pText_COG_Predictor = new wxTextCtrl( panelAIS, -1 );
    pDisplayGrid->Add( m_pText_COG_Predictor, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Show_Tracks = new wxCheckBox( panelAIS, -1, _("Show target tracks, length (min)") );
    pDisplayGrid->Add( m_pCheck_Show_Tracks, 1, wxALL, group_item_spacing );

    m_pText_Track_Length = new wxTextCtrl( panelAIS, -1 );
    pDisplayGrid->Add( m_pText_Track_Length, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Show_Moored = new wxCheckBox( panelAIS, -1, _("Hide anchored/moored targets, speed max (kn)") );
    pDisplayGrid->Add( m_pCheck_Show_Moored, 1, wxALL, group_item_spacing );

    m_pText_Moored_Speed = new wxTextCtrl( panelAIS, -1 );
    pDisplayGrid->Add( m_pText_Moored_Speed, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Show_Area_Notices = new wxCheckBox( panelAIS, -1,
            _("Show area notices (from AIS binary messages)") );
    pDisplayGrid->Add( m_pCheck_Show_Area_Notices, 1, wxALL, group_item_spacing );

    wxStaticText *pStatic_Dummy5 = new wxStaticText( panelAIS, -1, _T("") );
    pDisplayGrid->Add( pStatic_Dummy5, 1, wxALL | wxALL, group_item_spacing );

    m_pCheck_Draw_Target_Size = new wxCheckBox( panelAIS, -1, _("Show AIS targets real size") );
    pDisplayGrid->Add( m_pCheck_Draw_Target_Size, 1, wxALL, group_item_spacing );

    wxStaticText *pStatic_Dummy6 = new wxStaticText( panelAIS, -1, _T("") );
    pDisplayGrid->Add( pStatic_Dummy6, 1, wxALL | wxALL, group_item_spacing );

    m_pCheck_Show_Target_Name = new wxCheckBox( panelAIS, -1, _("Show names with AIS targets at scale greater than 1:") );
    pDisplayGrid->Add( m_pCheck_Show_Target_Name, 1, wxALL, group_item_spacing );

    m_pText_Show_Target_Name_Scale = new wxTextCtrl( panelAIS, -1 );
    pDisplayGrid->Add( m_pText_Show_Target_Name_Scale, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Wpl_Aprs = new wxCheckBox( panelAIS, -1, _("Treat WPL sentences as APRS position reports") );
    pDisplayGrid->Add( m_pCheck_Wpl_Aprs, 1, wxALL, group_item_spacing );

    wxStaticText *pStatic_Dummy7 = new wxStaticText( panelAIS, -1, _T("") );
    pDisplayGrid->Add( pStatic_Dummy7, 1, wxALL | wxALL, group_item_spacing );

    wxStaticText *pStatic_Dummy5a = new wxStaticText( panelAIS, -1, _T("") );
    pDisplayGrid->Add( pStatic_Dummy5a, 1, wxALL | wxALL, group_item_spacing );

    // Rollover
    wxStaticBox* rolloverBox = new wxStaticBox( panelAIS, wxID_ANY, _("Rollover") );
    wxStaticBoxSizer* rolloverSizer = new wxStaticBoxSizer( rolloverBox, wxVERTICAL );
    aisSizer->Add( rolloverSizer, 0, wxALL | wxEXPAND, border_size );

    wxStaticText *pStatic_Dummy4 = new wxStaticText( panelAIS, -1, _("\"Ship Name\" MMSI (Call Sign)") );
    rolloverSizer->Add( pStatic_Dummy4, 1, wxALL, 2 * group_item_spacing );

    m_pCheck_Rollover_Class = new wxCheckBox( panelAIS, -1, _("[Class] Type (Status)") );
    rolloverSizer->Add( m_pCheck_Rollover_Class, 1, wxALL, 2 * group_item_spacing );

    m_pCheck_Rollover_COG = new wxCheckBox( panelAIS, -1, _("SOG COG") );
    rolloverSizer->Add( m_pCheck_Rollover_COG, 1, wxALL, 2 * group_item_spacing );

    m_pCheck_Rollover_CPA = new wxCheckBox( panelAIS, -1, _("CPA TCPA") );
    rolloverSizer->Add( m_pCheck_Rollover_CPA, 1, wxALL, 2 * group_item_spacing );

    //      Alert Box
    wxStaticBox* alertBox = new wxStaticBox( panelAIS, wxID_ANY, _("CPA/TCPA Alerts") );
    wxStaticBoxSizer* alertSizer = new wxStaticBoxSizer( alertBox, wxVERTICAL );
    aisSizer->Add( alertSizer, 0, wxALL | wxEXPAND, group_item_spacing );

    wxFlexGridSizer *pAlertGrid = new wxFlexGridSizer( 2 );
    pAlertGrid->AddGrowableCol( 1 );
    alertSizer->Add( pAlertGrid, 0, wxALL | wxEXPAND, group_item_spacing );

    m_pCheck_AlertDialog = new wxCheckBox( panelAIS, ID_AISALERTDIALOG,
            _("Show CPA/TCPA Alert Dialog") );
    pAlertGrid->Add( m_pCheck_AlertDialog, 0, wxALL, group_item_spacing );

    wxButton *m_SelSound = new wxButton( panelAIS, ID_AISALERTSELECTSOUND,
            _("Select Alert Sound"), wxDefaultPosition, small_button_size, 0 );
    pAlertGrid->Add( m_SelSound, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_AlertAudio = new wxCheckBox( panelAIS, ID_AISALERTAUDIO,
                _("Play Sound on CPA/TCPA Alerts and DSC/SART emergencies.") );
    pAlertGrid->Add( m_pCheck_AlertAudio, 0, wxALL, group_item_spacing );

    wxButton *m_pPlay_Sound = new wxButton( panelAIS, ID_AISALERTTESTSOUND,
            _("Test Alert Sound"), wxDefaultPosition, small_button_size, 0 );
    pAlertGrid->Add( m_pPlay_Sound, 0, wxALL | wxALIGN_RIGHT, group_item_spacing );

    m_pCheck_Alert_Moored = new wxCheckBox( panelAIS, -1,
            _("Supress Alerts for anchored/moored targets") );
    pAlertGrid->Add( m_pCheck_Alert_Moored, 1, wxALL, group_item_spacing );

    wxStaticText *pStatic_Dummy2 = new wxStaticText( panelAIS, -1, _T("") );
    pAlertGrid->Add( pStatic_Dummy2, 1, wxALL | wxALL, group_item_spacing );

    m_pCheck_Ack_Timout = new wxCheckBox( panelAIS, -1,
            _("Enable Target Alert Acknowledge timeout (min)") );
    pAlertGrid->Add( m_pCheck_Ack_Timout, 1, wxALL, group_item_spacing );

    m_pText_ACK_Timeout = new wxTextCtrl( panelAIS, -1 );
    pAlertGrid->Add( m_pText_ACK_Timeout, 1, wxALL | wxALIGN_RIGHT, group_item_spacing );

    panelAIS->Layout();
}


void options::CreatePanel_UI( size_t parent, int border_size, int group_item_spacing,
        wxSize small_button_size )
{
    wxScrolledWindow *itemPanelFont = AddPage( parent, _("General Options") );


    m_itemBoxSizerFontPanel = new wxBoxSizer( wxVERTICAL );
    itemPanelFont->SetSizer( m_itemBoxSizerFontPanel );

    wxBoxSizer* langStyleBox = new wxBoxSizer( wxHORIZONTAL );
    m_itemBoxSizerFontPanel->Add( langStyleBox, 0, wxEXPAND | wxALL, border_size );

    wxStaticBox* itemLangStaticBox = new wxStaticBox( itemPanelFont, wxID_ANY,
            _("Language") );
    wxStaticBoxSizer* itemLangStaticBoxSizer = new wxStaticBoxSizer( itemLangStaticBox,
            wxVERTICAL );

    langStyleBox->Add( itemLangStaticBoxSizer, 1, wxEXPAND | wxALL, border_size );

    m_itemLangListBox = new wxChoice( itemPanelFont, ID_CHOICE_LANG );

    itemLangStaticBoxSizer->Add( m_itemLangListBox, 0, wxEXPAND | wxALL, border_size );

    wxStaticBox* itemFontStaticBox = new wxStaticBox( itemPanelFont, wxID_ANY,
            _("Fonts") );
    wxStaticBoxSizer* itemFontStaticBoxSizer = new wxStaticBoxSizer( itemFontStaticBox,
            wxHORIZONTAL );
    m_itemBoxSizerFontPanel->Add( itemFontStaticBoxSizer, 0, wxEXPAND | wxALL, border_size );

    m_itemFontElementListBox = new wxChoice( itemPanelFont, ID_CHOICE_FONTELEMENT );

    int nFonts = FontMgr::Get().GetNumFonts();
    for( int it = 0; it < nFonts; it++ ) {
        const wxString  & t = FontMgr::Get().GetDialogString( it );

        if( FontMgr::Get().GetConfigString(it).StartsWith( g_locale ) ) {
            m_itemFontElementListBox->Append(t);
        }
    }

    if( nFonts ) m_itemFontElementListBox->SetSelection( 0 );

    itemFontStaticBoxSizer->Add( m_itemFontElementListBox, 0, wxALL, border_size );

    wxButton* itemFontChooseButton = new wxButton( itemPanelFont, ID_BUTTONFONTCHOOSE,
            _("Choose Font..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFontStaticBoxSizer->Add( itemFontChooseButton, 0, wxALL, border_size );
#ifdef __WXGTK__
    wxButton* itemFontColorButton = new wxButton( itemPanelFont, ID_BUTTONFONTCOLOR,
            _("Choose Font Color..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFontStaticBoxSizer->Add( itemFontColorButton, 0, wxALL, border_size );
#endif
    wxStaticBox* itemStyleStaticBox = new wxStaticBox( itemPanelFont, wxID_ANY,
            _("Toolbar and Window Style") );
    wxStaticBoxSizer* itemStyleStaticBoxSizer = new wxStaticBoxSizer( itemStyleStaticBox,
            wxVERTICAL );
    langStyleBox->Add( itemStyleStaticBoxSizer, 1, wxEXPAND | wxALL, border_size );

    m_itemStyleListBox = new wxChoice( itemPanelFont, ID_STYLESCOMBOBOX );

    wxArrayPtrVoid styles = g_StyleManager->GetArrayOfStyles();
    for( unsigned int i = 0; i < styles.Count(); i++ ) {
        ocpnStyle::Style* style = (ocpnStyle::Style*) ( styles.Item( i ) );
        m_itemStyleListBox->Append( style->name );
    }
    m_itemStyleListBox->SetStringSelection( g_StyleManager->GetCurrentStyle()->name );
    itemStyleStaticBoxSizer->Add( m_itemStyleListBox, 1, wxEXPAND | wxALL, border_size );

    wxStaticBox* miscOptionsBox = new wxStaticBox( itemPanelFont, wxID_ANY, _("Interface Options") );
    wxStaticBoxSizer* miscOptions = new wxStaticBoxSizer( miscOptionsBox, wxVERTICAL );
    m_itemBoxSizerFontPanel->Add( miscOptions, 0, wxALL | wxEXPAND, border_size );

    pShowStatusBar = new wxCheckBox( itemPanelFont, ID_DEBUGCHECKBOX1, _("Show Status Bar") );
    pShowStatusBar->SetValue( FALSE );
    miscOptions->Add( pShowStatusBar, 0, wxALL, border_size );

#ifndef __WXOSX__
    pShowMenuBar = new wxCheckBox( itemPanelFont, wxID_ANY, _("Show Menu Bar") );
    pShowMenuBar->SetValue( FALSE );
    miscOptions->Add( pShowMenuBar, 0, wxALL, border_size );
#endif

    pShowChartBar = new wxCheckBox( itemPanelFont, wxID_ANY, _("Show Chart Bar") );
    pShowChartBar->SetValue( g_bShowChartBar );
    miscOptions->Add( pShowChartBar, 0, wxALL, border_size );

    pShowCompassWin = new wxCheckBox( itemPanelFont, wxID_ANY, _("Show Compass/GPS Status Window") );
    pShowCompassWin->SetValue( FALSE );
    miscOptions->Add( pShowCompassWin, 0, wxALL, border_size );
#ifdef __WXOSX__
    pFullScreenToolbar = new wxCheckBox( itemPanelFont, ID_FSTOOLBARCHECKBOX,
            _("Show Toolbar in Fullscreen Mode") );
    miscOptions->Add( pFullScreenToolbar, 0, wxALL, border_size );
#endif
    pTransparentToolbar = new wxCheckBox( itemPanelFont, ID_TRANSTOOLBARCHECKBOX,
            _("Enable Transparent Toolbar") );
    miscOptions->Add( pTransparentToolbar, 0, wxALL, border_size );
    if( g_bopengl ) pTransparentToolbar->Disable();

    //  Mobile/Touchscreen checkboxes
    pMobile = new wxCheckBox( itemPanelFont, ID_MOBILEBOX, _("Enable Touchscreen interface") );
    miscOptions->Add( pMobile, 0, wxALL, border_size );
    
    pResponsive = new wxCheckBox( itemPanelFont, ID_REPONSIVEBOX, _("Enable Tablet Scaled Graphics interface") );
    miscOptions->Add( pResponsive, 0, wxALL, border_size );
    
}

void options::CreateControls()
{
    int border_size = 4;
//    int check_spacing = 2;  // Not used
    int group_item_spacing = 2;           // use for items within one group, with Add(...wxALL)

    int font_size_y, font_descent, font_lead;
    GetTextExtent( _T("0"), NULL, &font_size_y, &font_descent, &font_lead );
    m_small_button_size = wxSize( -1, (int) ( 1.4 * ( font_size_y + font_descent + font_lead ) ) );

    //      Some members (pointers to controls) need to initialized
    pEnableZoomToCursor = NULL;
    pSmoothPanZoom = NULL;

    //      Check the display size.
    //      If "small", adjust some factors to squish out some more white space
    int width, height;
    ::wxDisplaySize( &width, &height );

    if(!g_bresponsive){
        if( height <= 800 ) {
            border_size = 2;
//            check_spacing = 2;  // Not used
            group_item_spacing = 1;
        }
    }

    labelFlags = wxSizerFlags(0).Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Border(wxALL, group_item_spacing);
    inputFlags = wxSizerFlags(0).Align(wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL).Border(wxALL, group_item_spacing);
    groupLabelFlags = wxSizerFlags(0).Align(wxALIGN_RIGHT | wxALIGN_TOP).Border(wxALL, group_item_spacing);
    groupInputFlags = wxSizerFlags(0).Align(wxALIGN_LEFT | wxALIGN_TOP).Border(wxBOTTOM, group_item_spacing*2).Expand();

#ifdef __WXGTK__
    groupLabelFlags.Border(wxTOP, group_item_spacing + 3);
#endif

    options* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer( wxVERTICAL );
    itemDialog1->SetSizer( itemBoxSizer2 );

    int flags = 0;
#ifndef __WXQT__
    flags = wxLB_TOP;
#endif
    
    m_pListbook = new wxListbook( itemDialog1, ID_NOTEBOOK, wxDefaultPosition, wxSize(-1, -1), flags);

#ifdef __WXMSW__
    //  Windows clips the width of listbook selectors to about twice icon size
    //  This makes the text render with ellipses if too large

    //  So, Measure and reduce the Font size on ListBook(ListView) selectors
    //  to allow text layout without ellipsis...
    wxBitmap tbmp = g_StyleManager->GetCurrentStyle()->GetIcon( _T("Display") );
    wxScreenDC sdc;
    int text_width = tbmp.GetWidth();
    if(sdc.IsOk())
        sdc.GetTextExtent(_T("Connections"), &text_width, NULL, NULL, NULL, GetOCPNScaledFont(_("Dialog")));
    
    if(text_width > tbmp.GetWidth() * 2 ){
        wxListView* lv = m_pListbook->GetListView();
        wxFont *qFont = GetOCPNScaledFont(_("Dialog"));         // to get type, weight, etc...
        
        wxFont *sFont = wxTheFontList->FindOrCreateFont( 10, qFont->GetFamily(), qFont->GetStyle(), qFont->GetWeight());
        lv->SetFont( *sFont );
    }
#endif
    
    m_topImgList = new wxImageList( 40, 40, true, 1 );
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

#if wxCHECK_VERSION(2, 8, 12)
    m_topImgList->Add( style->GetIcon( _T("Display") ) );
    m_topImgList->Add( style->GetIcon( _T("Charts") ) );
    m_topImgList->Add( style->GetIcon( _T("Connections") ) );
    m_topImgList->Add( style->GetIcon( _T("Ship") ) );
    m_topImgList->Add( style->GetIcon( _T("UI") ) );
    m_topImgList->Add( style->GetIcon( _T("Plugins") ) );
#else
    wxBitmap bmp;
    wxImage img;
    bmp = style->GetIcon( _T("Display") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
    bmp = style->GetIcon( _T("Charts") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
    bmp = style->GetIcon( _T("Connections") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
    bmp = style->GetIcon( _T("Ship") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
    bmp = style->GetIcon( _T("UI") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
    bmp = style->GetIcon( _T("Plugins") ); img = bmp.ConvertToImage(); img.ConvertAlphaToMask(128);
    bmp = wxBitmap( img ); m_topImgList->Add( bmp );
#endif

    m_pListbook->SetImageList( m_topImgList );
    itemBoxSizer2->Add( m_pListbook, 1,
            wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, border_size );

    wxBoxSizer* buttons = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer2->Add( buttons, 0, wxALIGN_RIGHT | wxALL, border_size );

    m_OKButton = new wxButton( itemDialog1, xID_OK, _("OK") );
    m_OKButton->SetDefault();
    buttons->Add( m_OKButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, border_size );

    m_CancelButton = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel") );
    buttons->Add( m_CancelButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, border_size );

    m_ApplyButton = new wxButton( itemDialog1, ID_APPLY, _("Apply") );
    buttons->Add( m_ApplyButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, border_size );

    m_pageDisplay = CreatePanel( _("Display") );
    CreatePanel_Display( m_pageDisplay, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_Units( m_pageDisplay, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_Advanced( m_pageDisplay, border_size, group_item_spacing, m_small_button_size );

    m_pageCharts = CreatePanel( _("Charts") );
    CreatePanel_ChartsLoad( m_pageCharts, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_VectorCharts( m_pageCharts, border_size, group_item_spacing, m_small_button_size );
    // ChartGroups must be created after ChartsLoad and must be at least third
    CreatePanel_ChartGroups( m_pageCharts, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_TidesCurrents( m_pageCharts, border_size, group_item_spacing, m_small_button_size );
    m_pageConnections = CreatePanel( _("Connections") );
    CreatePanel_NMEA( m_pageConnections, border_size, group_item_spacing, m_small_button_size );
    m_pageShips = CreatePanel( _("Ships") );
    CreatePanel_Ownship( m_pageShips, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_AIS( m_pageShips, border_size, group_item_spacing, m_small_button_size );
    CreatePanel_MMSI( m_pageShips, border_size, group_item_spacing, m_small_button_size );

    m_pageUI = CreatePanel( _("User Interface") );
    CreatePanel_UI( m_pageUI, border_size, group_item_spacing, m_small_button_size );

    m_pagePlugins = CreatePanel( _("Plugins") );
    itemPanelPlugins = AddPage( m_pagePlugins, _("Plugins") );

    itemBoxSizerPanelPlugins = new wxBoxSizer( wxVERTICAL );
    itemPanelPlugins->SetSizer( itemBoxSizerPanelPlugins );

    // load the disabled plugins finally because the user might want to enable them
    // I would prefer to change this so the plugins are only loaded if and when
    // they select the plugin page
    if(!g_bLoadedDisabledPlugins) {
        g_pi_manager->LoadAllPlugIns( g_Plugin_Dir, false );
        g_bLoadedDisabledPlugins = true;
    }

/*    Deferred
    //      Build the PlugIn Manager Panel
    m_pPlugInCtrl = new PluginListPanel( itemPanelPlugins, ID_PANELPIM, wxDefaultPosition,
            wxDefaultSize, g_pi_manager->GetPlugInArray() );
    m_pPlugInCtrl->SetScrollRate( 15, 15 );

    itemBoxSizerPanelPlugins->Add( m_pPlugInCtrl, 1, wxEXPAND|wxALL, border_size );
*/
    //      PlugIns can add panels, too
    if( g_pi_manager ) g_pi_manager->NotifySetupOptions();

    SetColorScheme( (ColorScheme) 0 );
    
    //Set the maximum size of the entire settings dialog
    SetSizeHints( -1, -1, width-100, height-100 );

    //  The s57 chart panel is the one which controls the minimum width required to avoid horizontal scroll bars
    vectorPanel->SetSizeHints( ps57Ctl );
    
    m_pListbook->Connect( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler( options::OnPageChange ), NULL, this );
}

void options::SetInitialPage( int page_sel)
{
    m_pListbook->SetSelection( page_sel );
    
    for (size_t i = 0; i < m_pListbook->GetPageCount(); i++)
    {
        wxNotebookPage* pg = m_pListbook->GetPage( i );
        
        if( pg->IsKindOf( CLASSINFO(wxNotebook))) {
            wxNotebook *nb = ((wxNotebook *)pg);
            nb->ChangeSelection(0);
        }
    }
}

void options::SetColorScheme( ColorScheme cs )
{
    DimeControl( this );

    wxListView* lv = m_pListbook->GetListView();
    lv->SetBackgroundColour(this->GetBackgroundColour());
}

void options::SetInitialSettings()
{
    wxString s;
    wxString dirname;

    // ChartsLoad

    int nDir = m_CurrentDirList.GetCount();

    for( int i = 0; i < nDir; i++ ) {
        dirname = m_CurrentDirList.Item( i ).fullpath;
        if( !dirname.IsEmpty() ) {
            if( pActiveChartsList ) {
                pActiveChartsList->Append( dirname );
            }
        }
    }

    // ChartGroups

    if( pActiveChartsList ) {
        UpdateWorkArrayFromTextCtl();
        groupsPanel->SetDBDirs( *m_pWorkDirList );

        // Make a deep copy of the current global Group Array
        groupsPanel->EmptyChartGroupArray( m_pGroupArray );
        delete m_pGroupArray;
        m_pGroupArray = groupsPanel->CloneChartGroupArray( g_pGroupArray );
        groupsPanel->SetGroupArray( m_pGroupArray );
        groupsPanel->SetInitialSettings();
    }

    if( m_pConfig ) {
        pShowStatusBar->SetValue( m_pConfig->m_bShowStatusBar );
#ifndef __WXOSX__
        pShowMenuBar->SetValue( m_pConfig->m_bShowMenuBar );
#endif
        pShowCompassWin->SetValue( m_pConfig->m_bShowCompassWin );
    }

    s.Printf( _T("%d"), g_COGAvgSec );
    pCOGUPUpdateSecs->SetValue( s );

    pCDOOutlines->SetValue( g_bShowOutlines );
    pCDOQuilting->SetValue( g_bQuiltEnable );
    pFullScreenQuilt->SetValue( !g_bFullScreenQuilt );
    pSDepthUnits->SetValue( g_bShowDepthUnits );
    pSkewComp->SetValue( g_bskew_comp );
    pMobile->SetValue( g_btouch );
    pResponsive->SetValue( g_bresponsive );
    pOverzoomEmphasis->SetValue( !g_fog_overzoom );
    pOZScaleVector->SetValue( !g_oz_vector_scale );

    pOpenGL->SetValue( g_bopengl );
    pSmoothPanZoom->SetValue( g_bsmoothpanzoom );
#if 0
    if( g_bEnableZoomToCursor || pEnableZoomToCursor->GetValue() ) {
        pSmoothPanZoom->SetValue( false );
        pSmoothPanZoom->Disable();
    }
#endif
    pCBMagShow->SetValue( g_bShowMag );
    
    s.Printf( _T("%4.1f"), g_UserVar );
    pMagVar->SetValue(s);
    
    pSDisplayGrid->SetValue( g_bDisplayGrid );

    pCBCourseUp->SetValue( g_bCourseUp );
    pCBNorthUp->SetValue( !g_bCourseUp );
    pCBLookAhead->SetValue( g_bLookAhead );

    if( fabs( wxRound( g_ownship_predictor_minutes ) - g_ownship_predictor_minutes ) > 1e-4 )
        s.Printf( _T("%6.2f"), g_ownship_predictor_minutes );
    else
        s.Printf( _T("%4.0f"), g_ownship_predictor_minutes );
    m_pText_OSCOG_Predictor->SetValue( s );

    if( fabs( wxRound( g_ownship_HDTpredictor_miles ) - g_ownship_HDTpredictor_miles ) > 1e-4 )
        s.Printf( _T("%6.2f"), g_ownship_HDTpredictor_miles );
    else
        s.Printf( _T("%4.0f"), g_ownship_HDTpredictor_miles );
    m_pText_OSHDT_Predictor->SetValue( s );
    
    m_pShipIconType->SetSelection( g_OwnShipIconType );
    wxCommandEvent eDummy;
    OnShipTypeSelect( eDummy );
    m_pOSLength->SetValue( wxString::Format( _T("%.1f"), g_n_ownship_length_meters ) );
    m_pOSWidth->SetValue( wxString::Format( _T("%.1f"), g_n_ownship_beam_meters ) );
    m_pOSGPSOffsetX->SetValue( wxString::Format( _T("%.1f"), g_n_gps_antenna_offset_x ) );
    m_pOSGPSOffsetY->SetValue( wxString::Format( _T("%.1f"), g_n_gps_antenna_offset_y ) );
    m_pOSMinSize->SetValue( wxString::Format( _T("%d"), g_n_ownship_min_mm ) );
    m_pText_ACRadius->SetValue( wxString::Format( _T("%.2f"), g_n_arrival_circle_radius ) );
    
    wxString buf;
    if( g_iNavAidRadarRingsNumberVisible > 10 ) g_iNavAidRadarRingsNumberVisible = 10;
    pNavAidRadarRingsNumberVisible->SetSelection( g_iNavAidRadarRingsNumberVisible );
    buf.Printf( _T("%.3f"), g_fNavAidRadarRingsStep );
    pNavAidRadarRingsStep->SetValue( buf );
    m_itemRadarRingsUnits->SetSelection( g_pNavAidRadarRingsStepUnits );
    OnRadarringSelect( eDummy );

    pWayPointPreventDragging->SetValue( g_bWayPointPreventDragging );
    pConfirmObjectDeletion->SetValue( g_bConfirmObjectDelete );

    pEnableZoomToCursor->SetValue( g_bEnableZoomToCursor );
#if 0
    if( pEnableZoomToCursor->GetValue() ) {
        pSmoothPanZoom->Disable();
    } else {
        pSmoothPanZoom->Enable();
    }
#endif

    pPreserveScale->SetValue( g_bPreserveScaleOnX );
    pPlayShipsBells->SetValue( g_bPlayShipsBells );
#ifdef __WXOSX__
    pFullScreenToolbar->SetValue( g_bFullscreenToolbar );
#endif
    pTransparentToolbar->SetValue( g_bTransparentToolbar );
    pSDMMFormat->Select( g_iSDMMFormat );
    pDistanceFormat->Select( g_iDistanceFormat );
    pSpeedFormat->Select( g_iSpeedFormat );

    pSailing->SetValue( g_bSailing );

    pTrackDaily->SetValue( g_bTrackDaily );
    pTrackHighlite->SetValue( g_bHighliteTracks );

    pTrackPrecision->SetSelection( g_nTrackPrecision );

    //    AIS Parameters
    //      CPA Box
    m_pCheck_CPA_Max->SetValue( g_bCPAMax );

    s.Printf( _T("%4.1f"), g_CPAMax_NM );
    m_pText_CPA_Max->SetValue( s );

    m_pCheck_CPA_Warn->SetValue( g_bCPAWarn );

    s.Printf( _T("%4.1f"), g_CPAWarn_NM );
    m_pText_CPA_Warn->SetValue( s );

    if(  m_pCheck_CPA_Warn->GetValue() ) {
        m_pCheck_CPA_WarnT->Enable();
        m_pCheck_CPA_WarnT->SetValue( g_bTCPA_Max );
    }
    else
        m_pCheck_CPA_WarnT->Disable();
    
    s.Printf( _T("%4.0f"), g_TCPA_Max );
    m_pText_CPA_WarnT->SetValue( s );
    
    //      Lost Targets
    m_pCheck_Mark_Lost->SetValue( g_bMarkLost );

    s.Printf( _T("%4.0f"), g_MarkLost_Mins );
    m_pText_Mark_Lost->SetValue( s );

    m_pCheck_Remove_Lost->SetValue( g_bRemoveLost );

    s.Printf( _T("%4.0f"), g_RemoveLost_Mins );
    m_pText_Remove_Lost->SetValue( s );

    //      Display
    m_pCheck_Show_COG->SetValue( g_bShowCOG );

    s.Printf( _T("%4.0f"), g_ShowCOG_Mins );
    m_pText_COG_Predictor->SetValue( s );

    m_pCheck_Show_Tracks->SetValue( g_bAISShowTracks );

    s.Printf( _T("%4.0f"), g_AISShowTracks_Mins );
    m_pText_Track_Length->SetValue( s );

    m_pCheck_Show_Moored->SetValue( !g_bShowMoored );

    s.Printf( _T("%4.1f"), g_ShowMoored_Kts );
    m_pText_Moored_Speed->SetValue( s );

    m_pCheck_Show_Area_Notices->SetValue( g_bShowAreaNotices );

    m_pCheck_Draw_Target_Size->SetValue( g_bDrawAISSize );

    m_pCheck_Show_Target_Name->SetValue( g_bShowAISName );

    s.Printf( _T("%d"), g_Show_Target_Name_Scale );
    m_pText_Show_Target_Name_Scale->SetValue( s );

    m_pCheck_Wpl_Aprs->SetValue( g_bWplIsAprsPosition );

    //      Alerts
    m_pCheck_AlertDialog->SetValue( g_bAIS_CPA_Alert );
    m_pCheck_AlertAudio->SetValue( g_bAIS_CPA_Alert_Audio );
    m_pCheck_Alert_Moored->SetValue( g_bAIS_CPA_Alert_Suppress_Moored );

    m_pCheck_Ack_Timout->SetValue( g_bAIS_ACK_Timeout );
    s.Printf( _T("%4.0f"), g_AckTimeout_Mins );
    m_pText_ACK_Timeout->SetValue( s );

    // Rollover
    m_pCheck_Rollover_Class->SetValue( g_bAISRolloverShowClass );
    m_pCheck_Rollover_COG->SetValue( g_bAISRolloverShowCOG );
    m_pCheck_Rollover_CPA->SetValue( g_bAISRolloverShowCPA );

    m_pSlider_Zoom->SetValue( g_chart_zoom_modifier );

    wxString screenmm;
    if(g_config_display_size_mm > 0){
        screenmm.Printf(_T("%d"), int(g_config_display_size_mm));
        pRBSizeManual->SetValue( true );
    }
    else{
        screenmm = _("Auto");
        pRBSizeAuto->SetValue( true );
        pScreenMM->Disable();
    }
    
    pScreenMM->SetValue(screenmm);

#ifdef USE_S57
    m_pSlider_CM93_Zoom->SetValue( g_cm93_zoom_factor );

//    Diplay Category
    if( ps52plib ) {

        //    S52 Primary Filters
        ps57CtlListBox->Clear();
        marinersStdXref.clear();

        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            wxString item;
            if( iPtr < ps52plib->OBJLDescriptions.size() ) {
                item = ps52plib->OBJLDescriptions[iPtr];
            } else {
                item = wxString( pOLE->OBJLName, wxConvUTF8 );
            }

            // The ListBox control will insert entries in sorted order, which means we need to
            // keep track of already inseted items that gets pushed down the line.
            int newpos = ps57CtlListBox->Append( item );
            marinersStdXref.push_back( newpos );
            for( size_t i=0; i<iPtr; i++ ) {
                if( marinersStdXref[i] >= newpos ) marinersStdXref[i]++;
            }

            ps57CtlListBox->Check( newpos, !( pOLE->nViz == 0 ) );
        }

        int nset = 2;                             // default OTHER
        switch( ps52plib->GetDisplayCategory() ){
            case ( DISPLAYBASE ):
                nset = 0;
                break;
            case ( STANDARD ):
                nset = 1;
                break;
            case ( OTHER ):
                nset = 2;
                break;
            case ( MARINERS_STANDARD ):
                nset = 3;
                break;
            default:
                nset = 3;
                break;
        }

        pDispCat->SetSelection( nset );

        ps57CtlListBox->Enable( MARINERS_STANDARD == ps52plib->GetDisplayCategory() );
        itemButtonClearList->Enable( MARINERS_STANDARD == ps52plib->GetDisplayCategory() );
        itemButtonSelectList->Enable( MARINERS_STANDARD == ps52plib->GetDisplayCategory() );

        //  Other Display Filters
        pCheck_SOUNDG->SetValue( ps52plib->m_bShowSoundg );
        pCheck_META->SetValue( ps52plib->m_bShowMeta );
        pCheck_SHOWIMPTEXT->SetValue( ps52plib->m_bShowS57ImportantTextOnly );
        pCheck_SCAMIN->SetValue( ps52plib->m_bUseSCAMIN );
        pCheck_ATONTEXT->SetValue( ps52plib->m_bShowAtonText );
        pCheck_LDISTEXT->SetValue( ps52plib->m_bShowLdisText );
        pCheck_XLSECTTEXT->SetValue( ps52plib->m_bExtendLightSectors );
        pCheck_DECLTEXT->SetValue( ps52plib->m_bDeClutterText );
        pCheck_NATIONALTEXT->SetValue( ps52plib->m_bShowNationalTexts );

        // Chart Display Style
        if( ps52plib->m_nSymbolStyle == PAPER_CHART ) pPointStyle->SetSelection( 0 );
        else
            pPointStyle->SetSelection( 1 );

        if( ps52plib->m_nBoundaryStyle == PLAIN_BOUNDARIES ) pBoundStyle->SetSelection( 0 );
        else
            pBoundStyle->SetSelection( 1 );

        if( S52_getMarinerParam( S52_MAR_TWO_SHADES ) == 1.0 ) p24Color->SetSelection( 0 );
        else
            p24Color->SetSelection( 1 );

        // Depths
        pDepthUnitSelect->SetSelection( ps52plib->m_nDepthUnitDisplay );
        UpdateOptionsUnits(); // sets depth values using the user's unit preference
    }
#endif

}

void options::UpdateOptionsUnits()
{
    int depthUnit = pDepthUnitSelect->GetSelection();
    
    // set depth unit labels
    wxString depthUnitStrings[] = { _("feet"), _("meters"), _("fathoms") };
    wxString depthUnitString = depthUnitStrings[depthUnit];
    m_depthUnitsShal->SetLabel(depthUnitString);
    m_depthUnitsSafe->SetLabel(depthUnitString);
    m_depthUnitsDeep->SetLabel(depthUnitString);
    
    // depth unit conversion factor
    float conv = 1;
    if ( depthUnit == 0 ) // feet
        conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
    else if ( depthUnit == 2 ) // fathoms
        conv = 0.3048f * 6; // 1 fathom is 6 feet
    
    // set depth input values
    wxString s;
    s.Printf( _T("%6.2f"), S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR ) / conv );
    s.Trim(false);
    m_ShallowCtl->SetValue( s );
    
    s.Printf( _T("%6.2f"), S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR ) / conv );
    s.Trim(false);
    m_SafetyCtl->SetValue( s );
    
    s.Printf( _T("%6.2f"), S52_getMarinerParam( S52_MAR_DEEP_CONTOUR ) / conv );
    s.Trim(false);
    m_DeepCtl->SetValue( s );
}

void options::OnSizeAutoButton( wxCommandEvent& event )
{
    pScreenMM->SetValue(_("Auto"));
    pScreenMM->Disable();
}

void options::OnSizeManualButton( wxCommandEvent& event )
{
    wxString screenmm;
    if(g_config_display_size_mm > 0){
        screenmm.Printf(_T("%d"), int(g_config_display_size_mm));
    }
    
    pScreenMM->SetValue(screenmm);
    pScreenMM->Enable();
    
}

void options::OnUnitsChoice( wxCommandEvent& event )
{
    UpdateOptionsUnits();
}

void options::OnCPAWarnClick( wxCommandEvent& event )
{
    if( m_pCheck_CPA_Warn->GetValue() ) {
        m_pCheck_CPA_WarnT->Enable();
    } else {
        m_pCheck_CPA_WarnT->SetValue( false );
        m_pCheck_CPA_WarnT->Disable();
    }
}

void options::OnShowGpsWindowCheckboxClick( wxCommandEvent& event )
{
    if( !m_cbNMEADebug->GetValue() ) {
        NMEALogWindow::Get().DestroyWindow();
    } else {
#ifdef __WXOSX__
        NMEALogWindow::Get().Create(this, 40);  // was: pParent , missing CheckBox update: m_cbNMEADebug->SetValue(false);
#else
        NMEALogWindow::Get().Create(pParent, 35);
#endif
        Raise();
    }
}

void options::OnZTCCheckboxClick( wxCommandEvent& event )
{
#if 0
    if( pEnableZoomToCursor->GetValue() ) {
        pSmoothPanZoom->Disable();
    } else {
        pSmoothPanZoom->Enable();
    }
#endif
}

void options::OnShipTypeSelect( wxCommandEvent& event )
{
    if( m_pShipIconType->GetSelection() == 0 ) {
        realSizes->ShowItems( false );
    } else {
        realSizes->ShowItems( true );
    }
    dispOptions->Layout();
    ownShip->Layout();
    itemPanelShip->Layout();
    itemPanelShip->Refresh();
    event.Skip();
}

void options::OnRadarringSelect( wxCommandEvent& event )
{
    if( pNavAidRadarRingsNumberVisible->GetSelection() == 0 ) {
        radarGrid->ShowItems( false );
    } else {
        radarGrid->ShowItems( true );
    }
    dispOptions->Layout();
    ownShip->Layout();
    itemPanelShip->Layout();
    itemPanelShip->Refresh();
    event.Skip();
}

void options::OnOpenGLOptions( wxCommandEvent& event )
{
#ifdef ocpnUSE_GL
    OpenGLOptionsDlg dlg(this, pOpenGL->GetValue());

    if(dlg.ShowModal() == wxID_OK) {
        if(g_bexpert)
            g_GLOptions.m_bUseAcceleratedPanning = dlg.m_cbUseAcceleratedPanning->GetValue();
        else
            g_GLOptions.m_bUseAcceleratedPanning = cc1->GetglCanvas()->CanAcceleratePanning();

        g_GLOptions.m_bTextureCompression = dlg.m_cbTextureCompression->GetValue();

        g_bShowFPS = dlg.m_cbShowFPS->GetValue();

        if(g_bexpert){
            g_GLOptions.m_bTextureCompressionCaching = dlg.m_cbTextureCompressionCaching->GetValue();
            g_GLOptions.m_iTextureMemorySize = dlg.m_sTextureMemorySize->GetValue();
        }
        else{
            g_GLOptions.m_bTextureCompressionCaching = g_GLOptions.m_bTextureCompression;
        }

        if(g_bopengl &&
           g_GLOptions.m_bTextureCompression != dlg.m_cbTextureCompression->GetValue()) {
            g_GLOptions.m_bTextureCompression = dlg.m_cbTextureCompression->GetValue();
            cc1->GetglCanvas()->SetupCompression();
            
            ::wxBeginBusyCursor();
            cc1->GetglCanvas()->ClearAllRasterTextures();
            ::wxEndBusyCursor();

        }
    }

    if(dlg.m_brebuild_cache) {
        Hide();
        cc1->Disable();
        BuildCompressedCache();
        cc1->Enable();
    }
#endif
}

void options::OnChartDirListSelect( wxCommandEvent& event )
{
    if(event.IsSelection()){
        m_removeBtn->Enable();
    }
    else{
        wxArrayInt sel;
        if(pActiveChartsList->GetSelections( sel ) )
            m_removeBtn->Enable();
        else
            m_removeBtn->Disable();
    }
    
}

void options::OnDisplayCategoryRadioButton( wxCommandEvent& event )
{
    int select = pDispCat->GetSelection();

    if( 3 == select ) {
        ps57CtlListBox->Enable();
        itemButtonClearList->Enable();
        itemButtonSelectList->Enable();
    }

    else {
        ps57CtlListBox->Disable();
        itemButtonClearList->Disable();
        itemButtonSelectList->Disable();
    }

    event.Skip();
}

void options::OnButtonClearClick( wxCommandEvent& event )
{
    int nOBJL = ps57CtlListBox->GetCount();
    for( int iPtr = 0; iPtr < nOBJL; iPtr++ )
        ps57CtlListBox->Check( iPtr, false );

    event.Skip();
}

void options::OnButtonSelectClick( wxCommandEvent& event )
{
    int nOBJL = ps57CtlListBox->GetCount();
    for( int iPtr = 0; iPtr < nOBJL; iPtr++ )
        ps57CtlListBox->Check( iPtr, true );

    event.Skip();
}

bool options::ShowToolTips()
{
    return TRUE;
}

void options::OnCharHook( wxKeyEvent& event ) {
    if( event.GetKeyCode() == WXK_RETURN ) {
        if( event.GetModifiers() == wxMOD_CONTROL ) {
            wxCommandEvent okEvent;
            okEvent.SetId( xID_OK );
            okEvent.SetEventType( wxEVT_COMMAND_BUTTON_CLICKED );
            GetEventHandler()->AddPendingEvent( okEvent );
        }
    }
    event.Skip();
}

void options::OnButtonaddClick( wxCommandEvent& event )
{
    wxString selDir;
    wxFileName dirname;
    wxDirDialog *dirSelector = new wxDirDialog( this, _("Add a directory containing chart files"),
            *pInit_Chart_Dir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );

    if( dirSelector->ShowModal() == wxID_CANCEL ) goto done;

    selDir = dirSelector->GetPath();
    dirname = wxFileName( selDir );

    pInit_Chart_Dir->Empty();
    if( !g_bportable )
        pInit_Chart_Dir->Append( dirname.GetPath() );

    if( g_bportable ) {
        wxFileName f( selDir );
        f.MakeRelativeTo( *pHome_Locn );
        pActiveChartsList->Append( f.GetFullPath() );
    } else
        pActiveChartsList->Append( selDir );

    k_charts |= CHANGE_CHARTS;

    pScanCheckBox->Disable();

    done:
    delete dirSelector;
    event.Skip();
}

void options::UpdateDisplayedChartDirList(ArrayOfCDI p)
{
    wxString dirname;
    if( pActiveChartsList ) {
        pActiveChartsList->Clear();
        int nDir = p.GetCount();
        for( int i = 0; i < nDir; i++ ) {
            dirname = p.Item( i ).fullpath;
            if( !dirname.IsEmpty() )
                pActiveChartsList->Append( dirname );
        }
    }
}

void options::UpdateWorkArrayFromTextCtl()
{
    wxString dirname;

    int n = pActiveChartsList->GetCount();
    if( m_pWorkDirList ) {
        m_pWorkDirList->Clear();
        for( int i = 0; i < n; i++ ) {
            dirname = pActiveChartsList->GetString( i );
            if( !dirname.IsEmpty() ) {
                //    This is a fix for OSX, which appends EOL to results of GetLineText()
                while( ( dirname.Last() == wxChar( _T('\n') ) )
                        || ( dirname.Last() == wxChar( _T('\r') ) ) )
                    dirname.RemoveLast();

                //    scan the current array to find a match
                //    if found, add the info to the work list, preserving the magic number
                //    If not found, make a new ChartDirInfo, and add it
                bool b_added = false;
//                        if(m_pCurrentDirList)
                {
                    int nDir = m_CurrentDirList.GetCount();

                    for( int i = 0; i < nDir; i++ ) {
                        if( m_CurrentDirList.Item( i ).fullpath == dirname ) {
                            ChartDirInfo cdi = m_CurrentDirList.Item( i );
                            m_pWorkDirList->Add( cdi );
                            b_added = true;
                            break;
                        }
                    }
                }
                if( !b_added ) {
                    ChartDirInfo cdin;
                    cdin.fullpath = dirname;
                    m_pWorkDirList->Add( cdin );
                }
            }
        }
    }
}

ConnectionParams *options::CreateConnectionParamsFromSelectedItem()
{
    if( !m_bNMEAParams_shown )
        return NULL;

    //  Special encoding for deleted connection
    if ( m_rbTypeSerial->GetValue() && m_comboPort->GetValue() == _T("Deleted" ))
        return NULL;

    //  DataStreams should be Input, Output, or Both
    if (!(m_cbInput->GetValue() || m_cbOutput->GetValue())) {
        OCPNMessageBox( NULL, _("Data connection must be input, output or both"), _("OpenCPN Info"), wxICON_HAND );

        return NULL;
    }

    if ( m_rbTypeSerial->GetValue() && m_comboPort->GetValue() == wxEmptyString )
    {
        OCPNMessageBox( NULL, _("You must select or enter the port..."), _("OpenCPN Info"), wxICON_HAND );
        return NULL;
    }
    //  TCP, GPSD and UDP require port field to be set.
    //  TCP clients, GPSD and UDP output sockets require an address
    else if ( m_rbTypeNet->GetValue()) {
        if (wxAtoi(m_tNetPort->GetValue()) == 0)
        {
            OCPNMessageBox( NULL, _("You must enter a port..."), _("OpenCPN Info"), wxICON_HAND );
            return NULL;
        }
        if (m_tNetAddress->GetValue() == wxEmptyString) {
            if ((m_rbNetProtoGPSD->GetValue()) ||
                (m_rbNetProtoUDP->GetValue() && m_cbOutput->GetValue()))
            {
                OCPNMessageBox( NULL, _("You must enter the address..."), _("OpenCPN Info"), wxICON_HAND );
                return NULL;
            } else {
                m_tNetAddress->SetValue(_T("0.0.0.0"));
            }
        }
    }

    ConnectionParams * pConnectionParams = new ConnectionParams();

    pConnectionParams->Valid = true;
    if ( m_rbTypeSerial->GetValue() )
        pConnectionParams->Type = SERIAL;
    else
        pConnectionParams->Type = NETWORK;
    
    //  Save the existing addr/port to allow closing of existing port
    pConnectionParams->LastNetworkAddress = pConnectionParams->NetworkAddress;
    pConnectionParams->LastNetworkPort = pConnectionParams->NetworkPort;
        
    pConnectionParams->NetworkAddress = m_tNetAddress->GetValue();
    pConnectionParams->NetworkPort = wxAtoi(m_tNetPort->GetValue());
    if ( m_rbNetProtoTCP->GetValue() )
        pConnectionParams->NetProtocol = TCP;
    else if ( m_rbNetProtoUDP->GetValue() )
        pConnectionParams->NetProtocol = UDP;
    else
        pConnectionParams->NetProtocol = GPSD;

    pConnectionParams->Baudrate = wxAtoi( m_choiceBaudRate->GetStringSelection() );
    pConnectionParams->Priority = wxAtoi( m_choicePriority->GetStringSelection() );
    pConnectionParams->ChecksumCheck = m_cbCheckCRC->GetValue();
    pConnectionParams->Garmin = m_cbGarminHost->GetValue();
    pConnectionParams->InputSentenceList = wxStringTokenize( m_tcInputStc->GetValue(), _T(",") );
    if ( m_rbIAccept->GetValue() )
        pConnectionParams->InputSentenceListType = WHITELIST;
    else
        pConnectionParams->InputSentenceListType = BLACKLIST;
    if (m_cbInput->GetValue()) {
        if (m_cbOutput->GetValue()) {
            pConnectionParams->IOSelect = DS_TYPE_INPUT_OUTPUT;
        } else {
            pConnectionParams->IOSelect = DS_TYPE_INPUT;
        }
    } else
        pConnectionParams->IOSelect = DS_TYPE_OUTPUT;

    pConnectionParams->OutputSentenceList = wxStringTokenize( m_tcOutputStc->GetValue(), _T(",") );
    if ( m_rbOAccept->GetValue() )
        pConnectionParams->OutputSentenceListType = WHITELIST;
    else
        pConnectionParams->OutputSentenceListType = BLACKLIST;
    pConnectionParams->Port = m_comboPort->GetValue().BeforeFirst(' ');
    pConnectionParams->Protocol = PROTO_NMEA0183;

    pConnectionParams->bEnabled = m_connection_enabled;
    pConnectionParams->b_IsSetup = false;
    
    return pConnectionParams;
}

void options::OnApplyClick( wxCommandEvent& event )
{
    ::wxBeginBusyCursor();

    m_returnChanges = 0;

    // Start with the stuff that requires intelligent validation.

    if( m_pShipIconType->GetSelection() > 0 ) {
        double n_ownship_length_meters;
        double n_ownship_beam_meters;
        double n_gps_antenna_offset_y;
        double n_gps_antenna_offset_x;
        long n_ownship_min_mm;
        m_pOSLength->GetValue().ToDouble( &n_ownship_length_meters );
        m_pOSWidth->GetValue().ToDouble( &n_ownship_beam_meters );
        m_pOSGPSOffsetX->GetValue().ToDouble( &n_gps_antenna_offset_x );
        m_pOSGPSOffsetY->GetValue().ToDouble( &n_gps_antenna_offset_y );
        m_pOSMinSize->GetValue().ToLong( &n_ownship_min_mm );
        wxString msg;
        if( n_ownship_length_meters <= 0 )
            msg += _("\n - your ship's length must be > 0");
        if( n_ownship_beam_meters <= 0 )
            msg += _("\n - your ship's beam must be > 0");
        if( fabs(n_gps_antenna_offset_x) > n_ownship_beam_meters/2.0 )
            msg += _("\n - your GPS offset from midship must be within your ship's beam");
        if( n_gps_antenna_offset_y < 0 || n_gps_antenna_offset_y > n_ownship_length_meters )
            msg += _("\n - your GPS offset from bow must be within your ship's length");
        if( n_ownship_min_mm <= 0 || n_ownship_min_mm > 100 )
            msg += _("\n - your minimum ship icon size must be between 1 and 100 mm");
        if( ! msg.IsEmpty() ) {
            msg.Prepend( _("The settings for own ship real size are not correct:") );
            OCPNMessageBox( this, msg, _("OpenCPN info"), wxICON_ERROR );
            ::wxEndBusyCursor();
            event.SetInt( wxID_STOP );
            return;
        }
        g_n_ownship_length_meters = n_ownship_length_meters;
        g_n_ownship_beam_meters = n_ownship_beam_meters;
        g_n_gps_antenna_offset_y = n_gps_antenna_offset_y;
        g_n_gps_antenna_offset_x = n_gps_antenna_offset_x;
        g_n_ownship_min_mm = (int)n_ownship_min_mm;
    }
    g_OwnShipIconType = m_pShipIconType->GetSelection();

    m_pText_ACRadius->GetValue().ToDouble( &g_n_arrival_circle_radius );
    
    //    Handle Chart Tab
    wxString dirname;

    if( pActiveChartsList ) {
        UpdateWorkArrayFromTextCtl();
    } else {
        m_pWorkDirList->Clear();
        int nDir = m_CurrentDirList.GetCount();

        for( int i = 0; i < nDir; i++ ) {
            ChartDirInfo cdi = m_CurrentDirList.Item( i );
            m_pWorkDirList->Add( cdi );
        }
    }
    groupsPanel->SetDBDirs( *m_pWorkDirList );          // update the Groups tab
    groupsPanel->m_treespopulated = false;

    int k_force = FORCE_UPDATE;
    if( pUpdateCheckBox ) {
        if( !pUpdateCheckBox->GetValue() ) k_force = 0;
        pUpdateCheckBox->Enable();
        pUpdateCheckBox->SetValue( false );
    } else {
        k_force = 0;
    }

    m_returnChanges |= k_force;

    int k_scan = SCAN_UPDATE;
    if( pScanCheckBox ) {
        if( !pScanCheckBox->GetValue() ) k_scan = 0;
        pScanCheckBox->Enable();
        pScanCheckBox->SetValue( false );
    } else {
        k_scan = 0;
    }

    m_returnChanges |= k_scan;

    // Chart Groups

    if( groupsPanel->modified ) {
        groupsPanel->EmptyChartGroupArray( g_pGroupArray );
        delete g_pGroupArray;
        g_pGroupArray = groupsPanel->CloneChartGroupArray( m_pGroupArray );
        m_returnChanges |= GROUPS_CHANGED;
    }

    // Handle Settings Tab

    if( m_pConfig ) {
        m_pConfig->m_bShowStatusBar = pShowStatusBar->GetValue();
#ifndef __WXOSX__
        m_pConfig->m_bShowMenuBar = pShowMenuBar->GetValue();
#endif
        m_pConfig->m_bShowCompassWin = pShowCompassWin->GetValue();
    }

    g_bShowChartBar = pShowChartBar->GetValue();

    wxString screenmm = pScreenMM->GetValue();
    long mm = -1;
    screenmm.ToLong(&mm);
    if(mm >0){
        g_config_display_size_mm = mm;
    }
    else{
        g_config_display_size_mm = -1;
    }

    // Connections page.
    g_bfilter_cogsog = m_cbFilterSogCog->GetValue();
    
    long filter_val = 1;
    m_tFilterSec->GetValue().ToLong( &filter_val );
    g_COGFilterSec = wxMin((int)filter_val, MAX_COGSOG_FILTER_SECONDS);
    g_COGFilterSec = wxMax(g_COGFilterSec, 1);
    g_SOGFilterSec = g_COGFilterSec;
    
    g_bMagneticAPB = m_cbAPBMagnetic->GetValue();
    g_NMEAAPBPrecision = m_choicePrecision->GetCurrentSelection();
    
    
    // NMEA Source
    long itemIndex = m_lcSources->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    
    //  If the stream selected exists, capture some of its existing parameters
    //  to facility identification and allow stop and restart of the stream
    wxString lastAddr;
    int lastPort = 0;
    if(itemIndex >=0){
        int params_index = m_lcSources->GetItemData( itemIndex );
        ConnectionParams *cpo = g_pConnectionParams->Item(params_index);
        if(cpo){
            lastAddr = cpo->NetworkAddress;
            lastPort = cpo->NetworkPort;
        }
    }
    
    if(!connectionsaved)
    {
        ConnectionParams * cp = CreateConnectionParamsFromSelectedItem();
        if(cp != NULL)
        {
            if (itemIndex >= 0)
            {
                int params_index = m_lcSources->GetItemData( itemIndex );
                g_pConnectionParams->RemoveAt(params_index);
                g_pConnectionParams->Insert(cp, params_index);
            }
            else
            {
                g_pConnectionParams->Add(cp);
                itemIndex = g_pConnectionParams->Count() - 1;
            }
            
            //  Record the previous parameters, if any
            cp->LastNetworkAddress = lastAddr;
            cp->LastNetworkPort = lastPort;
            
            FillSourceList();
            m_lcSources->SetItemState(itemIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_lcSources->Refresh();
            connectionsaved = true;
        } else {
            ::wxEndBusyCursor();
            if( m_bNMEAParams_shown )
                event.SetInt (wxID_STOP );
        }
    }
    
    //Recreate datastreams that are new, or have been edited
    for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
    {
        ConnectionParams *cp = g_pConnectionParams->Item(i);
        if( !cp->b_IsSetup ) {                  // Stream is new, or edited
            
            // Terminate and remove any existing stream with the same port name
            DataStream *pds_existing = g_pMUX->FindStream( cp->GetDSPort() );
            if(pds_existing)
                g_pMUX->StopAndRemoveStream( pds_existing );
            
            //  Try to stop any previous stream to avoid orphans
            pds_existing = g_pMUX->FindStream( cp->GetLastDSPort() );
            if(pds_existing)
                g_pMUX->StopAndRemoveStream( pds_existing );
            
            if( cp->bEnabled ) {
                dsPortType port_type = cp->IOSelect;
                DataStream *dstr = new DataStream( g_pMUX,
                                                  cp->GetDSPort(),
                                                  wxString::Format(wxT("%i"), cp->Baudrate),
                                                  port_type,
                                                  cp->Priority,
                                                  cp->Garmin
                                                  );
                dstr->SetInputFilter(cp->InputSentenceList);
                dstr->SetInputFilterType(cp->InputSentenceListType);
                dstr->SetOutputFilter(cp->OutputSentenceList);
                dstr->SetOutputFilterType(cp->OutputSentenceListType);
                dstr->SetChecksumCheck(cp->ChecksumCheck);
                
                g_pMUX->AddStream(dstr);
                
                cp->b_IsSetup = true;
            }
        }
    }
    
    g_bGarminHostUpload = m_cbGarminUploadHost->GetValue();
    if( m_cbFurunoGP3X->GetValue() )
        g_GPS_Ident = _T("FurunoGP3X");
    else
        g_GPS_Ident = _T("Generic");
    
    
    // End of Connections page

    g_bShowOutlines = pCDOOutlines->GetValue();
    g_bDisplayGrid = pSDisplayGrid->GetValue();

    bool temp_bquilting = pCDOQuilting->GetValue();
    if(!g_bQuiltEnable && temp_bquilting)
        cc1->ReloadVP(); /* compose the quilt */
    g_bQuiltEnable = temp_bquilting;
    g_bFullScreenQuilt = !pFullScreenQuilt->GetValue();

    g_bShowDepthUnits = pSDepthUnits->GetValue();
    g_bskew_comp = pSkewComp->GetValue();
    g_btouch = pMobile->GetValue();
    g_bresponsive = pResponsive->GetValue();

    g_fog_overzoom = !pOverzoomEmphasis->GetValue();
    g_oz_vector_scale = !pOZScaleVector->GetValue();

    bool bopengl_changed = g_bopengl != pOpenGL->GetValue();
    g_bopengl = pOpenGL->GetValue();

    g_bsmoothpanzoom = pSmoothPanZoom->GetValue();

    long update_val = 1;
    pCOGUPUpdateSecs->GetValue().ToLong( &update_val );
    g_COGAvgSec = wxMin((int)update_val, MAX_COG_AVERAGE_SECONDS);

    if(g_bCourseUp != pCBCourseUp->GetValue())
        gFrame->ToggleCourseUp();
        
    g_bLookAhead = pCBLookAhead->GetValue();

    g_bShowMag = pCBMagShow->GetValue();
    pMagVar->GetValue().ToDouble( &g_UserVar );
    
    m_pText_OSCOG_Predictor->GetValue().ToDouble( &g_ownship_predictor_minutes );
    m_pText_OSHDT_Predictor->GetValue().ToDouble( &g_ownship_HDTpredictor_miles );
    
    g_iNavAidRadarRingsNumberVisible = pNavAidRadarRingsNumberVisible->GetSelection();
    g_fNavAidRadarRingsStep = atof( pNavAidRadarRingsStep->GetValue().mb_str() );
    g_pNavAidRadarRingsStepUnits = m_itemRadarRingsUnits->GetSelection();
    g_bWayPointPreventDragging = pWayPointPreventDragging->GetValue();
    g_bConfirmObjectDelete = pConfirmObjectDeletion->GetValue();

    g_bPreserveScaleOnX = pPreserveScale->GetValue();

    g_bPlayShipsBells = pPlayShipsBells->GetValue();
#ifdef __WXOSX__
    g_bFullscreenToolbar = pFullScreenToolbar->GetValue();
#endif
    g_bTransparentToolbar = pTransparentToolbar->GetValue();
    g_iSDMMFormat = pSDMMFormat->GetSelection();
    g_iDistanceFormat = pDistanceFormat->GetSelection();
    g_iSpeedFormat = pSpeedFormat->GetSelection();

    g_bSailing = pSailing->GetValue();

    g_nTrackPrecision = pTrackPrecision->GetSelection();

    g_bTrackDaily = pTrackDaily->GetValue();
    g_bHighliteTracks = pTrackHighlite->GetValue();

    g_bEnableZoomToCursor = pEnableZoomToCursor->GetValue();

    //    AIS Parameters
    //      CPA Box
    g_bCPAMax = m_pCheck_CPA_Max->GetValue();
    m_pText_CPA_Max->GetValue().ToDouble( &g_CPAMax_NM );
    g_bCPAWarn = m_pCheck_CPA_Warn->GetValue();
    m_pText_CPA_Warn->GetValue().ToDouble( &g_CPAWarn_NM );
    g_bTCPA_Max = m_pCheck_CPA_WarnT->GetValue();
    m_pText_CPA_WarnT->GetValue().ToDouble( &g_TCPA_Max );

    //      Lost Targets
    g_bMarkLost = m_pCheck_Mark_Lost->GetValue();
    m_pText_Mark_Lost->GetValue().ToDouble( &g_MarkLost_Mins );
    g_bRemoveLost = m_pCheck_Remove_Lost->GetValue();
    m_pText_Remove_Lost->GetValue().ToDouble( &g_RemoveLost_Mins );

    //      Display
    g_bShowCOG = m_pCheck_Show_COG->GetValue();
    m_pText_COG_Predictor->GetValue().ToDouble( &g_ShowCOG_Mins );

    g_bAISShowTracks = m_pCheck_Show_Tracks->GetValue();
    m_pText_Track_Length->GetValue().ToDouble( &g_AISShowTracks_Mins );
    
    //  Update all the current targets
    if( g_pAIS ){
        AIS_Target_Hash::iterator it;
        AIS_Target_Hash *current_targets = g_pAIS->GetTargetList();
        for( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it ) {
            AIS_Target_Data *pAISTarget = it->second;
            if( NULL != pAISTarget ) {
                pAISTarget->b_show_track = g_bAISShowTracks;
            }
        }
    }
    

    g_bShowMoored = !m_pCheck_Show_Moored->GetValue();
    m_pText_Moored_Speed->GetValue().ToDouble( &g_ShowMoored_Kts );

    g_bShowAreaNotices = m_pCheck_Show_Area_Notices->GetValue();
    g_bDrawAISSize = m_pCheck_Draw_Target_Size->GetValue();
    g_bShowAISName = m_pCheck_Show_Target_Name->GetValue();
    long ais_name_scale = 5000;
    m_pText_Show_Target_Name_Scale->GetValue().ToLong( &ais_name_scale );
    g_Show_Target_Name_Scale = (int)wxMax( 5000, ais_name_scale );

    g_bWplIsAprsPosition = m_pCheck_Wpl_Aprs->GetValue();

    //      Alert
    g_bAIS_CPA_Alert = m_pCheck_AlertDialog->GetValue();
    g_bAIS_CPA_Alert_Audio = m_pCheck_AlertAudio->GetValue();
    g_bAIS_CPA_Alert_Suppress_Moored = m_pCheck_Alert_Moored->GetValue();

    g_bAIS_ACK_Timeout = m_pCheck_Ack_Timout->GetValue();
    m_pText_ACK_Timeout->GetValue().ToDouble( &g_AckTimeout_Mins );

    // Rollover
    g_bAISRolloverShowClass = m_pCheck_Rollover_Class->GetValue();
    g_bAISRolloverShowCOG = m_pCheck_Rollover_COG->GetValue();
    g_bAISRolloverShowCPA = m_pCheck_Rollover_CPA->GetValue();

    g_chart_zoom_modifier = m_pSlider_Zoom->GetValue();

#ifdef USE_S57
    //    Handle Vector Charts Tab

    g_cm93_zoom_factor = m_pSlider_CM93_Zoom->GetValue();

    int nOBJL = ps57CtlListBox->GetCount();

    for( int iPtr = 0; iPtr < nOBJL; iPtr++ ) {
        int itemIndex = -1;
        for( size_t i=0; i<marinersStdXref.size(); i++ ) {
            if( marinersStdXref[ i ] == iPtr ) {
                itemIndex = i;
                break;
            }
        }
        OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( itemIndex ) );
        pOLE->nViz = ps57CtlListBox->IsChecked( iPtr );
    }

    if( ps52plib ) {
        if( bopengl_changed ) {
            //    We need to do this now to handle the screen refresh that
            //    is automatically generated on Windows at closure of the options dialog...
            ps52plib->FlushSymbolCaches();
            ps52plib->ClearCNSYLUPArray();      // some CNSY depends on renderer (e.g. CARC)
            ps52plib->GenerateStateHash();

            m_returnChanges |= GL_CHANGED;
        }

        enum _DisCat nset = OTHER;
        switch( pDispCat->GetSelection() ){
            case 0:
                nset = DISPLAYBASE;
                break;
            case 1:
                nset = STANDARD;
                break;
            case 2:
                nset = OTHER;
                break;
            case 3:
                nset = MARINERS_STANDARD;
                break;
        }
        ps52plib-> SetDisplayCategory( nset );

        ps52plib->m_bShowSoundg = pCheck_SOUNDG->GetValue();
        ps52plib->m_bShowMeta = pCheck_META->GetValue();
        ps52plib->m_bShowS57ImportantTextOnly = pCheck_SHOWIMPTEXT->GetValue();
        ps52plib->m_bUseSCAMIN = pCheck_SCAMIN->GetValue();
        ps52plib->m_bShowAtonText = pCheck_ATONTEXT->GetValue();
        ps52plib->m_bShowLdisText = pCheck_LDISTEXT->GetValue();
        ps52plib->m_bExtendLightSectors = pCheck_XLSECTTEXT->GetValue();
        ps52plib->m_bDeClutterText = pCheck_DECLTEXT->GetValue();
        ps52plib->m_bShowNationalTexts = pCheck_NATIONALTEXT->GetValue();

        if( 0 == pPointStyle->GetSelection() )
            ps52plib->m_nSymbolStyle = PAPER_CHART;
        else
            ps52plib->m_nSymbolStyle = SIMPLIFIED;

        if( 0 == pBoundStyle->GetSelection() )
            ps52plib->m_nBoundaryStyle = PLAIN_BOUNDARIES;
        else
            ps52plib->m_nBoundaryStyle = SYMBOLIZED_BOUNDARIES;

        if( 0 == p24Color->GetSelection() )
            S52_setMarinerParam( S52_MAR_TWO_SHADES, 1.0 );
        else
            S52_setMarinerParam( S52_MAR_TWO_SHADES, 0.0 );

        // Depths
        double dval;
        int depthUnit = pDepthUnitSelect->GetSelection();
        
        float conv = 1;
        if ( depthUnit == 0 ) // feet
            conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
        else if ( depthUnit == 2 ) // fathoms
            conv = 0.3048f * 6; // 1 fathom is 6 feet

        if( ( m_SafetyCtl->GetValue() ).ToDouble( &dval ) ) {
            S52_setMarinerParam( S52_MAR_SAFETY_DEPTH, dval * conv );   // controls sounding display
            S52_setMarinerParam( S52_MAR_SAFETY_CONTOUR, dval * conv ); // controls colour
        }

        if( ( m_ShallowCtl->GetValue() ).ToDouble( &dval ) )
            S52_setMarinerParam( S52_MAR_SHALLOW_CONTOUR, dval * conv );

        if( ( m_DeepCtl->GetValue() ).ToDouble( &dval ) )
            S52_setMarinerParam( S52_MAR_DEEP_CONTOUR, dval * conv );

        ps52plib->UpdateMarinerParams();

        ps52plib->m_nDepthUnitDisplay = depthUnit;

        ps52plib->GenerateStateHash();
    }
#endif

//    User Interface Panel
    if( m_bVisitLang ) {
        wxString new_canon = _T("en_US");
        wxString lang_sel = m_itemLangListBox->GetStringSelection();

        int nLang = sizeof( lang_list ) / sizeof(int);
        for( int it = 0; it < nLang; it++ ) {
            const wxLanguageInfo * pli = wxLocale::GetLanguageInfo( lang_list[it] );
            if(pli){
                wxString lang_canonical = pli->CanonicalName;
                wxString test_string = GetOCPNKnownLanguage( lang_canonical, NULL );
                if( lang_sel == test_string ) {
                    new_canon = lang_canonical;
                    break;
                }
            }
        }

        wxString locale_old = g_locale;
        g_locale = new_canon;

        if( g_locale != locale_old ) m_returnChanges |= LOCALE_CHANGED;

        wxString oldStyle = g_StyleManager->GetCurrentStyle()->name;
        g_StyleManager->SetStyleNextInvocation( m_itemStyleListBox->GetStringSelection() );
        if( g_StyleManager->GetStyleNextInvocation() != oldStyle ) {
            m_returnChanges |= STYLE_CHANGED;
        }
        wxSizeEvent nullEvent;
        gFrame->OnSize( nullEvent );
    }

    //      PlugIn Manager Panel

    //      Pick up any changes to selections
    bool bnew_settings = g_pi_manager->UpdatePlugIns();
    if( bnew_settings ) m_returnChanges |= TOOLBAR_CHANGED;

    //      And keep config in sync
    g_pi_manager->UpdateConfig();

    //      PlugIns may have added panels
    if( g_pi_manager ) g_pi_manager->CloseAllPlugInPanels( (int) wxOK );

    //      Could be a lot smarter here
    m_returnChanges |= GENERIC_CHANGED;
    m_returnChanges |= k_vectorcharts;
    m_returnChanges |= k_charts;
    m_returnChanges |= m_groups_changed;
    m_returnChanges |= k_plugins;
    m_returnChanges |= k_tides;

    //  Pick up all the entries in the DataSelected control
    //  and update the global static array
    TideCurrentDataSet.Clear();
    int nEntry = tcDataSelected->GetCount();

    for( int i = 0; i < nEntry; i++ ) {
        wxString s = tcDataSelected->GetString( i );
        TideCurrentDataSet.Add( s );
    }

    if( event.GetId() == ID_APPLY ) {
        gFrame->ProcessOptionsDialog( m_returnChanges, this );
        cc1->ReloadVP();
    }

    ::wxEndBusyCursor();
}

void options::OnXidOkClick( wxCommandEvent& event )
{
    // When closing the form with Ctrl-Enter sometimes we get double events, the second is empty??
    if( event.GetEventObject() == NULL ) return;

    OnApplyClick( event );
    if( event.GetInt() == wxID_STOP ) return;

    //  Required to avoid intermittent crash on wxGTK
    m_pListbook->ChangeSelection(0);
    for (size_t i = 0; i < m_pListbook->GetPageCount(); i++)
    {
        wxNotebookPage* pg = m_pListbook->GetPage( i );

        if( pg->IsKindOf( CLASSINFO(wxNotebook))) {
                wxNotebook *nb = ((wxNotebook *)pg);
                nb->ChangeSelection(0);
        }
    }

    delete pActiveChartsList;
    delete ps57CtlListBox;
    delete tcDataSelected;

    lastWindowPos = GetPosition();
    lastWindowSize = GetSize();
    SetReturnCode( m_returnChanges );
    EndModal( m_returnChanges );
}

void options::OnButtondeleteClick( wxCommandEvent& event )
{

    wxString dirname;

    wxArrayInt pListBoxSelections;
    pActiveChartsList->GetSelections( pListBoxSelections );
    int nSelections = pListBoxSelections.GetCount();
    for( int i = 0; i < nSelections; i++ ) {
        pActiveChartsList->Delete( pListBoxSelections.Item( ( nSelections - i ) - 1 ) );
    }

    UpdateWorkArrayFromTextCtl();

    if( m_pWorkDirList ) {
        pActiveChartsList->Clear();

        int nDir = m_pWorkDirList->GetCount();
        for( int id = 0; id < nDir; id++ ) {
            dirname = m_pWorkDirList->Item( id ).fullpath;
            if( !dirname.IsEmpty() ) {
                pActiveChartsList->Append( dirname );
            }
        }
    }

    k_charts |= CHANGE_CHARTS;

    pScanCheckBox->Disable();

    event.Skip();
}

void options::OnDebugcheckbox1Click( wxCommandEvent& event )
{
    event.Skip();
}

void options::OnCancelClick( wxCommandEvent& event )
{
    //  Required to avoid intermittent crash on wxGTK
    if(m_groupsPage)
        m_groupsPage->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxListbookEventHandler( options::OnChartsPageChange ), NULL, this );
    
    m_pListbook->ChangeSelection(0);
    delete pActiveChartsList;
    delete ps57CtlListBox;

    lastWindowPos = GetPosition();
    lastWindowSize = GetSize();
    EndModal(0);
}

void options::OnClose( wxCloseEvent& event )
{
    //      PlugIns may have added panels
    if( g_pi_manager )
        g_pi_manager->CloseAllPlugInPanels( (int) wxOK );
    
    //  Required to avoid intermittent crash on wxGTK
    if(m_groupsPage)
        m_groupsPage->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxListbookEventHandler( options::OnChartsPageChange ), NULL, this );
    
    m_pListbook->ChangeSelection(0);
    delete pActiveChartsList;
    delete ps57CtlListBox;
    
    lastWindowPos = GetPosition();
    lastWindowSize = GetSize();
    
    EndModal(0);
}


void options::OnChooseFont( wxCommandEvent& event )
{
    wxString sel_text_element = m_itemFontElementListBox->GetStringSelection();

    wxFont *psfont;
    wxFontData font_data;

    wxFont *pif = FontMgr::Get().GetFont( sel_text_element );
    wxColour init_color = FontMgr::Get().GetFontColor( sel_text_element );

    wxFontData init_font_data;
    if( pif ) init_font_data.SetInitialFont( *pif );
    init_font_data.SetColour( init_color );

#ifdef __WXX11__
    X11FontPicker dg(pParent, init_font_data);
#else
    wxFontDialog dg( pParent, init_font_data );
#endif
    int retval = dg.ShowModal();
    if( wxID_CANCEL != retval ) {
        font_data = dg.GetFontData();
        wxFont font = font_data.GetChosenFont();
        psfont = new wxFont( font );
        wxColor color = font_data.GetColour();
        FontMgr::Get().SetFont( sel_text_element, psfont, color );

        pParent->UpdateAllFonts();
    }

    event.Skip();
}

#ifdef __WXGTK__
void options::OnChooseFontColor( wxCommandEvent& event )
{
    wxString sel_text_element = m_itemFontElementListBox->GetStringSelection();

    wxColourData colour_data;

    wxFont *pif = FontMgr::Get().GetFont( sel_text_element );
    wxColour init_color = FontMgr::Get().GetFontColor( sel_text_element );

    wxColourData init_colour_data;
    init_colour_data.SetColour( init_color );

    wxColourDialog dg( pParent, &init_colour_data );

    int retval = dg.ShowModal();
    if( wxID_CANCEL != retval ) {
        colour_data = dg.GetColourData();

        wxColor color = colour_data.GetColour();
        FontMgr::Get().SetFont( sel_text_element, pif, color );

        pParent->UpdateAllFonts();
    }

    event.Skip();
}
#endif

void options::OnChartsPageChange( wxListbookEvent& event )
{
    unsigned int i = event.GetSelection();

    //    User selected Chart Groups Page?
    //    If so, build the remaining UI elements
    if( 2 == i ) {                       // 2 is the index of "Charts" page
        if(!groupsPanel->m_UIcomplete)
            groupsPanel->CompletePanel();

        if(!groupsPanel->m_settingscomplete) {
            ::wxBeginBusyCursor();
            groupsPanel->CompleteInitialSettings();
            ::wxEndBusyCursor();
        }
        else if( !groupsPanel->m_treespopulated ) {
            groupsPanel->PopulateTrees();
            groupsPanel->m_treespopulated = true;
        }
    }

    event.Skip();               // Allow continued event processing
}


void options::OnPageChange( wxListbookEvent& event )
{
    unsigned int i = event.GetSelection();
    lastPage = i;

    //    User selected Chart Page?
    //    If so, build the "Charts" page variants
    if( 1 == i ) {                       // 2 is the index of "Charts" page
        k_charts = VISIT_CHARTS;
    }

    else if( m_pageUI == i ) {                       // 5 is the index of "User Interface" page
        if( !m_bVisitLang ) {
            ::wxBeginBusyCursor();

            int current_language = plocale_def_lang->GetLanguage();
            wxString current_sel = wxLocale::GetLanguageName( current_language );

            current_sel = GetOCPNKnownLanguage( g_locale, NULL );

            int nLang = sizeof( lang_list ) / sizeof(int);

#ifdef __WXMSW__
            // always add us english
            m_itemLangListBox->Append( _T("English (U.S.)") );

            wxString lang_dir = g_SData_Locn + _T("share/locale/");
            for( int it = 1; it < nLang; it++ ) {
                if( wxLocale::IsAvailable( lang_list[it] ) ) {
                    wxLocale ltest( lang_list[it], 0 );
                    ltest.AddCatalog( _T("opencpn") );
                    if( !ltest.IsLoaded( _T("opencpn") ) ) continue;

                    // Defaults
                    wxString loc_lang_name = wxLocale::GetLanguageName( lang_list[it] );
                    wxString widgets_lang_name = loc_lang_name;
                    wxString lang_canonical =
                            wxLocale::GetLanguageInfo( lang_list[it] )->CanonicalName;

                    //  Make opencpn substitutions
                    wxString lang_suffix;
                    loc_lang_name = GetOCPNKnownLanguage( lang_canonical, &lang_suffix );

                    //  Look explicitely to see if .mo is available
                    wxString test_dir = lang_dir + lang_suffix;
                    if( !wxDir::Exists( test_dir ) ) continue;

                    m_itemLangListBox->Append( loc_lang_name );
                }
            }
#else
            wxArrayString lang_array;

            // always add us english
            lang_array.Add(_T("en_US"));

            for( int it = 0; it < nLang; it++)
            {
                {
                    wxLog::EnableLogging(false);  // avoid "Cannot set locale to..." log message

                    wxLocale ltest(lang_list[it], 0);
                    ltest.AddCatalog(_T("opencpn"));

                    wxLog::EnableLogging(true);

                    if(ltest.IsLoaded(_T("opencpn")))
                    {
                        wxString s0 = wxLocale::GetLanguageInfo(lang_list[it])->CanonicalName;
                        wxString sl = wxLocale::GetLanguageName(lang_list[it]);
                        if(wxNOT_FOUND == lang_array.Index(s0))
                        lang_array.Add(s0);

                    }
                }
            }

            for(unsigned int i=0; i < lang_array.GetCount(); i++)
            {
                //  Make opencpn substitutions
                wxString loc_lang_name = GetOCPNKnownLanguage(lang_array[i], NULL);
                m_itemLangListBox->Append( loc_lang_name );
            }
#endif

            // BUGBUG
            //  Remember that wxLocale ctor has the effect of changing the system locale, including the "C" libraries.
            //  It should then also happen that the locale should be switched back to ocpn initial load setting
            //  upon the dtor of the above wxLocale instantiations....
            //  wxWidgets may do so internally, but there seems to be no effect upon the system libraries, so that
            //  functions like strftime() do not revert to the correct locale setting.
            //  Also, the catalog for the application is not reloaded by the ctor, so we must reload them directly
            //  So as workaround, we reset the locale explicitely.

            delete plocale_def_lang;
            plocale_def_lang = new wxLocale( current_language );

            setlocale( LC_NUMERIC, "C" );
            plocale_def_lang->AddCatalog( _T("opencpn") );

            m_itemLangListBox->SetStringSelection( current_sel );

            //      Initialize Language tab
            const wxLanguageInfo *pli = wxLocale::FindLanguageInfo( g_locale );
            if( pli ) {
                wxString clang = pli->Description;
//                        m_itemLangListBox->SetValue(clang);
            }

            m_bVisitLang = true;

            ::wxEndBusyCursor();
        }
    }

    else if( m_pagePlugins == i ) {                    // 7 is the index of "Plugins" page

        if( !m_pPlugInCtrl){
            //      Build the PlugIn Manager Panel
            ::wxBeginBusyCursor();
            
            m_pPlugInCtrl = new PluginListPanel( itemPanelPlugins, ID_PANELPIM, wxDefaultPosition,
                                                wxDefaultSize, g_pi_manager->GetPlugInArray() );
            m_pPlugInCtrl->SetScrollRate( 15, 15 );
            
            itemBoxSizerPanelPlugins->Add( m_pPlugInCtrl, 1, wxEXPAND|wxALL, 4 );
            
            itemBoxSizerPanelPlugins->Layout();
            
            //  Update the PlugIn page to reflect the state of individual selections
            m_pPlugInCtrl->UpdateSelections();
            
            ::wxEndBusyCursor();
        }

        k_plugins = TOOLBAR_CHANGED;
    }
}

//void options::OnNMEASourceChoice( wxCommandEvent& event )
//{
/*TODO
    int i = event.GetSelection();
    wxString src( m_itemNMEAListBox->GetString( i ) );
    if( ( src.Upper().Find( _T("GPSD") ) != wxNOT_FOUND )
            || ( src.Upper().Find( _T("LIBGPS") ) != wxNOT_FOUND ) ) {
        m_itemNMEA_TCPIP_StaticBox->Enable();
        m_itemNMEA_TCPIP_Source->Enable();

        m_itemNMEA_TCPIP_Source->Clear();
        m_itemNMEA_TCPIP_Source->WriteText( _T("localhost") ); // default

        wxString source;
        source = *pNMEADataSource;
        if( source.Upper().StartsWith( _T("GPSD") ) || source.Upper().StartsWith( _T("LIBGPS") ) ) {
            wxString ip;
            ip = source.AfterFirst( ':' );

            if( ip.Len() ) {
                m_itemNMEA_TCPIP_Source->Clear();
                m_itemNMEA_TCPIP_Source->WriteText( ip );
            }
        }
    } else {
        m_itemNMEA_TCPIP_StaticBox->Disable();
        m_itemNMEA_TCPIP_Source->Disable();
    }
*/
//}

void options::OnButtonSelectSound( wxCommandEvent& event )
{
    wxString sound_dir = g_SData_Locn;
    sound_dir.Append( _T("sounds") );

    wxFileDialog *openDialog = new wxFileDialog( NULL, _("Select Sound File"), sound_dir, wxT(""),
            _("WAV files (*.wav)|*.wav|All files (*.*)|*.*"), wxFD_OPEN );
    int response = openDialog->ShowModal();
    if( response == wxID_OK ) {
        if( g_bportable ) {
            wxFileName f( openDialog->GetPath() );
            f.MakeRelativeTo( *pHome_Locn );
            g_sAIS_Alert_Sound_File = f.GetFullPath();
        } else
            g_sAIS_Alert_Sound_File = openDialog->GetPath();

        g_anchorwatch_sound.UnLoad();
    }
}

void options::OnButtonTestSound( wxCommandEvent& event )
{

    OCPN_Sound AIS_Sound;
    AIS_Sound.Create( g_sAIS_Alert_Sound_File );

    if( AIS_Sound.IsOk() ) {
        
#ifndef __WXMSW__
        AIS_Sound.Play();
        int t = 0;
        while( AIS_Sound.IsPlaying() && (t < 10) ) {
            wxSleep(1);
            t++;
        }
        if( AIS_Sound.IsPlaying() )
            AIS_Sound.Stop();
 
#else
        AIS_Sound.Play(wxSOUND_SYNC);
#endif
    }

}

wxString GetOCPNKnownLanguage( wxString lang_canonical, wxString *lang_dir )
{
    wxString return_string;
    wxString dir_suffix;

    if( lang_canonical == _T("en_US") ) {
        dir_suffix = _T("en");
        return_string = wxString( "English (U.S.)", wxConvUTF8 );
    } else if( lang_canonical == _T("cs_CZ") ) {
        dir_suffix = _T("cs");
        return_string = wxString( "Čeština", wxConvUTF8 );
    } else if( lang_canonical == _T("da_DK") ) {
        dir_suffix = _T("da");
        return_string = wxString( "Dansk", wxConvUTF8 );
    } else if( lang_canonical == _T("de_DE") ) {
        dir_suffix = _T("de");
        return_string = wxString( "Deutsch", wxConvUTF8 );
    } else if( lang_canonical == _T("et_EE") ) {
        dir_suffix = _T("et");
        return_string = wxString( "Eesti", wxConvUTF8 );
    } else if( lang_canonical == _T("es_ES") ) {
        dir_suffix = _T("es");
        return_string = wxString( "Español", wxConvUTF8 );
    } else if( lang_canonical == _T("fr_FR") ) {
        dir_suffix = _T("fr");
        return_string = wxString( "Français", wxConvUTF8 );
    } else if( lang_canonical == _T("it_IT") ) {
        dir_suffix = _T("it");
        return_string = wxString( "Italiano", wxConvUTF8 );
    } else if( lang_canonical == _T("nl_NL") ) {
        dir_suffix = _T("nl");
        return_string = wxString( "Nederlands", wxConvUTF8 );
    } else if( lang_canonical == _T("pl_PL") ) {
        dir_suffix = _T("pl");
        return_string = wxString( "Polski", wxConvUTF8 );
    } else if( lang_canonical == _T("pt_PT") ) {
        dir_suffix = _T("pt_PT");
        return_string = wxString( "Português", wxConvUTF8 );
    } else if( lang_canonical == _T("pt_BR") ) {
        dir_suffix = _T("pt_BR");
        return_string = wxString( "Português Brasileiro", wxConvUTF8 );
    } else if( lang_canonical == _T("ru_RU") ) {
        dir_suffix = _T("ru");
        return_string = wxString( "Русский", wxConvUTF8 );
    } else if( lang_canonical == _T("sv_SE") ) {
        dir_suffix = _T("sv");
        return_string = wxString( "Svenska", wxConvUTF8 );
    } else if( lang_canonical == _T("fi_FI") ) {
        dir_suffix = _T("fi_FI");
        return_string = wxString( "Suomi", wxConvUTF8 );
    } else if( lang_canonical == _T("nb_NO") ) {
        dir_suffix = _T("nb_NO");
        return_string = wxString( "Norsk", wxConvUTF8 );
    } else if( lang_canonical == _T("tr_TR") ) {
        dir_suffix = _T("tr_TR");
        return_string = wxString( "Türkçe", wxConvUTF8 );
    } else if( lang_canonical == _T("el_GR") ) {
        dir_suffix = _T("el_GR");
        return_string = wxString( "Ελληνικά", wxConvUTF8 );
    } else if( lang_canonical == _T("hu_HU") ) {
        dir_suffix = _T("hu_HU");
        return_string = wxString( "Magyar", wxConvUTF8 );
    } else if( lang_canonical == _T("zh_TW") ) {
        dir_suffix = _T("zh_TW");
        return_string = wxString( "正體字", wxConvUTF8 );
    } else if( lang_canonical == _T("ca_ES") ) {
        dir_suffix = _T("ca_ES");
        return_string = wxString( "Catalan", wxConvUTF8 );
    } else if( lang_canonical == _T("gl_ES") ) {
        dir_suffix = _T("gl_ES");
        return_string = wxString( "Galician", wxConvUTF8 );
    } else {
        dir_suffix = lang_canonical;
        const wxLanguageInfo *info = wxLocale::FindLanguageInfo( lang_canonical );
        return_string = info->Description;
    }

    if( NULL != lang_dir ) *lang_dir = dir_suffix;

    return return_string;

}

ChartGroupArray* ChartGroupsUI::CloneChartGroupArray( ChartGroupArray* s )
{
    ChartGroupArray *d = new ChartGroupArray;
    for( unsigned int i = 0; i < s->GetCount(); i++ ) {
        ChartGroup *psg = s->Item( i );
        ChartGroup *pdg = new ChartGroup;
        pdg->m_group_name = psg->m_group_name;

        for( unsigned int j = 0; j < psg->m_element_array.GetCount(); j++ ) {
            ChartGroupElement *pde = new ChartGroupElement;
            pde->m_element_name = psg->m_element_array.Item( j )->m_element_name;
            for( unsigned int k = 0;
                    k < psg->m_element_array.Item( j )->m_missing_name_array.GetCount(); k++ ) {
                wxString missing_name = psg->m_element_array.Item( j )->m_missing_name_array.Item(k);
                pde->m_missing_name_array.Add( missing_name );
            }
            pdg->m_element_array.Add( pde );
        }
        d->Add( pdg );
    }
    return d;
}

void ChartGroupsUI::EmptyChartGroupArray( ChartGroupArray* s )
{
    if( !s ) return;
    for( unsigned int i = 0; i < s->GetCount(); i++ ) {
        ChartGroup *psg = s->Item( i );

        for( unsigned int j = 0; j < psg->m_element_array.GetCount(); j++ ) {
            ChartGroupElement *pe = psg->m_element_array.Item( j );
            pe->m_missing_name_array.Clear();
            psg->m_element_array.RemoveAt( j );
            delete pe;

        }
        s->RemoveAt( i );
        delete psg;
    }

    s->Clear();
}

//    Chart Groups dialog implementation

BEGIN_EVENT_TABLE( ChartGroupsUI, wxScrolledWindow )
    EVT_TREE_ITEM_EXPANDED( wxID_TREECTRL, ChartGroupsUI::OnNodeExpanded )
    EVT_BUTTON( ID_GROUPINSERTDIR, ChartGroupsUI::OnInsertChartItem )
    EVT_BUTTON( ID_GROUPREMOVEDIR, ChartGroupsUI::OnRemoveChartItem )
    EVT_NOTEBOOK_PAGE_CHANGED(ID_GROUPNOTEBOOK, ChartGroupsUI::OnGroupPageChange)
    EVT_BUTTON( ID_GROUPNEWGROUP, ChartGroupsUI::OnNewGroup )
    EVT_BUTTON( ID_GROUPDELETEGROUP, ChartGroupsUI::OnDeleteGroup )
END_EVENT_TABLE()

ChartGroupsUI::ChartGroupsUI( wxWindow* parent )
{
    Create( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL, _("Chart Groups") );

    m_GroupSelectedPage = -1;
    m_pActiveChartsTree = 0;
    pParent = parent;
    lastSelectedCtl = NULL;
    allAvailableCtl = NULL;
    defaultAllCtl = NULL;
    iFont = NULL;
    m_pAddButton = NULL;
    m_pRemoveButton = NULL;
    m_pDeleteGroupButton = NULL;
    m_pNewGroupButton = NULL;
    m_pGroupArray= NULL;
    m_GroupNB = NULL;
    modified = false;
    m_UIcomplete = false;
    m_treespopulated = false;
}

ChartGroupsUI::~ChartGroupsUI()
{
    m_DirCtrlArray.Clear();
    delete iFont;
}

void ChartGroupsUI::SetInitialSettings()
{
    m_settingscomplete = false;
    m_treespopulated = false;
}

void ChartGroupsUI::PopulateTrees()
{
    //    Fill in the "Active chart" tree control
    //    from the options dialog "Active Chart Directories" list
    wxArrayString dir_array;
    int nDir = m_db_dirs.GetCount();
    for( int i = 0; i < nDir; i++ ) {
        wxString dirname = m_db_dirs.Item( i ).fullpath;
        if( !dirname.IsEmpty() ) dir_array.Add( dirname );
    }

    PopulateTreeCtrl( allAvailableCtl->GetTreeCtrl(), dir_array, wxColour( 0, 0, 0 ) );
    m_pActiveChartsTree = allAvailableCtl->GetTreeCtrl();

    //    Fill in the Page 0 tree control
    //    from the options dialog "Active Chart Directories" list
    wxArrayString dir_array0;
    int nDir0 = m_db_dirs.GetCount();
    for( int i = 0; i < nDir0; i++ ) {
        wxString dirname = m_db_dirs.Item( i ).fullpath;
        if( !dirname.IsEmpty() ) dir_array0.Add( dirname );
    }
    PopulateTreeCtrl( defaultAllCtl->GetTreeCtrl(), dir_array0, wxColour( 128, 128, 128 ),
                      iFont );
}



void ChartGroupsUI::CompleteInitialSettings()
{
    PopulateTrees();

    BuildNotebookPages( m_pGroupArray );

    groupsSizer->Layout();

    m_settingscomplete = true;
    m_treespopulated = true;
}



void ChartGroupsUI::PopulateTreeCtrl( wxTreeCtrl *ptc, const wxArrayString &dir_array,
        const wxColour &col, wxFont *pFont )
{
    ptc->DeleteAllItems();

    wxDirItemData* rootData = new wxDirItemData( _T("Dummy"), _T("Dummy"), true );
    wxString rootName;
    rootName = _T("Dummy");
    wxTreeItemId m_rootId = ptc->AddRoot( rootName, 3, -1, rootData );
    ptc->SetItemHasChildren( m_rootId );

    wxString dirname;
    int nDir = dir_array.GetCount();
    for( int i = 0; i < nDir; i++ ) {
        wxString dirname = dir_array.Item( i );
        if( !dirname.IsEmpty() ) {
            wxDirItemData *dir_item = new wxDirItemData( dirname, dirname, true );
            wxTreeItemId id = ptc->AppendItem( m_rootId, dirname, 0, -1, dir_item );

            //wxWidgets bug workaraound (Ticket #10085)
            ptc->SetItemText(id, dirname);
            if( pFont ) ptc->SetItemFont( id, *pFont );
            ptc->SetItemTextColour( id, col );
            ptc->SetItemHasChildren( id );
        }
    }
}

void ChartGroupsUI::OnInsertChartItem( wxCommandEvent &event )
{
    wxString insert_candidate = allAvailableCtl->GetPath();
    if( !insert_candidate.IsEmpty() ) {
        if( m_DirCtrlArray.GetCount() ) {
            wxGenericDirCtrl *pDirCtrl = ( m_DirCtrlArray.Item( m_GroupSelectedPage ) );
            ChartGroup *pGroup = m_pGroupArray->Item( m_GroupSelectedPage - 1 );
            if( pDirCtrl ) {
                wxTreeCtrl *ptree = pDirCtrl->GetTreeCtrl();
                if( ptree ) {
                    if( ptree->IsEmpty() ) {
                        wxDirItemData* rootData = new wxDirItemData( wxEmptyString, wxEmptyString,
                                true );
                        wxString rootName = _T("Dummy");
                        wxTreeItemId rootId = ptree->AddRoot( rootName, 3, -1, rootData );

                        ptree->SetItemHasChildren( rootId );
                    }

                    wxTreeItemId root_Id = ptree->GetRootItem();
                    wxDirItemData *dir_item = new wxDirItemData( insert_candidate, insert_candidate,
                            true );
                    wxTreeItemId id = ptree->AppendItem( root_Id, insert_candidate, 0, -1,
                            dir_item );
                    if( wxDir::Exists( insert_candidate ) ) ptree->SetItemHasChildren( id );
                }

                ChartGroupElement *pnew_element = new ChartGroupElement;
                pnew_element->m_element_name = insert_candidate;
                pGroup->m_element_array.Add( pnew_element );
            }
        }
    }
    modified = true;
    allAvailableCtl->GetTreeCtrl()->UnselectAll();
    m_pAddButton->Disable();
}

void ChartGroupsUI::OnRemoveChartItem( wxCommandEvent &event )
{
    if( m_DirCtrlArray.GetCount() ) {
        wxGenericDirCtrl *pDirCtrl = ( m_DirCtrlArray.Item( m_GroupSelectedPage ) );
        ChartGroup *pGroup = m_pGroupArray->Item( m_GroupSelectedPage - 1 );

        if( pDirCtrl ) {
            wxString sel_item = pDirCtrl->GetPath();

            wxTreeCtrl *ptree = pDirCtrl->GetTreeCtrl();
            if( ptree && ptree->GetCount() ) {
                wxTreeItemId id = ptree->GetSelection();
                lastDeletedItem = id;
                if( id.IsOk() ) {
                    wxString branch_adder;
                    int group_item_index = FindGroupBranch( pGroup, ptree, id, &branch_adder );
                    if( group_item_index >= 0 ) {
                        ChartGroupElement *pelement = pGroup->m_element_array.Item(
                                group_item_index );
                        bool b_duplicate = false;
                        for( unsigned int k = 0; k < pelement->m_missing_name_array.GetCount();
                                k++ ) {
                            if( pelement->m_missing_name_array.Item( k ) == sel_item ) {
                                b_duplicate = true;
                                break;
                            }
                        }
                        if( !b_duplicate ) {
                            pelement->m_missing_name_array.Add( sel_item );
                        }

                        //    Special case...
                        //    If the selection is a branch itself,
                        //    Then delete from the tree, and delete from the group
                        if( branch_adder == _T("") ) {
                            ptree->Delete( id );
                            pGroup->m_element_array.RemoveAt( group_item_index );
                        } else {
                            ptree->SetItemTextColour( id, wxColour( 128, 128, 128 ) );
                            //   what about toggle back?
                        }
                    }
                }
                modified = true;
                lastSelectedCtl->Unselect();
                m_pRemoveButton->Disable();
                wxLogMessage( _T("Disable"));
            }
        }
    }
    event.Skip();
}

void ChartGroupsUI::OnGroupPageChange( wxNotebookEvent& event )
{
    m_GroupSelectedPage = event.GetSelection();
    allAvailableCtl->GetTreeCtrl()->UnselectAll();
    if( lastSelectedCtl ) lastSelectedCtl->UnselectAll();
    m_pRemoveButton->Disable();
    m_pAddButton->Disable();
}

void ChartGroupsUI::OnAvailableSelection( wxTreeEvent& event ) {
    if( allAvailableCtl && (event.GetEventObject() == allAvailableCtl->GetTreeCtrl()) ){
        wxTreeItemId item = allAvailableCtl->GetTreeCtrl()->GetSelection();
        if( item && item.IsOk() && m_GroupSelectedPage>0 ) {
            m_pAddButton->Enable();
        } else {
            m_pAddButton->Disable();
        }
    } else {
        lastSelectedCtl = (wxTreeCtrl*) event.GetEventObject();
        wxTreeItemId item = lastSelectedCtl->GetSelection();
        if( item && item.IsOk() && m_GroupSelectedPage>0 ) {
            // We need a trick for wxGTK here, since it gives us a Selection event with
            // the just deleted empty element after OnRemoveChartItem()
            wxString itemPath = ((wxGenericDirCtrl*) lastSelectedCtl->GetParent())->GetPath();
            if( itemPath.Length() ) m_pRemoveButton->Enable();
        } else {
            m_pRemoveButton->Disable();
        }
    }
    event.Skip();
}

void ChartGroupsUI::OnNewGroup( wxCommandEvent &event )
{
    wxTextEntryDialog *pd = new wxTextEntryDialog( this, _("Enter Group Name"),
            _("New Chart Group") );

    if( pd->ShowModal() == wxID_OK ) {
        AddEmptyGroupPage( pd->GetValue() );
        ChartGroup *pGroup = new ChartGroup;
        pGroup->m_group_name = pd->GetValue();
        m_pGroupArray->Add( pGroup );

        m_GroupSelectedPage = m_GroupNB->GetPageCount() - 1;      // select the new page
        m_GroupNB->ChangeSelection( m_GroupSelectedPage );
        modified = true;
    }
    delete pd;
}

void ChartGroupsUI::OnDeleteGroup( wxCommandEvent &event )
{
    if( 0 != m_GroupSelectedPage ) {
        m_DirCtrlArray.RemoveAt( m_GroupSelectedPage );
        if( m_pGroupArray ) m_pGroupArray->RemoveAt( m_GroupSelectedPage - 1 );
        m_GroupNB->DeletePage( m_GroupSelectedPage );
        modified = true;
    }
}

WX_DEFINE_OBJARRAY( ChartGroupElementArray );
WX_DEFINE_OBJARRAY( ChartGroupArray );

int ChartGroupsUI::FindGroupBranch( ChartGroup *pGroup, wxTreeCtrl *ptree, wxTreeItemId item,
        wxString *pbranch_adder )
{
    wxString branch_name;
    wxString branch_adder;

    wxTreeItemId current_node = item;
    while( current_node.IsOk() ) {

        wxTreeItemId parent_node = ptree->GetItemParent( current_node );
        if( !parent_node ) break;

        if( parent_node == ptree->GetRootItem() ) {
            branch_name = ptree->GetItemText( current_node );
            break;
        }

        branch_adder.Prepend( ptree->GetItemText( current_node ) );
        branch_adder.Prepend( wxString( wxFILE_SEP_PATH ) );

        current_node = ptree->GetItemParent( current_node );
    }

    //    Find the index and element pointer of the target branch in the Group
    ChartGroupElement *target_element = NULL;
    unsigned int target_item_index = -1;

    for( unsigned int i = 0; i < pGroup->m_element_array.GetCount(); i++ ) {
        wxString target = pGroup->m_element_array.Item( i )->m_element_name;
        if( branch_name == target ) {
            target_element = pGroup->m_element_array.Item( i );
            target_item_index = i;
            break;
        }
    }

    if( pbranch_adder ) *pbranch_adder = branch_adder;

    return target_item_index;
}

void ChartGroupsUI::OnNodeExpanded( wxTreeEvent& event )
{
    wxTreeItemId node = event.GetItem();

    if( m_GroupSelectedPage > 0 ) {
        wxGenericDirCtrl *pDirCtrl = ( m_DirCtrlArray.Item( m_GroupSelectedPage ) );
        ChartGroup *pGroup = m_pGroupArray->Item( m_GroupSelectedPage - 1 );
        if( pDirCtrl ) {
            wxTreeCtrl *ptree = pDirCtrl->GetTreeCtrl();

            wxString branch_adder;
            int target_item_index = FindGroupBranch( pGroup, ptree, node, &branch_adder );
            if( target_item_index >= 0 ) {
                ChartGroupElement *target_element = pGroup->m_element_array.Item(
                        target_item_index );
                wxString branch_name = target_element->m_element_name;

                //    Walk the children of the expanded node, marking any items which appear in
                //    the "missing" list
                if( ( target_element->m_missing_name_array.GetCount() ) ) {
                    wxString full_root = branch_name;
                    full_root += branch_adder;
                    full_root += wxString( wxFILE_SEP_PATH );

                    wxTreeItemIdValue cookie;
                    wxTreeItemId child = ptree->GetFirstChild( node, cookie );
                    while( child.IsOk() ) {
                        wxString target_string = full_root;
                        target_string += ptree->GetItemText( child );

                        for( unsigned int k = 0;
                                k < target_element->m_missing_name_array.GetCount(); k++ ) {
                            if( target_element->m_missing_name_array.Item( k ) == target_string ) {
                                ptree->SetItemTextColour( child, wxColour( 128, 128, 128 ) );
                                break;
                            }
                        }
                        child = ptree->GetNextChild( node, cookie );
                    }
                }
            }
        }
    }
}

void ChartGroupsUI::BuildNotebookPages( ChartGroupArray *pGroupArray )
{
    for( unsigned int i = 0; i < pGroupArray->GetCount(); i++ ) {
        ChartGroup *pGroup = pGroupArray->Item( i );
        wxTreeCtrl *ptc = AddEmptyGroupPage( pGroup->m_group_name );

        wxString itemname;
        int nItems = pGroup->m_element_array.GetCount();
        for( int i = 0; i < nItems; i++ ) {
            wxString itemname = pGroup->m_element_array.Item( i )->m_element_name;
            if( !itemname.IsEmpty() ) {
                wxDirItemData *dir_item = new wxDirItemData( itemname, itemname, true );
                wxTreeItemId id = ptc->AppendItem( ptc->GetRootItem(), itemname, 0, -1, dir_item );

                if( wxDir::Exists( itemname ) ) ptc->SetItemHasChildren( id );
            }
        }
    }
}

wxTreeCtrl *ChartGroupsUI::AddEmptyGroupPage( const wxString& label )
{
    wxGenericDirCtrl *GroupDirCtl = new wxGenericDirCtrl( m_GroupNB, wxID_ANY, _T("TESTDIR") );
    m_GroupNB->AddPage( GroupDirCtl, label );

    wxTreeCtrl *ptree = GroupDirCtl->GetTreeCtrl();
    ptree->DeleteAllItems();

    wxDirItemData* rootData = new wxDirItemData( wxEmptyString, wxEmptyString, true );
    wxString rootName = _T("Dummy");
    wxTreeItemId rootId = ptree->AddRoot( rootName, 3, -1, rootData );
    ptree->SetItemHasChildren( rootId );

    m_DirCtrlArray.Add( GroupDirCtl );

    return ptree;
}

void options::OnInsertTideDataLocation( wxCommandEvent &event )
{
    wxString sel_file;
    int response = wxID_CANCEL;

    wxFileDialog openDialog( NULL, _( "Select Tide/Current Data" ), g_TCData_Dir, wxT ( "" ),
                             wxT ( "Tide/Current Data files (*.IDX; *.TCD)|*.IDX;*.idx;*.TCD;*.tcd|All files (*.*)|*.*" ),
                                    wxFD_OPEN  );
    response = openDialog.ShowModal();
    if( response == wxID_OK ) {
        sel_file = openDialog.GetPath();

        if( g_bportable ) {
            wxFileName f( sel_file );
            f.MakeRelativeTo( *pHome_Locn );
            tcDataSelected->Append( f.GetFullPath() );
        } else
            tcDataSelected->Append( sel_file );

        //    Record the currently selected directory for later use
        wxFileName fn( sel_file );
        wxString data_dir = fn.GetPath();
        if( g_bportable ) {
            wxFileName f( data_dir );
            f.MakeRelativeTo( *pHome_Locn );
            g_TCData_Dir = f.GetFullPath();
        }
        else
            g_TCData_Dir = data_dir;
    }
}

void options::OnRemoveTideDataLocation( wxCommandEvent &event )
{
    wxArrayInt sels;
    int nSel = tcDataSelected->GetSelections(sels);
    wxArrayString a;
    for( int i=0 ; i < nSel ; i++) {
        a.Add( tcDataSelected->GetString(sels.Item( i )));
    }

    for (unsigned int i=0 ; i < a.Count() ; i++) {
        int b = tcDataSelected->FindString(a.Item(i));
        tcDataSelected->Delete( b );
    }
}

void options::OnValChange( wxCommandEvent& event )
{
    event.Skip();
}

void options::OnConnValChange( wxCommandEvent& event )
{
    connectionsaved = false;
    event.Skip();
}

void options::OnTypeSerialSelected( wxCommandEvent& event )
{
#ifdef __WXGTK__
    if( ! g_bserial_access_checked ){
        if( !CheckSerialAccess() ){
        }
        g_bserial_access_checked = true;
    }
#endif    
    
    OnConnValChange(event);
    SetNMEAFormToSerial();
}

void options::OnTypeNetSelected( wxCommandEvent& event )
{
    OnConnValChange(event);
    SetNMEAFormToNet();
}

void options::OnUploadFormatChange( wxCommandEvent& event )
{
    if ( event.GetEventObject() == m_cbGarminUploadHost && event.IsChecked() )
        m_cbFurunoGP3X->SetValue(false);
    else if ( event.GetEventObject() == m_cbFurunoGP3X && event.IsChecked() )
        m_cbGarminUploadHost->SetValue(false);
    event.Skip();
}

void options::ShowNMEACommon(bool visible)
{
    if ( visible )
    {
        m_rbTypeSerial->Show();
        m_rbTypeNet->Show();
        m_rbIAccept->Show();
        m_rbIIgnore->Show();
        m_rbOAccept->Show();
        m_rbOIgnore->Show();
        m_tcInputStc->Show();
        m_btnInputStcList->Show();
        m_tcOutputStc->Show();
        m_btnOutputStcList->Show();
        m_cbInput->Show();
        m_cbOutput->Show();
        m_stPrecision->Show();
        m_choicePrecision->Show();
        if (m_cbOutput->IsChecked()) {
            m_stPrecision->Enable(true);
            m_choicePrecision->Enable(true);
        } else {
            m_stPrecision->Enable(false);
            m_choicePrecision->Enable(false);
        }
        m_choicePriority->Show();
        m_stPriority->Show();
        m_cbCheckCRC->Show();
    }
    else
    {
        m_rbTypeSerial->Hide();
        m_rbTypeNet->Hide();
        m_rbIAccept->Hide();
        m_rbIIgnore->Hide();
        m_rbOAccept->Hide();
        m_rbOIgnore->Hide();
        m_tcInputStc->Hide();
        m_btnInputStcList->Hide();
        m_tcOutputStc->Hide();
        m_btnOutputStcList->Hide();
        m_cbInput->Hide();
        m_cbOutput->Hide();
        m_choicePriority->Hide();
        m_stPrecision->Hide();
        m_choicePrecision->Hide();
        m_stPriority->Hide();
        m_cbCheckCRC->Hide();
        sbSizerOutFilter->SetDimension(0,0,0,0);
        sbSizerInFilter->SetDimension(0,0,0,0);
        sbSizerConnectionProps->SetDimension(0,0,0,0);
    }

    m_bNMEAParams_shown = visible;
}

void options::ShowNMEANet(bool visible)
{
    if ( visible )
    {
        m_stNetAddr->Show();
        m_tNetAddress->Show();
        m_stNetPort->Show();
        m_tNetPort->Show();
        m_stNetProto->Show();
        m_rbNetProtoGPSD->Show();
        m_rbNetProtoTCP->Show();
        m_rbNetProtoUDP->Show();
    }
    else
    {
        m_stNetAddr->Hide();
        m_tNetAddress->Hide();
        m_stNetPort->Hide();
        m_tNetPort->Hide();
        m_stNetProto->Hide();
        m_rbNetProtoGPSD->Hide();
        m_rbNetProtoTCP->Hide();
        m_rbNetProtoUDP->Hide();
    }
}

void options::ShowNMEASerial(bool visible)
{
    if ( visible )
    {
        m_stSerBaudrate->Show();
        m_choiceBaudRate->Show();
        m_stSerPort->Show();
        m_comboPort->Show();
        m_stSerProtocol->Show();
        m_choiceSerialProtocol->Show();
        m_cbGarminHost->Show();
        gSizerNetProps->SetDimension(0,0,0,0);
    }
    else
    {
        m_stSerBaudrate->Hide();
        m_choiceBaudRate->Hide();
        m_stSerPort->Hide();
        m_comboPort->Hide();
        m_stSerProtocol->Hide();
        m_choiceSerialProtocol->Hide();
        m_cbGarminHost->Hide();
        gSizerSerProps->SetDimension(0,0,0,0);
    }
}

void options::SetNMEAFormToSerial()
{
    ShowNMEACommon( true );
    ShowNMEANet( false );
    ShowNMEASerial( true );
    m_pNMEAForm->FitInside();
    m_pNMEAForm->Layout();
    Fit();
    Layout();
    SetDSFormRWStates();
}

void options::SetNMEAFormToNet()
{
    ShowNMEACommon( true );
    ShowNMEANet( true );
    ShowNMEASerial( false );
    m_pNMEAForm->FitInside();
    m_pNMEAForm->Layout();
    Fit();
    Layout();
    SetDSFormRWStates();
}

void options::ClearNMEAForm()
{
    ShowNMEACommon( false );
    ShowNMEANet( false );
    ShowNMEASerial( false );
    m_pNMEAForm->FitInside();
    m_pNMEAForm->Layout();
    Fit();
    Layout();
}

wxString StringArrayToString(wxArrayString arr)
{
    wxString ret = wxEmptyString;
    for (size_t i = 0; i < arr.Count(); i++)
    {
        if (i > 0)
            ret.Append(_T(","));
        ret.Append(arr[i]);
    }
    return ret;
}

void options::SetDSFormRWStates()
{
    if (m_rbTypeSerial->GetValue())
    {
        m_cbInput->Enable(false);
        m_cbOutput->Enable(true);
        m_rbOAccept->Enable(true);
        m_rbOIgnore->Enable(true);
        m_btnOutputStcList->Enable(true);
    }
    else if (m_rbNetProtoGPSD->GetValue())
    {
        if (m_tNetPort->GetValue() == wxEmptyString)
            m_tNetPort->SetValue(_T("2947"));
        m_cbInput->SetValue(true);
        m_cbInput->Enable(false);
        m_cbOutput->SetValue(false);
        m_cbOutput->Enable(false);
        m_rbOAccept->Enable(false);
        m_rbOIgnore->Enable(false);
        m_btnOutputStcList->Enable(false);
    }
    else
    {
        if (m_tNetPort->GetValue() == wxEmptyString)
            m_tNetPort->SetValue(_T("10110"));
        m_cbInput->Enable(true);
        m_cbOutput->Enable(true);
        m_rbOAccept->Enable(true);
        m_rbOIgnore->Enable(true);
        m_btnOutputStcList->Enable(true);
    }
}

void options::SetConnectionParams(ConnectionParams *cp)
{
    m_comboPort->Select(m_comboPort->FindString(cp->Port));
    m_comboPort->SetValue(cp->Port);
    m_cbCheckCRC->SetValue(cp->ChecksumCheck);
    m_cbGarminHost->SetValue(cp->Garmin);
    m_cbInput->SetValue(!(cp->IOSelect == DS_TYPE_OUTPUT));
    m_cbOutput->SetValue(!(cp->IOSelect == DS_TYPE_INPUT));
    if(cp->InputSentenceListType == WHITELIST)
        m_rbIAccept->SetValue(true);
    else
        m_rbIIgnore->SetValue(true);
    if(cp->OutputSentenceListType == WHITELIST)
        m_rbOAccept->SetValue(true);
    else
        m_rbOIgnore->SetValue(true);
    m_tcInputStc->SetValue(StringArrayToString(cp->InputSentenceList));
    m_tcOutputStc->SetValue(StringArrayToString(cp->OutputSentenceList));
    m_choiceBaudRate->Select(m_choiceBaudRate->FindString(wxString::Format(_T("%d"),cp->Baudrate)));
    m_choiceSerialProtocol->Select(cp->Protocol); //TODO
    m_choicePriority->Select(m_choicePriority->FindString(wxString::Format(_T("%d"),cp->Priority)));

    m_tNetAddress->SetValue(cp->NetworkAddress);

    if( cp->NetworkPort == 0)
        m_tNetPort->SetValue(_T(""));
    else
        m_tNetPort->SetValue(wxString::Format(wxT("%i"), cp->NetworkPort));

    if(cp->NetProtocol == TCP)
        m_rbNetProtoTCP->SetValue(true);
    else if (cp->NetProtocol == UDP)
        m_rbNetProtoUDP->SetValue(true);
    else
        m_rbNetProtoGPSD->SetValue(true);

    if ( cp->Type == SERIAL )
    {
        m_rbTypeSerial->SetValue( true );
        SetNMEAFormToSerial();
    }
    else
    {
        m_rbTypeNet->SetValue( true );
        SetNMEAFormToNet();
    }

    m_connection_enabled = cp->bEnabled;
}

void options::SetDefaultConnectionParams()
{
    m_comboPort->Select(0);
    m_comboPort->SetValue(_T(""));
    m_cbCheckCRC->SetValue(true);
    m_cbGarminHost->SetValue(false);
    m_cbInput->SetValue(true);
    m_cbOutput->SetValue(false);
    m_rbIAccept->SetValue(true);
    m_rbOAccept->SetValue(true);
    m_tcInputStc->SetValue(_T(""));
    m_tcOutputStc->SetValue(_T(""));
    m_choiceBaudRate->Select(m_choiceBaudRate->FindString(_T("4800")));
//    m_choiceSerialProtocol->Select(cp->Protocol); //TODO
    m_choicePriority->Select(m_choicePriority->FindString(_T("1")));

    bool bserial = true;
#ifdef __WXGTK__
    if(!g_bserial_access_checked)
        bserial = false;
#endif
    
    m_rbTypeSerial->SetValue( bserial );
    m_rbTypeNet->SetValue( !bserial );
    
    if(bserial)
        SetNMEAFormToSerial();
    else
        SetNMEAFormToNet();
    
    m_connection_enabled = true;
}


void options::OnAddDatasourceClick( wxCommandEvent& event )
{
    connectionsaved = false;
    SetDefaultConnectionParams();

    long itemIndex = -1;
    for ( ;; )
    {
        itemIndex = m_lcSources->GetNextItem( itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        if ( itemIndex == -1 )
            break;
        m_lcSources->SetItemState( itemIndex, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED );
    }
    m_buttonRemove->Enable( false );
}

void options::FillSourceList()
{
    m_buttonRemove->Enable(false);
    m_lcSources->DeleteAllItems();
    for (size_t i = 0; i < g_pConnectionParams->Count(); i++)
    {
        wxListItem li;
        li.SetId( i );
        li.SetImage( g_pConnectionParams->Item(i)->bEnabled ? 1 : 0  );
        li.SetData( i );
        li.SetText( _T("") );

        long itemIndex = m_lcSources->InsertItem( li );

        m_lcSources->SetItem(itemIndex, 1, g_pConnectionParams->Item(i)->GetSourceTypeStr());
        m_lcSources->SetItem(itemIndex, 2, g_pConnectionParams->Item(i)->GetAddressStr());
        wxString prio_str;
        prio_str.Printf(_T("%d"), g_pConnectionParams->Item(i)->Priority );
        m_lcSources->SetItem(itemIndex, 3, prio_str);
        m_lcSources->SetItem(itemIndex, 4, g_pConnectionParams->Item(i)->GetParametersStr());
        m_lcSources->SetItem(itemIndex, 5, g_pConnectionParams->Item(i)->GetIOTypeValueStr());
        m_lcSources->SetItem(itemIndex, 6, g_pConnectionParams->Item(i)->GetFiltersStr());
    }

#ifdef __WXOSX__
    m_lcSources->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 1, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 2, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 3, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 4, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 5, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 6, wxLIST_AUTOSIZE );
#else
    m_lcSources->SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
    m_lcSources->SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
    m_lcSources->SetColumnWidth( 2, wxLIST_AUTOSIZE );
    m_lcSources->SetColumnWidth( 3, wxLIST_AUTOSIZE_USEHEADER );
    m_lcSources->SetColumnWidth( 4, wxLIST_AUTOSIZE_USEHEADER );
    m_lcSources->SetColumnWidth( 5, wxLIST_AUTOSIZE_USEHEADER );
    m_lcSources->SetColumnWidth( 6, wxLIST_AUTOSIZE );
#endif

    m_lcSources->SortItems( SortConnectionOnPriority, (long) m_lcSources );
}

void options::OnRemoveDatasourceClick( wxCommandEvent& event )
{
    long itemIndex = -1;
    for ( ;; )
    {
        itemIndex = m_lcSources->GetNextItem( itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        if ( itemIndex == -1 )
            break;

        int params_index = m_lcSources->GetItemData(itemIndex);
        if( params_index != -1 ){
            ConnectionParams *cp = g_pConnectionParams->Item(params_index);
            g_pConnectionParams->RemoveAt( params_index );

            DataStream *pds_existing = g_pMUX->FindStream( cp->GetDSPort() );
            if(pds_existing) 
                g_pMUX->StopAndRemoveStream( pds_existing );
        }
            
        //  Mark connection deleted
        m_rbTypeSerial->SetValue(true);
        m_comboPort->SetValue( _T("Deleted") );
    }
    FillSourceList();
    ShowNMEACommon( false );
    ShowNMEANet( false );
    ShowNMEASerial( false );
}

void options::OnSelectDatasource( wxListEvent& event )
{
    connectionsaved = false;
    int params_index = event.GetData();
    SetConnectionParams(g_pConnectionParams->Item(params_index));
    m_buttonRemove->Enable();
    event.Skip();
}

void options::OnBtnIStcs( wxCommandEvent& event )
{
    m_stcdialog_in->SetSentenceList(wxStringTokenize(m_tcInputStc->GetValue(), _T(",")));
    m_stcdialog_in->SetType(0, (m_rbIAccept->GetValue() == true) ? WHITELIST:BLACKLIST);

    if (m_stcdialog_in->ShowModal() == wxID_OK)
        m_tcInputStc->SetValue(m_stcdialog_in->GetSentencesAsText());
}

void options::OnBtnOStcs( wxCommandEvent& event )
{
    m_stcdialog_out->SetSentenceList(wxStringTokenize(m_tcOutputStc->GetValue(), _T(",")));
    m_stcdialog_out->SetType(1, (m_rbOAccept->GetValue() == true) ? WHITELIST:BLACKLIST);

    if (m_stcdialog_out->ShowModal() == wxID_OK)
        m_tcOutputStc->SetValue(m_stcdialog_out->GetSentencesAsText());
}

void options::OnNetProtocolSelected( wxCommandEvent& event )
{
    if (m_rbNetProtoGPSD->GetValue())
    {
        if (m_tNetPort->GetValue() == wxEmptyString)
            m_tNetPort->SetValue(_T("2947"));
    }
    else if (m_rbNetProtoUDP->GetValue())
    {
        if (m_tNetPort->GetValue() == wxEmptyString)
            m_tNetPort->SetValue(_T("10110"));
    }
    else if (m_rbNetProtoTCP->GetValue())
    {
        if (m_tNetPort->GetValue() == wxEmptyString)
            m_tNetPort->SetValue(_T("10110"));
    }

    SetDSFormRWStates();
    OnConnValChange(event);
}

void options::OnRbAcceptInput( wxCommandEvent& event )
{
    OnConnValChange(event);
}
void options::OnRbIgnoreInput( wxCommandEvent& event )
{
    OnConnValChange(event);
}

void options::OnRbOutput( wxCommandEvent& event )
{
    OnConnValChange(event);
}

void options::OnCbInput( wxCommandEvent& event )
{
    OnConnValChange(event);
}

void options::OnCbOutput( wxCommandEvent& event )
{
    OnConnValChange(event);
    if (m_cbOutput->IsChecked()) {
        m_stPrecision->Enable(true);
        m_choicePrecision->Enable(true);
    } else {
        m_stPrecision->Enable(false);
        m_choicePrecision->Enable(false);
    }
}

//SentenceListDlg

SentenceListDlg::SentenceListDlg( FilterDirection dir, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    m_dir = dir;
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer16;
    bSizer16 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer17;
    bSizer17 = new wxBoxSizer( wxHORIZONTAL );

    NMEA0183 nmea;
    standard_sentences = nmea.GetRecognizedArray();
    if(m_dir == FILTER_OUTPUT) {
        standard_sentences.Add(_T("ECRMB"));
        standard_sentences.Add(_T("ECRMC"));
        standard_sentences.Add(_T("ECAPB"));
    }

    standard_sentences.Add(_T("AIVDM"));
    standard_sentences.Add(_T("AIVDO"));
    standard_sentences.Add(_T("FRPOS"));
    standard_sentences.Add(_T("CD"));

    m_pclbBox = new wxStaticBox( this,  wxID_ANY, _T("")) ;

    wxStaticBoxSizer* sbSizerclb;
    sbSizerclb = new wxStaticBoxSizer( m_pclbBox , wxVERTICAL );
    bSizer17->Add( sbSizerclb, 1, wxALL|wxEXPAND, 5 );

    m_clbSentences = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, standard_sentences );


    sbSizerclb->Add( m_clbSentences, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer18;
    bSizer18 = new wxBoxSizer( wxVERTICAL );

    m_btnCheckAll = new wxButton( this, wxID_ANY, _("Select All"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer18->Add( m_btnCheckAll, 0, wxALL, 5 );

    m_btnClearAll = new wxButton( this, wxID_ANY, _("Clear All"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer18->Add( m_btnClearAll, 0, wxALL, 5 );

    bSizer18->AddSpacer(1);

    m_btnAdd = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer18->Add( m_btnAdd, 0, wxALL, 5 );

    m_btnDel = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnDel->Enable( false );

    bSizer18->Add( m_btnDel, 0, wxALL, 5 );


    bSizer17->Add( bSizer18, 0, wxALL|wxEXPAND, 5 );


    bSizer16->Add( bSizer17, 1, wxEXPAND, 5 );

    m_sdbSizer4 = new wxStdDialogButtonSizer();
    m_sdbSizer4OK = new wxButton( this, wxID_OK );
    m_sdbSizer4->AddButton( m_sdbSizer4OK );
    m_sdbSizer4Cancel = new wxButton( this, wxID_CANCEL );
    m_sdbSizer4->AddButton( m_sdbSizer4Cancel );
    m_sdbSizer4->Realize();

    bSizer16->Add( m_sdbSizer4, 0, wxALL|wxEXPAND, 5 );


    this->SetSizer( bSizer16 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    m_btnAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnAddClick ), NULL, this );
    m_btnDel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnDeleteClick ), NULL, this );
    m_sdbSizer4Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnCancelClick ), NULL, this );
    m_sdbSizer4OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnOkClick ), NULL, this );
    m_clbSentences->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( SentenceListDlg::OnCLBToggle ), NULL, this );
    m_clbSentences->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SentenceListDlg::OnCLBSelect ), NULL, this );
    m_btnCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnCheckAllClick ), NULL, this );
    m_btnClearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnClearAllClick ), NULL, this );

}

SentenceListDlg::~SentenceListDlg()
{
    // Disconnect Events
    m_btnAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnAddClick ), NULL, this );
    m_btnDel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnDeleteClick ), NULL, this );
    m_sdbSizer4Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnCancelClick ), NULL, this );
    m_sdbSizer4OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnOkClick ), NULL, this );
    m_clbSentences->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( SentenceListDlg::OnCLBToggle ), NULL, this );
    m_clbSentences->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SentenceListDlg::OnCLBSelect ), NULL, this );
    m_btnCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnCheckAllClick ), NULL, this );
    m_btnClearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SentenceListDlg::OnClearAllClick ), NULL, this );
}

void SentenceListDlg::SetSentenceList(wxArrayString sentences)
{
    m_sentences = sentences;

    if( (m_sentences.Count() == 0) && (m_type == WHITELIST) ){
        for (size_t i = 0; i < m_clbSentences->GetCount(); i++)
            m_clbSentences->Check(i, true);
    }
    if( (m_sentences.Count() == 0) && (m_type == BLACKLIST) ){
        for (size_t i = 0; i < m_clbSentences->GetCount(); i++)
            m_clbSentences->Check(i, false);
    }

    FillSentences();
}

wxString SentenceListDlg::GetSentencesAsText()
{
    return StringArrayToString(m_sentences);
}

void SentenceListDlg::BuildSentenceArray()
{
    m_sentences.Clear();
    wxString s;
    for (size_t i = 0; i < m_clbSentences->GetCount(); i++) {
        if( m_clbSentences->IsChecked(i))
            m_sentences.Add( m_clbSentences->GetString(i) );
    }
}

void SentenceListDlg::FillSentences()
{
    if(m_sentences.Count() == 0)
        return;

    for (size_t i = 0; i < m_clbSentences->GetCount(); i++)
        m_clbSentences->Check(i, false);

    for (size_t i = 0; i < m_sentences.Count(); i++) {
        int item = m_clbSentences->FindString(m_sentences[i]);
        if( wxNOT_FOUND != item )
            m_clbSentences->Check(item);
        else {
            m_clbSentences->Append(m_sentences[i]);
            int item = m_clbSentences->FindString(m_sentences[i]);
            m_clbSentences->Check(item);
        }

    }

    m_btnDel->Enable(false);
}

void SentenceListDlg::OnStcSelect( wxCommandEvent& event )
{
    m_btnDel->Enable();
}

void SentenceListDlg::OnCLBSelect( wxCommandEvent& event )
{
    // Only active the "Delete" button if the selection is not in the standard list
    int isel = m_clbSentences->GetSelection();
    bool bdelete = true;
    if(isel >= 0) {
        wxString s = m_clbSentences->GetString(isel);
        for (size_t i = 0; i < standard_sentences.Count(); i++) {
            if(standard_sentences[i] == s){
                bdelete = false;
                break;
            }
        }
    }
    else
        bdelete = false;
    m_btnDel->Enable( bdelete );

}

void SentenceListDlg::OnCLBToggle( wxCommandEvent& event )
{
    BuildSentenceArray();
}

void SentenceListDlg::OnAddClick( wxCommandEvent& event )
{
    wxString stc = wxGetTextFromUser(_("Enter the NMEA sentence (2, 3 or 5 characters)"), _("Enter the NMEA sentence"));
    if (stc.Length() == 2 ||stc.Length() == 3 || stc.Length() == 5)
    {
        m_sentences.Add(stc);
        m_clbSentences->Append(stc);
        int item = m_clbSentences->FindString(stc);
        m_clbSentences->Check(item);
    }
    else
        OCPNMessageBox( NULL, _("An NMEA sentence is generally 3 characters long (like RMC, GGA etc.)\n It can also have a two letter prefix identifying the source, or TALKER, of the message.\n The whole sentences then looks like GPGGA or AITXT.\n You may filter out all the sentences with certain TALKER prefix (like GP, AI etc.).\n\n The filter accepts just these three formats."),
                       _("OpenCPN Info"));
}

void SentenceListDlg::OnDeleteClick( wxCommandEvent& event )
{
    BuildSentenceArray();

    // One can only delete items that do not appear in the standard sentence list
    int isel = m_clbSentences->GetSelection();
    wxString s = m_clbSentences->GetString(isel);
    bool bdelete = true;
    for (size_t i = 0; i < standard_sentences.Count(); i++) {
        if(standard_sentences[i] == s){
            bdelete = false;
            break;
        }
    }

    if(bdelete) {
        m_sentences.Remove( s );
        m_clbSentences->Delete(isel);
    }

    FillSentences();
}

void SentenceListDlg::OnClearAllClick( wxCommandEvent& event )
{
    for (size_t i = 0; i < m_clbSentences->GetCount(); i++)
        m_clbSentences->Check(i, false);

    BuildSentenceArray();
}

void SentenceListDlg::OnCheckAllClick( wxCommandEvent& event )
{
    for (size_t i = 0; i < m_clbSentences->GetCount(); i++)
        m_clbSentences->Check(i, true);

    BuildSentenceArray();
}

void SentenceListDlg::SetType(int io, ListType type)
{
    m_type = type;

    switch (io)
    {
        case 0:                 // input
        default:
            if(type == WHITELIST)
                m_pclbBox->SetLabel(_("Accept Sentences"));
            else
                m_pclbBox->SetLabel(_("Ignore Sentences"));
            break;
        case 1:                 // output
            if(type == WHITELIST)
                m_pclbBox->SetLabel(_("Transmit Sentences"));
            else
                m_pclbBox->SetLabel(_("Drop Sentences"));
            break;
    }
    Refresh();
}



void SentenceListDlg::OnCancelClick( wxCommandEvent& event ) { event.Skip(); }
void SentenceListDlg::OnOkClick( wxCommandEvent& event ) { event.Skip(); }
 
//OpenGLOptionsDlg
enum { ID_BUTTON_REBUILD, ID_BUTTON_CLEAR };

BEGIN_EVENT_TABLE( OpenGLOptionsDlg, wxDialog )
EVT_BUTTON( ID_BUTTON_REBUILD, OpenGLOptionsDlg::OnButtonRebuild )
EVT_BUTTON( ID_BUTTON_CLEAR, OpenGLOptionsDlg::OnButtonClear )
END_EVENT_TABLE()


OpenGLOptionsDlg::OpenGLOptionsDlg( wxWindow* parent, bool glTicked )
{
    long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER;
#ifdef __WXOSX__
    style |= wxSTAY_ON_TOP;
#endif
    
    wxDialog::Create( parent, wxID_ANY, _("OpenGL Options"), wxDefaultPosition, wxDefaultSize,
                      style );
    
    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    SetFont( *qFont );

    m_brebuild_cache = false;

#ifdef ocpnUSE_GL
    m_bSizer1 = new wxFlexGridSizer( 3 );
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    if(g_bexpert) {
        m_cbUseAcceleratedPanning = new wxCheckBox(this, wxID_ANY, _("Use Accelerated Panning") );
        m_bSizer1->Add(m_cbUseAcceleratedPanning, 0, wxALL | wxEXPAND, 5);
        if( cc1->GetglCanvas()->CanAcceleratePanning() ) {
            m_cbUseAcceleratedPanning->Enable();
            m_cbUseAcceleratedPanning->SetValue(g_GLOptions.m_bUseAcceleratedPanning);
        } else {
            m_cbUseAcceleratedPanning->SetValue(false);
            m_cbUseAcceleratedPanning->Disable();
        }

        m_bSizer1->AddSpacer(1);
    }

    if(g_bexpert){
        m_cbTextureCompression = new wxCheckBox(this, wxID_ANY, _("Texture Compression") );
        m_cbTextureCompression->SetValue(g_GLOptions.m_bTextureCompression);
        m_bSizer1->Add(m_cbTextureCompression, 0, wxALL | wxEXPAND, 5);

        m_cbTextureCompressionCaching = new wxCheckBox(this, wxID_ANY, _("Texture Compression Caching") );
        m_cbTextureCompressionCaching->SetValue(g_GLOptions.m_bTextureCompressionCaching);
    
        /* disable caching if unsupported */
        extern PFNGLCOMPRESSEDTEXIMAGE2DPROC s_glCompressedTexImage2D;
        extern bool  b_glEntryPointsSet;

        if(b_glEntryPointsSet){
            if(!s_glCompressedTexImage2D) {
                g_GLOptions.m_bTextureCompressionCaching = false;
                m_cbTextureCompression->Disable();
                m_cbTextureCompression->SetValue(false);
            }
        }
        
        m_bSizer1->Add(m_cbTextureCompressionCaching, 0, wxALL | wxEXPAND, 5);
    }
    else {
        m_cbTextureCompression = new wxCheckBox(this, wxID_ANY, _("Texture Compression with Caching") );
        m_cbTextureCompression->SetValue(g_GLOptions.m_bTextureCompression);
        m_bSizer1->Add(m_cbTextureCompression, 0, wxALL | wxEXPAND, 5);
    }
        

        /* disable caching if unsupported */
    extern PFNGLCOMPRESSEDTEXIMAGE2DPROC s_glCompressedTexImage2D;
    if(!s_glCompressedTexImage2D) {
        g_GLOptions.m_bTextureCompressionCaching = false;
        m_cbTextureCompression->Disable();
    }
        
 
    if(g_bexpert){
        wxStaticText* stTextureMemorySize =
            new wxStaticText( this, wxID_STATIC, _("Texture Memory Size (MB)") );
#ifdef __WXOSX__
        m_bSizer1->Add( stTextureMemorySize, 0, wxLEFT | wxRIGHT | wxTOP, 5 );
#else
        m_bSizer1->Add( stTextureMemorySize, 0,
                       wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );
#endif

        m_sTextureMemorySize = new wxSpinCtrl( this );
        m_sTextureMemorySize->SetRange(1, 16384 );
        m_sTextureMemorySize->SetValue(g_GLOptions.m_iTextureMemorySize);
        m_bSizer1->Add(m_sTextureMemorySize, 0, wxALL | wxEXPAND, 5);

    }

    m_bSizer1->AddSpacer(0);
    m_bSizer1->AddSpacer(0);
    
    m_bRebuildTextureCache = new wxButton(this, ID_BUTTON_REBUILD, _("Rebuild Texture Cache") );
    m_bSizer1->Add(m_bRebuildTextureCache, 0, wxALL | wxEXPAND, 5);
    m_bRebuildTextureCache->Enable(g_GLOptions.m_bTextureCompressionCaching);

    if(!g_bopengl || g_raster_format == GL_RGB)
        m_bRebuildTextureCache->Disable();
    
    m_bClearTextureCache = new wxButton(this, ID_BUTTON_CLEAR, _("Clear Texture Cache") );
    m_bSizer1->Add(m_bClearTextureCache, 0, wxALL | wxEXPAND, 5);
    m_bClearTextureCache->Enable(g_GLOptions.m_bTextureCompressionCaching);
    
    m_stTextureCacheSize = new wxStaticText(this, wxID_STATIC, TextureCacheSize());
#ifdef __WXOSX__
    m_bSizer1->Add( m_stTextureCacheSize, 0,
                   wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 5 );
#else
    m_bSizer1->Add( m_stTextureCacheSize, 0,
                   wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );
#endif

    m_cbShowFPS = new wxCheckBox( this, wxID_ANY, _("Show FPS") );
#ifdef __WXOSX__
    m_bSizer1->Add( m_cbShowFPS, 0,  wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 5 );
#else
    m_bSizer1->Add( m_cbShowFPS, 0,  wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxADJUST_MINSIZE, 5 );
#endif
    m_cbShowFPS->SetValue(g_bShowFPS);

    wxStdDialogButtonSizer * m_sdbSizer4 = new wxStdDialogButtonSizer();
    wxButton *bOK = new wxButton( this, wxID_OK );
    m_sdbSizer4->AddButton( bOK );
    wxButton *bCancel = new wxButton( this, wxID_CANCEL );
    m_sdbSizer4->AddButton( bCancel );
    m_sdbSizer4->Realize();

    m_bSizer1->Add( m_sdbSizer4, 0, wxALL|wxEXPAND, 5 );

    this->SetSizer( m_bSizer1 );
    this->Layout();

    this->Centre( wxBOTH );

    Fit();
#endif
}

void OpenGLOptionsDlg::OnButtonRebuild( wxCommandEvent& event )
{
#ifdef ocpnUSE_GL
    if(g_GLOptions.m_bTextureCompressionCaching) {
        m_brebuild_cache = true;
        EndModal(wxID_CANCEL);
    }
#endif
}

void OpenGLOptionsDlg::OnButtonClear( wxCommandEvent& event )
{
#ifdef ocpnUSE_GL
    ::wxBeginBusyCursor();
    if(g_bopengl)
        cc1->GetglCanvas()->ClearAllRasterTextures();
    
    wxString path =  g_PrivateDataDir + wxFileName::GetPathSeparator() + _T("raster_texture_cache");
    if(::wxDirExists( path )){
        wxArrayString files;
        size_t nfiles = wxDir::GetAllFiles(path, &files);
        for(unsigned int i=0 ; i < files.GetCount() ; i++){
            ::wxRemoveFile(files[i]);
        }
    }
    
    m_stTextureCacheSize->SetLabel(TextureCacheSize());
    ::wxEndBusyCursor();
#endif
}

wxString OpenGLOptionsDlg::TextureCacheSize()
{
    wxString path =  g_PrivateDataDir + wxFileName::GetPathSeparator() + _T("raster_texture_cache");
    int total = 0;
    if(::wxDirExists( path )) {        
        wxArrayString files;
        size_t nfiles = wxDir::GetAllFiles(path, &files);
        for(unsigned int i=0 ; i < files.GetCount() ; i++){
            total += wxFile(files[i]).Length();
        }
    }
    
    return wxString::Format(_T("%.1f MB"), total/1024.0/1024.0);
}
