.text

.global _mem_bench

.balign 4

_mem_bench:
	mov	#-1,R0
ML00:
	add	#1,R0
	mov	#8,R7
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
ML02:
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	mov.l	@R4,R1
	
	mov.l	@R4,R1
	mov.l	@R4,R1

	dt	R7
	bf	ML02
	
	mov.l	@R5,R3
	tst	R3,R3
	bf	ML00
	rts
	nop
