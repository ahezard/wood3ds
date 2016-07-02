	.arch	armv5te
	.cpu	arm946e-s
	.text
	.align	2
	.arm
	.global	bswap
bswap:
	eor	r1, r0, r0, ror #16
	bic	r1, r1, #0xff0000
	mov	r0, r0, ror #8
	eor	r0, r0, r1, lsr #8
	bx	lr
	.end
