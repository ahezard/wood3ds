/*
    sram.s
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

	.thumb_func
ioRpgReadSram:
	push	{r0-r2, lr}
	mov	lr, r1
	mov	r1, #0xb7
	lsl	r1, #24	@ 0xb7000000
	lsr	r2, r0, #8
	orr	r1, r2
	lsl	r2, r0, #24
	adr	r0, buffer
	str	r1, [r0]
	mov	r1, #SRAM_FLAGS
	lsl	r1, #16
	orr	r1, r2
	str	r1, [r0, #4]
	mov	r1, #0xe1
	lsl	r1, #5
	mov	r2, lr
	bl	ioRpgSendCommand
	pop	{r0-r2, pc}
