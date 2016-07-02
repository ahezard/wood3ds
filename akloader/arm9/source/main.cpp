/*
    main.cpp
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

//#define NDS_BOOT
//#define DEBUG
#include <string.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbgtool.h"
#if defined(_STORAGE_rpg)
#include <iorpg.h>
#endif
#include <ctype.h>
#include <locale.h>

#include "memcleaner.h"
#include "loader.h"
#include "msgdisplay.h"
#include "rpgmaps.h"
#include "dlditool.h"
#include "patches.h"
#include "encryption.h"
#include "reset.h"

#include "../../share/ipctool.h"
#include "reset.h"
#include "../../share/gamecode.h"

#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
#define _MENU_NAME "/akmenu4.nds"
#elif defined(_STORAGE_r4)
#define _MENU_NAME "/_ds_menu.dat"
#elif defined(_STORAGE_r4idsn)
#define _MENU_NAME "/_dsmenu.dat"
#endif
static void resetToSlot2()
{
    // make arm9 loop code
    *((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address
    *((vu32*)0x027FFE34) = (u32)0x080000c0;

    sysSetCartOwner(BUS_OWNER_ARM7);  // ARM7 has access to GBA cart

    DC_FlushAll();
    DC_InvalidateAll();

    fifoSendValue32(FIFO_USER_01,SYS_BOOT_SLOT2);
    while(true)
    {
      while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY);
      u32 res=REG_IPC_FIFO_RX;
      if(FIFO_PACK_VALUE32(FIFO_USER_01,SYS_ARM7_READY_BOOT)==res) break;
    }

    swiSoftReset();
}

#ifdef __cplusplus
extern "C" {
#endif
void initSystem(void)
{
}
#ifdef __cplusplus
}
#endif

int main(void)
{
    irqInit();
    fifoInit();
    irqEnable( IRQ_VBLANK );

    sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );

    REG_POWERCNT|=POWER_ALL_2D;
    lcdSwap();

    initMsg();
    showMsg( MSGID_LOADING, MSG_COLOR );

    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE );    //sub bg 2 will be used to print text
    vramSetBankH( VRAM_H_SUB_BG );    // 32k
    zeroMemoryITCM(BG_GFX_SUB,32*1024);

#ifdef DEBUG
    // init console
    BG_PALETTE_SUB[255] = RGB15(31,31,31);    //by default font will be rendered with color 255
    REG_BG0CNT_SUB = BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY_0;
    consoleInit(NULL,0,BgType_Text4bpp,BgSize_T_256x256,8,0,false,true);
#endif
    WRAM_CR=3;

    bool succ = (ELM_Mount()&1)?false:true;

    //bool succ = fatInitDefault();
    dbg_printf( "init fat %s\n", succ ? "succ" : "fail" );
    if( !succ ) {
        showMsg( MSGID_FAT_ERROR, MSG_COLOR );
        return 1;
    }

#ifdef NDS_BOOT
    zeroMemoryITCM( (void *)0x2000000, 0x204000 );    // boot nds
#else
    zeroMemoryITCM( (void *)0x2000000, 0x3c0000 );  // normal loader
    zeroMemoryITCM((void*)0x027ff000,0x800); //clear IPC memory
    DC_FlushAll();
#endif//NDS_BOOT

    u32 patchParams[3]={0,0,0};
#ifdef NDS_BOOT
    const char * filename = "fat0:/akmenu4.nds";  // boot nds
#else
    ALIGN(4) char filename[MAX_FILENAME_LENGTH];          // normal loader
    memset( filename, 0, MAX_FILENAME_LENGTH );
#if defined(_STORAGE_rpg)
    ioRpgReadSram( SRAM_LOADING_FILENAME_START, filename, MAX_FILENAME_LENGTH );
    ioRpgReadSram( SRAM_LOADING_FILENAME_START + MAX_FILENAME_LENGTH, &patchParams, sizeof(patchParams) );
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    ALIGN(4) char savename[MAX_FILENAME_LENGTH];
    memset( savename, 0, MAX_FILENAME_LENGTH );
    memcpy(filename,(void*)0x23fda00,MAX_FILENAME_LENGTH);
    memcpy(savename,(void*)(0x23fda00+MAX_FILENAME_LENGTH),MAX_FILENAME_LENGTH);
    memcpy(patchParams,(u32*)0x23fd900,sizeof(patchParams));
#endif
#if defined(_STORAGE_r4) || defined(_STORAGE_r4idsn)
    patchParams[0]&=~(PATCH_DOWNLOAD_PLAY|PATCH_PROTECTION);
    patchParams[0]|=(PATCH_DMA|PATCH_SD_SAVE);
#elif defined(_STORAGE_ak2i)
    patchParams[0]|=PATCH_SD_SAVE;
#endif
#endif//NDS_BOOT

    strlwr( (char *)filename );
    dbg_printf("%s\n", filename );

    if( 0 == strncmp( "slot2:/", filename, 7 ) ){
        resetInterrupts();
        resetARM9Memory();
        resetToSlot2();
    }

    u32 originSpeed = 0;
    u32 changedSpeed = 0;
    u32 sizeMask=0;
    bool hasRSA = false;
    bool loadSucc = loadFile( (char *)filename, originSpeed, changedSpeed, sizeMask, hasRSA );
    if( loadSucc ) {
        dbg_printf("%08x\n", __NDSHeader->cardControl13 );
        if( (__NDSHeader->arm7destination >= 0x037F8000 || 0x23232323 == gamecode(__NDSHeader->gameCode)) ) { // 0x23232323 = "####", homebrew programs
#ifndef NDS_BOOT
            if( NULL == strstr((char *)filename,_MENU_NAME) ) {
                // DLDI patch
#if defined(_STORAGE_rpg)
                static const char * sdPatch = "fat0:/__rpg/rpg_sd.dldi";
                static const char * nandPatch = "fat0:/__rpg/rpg_nand.dldi";
#elif defined(_STORAGE_r4)
                static const char * sdPatch = "fat0:/__rpg/r4_sd.dldi";
                static const char * nandPatch = "fat0:/__rpg/r4_sd.dldi";
#elif defined(_STORAGE_ak2i)
                static const char * sdPatch = "fat0:/__rpg/ak2i_sd.dldi";
                static const char * nandPatch = "fat0:/__rpg/ak2i_sd.dldi";
#elif defined(_STORAGE_r4idsn)
                static const char * sdPatch = "fat0:/__rpg/r4idsn_sd.dldi";
                static const char * nandPatch = "fat0:/__rpg/r4idsn_sd.dldi";
#endif
                const char * patch = NULL;
                if( 0 == memcmp( filename, "fat0:/", 6 ) || 0 == memcmp( filename, "FAT0:/", 6 ) ) {
                    patch = nandPatch;
                } else if( 0 == memcmp( filename, "fat1:/", 6 ) || 0 == memcmp( filename, "FAT1:/", 6 ) ) {
                    patch = sdPatch;
                }
                if( NULL != patch ) {
                    dldiPatch( patch, (data_t *)__NDSHeader->arm9destination, __NDSHeader->arm9binarySize );
                }
                //argv support
                if(NULL!=patch)
                {
                  __system_argv->argvMagic=ARGV_MAGIC;
                  __system_argv->commandLine=(char*)(__NDSHeader->arm9destination+__NDSHeader->arm9binarySize);
                  __system_argv->length=strlen(filename);
                  memcpy(__system_argv->commandLine,filename,3);
                  memcpy(__system_argv->commandLine+3,filename+4,__system_argv->length-3);
                }
                if(patchParams[0]&PATCH_SOFT_RESET) patchHB();
            }
#endif
        } else {// official programs
            Decrypt();
            if( patchParams[0] & PATCH_DOWNLOAD_PLAY ) {
                if( !(hasRSA && __NDSHeader->cardControl13 != originSpeed) ) {
                    patchParams[0] &= ~PATCH_DOWNLOAD_PLAY;
                }
            }

            switch(__NDSHeader->unitCode)
            {
              case 0:
                {
                  patchDS(filename,patchParams,originSpeed,sizeMask);
                }
                break;
              case 2:
                {
                  patchDSi(filename,patchParams,sizeMask);
                }
                break;
            }
            // this must be put in the end of all disk operations
            // build some reading mappings
#if defined(_STORAGE_rpg)
            rpgBuildMaps((char*)filename,((patchParams[0]&PATCH_PROTECTION)?true:false));
#elif defined(_STORAGE_r4)
            r4BuildMaps(filename,savename);
#elif defined(_STORAGE_ak2i)
            ak2iBuildMaps(filename,savename,((patchParams[0]&PATCH_PROTECTION)?true:false));
#elif defined(_STORAGE_r4idsn)
            r4idsnBuildMaps(filename,savename);
#endif
        }

        dbg_printf("%08x\n", __NDSHeader->cardControl13 );
        ELM_Unmount();

        resetInterrupts();
        resetAndLoop();

    }
    else {
        dbg_printf("load (%s)fail\n", filename );
#ifdef NDS_BOOT
        showMsg( MSGID_SYS_FILE_MISSING, MSG_COLOR ); //boot nds
#endif//NDS_BOOT
        return 1;
    }

    return 0;
}
