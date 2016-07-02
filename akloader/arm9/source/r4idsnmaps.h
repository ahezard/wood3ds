/*
    r4idsnmaps.h
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

#ifndef __R4IDSNMAPS_H__
#define __R4IDSNMAPS_H__

cRPGMaps::cRPGMaps(const char* aPath,const char* aSave): iClusterSize(0),iFileDes(-1)
{
  iIsSDHC=isSDHC();
  BuildFATMap(aPath,false);
  BuildFATMap(aSave,true);
}

void cRPGMaps::Write(u32 aValue)
{
  Check();
  if((iMapCurrent&0xffff)==0) ioRpgEraseFlash(iMapCurrent);
  ioRpgWriteFlash(iMapCurrent,&aValue,4);
  iMapCurrent+=4;
}

void cRPGMaps::BuildFATMap(const char* aPath,bool aType)
{
  iFileDes=open(aPath,O_RDONLY);
  if(iFileDes>=0)
  {
    iMapStart=aType?NOR_FAT2_START:NOR_FAT1_START;
    iMapEnd=aType?NOR_FAT2_END:NOR_FAT1_END;
    iMapCurrent=iMapStart;
    IterateFileCluster();
    ELM_ClusterSizeFromHandle(iFileDes,&iClusterSize);
    close(iFileDes);
  }
}

void r4idsnBuildMaps(const char* path,const char* save)
{
  cRPGMaps maps(path,save);
  u32 clusterSize=maps.ClusterSize();
  u32 clusterSizeShift=0;
  while(clusterSize>512)
  {
    clusterSize>>=1;
    clusterSizeShift++;
  }
  ioRpgSetMapTableAddress(MTN_MAP_ROM,NOR_FAT1_START);
  ioRpgSetMapTableAddress(MTN_MAP_SAVE,NOR_FAT2_START);
  ioRpgSetDeviceStatus(clusterSizeShift,isSDHC());
  dbg_printf("sd cluster size %d\n", clusterSizeShift);
  ioRpgWaitCmdBusy(true);
}

#endif
