/*
    unprot_003.s
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

@ size - 0x40
.arm
	ldr	r0, [r7, r8] 
	ldr	r1, patch+0x10
	cmp	r0, r1
	bne	return
	adr	r0, patch
	ldmia	r0, {r0, r1, r2, r3}
	stmia	r2, {r0, r1}
	stmia	r3, {r0, r1}

return:
	mov	r0, #1
	ldmfd	sp!, {r4-r8,pc}

patch:
	.word	0xe3a00000	@r0
	.word	0xe12fff1e	@r1
@	.word	0xxxxxxxxx	@r2
@	.word	0xyyyyyyyy	@r3
@	.word	0xzzzzzzzz

.end
