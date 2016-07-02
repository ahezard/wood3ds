/*
    reset.cpp
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

#include "reset.h"
#include "memcleaner.h"
#include "../../share/ipctool.h"

ITCM_CODE static void BootITCM(void)
{
  while(true)
  {
    if(!(REG_IPC_FIFO_CR&IPC_FIFO_SEND_FULL))
    {
      REG_IPC_FIFO_TX=FIFO_PACK_VALUE32(FIFO_USER_01,SYS_BOOT_SLOT1);
      break;
    }
  }
  while(true)
  {
    while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY);
    u32 res=REG_IPC_FIFO_RX;
    if(FIFO_PACK_VALUE32(FIFO_USER_01,SYS_ARM7_READY_BOOT)==res) break;
  }
  REG_IPC_FIFO_CR=IPC_FIFO_SEND_CLEAR|IPC_FIFO_ERROR|IPC_FIFO_ENABLE;
}


ITCM_CODE void resetAndLoop(void)
{
  zeroMemoryITCM((void*)0x023c0000,0x3a000);
  if(copy_data_itcm) copyMemoryITCM((void*)0x06040000,(void*)data_destination_itcm,data_size_itcm);
  resetARM9Memory();
  IC_InvalidateAllITCM();
  DC_FlushAllITCM();
  DC_InvalidateAllITCM();

  BootITCM();
  swiSoftResetITCM();
}
