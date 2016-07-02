/*
    reset.h
    Copyright (C) 2007 Acekard, www.acekard.com
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

#ifndef __RESET_H__
#define __RESET_H__

#include <nds.h>

ITCM_CODE void resetAndLoop(void);
#ifdef __cplusplus
extern "C" {
#endif
ITCM_CODE void IC_InvalidateAllITCM(void);
ITCM_CODE void DC_FlushAllITCM(void);
ITCM_CODE void DC_InvalidateAllITCM(void);
ITCM_CODE void swiSoftResetITCM(void);
ITCM_CODE void fillMemoryITCM(void* anAddr,u32 aSize,u32 aValue);
ITCM_CODE void zeroMemoryITCM(void* anAddr,u32 aSize);
ITCM_CODE void copyMemoryITCM(void* aSrc,void* aDst,u32 aSize);
#ifdef __cplusplus
}
#endif

extern bool copy_data_itcm;
extern u32 data_destination_itcm;
extern u32 data_size_itcm;

#endif
