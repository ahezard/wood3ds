/*
    bootloader.s
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

/*
r0 - ptr to command
dont' call from arm
*/
	.thumb_func
ioRpgSendCommand_init:
	push	{r0-r3, lr}
	ldr	r1, CARD_COMMAND_0
	ldr	r2, [r0]
ioRpgSendCommand_init_fill:
	lsr	r3, r2, #24
	strb	r3, [r1]
	lsr	r3, r2, #16
	strb	r3, [r1, #1]
	lsr	r3, r2, #8
	strb	r3, [r1, #2]
	strb	r2, [r1, #3]
	cmp	r0, #0
	beq	rpgSendCommand_init_end
	ldr	r2, [r0, #4]
	add	r1, #4
	mov	r0, #0
	b	ioRpgSendCommand_init_fill
rpgSendCommand_init_end:
	pop	{r0-r3, pc}

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

/*
r0 - flash
r1 - memory
r2 - size
*/
	.thumb_func
cardReadSector:
	push	{r0-r7, lr}
	sub	sp, #8
	mov	r4, r0
	mov	r5, r1
	mov	r1, #0xb6
	lsl	r1, #24
	lsr	r2, r4, #8
	orr	r1, r2
	lsl	r2, r4, #24
	str	r1, [sp]
	str	r2, [sp, #4]
	mov	r0, sp
	bl	cardWaitReady
	mov	r1, #0xbf
	lsl	r1, #24	@ 0xbf000000
	lsr	r2, r4, #8
	orr	r1, r1, r2
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
	pop	{r0-r7, pc}

	.thumb_func
ReadBootloader:
	adr	r0, SetMap
	bl	cardWaitReady
	mov	r0, #0
	ldr	r7, Header
	mov	r1, r7
	bl	cardReadSector
	@ arm9
	ldr	r0, [r7, #0x20]	@ source
	ldr	r1, [r7, #0x28]	@ targe
	ldr	r2, [r7, #0x2c]	@ size
	mov	r3, #0x80
	lsl	r3, #2	@ #0x200
ReadBootloader_arm9_loop:
	bl	cardReadSector
	add	r0, r3
	add	r1, r3
	sub	r2, r3
	bgt	ReadBootloader_arm9_loop
	@ arm7
	ldr	r0, [r7, #0x30]	@ source
	ldr	r1, [r7, #0x38]	@ targe
	ldr	r2, [r7, #0x3c]	@ size
ReadBootloader_arm7_loop:
	bl	cardReadSector
	add	r0, r3
	add	r1, r3
	sub	r2, r3
	bgt	ReadBootloader_arm7_loop
	ldr	r0, arm9LockAddr2
	ldr	r1, [r7, #0x24]
	str	r1, [r0]
	add	r0, #0x10
	ldr	r1, [r7, #0x34]
	str	r1, [r0]
	mov	r0, #0
ReadBootloader_clear:
	str	r0, [r7]
	stmia	r7!, {r0}
	sub	r3, #4
	bgt	ReadBootloader_clear
	swi	0
