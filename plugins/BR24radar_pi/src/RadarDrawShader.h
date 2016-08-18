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

#ifndef _RADARDRAWSHADER_H_
#define _RADARDRAWSHADER_H_

#include "RadarDraw.h"

PLUGIN_BEGIN_NAMESPACE

#define SHADER_COLOR_CHANNELS (4)  // RGB + Alpha

class RadarDrawShader : public RadarDraw {
 public:
  RadarDrawShader(RadarInfo* ri) {
    m_ri = ri;
    m_start_line = LINES_PER_ROTATION;
    m_end_line = 0;
    m_texture = 0;
    m_fragment = 0;
    m_vertex = 0;
    m_program = 0;
    m_format = GL_RGBA;
    m_channels = SHADER_COLOR_CHANNELS;
    memset(m_data, 0, sizeof(m_data));
  }

  ~RadarDrawShader();

  bool Init();
  void DrawRadarImage();
  void ProcessRadarSpoke(int transparency, SpokeBearing angle, UINT8* data, size_t len);

 private:
  RadarInfo* m_ri;

  wxCriticalSection m_exclusive;  // protects the following three data structures
  unsigned char m_data[SHADER_COLOR_CHANNELS * LINES_PER_ROTATION * RETURNS_PER_LINE];
  int m_start_line;
  int m_end_line;

  int m_format;
  int m_channels;

  GLuint m_texture;
  GLuint m_fragment;
  GLuint m_vertex;
  GLuint m_program;
};

PLUGIN_END_NAMESPACE

#endif /* _RADARDRAWSHADER_H_ */
