/*
    gbanes.cpp
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
#include "gba_nes_patch_bin.h"

bool CGbaPatcher::PatchNes(void)
{
  bool res=false;
  u32 jump=Data()[0];
  if((jump&0xff000000)==0xea000000)
  {
    size_t index=(jump&0xffffff)+2;
    if(Data()[index]==0xe28f503c&&Data()[index+1]==0xe8b503d3&&Data()[index+2]==0xe129f007&&Data()[index+3]==0xe281deba&&Data()[index+4]==0xe129f008&&Data()[index+5]==0xe281debe&&Data()[index+6]==0xe129f009&&Data()[index+7]==0xe281dc0b&&Data()[index+8]==0xe92d0003&&Data()[index+9]==0xef110000&&Data()[index+10]==0xe8bd8001)
    {
      res=true;
      iSaveSize=8*1024;
      u32 patch=0xea000000|(0x3fdf5-index);
      iWriter->Write(36+index*4,(u8*)&patch,sizeof(patch));
      iWriter->Write(0xff800,gba_nes_patch_bin,gba_nes_patch_bin_size);
      patch=Data()[index+17];
      iWriter->Write(0xff840,(u8*)&patch,sizeof(patch));
      size_t index2=(patch-0x08000000)/4;
      if(Data()[index2-1]==0x3032)
      {
        patch=0x060000f8;
        iWriter->Write(0xff86c,(u8*)&patch,sizeof(patch));
      }
    }
  }
  return res;
}
