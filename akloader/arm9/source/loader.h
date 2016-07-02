/*
    loader.h
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LOADER_H_
#define _LOADER_H_

#include <nds.h>
#include <stdio.h>
#include <string.h>

bool loadFile(const char* filename,u32& originSpeed,u32& changedSpeed,u32& sizeMask,bool& hasRSA);

#ifdef __cplusplus
extern "C" {
#endif
u32 size_mask(u32 in);
#ifdef __cplusplus
}
#endif

#endif//_LOADER_H_
