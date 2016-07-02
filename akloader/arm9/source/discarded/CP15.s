
.arm
.align 4


.global CP15_GetID
CP15_GetID:
		mrc	p15, 0, r0, c0, c0, 0
		bx	lr


.global CP15_GetCacheType
CP15_GetCacheType:
		mrc	p15, 0, r0, c0, c0, 1
		bx	lr


.global CP15_GetTCMSize
CP15_GetTCMSize:
		mrc	p15, 0, r0, c0, c0, 2
		bx	lr


.global CP15_GetControl
.global CP15_SetControl

CP15_GetControl:
		mrc	p15, 0, r0, c1, c0, 0
		bx	lr

CP15_SetControl:
		mcr	p15, 0, r0, c1, c0, 0
		bx	lr


.global CP15_GetDataCachable
.global CP15_GetInstructionCachable
.global CP15_SetDataCachable
.global CP15_SetInstructionCachable

CP15_GetDataCachable:
		mrc	p15, 0, r0, c2, c0, 0
		bx	lr

CP15_GetInstructionCachable:
		mrc	p15, 0, r0, c2, c0, 1
		bx	lr

CP15_SetDataCachable:
		mcr	p15, 0, r0, c2, c0, 0
		bx	lr

CP15_SetInstructionCachable:
		mcr	p15, 0, r0, c2, c0, 1
		bx	lr


.global CP15_GetDataBufferable
.global CP15_SetDataBufferable

CP15_GetDataBufferable:
		mrc	p15, 0, r0, c3, c0, 0
		bx	lr

CP15_SetDataBufferable:
		mcr	p15, 0, r0, c3, c0, 0
		bx	lr


.global CP15_GetDataPermissions
.global CP15_GetInstructionPermissions
.global CP15_SetDataPermissions
.global CP15_SetInstructionPermissions

CP15_GetDataPermissions:
		mrc	p15, 0, r0, c5, c0, 2
		bx	lr

CP15_GetInstructionPermissions:
		mrc	p15, 0, r0, c5, c0, 3
		bx	lr

CP15_SetDataPermissions:
		mcr	p15, 0, r0, c5, c0, 2
		bx	lr

CP15_SetInstructionPermissions:
		mcr	p15, 0, r0, c5, c0, 3
		bx	lr


.global	CP15_GetRegion0
.global	CP15_GetRegion1
.global	CP15_GetRegion2
.global	CP15_GetRegion3
.global	CP15_GetRegion4
.global	CP15_GetRegion5
.global	CP15_GetRegion6
.global	CP15_GetRegion7
.global	CP15_SetRegion0
.global	CP15_SetRegion1
.global	CP15_SetRegion2
.global	CP15_SetRegion3
.global	CP15_SetRegion4
.global	CP15_SetRegion5
.global	CP15_SetRegion6
.global	CP15_SetRegion7

CP15_GetRegion0:
		mrc	p15, 0, r0, c6, c0, 0
		bx	lr
CP15_GetRegion1:
		mrc	p15, 0, r0, c6, c1, 0
		bx	lr
CP15_GetRegion2:
		mrc	p15, 0, r0, c6, c2, 0
		bx	lr
CP15_GetRegion3:
		mrc	p15, 0, r0, c6, c3, 0
		bx	lr
CP15_GetRegion4:
		mrc	p15, 0, r0, c6, c4, 0
		bx	lr
CP15_GetRegion5:
		mrc	p15, 0, r0, c6, c5, 0
		bx	lr
CP15_GetRegion6:
		mrc	p15, 0, r0, c6, c6, 0
		bx	lr
CP15_GetRegion7:
		mrc	p15, 0, r0, c6, c7, 0
		bx	lr


CP15_SetRegion0:
		mcr	p15, 0, r0, c6, c0, 0
		bx	lr
CP15_SetRegion1:
		mcr	p15, 0, r0, c6, c1, 0
		bx	lr
CP15_SetRegion2:
		mcr	p15, 0, r0, c6, c2, 0
		bx	lr
CP15_SetRegion3:
		mcr	p15, 0, r0, c6, c3, 0
		bx	lr
CP15_SetRegion4:
		mcr	p15, 0, r0, c6, c4, 0
		bx	lr
CP15_SetRegion5:
		mcr	p15, 0, r0, c6, c5, 0
		bx	lr
CP15_SetRegion6:
		mcr	p15, 0, r0, c6, c6, 0
		bx	lr
CP15_SetRegion7:
		mcr	p15, 0, r0, c6, c7, 0
		bx	lr


.global CP15_FlushICache
.global CP15_FlushICacheEntry
.global CP15_PrefetchICacheLine
.global CP15_FlushDCache
.global CP15_FlushDCacheEntry
.global CP15_CleanDCacheEntry
.global CP15_CleanAndFlushDCache
.global CP15_CleanAndFlushDCacheEntry
.global CP15_CleanDCacheEntryByIndex
.global CP15_CleanAndFlushDCacheEntryByIndex

CP15_FlushICache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c5, 0
		bx	lr

CP15_FlushICacheEntry:
		mcr	p15, 0, r0, c7, c5, 1
		bx	lr

CP15_PrefetchICacheLine:
		mcr	p15, 0, r0, c7, c13, 1
		bx	lr

CP15_FlushDCache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c6, 0
		bx	lr

CP15_FlushDCacheEntry:
		mcr	p15, 0, r0, c7, c6, 1
		bx	lr

CP15_CleanDCacheEntry:
		mcr	p15, 0, r0, c7, c10, 1
		bx	lr

CP15_CleanAndFlushDCache:
		mov	r1, #0
	outer_loop:
		mov	r0, #0
	inner_loop:
		orr	r2, r1, r0
		mcr	p15, 0, r2, c7, c14, 2
		add	r0, r0, #0x20
		cmp	r0, #0x400
		bne	inner_loop
		add	r1, r1, #0x40000000
		cmp	r1, #0x0
		bne	outer_loop
		bx	lr

CP15_CleanAndFlushDCacheEntry:
		mcr	p15, 0, r0, c7, c14, 1
		bx	lr

CP15_CleanDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c10, 2
		bx	lr

CP15_CleanAndFlushDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c14, 2
		bx	lr


.global CP15_DrainWriteBuffer
CP15_DrainWriteBuffer:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c10, 4
		bx	lr


.global CP15_WaitForInterrupt
CP15_WaitForInterrupt:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c0, 4
		bx	lr


.global CP15_GetDCacheLockdown
.global CP15_GetICacheLockdown
.global CP15_SetDCacheLockdown
.global CP15_SetICacheLockdown

CP15_GetDCacheLockdown:
		mrc	p15, 0, r0, c9, c0, 0
		bx	lr

CP15_GetICacheLockdown:
		mrc	p15, 0, r0, c9, c0, 1
		bx	lr

CP15_SetDCacheLockdown:
		mcr	p15, 0, r0, c9, c0, 0
		bx	lr

CP15_SetICacheLockdown:
		mcr	p15, 0, r0, c9, c0, 1
		bx	lr


.global CP15_GetDTCM
.global CP15_GetITCM
.global CP15_SetDTCM
.global CP15_SetITCM

CP15_GetDTCM:
		mrc	p15, 0, r0, c9, c1, 0
		bx	lr

CP15_GetITCM:
		mrc	p15, 0, r0, c9, c1, 1
		bx	lr

CP15_SetDTCM:
		mcr	p15, 0, r0, c9, c1, 0
		bx	lr

CP15_SetITCM:
		mcr	p15, 0, r0, c9, c1, 1
		bx	lr

.global CP15_ITCMEnableDefault
CP15_ITCMEnableDefault:
		mcr	p15, 0, r0, c7, c5	

		mov r0, #0							@make sure protection regions are all disabled
		mcr p15, 0, r0, c6, c0, 0
		mcr p15, 0, r0, c6, c1, 0
		mcr p15, 0, r0, c6, c2, 0
		mcr p15, 0, r0, c6, c3, 0
		mcr p15, 0, r0, c6, c4, 0
		mcr p15, 0, r0, c6, c5, 0
		mcr p15, 0, r0, c6, c6, 0
		mcr p15, 0, r0, c6, c7, 0
		
		ldr r0, =0x3F					@set read write access on regon 0 and 1
		mcr p15, 0, r0, c6, c0, 0

		ldr r0, =0x3					@set all mem to read write			
		mcr p15, 0, r0, c5, c0, 3
	
		mov r0, #1      					@enable catching for region 1
		mcr p15, 0, r0, c2, c0, 1
	
		ldr r0, =0x5707D			@enable ITCM and Protection unit
		mrc p15, 0, r0, c1, c0, 0
		orr r0, r1, r0
		mcr p15, 0, r0, c1, c0, 0

.end
