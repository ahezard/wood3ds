/*
    reader.s
    Copyright (C) 2010 yellow wood goblin

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

reader:
	stmfd	sp!, {r3-r7, lr}
	mov	r6, #0x04000000
	ldr	r7, [r6, #0x208]
	str	r6, [r6, #0x208]
	ldr	r5, [r6, #0x204]
	bic	r2, r5, #0x800
	str	r2, [r6, #0x204]
	mov	r4, r0
	cmp	r1, #0x8000
	movlo	r1, #0x8000
	.word	0xefc0def3
	.word	0xefc0def4
	.word	0xefc0def5
	ldr	r2, CARDi_SetRomOp
	blx	r2
	mov	r3, #0x04100000
	ldr	r1, RomFlag
	str	r1, [r6, #0x1a4]
reader_loop:
	ldr	r2, [r6, #0x1a4]
	ands	r1, r2, #0x800000
	beq	reader_busy
	ldr	r1, [r3, #0x10]
	str	r1, [r4], #4
reader_busy:
	ands	r1, r2, #0x80000000
	bne	reader_loop
	str	r5, [r6, #0x204]
	str	r7, [r6, #0x208]
	ldmfd	sp!, {r3-r7, pc}

CARDi_SetRomOp:
	.word	0xefc0def1
RomFlag:
	.word	0xefc0def2
