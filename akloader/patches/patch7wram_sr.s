/*
    patch7wram_sr.s
    Copyright (C) 2008 somebody
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
	adr	r1, (start+1)
	bx	r1
.thumb
start:
	ldr	r3, main_addr
	ldr	r1, [r3, #0xc]
	adr	r2, REG_IME
	ldrsb	r2, [r2, r1]
	add	pc, r2
	nop
@ base - state 2
	b	start
@ state 1
	bx	lr
@ state 3
	ldr	r1, main_code
	bx	r1
@ state 0
	ldr	r0, REG_IME
	strh	r1, [r0]
	add	r0, #0xfc
	add	r0, #0xfc
	mov	r2, #0x47
	lsl	r2, r2, #2	@ 0x11c
clearSound:
	str	r1, [r0, r2]
	sub	r2, #4
	bge	clearSound
	mov	r1, #2
	str	r1, [r3, #0xc]
	b	start
	.align 2
main_addr:
	.word	0x27fff60
main_code:
	.word	0xEFC0DE01
REG_IME:
	.word	0x04000208
.end

