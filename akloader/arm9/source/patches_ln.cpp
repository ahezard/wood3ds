/*
    patches_ln.cpp
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
#include "bank_nor_1_bin.h"
#include "bank_nor_2_bin.h"
#include "il2_bin.h"
#include <string.h>

void CDsPatcher::PatchLinkage(void)
{
  if(IsPSRAM())
  {
    for(u32* ii=(u32*)bank_nor_1_bin;ii<(u32*)(bank_nor_1_bin+bank_nor_1_bin_size);++ii)
    {
      if(0x13a00020==*ii)
      {
        *ii=0x13a00070;
      }
      else if(0x03a00010==*ii)
      {
        *ii=0x03a00060;
      }
    }
    for(u32* ii=(u32*)bank_nor_2_bin;ii<(u32*)(bank_nor_2_bin+bank_nor_2_bin_size);++ii)
    {
      if(0x13a00020==*ii)
      {
        *ii=0x13a00070;
      }
      else if(0x03a00010==*ii)
      {
        *ii=0x03a00060;
      }
    }
  }
  if(PatchPokemon()) ;
  else if(PatchIL2()) ;
}

bool CDsPatcher::PatchPokemon(void)
{
  bool res=false;
  //gba:
  //BPR*: Pokemon - Fire Red Version
  //AXP*: Pokemon - Sapphire Version
  //AXV*: Pokemon - Ruby Version
  //BPG*: Pokemon - Leaf Green Version
  //BPE*: Pokemon - Emerald Version
  if(IsPokemonGame()&&(iGbaGameCodeAll==0x525042||iGbaGameCodeAll==0x505841||iGbaGameCodeAll==0x565841||iGbaGameCodeAll==0x475042||iGbaGameCodeAll==0x455042))
  {
    NeedUncompressed();
    u32* array[2][2]={{NULL,NULL},{NULL,NULL}};
    for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());++ii)
    {
      //24 30 9F E5 AA 20 A0 E3 20 10 9F E5 00 20 C3 E5 55 20 A0 E3 00 20 C1 E5 B0 10 A0 E3 00 10 C3 E5 0A 14 A0 E3 00 00 C1 E5 1E FF 2F E1 55 55 00 0A AA 2A 00 0A
      if(*(ii)==0xe59f3024&&*(ii+1)==0xe3a020aa&&*(ii+2)==0xe59f1020&&*(ii+3)==0xe5c32000&&*(ii+4)==0xe3a02055&&*(ii+5)==0xe5c12000&&*(ii+6)==0xe3a010b0&&*(ii+7)==0xe5c31000&&*(ii+8)==0xe3a0140a&&*(ii+9)==0xe5c10000&&*(ii+10)==0xe12fff1e&&*(ii+11)==0x0a005555&&*(ii+12)==0x0a002aaa)
      {
        if(*(ii-1)==0x0a000001&&*(ii-71)==0xe92d4030)
        {
          array[0][0]=ii-71;
        }
        else
        {
          array[1][0]=ii;
        }
        if((array[0][0]&&array[0][1])||(array[1][0]&&array[1][1])) break;
      }
      //55 55 00 0A AA 2A 00 0A ? ? ? ? F0 4F 2D E9
      else if(*(ii)==0x0a005555&&*(ii+1)==0x0a002aaa&&*(ii+3)==0xe92d4ff0)
      {
        array[0][1]=ii-21;
        if(array[0][0]&&array[0][1]) break;
      }
      //55 55 00 0A ? ? ? ? AA 2A 00 0A ? ? ? ? 08 40 2D E9
      else if(*(ii)==0x0a005555&&*(ii+2)==0x0a002aaa&&*(ii+4)==0xe92d4008)
      {
        array[1][1]=ii-94;
        if(array[1][0]&&array[1][1]) break;
      }
    }
    if(array[0][0]&&array[0][1])
    {
      u32 size=bank_nor_1_bin_size/4;
      u32* ptr=(u32*)bank_nor_1_bin;
      u32* target=array[0][0]+64;
      for(u32 ii=0;ii<size;ii++) PatchMem(KArm9,s32(target+ii),*(ptr+ii));
      PatchMem(KArm9,s32(array[0][0]+0),0xe3a00062); //mov r0, #0x62
      PatchMem(KArm9,s32(array[0][0]+1),0xe2800c13); //add r0, #0x1300
      PatchMem(KArm9,s32(array[0][0]+2),0xe12fff1e); //bx lr

      //1st / erase
      PatchMem(KArm9,s32(array[0][1]+24),0xe3a00000); //mov r0, #0
      PatchMem(KArm9,s32(array[0][1]+25),0xe12fff1e); //bx lr

      //2nd / write byte
      PatchMem(KArm9,s32(array[0][1]+0),0xe5d02000); //ldrb r2, [r0]
      PatchMem(KArm9,s32(array[0][1]+1),0xe5c12000); //strb r2, [r1]
      PatchMem(KArm9,s32(array[0][1]+2),0xe3a00000); //mov r0, #0
      PatchMem(KArm9,s32(array[0][1]+3),0xe12fff1e); //bx lr

      res=true;
    }
    else if(array[1][0]&&array[1][1])
    {
      u32 size=bank_nor_2_bin_size/4;
      u32* ptr=(u32*)bank_nor_2_bin;
      u32* target=array[1][0];
      for(u32 ii=0;ii<size;ii++) PatchMem(KArm9,s32(target+ii),*(ptr+ii));

      //1st / erase
      PatchMem(KArm9,s32(array[1][1]+0),0xe3a00000); //mov r0, #0
      PatchMem(KArm9,s32(array[1][1]+1),0xe12fff1e); //bx lr

      //2nd / write byte 98
      PatchMem(KArm9,s32(array[1][1]+0x62),0xe5d02000); //ldrb r2, [r0]
      PatchMem(KArm9,s32(array[1][1]+0x63),0xe5c12000); //strb r2, [r1]
      PatchMem(KArm9,s32(array[1][1]+0x64),0xe3a00000); //mov r0, #0
      PatchMem(KArm9,s32(array[1][1]+0x65),0xe12fff1e); //bx lr
      res=true;
    }
  }
  return res;
}

bool CPatcher::IsPokemonGame(void)
{
  //nds:
  //APA*: Pokemon - Pearl Version
  //CPU*: Pokemon - Platinum Version
  //ADA*: Pokemon - Diamond Version
  //IPK*: Pokemon - HeartGold Version
  //IPG*: Pokemon - SoulSilver Version
  return (GameCodeAll()==0x00415041||GameCodeAll()==0x555043||GameCodeAll()==0x414441||GameCodeAll()==0x4b5049||GameCodeAll()==0x475049);
}

bool CPatcher::IsWiiLinkage(void)
{
  return IsPokemonGame()&&IsDMA();
}

//this is not really linkage patch, but i dont' want to add special option for one game
bool CDsPatcher::PatchIL2(void)
{
  bool res=false;
  s32 first_=0x2009334,second=0x2009358;
  switch(GameCode())
  {
    //"IL-2 Sturmovik - Birds of Prey (USA) (En,Fr,De,Es,It)"
    case 0x45524942:
      for(u32* ii=(u32*)il2_bin;ii<(u32*)(il2_bin+il2_bin_size);++ii)
      {
        if(0xbd1==*ii)
        {
          *ii=0xbce;
          break;
        }
      }
      first_=0x200a18c;
      second=0x200a1b0;
    //"IL-2 Sturmovik - Birds of Prey (Europe) (En,Fr,De,Es,It)"
    case 0x50524942:
      res=true;
      memcpy((void*)KMemProtMe,il2_bin,il2_bin_size);
      PatchMem(KArm9,first_,GenerateArmBL(first_,KMemProtMe+0x00,true)); //bl entry1
      PatchMem(KArm9,second,GenerateArmBL(second,KMemProtMe+0x20,true)); //bl entry2
      FixProtectionRegion2();
      break;
  }
  return res;
}
