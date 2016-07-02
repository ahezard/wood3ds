/*
    memcleaner.cpp
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
#include "reset.h"

void resetInterrupts(void)
{
  REG_IME=IME_DISABLE;
}

ITCM_CODE void resetARM9Memory(void)
{
  // DMA & TIMERS
  for(u32 ii=0;ii<4;++ii)
  {
    DMA_CR(ii)=0;
    DMA_SRC(ii)=0;
    DMA_DEST(ii)=0;
    DMA_FILL(ii)=0;
    TIMER_CR(ii)=0;
    TIMER_DATA(ii)=0;
  }

  while(REG_DISPSTAT&DISP_IN_VBLANK) ;
  while((REG_DISPSTAT&DISP_IN_VBLANK)==0) ; // wait for VBLANK to avoid screen picture break

  // VIDEO
  VRAM_CR=0x80808080;
  VRAM_E_CR=0x80;
  VRAM_F_CR=0x80;
  VRAM_G_CR=0x80;
  VRAM_H_CR=0x80;
  VRAM_I_CR=0x80;

  zeroMemoryITCM(BG_PALETTE,2048); //0x05000000
  zeroMemoryITCM(OAM,2048); //0x07000000
  zeroMemoryITCM((void*)0x04000000,0x60);
  zeroMemoryITCM((void*)0x04001000,0x60);
  zeroMemoryITCM(VRAM,656*1024); //0x06800000..0x068a3fff

  VRAM_CR=0x00000000;
  VRAM_E_CR=0;
  VRAM_F_CR=0;
  VRAM_G_CR=0;
  VRAM_H_CR=0;
  VRAM_I_CR=0;

  REG_POWERCNT=0x820f; // turn on all engines
  REG_EXMEMCNT=0xe880; // restore memory control

  // Interrupt
  REG_IE=0;
  REG_IF=~0;
  REG_IPC_SYNC=0;

#if defined(_STORAGE_ak2i)
  *(u32*)0x27ffc00=0x0fc2;
  *(u16*)0x27ffc10=0x5835;
#endif
}
