/*
    card_simple.s
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
setupSectors:
	push	{r0-r3, lr}
	mov	r2, r0
	ldr	r3, clusterShift
	lsr	r0, r3

	bl	stopSDTransmission

	lsl	r0, #2
	mov	r1, #2
	lsl	r1, #16	@ 0x020000
	add	r0, r1
	mov	r1, sp
	bl	ioRpgReadSram

	mov	r1, #0x20
	sub	r1, r3

	ldr	r0, [sp]

	lsl	r2, r1
	lsr	r2, r1
	ldr	r3, sdhc
	cmp	r3, #1
	bne	setupSectors_return
	lsr	r2, #9
setupSectors_return:
	add	r0, r0, r2
	str	r0, [sp]
	pop	{r0-r3, pc}

	.thumb_func
setupTransfer:
	push	{r0-r2, lr}
	bl	setupSectors
	bl	stopSDTransmission
	bl	sddReadSingleBlock
	mov	r1, #0xb7
	lsl	r1, #24	@ 0xb7000000
	lsr	r2, r0, #8
	orr	r1, r1, r2
	lsl	r2, r0, #24
	push	{r1, r2}
	mov	r0, sp
	bl	ioRpgSendCommand_init
	add	sp, #8
	pop	{r0-r2, pc}

	.thumb_func
sddReadSingleBlock:
	push	{r0-r3, lr}
	sub	sp, #8
	bl	stopSDTransmission
	ldr	r1, sddRead
	str	r1, [sp]
	str	r0, [sp, #4]
	mov	r0, sp
	mov	r1, #0
	mov	r2, #0x50
	mov	r3, #0
	bl	ioRpgSendCommand
	mov	r0, #7
	bl	waitSDState
	add	sp, #8
	pop	{r0-r3, pc}
