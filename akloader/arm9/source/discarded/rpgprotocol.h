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
    MTN_MAP_A            = 0,
    MTN_MAP_B            = 1,
    MTN_NAND_OFFSET1    = 2,
    MTN_SAVE_TABLE        = 3
};

enum DEVICE_NAME
{
    DN_DEFAULT    = 0,
    DN_NAND        = 1,
    DN_SRAM        = 2,
    DN_SD        = 3
};

enum SRAM_ALLOC {
    SRAM_NANDLUT_START = 0x000000,
    SRAM_SAVETABLE_START = 0x010000,
    SRAM_FAT_START = 0x020000,
    SRAM_LOADING_FILENAME = 0x0A0000,
    SRAM_DISKBUFFER_START = 0x0FF000,
    SRAM_SAVEDATA_START = 0x100000,
};

void rpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer );

bool rpgWaitCmdBusy( bool forceWait );

void rpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize );

void rpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress );

void rpgReadSram( u32 address, void * buffer, u32 length );

void rpgWriteSram( u32 address, const void * data, u32 length );

void rpgReadNand( u32 address, void * buffer, u32 length );

void rpgReadNandRedundant( u32 address, void * buffer );

void rpgWriteNand( u32 address, const void * data, u32 length );

void rpgPageCopySramToNand( u32 src, u32 dest );

void rpgPageCopyNandToSram( u32 src, u32 dest );

void rpgEraseNand( u32 blockAddr );

void rpgPageCopyNandToNand( u32 src, u32 dest );

void rpgPageCopyNandInternal( u32 src, u32 dest );

void rpgReadSD512( u32 address, void * buffer );

void rpgWriteSD512( u32 address, const void * data );

void rpgPageCopySramToSD( u32 src, u32 dest );

void rpgPageCopySDToSram( u32 src, u32 dest );

void rpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] );


#endif//_RPGPROTOCOL_H_
