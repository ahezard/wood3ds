/*
    unprot_015.s
    Copyright (C) 2010 green wood goblin

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
	mov	r5, r0	@ original code1
	sub	sp, #8	@ space for additional ret and ptr to data
	stmfd	sp!, {r0-r5, lr}
	add	r4, r5, #4
	add	r5, sp, #36
	ldmia	r5, {r0, r1, r2}
	sub	r5, #8
	adr	r3, patcher1
	stmia	r5, {r0, r1, r2, r3, r4}
	ldmfd	sp!, {r0-r5, pc}

	.include	"include/unprot_common_002.s"
.end
