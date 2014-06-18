/***************************************************************************
 *
 *
 * Project:  OpenCPN
 * Purpose:  PlugIn Manager Object
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
 ***************************************************************************/

#ifndef _PLUGINMGR_H_
#define _PLUGINMGR_H_

#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/dynlib.h>

#ifdef ocpnUSE_GL
#include <wx/glcanvas.h>
#endif

#include "ocpn_plugin.h"
#include "chart1.h"                 // for MyFrame
#include "chcanv.h"                 // for ViewPort
#include "datastream.h"             // for GenericPosDat
#include "OCPN_Sound.h"
#include "s52s57.h"
#include "s57chart.h"               // for Object list

//For widgets...
#include "wx/hyperlink.h"
#include <wx/choice.h>
#include <wx/tglbtn.h>
#include <wx/bmpcbox.h>

//    Include wxJSON headers
//    We undefine MIN/MAX so avoid warning of redefinition coming from
//    json_defs.h
//    Definitions checked manually, and are identical
#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#include "wx/json_defs.h"
#include "wx/jsonwriter.h"

//    Assorted static helper routines

PlugIn_AIS_Target *Create_PI_AIS_Target(AIS_Target_Data *ptarget);

class PluginListPanel;
class PluginPanel;

typedef struct {
    wxString name;      // name of the plugin
    int version_major;  // major version
    int version_minor;  // minor version
    bool hard;          // hard blacklist - if true, don't load it at all, if false, load it and just warn the user
    bool all_lower;     // if true, blacklist also all the lower versions of the plugin
} BlackListedPlugin;

const BlackListedPlugin PluginBlacklist[] = {
    { _T("aisradar_pi"), 0, 95, true, true },
    { _T("radar_pi"), 0, 95, true, true },             // GCC alias for aisradar_pi
    { _T("watchdog_pi"), 1, 00, false, true }
};

//----------------------------------------------------------------------------
// PlugIn Messaging scheme Event
//----------------------------------------------------------------------------

class OCPN_MsgEvent: public wxEvent
{
public:
    OCPN_MsgEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

    OCPN_MsgEvent(const OCPN_MsgEvent & event)
    : wxEvent(event),
    m_MessageID(event.m_MessageID),
    m_MessageText(event.m_MessageText)
    { }

    ~OCPN_MsgEvent( );

    // accessors
    wxString GetID() { return m_MessageID; }
    wxString GetJSONText() { return m_MessageText; }
    
    void SetID(const wxString &string) { m_MessageID = string; }
    void SetJSONText(const wxString &string) { m_MessageText = string; }


    // required for sending with wxPostEvent()
    wxEvent *Clone() const; 

private:
    wxString    m_MessageID;
    wxString    m_MessageText;


};

extern  const wxEventType wxEVT_OCPN_MSG;
    

//-----------------------------------------------------------------------------------------------------
//
//          The PlugIn Container Specification
//
//-----------------------------------------------------------------------------------------------------
class PlugInContainer
{
      public:
            PlugInContainer(){ m_pplugin = NULL;
                               m_bEnabled = false;
                               m_bInitState = false;
                               m_bToolboxPanel = false;
                               m_bitmap = NULL; }

            opencpn_plugin    *m_pplugin;
            bool              m_bEnabled;
            bool              m_bInitState;
            bool              m_bToolboxPanel;
            int               m_cap_flag;             // PlugIn Capabilities descriptor
            wxString          m_plugin_file;          // The full file path
            wxString          m_plugin_filename;          // The short file path
            destroy_t         *m_destroy_fn;
            wxDynamicLibrary  *m_plibrary;
            wxString          m_common_name;            // A common name string for the plugin
            wxString          m_short_description;
            wxString          m_long_description;
            int               m_api_version;
            int               m_version_major;
            int               m_version_minor;
            wxBitmap         *m_bitmap;

};

//    Declare an array of PlugIn Containers
WX_DEFINE_ARRAY_PTR(PlugInContainer *, ArrayOfPlugIns);

class PlugInMenuItemContainer
{
      public:
            wxMenuItem        *pmenu_item;
            opencpn_plugin    *m_pplugin;
            bool              b_viz;
            bool              b_grey;
            int               id;
};

//    Define an array of PlugIn MenuItem Containers
WX_DEFINE_ARRAY_PTR(PlugInMenuItemContainer *, ArrayOfPlugInMenuItems);


class PlugInToolbarToolContainer
{
      public:
            PlugInToolbarToolContainer();
            ~PlugInToolbarToolContainer();

            opencpn_plugin    *m_pplugin;
            int               id;
            wxString          label;
            wxBitmap          *bitmap_day;
            wxBitmap          *bitmap_dusk;
            wxBitmap          *bitmap_night;
            wxBitmap          *bitmap_Rollover;

            wxItemKind        kind;
            wxString          shortHelp;
            wxString          longHelp;
            wxObject          *clientData;
            int               position;
            bool              b_viz;
            bool              b_toggle;
            int               tool_sel;

};

//    Define an array of PlugIn ToolbarTool Containers
WX_DEFINE_ARRAY_PTR(PlugInToolbarToolContainer *, ArrayOfPlugInToolbarTools);



//-----------------------------------------------------------------------------------------------------
//
//          The PlugIn Manager Specification
//
//-----------------------------------------------------------------------------------------------------

class PlugInManager
{

public:
      PlugInManager(MyFrame *parent);
      virtual ~PlugInManager();

      bool LoadAllPlugIns(const wxString &plugin_dir, bool enabled_plugins);
      bool UnLoadAllPlugIns();
      bool DeactivateAllPlugIns();
      bool UpdatePlugIns();

      bool UpdateConfig();

      PlugInContainer *LoadPlugIn(wxString plugin_file);
      ArrayOfPlugIns *GetPlugInArray(){ return &plugin_array; }

      bool RenderAllCanvasOverlayPlugIns( ocpnDC &dc, const ViewPort &vp);
      bool RenderAllGLCanvasOverlayPlugIns( wxGLContext *pcontext, const ViewPort &vp);
      void SendCursorLatLonToAllPlugIns( double lat, double lon);
      void SendViewPortToRequestingPlugIns( ViewPort &vp );

      void NotifySetupOptions();
      void CloseAllPlugInPanels( int );

      ArrayOfPlugInToolbarTools &GetPluginToolbarToolArray(){ return m_PlugInToolbarTools; }
      int AddToolbarTool(wxString label, wxBitmap *bitmap, wxBitmap *bmpDisabled,
                         wxItemKind kind, wxString shortHelp, wxString longHelp,
                         wxObject *clientData, int position,
                         int tool_sel, opencpn_plugin *pplugin );

      void RemoveToolbarTool(int tool_id);
      void SetToolbarToolViz(int tool_id, bool viz);
      void SetToolbarItemState(int tool_id, bool toggle);
      void SetToolbarItemBitmaps(int item, wxBitmap *bitmap, wxBitmap *bmpDisabled);
      opencpn_plugin *FindToolOwner(const int id);
      wxString GetToolOwnerCommonName(const int id);

      ArrayOfPlugInMenuItems &GetPluginContextMenuItemArray(){ return m_PlugInMenuItems; }
      int AddCanvasContextMenuItem(wxMenuItem *pitem, opencpn_plugin *pplugin );
      void RemoveCanvasContextMenuItem(int item);
      void SetCanvasContextMenuItemViz(int item, bool viz);
      void SetCanvasContextMenuItemGrey(int item, bool grey);

      void SendNMEASentenceToAllPlugIns(const wxString &sentence);
      void SendPositionFixToAllPlugIns(GenericPosDatEx *ppos);
      void SendAISSentenceToAllPlugIns(const wxString &sentence);
      void SendJSONMessageToAllPlugins(const wxString &message_id, wxJSONValue v);
      void SendMessageToAllPlugins(const wxString &message_id, const wxString &message_body);

      void SendResizeEventToAllPlugIns(int x, int y);
      void SetColorSchemeForAllPlugIns(ColorScheme cs);
      void NotifyAuiPlugIns(void);
      bool CallLateInit(void);

      wxArrayString GetPlugInChartClassNameArray(void);

      ListOfPI_S57Obj *GetPlugInObjRuleListAtLatLon( ChartPlugInWrapper *target, float zlat, float zlon,
                                                    float SelectRadius, const ViewPort& vp );
      wxString CreateObjDescriptions( ChartPlugInWrapper *target, ListOfPI_S57Obj *rule_list );

      wxString GetLastError();
      MyFrame *GetParentFrame(){ return pParent; }

      void DimeWindow(wxWindow *win);

      OCPN_Sound        m_plugin_sound;

private:
      bool CheckBlacklistedPlugin(opencpn_plugin* plugin);
      bool DeactivatePlugIn(PlugInContainer *pic);
      wxBitmap *BuildDimmedToolBitmap(wxBitmap *pbmp_normal, unsigned char dim_ratio);
      bool UpDateChartDataTypes(void);
      bool CheckPluginCompatibility(wxString plugin_file);

      MyFrame                 *pParent;

      ArrayOfPlugIns    plugin_array;
      wxString          m_last_error_string;

      ArrayOfPlugInMenuItems        m_PlugInMenuItems;
      ArrayOfPlugInToolbarTools     m_PlugInToolbarTools;

      wxString          m_plugin_location;

      int               m_plugin_tool_id_next;
      int               m_plugin_menu_item_id_next;
      wxBitmap          m_cached_overlay_bm;



};

WX_DEFINE_ARRAY_PTR(PluginPanel *, ArrayOfPluginPanel);

class PluginListPanel: public wxScrolledWindow
{
public:
      PluginListPanel( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, ArrayOfPlugIns *pPluginArray );
      ~PluginListPanel();

      void SelectPlugin( PluginPanel *pi );
      void UpdateSelections();

private:
      ArrayOfPlugIns     *m_pPluginArray;
      ArrayOfPluginPanel  m_PluginItems;
      PluginPanel        *m_PluginSelected;
};

class PluginPanel: public wxPanel
{
public:
      PluginPanel( PluginListPanel *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, PlugInContainer *p_plugin );
      ~PluginPanel();

      void OnPluginSelected( wxMouseEvent &event );
      void SetSelected( bool selected );
      void OnPluginPreferences( wxCommandEvent& event );
      void OnPluginEnable( wxCommandEvent& event );
      void SetEnabled( bool enabled );
      bool GetSelected(){ return m_bSelected; }

private:
      PluginListPanel *m_PluginListPanel;
      bool             m_bSelected;
      PlugInContainer *m_pPlugin;
      wxStaticText    *m_pName;
      wxStaticText    *m_pVersion;
      wxStaticText    *m_pDescription;
//      wxBoxSizer      *m_pButtons;
      wxFlexGridSizer      *m_pButtons;
      wxButton        *m_pButtonEnable;
      wxButton        *m_pButtonPreferences;
};


//  API 1.11 adds access to S52 Presentation library
//  These are some wrapper conversion utilities

class S52PLIB_Context
{
public:
    S52PLIB_Context(){
        bBBObj_valid = false;
        bCS_Added = false;
        bFText_Added = false;
        CSrules = NULL;
        FText = NULL;
        ChildRazRules = NULL;
        MPSRulesList = NULL;
        };

    ~S52PLIB_Context(){};

    wxBoundingBox           BBObj;                  // lat/lon BBox of the rendered object
    bool                    bBBObj_valid;           // set after the BBObj has been calculated once.

    Rules                   *CSrules;               // per object conditional symbology
    int                     bCS_Added;

    S52_TextC                *FText;
    int                     bFText_Added;
    wxRect                  rText;

    LUPrec                  *LUP;
    ObjRazRules             *ChildRazRules;
    mps_container           *MPSRulesList;
};



void CreateCompatibleS57Object( PI_S57Obj *pObj, S57Obj *cobj );
void UpdatePIObjectPlibContext( PI_S57Obj *pObj, S57Obj *cobj );

#endif            // _PLUGINMGR_H_

