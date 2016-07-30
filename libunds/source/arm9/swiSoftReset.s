	.arch	armv4t
	.cpu	arm7tdmi
	.text
	.align	2
	.global swiSoftReset_alt
	.thumb_func
swiSoftReset_alt:
	swi	0x00
	.end
