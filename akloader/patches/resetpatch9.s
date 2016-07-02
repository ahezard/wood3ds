/*
    resetpatch9.s
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

start:
	stmfd	sp!, {r0-r4}
	ldr	r0, pirqHandler9
	ldr	r1, irqHandler9
	str	r1, [r0]
	mov	r3, #0x2800000
@r0 - value
@r1 - reset state
@r2 - 0x4000208
@r3 - 0x2800000
@r4 - old ime state
reset_wait:
	ldr	r1, [r3,#-0x94]
	cmp	r1, #1
	bne	reseting
	ldr	r2, REG_IME
	ldr	r4, [r2]
	str	r3, [r2]	@ disable interrupts
	ldrh	r0, [r2, #-0xd8]
	cmp	r0, #0x7c
	@ is card ready?
	ldreq	r0, [r2, #-0x64]
	andeqs	r0, #0x80000000
	@is swap buffers finished?
	ldreq	r0, [r2, #0x3fc]
	cmpeq	r0, #0
reset_return:
	strne	r4, [r2]	@ restore interrupts
	ldmnefd	sp!, {r0-r4}
	ldrne	pc, irqHandler9
	mov	r0, #0x90000	@ wait 35ms
	swi	0x030000
	@check swap buffers again
	ldr	r0, [r2, #0x3fc]
	cmp	r0, #0
	bne	reset_return
	@ set state
	mov	r1, #0
	str	r1, [r3, #-0x94]
reseting:
	cmp	r1, #0
	beq	reset_wait

reset_stage_2:
	ldr	r0, new_access @needed for reset_extra
	mcr	p15, 0, r0, c6, c2
	mov	r4, #0x4000000
	mov	r0, #0
	@str	r0, [r4, #0x208]
	str	r0, [r4, #0x210]
	sub	r2, r0, #1
	str	r2, [r4, #0x214]
reset_dma:
	add	r1, r4, #0xb0
	mov	r2, #0x0c
	blx	memset
reset_master_brightness:
	str	r0, [r4, #0x6c]
	add	r1, r4, #0x1000
	str	r0, [r1, #0x6c]
copy_arm9:
	adr	r1, reset_extra
	ldr	r3, reset_extra_address
	ldr	r2, reset_extra_size
copy_arm9_loop:
	cmp	r3, r2
	ldmltia	r1!, {r12}
	stmltia	r3!, {r12}
	blt	copy_arm9_loop

DC_FlushAll:
	mov	r1, #0
DC_outer_loop:
	mov	r3, #0
DC_inner_loop:
	orr	r2, r1, r3
	mcr	p15, 0, r2, c7, c14, 2
	add	r3, r3, #0x20
	cmp	r3, #0x400
	bne	DC_inner_loop
	add	r1, r1, #0x40000000
	cmp	r1, #0
	bne	DC_outer_loop

	ldr	pc, reset_extra_address

reset_extra_address:
	.word	0x23b0000
new_access:
	.word	0x27ff017
irqHandler9:
	.word	0xefc0de01
pirqHandler9:
	.word	0xefc0de02

reset_extra:
@copied to 0x23b0000
@r4 - #0x4000000
@r0 - #0
	mov	r1, #0x4000
	str	r1, [r4,#0x204]
	ldr	r1, stopSD
	.word	0xefc0de10
	mov	r1, #0xe800
	str	r1, [r4,#0x204]

	ldr	r1, VRAM_value
	str	r1, [r4,#0x240]
	mov	r1, #0x5000000
	mov	r2, #0x800
	blx	memset
	mov	r1, #0x7000000
	blx	memset
	str	r0, [r4]
	mov	r1, #0x1000
	str	r0, [r4,r1]
	str	r0, [r4,#0x240]
	ldr	r1, Power_value
	mov	r2, #0x304
	strh	r1, [r4,r2]
	@ magic of wood goblins
	str	r0, [r4,#0x500]	
	mov	r1, #6
reset_extra_triangle:
	str	r0, [r4,#0x48c]	
	subs	r1, #1
	bne	reset_extra_triangle
	str	r0, [r4,#0x504]	
	str	r0, [r4,#0x540]	

	mov	r0, #3
	strb	r0, [r4,#0x247]

	ldr	r0, arm9LockAddr2
	sub	r1, r0, #0x1c
	str	r1, [r0]
	ldr	r0, arm9LockCode1
	str	r0, [r1]

	mov	r0, #0x2800000
	mov	r1, #3
	str	r1, [r0,#-0x94]

	swi	0

	.include	"bootloader.s"
	.thumb_func
@ in:
@ r0 - fill byte
@ r1 - destination
@ r2 - length in words
@ out:
@ r1 = r1 + 4 * r2
memset:
	push	{r2, lr}
memset_loop:
	stmia	r1!, {r0}
	sub	r2, #1
	bgt	memset_loop
	pop	{r2, pc}

	.align	2
	.word	0xefc0de03
	.thumb_func
reset_stage_3:
	mov	r0, #0
	ldr	r4, REG_IME
	str	r0, [r4]
	str	r0, [r4, #8]
	sub	r1, r0, #1
	str	r1, [r4,#0xc]

	mov	r1, #1
	lsl	r1, #0x19	@ 0x2000000
	mov	r2, #0x3b
	lsl	r2, #0x0e	@ 0xec000
	bl	memset	@ r1 = 0x23b0000
	ldr	r1, BootLoader_arm7_target
	mov	r2, #1
	lsl	r2, #0x0b	@ 0x800
	bl	memset

	ldr	r1, mem_area1	@0x27ffc08: games shared data1, 0x27ffc20 - reset parameter.
	mov	r2, #0x18
	bl	memset	@ r1 = 0x27ffc80
	sub	r1, #0x40	@ r1 = 0x27ffc40
	mov	r3, #1
	strb	r3, [r1]
	add	r1, #0xc0
	add	r1, #0x80	@0x27ffd80: games shared data2
	bl	memset	@ r1 = 0x27ffe00
	add	r1, #0xc0
	add	r1, #0xc0	@0x27fff80: games ipc area.
	bl	memset	@ r1 = 0x2800000
	mov	r3, #0x7	@0x27ff200: r.p.g. bootloader ipc area.
	lsl	r3, #9
	sub	r1, r3
	mov	r2, #0x40
	bl	memset
	mov	r1, #0x3f
	lsl	r1, #3
	add	r1, r4	@ r1 = 0x4000400
	add	r2, #8
	bl	memset	@ r1 = 0x4000520
	sub	r1, #0x1c
	mov	r0, #2
	lsl	r0, #8
	str	r0, [r1]

	b	ReadBootloader

	.align	2
	.include	"bootloader_data.s"
Power_value:
	.word	0x820f
VRAM_value:
	.word	0x80808080
stopSD:
	.word	0xefc0de11
arm9LockCode1:
	.word	0xe59ff014
mem_area1:
	.word	0x27ffc20
REG_IME:
	.word	0x4000208

	.arm
reset_extra_size:
	.word	.-reset_extra+0x23b0000

.end
