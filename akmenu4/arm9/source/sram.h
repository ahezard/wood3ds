/*
    sram.h
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

#ifndef __SRAM_H__
#define __SRAM_H__

#include <nds.h>
#include <stdio.h>

#define SRAM_PAGES 128
#define SRAM_SAVE_PAGES 32
#define SRAM_PAGE_SIZE 0x1000
#define SRAM_SAVE_SIZE SRAM_SAVE_PAGES*SRAM_PAGE_SIZE
#define SAVE_GBA_HEADER_MAGIC 0x42474b41

class cSram
{
  public:
    struct sSaveInfo
    {
      u32 marker;
      u32 offset;
      u32 size;
      u32 header;
      sSaveInfo(): marker(SAVE_GBA_HEADER_MAGIC),offset(0),size(SRAM_SAVE_SIZE),header(0) {};
      sSaveInfo(u32 aSize): marker(SAVE_GBA_HEADER_MAGIC),offset(0),size(aSize),header(0) {};
    };
  public:
    static void SaveSramToFile(const char* romName,u16 aStartPage);
    static void LoadSramFromFile(const char* romName,u16 aStartPage);
    static void CreateDefaultFile(const char* romName,u32 size);
    static u8* SaveSramToMemory(u16 aStartPage,sSaveInfo& aSaveInfo,bool aShowProgress);
    static void ProcessRAW(FILE* aFile,sSaveInfo& anInfo);
  private:
    static bool SaveFileName(const char* romName,char* saveName);
    static void BlankSram(u16 aStartPage);
    static void NormalizeSize(sSaveInfo& anInfo);
};

#endif
