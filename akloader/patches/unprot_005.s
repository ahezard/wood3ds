/*
    unprot_005.s
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

@ size - 0x6c
.arm
	ldmib	r5, {r0, r1}
	stmfd	sp!, {r0-r8, lr}
	add	r1, r0, r1
	adr	r2, patch
	ldmia	r2!, {r3, r4}
	ldmia	r2!, {r5, r6}
loop:
	cmp	r0, r1
	bhs	exit
	ldmia	r0, {r7, r8}
	cmp	r7, r5
	cmpeq	r8, r6
	bne	continue
	stmia	r0, {r3, r4}
	ldmia	r2!, {r5, r6}
	cmp	r5, #0
	beq	exit

continue:
	add	r0, #4
	b	loop
	
exit:
	ldmfd	sp!, {r0-r8, pc}

patch:
	.word	0xe3a00000	@r3
	.word	0xe12fff1e	@r4
	.word	0xe92d43f0 
	.word	0xe24dd05c
	.word	0xe92d4ff8
	.word	0xe24dde22
	.word	0
	.word	0

.end
