/*
    hb_reset.s
    Copyright (C) 2011 yellow wood goblin

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
	b	start_arm7
start_arm9:
	mrs	r0, spsr	@ original code
	.word	0xefc0de03
	stmfd	sp!, {r0-r4}
	mov	r3, #0x2800000
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
	strne	r4, [r2]	@ restore interrupts
	ldmnefd	sp!, {r0-r4}
	ldrne	pc, back_arm9
	@ set state
	mov	r1, #0
	str	r1, [r3, #-0x94]
reseting:
	cmp	r1, #0
	beq	reset_wait
	ldr	r4, REG_IME
	blx	clean
	mov	r1, #0xe800
	str	r1, [r4,#-4]
	swi	0

start_arm7:
	mrs	r0, spsr	@ original code
	stmfd	sp!, {r1, r6}
	mov	r6, #0x2800000
	ldr	r1, [r6,#-0x94]
	cmp	r1, #0
	ldmnefd	sp!, {r1, r6}
	ldrne	pc, back_arm7
	ldr	r4, REG_IME
	adr	r0, (reset_stage_0+1)
	bx	r0
@ r4 - 0x4000208
	.thumb
reset_stage_0:
	bl	clean	
	@ clear sound
	mov	r1, #0x3f
	lsl	r1, #3
	add	r1, r4	@ r1 = 0x4000400
	mov	r2, #0x48
	bl	memset	@ r1 = 0x4000520
	sub	r1, #0x1c
	mov	r0, #2
	lsl	r0, #8
	str	r0, [r1]

	adr	r0, reset_stage_2
	mov	r1, #0x38
	lsl	r1, #20
	mov	sp, r1
	ldr	r2, reset_stage_2_size
	add	r4, r1, #1

reset_stage_0_copy:
	ldmia	r0!, {r3}
	stmia	r1!, {r3}
	cmp	r1, r2
	blt	reset_stage_0_copy
	bx	r4

	.align	2
reset_stage_2:
	adr	r5, arm9stub
	ldmia   r5!, {r0-r4}
	str     r4, [r4,#0x24]
	stmia   r4!, {r0-r3}

	mov	r0, #2
	sub	r6, #0x94
	str	r0, [r6]
wait_arm9:
	ldr	r0, [r6]
	cmp	r0, #3
	bne	wait_arm9

	mov	r0, #0
	mov	r1, #1
	lsl	r1, #0x19	@ 0x2000000
	ldr	r2, memsize
	bl	memset
	ldr	r0, memaddress
	ldr	r1, memvalue
	strh	r1, [r0]
	lsr	r1, #16
	strh	r1, [r0, #0x10]
	mov	r1, #1
	str	r1, [r0, #0x40]
	b	ReadBootloader

	.include	"bootloader.s"

	.thumb_func
memset:
	push	{r2, lr}
memset_loop:
	stmia	r1!, {r0}
	sub	r2, #1
	bgt	memset_loop
	pop	{r2, pc}

	.thumb_func
clean:
	push	{lr}
	@disable interrupts
	mov	r0, #0
	str	r0, [r4]
	str	r0, [r4, #8]
	sub	r1, r0, #1
	str	r1, [r4,#0xc]
	@clear dma
	mov	r1, r4
	sub	r1, #0xff
	sub	r1, #0x59
	mov	r2, #0x60
	bl	memset
	pop	{pc}

	.align	2
	.include	"bootloader_data.s"
back_arm7:
	.word	0xefc0de01
back_arm9:
	.word	0xefc0de02
REG_IME:
	.word	0x4000208
	.arm
arm9stub:
	mov	r0, #0x2800000
	mov	r1, #3
	str	r1, [r0, #-0x94]
	ldr	pc, [pc, #0x10]
	.word  0x027ffe00
memsize:
	.word	0xfff80
memaddress:
	.word	0x027ffc00
memvalue:
	.word	0x58350fc2
reset_stage_2_size:
	.word	.-reset_stage_2+0x03800000
.end
