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









#ifndef _AKPROTOCOL_H_
#define _AKPROTOCOL_H_

void AKP_set_direct_sd_mode();

void AKP_set_normal_sd_mode();

void AKP_set_rom_offset( u32 offset, bool flashMode = false );

enum SLOT1_ROM_MODE
{
    SLOT1_ROM_MODE_FLASH = 0,
    SLOT1_ROM_MODE_SD = 1
};

void AKP_set_slot1_rom_mode( SLOT1_ROM_MODE mode );

enum SAVE_TYPE
{
    SAVE_TYPE_EEPROM_4KBIT,
    SAVE_TYPE_EEPROM_64K_BIT,
    SAVE_TYPE_EEPROM_512KBITS,
    SAVE_TYPE_FLASH_2MBITS,
    SAVE_TYPE_FLASH_4MBITS
};

void AKP_set_save_type( SAVE_TYPE );

void AKP_set_FAT_param( bool switchON, u32 clusterSize, u32 clusterZeroAtSector );

bool AKP_build_FAT_map( const char * path );

#endif//_AKPROTOCOL_H_
