/*
    iowood.c
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

#include <ior4idsn.h>
#define NAND 0
#define SD   1

void NandFast(void)
{
}

void NandFlush(void)
{
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
      return sddInitSD();
    case SD:
      return FALSE;
  }
  return FALSE;
}

int ioWoodStatus(unsigned char aDrive)
{
  if(aDrive==NAND) return TRUE;
  return FALSE;
}

int ioWoodRead(unsigned char aDrive,unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  if(aDrive==NAND)
  {
    sddReadBlocks(aSector,aCount,aBuffer);
    return TRUE;
  }
  return FALSE;
}

int ioWoodWrite(unsigned char aDrive,const unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  if(aDrive==NAND)
  {
    sddWriteBlocks(aSector,aCount,aBuffer);
    return TRUE;
  }
  return FALSE;
}

void ioWoodSync(unsigned char aDrive)
{
}
