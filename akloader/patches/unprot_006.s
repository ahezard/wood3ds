/*
    unprot_006.s
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

@ size - 0xa0/160
.arm
	stmfd	sp!, {r0-r11}
	add	r2, sp, #52
	ldmia	r2, {r0, r1}
	add	r1, r0, r1
	adr	r2, patch
	ldmia	r2, {r6, r7, r8, r9, r10}
	adr	r11, reader
loop:
	cmp	r0, r1
	bhs	exit
	ldmia	r0, {r2, r3, r4, r5}
	cmp	r2, r6
	cmpeq	r3, r7
	cmpeq	r4, r8
	cmpeq	r5, r9
	stmeqia r0, {r10, r11}
	mcreq	p15, 0, r0, c7, c14, 1
	addeq	r0, #4
	mcreq	p15, 0, r0, c7, c14, 1
	add	r0, #4
	b	loop
	
exit:
	ldmfd	sp!, {r0-r11}
	add	sp, #0x2c
	bx	lr

patch:
	@ F8 4F 2D E9 10 D0 4D E2 00 B0 A0 E1 00 10 8D E5
	.word	0xe92d4ff8	@ r6 - stmfd sp!, {r3-r11,lr}
	.word	0xe24dd010	@ r7 - sub sp, sp, #0x10
	.word	0xe1a0b000	@ r8 - mov r11, r0
	.word	0xe58d1000      @ r9 - str r1, [sp]
	.word	0xe51ff004	@ r10 - ldr pc, [pc, #-4]

reader:
	subs	r1, #0x8000
	movlt	r1, #0
	add	r1, #0x4000
	mov	r12, #0x02800000
	ldr	r12, [r12, #-0x1d8]
	add	r1, r12
	add	r2, r0
reader_loop:
	cmp	r0, r2
	ldmltia	r1!, {r12}
	stmltia	r0!, {r12}
	blt	reader_loop
	bx	lr			

.end
