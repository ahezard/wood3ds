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
#include "program_flash.h"

void set_flash_mode()
{
    //enter flash mode
    u8 cmd[] = { 0, 0, 0, 0, 0, 0, 0, 0xD1 };
    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x00000000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}
}

void set_flash_offset( u32 offset )
{
    offset -= 7;
    u8 b1 = (offset & 0xFF000000) >> 24;
    u8 b2 = (offset & 0x00FF0000) >> 16;
    u8 b3 = (offset & 0x0000FF00) >> 8;
    u8 b4 = (offset & 0x000000FF);
    u8 cmd[] = { 0, 0, 0, b4, b3, b2, b1, 0xD0 };//0xD0, b1, b2, b3, b4, 0x00, 0x00, 0x00 );

    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x00000000 | CARD_27;

    while( CARD_CR2 & CARD_BUSY ) {}
}

void sector_erase( u32 addr )
{
    u8 b1 = (addr & 0x70000) >> 16;
    u8 b2 = (addr & 0x0FF00) >> 8;
    u8 b3 = (addr & 0x000FF);

    ////////
    u8 cmd[] = { 0, 0, 0x80, 0x30, b3, b2, b1, 0xD4 };
    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x01000000 | CARD_27;
    //dbg_printf("erasing sector %08xH\n", addr );
    while( CARD_CR2 & CARD_BUSY )
    {
        while (!(CARD_CR2 & CARD_DATA_READY)) {}
        u32 temp = 0;
        temp = CARD_DATA_RD;
    }
    swiDelay( 1000000 );
}


void write_flash_4byte( u32 addr_4, u32 data )
{
    u8 * pdata = (u8 *)&data;
    for( u32 i = 0; i < 4; ++i )
    {
        u32 addr = addr_4 + i;
        //ready_write();
        u8 b1 = (addr & 0x70000) >> 16;
        u8 b2 = (addr & 0x0FF00) >> 8;
        u8 b3 = (addr & 0x000FF);

        u8 cmd[] = { 0, 0, 0, pdata[i], b3, b2, b1, 0xD4 };
        cardWriteCommand( cmd );
        CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x01000000;// | CARD_27;
        //dbg_printf("A:%06xH, D:%02x\n", addr, pdata[i] );

        while( CARD_CR2 & CARD_BUSY )
        {
            while (!(CARD_CR2 & CARD_DATA_READY)) {}
            u32 temp = 0;
            temp = CARD_DATA_RD;
        }
        //swiDelay( 256 );

        //u8 cmd_dummy[] = { 0, 0, 0, 0, 0, 0, 0, 0xD4 };
        //cardWriteCommand( cmd_dummy );
        //CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00180000 | 0x00000000 | CARD_27;
        //while( CARD_CR2 & CARD_BUSY ) {}

    }
}

