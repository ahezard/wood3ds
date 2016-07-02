/*
    repatch9.s
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

.arm

func_5:
	ldr	r1, stopSD
	.word	0xefc0de10
	@ check for custom binary
	ldr	r0, RomBaseOffset
	ldr	r0, [r0]
	cmp	r0, #0
	bne	func_5_return
	@ main binary
	adr	r0, fixed
	ldmia	r0, {r0, r1}
	cmp	r0, #0
	strne	r1, [r0]
	blx	UncompressRom
	adr	r0, PatchTable
	ldr	r3, [r0], #4
	cmp	r3, #0
	beq	func_5_return
func_5_loop:
	ldmia	r0!, {r1, r2}
	cmp	r1, #0
	strge	r2, [r1]	
	rsblt	r1, #0
	strlth	r2, [r1]	
	subs	r3, #1
	bne	func_5_loop
func_5_return:

DC_FlushAll:
	mov	r1, #0
outer_loop:
	mov	r0, #0
inner_loop:
	orr	r2, r1, r0			@ generate segment and line address
	mcr	p15, 0, r2, c7, c14, 2		@ clean and flush the line
	add	r0, r0, #32
	cmp	r0, #0x400
	bne	inner_loop
	add	r1, r1, #0x40000000
	cmp	r1, #0
	bne	outer_loop

IC_InvalidateAll:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0

	mov	lr, r12
	mov	r11, #0
	bx	r12

stopSD:
	.word	0xefc0de11
fixed:
	.word	0xefc0dec1
	.word	0xefc0dec1
RomBaseOffset:
	.word	0x27ffc2c

	.thumb_func
UncompressRom:
	push	{r0-r7, lr}
	cmp	r0, #0
	beq	UncompressRom_return
	ldr	r1, [r0]
	mov	r3, #0
	str	r3, [r0]
	mov	r3, r1
	sub	r3, r3, #8
	ldr	r0, [r3]
	ldr	r6, mask1
	sub	r3, r1, #4
	ldr	r2, [r3]
	lsr	r3, r0, #24
	and	r0, r0, r6
	add	r2, r1, r2
	sub	r3, r1, r3
	sub	r6, r1, r0
	b	UncompressRom_loop1_end
UncompressRom_loop1:
	sub	r3, r3, #1
	ldrb	r0, [r3]
	mov	r1, #8
UncompressRom_loop2:
	sub	r1, r1, #1
	bcc	UncompressRom_loop1_end
	mov	r5, #128
	sub	r4, r3, #1
	tst	r0, r5
	bne	UncompressRom_loop2skip
	mov	r3, r4
	ldrb	r4, [r4]
	sub	r2, r2, #1
	strb	r4, [r2]
	b	UncompressRom_loop2_end
UncompressRom_loop2skip:
	ldrb	r7, [r4]
	sub	r3, r3, #2
	ldrb	r4, [r3]
	lsl	r5, r7, #8
	orr	r5, r5, r4
	ldr	r4, mask2
	add	r7, r7, #32
	and	r5, r5, r4
	add	r5, r2, r5
	mov	r4, #0
	mov	lr, r3
UncompressRom_loop3:
	ldrb	r3, [r5, #2]
	sub	r2, r2, #1
	sub	r4, r4, #16
	strb	r3, [r2]
	sub	r5, r5, #1
	cmn	r7, r4
	bpl	UncompressRom_loop3
	mov	r3, lr
UncompressRom_loop2_end:
	cmp	r3, r6
	bls	UncompressRom_return
	lsl	r0, r0, #1
	b	UncompressRom_loop2
UncompressRom_loop1_end:
	cmp	r3, r6
	bhi	UncompressRom_loop1
UncompressRom_return:
	pop	{r0-r7, pc}

	.align	2
mask1:
	.word	0x00ffffff
mask2:
	.word	0xffff0fff

PatchTable:

.end
