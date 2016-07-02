/*
    gbapatcher.h
    Copyright (C) 2008-2009 somebody
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

#ifndef __GBAPATCHER_H__
#define __GBAPATCHER_H__

#include <nds.h>
#include <setjmp.h>

//typedef void (*TFlashWriteFunc)(u32 address,const u8* buffer,u32 size);

class CGbaWriter
{
  public:
    virtual ~CGbaWriter();
    virtual void Open(void)=0;
    virtual void Write(u32 address,const u8* buffer,u32 size)=0;
    virtual void Close(void)=0;
    virtual u32 MaxSize(void)=0;
};

class CGbaPatcher
{
  private:
    enum TPatchType
    {
      //flash
      ESRAMLabel,
      EEEPROMLabel,
      EEEPROMRead,
      EEEPROMWrite,
      EEEPROMWrite124,
      EEEPROMWrite126,
      EFLASH1M10XLabel,
      EFLASH1M102_1,
      EFLASH1M102_2,
      EFLASH1M102_3,
      EFLASH1M102_4,
      EFLASH1M102_5,
      EFLASH1M103_1,
      EFLASH1M103_2,
      EFLASH1M103_3,
      EFLASH1M103_4,
      EFLASHLabel,
      EFLASH_1,
      EFLASH_2,
      EFLASH_3,
      EFLASH_4,
      EFLASHV123_1,
      EFLASHV123_2,
      EFLASHV123_3,
      EFLASHV123_4,
      EFLASHV123_5,
      EFLASH512Label,
      EFLASH512_1,
      EFLASH512_2,
      //last
      ELast
    };
    enum
    {
      EMax=32
    };
  private:
    struct SPatchInfo
    {
      u32 iOffset;
      TPatchType iType;
    };
    struct SPatchInfo2
    {
      u32 iOffset;
      u32 iValue;
    };
    struct SPatchInfo3
    {
      u32 iLocation;
      u32 iAddress;
      u32 iRa;
      u32 iRb;
    };
    struct SPatchInfo4
    {
      u32 iOffset;
      u16 iValue;
    };
  private:
    SPatchInfo iPatchInfo[EMax];
    u32 iCount;
    SPatchInfo2 iPatchInfo2[EMax*2];
    u32 iCount2;
    SPatchInfo3 iPatchInfo3[EMax];
    u32 iCount3;
    SPatchInfo4 iPatchInfo4[EMax];
    u32 iCount4;
    u32 iSize;
    u32* iData;
    u32 iTrimSize;
    u32 iResultSize;
    jmp_buf iJumpBuf;
    u32 iSaveSize;
    CGbaWriter* iWriter;
    u8 iRamPage;
  private:
    CGbaPatcher();
    void Error(const char* anError);
    inline u32* Data(void) {return iData;};
    inline u16* Data16(void) {return (u16*)iData;};
    inline u8* Data8(void) {return (u8*)iData;};
    void Add(u32 anOffset,TPatchType aType);
    void Add2(u32 anOffset,u32 aValue);
    void Add3(u32 aLocation,u32 anAddress,u32 aRa,u32 aRb);
    void Add4(u32 anOffset,u16 aValue);
    void PatchInternal(void);
    //flash
    bool PatchSRAM(void);
    bool PatchEEPROM(void);
    bool PatchFLASH1M_V10X(void);
    bool PatchFLASH(void);
    void PatchEEPROM111(void);
    //sleep
    bool PatchSleep(void);
    //common sleep
    void CommonSleepSearch(void);
    void CommonSleepPatch(void);
    void SetTrimSize(void);
    //classic nes collection
    bool PatchNes(void);
    //cracks
    bool PatchDragonBallZ(void);
#ifdef __TEST
  private:
    void DumpPatchInfo(void);
#endif
  public:
    CGbaPatcher(u32 aSize,CGbaWriter* aWriter,u32* aData,u8 aRamPage);
    u32 Patch(void);
    inline u32 ResultSize(void) {return iResultSize;};
};

#endif
