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
 *   This m_program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this m_program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "RadarDrawShader.h"
#include "shaderutil.h"
#include "drawutil.h"

PLUGIN_BEGIN_NAMESPACE

// identity vertex program (does nothing special)
static const char *VertexShaderText =
    "void main() \n"
    "{ \n"
    "   gl_TexCoord[0] = gl_MultiTexCoord0; \n"
    "   gl_Position = ftransform(); \n"
    "} \n";

// Convert to rectangular to polar coordinates for radar image in texture
static const char *FragmentShaderText =
    "uniform sampler2D tex2d; \n"
    "void main() \n"
    "{ \n"
    "   float d = length(gl_TexCoord[0].xy);\n"
    "   if (d >= 1.0) \n"
    "      discard; \n"
    "   float a = atan(gl_TexCoord[0].y, gl_TexCoord[0].x) / 6.28318; \n"
    "   gl_FragColor = vec4(1, 0, 0, texture2D(tex2d, vec2(d, a)).x); \n"
    "} \n";

static const char *FragmentShaderColorText =
    "uniform sampler2D tex2d; \n"
    "void main() \n"
    "{ \n"
    "   float d = length(gl_TexCoord[0].xy);\n"
    "   if (d >= 1.0) \n"
    "      discard; \n"
    "   float a = atan(gl_TexCoord[0].y, gl_TexCoord[0].x) / 6.28318; \n"
    "   gl_FragColor = texture2D(tex2d, vec2(d, a)); \n"
    "} \n";

bool RadarDrawShader::Init(int color_option) {
  if (color_option) {
    m_format = GL_RGBA;
    m_channels = SHADER_COLOR_CHANNELS;
  } else {
    m_format = GL_LUMINANCE;
    m_channels = 1;
  }

  if (!CompileShader && !ShadersSupported()) {
    wxLogError(wxT("BR24radar_pi: the OpenGL system of this computer does not support shader m_programs"));
    return false;
  }

  if (!CompileShaderText(&m_vertex, GL_VERTEX_SHADER, VertexShaderText) ||
      !CompileShaderText(&m_fragment, GL_FRAGMENT_SHADER, color_option > 0 ? FragmentShaderColorText : FragmentShaderText)) {
    wxLogError(wxT("BR24radar_pi: the OpenGL system of this computer failed to compile shader programs"));
    return false;
  }

  m_program = LinkShaders(m_vertex, m_fragment);
  if (!m_program) {
    wxLogError(wxT("BR24radar_pi: GPU oriented OpenGL failed to link shader program"));
    return false;
  }

  if (!m_texture) {
    glGenTextures(1, &m_texture);
  }
  glBindTexture(GL_TEXTURE_2D, m_texture);
  // Tell the GPU the size of the texture:
  glTexImage2D(/* target          = */ GL_TEXTURE_2D,
               /* level           = */ 0,
               /* internal_format = */ m_format,
               /* width           = */ RETURNS_PER_LINE,
               /* heigth          = */ LINES_PER_ROTATION,
               /* border          = */ 0,
               /* format          = */ m_format,
               /* type            = */ GL_UNSIGNED_BYTE,
               /* data            = */ m_data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_start_line = -1;
  m_end_line = 0;

  return true;
}

RadarDrawShader::~RadarDrawShader() {
  wxCriticalSectionLocker lock(m_exclusive);

  if (m_vertex) {
    DeleteShader(m_vertex);
    m_vertex = 0;
  }
  if (m_fragment) {
    DeleteShader(m_fragment);
    m_fragment = 0;
  }
  if (m_program) {
    DeleteProgram(m_program);
    m_program = 0;
  }
  if (m_texture) {
    glDeleteTextures(1, &m_texture);
    m_texture = 0;
  }
}

void RadarDrawShader::DrawRadarImage() {
  wxCriticalSectionLocker lock(m_exclusive);

  if (!m_program || !m_texture) {
    return;
  }

  glPushAttrib(GL_TEXTURE_BIT);

  UseProgram(m_program);

  glBindTexture(GL_TEXTURE_2D, m_texture);

  if (m_start_line > -1) {
    // Since the last time we have received data from [m_start_line, m_end_line>
    // so we only need to update the texture for those data lines.
    if (m_end_line < m_start_line) {
      // if the new data wraps past the end of the texture
      // tell it the two parts separately
      // First remap [0, m_end_line>
      glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                      /* level =    */ 0,
                      /* x-offset = */ 0,
                      /* y-offset = */ 0,
                      /* width =    */ RETURNS_PER_LINE,
                      /* height =   */ m_end_line,
                      /* format =   */ m_format,
                      /* type =     */ GL_UNSIGNED_BYTE,
                      /* pixels =   */ m_data);
      // And then remap [m_start_line, LINES_PER_ROTATION>
      glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                      /* level =    */ 0,
                      /* x-offset = */ 0,
                      /* y-offset = */ m_start_line,
                      /* width =    */ RETURNS_PER_LINE,
                      /* height =   */ LINES_PER_ROTATION - m_start_line,
                      /* format =   */ m_format,
                      /* type =     */ GL_UNSIGNED_BYTE,
                      /* pixels =   */ m_data + m_start_line * RETURNS_PER_LINE * m_channels);
    } else {
      // Remap [m_start_line, m_end_line>
      glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                      /* level =    */ 0,
                      /* x-offset = */ 0,
                      /* y-offset = */ m_start_line,
                      /* width =    */ RETURNS_PER_LINE,
                      /* height =   */ m_end_line - m_start_line,
                      /* format =   */ m_format,
                      /* type =     */ GL_UNSIGNED_BYTE,
                      /* pixels =   */ m_data + m_start_line * RETURNS_PER_LINE * m_channels);
    }
    m_start_line = -1;
    m_end_line = 0;
  }

  // We tell the GPU to draw a square from (-512,-512) to (+512,+512).
  // The shader morphs this into a circle.
  float fullscale = 512;
  glBegin(GL_QUADS);
  glTexCoord2f(-1, -1);
  glVertex2f(-fullscale, -fullscale);
  glTexCoord2f(1, -1);
  glVertex2f(fullscale, -fullscale);
  glTexCoord2f(1, 1);
  glVertex2f(fullscale, fullscale);
  glTexCoord2f(-1, 1);
  glVertex2f(-fullscale, fullscale);
  glEnd();

  UseProgram(0);
  glPopAttrib();
}

void RadarDrawShader::ProcessRadarSpoke(int transparency, SpokeBearing angle, UINT8 *data, size_t len) {
  GLubyte alpha = 255 * (MAX_OVERLAY_TRANSPARENCY - transparency) / MAX_OVERLAY_TRANSPARENCY;
  wxCriticalSectionLocker lock(m_exclusive);

  if (m_start_line == -1) {
    m_start_line = angle;  // Note that this only runs once after each draw,
  }
  m_end_line = angle + 1;  // whereas this keeps running every draw operation

  if (m_channels == SHADER_COLOR_CHANNELS) {
    unsigned char *d = m_data + (angle * RETURNS_PER_LINE) * m_channels;
    for (size_t r = 0; r < len; r++) {
      GLubyte strength = data[r];
      BlobColor color = m_ri->m_color_map[strength];
      d[0] = m_ri->m_color_map_red[color];
      d[1] = m_ri->m_color_map_green[color];
      d[2] = m_ri->m_color_map_blue[color];
      d[3] = color != BLOB_NONE ? alpha : 0;
      d += m_channels;
    }
  } else {
    unsigned char *d = m_data + (angle * RETURNS_PER_LINE);
    for (size_t r = 0; r < len; r++) {
      GLubyte strength = data[r];
      BlobColor color = m_ri->m_color_map[strength];
      *d++ = (m_ri->m_color_map_red[color] * alpha) >> 8;
    }
  }
}

PLUGIN_END_NAMESPACE
