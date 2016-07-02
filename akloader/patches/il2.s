/*
    il2.s
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
@ 0x2000270
@ from 2009334
@ 0xebffdbcd
entry1:
	mov	r0, r4
	ldr	r1, idx1
	add	r2, r1, #6
	cmp	r6, r1
	addeq	r0, #0x30
	cmp	r6, r2
	addeq	r0, #0x3cc
	bx	lr

@ 0x2000290
@ from 2009358
@ 0xebffdbcc
entry2:
	ldr	r0, idx1
	cmp	r6, r0
	bne	check2
	mov	r1, #6
	str	r1, [r4, #0x18]
	add	r0, r4, #0x10c
	adr	r1, data1
	mov	r2, #0x30
loop:
	ldr	r3, [r1], #4
	str	r3, [r0], #4
	subs	r2, r2, #4
	bne	loop

check2:
	add	r0, #6
	cmp	r6, r0
	bne	exit
	adr	r0, data2
	mov	r1, r4
	swi	0x110000

exit:
	bx	lr

idx1:
	.word	0xbd1

data1:
	.word	0
	.word	1
	.word	0x90
	.word	3
	.word	0x8f
	.word	0
	.word	0x92
	.word	2
	.word	3
	.word	2
	.word	0
	.word	0
data2:
	.word	0x0006dc10
	.word	0x00000f05
	.word	0x03400100
	.word	0x560b40bb
	.word	0x10130003
	.word	0x00021700
	.word	0xc41b1018
	.word	0x102300d0
	.word	0x2b00c302
	.word	0xffffff85
	.word	0x0f403657
	.word	0x043b0012
	.word	0x43101b40
	.word	0xf0f51a00
	.word	0xf014f002
	.word	0x02530026
	.word	0x40c68740
	.word	0x40c55f6b
	.word	0x6bc0416b
	.word	0x6ff06750
	.word	0x8ff07df0
	.word	0xc86bb0ab
	.word	0x40c7d740
	.word	0xd7c04cd7
	.word	0x50fa9310
	.word	0xf0dff0d3
	.word	0x70fff0ed
	.word	0x4341cad7
	.word	0x4341bfc9
	.word	0x1143c157
	.word	0xf13f9173
	.word	0xf15df14f
	.word	0xcb31ab43
	.word	0xcbaf41cc
	.word	0xc162af41
	.word	0xf1b311af
	.word	0xbff1abd1
	.word	0xaff1cdf1
	.word	0x42ce0000
	.word	0x42cd571b
	.word	0x1bc26d1b
	.word	0xf117f205
	.word	0xd538f24f
	.word	0xa7021bf2
	.word	0xcf8742d0
	.word	0xc2788742
	.word	0xf2067d87
	.word	0xf24ff183
	.word	0x0387f2a4
	.word	0xf342d213
	.word	0xf342d157
	.word	0x07f3c283
	.word	0x4ff1eff2
	.word	0xf2d510f3
	.word	0xd47f03f3
	.word	0x43d35f43
	.word	0x5fc38e5f
	.word	0x5bf3087d
	.word	0x7ff34ff1
	.word	0xeb035ff3
	.word	0x57cb43d6
	.word	0x99cb43d5
	.word	0xf309cbc3
	.word	0xf3d6f3c7
	.word	0xcbf3d67f
	.word	0x44d85704
	.word	0x3f44d737
	.word	0xfa0a37d4
	.word	0x42f433f4
	.word	0x37f47ff3
	.word	0x44dac304
	.word	0x44dfd9a3
	.word	0x0b37d4ab
	.word	0xaef49ff4
	.word	0xa3f47ff3
	.word	0xdc5b2f05
	.word	0x45db0f45
	.word	0x0ccbd317
	.word	0x1af50bf5
	.word	0xa57ff3eb
	.word	0xde0f553f
	.word	0x45dd7b45
	.word	0x7dcbd383
	.word	0xf577f50d
	.word	0xa598f586
	.word	0xe07b553f
	.word	0xdf6fe745
	.word	0xcbd3ef45
	.word	0xf5e3f50e
	.word	0xa504f6f2
	.word	0xe755af3f
	.word	0xe15346e2
	.word	0xcbd35b46
	.word	0x4ff69716
	.word	0xf661f6e0
	.word	0x00af7573
