.text

.global _tick_count

.align 4

_tick_count:
	mov	#-1,R0
TL00:
	add	#1,R0
	mov.l	@R4,R3
	tst	R3,R3
	bf	TL00
	rts
	nop
