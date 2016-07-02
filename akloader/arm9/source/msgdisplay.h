/*
    msgdisplay.h
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

#ifndef _MSGDISPLAY_H_
#define _MSGDISPLAY_H_

#include <nds.h>

enum MSG_IDS
{
    MSGID_FAT_ERROR = 0,
    MSGID_LOADING,
    MSGID_SYS_FILE_MISSING
};

void initMsg();
void cleanMsg();
void setMsg( u32 msgID, u16 color );
void showMsg();
void showMsg( u32 msgID, u16 color );
void showMsgandWait( u32 msgID, u16 color );

#if defined(_STORAGE_r4)
#define MSG_COLOR RGB15(31,31,11)
#elif defined(_STORAGE_r4idsn)
#define MSG_COLOR RGB15(31,31,31)
#else
#define MSG_COLOR RGB15(11,11,31)
#endif

#define MSG_ERROR_MAP_COLOR RGB15(31,0,0)
#define MSG_ERROR_SR_COLOR RGB15(11,31,11)
#define MSG_ERROR_SD_COLOR RGB15(31,11,31)

#endif//_MSGDISPLAY_H_
