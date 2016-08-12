/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Navico BR24 Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
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

/*
 * This includes all OS and wxWidget includes
 */

#ifndef _PI_COMMON_H_
#define _PI_COMMON_H_

#define PLUGIN_NAMESPACE br24
#define PLUGIN_BEGIN_NAMESPACE namespace PLUGIN_NAMESPACE {
#define PLUGIN_END_NAMESPACE }

// For OpenGL
extern "C" {

#ifdef __WXGTK__
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"
#endif

#ifdef __WXOSX__
#include <OpenGL/gl3.h>  // from ..../Frameworks/OpenGL.framework/Headers/gl.h
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#ifdef WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#define GL_GLEXT_LEGACY
#include <GL/gl.h>
#include <GL/glu.h>
#include <opengl/GL/glext.h>
#endif

}  // end "extern C"

#include "wx/wxprec.h"
#ifdef __WXOSX__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif
#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/aui/framemanager.h"
#ifdef __WXOSX__
#pragma clang diagnostic pop
#endif

#include <wx/socket.h>
#include <wx/apptrait.h>
#include <wx/sckaddr.h>
#include <wx/datetime.h>
#include <wx/fileconf.h>
#include <wx/glcanvas.h>
#include <wx/mstream.h>
#include <fstream>
#include <stdint.h>

using namespace std;

#ifdef __WXGTK__
#include <netinet/in.h>
#include <sys/ioctl.h>
#endif

#ifdef __WXOSX__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

// Load the ocpn_plugin. On OS X this generates many warnings, suppress these.
#ifdef __WXOSX__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#include "../../../include/ocpn_plugin.h"
#ifdef __WXOSX__
#pragma clang diagnostic pop
#endif

#ifndef SOCKET
#define SOCKET int
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((SOCKET)~0)
#endif

#ifdef __WXMSW__
#define SOCKETERRSTR (strerror(WSAGetLastError()))
#else
#include <errno.h>
#define SOCKETERRSTR (strerror(errno))
#define closesocket(fd) close(fd)
#endif

#ifndef __WXMSW__
#ifndef UINT8
#define UINT8 uint8_t
#endif
#ifndef UINT16
#define UINT16 uint16_t
#endif
#ifndef UINT32
#define UINT32 uint32_t
#endif
#define wxTPRId64 wxT("ld")
#else
#define wxTPRId64 wxT("I64d")
#endif

#ifndef INT16_MIN
#define INT16_MIN (-32768)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX (255)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define MILLISECONDS_PER_SECOND (1000)

#ifndef PI
#define PI (3.1415926535897931160E0)
#endif
#ifndef deg2rad
#define deg2rad(x) ((x)*2 * PI / 360.0)
#endif
#ifndef rad2deg
#define rad2deg(x) ((x)*360.0 / (2 * PI))
#endif

#endif
