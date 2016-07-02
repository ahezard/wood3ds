/*
    sd_write.s
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

	.thumb_func
ioRpgPushData:
	push	{r0-r4, lr}
	ldr	r4, CARD_CR1
ioRpgPushData_loop:
	ldmia	r0!, {r2, r3}
	str	r2, [r4, #0x8]
	str	r3, [r4, #0xc]
	ldr	r2, ROMCTRL_Push
	str	r2, [r4, #0x4]
ioRpgPushData_busy:
	ldr	r2, [r4, #0x4]
	cmp	r2, #0
	blt	ioRpgPushData_busy
	sub	r1, r1, #8
	bne	ioRpgPushData_loop
	pop	{r0-r4, pc}

	.thumb_func
sddReadSingleBlock:
sddReadSingleBlock_nand:
	push	{r0-r3, lr}
	sub	sp, #8
	mov	r3, r1
	bl	stopSDTransmission
	str	r0, [sp, #4]
	ldr	r0, sddRead
	str	r0, [sp]
	mov	r0, sp
	mov	r1, #0x50
	mov	r2, #0
	bl	ioRpgSendCommand
	mov	r0, #7
	bl	waitSDState
	adr	r0, sddReadData
	mov	r1, #6
	lsl	r1, #8
	mov	r2, r3
	bl	ioRpgSendCommand
	add	sp, #8
	pop	{r0-r3, pc}

	.thumb_func
sddWriteSingleBlock:
sddWriteSingleBlock_nand:
	push	{r0-r3, lr}
	sub	sp, #8
	mov	r3, r1
	bl	stopSDTransmission
	str	r0, [sp, #4]
	ldr	r0, sddWrite
	str	r0, [sp]
	mov	r0, sp
	mov	r1, #0x80
	mov	r2, #0
	bl	ioRpgSendCommand
	mov	r0, r3
	mov	r1, #0x80
	lsl	r1, #2	@ #0x200
	bl	ioRpgPushData
	mov	r0, #0
	bl	waitSDState
	add	sp, #8
	pop	{r0-r3, pc}
