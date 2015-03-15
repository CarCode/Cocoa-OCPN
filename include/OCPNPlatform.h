/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Platform specific support utilities
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2015 by David S. Register                               *
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

#ifndef OCPNPLATFORM_H
#define OCPNPLATFORM_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/log.h>
#include <wx/stdpaths.h>

#include <stdio.h>

class OCPNPlatform
{
public:    
    OCPNPlatform();
    ~OCPNPlatform();


//      Internal Device Support
    static bool hasInternalGPS(wxString profile = _T(""));      // GPS

    static bool hasInternalBT(wxString profile = _T(""));       // Bluetooth
    bool startBluetoothScan();
    wxArrayString getBluetoothScanResults();

//  Per-Platform initialization support    

    //  Called from MyApp() immediately upon entry to MyApp::OnInit()
    static void Initialize_1( void );

    //  Called from MyApp() immediately before creation of MyFrame()
    static void Initialize_2( void );

    //  Called from MyApp() just before end of MyApp::OnInit()
    static void Initialize_3( void );

    static void OnExit_1( void );
    static void OnExit_2( void );

//--------------------------------------------------------------------------
//      Per-Platform file/directory support
//--------------------------------------------------------------------------

    wxStandardPaths& GetStdPaths();
    wxString &GetHomeDir();
    wxString &GetExePath();
    wxString &GetSharedDataDir();
    wxString &GetPrivateDataDir();
    wxString &GetPluginDir();
    wxString &GetConfigFileName();
    wxString *GetPluginDirPtr();
    wxString *GetSharedDataDirPtr();
    wxString *GetPrivateDataDirPtr();
    bool InitializeLogFile( void );
#ifdef __WXOSX__
    wxString &GetLogFileName();
#endif
    void CloseLogFile( void );
    wxString    &GetLargeLogMessage( void ){ return large_log_message; }
    FILE        *GetLogFilePtr(){ return flog; }

private:
    wxString    m_homeDir;
    wxString    m_exePath;
    wxString    m_SData_Dir;
    wxString    m_PrivateDataDir;
    wxString    m_PluginsDir;
    wxString    m_config_file_name;
    wxString    mlog_file;
    FILE        *flog;
    wxLog       *m_Oldlogger;
    wxString    large_log_message;



};


#endif          //guard
