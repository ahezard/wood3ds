/*
    savechip.h
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

#ifndef _SAVECHIP_H_
#define _SAVECHIP_H_

#include <nds.h>

#if defined(_STORAGE_rpg)
enum CHIP_TYPE
{
    CT_NONE = 0,
    CT_4K,
    CT_512K,
    CT_8M,
    CT_AUTO
};

void saveChipRead( u32 address, u8 * data, u32 length, u8 chipType );
void saveChipWrite( u32 address, u8 * data, u32 length, u8 chipType );
void saveChipSectorErase( u32 address );
int saveChipFlashErase(u32 sectorCount);
void saveChipEEPROMErase( int type );
u8 saveChipGetType();
u32 saveChipSize( u8 chipType );
u8 saveChipCMD( u8 cmd, u32 address );

#endif

#endif//_SAVECHIP_H_
