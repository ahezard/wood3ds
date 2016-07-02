/*
    unprot_004.s
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

@ size - 0x??
.thumb
	ldr	r0, [sp] 
	ldr	r1, patch+0x10
	cmp	r0, r1
	bne	return
	adr	r4, patch
	ldmia	r4!, {r0, r1, r2, r3}
	stmia	r2!, {r0, r1}
	stmia	r3!, {r0, r1}
return:
	mov	r0, #1
	add	sp, #0x2c
	pop	{pc}

	.align	2
patch:
	.word	0xe3a00000	@r0
	.word	0xe12fff1e	@r1
@	.word	0xxxxxxxxx	@r2
@	.word	0xyyyyyyyy	@r3
@	.word	0xzzzzzzzz

.end
