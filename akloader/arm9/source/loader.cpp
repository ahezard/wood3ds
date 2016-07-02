/*
    loader.cpp
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

#include <malloc.h>
#include "loader.h"
#include "../../share/ipctool.h"
#include "../../share/gamecode.h"
#include "reset.h"
#include "dbgtool.h"
#include <fcntl.h>
#include <unistd.h>

#if defined(_STORAGE_r4)
#include <ior4.h>
#include "rpgmaps.h"
static void loadMenu(const char* filename)
{
  ioR4ReadCardInfo();
  ioR4SendMap(GetR4Address(filename)&0xfffffffe);
  ioR4ReadCardInfo();
  zeroMemoryITCM((void*)0x027ffe00,0x200);
  ioR4CryptedRead(0,(u32*)0x027ffe00,128);
  u32 binsize=__NDSHeader->arm9binarySize,pos=0;
  while(pos<binsize)
  {
    ioR4CryptedRead(__NDSHeader->arm9romOffset+pos,(u32*)(__NDSHeader->arm9destination+pos),128);
    pos+=512;
  }
  binsize=__NDSHeader->arm7binarySize;pos=0;
  while(pos<binsize)
  {
    ioR4CryptedRead(__NDSHeader->arm7romOffset+pos,(u32*)(__NDSHeader->arm7destination+pos),128);
    pos+=512;
  }
}
#endif

bool loadFile(const char* filename,u32& originSpeed,u32& changedSpeed,u32& sizeMask,bool& hasRSA)
{
#if defined(_STORAGE_r4)
  if(strstr(filename,"/_ds_menu.dat"))
  {
    loadMenu(filename);
    return true;
  }
#endif
  int f=open(filename,O_RDONLY);
  if(f<0) return false;

  struct stat st;
  if(0==fstat(f,&st)) sizeMask=size_mask(st.st_size);
  else sizeMask=0xffffffff;

  // load_header
  lseek(f,0,SEEK_SET);
  // save card control info
  changedSpeed=__NDSHeader->cardControl13;
  zeroMemoryITCM((void*)0x027ffe00,0x200);
  read(f,(u8*)__NDSHeader,352);
  originSpeed=__NDSHeader->cardControl13;

  // load_arm9
  u8* arm9_dest_mem=(u8*)__NDSHeader->arm9destination;
  u32 arm9_code=__NDSHeader->arm9romOffset;
  dbg_printf("arm9 code offset %08x\n",arm9_code);
  u32 arm9_size=__NDSHeader->arm9binarySize;
  u32 arm9_curr_size=arm9_size;
  if(((u32)arm9_dest_mem+arm9_size)>0x023c0000)
  {
    arm9_curr_size=0x023c0000-(u32)arm9_dest_mem;
    dbg_printf("big arm9 %08x\n",arm9_curr_size);
  }

  lseek(f,arm9_code,SEEK_SET);

  /*ssize_t readed=*/read(f,arm9_dest_mem,arm9_curr_size);

  if(arm9_curr_size<arm9_size)
  {
    copy_data_itcm=true;
    data_size_itcm=arm9_size-arm9_curr_size;
    u8* target=(u8*)0x06040000;u8* source=(u8*)0x23fc000;
    while(arm9_curr_size<arm9_size)
    {
      u32 tail=arm9_size-arm9_curr_size;
      if(tail>0x2000) tail=0x2000;
      read(f,source,tail);
      copyMemoryITCM(source,target,tail);
      target+=tail;
      arm9_curr_size+=tail;
    }
    zeroMemoryITCM((void*)0x23fc000,0x2000);
    DC_FlushRange((void*)0x23fc000,0x2000);
  }

  dbg_printf("arm9 code offset %08x\n",arm9_code);
  dbg_printf("arm9 start memory %08x\n",(u32)arm9_dest_mem);
  dbg_printf("arm9 entry code:\n");
  for(size_t i=0;i<4;++i)
  {
    dbg_printf("%08x",*(vu32*)(__NDSHeader->arm9executeAddress+i*4));
  }

  // load_arm7
  if( __NDSHeader->arm7destination >= 0x037F8000 || 0x23232323 == gamecode(__NDSHeader->gameCode) ) //23->'#'
  {
    u32 arm7_code=__NDSHeader->arm7romOffset;
    u32 arm7_size=__NDSHeader->arm7binarySize;
    lseek(f,arm7_code,SEEK_SET);
    while(arm7_size>0)
    {
      u32 send_size=0x2000;
      if(arm7_size<send_size) send_size=arm7_size;
      read(f,(void*)0x23fc000,send_size);
      DC_FlushRange((void*)0x23fc000,send_size);
      fifoSendAndReceive(FIFO_USER_02,send_size,HB_ARM7_LOAD_DONE);
      arm7_size-=send_size;
    }
    zeroMemoryITCM((void*)0x23fc000,0x2000);
    DC_FlushRange((void*)0x23fc000,0x2000);
  }
  else
  {
    u8* arm7_dest_mem=(u8*)__NDSHeader->arm7destination;
    u32 arm7_code=__NDSHeader->arm7romOffset;
    u32 arm7_size=__NDSHeader->arm7binarySize;
    lseek(f,arm7_code,SEEK_SET);
    ssize_t loaded_arm7_size=read(f,arm7_dest_mem,arm7_size);
    (void)loaded_arm7_size;
    dbg_printf("arm7 start memory %08x\n",(u32)arm7_dest_mem);
    dbg_printf("arm7 size: %08x/%08x\n",arm7_size,loaded_arm7_size);
  }

  lseek(f,__NDSHeader->romSize,SEEK_SET);
  u32 rsaSign=0;
  read(f,&rsaSign,4);
  if(0x00016361==rsaSign) // if this program has a rsa signature, try to do not change header
  {
    hasRSA = true;
  }
  close(f);

  // overwrite card control info only if setting speed slower than origin speed
  // keeping origin header can get better compatibility and make most download play program to run
  if((changedSpeed&0x1fff)>(__NDSHeader->cardControl13&0x1fff))
  {
    __NDSHeader->cardControl13=changedSpeed;
  }

  //this kills download play in korean games.
  //__NDSHeader->reserved1[8]=0; // change this for run iQue game on U/E/J machines
  //__NDSHeader->headerCRC16=swiCRC16(0xFFFF,__NDSHeader,0x15E);

  cwl();
  wait_press_b();

  return true;
}
