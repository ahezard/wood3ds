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









#include <nds.h>
#include <stdio.h>
#include "dbgtool.h"


typedef struct _PATCH_DATA
{
    u32 offset;
    u32 data;
} PATCH_DATA;

typedef struct _PATCH_INFO
{
    u32 magicWords[4];
    PATCH_DATA data[4];
} PATCH_INFO;

PATCH_INFO saveWaitPatch = {
    { 0xE1A00009, 0xE1A0100B, 0xE1A02005, 0xE59F303C },
    {{ 6, 0xE3A01401 }, { 6, 0xE3A01401 }, { 6, 0xE3A01401 }, { 6, 0xE3A01401 }}
};

inline bool compareAndPatchMem( void * mem, void * patch )
{
    if( *(u32 *)mem == *(u32 *)patch &&
        *((u32 *)mem+1) ==*((u32 *)patch+1) &&
        *((u32 *)mem+2) ==*((u32 *)patch+2) &&
        *((u32 *)mem+3) ==*((u32 *)patch+3) )
    {
        dbg_printf( "found save 0x%08x, patching:\n", (u32)mem );
        PATCH_DATA * pData = ((PATCH_INFO *)patch)->data;
        for( u8 i = 0; i < 4; ++i ) {
            if( 0 != pData[i].data ) {
                u32 offset = pData[i].offset;
                dbg_printf("%08x:%08x -> %08x\n", (u32)mem+offset, *((u32 *)mem + offset), pData[i].data );
                *((u32 *)mem + offset) = pData[i].data;
            }
        }
        return true;
    }
    return false;
}


void patchSave( void * arm7Mem, u32 arm7MemSize )
{
    u32 * pArm7Mem = (u32 * )arm7Mem;
    u32 * arm7MemEnd = pArm7Mem + arm7MemSize;
    while( pArm7Mem < arm7MemEnd ) {
        if( compareAndPatchMem( pArm7Mem, &saveWaitPatch ) ) {
            //break;
        }
        ++pArm7Mem;
    }
}

void patchReadRom( void * arm9Mem, u32 arm9MemSize )

{
}


