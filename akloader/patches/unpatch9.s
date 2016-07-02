/*
    unpatch9.s
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
unpatch9:
	stmfd	sp!, {r0-r3}
	adr	r0, data
	ldmia	r0, {r1, r2, r3, lr}
	str	r2, [r1]
	str	lr, [r3]
	ldmfd	sp!, {r0-r3}
	bx	lr
data:
	.word	0xEFC0DE01
	.word	0xEFC0DE02
	.word	0xEFC0DE03
	.word	0xEFC0DE04
	.end
