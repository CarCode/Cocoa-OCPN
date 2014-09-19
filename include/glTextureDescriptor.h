/******************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2013 by David S. Register                               *
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
 ***************************************************************************
 */

#ifndef __GLTEXTUREDESCRIPTOR_H__
#define __GLTEXTUREDESCRIPTOR_H__

#include "dychart.h"

#define CA_READ         0
#define CA_WRITE        1

#define GPU_TEXTURE_UNKNOWN             0
#define GPU_TEXTURE_UNCOMPRESSED        1
#define GPU_TEXTURE_COMPRESSED          2

class glTextureDescriptor
{
public:
    glTextureDescriptor();
    ~glTextureDescriptor();
    void FreeAll();
    void FreeMap();
    void FreeCompLevel(int level);
    
    size_t GetMapArrayAlloc(void);
    size_t GetCompArrayAlloc(void);
    size_t GetCompCompArrayAlloc(void);

    unsigned char *CompressedArrayAccess( int mode, unsigned char *write_data, int level);
    unsigned char *CompCompArrayAccess( int mode, unsigned char *write_data, int level);
    GLuint tex_name;
    int level_min;
    int x;
    int y;
    int nGPU_compressed;
    int nCache_Color;
    
    unsigned char       *map_array[10];
    int                 miplevel_upload[10];
    int                 compcomp_size[10];
    
private:    
    unsigned char *comp_array[10];
    unsigned char *compcomp_array[10];
};


#endif
