/*
    gbasleep.cpp
    Copyright (C) 2007 By Dan Weiss (Dwedit)
    Copyright (C) 2008-2009 somebody
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

#include "gbapatcher.h"
#include <stdio.h>
#include <string.h>
#include "gba_sleep_patch_bin.h"

const int ACTIVATION_SIZE=36;
const int FAKE_SIZE=512*1024; //search in first 512k
const int PATCH_LENGTH=2048;

void CGbaPatcher::CommonSleepSearch(void)
{
  //The LDR,=xxxxxxxx ARM instruction:
  //
  //Load/Store=1
  //Writeback=0
  //Byte/Word=0
  //Up/Down=either
  //Pre/Post=?
  //Immediate Offset=0
  //rn = r15
  //cond= always

  //1110 01 0 p u 0 0 1 1111 rrrr oooooooooooo
  //Instruction Mask:
  //1111 11 1 0 0 1 1 1 1111 0000 000000000000
  //Instruction Bits:
  //1110 01 0 0 0 0 0 1 1111 0000 000000000000

  //The STR rd,[rn] ARM instruction:
  //Load/Store=0
  //Writeback=0
  //Byte/Word=0
  //Up/Down=?
  //Pre/Post=?
  //Immediate Offset=0
  //cond= always

  //1110 01 0 p u 0 0 0 rnrn rdrd 000000000000
  //MASK:
  //1111 11 1 0 0 1 1 1 0000 0000 111111111111
  //BITS:
  //1110 01 0 0 0 0 0 0 0000 0000 000000000000

  //THUMB:
  //LDR rd,=xxxxxxxx
  //01001rrroooooooo
  //1111100000000000
  //0100100000000000

  //STR rd,[rb]
  //01100 00000 bbb ddd
  //11111 11111 000 000
  //01100 00000 000 000
  const u32 ldr_mask=0xFE7F0000;
  const u32 ldr_bits=0xE41F0000;
  const u32 str_mask=0xFE700FFF;
  const u32 str_bits=0xE4000000;
  const u32 t_ldr_mask=0xF800;
  const u32 t_ldr_bits=0x4800;
  const u32 t_str_mask=0xFFC0;
  const u32 t_str_bits=0x6000;

  u32 reg_data[16]; //contents of register
  u32 reg_addr[16]; //address of the data which got written
  u32 reg_lastwrite[16]; //PC at which the data was written to

  memset(reg_data,0,sizeof(reg_data));
  memset(reg_addr,0,sizeof(reg_addr));
  memset(reg_lastwrite,0,sizeof(reg_lastwrite));

  SetTrimSize();

  if((iTrimSize+PATCH_LENGTH)>iWriter->MaxSize()) return;

  u32 fake_size=FAKE_SIZE;
  if(fake_size>iTrimSize) fake_size=iTrimSize;

  //arm
  u32 patch_location=iTrimSize;
  u32 activation_location=patch_location+gba_sleep_patch_bin_size;
  u32 search_size=fake_size/sizeof(u32);
  for(u32 ii=0;ii<search_size;ii++)
  {
    u32 word=Data()[ii];
    //check for LDR xx,=xxxxxxxx
    if((word&ldr_mask)==ldr_bits)
    {
      u32 pc=ii*sizeof(u32)+8;
      //1110 01 0 p u 0 0 1 1111 rrrr oooooooooooo
      //0000 00 0 0 1 0 0 0 0000 0000 000000000000
      int updown=-1;
      if((word&0x800000))
      {
        updown=1;
      }
      int rd=((word>>12)&0x0F);
      int offset=(word&0xFFF);
      u32 address=(updown)*offset + pc;
      u32 memdata=0xDEADBEEF;
      if(address>=0&&address<iSize-3)
      {
        if((address&0x03)==0x00)
        {
          memdata=Data()[address/sizeof(u32)];
        }
      }
      reg_data[rd]=memdata;
      reg_addr[rd]=address;
      reg_lastwrite[rd]=ii*sizeof(u32);
    }
    else if((word&str_mask)==str_bits)
    {
      //1110 01 0 p u 0 0 0 rnrn rdrd 000000000000
      int rn,rd;
      rn=((word>>16)&0x0F);
      rd=((word>>12)&0x0F);
      u32 myaddress=ii*sizeof(u32);
      bool okay=true
       &&(reg_data[rn]==0x03007FFC)
       &&(myaddress-reg_lastwrite[rd]<64)
       &&(myaddress-reg_lastwrite[rn]<64)
       &&(reg_lastwrite[rd]!=0)
       &&(reg_lastwrite[rn]!=0)
       &&((reg_data[rd]&0xFF000000)==0x03000000)
       ;
      if(!okay&&reg_data[rn]==0x03007FFC)
      {
        if(myaddress==0xE0)
        {
          okay=true;
        }
      }
      if(myaddress<reg_lastwrite[rd]||myaddress<reg_lastwrite[rn])
      {
        okay=false;
      }
      if(okay)
      {
        {
          //str rd,[rn] >> BX rn
          Add2(myaddress,0xE12FFF10+rn);
          //ldr rn,=XXXX >> ldr rn,=activation_jump
          u32 data_address=reg_addr[rn];
          if(data_address>=0&&data_address<iSize-3)
          {
            Add2(data_address,0x08000000+activation_location);
          }
        }
        Add3(activation_location,myaddress+4,rd,rn);
        activation_location+=ACTIVATION_SIZE;
      }
    }
  }
  //thumb
  memset(reg_data,0,sizeof(reg_data));
  memset(reg_addr,0,sizeof(reg_addr));
  memset(reg_lastwrite,0,sizeof(reg_lastwrite));

  search_size=fake_size/sizeof(u16);
  //this code is Little Endian Only
  for(u32 ii=0;ii<search_size;ii++)
  {
    u16 word=Data16()[ii];
    //check for LDR xx,=xxxxxxxx
    if ((word&t_ldr_mask)==t_ldr_bits)
    {
      u32 pc=ii*sizeof(u16)+4;
      //01001rrroooooooo
      int rd=((word>>8)&0x07);
      int offset=(word&0xFF);
      u32 address=offset*4+pc;
      u32 memdata=0xDEADBEEF;
      address&=~0x03;

      if(address>=0&&address<iSize-3)
      {
        if((address&0x03)==0x00)
        {
          memdata=Data()[address/sizeof(u32)];
        }
      }
      reg_data[rd]=memdata;
      reg_addr[rd]=address;
      reg_lastwrite[rd]=ii*sizeof(u16);
    }
    else if((word&t_str_mask)==t_str_bits)
    {
      //01100 00000 bbb ddd
      int rb,rd;
      rb=((word>>3)&0x07);
      rd=((word>>0)&0x07);
      u32 myaddress=ii*sizeof(u16);
      bool okay=true
       &&(reg_data[rb]==0x03007FFC)
       &&(myaddress-reg_lastwrite[rd]<80)
       &&(myaddress-reg_lastwrite[rb]<64)
       &&(reg_lastwrite[rd]!=0)
       &&(reg_lastwrite[rb]!=0)
       &&((reg_data[rd]&0xFF000000)==0x03000000)
       ;

      if(okay)
      {
        {
          //str rd,[rn] >> BX rb
          Add4(myaddress,0x4700+(rb<<3));
          //ldr rn,=XXXX >> ldr rn,=activation_jump
          u32 data_address=reg_addr[rb];
          if(data_address>=0&&data_address<iSize-3)
          {
            Add2(data_address,0x08000000+activation_location);
          }
        }
        Add3(activation_location,myaddress+1+2,rd,rb);
        activation_location+=ACTIVATION_SIZE;
        reg_data[rb]=0; //for mario kart. may regress for another games.
      }
    }
  }
}

void CGbaPatcher::SetTrimSize(void)
{
  u8 byte=Data8()[iSize-1];
  u32 bottom=iSize-2,top=bottom-PATCH_LENGTH-16,alignedSize=iSize+(16-(iSize&15));
  for(u32 ii=bottom;ii>=top;ii--)
  {
    if(Data8()[ii]!=byte||ii==top)
    {
      iTrimSize=ii+4;
      iTrimSize=iTrimSize+(16-(iTrimSize&15));
      if(iTrimSize>alignedSize) iTrimSize=alignedSize;
      break;
    }
  }
}

void CGbaPatcher::CommonSleepPatch(void)
{
  if(!(iCount2+iCount4)) return;
  for(u32 ii=0;ii<iCount2;ii++)
  {
    iWriter->Write(iPatchInfo2[ii].iOffset,(u8*)&(iPatchInfo2[ii].iValue),sizeof(iPatchInfo2[ii].iValue));
  }
  for(u32 ii=0;ii<iCount4;ii++)
  {
    iWriter->Write(iPatchInfo4[ii].iOffset,(u8*)&(iPatchInfo4[ii].iValue),sizeof(iPatchInfo4[ii].iValue));
  }
  iResultSize=iTrimSize;
  iWriter->Write(iTrimSize,gba_sleep_patch_bin,gba_sleep_patch_bin_size);
  iResultSize=iTrimSize+gba_sleep_patch_bin_size+ACTIVATION_SIZE*iCount3;
  for(u32 ii=0;ii<iCount3;ii++)
  {
    u32 mem[9];
    mem[0]=0xE92D5003;
    mem[1]=0xE1A0C000+iPatchInfo3[ii].iRa;
    mem[2]=0xE1A01000+iPatchInfo3[ii].iRb;
    mem[3]=0xE1A0000C;
    mem[4]=0xEB000000+((0x00FFFFFF)&((iTrimSize-(iPatchInfo3[ii].iLocation+8+16))>>2));
    mem[5]=0xE8BD5003;
    mem[6]=0xE59F0000+(iPatchInfo3[ii].iRb<<12);
    mem[7]=0xE12FFF10+iPatchInfo3[ii].iRb;
    mem[8]=iPatchInfo3[ii].iAddress+0x08000000;
    iWriter->Write(iPatchInfo3[ii].iLocation,(u8*)mem,sizeof(mem));
  }
}
