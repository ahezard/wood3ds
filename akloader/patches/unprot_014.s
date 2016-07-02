/*
    unprot_014.s
    Copyright (C) 2010 red wood goblin

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
	ldmib	r5, {r0, r1}
	stmfd	sp!, {r0-r3, lr}
	adr	r0, data
	ldmia	r0, {r0, r1, r2}
	ldr	r3, [r5]
	cmp	r0, r3
	bne	exit
	str	r2, [r1]
	mcr	p15, 0, r1, c7, c14, 1
exit:
	ldmfd	sp!, {r0-r3, pc}
data:
	.word	0xefc0de01	@ overlay
	.word	0xefc0de02	@ address
	.word	0x47702001	@ mov r0, #1|bx lr

.end
