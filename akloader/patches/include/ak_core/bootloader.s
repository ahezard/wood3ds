/*
    bootloader.s
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
@ r1 - 0x200 - size
@ r2 - 0x200 - latency
@ r3 - target memory
@ r4 - position
@ r5 - inner counter
@ r6 - outer counter
ReadBootloader:
	push	{r0, r1}	@ r0, r1 really not stored, but used as storage

	mov	r1, #NAND_FLAGS
	lsl	r1, #16
	str	r1, [sp, #4]

	ldr	r4, rpgReadNand
	ldr	r1, BootLoader_arm9_target
	mov	r0, sp
	mov	r2, #128
	lsl	r2, #2 @ 0x200
	mov	r5, #ARM9_SIZE
	mov	r6, #2
ReadBootloader_loop:
	str	r4, [sp]
	bl	rpgSendCommand
	add	r4, #2
	add	r1, r2
	sub	r5, #1
	bne	ReadBootloader_loop
	ldr	r1, BootLoader_arm7_target
	mov	r5, #6
	sub	r6, #1
	bne	ReadBootloader_loop

	ldr	r0, arm9LockAddr2
	ldr	r1, BootLoader_arm9_entry
	str	r1, [r0]
	add	r0, #0x10
	ldr	r1, BootLoader_arm7_target
	str	r1, [r0]
	swi	0

	.thumb_func
rpgSendCommand:
	push	{r0-r4, lr}

	ldr	r2, CARD_CR1H
	mov	r3, #0x80 @CARD_CR1_ENABLE
	strb	r3, [r2]

	add	r2, #7 @CARD_COMMAND_0
	ldr	r3, [r0]
rpgSendCommand_fill:
	mov	r4, #24
	ror	r3, r4
	strb	r3, [r2]
	add	r2, #1
	mov	r4, #3
	tst	r2, r4
	bne	rpgSendCommand_fill
	mov	r4, #0x10
	ldr	r3, [r0, #4]
	tst	r2, r4
	beq	rpgSendCommand_fill
@ r0 - not used
rpgSendCommand_fill_end:
	sub	r2, r2, #0xc @ CARD_CR2
	ldr	r0, rpgSendCommand_value
	str	r0, [r2]
	ldr	r4, CARD_DATA_RD
rpgSendCommand_busy:
	mov	r0, #128
	lsl	r0, #16 @ CARD_DATA_READY
	ldr	r3, [r2]
	tst	r3, r0	@ CARD_DATA_READY
	beq	rpgSendCommand_skip
	ldr	r0, [r4]
	stmia	r1!, {r0}
rpgSendCommand_skip:
	cmp	r3, #0	@ CARD_BUSY
	blt	rpgSendCommand_busy
	pop	{r0-r4, pc}
