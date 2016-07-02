/*
    dma4.s
    Copyright (C) 2008 somebody
    Copyright (C) 2009 yellow wood goblin

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

	.include	"sd_read.s"
	.include	"card_dma.s"
	.thumb_func
	@ r3 rom type
SetupDma:
	push	{r0-r2, lr}
	mov	r0, #0xc4
	blx	IsCartDmaChannel
	beq	SetupDma_found
	mov	r0, #0xd0
	blx	IsCartDmaChannel
	beq	SetupDma_found
	mov	r0, #0xdc
	blx	IsCartDmaChannel
	beq	SetupDma_found
	mov	r0, #0
	mov	r1, #0
SetupDma_found:
	adr	r2, CartDmaChannel
	stmia	r2!, {r0, r1}
	cmp	r0, #0
	beq	SetupDma_return
	mov	r2, #8
	lsl	r2, #0x1c
	bic	r1, r2
	str	r1, [r0]
@************************************
	cmp	r3, #2
	bne	SetupDma_return
	sub	r0, #4
	ldr	r1, [r0]
	ldr	r2, lastTargetAddress
	cmp	r1, r2
	bne	SetupDma_skip
	mov	r2, #0x80
	lsl	r2, #2	@ 0x200
	add	r1, r2
	str	r1, [r0]
SetupDma_skip:
	adr	r0, lastTargetAddress
	str	r1, [r0]
@************************************
SetupDma_return:
	pop	{r0-r2, pc}

	.align	2
	.include	"sd_read_data.s"
	.include	"card_dma_data.s"
CartDmaChannel:
	.word	0
CartDmaChannelVal:
	.word	0
lastTargetAddress:
	.word	0xEFC0DEC3

	.arm
IsCartDmaChannel:
	add	r0, r0, #0x4000000
	ldr	r1, [r0]
	and	r2, r1, #0x38000000
	cmp	r2, #0x28000000
	bx	lr

	.word	0xEFC0DEF2

func_2:
	mov	r3, #0
func_2_direct:
	mov	r12, lr
	mov	r1, #0x4000000
func_2_busy:
	ldr	r2, [r1, #0x1a4]
	tst	r2, #0x80000000
	bne	func_2_busy
	adr	r1, fixed
	ldmia	r1, {r1, r2}
	cmp	r1, #0
	beq	func_2_fixed
	str	r2, [r1]
func_2_fixed:
	cmp	r0, #0xb8000000
	beq	func_2_getchipid
	mov	r0, r0, lsl#8
	ldr	r1, sizeMask
	and	r0, r0, r1

	blx	SetupDma
	@ r3 rom type value unused from here

	@ disable IRQ
	mrs	r3, cpsr
	orr	r1, r3, #0x80
	ldr	r2, CartDmaChannel
	cmp	r2, #0
	msrne	cpsr_c, r1
	
	@ in r0
	@ trash r0-r2, lr
	blx	setupTransfer
	@ restore DMA
	ldrd	r0, CartDmaChannel
	cmp	r0, #0
	strne	r1, [r0]

	@ restore IRQ
	msr	cpsr_c, r3
func_2_return:
	bx	r12
sizeMask:
	.word	0xEFC0DEC2
func_2_getchipid:
	blx	stopSDTransmission
	mov	r0, r0, lsr#24
	mov	r1, #0x4000000
	strb	r0, [r1, #0x1a8]
	mov	r2, #0xffffffff
	str	r2, lastTargetAddress
	b	func_2_return

fixed:
	.word	0xEFC0DEC1
	.word	0xEFC0DEC1

	.word	0xEFC0DEF3
func_3:
	stmfd	sp!, {r0-r2, lr}	@ trashed in setupTransfer
	blx	stopSDTransmission
	blx	setupTransfer
	ldmfd	sp!, {r0-r2, lr}
	bx	r5

	.word	0xEFC0DEF4
func_4:
	stmfd	sp!, {r0-r2, lr}	@ trashed in setupTransfer
	blx	stopSDTransmission
	blx	setupTransfer
	ldmfd	sp!, {r0-r2, pc}

	.word	0xEFC0DEF6
func_6:
	@twl
	mov	r0, r0, lsl#24
	orr	r0, r0, r1, lsr#8
	mov	r3, #2
	b	func_2_direct
.end
