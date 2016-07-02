/*
    unprot_010.s
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

@
	.arm

patcher1:

	ldrd	r0, patch1
	str	r0, [r1]
	.word	0xefc0de06	@ original code1

patcher2:
	mov	r0, #0	@ original code2
	stmfd	sp!, {r0-r1, lr}
	ldrd	r0, patch2
	.word	0xefc0de05	@ cmp r1, dst
	bne	patcher2_exit
	str	r0, [r1]
patcher2_exit:
	ldmfd	sp!, {r0-r1, pc}

	.include	"include/reader.s"

patch1:
	.word	0xefc0de01	@ value
	.word	0xefc0de02	@ address
patch2:
	.word	0xefc0de03	@ value
	.word	0xefc0de04	@ address

.end
