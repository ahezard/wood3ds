/*
    romlauncher.cpp
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

#include "iocmn.h"
#include "romlauncher.h"
#include "../../../akloader/share/flags.h"
#include "language.h"
#include "save64m.h"
#include "exptools.h"
#include "romloader.h"
#include "sdidentify.h"
#include "cheatwnd.h"

#if defined(_STORAGE_rpg)
static bool OnlySDGame(u32 aGameCode)
{
  if(0x45444759==aGameCode) //YGDE: 2209 - Diary Girl (USA)
  {
    return false;
  }
  switch(aGameCode&0xffffff)
  {
    case 0x385959: //YY8*: 2436 - Kakushi E Mystery - Riku to Yohan - Kieta 2-mai no E (Japan)
    case 0x425841: //AXB*: 2385 - Daigassou! Band-Brothers DX (Japan)
    case 0x425855: //UXB*: 4950 - Jam with the Band (Europe) (En,Fr,De,Es,It)
    case 0x524f55: //UOR*: 3690 - Made in Ore (Japan) & 4812 - WarioWare D.I.Y. (USA) & 4913 - WarioWare - Do It Yourself (Europe) (En,Fr,De,Es,It)
    case 0x414156: //VAA*: 5054 - Eigokoro Kyoushitsu DS (Japan) (NDSi Enhanced)
    case 0x444759: //YGD*: 3453 - Winx Club - Secret Diary 2009 (Europe) (En,Fr,De,Es,It)
    case 0x565042: //BPV*: 4244 - Puzzler World (Europe) (En,Fr,De,Es,It)
      return true;
  }
  return false;
}
#endif

static SAVE_TYPE PrefillGame(u32 aGameCode)
{
  if(0x45444759==aGameCode) //YGDE: 2209 - Diary Girl (USA)
  {
    //usa game dont have 16M save
    return ST_UNKNOWN;
  }
  switch(aGameCode&0xffffff)
  {
    case 0x385959: //YY8*: 2436 - Kakushi E Mystery - Riku to Yohan - Kieta 2-mai no E (Japan)
      return ST_64K;
#if defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    case 0x533243: //C2S*: Pokemon Mystery Dungeon - Explorers of Sky
      return ST_1M;
#endif
    case 0x593241: //A2Y*: Ganbaru Watashi no Kakei Diary
    case 0x4c5a41: //AZL*: Nintendo Presents - Style Boutique
    case 0x494b42: //BKI*: Legend of Zelda, The - Spirit Tracks
    case 0x503643: //C6P*: Picross 3D
    case 0x574d49: //IMW*: Walk with Me!
    case 0x4e4259: //YBN*: 100 Classic Book Collection
    case 0x544556: //VET*: 1000 Cooking Recipes from Elle a Table
      return ST_8M;
    case 0x444759: //YGD*: Winx Club - Secret Diary 2009
    case 0x565042: //BPV*: Puzzler World
      return ST_16M;
    case 0x425841: //AXB*: Daigassou! Band-Brothers DX
    case 0x414156: //VAA*: Eigokoro Kyoushitsu DS
      return ST_64M;
  }
  return ST_UNKNOWN;
}

static u32 SaveMask(SAVE_TYPE st)
{
  u32 result;
  switch(st)
  {
    case ST_NOSAVE:
      result=PATCH_SAVE_NO;
      break;
    case ST_4K:
      result=0;
      break;
    case ST_64K:
      result=4;
      break;
    case ST_512K:
      result=7;
      break;
    case ST_1M:
      result=8;
      break;
    case ST_2M:
      result=9;
      break;
    case ST_4M:
      result=10;
      break;
    case ST_8M:
      result=11;
      break;
    case ST_16M:
      result=12;
      break;
    case ST_32M:
      result=13;
      break;
    case ST_64M:
      result=14;
      break;
    default:
      result=10; //4M default
      break;
  }
  return result;
}

static u32 SaveSize(SAVE_TYPE st)
{
  u32 result;
  switch(st)
  {
    case ST_NOSAVE:
      result=0;
      break;
    case ST_4K:
      result=512;
      break;
    case ST_64K:
      result=8192;
      break;
    case ST_512K:
      result=65536;
      break;
    case ST_1M:
      result=131072;
      break;
    case ST_2M:
      result=262144;
      break;
    case ST_4M:
      result=524288;
      break;
    case ST_8M:
      result=1048576;
      break;
    case ST_16M:
      result=2097152;
      break;
    case ST_32M:
      result=4194304;
      break;
    case ST_64M:
      result=8388608;
      break;
    default:
      result=524288; //4M default
      break;
  };
  return result;
}

TLaunchResult launchRom(const std::string& aFullPath,DSRomInfo& aRomInfo,bool aMenu)
{
  u32 flags=0;
  long cheatOffset=0; size_t cheatSize=0;
#if defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
  std::string saveName;
#endif
  if(!aRomInfo.isHomebrew())
  {
    u32 gameCode;
    memcpy(&gameCode,aRomInfo.saveInfo().gameCode,sizeof(gameCode)); //because alignment
    bool isBigSave=false;
    u32 bigSaveSize=8*1024*1024;
    u32 bigSaveMask=14;
    // reading speed setting
    std::string disk=aFullPath.substr(0,5);
    bool dma=false,protection=aRomInfo.saveInfo().isProtection(); u32 speed=0;
    int mount=0;
#if defined(_STORAGE_rpg)
    if(disk=="fat0:"||disk=="FAT0:") // if we are using internal NAND flash, use fast reading setting
    {
      speed=0xd0;
      if(OnlySDGame(gameCode))
      {
        return ELaunchSDOnly;
      }
    }
    else if(disk=="fat1:"||disk=="FAT1:") // check sd card, warn user if sd card is not suitable for running offical ds program
#endif
    {
      if(protection) speed=0x1fff;
      mount=ioSD();
      dma=aRomInfo.saveInfo().isDMA();
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
      if(dma)
#endif
      {
        flags|=PATCH_DMA;
      }
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
      else
      {
        u32 sdSpeed=sdidCheckSDSpeed(8192);
        if(sdSpeed>=0x2000)
        {
          return ELaunchSlowSD;
        }
        else
        {
          speed=(sdSpeed&0x1fff);
        }
      }
#endif
/*
      if((gameCode&0xffffff)==0x425841) //2385 - Daigassou! Band-Brothers DX (Japan)
      {
        isBigSave=true;
      }
      if((gameCode&0xffffff)==0x414156) //5054 - Eigokoro Kyoushitsu DS (Japan) (NDSi Enhanced)
      {
        isBigSave=true;
      }
*/
      if((gameCode&0xffffff)==0x425855) //4950 - Jam with the Band (Europe) (En,Fr,De,Es,It)
      {
        isBigSave=true;
        bigSaveSize=32*1024*1024;
        bigSaveMask=0;
      }
      if((gameCode&0xffffff)==0x524f55) //3690 - Made in Ore (Japan) & 4812 - WarioWare D.I.Y. (USA)
      {
        isBigSave=true;
        bigSaveSize=32*1024*1024;
        bigSaveMask=0;
      }
    }

#if defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    saveName=cSaveManager::generateSaveName(aFullPath,aRomInfo.saveInfo().getSlot());
#endif
    if(isBigSave)
    {
      isBigSave=cSaveManager::initializeSaveFile(aFullPath,aRomInfo.saveInfo().getSlot(),bigSaveSize);
      if(!isBigSave) return ELaunchNoFreeSpace;
    }
    if
    (
      isBigSave
#if defined(_STORAGE_rpg)
      &&cSave64m::GenerateSaveMap(aFullPath,aRomInfo.saveInfo().getSlot())
#endif
    )
    {
      flags|=PATCH_SD_SAVE|(bigSaveMask<<PATCH_SAVE_SHIFT);
#if defined(_STORAGE_rpg)
      saveManager().restoreSaveData(aFullPath.c_str(),ST_NOSAVE,aRomInfo.saveInfo().getSlot());
      if(!dma) speed=0x1fff;
#elif defined(_STORAGE_r4)
      saveManager().saveLastInfo(aFullPath);
      if(!bigSaveMask) //MiO
      {
        if(!cSave64m::GenerateSaveMap(saveName)) return ELaunchNoFreeSpace;
      }
#elif defined(_STORAGE_ak2i)
      saveManager().saveLastInfo(aFullPath);
      if(!dma) speed=0x1fff;
#elif defined(_STORAGE_r4idsn)
      saveManager().saveLastInfo(aFullPath);
#endif
    }
    else
    {
      // restore save data only for offical programs
      SAVE_TYPE st=(SAVE_TYPE)aRomInfo.saveInfo().saveType;
      if(ST_UNKNOWN==st) st=ST_AUTO;
      SAVE_TYPE st_new=PrefillGame(gameCode);
      if(st_new!=ST_UNKNOWN) //special save size
      {
        if(st_new!=st)
        {
          st=st_new;
          aRomInfo.saveInfo().saveType=st;
          saveManager().updateCustomSaveList(aRomInfo.saveInfo());
        }
      }
#if defined(_STORAGE_rpg)
      if(ioSD()==mount&&(aRomInfo.saveInfo().isSDSave()||OnlySDGame(gameCode)))
#endif
      {
        if
        (
          cSaveManager::initializeSaveFile(aFullPath,aRomInfo.saveInfo().getSlot(),SaveSize(st))
#if defined(_STORAGE_rpg)
          &&cSave64m::GenerateSaveMap(aFullPath,aRomInfo.saveInfo().getSlot())
#endif
        )
        {
          flags|=PATCH_SD_SAVE|(SaveMask(st)<<PATCH_SAVE_SHIFT);
#if defined(_STORAGE_rpg)
          saveManager().restoreSaveData(aFullPath.c_str(),ST_NOSAVE,aRomInfo.saveInfo().getSlot());
          if(!dma) speed=0x1fff;
#elif defined(_STORAGE_r4) || defined(_STORAGE_r4idsn)
          saveManager().saveLastInfo(aFullPath);
#elif defined(_STORAGE_ak2i)
          saveManager().saveLastInfo(aFullPath);
          if(!dma) speed=0x1fff;
#endif
        }
        else
        {
          return ELaunchNoFreeSpace;
        }
      }
#if defined(_STORAGE_rpg)
      else
      {
        if(!saveManager().restoreSaveData(aFullPath.c_str(),st,aRomInfo.saveInfo().getSlot()))
        {
          return ELaunchRestoreFail;
        }
      }
#endif
    }
    __NDSHeader->cardControl13=0x00406000|speed;
    //3in1 support
    if(gameCode==0x4a524255||gameCode==0x50524255)
    {
      expansion().SoftReset();
      cExpansion::SetRompage(0x300);
      cExpansion::OpenNorWrite();
      cExpansion::EnableBrowser();
    }
    if(aRomInfo.saveInfo().getRumble())
    {
      expansion().SoftReset();
      cExpansion::SetShake(0xEF+aRomInfo.saveInfo().getRumble());
    }
    if(aRomInfo.saveInfo().isDownloadPlay()) flags|=PATCH_DOWNLOAD_PLAY;
    if(aRomInfo.saveInfo().isCheat())
    {
      u32 gameCode,crc32;
      if(cCheatWnd::romData(aFullPath,gameCode,crc32))
      {
        FILE* dat=fopen(SFN_CHEATS,"rb");
        if(dat)
        {
          if(cCheatWnd::searchCheatData(dat,gameCode,crc32,cheatOffset,cheatSize))
          {
            flags|=PATCH_CHEATS;
          }
          fclose(dat);
        }
      }
    }
    if(aRomInfo.saveInfo().isSoftReset()) flags|=PATCH_SOFT_RESET;
    if(expansion().Rampage()==cExpansion::EPsramPage) flags|=PATCH_PSRAM;
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
    if(protection)
    {
      u32 bytesPerCluster;
      if(ELM_ClusterSizeFromDisk(mount,&bytesPerCluster)&&cSaveManager::generateProtectionFix(aFullPath,bytesPerCluster))
      {
        flags|=PATCH_PROTECTION;
      }
    }
#endif
    if(aRomInfo.saveInfo().isLinkage()) flags|=PATCH_LINKAGE;
    u8 language=aRomInfo.saveInfo().getLanguage();
    if(language) flags|=(language<<PATCH_LANGUAGE_SHIFT)&PATCH_LANGUAGE_MASK;
  }
  else
  {
    if(!aMenu) saveManager().saveLastInfo(aFullPath);
    const u16 lameboy[]={'L','a','m','e','b','o','y'};
    if(memcmp(aRomInfo.banner().titles[0],lameboy,sizeof(lameboy))==0)
    {
      expansion().SoftReset();
      cExpansion::SetShake(0xF0);
    }
    if(gs().homebrewreset) flags|=PATCH_SOFT_RESET;
  }
#if defined(_STORAGE_rpg)
  loadRom(aFullPath,flags,cheatOffset,cheatSize);
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
  loadRom(aFullPath,saveName,flags,cheatOffset,cheatSize);
#endif
  return ELaunchRomOk;
}

void autoLaunchRom(const std::string& aFullPath)
{
  DSRomInfo rominfo;
  rominfo.MayBeDSRom(aFullPath);
  if(rominfo.isDSRom()) launchRom(aFullPath,rominfo,false);
}
