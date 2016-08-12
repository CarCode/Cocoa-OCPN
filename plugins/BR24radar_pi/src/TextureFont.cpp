/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  texture OpenGL text rendering built from wxFont
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2014 Sean D'Epagnier                                    *
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

#include "TextureFont.h"

PLUGIN_BEGIN_NAMESPACE

void TextureFont::Build(wxFont &font, bool blur, bool luminance) {
  /* avoid rebuilding if the parameters are the same */
  if (font == m_font && blur == m_blur) return;

  m_font = font;
  m_blur = blur;

  wxBitmap bmp(256, 256);
  wxMemoryDC dc(bmp);

  dc.SetFont(font);

  int maxglyphw = 0, maxglyphh = 0;
  for (int i = MIN_GLYPH; i < MAX_GLYPH; i++) {
    wxCoord gw, gh;
    wxString text;
    if (i == DEGREE_GLYPH)
      text = wxString::Format(_T("%c"), 0x00B0);  //_T("°");
    else
      text = wxString::Format(_T("%c"), i);
    wxCoord descent, exlead;
    dc.GetTextExtent(text, &gw, &gh, &descent, &exlead, &font);  // measure the text

    m_tgi[i].width = gw;
    m_tgi[i].height = gh;

    m_tgi[i].advance = gw;

    maxglyphw = wxMax(gw, maxglyphw);
    maxglyphh = wxMax(gh, maxglyphh);
  }

  /* add extra pixel to give a border between rows of characters
     without this, in some cases a faint line can be see on the edge
     from the character above */
  maxglyphh++;

  int w = COLS_GLYPHS * maxglyphw;
  int h = ROWS_GLYPHS * maxglyphh;

  wxASSERT(w < 2048 && h < 2048);

  /* make power of 2 */
  for (tex_w = 1; tex_w < w; tex_w *= 2)
    ;
  for (tex_h = 1; tex_h < h; tex_h *= 2)
    ;

  wxBitmap tbmp(tex_w, tex_h);
  dc.SelectObject(tbmp);

  /* fill bitmap with black */
  dc.SetBackground(wxBrush(wxColour(0, 0, 0)));
  dc.Clear();

  /* draw the text white */
  dc.SetTextForeground(wxColour(255, 255, 255));

  int row = 0, col = 0;
  for (int i = MIN_GLYPH; i < MAX_GLYPH; i++) {
    if (col == COLS_GLYPHS) {
      col = 0;
      row++;
    }

    m_tgi[i].x = col * maxglyphw;
    m_tgi[i].y = row * maxglyphh;

    wxString text;
    if (i == DEGREE_GLYPH)
      text = wxString::Format(_T("%c"), 0x00B0);  //_T("°");
    else
      text = wxString::Format(_T("%c"), i);

    dc.DrawText(text, m_tgi[i].x, m_tgi[i].y);
    col++;
  }

  wxImage image = tbmp.ConvertToImage();

  GLuint format, internalformat;
  int stride;

  if (luminance) {
    format = GL_LUMINANCE_ALPHA;
    stride = 2;
  } else {
    format = GL_ALPHA;
    stride = 1;
  }

  internalformat = format;

  if (m_blur) image = image.Blur(1);

  unsigned char *imgdata = image.GetData();
  unsigned char *teximage = (unsigned char *)malloc(stride * tex_w * tex_h);

  if (teximage && imgdata) {
    for (int j = 0; j < tex_w * tex_h; j++)
      for (int k = 0; k < stride; k++) teximage[j * stride + k] = imgdata[3 * j];
  }
  if (m_texobj) Delete();

  glGenTextures(1, &m_texobj);
  glBindTexture(GL_TEXTURE_2D, m_texobj);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, internalformat, tex_w, tex_h, 0, format, GL_UNSIGNED_BYTE, teximage);

  free(teximage);
}

void TextureFont::Delete() {
  glDeleteTextures(1, &m_texobj);
  m_texobj = 0;
}

void TextureFont::GetTextExtent(const wxString &string, int *width, int *height) {
  int w0 = 0, w1 = 0, h = 0;

  for (unsigned int i = 0; i < string.size(); i++) {
    wchar_t c = string[i];
    if (c == '\n') {
      h += m_tgi[(int)'A'].height;
      w1 = wxMax(w0, w1);
      w0 = 0;
      continue;
    }

    if (c == 0x00B0) c = DEGREE_GLYPH;

    if (c < MIN_GLYPH || c >= MAX_GLYPH) {
      // outside font
      wxMemoryDC dc;
      dc.SetFont(m_font);
      wxCoord gw, gh;
      dc.GetTextExtent(c, &gw, &gh);  // measure the text
      w0 += gw;
      if (h > gh) gh = h;
      continue;
    }

    TexGlyphInfo &tgisi = m_tgi[c];

    w0 += tgisi.advance;
    if (tgisi.height > h) h = tgisi.height;
  }
  if (width) *width = wxMax(w0, w1);
  if (height) *height = h;
}

void TextureFont::RenderGlyph(wchar_t c) {
  /* degree symbol */
  if (c == 0x00B0)
    c = DEGREE_GLYPH;
  else if (c < MIN_GLYPH || c >= MAX_GLYPH) {
    // outside font, render with draw pixels (slow)
    wxMemoryDC dc;
    dc.SetFont(m_font);
    wxCoord gw, gh;
    dc.GetTextExtent(c, &gw, &gh);  // measure the text
    wxBitmap bmp(gw, gh);
    dc.SelectObject(bmp);
    dc.SetBackground(wxBrush(wxColour(0, 0, 0)));
    dc.Clear();
    /* draw the text white */
    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.DrawText(c, 0, 0);
    wxImage image = bmp.ConvertToImage();
    if (m_blur) image = image.Blur(1);
    unsigned char *imgdata = image.GetData();

    char *data = new char[gw * gh * 2];

    if (data && imgdata) {
      for (int i = 0; i < gw * gh; i++) {
        data[2 * i + 0] = imgdata[3 * i];
        data[2 * i + 1] = imgdata[3 * i];
      }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /* make power of 2 */
    int w, h;
    for (w = 1; w < gw; w *= 2)
      ;
    for (h = 1; h < gh; h *= 2)
      ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, h, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gw, gh, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
    float u = (float)gw / w, v = (float)gh / h;
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2i(0, 0);
    glTexCoord2f(u, 0);
    glVertex2i(gw, 0);
    glTexCoord2f(u, v);
    glVertex2i(gw, gh);
    glTexCoord2f(0, v);
    glVertex2i(0, gh);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_texobj);
    delete[] data;

    glTranslatef(gw, 0.0, 0.0);
    return;
  }

  TexGlyphInfo &tgic = m_tgi[c];

  int x = tgic.x, y = tgic.y;
  float w = tgic.width, h = tgic.height;
  float tx1 = (float)x / tex_w;
  float tx2 = (float)(x + w) / tex_w;
  float ty1 = (float)y / tex_h;
  float ty2 = (float)(y + h) / tex_h;

  glBegin(GL_QUADS);

  glTexCoord2f(tx1, ty1);
  glVertex2i(0, 0);
  glTexCoord2f(tx2, ty1);
  glVertex2i(w, 0);
  glTexCoord2f(tx2, ty2);
  glVertex2i(w, h);
  glTexCoord2f(tx1, ty2);
  glVertex2i(0, h);

  glEnd();
  glTranslatef(tgic.advance, 0.0, 0.0);
}

void TextureFont::RenderString(const wxString &string, int x, int y) {
  glPushMatrix();
  glTranslatef(x, y, 0);

  glPushAttrib(GL_TEXTURE_BIT);
  glBindTexture(GL_TEXTURE_2D, m_texobj);
  glPushMatrix();

  for (unsigned int i = 0; i < string.size(); i++) {
    wchar_t x = string[i];

    if (x == '\n') {
      glPopMatrix();
      glTranslatef(0, m_tgi[(int)'A'].height, 0);
      glPushMatrix();
      continue;
    }
    RenderGlyph(x);
  }

  glPopMatrix();
  glPopAttrib();
  glPopMatrix();
}

PLUGIN_END_NAMESPACE
