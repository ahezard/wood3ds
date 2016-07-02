/*
    sd_read.s
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

@must be 1st function, called by soft reset and repatch9.
	.thumb_func
stopSDTransmission:
	bx	lr

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

	.thumb_func
ioRpgSendCommand:
	push	{r0-r6, lr}
	ldr	r6, CARD_CR1
	mov	r4, #0x80
	strb	r4, [r6, #1]
	bl	ioRpgSendCommand_init
	mov	r4, #0
	mov	r5, #0x80
	lsl	r5, #2	@ 0x200
	cmp	r1, r5
	bne	ioRpgSendCommand_not_200
	mov	r4, #1
	lsl	r4, #24	@ 0x1000000
ioRpgSendCommand_not_200:
	cmp	r1, #4
	bne	ioRpgSendCommand_not_4
	mov	r4, #7
	lsl	r4, #24	@ 0x7000000
ioRpgSendCommand_not_4:	
	ldr	r5, ROMCTRL_Common
	orr	r5, r5, r4
	orr	r5, r5, r2
	@add	r5, r5, #0x16
	str	r5, [r6, #4]
	ldr	r1, CARD_DATA_RD
	mov	r4, #0x80
	lsl	r4, #16	@ 0x800000 / CARD_DATA_READY
ioRpgSendCommand_busy:
	ldr	r0, [r6, #4]
	tst	r0, r4	@ CARD_DATA_READY
	beq	ioRpgSendCommand_skip
	ldr	r2, [r1]
	cmp	r3, #0
	beq	ioRpgSendCommand_skip
	stmia	r3!, {r2}
ioRpgSendCommand_skip:
	cmp	r0, #0	@ CARD_BUSY
	blt	ioRpgSendCommand_busy
	mov	r5, #0xc0
	strb	r5, [r6, #1]
	pop	{r0-r6, pc}

/*
r0 - command
*/
	.thumb_func
cardWaitReady:
	push	{r0-r7, lr}
	mov	r7, #0
	ldr	r6, CARD_CR1
	mov	r4, #0x80
	strb	r4, [r6, #1]
cardWaitReady_mainloop:
	bl	ioRpgSendCommand_init
	ldr	r5, ROMCTRL_R4
	str	r5, [r6, #4]
	ldr	r1, CARD_DATA_RD
	mov	r4, #0x80
	lsl	r4, #16	@ 0x800000 / CARD_DATA_READY
cardWaitReady_busy:
	ldr	r3, [r6, #4]
	tst	r3, r4	@ CARD_DATA_READY
	beq	cardWaitReady_skip
	ldr	r2, [r1]
	cmp	r2, #0
	bne	cardWaitReady_skip
	mov	r7, #1
cardWaitReady_skip:
	cmp	r3, #0	@ CARD_BUSY
	blt	cardWaitReady_busy
	cmp	r7, #0
	beq	cardWaitReady_mainloop
	mov	r5, #0xc0
	strb	r5, [r6, #1]
	pop	{r0-r7, pc}
