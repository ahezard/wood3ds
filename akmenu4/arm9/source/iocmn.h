/*
    iocmn.h
    Copyright (C) 2010 yellow wood goblin

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

#ifndef __IOCMN_H__
#define __IOCMN_H__

#if defined(_STORAGE_rpg)
#include <iorpg.h>
#elif defined(_STORAGE_r4)
#include <ior4.h>
#elif defined(_STORAGE_ak2i)
#include <ioak2i.h>
#elif defined(_STORAGE_r4idsn)
#include <ior4idsn.h>
#endif

#endif
