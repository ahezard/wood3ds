/*
    sw_cw_ja.s
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

	.thumb
	ldr	r1, [r0,#0x74]
	cmp	r1, #0
	bne	continue
	push	{r0, r3}
	swi	0x05
	pop	{r0, r3}
	ldr	r1, [r0,#0x74]
	cmp	r1, #0
	beq	finish
continue:
	mov	r0, #0
	bx	lr
finish:
	add	r0, #0x90
	ldr	r0, [r0]
	cmp	r0, #0
	beq	fail
	mov	r0, #1
	bx	lr
fail:
	mov	r0, #0
	mvn	r0, r0
	bx	lr
	.end
