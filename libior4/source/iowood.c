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

#include <nds.h>
bool startup(void);
bool isInserted(void);
bool readSectors(u32 sector,u32 numSecs,void* buffer);
bool writeSectors(u32 sector,u32 numSecs,void* buffer);
bool clearStatus(void);
bool shutdown(void);
#define NAND 0
#define SD   1

void NandFast(void)
{
}

void NandFlush(void)
{
}

int ioWoodInit(unsigned char aDrive)
{
  if(aDrive==NAND) return (startup()&&isInserted());
  return FALSE;
}

int ioWoodStatus(unsigned char aDrive)
{
  if(aDrive==NAND) return isInserted();
  return FALSE;
}

int ioWoodRead(unsigned char aDrive,unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  if(aDrive==NAND) return readSectors(aSector,aCount,aBuffer);
  return FALSE;
}

int ioWoodWrite(unsigned char aDrive,const unsigned char* aBuffer,unsigned long aSector,unsigned char aCount)
{
  if(aDrive==NAND) return writeSectors(aSector,aCount,(void*)aBuffer);
  return FALSE;
}

void ioWoodSync(unsigned char aDrive)
{
  if(aDrive==NAND) clearStatus();
}
