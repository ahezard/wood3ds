/*
    patches_sr.cpp
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
#include "reset.h"
#include <string.h>
#include "patch7_bin.h"
#include "patch7wram_sr_bin.h"
#include "resetpatch9_bin.h"
#if defined(_STORAGE_r4)
#include "rpgmaps.h"
#endif
#include "dbgtool.h"

u32 CDsPatcher::SoftResetTarget(void)
{
  switch(GameCodeAll())
  {
    case 0x4d4441: //Welcome to Animal Crossing - Wild World
    case 0x323641: //Welcome to Animal Crossing - Wild World (Demo)
    case 0x564341: //Castlevania - Dawn of Sorrow
    case 0x374641: //FIFA 07
    case 0x505841: //Picross DS
    case 0x375241: //Pinky Street - Kira Kira Music Hour
    case 0x375259: //Pinky Street - Kira Kira Music Night
    case 0x324a59: //Juiced 2 - Hot Import Nights
    case 0x485341: //Asphalt - Urban GT
    case 0x503441: //Asphalt - Urban GT 2
    case 0x474e59: //Ninja Gaiden - Dragon Sword
    case 0x4c5741: //World Ends with You, The
    case 0x414159: //Arkanoid DS
    case 0x454659: //Fire Emblem - Shadow Dragon
    case 0x545559: //Ultimate Mortal Kombat
    case 0x444e43: //Need for Speed - Undercover
    case 0x4e4e42: //Need for Speed - Nitro
    case 0x515143: //AFL Mascot Manor
    case 0x4b5143: //NRL Mascot Mania
    case 0x503643: //Picross 3D
    case 0x4e4843: //Might & Magic - Clash of Heroes
    case 0x385859: //Tomb Raider - Underworld
    case 0x514459: //Dragon Quest IX - Sentinels of the Starry Skies
    case 0x393643: //Dirt 2
    case 0x414642: //FIFA 10
    case 0x4c4342: //Sideswiped
      return KMemSoftResetITCM;
    default:
      return KMemSoftReset;
  }
}

void CDsPatcher::PatchArm9(u32& anArm7Address)
{
  anArm7Address=0;

  u32 original_isr=0;
  u32* original_isr_addr=NULL;
  u32* start=(u32*)__NDSHeader->arm9executeAddress;
  if(GameCodeAll()==0x474942) start=(u32*)0x2000800; //Combat of Giants - Mutant Insects
  while(0xffff0000!=*(start++)) ;
  original_isr_addr=start-3;
  original_isr=*original_isr_addr;
  u32 reset_destination=SoftResetTarget();

  if(reset_destination==KMemSoftReset)
  {
    FixProtectionRegion2();
  }

  u32* arm9start=NULL;u32* arm7start=NULL;
#if defined(_STORAGE_r4)
  u32 loader_addr=GetR4Address("fat0:/_DS_MENU.DAT")&0xfffffffe;
#endif
  for(u32* ii=(u32*)resetpatch9_bin;ii<(u32*)(resetpatch9_bin+resetpatch9_bin_size);++ii)
  {
    if(0xEFC0DE01==*ii) // old isr
    {
      *ii=original_isr;
    }
    else if(0xEFC0DE02==*ii) //patched address
    {
      *ii=(u32)original_isr_addr;
      arm9start=ii;
    }
    else if(0xEFC0DE03==*ii)
    {
      *ii=(u32)original_isr_addr;
      arm7start=ii;
    }
    else if(0xEFC0DE10==*ii)
    {
      if(IsDMA()) *ii=0xe12fff31; //blx r1
      else *ii=0xe1a00000; //nop
    }
    else if(0xEFC0DE11==*ii)
    {
      *ii=iPatchDMA4Entry+1;
    }
#if defined(_STORAGE_r4)
    else if(0xefc0de20==*ii)
    {
      *ii=0xb4000000|(loader_addr>>8);
    }
    else if(0xefc0de21==*ii)
    {
      *ii=loader_addr<<24;
    }
#endif
  }
  if(arm7start&&arm9start)
  {
    anArm7Address=(arm7start-arm9start)*4+0x23b0000;
  }
  PatchMem(KArm9,s32(original_isr_addr),reset_destination);
  memcpy((void*)reset_destination,resetpatch9_bin,resetpatch9_bin_size);
}

void CDsiPatcher::PatchArm9(u32& anArm7Address)
{
  u32 original_isr=0;
  u32* original_isr_addr=NULL;
  for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+__NDSHeader->arm9binarySize);++ii)
  {
    if(0xffff0000==*ii)
    {
      original_isr_addr=ii-3;
      original_isr=*original_isr_addr;
      break;
    }
  }
  u32* arm9start=NULL;u32* arm7start=NULL;
#if defined(_STORAGE_r4)
  u32 loader_addr=GetR4Address("fat0:/_DS_MENU.DAT")&0xfffffffe;
#endif
  for(u32* ii=(u32*)resetpatch9_bin;ii<(u32*)(resetpatch9_bin+resetpatch9_bin_size);++ii)
  {
    if(0xEFC0DE01==*ii) // old isr
    {
      *ii=original_isr;
    }
    else if(0xEFC0DE02==*ii) //patched address
    {
      *ii=(u32)original_isr_addr;
      arm9start=ii;
    }
    else if(0xEFC0DE03==*ii)
    {
      *ii=(u32)original_isr_addr;
      arm7start=ii;
    }
    else if(0xEFC0DE10==*ii)
    {
      if(IsDMA()) *ii=0xe12fff31; //blx r1
      else *ii=0xe1a00000; //nop
    }
    else if(0xEFC0DE11==*ii)
    {
      *ii=iPatchDMA4Entry+1;
    }
#if defined(_STORAGE_r4)
    else if(0xefc0de20==*ii)
    {
      *ii=0xb4000000|(loader_addr>>8);
    }
    else if(0xefc0de21==*ii)
    {
      *ii=loader_addr<<24;
    }
#endif
  }
  if(arm7start&&arm9start)
  {
    anArm7Address=(arm7start-arm9start)*4+0x23b0000;
  }
  PatchMem(KArm9,s32(original_isr_addr),iPatchArm9Entry);
  memcpy((void*)iPatchArm9Entry,resetpatch9_bin,resetpatch9_bin_size);
}

void CPatcher::SearchArm7(void)
{
  u32* top=(u32*)__NDSHeader->arm7destination;
  u32* bottom=(u32*)(__NDSHeader->arm7destination+__NDSHeader->arm7binarySize);
  u32* target=NULL; bool twl=false;
  u32* getid=NULL; bool thumb=false;
  for(u32* ii=top;ii<bottom;++ii)
  {
    //isr handler
    //00 40 2D E9 01 C3 A0 E3 21 CE 8C E2 08 10 1C E5 00 00 51 E3
    //stmfd   sp!, {lr}
    //mov     r12, #0x4000000
    //add     r12, r12, #0x210
    //ldr     r1, [r12,#-8]
    //cmp     r1, #0
    if(0xe92d4000==*ii&&0xe3a0c301==*(ii+1)&&0xe28cce21==*(ii+2)&&0xe51c1008==*(ii+3)&&0xe3510000==*(ii+4))
    {
      for(u32 jj=5;jj<50;++jj)
      {
        //nitro
        //08 10 9F E5 00 01 91 E7 04 E0 9F E5 10 FF 2F E1
        //ldr     r1, [pc, #8]
        //ldr     r0, [r1,r0,lsl#2]
        //ldr     lr, [pc, #4]
        //bx      r0
        if(0xe59f1008==*(ii+jj)&&0xe7910100==*(ii+jj+1)&&0xe59fe004==*(ii+jj+2)&&0xe12fff10==*(ii+jj+3))
        {
          target=ii+jj+4;
          break;
        }
        //twl
        //10 20 9F E5 F0 2F 82 E8 0C E0 9F E5 10 FF 2F E1
        //ldr     r2, [pc, #16]
        //stmia   r2, {r4-r11,sp}
        //ldr     lr, [pc, #12]
        //bx      r0
        else if(0xe59f2010==*(ii+jj)&&0xe8822ff0==*(ii+jj+1)&&0xe59fe00c==*(ii+jj+2)&&0xe12fff10==*(ii+jj+3))
        {
          twl=true;
          target=ii+jj+4;
          break;
        }
      }
    }
    //place for softreset handler
    else
    //nfs: underground 2 (u)
    //00 40 2D E9 04 D0 4D E2 2E 03 A0 E3
    //stmfd   sp!, {lr}
    //sub     sp, sp, #4
    //mov     r0, #0xb8000000
    if(0xe92d4000==*ii&&0xe24dd004==*(ii+1)&&0xe3a0032e==*(ii+2))
    {
      getid=ii;
    }
    //kirby: canvas curse (u)
    //00 B5 81 B0 0B 48 00 21 ... 00 00 00 B8
    //push    {lr}
    //sub     sp, sp, #4
    //ldr     r0, =0xb8000000
    //movs    r1, #0
    else if(0xb081b500==*ii&&0x21004800==((*(ii+1))&0xffffff00)&&0xb8000000==*(ii+2+((*(ii+1))&0xff)))
    {
      thumb=true;
      getid=ii;
    }
    //the legend of zelda: phantom hourglass (u)
    //08 40 2D E9 2E 03 A0 E3 00 10 A0 E3
    //stmfd   sp!, {r3,lr}
    //mov     r0, #0xb8000000
    //mov     r1, #0
    //petz - dogz talent show (u)
    //08 40 2D E9 B8 00 A0 E3 00 10 A0 E3
    //stmfd   sp!, {r3,lr}
    //mov     r0, #0xb8
    //mov     r1, #0
    else if(0xe92d4008==*ii&&(0xe3a0032e==*(ii+1)||0xe3a000b8==*(ii+1))&&0xe3a01000==*(ii+2))
    {
      getid=ii;
    }
    //arkanoid ds (u)
    //08 B5 2E 20 80 06 00 21
    //push    {r3,lr}
    //movs    r0, #0x2e
    //lsls    r0, r0, #0x1a
    //movs    r1, #0
    else if(0x202eb508==*ii&&0x21000680==*(ii+1))
    {
      thumb=true;
      getid=ii;
    }
    //sonic & sega all-stars racing (u)
    //08 B5 B8 20 00 21
    //push    {r3,lr}
    //movs    r0, #0xb8
    //movs    r1, #0
    else if(0x20b8b508==*ii&&0x00002100==((*(ii+1))&0xffff))
    {
      thumb=true;
      getid=ii;
    }
    //mon coach personnel - mes recettes plaisir et ligne (f)
    //10 40 2D E9 B8 40 A0 E3 ? ? ? ? 00 00 50 E3 90 40 A0 13
    //stmfd   sp!, {r4,lr}
    //mov     r4, #0xb8
    //...
    //cmp     r0, #0
    //movne   r4, #0x90
    else if(0xe92d4010==*ii&&0xe3a040b8==*(ii+1)&&0xe3500000==*(ii+3)&&0x13a04090==*(ii+4))
    {
      getid=ii;
    }
    //just sing! (e)
    //38 40 2D E9 B8 50 A0 E3 ? ? ? ? 00 00 50 E3 90 50 A0 13
    //stmfd   sp!, {r3-r5,lr}
    //mov     r5, #0xb8
    //...
    //cmp     r0, #0
    //movne   r5, #0x90
    else if(0xe92d4038==*ii&&0xe3a050b8==*(ii+1)&&0xe3500000==*(ii+3)&&0x13a05090==*(ii+4))
    {
      getid=ii;
    }
    //bookworm (u)
    //10 40 2D E9 00 40 A0 E3 04 10 A0 E1 B8 00 A0 E3
    //stmfd   sp!, {r4,lr}
    //mov     r4, #0
    //mov     r1, r4
    //mov     r0, #0xb8
    else if(0xe92d4010==*ii&&0xe3a04000==*(ii+1)&&0xe1a01004==*(ii+2)&&0xe3a000b8==*(ii+3))
    {
      getid=ii;
    }
    //camp rock - final jam (e)
    //10 B5 B8 24 ? ? ? ? 00 28 ? ?
    //push    {r4,lr}
    //movs    r4, #0xb8
    //...
    //cmp     r0, #0
    else if(0x24b8b510==*ii&&0x00002800==((*(ii+2))&0xffff))
    {
      thumb=true;
      getid=ii;
    }
    //softreset hook
    else
    //00 10 0F E1 80 20 C1 E3 00 20 82 E1 02 F0 21 E1 80 00 01 E2 1E FF 2F E1
    //mrs r1, cpsr
    //bic r2, r1, #0x80
    //orr r2, r2, r0
    //msr cpsr_c, r2
    //and r0, r1, #0x80
    //bx lr
    if(0xe10f1000==*ii&&0xe3c12080==*(ii+1)&&0xe1822000==*(ii+2)&&0xe121f002==*(ii+3)&&0xe2010080==*(ii+4)&&0xe12fff1e==*(ii+5))
    {
      iSoftResetHook=u32(ii+5);
    }
    if(target&&getid&&iSoftResetHook) break;
  }
  if(!target)
  {
    dbg_printf("IntrMain not found\n");
    return;
  }
  u32 diff=0;
  if(twl)
  {
    if(0xe12fff1e!=target[11])
    {
      dbg_printf("unknown twl IntrMain\n");
      return;
    }
    diff=target[3]-(u32)(target+14);
  }
  else
  {
    diff=target[1]-(u32)(target+2);
  }
  iVBlank=(target[0]-diff);
  if(!getid)
  {
    dbg_printf("getid not found\n");
    return;
  }
  iSoftResetHandler=(u32)(getid+(thumb?2:3));
  if(thumb)
  {
    PatchMem(KArm7,s32(getid),0x47704800); //ldr r0, =0xfc2
    PatchMem(KArm7,s32(getid+1),0xfc2);    //bx lr
  }
  else
  {
    PatchMem(KArm7,s32(getid),0xe1df00b0);   // ldrh r0, =0xfc2
    PatchMem(KArm7,s32(getid+1),0xe12fff1e); // bx lr
    PatchMem(KArm7,s32(getid+2),0xfc2);
  }
}

void CPatcher::PatchArm7E(u32 anArm7Address2)
{
  SearchArm7();
  if(iVBlank&&iSoftResetHandler&&iSoftResetHook)
  {
    if(IsSoftReset()) PatchSoftReset7(anArm7Address2);
    if(IsCheats()) PatchCheats();
  }
  else
  {
    showMsgandWait(MSGID_LOADING,MSG_ERROR_SR_COLOR);
  }
}

void CPatcher::PatchSoftReset7(u32 anArm7Address2)
{
  for(u32* ii=(u32*)patch7wram_sr_bin;ii<(u32*)(patch7wram_sr_bin+patch7wram_sr_bin_size);++ii)
  {
    if(0xefc0de01==*ii)
    {
      *ii=anArm7Address2+1;
    }
  }
  u32 size=patch7wram_sr_bin_size/4;
  u32* ptr=(u32*)patch7wram_sr_bin;
  for(u32 ii=0;ii<size;ii++) PatchMem(KArm7,iSoftResetHandler+ii*sizeof(u32),*(ptr+ii));
  PatchMem(KArm7,iSoftResetHook,GenerateArmBL(iSoftResetHook,iSoftResetHandler,false));
}

void CPatcher::PatchCheats(void)
{
  PatchMem(KArm7,0x27fff68,*(u32*)iVBlank);
  PatchMem(KArm7,iVBlank,iPatchArm7Entry);
  memcpy((void*)iPatchArm7Entry,patch7_bin,patch7_bin_size);
  iPatchArm7Entry=LoadARCodeTable(iPatchArm7Entry,iPatchArm7Entry+patch7_bin_size);
}

void CPatcher::FixProtectionRegion2(void)
{
  if(!IsDMA()&&!iProtectionRegion2Fixed)
  {
    NeedUncompressed();
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
    }
    iProtectionRegion2Fixed=true;
  }
}
