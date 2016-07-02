/*
    ior4idsn.h
    Copyright (C) 2010 r4idsn team

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

#ifndef __IOR4IDSN_H__
#define __IOR4IDSN_H__

#include <nds.h>
#ifdef __cplusplus
extern "C" {
#endif
enum MAP_TABLE_NAME
{
  MTN_NOR_OFFSET = 0,
  MTN_MAP_ROM    = 1,
  MTN_MAP_SAVE   = 2
};

enum NOR_ALLOC
{
  NOR_BOOTMODULE_START = 0x000000,
  NOR_BOOTMODULE_END = 0x30000-1,
  NOR_FAT1_START = 0x40000, // rom file fat clusters map table
  NOR_FAT1_END = 0x80000-1,
  NOR_FAT2_START = 0x30000, // save file fat clusters map table
  NOR_FAT2_END = 0x40000-1
};

bool isSDHC(void);
u32 ioVersion(void);
u32 ioSize(void);
int ioSD(void);

void ioRpgSendCommand(u32 command[2],u32 pageSize,u32 latency,void* buffer);
bool ioRpgWaitCmdBusy(bool forceWait);
void ioRpgWaitSDState(u8 state);

#define KEY_PARAM 0x00406000
#define CARD_COMMAND32 ((vuint32*)0x040001A8)
static inline void ioRpgPushData(const void* data,u16 length)
{
  if(0==((u32)data&0x03))
  {
    u32* pData32=(u32*)data;
    while(length)
    {
      CARD_COMMAND32[0]=*pData32++;
      CARD_COMMAND32[1]=*pData32++;
      length-=8;
      REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET;
      while(REG_ROMCTRL&CARD_BUSY) {}
    }
  }
  else
  {
    u8* pData8=(u8*)data;
    while(length)
    {
      CARD_COMMAND[0]=*pData8++;
      CARD_COMMAND[1]=*pData8++;
      CARD_COMMAND[2]=*pData8++;
      CARD_COMMAND[3]=*pData8++;
      CARD_COMMAND[4]=*pData8++;
      CARD_COMMAND[5]=*pData8++;
      CARD_COMMAND[6]=*pData8++;
      CARD_COMMAND[7]=*pData8++;
      length-=8;
      REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET;
      while(REG_ROMCTRL&CARD_BUSY) {}
    }
  }
}

void ioRpgSetDeviceStatus(u8 tableBlockSize,bool isSDHC);
void ioRpgSetMapTableAddress(u32 tableName,u32 tableInRamAddress);

void ioRpgWriteFlashByte(u32 address,u8 data);
void ioRpgWriteFlash(u32 address,const void* data,u32 length);
void ioRpgEraseFlash(u32 address);

bool sddInitSD(void);
void sddReadBlocks(u32 addr,u32 blockCount,void* buffer);
void sddWriteBlocks(u32 addr,u32 blockCount,const void* buffer);
void sddGetSDInfo(u8 info[8]);
#ifdef __cplusplus
}
#endif

#endif//_IORPG_H_
