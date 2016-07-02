/*
    gbaloader.cpp
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

#include <nds.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "exptools.h"
#include "../../share/fifotool.h"
#include "progresswnd.h"
#include "language.h"
#include "sram.h"
#include "gbapatcher.h"
#include "gbaloader.h"
#include <elm.h>

#define LEN 0x100000
#define LEN_NOR 0x8000
#define MAX_PSRAM 0x1000000
#define MAX_NOR 0x2000000

// writers >

class CGbaWriterPSRAM: public CGbaWriter
{
  public:
    void Open(void);
    void Write(u32 address,const u8* buffer,u32 size);
    void Close(void);
    u32 MaxSize(void);
};

void CGbaWriterPSRAM::Open(void)
{
}

void CGbaWriterPSRAM::Write(u32 address,const u8* buffer,u32 size)
{
  cExpansion::WritePSRAM(address,buffer,size);
}

void CGbaWriterPSRAM::Close(void)
{
}

u32 CGbaWriterPSRAM::MaxSize(void)
{
  return 16*1024*1024;
}

class CGbaWriterNor: public CGbaWriter
{
  private:
    u8* iBuffer;
    u32 iCurPage;
  private:
    enum
    {
      EPageSize=0x40000,
      EGuardSize=256
    };
  private:
    void Commit(void);
    void Fill(void);
  public:
    void Open(void);
    void Write(u32 address,const u8* buffer,u32 size);
    void Close(void);
    u32 MaxSize(void);
};

void CGbaWriterNor::Commit(void)
{
  u8* backupBuffer=NULL;
  if(iCurPage!=0)
  {
    backupBuffer=(u8*)malloc(0x4000);
    if(backupBuffer) memcpy(backupBuffer,(void*)0x08004000,0x4000);
  }
  expansion().Block_Erase(iCurPage);
  for(u32 ii=0;ii<8;ii++)
    expansion().WriteNorFlash(iCurPage+ii*0x8000,iBuffer+ii*0x8000,0x8000);
  if(backupBuffer&&memcmp(backupBuffer,(void*)0x08004000,0x4000)!=0)
  {
    expansion().WriteNorFlash(0x4000,backupBuffer,0x4000);
    free(backupBuffer);
  }
}

void CGbaWriterNor::Fill(void)
{
  memcpy(iBuffer,(void*)(0x08000000+iCurPage),EPageSize+EGuardSize);
}

void CGbaWriterNor::Open(void)
{
  iCurPage=0x666;
  iBuffer=(u8*)malloc(EPageSize+EGuardSize);
}

void CGbaWriterNor::Write(u32 address,const u8* buffer,u32 size)
{
  if(iBuffer)
  {
    if(iCurPage==0x666) //first time
    {
      iCurPage=address&~0x3ffff;
      Fill();
    }
    u32 newPage=address&~0x3ffff;
    if(newPage==iCurPage)
    {
      memcpy(iBuffer+(address-iCurPage),buffer,size);
    }
    else
    {
      Commit();
      u8 overlap[EGuardSize]; bool overlaped=false;
      memcpy(overlap,iBuffer+EPageSize,EGuardSize);
      if(memcmp(overlap,(void*)(0x08000000+iCurPage+EPageSize),EGuardSize))
      {
        if(newPage-iCurPage==EPageSize)
          overlaped=true;
        else
        {
          iCurPage+=EPageSize;
          Fill();
          memcpy(iBuffer,overlap,EGuardSize);
          Commit();
        }
      }
      iCurPage=newPage;
      Fill();
      if(overlaped) memcpy(iBuffer,overlap,EGuardSize);
      Write(address,buffer,size);
    }
  }
}

void CGbaWriterNor::Close(void)
{
  if(iBuffer)
  {
    if(iCurPage!=0x666)
    {
      Commit();
    }
    free(iBuffer);
  }
}

u32 CGbaWriterNor::MaxSize(void)
{
  return 32*1024*1024;
}

// writers <

CGbaLoader::CGbaLoader(const std::string& aFileName): iFileName(aFileName)
{
}

bool CGbaLoader::Load(bool aForce,bool aNotStart)
{
  if(!expansion().IsValid()) return false;
  bool load=false,nor=false;
  struct stat st;
  if(-1==stat(iFileName.c_str(),&st))
  {
    return false;
  }
  iSize=st.st_size;
  if(iSize>MAX_PSRAM)
  {
    nor=true;
  }
  else if(iSize>MAX_NOR)
  {
    akui::messageBox( NULL, LANG("gba warn", "title"), LANG("gba warn", "text"), MB_OK );
    return false;
  }

  FILE* gbaFile=fopen(iFileName.c_str(),"rb");
  if(gbaFile)
  {
    sGBAHeader header;
    fread(&header,1,sizeof(header),gbaFile);
    fclose(gbaFile);
    if(header.is96h==0x96)
    {
      load=LoadInternal(nor,aForce);
    }
  }
  if(!aNotStart&&load&&!CheckLink())
  {
    StartGBA();
  }
  return load;
}

bool CGbaLoader::CheckPSRAM(u32 aSize)
{
  return (aSize>MAX_PSRAM)?false:true;
}

void CGbaLoader::StartGBA(void)
{
  LoadBorder();
  ELM_Unmount();
  BootGBA();
  while(true) swiWaitForVBlank();
}

bool CGbaLoader::CheckLink(void)
{
  u32* data=(u32*)0x08000000;
  if(data[0]==0xffffffff&&data[43]==0x4a42425a) return true;
  return false;
}

void CGbaLoader::LoadBorder(void)
{
  videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, VRAM_C_SUB_BG_0x06200000, VRAM_D_LCD);

  // for the main screen
  REG_BG3CNT = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_WRAP_OFF;
  REG_BG3PA = 1 << 8; //scale x
  REG_BG3PB = 0; //rotation x
  REG_BG3PC = 0; //rotation y
  REG_BG3PD = 1 << 8; //scale y
  REG_BG3X = 0; //translation x
  REG_BG3Y = 0; //translation y

  memset((void*)BG_BMP_RAM(0),0,0x18000);
  memset((void*)BG_BMP_RAM(8),0,0x18000);

  cBMP15 frameBMP=createBMP15FromFile(SFN_GBAFRAME);
  if(frameBMP.valid()&&frameBMP.width()==SCREEN_WIDTH&&frameBMP.height()==SCREEN_HEIGHT)
  {
    DC_FlushRange(frameBMP.buffer(),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    dmaCopy(frameBMP.buffer(),(void*)BG_BMP_RAM(0),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    dmaCopy(frameBMP.buffer(),(void*)BG_BMP_RAM(8),SCREEN_WIDTH*SCREEN_HEIGHT*2);
  }
}

void CGbaLoader::BootGBA(void)
{
  sysSetBusOwners(BUS_OWNER_ARM7,BUS_OWNER_ARM7);
  if(PersonalData->gbaScreen)
    REG_POWERCNT=1;
  else
    REG_POWERCNT=(POWER_SWAP_LCDS|1)&0xffff;
  fifoSendValue32(FIFO_USER_01,MENU_MSG_GBA);
}

bool CGbaLoader::LoadPSRAM(void)
{
  bool load=false;
  int gbaFile=open(iFileName.c_str(),O_RDONLY);
  if(gbaFile>=0)
  {
    u8* buf=(u8*)malloc(LEN);
    if(buf)
    {
      REG_EXMEMCNT&=~0x0880;
      cExpansion::SetRompage(381);
      cExpansion::OpenNorWrite();
      cExpansion::SetSerialMode();

      progressWnd().setTipText(LANG("progress window","gba load" ));
      progressWnd().show();
      progressWnd().setPercent(0);

      for(u32 address=0;address<iSize&&address<MAX_PSRAM;address+=LEN)
      {
        memset(buf,0xff,LEN);
        read(gbaFile,buf,LEN);
        cExpansion::WritePSRAM(address,buf,LEN);
        progressWnd().setPercent(address*100/iSize);
      }

      progressWnd().setPercent(100);
      progressWnd().hide();

      CGbaWriterPSRAM writer;
      u32 saveSize=CGbaPatcher(iSize,&writer,(u32*)0x08060000,cExpansion::EPsramPage).Patch();
      cSram::CreateDefaultFile(iFileName.c_str(),saveSize);

      cExpansion::CloseNorWrite();
      load=true;

      free(buf);
    }
    close(gbaFile);
  }
  return load;
}

bool CGbaLoader::LoadNor(void)
{
  bool load=false;
  FILE* gbaFile=fopen(iFileName.c_str(),"rb");
  if(gbaFile)
  {
    u8* buf=(u8*)malloc(LEN_NOR);
    if(buf)
    {
      //erase
      cExpansion::OpenNorWrite();
      cExpansion::SetSerialMode();
      progressWnd().setTipText(LANG("progress window","erase nor" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      for(u32 address=0;address<iSize&&address<MAX_NOR;address+=0x40000)
      {
        expansion().Block_Erase(address);
        progressWnd().setPercent(address*100/iSize);
      }
      progressWnd().setPercent(100);
      progressWnd().hide();
      //write
      progressWnd().setTipText(LANG("progress window","gba load" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      for(u32 address=0;address<iSize&&address<MAX_NOR;address+=LEN_NOR)
      {
        memset(buf,0xff,LEN_NOR);
        fread(buf,LEN_NOR,1,gbaFile);
        expansion().WriteNorFlash(address,buf,LEN_NOR);
        progressWnd().setPercent(address*100/iSize);
      }
      progressWnd().setPercent(100);
      progressWnd().hide();

      CGbaWriterNor writer;
      u32 saveSize=CGbaPatcher(iSize,&writer,(u32*)0x08000000,cExpansion::ENorPage).Patch();
      cSram::CreateDefaultFile(iFileName.c_str(),saveSize);
/*
  FILE *log;
  log=fopen("fat0:/test.bin","wb");
  fwrite((void*)0x08000000,iSize,1,log);
  fclose(log);
// */
      cExpansion::CloseNorWrite();
      load=true;
      free(buf);
    }
    fclose(gbaFile);
  }
  return load;
}

void CGbaLoader::InitNor(void)
{
  cExpansion::SetRompage(0);
  expansion().SetRampage(cExpansion::ENorPage);
}

void CGbaLoader::InitPSRAM(void)
{
  cExpansion::SetRompage(384);
  expansion().SetRampage(cExpansion::EPsramPage);
}

bool CGbaLoader::StoreOldSave(std::string& aFileName)
{
  bool res=false;
  const u8 sign[]="ACEKARD R.P.G GBA SIGN*";
  u8 buffer[sizeof(sign)];
  expansion().SetRampage(0);
  cExpansion::ReadSram(0x0A000000,buffer,sizeof(buffer));
  if(memcmp(buffer,sign,sizeof(buffer))) //old save
  {
    if(aFileName!="")
    {
      cSram::SaveSramToFile(aFileName.c_str(),0);
      res=true;
    }
    expansion().SetRampage(0);
    cExpansion::WriteSram(0x0A000000,sign,sizeof(sign));
  }
  return res;
}

bool CGbaLoader::LoadInternal(bool nor,bool force)
{
  bool load=false;
  expansion().SoftReset();
  std::string oldFile,oldFileNOR;
  CIniFile f;
  oldFile=f.LoadIniFile(SFN_LAST_GBA_SAVEINFO)?f.GetString("Save Info","lastLoaded",""):"";
  oldFileNOR=f.LoadIniFile(SFN_LAST_GBA_SAVEINFO)?f.GetString("Save Info","lastLoadedNOR",""):"";
  if(StoreOldSave(oldFile))
  {
    oldFile="";
    f.SetString("Save Info","lastLoaded","");
    f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
  }
  u32 state=0;
  if(nor) state|=1;
  if(oldFile==iFileName) state|=2;
  if(oldFileNOR==iFileName) state|=4;
  if(force) state|=8;

  switch(state)
  {
    case 0:
      if(oldFile!="") cSram::SaveSramToFile(oldFile.c_str(),cExpansion::EPsramPage);
      cSram::LoadSramFromFile(iFileName.c_str(),cExpansion::EPsramPage);
      f.SetString("Save Info","lastLoaded",iFileName);
      f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
      load=LoadPSRAM();
      InitPSRAM();
      break;
    case 10:
    case 11:
      if(oldFile!="") cSram::SaveSramToFile(oldFile.c_str(),cExpansion::EPsramPage);
      f.SetString("Save Info","lastLoaded","");
    case 1:
    case 8:
    case 9:
      if(oldFileNOR!="") cSram::SaveSramToFile(oldFileNOR.c_str(),cExpansion::ENorPage);
      cSram::LoadSramFromFile(iFileName.c_str(),cExpansion::ENorPage);
      f.SetString("Save Info","lastLoadedNOR",iFileName);
      f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
      load=LoadNor();
      InitNor();
      break;
    case 2:
    case 3:
      load=LoadPSRAM();
      InitPSRAM();
      break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 12:
    case 13:
    case 14:
    case 15:
      load=true;
      InitNor();
      break;
    default:
      expansion().SoftReset();
      break;
  }
  return load;
}
