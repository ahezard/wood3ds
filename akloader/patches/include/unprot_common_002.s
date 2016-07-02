/*
    unprot_common_002.s
    Copyright (C) 2010 red wood goblin

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

patcher1:
	ldmfd	sp!, {r2}
	stmfd	sp!, {r0-r5}
	ldmia	r2, {r0, r1}
	add	r1, r0, r1
	adr	r2, sign1
	adr	r3, patcher2
	mov	r4, #4
	ldr	r5, push_lr_pc
	bl	patcher_common
	adr	r2, sign2
	adr	r3, patcher3
	mov	r4, #8
	mov	r5, #0
	bl	patcher_common
	ldmfd	sp!, {r0-r5, pc}

patcher2:
	str	r2, [r0, #4]	@ original code2
	str	r2, [r0]	@ original code2
	str	lr, [sp, #4]	@ fix pc
	stmfd	sp!, {r0-r5}
	mov	r1, r5
	sub	r0, r1, #16
	adr	r2, sign2
	adr	r3, patcher3
	mov	r4, #8
	mov	r5, #0
	bl	patcher_common
	ldmfd	sp!, {r0-r5, lr, pc}

patcher3:
	mov	r0, #0	@ original code3
	stmfd	sp!, {r0-r8, lr}
	mov	r0, r5
	add	r1, r0, r11
	adr	r2, sign3
	adr	r3, reader
	mov	r4, #4
	mov	r5, #0
	bl	patcher_common
patcher_common_exit:
	ldmfd	sp!, {r0-r8, pc}

patcher_common:
	stmfd	sp!, {r0-r8, lr}
patcher_common_loop:
	blx	search
	cmp	r0, #0
	beq	patcher_common_exit
	sub	r6, r0, r4
	blx	generate
	orr	r7, #0xeb000000
	str	r7, [r6]
	mcr	p15, 0, r6, c7, c14, 1
	cmp	r5, #0
	subne	r6, #4
	strne	r5, [r6]
	mcrne	p15, 0, r6, c7, c14, 1
	b	patcher_common_loop

	@ r4 - address
reader:
	stmfd	sp!, {r0-r3, r12, lr}
	mov	r1, r4
	cmp	r1, #0x8000
	movlo	r1, #0x8000
	.word	0xefc0def3
	.word	0xefc0def4
	.word	0xefc0def5
	ldr	r2, CARDi_SetRomOp
	blx	r2
	mov	lr, #0
	ldmfd	sp!, {r0-r3, r12, pc}

CARDi_SetRomOp:
	.word	0xefc0def1

sign1:
	.word	0x3afffff8	@ bcc xxx
	.word	0xe5802004	@ str r2, [r0, #4]
	.word	0xe5802000	@ str r2, [r0]

sign2:
	.word	0x3affffae	@ bcc xxx
	.word	0xe3a00000	@ mov r0, #0
	.word	0xe28dd008	@ add sp, #8

sign3:
	.word	0xe5c101ad	@ strb r0, [r1, #0x1ad]
	.word	0xe5c101ae	@ strb r0, [r1, #0x1ae]
	.word	0xe5c101af	@ strb r0, [r1, #0x1af]

push_lr_pc:
	.word	0xe92dc000	@ stmfd sp!, {lr, pc}

	.thumb_func
@ r0 - start search
@ r1 - end search
@ r2 - address of three search bytes

search:
	push	{r2-r7, lr}
	ldmia	r2, {r2, r3, r4}
search_loop:
	cmp	r0, r1
	bhs	search_fail
	ldmia	r0!, {r5, r6, r7}
	cmp	r2, r5
	bne	search_skip
	cmp	r3, r6
	bne	search_skip
	cmp	r4, r7
	beq	search_exit
search_skip:
	sub	r0, #8
	b	search_loop
search_fail:
	mov	r0, #0
search_exit:
	pop	{r2-r7, pc}

	.thumb_func
@ r3 - destination
@ r6 - source
@ r7 - result

generate:
	sub	r7, r3, r6
	sub	r7, #8
	lsl	r7, #6
	lsr	r7, #8
	bx	lr

	.align	2
