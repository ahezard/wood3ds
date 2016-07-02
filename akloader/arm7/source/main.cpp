/*
    main.cpp
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
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

#include <nds.h>
#include <string.h>
#include <nds/arm7/clock.h>
#include "../../share/ipctool.h"

void VblankHandler(void) {}

// arm9 can not access memory space 0x37f8000 - 0x380ffff
// load those arm7 code which can not be loaded by arm9 here
// this function can not load official roms because their arm7 code are big
static u32 homebrewOffset=0;
static void loadHomebrewArm7Code(u32 aSize)
{
  // load_arm7
  memcpy((void*)(__NDSHeader->arm7destination+homebrewOffset),(void*)0x23fc000,aSize);
  homebrewOffset+=aSize;
  fifoSendValue32(FIFO_USER_02,HB_ARM7_LOAD_DONE);
}

#ifdef __cplusplus
extern "C" {
#endif
void swiSoftReset_0(void);
void readBios(void* dest,u32 src,u32 size);
#ifdef __cplusplus
}
#endif

static void getKey(void* aBuffer)
{
  readBios(aBuffer,0x30,IPC_KEY_SIZE*sizeof(u32));
  fifoSendValue32(FIFO_USER_03,ENC_KEY_FINISHED);
}

static void prepairReset()
{
  REG_IME=IME_DISABLE;

  REG_POWERCNT|=POWER_SOUND;

  //reset DMA&Timers
  for(u32 ii=0;ii<4;++ii)
  {
    DMA_SRC(ii)=0;
    DMA_DEST(ii)=0;
    DMA_CR(ii)=0;
    TIMER_CR(ii)=0;
    TIMER_DATA(ii)=0;
  }

  REG_IE=0;
  REG_IF=~0;
  REG_IPC_SYNC=0;

  fifoSendValue32(FIFO_USER_01,SYS_ARM7_READY_BOOT);
}

void systemMsgHandler(u32 value, void* data)
{
  switch(value)
  {
    case SYS_BOOT_SLOT1:
    case SYS_BOOT_SLOT2:
      prepairReset();
      swiSoftReset_0();
      break;
  }
}

void homebrewMsgHandler(u32 value, void* data)
{
  loadHomebrewArm7Code(value);
}

void encryptionMsgHandler(u32 value, void* data)
{
  getKey((void*)value);
}

static u32 GenerateArmB(u32 aSource,u32 aTarget)
{
  u32 result=0;
  s32 diff=aTarget-aSource-8;
  if(diff<=33554428&&diff>=-33554432&&(diff&3)==0)
  {
    result=0xea000000|((diff>>2)&0xffffff);
  }
  return result;
}

void resetMsgHandler(u32 value, void* data)
{
  u32* isr=NULL;
  for(u32* ii=(u32*)__NDSHeader->arm7destination;ii<(u32*)(__NDSHeader->arm7destination+__NDSHeader->arm7binarySize);ii++)
  {
    //00 00 4F E1
    if(0xe14f0000==*ii) //mrs r0, spsr
    {
      isr=ii;
      break;
    }
  }
  if(isr)
  {
    *isr=GenerateArmB((u32)isr,value);
  }
  fifoSendValue32(FIFO_USER_04,(u32)isr);
}

#ifdef __cplusplus
extern "C" {
#endif
void __libnds_exit(int rc) {}
#ifdef __cplusplus
}
#endif

int main(int argc,char** argv)
{
  irqInit();
  fifoInit();
  irqSet(IRQ_VBLANK, VblankHandler);
  irqEnable(IRQ_VBLANK);
  fifoSetValue32Handler(FIFO_USER_01, systemMsgHandler, 0);
  fifoSetValue32Handler(FIFO_USER_02, homebrewMsgHandler, 0);
  fifoSetValue32Handler(FIFO_USER_03, encryptionMsgHandler, 0);
  fifoSetValue32Handler(FIFO_USER_04, resetMsgHandler, 0);

  // Keep the ARM7 out of main RAM
  while(true) swiWaitForVBlank();
}
