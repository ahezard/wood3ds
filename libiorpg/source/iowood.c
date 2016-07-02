/*
    iowood.c
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

#include <iorpg.h>
#define NAND 0
#define SD   1

static bool gNandFast=false;

void NandFast(void)
{
  gNandFast=true;
}

void NandFlush(void)
{
  gNandFast=false;
  ndFinishPartialCopy();
}

static bool isInserted(void)
{
  u32 cmd[2]={0xB8000000,0x00000000};
  u32 data=0;
  ioRpgSendCommand(cmd,4,0,&data);
  return (0x00000fc2==data);
}

int ioWoodInit(unsigned char aDrive)
{
  switch(aDrive)
  {
    case NAND:
      if(!isInserted()) break;
      return ndInitNAND();
    case SD:
      return sddInitSD();
  }
  return FALSE;
}

int ioWoodStatus(unsigned char aDrive)
{
  return TRUE;
}

int ioWoodRead(unsigned char aDrive,unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  switch(aDrive)
  {
    case NAND:
      {
        bool noError=true;
        u8 retry=4;
        do
        {
          ndReadPages(aSector<<9,aCount,aBuffer);
          noError=ndCheckError();
        } while(!noError&&--retry);
        return noError?TRUE:FALSE;
      }
    case SD:
      sddReadBlocks(aSector,aCount,aBuffer);
      return TRUE;
  }
  return FALSE;
}

int ioWoodWrite(unsigned char aDrive,const unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  switch(aDrive)
  {
    case NAND:
      if(gNandFast)
        ndWritePages(aSector<<9,aCount,aBuffer);
      else
        ndWritePagesSafe(aSector<<9,aCount,aBuffer);
      return TRUE;
    case SD:
      sddWriteBlocks(aSector,aCount,aBuffer);
      return TRUE;
  }
  return FALSE;
}

void ioWoodSync(unsigned char aDrive)
{
  if(aDrive==NAND)
  {
    ndFinishPartialCopy();
  }
}
