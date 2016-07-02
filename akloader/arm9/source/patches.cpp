/*
    patches.cpp
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

#include "patches.h"
#include <string.h>
#include <stdio.h>
#include "../../share/gamecode.h"

void patchDS(const char* aFileName,u32* aParams,u32 aSpeed,u32 aSizeMask)
{
  CDsPatcher patcher(aFileName,aParams,aSpeed,aSizeMask);
  patcher.Patch();
}

void patchDSi(const char* aFileName,u32* aParams,u32 aSizeMask)
{
  CDsiPatcher patcher(aFileName,aParams,aSizeMask);
  patcher.Patch();
}

void patchHB(void)
{
  CHbPatcher patcher;
  patcher.Patch();
}

u32 GenerateThumbBL(u32 aSource,u32 aTarget)
{
  u32 result=0;
  s32 diff=aTarget-aSource-4;
  if(diff<=4194302&&diff>=-4194304&&(diff&1)==0)
  {
    result=(0xf8000000|((diff&0xffe)<<15))|(0xf000|((diff>>12)&0x7ff));
  }
  return result;
}

u32 GenerateArmBL(u32 aSource,u32 aTarget,bool aType)
{
  u32 result=0;
  s32 diff=aTarget-aSource-8;
  if(diff<=33554428&&diff>=-33554432&&(diff&3)==0)
  {
    result=(aType?0xeb000000:0xea000000)|((diff>>2)&0xffffff);
  }
  return result;
}

CPatcher::CPatcher(const char* aFileName,u32* aParams,u32 aSizeMask):
  iUnCompressed(false),iFixedAddr(0),iFixedData(0),iUncompressedSize(0),
  iFileName(aFileName),iParam(aParams[0]),
  iDmaFuncs(),iPatchDMA4Entry(0),iPatchDMA4Ptr(0),iUsePatchData(false),iSizeMask(aSizeMask),
  iPatchArm7Entry(0),iProtectionRegion2Fixed(false),
  iSDSaveStart(0),iSDSaveEnd(0),
  iCheatOffset(aParams[1]),iCheatSize(aParams[2]),
  iVBlank(0),iSoftResetHandler(0),iSoftResetHook(0)
{
  iPatchSize[KArm7]=iPatchSize[KArm9]=0;
  iGameCode=gamecode(__NDSHeader->gameCode);
  iGameCodeAll=iGameCode&0x00ffffff;
}

void CPatcher::PatchMem(u32 aCpu,s32 anOffset,u32 aValue)
{
  if(anOffset>=0)
  {
    *(u32*)anOffset=aValue;
  }
  else
  {
    *(u16*)(-anOffset)=aValue;
  }
  if(iPatchSize[aCpu]<((aCpu==KArm7)?KPatchInfoSize7:KPatchInfoSize9))
  {
    TPatchInfo* patchData=(aCpu==KArm7)?iPatchData7:iPatchData9;
    patchData[iPatchSize[aCpu]].iOffset=anOffset;
    patchData[iPatchSize[aCpu]].iValue=aValue;
    ++iPatchSize[aCpu];
  }
}

void CPatcher::NeedUncompressed(void)
{
  if(!iUnCompressed)
  {
    iUncompressedSize=UncompressRom((u8*)__NDSHeader->arm9destination,__NDSHeader->arm9binarySize,0);
    if(!iUncompressedSize) iUncompressedSize=__NDSHeader->arm9binarySize;
    iUnCompressed=true;
  }
}

u32 CPatcher::UncompressRom(u8* aMainMemory,u32 aCodeLength,u32 aMemOffset)
{
  u8* ADDR1=NULL;
  u8* ADDR1_END=NULL;
  u8* ADDR2=NULL;
  u8* ADDR3=NULL;

  u8* pBuffer32=(u8*)(aMainMemory);
  u8* pBuffer32End=(u8*)(aMainMemory+aCodeLength);

  while(pBuffer32<pBuffer32End)
  {
    if(0xDEC00621==*(u32*)pBuffer32&&0x2106C0DE==*(u32*)(pBuffer32+4))
    {
      ADDR1=(u8*)(*(u32*)(pBuffer32-8));
      iFixedAddr=(u32)(pBuffer32-8);
      iFixedData=*(u32*)(pBuffer32-8);
      *(u32*)(pBuffer32-8)=0;
      break;
    }
    pBuffer32+=4;
  }
  if(0==ADDR1)
  {
    iFixedAddr=0;
    return 0;
  }

  u32 A=*(u32*)(ADDR1+aMemOffset-4);
  u32 B=*(u32*)(ADDR1+aMemOffset-8);
  ADDR1_END=ADDR1+A;
  ADDR2=ADDR1-(B>>24);
  B&=~0xff000000;
  ADDR3=ADDR1-B;
  u32 uncompressEnd=((u32)ADDR1_END)-((u32)aMainMemory);

  while(!(ADDR2<=ADDR3))
  {
    u32 marku8=*(--ADDR2+aMemOffset);
    //ADDR2-=1;
    int count=8;
    while( true )
    {
      count--;
      if(count<0) break;
      if(0==(marku8&0x80))
      {
        *(--ADDR1_END+aMemOffset)=*(--ADDR2+aMemOffset);
      }
      else
      {
        int u8_r12=*(--ADDR2+aMemOffset);
        int u8_r7=*(--ADDR2+aMemOffset);
        u8_r7|=(u8_r12<<8);
        u8_r7&=~0xf000;
        u8_r7+=2;
        u8_r12+=0x20;
        do
        {
          u8 realu8=*(ADDR1_END+aMemOffset+u8_r7);
          *(--ADDR1_END+aMemOffset)=realu8;
          u8_r12-=0x10;
        } while(u8_r12>=0);
      }
      marku8<<=1;
      if(ADDR2<=ADDR3) break;
    }
  }
  return uncompressEnd;
}

u32 CPatcher::SaveMask(void)
{
  u32 shift=((iParam&PATCH_SAVE_MASK)>>PATCH_SAVE_SHIFT);
  if(shift==PATCH_SAVE_NO) return 0;
  return (1<<(shift+9))-1;
}

void CPatcher::PatchMPU2(s32 anAddress)
{
  switch(GameCodeAll())
  {
    case 0x534b42: //"5116 - Keshikasu-kun Battle Castival (Japan)"
      return;
  }
  PatchMem(KArm9,anAddress,0xe1a00000); //nop
}

CDsPatcher::CDsPatcher(const char* aFileName,u32* aParams,u32 aSpeed,u32 aSizeMask): CPatcher(aFileName,aParams,aSizeMask),iSpeed(aSpeed)
{
  iPatchArm7Entry=0x23fe000;
  iPatchDMA4Entry=KMemDma;
  if(GameCodeAll()==0x4d4441||GameCodeAll()==0x323641) //ADM*/A62*: Animal Crossing - Wild World
  {
    iPatchDMA4Entry=0x1007000;
    iPatchArm7Entry=0x23ff100;
  }
  else if(IsPokemonGame())
  {
    iPatchArm7Entry=0x23fd000;
  }
  iGbaGameCode=((u32*)0x08000000)[43];
  iGbaGameCodeAll=iGbaGameCode&0x00ffffff;
  iPatchDMA4Ptr=iPatchDMA4Entry;
}

void CDsPatcher::Patch(void)
{
  PatchDumbSave();
  FixBugs();
  FixIntros(); //FIXME: this is temporary workaround. hope will be remooved in 2012 :)
  if(!IsProtection())
  {
    PatchFlashCardCheck();
  }
  if(IsLinkage())
  {
    PatchLinkage();
  }
  if(Language())
  {
    PatchLanguage(false);
  }
  if(IsSDSave()&&SaveMask())
  {
    PatchSDSave();
  }
  if(IsDMA())
  {
    PatchDMA4();
    *(u32*)0x027fff64=0; //sdState
  }
  if(IsDownloadPlay())
  {
    PatchDownloadplay();
  }
  if(IsCommon())
  {
    u32 arm7address=0;
    if(IsSoftReset())
    {
      PatchArm9(arm7address);
      *(u32*)0x27fff6c=1; //arm9ResetFlag
    }
    PatchArm7E(arm7address);
  }
  PatchiDS(); //allow ids games work on common ds.
  PatchRepatch();
}

//0x2000000 - resetpatch9 - 1k
//0x2000400 - patch7 - 3k
//0x2001000 - dma4 - 11k
//0x2003c00 - unprot_* - 1k
CDsiPatcher::CDsiPatcher(const char* aFileName,u32* aParams,u32 aSizeMask): CPatcher(aFileName,aParams,aSizeMask),iPatchArm9Entry(0x2000000)
{
  switch(Type())
  {
    case EType0:
      iPatchArm7Entry=0x2000400;
      iPatchDMA4Entry=0x2001000;
      break;
    case EType1:
      iPatchArm7Entry=0x23ff100;
      iPatchDMA4Entry=0x1007000;
      break;
    case EType2:
      iPatchArm7Entry=0x23fe000;
      iPatchDMA4Entry=0x23fc800;
      iPatchArm9Entry=0x23fc000;
      FixRam();
      break;
  }
  iPatchDMA4Ptr=iPatchDMA4Entry;
}

void CDsiPatcher::Patch(void)
{
  PatchDumbSave();
  FixBugs();
  if(!IsProtection())
  {
    PatchFlashCardCheck();
  }
  if(Language())
  {
    PatchLanguage(true);
  }
  if(IsSDSave()&&SaveMask())
  {
    PatchSDSave();
  }
  if(IsDMA())
  {
    PatchDMA4();
    *(u32*)0x027fff64=0; //sdState
  }
  if(IsCommon())
  {
    u32 arm7address=0;
    if(IsSoftReset())
    {
      PatchArm9(arm7address);
      *(u32*)0x27fff6c=1; //arm9ResetFlag
    }
    PatchArm7E(arm7address);
  }
  PatchRepatch();
}

CDsiPatcher::TType CDsiPatcher::Type(void)
{
  switch(GameCodeAll())
  {
    case 0x00505256: //Imagine Journalist
    case 0x004c4256: //Biggest Loser, The
    case 0x00565056: //Imagine Rescue Vet
    case 0x00394856: //Imagine - Champion Rider
    case 0x00414256: //Imagine - Babyz Fashion
      return EType1;
    case 0x00485856: //Katekyoo Hitman Reborn! DS - Flame Rumble XX - Chou Kessen! Real 6 Chouka
    case 0x00415249: //Pocket Monsters - White
    case 0x00425249: //Pocket Monsters - Black
    case 0x00444356: //Solatorobo
    case 0x00455456: //Tron Evolution
    case 0x00454456: //Super Kaseki Horider
      return EType2;
  }
  return EType0;
}
