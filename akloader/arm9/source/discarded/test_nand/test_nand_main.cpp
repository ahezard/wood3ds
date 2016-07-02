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
#include <fat.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

//#include "chips.h"
#include "../rpgprotocol.h"
#include "dbgtool.h"


char * cmds[] = {
"empty",
"empty",
"read nand (528)",
"fill sram",

"copy nand to sram(528)",
"read sram",
"check chip state",
"empty",
"read redund"
};

u32 operationCount = 0;

size_t selectedLine = 0;

u32 nandAddress = 0;

void printcmds()
{
    dbg_printf( "\x1b[0;0H  addr: %08x, op: %d\n", nandAddress, operationCount );
    u32 cmdCount = sizeof(cmds) / sizeof(char*);
    for( size_t i = 0; i < cmdCount; ++i )
    {
        if( i == selectedLine )
            dbg_printf( "->" );
        else
            dbg_printf( "  " );
        dbg_printf( "%s\n", cmds[i] );
    }
}

void printPercent( u32 percent )
{
    dbg_printf( "\x1b[23;0H%d%s", percent, "%" );
}

void doCmd();

//---------------------------------------------------------------------------------
int test_nand_main(void)
{
    ////---------------------------------------------------------------------------------
    //irqInit();
    //irqEnable(IRQ_VBLANK);

    //sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );
    //
    //vramSetMainBanks( VRAM_A_MAIN_SPRITE,
    //                VRAM_B_MAIN_SPRITE,
    //                VRAM_C_SUB_BG,
    //                VRAM_D_MAIN_BG_0x06000000 );

    //videoSetMode( 0 );
    //
    //videoSetModeSub( 0 );//MODE_5_2D | DISPLAY_BG2_ACTIVE );
    //lcdMainOnBottom();
    //
    //BG0_CR = BG_MAP_BASE(6);
    //BG2_CR = 0x4084 | BG_BMP_BASE(2);
    //for( int i = 0; i < 256 * 192; ++i )
    //{
    //    *((u16 *)BG_BMP_RAM(2) + i) = RGB15(0,15,15) | (1 << 15);
    //}

    //BG_PALETTE[255] = RGB15(31,31,31);    //by default font will be rendered with color 255

    ////consoleInit() is a lot more flexible but this gets you up and running quick
    //consoleInitDefault((u16*)SCREEN_BASE_BLOCK(6), (u16*)CHAR_BASE_BLOCK(0), 16);
    ////
    ////bool succ = fatInitDefault();
    ////dbg_printf( "FAT %s\n", succ ? "OK" : "fail" );

    //videoSetMode( MODE_5_2D
    //                //| DISPLAY_SPR_ACTIVE
    //                | DISPLAY_BG0_ACTIVE
    //                //| DISPLAY_BG1_ACTIVE
    //                | DISPLAY_BG2_ACTIVE
    //                //| DISPLAY_BG3_ACTIVE
    //                );
    //////dbg_printf("\x1b[10;0HTouch x = %04X, %04X\n", touchXY.x, touchXY.px);

    keysSetRepeat( 20, 1 );
    size_t cmdCount = sizeof(cmds) / sizeof(char*);
    while(1)
    {
        scanKeys();
        u32 keysdown = keysDown();
        u32 keysheld = keysHeld();
        u32 keysdownRepeat = keysDownRepeat();
        if( keysdown & KEY_DOWN || keysdownRepeat & KEY_DOWN ) {
            if( selectedLine == cmdCount - 1 )
                selectedLine = 0;
            else
                selectedLine++;
        }

        if( keysdown & KEY_UP || keysdownRepeat & KEY_UP ) {
            if( selectedLine == 0 )
                selectedLine = cmdCount - 1;
            else
                selectedLine--;
        }

        if( keysdown & KEY_LEFT || keysdownRepeat & KEY_LEFT ) {
            if( keysheld & KEY_SELECT )
                nandAddress -= 0x20000;
            else if( keysheld & KEY_START )
                nandAddress -= 0x20000 * 64;
            else
                nandAddress -= 512;
        }
        if( keysdown & KEY_RIGHT || keysdownRepeat & KEY_RIGHT ) {
            if( keysheld & KEY_SELECT )
                nandAddress += 0x20000;
            else if( keysheld & KEY_START )
                nandAddress += 0x20000 * 64;
            else
                nandAddress += 512;
        }
        if( keysdown & KEY_A ) {
            doCmd();
        }
        if( keysdownRepeat & KEY_B ) {
            doCmd();
        }
        if( keysdown & KEY_L ) {
            consoleClear();
        }

        printcmds();

        swiWaitForVBlank();
    }

    return 0;
}



void fillSram()
{
    FILE * f = fopen( "random_data.bin", "rb" );
    if( NULL == f ) { // try to create a new savefile and try to allocate disk space
        dbg_printf("open file random_data.bin fail\n");
        return;
    }
    const u32 blockSize = 512;
    u8 buffer[blockSize];
    u32 readCount = 0;

    for( u32 i = 0; i < 1024*1024*2; i += blockSize )
    {
        u32 readed = fread( buffer, 1, blockSize, f );
        readCount += readed;
        if( blockSize != readed ) {
            break;
        }
        rpgWriteSram512( i, buffer );
        u32 percent = readCount * 100 / (1024 * 1024 * 2);
        printPercent( percent );
    }

    fclose( f );
    dbg_printf("fill sram ok");
}

void doCmd()
{
    ++operationCount;
    rpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );
    //if( 0 == selectedLine )
    //{//erase nand
    //    rpgEraseNand( nandAddress >> 17 );
    //}
    //if( 1 == selectedLine )
    //{//copy sram to nand
    //    rpgPageCopySramToNand( 0, nandAddress );
    //}
    if( 2 == selectedLine )
    {//read nand
        ALIGN(4) u8 buffer[512];
        rpgReadNand512( nandAddress, buffer );
        for( u8 i = 0; i < 128; ++i )
            dbg_printf("%02x", buffer[i] );
        dbg_printf("\n");
    }
    if( 3 == selectedLine )
    {//fill sram
        fillSram();
    }
    if( 4 == selectedLine )
    { //copy nand to sram(528)
        rpgPageCopyNandToSram( nandAddress, 0 );
    }
    if( 5 == selectedLine )
    { // read sram
        ALIGN(4) u8 buffer[512];
        rpgReadSram512( 0, buffer );
        for( u8 i = 0; i < 128; ++i )
            dbg_printf("%02x", buffer[i] );
        dbg_printf("\n");
    }
    if( 6 == selectedLine )
    { // check chip state
        u32 cmd[2] = { 0xB8000000, 0x00000000 };
        u32 data = 0;
        rpgSendCommand(    cmd, 4, 0, &data );
        dbg_printf("%08x", data );
    }

    //if( 7 == selectedLine )
    //{ // erase all nand
    //    for( u32 i = 0; i < 8192; ++i )
    //    {
    //        rpgEraseNand( i << 17 );
    //        printPercent( i * 100 / 8192 );
    //    }
    //}

    if( 8 == selectedLine )
    { // read redund
        //ALIGN(4) u8 buffer[1024];
        //rpgReadNand528( nandAddress, buffer );

        //for( u32 i = 0; i < 16; ++i )
        //{
        //    dbg_printf("%02x", buffer[512 + i]);
        //}
        ALIGN(4) u8 redundData[16];            // redundant 数据的大小是16，取前12，最后4个字节是ff
        memset( redundData, 0, 16 );
        rpgReadNandRedundant( nandAddress, redundData );
        for( u32 i = 0; i < 16; ++i )
        {
            dbg_printf("%02x", redundData[i]);
        }
    }

}
