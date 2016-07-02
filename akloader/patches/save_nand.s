/*
    save_nand.s
    Copyright (C) 2009-2010 yellow wood goblin

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
start - 0x2002be4
length - 600
*/

	.thumb
/*
in:
r0 - memory
r1 - flash
r2 - size
r3 - type
     0 read
     1 write
used:
r0 - param
r1 - memory
r2 - size
r3 - type
r4 - flash
r5 - 0x200
r6 - interrupt state
*/
	.thumb_func
ProcessSave:
	push	{r1-r6, lr}
	mov	r4, r1
	mov	r1, r0
	bl	disableIRQ
	ldr	r0, mod_32M
	and	r4, r0
	mov	r5, #0x80
	lsl	r5, #2	@ 0x200
ProcessSave_loop:
	cmp	r2, #0
	ble	ProcessSave_return
	mov	r0, r4
	bl	lb2pb
	cmp	r3, #0
	bne	ProcessSave_write
	bl	sddReadSingleBlock_nand
	b	ProcessSave_tail
ProcessSave_write:
	bl	sddWriteSingleBlock_nand
ProcessSave_tail:
	add	r1, r1, r5	@ 0x200
	add	r4, r4, r5	@ 0x200
	sub	r2, r2, r5	@ 0x200
	b	ProcessSave_loop
ProcessSave_return:
	bl	restoreIRQ
	mov	r0, #1
	pop	{r1-r6, pc}

	.include	"nand.s"

	.thumb_func
disableIRQ:
	ldr	r0, REG_IE
	ldr	r6, [r0]
	mov	r5, r6	@ disable vblank
	lsr	r5, #1
	lsl	r5, #1
	str	r5, [r0]
	lsl	r6, #31
	lsr	r6, #31
	bx	lr
	
	.thumb_func
restoreIRQ:
	ldr	r0, REG_IE
	ldr	r5, [r0]
	orr	r5, r6
	str	r5, [r0]
	bx	lr

	.align 2
	.include	"nand_data.s"
mod_32M:
	.word	0x1ffffff
REG_IE:
	.word	0x04000210
	.end
