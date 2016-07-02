/*
    unprot_017.s
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
	.thumb
	sub	sp, #8	@ space for additional ret and ptr to data
	push	{r1-r7, lr}
	add	r7, sp, #40
	ldmia	r7!, {r0, r1, r2, r3, r4}
	sub	r7, #28
	add	r6, r5, #4
	adr	r5, patcher1
	stmia	r7!, {r0, r1, r2, r3, r4, r5, r6}
	ldr	r0, boot_type
	ldrh	r0, [r0]
	pop	{r1-r7, pc}

	.align	2
boot_type:
	.word	0x027ffc40
	.arm

	.include	"include/unprot_common_002.s"
.end
