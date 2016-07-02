/*
    sd_read.s
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

@must be 1st function, called by soft reset and repatch9.
	.thumb_func
stopSDTransmission:
	push	{r0, lr}
	bl	waitSDStateFast
	cmp	r0, #5
	bcc	stopSDTransmission_return
	cmp	r0, #9
	bhi	stopSDTransmission_return
	mov	r0, #7
	bl	waitSDState
	bl	stopSD_internal
	mov	r0, #0xa0
	lsl	r0, #2	@ 0x280
stopSDTransmission_wait:
	sub	r0, #1
	bcs	stopSDTransmission_wait
	mov	r0, #0
	bl	waitSDState
stopSDTransmission_return:
	ldr	r0, sdState
	mov	lr, r1
	mov	r1, #0
	str	r1, [r0]
	mov	r1, lr
	pop	{r0, pc}

	.thumb_func
stopSD_internal:
	push	{r1-r2, lr}
	adr	r0, sddStop
	mov	r1, #0x50
	mov	r2, #0
	bl	ioRpgSendCommand
	pop	{r1-r2, pc}

/*
r0 - ptr to command
*/
	.thumb_func
ioRpgSendCommand_init:
	push	{r1-r3}
	ldr	r1, CARD_COMMAND_0
	ldr	r2, [r0]
ioRpgSendCommand_init_fill:
	mov	r3, #24
	ror	r2, r3
	strb	r2, [r1]
	add	r1, #1
	mov	r3, #3
	tst	r1, r3
	bne	ioRpgSendCommand_init_fill
	mov	r3, #0x10
	ldr	r2, [r0, #4]
	tst	r1, r3
	beq	ioRpgSendCommand_init_fill
	pop	{r1-r3}
	bx	lr	@ decrease stack usage

@ r0 - command
@ r1 - 0..9 - latency, 10..12 - size
@ r2 - buffer
	.thumb_func
ioRpgSendCommand:
	push	{r0-r3, lr}
	ldr	r3, CARD_CR1
	mov	r0, #0x80
	strb	r0, [r3, #1]
	ldr	r0, [sp]
	bl	ioRpgSendCommand_init
	lsr	r0, r1, #10
	lsl	r0, #24
	lsl	r1, #22
	lsr	r1, #22
	orr	r1, r0
	ldr	r0, ROMCTRL_Common
	orr	r0, r1
	add	r0, #0x16
	str	r0, [r3, #4]
ioRpgSendCommand_busy:
	mov	r1, #0x80
	lsl	r1, #16	@ 0x800000 / CARD_DATA_READY
	ldr	r0, [r3, #4]
	tst	r0, r1	@ CARD_DATA_READY
	beq	ioRpgSendCommand_skip
	ldr	r1, CARD_DATA_RD
	ldr	r1, [r1]
	cmp	r2, #0
	beq	ioRpgSendCommand_skip
	stmia	r2!, {r1}
ioRpgSendCommand_skip:
	cmp	r0, #0	@ CARD_BUSY
	blt	ioRpgSendCommand_busy
	mov	r0, #0xc0
	strb	r0, [r3, #1]
	pop	{r0-r3, pc}

	.thumb_func
waitSDStateFast:
	push	{r1, r2, r3, lr}
	mov	r3, #0xff
	b	waitSDState_loop
	.thumb_func
waitSDState:
	push	{r1, r2, r3, lr}
	mov	r3, r0
waitSDState_loop:
	adr	r0, sddState
	mov	r1, #0x1c
	lsl	r1, #8
	add	r1, #4
	adr	r2, buffer
	bl	ioRpgSendCommand
	ldr	r1, CARD_ID
	ldr	r0, [r2]
	cmp	r0, r1
	beq	waitSDState_loop
	lsl	r0, #STATUS_SHIFT
	lsr	r0, #28
	cmp	r3, #0xff
	beq	waitSDState_exit
	cmp	r0, r3
	bne	waitSDState_loop
waitSDState_exit:
	pop	{r1, r2, r3, pc}
