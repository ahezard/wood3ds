/*
    dlditool.cpp
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

#include <string.h>
#include "dlditool.h"
#include <fcntl.h>
#include <unistd.h>


#define FIX_ALL    0x01
#define FIX_GLUE    0x02
#define FIX_GOT    0x04
#define FIX_BSS    0x08

static const data_t dldiMagicString[] = "\xED\xA5\x8D\xBF Chishm";

enum DldiOffsets {
    DO_magicString = 0x00,            // "\xED\xA5\x8D\xBF Chishm"
    DO_magicToken = 0x00,            // 0xBF8DA5ED
    DO_magicShortString = 0x04,        // " Chishm"
    DO_version = 0x0C,
    DO_driverSize = 0x0D,
    DO_fixSections = 0x0E,
    DO_allocatedSpace = 0x0F,

    DO_friendlyName = 0x10,

    DO_text_start = 0x40,            // Data start
    DO_data_end = 0x44,                // Data end
    DO_glue_start = 0x48,            // Interworking glue start    -- Needs address fixing
    DO_glue_end = 0x4C,                // Interworking glue end
    DO_got_start = 0x50,            // GOT start                    -- Needs address fixing
    DO_got_end = 0x54,                // GOT end
    DO_bss_start = 0x58,            // bss start                    -- Needs setting to zero
    DO_bss_end = 0x5C,                // bss end

    // IO_INTERFACE data
    DO_ioType = 0x60,
    DO_features = 0x64,
    DO_startup = 0x68,
    DO_isInserted = 0x6C,
    DO_readSectors = 0x70,
    DO_writeSectors = 0x74,
    DO_clearStatus = 0x78,
    DO_shutdown = 0x7C,
    DO_code = 0x80
};

static addr_t readAddr( data_t * mem, addr_t offset )
{
    return (addr_t)(
        (mem[offset + 0] << 0) |
        (mem[offset + 1] << 8) |
        (mem[offset + 2] << 16) |
        (mem[offset + 3] << 24)
        );
}

static void writeAddr( data_t * mem, addr_t offset, addr_t value )
{
    mem[offset + 0] = (data_t)(value >> 0);
    mem[offset + 1] = (data_t)(value >> 8);
    mem[offset + 2] = (data_t)(value >> 16);
    mem[offset + 3] = (data_t)(value >> 24);
}

static addr_t quickFind( const void * data, const void * search, size_t dataLen, size_t searchLen ) {
    const int * dataChunk = (const int *) data;
    const int searchChunk = ((const int *)search)[0];
    addr_t dataChunkEnd = (addr_t)(dataLen / sizeof(int));

    for( addr_t i = 0; i < dataChunkEnd; ++i ) {
        if( searchChunk == dataChunk[i] ) {
            if( (i * sizeof(int) + searchLen) > dataLen ) {
                return -1;
            }
            if( memcmp( &dataChunk[i], search, searchLen ) == 0) {
                return i * sizeof(int);
            }
        }
    }

    return -1;
}

bool dldiPatch( const char * dldiFilename, void * appDataMem, size_t appDataLength )
{
    int patchPosition = quickFind( appDataMem, dldiMagicString, appDataLength, sizeof(dldiMagicString)/sizeof(data_t) );
    if( -1 == patchPosition ) {
        return false;
    }

    if( NULL == dldiFilename ) {
        return false;
    }

    int fdldi=open(dldiFilename,O_RDONLY);
    if(fdldi<0) return false;

    off_t dldiMemLength=lseek(fdldi,0,SEEK_END);

    //void * dldiMem = malloc(dldiMemLength);
    static char dldiMem[8*1024];
    lseek(fdldi,0,SEEK_SET);
    ssize_t readed=read(fdldi,dldiMem,dldiMemLength);
    close(fdldi);

    if(readed!=dldiMemLength)
    {
      //free( dldiMem );
      return false;
    }

    data_t * pAH = ((data_t *)appDataMem) + patchPosition;
    data_t * pDH = (data_t *)dldiMem;

    addr_t appMemOffset = readAddr( pAH, DO_text_start );
    if( appMemOffset == 0 ) {
        appMemOffset = readAddr( pAH, DO_startup ) - DO_code;
    }
    addr_t ddmemOffset = readAddr( pDH, DO_text_start );
    addr_t relocationOffset = appMemOffset - ddmemOffset;

    // Copy the DLDI patch into the application
    pDH[DO_allocatedSpace] = pAH[DO_allocatedSpace];
    memset( pAH, 0, (1 << pAH[DO_allocatedSpace]) );
    memcpy( pAH, pDH, dldiMemLength );

    // Fix the section pointers in the header
    writeAddr( pAH, DO_text_start, readAddr( pAH, DO_text_start) + relocationOffset);
    writeAddr( pAH, DO_data_end, readAddr( pAH, DO_data_end) + relocationOffset);
    writeAddr( pAH, DO_glue_start, readAddr( pAH, DO_glue_start) + relocationOffset);
    writeAddr( pAH, DO_glue_end, readAddr( pAH, DO_glue_end) + relocationOffset);
    writeAddr( pAH, DO_got_start, readAddr( pAH, DO_got_start) + relocationOffset);
    writeAddr( pAH, DO_got_end, readAddr( pAH, DO_got_end) + relocationOffset);
    writeAddr( pAH, DO_bss_start, readAddr( pAH, DO_bss_start) + relocationOffset);
    writeAddr( pAH, DO_bss_end, readAddr( pAH, DO_bss_end) + relocationOffset);
    // Fix the function pointers in the header
    writeAddr( pAH, DO_startup, readAddr( pAH, DO_startup) + relocationOffset);
    writeAddr( pAH, DO_isInserted, readAddr( pAH, DO_isInserted) + relocationOffset);
    writeAddr( pAH, DO_readSectors, readAddr( pAH, DO_readSectors) + relocationOffset);
    writeAddr( pAH, DO_writeSectors, readAddr( pAH, DO_writeSectors) + relocationOffset);
    writeAddr( pAH, DO_clearStatus, readAddr( pAH, DO_clearStatus) + relocationOffset);
    writeAddr( pAH, DO_shutdown, readAddr( pAH, DO_shutdown) + relocationOffset);


    addr_t ddmemStart = readAddr( pDH, DO_text_start );
    addr_t ddmemSize = (1 << pDH[DO_driverSize]);
    addr_t ddmemEnd = ddmemStart + ddmemSize;

    addr_t addrIter = 0;
    if (pDH[DO_fixSections] & FIX_ALL) {
        // Search through and fix pointers within the data section of the file
        for (addrIter = (readAddr(pDH, DO_text_start) - ddmemStart); addrIter < (readAddr(pDH, DO_data_end) - ddmemStart); addrIter++) {
            if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
                writeAddr( pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
            }
        }
    }

    if (pDH[DO_fixSections] & FIX_GLUE) {
        // Search through and fix pointers within the glue section of the file
        for (addrIter = (readAddr(pDH, DO_glue_start) - ddmemStart); addrIter < (readAddr(pDH, DO_glue_end) - ddmemStart); addrIter++) {
            if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
                writeAddr( pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
            }
        }
    }

    if (pDH[DO_fixSections] & FIX_GOT) {
        // Search through and fix pointers within the Global Offset Table section of the file
        for (addrIter = (readAddr(pDH, DO_got_start) - ddmemStart); addrIter < (readAddr(pDH, DO_got_end) - ddmemStart); addrIter++) {
            if ((ddmemStart <= readAddr(pAH, addrIter)) && (readAddr(pAH, addrIter) < ddmemEnd)) {
                writeAddr( pAH, addrIter, readAddr(pAH, addrIter) + relocationOffset);
            }
        }
    }

    if (pDH[DO_fixSections] & FIX_BSS) {
        // Initialise the BSS to 0
        memset( &pAH[readAddr(pDH, DO_bss_start) - ddmemStart] , 0, readAddr(pDH, DO_bss_end) - readAddr(pDH, DO_bss_start) );
    }

    //free( dldiMem );

    //wait_press_b();

    return true;
}
