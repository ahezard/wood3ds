/*
    sdidentify.cpp
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

#include "iocmn.h"
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
#include <elm.h>
#include "timer.h"
#include "sdidentify.h"
#include "irqs.h"
#include "inifile.h"
#include "stringtool.h"
#include "systemfilenames.h"
#include "language.h"
#include "progresswnd.h"

std::string sdidGetSDName()
{
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    for( u8 i = 0; i < 5; ++i ) {
        if( ' ' ==sdinfo[i+1] )
            sdinfo[i+1] = '_';
    }
    return (const char *)(sdinfo + 1);

}

std::string sdidGetSDManufacturerID()
{
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    return formatString( "%02x", sdinfo[0] );
}

std::string sdidGetSDManufacturerName()
{
    CIniFile ini( SFN_SDCARD_LIST );
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    std::string manufacturerID = formatString( "%02x", sdinfo[0] );
    std::string manufacturerName = ini.GetString( "SD Card Manufacturers", manufacturerID, manufacturerID );
    return manufacturerName;
}

u32 sdidCheckSDSpeed( u32 readCount )
{
    CIniFile ini( SFN_SDCARD_LIST );

    std::string name = sdidGetSDName();
    std::string manufacturerID = sdidGetSDManufacturerID();

    u32 ret = ini.GetInt( "SD Card Speed", manufacturerID + name, 0 );

    if( ret != 0 )
        return ret;

    u32 totalSectors;
    if(!ELM_SectorsFromDisk(ioSD(),&totalSectors))
    {
        dbg_printf("no sd card\n");
        return 0;
    }
    dbg_printf("total sectors %d, max addr %08x\n", totalSectors, (totalSectors << 9) - 0x200);

    // some sd card needs to be readed one time for initialization
    sddReadBlocks( 0, 1, NULL );

    irq().vblankStop();
    double maxAccessTime = 0.f;

    std::string tipText = LANG("progress window", "first use sd");
    progressWnd().setTipText( tipText );
    progressWnd().show();
    progressWnd().setPercent( 0 );

    vu64 tick1 = 0;
    vu64 tick2 = 0;
    for( size_t i = 0; i < readCount; ++i ) {
        u32 randAddr = ((rand() % totalSectors)<<(isSDHC()?0:9) ) & (~(0x200-1));

        u32 sdReadSingleBlock[2] = { 0xD5030011, randAddr }; // input read address here
        ioRpgSendCommand( sdReadSingleBlock, 0, 80, NULL );
        timer().initTimer();
        tick1 = timer().getTick();
        ioRpgWaitCmdBusy( true );
        tick2 = timer().getTick();

        u32 readSD[2] = { 0xB7000000, 0x00000000 | 0x00130000 }; // address dont care here
        ioRpgSendCommand( readSD, 512, 4, NULL );

        if( tick2 < tick1 ) {
            tick2 += 65536;
        }
        tick1 = tick2 - tick1;
        double timeCostUs = timer().tickToUs( tick1 );
        if( timeCostUs > maxAccessTime ) {
            maxAccessTime = timeCostUs;
        }

        u32 percent = i * 100 / readCount;
        if( (i & 0x1ff) == 0x1ff ) {
            dbg_printf( "%02d ", percent );
            progressWnd().setPercent( percent );
        }
    }
    progressWnd().hide();
    irq().vblankStart();


    maxAccessTime = ((u32)(maxAccessTime / 15 + 0.5)) * 15;

    ret = ((u32)(maxAccessTime * 1000 / 150));

    dbg_printf("max access time: max %4.2fus\n", maxAccessTime );

    ini.SetInt( "SD Card Speed", manufacturerID + name, ret );
    ini.SaveIniFile( SFN_SDCARD_LIST );

    return ret;
}
#endif
