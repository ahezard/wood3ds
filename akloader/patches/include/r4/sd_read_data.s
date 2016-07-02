/*
    sd_read_data.s
    Copyright (C) 2010 yellow wood goblin

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

CARD_COMMAND_0:
	.word	0x40001a8
CARD_CR1:
	.word	0x40001a0
ROMCTRL_Common:
	.word	0xa0586000
@ check in softreset start
CARD_DATA_RD:
	.word	0x4100010
CARD_ID:
	.word	0xfc2
@ check in softreset finish
sddStop:
	.word	0xd501000c
	.word	0
sddState:
	.word 	0xc0000000
	.word 	0
ROMCTRL_R4:
	.word	0xa7586000
