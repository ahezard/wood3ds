/*
    patches_da.cpp
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
#if defined(_STORAGE_rpg)
#include <iorpg.h>
#elif defined(_STORAGE_r4)
#include <ior4.h>
#elif defined(_STORAGE_ak2i)
#include <ioak2i.h>
#elif defined(_STORAGE_r4idsn)
#include <ior4idsn.h>
#endif
#include <string.h>
#include "dma4_bin.h"
#include "sw_cw_ja_bin.h"

void CPatcher::InitDMA4(void)
{
  u32 clusterSize,clusterShift=0,sdhc=isSDHC()?0:9,clusterMask;
  ELM_ClusterSizeFromDisk(ioSD(),&clusterSize);
  clusterMask=(clusterSize>>9)-1;
  while(clusterSize>512)
  {
    clusterSize>>=1;
    clusterShift++;
  }
  memcpy((void*)iPatchDMA4Entry,dma4_bin,dma4_bin_size);
  iPatchDMA4Ptr=iPatchDMA4Entry+dma4_bin_size;
  NeedUncompressed();
  for(u32* ii=(u32*)iPatchDMA4Entry;ii<(u32*)(iPatchDMA4Entry+dma4_bin_size);ii++)
  {
    if(0xEFC0DEF2==*ii)
    {
      iDmaFuncs.iFunc2=ii+1;
    }
    else if(0xEFC0DEF3==*ii)
    {
      iDmaFuncs.iFunc3=ii+1;
    }
    else if(0xEFC0DEF4==*ii)
    {
      iDmaFuncs.iFunc4=ii+1;
    }
    else if(0xEFC0DEF6==*ii)
    {
      iDmaFuncs.iFunc6=ii+1;
    }
    else if(0xEFC0DEC1==*ii)
    {
      if(*ii==*(ii+1))
      {
        *ii=FixedAddr();
        *(ii+1)=FixedData();
      }
    }
    else if(0xEFC0DEC2==*ii)
    {
      *ii=iSizeMask;
    }
    else if(0xEFC0DEC3==*ii)
    {
      *ii=0xffffffff;
      iDmaFuncs.iLastAddr=ii;
    }
    else if(0xEFC0DE91==*ii)
    {
      *ii=sdhc;
    }
    else if(0xEFC0DE92==*ii)
    {
      *ii=clusterShift;
    }
    else if(0xEFC0DE93==*ii)
    {
      *ii=clusterMask;
    }
    else if(0xEFC0DE94==*ii)
    {
      *ii=ioSize()-1;
    }
    else if(0xEFC0DE95==*ii)
    {
      *ii=0xfc2;
      iDmaFuncs.iCardId=ii;
    }
  }
  iDmaFuncs.iFunc5=PatchRepatch9();
}

void CPatcher::PatchCardId(s32 aMemory)
{
#if defined(_STORAGE_r4)
  PatchMem(KArm9,aMemory,0xe59f0004);    //ldr r0, address
  PatchMem(KArm9,aMemory+4,0xe5900000);  //ldr r0, [r0]
  PatchMem(KArm9,aMemory+8,0xe12fff1e);  //bx lr
  PatchMem(KArm9,aMemory+12,0x027ffc00); //address: .word 0x27ffc00
#elif defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
  PatchMem(KArm9,aMemory,0xe59f0014);    //ldr r0, flag
  PatchMem(KArm9,aMemory+4,0xe3e01000);  //mov r1, #0xffffffff
  PatchMem(KArm9,aMemory+8,0xe5801000);  //str r1, [r0]
  PatchMem(KArm9,aMemory+12,0xe59f0004); //ldr r0, address
  PatchMem(KArm9,aMemory+16,0xe5900000); //ldr r0, [r0]
  PatchMem(KArm9,aMemory+20,0xe12fff1e); //bx lr
  PatchMem(KArm9,aMemory+24,(u32)iDmaFuncs.iCardId); //address: .word xxx
  PatchMem(KArm9,aMemory+28,(u32)iDmaFuncs.iLastAddr); //flag: .word xxx
#endif
}

#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
void CPatcher::PatchCardIdThumb(s32 aMemory)
{
  PatchMem(KArm9,aMemory,0x21004804);    //ldr r0, flag|mov r1, #0
  PatchMem(KArm9,aMemory+4,0x60013901);  //sub r1, #1|str r1, [r0]
  PatchMem(KArm9,aMemory+8,0x68004801);  //ldr r0, address|ldr r0, [r0]
  PatchMem(KArm9,aMemory+12,0x46c04770); //bx lr|nop
  PatchMem(KArm9,aMemory+16,(u32)iDmaFuncs.iCardId); //address: .word xxx
  PatchMem(KArm9,aMemory+20,(u32)iDmaFuncs.iLastAddr); //flag: .word xxx
}
#endif

void CDsPatcher::PatchDMA4(void)
{
  InitDMA4();
  for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());ii++)
  {
    //12 0F 06 EE 1E FF 2F E1
    if(*ii==0xee060f12) //mcr p15, 0, r0, c6, c2
    {
      if(*(ii+1)==0xe12fff1e) //bx lr
      {
        PatchMPU2(s32(ii));
      }
    }
    //70 30 9F E5 00 20 93 E5 02 21 12 E2 FC FF FF 1A
    else if(*ii==0xe59f3070) //ldr r3, =0x40001a4
    {
      if(*(ii+1)==0xe5932000&&*(ii+2)==0xe2122102&&*(ii+3)==0x1afffffc) //xxx: ldr r2, [r3]|ands r2, r2, #0x80000000|bne xxx
      {
        PatchMem(KArm9,s32(ii),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
    }
    //50 30 9F E5 00 20 93 E5 02 01 12 E3 FC FF FF 1A
    else if(*ii==0xe59f3050) //ldr r3, =0x40001a4
    {
      if(*(ii+1)==0xe5932000&&*(ii+2)==0xe3120102&&*(ii+3)==0x1afffffc) //xxx: ldr r2, [r3]|tst r2, #0x80000000|bne xxx
      {
        PatchMem(KArm9,s32(ii),0xe51ff004);//ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
    }
    else if(GameCodeAll()==0x445041) //Pokemon Dash
    {
      //25 14 A0 E1 B7 C4 81 E3 05 5C A0 E1
      if(*ii==0xe1a01425&&*(ii+1)==0xe381c4b7&&*(ii+2)==0xe1a05c05&&*(ii+28)==0xe5c25000) //mov r1, r5,lsr#8|orr r12, r1, #0xb7000000|mov r5, r5, lsl#24|strb r5, [r2]
      {
        PatchMem(KArm9,s32(ii+1),0xe1a05000); //mov r5, r0
        PatchMem(KArm9,s32(ii+2),0xe1a00001); //mov r0, r1
        PatchMem(KArm9,s32(ii+3),0xe28fe004); //adr lr, [pc, #4]
        PatchMem(KArm9,s32(ii+4),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+5),(u32)iDmaFuncs.iFunc2);
        PatchMem(KArm9,s32(ii+6),0xe1a00005); //mov r0, r5
        PatchMem(KArm9,s32(ii+7),0xe28ff048); //adr pc, xxx
        PatchMem(KArm9,s32(ii+28),0xe1a00000); //nop
      }
    }
    //00 E0 C8 E5 7C 80 9F E5 00 E0 C8 E5
    else if(*ii==0xe5c8e000) //strb lr, [r8]
    {
      if(*(ii+1)==0xe59f807c&&*(ii+2)==0xe5c8e000) //ldr r8, =xxx|strb lr, [r8]
      {
        PatchMem(KArm9,s32(ii),0xe28fe004);//adr lr, [pc, #4]
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc4);
      }
    }
    //0C 50 C7 E5 0D 50 C7 E5 0E 50 C7 E5 03 30 87 E5
    else if(*ii==0xe5c7500c) //strb r5, [r7,#0xc]
    {
      if(*(ii+1)==0xe5c7500d&&*(ii+2)==0xe5c7500e&&*(ii+3)==0xe5873003) //strb r5, [r7,#0xd]|strb r5, [r7,#0xe]|str r3, [r7,#3]
      {
        PatchMem(KArm9,s32(ii),0xe28f5004);//adr r5, [pc, #4]
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc3);
      }
    }
    //FF 07 9B E8 00 B0 A0 E3 1C FF 2F E1
    else if(*ii==0xe89b07ff) //ldmia r11, {r0-r10}
    {
      if(*(ii+1)==0xe3a0b000&&*(ii+2)==0xe12fff1c) //mov r11, #0|bx r12
      {
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc5);
        iUsePatchData=true;
      }
    }
    else if(*ii==0xe5900000) //ldr r0, [r0]
    {
      //00 00 90 E5 60 00 90 E5 07 04 C0 E3 A1 04 80 E3 04 00 8A E5
      //latency 1
      if(*(ii+1)==0xe5900060&&*(ii+2)==0xe3c00407&&*(ii+3)==0xe38004a1&&*(ii+4)==0xe58a0004) //ldr r0, [r0,#0x60]|bic r0, r0, #0x7000000|orr r0, r0, #0xa1000000|str r0, [r10,#4]
      {
        PatchMem(KArm9,s32(ii+1),0xe3a004a1); //mov r0, #0xa1000000
        PatchMem(KArm9,s32(ii+2),0xe3800841); //orr r0, #0x00410000
        PatchMem(KArm9,s32(ii+3),0xe3800a06); //orr r0, #0x00006000
      }
      //00 00 90 E5 07 04 C0 E3 A1 04 80 E3 04 00 8A E5
      //latency 3
      else if(*(ii+1)==0xe3c00407&&*(ii+2)==0xe38004a1&&*(ii+3)==0xe58a0004) //bic r0, r0, #0x7000000|orr r0, r0, #0xa1000000|str r0, [r10,#4]
      {
        PatchMem(KArm9,s32(ii+0),0xe3a004a1); //mov r0, #0xa1000000
        PatchMem(KArm9,s32(ii+1),0xe3800841); //orr r0, #0x00410000
        PatchMem(KArm9,s32(ii+2),0xe3800a06); //orr r0, #0x00006000
      }
      //00 00 90 E5 00 00 54 13 07 04 C0 E3 01 50 A0 13 A1 04 80 E3
      //latency 9
      else if(((*(ii+1))&0xfff0ffff)==0x13500000&&*(ii+2)==0xe3c00407&&((*(ii+3))&0xffff0fff)==0x13a00001&&*(ii+4)==0xe38004a1) //cmpne r4, #0|bic r0, r0, #0x7000000|movne r5, #1|orr r0, r0, #0xa1000000
      {
        PatchMem(KArm9,s32(ii+0),0xe3a004a1); //mov r0, #0xa1000000
        PatchMem(KArm9,s32(ii+2),0xe3800841); //orr r0, #0x00410000
        PatchMem(KArm9,s32(ii+4),0xe3800a06); //orr r0, #0x00006000
      }
    }
    else if(((*ii)&0xff00ffff)==0x48006e01) //ldr r1, [r0,#0x60]|ldr r0, =0xf8ffffff
    {
      //01 6E 20 48 01 40 A1 20 00 06 01 43 00 98 41 60
      //latency 2
      if(*(ii+1)==0x20a14001&&*(ii+2)==0x43010600&&*(ii+3)==0x60419800) //ands r1, r0|movs r0, #0xa1|lsls r0, r0, #0x18|orrs r1, r0|ldr r0, [sp]|str r1, [r0,#4]
      {
        //Infinite Space
        if(0x00433643!=GameCodeAll())
        {
          PatchMem(KArm9,s32(ii+1),0xe0014900); //ldr r1, data|b xxx
          PatchMem(KArm9,s32(ii+2),0xa1416000); //data: .word 0xa1416000|xxx:
        }
      }
      //01 6E 21 48 01 40 21 48 01 43 00 98
      //latency 11
      else if(((*(ii+1))&0xff00ffff)==0x48004001&&*(ii+2)==0x98004301) //ands r1, r0|ldr r0, =0xa1000000|orrs r1, r0|ldr r0, [sp]
      {
        PatchMem(KArm9,s32(ii+0),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+1),0xa1416000); //data: .word 0xa1416000|xxx:
        PatchMem(KArm9,s32(ii+2),0x980046c0);
      }
    }
    //60 00 90 E5 01 60 A0 13 07 04 C0 E3 A1 04 80 E3 00 00 56 E3 04 00 8B E5
    //latency 4
    else if(*ii==0xe5900060) //ldr r0, [r0,#0x60]
    {
      if(((*(ii+1))&0xffff0fff)==0x13a00001&&*(ii+2)==0xe3c00407&&*(ii+3)==0xe38004a1&&((*(ii+4))&0xfff0ffff)==0xe3500000&&((*(ii+5))&0xfff0ffff)==0xe5800004) //movne rX, #1|bic r0, r0, #0x7000000|orr r0, r0, #0xa1000000|cmp rX, #0|str r0, [rY,#4]
      {
        PatchMem(KArm9,s32(ii+0),0xe3a004a1); //mov r0, #0xa1000000
        PatchMem(KArm9,s32(ii+2),0xe3800841); //orr r0, #0x00410000
        PatchMem(KArm9,s32(ii+3),0xe3800a06); //orr r0, #0x00006000
      }
    }
    //00 10 91 E5 07 14 C1 E3 A1 14 81 E3 00 10 80 E5
    //latency 5
    else if(*ii==0xe5911000) //ldr r1, [r1]
    {
      if(*(ii+1)==0xe3c11407&&*(ii+2)==0xe38114a1&&*(ii+3)==0xe5801000) //bic r1, r1, #0x7000000|orr r1, r1, #0xa1000000|str r1, [r0]
      {
        PatchMem(KArm9,s32(ii+0),0xe3a014a1); //mov r1, #0xa1000000
        PatchMem(KArm9,s32(ii+1),0xe3811841); //orr r1, #0x00410000
        PatchMem(KArm9,s32(ii+2),0xe3811a06); //orr r1, #0x00006000
      }
    }
    //04 48 01 40 A1 20 00 06 01 43 03 48
    //latency 6
    else if(*ii==0x40014804) //ldr r0, =0xf8ffffff|ands r1, r0
    {
      if(*(ii+1)==0x060020a1&&*(ii+2)==0x48034301) //movs r0, #0xa1|lsls r0, r0, #0x18|orrs r1, r0|ldr r0, =0x40001a4
      {
        PatchMem(KArm9,s32(ii+0),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+1),0xa1416000); //data: .word 0xa1416000|xxx:
        PatchMem(KArm9,s32(ii+2),0x480346c0);
      }
    }
    //01 68 05 48 01 40 A1 20 00 06 01 43
    //latency 7
    //World of Zoo (Europe) (En,Fr,De,Es,It,Nl,Sv,Da)
    else if(*ii==0x48056801) //ldr r1, [r0]|ldr r0, =0xf8ffffff
    {
      if(*(ii+1)==0x20a14001&&*(ii+2)==0x43010600) //ands r1, r0|movs r0, #0xa1|lsls r0, r0, #0x18|orrs r1, r0
      {
        PatchMem(KArm9,s32(ii+1),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+2),0xa1416000); //data: .word 0xa1416000|xxx:
      }
    }
    else if(((*ii)&0xffffff00)==0x40014800) //ldr r0, =0xf8ffffff|ands r1, r0
    {
      //1C 48 01 40 1C 48 01 43 00 98 41 60
      //latency 8
      if(((*(ii+1))&0xffffff00)==0x43014800&&*(ii+2)==0x60419800) //ldr r0, =0xa1000000|orrs r1, r0|ldr r0, [sp]|str r1, [r0,#4]
      {
        PatchMem(KArm9,s32(ii+0),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+1),0xa1416000); //data: .word 0xa1416000|xxx:
      }
      //1F 48 01 40 A1 20 00 06 01 43 00 98
      //latency 12
      //Nacho Libre (USA) || Dead'n'Furious (Europe) (En,Fr,De,Es,It)
      else if(*(ii+1)==0x060020a1&&*(ii+2)==0x98004301) //movs r0, #0xa1|lsls r0, r0, #0x18|orrs r1, r0|ldr r0, [sp]
      {
        PatchMem(KArm9,s32(ii+0),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+1),0xa1416000); //data: .word 0xa1416000|xxx:
        PatchMem(KArm9,s32(ii+2),0x980046c0);
      }
    }
    //01 20 00 06 0a 1c 02 43 c1 01 11 43 40 01 01 43
    //latency 10
    //Think - Training fuer den Kopf - Kids (Europe) (En,De) || Tropix (USA)
    else if(*ii==0x06002001) //mov r0, #1|lsl r0, #0x18
    {
      if(*(ii+1)==0x43021c0a&&*(ii+2)==0x431101c1&&*(ii+3)==0x43010140) //mov r2,r1|orr r2, r0|lsl r1, r0, #7|orr r1, r2|lsl r0, #5|orr r1, r0
      {
        PatchMem(KArm9,s32(ii+2),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+3),0xa1416000); //data: .word 0xa1416000|xxx:
      }
    }
    //10 B4 81 B0 10 4B 11 4A
    else if(*ii==0xb081b410) //push {r4}|sub sp, sp, #4
    {
      if(*(ii+1)==0x4a114b10) //ldr r3, =0x40001a4|ldr r2, =0x80000000
      {
        PatchMem(KArm9,s32(ii),0x47104a00); //ldr r2, [pc]|bx r2
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
    }
    //18 B4 02 22 0B 4C 92 07
    else if(*ii==0x2202b418)
    {
      if(*(ii+1)==0x07924c0b) //
      {
        PatchMem(KArm9,s32(ii),0x47104a00); //ldr r2, [pc]|bx r2
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
    }
    else if(GameCode()==0x50394943||GameCode()==0x45585443) //Think - Training fuer den Kopf - Kids (Europe) (En,De) || Tropix (USA)
    {
      //88 72 CB 72 1B 06 18 0C 08 73 18 0A 48 73 8B 73
      if(*ii==0x72cb7288) //strb r0, [r1,#0xa]|strb r3, [r1,#0xb]
      {
        if(*(ii+1)==0x0c18061b&&*(ii+2)==0x0a187308&&*(ii+3)==0x738b7348) //lsl r3, r3, #0x18|lsr r0, r3, #0x10|strb r0, [r1,#0xc]|lsr r0, r3, #8|strb r0, [r1,#0xd]|strb r3, [r1,#0xe]
        {
          PatchMem(KArm9,s32(ii),0x3001A003); //adr r0, back|add r0, #1
          PatchMem(KArm9,s32(ii+1),0x1c184686); //mov lr, r0|mov r0, r3
          PatchMem(KArm9,s32(ii+2),0x47184b00); //ldr r3, call|bx r3
          PatchMem(KArm9,s32(ii+3),(u32)iDmaFuncs.iFunc4);
        }
      }
      //1D 43 2B 0E C3 71 2B 0C 03 72 2B 0A 43 72 13 06
      else if(*ii==0x0e2b431d) //orr r5, r3|lsr r3, r5, #0x18
      {
        if(*(ii+1)==0x0c2b71c3&&*(ii+2)==0x0a2b7203&&*(ii+3)==0x06137243) //strb r3, [r0,#7]|lsr r3, r5, #0x10|strb r3, [r0,#8]|lsr r3, r5, #8|strb r3, [r0,#9]|lsl r3, r2, #0x18
        {
          PatchMem(KArm9,s32(ii),0x3001a004); //adr r0, back|add r0, #1
          PatchMem(KArm9,s32(ii+1),0x1c284686); //mov lr, r0|mov r0, r5
          PatchMem(KArm9,s32(ii+2),0x1c0e4b01); //ldr r3, call|mov r6, r1
          PatchMem(KArm9,s32(ii+3),0x46c04718); //bx r3|nop
          PatchMem(KArm9,s32(ii+4),(u32)iDmaFuncs.iFunc2);
          PatchMem(KArm9,s32(ii+5),0xe0031c31); //mov r1, r6|b exit
        }
      }
      //1B 06 1D 43 2B 0E C3 71 2B 0C 03 72 2B 0A 43 72
      else if(*ii==0x431d061b) //lsl r3, r3, #0x18|orr r5, r3
      {
        if(*(ii+1)==0x71c30e2b&&*(ii+2)==0x72030c2b&&*(ii+3)==0x72430a2b) //lsr r3, r5, #0x18|strb r3, [r0,#7]|lsr r3, r5, #0x10|strb r3, [r0,#8]|lsr r3, r5, #8|strb r3, [r0,#9]
        {
          PatchMem(KArm9,s32(ii),0x3001a004); //adr r0, back|add r0, #1
          PatchMem(KArm9,s32(ii+1),0x1c284686); //mov lr, r0|mov r0, r5
          PatchMem(KArm9,s32(ii+2),0x1c0e4b01); //ldr r3, call|mov r6, r1
          PatchMem(KArm9,s32(ii+3),0x46c04718); //bx r3|nop
          PatchMem(KArm9,s32(ii+4),(u32)iDmaFuncs.iFunc2);
          PatchMem(KArm9,s32(ii+5),0xe0041c31); //mov r1, r6|b exit
        }
      }
    }
    else if(GameCode()==0x4a4d4d41) // Minna no Mahjong DS (Japan) (v1.1)
    {
      if(*ii==0xe92d4010&&*(ii+1)==0xe1a04000&&*(ii+2)==0xe1a0e001&&*(ii+3)==0xe59f0098&&*(ii+4)==0xe5900000&&*(ii+5)==0xe2000102&&*(ii+6)==0xe3500000&&*(ii+7)==0x1afffffa) //stmfd sp!, {r4,lr}|mov r4, r0|mov lr, r1|xxx:ldr r0, =0x40001a4|ldr r0, [r0]|and r0, r0, #0x80000000|cmp r0, #0|bne xxx
      {
        PatchMem(KArm9,s32(ii),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
      else if(*ii==0xe3a01000&&*(ii+1)==0xe59f0090&&*(ii+2)==0xe5c01000) //mov r1, #0|ldr r0, =0x40001af|strb r1, [r0]
      {
        PatchMem(KArm9,s32(ii),0xe28f5004);//adr r5, [pc, #4]
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc3);
      }
      //latency 13
      //00 00 81 E1 02 01 80 E3 02 02 80 E3
      else if(*ii==0xe1810000&&*(ii+1)==0xe3800102&&*(ii+2)==0xe3800202) //orr r0, r1, r0|orr r0, r0, #0x80000000|orr r0, r0, #0x20000000
      {
        PatchMem(KArm9,s32(ii+0),0xe3a004a1); //mov r0, #0xa1000000
        PatchMem(KArm9,s32(ii+1),0xe3800841); //orr r0, #0x00410000
        PatchMem(KArm9,s32(ii+2),0xe3800a06); //orr r0, #0x00006000
      }
    }
    //F0 B4 0A 0A 00 06 16 1C 06 43 02 20
    //twl thumb
    else if(*ii==0x0a0ab4f0)
    {
      if(*(ii+1)==0x1c160600&&*(ii+2)==0x20024306)
      {
        PatchMem(KArm9,s32(ii),0x47104a00); //ldr r2, [pc]|bx r2
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc6);
      }
    }
    //38 40 2D E9 00 0C A0 E1 21 24 80 E1 01 3C A0 E1
    //twl arm
    else if(*ii==0xe92d4038) //stmfd sp!, {r3-r5,lr}
    {
      if(*(ii+1)==0xe1a00c00&&*(ii+2)==0xe1802421&&*(ii+3)==0xe1a03c01) //mov r0, r0, lsl#24|orr r2, r0, r1, lsr#8|mov r3, r1, lsl#24
      {
        PatchMem(KArm9,s32(ii),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc6);
      }
    }
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    else if(!IsProtection()||0x004c5741==GameCodeAll()) //"World Ends with You, The"
    {
      //getcardid
      //tetris ds (u)
      //00 40 2D E9 04 D0 4D E2 2E 03 A0 E3 00 10 A0 E3
      //stmfd sp!, {lr}
      //sub sp, sp, #4
      //mov r0, #0xb8000000
      //mov r1, #0
      if(*ii==0xe92d4000&&*(ii+1)==0xe24dd004&&*(ii+2)==0xe3a0032e&&*(ii+3)==0xe3a01000)
      {
        PatchCardId((s32)ii);
      }
      //star fox command (u)
      //00 B5 81 B0 ? 48 00 21
      //push {lr}
      //sub sp, sp, #4
      //ldr r0, =0xb8000000
      //movs r1, #0
      else if(*ii==0xb081b500&&((*(ii+1))&0xffffff00)==0x21004800&&*(ii+2+((*(ii+1))&0xff))==0xb8000000)
      {
        PatchCardIdThumb((s32)ii);
      }
      //dead'n'furious (e)
      //08 B5 2E 20 80 06 00 21
      //push {r3,lr}
      //movs r0, #0x2e
      //lsls r0, r0, #0x1a
      //movs r1, #0
      else if(*ii==0x202eb508&&*(ii+1)==0x21000680)
      {
        PatchCardIdThumb((s32)ii);
      }
      //the world ends with you (e)
      //08 40 2D E9 2E 03 A0 E3 00 10 A0 E3
      //stmfd sp!, {r3,lr}
      //mov r0, #0xb8000000
      //mov r1, #0
      else if(*ii==0xe92d4008&&*(ii+1)==0xe3a0032e&&*(ii+2)==0xe3a01000)
      {
        PatchCardId((s32)ii);
      }
      //the legend of zelda - spirit tracks (u)
      //10 40 2D E9 00 40 A0 E3 04 10 A0 E1 2E 03 A0 E3
      //stmfd sp!, {r4,lr}
      //mov r4, #0
      //mov r1, r4
      //mov r0, #0xb8000000
      else if(*ii==0xe92d4010&&*(ii+1)==0xe3a04000&&*(ii+2)==0xe1a01004&&*(ii+3)==0xe3a0032e)
      {
        PatchCardId((s32)ii);
      }
      //twl-nitro
      //barbie and the three musketeers (e)
      //08 40 2D E9 B8 00 A0 E3 00 10 A0 E3 ? ? ? EB
      //stmfd sp!, {r3,lr}
      //mov r0, #0xb8
      //mov r1, #0
      //bl xxx
      else if(*ii==0xe92d4008&&*(ii+1)==0xe3a000b8&&*(ii+2)==0xe3a01000&&((*(ii+3))&0xff000000)==0xeb000000)
      {
        PatchCardId((s32)ii);
      }
      //twl-nitro
      //world of zoo (e)
      //08 B5 B8 20 00 21 FF F7
      //push {r3,lr}
      //movs r0, #0xb8
      //movs r1, #0
      else if(*ii==0x20b8b508&&((*(ii+1))&0xf800ffff)==0xf0002100)
      {
        PatchCardIdThumb((s32)ii);
      }
      //twl-nitro
      //bookworm (u)
      //10 40 2D E9 00 40 A0 E3 04 10 A0 E1 B8 00 A0 E3
      //stmfd sp!, {r4,lr}
      //mov r4, #0
      //mov r1, r4
      //mov r0, #0xb8
      else if(*ii==0xe92d4010&&*(ii+1)==0xe3a04000&&*(ii+2)==0xe1a01004&&*(ii+3)==0xe3a000b8)
      {
        PatchCardId((s32)ii);
      }
    }
#endif
  }
#if defined(_STORAGE_r4)
  //b8 spam
  if(0x504c5741==GameCode()||0x454c5741==GameCode()) // "World Ends with You, The (Europe)" & "World Ends with You, The (USA)"
  {
    PatchCardId(0x02042f48);
  }
  else if(0x45545559==GameCode()) //"Ultimate Mortal Kombat (USA) (En,Fr,De,Es,It)"
  {
    PatchCardId(0x020b3f24);
  }
  else if(0x50545559==GameCode()) //"Ultimate Mortal Kombat (Europe) (En,Fr,De,Es,It)"
  {
    PatchCardId(0x020b4524);
  }
  else if(0x4a435742==GameCode()) //"Minna no Conveni (Japan)"
  {
    PatchCardId(0x0200ce10);
  }
#endif
  //vblank syncro
  if(0x45574c43==GameCode()||0x50574c43==GameCode()) //"Star Wars - The Clone Wars - Jedi Alliance (USA) (En,Fr,De,Es)" & "Star Wars - The Clone Wars - Jedi Alliance (Europe) (En,Fr,De,Es)"
  {
    memcpy((void*)KMemProtMe,sw_cw_ja_bin,sw_cw_ja_bin_size);
    PatchMem(KArm9,0x0205653c,0x47084900); //ldr r1, [pc]|bx r1
    PatchMem(KArm9,0x02056540,KMemProtMe+1);
  }
}

void CDsiPatcher::PatchDMA4(void)
{
  InitDMA4();
  for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());ii++)
  {
    //F0 B4 0A 0A 00 06 16 1C 06 43 02 20
    if(*ii==0x0a0ab4f0)
    {
      if(*(ii+1)==0x1c160600&&*(ii+2)==0x20024306)
      {
        PatchMem(KArm9,s32(ii),0x47104a00); //ldr r2, [pc]|bx r2
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc6);
      }
    }
    //38 40 2D E9 00 0C A0 E1 21 24 80 E1 01 3C A0 E1
    else if(*ii==0xe92d4038) //stmfd sp!, {r3-r5,lr}
    {
      if(*(ii+1)==0xe1a00c00&&*(ii+2)==0xe1802421&&*(ii+3)==0xe1a03c01) //mov r0, r0, lsl#24|orr r2, r0, r1, lsr#8|mov r3, r1, lsl#24
      {
        PatchMem(KArm9,s32(ii),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc6);
      }
    }
    //50 30 9F E5 00 20 93 E5 02 01 12 E3 FC FF FF 1A
    //mario vs. donkey kong - mini-land mayhem
    else if(*ii==0xe59f3050) //ldr r3, =0x40001a4
    {
      if(*(ii+1)==0xe5932000&&*(ii+2)==0xe3120102&&*(ii+3)==0x1afffffc) //xxx: ldr r2, [r3]|tst r2, #0x80000000|bne xxx
      {
        PatchMem(KArm9,s32(ii),0xe51ff004);//ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+1),(u32)iDmaFuncs.iFunc2);
      }
    }
    //0C 50 C7 E5 0D 50 C7 E5 0E 50 C7 E5 03 30 87 E5
    else if(*ii==0xe5c7500c) //strb r5, [r7,#0xc]
    {
      if(*(ii+1)==0xe5c7500d&&*(ii+2)==0xe5c7500e&&*(ii+3)==0xe5873003) //strb r5, [r7,#0xd]|strb r5, [r7,#0xe]|str r3, [r7,#3]
      {
        PatchMem(KArm9,s32(ii),0xe28f5004);//adr r5, [pc, #4]
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc3);
      }
    }
    //FF 07 9B E8 00 B0 A0 E3 1C FF 2F E1
    else if(*ii==0xe89b07ff) //ldmia r11, {r0-r10}
    {
      if(*(ii+1)==0xe3a0b000&&*(ii+2)==0xe12fff1c) //mov r11, #0|bx r12
      {
        PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
        PatchMem(KArm9,s32(ii+2),(u32)iDmaFuncs.iFunc5);
        iUsePatchData=true;
      }
    }
    //00 10 91 E5 07 14 C1 E3 A1 14 81 E3 00 10 80 E5
    //latency 5
    else if(*ii==0xe5911000) //ldr r1, [r1]
    {
      if(*(ii+1)==0xe3c11407&&*(ii+2)==0xe38114a1&&*(ii+3)==0xe5801000) //bic r1, r1, #0x7000000|orr r1, r1, #0xa1000000|str r1, [r0]
      {
        PatchMem(KArm9,s32(ii+0),0xe3a014a1); //mov r1, #0xa1000000
        PatchMem(KArm9,s32(ii+1),0xe3811841); //orr r1, #0x00410000
        PatchMem(KArm9,s32(ii+2),0xe3811a06); //orr r1, #0x00006000
      }
    }
    //04 48 01 40 A1 20 00 06 01 43 03 48
    //latency 6
    else if(*ii==0x40014804) //ldr r0, =0xf8ffffff|ands r1, r0
    {
      if(*(ii+1)==0x060020a1&&*(ii+2)==0x48034301) //movs r0, #0xa1|lsls r0, r0, #0x18|orrs r1, r0|ldr r0, =0x40001a4
      {
        PatchMem(KArm9,s32(ii+0),0xe0014900); //ldr r1, data|b xxx
        PatchMem(KArm9,s32(ii+1),0xa1416000); //data: .word 0xa1416000|xxx:
        PatchMem(KArm9,s32(ii+2),0x480346c0);
      }
    }
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    //getcardid
    //twl only
    //mon coach personnel - mes recettes plaisir et ligne (f)
    //10 B5 B8 24 ? ? ? ? 00 28 00 D0 90 24 20 1C
    //push {r4,lr}
    //movs r4, #0xb8
    //bl xxx
    //cmp r0, #0
    //beq yyy
    //movs r4, #0x90
    //yyy:
    //adds r0, r4, #0
    else if(*ii==0x24b8b510&&*(ii+2)==0xd0002800&&*(ii+3)==0x1c202490)
    {
      PatchCardIdThumb((s32)ii);
    }
    //twl only
    //classic word games (u)
    //10 40 2D E9 B8 40 A0 E3 ? ? ? ? 00 00 50 E3 90 40 A0 13
    //stmfd sp!, {r4,lr} //r3?, r5?
    //mov r4, #0xb8 //r5?
    //bl xxx
    //cmp r0, #0
    //movne r4, #0x90 //r5?
    else if(((*ii)&0xffffffd7)==0xe92d4010&&((*(ii+1))&0xffffefff)==0xe3a040b8&&*(ii+3)==0xe3500000&&((*(ii+4))&0xffffefff)==0x13a04090)
    {
      PatchCardId((s32)ii);
    }
#endif
  }
  PatchFlashCardCheck3();
}

void CDsiPatcher::PatchFlashCardCheck3(void)
{
  //patch "4376 - Rabbids Go Home (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
  if(0x56475256==GameCode()) //VRGV
  {
    NeedUncompressed();
    PatchOverlay8(0x0203d108,KDsiProtMe,0x0218da4c,0x0218da54); //size 1024
  }
  //patch "5220 - Rabbids Go Home (USA) (NDSi Enhanced)"
  else if(0x45475256==GameCode()) //VRGE
  {
    NeedUncompressed();
    PatchOverlay8(0x0203cf18,KDsiProtMe,0x0218d5c4,0x0218d5cc); //size 1024
  }
}
