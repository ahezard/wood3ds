/*
    bootloader_data.s
    Copyright (C) 2010-2011 yellow wood goblin

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

CARD_CR1H:
	.word	0x40001a1
rpgSendCommand_value:
	.word	0xa1406200
CARD_DATA_RD:
	.word	0x4100010
rpgReadNand:
	.word	READ_NAND
BootLoader_arm9_target:
	.word	0x2200000
BootLoader_arm7_target:
	.word	ARM7_TARGET
arm9LockAddr2:
	.word	0x27ffe24
BootLoader_arm9_entry:
	.word	0x2204000
