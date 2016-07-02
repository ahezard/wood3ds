/*
    sd_save.s
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
	adr	r12, (ProcessSave+1)
	bx	r12

	.thumb_func
@ r3 - f_shift / type
@ r4 - flash
@ r5 - f_size
@ r6 - size
@ r7 - memory
@ r12 - p_addr
@ [sp] - data
@ [sp+4] - type
ProcessSave:
	push	{r0-r7, lr}
	sub	sp, #508
	sub	sp, #4
	ldr	r7, [r0, #12]
	ldr	r4, [r0, #16]
	cmp	r1, #0
	bne	ProcessSave_data_no_read
	@main memory access - slow
	mov	r3, r4
	mov	r4, r7
	mov	r7, r3
ProcessSave_data_no_read:
	ldr	r3, mod_size
	and	r4, r4, r3
	ldr	r6, [r0, #20]

	mov	r5, #128
	lsl	r5, r5, #2
	sub	r3, r5, #1
	and	r3, r4, r3
	sub	r5, r5, r3

	cmp	r5, r6
	bls	ProcessSave_skip_small_size
	mov	r5, r6
ProcessSave_skip_small_size:
	mov	r0, r4
	bl	lb2pb
	mov	r12, r0

	mov	r1, sp	@ buffer
	bl	sddReadSingleBlock

	mov	r0, r7
	@ r1 = buffer+f_shift
	add	r1, r3
	mov	r2, r5

	ldr	r3, [sp, #516]	@ r1 - type
	cmp	r3, #0
	beq	ProcessSave_HeadRead
	cmp	r3, #1
	bne	ProcessSave_HeadVerify
ProcessSave_HeadWrite:
	mov	r0, r1
	mov	r1, r7
	bl	memcpy
	mov	r0, r12
	mov	r1, sp	@ buffer
	bl	sddWriteSingleBlock
	b	ProcessSave_loop
ProcessSave_HeadRead:
	bl	memcpy
	b	ProcessSave_loop
ProcessSave_HeadVerify:
	bl	memcmp
	cmp	r0, #0
	bne	ProcessSave_verify_fail
ProcessSave_loop:
	add	r4, r4, r5
	ldr	r0, mod_size
	and	r4, r4, r0
	sub	r6, r6, r5
	add	r7, r7, r5
	mov	r5, #128
	lsl	r5, r5, #2
	cmp	r6, r5
	blo	ProcessSave_tail
	mov	r0, r4
	bl	lb2pb
	mov	r1, r7
	cmp	r3, #1
	bne	ProcessSave_LoopRead
	bl	sddWriteSingleBlock
	b	ProcessSave_loop
ProcessSave_LoopRead:
	cmp	r3, #2
	bne	ProcessSave_LoopRead2
	mov	r1, sp	@ buffer
ProcessSave_LoopRead2:
	bl	sddReadSingleBlock
	cmp	r3, #2
	bne	ProcessSave_loop
	mov	r0, r7
	mov	r2, r5
	bl	memcmp
	cmp	r0, #0
	beq	ProcessSave_loop
	b	ProcessSave_verify_fail
ProcessSave_tail:
	cmp	r6, #0
	beq	ProcessSave_exit
	mov	r0, r4
	bl	lb2pb
	mov	r1, sp	@ buffer
	mov	r4, r0
	bl	sddReadSingleBlock

	mov	r0, r7
	mov	r2, r6

	cmp	r3, #0
	beq	ProcessSave_TailRead
	cmp	r3, #1
	bne	ProcessSave_TailVerify

ProcessSave_TailWrite:
	mov	r0, sp	@ buffer
	mov	r1, r7
	bl	memcpy
	mov	r0, r4
	mov	r1, sp	@ buffer
	bl	sddWriteSingleBlock
	b	ProcessSave_exit
ProcessSave_TailRead:
	bl	memcpy
	b	ProcessSave_exit
ProcessSave_TailVerify:
	bl	memcmp
	cmp	r0, #0
	beq	ProcessSave_exit
ProcessSave_verify_fail:
	mov	r0, #1
	ldr	r1, [sp, #512]	@ r0 - data
	str	r0, [r1]
ProcessSave_exit:
	add	sp, #508
	add	sp, #4
	pop	{r0-r7}
	pop	{r0}
	bx	r0

	.include	"include/memcpy.s"
	.include	"include/memcmp.s"
	.include	"sd_read.s"
	.include	"sd_write.s"

	.align	2
	.include	"sd_read_data.s"
	.include	"sd_write_data.s"
mod_size:
	.word	0xefc0de01
