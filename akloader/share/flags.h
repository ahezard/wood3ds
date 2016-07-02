/*
    flags.h
    Copyright (C) 2008 somebody
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

#ifndef __FLAGS_H__
#define __FLAGS_H__

#include <nds.h>

#define PATCH_DOWNLOAD_PLAY  BIT(0)
#define PATCH_CHEATS         BIT(1)
#define PATCH_SOFT_RESET     BIT(2)
#define PATCH_DMA            BIT(3)
#define PATCH_SD_SAVE        BIT(4)
#define PATCH_PSRAM          BIT(5)
#define PATCH_PROTECTION     BIT(6)
#define PATCH_LINKAGE        BIT(7)
#define PATCH_SAVE_MASK      0x00001f00
#define PATCH_SAVE_SHIFT     8
#define PATCH_SAVE_NO        0x1f
#define PATCH_LANGUAGE_MASK  0x0000e000
#define PATCH_LANGUAGE_SHIFT 13
//#define PATCH_NEXT          BIT(16)

#endif
