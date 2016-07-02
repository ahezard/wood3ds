/*
    memcpy.s
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

/*
r0 - dst
r1 - src
r2 - size in bytes
dont' call from arm
*/

	.thumb_func
memcpy:
	push	{r0-r4, lr}
	mov	r4, #0
	b	memcpy_check
memcpy_loop:
	ldrb	r3, [r1, r4]
	sub	r2, #1
	strb	r3, [r0, r4]
	add	r4, #1
memcpy_check:
	cmp	r2, #0
	bne	memcpy_loop
	pop	{r0-r4, pc}
