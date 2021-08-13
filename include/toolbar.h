/* ***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Toolbar
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

#ifndef _TOOLBAR_H__
#define _TOOLBAR_H__

#include "wx/tbarbase.h"
#include <wx/dynarray.h>
#include "styles.h"
#include <vector>

class ocpnFloatingToolbarDialog;

class ToolbarItemContainer{
public:
    ToolbarItemContainer();
    ~ToolbarItemContainer(){}
    
    ToolbarItemContainer( int toolid, wxBitmap bmpNormal, wxBitmap bmpDisabled,
                            wxItemKind kind, wxString tooltip, wxString label)
    {   m_ID = toolid;
        m_tipString = tooltip;
        m_label = label;
        m_toolKind = kind;
        m_bmpNormal = bmpNormal;
        m_bmpDisabled = bmpDisabled;
        m_bRequired = false;
        m_bPlugin = false;
    }

    ToolbarItemContainer( int toolid, wxBitmap bmpNormal, 
                            wxItemKind kind, wxString tooltip, wxString label)
    {   m_ID = toolid;
        m_tipString = tooltip;
        m_label = label;
        m_toolKind = kind;
        m_bmpNormal = bmpNormal;
        m_bmpDisabled = wxNullBitmap;
        m_bRequired = false;
        m_bPlugin = false;
    }

    
    int m_ID;
    wxString m_tipString;
    wxString m_label;
    wxItemKind m_toolKind;
    bool m_bRequired;
    bool m_bPlugin;

    
    wxBitmap m_bmpNormal;
    wxBitmap m_bmpDisabled;
    wxToolBarToolBase *m_tool;
    
    //  Supplemental SVG icons for plugin tools
    wxString m_NormalIconSVG;
    wxString m_RolloverIconSVG;
    wxString m_ToggledIconSVG;
};

typedef std::vector<ToolbarItemContainer *> ArrayOfToolbarItemContainer;


#define TOOLTIPON_TIMER       10000
#define TOOLTIPOFF_TIMER      10001

enum {
    TOOLBAR_HIDE_TO_GRABBER = 0,
    TOOLBAR_HIDE_TO_FIRST_TOOL,
};

class ToolTipWin;
class ocpnToolBarTool;

// ----------------------------------------------------------------------------
// ocpnToolBarSimple is a generic toolbar implementation in pure wxWidgets
//    Adapted from wxToolBarSimple( deprecated )
// ----------------------------------------------------------------------------

class ocpnToolBarSimple: public wxControl {
public:
      // ctors and dtor
      ocpnToolBarSimple() {
            Init();
      }

      ocpnToolBarSimple( wxWindow *parent, wxWindowID winid, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL,
                  const wxString& name = wxToolBarNameStr ) : m_one_shot(500)  {
            Init();

            Create( parent, winid, pos, size, style, name );
      }

      bool Create( wxWindow *parent, wxWindowID winid, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTB_HORIZONTAL,
                  const wxString& name = wxToolBarNameStr );

      virtual ~ocpnToolBarSimple();

      virtual void SetToggledBackgroundColour( wxColour c ) {
            m_toggle_bg_color = c;
      }
      ;
      virtual void SetColorScheme( ColorScheme cs );

      // implementation from now on
      // --------------------------

      // event handlers
      void OnPaint( wxPaintEvent& event );
      void OnSize( wxSizeEvent& event );
      void OnMouseEvent( wxMouseEvent& event );
      void OnKillFocus( wxFocusEvent& event );
      void OnToolTipTimerEvent( wxTimerEvent& event );
      void OnToolTipOffTimerEvent( wxTimerEvent& event );

      wxToolBarToolBase *AddTool( int toolid, const wxString& label, const wxBitmap& bitmap,
                  const wxBitmap& bmpDisabled, wxItemKind kind = wxITEM_NORMAL,
                  const wxString& shortHelp = wxEmptyString, const wxString& longHelp =
                              wxEmptyString, wxObject *data = NULL );

      wxToolBarToolBase *AddTool( int toolid, const wxString& label, const wxBitmap& bitmap,
                  const wxString& shortHelp = wxEmptyString, wxItemKind kind = wxITEM_NORMAL ) {
            return AddTool( toolid, label, bitmap, wxNullBitmap, kind, shortHelp );
      }

      wxToolBarToolBase *InsertTool( size_t pos, int id, const wxString& label,
                  const wxBitmap& bitmap, const wxBitmap& bmpDisabled, wxItemKind kind,
                  const wxString& shortHelp, const wxString& longHelp, wxObject *clientData );

      wxToolBarToolBase *InsertTool( size_t pos, wxToolBarToolBase *tool );

      // Only allow toggle if returns true. Call when left button up.
      virtual bool OnLeftClick( int toolid, bool toggleDown );

      // Call when right button down.
      virtual void OnRightClick( int toolid, long x, long y );

      // Called when the mouse cursor enters a tool bitmap.
      // Argument is wxID_ANY if mouse is exiting the toolbar.
      virtual void OnMouseEnter( int toolid );
      virtual void DoPluginToolUp();

      size_t GetToolsCount() const { return m_tools.GetCount(); }
      void SetToolShowCount( int count ){ m_nShowTools = count; }
      int GetToolShowCount(){ return m_nShowTools; }

      int GetNoRowsOrColumns() { return m_currentRowsOrColumns; };
      int GetLineCount() { return m_LineCount; };
      int GetVisibleToolCount();

      void SetToolNormalBitmapEx(wxToolBarToolBase *tool, const wxString & iconname);
      void SetToolNormalBitmapSVG(wxToolBarToolBase *tool, wxString fileSVG);

      void EnableRolloverBitmaps( bool enable ){ m_tbenableRolloverBitmaps = enable; }
      
      // get the control with the given id or return NULL
      virtual wxControl *FindControl( int toolid );

      // add a separator to the toolbar
      virtual wxToolBarToolBase *AddSeparator();
      virtual wxToolBarToolBase *InsertSeparator( size_t pos );

      // remove the tool from the toolbar: the caller is responsible for actually
      // deleting the pointer
      virtual wxToolBarToolBase *RemoveTool( int toolid );

      // delete tool either by index or by position
      virtual bool DeleteToolByPos( size_t pos );
      virtual bool DeleteTool( int toolid );

      // delete all tools
      virtual void ClearTools();

      // must be called after all buttons have been created to finish toolbar
      // initialisation
      virtual bool Realize();

      // tools state
      // -----------

      virtual void EnableTool( int toolid, bool enable );
      virtual void ToggleTool( int toolid, bool toggle );

      virtual void SetToolBitmaps( int toolid, wxBitmap *bmp, wxBitmap *bmpRollover );
      virtual void SetToolBitmapsSVG( int id, wxString fileSVGNormal,
                                      wxString fileSVGRollover,
                                      wxString fileSVGToggled );
      
      void InvalidateBitmaps();

      // set/get tools client data (not for controls)
      virtual wxObject *GetToolClientData( int toolid ) const;
      virtual void SetToolClientData( int toolid, wxObject *clientData );

      // returns tool pos, or wxNOT_FOUND if tool isn't found
      virtual int GetToolPos( int id ) const;

      // return true if the tool is toggled
      virtual bool GetToolState( int toolid ) const;

      virtual bool GetToolEnabled( int toolid ) const;

      virtual void SetToolShortHelp( int toolid, const wxString& helpString );
      virtual wxString GetToolShortHelp( int toolid ) const;
      virtual void SetToolLongHelp( int toolid, const wxString& helpString );
      virtual wxString GetToolLongHelp( int toolid ) const;

      virtual void SetToolTooltipHiViz( int id, bool b_hiviz );

      virtual void SetSizeFactor( float factor){ m_sizefactor = factor; InvalidateBitmaps(); }
      // toolbar geometry
      // ----------------

      // the toolbar can wrap - limit the number of columns or rows it may take
      void SetMaxRowsCols( int rows, int cols ) {
            m_maxRows = rows;
            m_maxCols = cols;
      }
      int GetMaxRows() const {
            return m_maxRows;
      }
      int GetMaxCols() const {
            return m_maxCols;
      }

      // get/set the size of the bitmaps used by the toolbar: should be called
      // before adding any tools to the toolbar
      virtual void SetToolBitmapSize( const wxSize& size ) {
            m_defaultWidth = size.x;
            m_defaultHeight = size.y;
      }
      virtual wxSize GetToolBitmapSize() const {
            return wxSize( m_defaultWidth, m_defaultHeight );
      }

      // the button size in some implementations is bigger than the bitmap size:
      // get the total button size (by default the same as bitmap size)
      virtual wxSize GetToolSize() const {
            return GetToolBitmapSize();
      }

      virtual wxRect GetToolRect( int tool_id );

      // returns a (non separator) tool containing the point (x, y) or NULL if
      // there is no tool at this point (corrdinates are client)
      wxToolBarToolBase *FindToolForPosition( wxCoord x, wxCoord y );

      // find the tool by id
      wxToolBarToolBase *FindById( int toolid ) const;

      // return true if this is a vertical toolbar, otherwise false
      bool IsVertical() const {
            return HasFlag( wxTB_LEFT | wxTB_RIGHT | wxTB_VERTICAL);
      }

      // the list of all our tools
      wxToolBarToolsList m_tools;

      // the maximum number of toolbar rows/columns
      int m_maxRows;
      int m_maxCols;

      // the size of the toolbar bitmaps
      wxCoord m_defaultWidth, m_defaultHeight;

      void HideTooltip();
      void KillTooltip();
      void EnableTooltips();
      void DisableTooltips();

protected:
      // common part of all ctors
      void Init();

      // implement base class pure virtuals
      virtual wxToolBarToolBase *DoAddTool( int toolid, const wxString& label,
                  const wxBitmap& bitmap, const wxBitmap& bmpDisabled, wxItemKind kind,
                  const wxString& shortHelp = wxEmptyString, const wxString& longHelp =
                              wxEmptyString, wxObject *clientData = NULL, wxCoord xPos =
                              wxDefaultCoord, wxCoord yPos = wxDefaultCoord );

      virtual bool DoInsertTool( size_t pos, wxToolBarToolBase *tool );
      virtual bool DoDeleteTool( size_t pos, wxToolBarToolBase *tool );

      virtual void DoEnableTool( wxToolBarToolBase *tool, bool enable );
      virtual void DoToggleTool( wxToolBarToolBase *tool, bool toggle );

      virtual wxToolBarToolBase *CreateTool( int winid, const wxString& label,
                  const wxBitmap& bmpNormal, const wxBitmap& bmpDisabled, wxItemKind kind,
                  wxObject *clientData, const wxString& shortHelp, const wxString& longHelp );

      // helpers
      void DrawTool( wxToolBarToolBase *tool );
      virtual void DrawTool( wxDC& dc, wxToolBarToolBase *tool );
      virtual void SpringUpButton( int index );

      int m_currentRowsOrColumns;
      int m_LineCount;

      int m_pressedTool, m_currentTool;

      wxCoord m_lastX, m_lastY;
      wxCoord m_maxWidth, m_maxHeight;
      wxCoord m_xPos, m_yPos;

      wxColour m_toggle_bg_color;
      wxColour m_toolOutlineColour;
      ToolTipWin *m_pToolTipWin;
      ocpnToolBarTool *m_last_ro_tool;

      ColorScheme m_currentColorScheme;

      wxTimer m_tooltip_timer;
      int m_one_shot;
      wxTimer m_tooltipoff_timer;
      int m_tooltip_off;
      bool m_btooltip_show;

      bool m_btoolbar_is_zooming;

      ocpnStyle::Style* m_style;

      float m_sizefactor;

      int m_last_plugin_down_id;
      bool m_leftDown;
      int m_nShowTools;
      bool m_tbenableRolloverBitmaps;

private:
DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------------------------------
//    ocpnFloatingToolbarDialog Specification
//----------------------------------------------------------------------------------------------------------

#define FADE_TIMER 2
#define DESTROY_TIMER 3

class ocpnFloatingToolbarDialog: public wxFrame {
DECLARE_EVENT_TABLE()

public:
      ocpnFloatingToolbarDialog( wxWindow *parent, wxPoint position, long orient, float size_factor );
      ~ocpnFloatingToolbarDialog();

      void OnClose( wxCloseEvent& event );
      void OnWindowCreate( wxWindowCreateEvent& event );
      void OnToolLeftClick( wxCommandEvent& event );
      virtual void OnKeyDown( wxKeyEvent &event );
      virtual void OnKeyUp( wxKeyEvent &event );
      void MouseEvent( wxMouseEvent& event );
      void FadeTimerEvent( wxTimerEvent& event );
      bool IsToolbarShown() { return ( m_ptoolbar != 0 ); }
      float GetScaleFactor() { return m_sizefactor; }

      void Realize();
      ocpnToolBarSimple *GetToolbar();
      ocpnToolBarSimple *CreateNewToolbar();
      void SetToolbarHideMethod(int n_method ){ n_toolbarHideMethod = n_method; }

      void SetToolConfigString(wxString string){ m_configString = string; }
      wxString GetToolConfigString(){ return m_configString; }

      float GetSizeFactor(){ return m_sizefactor; }

      void CreateConfigMenu();
      bool _toolbarConfigMenuUtil( ToolbarItemContainer *tic );

      void SetCornerRadius( int radius){ m_cornerRadius = radius; }

      void HideTooltip();
      void ShowTooltips();
      void EnableTooltips() { if(m_ptoolbar) m_ptoolbar->EnableTooltips(); }
      void DisableTooltips() { if(m_ptoolbar) m_ptoolbar->DisableTooltips(); }
      void EnableTool( int toolid, bool enable );
      void SetToolShortHelp( int toolid, const wxString& helpString );

      void DestroyToolBar();
      void ToggleOrientation();
      void MoveDialogInScreenCoords( wxPoint posn, wxPoint posn_old );
      void SetDefaultPosition();
      void LockPosition(bool lock){ m_block = lock; }
      virtual void SetColorScheme( ColorScheme cs );
      ColorScheme GetColorScheme(){ return m_cs; }
      void GetFrameRelativePosition( int *x, int *y);
      void RestoreRelativePosition( int x, int y );

      void SetGeometry(bool bAvoid, wxRect rectAvoid);
      void SetMinX( int offset ){ m_dock_min_x = offset; }
      void SetMinY( int offset ){ m_dock_min_y = offset; }
      long GetOrient() { return m_orient; }
      wxSize GetToolSize();

      void RefreshFadeTimer();
      void SetAutoHideTimer(int time);
      void SetAutoHide( bool hide ){ m_bAutoHideToolbar = hide; }
      
      size_t GetToolCount();
      void SetToolShowMask( wxString mask );
      wxString GetToolShowMask( void ){ return m_toolShowMask; }
      
      void SetToolShowCount( int count );
      int GetToolShowCount( void );
      
      bool CheckAndAddPlugInTool( ocpnToolBarSimple *tb );
      bool AddDefaultPositionPlugInTools( ocpnToolBarSimple *tb );
      ocpnToolBarSimple *CreateMyToolbar();

      int GetDockX() { return m_dock_x; }
      int GetDockY() { return m_dock_y; }
      void SetDockX( int x) { m_dock_x = x; }
      void SetDockY( int y) { m_dock_y = y; }

      void SetYAuxOffset( int offset ){ m_auxOffsetY = offset; }

      bool toolbarConfigChanged;

      wxMenu  *m_FloatingToolbarConfigMenu;

      wxString m_tblastAISiconName;
      wxToolBarToolBase *m_pTBAISTool;
      bool m_toolbar_scale_tools_shown;
      void SetBackGroundColorString( wxString colorRef );
      void SetULDockPosition(wxPoint position);
      
      ArrayOfToolbarItemContainer m_Items;
      
      void AddToolItem(ToolbarItemContainer *item);
      int RebuildToolbar();
      void EnableRolloverBitmaps( bool bEnable );
      bool GetEnableRolloverBitmaps(){ return m_enableRolloverBitmaps; }
      
protected:
    ocpnToolBarSimple *m_ptoolbar;
    
private:
      void DoFade( int value );

      wxWindow *m_pparent;
      wxBoxSizer *m_topSizer;

      long m_orient;
      wxTimer m_fade_timer;
      int m_opacity;
      ColorScheme m_cs;

      wxPoint m_position;
      int m_dock_x;
      int m_dock_y;
      int m_dock_min_x;
      int m_dock_min_y;

      ocpnStyle::Style* m_style;
      bool m_block;

      bool m_marginsInvisible;
      float m_sizefactor;

      bool m_bAutoHideToolbar;
      int m_nAutoHideToolbar;

      wxString m_backcolorString;
      int m_cornerRadius;
      wxString m_toolShowMask;
      int n_toolbarHideMethod;
      bool b_canToggleOrientation;
      wxString m_configString;
      bool m_enableRolloverBitmaps;
      int m_auxOffsetY;
};

//---------------------------------------------------------------------------

class ToolbarMOBDialog : public wxDialog{
private:
    std::vector<wxRadioButton*> choices;

public:
    ToolbarMOBDialog( wxWindow* parent );
    int GetSelection();
};


#define SYMBOL_ToolbarChoices_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX

class ToolbarChoicesDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( ToolbarChoicesDialog )
    DECLARE_EVENT_TABLE()
    
public:
    /// Constructors
    ToolbarChoicesDialog( );
    ToolbarChoicesDialog( wxWindow* parent, wxWindow *sponsor, wxWindowID id = -1,
                        const wxString& caption = _T(""),
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = SYMBOL_ToolbarChoices_STYLE );
    
    ~ToolbarChoicesDialog();
    
    void SetColorScheme(ColorScheme cs);
    void RecalculateSize( void );
    void CreateControls();
    
    void OnCancelClick( wxCommandEvent& event );
    void OnOkClick( wxCommandEvent& event );
    
    
    wxButton*     m_CancelButton;
    wxButton*     m_OKButton;

    std::vector<wxCheckBox*> cboxes;
    wxMenu        *m_configMenu;
    ocpnFloatingToolbarDialog *m_ToolbarDialogAncestor;
};

#endif
