/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#include "rpgprotocol.h"
#include <stdio.h>

#define PAGESIZE_0            (0<<24)
#define PAGESIZE_4            (7<<24)
#define PAGESIZE_512        (1<<24)
#define PAGESIZE_1K            (2<<24)
#define PAGESIZE_2K            (3<<24)

void xrpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer )
{
    CARD_CR1H = CARD_CR1_ENABLE | CARD_CR1_IRQ;

    for( u32 i=0; i<2; ++i )
    {
        CARD_COMMAND[i*4+0] = command[i]>>24;
        CARD_COMMAND[i*4+1] = command[i]>>16;
        CARD_COMMAND[i*4+2] = command[i]>>8;
        CARD_COMMAND[i*4+3] = command[i]>>0;
    }

    switch( pageSize )
    {
    case 0:
        pageSize = PAGESIZE_0;
        break;
    case 4:
        pageSize = PAGESIZE_4;
        break;
    case 512:
        pageSize = PAGESIZE_512;
        break;
    case 1024:
        pageSize = PAGESIZE_1K;
        break;
    case 2048:
        pageSize = PAGESIZE_2K;
        break;
    default:
        pageSize = PAGESIZE_0;
    }

    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | pageSize | latency | 0x00180000 | 0x00406000 /*((*(unsigned int*)0x027ffe60) & ~0x07001FFF)*/;
    //printf("send %08x %08x\n", command[0], command[1] );

    u32 * pbuf = (u32 *)buffer;
    bool useBuf = ( NULL != pbuf );
    while( CARD_CR2 & CARD_BUSY )
    {
        if( CARD_CR2 & CARD_DATA_READY ) {
            u32 data = CARD_DATA_RD;
            if( useBuf )
                *pbuf++ = data;
        }
    }
}

void xrpgWaitCmdBusy()
{
    u32 cmd[2] = { 0xB8000000, 0x00000000 };
    u32 data = 0;
    do {
        xrpgSendCommand(    cmd, 4, 0, &data );
        //printf("%08x", data );
    } while( 0x00000fc2 != data );
}

void xrpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize )
{
    u8 byte6 = deviceSelection | (tables<<2);
    u32 cmd[2] = { 0xD7000000,
                    (byte6<<16)|(table1BlockSize<<8)|table0BlockSize };
    //{ table0BlockSize, table1BlockSize, byte6, 00, 00, 00, 00, 0xD7 };
    xrpgSendCommand( cmd, 0, 0, NULL );
}

void xrpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress )
{
    tableName &= 0x0F;
    u32 cmd[2] = { 0xD0000000 | (tableInRamAddress >> 8),
                    ((tableInRamAddress & 0xff) << 24) | ((u8)tableName << 16) };

    xrpgSendCommand( cmd, 0, 0, NULL );
}


u32 xrpgReadSram4( u32 address )
{
    xrpgSetDeviceStatus( 0x02, 0x00, 0x00, 0x00 );
    u32 cmd[2] = { 0xB7000000 | (address >> 8), (address & 0xff) << 24 };
    //{ ((address & 0xff) << 24) | 0x00000000, 0xB7000000 | (address >> 8) };
    u32 data = 0xFF;
    xrpgSendCommand( cmd, 4, 0, &data );
    return data;
}

void xrpgReadSram512( u32 address, void * buffer )
{
    xrpgSetDeviceStatus( 0x02, 0x00, 0x00, 0x00 );
    u32 cmd[2] = { 0xB7000000 | (address >> 8), (address & 0xff) << 24 };
    //{ ((address & 0xff) << 24) | 0x00000000, 0xB7000000 | (address >> 8) };
    xrpgSendCommand( cmd, 512, 0, buffer );
}

void xrpgReadSram528( u32 address, void * buffer )
{
    xrpgSetDeviceStatus( 0x02, 0x00, 0x00, 0x00 );
    u32 cmd[2] = { 0xB7000000 | (address >> 8), (address & 0xff) << 24 };
    //{ ((address & 0xff) << 24) | 0x00000000, 0xB7000000 | (address >> 8) };
    xrpgSendCommand( cmd, 1024, 0, buffer );
}

void xrpgWriteSram( u32 address, u32 data, u8 byteCount )
{
    byteCount = (byteCount - 1) & 3;
    // 这个协议比较奇怪，0 表示 1，3表示4，以此类推
    u32 cmd[2] = { 0xD9000000 | (byteCount << 22) | (address & 0x00ffffff), data };
    xrpgSendCommand( cmd, 4, 0, NULL );
}

void xrpgWriteSram512( u32 address, void * buffer )
{
    u32 * pbuffer = (u32 *)buffer;
    for( u8 i = 0; i < 512 / 4; ++i )
    {
        xrpgWriteSram( address, *(pbuffer+i), 4 );
        address += 4;
    }
}

void xrpgReadNand512( u32 address, void * buffer )
{
    //xrpgSetDeviceStatus( 0x01, 0x00, 0x00, 0x00 );
    //u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) };
    //xrpgSendCommand( cmd, 512, 0x200, buffer );

    xrpgPageCopyNandToSram( address, 0 );
    xrpgReadSram512( 0, buffer );
}

void xrpgReadNand528( u32 address, void * buffer )
{
    //xrpgSetDeviceStatus( 0x01, 0x00, 0x00, 0x00 );
    //u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) };
    //xrpgSendCommand( cmd, 1024, 0x200, buffer );

    xrpgPageCopyNandToSram( address, 0 );
    xrpgReadSram528( 0, buffer );
}

void xrpgReadNand2K( u32 address, void * buffer )
{
    u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) };
    xrpgSendCommand( cmd, 2048, 0x200, buffer );
}


void xrpgPageCopySramToNand( u32 src, u32 dest )
{
    u32 cmd[2] = { 0xDA000000 | (src & 0x00ffffff), dest };
    xrpgSendCommand( cmd, 0, 0, NULL );
    xrpgWaitCmdBusy();
}

void xrpgPageCopyNandToSram( u32 src, u32 dest )
{
    u32 cmd[2] = { 0xDB000000 | (dest & 0x00ffffff), src };
    xrpgSendCommand( cmd, 0, 0, NULL );
    xrpgWaitCmdBusy();
}

void xrpgEraseNand( u32 block )
{
    block <<= 17;
    u32 cmd[2] = { 0xD6030000, block };
    xrpgSendCommand( cmd, 0, 0, NULL );
    xrpgWaitCmdBusy();
}

void xrpgNandPageCopy( u32 src, u32 dest )
{
    u32 cmd[2] = { 0xDE000000 | (src>>11), (dest>>11) };
    rpgSendCommand( cmd, 0, 0, NULL );
    rpgWaitCmdBusy();
}

void xrpgReadSD512( u32 address, void * buffer )
{
    xrpgSetDeviceStatus( 0x03, 0x00, 0x00, 0x00 );
    u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) };
    xrpgSendCommand( cmd, 512, 0xf00, buffer );
}

void xrpgPageCopySramToSD( u32 src, u32 dest )
{
    u32 cmd[2] = { 0xDC000000 | (src & 0x00ffffff), dest };
    xrpgSendCommand( cmd, 0, 0, NULL );
    xrpgWaitCmdBusy();
}

void xrpgPageCopySDToSram( u32 src, u32 dest )
{
    u32 cmd[2] = { 0xDD000000 | (dest & 0x00ffffff), src };
    xrpgSendCommand( cmd, 0, 0, NULL );
    xrpgWaitCmdBusy();
}

void xrpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] )
{
    xrpgSetMapTableAddress( SAVE_TABLE, tableAddress );
    for( size_t i = 0; i < 64; ++i ) {
        //xrpgWriteSram( tableAddress + i * 4, nandAddress[i], 4 );
        xrpgWriteSram( tableAddress + i * 4, ((i + 128) << 17), 4 );
    }
}
