/*
    bank_nor_1.s
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

SetRamPage_continue:
	add	r3, r3, #0x1bc0000
	moveq	r0, #0x10
	strh	r0, [r3]
	add	r3, r3, #0x3c0000
	strh	r1, [r3]
	bx	lr
base:
	.word	0x9fe0000
SetRamPage:
	ldr	r3, base
	mov	r2, #0xd200
	strh	r2, [r3]
	mov	r1, #0x1500
	mov	r3, #0x8000000
	strh	r1, [r3]
	add	r3, r3, #0x20000
	strh	r2, [r3]
	cmp	r0, #0
	add	r3, r3, #0x20000
	strh	r1, [r3]
	movne	r0, #0x20
	b	SetRamPage_continue
.end
