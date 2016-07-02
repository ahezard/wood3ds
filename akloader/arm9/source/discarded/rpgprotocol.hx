#ifndef _RPGPROTOCOL_H_
#define _RPGPROTOCOL_H_

#include <nds.h>

enum MAP_TABLE_NAME
{
	MTN_A			= 0,
	MTN_B			= 1,
	NAND_OFFSET1	= 2,
	SAVE_TABLE		= 3
};

enum DEVICE_NAME
{
	DN_DEFAULT	= 0,
	DN_NAND		= 1,
	DN_SRAM		= 2,
	DN_SD		= 3
};

void rpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer );

void rpgWaitCmdBusy();

void rpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize );

void rpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress );

u32 rpgReadSram4( u32 address );

void rpgReadSram512( u32 address, void * buffer );

void rpgReadSram528( u32 address, void * buffer );

void rpgWriteSram( u32 address, u32 data, u8 byteCount );

void rpgWriteSram512( u32 address, void * buffer );

void rpgReadNand512( u32 address, void * buffer );

void rpgReadNand528( u32 address, void * buffer );

void rpgReadNand2KRedundant( u32 address, void * buffer );

void rpgPageCopySramToNand( u32 src, u32 dest );

void rpgPageCopyNandToSram( u32 src, u32 dest );

void rpgEraseNand( u32 blockAddr );

void rpgNandPageCopy( u32 src, u32 dest );

void rpgReadSD512( u32 address, void * buffer );

void rpgPageCopySramToSD( u32 src, u32 dest );

void rpgPageCopySDToSram( u32 src, u32 dest );

void rpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] );

#endif//_RPGPROTOCOL_H_
