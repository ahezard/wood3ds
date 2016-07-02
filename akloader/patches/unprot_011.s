/*
    unprot_011.s
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

@
	.thumb
	sub	sp, #8	@ space for additional ret and ptr to data
	push	{r1-r7, lr}
	add	r7, sp, #40
	ldmia	r7!, {r0, r1, r2, r3, r4}
	sub	r7, #28
	add	r6, r5, #4
	adr	r5, patcher1
	add	r5, #1
	stmia	r7!, {r0, r1, r2, r3, r4, r5, r6}
	ldr	r0, boot_type
	ldrh	r0, [r0]
	pop	{r1-r7, pc}

	.align	2
patcher1:
	pop	{r2}
	push	{r0-r5}
	ldmia	r2!, {r0, r1}
	add	r1, r0, r1
	adr	r2, sign1
	adr	r3, patcher2
	mov	r4, #8
	mov	r5, #0xeb
	lsl	r5, #24
	blx	patcher_common
	pop	{r0-r5, pc}

	.align	2
	.arm
patcher2:
	mov	r0, #0	@ original code3
	stmfd	sp!, {r0-r8, lr}
	mov	r0, r8
	add	r1, r0, r7
	adr	r2, sign2
	adr	r3, reader
	mov	r4, #0x24
	mov	r5, #0xea000000
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
	orr	r7, r5
	str	r7, [r6]
	mcr	p15, 0, r6, c7, c14, 1
	b	patcher_common_loop

	.include	"include/reader.s"

sign1:
	.word	0x3affffd0	@ bcc xxx
	.word	0xe3a00000	@ mov r0, #0
	.word	0xe28ddc01	@ add sp, #0x100

sign2:
	.word	0xe1a00800	@ mov r0, r0 lsl#0x10
	.word	0xe1a0b820	@ mov r11, r0 lsr#0x10
	.word	0xe1a0000b	@ mov r0, r11

boot_type:
	.word	0x027ffc40

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
.end
