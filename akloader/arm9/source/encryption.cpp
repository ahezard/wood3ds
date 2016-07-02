/*
    encryption.cpp
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

#include "encryption.h"
#include "../../share/gamecode.h"
#include "../../share/ipctool.h"
#include "reset.h"

void Decrypt(void)
{
  if(0x4000==__NDSHeader->arm9romOffset&&swiCRC16(0xffff,(void*)__NDSHeader->arm9destination,0x4000)==__NDSHeader->secureCRC16)
  {
    CEncryption enc;
    enc.DecryptSecureArea();
  }
}

CEncryption::CEncryption()
{
  iGameCode=gamecode(__NDSHeader->gameCode);
  iCardHash=(u32*)0x23b0000;
}

CEncryption::~CEncryption()
{
  zeroMemoryITCM((void*)iCardHash,IPC_KEY_SIZE*sizeof(u32));
  DC_FlushAll();
  DC_InvalidateAll();
}

void CEncryption::DecryptSecureArea(void)
{
  u32* secureArea=(u32*)__NDSHeader->arm9destination;
  InitKey(iGameCode,false);
  CryptDown(secureArea);
  InitKey(iGameCode,true);
  for(int ii=0;ii<0x200;ii+=2) CryptDown(secureArea+ii);
  if(secureArea[0]==0x72636e65/*'encr'*/&&secureArea[1]==0x6a624f79/*'yObj'*/)
  {
    secureArea[0]=0xe7ffdeff;
    secureArea[1]=0xe7ffdeff;
  }
}

void CEncryption::InitKey(u32 aGameCode,bool aType)
{
  DC_InvalidateRange(iCardHash,IPC_KEY_SIZE*sizeof(u32));
  fifoSendAndReceive(FIFO_USER_03,(u32)iCardHash,ENC_KEY_FINISHED);
  iKeyCode[0]=aGameCode;
  iKeyCode[1]=aGameCode/2;
  iKeyCode[2]=aGameCode*2;

  ApplyKey();
  ApplyKey();
  iKeyCode[1]=iKeyCode[1]*2;
  iKeyCode[2]=iKeyCode[2]/2;
  if(aType) ApplyKey();
}

void CEncryption::ApplyKey(void)
{
  u32 scratch[2];

  CryptUp(&iKeyCode[1]);
  CryptUp(&iKeyCode[0]);
  memset(scratch,0,sizeof(scratch));

  for(int ii=0;ii<0x12;++ii)
  {
    iCardHash[ii]=iCardHash[ii]^bswap(iKeyCode[ii%2]);
  }
  for(int ii=0;ii<IPC_KEY_SIZE;ii+=2)
  {
    CryptUp(scratch);
    iCardHash[ii]=scratch[1];
    iCardHash[ii+1]=scratch[0];
  }
}

void CEncryption::CryptUp(u32* aPtr)
{
  u32 x=aPtr[1];
  u32 y=aPtr[0];
  u32 z;
  for(int ii=0;ii<0x10;++ii)
  {
    z=iCardHash[ii]^x;
    x=iCardHash[0x012+((z>>24)&0xff)];
    x=iCardHash[0x112+((z>>16)&0xff)]+x;
    x=iCardHash[0x212+((z>>8)&0xff)]^x;
    x=iCardHash[0x312+((z>>0)&0xff)]+x;
    x=y^x;
    y=z;
  }
  aPtr[0]=x^iCardHash[0x10];
  aPtr[1]=y^iCardHash[0x11];
}

void CEncryption::CryptDown(u32* aPtr)
{
  u32 x=aPtr[1];
  u32 y=aPtr[0];
  u32 z;
  for(int ii=0x11;ii>0x01;--ii)
  {
    z=iCardHash[ii]^x;
    x=iCardHash[0x012+((z>>24)&0xff)];
    x=iCardHash[0x112+((z>>16)&0xff)]+x;
    x=iCardHash[0x212+((z>>8)&0xff)]^x;
    x=iCardHash[0x312+((z>>0)&0xff)]+x;
    x=y^x;
    y=z;
  }
  aPtr[0]=x^iCardHash[0x01];
  aPtr[1]=y^iCardHash[0x00];
}
