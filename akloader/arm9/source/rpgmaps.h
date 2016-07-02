/*
    rpgmaps.h
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

#ifndef _RPG_MAPS_H_
#define _RPG_MAPS_H_

#if defined(_STORAGE_rpg)
void rpgBuildMaps(const char* path,bool protection);
#elif defined(_STORAGE_r4)
u32 GetR4Address(const char* filename);
void r4BuildMaps(const char* path,const char* save);
#elif defined(_STORAGE_ak2i)
void ak2iBuildMaps(const char* path,const char* save,bool protection);
#elif defined(_STORAGE_r4idsn)
void r4idsnBuildMaps(const char* path,const char* save);
#endif

#endif//_RPG_MAPS_H_
