/***************************************************************************
 * Project:  OpenCPN
 * Purpose:  Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Mauro Calvi                                     *
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
 ***************************************************************************/

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/event.h"
    #include "wx/string.h"
#endif //precompiled headers

#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <wx/aui/aui.h>
#include <wx/utils.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#include <typeinfo>
#include "squiddio_pi.h"
#include "icons.h"

#include "PoiMan.h"
#include "Poi.h"
#include "NavObjectCollection.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(LayerList);
WX_DEFINE_LIST(HyperlinkList );
WX_DEFINE_LIST(Plugin_HyperlinkList);

#define ONLINE_CHECK_RETRY 10

// the class factories, used to create and destroy instances of the PlugIn
//

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
    return new squiddio_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p){
    delete p;
}

// these variables are shared with NavObjectCollection
PoiMan *pPoiMan;

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

squiddio_pi::squiddio_pi( void *ppimgr )
    :opencpn_plugin_110( ppimgr ) // constructor initialization
{
    // Create the PlugIn icons
    initialize_images();
}

squiddio_pi::~squiddio_pi( void )
{
    delete _img_marina_grn;
    delete _img_anchor_blu;
    delete _img_club_pur ;
    delete _img_fuelpump_red;
    delete _img_pier_yel;
    delete _img_ramp_azu;
}

int squiddio_pi::Init(void)
{
    AddLocaleCatalog( _T("opencpn-squiddio_pi"));
    //      printf("squiddio_pi Init()\n");
    wxLogMessage( _T("squiddio_pi: Init()") );

    // Get a pointer to the opencpn display canvas, to use as a parent for windows created
    m_parent_window = GetOCPNCanvasWindow();
    
    last_online_chk = 0;

    wxMenu dummy_menu;

    wxMenuItem *pmi = new wxMenuItem(&dummy_menu, -1,
            _("Show local sQuiddio destinations"));
    m_show_id = AddCanvasContextMenuItem(pmi, this);
    SetCanvasContextMenuItemViz(m_show_id, false);

    wxMenuItem *pmih = new wxMenuItem(&dummy_menu, -1,
            _("Hide local sQuiddio destinations"));
    m_hide_id = AddCanvasContextMenuItem(pmih, this);
    SetCanvasContextMenuItemViz(m_hide_id, false);

    wxMenuItem *updi = new wxMenuItem(&dummy_menu, -1,
            _("Download local sQuiddio destinations"));
    m_update_id = AddCanvasContextMenuItem(updi, this);
    SetCanvasContextMenuItemViz(m_update_id, true);

    wxMenuItem *repi = new wxMenuItem(&dummy_menu, -1,
            _("Report a Destination at this location"));
    m_report_id = AddCanvasContextMenuItem(repi, this);
    SetCanvasContextMenuItemViz(m_report_id, true);

    AddCustomWaypointIcon(_img_marina_grn,  _T("marina_grn"),   _T("Marina"));
    AddCustomWaypointIcon(_img_anchor_blu,  _T("anchor_blu"),   _T("Anchorage/Buoys"));
    AddCustomWaypointIcon(_img_club_pur,    _T("club_pur"),     _T("Yacht Club"));
    AddCustomWaypointIcon(_img_fuelpump_red,_T("fuelpump_red"), _T("Fuel Station"));
    AddCustomWaypointIcon(_img_pier_yel,    _T("pier_yel"),     _T("Dock/Pier"));
    AddCustomWaypointIcon(_img_ramp_azu,    _T("ramp_azu"),     _T("Boat Ramp"));

    pLayerList  = new LayerList;
    pPoiMan     = new PoiMan;
    link        = new Plugin_Hyperlink;

    m_pconfig = GetOCPNConfigObject();
    LoadConfig();

    layerdir = *GetpPrivateApplicationDataLocation();
    layerdir += wxFileName::GetPathSeparator();
#ifdef __WXOSX__
    layerdir += _T("opencpn/plugins/squiddio");
#else
    layerdir += _T("squiddio");
#endif
    if( !wxDir::Exists( layerdir ) )
        wxFileName::Mkdir(layerdir);

    if( wxDir::Exists( layerdir ) ) {
        wxString laymsg;
        laymsg.Printf( wxT("squiddio_pi: getting .gpx layer files from: %s"), layerdir.c_str() );
        wxLogMessage( laymsg );

        LoadLayers(layerdir);

        Layer * l;
        LayerList::iterator it;
        int index = 0;
        for (it = (*pLayerList).begin(); it != (*pLayerList).end(); ++it, ++index) {
            l = (Layer *) (*it);
            l->SetVisibleNames( false );
            RenderLayerContentsOnChart(l);
        }
    }

    return (
    INSTALLS_CONTEXTMENU_ITEMS |
    WANTS_CURSOR_LATLON |
    USES_AUI_MANAGER);
}


bool squiddio_pi::DeInit(void)
{
    /*
    m_AUImgr->DetachPane(m_pdemo_window);

    if (m_pdemo_window) {
        m_pdemo_window->Close();
//          m_pdemo_window->Destroy(); //Gives a Segmentation fault
    }
    */
    RemoveCanvasContextMenuItem(m_show_id);
    RemoveCanvasContextMenuItem(m_hide_id);
    RemoveCanvasContextMenuItem(m_update_id);
    RemoveCanvasContextMenuItem(m_report_id);
    
    LayerList::iterator it;
    int index = 0;
    for (it = (*pLayerList).begin(); it != (*pLayerList).end(); ++it, ++index) {
        Layer * l = (Layer *) (*it);
        l->SetVisibleOnChart( false );
        RenderLayerContentsOnChart( l, false );
        pLayerList->DeleteObject( l );
    }
    
    RequestRefresh(m_parent_window);

    delete pLayerList;
    delete pPoiMan;
    delete link;
    return true;
}

bool squiddio_pi::LoadConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;
#ifdef __WXOSX__
    pConf->SetPath ( _T( "/PlugIns/libsquiddio_pi" ) );
#else
    pConf->SetPath ( _T( "/PlugIns/libsquiddio_pi.so" ) );
#endif
    pConf->Read( _T ( "VisibleLayers" ), &g_VisibleLayers );
    pConf->Read( _T ( "InvisibleLayers" ), &g_InvisibleLayers );
    return true;
}

bool squiddio_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;
#ifdef __WXOSX__
    pConf->SetPath ( _T( "/PlugIns/libsquiddio_pi" ) );
#else
    pConf->SetPath ( _T( "/PlugIns/libsquiddio_pi.so" ) );
#endif
    pConf->Write ( _T ( "VisibleLayers" ), g_VisibleLayers );
    pConf->Write ( _T ( "InvisibleLayers" ), g_InvisibleLayers );

    return true;
}

bool squiddio_pi::LoadLayers(wxString &path)
{
    wxArrayString file_array;
    wxDir dir;
    Layer *l;
    dir.Open( path );
    if( dir.IsOpened() ) {
        wxString filename;
        bool cont = dir.GetFirst( &filename );
        while( cont )
        {
            file_array.Clear();
            filename.Prepend( wxFileName::GetPathSeparator() );
            filename.Prepend( path );
            wxFileName f( filename );
            size_t nfiles = 0;
            if( f.GetExt().IsSameAs( wxT("gpx") ) )
                file_array.Add( filename); // single-gpx-file layer
            else{
                wxDir dir( filename );
                if( dir.IsOpened() ){
                    nfiles = dir.GetAllFiles( filename, &file_array, wxT("*.gpx") );      // layers subdirectory set
                }
            }

            if( file_array.GetCount() )
            {
                l = new Layer();
                l->m_LayerID = ++g_LayerIdx;
                l->m_LayerFileName = file_array[0];
                if( file_array.GetCount() <= 1 )
                    wxFileName::SplitPath( file_array[0], NULL, NULL, &( l->m_LayerName ), NULL, NULL );
                else
                    wxFileName::SplitPath( filename, NULL, NULL, &( l->m_LayerName ), NULL, NULL );

                bool bLayerViz = false;

                if( g_VisibleLayers.Contains( l->m_LayerName ) )
                    bLayerViz = true;

                l->m_bIsVisibleOnChart = bLayerViz;

                wxString laymsg;
                laymsg.Printf( wxT("squiddio_pi: new layer %d: %s"), l->m_LayerID, l->m_LayerName.c_str() );
                wxLogMessage( laymsg );

                pLayerList->Insert( l );

                //  Load the entire file array as a single layer
                for( unsigned int i = 0; i < file_array.GetCount(); i++ ) {
                    wxString file_path = file_array[i];
                    if( ::wxFileExists( file_path ) ) {
                        LoadLayerItems( file_path, l ,bLayerViz);
                    }
                }
            }
            cont = dir.GetNext( &filename );
        }
    }
    return true;

}

Layer * squiddio_pi::LoadLayer(wxString file_path, wxString region)
{
    Layer * l = new Layer();
    if( ::wxFileExists( file_path ) )
    {
        l->m_LayerID = ++g_LayerIdx;
        l->m_LayerName = _T("SQ_")+region;
        l->m_LayerFileName = file_path;
        l->m_bIsVisibleOnChart = true;
        pLayerList->Insert( l );

        LoadLayerItems(file_path, l, true);
    }
    return l;
}

bool squiddio_pi::LoadLayerItems(wxString &file_path, Layer *l, bool show)
{
    NavObjectCollection1 *pSet = new NavObjectCollection1;
    pSet->load_file(file_path.fn_str());
    long nItems = pSet->LoadAllGPXObjectsAsLayer(l->m_LayerID, show);

    l->m_NoOfItems += nItems;
#ifdef __WXOSX__
    wxString objmsg;
    objmsg.Printf( wxT("squiddio_pi: loaded GPX file %s with %ld items."), file_path.c_str(), nItems );
    wxLogMessage( objmsg );
#else
    wxString objmsg;
    objmsg.Printf( wxT("squiddio_pi: loaded GPX file %s with %d items."), file_path.c_str(), nItems );
    wxLogMessage( objmsg );
#endif
    delete pSet;
    return nItems > 0;
}

void squiddio_pi::RenderLayerContentsOnChart( Layer *layer, bool save_config )
{
    // Process POIs in this layer

    wxPoiListNode *node = pPoiMan->GetWaypointList()->GetFirst();

    while( node )
    {
        Poi *rp = node->GetData();
        if( rp && ( rp->m_LayerID == layer->m_LayerID ) ) {
            rp->SetVisible( layer->IsVisibleOnChart() );
            rp->SetNameShown( false );
            if (layer->IsVisibleOnChart())
                ShowPOI(rp);
            else
                HidePOI(rp);
        }
        node = node->GetNext();
    }

    if (layer->IsVisibleOnChart())
    {
        if (!g_VisibleLayers.Contains(layer->m_LayerName))
            g_VisibleLayers.Append(layer->m_LayerName+_T(";"));
        g_InvisibleLayers.Replace(layer->m_LayerName+_T(";"),wxEmptyString);
    } else{
        if (!g_InvisibleLayers.Contains(layer->m_LayerName))
            g_InvisibleLayers.Append(layer->m_LayerName+_T(";"));
        g_VisibleLayers.Replace(layer->m_LayerName+_T(";"),wxEmptyString);
    }
    RequestRefresh(m_parent_window);
    if (save_config)
        SaveConfig();
}

bool squiddio_pi::ShowPOI(Poi * wp)
{
    double lat = wp->m_lat;
    double lon = wp->m_lon;
    wxString name = wp->GetName();
    wxString m_GUID = wp->m_GUID;
    wxString m_iconname = wp->m_IconName;

    PlugIn_Waypoint * pPoint = new PlugIn_Waypoint(lat, lon, m_iconname, name, m_GUID);
    pPoint->m_MarkDescription = wp->m_MarkDescription;

    wxHyperlinkListNode *linknode = wp->m_HyperlinkList->GetFirst();
    wp_link = linknode->GetData();
    link->Link = wp_link->Link;
    link->DescrText = wp_link->DescrText;
    link->Type = wxEmptyString;

    pPoint->m_HyperlinkList = new Plugin_HyperlinkList;
    pPoint->m_HyperlinkList->Insert( link );

    bool added = AddSingleWaypoint(pPoint, false);
    return added;
}

bool squiddio_pi::HidePOI(Poi * wp)
{
    return DeleteSingleWaypoint( wp->m_GUID );
}

void squiddio_pi::UpdateAuiStatus(void)
{
    //    This method is called after the PlugIn is initialized
    //    and the frame has done its initial layout, possibly from a saved wxAuiManager "Perspective"
    //    It is a chance for the PlugIn to syncronize itself internally with the state of any Panes that
    //    were added to the frame in the PlugIn ctor.

    //    We use this callback here to keep the context menu selection in sync with the window state

    SetCanvasContextMenuItemViz(m_hide_id, false);
    SetCanvasContextMenuItemViz(m_show_id, false);

    SetCanvasContextMenuItemViz(m_update_id, IsOnline() );
    SetCanvasContextMenuItemViz(m_report_id, IsOnline() );

}

void squiddio_pi::SetCursorLatLon(double lat, double lon)
{
        m_cursor_lon = lon;
        m_cursor_lat = lat;

        int lat_sector = abs( m_cursor_lat/5);
        int lon_sector = abs( m_cursor_lon/5);
        wxString lat_quadrant = (m_cursor_lat > 0) ? (_T("N")) : (_T("S"));
        wxString lon_quadrant = (m_cursor_lon > 0) ? (_T("E")) : (_T("W"));
        local_region =  lat_quadrant << wxString::Format(wxT("%02i"),lat_sector)<< lon_quadrant << wxString::Format(wxT("%02i"),lon_sector);

        local_sq_layer = GetLocalLayer();
        if (local_sq_layer != NULL ){
            SetCanvasContextMenuItemViz(m_hide_id, local_sq_layer->IsVisibleOnChart() );
            SetCanvasContextMenuItemViz(m_show_id, !local_sq_layer->IsVisibleOnChart());
        }else{
            SetCanvasContextMenuItemViz(m_hide_id, false);
            SetCanvasContextMenuItemViz(m_show_id, false);
        }
}

void squiddio_pi::OnContextMenuItemCallback(int id)
{
    wxLogMessage(_T("squiddio_pi: OnContextMenuCallBack()"));

    wxString request_region = local_region; // fixes the cursor's hover region at time of request so that intervening mouse movements do not alter the layer name that will be created
    Layer * request_layer = local_sq_layer; // fixes the layer at time of request so that intervening mouse movements do not alter the layer name that will be created

    if (id == m_show_id || id == m_hide_id)
    {
        request_layer->SetVisibleOnChart( !request_layer->IsVisibleOnChart() );
        RenderLayerContentsOnChart( request_layer );
        wxLogMessage(_T("squiddio_pi: toggled layer: ")+request_layer->m_LayerName);
    }
    else if (id == m_update_id )
    {
        wxString layerContents;
        Layer * new_layer = NULL;

        if (IsOnline())
            layerContents = DownloadLayer();

        wxString gpxFilePath = layerdir;
        appendOSDirSlash( &gpxFilePath );
        gpxFilePath.Append(_T("SQ_")+request_region+_T(".gpx"));

        if (layerContents.length() != 0 )
        {
            if (layerContents.length()> 400 )
            {
                isLayerUpdate = SaveLayer(layerContents, gpxFilePath);
                if (isLayerUpdate && request_layer != NULL ){
                    // hide and delete the current layer
                    request_layer ->SetVisibleOnChart( false );
                    RenderLayerContentsOnChart(request_layer);
                    pLayerList->DeleteObject( request_layer );
                }
                new_layer = LoadLayer(gpxFilePath, request_region);
                new_layer->SetVisibleNames( false );
                RenderLayerContentsOnChart(new_layer);

                if (isLayerUpdate)
                {
                    wxMessageBox(_("Local destinations have been updated"));
                }
            } else {
                wxMessageBox(_("No destinations available for the region"));
            }
        } else {
            wxMessageBox(_("Server not responding. Check your Internet connection"));
        }
    } else if (id == m_report_id)
    {
        wxString url_path = _T("http://squidd.io/locations/new?lat=");
        url_path.Append(wxString::Format(wxT("%f"), m_cursor_lat) << _T("&lon=") << wxString::Format(wxT("%f"), m_cursor_lon));
        if (!IsOnline() || !wxLaunchDefaultBrowser(url_path))
            wxMessageBox(_("Could not launch default browser. Check your Internet connection"));
    }
}

wxString squiddio_pi::DownloadLayer()
{
    // --------------------------------- setup http GET request
    int cnt = 0;
    wxString res;
    wxHTTP get;
    get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
    get.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...

    while (!get.Connect(_T("squidd.io")))
    {
        if (cnt > 10)
            break;
        wxSleep(1);
        cnt++;
    }

    wxApp::IsMainLoopRunning();

    wxString url_path = _T("/places/download_xml_layers.xml?region=")+local_region;
    //wxLogMessage(url_path);

    wxInputStream *httpStream = get.GetInputStream(url_path );

    if (get.GetError() == wxPROTO_NOERR)
    {
        wxStringOutputStream out_stream(&res);
        httpStream->Read(out_stream);
    }
    else
    {
        wxMessageBox(_("Squiddio_pi: unable to connect to host"));
    }
    wxDELETE(httpStream);
    get.Close();
    return res;
}

bool squiddio_pi::SaveLayer(wxString layerStr, wxString file_path)
{
    // write file to the squiddio directory
    bool isUpdate = wxFile::Exists(file_path);
    wxFile gpxFile;

    if (isUpdate)
        wxLogMessage(_T("squiddio_pi: updating existing layer file"));
    else
        wxLogMessage(_T("squiddio_pi: creating new layer file"));

    if (gpxFile.Create( file_path , true )){
           gpxFile.Write(layerStr);
           gpxFile.Close();
    }
    else
        wxLogMessage(_T("squiddio_pi: unable to create layer file"));
    return isUpdate;
}

bool squiddio_pi::IsOnline()
{
    if (wxDateTime::GetTimeNow() < last_online_chk + ONLINE_CHECK_RETRY)
        return last_online;
    wxHTTP get;
    get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
    get.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...
    int cnt = 0;

    while (!get.Connect(_T("yahoo.com")))
    {
        if (cnt > 5)
            last_online = false;
            return false;
        wxSleep(1);
        cnt++;
    }
    get.Close();
    last_online = true;
    return true;
}

Layer * squiddio_pi::GetLocalLayer()
{
    LayerList::iterator it;
    int index = 0;
    wxString layer_name = _T("SQ_") + local_region;
    Layer *lay = NULL;

    for (it = (*pLayerList).begin(); it != (*pLayerList).end(); ++it, ++index)
    {
        Layer * l = (Layer *) (*it);
        if(l->m_LayerName== layer_name){
            lay = l;
            break;
        }
    }
    return lay;
}

void squiddio_pi::appendOSDirSlash( wxString* pString )
{
    wxChar sep = wxFileName::GetPathSeparator();
    if( pString->Last() != sep ) pString->Append( sep );
}

int squiddio_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int squiddio_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int squiddio_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int squiddio_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}
wxBitmap *squiddio_pi::GetPlugInBitmap()
{
    return _img_marina_grn;
}

wxString squiddio_pi::GetCommonName()
{
    return _("Squiddio");
}

wxString squiddio_pi::GetShortDescription()
{
    return _("Squiddio for OpenCPN");
}

wxString squiddio_pi::GetLongDescription()
{
    return _("User-sourced database of sailing destinations.\n\n\
To download destinations for a desired region (requires Internet connection):\n\
* Position cursor on area where you want to view destinations and right click mouse\n\
* Select 'Download local sQuiddio destinations' from context-sensitive menu.\n\n\
Destinations appear as OpenCPN waypoints: \n\
* Right-click on waypoint for link to sQuiddio's destination page. \n\
* Follow link to rate destination and add comments online.\n\n\
Other menu options: \n\
* Toggle visibility for local destinations on/off \n\
* Submit a new destination (requires Internet connection and free user account)");
}

bool squiddio_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    return false;
}

bool squiddio_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    return false;

}

int squiddio_pi::GetToolbarToolCount(void)
{
    return 1;
}

void squiddio_pi::ShowPreferencesDialog(wxWindow* parent)
{
}

void squiddio_pi::OnToolbarToolCallback(int id)
{
}

void squiddio_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
}

void squiddio_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
}
