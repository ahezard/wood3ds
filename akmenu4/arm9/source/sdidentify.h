/*
    sdidentify.h
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

#ifndef _SDIDENTIFY_H_
#define _SDIDENTIFY_H_

#include <nds.h>
#include <string>

#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)

std::string sdidGetSDName();

std::string sdidGetSDManufacturerID();

std::string sdidGetSDManufacturerName();

u32 sdidCheckSDSpeed( u32 readCount );

#endif

#endif//_SDIDENTIFY_H_
