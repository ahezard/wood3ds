/*
    msgdisplay.cpp
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

#include <stdio.h>
#include <string.h>
#include "msgdisplay.h"
#include "reset.h"
#include "msgs_bin.h"


#define PMSGS(x) ((u8 *)(msgs_bin + x * 768))
#define MSGCOUNT (msgs_bin_size / 768)

static u16 * videoBuffer1;
static u16 * videoBuffer2;

void initMsg()
{
    vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
    vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );
    vramSetBankC( VRAM_C_MAIN_BG_0x06040000 ); //homebrew buffer
    vramSetBankD( VRAM_D_MAIN_BG_0x06060000 ); //homebrew buffer
    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);

    REG_BG2CNT = BG_BMP16_256x256;
    REG_BG2PA = 1 << 8; // 2 =放大倍数
    REG_BG2PD = 1 << 8;  // 2 =放大倍数
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2Y = 0;
    REG_BG2X = 0;

    videoBuffer1 = (u16 *)0x06000000;
    videoBuffer2 = (u16 *)0x06020000;

}

void cleanMsg()
{
    fillMemoryITCM( (void *)videoBuffer2, 0x20000, 0x80008000 );
}

void setMsg( u32 msgID, u16 color )
{
    if( msgID >= MSGCOUNT )
        return;

    u8 * msgBuffer = PMSGS( msgID );
    u32 bufferOffset = 256 * 84;

    for( u32 i = 0; i < 256 * 24 / 8; ++i )    {
        for( u8 j = 0; j < 8; ++j ) {
            videoBuffer2[i*8+j+bufferOffset] = // 0 = black pixel, 1 = white pixel
                ((msgBuffer[i] >> (7-j)) & 1) ? (1 << 15) | color : 0x8000;
        }
    }
}

void showMsg()
{
    u16 * temp = videoBuffer1;
    videoBuffer1 = videoBuffer2;
    videoBuffer2 = temp;
    REG_BG2CNT ^= BG_BMP_BASE( 128 / 16 );
}

void showMsg( u32 msgID, u16 color )
{
    cleanMsg();
    setMsg( msgID, color );
    showMsg();
}

void showMsgandWait( u32 msgID, u16 color )
{
    showMsg( msgID, color);
    for(size_t ii=0;ii<90;++ii) swiWaitForVBlank();
}
