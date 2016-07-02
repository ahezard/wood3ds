/*
    card_dma_data.s
    Copyright (C) 2010-2011 yellow wood goblin

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

lastLogicCluster:
	.word	0xffffffff
lastPhysicSector:
	.word	0xffffffff
lastReadedPhysicSector:
	.word	0xffffffff
sddSingleRead:
	.word	SINGLE_READ
sddMultiRead:
	.word	MULTI_READ
sddGetData:
	.word   0xb7000000
	.word	0x00130000
sdhc:
	.word	0xEFC0DE91	@ 0 - sdhc, 9 - sd
clusterShift:
	.word	0xEFC0DE92	@ translate sector to cluster
clusterMask:
	.word	0xEFC0DE93	@ cluster size in sectors
lastCardSector:
	.word	0xEFC0DE94	@ last physical sector at card
cardId:
	.word	0xEFC0DE95
