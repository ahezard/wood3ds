/*
    testcases.cpp
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

#include <nds.h>
#include <unistd.h>
#include <elm.h>
#include <sys/iosupport.h>
#if defined(_STORAGE_rpg)
#include <iorpg.h>
#endif
#include <string.h>

#include "dbgtool.h"
#include "testcases.h"
#include "inifile.h"
#include "unicode.h"
#include "savemngr.h"

#ifdef DEBUG

void testIni()
{
    CIniFile ini;
    ini.LoadIniFile( "fat0:/moonshl/moonshl.ini" );
    int bright = ini.GetInt( "System", "NDSLiteDefaultBrightness", 0 );
    int WhenStandby= ini.GetInt( "BacklightTimeout", "WhenStandby", 0 );
    int WhenPicture = ini.GetInt( "BacklightTimeout", "WhenPicture", 0 );
    int WhenText= ini.GetInt( "BacklightTimeout", "WhenText", 0 );
    int WhenSound= ini.GetInt( "BacklightTimeout", "WhenSound", 0 );
    std::string FormatDate = ini.GetString( "ClockPlugin", "FormatDate", "" );

    dbg_printf("bright %d\nWhenStandby %d\nWhenPicture %d\nWhenText %d\nWhenSound %d\nFormatDate %s\n",
        bright,
        WhenStandby,
        WhenPicture,
        WhenText,
        WhenSound,
        FormatDate.c_str() ) ;
}

void testSpinBox()
{
    //_testSpinBox = new cSpinBox( 0, 0, 64, 18, this, "" );
    //_testSpinBox->loadAppearance( "", "" );
    //_testSpinBox->setSize( cSize(64, 18) );
    //_testSpinBox->setTextColor( RGB15(31,31,31) );
    //_testSpinBox->setRelativePosition( cPoint(100,0) );
    //_testSpinBox->insertItem( "parm1", 0 );
    //_testSpinBox->insertItem( "parm2", 1 );
    //_testSpinBox->insertItem( "parm3", 2 );
    //_testSpinBox->insertItem( "parm4", 3 );
    //_testSpinBox->selectItem( 0 );
    //addChildWindow( _testSpinBox );


    //if( NULL == _renderDesc )
    //    _renderDesc = new cBitmapDesc();
}

void testSettingWnd()
{
    //_values.clear();
    //_values.push_back( "YES" );
    //_values.push_back( "NO" );
    //settingWnd.addSettingItem( "active zoom file icon", _values, 4 );

    //_values.clear();
    //_values.push_back( "YES" );
    //_values.push_back( "NO" );
    //_values.push_back( "$1000000" );
    //settingWnd.addSettingItem( "gimme all your money?", _values, 2 );
}

void testUnicode()
{
    /*
    u16 uniString[2];
    uniString[0] = 0xfb30;
    uniString[1] = 0;
    std::string localString = unicode_to_local_string( uniString, 1, NULL );

    dbg_printf( "%s", localString.c_str() );
    */
}

void testCopy()
{
#define CONTINUOUS_COPY_SIZE 16384
#if defined(_STORAGE_rpg)
    ioRpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );
#endif
    static ALIGN(4) u8 copyBuffer[CONTINUOUS_COPY_SIZE];

    struct stat srcSt;
    if( 0 != stat( "fat0:/1Mdummy.nds", &srcSt ) ) {
        printf( "copy file error\n" );
        return;
    }
    FILE * rf = fopen( "fat0:/1Mdummy.nds", "rb" );
    FILE * wf = fopen( "fat0:/__rpg/1Mdummy.nds", "wb" );

    u32 writeCount = srcSt.st_size / CONTINUOUS_COPY_SIZE;
    if( srcSt.st_size & (CONTINUOUS_COPY_SIZE - 1) )
        writeCount++;

    u8 percent = 0;
    u32 total = 0;
    for( size_t i = 0; i < writeCount; ++i ) {
        if( !(percent & 3) ) {
            printf( "%d%s", percent, "%" );
        }

        u32 readed = fread( copyBuffer, 1, CONTINUOUS_COPY_SIZE, rf );
        u32 written = fwrite( copyBuffer, 1, (int)readed, wf );
        if( written != readed ) {
            //printf("err %d\n", errno );
            printf("COPY FILE ERROR! %d/%d\n", readed, written );
            // judge error types in errno
            fclose( rf );
            fclose( wf );
            return;
        }
        total += written;
        percent = total * 100 / srcSt.st_size;
    }
    fclose( rf );
    fclose( wf );
    return;
}

void testWrite()
{
#define CONTINUOUS_COPY_SIZE 16384
#if defined(_STORAGE_rpg)
    ioRpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );
#endif
    static ALIGN(4) u8 copyBuffer[CONTINUOUS_COPY_SIZE];

    FILE * wf = fopen( "fat0:/__rpg/1mdummy2.bin", "wb" );

    u32 writeCount = 1024 * 1024 / CONTINUOUS_COPY_SIZE;

    u8 percent = 0;
    u32 total = 0;
    for( size_t i = 0; i < writeCount; ++i ) {
        if( !(percent & 3) ) {
            printf( "%d%s", percent, "%" );
        }
        for( size_t i = 0; i < CONTINUOUS_COPY_SIZE; ++i ) {
            copyBuffer[i] = rand() & 0xff;
        }

        u32 written = fwrite( copyBuffer, 1, (int)CONTINUOUS_COPY_SIZE, wf );
        if( written != CONTINUOUS_COPY_SIZE ) {
            //printf("err %d\n", errno );
            printf("WRITE FILE ERROR! %d/%d\n", CONTINUOUS_COPY_SIZE, written );
            // judge error types in errno
            fclose( wf );
            return;
        }
        total += written;
        percent = total * 100 / (1024 * 1024);
    }
    fclose( wf );
    return;
}

void testWriteFile()
{
    FILE * f = fopen( "fat0:/testWriteSpeed.bin", "wb" );
    //FILE * f = fopen( "fat0:/test_nand4.nds", "wb" );
    if( NULL == f )
        return;
#define SAVEDATA_BUFFER_SIZE (1024 * 1024)
//#define SAVEDATA_BUFFER_SIZE (116 * 1024)
    u8 * saveDataBuffer = new u8[SAVEDATA_BUFFER_SIZE];
    //static ALIGN(4) u8 readNandBuffer[2112];
    memset( saveDataBuffer, 0x5a, SAVEDATA_BUFFER_SIZE );
    //u8 * pSaveDataBuffer = saveDataBuffer;

    u32 written = 0;
    for( u32 i = 0; i < 64; ++i ) {
        written = fwrite( saveDataBuffer, 1, SAVEDATA_BUFFER_SIZE, f );
        dbg_printf( "%d ", i );
    }
    dbg_printf("64MB WRITE DONE\n" );

    fclose( f );
}

#include "files.h"
void testViolenceCopy()
{
    while( true ) {
        unlink( "fat0:/800mdummy.rar" );
        if( !copyFile( "fat1:/800mdummy.rar", "fat0:/800mdummy.rar", false ) )
            return;

        FILE * f = fopen("fat0:/800mdummy.rar", "rb");
        u32 readed = 0;
        //PrintFreeMem();
        u8 * buffer = new u8[1048576];
        //PrintFreeMem();
        do {
            readed = fread( buffer, 1, 1048555, f );
            dbg_printf(".");
            scanKeys();
            if( keysHeld() & KEY_R ) {
                delete[] buffer;
                dbg_printf("\n");
                fclose(f);
                return;
            }
        } while( readed > 0 );
        delete[] buffer;
        dbg_printf("\n");

        fclose(f);
    }
}

void testSDIdentify()
{
    //u16 sdspeed = sdidCheckSDSpeed( 4096 );
    //dbg_printf("sd speed %04x\n\n", sdspeed );
}

void testTextOut()
{
    //char msg[32] = {};
    //for( u8 i = 1;i < 16; ++i )
    //{
    //    msg[i-1] = i;
    //}
    //msg[16] = 0xfd;msg[17] = 0xfe;msg[18] = 0xff;msg[19] = 0x1;
    //u32 id = messageBox( NULL, "\x01\x02\x03\x04 blah blah",
    //    msg, MB_OK | MB_CANCEL );// | MB_YES | MB_NO );
    //dbg_printf( "msgbox return %d\n", id );
}

void testLoadWithOutSave()
{
    //std::string shortPath = _mainList->getSelectedShortPath();

    //if( shortPath[shortPath.size()-1] == '/' ) {
    //    _mainList->enterDir( shortPath );
    //    return;
    //}

    //DSRomInfo rominfo;
    //_mainList->getRomInfo( _mainList->selectedRowId(), rominfo );

    //if( !rominfo.isDSRom() )
    //    return;

    //std::string fullPath = _mainList->getSelectedFullPath();

    //dbg_printf("(%s)\n", fullPath.c_str() );
    //dbg_printf("%d\n", fullPath[fullPath.size()-1] );

    //saveManager().unlockSave( (SAVE_TYPE)rominfo.saveInfo().saveType, false );

    //loadRom( shortPath );
}


#if defined(_STORAGE_rpg)
static void checkRedund( u32 saveBlockTable[64] )
{
    ALIGN(4) u8 cfgPage[528];
    dbg_printf( "checkRedund cfgPage\n");
    ioRpgReadNand( 0x00000000, cfgPage, 528 );

    // read zones count
    u32 totalZones = 1 << (cfgPage[0x0c] - 10);
    //wait_press_b();

    for( size_t i = 0; i < 64; ++i )
        saveBlockTable[i] = 0xFFFFFFFF;

    ALIGN(4) u8 redundData[16];
    u8 ctByte = 0x00;
    u32 saveBlockCountCheck = 0;
    for( size_t i = 0; i < totalZones * 1024; ++i ) {
        ioRpgReadNandRedundant( (i << 17) + 512 * 3, redundData );
        if( redundData[0] != 0xff && redundData[0] != 0x00 )
            dbg_printf("(%02x%02x%02x)", redundData[1],redundData[2],redundData[3] );
        if( redundData[0] >= 0x80 && redundData[0] < 0x80 + 64 ) {
            u8 saveIndex = redundData[3] - 0x80;
            if( saveIndex < 64 ) {
                saveBlockCountCheck++;
                if( saveBlockTable[saveIndex] != 0xFFFFFFFF ) {
                    dbg_printf("checkRedund() FATAL ERROR: Multi save block assign found\n");
                    dbg_printf("saveTable[%d] 1=%08x 2=%08x\n", saveIndex, saveBlockTable[saveIndex], (i << 17) );
                    wait_press_b();
                }
                saveBlockTable[saveIndex] = (i << 17);
                if( redundData[2] >= 0xc0 && redundData[2] <= 0xc4 && 0 == ctByte ) {
                    ctByte = redundData[2];
                }
            } else {
                dbg_printf("idx err, shouldn't have a %d\n", saveIndex );
                dbg_printf("redunt0-4:(%02x%02x%02x%02x%02x)\n", redundData[0], redundData[1],redundData[2],redundData[3], redundData[4] );
                dbg_printf("at block: %d\n", i );
                wait_press_b();
            }
        }
    }

    printf("\n");
    if( saveBlockCountCheck != 0 && saveBlockCountCheck != 32 && saveBlockCountCheck != 64 )
    {
        dbg_printf("checkRedund() FATAL ERROR: saveblock COUNT ERROR %d\n", saveBlockCountCheck );
        dbg_printf("totalZones %d\n", totalZones );
        wait_press_b();
    } else {
        dbg_printf("checkRedund() OK OK OK OK %d\n", saveBlockCountCheck );
    }


}

void testViolenceEEP4Kauto( u32 count )
{
    static ALIGN(4) u8 buffer[512];
    static ALIGN(4) u8 verifyBuffer[512];

    memset( buffer, 0xff, 512 );
    u32 saveBlockTable1[64];
    u32 saveBlockTable2[64];
    u32 round = 0;
    while( count-- )
    {
        saveManager().unlockSave( ST_AUTO, true );

        checkRedund( saveBlockTable1 );
        printf("4K");

        for( size_t count = 0; count < 8; ++count ) {
            for( size_t i = 0; i < 512; ++i ) {
                buffer[i] = rand() & 0xff;
            }
            u8 addr = rand() % 512;
            while( 0 == (addr % 16) ) {
                addr = rand() % 512;
            }
            u8 length = 16 - (addr % 16);
            length &= (~1);
            if( length > 0 ) {
                saveChipWrite( addr, buffer, length, 1 );
                saveChipRead( addr, verifyBuffer, length, 1 );
                if( 0 != memcmp( buffer, verifyBuffer, length ) ) {
                    dbg_printf("save verify error\n");
                    wait_press_b();
                }
            }
            dbg_printf(".");
        }
        dbg_printf("\n");

        checkRedund( saveBlockTable2 );
        printf("4K");

        if( 0 != memcmp( saveBlockTable1, saveBlockTable2, 64 * 4 ) ) {
            dbg_printf("4K save error, table1/2 are different\n");
            for( size_t i = 0; i < 64; ++i ) {
                if( saveBlockTable1[i] != saveBlockTable2[i] ) {
                    dbg_printf("%d:%08x-%08x\n", i, saveBlockTable1[i], saveBlockTable2[i]);
                }
            }
            dbg_printf("round %d\n", round );
            wait_press_b();
        }

////////////////////////
        saveManager().clearSaveBlocks();

        dbg_printf("round %d wait 1 seconds....\n", round );
        swiDelay( 16 * 1000 * 1000 * 1 );

        scanKeys();
        if( keysHeld() & KEY_R ) {
            return;
        }
        ++round;

    }
}

void testViolenceEEP512Kauto( u32 count )
{
    static ALIGN(4) u8 buffer[512];
    static ALIGN(4) u8 verifyBuffer[512];

    memset( buffer, 0xff, 512 );
    u32 saveBlockTable1[64];
    u32 saveBlockTable2[64];
    u32 round = 0;
    while( count-- )
    {
        saveManager().unlockSave( ST_AUTO, true );

        checkRedund( saveBlockTable1 );
        printf("512K");

        for( size_t count = 0; count < 32; ++count ) {
            for( size_t i = 0; i < 512; ++i ) {
                buffer[i] = rand() & 0xff;
            }
            u16 addr = rand() % 65536;
            while( 0 == (addr % 128) ) {
                addr = rand() % 65536;
            }
            u8 length = 128 - (addr % 128);
            length &= (~1);
            if( length > 0 ) {
                saveChipWrite( addr, buffer, length, 2 );
                saveChipRead( addr, verifyBuffer, length, 2 );
                if( 0 != memcmp( buffer, verifyBuffer, length ) ) {
                    dbg_printf("save verify error\n");
                    wait_press_b();
                }
            }
            dbg_printf(".");
        }
        dbg_printf("\n");

        checkRedund( saveBlockTable2 );
        printf("512K");

        if( 0 != memcmp( saveBlockTable1, saveBlockTable2, 64 * 4 ) ) {
            dbg_printf("512K save error, table1/2 are different\n");
            for( size_t i = 0; i < 64; ++i ) {
                if( saveBlockTable1[i] != saveBlockTable2[i] ) {
                    dbg_printf("%d:%08x-%08x\n", i, saveBlockTable1[i], saveBlockTable2[i]);
                }
            }
            dbg_printf("round %d\n", round );
            wait_press_b();
        }

////////////////////////
        saveManager().clearSaveBlocks();

        dbg_printf("round %d wait 1 seconds....\n", round );
        swiDelay( 16 * 1000 * 1000 * 1 );

        scanKeys();
        if( keysHeld() & KEY_R ) {
            return;
        }
        ++round;

    }
}

void testViolenceEEP8Mauto( u32 count )
{
    static ALIGN(4) u8 buffer[512];
    static ALIGN(4) u8 verifyBuffer[512];

    memset( buffer, 0xff, 512 );
    u32 saveBlockTable1[64];
    u32 saveBlockTable2[64];
    u32 round = 0;
    while( count-- )
    {
        saveManager().unlockSave( ST_AUTO, true );

        checkRedund( saveBlockTable1 );
        printf("8M");

        for( size_t count = 0; count < 1; ++count ) {
            for( size_t i = 0; i < 512; ++i ) {
                buffer[i] = rand() & 0xff;
            }
            u32 addr = rand() % 1048576;
            while( 0 == (addr % 256) ) {
                addr = rand() % 1048576;
            }
            u8 length = 256 - (addr % 256);
            length &= (~1);
            if( length > 0 ) {
                saveChipWrite( addr, buffer, length, 3 );
                saveChipRead( addr, verifyBuffer, length, 3 );
                if( 0 != memcmp( buffer, verifyBuffer, length ) ) {
                    dbg_printf("save verify error\n");
                    wait_press_b();
                }
            }
            dbg_printf(".");
        }
        dbg_printf("\n");

        checkRedund( saveBlockTable2 );
        printf("8M");

        if( 0 != memcmp( saveBlockTable1, saveBlockTable2, 64 * 4 ) ) {
            dbg_printf("8M save error, table1/2 are different\n");
            for( size_t i = 0; i < 64; ++i ) {
                if( saveBlockTable1[i] != saveBlockTable2[i] ) {
                    dbg_printf("%d:%08x-%08x\n", i, saveBlockTable1[i], saveBlockTable2[i]);
                }
            }
            dbg_printf("round %d\n", round );
            wait_press_b();
        }

////////////////////////
        saveManager().clearSaveBlocks();

        dbg_printf("round %d wait 1 seconds....\n", round );
        swiDelay( 16 * 1000 * 1000 * 1 );

        scanKeys();
        if( keysHeld() & KEY_R ) {
            return;
        }
        ++round;

    }
}

void testEEPReadStatus()
{
    REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

    REG_AUXSPIDATA = 0x05;
    while (REG_AUXSPICNT & /*BUSY*/0x80);

    REG_AUXSPIDATA = 0x00;
    while (REG_AUXSPICNT & /*BUSY*/0x80);

    u8 d = REG_AUXSPIDATA;

    dbg_printf("cmd 05 returns %02x\n", d );

    REG_AUXSPICNT = /*MODE*/0x40;

}
#endif

void testGetDiskFreeSpace()
{
    u64 dirSize = 0;
    if( getDirSize( "fat0:/", true, &dirSize ) ) {
        dbg_printf("fat0 file takes %d bytes\n", dirSize );
    }
}

#endif//DEBUG
