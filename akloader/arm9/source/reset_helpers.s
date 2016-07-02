/*
    reset_helpers.s
    Copyright (C) 2007 Acekard, www.acekard.com
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

#define ICACHE_SIZE 0x2000
#define DCACHE_SIZE 0x1000
#define CACHE_LINE_SIZE 32

	.arch	armv5te
	.cpu	arm946e-s
	.section	.itcm,"ax",%progbits
	.align	2
	.arm
	.global	IC_InvalidateAllITCM
IC_InvalidateAllITCM:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0
	bx	lr
	.global	DC_FlushAllITCM
DC_FlushAllITCM:
	mov	r1, #0
outer_loop:
	mov	r0, #0
inner_loop:
	orr	r2, r1, r0	@ generate segment and line address
	mcr	p15, 0, r2, c7, c14, 2	@ clean and flush the line
	add	r0, r0, #CACHE_LINE_SIZE
	cmp	r0, #DCACHE_SIZE/4
	bne	inner_loop
	add	r1, r1, #0x40000000
	cmp	r1, #0
	bne	outer_loop
	bx	lr
	.global	DC_InvalidateAllITCM
DC_InvalidateAllITCM:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c6, 0
	bx	lr
	.global	swiSoftResetITCM
swiSoftResetITCM:
	swi	0x000000
	bx	lr
	.global	zeroMemoryITCM
zeroMemoryITCM:
	mov	r2, #0
	.global	fillMemoryITCM
fillMemoryITCM:
	stmfd	sp!, {r4-r9, lr}
	add	r1, r0
	mov	r3, r2
	mov	r4, r2
	mov	r5, r2
	mov	r6, r2
	mov	r7, r2
	mov	r8, r2
	mov	r9, r2
fillMemoryITCM_loop:
	cmp	r0, r1
	stmltia	r0!, {r2-r9}
	blt	fillMemoryITCM_loop
	ldmfd	sp!, {r4-r9, pc}
	.global	copyMemoryITCM
copyMemoryITCM:
	stmfd	sp!, {r4-r10, lr}
	add	r10, r0, r2
copyMemoryITCM_loop:
	cmp	r0, r10
	ldmltia	r0!, {r2-r9}
	stmltia	r1!, {r2-r9}
	blt	copyMemoryITCM_loop
	ldmfd	sp!, {r4-r10, pc}

	.end
