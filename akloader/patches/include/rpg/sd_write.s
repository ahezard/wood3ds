/*
    sd_write.s
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
lb2pb:
	push	{r0, r1, lr}
	bl	stopSDTransmission
	lsr	r0, #9
	lsl	r0, #2
	mov	r1, #1
	lsl	r1, #20	@ 0x100000
	add	r0, r1
	mov	r1, sp
	bl	ioRpgReadSram
	pop	{r0, r1, pc}

	.include	"include/ak_core/sd_write.s"
