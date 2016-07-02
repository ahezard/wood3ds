/*
    size_mask.s
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

	.arch	armv5te
	.cpu	arm946e-s
	.text
	.align	2
	.arm
	.global	size_mask
size_mask:
	sub	r0, #1
	clz	r0, r0
	mvn	r1, #0
	rsb	r0, r0, #32
	mvn	r0, r1, lsl r0
	bx	lr
	.end
