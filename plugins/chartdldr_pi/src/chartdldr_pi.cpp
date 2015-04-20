/***************************************************************************
 * $Id: chartdldr_pi.cpp,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Chart downloader Plugin
 * Author:   Pavel Kalian
 *
 ***************************************************************************
 *   Copyright (C) 2011 by Pavel Kalian                                    *
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
 ***************************************************************************/


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "chartdldr_pi.h"
#include <wx/stdpaths.h>
#include <wx/url.h>
#include <wx/progdlg.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/filesys.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <memory>
#include <wx/regex.h>
#include "unrar/rar.hpp"

#include <wx/arrimpl.cpp>
    WX_DEFINE_OBJARRAY(wxArrayOfChartSources);
    WX_DEFINE_OBJARRAY(wxArrayOfDateTime);

#include <fstream>

#ifdef __WXMAC__
#define CATALOGS_NAME_WIDTH 300
#define CATALOGS_DATE_WIDTH 120
#define CATALOGS_PATH_WIDTH 100
#define CHARTS_NAME_WIDTH 300
#define CHARTS_STATUS_WIDTH 100
#define CHARTS_DATE_WIDTH 120
#else
#define CATALOGS_NAME_WIDTH 200
#define CATALOGS_DATE_WIDTH 130
#define CATALOGS_PATH_WIDTH 250
#define CHARTS_NAME_WIDTH 320
#define CHARTS_STATUS_WIDTH 150
#define CHARTS_DATE_WIDTH 130
#endif // __WXMAC__

#define CHART_DIR "Charts"

void write_file( const wxString extract_file, char *data, unsigned long datasize )
{
    wxFileName fn(extract_file);
    if( wxDirExists( fn.GetPath() ) )
    {
        if( !wxFileName::Mkdir(fn.GetPath(), 0755, wxPATH_MKDIR_FULL) )
        {
            wxLogError(_T("Can not create directory '") + fn.GetPath() + _T("'."));
            return;
        }
    }
    wxFileOutputStream f(extract_file);
    f.Write(data, datasize);
    f.Close();
}

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new chartdldr_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    ChartDldr PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

chartdldr_pi::chartdldr_pi( void *ppimgr ) : opencpn_plugin_112( ppimgr )
{
    // Create the PlugIn icons
    initialize_images();

    m_chartSources = NULL;
    m_parent_window = NULL;
    m_pChartCatalog = NULL;
    m_pChartSource = NULL;
    m_pconfig = NULL;
    m_preselect_new = false;
    m_preselect_updated = false;
    m_allow_bulk_update = false;
    m_pOptionsPage = NULL;
    m_selected_source = -1;
    m_dldrpanel = NULL;
    m_leftclick_tool_id = -1;
    m_schartdldr_sources = wxEmptyString;
    wxCurlBase::Init();
}

int chartdldr_pi::Init( void )
{
    AddLocaleCatalog( _T("opencpn-chartdldr_pi") );

    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();
    m_pOptionsPage = NULL;

    m_chartSources = new wxArrayOfChartSources();
    m_pChartCatalog = new ChartCatalog;
    m_pChartSource = NULL;

    //    And load the configuration items
    LoadConfig();

    wxStringTokenizer st(m_schartdldr_sources, _T("|"), wxTOKEN_DEFAULT);
    while( st.HasMoreTokens() )
    {
        wxString s1 = st.GetNextToken();
        wxString s2 = st.GetNextToken();
        wxString s3 = st.GetNextToken();
        m_chartSources->Add(new ChartSource(s1, s2, s3));
    }
    return (
            WANTS_PREFERENCES         |
            WANTS_CONFIG              |
            INSTALLS_TOOLBOX_PAGE
            );
}

bool chartdldr_pi::DeInit( void )
{
    m_chartSources->Clear();
    wxDELETE(m_chartSources);
    wxDELETE(m_pChartCatalog);
    wxDELETE(m_pChartSource);
    /* TODO: Seth */
//      dialog->Close();
//      dialog->Destroy();
//      wxDELETE(dialog);
    /* We must delete remaining page if the plugin is disabled while in Options dialog */
    if( m_pOptionsPage )
    {
        if( DeleteOptionsPage( m_pOptionsPage ) )
            m_pOptionsPage = NULL;
        // TODO: any other memory leak?
    }
    return true;
}

int chartdldr_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int chartdldr_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int chartdldr_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int chartdldr_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *chartdldr_pi::GetPlugInBitmap()
{
    return _img_chartdldr_pi;
}

wxString chartdldr_pi::GetCommonName()
{
    return _("ChartDownloader");
}

wxString chartdldr_pi::GetShortDescription()
{
    return _("Chart Downloader PlugIn for OpenCPN");
}

wxString chartdldr_pi::GetLongDescription()
{
    return _("Chart Downloader PlugIn for OpenCPN\n\
Manages chart downloads and updates from sources supporting\n\
NOAA Chart Catalog format");
}

void chartdldr_pi::OnSetupOptions( void )
{
    m_pOptionsPage = AddOptionsPage( PI_OPTIONS_PARENT_CHARTS, _("Chart Downloader") );
    if( ! m_pOptionsPage )
    {
        wxLogMessage( _T("Error: chartdldr_pi::OnSetupOptions AddOptionsPage failed!") );
        return;
    }
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    m_pOptionsPage->SetSizer( sizer );

    m_dldrpanel = new ChartDldrPanelImpl( this, m_pOptionsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );

    m_pOptionsPage->InvalidateBestSize();
    sizer->Add( m_dldrpanel, 1, wxALL | wxEXPAND );
    m_dldrpanel->SetBulkUpdate( m_allow_bulk_update );
    m_dldrpanel->FitInside();
}

void chartdldr_pi::OnCloseToolboxPanel( int page_sel, int ok_apply_cancel )
{
    /* TODO: Seth */
    m_selected_source = m_dldrpanel->GetSelectedCatalog();
    SaveConfig();
}

bool chartdldr_pi::LoadConfig( void )
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if( pConf )
    {
        pConf->SetPath ( _T ( "/Settings/ChartDnldr" ) );
        pConf->Read ( _T ( "ChartSources" ), &m_schartdldr_sources, wxEmptyString );
        pConf->Read ( _T ( "Source" ), &m_selected_source, -1 );

        wxFileName fn(wxStandardPaths::Get().GetDocumentsDir(), wxEmptyString);
        fn.AppendDir(_T(CHART_DIR));
        pConf->Read ( _T ( "BaseChartDir" ), &m_base_chart_dir,  fn.GetPath() );
        pConf->Read ( _T ( "PreselectNew" ), &m_preselect_new, false );
        pConf->Read ( _T ( "PreselectUpdated" ), &m_preselect_updated, true );
        pConf->Read ( _T ( "AllowBulkUpdate" ), &m_allow_bulk_update, false );
        return true;
    }
    else
        return false;
}

bool chartdldr_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    m_schartdldr_sources.Clear();

    for( size_t i = 0; i < m_chartSources->GetCount(); i++ )
    {
        ChartSource *cs = m_chartSources->Item(i);
        m_schartdldr_sources.Append(wxString::Format(_T("%s|%s|%s|"), cs->GetName().c_str(), cs->GetUrl().c_str(), cs->GetDir().c_str()));
    }

    if( pConf )
    {
        pConf->SetPath ( _T ( "/Settings/ChartDnldr" ) );
        pConf->Write ( _T ( "ChartSources" ), m_schartdldr_sources );
        pConf->Write ( _T ( "Source" ), m_selected_source );
        pConf->Write ( _T ( "BaseChartDir" ), m_base_chart_dir );
        pConf->Write ( _T ( "PreselectNew" ), m_preselect_new );
        pConf->Write ( _T ( "PreselectUpdated" ), m_preselect_updated );
        pConf->Write ( _T ( "AllowBulkUpdate" ), m_allow_bulk_update );

        return true;
    }
    else
        return false;
}

void chartdldr_pi::ShowPreferencesDialog( wxWindow* parent )
{
    ChartDldrPrefsDlgImpl *dialog = new ChartDldrPrefsDlgImpl(m_parent_window);
    dialog->SetPath(m_base_chart_dir);
    dialog->SetPreferences(m_preselect_new, m_preselect_updated, m_allow_bulk_update);
    if( wxID_OK == dialog->ShowModal() )
    {
        m_base_chart_dir = dialog->GetPath();
        dialog->GetPreferences(m_preselect_new, m_preselect_updated, m_allow_bulk_update);
        SaveConfig();
        m_dldrpanel->SetBulkUpdate( m_allow_bulk_update );
    }
    dialog->Close();
    dialog->Destroy();
    wxDELETE(dialog);
}

ChartSource::ChartSource( wxString name, wxString url, wxString localdir )
{
    m_name = name;
    m_url = url;
    m_dir = localdir;
}

#define ID_MNU_SELALL 2001
#define ID_MNU_DELALL 2002
#define ID_MNU_INVSEL 2003
#define ID_MNU_SELUPD 2004
#define ID_MNU_SELNEW 2005

void ChartDldrPanelImpl::OnPopupClick( wxCommandEvent &evt )
{
    switch( evt.GetId() )
    {
        case ID_MNU_SELALL:
            m_clCharts->CheckAll(true);
            break;
        case ID_MNU_DELALL:
            m_clCharts->CheckAll(false);
            break;
        case ID_MNU_INVSEL:
            for (int i = 0; i < m_clCharts->GetItemCount(); i++)
                m_clCharts->Check(i, !m_clCharts->IsChecked(i));
            break;
        case ID_MNU_SELUPD:
        {
            ChartSource *cs = pPlugIn->m_chartSources->Item(GetSelectedCatalog());
            FillFromFile(cs->GetUrl(), cs->GetDir(), false, true);
            break;
        }
        case ID_MNU_SELNEW:
        {
            ChartSource *cs = pPlugIn->m_chartSources->Item(GetSelectedCatalog());
            FillFromFile(cs->GetUrl(), cs->GetDir(), true, false);
            break;
        }
    }
}

void ChartDldrPanelImpl::OnContextMenu( wxMouseEvent& event )
{
    if( m_clCharts->GetItemCount() == 0 )
        return;
    wxMenu menu;
    wxPoint point = event.GetPosition();
    wxPoint p1 = ((wxWindow *)m_clCharts)->GetPosition();

    // add stuff
    menu.Append(ID_MNU_SELALL, _("Select all"), wxT(""));
    menu.Append(ID_MNU_DELALL, _("Deselect all"), wxT(""));
    menu.Append(ID_MNU_INVSEL, _("Invert selection"), wxT(""));
    menu.Append(ID_MNU_SELUPD, _("Select updated"), wxT(""));
    menu.Append(ID_MNU_SELNEW, _("Select new"), wxT(""));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ChartDldrPanelImpl::OnPopupClick, NULL, this);
    // and then display
    PopupMenu(&menu, p1.x + point.x, p1.y + point.y);
}

void ChartDldrPanelImpl::OnShowLocalDir( wxCommandEvent& event )
{
#ifdef __WXGTK__
    wxExecute(wxString::Format(_T("xdg-open %s"), pPlugIn->m_pChartSource->GetDir().c_str()));
#endif
#ifdef __WXMAC__
    wxExecute(wxString::Format(_T("open %s"), pPlugIn->m_pChartSource->GetDir().c_str()));
#endif
#ifdef __WXMSW__
    wxExecute(wxString::Format(_T("explorer %s"), pPlugIn->m_pChartSource->GetDir().c_str()));
#endif
}

void ChartDldrPanelImpl::SetSource( int id )
{
    wxSetCursor(wxCURSOR_WAIT);
    wxYield();
    pPlugIn->SetSourceId( id );
    
    m_bDeleteSource->Enable( id >= 0 );
    m_bUpdateChartList->Enable( id >= 0 );
    m_bEditSource->Enable( id >= 0 );

    // TODO: DAN - Need to optimze to only update the chart list if needed.
    //             Right now it updates multiple times unnecessarily.
    CleanForm();
    if( id >= 0 && id < (int)pPlugIn->m_chartSources->Count() )
    {
        ChartSource *cs = pPlugIn->m_chartSources->Item(id);
        cs->UpdateLocalFiles();
        pPlugIn->m_pChartSource = cs;
        FillFromFile(cs->GetUrl(), cs->GetDir(), pPlugIn->m_preselect_new, pPlugIn->m_preselect_updated);
    }
    else
    {
        pPlugIn->m_pChartSource = NULL;
    }
    wxSetCursor(wxCURSOR_DEFAULT);
}

void ChartDldrPanelImpl::SelectSource( wxListEvent& event )
{
    SetSource(GetSelectedCatalog());
    event.Skip();
}

void ChartDldrPanelImpl::SetBulkUpdate( bool bulk_update )
{
    m_bUpdateAllCharts->Enable( bulk_update );
    m_bUpdateAllCharts->Show( bulk_update );
    Layout();
    m_parent->Layout();
}

void ChartDldrPanelImpl::CleanForm()
{
    m_clCharts->DeleteAllItems();
    m_stCatalogInfo->Show( false );
}

void ChartDldrPanelImpl::FillFromFile( wxString url, wxString dir, bool selnew, bool selupd )
{
    //load if exists
    wxStringTokenizer tk(url, _T("/"));
    wxString file;
    do
    {
        file = tk.GetNextToken();
    }
    while( tk.HasMoreTokens() );
    wxFileName fn;
    fn.SetFullName(file);
    fn.SetPath(dir);
    wxString path = fn.GetFullPath();
    if( wxFileExists(path) )
    {
        pPlugIn->m_pChartCatalog->LoadFromFile(path);
//            m_tChartSourceInfo->SetValue(pPlugIn->m_pChartCatalog->GetDescription());
        //fill in the rest of the form
        m_clCharts->DeleteAllItems();
        size_t updated_charts = 0;
        size_t new_charts = 0;
        for( size_t i = 0; i < pPlugIn->m_pChartCatalog->charts->Count(); i++ )
        {
            wxListItem li;
            li.SetId(i);
            li.SetText(pPlugIn->m_pChartCatalog->charts->Item(i).GetChartTitle());
            long x = m_clCharts->InsertItem(li);
            m_clCharts->SetItem(x, 0, pPlugIn->m_pChartCatalog->charts->Item(i).GetChartTitle());
            wxString file = pPlugIn->m_pChartCatalog->charts->Item(i).GetChartFilename(true);
            if( !pPlugIn->m_pChartSource->ExistsLocaly(pPlugIn->m_pChartCatalog->charts->Item(i).number, file) )
            {
                new_charts++;
                m_clCharts->SetItem(x, 1, _("New"));
                if (selnew)
                    m_clCharts->Check(x, true);
            }
            else
            {
                if( pPlugIn->m_pChartSource->IsNewerThanLocal(pPlugIn->m_pChartCatalog->charts->Item(i).number, file, pPlugIn->m_pChartCatalog->charts->Item(i).GetUpdateDatetime()) )
                {
                    updated_charts++;
                    m_clCharts->SetItem(x, 1, _("Update available"));
                    if (selupd)
                        m_clCharts->Check(x, true);
                }
                else
                {
                    m_clCharts->SetItem(x, 1, _("Up to date"));
                }
            }
            m_clCharts->SetItem(x, 2, pPlugIn->m_pChartCatalog->charts->Item(i).GetUpdateDatetime().Format(_T("%Y-%m-%d %H:%M")));
        }
#ifdef __WXOSX__
        m_stCatalogInfo->SetLabel( wxString::Format( _("%lu charts total, %lu updated, %lu new"), pPlugIn->m_pChartCatalog->charts->Count(), updated_charts, new_charts ) );
#else
        m_stCatalogInfo->SetLabel( wxString::Format( _("%u charts total, %u updated, %u new"), pPlugIn->m_pChartCatalog->charts->Count(), updated_charts, new_charts ) );
#endif
        m_stCatalogInfo->Show( true );
    }
}

bool ChartSource::ExistsLocaly( wxString chart_number, wxString filename )
{
    wxStringTokenizer tk(filename, _T("."));
    wxString file = tk.GetNextToken().MakeLower();
    if( !m_update_data.empty() )
    {
        return m_update_data.find(std::string(chart_number.Lower().mb_str())) != m_update_data.end() || m_update_data.find(std::string(file.mb_str())) != m_update_data.end();
    }
    for( size_t i = 0; i < m_localfiles.Count(); i++ )
    {
        if (m_localfiles.Item(i) == file)
            return true;
    }
    return false;
}

bool ChartSource::IsNewerThanLocal( wxString chart_number, wxString filename, wxDateTime validDate )
{
    wxStringTokenizer tk(filename, _T("."));
    wxString file = tk.GetNextToken().MakeLower();
    if( !m_update_data.empty() )
    {
        if( m_update_data[std::string(chart_number.Lower().mbc_str())] < validDate.GetTicks() && m_update_data[std::string(file.mbc_str())] < validDate.GetTicks() )
            return true;
        else
            return false;
    }
    bool update_candidate = false;

    for( size_t i = 0; i < m_localfiles.Count(); i++ )
    {
        if( m_localfiles.Item(i) == file )
        {
            if( validDate.IsLaterThan(m_localdt.Item(i)) )
            {
                update_candidate = true;
            }
            else
                return false;
        }
    }
    return update_candidate;
}

int ChartDldrPanelImpl::GetSelectedCatalog()
{
    long item = m_lbChartSources->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    return item;
}

void ChartDldrPanelImpl::SelectCatalog( int item )
{
    if( item >= 0 )
    {
        m_bDeleteSource->Enable();
        m_bUpdateChartList->Enable();
    }
    else
    {
        m_bDeleteSource->Disable();
        m_bUpdateChartList->Disable();
    }
    m_lbChartSources->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void ChartDldrPanelImpl::AppendCatalog( ChartSource *cs )
{
    long id = m_lbChartSources->GetItemCount();
    m_lbChartSources->InsertItem(id, cs->GetName());
    m_lbChartSources->SetItem(id, 1, _("(Please update first)"));
    m_lbChartSources->SetItem(id, 2, cs->GetDir());
    wxURI url(cs->GetUrl());
    if( url.IsReference() )
    {
        wxMessageBox(_("Error, the URL to the chart source data seems wrong."), _("Error"));
        return;
    }
    wxFileName fn(url.GetPath());
    fn.SetPath(cs->GetDir());
    wxString path = fn.GetFullPath();
    if( wxFileExists(path) )
    {
        if( pPlugIn->m_pChartCatalog->LoadFromFile(path, true) )
        {
            m_lbChartSources->SetItem(id, 0, pPlugIn->m_pChartCatalog->title);
            m_lbChartSources->SetItem(id, 1, pPlugIn->m_pChartCatalog->GetReleaseDate().Format(_T("%Y-%m-%d %H:%M")));
            m_lbChartSources->SetItem(id, 2, path);
        }
    }
}

void ChartDldrPanelImpl::UpdateAllCharts( wxCommandEvent& event )
{
    int failed_to_update = 0;
    int attempted_to_update = 0;
    if( (pPlugIn->m_preselect_new) && (pPlugIn->m_preselect_updated) )
    {
        wxMessageDialog mess(this, _("You have chosen to update all chart catalogs.\nThen download all new and updated charts.\nThis may take a long time."),
                _("Chart Downloader"), wxOK | wxCANCEL);
        if( mess.ShowModal() == wxID_CANCEL )
            return;
    }
    else if( pPlugIn->m_preselect_new )
    {
        wxMessageDialog mess(this, _("You have chosen to update all chart catalogs.\nThen download only new (but not updated) charts.\nThis may take a long time."),
                _("Chart Downloader"), wxOK | wxCANCEL);
        if (mess.ShowModal() == wxID_CANCEL) return;
    }
    else if( pPlugIn->m_preselect_updated )
    {
        wxMessageDialog mess(this, _("You have chosen to update all chart catalogs.\nThen download only updated (but not new) charts.\nThis may take a long time."),
                _("Chart Downloader"), wxOK | wxCANCEL);
        if (mess.ShowModal() == wxID_CANCEL) return;
    }
    updatingAll = true;
    for( long chartIndex = 0; chartIndex < m_lbChartSources->GetItemCount(); chartIndex++ )
    {
        m_lbChartSources->SetItemState(chartIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        UpdateChartList( event );
        if( cancelled )
            break;
        DownloadCharts( event );
        attempted_to_update += downloading;
        failed_to_update += failed_downloads;
        if( cancelled )
            break;
    }
    wxLogMessage( wxString::Format(_T("chartdldr_pi::UpdateAllCharts() downloaded %d out of %d charts."), attempted_to_update - failed_to_update, attempted_to_update) );
    if( failed_to_update > 0 )
        wxMessageBox( wxString::Format( _("%d out of %d charts failed to download.\nCheck the list, verify there is a working Internet connection and repeat the operation if needed."),
                failed_to_update, attempted_to_update ), _("Chart Downloader"), wxOK | wxICON_ERROR );
    if( attempted_to_update > failed_to_update )
        wxMessageBox( _("You have added/updated some of your charts.\nTo make sure OpenCPN knows about them, go to the 'Chart Files' tab and select the 'Scan Charts and Update Database' option."),
                _("Chart Downloader"), wxOK | wxICON_INFORMATION );
    updatingAll = false;
    cancelled = false;
}


void ChartDldrPanelImpl::UpdateChartList( wxCommandEvent& event )
{
    //TODO: check if everything exists and we can write to the output dir etc.
    if( !m_lbChartSources->GetSelectedItemCount() )
        return;
    ChartSource *cs = pPlugIn->m_chartSources->Item(GetSelectedCatalog());
    wxURI url(cs->GetUrl());
    if( url.IsReference() )
    {
        wxMessageBox(_("Error, the URL to the chart source data seems wrong."), _("Error"));
        return;
    }

    wxStringTokenizer tk(url.GetPath(), _T("/"));
    wxString file;
    do
    {
        file = tk.GetNextToken();
    }
    while( tk.HasMoreTokens() );
    wxFileName fn;
    fn.SetFullName(file);
    fn.SetPath(cs->GetDir());
    if( !wxDirExists(cs->GetDir()) )
    {
        if( !wxFileName::Mkdir(cs->GetDir(), 0755, wxPATH_MKDIR_FULL) )
        {
            wxMessageBox(wxString::Format(_("Directory %s can't be created."), cs->GetDir().c_str()), _("Chart Downloader"));
            return;
        }
    }

    wxFileName tfn = wxFileName::CreateTempFileName( fn.GetFullPath() );
    wxFileOutputStream output( tfn.GetFullPath() );
    wxCurlDownloadDialog ddlg(url.BuildURI(), &output, _("Downloading file"),
            _("Reading Headers: ") + url.BuildURI(), wxNullBitmap, this,
            wxCTDS_ELAPSED_TIME|wxCTDS_ESTIMATED_TIME|wxCTDS_REMAINING_TIME|wxCTDS_SPEED|wxCTDS_SIZE|wxCTDS_URL|wxCTDS_CAN_PAUSE|wxCTDS_CAN_ABORT|wxCTDS_AUTO_CLOSE);
    ddlg.SetSize(this->GetSize().GetWidth(), ddlg.GetSize().GetHeight());
    wxCurlDialogReturnFlag ret = ddlg.RunModal();
    output.Close();
    switch( ret )
    {
        case wxCDRF_SUCCESS:
        {
            if ( wxCopyFile( tfn.GetFullPath(), fn.GetFullPath() ) )
            {
                FillFromFile(url.GetPath(), fn.GetPath(), pPlugIn->m_preselect_new, pPlugIn->m_preselect_updated);
                long id = GetSelectedCatalog();
                m_lbChartSources->SetItem(id, 0, pPlugIn->m_pChartCatalog->title);
                m_lbChartSources->SetItem(id, 1, pPlugIn->m_pChartCatalog->GetReleaseDate().Format(_T("%Y-%m-%d %H:%M")));
                m_lbChartSources->SetItem(id, 2, cs->GetDir());
            }
            else
                wxMessageBox(wxString::Format( _("Failed to Find New Catalog: %s "), url.BuildURI().c_str() ),
                        _("Chart Downloader"), wxOK | wxICON_ERROR);
            break;
        }
        case wxCDRF_FAILED:
        {
            wxMessageBox(wxString::Format( _("Failed to Download Catalog: %s \nVerify there is a working Internet connection."), url.BuildURI().c_str() ),
                    _("Chart Downloader"), wxOK | wxICON_ERROR);
            break;
        }
        case wxCDRF_USER_ABORTED:
        {
            cancelled = true;
            break;
        }
        default:
            wxASSERT( false );  // This should never happen because we handle all possible cases of ret
    }
    wxRemoveFile ( tfn.GetFullPath() );
}

void ChartSource::GetLocalFiles()
{
    if( !UpdateDataExists() || m_update_data.empty() )
    {
        wxArrayString *allFiles = new wxArrayString;
        if( wxDirExists(GetDir()) )
            wxDir::GetAllFiles(GetDir(), allFiles);
        m_localdt.Clear();
        m_localfiles.Clear();
        wxDateTime ct, mt, at;
        wxString name;
        for( size_t i = 0; i < allFiles->Count(); i++ )
        {
            wxFileName fn(allFiles->Item(i));
            name = fn.GetFullName().Lower();
            // Only add unique files names to the local list.
            // This is safe because all chart names within a catalog
            // are necessarily unique.
            if( !ExistsLocaly(wxEmptyString, name) )
            {
                fn.GetTimes(&at, &mt, &ct);
                m_localdt.Add(mt);
                m_localfiles.Add(fn.GetName().Lower());

                wxStringTokenizer tk(name, _T("."));
                wxString file = tk.GetNextToken().MakeLower();
                m_update_data[std::string(file.mbc_str())] = mt.GetTicks();
            }
        }
        allFiles->Clear();
        wxDELETE(allFiles);
        SaveUpdateData();
    }
    else
    {
        LoadUpdateData();
    }
}

bool ChartSource::UpdateDataExists()
{
    return wxFileExists( GetDir() + wxFileName::GetPathSeparator() + _T(UPDATE_DATA_FILENAME) );
}

void ChartSource::LoadUpdateData()
{
    m_update_data.clear();
    wxString fn = GetDir() + wxFileName::GetPathSeparator() + _T(UPDATE_DATA_FILENAME);

    if( !wxFileExists( fn ) )
        return;
    
    std::ifstream infile( fn.mb_str() );
    
    std::string key;
    long value;

    while( infile >> key >> value )
        m_update_data[key] = value;
    
    infile.close();
}

void ChartSource::SaveUpdateData()
{
    wxString fn = GetDir() + wxFileName::GetPathSeparator() + _T(UPDATE_DATA_FILENAME);

    std::ofstream outfile( fn.mb_str() );
    if( !outfile.is_open() )
        return;
    
    std::map<std::string, time_t>::iterator iter;
    for( iter = m_update_data.begin(); iter != m_update_data.end(); ++iter )
    {
        if( iter->first.find(" ") == std::string::npos )
            outfile << iter->first << " " << iter->second << "\n";
    }
    
    outfile.close();
}

void ChartSource::ChartUpdated( wxString chart_number, time_t timestamp )
{
    m_update_data[std::string(chart_number.Lower().mb_str())] = timestamp;
    SaveUpdateData();
}

bool ChartDldrPanelImpl::DownloadChart( wxString url, wxString file, wxString title )
{
    if( cancelled )
        return false;
    downloading++;

    downloadInProgress = true;
    wxFileOutputStream output(file);
    wxCurlDownloadDialog ddlg(url, &output, wxString::Format(_("Downloading file %d of %d"), downloading, to_download),
            wxString::Format(_("Chart: %s"), title.c_str()), wxNullBitmap, this,
            wxCTDS_ELAPSED_TIME|wxCTDS_ESTIMATED_TIME|wxCTDS_REMAINING_TIME|wxCTDS_SPEED|wxCTDS_SIZE|wxCTDS_URL|wxCTDS_CAN_PAUSE|wxCTDS_CAN_ABORT|wxCTDS_AUTO_CLOSE);
    ddlg.SetSize(this->GetSize().GetWidth(), ddlg.GetSize().GetHeight());
    wxCurlDialogReturnFlag ret = ddlg.RunModal();
    output.Close(); 
    switch( ret )
    {
        case wxCDRF_SUCCESS:
            return true;
        case wxCDRF_FAILED:
        {
//            wxMessageBox(wxString::Format( _("Failed to Download: %s \nVerify there is a working Internet connection."), url.c_str() ), 
//            _("Chart Downloader"), wxOK | wxICON_ERROR);
            failed_downloads++;
            wxRemoveFile( file );
            return false;
        }
        case wxCDRF_USER_ABORTED:
        {
            failed_downloads++;
            wxRemoveFile( file );
            cancelled = true;
            return false;
        }
    }
    return false;
}

void ChartDldrPanelImpl::DownloadCharts( wxCommandEvent& event )
{
    failed_downloads = 0;
    cancelled = false;
    if( !m_lbChartSources->GetSelectedItemCount() && !updatingAll )
    {
        wxMessageBox(_("No charts selected for download."));
        return;
    }
    ChartSource *cs = pPlugIn->m_chartSources->Item(GetSelectedCatalog());
    if( m_clCharts->GetCheckedItemCount() == 0 && !updatingAll )
    {
        wxMessageBox(_("No charts selected for download."));
        return;
    }
    to_download = m_clCharts->GetCheckedItemCount();
    downloading = 0;
    for( int i = 0; i < m_clCharts->GetItemCount(); i++ )
    {
        //Prepare download queues
        if( m_clCharts->IsChecked(i) )
        {
            if( pPlugIn->m_pChartCatalog->charts->Item(i).NeedsManualDownload() )
            {
                if( wxYES ==
                        wxMessageBox(
                                wxString::Format( _("The selected chart '%s' can't be downloaded automatically, do you want me to open a browser window and download them manually?\n\n \
After downloading the charts, please extract them to %s"), pPlugIn->m_pChartCatalog->charts->Item(i).title.c_str(), pPlugIn->m_pChartSource->GetDir().c_str() ), _("Chart Downloader"), wxYES_NO | wxCENTRE | wxICON_QUESTION ) )
                {
                    wxLaunchDefaultBrowser( pPlugIn->m_pChartCatalog->charts->Item(i).GetManualDownloadUrl() );
                }
            }
            else
            {
                //download queue
                wxURI url(pPlugIn->m_pChartCatalog->charts->Item(i).GetDownloadLocation());
                if( url.IsReference() )
                {
                    wxMessageBox(wxString::Format(_("Error, the URL to the chart (%s) data seems wrong."), url.BuildURI().c_str()), _("Error"));
                    this->Enable();
                    return;
                }
                //construct local file path
                wxString file = pPlugIn->m_pChartCatalog->charts->Item(i).GetChartFilename();
                wxFileName fn;
                fn.SetFullName(file);
                fn.SetPath(cs->GetDir());
                wxString path = fn.GetFullPath();
                if( wxFileExists(path) )
                    wxRemoveFile(path);
                wxString title = pPlugIn->m_pChartCatalog->charts->Item(i).GetChartTitle();

                if( DownloadChart(url.BuildURI(), path, title) )
                {
                    wxFileName fn(path);
                    pPlugIn->ProcessFile(path, fn.GetPath(), true, pPlugIn->m_pChartCatalog->charts->Item(i).GetUpdateDatetime());
                    cs->ChartUpdated( pPlugIn->m_pChartCatalog->charts->Item(i).number, pPlugIn->m_pChartCatalog->charts->Item(i).GetUpdateDatetime().GetTicks() );
                }
            }
        }
        if( cancelled )
            break;
    }
    SetSource(GetSelectedCatalog());
    if( failed_downloads > 0 && !updatingAll )
        wxMessageBox( wxString::Format( _("%d out of %d charts failed to download.\nCheck the list, verify there is a working Internet connection and repeat the operation if needed."), failed_downloads,downloading ),
                _("Chart Downloader"), wxOK | wxICON_ERROR );
    if( (downloading-failed_downloads > 0) && !updatingAll )
        wxMessageBox( _("You have added/updated some of your charts.\nTo make sure OpenCPN knows about them, go to the 'Chart Files' tab and select the 'Scan Charts and Update Database' option."),
                _("Chart Downloader"), wxOK | wxICON_INFORMATION );
}

ChartDldrPanelImpl::~ChartDldrPanelImpl()
{
    wxCurlBase::Shutdown();
    m_lbChartSources->ClearAll();
    ((wxListCtrl *)m_clCharts)->DeleteAllItems();
}

ChartDldrPanelImpl::ChartDldrPanelImpl( chartdldr_pi* plugin, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
    : ChartDldrPanel( parent, id, pos, size, style )
{
    m_bDeleteSource->Disable();
    m_bUpdateChartList->Disable();
    m_lbChartSources->InsertColumn (0, _("Catalog"), wxLIST_FORMAT_LEFT, CATALOGS_NAME_WIDTH);
    m_lbChartSources->InsertColumn (1, _("Released"), wxLIST_FORMAT_LEFT, CATALOGS_DATE_WIDTH);
    m_lbChartSources->InsertColumn (2, _("Local path"), wxLIST_FORMAT_LEFT, CATALOGS_PATH_WIDTH);

    // Add columns
    ((wxListCtrl *)m_clCharts)->InsertColumn(0, _("Title"), wxLIST_FORMAT_LEFT, CHARTS_NAME_WIDTH);
    ((wxListCtrl *)m_clCharts)->InsertColumn(1, _("Status"), wxLIST_FORMAT_LEFT, CHARTS_STATUS_WIDTH);
    ((wxListCtrl *)m_clCharts)->InsertColumn(2, _("Latest"), wxLIST_FORMAT_LEFT, CHARTS_DATE_WIDTH);

    downloadInProgress = false;
    cancelled = false;
    to_download = -1;
    downloading = -1;
    updatingAll = false;
    pPlugIn = plugin;
    m_populated = false;
    failed_downloads = 0;
    m_stCatalogInfo->SetLabel( wxEmptyString );
}

void ChartDldrPanelImpl::OnPaint( wxPaintEvent& event )
{
    if( !m_populated )
    {
        m_populated = true;
        for (size_t i = 0; i < pPlugIn->m_chartSources->GetCount(); i++)
        {
            AppendCatalog(pPlugIn->m_chartSources->Item(i));
        }
        SelectCatalog(pPlugIn->GetSourceId());
        SetSource(pPlugIn->GetSourceId());
    }
    event.Skip();
}

void ChartDldrPanelImpl::DeleteSource( wxCommandEvent& event )
{
    if( !m_lbChartSources->GetSelectedItemCount() )
        return;
    if( wxYES != wxMessageBox(_("Do you really want to remove the chart source?\nThe local chart files will not be removed, but you will not be able to update the charts anymore."),
                                 _("Chart Downloader"), wxYES_NO | wxCENTRE, this) )
        return;
	int ToBeRemoved = GetSelectedCatalog();
	m_lbChartSources->SetItemState(ToBeRemoved,  0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
    pPlugIn->m_chartSources->RemoveAt(ToBeRemoved);
    m_lbChartSources->DeleteItem(ToBeRemoved);
    CleanForm();
    pPlugIn->SetSourceId(-1);
    SelectCatalog(-1);
    pPlugIn->SaveConfig();
    event.Skip();
}

void ChartDldrPanelImpl::AddSource( wxCommandEvent& event )
{
    ChartDldrGuiAddSourceDlg *dialog = new ChartDldrGuiAddSourceDlg(this);
    dialog->SetBasePath(pPlugIn->GetBaseChartDir());
    dialog->CenterOnScreen();
    if( dialog->ShowModal() == wxID_OK )
    {
        ChartSource *cs = new ChartSource(dialog->m_tSourceName->GetValue(), dialog->m_tChartSourceUrl->GetValue(), dialog->m_dpChartDirectory->GetTextCtrlValue());
        pPlugIn->m_chartSources->Add(cs);
        AppendCatalog(cs);
        bool covered = false;
        for( size_t i = 0; i < GetChartDBDirArrayString().GetCount(); i++ )
        {
            if( cs->GetDir().StartsWith((GetChartDBDirArrayString().Item(i))) )
            {
                covered = true;
                break;
            }
        }
        if( !covered )
            wxMessageBox( wxString::Format(_("Path %s seems not to be covered by your configured Chart Directories.\nTo see the charts you have to adjust the configuration on the 'Chart Files' tab."), cs->GetDir().c_str()),
                         _("Chart Downloader") );
        SelectCatalog(m_lbChartSources->GetItemCount() - 1);
        pPlugIn->SaveConfig();
    }
    dialog->Close();
    dialog->Destroy();
    wxDELETE(dialog);
    event.Skip();
}

void ChartDldrPanelImpl::DoEditSource()
{
    if( !m_lbChartSources->GetSelectedItemCount() )
        return;
    int cat = GetSelectedCatalog();
    ChartDldrGuiAddSourceDlg *dialog = new ChartDldrGuiAddSourceDlg(this);
    dialog->SetBasePath(pPlugIn->GetBaseChartDir());
    dialog->SetSourceEdit(pPlugIn->m_chartSources->Item(cat));
    dialog->SetTitle(_("Edit Chart Source"));
    dialog->CenterOnScreen();
    if( dialog->ShowModal() == wxID_OK )
    {
        pPlugIn->m_chartSources->Item(cat)->SetName(dialog->m_tSourceName->GetValue());
        pPlugIn->m_chartSources->Item(cat)->SetUrl(dialog->m_tChartSourceUrl->GetValue());
        pPlugIn->m_chartSources->Item(cat)->SetDir(dialog->m_dpChartDirectory->GetTextCtrlValue());
        
        m_lbChartSources->SetItem(cat, 0, pPlugIn->m_chartSources->Item(cat)->GetName());
        m_lbChartSources->SetItem(cat, 1, _("(Please update first)"));
        m_lbChartSources->SetItem(cat, 2, pPlugIn->m_chartSources->Item(cat)->GetDir());
        wxURI url(pPlugIn->m_chartSources->Item(cat)->GetUrl());
        wxFileName fn(url.GetPath());
        fn.SetPath(pPlugIn->m_chartSources->Item(cat)->GetDir());
        wxString path = fn.GetFullPath();
        if( wxFileExists(path) )
        {
            if( pPlugIn->m_pChartCatalog->LoadFromFile(path, true) )
            {
                m_lbChartSources->SetItem(cat, 0, pPlugIn->m_pChartCatalog->title);
                m_lbChartSources->SetItem(cat, 1, pPlugIn->m_pChartCatalog->GetReleaseDate().Format(_T("%Y-%m-%d %H:%M")));
                m_lbChartSources->SetItem(cat, 2, path);
            }
        }
        bool covered = false;
        for( size_t i = 0; i < GetChartDBDirArrayString().GetCount(); i++ )
        {
            if( pPlugIn->m_chartSources->Item(cat)->GetDir().StartsWith((GetChartDBDirArrayString().Item(i))) )
            {
                covered = true;
                break;
            }
        }
        if( !covered )
            wxMessageBox( wxString::Format(_("Path %s seems not to be covered by your configured Chart Directories.\nTo see the charts you have to adjust the configuration on the 'Chart Files' tab."), pPlugIn->m_chartSources->Item(cat)->GetDir().c_str()),
                         _("Chart Downloader") );

        pPlugIn->SaveConfig();
        SetSource(cat);
    }
    dialog->Close();
    dialog->Destroy();
    wxDELETE(dialog);    
}

void ChartDldrPanelImpl::EditSource( wxCommandEvent& event )
{
    DoEditSource();
    event.Skip();
}

void ChartDldrPanelImpl::OnLeftDClick( wxMouseEvent& event )
{
    DoEditSource();
    event.Skip();
}

bool chartdldr_pi::ProcessFile( const wxString& aFile, const wxString& aTargetDir, bool aStripPath, wxDateTime aMTime )
{
    if( aFile.Lower().EndsWith(_T("zip")) ) //Zip compressed
    {
        bool ret = ExtractZipFiles( aFile, aTargetDir, aStripPath, aMTime, false);
        if( ret )
            wxRemoveFile(aFile);
        else
            wxLogError(_T("chartdldr_pi: Unable to extract: ") + aFile );
        return ret;
    }
    else if( aFile.Lower().EndsWith(_T("rar")) ) //Rar compressed
    {
        bool ret = ExtractRarFiles( aFile, aTargetDir, aStripPath, aMTime, false);
        if( ret )
            wxRemoveFile(aFile);
        else
            wxLogError(_T("chartdldr_pi: Unable to extract: ") + aFile );
        return ret;
    }
    else //Uncompressed
    {
        wxFileName fn(aFile);
        if( fn.GetPath() != aTargetDir ) //We have to move the file somewhere
        {
            if( !wxDirExists(aTargetDir) )
            {
                if( wxFileName::Mkdir(aTargetDir, 0755, wxPATH_MKDIR_FULL) )
                {
                    if( !wxRenameFile(aFile, aTargetDir) )
                        return false;
                }
                else
                    return false;
            }
        }
        wxString name = fn.GetFullName();
        fn.Clear();
        fn.Assign(aTargetDir, name);
        fn.SetTimes(&aMTime, &aMTime, &aMTime);
    }
    return true;
}

bool chartdldr_pi::ExtractRarFiles( const wxString& aRarFile, const wxString& aTargetDir, bool aStripPath, wxDateTime aMTime, bool aRemoveRar )
{
    wxString cmd;
    if( aStripPath )
        cmd = _T("e");
    else
        cmd = _T("x");
    int argc = 5;

    char command[2];
    strncpy(command, (const char*)cmd.mb_str(wxConvUTF8), 1);
    char file[1024];
    strncpy(file, (const char*)aRarFile.mb_str(wxConvUTF8), 1023);
    char target[1024];
    strncpy(target, (const char*)aTargetDir.mb_str(wxConvUTF8), 1023);
    char *argv[] = {const_cast<char *>("unrar"), command, const_cast<char *>("-y"), file, target};
#ifdef _UNIX
    setlocale(LC_ALL,"");
#endif

    InitConsole();
    ErrHandler.SetSignalHandlers(true);

#ifdef SFX_MODULE
    wchar ModuleName[NM];
#ifdef _WIN_ALL
    GetModuleFileName(NULL,ModuleName,ASIZE(ModuleName));
#else
    CharToWide(argv[0],ModuleName,ASIZE(ModuleName));
#endif
#endif

#ifdef _WIN_ALL
    SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT|SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE) && !defined(SHELL_EXT)
    // Must be initialized, normal initialization can be skipped in case of
    // exception.
    bool ShutdownOnClose=false;
#endif

    try 
    {
        CommandData *Cmd=new CommandData;
#ifdef SFX_MODULE
        wcscpy(Cmd->Command,L"X");
        char *Switch=argc>1 ? argv[1]:NULL;
        if (Switch!=NULL && Cmd->IsSwitch(Switch[0]))
        {
            int UpperCmd=etoupper(Switch[1]);
            switch(UpperCmd)
            {
                case 'T':
                case 'V':
                    Cmd->Command[0]=UpperCmd;
                    break;
                case '?':
                    Cmd->OutHelp(RARX_SUCCESS);
                break;
            }
        }
        Cmd->AddArcName(ModuleName);
        Cmd->ParseDone();
#else // !SFX_MODULE
        Cmd->ParseCommandLine(true,argc,argv);
        if (!Cmd->ConfigDisabled)
        {
            Cmd->ReadConfig();
            Cmd->ParseEnvVar();
        }
        Cmd->ParseCommandLine(false,argc,argv);
#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE) && !defined(SHELL_EXT)
        ShutdownOnClose=Cmd->Shutdown;
#endif

        uiInit(Cmd->Sound);
        InitConsoleOptions(Cmd->MsgStream);
        InitLogOptions(Cmd->LogName,Cmd->ErrlogCharset);
        ErrHandler.SetSilent(Cmd->AllYes || Cmd->MsgStream==MSG_NULL);
        ErrHandler.SetShutdown(Cmd->Shutdown);

        Cmd->OutTitle();
        Cmd->ProcessCommand();
        delete Cmd;
    }
    catch (RAR_EXIT ErrCode)
    {
        ErrHandler.SetErrorCode(ErrCode);
    }
    catch (std::bad_alloc&)
    {
        ErrHandler.MemoryErrorMsg();
        ErrHandler.SetErrorCode(RARX_MEMORY);
    }
    catch (...)
    {
        ErrHandler.SetErrorCode(RARX_FATAL);
    }

#if defined(_WIN_ALL) && !defined(SFX_MODULE) && !defined(SHELL_EXT)
    if (ShutdownOnClose)
        Shutdown();
#endif
    ErrHandler.MainExit=true;
    //return ErrHandler.GetErrorCode();
  
    if( aRemoveRar )
        wxRemoveFile(aRarFile);

    return true;
}

bool chartdldr_pi::ExtractZipFiles( const wxString& aZipFile, const wxString& aTargetDir, bool aStripPath, wxDateTime aMTime, bool aRemoveZip )
{
    bool ret = true;

    std::auto_ptr<wxZipEntry> entry(new wxZipEntry());

    do
    {
        //wxLogError(_T("chartdldr_pi: Going to extract '")+aZipFile+_T("'."));
        wxFileInputStream in(aZipFile);

        if( !in )
        {
            wxLogError(_T("Can not open file '")+aZipFile+_T("'."));
            ret = false;
            break;
        }
        wxZipInputStream zip(in);

        while( entry.reset(zip.GetNextEntry()), entry.get() != NULL )
        {
            // access meta-data
            wxString name = entry->GetName();
            if( aStripPath )
            {
                wxFileName fn(name);
                /* We can completly replace the entry path */
                //fn.SetPath(aTargetDir);
                //name = fn.GetFullPath();
                /* Or only remove the first dir (eg. ENC_ROOT) */
                fn.RemoveDir(0);
                name = aTargetDir + wxFileName::GetPathSeparator() + fn.GetFullPath();
            }
            else
            {
                name = aTargetDir + wxFileName::GetPathSeparator() + name;
            }

            // read 'zip' to access the entry's data
            if( entry->IsDir() )
            {
                int perm = entry->GetMode();
                if( !wxFileName::Mkdir(name, perm, wxPATH_MKDIR_FULL) )
                {
                    wxLogError(_T("Can not create directory '") + name + _T("'."));
                    ret = false;
                    break;
                }
            }
            else
            {
                if( !zip.OpenEntry(*entry.get()) )
                {
                    wxLogError(_T("Can not open zip entry '") + entry->GetName() + _T("'."));
                    ret = false;
                    break;
                }
                if( !zip.CanRead() )
                {
                    wxLogError(_T("Can not read zip entry '") + entry->GetName() + _T("'."));
                    ret = false;
                    break;
                }

                wxFileName fn(name);
                if( !fn.DirExists() )
                {
                    if( !wxFileName::Mkdir(fn.GetPath()) )
                    {
                        wxLogError(_T("Can not create directory '") + fn.GetPath() + _T("'."));
                        ret = false;
                        break;
                    }
                }

                wxFileOutputStream file(name);

                if( !file )
                {
                    wxLogError(_T("Can not create file '")+name+_T("'."));
                    ret = false;
                    break;
                }
                zip.Read(file);
                fn.SetTimes(&aMTime, &aMTime, &aMTime);
            }

        }

    }
    while(false);

    if( aRemoveZip )
        wxRemoveFile(aZipFile);

    return ret;
}

ChartDldrGuiAddSourceDlg::ChartDldrGuiAddSourceDlg( wxWindow* parent ) : AddSourceDlg( parent )
{
    p_iconList = new wxImageList(16, 16);
    wxFileName fn;
#ifdef __WXOSX__
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
    fn.SetPath(std_path.GetUserConfigDir());  // should be ~/Library/Preferences
    fn.AppendDir(_T("opencpn"));
#else
    fn.SetPath(*GetpSharedDataLocation());
#endif
    fn.AppendDir(_T("plugins"));
    fn.AppendDir(_T("chartdldr_pi"));
    fn.AppendDir(_T("data"));
    fn.SetFullName(_T("folder215.png"));
    p_iconList->Add(wxBitmap(fn.GetFullPath(), wxBITMAP_TYPE_PNG));
    fn.SetFullName(_T("open182.png"));
    p_iconList->Add(wxBitmap(fn.GetFullPath(), wxBITMAP_TYPE_PNG));
    m_treeCtrl1->AssignImageList(p_iconList);
    m_base_path = wxEmptyString;
    m_last_path = wxEmptyString;
    LoadSources();
    m_nbChoice->SetSelection(0);
    //m_treeCtrl1->ExpandAll();
    if( parent )
        SetSize(parent->GetSize().GetWidth(), GetSize().GetHeight());
}

bool ChartDldrGuiAddSourceDlg::LoadSources()
{
    wxTreeItemId tree = m_treeCtrl1->AddRoot(_T("root"));

    wxFileName fn;
#ifdef __WXOSX__
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
    fn.SetPath(std_path.GetUserConfigDir());  // should be ~/Library/Preferences
    fn.AppendDir(_T("opencpn"));
#else
    fn.SetPath(*GetpSharedDataLocation());
#endif
    fn.AppendDir(_T("plugins"));
    fn.AppendDir(_T("chartdldr_pi"));
    fn.AppendDir(_T("data"));
    fn.SetFullName(_T("chart_sources.xml"));
    if( !fn.FileExists() )
    {
        wxLogMessage( wxString::Format(_T("Error: chartdldr_pi::LoadSources() %s not found!"), fn.GetFullPath().c_str()) );
        return false;
    }
    wxString path = fn.GetFullPath();
    TiXmlDocument * doc = new TiXmlDocument();
    bool ret = doc->LoadFile(path.mb_str(), TIXML_ENCODING_UTF8);
    if( ret )
    {
        TiXmlElement * root = doc->RootElement();
        wxString rootName = wxString::FromUTF8( root->Value() );
        TiXmlNode *child;
        for ( child = root->FirstChild(); child != 0; child = child->NextSibling())
        {
            wxString s = wxString::FromUTF8(child->Value());
            if (s == _T("sections"))
                LoadSections(tree, child);
        }
    }
    doc->Clear();
    wxDELETE(doc);
    return true;
}

bool ChartDldrGuiAddSourceDlg::LoadSections( const wxTreeItemId &root, TiXmlNode *node )
{
    for( TiXmlNode *child = node->FirstChild(); child != 0; child = child->NextSibling() )
    {
        wxString s = wxString::FromUTF8(child->Value());
        if (s == _T("section"))
            LoadSection(root, child);
    }
    return true;
}

bool ChartDldrGuiAddSourceDlg::LoadSection( const wxTreeItemId &root, TiXmlNode *node )
{
    wxTreeItemId item;
    for( TiXmlNode *child = node->FirstChildElement(); child != 0; child = child->NextSibling() )
    {
        wxString s = wxString::FromUTF8(child->Value());
        if( s == _T("name") )
            item = m_treeCtrl1->AppendItem(root, wxString::FromUTF8(child->FirstChild()->Value()), 0, 0);
        if( s == _T("sections") )
            LoadSections(item, child);
        if( s == _T("catalogs") )
            LoadCatalogs(item, child);
    }
    return true;
}

bool ChartDldrGuiAddSourceDlg::LoadCatalogs( const wxTreeItemId &root, TiXmlNode *node )
{
    for( TiXmlNode *child = node->FirstChild(); child != 0; child = child->NextSibling() )
    {
        wxString s = wxString::FromUTF8(child->Value());
        if( s == _T("catalog") )
            LoadCatalog(root, child);
    }
    return true;
}

bool ChartDldrGuiAddSourceDlg::LoadCatalog( const wxTreeItemId &root, TiXmlNode *node )
{
    wxString name, type, location, dir;
    for( TiXmlNode *child = node->FirstChild(); child != 0; child = child->NextSibling() )
    {
        wxString s = wxString::FromUTF8(child->Value());
        if( s == _T("name") )
            name = wxString::FromUTF8(child->FirstChild()->Value());
        if( s == _T("type") )
            type = wxString::FromUTF8(child->FirstChild()->Value());
        if( s == _T("location") )
            location = wxString::FromUTF8(child->FirstChild()->Value());
        if( s == _T("dir") )
            dir = wxString::FromUTF8(child->FirstChild()->Value());
    }
    ChartSource *cs = new ChartSource(name, location, dir);
    m_treeCtrl1->AppendItem(root, name, 1, 1, cs);
    return true;
}

ChartDldrGuiAddSourceDlg::~ChartDldrGuiAddSourceDlg()
{
}

wxString ChartDldrGuiAddSourceDlg::FixPath( wxString path )
{
    wxString sep( wxFileName::GetPathSeparator() );
    wxString s = path;
    s.Replace(_T("/"), sep, true);
    s.Replace(_T(USERDATA), m_base_path);
    s.Replace(sep + sep, sep);
    return s;
}

void ChartDldrGuiAddSourceDlg::OnChangeType( wxCommandEvent& event )
{
    m_treeCtrl1->Enable(m_nbChoice->GetSelection()==0);
    m_tSourceName->Enable(m_nbChoice->GetSelection()==1);
    m_tChartSourceUrl->Enable(m_nbChoice->GetSelection()==1);
}

void ChartDldrGuiAddSourceDlg::OnSourceSelected( wxTreeEvent& event )
{
    wxTreeItemId item = m_treeCtrl1->GetSelection();
    ChartSource *cs = (ChartSource *)(m_treeCtrl1->GetItemData(item));
    if( cs ) {
        m_tSourceName->SetValue(cs->GetName());
        m_tChartSourceUrl->SetValue(cs->GetUrl());
        if( m_dpChartDirectory->GetPath() == m_last_path )
        {
            m_dpChartDirectory->SetPath(FixPath(cs->GetDir()));
            m_last_path = m_dpChartDirectory->GetPath();
        }
    }
    event.Skip();
}

void ChartDldrGuiAddSourceDlg::SetSourceEdit( ChartSource* cs )
{
    m_nbChoice->SetSelection(1);
    m_tChartSourceUrl->Enable();
    m_treeCtrl1->Disable();
    m_tSourceName->SetValue(cs->GetName());
    m_tChartSourceUrl->SetValue(cs->GetUrl());
    m_dpChartDirectory->SetPath(FixPath(cs->GetDir()));
}

ChartDldrPrefsDlgImpl::ChartDldrPrefsDlgImpl( wxWindow* parent ) : ChartDldrPrefsDlg( parent )
{
}

ChartDldrPrefsDlgImpl::~ChartDldrPrefsDlgImpl()
{    
}

void ChartDldrPrefsDlgImpl::SetPath( const wxString path )
{
    //if( !wxDirExists(path) )
        //if( !wxFileName::Mkdir(path, 0755, wxPATH_MKDIR_FULL) )
        //{
        //    wxMessageBox(wxString::Format(_("Directory %s can't be created."), m_dpDefaultDir->GetTextCtrlValue().c_str()), _("Chart Downloader"));
        //    return;
        //}
    m_dpDefaultDir->SetPath(path);
}

void ChartDldrPrefsDlgImpl::GetPreferences( bool &preselect_new, bool &preselect_updated, bool &bulk_update )
{
    preselect_new = m_cbSelectNew->GetValue();
    preselect_updated = m_cbSelectUpdated->GetValue();
    bulk_update = m_cbBulkUpdate->GetValue();
}
void ChartDldrPrefsDlgImpl::SetPreferences( bool preselect_new, bool preselect_updated, bool bulk_update )
{
    m_cbSelectNew->SetValue(preselect_new);
    m_cbSelectUpdated->SetValue(preselect_updated);
    m_cbBulkUpdate->SetValue(bulk_update);
}

void ChartDldrGuiAddSourceDlg::OnOkClick( wxCommandEvent& event )
{
    wxString msg = wxEmptyString;
    
    if( m_nbChoice->GetSelection()==0 )
    {
        wxTreeItemId item = m_treeCtrl1->GetSelection();
        if( m_treeCtrl1->GetSelection().IsOk() )
        {
            ChartSource *cs = (ChartSource *)(m_treeCtrl1->GetItemData(item));
            if ( !cs )
                msg += _("You must select one of the predefined chart sources or create one of your own.\n");
        }
        else
            msg += _("You must select one of the predefined chart sources or create one of your own.\n");
    }
    if( m_nbChoice->GetSelection()==1 && m_tSourceName->GetValue() == wxEmptyString )
        msg += _("The chart source must have a name.\n");
    wxURI url(m_tChartSourceUrl->GetValue());
    if( m_nbChoice->GetSelection()==1 && ( m_tChartSourceUrl->GetValue() == wxEmptyString || !ValidateUrl(m_tChartSourceUrl->GetValue()) ) )
        msg += _("The chart source must have a valid URL.\n");
    if( m_dpChartDirectory->GetPath() == wxEmptyString )
        msg += _("You must select a local folder to store the charts.\n");
    else
        if( !wxDirExists(m_dpChartDirectory->GetTextCtrlValue()) )
            if( !wxFileName::Mkdir(m_dpChartDirectory->GetTextCtrlValue(), 0755, wxPATH_MKDIR_FULL) )
                msg += wxString::Format(_("Directory %s can't be created."), m_dpChartDirectory->GetTextCtrlValue().c_str()) + _T("\n");
    
    if( msg != wxEmptyString )
        wxMessageBox( msg, _("Chart source definition problem"), wxOK | wxCENTRE | wxICON_ERROR );
    else
        event.Skip();
}

void ChartDldrPrefsDlgImpl::OnOkClick( wxCommandEvent& event )
{
    if( !wxDirExists(m_dpDefaultDir->GetTextCtrlValue()) )
        if( !wxFileName::Mkdir(m_dpDefaultDir->GetTextCtrlValue(), 0755, wxPATH_MKDIR_FULL) )
        {
            wxMessageBox(wxString::Format(_("Directory %s can't be created."), m_dpDefaultDir->GetTextCtrlValue().c_str()), _("Chart Downloader"));
            return;
        }
    event.Skip();
}


bool ChartDldrGuiAddSourceDlg::ValidateUrl( const wxString Url, bool catalog_xml )
{
    wxRegEx re;
    if( catalog_xml )
        re.Compile( _T("^https?\\://[a-zA-Z0-9\\./_-]*\\.[xX][mM][lL]$") ); //TODO: wxRegEx sucks a bit, this RE is way too naive
    else
        re.Compile( _T("^https?\\://[a-zA-Z0-9\\./_-]*$") ); //TODO: wxRegEx sucks a bit, this RE is way too naive
    return re.Matches(Url);
}
