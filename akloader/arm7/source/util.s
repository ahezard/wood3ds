	.arch	armv4t
	.cpu	arm7tdmi
	.text
	.align	2
	.global swiSoftReset_0
	.thumb_func
swiSoftReset_0:
	swi	0x00
	.end
