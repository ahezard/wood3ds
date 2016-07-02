/*
    patches_la.cpp
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

#include "patches.h"

void CPatcher::PatchLanguage(bool aDSi)
{
  u32 language=Language();
  if(!language) return;
  --language;
  NeedUncompressed();
  for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());ii++)
  {
    if(0xe1dc26b4==*ii) //ldrh r2, [r12, #0x64]
    {
      //B4 26 DC E1 06 00 8C E2 04 10 84 E2 82 2E A0 E1 A2 2E A0 E1 00 20 C4 E5
      //add r0, r12, #6|add r1, r4, #4|mov r2, r2, lsl#29|mov r2, r2, lsr#29|strb r2, [r4]
      //mario slam basketball (e)
      if(0xe28c0006==*(ii+1)&&0xe2841004==*(ii+2)&&0xe1a02e82==*(ii+3)&&0xe1a02ea2==*(ii+4)&&0xe5c42000==*(ii+5))
      {
        PatchMem(KArm9,s32(ii+4),0xe3a02000|language); //mov r2, language
        break;
      }
    }
    else if(0x88093164==*ii) //add r1, #0x64|ldrh r1, [r1]
    {
      //64 31 09 88 49 07 49 0F 21 70 81 78 09 07 09 0F
      //lsl r1, #0x1d|lsr r1, #0x1d|strb r1, [r4]|ldrb r1, [r0, #2]|lsl r1, #0x1c|lsr r1, #0x1c
      //tom clancy's splinter cell - chaos theory (e)
      if(0x0f490749==*(ii+1)&&0x78817021==*(ii+2)&&0x0f090709==*(ii+3))
      {
        PatchMem(KArm9,-(s32(ii)+6),0x2100|language); //mov r1, language
        break;
      }
    }
    else if(0xe1d426b4==*ii) //ldrh r2, [r4,#0x64]
    {
      //B4 26 D4 E1 06 00 84 E2 04 10 85 E2 82 2E A0 E1 A2 2E A0 E1 00 20 C5 E5
      //add r0, r4, #6|add r1, r5, #4|mov r2, r2, lsl#29|mov r2, r2, lsr#29|strb r2, [r5]
      //lego harry potter - years 1-4 (u)
      if(0xe2840006==*(ii+1)&&0xe2851004==*(ii+2)&&0xe1a02e82==*(ii+3)&&0xe1a02ea2==*(ii+4)&&0xe5c52000==*(ii+5))
      {
        PatchMem(KArm9,s32(ii+4),0xe3a02000|language); //mov r2, language
        if(!aDSi) break;
      }
    }
    else if(0x88003064==*ii) //add r0, #0x64|ldrh r0, [r0]
    {
      //64 30 00 88 14 22 40 07 40 0F 28 70 A0 78 00 07
      //mov r2, #0x14|lsl r0, #0x1d|lsr r0, #0x1d|strb r0, [r5]|ldrb r0, [r4, #2]|lsl r0, #0x1c
      //shrek - forever after (e)
      if(0x07402214==*(ii+1)&&0x70280f40==*(ii+2)&&0x070078a0==*(ii+3))
      {
        PatchMem(KArm9,-(s32(ii)+8),0x2000|language); //mov r0, language
        break;
      }
    }
    else if(0x05c56054==*ii) //streqb r6, [r5,#0x54]
    {
      //54 60 C5 05 B4 06 D4 01 80 0E A0 01 A0 0E A0 01 00 00 C5 E5
      //ldreqh r0, [r4,#0x64]|moveq r0, r0,lsl#29|moveq r0, r0,lsr#29|strb r0, [r5]
      //mario vs. donkey kong - mini-land mayhem! (u)
      if(0x01d406b4==*(ii+1)&&0x01a00e80==*(ii+2)&&0x01a00ea0==*(ii+3)&&0xe5c50000==*(ii+4))
      {
        PatchMem(KArm9,s32(ii+3),0xe3a00000|language); //mov r0, language
        break;
      }
    }
  }
}
