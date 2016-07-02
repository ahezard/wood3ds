/*
    romloader.h
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

#ifndef _ROMLOADER_H_
#define _ROMLOADER_H_

#include <string>

#if defined(_STORAGE_rpg)
bool loadRom( const std::string & filename, u32 flags, long cheatOffset,size_t cheatSize );
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
bool loadRom( const std::string & filename, const std::string & savename, u32 flags, long cheatOffset,size_t cheatSize );
#endif

#endif//_ROMLOADER_H_
