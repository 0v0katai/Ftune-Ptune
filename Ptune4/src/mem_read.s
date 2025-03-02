.text

.global _mem_read

.align 4

_mem_read:
	mov	r4, r0
LOOP:
	mov.l	@r0, r3
	mov.l	@r0+, r2
	cmp/eq	r2, r3
	bf	RET
	dt	r7
	bf/s	LOOP
	nop
	mov	#0, r0
RET:
	mov.l	r6, @r5
	rts
	nop
