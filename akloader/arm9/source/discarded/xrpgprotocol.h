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









#ifndef _RPGPROTOCOL_H_
#define _RPGPROTOCOL_H_

#include <nds.h>

enum MAP_TABLE_NAME
{
    MTN_A            = 0,
    MTN_B            = 1,
    NAND_OFFSET1    = 2,
    SAVE_TABLE        = 3
};

enum DEVICE_NAME
{
    DN_DEFAULT    = 0,
    DN_NAND        = 1,
    DN_SRAM        = 2,
    DN_SD        = 3,
};

void xrpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer );

void xrpgWaitCmdBusy();

void xrpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize );

void xrpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress );

u32 xrpgReadSram4( u32 address );

void xrpgReadSram512( u32 address, void * buffer );

void xrpgReadSram528( u32 address, void * buffer );

void xrpgWriteSram( u32 address, u32 data, u8 byteCount );

void xrpgWriteSram512( u32 address, void * buffer );

void xrpgReadNand512( u32 address, void * buffer );

void xrpgReadNand528( u32 address, void * buffer );

void xrpgReadNand2K( u32 address, void * buffer );

void xrpgReadNand2112( u32 address, void * buffer );

void xrpgPageCopySramToNand( u32 src, u32 dest );

void xrpgPageCopyNandToSram( u32 src, u32 dest );

void xrpgEraseNand( u32 block );

void xrpgNandPageCopy( u32 src, u32 dest );

void xrpgReadSD512( u32 address, void * buffer );

void xrpgPageCopySramToSD( u32 src, u32 dest );

void xrpgPageCopySDToSram( u32 src, u32 dest );

void xrpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] );


#endif//_RPGPROTOCOL_H_
