/* *************************************************************************
 * Project:  OpenCPN
 * Purpose:  Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2020 by Mauro Calvi                                     *
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
#endif //precompiled headers

#include "squiddio_pi.h"
#include <wx/fileconf.h>
#include <wx/fontdlg.h>
#include "sJSON.h"
#include "version.h"

#include "wxJSON/jsonwriter.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/filesys.h>

extern squiddio_pi        *g_squiddio_pi;


bool squiddio_pi::UnzipFile(const wxString& aZipFile, const wxString& aTargetDir) {
    
//     https://wiki.wxwidgets.org/WxZipInputStream
    bool ret = true;

    std::unique_ptr<wxZipEntry> entry(new wxZipEntry());

    do {

        wxFileInputStream in(aZipFile);

        if (!in) {
            wxLogMessage(_T("squiddio_pi: Cannot open ZIP file '") + aZipFile + _T("'."));
            ret = false;
            break;
        }

        wxZipInputStream zip(in);

        while (entry.reset(zip.GetNextEntry()), entry.get() != NULL) {
            // access meta-data
            wxString name = entry->GetName();

            int pos = name.Find(".kap");
            if (pos >0 )
                name = aTargetDir + wxFileName::GetPathSeparator() + name;
            else
                // save non kap files in squiddio directory
                name = layerdir + wxFileName::GetPathSeparator() + name;


            // read 'zip' to access the entry's data
            if (entry->IsDir()) {
                int perm = entry->GetMode();
                wxFileName::Mkdir(name, perm, wxPATH_MKDIR_FULL);
            }
            else {
                zip.OpenEntry(*entry.get());
                if (!zip.CanRead()) {
                    wxLogError(_T("Squiddio_pi: Cannot read ZIPped entry '") + entry->GetName() + _T("'."));
                    ret = false;
                    break;
                }
                wxFileOutputStream file(name);

                if (!file) {
                    wxLogError(_T("Squiddio_pi: Cannot create file '") + name + _T("'."));
                    ret = false;
                    break;
                }
                zip.Read(file);
            }
        }

    } while (false);

    return ret;
}


bool squiddio_pi::ProcessZipFile(wxString chart_dir, wxString tmp_file)
{
    bool ret;
    
    wxFile f( tmp_file );
    wxFileOffset zipped_file_length = f.Length();
    if (zipped_file_length < 1000 ) {
        // not a valid zipped file, i.e. squiddio returned error condition 
        wxString res = wxEmptyString;
        f.ReadAll( &res );
        wxLogMessage(_T("squiddio_pi: error in response file: ") + res) ;
        wxMessageBox(res);
        ret = false;
    } else {
        bool unzip;
        unzip = UnzipFile(tmp_file, chart_dir);

        if (unzip == true) {
            wxLogMessage(wxString::Format(_T("squiddio_pi: extracted charts to: %s"), chart_dir));
            wxRemoveFile(tmp_file); //remove the zip file
            ret = true;
        }
        else {
            wxLogMessage(wxString::Format(_T("squiddio_pi: unable to extract charts to: %s"), chart_dir));
            ret = false;
        }
    }
    return ret;
}

bool squiddio_pi::IsPOIinLayer(int layer_id) 
{
    // POI belongs to visible layer?
    Layer * l;
    LayerList::iterator it;
    int index = 0;
    bool visible_layer = false;
    for (it = (*pLayerList).begin(); it != (*pLayerList).end();
            ++it, ++index) {
        l = (Layer *) (*it);
        if (l->m_LayerID == layer_id) {
            visible_layer = true;
            break;
        }
    }
    return visible_layer;
}


void squiddio_pi::DownloadSatImages() {

    wxLogMessage(_T("squiddio_pi: download sat. image: "));

    double center_lat = m_vp->clat;
    double center_lon = m_vp->clon;
    double max_lat    = m_vp->lat_max;
    double max_lon    = m_vp->lon_max;


    wxPoiListNode *node = pPoiMan->GetWaypointList()->GetFirst();
    PoiList temp_list;
    wxString id_str;

    double poi_lat, poi_lon;
    wxString m_GUID;
    wxArrayString guid_array;
    const wxChar * sep_dash = _T("-"), * sep_undersc = _T("_"), * sep_comma = _T(",");
    int poi_count = 0;
    

    while (node) {
        // is POI candidate for download?
        Poi *rp = node->GetData();
        poi_lat = rp->GetLatitude();
        poi_lon = rp->GetLongitude();

        if ( poi_lat > m_vp->lat_min && poi_lat < m_vp->lat_max && poi_lon > m_vp->lon_min && poi_lon < m_vp->lon_max 
            && ShowType(rp) && IsPOIinLayer(rp->m_LayerID) ) {
          temp_list.Append( rp );
          m_GUID = rp->m_GUID;
          guid_array = wxSplit(m_GUID, * sep_dash);
          id_str += ((poi_count >0 ) ? sep_undersc : wxEmptyString) + guid_array[3] ; 
          poi_count += 1;
        }
        node = node->GetNext();
    }

    if (poi_count <= 0) 
        wxMessageBox(_("No sQuiddio POIs in viewport. Download local POIs, zoom out or pan to a different area."));
    else if (poi_count > 20)
        wxMessageBox(_("Too many sQuiddio POIs in viewport. Zoom in to reduce the number of POIs to less than 20"));
    else {
        wxString poi_count_str = wxString::Format(wxT("%i"),poi_count);
//         int zoom_levels = g_ZoomLevels.Freq(* _T(",")) + 1;
        int num_zooms = wxSplit(g_ZoomLevels, * sep_comma).GetCount();

        wxString mess_prompt = _("You are about to download satellite maps for ")+ poi_count_str + _(" POI") + ((poi_count > 1) ? _("s") : _T(""));
        mess_prompt +=  _(" at zoom level") +  ((num_zooms > 1) ? _("s ") : _T(" ")) + g_ZoomLevels;
        if (g_DownloadVPMap)
            mess_prompt += _(", plus one map for the entire viewport.");
        float download_size = (poi_count * num_zooms + ((g_DownloadVPMap) ? 1 : 0) ) * 0.82;
        wxString download_size_str = wxString::Format(wxT("%.0f"), download_size);
        mess_prompt += _("\n\nThe estimated compressed file size is approximately ") + download_size_str + " MB. ";
        if (download_size > 20.0)
            mess_prompt += _("\nZoom in to a lower number of visible POIs to reduce download time, or reduce the number of zoom levels. ");
        mess_prompt += _("\n\nSee settings in your squiddio plugin -> Downloads tab to change your zoom levels");
        if (g_DownloadVPMap)
            mess_prompt += _(" or exclude the viewport map.");

        wxMessageDialog mess (NULL, mess_prompt, _("Goodle Maps Downloader"), wxOK | wxCANCEL);
        if( mess.ShowModal() == wxID_CANCEL )
            return;

        wxString tmp_file = wxFileName::CreateTempFileName(_T("squiddio_pi"));
        wxString chart_dir = g_BaseChartDir + wxFileName::GetPathSeparator();

        wxString zoom_param = g_ZoomLevels;
        zoom_param.Replace(sep_comma, sep_undersc);

        wxString url_path = g_DomainName + _T("/places/") + id_str + _T("/download_kap_files");
        url_path.Append(_T("?zooms=") + zoom_param );  // gmaps zoom levels
        if (g_DownloadVPMap) {
            url_path.Append(_T("&lat=") + wxString::Format(wxT("%f"), center_lat) );  // center latitude
            url_path.Append(_T("&lon=") + wxString::Format(wxT("%f"), center_lon) );  // center longitude
            url_path.Append(_T("&m_lat=") + wxString::Format(wxT("%f"), max_lat));    // max latitude of viewport
            url_path.Append(_T("&m_lon=") + wxString::Format(wxT("%f"), max_lon));    // max longitude of viewport
        }
        url_path.Append(_T("&compressed=true"));
        url_path.Append(_T("&squiddio_id=")+g_Email+_T("&api_key="+g_ApiKey));
        url_path.Append(_T("&source=ocpn_plugin&version=") + g_UrlVersion); // plugin identifiers

        wxString download_message = wxString::Format(wxT("Downloading %i images... "), poi_count * num_zooms);
        OCPN_DLStatus result = OCPN_downloadFile(url_path , tmp_file, _("Downloading"), download_message, wxNullBitmap, m_parent_window, OCPN_DLDS_ELAPSED_TIME|OCPN_DLDS_AUTO_CLOSE|OCPN_DLDS_SIZE|OCPN_DLDS_SPEED|OCPN_DLDS_REMAINING_TIME, 10
        );

        if( result == OCPN_DL_NO_ERROR )
        {
            if (ProcessZipFile(chart_dir, tmp_file)) {
                // UpdateChartDBInplace doesn't seem to update the chart group (with force param set to true or false.)
                // The downloaded gmaps will not appear if the chart group is currently selected, unless user performs a manual
                // chart database update. They will, however, appear if "All Active Charts" is selected under Chart Groups
                bool updated = UpdateChartDBInplace(GetChartDBDirArrayString(), false, true);
                if (!updated) 
                    wxMessageBox(_("Unable to update the chart database"));
                else 
                    wxLogMessage(_T("squiddio_pi: added KAP files to database:") + tmp_file);
            }    
        }
        else
        {
            wxLogMessage(_T("squiddio_pi: unable to connect to host"));
        }
    }
}

