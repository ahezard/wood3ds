/*
    patches_hb.cpp
    Copyright (C) 2011 yellow wood goblin

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
#include "hb_reset_bin.h"
#include "reset.h"

static u32 GetValueFromLDR(u32 ptr)
{
  u32 result=0;
  u32 value=*(u32*)ptr;
  if((value&0xffff0000)==0xe59f0000)
  {
    ptr+=(value&0xfff)+8;
    result=*(u32*)ptr;
  }
  return result;
}

void CHbPatcher::Patch(void)
{
  bool installed=false;
  u32* isr9=NULL;
  u32 start=0,startITCM=0,isr9real=0;
  for(size_t ii=0;ii<2;++ii)
  {
    if(ii&&!copy_data_itcm) break;
    u32* begin=(u32*)__NDSHeader->arm9destination;
    u32* end=(u32*)(copy_data_itcm?0x023c0000:(__NDSHeader->arm9destination+__NDSHeader->arm9binarySize));
    if(ii)
    {
      begin=(u32*)0x06040000;
      end=(u32*)(0x06040000+data_size_itcm);
    }
    for(u32* jj=begin;jj<end;++jj)
    {
      if(isr9&&start) break;
      //00 00 4F E1
      if(0xe14f0000==*jj) //mrs r0, spsr
      {
        isr9=jj;
        isr9real=(u32)jj;
        if(ii) isr9real=(isr9real-0x06040000)+0x023c0000;
      }
      //1F 00 A0 E3 00 F0 29 E1
      else if(0xe3a0001f==*jj&&0xe129f000==*(jj+1)&&start==0) //mov r0, #0x1f|msr cpsr_cf, r0
      {
        start=GetValueFromLDR((u32)(jj+3));
        startITCM=GetValueFromLDR((u32)(jj+4));
      }
    }
  }
  if(isr9&&start)
  {
    fifoSendValue32(FIFO_USER_04,0x23ff900);
    while(!fifoCheckValue32(FIFO_USER_04));
    u32 isr7=fifoGetValue32(FIFO_USER_04);
    if(isr7)
    {
      u32 isr9_itcm=((u32)isr9real)-start+startITCM;
      u32 second_instruction=isr9[1];
      isr9[0]=0xe51ff004; //ldr pc, [pc, #-4]
      isr9[1]=0x23ff904;
      memcpy((void*)0x23ff900,hb_reset_bin,hb_reset_bin_size);
      for(u32* ii=(u32*)0x23ff900;ii<(u32*)(0x23ff900+hb_reset_bin_size);ii++)
      {
        if(0xefc0de01==*ii)
        {
          *ii=isr7+4;
        }
        else if(0xefc0de02==*ii)
        {
          *ii=isr9_itcm+8;
        }
        else if(0xefc0de03==*ii)
        {
          *ii=second_instruction;
        }
      }
      *(u32*)0x27fff6c=1; //arm9ResetFlag
      installed=true;
    }
  }
  if(!installed)
  {
    showMsgandWait(MSGID_LOADING,MSG_ERROR_SR_COLOR);
  }
}
