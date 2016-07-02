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
#include <string.h>
#include <stdio.h>
#include <fat.h>
#include <malloc.h>

#include "akprotocol.h"
#include "program_flash.h"
#include <iorpg.h>

void AKP_set_direct_sd_mode()
{
    u8 cmd[] = { 0, 0, 0, 0, 0, 0, 0, 0xd3 };

    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00000000 | 0x00406000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}

    memset( cmd, 0xff, 8 );// = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00000000 | 0x00406000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}
}

void AKP_set_normal_sd_mode()
{
    u8 cmd[] = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F };

    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00000000 | 0x00406000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}
}

void AKP_set_rom_offset( u32 offset, bool flashMode )
{
    if( flashMode )
        offset -= 7;
    u8 b1 = (offset & 0xFF000000) >> 24;
    u8 b2 = (offset & 0x00FF0000) >> 16;
    u8 b3 = (offset & 0x0000FF00) >> 8;
    u8 b4 = (offset & 0x000000FF);
    u8 cmd[] = { 0, 0, 0, b4, b3, b2, b1, 0xD0 };

    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x00000000 | CARD_27;

    while( CARD_CR2 & CARD_BUSY ) {}
}

void AKP_set_slot1_rom_mode( SLOT1_ROM_MODE mode )
{
    if( SLOT1_ROM_MODE_SD == mode )
    {
        // switch from Flash mode to SD mode
        u8 cmd[] = { 0, 0, 0, 0, 0, 0, 0x80, 0xD1 };
        cardWriteCommand( cmd );
    }
    else if( SLOT1_ROM_MODE_FLASH == mode )
    {
        // switch from SD mode to Flash mode
        u8 cmd[] = { 0, 0, 0, 0, 0, 0, 0x00, 0xD1 };
        cardWriteCommand( cmd );
    }

    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x00000000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}
}

void AKP_set_fat_param( bool switchON, u32 clusterSize, u32 clusterZeroAtSector )
{
    u8 offset = 0;
    while( clusterSize > 1 )
    {
        clusterSize >>= 1;
        ++offset;
    }
    offset -= 8;
    u8 byte2 = (switchON ? 0xC0 : 0x80) | (0x0f&offset);

    const u8 * czs = (u8*)(&clusterZeroAtSector);

    u8 cmd[] = { 0, czs[0], czs[1], czs[2], czs[3], 0x00, byte2, 0xD1 };
    cardWriteCommand( cmd );
    CARD_CR2 = CARD_ACTIVATE | CARD_nRESET | 0x00406000 | 0x00000000 | CARD_27;
    while( CARD_CR2 & CARD_BUSY ) {}
    for( int i = 7; i >= 0; --i ) {
        dbg_printf( "%02x", cmd[i] );
    }
    dbg_printf( "\n" );
}

static void dirFromFullPath( const char * path, char * dir )
{
    char * p = strrchr( path, '/' );
    memcpy( dir, path, p - path + 1 );
    dir[p-path+1]=0;
}

static char * filenameFromFullPath( const char * path )
{
    char * p = strrchr( path, '/' );
    return p+1;
}

// if NULL == fatMap, return needed size of fatMap, else return 0
// if mapSize == 0, do not need a fatmap
static u32 iterateFileCluster( PARTITION * partition, u32 firstCluster, u32 * fatMap )
{
    u32 cluster = firstCluster;
    u32 mapSize = 1;   // a file takes at least 1 cluster
    u32 * pfatMap = fatMap;
    bool needMap = false;

    while( cluster != CLUSTER_EOF )
    {
        u32 clusterStartSector = _FAT_fat_clusterToSector( partition, cluster );
        if( NULL != pfatMap )
            *pfatMap++ = clusterStartSector * 512;
        u32 lastCluster = cluster;
        cluster = _FAT_fat_nextCluster( partition, cluster );
        if( cluster != lastCluster + 1 && cluster != CLUSTER_EOF )
        {
            needMap = true;
            dbg_printf("need FAT map:\n%08x->%08x\n", lastCluster, cluster );
        }
        ++mapSize;
    }
    if( needMap )
        return mapSize;
    else
        return 0;
}

bool AKP_build_FAT_map( const char * path )
{
    char dir[512];
    char * pdir = dir;
    dirFromFullPath( path, dir );
    dbg_printf("dir: %s\n", dir );
    char * filename = filenameFromFullPath( path );
    dbg_printf("file: %s\n", filename );

    /////////////////////////////////////////
    PARTITION* partition = NULL;

    // Get the partition this directory is on
    partition = _FAT_partition_getPartitionFromPath (pdir);

    if (partition == NULL) {
        //r->_errno = ENODEV;
        dbg_printf("ENODEV\n");
        return false;
    }

    // Move the path pointer to the start of the actual path
    if (strchr (pdir, ':') != NULL) {
        pdir = strchr (pdir, ':') + 1;
    }
    if (strchr (pdir, ':') != NULL) {
        //r->_errno = EINVAL;
        dbg_printf("EINVAL\n");
        return false;
    }

    // Set the default device to match this one
    if (!_FAT_partition_setDefaultPartition (partition)) {
        //r->_errno = ENOENT;
        dbg_printf("ENOENT\n");
        return false;
    }

    // Try changing directory
    if (_FAT_directory_chdir (partition, pdir)) {
        // Successful
        dbg_printf("SUCCESS fuck\n");
        //return 1;
    } else {
        // Failed
        //r->_errno = ENOTDIR;
        dbg_printf("ENOTDIR\n");
        return false;
    }

    /////////////////////////////////////////

    DIR_ENTRY entry;
    _FAT_directory_entryFromPath( partition, &entry, filename, NULL );

    u32 fileFirstCluster = _FAT_directory_entryGetCluster( entry.entryData );
    if( CLUSTER_FREE == fileFirstCluster ) {
        dbg_printf("first cluster fail\n");
        return false;
    }
    dbg_printf( "%s startClus %08x\n", filename, fileFirstCluster );

    // compute memory requirement for fat map
    u32 cluster = fileFirstCluster;
    u32 mapSize = iterateFileCluster( partition, cluster, NULL );
    bool needMap = ( 0 != mapSize );

    //dbg_printf( "%s ClusSize %08x\n", filename, fatMap.size() );
    dbg_printf( "%s mapSize %d\n", filename, mapSize );
    dbg_printf( "need FAT map %d\n", needMap );

    u32 * fatMap = NULL;
    //u32 fatMap[2048];

    if( !needMap ) {
        u32 rom_offset = 0x200 * _FAT_fat_clusterToSector( partition, fileFirstCluster );
        AKP_set_slot1_rom_mode( SLOT1_ROM_MODE_SD );
        AKP_set_rom_offset( rom_offset, false );
        return false;
    } else {
        fatMap = (u32 *)malloc(mapSize*sizeof(u32));
        // build fat map
        iterateFileCluster( partition, cluster, fatMap );
    }

    u32 fatMapAddress = 0x40000;
    u32 fatMapSize = 0x10000;
    AKP_set_slot1_rom_mode( SLOT1_ROM_MODE_FLASH );
    AKP_set_rom_offset( 0, true );

    u16 factor = 0;

    for( u32 i = fatMapAddress; i < fatMapAddress + fatMapSize; i += 0x1000 )
    {
        sector_erase( i );
        factor = (i - 0x40000) * 8 / fatMapSize;
        BLEND_Y = factor;
        SUB_BLEND_Y = factor;
    }

    //for( size_t i = 0; i < fatMap.size(); ++i )
    for( size_t i = 0; i < mapSize; ++i )
    {
        write_flash_4byte( fatMapAddress + i * 4, fatMap[i] );
        factor = 8 + i * 8 / mapSize;
        BLEND_Y = factor;
        SUB_BLEND_Y = factor;
    }
    dbg_printf("fat map build done.\n" );
    AKP_set_slot1_rom_mode( SLOT1_ROM_MODE_SD );
    AKP_set_rom_offset( 0, false );

    u32 clusterZeroAtSector = _FAT_fat_clusterToSector( partition, 0 );
    AKP_set_fat_param( true, partition->bytesPerCluster, partition->dataStart );
    dbg_printf("clusterZeroAtSector A %08x\n", clusterZeroAtSector );
    dbg_printf("clusterZeroAtSector B %08x\n", _FAT_fat_clusterToSector( partition, 1752 ) );
    //wait_press_b();

    if( NULL != fatMap )
        free( fatMap );

    return true;
}
