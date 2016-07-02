/*
    save64m.cpp
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

#if defined(_STORAGE_rpg) || defined(_STORAGE_r4)

#include "save64m.h"
#include <fcntl.h>
#include <unistd.h>
#include "dbgtool.h"
#include "../../share/memtool.h"
#if defined(_STORAGE_rpg)
#include "savemngr.h"
#include <iorpg.h>
#elif defined(_STORAGE_r4)
#include <ior4.h>
#endif

void cSave64m::IterateFileCluster(int aFileDes,u32* aFatMap)
{
  u32 sectorsPerCluster;
  u32 shift=isSDHC()?0:9;
  if(ELM_SectorsPerClusterFromHandle(aFileDes,&sectorsPerCluster))
  {
    u32 cluster=1;
    while(true)
    {
      u32 sector;
      cluster=ELM_GetFAT(aFileDes,cluster,&sector);
      if(!cluster) break;
      for(u32 ii=0;ii<sectorsPerCluster;ii++)
        *aFatMap++=(sector+ii)<<shift;
    }
  }
}

bool cSave64m::GenerateSaveMapInternal(const char* aPath,u32* aFatMap)
{
  int fd=open(aPath,O_RDONLY);
  if(fd<0) return false;
  IterateFileCluster(fd,aFatMap);
  close(fd);
  return true;
}

#if defined(_STORAGE_rpg)
bool cSave64m::GenerateSaveMap(const std::string& aRomFilename,u8 aSlot)
{
  bool res=false;
  u32* buffer=new u32[KMaxSave];
  fillMemory(buffer,KMaxSave*sizeof(u32),0xffffffff);
  std::string saveFilename=cSaveManager::generateSaveName(aRomFilename,aSlot);
  if(GenerateSaveMapInternal(saveFilename.c_str(),buffer))
  {
    u32 saveMapAddress=SRAM_SAVEDATA_START;
    for(u32 ii=0;ii<KMaxSave;++ii)
    {
      ioRpgWriteSram(saveMapAddress+ii*4,buffer+ii,sizeof(u32));
    }
    res=true;
  }
  delete[] buffer;
  return res;
}
#elif defined(_STORAGE_r4)
bool cSave64m::GenerateSaveMap(std::string& aSaveName)
{
  bool success=false;
  std::string result=aSaveName;
  u32* buffer=new u32[KMaxSave];
  fillMemory(buffer,KMaxSave*sizeof(u32),0xffffffff);
  if(GenerateSaveMapInternal(aSaveName.c_str(),buffer))
  {
    result+=".fix";
    int f=open(result.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
    if(f>=0)
    {
      if(write(f,buffer,KMaxSave*sizeof(u32))==KMaxSave*sizeof(u32))
      {
        success=true;
        aSaveName=result;
      }
      close(f);
    }
  }
  return success;
}
#endif

#endif
