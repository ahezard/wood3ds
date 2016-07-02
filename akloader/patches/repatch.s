/*
    repatch.s
    Copyright (C) 2008 somebody
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
	adr	r0, (func_5+1)
	bx	r0

	.thumb_func
func_5:
	@ check for custom binary
	ldr	r0, RomBaseOffset
	ldr	r0, [r0]
	cmp	r0, #0
	bne	func_5_return	
	@ main binary
	adr	r0, PatchTable
	ldmia	r0!, {r3}
	cmp	r3, #0
	beq	func_5_return
func_5_loop:
	ldmia	r0!, {r1, r2}
	cmp	r1, #0
	blt	func_5_hword
	str	r2, [r1]	
	b	func_5_loop_next
func_5_hword:
	neg	r1, r1
	strh	r2, [r1]	
func_5_loop_next:
	sub	r3, #1
	bne	func_5_loop
	adr	r0, WRAMStart
	ldmia	r0, {r0, r1, r2}
	cmp	r0, #0
	beq	func_5_return
func_5_sd:
	ldr	r3, [r0]
	cmp	r3, r2
	beq	func_5_sd_found
	add	r0, #4
	cmp	r0, r1
	bne	func_5_sd
	b	func_5_return
func_5_sd_found:
	adr	r1, sd_save
	ldmia	r1!, {r2, r3}
func_5_sd_copy:
	ldmia	r0!, {r1}
	stmia	r2!, {r1}
	cmp	r2, r3
	bne	func_5_sd_copy
func_5_return:
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bx	r12

	.align	2
RomBaseOffset:
	.word	0x27ffc2c
WRAMStart:
	.word	0xefc0de01	@ 0x037f8000 or 0
	.word	0x03810000
	.word	0xe28fc001	@ mov r12, pc+1

sd_save:
	.word	0xefc0de02	@ target
	.word	0xefc0de03	@ target+length

PatchTable:

.end
