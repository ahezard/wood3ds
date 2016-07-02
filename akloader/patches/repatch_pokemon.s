/*
    repatch_pokemon.s
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
	@ check for custom binary
	ldr	r0, RomBaseOffset
	ldr	r0, [r0]
	cmp	r0, #0
	bne	patch_eoo
	@ main binary

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
func_5_sd_start:
	adr	r0, WRAMStart
	ldmia	r0, {r0, r1, r2}
	cmp	r0, #0
	beq	func_5_return
func_5_sd:
	ldr	r3, [r0]
	cmp	r3, r2
	beq	func_5_sd_found
	add	r0, #4
	cmp	r0, r1
	bne	func_5_sd
	b	func_5_return
func_5_sd_found:
	adr	r1, sd_save
	ldmia	r1!, {r2, r3}
	add	r3, r2
func_5_sd_copy:
	ldmia	r0!, {r1}
	stmia	r2!, {r1}
	cmp	r2, r3
	bne	func_5_sd_copy
func_5_return:
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bx	r12

patch_eoo:
	stmfd	sp!, {r4-r10, lr}
	adr	r0, prot_sign
	ldmia	r0, {r4, r5, r6, r7, r8, r9, r10}
	mov	r0, #0x02000000
	add	r1, r0, #0x00030000
patch_eoo_loop:
	ldmia	r0, {r2, r3}
	cmp	r2, r4
	cmpeq	r3, r5
	bleq	patch_prot
	cmp	r2, r6
	cmpeq	r3, r7
	bleq	patch_multi
	cmp	r2, r8
	cmpeq	r3, r9
	bleq	patch_read2
	cmp	r2, r10
	bleq	patch_check
	add	r0, #4
	cmp	r0, r1
	blt	patch_eoo_loop

	adr	r0, reset_sign
	ldmia	r0, {r4, r5, r6, r7}
	mov	r0, #0x02380000
	add	r1, r0, #0x00030000
patch_eoo_loop2:
	ldmia	r0, {r2, r3}
	cmp	r2, r4
	cmpeq	r3, r5
	bleq	patch_eoo_reset2
	cmp	r2, r6
	cmpeq	r3, r7
	bleq	patch_eoo_reset
	add	r0, #4
	cmp	r0, r1
	blt	patch_eoo_loop2

	ldmfd	sp!, {r4-r10, lr}
	ldr	r0, intermediate_area
	str	r0, sd_save
	b	func_5_sd_start

patch_eoo2:
	stmfd	sp!, {r0-r4, lr}
	bl	patch_client1_internal
	ldmfd	sp!, {r0-r4, lr}
	bx	r12

patch_client1:
	stmfd	sp!, {r0-r4, lr}
	bl	patch_client1_internal
	ldmfd	sp!, {r0-r4, lr}
	bx	r4

@ clobbers r0-r4
patch_client1_internal:
@ sd patching start
	ldr	r0, WRAMStart
	cmp	r0, #0
	beq	patch_client1_internal_start
	stmfd	sp!, {r5-r10}
	mov	r0, #0x02380000
	add	r1, r0, #0x00030000
	mov	r2, #1
	adr	r3, save_sign_1
	ldmia	r3, {r3, r4, r6, r7, r8, r9}
patch_client1_internal_sd_save_loop:
	ldr	r10, [r0]
	cmp	r10, r4
	orreq	r6, #1
	cmpne	r10, r3
	subeq	r0, #8
	moveq	r5, r2, lsr#1
	addeq	r5, r9
	stmeqia	r0!, {r5, r6, r7, r8}
	addeq	r2, #1
	add	r0, #4
	cmp	r0, r1
	blt	patch_client1_internal_sd_save_loop
	mov	r0, #0x02000000	
	ldr	r1, call_op
	ldr	r2, intermediate_area
	stmia	r0, {r1, r2}
	ldmfd	sp!, {r5-r10}
@ sd patching finish
patch_client1_internal_start:
	mov	r0, #0x02000000
	add	r1, r0, #0x00080000
	ldr	r4, decompress_sign
patch_client1_internal_loop:
	ldr	r2, [r0]
	cmp	r2, r4
	ldreq	r2, call_op
	adreq	r3, patch_client2
	stmeqia	r0!, {r2, r3}
	addne	r0, #4
	cmp	r0, r1
	blt	patch_client1_internal_loop
	bx	lr

patch_client2:
	stmfd	sp!, {r8-r9, lr}
	adr	r0, multi1_sign
	ldmia	r0, {r2, r3, r4, r5}
	adr	r0, prot_sign
	ldmia	r0, {r6, r7}
	mov	r0, #0x02000000
	add	r1, r0, #0x00080000
patch_client2_loop:
	ldmia	r0, {r8, r9}
	cmp	r8, r2
	cmpeq	r9, r3
	bleq	patch_multi
	cmp	r8, r4
	cmpeq	r9, r5
	bleq	patch_multi
	cmp	r8, r6
	cmpeq	r9, r7
	bleq	patch_prot
	add	r0, #4
	cmp	r0, r1
	blt	patch_client2_loop

	bl	DC_FlushAll

	ldmfd	sp!, {r8-r9, lr}
	ldmfd	sp!, {r4-r7}	@ original code
	bx	lr

patch_multi:
	stmfd	sp!, {r1, r2, lr}
	ldr	r1, call_op
	ldr	r2, func_2_addr
	cmp	r2, #0	@ check if we start from nand
	stmneia	r0, {r1, r2}
	ldmfd	sp!, {r1, r2, pc}

patch_prot:
	stmfd	sp!, {r1, lr}
	ldr	r1, nop_op
	str	r1, [r0]
	ldmfd	sp!, {r1, pc}

patch_eoo_reset:
	stmfd	sp!, {r1, r2, lr}
	ldr	r1, call_op
	adr	r2, patch_client1
	stmia	r0, {r1, r2}
	ldmfd	sp!, {r1, r2, pc}
	
patch_eoo_reset2:
	stmfd	sp!, {r1, r2, lr}
	ldr	r1, call_op
	adr	r2, patch_eoo2
	stmia	r0, {r1, r2}
	ldmfd	sp!, {r1, r2, pc}

patch_read2:
	stmfd	sp!, {r1, r2, r3, lr}
	ldr	r1, ret3_op
	ldr	r2, call_op
	ldr	r3, func_3_addr
	cmp	r3, #0	@ check if we start from nand
	stmneia	r0, {r1, r2, r3}
	ldmfd	sp!, {r1, r2, r3, pc}

patch_check:
	stmfd	sp!, {r1, lr}
	ldr	r1, [r0, #12]
	eor	r1, #0xf0000000
	str	r1, [r0, #12]
	ldmfd	sp!, {r1, pc}

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
	bx	lr

RomBaseOffset:
	.word	0x27ffc2c
WRAMStart:
	.word	0xefc0de01	@ 0x037f8000 or 0
	.word	0x03810000
	.word	0xe28fc001	@ mov r12, pc+1
sd_save:
	.word	0xefc0de02	@ target
	.word	0xefc0de04	@ length
func_2_addr:
	.word	0xefc0def2
func_3_addr:
	.word	0xefc0def3
intermediate_area:
	.word	0x023ff100

@ must be in this order
prot_sign:
	.word	0xee060f12	@ mcr p15, 0, r0, c6, c2
	.word	0xe12fff1e	@ bx lr
read1_sign:
	.word	0xe59f3050	@ ldr r3, =0x40001a4
	.word	0xe5932000	@ ldr r2, [r3]
read2_sign:
	.word	0xe5c7500c	@ strb r5, [r7,#0xc]
	.word	0xe5c7500d	@ strb r5, [r7,#0xd]
check_sign:
	.word	0xe2811040	@ add r1, #0x40
@ finish of pre ordered data

@ must be in this order
multi1_sign:	@ revolution
	.word	0xe59f3070	@ ldr r3, =0x40001a4
	.word	0xe5932000	@ ldr r2, [r3]
multi2_sign:	@ ranch / platinum
	.word	0xe59f3050	@ ldr r3, =0x40001a4
	.word	0xe5932000	@ ldr r2, [r3]
@ finish of pre ordered data

@ must be in this order
reset_sign:
	.word	0xe3a03000	@ mov r3, #0
	.word	0xe12fff1c      @ bx r12
eoo_sign:
	.word	0xe12fff14	@ bx r4
	.word	0xe3a00000	@ mov r0, #0
@ finish of pre ordered data

@ must be in this order
save_sign_1:
	.word	0xe5922014
save_sign_2:
	.word	0xe5932014
save_op:
	.word	0xe1a00002	@ mov	r0, r2
	.word	0xe28fe000	@ adr	lr, return
	.word	0xe3a0f402	@ mov	pc, #0x2000000
	.word	0xe3a01000	@ mov	r1, #0
@ finish of pre ordered data

call_op:
	.word	0xe51ff004	@ ldr pc, [pc, #-4]
nop_op:
	.word	0xe1a00000	@ nop
ret3_op:
	.word	0xe28f5004	@ adr r5, [pc, #4]
decompress_sign:
	.word	0xe8bd00f0	@ ldmfd sp!, {r4-r7}

PatchTable:

.end
