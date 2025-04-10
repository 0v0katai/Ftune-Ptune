.text

.global _mem_write

.balign 4

_mem_write:
	mov.l	K5555, r2
	mov.l	KAAAA, r3
	mov	r4, r0
LOOPW:
	mov.l	r3, @r0
	mov.l	r2, @r0
	mov.l	@r0, r4
	cmp/eq	r2, r4
	bf	RETW
	mov.l	r2, @r0
	mov.l	r3, @r0
	mov.l	@r0+, r4
	cmp/eq	r3, r4
	bf	RETW
	dt	r7
	bf/s	LOOPW
	nop
	mov	#0, r0
RETW:
	mov.l	r6, @r5
	rts
	nop

.balign 4

K5555:	.long	0x55555555
KAAAA:	.long	0xAAAAAAAA
