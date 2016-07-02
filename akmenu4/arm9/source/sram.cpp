/*
    sram.cpp
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

#include "sram.h"
#include "exptools.h"
#include <malloc.h>
#include <string.h>
#include "progresswnd.h"
#include "language.h"
#include <elm.h>

bool cSram::SaveFileName(const char* romName,char* saveName)
{
  saveName[0]=0;
  if(strlen(romName)>251) return false;
  strcpy(saveName,romName);
  strcat(saveName,".sav");
  return true;
}

void cSram::BlankSram(u16 aStartPage)
{
  u8* buf=(u8*)malloc(SRAM_PAGE_SIZE);
  if(buf)
  {
    cExpansion::OpenNorWrite();
    memset(buf,0,SRAM_PAGE_SIZE);
    for(u32 i=0;i<SRAM_SAVE_PAGES;i++)
    {
      expansion().SetRampage(i+aStartPage);
      cExpansion::WriteSram(0x0A000000,buf,SRAM_PAGE_SIZE);
    }
    free(buf);
  }
  cExpansion::CloseNorWrite();
}

void cSram::LoadSramFromFile(const char* romName,u16 aStartPage)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;

  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    sSaveInfo saveInfo;
    ProcessRAW(saveFile,saveInfo);
    u8* buf=(u8*)malloc(saveInfo.size);
    if(buf)
    {
      BlankSram(aStartPage);
      progressWnd().setTipText(LANG("progress window","gba save load" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      memset(buf,0,saveInfo.size);
      size_t ret=fread(buf,saveInfo.size,1,saveFile);
      progressWnd().setPercent(10);
      if(ret==1&&!ferror(saveFile))
      {
        int page=saveInfo.offset/SRAM_PAGE_SIZE,offset=saveInfo.offset%SRAM_PAGE_SIZE,size=saveInfo.size,bufOffset=0,ii=0;
        while(page<SRAM_SAVE_PAGES&&size>0)
        {
          expansion().SetRampage(page+aStartPage);
          int maxSize=SRAM_PAGE_SIZE-offset,bufSize=(size>maxSize)?maxSize:size;
          cExpansion::WriteSram(0x0A000000,buf+bufOffset,bufSize);
          bufOffset+=bufSize;
          size-=bufSize;
          offset=0;
          page++;
          if(ii++%4==0) progressWnd().setPercent(10+bufOffset*90/saveInfo.size);
        }
      }
      progressWnd().setPercent(100);
      progressWnd().hide();
      free(buf);
    }
    fclose(saveFile);
    expansion().SetRampage(0);
  }
  else
  {
    BlankSram(aStartPage);
  }
}

void cSram::SaveSramToFile(const char* romName,u16 aStartPage)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;
  sSaveInfo saveInfo;
  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    fread(&saveInfo,sizeof(saveInfo),1,saveFile);
    if(saveInfo.marker!=SAVE_GBA_HEADER_MAGIC) //raw save
    {
      saveInfo.marker=SAVE_GBA_HEADER_MAGIC;
      saveInfo.offset=0;
      fseek(saveFile,0,SEEK_END);
      saveInfo.size=ftell(saveFile);
      saveInfo.header=0;
    }
    fclose(saveFile);
    saveFile=NULL;
  }
  NormalizeSize(saveInfo);
  NandFast();
  saveFile=fopen(saveName,"wb");
  if(saveFile)
  {
    fwrite(&saveInfo,sizeof(saveInfo),1,saveFile);
    progressWnd().setTipText(LANG("progress window","gba save store" ));
    progressWnd().show();
    progressWnd().setPercent(0);
    u8* buf=SaveSramToMemory(aStartPage,saveInfo,true);
    if(buf)
    {
      fwrite(buf,saveInfo.size,1,saveFile);
      free(buf);
    }
    progressWnd().setPercent(100);
    progressWnd().hide();
    fclose(saveFile);
  }
  NandFlush();
}

u8* cSram::SaveSramToMemory(u16 aStartPage,sSaveInfo& aSaveInfo,bool aShowProgress)
{
  u8* buf=(u8*)malloc(aSaveInfo.size);
  if(buf)
  {
    cExpansion::OpenNorWrite();
    memset(buf,0,aSaveInfo.size);

    int page=aSaveInfo.offset/SRAM_PAGE_SIZE,offset=aSaveInfo.offset%SRAM_PAGE_SIZE,size=aSaveInfo.size,bufOffset=0,ii=0;
    while(page<SRAM_SAVE_PAGES&&size>0)
    {
      expansion().SetRampage(page+aStartPage);
      int maxSize=SRAM_PAGE_SIZE-offset,bufSize=(size>maxSize)?maxSize:size;
      cExpansion::ReadSram(0x0A000000,buf+bufOffset,bufSize);
      bufOffset+=bufSize;
      size-=bufSize;
      offset=0;
      page++;
      if(aShowProgress&&ii++%4==0) progressWnd().setPercent(bufOffset*90/aSaveInfo.size);
    }
    cExpansion::CloseNorWrite();
  }
  return buf;
}

void cSram::CreateDefaultFile(const char* romName,u32 size)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;
  sSaveInfo saveInfo(size);
  NormalizeSize(saveInfo);
  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    fclose(saveFile);
  }
  else
  {
    NandFast();
    FILE* saveFile=fopen(saveName,"wb");
    if(saveFile)
    {
      fwrite(&saveInfo,sizeof(saveInfo),1,saveFile);
      fclose(saveFile);
    }
    NandFlush();
  }
}

void cSram::NormalizeSize(sSaveInfo& anInfo)
{
  if((anInfo.offset+anInfo.size)>SRAM_SAVE_SIZE)
  {
    if(anInfo.offset<=SRAM_SAVE_SIZE)
    {
      anInfo.size=SRAM_SAVE_SIZE-anInfo.offset;
    }
    else
    {
      anInfo.size=0;
    }
  }
}

void cSram::ProcessRAW(FILE* aFile,sSaveInfo& anInfo)
{
  fread(&anInfo,sizeof(anInfo),1,aFile);
  if(anInfo.marker!=SAVE_GBA_HEADER_MAGIC) //raw save
  {
    anInfo.marker=SAVE_GBA_HEADER_MAGIC;
    anInfo.offset=0;
    fseek(aFile,0,SEEK_END);
    anInfo.size=ftell(aFile);
    fseek(aFile,0,SEEK_SET);
    anInfo.header=0;
  }
  NormalizeSize(anInfo);
}
