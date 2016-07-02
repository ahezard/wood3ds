/*
    card_dma.s
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
setupTransfer:
	push	{lr}
	lsr	r0, #9
@ lookup sector start
setupTransfer_lookupSector:
	mov	r2, r0
	ldr	r0, clusterShift
	mov	r1, r2
	lsr	r1, r0
	ldr	r0, lastLogicCluster
	cmp	r1, r0
	beq	setupTransfer_lookupSector_return
	adr	r0, lastLogicCluster
	str	r1, [r0]
	bl	stopSDTransmission
	lsl	r0, r1, #2
	mov	r1, #FAT1_START
	lsl	r1, #16
	add	r0, r1
	adr	r1, lastPhysicSector
	bl	ioRpgReadSram
	ldr	r0, lastPhysicSector
	ldr	r1, sdhc
	lsr	r0, r1
	adr	r1, lastPhysicSector
	str	r0, [r1]
setupTransfer_lookupSector_return:
	ldr	r1, clusterMask
	and	r2, r1
	ldr	r1, lastPhysicSector
	add	r0, r2, r1
@ lookup sector finish
	adr	r2, lastReadedPhysicSector
	ldr	r1, [r2]
	str	r0, [r2]
	ldr	r2, lastCardSector
	cmp	r0, r2
	bne	setupTransfer_check
	mov	r1, #0
	b	setupTransfer_read
setupTransfer_check:
	ldr	r2, sdState
	ldr	r2, [r2]
	cmp	r2, #0
	beq	setupTransfer_new
	add	r1, #1
	cmp	r0, r1
	beq	setupTransfer_continue
setupTransfer_new:
	mov	r1, #1
setupTransfer_read:
@ read start
	ldr	r2, sdhc
	lsl	r0, r2
	bl	stopSDTransmission
	bl	getCardId
	cmp	r1, #0
	bne	setupTransfer_read_multi
	ldr	r1, sddSingleRead
	b	setupTransfer_read_skip
setupTransfer_read_multi:
	@ multi read, r1=1
	ldr	r2, sdState
	str	r1, [r2]
	ldr	r1, sddMultiRead
setupTransfer_read_skip:
	mov	r2, r0
	adr	r0, buffer
	str	r1, [r0]
	str	r2, [r0, #4]
	mov	r1, #0x50
	mov	r2, #0
	bl	ioRpgSendCommand
@ read finish
setupTransfer_continue:
	mov	r0, #7
	bl	waitSDState
	adr	r0, sddGetData
	bl	ioRpgSendCommand_init
	pop	{pc}

	.thumb_func
getCardId:
	push	{r0-r2, lr}
	adr	r0, buffer
	mov	r2, #0xb8
	strb	r2, [r0, #3]
	mov	r1, #0x1c
	lsl	r1, #8
	mov	r2, r0
	bl	ioRpgSendCommand
	ldr	r0, [r2]
	adr	r1, cardId
	str	r0, [r1]
	pop	{r0-r2, pc}
