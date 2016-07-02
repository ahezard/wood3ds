/*
    memcmp.s
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

	.thumb_func
memcmp:
	push	{r1-r5, lr}
	cmp	r2, #0
	beq	memcmp_equal
	mov	r3, #0
memcmp_loop:
	ldrb	r5, [r0, r3]
	ldrb	r4, [r1, r3]
	cmp	r5, r4
	beq	memcmp_match
	mov	r0, #1
	b	memcmp_return
memcmp_match:
	add	r3, r3, #1
	cmp	r2, r3
	bne	memcmp_loop
memcmp_equal:
	mov	r0, #0
memcmp_return:
	pop	{r1-r5, pc}
