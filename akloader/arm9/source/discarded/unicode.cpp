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









#include "unicode.h"

u16 unicodeL2UTable[0x10000];
//u16 unicodeU2LTable[0x10000];
u8 unicodeAnkTable[0x100];

inline void zeroMemory( void * addr, u32 count )
{
    u32 zero = 0;
    swiFastCopy( (void*)(&zero), addr, (count>>2) | COPY_MODE_WORD | COPY_MODE_FILL);
}

bool initUnicode()
{
    zeroMemory( unicodeL2UTable, 0x10000 );
    //zeroMemory( unicodeU2LTable, 0x10000 );
    zeroMemory( unicodeAnkTable, 0x100 );

    FILE * l2uf = fopen( "/system/unicode.l2u", "rb" );
    if( l2uf ) {
        size_t readed = fread( (u8 *)unicodeL2UTable, 1, 0x200, l2uf );
        size_t count = 0;
        while( readed == 0x200 ) {
            count += readed;
            readed = fread( (u8 *)unicodeL2UTable + count, 1, 0x200, l2uf );
            //dbg_printf("readed %d, count %d\n", readed, count );
        }
        fclose( l2uf );
        l2uf = NULL;
    } else {
        return false;
    }

    iprintf("unicodeL2UTable ok\n");

    //FILE * u2lf = fopen( "/system/unicode.u2l", "rb" );
    //if( u2lf ) {
    //    size_t readed = fread( (u8 *)unicodeU2LTable, 1, 0x200, u2lf );
    //    size_t count = 0;
    //    while( readed == 0x200 ) {
    //        count += readed;
    //        readed = fread( (u8 *)unicodeU2LTable + count, 1, 0x200, u2lf );
    //        //dbg_printf("readed %d, count %d\n", readed, count );
    //    }
    //    fclose( u2lf );
    //    u2lf = NULL;
    //} else {
    //    return false;
    //}

    //iprintf("unicodeU2LTable ok\n");

    FILE * ankf = fopen( "/system/unicode.ank", "rb" );
    if( ankf ) {
        size_t readed = fread( unicodeAnkTable, 1, 0x200, ankf );
        size_t count = 0;
        while( readed == 0x200 ) {
            count += readed;
            readed = fread( unicodeAnkTable + count, 1, 0x200, ankf );
        }
        fclose( ankf );
        ankf = NULL;
    } else {
        return false;
    }
    return true;
}
