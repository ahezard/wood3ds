/*
    ior4.h
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

#ifndef __IOR4_H__
#define __IOR4_H__

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif
u32 isSDHC(void);
u32 ioR4ReadCardInfo(void);
void ioR4SendMap(u32 address);
u32 ioVersion(void);
void ioR4CryptedRead(u32 address,u32* destination,u32 length);
u32 ioSize(void);
int ioSD(void);
#ifdef __cplusplus
}
#endif

#endif
