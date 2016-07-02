/*
    ipctool.h
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

#ifndef _IPC_TOOL_H_
#define _IPC_TOOL_H_

#define IPC_KEY_SIZE 0x412

#define SYS_BOOT_SLOT1 1
#define SYS_BOOT_SLOT2 2
#define SYS_ARM7_READY_BOOT 3
#define HB_ARM7_LOAD_DONE 1
#define ENC_KEY_FINISHED 1

#ifdef ARM9
inline void fifoSendAndReceive(int channel,u32 value,u32 result)
{
  fifoSendValue32(channel,value);
  do
  {
    while(!fifoCheckValue32(channel));
  } while(fifoGetValue32(channel)!=result);
}
#endif

#endif//_IPC_TOOL_H_
