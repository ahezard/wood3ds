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
lb2pb:
	lsr	r0, #9
	lsl	r0, #9
	bx	lr

	.thumb_func
sddReadSingleBlock:
	push	{r0-r5, lr}
	sub	sp, #8
	mov	r4, r0
	mov	r5, r1
	bl	stopSDTransmission
	mov	r1, #0xb2
	lsl	r1, #24
	lsr	r2, r4, #8
	orr	r1, r2
	lsl	r2, r4, #24
	str	r1, [sp]
	str	r2, [sp, #4]
	mov	r0, sp
	bl	cardWaitReady
	mov	r1, #0xb3
	lsl	r1, #24
	lsr	r2, r4, #8
	orr	r1, r2
	lsl	r2, r4, #24
	str	r1, [sp]
	str	r2, [sp, #4]
	mov	r0, sp
	mov	r1, #0x80
	lsl	r1, #2	@ #0x200
	mov	r2, #0
	mov	r3, r5
	bl	ioRpgSendCommand
	add	sp, #8
	pop	{r0-r5, pc}

/*
r0 - command
r1 - buffer
*/
	.thumb_func
cardWrite:
	push	{r0-r7, lr}
	mov	r7, r1
	ldr	r6, CARD_CR1
	mov	r4, #0x80
	strb	r4, [r6, #1]
	bl	ioRpgSendCommand_init
	ldr	r5, ROMCTRL_R4_Push
	str	r5, [r6, #4]
	ldr	r1, CARD_DATA_RD
	mov	r4, #0x80
	lsl	r4, #16	@ 0x800000 / CARD_DATA_READY
	mov	r0, r7
	add	r0, #0x80
	add	r0, #0x80
	add	r0, #0x80
	add	r0, #0x80
cardWrite_busy:
	ldr	r3, [r6, #4]
	tst	r3, r4	@ CARD_DATA_READY
	beq	cardWrite_skip
	cmp	r7, r0
	bhs	cardWrite_buffer_end
	ldr	r2, [r7]
cardWrite_buffer_end:
	str	r2, [r1]
	add	r7, #4
cardWrite_skip:
	cmp	r3, #0	@ CARD_BUSY
	blt	cardWrite_busy
	mov	r5, #0xc0
	strb	r5, [r6, #1]
	pop	{r0-r7, pc}

	.thumb_func
sddWriteSingleBlock:
	push	{r0-r5, lr}
	sub	sp, #8
	mov	r4, r0
	mov	r5, r1
	bl	stopSDTransmission
	mov	r1, #0xbd
	lsl	r1, #24
	lsr	r2, r4, #8
	orr	r1, r2
	lsl	r2, r4, #24
	str	r1, [sp]
	str	r2, [sp, #4]
	mov	r0, sp
	mov	r1, r5
	bl	cardWrite
	mov	r1, #0xbe
	lsl	r1, #24
	lsr	r2, r4, #8
	orr	r1, r2
	lsl	r2, r4, #24
	str	r1, [sp]
	str	r2, [sp, #4]
	mov	r0, sp
	bl	cardWaitReady
	add	sp, #8
	pop	{r0-r5, pc}
