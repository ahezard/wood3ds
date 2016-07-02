/*
    unprot_008.s
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
	ldr	r1, [sp]
	cmp	r1, #1
	bne	return
	adr	r1, patch
	ldmia	r1, {r1, r2, r3, r4}
	str	r3, [r1]
	str	r4, [r2]
return:
	add	sp, #0x2c
	bx	lr
patch:
	.word	0xefc0de01
	.word	0xefc0de02
	.word	0xe3a05921	@ mov	r5, #0x84000
	.word	0xe3a04000	@ mov	r4, #0

.end
