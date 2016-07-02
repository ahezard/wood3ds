/*
    patches_ar.cpp
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
#include "dbgtool.h"
#include "reset.h"
#include <fcntl.h>
#include <unistd.h>

u32 CPatcher::LoadARCodeTable(u32 aPatchEntry7,u32 aPatchEntry7End)
{
  u32 codeTableStart=(aPatchEntry7End+7)&~7;
  u32* codeTable=(u32*)codeTableStart;
  u32* ccode=(u32*)0x06040000;
  if(iCheatSize<=0x40000)
  {
    int fc=open("fat0:/__rpg/cheats/usrcheat.dat",O_RDONLY);
    if(fc>=0)
    {
      lseek(fc,iCheatOffset,SEEK_SET);
      u32 size=iCheatSize;
      u8* target=(u8*)0x06040000;u8* source=(u8*)0x23b0000;
      while(size)
      {
        u32 tail=size;
        if(tail>0x10000) tail=0x10000;
        read(fc,source,tail);
        copyMemoryITCM(source,target,tail);
        target+=tail;
        size-=tail;
      }
      close(fc);
    }
  }
  ccode=(u32*)(((u32)ccode+strlen((char*)ccode)+4)&~3);
  u32 cheatCount=*ccode;
  cheatCount&=0x0fffffff;
  ccode+=9;

  u32 cc=0;
  while(cc<cheatCount)
  {
    u32 folderCount=1;
    char* folderName=NULL;
    char* folderNote=NULL;
    if((*ccode>>28)&1)
    {
      folderCount=*ccode&0x00ffffff;
      folderName=(char*)((u32)ccode+4);
      folderNote=(char*)((u32)folderName+strlen(folderName)+1);
      cc++;
      ccode=(u32*)(((u32)folderName+strlen(folderName)+1+strlen(folderNote)+1+3)&~3);
    }

    for(size_t ii=0;ii<folderCount;++ii)
    {
      char* cheatName=(char*)((u32)ccode+4);
      char* cheatNote=(char*)((u32)cheatName+strlen(cheatName)+1);
      u32* cheatData=(u32*)(((u32)cheatNote+strlen(cheatNote)+1+3)&~3);
      u32 cheatDataLen=(*cheatData++)>>1;

      if(cheatDataLen&&(*ccode&0xff000000))
      {
        u32 code,value;
        u32 skip=0;
        u32 D4Type=0;
        bool custom_asm=false;

        for(size_t jj=0;jj<cheatDataLen;++jj)
        {
          code=cheatData[jj*2];
          value=cheatData[jj*2+1];
          if(code==0x023FE424)
          {
            skip=1;
            if(value==0xE0833004) //Default
            {
              dbg_printf("D4 ADD code type\n");
              D4Type=0;
            }
            else if(value==0xE1833004)
            {
              dbg_printf("D4 ORR code type\n");
              D4Type=1;
            }
            else if(value==0xE0033004)
            {
              dbg_printf("D4 AND code type\n");
              D4Type=2;
            }
            else if(value==0xE0233004)
            {
              dbg_printf("D4 XOR code type\n");
              D4Type=3;
            }
            else if(value==0xE1A03413)
            {
              dbg_printf("D4 LSL code type\n");
              D4Type=4;
            }
            else if(value==0xE1A03433)
            {
              dbg_printf("D4 LSR code type\n");
              D4Type=5;
            }
            else if(value==0xE1A03733)
            {
              dbg_printf("D4 ROR code type\n");
              D4Type=6;
            }
            else if(value==0xE1A03533)
            {
              dbg_printf("D4 ASR code type\n");
              D4Type=7;
            }
            else if(value==0xE0030493)
            {
              dbg_printf("D4 MUL code type\n");
              D4Type=8;
            }
            else
            {
              dbg_printf("D4 code type=%08X \n",value);
            }
          }
          else if(code==0xD4000000)
          {
            code|=D4Type;
          }
          else if(code==0xE0000000)
          {
            if(custom_asm)
            {
              code=0xC2000000; //Modify E code
              dbg_printf("Custom Asm detected and modified to work.\n");
            }
          }
          else if(code==0x023FE074&&value==0x012FFF11)
          {
            skip=1; //Don't copy this code.
            custom_asm=true;
          }
          else if(code==0x023FE074&&value==0xE3520003)
          {
            skip=1; //Don't copy this code.
            custom_asm=false;
          }
          if(!skip)
          {
            *codeTable++=code;
            *codeTable++=value;
          }
          if(skip)
          {
            skip--;
          }
        }
      }
      cc++;
      ccode=(u32*)((u32)ccode+(((*ccode&0x00ffffff)+1)*4));
    }
  }

  *codeTable++=0xCF000000;
  *codeTable++=0x00000000;

  // fix some addresses
  for(u32* ii=(u32*)aPatchEntry7;ii<(u32*)(aPatchEntry7End);++ii)
  {
    if(0xEFC0DE23==*ii) //ar code table start
    {
      *ii=codeTableStart;
    }
  }
  zeroMemoryITCM((void*)0x23b0000,0x10000);
  DC_FlushAll();
  DC_InvalidateAll();
  return (u32)codeTable;
}
